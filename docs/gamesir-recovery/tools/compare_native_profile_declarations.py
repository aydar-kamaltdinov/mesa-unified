"""Read declarative Python profile AST, without importing/running its generator.
Compare only statically understood named properties with native record values.
Unknown expressions remain explicit; equality is not hardware validation.
"""
from pathlib import Path
import ast,json,operator,hashlib,collections
from index_native_coverage import ROOT
E=ROOT/'native-complete/rutabaga-two-hours-20261027';W=ROOT/'mesa-gamesir-recovered'
p=W/'src/freedreno/common/freedreno_devices.py';tree=ast.parse(p.read_text())
assign={n.targets[0].id:n.value for n in tree.body if isinstance(n,ast.Assign) and len(n.targets)==1 and isinstance(n.targets[0],ast.Name)}
info_tree=ast.parse((W/'src/freedreno/common/freedreno_dev_info.py').read_text());defs={n.targets[0].id:n.value for n in info_tree.body if isinstance(n,ast.Assign) and len(n.targets)==1 and isinstance(n.targets[0],ast.Name)}
quirks=ast.literal_eval(defs['quirk_names']);aliases=ast.literal_eval(defs['legacy_quirk_aliases'])
enums={}
for c in info_tree.body:
 if isinstance(c,ast.ClassDef) and c.name in ['CHIP','CCUColorCacheFraction']:
  enums[c.name]={n.targets[0].id:ast.literal_eval(n.value) for n in c.body if isinstance(n,ast.Assign)}
