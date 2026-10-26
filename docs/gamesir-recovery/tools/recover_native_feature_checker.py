"""Bounded static ARM routing for the original feature checker.

No target code is loaded/executed. A synthetic GetPhysicalDeviceFeatures2 response
sets supported payloads to zero; original extension/API conditions are enabled in
an isolated model. One requested bit selects each diagnostic route. These are
reconstruction probes, not Vulkan/device/GPU validation or a general emulator.
"""
from pathlib import Path
import re,json,hashlib,struct,collections
from index_native_coverage import ROOT,elf
E=ROOT/'native-complete/rutabaga-remaining-two-hours-20261027';F=ROOT/'native-complete';PRE=F/'rutabaga-two-hours-20261027'
raw,sections,functions=elf(ROOT/'originals/gs_rutabaga_v3.so');selected=[next(f for f in functions if f['name']==n)for n in ['vk_physical_device_check_device_features','check_physical_device_features']];main,corefn=selected;code={}
assert hashlib.sha256(raw).hexdigest()=='90c056d7919a1da8ef0fd3131403729b81b05fe3c7373e772e3ceaa522ad397e', 'This reconstruction model is bound to the original v3 binary'
for line in (F/'gamesir.arm64.txt').read_text().splitlines():
 m=re.match(r'\s*([a-f0-9]+): ([a-f0-9]{8})\s+(\S+)\s*(.*)',line)
 if m:
  pc=int(m[1],16)
  if any(f['address']<=pc<f['address']+f['size']for f in selected):
   s=next(s for s in sections if s['type']!=8 and s['address']<=pc< s['address']+s['size']);offset=s['offset']+pc-s['address'];word=int(m[2],16);assert raw[offset:offset+4]==word.to_bytes(4,'little')
   code[pc]=(m[3],m[4].split('//')[0].strip(),word)
assert len(code)*4==sum(f['size']for f in selected)
getters=json.loads((PRE/'recovered-feature-getter-fields.json').read_text());types={}
assert hashlib.sha256((ROOT/'mesa-gamesir-recovered/src/vulkan/registry/vk.xml').read_bytes()).hexdigest()==getters['source_registry_sha256'], 'Regenerate/reconcile ABI maps before using a different registry'
for row in getters['rows']:
 st=types.setdefault(row['stype'],dict(name=row['struct'],fields=[]));st['fields'].append(dict(name=row['member'],offset=row['api_member_offset']))
types[1000059000]=dict(name='VkPhysicalDeviceFeatures2',fields=[dict(name=r['member'],offset=r['api_member_offset']+16)for r in getters['core']])
for st in types.values():st['size']=(max(f['offset']+4 for f in st['fields'])+7)//8*8
P=0x1000000000;K=0x2000000000;C=0x3000000000;S=0x4000000000
MASK=(1<<64)-1

