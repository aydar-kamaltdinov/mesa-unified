"""Recover bounded native feature-byte write provenance from exported SSA.
Only constant expressions / reads from original read-only sections fold. Never runs SO.
"""
from pathlib import Path
import csv,json,struct,re,collections,hashlib
from index_native_coverage import ROOT,elf
E=ROOT/'native-complete/rutabaga-two-hours-20261027'
raw,sections,functions=elf(ROOT/'originals/gs_rutabaga_v3.so')
nodes={int(r['node']):r for r in csv.DictReader((E/'feature-ssa/value-nodes.tsv').open(),delimiter='\t')}
def ins(n):return [int(x) for x in nodes[n]['inputs'].split(',') if x]
cache={};active=set()
def const(n):
 if n in cache:return cache[n]
 if n in active:return None
 active.add(n);r=nodes[n];op=r['definition'];args=ins(n);v=None
 if op.startswith('CONST_0x'):v=int(op[8:],16)
 elif op in ('COPY','CAST','INT_ZEXT'):v=const(args[0])
 elif op=='INT_SEXT':
  v=const(args[0]);bits=int(nodes[args[0]]['size'])*8
  if v is not None and v>>(bits-1):v-=1<<bits
 elif op=='LOAD':
  p=nodes[args[1]]['pointer_expression']
  if re.fullmatch(r'0x[0-9a-f]+',p):
   addr=int(p,16)-0x100000;size=int(r['size'])
   for s in sections:
    if s['flags']&2 and not s['flags']&1 and s['type']!=8 and s['address']<=addr and addr+size<=s['address']+s['size']:
     off=s['offset']+addr-s['address'];v=int.from_bytes(raw[off:off+size],'little');break
 elif op=='MULTIEQUAL':
  vs=[const(a) for a in args]
  if vs and all(x is not None and x==vs[0] for x in vs):v=vs[0]
 elif op=='SUBPIECE':
  a,b=map(const,args)
  if a is not None and b is not None:v=a>>(8*b)
 elif op=='PIECE':
  a,b=map(const,args)
  if a is not None and b is not None:v=(a<<(8*int(nodes[args[1]]['size'])))|b
 elif len(args)==2 and op in ('INT_ADD','INT_SUB','INT_MULT','INT_AND','INT_OR','INT_XOR','INT_LEFT','INT_RIGHT','INT_EQUAL','INT_NOTEQUAL','INT_LESS','INT_LESSEQUAL','BOOL_AND','BOOL_OR','BOOL_XOR'):
  a,b=map(const,args)
  if a is not None and b is not None:
   if op=='INT_ADD':v=a+b
   elif op=='INT_SUB':v=a-b
   elif op=='INT_MULT':v=a*b
   elif op=='INT_AND':v=a&b
   elif op=='INT_OR':v=a|b
   elif op=='INT_XOR':v=a^b
   elif op=='INT_LEFT':v=a<<b
   elif op=='INT_RIGHT':v=a>>b
   elif op=='INT_EQUAL':v=int(a==b)
   elif op=='INT_NOTEQUAL':v=int(a!=b)
   elif op=='INT_LESS':v=int(a<b)
   elif op=='INT_LESSEQUAL':v=int(a<=b)
   elif op=='BOOL_AND':v=int(bool(a)and bool(b))
   elif op=='BOOL_OR':v=int(bool(a)or bool(b))
   elif op=='BOOL_XOR':v=int(bool(a)!=bool(b))
 if v is not None:v&=(1<<(8*int(r['size'])))-1
 active.remove(n);cache[n]=v;return v

def dependencies(n):
 seen=set();todo=[n];leaves=[]
 while todo:
  x=todo.pop()
  if x in seen:continue
  seen.add(x);r=nodes[x]
  if r['definition']=='LOAD' or r['definition'].startswith('CALL') or r['definition']=='INPUT':
   if const(x) is None:leaves.append(dict(node=x,definition=r['definition'],elf=r['elf_instruction'],pointer=r['pointer_expression']))
  if len(seen)>3000:raise RuntimeError('unexpected expansion')
  todo+=ins(x)
 return sorted(leaves,key=lambda x:x['node'])

# Original memset executes before the named feature writes. Its exact call is
# separately bounded by native PLT/name recovery; no target library is run.
features={int(a,16):n for a,n in (line.split('\t')for line in (E/'original-feature-fields.tsv').read_text().splitlines())}
rows=[dict(physical_offset=hex(o),name=features.get(o),writes=[])for o in range(0x207,0x443)]
for call in csv.DictReader((E/'feature-ssa/extension-memory-calls.tsv').open(),delimiter='\t'):
 if call['elf_instruction']!='988bd8' or call['physical_offset']!='205':continue
 args=[int(x)for x in call['input_nodes'].split(',')];value=const(args[1]);count=const(args[2]);assert value==0 and count==0x23e
 for row in rows:row['writes'].append(dict(elf='988bd8',literal_byte=0,source='original initial memset zero range'))
assert all(r['writes']for r in rows)
for store in csv.DictReader((E/'feature-ssa/extension-stores.tsv').open(),delimiter='\t'):
 base=int(store['extension_index']);width=int(store['bytes']);n=int(store['value_node']);v=const(n)
 for byte in range(width):
  if 0<=base+byte<len(rows):rows[base+byte]['writes'].append(dict(elf=store['elf_instruction'],value_node=n,width=width,byte=byte,literal_byte=None if v is None else(v>>(8*byte))&255,source='original SSA STORE'))
counts=collections.Counter()
for row in rows:
 row['last_lexical_write']=row['writes'][-1];v=row['last_lexical_write']['literal_byte'];status='conditional-or-symbolic'if v is None else'literal-enabled'if v==1 else'literal-disabled'if v==0 else'other-literal'
 row['classification']=status;counts[status]+=1
 if v is None:row['dependencies']=dependencies(row['last_lexical_write']['value_node'])
result=dict(binary_sha256=hashlib.sha256(raw).hexdigest(),counts=dict(counts),named_fields=len(features),physical_byte_range=['0x207','0x443(exclusive)'],rows=rows,limits=['Bounded static initializer writes, no target execution.','Last lexical write is not a general control-flow/path proof; retain all writes for review.','Same original memset then203 later SSA stores; inputs and conditional predicates are not evaluated as enabled.','Unnamed byte0x3cc is retained without inventing a feature name.','Live enumeration, full API/GPU semantics and feature activation remain unvalidated.'])
(E/'recovered-feature-initializer-stores.json').write_text(json.dumps(result,indent=2)+'\n')
print('feature bytes',len(rows),'named',len(features),'classifications',dict(counts))
