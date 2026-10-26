"""Conservative incremental AArch64 ELF triage, never load/execute the driver.

Exact named entries and raw words are evidence, not semantic equivalence.
Branch normalization retains symbolic destinations and all nonbranch words.
It deliberately does not erase ADRP/literal/data differences or guess matches
for stripped, cloned, merged or ambiguous functions.
"""
from pathlib import Path
import argparse,collections,hashlib,json,struct,sys
from index_native_coverage import elf

def sha(data):return hashlib.sha256(data).hexdigest()
def snapshot(path):
 raw,sections,functions=elf(path)
 funcs=[f for f in functions if f['size']]
 entries=collections.defaultdict(list)
 for f in funcs:entries[f['address']].append(f)
 def body(f):
  s=next(s for s in sections if s['address']<=f['address']<s['address']+s['size'] and s['flags']&4)
  p=s['offset']+f['address']-s['address'];return raw[p:p+f['size']]
 def entry(a):
  names=entries.get(a)
  return sorted((f.get('source_file') or '',f['name']) for f in names) if names else None
 def normalized(f):
  data=body(f);tokens=[];calls=[];unresolved=[]
  if len(data)%4:return None,[],['non-word-aligned-size']
  for i in range(0,len(data),4):
   w=struct.unpack_from('<I',data,i)[0];a=f['address']+i
   if w&0x7c000000==0x14000000:bits,shift=26,0
   elif w&0xff000010==0x54000000 or w&0x7e000000==0x34000000:bits,shift=19,5
   elif w&0x7e000000==0x36000000:bits,shift=14,5
   else:tokens.append(('word',w));continue
   v=(w>>shift)&((1<<bits)-1)
   if v&(1<<(bits-1)):v-=1<<bits
   target=a+4*v;cleared=w&~(((1<<bits)-1)<<shift)
   if f['address']<=target<f['address']+f['size']:dest=('self',target-f['address'])
   elif entry(target):
    dest=('named-entry',entry(target));calls.append(target)
   else:
    # An unnamed PLT/interior destination must stay visible, never normalized
    # to an arbitrary name or an empty placeholder.
    dest=('unresolved-address',target);unresolved.append(hex(target))
   tokens.append((cleared,dest))
  return sha(json.dumps(tokens,sort_keys=True).encode()),sorted(set(calls)),unresolved
 bykey=collections.defaultdict(list);byaddress={}
 for f in funcs:
  r=dict(f);r['normalized'],r['direct_targets'],r['unresolved_branch_targets']=normalized(f);bykey[(f.get('source_file') or '',f['name'])].append(r);byaddress.setdefault(f['address'],r)
 alloc_data={s['name']:dict(bytes=s['size'],sha256=sha(raw[s['offset']:s['offset']+s['size']])) for s in sections if s['flags']&2 and not s['flags']&4 and s['type']!=8}
 return dict(path=str(path.resolve()),sha256=sha(raw),functions=funcs,bykey=bykey,byaddress=byaddress,data=alloc_data,symbols_available=any(s['type']==2 for s in sections))

