"""Recover positive original property getter byte routes by bounded symbolic ARM dataflow.

This is a deliberately small interpreter for the observed leaf getter instruction
set, not target code execution or general CPU emulation. Physical property bytes
are symbolic full-byte inputs. Every executed instruction must have explicit semantics;
unknown branches, pointers, instructions and unexpected stores fail closed.
"""
from pathlib import Path
import re,json,hashlib,xml.etree.ElementTree as ET,collections
from index_native_coverage import ROOT,elf
E=ROOT/'native-complete/rutabaga-two-hours-20261027';F=ROOT/'native-complete';W=ROOT/'mesa-gamesir-recovered'
raw,secs,funcs=elf(ROOT/'originals/gs_rutabaga_v3.so');fn=next(f for f in funcs if f['name']=='vk_common_GetPhysicalDeviceProperties2');lo=fn['address'];hi=lo+fn['size']
code={}
for line in (F/'gamesir.arm64.txt').read_text().splitlines():
 m=re.match(r'\s*([a-f0-9]+): ([a-f0-9]{8})\s+(\S+)\s*(.*)',line)
 if m and lo<=int(m[1],16)<hi:code[int(m[1],16)]=(m[3],m[4].split('//')[0].strip(),m[2])
assert len(code)*4==fn['size']
section=next(s for s in secs if s['type']!=8 and s['address']<=lo and hi<=s['address']+s['size'])
for pc,(_,_,word)in code.items():
 offset=section['offset']+pc-section['address']
 assert raw[offset:offset+4]==int(word,16).to_bytes(4,'little'),hex(pc)
xml=W/'src/vulkan/registry/vk.xml';root=ET.parse(xml).getroot();enums={};aliases={}
for parent in root.iter():
 if parent.tag not in ['enums','extension','require']:continue
 extnum=parent.get('number') if parent.tag=='extension' else None
 for e in parent.findall('.//enum') if parent.tag=='extension' else parent.findall('enum'):
  n=e.get('name');v=e.get('value')
  if not n:continue
  if e.get('alias'):aliases[n]=e.get('alias')
  elif v and re.fullmatch(r'-?(0x[0-9a-fA-F]+|[0-9]+)',v):enums[n]=int(v,0)
  elif e.get('offset') and (e.get('extnumber') or extnum):
   value=1000000000+(int(e.get('extnumber')or extnum)-1)*1000+int(e.get('offset'));enums[n]=-value if e.get('dir')=='-' else value
for _ in range(len(aliases)):
 for n,a in aliases.items():
  if a in enums:enums[n]=enums[a]