class Unknown(Exception):pass
registers=json.loads((E/'current-register-constants.json').read_text())['values']
cache={}
def ev(n):
 if isinstance(n,ast.Constant):return n.value
 if isinstance(n,ast.Name):
  if n.id in cache:return cache[n.id]
  if n.id in assign:cache[n.id]=ev(assign[n.id]);return cache[n.id]
  raise Unknown(n.id)
 if isinstance(n,(ast.List,ast.Tuple)):return [ev(x) for x in n.elts]
 if isinstance(n,ast.IfExp):return ev(n.body) if ev(n.test) else ev(n.orelse)
 if isinstance(n,ast.Compare)and len(n.ops)==1:
  a,b=ev(n.left),ev(n.comparators[0]);op=n.ops[0]
  if isinstance(op,ast.Eq):return a==b
  if isinstance(op,ast.NotEq):return a!=b
  raise Unknown(ast.unparse(n))
 if isinstance(n,ast.ListComp)and len(n.generators)==1 and not n.generators[0].ifs:
  g=n.generators[0];names=[x.id for x in g.target.elts] if isinstance(g.target,ast.Tuple) else [g.target.id]
  class Bind(ast.NodeTransformer):
   def __init__(self,env):self.env=env
   def visit_Name(self,x):return ast.Constant(self.env[x.id]) if x.id in self.env else x
  result=[]
  for value in ev(g.iter):
   bound=dict(zip(names,value if len(names)>1 else [value]));expr=Bind(bound).visit(ast.parse(ast.unparse(n.elt),mode='eval').body);result.append(ev(expr))
  return result
 if isinstance(n,ast.UnaryOp):
  return {ast.USub:operator.neg,ast.UAdd:operator.pos,ast.Invert:operator.invert}[type(n.op)](ev(n.operand))
 if isinstance(n,ast.BinOp):
  fn={ast.Add:operator.add,ast.Sub:operator.sub,ast.Mult:operator.mul,ast.FloorDiv:operator.floordiv,ast.Div:operator.truediv,ast.LShift:operator.lshift,ast.RShift:operator.rshift,ast.BitOr:operator.or_,ast.BitAnd:operator.and_,ast.BitXor:operator.xor}.get(type(n.op))
  if fn:return fn(ev(n.left),ev(n.right))
 if isinstance(n,ast.Attribute):
  if isinstance(n.value,ast.Name) and n.value.id=='A6XXRegs':return registers[n.attr]
  if n.attr=='value':return ev(n.value)
  if isinstance(n.value,ast.Name) and n.value.id in enums:return enums[n.value.id][n.attr]
 if isinstance(n,ast.Call) and isinstance(n.func,ast.Name):
  name=n.func.id
  if name=='GPUProps':
   out={}
   for k in n.keywords:
    try:out[k.arg]=ev(k.value)
    except Unknown:out[k.arg]={'unresolved':ast.unparse(k.value)}
   return out
  if name=='GPUId':
   a=[ev(x) for x in n.args];k={x.arg:ev(x.value) for x in n.keywords};gpu=k.get('gpu_id',a[0] if a else 0);chip=k.get('chip_id')
   if chip is None:chip=(gpu//100<<24)|((gpu%100)//10<<16)|((gpu%10)<<8)|255
   return dict(gpu_id=gpu,chip_id=hex(chip),name=k.get('name','FD'+str(gpu)))
  if name in ('GPUInfo','A6xxGPUInfo'):
   chip=ev(n.args[0]);k={x.arg:x.value for x in n.keywords};props={}
   if name=='A6xxGPUInfo':
    template=ev(n.args[1]) if len(n.args)>1 else ev(k['template']);template=template if isinstance(template,list) else [template]
    for t in template:props.update(t)
   elif 'reg_size_vec4' in k:props['reg_size_vec4']=ev(k['reg_size_vec4'])
   header,magic=profile_header_and_magic(n)
   return dict(chip=chip,properties=props,header=header,raw_magic=magic)
  if name=='max_bitfield_val':
   high,low,shift=[ev(x) for x in n.args];return ((1<<(high-low))-1)<<shift
 raise Unknown(ast.unparse(n))
def constructor(name):
 cls=next(c for c in info_tree.body if isinstance(c,ast.ClassDef)and c.name==name)
 return next(f for f in cls.body if isinstance(f,ast.FunctionDef)and f.name=='__init__')
def defaults(fn):
 return {a.arg:ev(v) for a,v in zip(fn.args.args[-len(fn.args.defaults):],fn.args.defaults)} if fn.args.defaults else {}
def local(n,env):
 if isinstance(n,ast.Name)and n.id in env:return env[n.id]
 if isinstance(n,ast.Attribute)and n.attr=='value':return local(n.value,env)
 return ev(n)
def profile_header_and_magic(call):
 name=call.func.id;fn=constructor(name);args={**defaults(fn),**{a.arg:ev(v) for a,v in zip(fn.args.args[1:],call.args)},**{k.arg:ev(k.value) for k in call.keywords}}
 magic=[]
 if name=='A6xxGPUInfo':
  args['compute_lb_size']=0 if args['chip']==6 else 40*1024
  supercall=next(n.value for n in fn.body if isinstance(n,ast.Expr)and isinstance(n.value,ast.Call)and isinstance(n.value.func,ast.Attribute)and n.value.func.attr=='__init__')
  baseargs={**defaults(constructor('GPUInfo')),**{k.arg:local(k.value,args) for k in supercall.keywords}}
  baseargs['chip']=args['chip']
  magic=args.get('raw_magic_regs') or []
 else:baseargs=args
 header={}
 for stmt in constructor('GPUInfo').body:
  if isinstance(stmt,ast.Assign)and len(stmt.targets)==1:
   target=stmt.targets[0]
   if isinstance(target,ast.Attribute)and isinstance(target.value,ast.Name)and target.value.id=='self' and target.attr!='props':header[target.attr]=local(stmt.value,baseargs)
 if name=='A6xxGPUInfo':header['num_slices']=args['num_slices']
 return header,[{'reg':hex(reg),'value':hex(value&0xffffffff)} for reg,value in magic]

records=[];skipped=[]
for n in tree.body:
 if not(isinstance(n,ast.Expr)and isinstance(n.value,ast.Call)and isinstance(n.value.func,ast.Name)and n.value.func.id=='add_gpus'):continue
 try:
  ids=ev(n.value.args[0]);info=ev(n.value.args[1])
  for ident in ids:records.append(dict(ident,**info,line=n.lineno))
 except Unknown as ex:skipped.append(dict(line=n.lineno,reason=str(ex)))
index=collections.defaultdict(list)
for r in records:index[(r['gpu_id'],r['chip_id'])].append(r)
native=json.loads((E/'native-device-records.json').read_text());fields=json.loads((E/'native-device-property-names.json').read_text())['named_fields'];types={x['name']:x['parser'] for x in fields}
rows=[]
for n in native['records']:
 match=index.get((n['gpu_id'],n['chip_id']),[])
 if len(match)!=1:rows.append(dict(name=n['name'],chip_id=n['chip_id'],state='missing-or-ambiguous',source_matches=len(match)));continue
 s=match[0];diff=[];equal=0;unknown=[]
 for old,value in n['properties'].items():
  target=aliases.get(old,old);target=target.split('_')[0] if target.startswith('QCTDD') else target
  local=s['properties'].get(target,False if types[old]=='bool' else 0)
  if isinstance(local,dict):unknown.append(dict(property=old,expression=local));continue
  if value!=local:diff.append(dict(property=old,current_property=target,native=value,source=local))
  else:equal+=1
 header_diff=[]
 for field,value in n['header'].items():
  local_value=s['header'].get(field,0)&0xffffffff
  if value!=local_value:header_diff.append(dict(field=field,native=value,source=local_value))
 native_magic={x['reg']:x['value'] for x in n['raw_magic']};source_magic={x['reg']:x['value'] for x in s['raw_magic']}
 magic_diff=[dict(reg=k,native=native_magic.get(k),source=source_magic.get(k)) for k in sorted(native_magic.keys()|source_magic.keys()) if native_magic.get(k)!=source_magic.get(k)]
 rows.append(dict(name=n['name'],chip_id=n['chip_id'],chip=n['chip'],source_line=s['line'],state='static-declarations-compared',equal=equal,differences=diff,unresolved=unknown,header_differences=header_diff,raw_magic_differences=magic_diff,native_legacy_magic=n['legacy_magic'],source_raw_magic=s['raw_magic']))
result=dict(source_sha256=hashlib.sha256(p.read_bytes()).hexdigest(),source_profiles=len(records),unparsed_source_records=skipped,rows=rows,limits=['Restricted AST reader; does not run Python generator, driver or shaders.','Only declared props and zero-initialized absent defaults modeled; no magic-register/capability execution validation.','Current quirks use normalized QCTDD ids; matching spellings alone do not prove behavioral equivalence.','Differences can be retained upstream fixes or deliberate local changes, not automatically missing GameSir ports.'])
(E/'native-profile-declaration-comparison.json').write_text(json.dumps(result,indent=2)+'\n');print('source records',len(records),'unparsed',skipped)
for row in rows:
 if row.get('chip',0)>=6:print(row['name'],row['chip_id'],'props_equal',row.get('equal'),'header_diff',row.get('header_differences'),'raw_diff',row.get('raw_magic_differences'))