def run(stype,member_offset,core=False,api_version=0xffffffff,extension_offsets=None,supported_value=0,requests=None,max_steps=40000):
 regs={f'x{i}':0 for i in range(31)};regs.update(sp=S,x0=P,x1=K);vectors={};mem={};trace=[];queries=[];flags=(0,1,1,0);stack_records=[];loads=[];support_loads=[];supported_abi={};physical_reads=[];register_tags={};api_comparisons=[];tagged_compare=None;warnings=[]
 def width(r):return 64 if r=='sp'or r.startswith('x')else 32
 def put(a,value,size):
  assert size in [1,2,4,8,16],size
  for i in range(size):mem[a+i]=(value>>(8*i))&255
 def read(a,size):
  if all(a+i in mem for i in range(size)):return sum(mem[a+i]<<(8*i)for i in range(size))
  for s in sections:
   if s['type']!=8 and s['address']<=a and a+size<=s['address']+s['size']:
    offset=s['offset']+a-s['address'];return int.from_bytes(raw[offset:offset+size],'little')
  raise ValueError(('unknown memory',hex(a),size,hex(trace[-1])))
 def cstr(a):
  result=bytearray()
  for _ in range(300):
   byte=read(a+len(result),1)
   if byte==0:return result.decode()
   result.append(byte)
  raise ValueError('unterminated string')
 def get(r):
  if r in ['xzr','wzr']:return 0
  if r.startswith(('q','d','s'))and r!='sp':return vectors.get(int(r[1:]),0)&((1<<({'q':128,'d':64,'s':32}[r[0]]))-1)
  assert r=='sp'or re.fullmatch(r'[wx][0-9]+',r),r
  return regs['x'+r[1:]if r.startswith('w')else r]&((1<<width(r))-1)
 def setreg(r,v):
  register_tags.pop('x'+r[1:]if r.startswith('w')else r,None)
  if r in ['xzr','wzr']:return
  if r.startswith(('q','d','s'))and r!='sp':vectors[int(r[1:])]=v&((1<<({'q':128,'d':64,'s':32}[r[0]]))-1);return
  assert r=='sp'or re.fullmatch(r'[wx][0-9]+',r),r
  regs['x'+r[1:]if r.startswith('w')else r]=v&((1<<width(r))-1)
 def value(x):return int(x[1:],0)if x.startswith('#')else get(x)
 def operand(parts):
  v=value(parts[0])
  if len(parts)>1:
   shift,amount=parts[1].split(' ')
   if shift=='uxtw':v&=0xffffffff
   elif shift=='sxtw':v=(v&0xffffffff)-(1<<32)if v&0x80000000 else v&0xffffffff
   elif shift!='lsl':raise ValueError(('shift',parts))
   v<<=int(amount[1:],0)
  return v
 def memory(text):
  m=re.fullmatch(r'\[([^,\]]+)(?:, ([^\]]+))?\](!)?(?:, (#[^ ]+))?',text);assert m,text
  base=get(m[1]);off=operand(m[2].split(', '))if m[2]else 0;a=(base+off)&MASK
  if m[3]:setreg(m[1],a)
  if m[4]:setreg(m[1],base+value(m[4]))
  return a
 def compare(a,b,size):
  nonlocal flags
  mask=(1<<size)-1;a&=mask;b&=mask;r=(a-b)&mask
  flags=(r>>(size-1),int(r==0),int(a>=b),int(bool((a^b)&(a^r)&(1<<(size-1)))))
 def cond(name):
  n,z,c,v=flags
  return {'eq':z==1,'ne':z==0,'hi':c==1 and z==0,'ls':c==0 or z==1,'hs':c==1,'lo':c==0,'gt':z==0 and n==v,'le':z==1 or n!=v,'ge':n==v,'lt':n!=v}[name]
 # Isolated synthetic physical object: all original extension predicates true,
 # maximal API version. This never writes the real source/device policy.
 for a in range(P,P+0x2100):mem[a]=0
 for a in range(P+0x58,P+0x207):mem[a]=int(extension_offsets is None or a-P in extension_offsets)
 put(P+0x448,api_version,4)
 for a in range(K,K+72):mem[a]=0
 if requests is not None:
  assert not core
  put(K+8,C if requests else 0,8)
  for index,request in enumerate(requests):
   at=C+index*0x1000;st=request['stype'];size=types[st]['size']if st in types else 16
   for a in range(at,at+size):mem[a]=0
   put(at,st,4);put(at+8,at+0x1000 if index+1<len(requests)else 0,8)
   for offset in request.get('offsets',[]):put(at+offset,1,4)
 else:
  fields=getters['core']if core else types[stype]['fields'];size=220 if core else types[stype]['size']
  for a in range(C,C+size):mem[a]=0
  if core:put(K+64,C,8)
  else:put(K+8,C,8);put(C,stype,4)
  if member_offset is not None:
   for offset in [member_offset]if isinstance(member_offset,int)else member_offset:put(C+offset,1,4)
 pc=main['address']
 for step in range(max_steps):
  assert pc in code,('outside functions',hex(pc));op,args,word=code[pc];trace.append(pc);nxt=pc+4
  if op in ['ldr','ldrb','ldur']:
   m=re.fullmatch(r'(\w+), (\[.*\]!?(:?[, ]+#[-0-9xa-f]+)?)',args);assert m,args
   r=m[1];size=1 if op=='ldrb'else {'w':4,'x':8,'q':16,'d':8,'s':4}[r[0]];a=memory(m[2]);v=read(a,size);setreg(r,v)
   if a==P+0x448:register_tags['x'+r[1:]if r.startswith('w')else r]='API'
   if P<=a<P+0x2100:physical_reads.append(dict(pc=hex(pc),offset=a-P,size=size,value=v,after_query=bool(queries)))
   if C<=a<C+0x1000:loads.append(dict(pc=hex(pc),offset=a-C,size=size,value=v))
   if a in supported_abi:support_loads.append(dict(pc=hex(pc),size=size,value=v,**supported_abi[a]))
  elif op in ['str','strb','stur']:
   m=re.fullmatch(r'(\w+), (\[.*\]!?(:?[, ]+#[-0-9xa-f]+)?)',args);assert m,args
   r=m[1];size=1 if op=='strb'else {'w':4,'x':8,'q':16,'d':8,'s':4}[r[0]];put(memory(m[2]),get(r),size)
  elif op in ['ldp','stp']:
   m=re.fullmatch(r'(\w+), (\w+), (\[.*\]!?(:?[, ]+#[-0-9xa-f]+)?)',args);assert m,args
   a=memory(m[3]);size={'w':4,'x':8,'q':16,'d':8}[m[1][0]]
   if op=='ldp':setreg(m[1],read(a,size));setreg(m[2],read(a+size,size))
   else:put(a,get(m[1]),size);put(a+size,get(m[2]),size)
  elif op in ['add','sub','subs','orr']:
   parts=args.split(', ');r=parts[0];a=value(parts[1]);b=operand(parts[2:]);v=a+b if op=='add'else a|b if op=='orr'else a-b
   if op=='subs':compare(a,b,width(r))
   setreg(r,v)
  elif op=='cmp':
   parts=args.split(', ');compare(value(parts[0]),operand(parts[1:]),width(parts[0]));left=register_tags.get('x'+parts[0][1:]if parts[0].startswith('w')else parts[0]);right=register_tags.get('x'+parts[1][1:]if parts[1].startswith('w')else parts[1]);tagged_compare=None
   if left=='API'or right=='API':
    tagged_compare=dict(cmp_elf=hex(pc),api_left=left=='API',other_value=operand(parts[1:])if left=='API'else value(parts[0]),after_query=bool(queries),branches=[]);api_comparisons.append(tagged_compare)
  elif op=='mov':
   r,v=args.split(', ');tag=register_tags.get('x'+v[1:]if v.startswith('w')else v);setreg(r,value(v))
   if tag:register_tags['x'+r[1:]if r.startswith('w')else r]=tag
  elif op=='movk':
   r,v,shift=args.split(', ');shift=int(shift.split('#')[1],0);setreg(r,(get(r)&~(0xffff<<shift))|(value(v)<<shift))
  elif op=='movi':
   assert args=='v0.2d, #0000000000000000',args;vectors[0]=0
  elif op in ['adr','adrp']:
   r=args.split(', ')[0];imm=((word>>29)&3)|(((word>>5)&0x7ffff)<<2);imm-=1<<21 if imm&(1<<20)else 0
   setreg(r,(pc&~4095)+(imm<<12)if op=='adrp'else pc+imm)
  elif op=='b':
   nxt=int(args.split()[0],16)
   if nxt==0xa5604c:
    return dict(outcome='feature-error',struct=cstr(get('x5')),member=cstr(get('x6')),error_result=get('w1'),error_call_elf=hex(pc),error_kind='tail-B',query=queries,requested_loads=loads,supported_loads=support_loads,physical_reads=physical_reads,api_comparisons=api_comparisons,warnings=warnings,trace=trace)
  elif op.startswith('b.'):
   taken=cond(op[2:])
   if tagged_compare is not None:tagged_compare['branches'].append(dict(elf=hex(pc),condition=op[2:],taken=taken))
   if taken:nxt=int(args.split()[0],16)
  elif op in ['cbz','cbnz']:
   r,target=args.split(', ')
   if(get(r)==0)==(op=='cbz'):nxt=int(target.split()[0],16)
  elif op in ['tbnz','tbz']:
   r,bit,target=args.split(', ')
   if bool(get(r)&(1<<int(bit[1:],0)))==(op=='tbnz'):nxt=int(target.split()[0],16)
  elif op=='blr':
   assert pc==0xa2dec4,hex(pc);root_ptr=get('x1');at=root_ptr;seen=set()
   while at:
    assert at not in seen;seen.add(at);st=read(at,4);t=types[st];stack_records.append(dict(stype=st,name=t['name'],offset_from_entry_sp=at-S,size=t['size']))
    for field in t['fields']:
     put(at+field['offset'],supported_value,4);supported_abi[at+field['offset']]=dict(stype=st,member=field['name'],api_offset=field['offset'],stack_offset=at+field['offset']-S)
    at=read(at+8,8)
   queries.append(dict(pc=hex(pc),root_offset=root_ptr-S,chain=stack_records.copy()))
   for i in range(19):regs[f'x{i}']=0
   regs['x30']=nxt;register_tags.clear();tagged_compare=None
  elif op=='bl':
   target=int(args.split()[0],16)
   if target==0xa5604c:
    return dict(outcome='feature-error',struct=cstr(get('x5')),member=cstr(get('x6')),error_result=get('w1'),error_call_elf=hex(pc),error_kind='BL',query=queries,requested_loads=loads,supported_loads=support_loads,physical_reads=physical_reads,api_comparisons=api_comparisons,warnings=warnings,trace=trace)
   if target==0xc793d0:
    # Diagnostic-only pure name conversion. Its text is never used for control.
    label=f'synthetic_type_name_{get("w0")}'.encode()+b'\0';address=0x7000000000
    for i,byte in enumerate(label):mem[address+i]=byte
    for i in range(19):regs[f'x{i}']=0
    regs['x0']=address;regs['x30']=nxt;register_tags.clear();tagged_compare=None
   elif target==0xa559b0:
    warnings.append(dict(pc=hex(pc),kind='duplicate structure diagnostic; logging abstracted, no external message'))
    for i in range(19):regs[f'x{i}']=0
    regs['x30']=nxt;register_tags.clear();tagged_compare=None
   else:
    assert target==corefn['address'],('unexpected external call',hex(pc),args)
    regs['x30']=nxt;nxt=target
  elif op=='ret':
   if get('x30')==0:return dict(outcome='return',result=get('w0'),query=queries,requested_loads=loads,supported_loads=support_loads,physical_reads=physical_reads,api_comparisons=api_comparisons,warnings=warnings,trace=trace)
   nxt=get('x30')
  else:raise ValueError(('opcode',op,args,hex(pc)))
  pc=nxt
 raise ValueError('step limit')

