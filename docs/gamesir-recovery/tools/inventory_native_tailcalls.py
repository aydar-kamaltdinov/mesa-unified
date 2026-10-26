"""Exact-ELF direct-tailcall analysis contracts; never execute target code.

A named target entry plus a nearby stack restore is positive evidence for a
call-return boundary. This does not establish ABI correctness or source parity.
"""
from pathlib import Path
import os
import json,struct,re,argparse
from index_native_coverage import elf
R=Path(os.environ.get('TURNIP_RECOVERY_ROOT',str(Path(__file__).resolve().parents[1])))
a=argparse.ArgumentParser();a.add_argument('scope',choices=['tu','ir3']);args=a.parse_args()
E=R/'native-complete/rutabaga-linkage-hour-20261025'
raw,ss,fs=elf(R/'originals/gs_rutabaga_v3.so');metadata=json.loads((R/'native-complete/gamesir-functions.json').read_text());byaddr={f['address']:f for f in fs if f['size']}
def off(v):return next(s['offset']+v-s['address'] for s in ss if s['type']!=8 and s['address']<=v<s['address']+s['size'])
def word(v):return struct.unpack_from('<I',raw,off(v))[0]
def restore(w):return w&0xffc07fff==0xa8c07bfd or w&0xff8003ff==0x910003ff
rows=[];sources=set()
for f in metadata:
 source=f.get('source_file') or ''
 eligible=(source.startswith('tu_') if args.scope=='tu' else source.startswith('ir3') or f['name'].startswith('ir3_'))
 if not eligible or not f['size']:continue
 for v in range(f['address'],f['address']+f['size'],4):
  w=word(v)
  if w&0xfc000000!=0x14000000:continue
  imm=w&0x3ffffff
  if imm&0x2000000:imm-=0x4000000
  target=v+imm*4
  if f['address']<=target<f['address']+f['size'] or target not in byaddr:continue
  if not any(restore(word(p)) for p in range(max(f['address'],v-24),v,4)):continue
  rows.append(dict(function=f['name'],function_ELF=hex(f['address']),branch_ELF=hex(v),target_ELF=hex(target),target_name=byaddr[target]['name'],evidence='direct B outside bounded symbol to named ELF function entry, with stack-frame restore in preceding six instructions'))
  sources.add(f['address']+0x100000)
prefix=args.scope+'-tailcall';(E/(prefix+'-contracts.json')).write_text(json.dumps(dict(scope='Positive stack-restoring direct tailcalls only; no indirect/table/interior targets overridden',rows=rows),indent=2)+'\n')
(E/(prefix+'-contracts.tsv')).write_text(''.join(f"{int(r['branch_ELF'],0)+0x100000:x}\t{r['target_name']}\n" for r in rows))
jobs=[]
for line in (R/'native-complete/gamesir-export-jobs.tsv').read_text().splitlines():
 if int(line.split('\t')[0],16) in sources:jobs.append(line)
(E/(prefix+'-jobs.tsv')).write_text('\n'.join(jobs)+'\n');print(args.scope,len(rows),'sites;',len(jobs),'canonical jobs')
