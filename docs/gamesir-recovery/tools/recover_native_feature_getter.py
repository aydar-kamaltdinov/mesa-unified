"""Recover original feature getter field routes by bounded symbolic ARM dataflow.

This is a deliberately small interpreter for the observed leaf getter instruction
set, not target code execution or general CPU emulation. Physical feature bytes
are symbolic 0/1 bits. Every executed instruction must have explicit semantics;
unknown branches, pointers, instructions and unexpected stores fail closed.
"""
from pathlib import Path
import re,json,hashlib,xml.etree.ElementTree as ET,collections
from index_native_coverage import ROOT,elf
E=ROOT/'native-complete/rutabaga-two-hours-20261027';F=ROOT/'native-complete';W=ROOT/'mesa-gamesir-recovered'
raw,secs,funcs=elf(ROOT/'originals/gs_rutabaga_v3.so');fn=next(f for f in funcs if f['name']=='vk_common_GetPhysicalDeviceFeatures2');lo=fn['address'];hi=lo+fn['size']
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
structs=[]
for t in root.findall('./types/type'):
 name=t.get('name')or t.findtext('name');members=t.findall('member')
 if not name or not name.startswith('VkPhysicalDevice')or not members or name=='VkPhysicalDeviceFeatures':continue
 if members[0].findtext('name')!='sType' or not members[0].get('values'):continue
 if not all(m.findtext('type')=='VkBool32' for m in members[2:])or len(members)<3:continue
 st=members[0].get('values')
 if st not in enums:continue
 structs.append(dict(name=name,stype=enums[st],members=[m.findtext('name')for m in members[2:]]))
core=[m.findtext('name')for m in root.find("./types/type[@name='VkPhysicalDeviceFeatures']").findall('member')];assert len(core)==55
P=0x100000000;O=0x200000000;C=0x200100000;S=0x300000000

def bits(v,n):return tuple((v>>i)&1 for i in range(n))
def integer(v):
 assert all(isinstance(x,int)for x in v),('nonconstant value',v)
 return sum(x<<i for i,x in enumerate(v))
def width(r):return 64 if r=='sp' else {'w':32,'x':64,'s':32,'d':64,'q':128,'v':128}.get(r[0],64)
def regkey(r):return 'v'+re.search(r'\d+',r)[0] if re.fullmatch(r'[sdqv][0-9]+',r) else 'x'+r[1:] if r[0]=='w' else r