def main_cli():
 import argparse
 parser=argparse.ArgumentParser();parser.add_argument('--all',action='store_true');parser.add_argument('--guards',action='store_true');parser.add_argument('--order',action='store_true');parser.add_argument('--accepted',action='store_true');parser.add_argument('--chains',action='store_true');args=parser.parse_args();
 if args.chains:return recover_chains();
 if args.accepted:return recover_nonzero_branches();
 if args.order:return recover_priority();
 if args.guards:return recover_guards()
 rows=[];failures=[];visited=set();stack={}
 jobs=[(1000286000,16,False,'example')]
 if args.all:
  jobs=[]
  for stype,t in types.items():
   for field in t['fields']:jobs.append((stype,field['offset'],False,field['name']))
  jobs +=[(0,r['api_member_offset'],True,r['member'])for r in getters['core']]
 for stype,offset,core,name in jobs:
  try:r=run(stype,offset,core)
  except Exception as ex:failures.append(dict(stype=stype,offset=offset,core=core,error=str(ex)));continue
  visited.update(r.pop('trace'))
  for query in r['query']:
   for record in query['chain']:stack[record['stype']]=record
  r.update(input_stype=stype,input_member_offset=offset,input_name=name,core=core);rows.append(r)
 report=dict(original_binary_sha256=hashlib.sha256(raw).hexdigest(),functions=selected,rows=rows,failures=failures,stack_records=list(stack.values()),unique_instruction_sites=len(visited),limits=['Static ARM model only; target SO never executed.','Synthetic query response supports no feature bits, while extension/API eligibility is enabled in the model.','Diagnostic routes/stack ABI reconstruction do not validate live feature eligibility, backend behavior or accepted/GPU cases.'])
 (E/('checker-routes.json'if args.all else'checker-single-route.json')).write_text(json.dumps(report,indent=2)+'\n')
 print('routes',len(rows),'failures',len(failures),'stack structs',len(stack),'visited',len(visited));print(failures[:3]);print([{k:v for k,v in r.items()if k not in['query','requested_loads']}for r in rows[:2]])