types={t.get('name')or t.findtext('name'):t for t in root.findall('./types/type')}
size_cache={}
def layout(name):
 if name in size_cache:return size_cache[name]
 if name in ['void']:raise ValueError('void layout')
 fixed={'char':(1,1),'uint8_t':(1,1),'int8_t':(1,1),'uint16_t':(2,2),'int16_t':(2,2),'uint32_t':(4,4),'int32_t':(4,4),'float':(4,4),'uint64_t':(8,8),'int64_t':(8,8),'double':(8,8),'size_t':(8,8)}
 if name in fixed:return (*fixed[name],[])
 t=types[name]
 if t.get('alias'):return layout(t.get('alias'))
 category=t.get('category')
 if category in ['enum']:return(4,4,[])
 if category in ['handle','funcpointer']:return(8,8,[])
 if category in ['basetype','bitmask']:return layout(t.findtext('type'))
 if category not in ['struct']:raise ValueError(('unsupported layout',name,category))
 offset=0;alignment=1;fields=[]
 for m in t.findall('member'):
  n=m.findtext('name');ty=m.findtext('type');text=''.join(m.itertext())
  if '*'in text:sz,align,sub=8,8,[]
  else:sz,align,sub=layout(ty)
  dims=re.findall(r'\[([^\]]+)\]',text);count=1
  for d in dims:count*=int(d,0)if re.fullmatch(r'[0-9]+',d)else enums[d]
  offset=(offset+align-1)//align*align
  if sub and not dims:
   fields.extend(dict(name=n+'.'+f['name'],offset=offset+f['offset'],size=f['size'],c_type=f['c_type'],array_count=f['array_count'])for f in sub)
  else:fields.append(dict(name=n,offset=offset,size=sz*count,c_type=ty,array_count=count))
  offset+=sz*count;alignment=max(alignment,align)
 size_cache[name]=((offset+alignment-1)//alignment*alignment,alignment,fields);return size_cache[name]
structs=[];layout_errors=[]
for name,t in types.items():
 if not name or not name.startswith('VkPhysicalDevice')or'Properties'not in name or t.get('alias'):continue
 members=t.findall('member')
 if not members or members[0].findtext('name')!='sType':continue
 st=members[0].get('values')
 if st not in enums:continue
 try:sz,align,fields=layout(name)
 except(Exception)as ex:layout_errors.append(dict(name=name,error=str(ex)));continue
 structs.append(dict(name=name,stype=enums[st],size=sz,fields=[f for f in fields if f['offset']>=16]))
core=layout('VkPhysicalDeviceProperties')[2]
P=0x100000000;O=0x200000000;C=0x200100000;S=0x300000000

def bits(v,n):return tuple((v>>i)&1 for i in range(n))
def integer(v):
 assert all(isinstance(x,int)for x in v),('nonconstant value',v)
 return sum(x<<i for i,x in enumerate(v))
def width(r):return 64 if r=='sp' else {'w':32,'x':64,'s':32,'d':64,'q':128,'v':128}.get(r[0],64)
def regkey(r):return 'v'+re.search(r'\d+',r)[0] if re.fullmatch(r'[sdqv][0-9]+',r) else 'x'+r[1:] if r[0]=='w' else r

def recover(stype,partial,null_query=False):
 regs={'x0':bits(P,64),'x1':bits(O,64),'sp':bits(S,64)};mem={};stores={};trace=[];comp=None
 def putmem(a,v):
  assert len(v)%8==0
  for i in range(len(v)//8):mem[a+i]=v[8*i:8*i+8]
 if null_query:
  for at in range(C,C+0x1000):putmem(at,bits(0,8))
 putmem(O,bits(1000059001,32));putmem(O+8,bits(C,64));putmem(C,bits(stype,32));putmem(C+8,bits(0,64))
 def get(r):
  if r in ['xzr','wzr']:return bits(0,width(r))
  k=regkey(r)
  if k not in regs:regs[k]=tuple(f'initial:{k}:{i}'for i in range(128 if k[0]=='v' else 64))
  return regs[k][:width(r)]
 def setreg(r,v):
  assert len(v)==width(r),(r,len(v))
  if r in ['xzr','wzr']:return
  k=regkey(r);regs[k]=v+bits(0,(128 if k[0]=='v' else 64)-len(v))
 def value(s):return int(s[1:],0)if s.startswith('#')else integer(get(s))
 def addr(s):
  m=re.fullmatch(r'\[([^,\]]+)(?:, ([^\]]+))?\](!)?(?:, (#[^ ]+))?',s);assert m,s
  result=integer(get(m[1]))+(value(m[2])if m[2]else 0)
  if m[3]:setreg(m[1],bits(result,width(m[1])))
  if m[4]:setreg(m[1],bits(integer(get(m[1]))+value(m[4]),width(m[1])))
  return result
 def load(a,n):
  out=[]
  for offset in range(n//8):
   at=a+offset
   if P<=at<P+0x10000:out.extend(f'physical:{at-P:x}:{i}'for i in range(8))
   else:
    assert at in mem,('uninitialized memory',hex(at))
    out.extend(mem[at])
  return tuple(out)
 def store(a,v,pc):
  assert S-0x300<=a<S+0x20 or O<=a<O+0x1000 or C<=a<C+0x1000,('unexpected store',hex(a))
  putmem(a,v)
  if O<=a<O+0x1000 or C<=a<C+0x1000:
   for off in range(0,len(v),8):stores[a+off//8]=dict(bits=v[off:off+8],store_elf=hex(pc))
 partial['stores']=stores;partial['trace']=trace
 pc=lo
 for count in range(10000):
  assert pc in code,('outside getter',hex(pc));op,args,word=code[pc];trace.append(pc);nxt=pc+4
  if op in ['ldr','ldrb','ldur']:
   m=re.fullmatch(r'(\w+), (\[.*\]!?(?:[, ]+#[-0-9xa-f]+)?)',args);assert m,args
   r,at=m[1],addr(m[2]);n=8 if op=='ldrb'else width(r);setreg(r,load(at,n)+bits(0,width(r)-n))
  elif op in ['str','stur']:
   m=re.fullmatch(r'(\w+), (\[.*\]!?(?:[, ]+#[-0-9xa-f]+)?)',args);assert m,args
   store(addr(m[2]),get(m[1]),pc)
  elif op in ['ldp','stp']:
   m=re.fullmatch(r'(\w+), (\w+), (\[.*\]!?(?:[, ]+#[-0-9xa-f]+)?)',args);assert m,args
   r1,r2=m[1],m[2];at=addr(m[3]);n=width(r1);assert n==width(r2)
   if op=='ldp':setreg(r1,load(at,n));setreg(r2,load(at+n//8,n))
   else:store(at,get(r1),pc);store(at+n//8,get(r2),pc)
  elif op in ['add','sub','subs']:
   r,a,b=args.split(', ');av=value(a);bv=value(b);v=av+(1 if op=='add'else-1)*bv;setreg(r,bits(v,width(r)))
   if op=='subs':comp=(av,bv)
  elif op=='mov':
   r,v=args.split(', ')
   lane=re.fullmatch(r'v(\d+)\.([sd])\[([0-3])\]',r)
   if lane:
    source=re.fullmatch(r'v(\d+)\.([sd])\[([0-3])\]',v);assert source and source[2]==lane[2],args
    lane_size={'s':32,'d':64}[lane[2]];assert int(lane[3])*lane_size<128 and int(source[3])*lane_size<128
    dest=list(get('v'+lane[1]));value_bits=get('v'+source[1]);start=int(lane[3])*lane_size;source_start=int(source[3])*lane_size
    dest[start:start+lane_size]=value_bits[source_start:source_start+lane_size];setreg('v'+lane[1],tuple(dest))
   else:
    assert r=='sp'or re.fullmatch(r'[wxsdqv][0-9]+',r),('unsupported register',r)
    assert v.startswith('#')or v in ['sp','xzr','wzr']or re.fullmatch(r'[wxsdqv][0-9]+',v),('unsupported register',v)
    setreg(r,bits(value(v),width(r))if v.startswith('#')else get(v)[:width(r)])
  elif op=='movk':
   r,v,shift=args.split(', ');shift=int(shift.split('#')[1],0);old=integer(get(r));v=(old&~(0xffff<<shift))|(value(v)<<shift);setreg(r,bits(v,width(r)))
  elif op=='ushll':
   r,src,shift=args.split(', ');assert shift=='#0x0';m=re.fullmatch(r'v(\d+)\.(\d+)([bhsd])',src);assert m,src
   lane={'b':8,'h':16,'s':32,'d':64}[m[3]];countlanes=int(m[2]);sv=get('v'+m[1]);out=()
   for i in range(countlanes):out+=sv[i*lane:(i+1)*lane]+bits(0,lane)
   assert len(out)==128;setreg('v'+r.split('.')[0][1:],out)
  elif op=='bic':
   r,imm=args.split(', ');assert r=='v0.4h'and imm=='#0xfe';old=list(get('v0'))
   for lane in range(4):
    for i in range(1,8):old[lane*16+i]=0
   # 64-bit SIMD destination zeros upper64 bits.
   old[64:]=bits(0,64);setreg('v0',tuple(old))
  elif op=='zip1':
   r,a,b=args.split(', ');m=re.fullmatch(r'v(\d+)\.(\d+)([sd])',r);assert m,args
   lanes=int(m[2]);lane={'s':32,'d':64}[m[3]];assert all(x.endswith('.'+m[2]+m[3])for x in [a,b]),args
   av=get(a.split('.')[0]);bv=get(b.split('.')[0]);out=()
   for i in range(lanes//2):out+=av[i*lane:(i+1)*lane]+bv[i*lane:(i+1)*lane]
   setreg('v'+m[1],out+bits(0,128-len(out)))
  elif op in ['and','lsl','umull']:
   r,a,b=args.split(', ');av=value(a);bv=value(b)
   v=av&bv if op=='and'else av<<bv if op=='lsl'else av*bv
   setreg(r,bits(v,width(r)))
  elif op=='csel':
   r,a,b,condition=args.split(', ');assert comp is not None;av,bv=comp
   yes={'eq':av==bv,'ne':av!=bv,'lo':av<bv,'hs':av>=bv,'gt':av>bv,'le':av<=bv}[condition]
   setreg(r,get(a if yes else b))
  elif op=='bl':
   assert args.startswith('0xce5370 '),args
   dest=integer(get('x0'));source=integer(get('x1'));length=integer(get('x2'));assert length<=4096
   for i in range(length):store(dest+i,load(source+i,8),pc)
  elif op=='cmp':
   a,b=args.split(', ');size=width(a);av=value(a);bv=value(b);av=av-(1<<size)if av>>(size-1)else av;bv=bv-(1<<size)if bv>>(size-1)else bv;comp=(av,bv)
  elif op=='b':nxt=int(args.split()[0],16)
  elif op.startswith('b.'):
   assert comp is not None;av,bv=comp;condition={'b.eq':av==bv,'b.ne':av!=bv,'b.gt':av>bv,'b.le':av<=bv,'b.lo':av<bv}[op]
   if condition:nxt=int(args.split()[0],16)
  elif op in ['cbz','cbnz']:
   r,to=args.split(', ')
   if (integer(get(r))==0)==(op=='cbz'):nxt=int(to.split()[0],16)
  elif op=='ret':return stores,trace
  else:raise AssertionError(('unsupported opcode',op,args))
  pc=nxt
 raise AssertionError('step limit')


rows=[];empty=[];partial_cases=[];visited=set();root_records={};handled=[]
def routes(stores,base,fields):
 result=[]
 for field in fields:
  copied=[stores.get(base+field['offset']+i)for i in range(field['size'])]
  if not any(copied):continue
  chunks=[]
  for i,cell in enumerate(copied):
   if cell is None:chunks.append(dict(api_byte=i,kind='unwritten'));continue
   v=cell['bits'];m=re.fullmatch(r'physical:([0-9a-f]+):0',str(v[0]))
   if m and v==tuple(f"physical:{m[1]}:{k}"for k in range(8)):
    entry=dict(api_byte=i,kind='copy',physical_offset=int(m[1],16),store_elf=cell['store_elf'])
   elif all(isinstance(x,int)for x in v):entry=dict(api_byte=i,kind='literal',value=integer(v),store_elf=cell['store_elf'])
   else:entry=dict(api_byte=i,kind='symbolic',bits=v,store_elf=cell['store_elf'])
   # Compress adjacent physical copy bytes, retaining every contributing store.
   if chunks and entry['kind']=='copy' and chunks[-1]['kind']=='copy' and entry['api_byte']==chunks[-1]['api_byte']+chunks[-1].get('bytes',1) and entry['physical_offset']==chunks[-1]['physical_offset']+chunks[-1].get('bytes',1):
    prev=chunks[-1];prev['bytes']=prev.get('bytes',1)+1
    if entry['store_elf']not in prev['store_instructions']:prev['store_instructions'].append(entry['store_elf'])
   else:
    if entry['kind']=='copy':entry['bytes']=1;entry['store_instructions']=[entry.pop('store_elf')]
    chunks.append(entry)
  result.append(dict(member=field['name'],api_member_offset=field['offset'],size=field['size'],c_type=field['c_type'],array_count=field['array_count'],fully_written=all(copied),byte_routes=chunks))
 return result
for st in structs:
 partial={};error=None
 try:stores,trace=recover(st['stype'],partial)
 except(Exception)as ex:stores=partial.get('stores',{});trace=partial.get('trace',[]);error=str(ex)
 visited.update(trace)
 for r in routes(stores,O,[dict(f,offset=f['offset']+16)for f in core]):root_records[r['member']]=r
 mapped=routes(stores,C,st['fields'])
 if mapped:
  handled.append(st['name']);rows.extend(dict(struct=st['name'],stype=st['stype'],**r)for r in mapped)
 else:empty.append(st['name'])
 if error:partial_cases.append(dict(struct=st['name'],stype=st['stype'],reason=error,last_instruction=hex(trace[-1])if trace else None))
null_query_cases=[]
for case in partial_cases:
 st=next(t for t in structs if t['name']==case['struct']);partial={};error=None
 try:stores,trace=recover(st['stype'],partial,True)
 except(Exception)as ex:stores=partial.get('stores',{});trace=partial.get('trace',[]);error=str(ex)
 visited.update(trace)
 null_query_cases.append(dict(struct=st['name'],assumption='Caller property payload initially zero: null output arrays and zero capacities; not other valid callers',complete_path=error is None,error=error,rows=routes(stores,C,st['fields'])))
result=dict(original_binary_sha256=hashlib.sha256(raw).hexdigest(),getter_elf=hex(lo),getter_size=fn['size'],source_registry_sha256=hashlib.sha256(xml.read_bytes()).hexdigest(),mapped_structures=len(handled),mapped_member_records=len(rows),core_member_records=len(root_records),instruction_count=len(code),visited_static_instructions=len(visited),unvisited_instruction_addresses=[hex(a)for a in code if a not in visited],core=list(root_records.values()),rows=rows,partial_cases=partial_cases,current_registry_structures_without_original_stores=empty,layout_errors=layout_errors,null_query_cases=null_query_cases,limits=['Positive static getter byte routes only; no driver execution.','AArch64 registry field layout, no assumption of current Mesa physical ABI.','Dynamic/user-pointer/copy/count paths fail closed and retain partial routes; see partial_cases.','Struct/member presence is not advertised support or a valid initializer/feature contract.','Initialization values, SDK/platform filters and API/GPU behavior remain deferred.'])
(E/'recovered-property-getter-fields.json').write_text(json.dumps(result,indent=2)+'\n')
print({k:v for k,v in result.items()if k not in ['core','rows','partial_cases','unvisited_instruction_addresses','current_registry_structures_without_original_stores','layout_errors','null_query_cases','limits']})
print('partial',len(partial_cases),'empty',len(empty),'layout_errors',len(layout_errors))
print(partial_cases[:3])

# Positive, non-overlapping original physical fields for later typed analysis.
# No caller-input scalar or pointer is renamed into a physical-device field.
candidates={};conflicts=[]
for row in list(root_records.values())+rows+[dict(r,struct=c['struct'])for c in null_query_cases for r in c['rows']]:
 segments=row['byte_routes']
 if not row['fully_written']or len(segments)!=1 or segments[0]['kind']!='copy' or segments[0]['api_byte']!=0 or segments[0]['bytes']!=row['size']:continue
 offset=segments[0]['physical_offset'];name=re.sub(r'[^A-Za-z0-9_]','_',row['member'].removeprefix('limits.').removeprefix('sparseProperties.'))
 if offset in candidates:
  if candidates[offset]['size']!=row['size']:conflicts.append(dict(offset=hex(offset),old=candidates[offset]['name'],new=name));continue
  candidates[offset]['aliases'].add(name);continue
 candidates[offset]=dict(name=name,size=row['size'],c_type=row['c_type'],array_count=row['array_count'],aliases={name})
selected=[];used=set();overlaps=[]
for offset,field in sorted(candidates.items()):
 region=set(range(offset,offset+field['size']))
 if region&used:overlaps.append(dict(offset=hex(offset),field=field['name']));continue
 assert offset>=0x444 and offset+field['size']<=0x1638
 used|=region;selected.append(dict(offset=hex(offset),**{k:sorted(v)if isinstance(v,set)else v for k,v in field.items()}))
names=collections.Counter(f['name']for f in selected)
for field in selected:
 if names[field['name']]>1:field['name']+='_'+field['offset'][2:]
(E/'original-property-physical-fields.json').write_text(json.dumps(dict(fields=selected,source='recovered-property-getter-fields.json',size_conflicts=conflicts,partial_overlaps=overlaps,limits=['Positive full-field contiguous byte copies only.','Getter type/name routes, no initialization or feature support validation.','Opaque/unobserved bytes retain no inferred names.']),indent=2)+'\n')
(E/'original-property-fields.tsv').write_text(''.join(f"{int(f['offset'],16):x}\t{f['size']}\t{f['c_type']}\t{f['array_count']}\t{f['name']}\n"for f in selected))
print('Original non-overlapping physical property fields',len(selected),'bytes',len(used))
