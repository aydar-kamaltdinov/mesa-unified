"""Classify residual checker blocks without faking successful execution.

Duplicate diagnostics are interpreted with pure name/log abstractions. Error
post-call jumps and internal stack-link cycle guards are classified statically;
we do not corrupt pointers or pretend those paths were exercised.
"""
import json,hashlib,collections
import recover_native_feature_checker as m
prior=json.loads((m.E/'checker-site-coverage.json').read_text());visited=set(int(pc,16)for pc in prior['visited_instruction_addresses']);cases=[]
for stype,t in m.types.items():
 requests=[dict(stype=stype,offsets=[]),dict(stype=stype,offsets=[])]
 r=m.run(stype,None,requests=requests,supported_value=1);visited.update(r['trace']);cases.append(dict(stype=stype,struct=t['name'],warnings=len(r['warnings']),result=r.get('result')))
assert all(r['result']==0 and r['warnings']==(0 if r['stype']==1000059000 else 1)for r in cases)
# The KHR divisor guard is the last short-circuit arm after core/EXT tests.
guard_doc=json.loads((m.E/'checker-eligibility-guards.json').read_text())
divisor=next(g for g in guard_doc['rows']if g['name']=='VkPhysicalDeviceVertexAttributeDivisorFeatures')
rare=m.run(divisor['stype'],[],api_version=0,extension_offsets={0xc8});assert 0xa29e74 in rare['trace'];visited.update(rare['trace'])
stack_doc=json.loads((m.E/'checker-routes.json').read_text());stack={r['offset_from_entry_sp']+8:r for r in stack_doc['stack_records']}
# STP preindex-0x60, then SUB0x2000, SUB0x890 from the verified listing.
assert m.code[m.main['address']][0]=='stp' and '#-0x60' in m.code[m.main['address']][1]
body_sp_delta=-0x28f0
remaining=set(m.code)-visited;classified={};cycles=[]
for pc in sorted(remaining):
 op,args,_=m.code[pc]
 if op=='b'and pc-4 in m.code and m.code[pc-4][0]=='bl'and m.code[pc-4][1].startswith('0xa5604c '):
  classified[pc]='return epilogue after __vk_errorf (model deliberately stopped at diagnostic)'
 if op=='mov'and args=='w0, #-0xd'and pc+4 in m.code:
  jump_op,jump_args,_=m.code[pc+4];assert jump_op=='b'and jump_args.startswith('0xa332c8 ')
  incoming=[]
  for source,(branch,operands,_)in m.code.items():
   if branch=='cbnz'and int(operands.split(', ')[1].split()[0],16)==pc:
    register=operands.split(', ')[0];load=m.code[source-4];assert load[0]=='ldr'and load[1].startswith(register+', [sp, ')
    incoming.append(dict(branch_elf=hex(source),load_elf=hex(source-4),load=load[1]))
  assert len(incoming)==1,(hex(pc),incoming)
  load=incoming[0]['load'];import re
  offset=int(re.search(r'\[sp, #(0x[0-9a-f]+)\]',load)[1],16)+body_sp_delta
  record=stack[offset];assert record['stype']!=1000059000
  cycles.append(dict(return_elf=hex(pc),result=-13,guard=incoming[0],stack_type=record));classified[pc]='internal query-stack pNext cycle guard; source has VK_ERROR_UNKNOWN';classified[pc+4]='same cycle-guard return epilogue'
for pc in sorted(remaining):
 if pc not in classified:
  op,args,_=m.code[pc]
  if op=='b'and int(args.split()[0],16)==0xa332c8:
   classified[pc]='return epilogue after core-feature-check failure (diagnostic model returns at helper)'
unknown=sorted(remaining-set(classified));assert len(cycles)==274 and len({r['stack_type']['stype']for r in cycles})==274
assert not unknown,unknown
result=dict(original_binary_sha256=hashlib.sha256(m.raw).hexdigest(),instruction_sites=len(m.code),observed_model_sites=len(visited),unobserved_classified_sites=len(classified),unclassified_sites=len(unknown),duplicate_types=len(cases),duplicate_warning_types=sum(r['warnings']for r in cases),duplicate_cases=cases,cycle_guard_routes=cycles,classified=[dict(elf=hex(pc),kind=classified[pc])for pc in sorted(classified)],unclassified=[dict(elf=hex(pc),opcode=m.code[pc][0],operands=m.code[pc][1])for pc in unknown],visited_instruction_addresses=[hex(pc)for pc in sorted(visited)],limits=__doc__.splitlines())
(m.E/'checker-residual-classification.json').write_text(json.dumps(result,indent=2)+'\n');print({k:v for k,v in result.items()if k not in['duplicate_cases','cycle_guard_routes','classified','unclassified','visited_instruction_addresses','limits']});print('unknown',result['unclassified'][:24])