def recover_guards():
 table=json.loads((PRE/'original-extension-table.json').read_text());extension_names={0x58+i:r['name']for i,r in enumerate(table['rows'])};rows=[];problems=[]
 versions=[0,0x400000,0x401000,0x402000,0x403000,0x404000]
 for stype,t in types.items():
  offset=t['fields'][0]['offset'];zero=run(stype,offset,False,0,set());seen=sorted({r['offset']for r in zero['physical_reads']if 0x58<=r['offset']<0x207});always=zero['outcome']=='feature-error';passed=[]
  for byte in seen:
   r=run(stype,offset,False,0,{byte})
   if r['outcome']=='feature-error':passed.append(byte)
  min_api=None
  if always:min_api=0
  elif any(r['offset']==0x448 for r in zero['physical_reads']):
   for version in versions[1:]:
    r=run(stype,offset,False,version,set())
    if r['outcome']=='feature-error':min_api=version;break
  if seen and passed!=seen:problems.append(dict(stype=stype,seen=seen,passed=passed))
  phases={name:sorted({r['offset']for r in zero['physical_reads']if bool(r['after_query'])==phase and(0x58<=r['offset']<0x207 or r['offset']==0x448)})for name,phase in [('query_chain',False),('validation',True)]}
  rows.append(dict(name=t['name'],stype=stype,unconditional=always,minimum_core_version_from_standard_thresholds=hex(min_api)if min_api is not None else None,extension_bytes=[dict(physical_offset=hex(b),name=extension_names[b])for b in seen],each_extension_alone_enables=passed==seen,api_comparisons=zero['api_comparisons'],guard_load_sites=[r for r in zero['physical_reads']if 0x58<=r['offset']<0x207 or r['offset']==0x448],phase_dependencies=phases))
 report=dict(original_binary_sha256=hashlib.sha256(raw).hexdigest(),rows=rows,non_OR_or_unresolved=problems,limits=['Static routing under explicit synthetic GetFeatures2 response; no target execution.','Extension probes and core thresholds expose original eligibility dependencies, not real device exposure.','Core threshold probes cover Vulkan1.0 through1.4; raw guard load sites retained for precise instruction/source review.','Unusual/non-OR cases stay explicit rather than being forced into a formula.'])
 (E/'checker-eligibility-guards.json').write_text(json.dumps(report,indent=2)+'\n');print('guard structs',len(rows),'nonOR/unresolved',len(problems));print(problems[:3])