def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--old',type=Path,required=True);ap.add_argument('--new',type=Path,required=True);ap.add_argument('--out',type=Path,required=True);ap.add_argument('--ghidra-bias',type=lambda x:int(x,0),default=0x100000);a=ap.parse_args()
 if a.out.exists() and any(a.out.iterdir()):ap.error('--out must be new or empty; never overwrite a prior comparison')
 a.out.mkdir(parents=True,exist_ok=True);old=snapshot(a.old);new=snapshot(a.new);rows=[];changed_addresses=set();jobs={};counts=collections.Counter()
 for k in sorted(old['bykey'].keys()|new['bykey'].keys()):
  before=old['bykey'].get(k,[]);after=new['bykey'].get(k,[]);row=dict(source_file=k[0] or None,name=k[1])
  if len(before)>1 or len(after)>1:
   row.update(status='ambiguous-name',old_entries=[hex(f['address']) for f in before],new_entries=[hex(f['address']) for f in after])
   for f in after:changed_addresses.add(f['address']);jobs.setdefault(f['address'],f)
  elif not after:row.update(status='removed',old_address=hex(before[0]['address']))
  elif not before:
   f=after[0];row.update(status='added',new_address=hex(f['address']));changed_addresses.add(f['address']);jobs.setdefault(f['address'],f)
  else:
   o,n=before[0],after[0];row.update(old_address=hex(o['address']),new_address=hex(n['address']),old_size=o['size'],new_size=n['size'],old_code_sha256=o['sha256'],new_code_sha256=n['sha256'])
   if o['sha256']==n['sha256']:row['status']='same-code-bytes'
   elif o['normalized'] is not None and o['normalized']==n['normalized']:
    row.update(status='same-after-direct-branch-normalization',caution='Nonbranch words identical and named direct destinations matched; callee/data/ABI behavior still requires dependency review')
   else:
    row['status']='changed-code';changed_addresses.add(n['address']);jobs.setdefault(n['address'],n)
  counts[row['status']]+=1;rows.append(row)
 data_changed=[]
 for name in sorted(old['data'].keys()|new['data'].keys()):
  if old['data'].get(name)!=new['data'].get(name):data_changed.append(dict(section=name,old=old['data'].get(name),new=new['data'].get(name)))
 # Raw reverse edges: unchanged callers are context-review targets, not new
 # decompilation jobs. This graph excludes arbitrary indirect targets.
 reverse=collections.defaultdict(set)
 for address,f in new['byaddress'].items():
  for t in f['direct_targets']:reverse[t].add(address)
 seen=set(changed_addresses);todo=list(seen)
 while todo:
  for caller in reverse[todo.pop()]:
   if caller not in seen:seen.add(caller);todo.append(caller)
 callers=[dict(address=hex(v),name=new['byaddress'][v]['name']) for v in sorted(seen-changed_addresses)]
 report=dict(old_binary=dict(path=old['path'],sha256=old['sha256'],symtab=old['symbols_available']),new_binary=dict(path=new['path'],sha256=new['sha256'],symtab=new['symbols_available']),counts=dict(counts),rows=rows,data_section_changes=data_changed,context_review_callers=callers,unique_decompilation_jobs=len(jobs),limits=['No semantic-equivalence/completeness claim from hashes or names.','All allocated data-section changes remain an explicit review frontier, even for same-code functions. Pointer relocation alone can change data hashes.','ADRP/ADR/literal addresses are NOT blindly erased; conservative false positives are expected.','No arbitrary indirect-call, global-object reference, CFG similarity or cross-compiler matching claim.','Stripped symbols/changed LTO/clones/ICF need structural/manual matching; do not assume a small delta.','Removed/ambiguous entries require inspection; jobs include all named new candidates without guessing identity.','Ghidra jobs use an explicit image bias, default0x100000; verify actual import image base.'],driver_executed=False)
 (a.out/'comparison.json').write_text(json.dumps(report,indent=2)+'\n')
 (a.out/'new-decompile-jobs.tsv').write_text(''.join(f"{v+a.ghidra_bias:x}\t{f['size']:x}\t{f['name']}\n" for v,f in sorted(jobs.items())))
 (a.out/'summary.md').write_text('# Native version comparison\n\n'+f"Old SHA256: `{old['sha256']}`\n\nNew SHA256: `{new['sha256']}`\n\nUnique candidate C jobs: {len(jobs)}; direct-call context-review entries: {len(callers)}; changed allocated data sections: {len(data_changed)}.\n\n"+'\n'.join(f'- {k}: {v}' for k,v in sorted(counts.items()))+'\n\nThese are triage classes, not proof of unchanged behavior. Same-code bodies may depend on changed callees, tables, descriptors or ABI. No driver build/execution.\n')
 print(json.dumps(dict(counts=dict(counts),unique_decompilation_jobs=len(jobs),context_review_callers=len(callers),changed_data_sections=len(data_changed),output=str(a.out)),indent=2))
if __name__=='__main__':main()
