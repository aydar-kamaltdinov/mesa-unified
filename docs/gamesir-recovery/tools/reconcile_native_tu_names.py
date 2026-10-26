"""Record reviewed native/local names without treating names as semantic proof."""
from pathlib import Path
import os
import json,hashlib
R=Path(os.environ.get('TURNIP_RECOVERY_ROOT',str(Path(__file__).resolve().parents[1])));W=R/'mesa-gamesir-recovered';E=R/'native-complete/rutabaga-linkage-hour-20261025';V='src/freedreno/vulkan/'
M={}
def put(names,path,anchor,kind='local-functional-counterpart'):
 for n in names.split():M[n]=(path,anchor,kind)
put('build_copy_memory_to_image_indirect_fs_shader',V+'tu_nir_indirect_copy.c','tu_nir_copy_memory_to_image_indirect_fs')
put('indirect_copy_command_address',V+'tu_nir_indirect_copy.c','nir_pack_64_2x32(b, nir_trim_vector(b, c0, 2))','inlined-local-counterpart')
put('indirect_copy_load_u32',V+'tu_nir_indirect_copy.c','load_command_u32')
put('indirect_copy_div_round_up',V+'tu_nir_indirect_copy.c','div_round_up')
put('indirect_copy_load_global',V+'tu_nir_indirect_copy.c','load_pixel_word')
put('d24_bytes_to_uint',V+'tu_nir_resolve.c','d24_stencil_from_bytes','split-local-counterpart-depth-plus-stencil')
put('indirect_copy_format_info',V+'tu_indirect_copy_format.h','tu_get_indirect_copy_format')
for native,local in [('tu_mesh_draw_begin','tu_a8_build_mesh_draw_begin'),('tu_emit_mesh_view_state','tu_a8_build_mesh_view_state'),('tu_emit_mesh_dispatch_packet','tu_a8_build_mesh_dispatch_packet'),('tu_mesh_draw_end','tu_a8_build_mesh_draw_end')]:put(native,V+'tu_mesh_dispatch.cc',local,'packet-builder-counterpart-caller-separate')
put('tu_get_copy_memory_indirect_pipeline',V+'tu_copy_memory_indirect.cc','get_copy_memory_indirect_pipeline')
put('remap_type_in_place',V+'tu_nir_lower_cooperative_matrix.c','remap_type','different-local-return-style')
put('_nir_select_from_array_helper','src/compiler/nir/nir_builder.h','_nir_select_from_array_helper','common-nir-header-not-missing-driver-helper')
put('store_cmat_scratch',V+'tu_nir_lower_cooperative_matrix.c','nir_store_deref(b, cmat_scratch','inlined-local-counterpart')
put('load_cmat_scratch',V+'tu_nir_lower_cooperative_matrix.c','nir_load_deref(b, cmat_scratch','inlined-local-counterpart')
put('lower_task_prepare_intrinsic',V+'tu_nir_lower_mesh.cc','prepare_task_intrinsic')
put('tu_nir_lower_mesh',V+'tu_nir_lower_mesh.cc','tu_nir_lower_mesh_aqe_recovered','split-wrapper-counterpart')
put('lower_mesh_intrinsic',V+'tu_nir_lower_mesh.cc','lower_mesh_output','split-across-input-output-count-special-helpers')
put('aqe_global_workgroup_index',V+'tu_nir_lower_mesh.cc','aqe_global_index')
put('task_aqe_pointer mesh_aqe_pointer',V+'tu_nir_lower_mesh.cc','aqe_pointer','shared-parameterized-local-helper')
put('mesh_aqe_workgroup_offset',V+'tu_nir_lower_mesh.cc','mesh_primitive_offset','split-local-buffer-stride-expressions')
put('emit_mesh_aqe_metadata_init',V+'tu_nir_lower_mesh.cc','mesh_publish_counts','split-count-and-flags-publication-counterpart')
put('mesh_output_address',V+'tu_nir_lower_mesh.cc','buffer->stride * buffer->max_count','inlined-local-output-address-counterpart')
for n in ['_nir_build_load_const_ir3','_nir_build_load_global_offset','_nir_build_store_global_offset','_nir_build_store_shared','_nir_build_load_shared','_nir_build_global_atomic']:
 put(n,'src/compiler/nir/nir_intrinsics.py',n.removeprefix('_nir_build_').removeprefix('load_').removeprefix('store_'),'generated-nir-builder-not-missing-driver-function')
put('kgsl_native_timeline_supported',V+'tu_knl_kgsl.cc','kgsl_native_timeline_supported','new-native-source-transfer-this-hour')
put('kgsl_signal_timelines_cpu',V+'tu_knl_kgsl.cc','kgsl_timeline_signal_ioctl','existing-shared-ioctl-wrapper-plus-callers')
put('kgsl_wait_syncs_init',V+'tu_knl_kgsl.cc','kgsl_queue_submit_timeline','different-local-sync-types-wait-builder-already-adapted')
x=json.loads((E/'tu-new-name-frontier.json').read_text());rows=[]
for r in x['rows']:
 path,anchor,status=M[r['name']];p=W/path;s=p.read_text();assert anchor in s,(path,anchor)
 rows.append(dict(**r,local_path=path,local_anchor=anchor,anchor_line=s[:s.index(anchor)].count('\n')+1,local_sha256=hashlib.sha256(p.read_bytes()).hexdigest(),classification=status,semantic_equivalence_proved=False))
assert len(rows)==34
result=dict(scope='Reconciliation of34 name-search misses only, not whole-driver port closure',rows=rows,observations=['Mesh view/end/dispatch packet source already encodes original register headers, lengths, AQE resource partition and ordinal wrap; compiler/firmware integration remains unvalidated.','Original TU file ownership does not imply same local file/function decomposition.','KGSL probe create+destroy condition was a real missing predicate and is now separately transferred.','Original IR3 mesh count/output marker cases are consumed by earlier lowering; no silent backend no-op is added merely to match case names.','Cooperative matrix and indirect-copy counterparts are locations for full final comparison, not an automatic pass.'],build=False,full_validation_deferred=True)
(E/'tu-helper-name-reconciliation.json').write_text(json.dumps(result,indent=2)+'\n');(W/'docs/gamesir-recovery/tu-helper-name-reconciliation.json').write_text(json.dumps(result,indent=2)+'\n')
p=E/'ir3-intrinsic-dispatch-name-inventory.json';d=json.loads(p.read_text());d['known_boundary']='Three marker cases are already recorded in GAMESIR_REMAINING.md historical mesh/compiler section: consumed by lowering, not a new missing ISA backend. Do not add silent no-ops to mirror labels.';p.write_text(json.dumps(d,indent=2)+'\n')
print('Recorded34 original/helper locations; no semantic-closure claim')