def recover_priority():
 rows=[]
 groups=[(stype,t['name'],{f['name']:f['offset']for f in t['fields']},False)for stype,t in types.items()]
 groups.append((0,'VkPhysicalDeviceFeatures',{r['member']:r['api_member_offset']for r in getters['core']},True))
 for stype,name,fields,core in groups:
  pending=dict(fields);order=[];diagnostic=None
  while pending:
   result=run(stype,list(pending.values()),core)
   assert result['outcome']=='feature-error',(name,result['outcome'])
   member=result['member'];assert member in pending,(name,member)
   order.append(dict(member=member,api_offset=pending.pop(member),error_elf=result['error_call_elf'],supported_load=result['supported_loads'][-1]));diagnostic=result['struct']
  zero=run(stype,[],core);assert zero['outcome']=='return'and zero['result']==0,(name,zero['outcome'])
  rows.append(dict(stype=stype,layout_name=name,native_diagnostic_name=diagnostic,core=core,order=order))
 (E/'checker-field-priority.json').write_text(json.dumps(dict(original_binary_sha256=hashlib.sha256(raw).hexdigest(),rows=rows,field_records=sum(len(r['order'])for r in rows),limits=['Original rejection priority under the explicit all-zero query-response model.','All requested flags start1, then the rejected field is cleared to expose the next diagnostic.','No real driver/device execution, accepted/GPU behavior or arbitrary pointer/lifetime validation.']),indent=2)+'\n')
 print('priority groups',len(rows),'field routes',sum(len(r['order'])for r in rows))