def recover(stype):
 regs={'x0':bits(P,64),'x1':bits(O,64),'sp':bits(S,64)};mem={};stores={};trace=[];comp=None
 def putmem(a,v):
  assert len(v)%8==0
  for i in range(len(v)//8):mem[a+i]=v[8*i:8*i+8]
 putmem(O,bits(1000059000,32));putmem(O+8,bits(C,64));putmem(C,bits(stype,32));putmem(C+8,bits(0,64))
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
  m=re.fullmatch(r'\[([^,\]]+)(?:, (#[^\]]+))?\]',s);assert m,s
  return integer(get(m[1]))+(value(m[2])if m[2]else 0)
 def load(a,n):
  out=[]
  for offset in range(n//8):
   at=a+offset
   if P+0x207<=at<P+0x445:out.extend((f'feature:{at-P:x}',0,0,0,0,0,0,0))
   else:
    assert at in mem,('uninitialized memory',hex(at))
    out.extend(mem[at])
  return tuple(out)
 def store(a,v,pc):
  assert S-0xe0<=a<S+0x20 or O<=a<O+0x1000 or C<=a<C+0x1000,('unexpected store',hex(a))
  putmem(a,v)
  if O<=a<O+0x1000 or C<=a<C+0x1000:
   for off in range(0,len(v),32):
    chunk=v[off:off+32]
    assert len(chunk)==32 and all(x==0 for x in chunk[1:]),('not one bool copy',hex(pc),chunk)
    assert isinstance(chunk[0],str)and chunk[0].startswith('feature:'),('not feature source',hex(pc),chunk)
    stores[a+off//8]=dict(native_physical_offset='0x'+chunk[0].split(':')[1],store_elf=hex(pc))
 pc=lo
 for count in range(10000):
  assert pc in code,('outside getter',hex(pc));op,args,word=code[pc];trace.append(pc);nxt=pc+4
  if op in ['ldr','ldrb','ldur']:
   m=re.fullmatch(r'(\w+), (\[.*\])',args);assert m,args
   r,at=m[1],addr(m[2]);n=8 if op=='ldrb'else width(r);setreg(r,load(at,n)+bits(0,width(r)-n))
  elif op in ['str','stur']:
   m=re.fullmatch(r'(\w+), (\[.*\])',args);assert m,args
   store(addr(m[2]),get(m[1]),pc)
  elif op in ['ldp','stp']:
   m=re.fullmatch(r'(\w+), (\w+), (\[.*\])',args);assert m,args
   r1,r2=m[1],m[2];at=addr(m[3]);n=width(r1);assert n==width(r2)
   if op=='ldp':setreg(r1,load(at,n));setreg(r2,load(at+n//8,n))
   else:store(at,get(r1),pc);store(at+n//8,get(r2),pc)
  elif op in ['add','sub']:
   r,a,b=args.split(', ');v=value(a)+(1 if op=='add'else-1)*value(b);setreg(r,bits(v,width(r)))
  elif op=='mov':
   r,v=args.split(', ');setreg(r,bits(value(v),width(r)))
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
  elif op=='cmp':
   a,b=args.split(', ');size=width(a);av=value(a);bv=value(b);av=av-(1<<size)if av>>(size-1)else av;bv=bv-(1<<size)if bv>>(size-1)else bv;comp=(av,bv)
  elif op=='b':nxt=int(args.split()[0],16)
  elif op.startswith('b.'):
   assert comp is not None;av,bv=comp;condition={'b.eq':av==bv,'b.ne':av!=bv,'b.gt':av>bv,'b.le':av<=bv}[op]
   if condition:nxt=int(args.split()[0],16)
  elif op=='cbz':
   r,to=args.split(', ')
   if integer(get(r))==0:nxt=int(to.split()[0],16)
  elif op=='ret':return stores,trace
  else:raise AssertionError(('unsupported opcode',op,args))
  pc=nxt
 raise AssertionError('step limit')

rows=[];empty=[];visited=set();rootmaps=set();core_routes={}
for st in structs:
 stores,trace=recover(st['stype']);visited.update(trace)
 rmap={}
 for i,name in enumerate(core):
  m=stores[O+16+4*i];rmap[name]=m['native_physical_offset'];core_routes[name]=m
 rootmaps.add(tuple(rmap.items()))
 found=0
 for i,name in enumerate(st['members']):
  a=C+16+4*i
  if a in stores:rows.append(dict(struct=st['name'],stype=st['stype'],member=name,api_member_offset=16+4*i,**stores[a]));found+=1
 extra=[a for a in stores if C<=a<C+0x1000 and not C+16<=a<C+16+4*len(st['members'])]
 assert not extra,(st,extra)
 if found:assert found==len(st['members']),(st,found)
 else:empty.append(st['name'])
assert len(rootmaps)==1
core_rows=[dict(struct='VkPhysicalDeviceFeatures',member=n,api_member_offset=4*i,**core_routes[n])for i,n in enumerate(core)]
result=dict(original_binary_sha256=hashlib.sha256(raw).hexdigest(),getter_elf=hex(lo),getter_size=fn['size'],source_registry_sha256=hashlib.sha256(xml.read_bytes()).hexdigest(),mapped_structures=len(set(r['struct']for r in rows)),mapped_selector_values=len(set(r['stype']for r in rows)),member_records=len(rows),core_records=len(core_rows),distinct_physical_offsets=len(set(r['native_physical_offset']for r in rows+core_rows)),executed_symbolic_instructions=len(visited),instruction_count=len(code),unvisited_instruction_addresses=[hex(a)for a in code if a not in visited],core=core_rows,rows=rows,current_registry_structures_without_original_stores=empty,limits=['Bounded static symbolic routes for exact original getter only; original code never executed.','Feature-byte inputs modeled as boolean0/1; no arbitrary byte/undefined input semantics claimed.','Registry names/order/structure offsets used; offsets are original physical ABI, not current Mesa ABI.','Initialization values, platform filters, advertised support and actual GPU feature behavior remain separate.'])
(E/'recovered-feature-getter-fields.json').write_text(json.dumps(result,indent=2)+'\n')
print({k:v for k,v in result.items()if k not in ['core','rows','current_registry_structures_without_original_stores','unvisited_instruction_addresses','limits']})
print('registry no stores',len(empty),'unvisited',len(result['unvisited_instruction_addresses']))

# Emit the same names in a compact Ghidra original-ABI field map. The rename
# table is parsed as data, not imported/executed as a generator.
import ast
module=ast.parse((W/'src/vulkan/util/vk_physical_device_features_gen.py').read_text())
renamed=next(ast.literal_eval(n.value)for n in module.body if isinstance(n,ast.Assign)and any(isinstance(t,ast.Name)and t.id=='RENAMED_FEATURES'for t in n.targets))
fields=collections.defaultdict(set)
for row in core_rows+rows:
 name=renamed.get((row['struct'].removeprefix('VkPhysicalDevice'),row['member']),row['member'])
 fields[int(row['native_physical_offset'],16)].add(name)
assert all(len(n)==1 for n in fields.values())
assert len({next(iter(n))for n in fields.values()})==len(fields)
(E/'original-feature-fields.tsv').write_text(''.join(f'{o:x}\t{next(iter(n))}\n'for o,n in sorted(fields.items())))
