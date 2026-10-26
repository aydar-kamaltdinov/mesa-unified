"""Instruction-site accounting for the original two feature-checker functions.

Reuses the bounded static ARM interpreter with explicit synthetic callbacks.
This is reconstruction coverage, not a proof of all values/paths or API/GPU use.
"""
from pathlib import Path
import json,hashlib,collections
import recover_native_feature_checker as m
visited=set();jobs=0

def observe(stype,offset,core=False,**kwargs):
 global jobs
 result=m.run(stype,offset,core,**kwargs);visited.update(result['trace']);jobs+=1
 if jobs%100==0:print('jobs',jobs,'sites',len(visited),flush=True)
 return result
for stype,t in m.types.items():
 offsets=[f['offset']for f in t['fields']]
 for offset in offsets:observe(stype,offset)
 observe(stype,offsets,supported_value=1)
 observe(stype,[],supported_value=1)
 for version in[0,0x400000,0x401000,0x402000,0x403000,0x404000]:observe(stype,offsets,api_version=version,extension_offsets=set(),supported_value=1)
core_offsets=[r['api_member_offset']for r in m.getters['core']]
for offset in core_offsets:observe(0,offset,True)
observe(0,core_offsets,True,supported_value=1);observe(0,[],True,supported_value=1)
guards=json.loads((m.E/'checker-eligibility-guards.json').read_text())
for guard in guards['rows']:
 for byte in guard['extension_bytes'][1:]:observe(guard['stype'],[],api_version=0,extension_offsets={int(byte['physical_offset'],16)})
first=next(iter(m.types));field=m.types[first]['fields'][0]['offset']
for requests in[[],[dict(stype=0x7ffffffe,offsets=[])],[dict(stype=first,offsets=[]),dict(stype=first,offsets=[field])]]:
 observe(first,None,requests=requests,supported_value=1)
remaining=sorted(set(m.code)-visited);byfn=[]
for f in m.selected:
 known=set(range(f['address'],f['address']+f['size'],4));missing=sorted(known-visited)
 byfn.append(dict(name=f['name'],elf=hex(f['address']),instruction_sites=len(known),visited_sites=len(known&visited),remaining=[dict(elf=hex(pc),opcode=m.code[pc][0],operands=m.code[pc][1])for pc in missing]))
result=dict(original_binary_sha256=hashlib.sha256(m.raw).hexdigest(),model_scenarios=jobs,instruction_sites=len(m.code),visited_sites=len(visited),remaining_instruction_sites=len(remaining),functions=byfn,visited_instruction_addresses=[hex(pc)for pc in sorted(visited)],production_driver_changed=False,limits=__doc__.splitlines())
(m.E/'checker-site-coverage.json').write_text(json.dumps(result,indent=2)+'\n')
print('DONE',jobs,'visited',len(visited),'remaining',len(remaining),flush=True)