def recover_nonzero_branches():
 rows=[];failures=[]
 groups=[(stype,t['name'],t['fields'],False)for stype,t in types.items()]
 groups.append((0,'VkPhysicalDeviceFeatures',[dict(name=r['member'],offset=r['api_member_offset'])for r in getters['core']],True))
 for stype,name,fields,core in groups:
  for field in fields:
   result=run(stype,field['offset'],core,supported_value=1)
   if result['outcome']!='return'or result['result']!=0:failures.append(dict(struct=name,member=field['name'],outcome=result['outcome']))
   rows.append(dict(stype=stype,struct=name,member=field['name'],result=result.get('result'),queried=len(result['query'])))
 report=dict(original_binary_sha256=hashlib.sha256(raw).hexdigest(),rows=rows,failures=failures,limits=['Static original-ARM interpretation with explicit synthetic query payload1.','This complements the zero-payload diagnostic routes; no driver/SO/GPU execution or real feature exposure.','Original extension/API conditions enabled only in this isolated model; full driver/API validation deferred.'])
 (E/'checker-nonzero-supported-branches.json').write_text(json.dumps(report,indent=2)+'\n');print('nonzero-supported routes',len(rows),'nonzero/error outcomes',len(failures));assert not failures

def recover_chains():
 chosen=list(types)[:3];first=chosen[0];offset=types[first]['fields'][0]['offset'];all_types=list(types)
 scenarios=[('three-distinct',[dict(stype=st,offsets=[f['offset']for f in types[st]['fields']])for st in chosen],1),('all-known-memory-views',[dict(stype=st,offsets=[f['offset']for f in types[st]['fields']])for st in all_types],1),('duplicate-accepted',[dict(stype=first,offsets=[]),dict(stype=first,offsets=[offset])],1),('duplicate-second-rejected',[dict(stype=first,offsets=[]),dict(stype=first,offsets=[offset])],0),('unknown-surrounding-known',[dict(stype=0x7ffffffe,offsets=[]),dict(stype=first,offsets=[offset]),dict(stype=0x7ffffffd,offsets=[])],1)]
 rows=[]
 for name,requests,support in scenarios:
  r=run(first,None,supported_value=support,requests=requests,max_steps=600000);trace=r.pop('trace');expected=[1000059000]+list(dict.fromkeys(q['stype']for q in requests if q['stype']in types and q['stype']!=1000059000));observed=[q['stype']for q in r['query'][0]['chain']]
  assert observed==expected,(name,observed,expected)
  rows.append(dict(name=name,requested_nodes=len(requests),supported_value=support,observed_query_types=observed,query_calls=len(r['query']),duplicate_warnings=len(r['warnings']),outcome=r['outcome'],result=r.get('result'),diagnostic_type=r.get('struct'),diagnostic_member=r.get('member'),model_instruction_steps=len(trace)))
 (E/'checker-chain-protocol.json').write_text(json.dumps(dict(original_binary_sha256=hashlib.sha256(raw).hexdigest(),rows=rows,limits=['Static ARM model, synthetic reply and diagnostic-only helper/logging abstractions; no target execution.','All-known chain is a memory/protocol stress case, not a claim it satisfies every Vulkan pNext/feature-combination VUID.','Unknown/duplicate cases describe original generic behavior, not API conformance or a new driver feature.','No arbitrary corrupt pointers/cycles/lifetimes/query callback mutation/GPU validation.']),indent=2)+'\n')
 print([{k:v for k,v in r.items()if k!='observed_query_types'}for r in rows])

if __name__=='__main__':main_cli()
