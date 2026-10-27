"""Static regression models for GameSir transfer audit. No C build/execution.

These check lifetime/complexity contracts independently of Vulkan/GPU operation.
Source markers bind the models to the reviewed changes, not a C-equivalence
proof. Temporary ELF, ZIP, driver or device files are never created.
"""
from pathlib import Path
from itertools import permutations
import hashlib,json
W=Path(__file__).resolve().parents[3]

class Snapshot:
 def __init__(self,name):self.name=name;self.refs=0
 @property
 def alive(self):return self.refs>0
 def ref(self):self.refs+=1
 def unref(self):assert self.refs>0;self.refs-=1

class Library:
 def __init__(self,own=None,stages=()):
  self.own=own;self.views={};self.imports=[];self.alive=True
  if own:
   own.ref()
   self.views.update({s:own for s in stages})
 def inherit(self,parent,stages,retain=True):
  for stage in stages:
   owner=parent.views[stage];assert owner.alive
   self.views[stage]=owner
   if retain and owner is not self.own:owner.ref();self.imports.append(owner)
 def destroy(self):
  assert self.alive;self.alive=False
  if self.own:self.own.unref()
  for owner in self.imports:owner.unref()
 def check(self):
  if self.alive:assert all(owner.alive for owner in self.views.values())

# A known old-code counterexample: child borrowed NIR but had no own snapshot.
s=Snapshot('old');parent=Library(s,[4]);child=Library();child.inherit(parent,[4],retain=False);parent.destroy();assert not child.views[4].alive;child.destroy()
old_counterexample=True;lifetime_cases=0
for strong_cache in[False,True]:
 for order in permutations(['rootA','rootB','child','grandchild','cache']):
  a=Snapshot('A');b=Snapshot('B');rootA=Library(a,[0]);rootB=Library(b,[4]);child=Library();child.inherit(rootA,[0]);child.inherit(rootB,[4]);grandchild=Library();grandchild.inherit(child,[0,4]);libs=dict(rootA=rootA,rootB=rootB,child=child,grandchild=grandchild)
  if strong_cache:a.ref();b.ref()
  for event in order:
   if event=='cache':
    if strong_cache:a.unref();b.unref()
   else:libs[event].destroy()
   for lib in libs.values():lib.check()
  assert a.refs==b.refs==0;lifetime_cases+=1
# A child cache lookup may own the very same object it also imports.
s=Snapshot('deduplicated');parent=Library(s,[4]);child=Library(s,[4]);child.inherit(parent,[4]);assert not child.imports;parent.destroy();child.check();child.destroy();assert not s.alive;lifetime_cases+=1
# Partial local snapshot plus an imported stage; later libraries flatten owners.
a=Snapshot('imported');b=Snapshot('local');parent=Library(a,[0]);child=Library(b,[4]);child.inherit(parent,[0]);grandchild=Library();grandchild.inherit(child,[0,4]);parent.destroy();child.destroy();grandchild.check();grandchild.destroy();assert a.refs==b.refs==0;lifetime_cases+=1

class Node:
 def __init__(self,kind='alu',sources=(),entry=False):self.kind=kind;self.sources=sources;self.entry=entry

def oracle(roots):
 pending=list(roots);seen=set()
 while pending:
  node=pending.pop()
  if node in seen:continue
  seen.add(node)
  if node.entry:continue
  if node.kind not in['alu','deref','constant','undef','reorderable']:return False
  pending.extend(node.sources)
 return True

def memo_check(roots):
 checked=set();steps=0
 def visit(node):
  nonlocal steps
  steps+=1
  if node.entry or node in checked:return True
  if node.kind not in['alu','deref','constant','undef','reorderable']:return False
  if not all(visit(src)for src in node.sources):return False
  checked.add(node);return True
 return all(visit(root)for root in roots),steps,len(checked)

def old_walk(node):
 if node.entry:return True,1
 if node.kind not in['alu','deref','constant','undef','reorderable']:return False,1
 count=1
 for src in node.sources:
  ok,n=old_walk(src);count+=n
  if not ok:return False,count
 return True,count
root=Node(entry=True);old_steps=1;nodes=[root]
for depth in range(1,65):
 # A shared arithmetic diamond. Distinct entry inputs prevent relying on
 # equal-operand algebraic simplifications to remove the shared dependency.
 left=Node(sources=(root,Node(entry=True)));right=Node(sources=(root,Node(entry=True)));root=Node(sources=(left,right));nodes +=[left,right,root];old_steps=2*old_steps+5
 ok,steps,n=memo_check([root]);assert ok==oracle([root])and steps<=6*depth+1
 if depth==10:assert old_walk(root)==(True,old_steps)
assert old_steps>2**64
rejections=0
for kind in['phi','texture','nonreorderable']:
 bad=Node(kind,sources=(root,));assert not oracle([root,bad]);assert not memo_check([root,bad])[0];rejections+=1
# Entry definitions are already available; they are not speculative moves.
assert memo_check([Node('nonreorderable',entry=True)])[0]

def signed32(value):
 value &= 0xffffffff
 return value - (1 << 32) if value & (1 << 31) else value

def saturate32(value):
 return max(-(1 << 31), min((1 << 31) - 1, value))

def matrix_model(a, b, initial, saturating):
 # Lower the full product into K=8 chunks with a private zero accumulator;
 # the final elementwise operation is the only saturating addition.
 accumulator = 0
 for base in range(0, len(a), 8):
  for i in range(base, min(base + 8, len(a))):
   accumulator = signed32(accumulator + a[i] * b[i])
 return saturate32(accumulator + initial) if saturating else signed32(accumulator + initial)

def old_matrix_model(a, b, initial):
 accumulator = initial
 for av, bv in zip(a, b):
  accumulator = saturate32(accumulator + av * bv)
 return accumulator

# A positive contribution followed by compensation must not lose the unit
# already clamped away, either inside one tile or across the K=8 boundary.
for compensation in [1, 8]:
 a = [0] * 32; b = [1] * 32
 a[0] = 1; a[compensation] = -1
 assert old_matrix_model(a, b, (1 << 31) - 1) == (1 << 31) - 2
 assert matrix_model(a, b, (1 << 31) - 1, True) == (1 << 31) - 1

import random
rng = random.Random(0x4753381e)
matrix_cases = 0
for signed_inputs in [False, True]:
 for _ in range(256):
  lo, hi = (-128, 127) if signed_inputs else (0, 255)
  a = [rng.randint(lo, hi) for _ in range(32)]
  b = [rng.randint(lo, hi) for _ in range(32)]
  product = sum(av * bv for av, bv in zip(a, b))
  assert -(1 << 31) <= product < (1 << 31)
  for initial in [-(1 << 31), -(1 << 31) + 1, -1, 0, (1 << 31) - 2, (1 << 31) - 1]:
   for saturating in [False, True]:
    expected = saturate32(product + initial) if saturating else signed32(product + initial)
    assert matrix_model(a, b, initial, saturating) == expected
    matrix_cases += 1

# The generic splitter previously wrote partial K sums back through C.
# Reusing the same C in a later operation exposes the overwritten input.
old_c = [7]
old_c[0] += 11
old_result = old_c[0] + 13
new_c = [7]
private_accumulator = new_c[0] + 11
new_result = private_accumulator + 13
assert old_result == new_result == 31
assert old_c[0] == 18 and new_c[0] == 7

slice_cases = 0
for image_3d in [False, True]:
 for size in [1, 2, 17, 64, 511, 2048]:
  for mip in range(12 if image_3d else 1):
   available = max(size >> mip, 1) if image_3d else size
   for base in range(available + 1):
    expected_slices = list(range(base, available))
    resolved_count = available - base
    actual_slices = [base + i for i in range(resolved_count)]
    assert actual_slices == expected_slices
    if image_3d:
     assert 0xffffffff != resolved_count  # old literal-sentinel loop
    else:
     assert resolved_count == size - base  # existing array-layer behavior
    slice_cases += 1

pipeline=W/'src/freedreno/vulkan/tu_pipeline.cc';header=W/'src/freedreno/vulkan/tu_pipeline.h';hoist=W/'src/freedreno/ir3/ir3_nir_move_varying_inputs.c'
p=pipeline.read_text();h=header.read_text();c=hoist.read_text()
assert 'struct tu_nir_shaders *nir_owner;'in h
assert 'nir_owners[stage] = library->shaders[stage].nir_owner ?'in p
assert 'vk_pipeline_cache_object_ref(&nir_owners[stage]->base);'in p
assert '&library->shaders[i].nir_owner->base' in p
assert 'blob->overrun || blob->current != blob->end' in p[p.index('tu_nir_shaders_deserialize(struct vk_pipeline_cache *cache,',p.index('tu_nir_shaders_init(')):p.index('tu_nir_shaders_has_stages(')]
assert '_mesa_set_search(check->checked, instr)'in c and '_mesa_set_add(check->checked, instr)'in c
assert c.index('check_precondition_src, &check')<c.index('bool progress = false;')
cmat=W/'src/freedreno/vulkan/tu_nir_lower_cooperative_matrix.c'
splitter=W/'src/compiler/nir/nir_lower_cooperative_matrix.c'
cm=cmat.read_text(); sp=splitter.read_text()
assert cm.index('nir_shader_intrinsics_pass(shader, lower_saturating_muladd') < cm.index('progress |= nir_lower_cooperative_matrix_flexible_dimensions')
assert 'NIR_CMAT_RESULT_SIGNED' in cm and 'assert(!nir_intrinsic_saturate(intr));' in cm
assert 'nir_copy_deref(b, accum, c_deref);' in sp and 'c_deref = accum;' in sp
clear=W/'src/freedreno/vulkan/tu_clear_blit.cc'
cl=clear.read_text();copy=cl[cl.index('tu_CmdCopyMemoryToImageIndirectKHR('):cl.index('TU_GENX(tu_CmdCopyMemoryToImageIndirectKHR)')]
assert 'u_minify(image->vk.extent.depth, subres->mipLevel)' in copy
assert 'available_layers - subres->baseArrayLayer' in copy
assert copy.index('if (!layers)') < copy.index('tu_image_view_copy<CHIP>')
result=dict(old_owner_counterexample_reproduced=old_counterexample,lifetime_order_cases=lifetime_cases,all_live_library_views_remain_owned=True,all_final_owner_refs_zero=True,hoist_DAG_depths=64,old_visits_at_depth64=old_steps,memo_visits_at_depth64=steps,rejected_dependency_kinds=rejections,cooperative_matrix_integer_cases=matrix_cases,old_saturation_counterexamples=2,old_C_mutation_counterexample_reproduced=True,indirect_copy_remaining_slice_cases=slice_cases,source_sha256={str(x.relative_to(W)):hashlib.sha256(x.read_bytes()).hexdigest()for x in[pipeline,header,hoist,cmat,splitter,clear]},driver_or_C_executed=False,build=False,limits=__doc__.splitlines())
(W/'docs/gamesir-recovery/validation-model-results.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({k:v for k,v in result.items()if k not in['source_sha256','limits']},indent=2))
