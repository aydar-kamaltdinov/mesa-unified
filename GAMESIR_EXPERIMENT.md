Current unbuilt rutabaga checkpoint: policy0x47533817. The native compute/fragment helper now includes TASK/MESH across subgroup size, phi precision, soft-SY scheduling and the SAMGQ exclusion. See GAMESIR_REMAINING.md and docs/gamesir-recovery/ for current status; earlier front matter below is historical.

Current unbuilt rutabaga source: policy0x47533816. Restored SubgroupUniformControlFlowKHR metadata and native compute-class linear-dispatch policy for maximal reconvergence / dual-wave uniform control flow. TASK/MESH gates remain held. Older policy15/14 statements below are historical.

Current unbuilt source on rutabaga: compiler/cache policy0x47533815. Restored native TASK/MESH WorkgroupID vector copies, MESH copy-propagation guard and fixed shared-input precolor0xcf; A8 single-subpass MSAA GMEM policy also restored. Feature gates remain held; no build/runtime validation. Earlier policy14 and historical statuses below predate these transfers.

Current sparse compiler/VPC/prefetch/AQE recovery: policy0x47533814, UNBUILT. Native coupled VPC/setup range+borrowed remap and generic/clip/cull/PrimitiveID map priority restored. Earlier policy13 below is an intermediate checkpoint. Held TASK/MESH gates and current bounds retained; full validation deferred. Evidence: native-complete/sparse-vpc-prefetch-prefix-hour-20261024/.

Current sparse compiler/VPC/prefetch/AQE recovery: policy0x47533813, UNBUILT. Restored coupled native primitive range scans and borrowed-FS remap across setup/final VPC; removed local physical/repacked bypasses. Previous policy12/pass9 below is historical. Current128-scalar guards and held feature gates retained. Evidence: native-complete/sparse-vpc-prefetch-prefix-hour-20261024/.

Current sparse-address/AQE source checkpoint: policy0x47533812, UNBUILT. Native shared-mirror volatility plus AQE DrawID/ViewIndex preamble publication, multi-copy STSC expansion and A8 compute-like optional preamble/descriptor-prefetch policy transferred. Named mesh-multiview flag is hashed/serialized; all four cache keys advanced together. Previous policy11/pass7 is a historical intermediate checkpoint. No build, shader execution, GPU/device action or feature activation. Evidence: native-complete/sparse-address-aqe-hour-20261024/.

Current sparse-address/AQE hour: compiler/cache policy0x47533811, UNBUILT. Recovered conditional volatile shared mirrors for CullPrimitive/PrimitiveShadingRate; Layer/Viewport remain ordinary. Previous policy10 checkpoint below is historical. No build, driver execution or device action. Evidence: native-complete/sparse-address-aqe-hour-20261024/shared-special-access-source-evidence.json (relative to recovery root).

> Latest source continuation (2026-10-24, sparse FS/AQE hour): UNBUILT, coordinated compiler/cache policy0x47533810. Restored the original flat-input pre-rasterflat/per-primitive last-vertex selection, both AQE indirect zero-draw exits, and the FS nonzero primitive/layer-viewport-base policy: retain ordinary CP but disable immediate-to-constant lowering and skip RT aliases/constant-to-preamble as one native bundle. The earlier four FS/VPC contracts and A8 FLAT_B transfer remain present. See ../native-complete/sparse-aqe-hour-20261024/. No driver/shader execution tests or device action; feature gates unchanged. Earlier source headers below are historical.

> Recovery continuation 2026-10-24: original experimental Git history remains missing. Current source adds A8 FLAT_B assembly, A8 VPC register0x925a, preserved filtered-FS compiler flags and the original mesh-FS input-record prescan. Coordinated cache policy0x4753380f. No driver build, compiler/GPU test or device action for this source. See ../native-complete/mesh-hour-20261024/ and GAMESIR_REMAINING.md; earlier snapshots below are historical.

# GameSir reconstruction experiment (work in progress)

> Current source handoff (2026-10-19): the latest preliminary source is UNBUILT. Follow-ups after installed39fddd2d restore the real-application-FS predicate for mesh AQE/VPC and the original A8 uniformAndStorageBuffer16BitAccess policy. Compiler/cache schema remains0x4753380d. This continuation used source/native-disassembly review only; no driver/component build, test suites or device upload. TASK/MESH compilation remains rejected. Installed39fddd2d is a provisional baseline candidate; historical sections below retain earlier checkpoint results.

Base: `0a1a3b5aa42` (stage66). Branch: `ak-experiment-gamesir-preliminary`.

The requested scope includes new GameSir capabilities, not only ordinary A830 paths. This checkpoint does **not** implement the entire requested feature set and must not be described as a complete GameSir port. No device performance or rendering correctness is established by host compilation.

## Applied source changes

- Audit fixes: scalar quad-vote reduction, halt-aware RA validation and image-query SPIR-V result width.
- Experimental GameSir behavior: conditional dynamic broadcast on A8xx; half RGETPOS widened to F32; GETINFO .x mask and register-index copy for an already-half result.
- Restore the stage63 indirect dispatch/CP_MEMCPY and 3D-blit draw-state updates previously reverted in stage66.
- Vulkan-typed KGSL context, with an untyped-context fallback.

Existing native KGSL timeline support was incorrectly described as missing in earlier notes. It already exists in stage66; copying a second implementation would be inappropriate. Detailed GameSir differences still need review.

## Mandatory work before the requested full experimental driver

- **Mesh/task:** Eight-stage storage/loops, compiler linkage, GPL snapshots/cache, EXT preparation and AQE/output lowering, FS input/VPC mapping, draw state/CCU, query routes and original limits are source-connected. These are no longer wholly missing implementations. Remaining source/activation boundaries include sparse primitive-coordinate adaptation and AQE context/shared-bank/prefix synchronization, including runtime ordering of the now-restored DrawID/ViewIndex preamble copies. `tu_compile_shaders` still rejects TASK/MESH; extension exposure is held. Integrated compiler/API/cache/GPU validation is deferred. Original NV_mesh_shader is unadvertised and is excluded from advertised-feature parity; no speculative NV backend is required. See M1–M6 in the current remaining-work registry.
- **Cooperative matrix:** Source lowering, property enumeration, base feature/stage metadata and physical-storage SPIR-V tests are ported. Descriptor robustness, FP edge cases and GPU validation remain open.
- **Indirect memory and image copy:** Both KHR commands, shader pipelines, format policy, feature and queue metadata are ported. GPU synchronization/rendering validation and Android exposure remain open.
- **Fragment barycentric and custom derivative intrinsics:** All five custom intrinsics have source/backend support. Barycentric coordinate lowering, primitive-specific backend and VPC generation state are ported; per-vertex input loads, repacking and VPC padding are ported; Static and non-LTO library primitive/provoking keys are now ported; dynamic topology/provoking behavior remains unresolved (a separate selector has not been proved necessary or recovered). The A6/A7 sample-center caller is connected behind default-off IR3_GAMESIR_SAMPLE_CENTER for polarity validation.
- **Depth/stencil resolve modes:** Six recovered builders plus two source corrections for D24 MAX/AVERAGE are registered; the 3D blitter selects them through recovered mode flags and an additional internal D24 flag. Legacy/dynamic pass modes now reach same-format sysmem recording with existing depth/stencil views. New modes now reach both sysmem and A8xx non-FDM GMEM shader recording. Older GPUs and FDM retain the sysmem fallback. Full view-reinterpretation parity, FDM GMEM handling and Vulkan/GPU validation remain open; SAMPLE_ZERO/CUSTOM advertisement is unchanged.
- **Resolve transfer function (maintenance10 subset):** The original skip-transfer flag is recovered and connected from legacy/dynamic attachment metadata to sysmem resolves. Explicit resolve-image mode/skip flags and DS aspects are now connected, including per-region plane views. Temporary linear views retain mip/layer bases. Skip-transfer GMEM is now connected for A8xx without FDM; older GPUs and FDM retain sysmem. Complete ENABLE-transfer semantics, remaining entrypoints/properties and hardware validation are pending; the extension is not advertised.
- **A8xx ordinary color resolves:** GameSir shader averaging for non-integer color is now connected without FDM, including ordinary sRGB. Integer/older-GPU/FDM paths retain prior behavior; property exposure and GPU/performance validation are pending.
- **Mesh CCU configuration:** The third state is activated by mesh draw setup, not established as a prerequisite for ordinary extended resolves. Its arithmetic is native-validated; full mesh activation, address eligibility and source integration remain open.
- **Indexed draw split and game-specific compatibility:** Recovered guarded A830 split/WFI and exact Cyberpunk2077 application policy are source-connected; final execution/performance validation remains.
- **A7 sample-center and z-clamp paths:** Sample-center caller is connected behind a default-off diagnostic switch; polarity remains unvalidated. Unrestricted-depth enabled byte and A7/A8 clamp/viewport rules are recovered in source, feature exposure remains gated.
- **CP_START_BIN packet differences:** Standalone tile-select already matches OFFSET. The inlined A8 prefix uses selected-pipe pseudo addresses plus opcode0x2d; source transfer is connected behind TU_GAMESIR_BIN_DATA=false. Packet execution, submit/preemption/concurrency and unknown pseudo IDs11–15 remain open.
- **A850 hardware draw parameters and derivative dispatch property:** Additional device-dependent behavior; unsupported hardware must not gain capabilities by unconditional feature bits.
- **AMDX/CUDA and other symbol-only entries:** Presence of stubs or common Mesa routines in ELF is not evidence of a working extension.

## Validation so far

The modified IR3 objects are linked against the actual stage66 host libraries. NIR validation and compilation pass in 15 sample-position, 24 GETINFO and 12 broadcast cases across A630/A730/A830. These tests cover compiler paths and ISA construction; they do not execute the resulting shaders on a GPU. A8xx dynamic broadcast can increase generated code size; this is not a proven optimization.

The complete source diff and output hashes are recorded outside the repository under `outputs/gamesir-preliminary`. Build recipe: API29 NDK compiler, SDK33 Mesa extension gate, release, KGSL, oryon-1. No unsupported extensions are enabled just to imitate an extension list.

The earlier historical audit fixes remain in `audit/commit-validity-fixes`, separate from this experiment. The final combined audit commit is still pending.

## Newly recovered capability constraints

- GameSir advertises mesh/task only with its configuration guard clear, gen >= 8 and `(gmem_size >> 21) > 8`. The GMEM field is independently traced to `tu_knl_kgsl_load` and the TU_GMEM override. The `instance+0x326` guard is `tu_disable_mesh_shader`: the option parser base at instance+0x2d8 and the recovered name at offset 0x4e independently establish the mapping.
- KHR cooperative matrix is gated by `threadsize_base == 64` and no exposed double thread size. The original isolated GameSir property-query function was executed on the host with mock physical-device records: 156 capacity/gating cases pass. Its 21 descriptors have M=64, N in {64,32,16}, float16/float32 and signed/unsigned 8-bit input combinations, with the recovered saturation cases. This validates the property enumeration, not matrix execution or the lowering implementation.
- The GameSir property probe executes only one bounded leaf function and its identified read-only literals; it does not initialize or load the full Android driver.

## Android extension exposure prerequisite

The current SDK33 build uses android-strict=true. The registry generator gates KHR fragment barycentric at SDK34, KHR cooperative matrix at SDK35 and KHR indirect copies at SDK37. EXT/vendor extensions follow a different policy. Full feature testing therefore also needs a deliberate exposure policy and compatibility checks; merely implementing an entrypoint will not expose every requested extension under this build configuration. No SDK/exposure flag was changed in this checkpoint.

## Indirect-copy reconstruction pass

`tu_copy_memory_indirect.cc` and its GLSL source now implement the memory-to-memory KHR command with one 64-thread workgroup per indirect record, 64-bit addresses/length and four-byte command alignment. The source uses the recovered GameSir algorithm; the extracted binary shader is evidence, not a shipped shader blob.

Validation:

- Both original GameSir SPIR-V and the source-built replacement pass SPIR-V validation. Compiling both through our NIR/IR3 pipeline yields identical ISA on A630, A730 and A830 (six compilations).
- A bounded, fail-closed interpreter of the actual optimized SPIR-V tests 72 cases per module, including zero size, workgroup boundaries, high device addresses and command records at four modulo eight. No out-of-range reads, duplicate writes or missing words are observed. This does not validate GPU cache behavior.
- The 3D memory-to-image path is now ported (see later command section). Its format-classification helper is ported to `tu_indirect_copy_format.h`. Results match original GameSir code for 5,760 destination/source/aspect combinations across all 480 pipe-format records, including transformed depth/stencil sources. This tests classification, not rendered pixel conversion or format support advertisement.
- The new header compiles as both C and C++. Android builds the memory command for all three GPU generations.

The existing acceleration-structure compute-state helper did not preserve `compute_load_state`. The shared helper in `tu_meta_compute.h` restores it and dirties compute descriptor state as well as push constants. Sixteen regression cases pass; the original helper fails the restoration assertion. The old helper is inherited upstream code (blame `671e3a65a667`); that attribution does not establish when the missing load-state restoration became an observable bug.

A standalone fix without the new-feature refactoring is saved as `outputs/stage66-validation/0004-meta-compute-state-restore.patch` for the final audit batch. It has been checked against stage66 but is not applied to that validation worktree.

The KHR extension remains hidden by the current SDK33 strict allowlist; commands and format support are now implemented. Mesh/task, cooperative matrix, barycentric, extended resolves/CCU and compatibility policy are still mandatory open work. The full requested preliminary package is not ready.

Reference for the four-byte source/destination/size constraints: [Vulkan VkCopyMemoryIndirectCommandKHR](https://docs.vulkan.org/refpages/latest/refpages/source/VkCopyMemoryIndirectCommandKHR.html).

## Indirect memory-to-image vertex shader

The new `tu_nir_indirect_copy.c` reconstructs the vertex shader inlined into GameSir's tu_init_clear_blit_shaders (original global shader slot 3). It reads GPU command offsets/extents, converts them to format-block coordinates and emits the two rectangle endpoints plus copy coordinates. It is registered as a global shader and is used by the new memory-to-image command.

The source passes NIR validation and 1,664 specialized constant-folding checks covering block sizes, rectangle origins, extents and both endpoints. Actual IR3 compilation passes for A630/A730/A830. These are not comparisons against GameSir compiler ISA or GPU rendering results. The FS slots recovered in GameSir are 6 (float output) and 7 (integer output), selected by r3d type 3 and flags 0x800/0x1000. The fragment shader and command recording are now ported; GPU validation and extension features/properties/exposure remain open.

## Indirect memory-to-image fragment shaders and command

The float and integer fragment builders reconstruct command-address loading, row/image-pitch defaults, 64-bit source addressing, layer stepping and GameSir's format conversions. The generic unpacker reads only the channel bytes required by its descriptor. Specialized NIR evaluation passes 336 format/pattern/pitch cases against Mesa unpacking, with bounded source reads; this includes three- and six-byte pixels and transformed Z24 depth/stencil sources. Both variants compile through IR3 for A630/A730/A830 (six compilations).

`tu_CmdCopyMemoryToImageIndirectKHR` now records per-copy/per-layer rectangles through the existing r3d path, uploads VS/FS constants, selects float/integer output, restores draw state and disables LRZ after depth writes. An extracted-body recording test passes 108 cases with mocked command-stream operations: high addresses, four-modulo-eight command records, strides, zero/multiple records, array/3D layers, remaining array layers and depth/stencil formats. This tests CPU recording decisions, not GPU packets or synchronization.

All 21 global shaders, including optional resolves, compile and fit in the enlarged 32 KiB shader area on A630/A730/A830. Occupancy is 21,888 / 21,248 / 22,016 bytes respectively; the previous 16 KiB area was insufficient. These use the actual builders and global initialization/compiler code with a mock device storage allocation.

GameSir's indirect-image destination whitelist is independently checked against the original 122-byte jump table at ELF 0xc0b55: 47 accepted formats. Only A8xx+ with TRANSFER_DST qualifies. The corresponding format feature is ported and gated on the extension being supported. Other metadata-classifiable formats do not gain support by inference. The extension is still unadvertised; feature/property/queue validation and Android exposure remain pending. No GPU image-copy test has run.

For this API, 3D starting slice and slice count are intentionally supplied in the CPU `pImageSubresources` array. The lack of GPU imageOffset.z handling in GameSir's command is therefore not evidence of a bug. Reference: [vkCmdCopyMemoryToImageIndirectKHR](https://docs.vulkan.org/refpages/latest/refpages/source/vkCmdCopyMemoryToImageIndirectKHR.html).

Cooperative matrix reconstruction evidence is now annotated with opcode names read from GameSir's own intrinsic/ALU metadata tables (`outputs/gamesir-preliminary/cooperative-lowering-annotated.c` and `cooperative-opcodes.json`). It uses 8×8×8 flexible-dimension lowering, a 64-invocation data layout, and thirteen recovered cmat operation cases. This was the recovery evidence used for the source lowering described below.

## Cooperative matrix source port

`tu_nir_lower_cooperative_matrix.c` now reconstructs the thirteen recovered KHR cmat operation cases, including load/store, construction/copy, extraction/insertion, scalar/unary/binary operations, conversion/bitcast and muladd. Mesa's flexible-dimension pass splits matrices into 8×8×8 tiles, with one value per invocation in a 64-lane subgroup. The GameSir choice of shared scratch when there is no pre-existing shared variable, and shuffle otherwise, is preserved. Subgroup acquire/release barriers protect the shared scratch. The 2,048-word scratch fits 16 subgroups of 64 invocations, consistent with the existing 64-lane/no-double/max_waves16 support gate.

The pass is wired at the start of `tu_lower_nir`, before access analysis and explicit shared/global memory lowering, matching the original call position. The property enumeration command returns the 21 recovered descriptors with the original threadsize64/no-double gate. It preserves caller sType/pNext and output bounds; 156 enumeration/gating/capacity cases match the original native descriptors.

Validation uses the actual lowered NIR, a fail-closed CPU interpreter for straight-line ALU/shuffle/shared/global accesses and the real IR3 compiler. Twelve 8×8 cases cover all thirteen operation handlers, type conversion, signed saturation and both exchange paths. At subgroup IDs0 and15, 1,536 result values agree with independent arithmetic. Forty-two full-matrix cases cover each of the original 21 property descriptors through both exchange paths: 100,352 matrix elements agree, with bounded input reads and exactly one write per output. Shapes include M64/N64,32,16 and K8/16/32, F16/F32, signed/unsigned8 inputs and signed32 accumulation, including saturating variants. Both sets compile through IR3 on A830. Host tests are not original GameSir ISA comparisons or GPU validation; finite integer-valued float inputs also do not establish complete floating-point edge-case behavior.

Remaining cooperative-matrix prerequisites: descriptor robustness and floating-point edge cases, complete GPU pipeline validation and Android exposure/compatibility. The physical-storage-buffer SPIR-V path and base feature/stage metadata are now checked as described below. The extension is still unadvertised. This checkpoint therefore does not complete the requested full preliminary package.

## Physical-storage SPIR-V and capability metadata

Twenty-one generated GLSL shaders, one per original GameSir property descriptor, pass glslang and spirv-val for Vulkan1.3. Their actual SPIR-V is imported through Mesa's VTN, the relevant common Vulkan preprocessing and Turnip's early NIR optimizations, then the recovered matrix lowering. Across both shared/shuffle paths, all42 imported cases compile through IR3 and 100,352 output elements match the independent matrix arithmetic interpreter. Reads are bounded and output writes are complete and nonduplicated. This covers physical-storage-buffer resources, not descriptor robustness, GPU execution or every floating-point input.

The source now sets KHR cooperative-matrix support only for threadsize_base64 with no exposed double size, compute-stage support, and base feature flags. The robust-matrix-buffer feature deliberately remains false pending validation; it must not be reported as fully recovered. KHR indirect-copy support is conservatively restricted to A8xx+ here, matching the recovered image format path; supportedQueues is GRAPHICS|COMPUTE|TRANSFER (GameSir value7). Earlier GameSir feature-query recovery alone did not prove its extension gate on every older GPU, so this restriction is not claimed as an exact reproduction of that gate. Both extensions remain filtered out of enumeration by the SDK33 ANDROID_STRICT allowlist. No SDK/strict build flag was changed.

## Packed three-component derivatives

Two of the five GameSir-specific NIR intrinsics are now reconstructed: ddx_3d_ir3 and ddy_3d_ir3. Each descriptor is independently resolved from the original ELF: src vec3, dst vec3, F32 only, no indices, flags13 (eliminable quad-group operation). The IR3 backend emits DSX.3D/DSY.3D with xyz masks; perspective interpolateAtOffset uses these instead of three scalar derivatives per axis. Divergence analysis, coarse-helper classification, constant folding, CSE and discard-movement handling recognize the new operations.

Compiler comparison:12 stage66 baseline plus15 reconstructed cases across A630/A730/A830. All six perspective cases change from six scalar derivative instructions to two packed instructions; all six nonperspective cases retain identical ISA. A830's constant-offset test shrinks from384 to256 bytes; other tested sizes are unchanged. Three direct vec3 tests compile as expected and verify coarse-helper classification. Four finite/nonfinite constant-folding cases pass, preserving NaN behavior for NaN/Inf inputs. All12 existing IR3 delay regression cases pass. These are compiler/CPU checks, not a measured FPS or temperature result and not GPU derivative execution.

A fresh native `work/gamesir-preliminary/build-host` was built for the new NIR intrinsic numbering. Current tests must use matching headers/libraries, not pre-change objects mixed with new enums. The current-ABI SPIR-V matrix rerun passes42 cases; both indirect image FS variants compile on three generations, and all21 global shaders still fit the32KiB area (maximum22,016 bytes). Use the `_current.py` probes for this ABI. Android KGSL SDK33 compilation also passes. TU_BUILD_ID_OVERRIDE is absent in the active build; normal build identity separates its caches.

The remaining three new intrinsic names are load_barycentric_coord_ir3, load_sample_pos_from_id_ir3 and select_barycentric_at_center_ir3. Full barycentric support, mesh/task, extended resolves/CCU and the other ledger items are still open. Checkpoint230817c8 was subsequently installed in GameHub and copied to Download on2026-10-17. Its artifacts and deployment manifest are preserved under outputs/gamesir-preliminary/checkpoints/230817c8ccc3; further source work does not change that installed driver.


## Hardware sample-position identity and coordinate-select backend

The original ELF independently confirms two further intrinsic descriptors: load_sample_pos_from_id_ir3 (one scalar source, vec2 F32 result) and select_barycentric_at_center_ir3 (sources scalar/vec2/vec2, vec2 F32 result), both eliminable but not reorderable, without indices. The first now identifies zero-center RGETPOS separately from the generic half-center NIR operation. Both load_sample_pos and load_sample_pos_from_id lower to the private operation plus0.5; replacing the generic instruction makes the pass idempotent. This is structural robustness, not evidence that the previous single-pass pipeline caused a rendering bug. The select backend preserves GameSir's first/condition/second SEL_B32 operands, including signed16-to32 condition widening.

Validation:30 lowering/idempotence cases and30 real IR3 compilations across A630/A730/A830, smooth/nonperspective modes, generic/private sample sources and direct select with boolean/16-bit conditions. Sixteen CPU constant-folding cases confirm the zero-to-half-center conversion. Original descriptor fields match source assertions. The isolated original uses_sample_pos_ir3 predicate passes451 bounded cases including generic/private leaves, ALU source positions and depth1..32; no driver initialization or GPU execution is involved.

The recovered A6/A7 conditional caller is deliberately saved separately in outputs/gamesir-preliminary/sample-center-activation-unvalidated.patch. Its zero-offset comparison and recovered SEL_B32 argument order raise a polarity question under ordinary select semantics; host compilation alone cannot settle hardware/rendering correctness. The candidate previously passed30 reconstruction/guard compilations, but it is NOT enabled in this checkpoint. Do not describe A7 sample-center handling as finished or as a proven fix. The private select backend is currently available to direct tests, without a production caller. Full barycentric remains pending, as does z-clamp policy.

After the intrinsic enum changes, the actual current-ABI matrix import regression passes42 cases/100,352 elements, and packed derivatives retain the previous27 compilation results. All12 delay regressions pass. Indirect FS and global shader capacity probes must rebuild tu_nir_indirect_copy.o against the current generated headers: an old standalone object triggered a host ABI assertion and was corrected in the probe dependency recipe, not in driver code.


## Fragment vertex weights and VPC basis generation

The fifth custom intrinsic, load_barycentric_coord_ir3, matches the original ELF descriptor: vec2 source, vec3 result, unrestricted destination-size mask, one interpolation-mode index and flags3. Its NIR gather-info and divergence handling are wired. The coordinate-at-offset/sample pass converts coordinate loads to a normal at-offset pair followed by the private coordinate operation, before the existing barycentric and derivative passes.

IR3 now handles coordinate pixel/centroid/sample loads and the private operation. For points it emits (1,0,0); for lines it emits (1-(i+j),i+j,0). Triangles read three VPC-generated vertex-basis varying components, matching GameSir's path rather than substituting an unverified (1-i-j,i,j) formula. The FS primitive key preserves recovered class values1/2/3 for points/lines/triangles, with0 unspecified. Key equality, FS invalidation and used-key masking include the field only when coordinate loads use it. Vulkan pipeline/GPL/dynamic-topology selection of this key is still pending; forced-key host tests do not establish that integration.

The temporary input locations0x100..0x102 are relocated after ordinary inputs. GameSir's isolated original fixup passes1,125 scalar layout cases. Our extracted source fixup passes1,956 scalar/repeated layouts;654 synthetic repeated layouts require counting more than the first input location. That is an intentional repeat-aware bound consistent with existing legalize max_bary logic, not proof of a triggered GameSir bug. Ordinary real shader probes validate bases0/4/20 and124. Legalize includes the three new locations in total_in, so VPC NUMNONPOSVAR includes them as well. The source checks the128-bit transfer-mask bound; the full extension's input-limit policy must be settled before advertisement. Three overflow cases are rejected by the debug compiler's internal assertion; this is not a graceful release-runtime fallback.

The VPC generation bit17 is recovered consistently from GameSir's A6/A7/A8 packet emission and described in the register XML. The fragment state enables that bit only when the compiler uses the generated basis and includes all three locations in the transfer mask. Two hundred fifty actual source mask layouts and750 words from the actual generated register builder agree with the recovered packet formula, including no-extra-coordinates cases and total_in including the appended basis. This is CPU packet construction, not hardware execution.

Validation:324 NIR/IR3 cases across three GPUs, two interpolation modes, three forced primitive classes, six coordinate operations and three ordinary input layouts; three valid124-component boundaries; three debug overflow assertions; variant cache reuse and key classification checks. Ten actual GLSL/SPIR-V modules pass glslang/spirv-val and compile through Mesa import/preprocessing and IR3 in90 cases across the same GPUs/classes. No GPU vertex-weight rendering test has run. Android SDK33 KGSL compilation succeeds with the VPC state addition.

At that checkpoint the per-vertex-array input backend was NOT ported; the next section records its subsequent source port. Recovery has identified original load_input_vertex/load_per_vertex_input handlers, input-record flag bit6, per-vertex-first repacking and padding modes. The initially ambiguous LDLV store of0x4000 at instruction+0x88 is now resolved to barrier_class bit14, not instruction flags. Release-layout static assertions confirm flags+0xc, barrier_class+0x88 and list node+0x90. The isolated original104-byte LDLV encoder passes7,936 native words across scalar/three-result loads, SY/JP, input locations and destination registers; toggling this barrier marker does not change ISA. Its scheduling role/name still needs recovery; do not substitute A1EN or invent a new hardware mode. See outputs/gamesir-preliminary/barycentric-per-vertex-pending.json. Mesh/task, resolve/CCU, the other mandatory features, source audit and GPU validation remain open; neither full barycentric support nor a complete requested package is claimed.

After the NIR enum addition, the current-ABI cooperative import regression still passes42 cases/100,352 elements; the27 packed-derivative compilations,30 sample-position compilations,16 numerical center conversions,12 delay regressions, six indirect FS compilations and all63 global shader compilations pass. Maximum global shader occupancy remains22,016/32,768 bytes. The held A6/A7 sample-center activation patch is rebased and apply-checked but remains unapplied. The phone retains230817c8 while this newer source checkpoint is validated locally.


## Per-vertex fragment inputs (2026-10-18)

The IR3 backend now handles load_input_vertex/load_per_vertex_input. Vertex0 uses scalar LDLV; vertices1/2 share a three-result LDLV and extracted lanes1/2. Explicit indices clamp to the highest valid vertex under forced primitive classes1/2/3. The recovered input flag drives per-vertex-first packing, four-location input reservations and VPC ZERO/ONE padding, including ordinary inputs when a per-vertex input exists. The isolated original repacker and the actual source repacker each pass2,048 layouts. The extracted source padding passes4,096 mixed/disabled/hole/component layouts.

LDLV barrier_class bit14 is represented as IR3_BARRIER_VARYING_R (our descriptive name, not recovered original spelling). Actual scheduler dependency checks cover400 pairs, including full barriers and the existing kill masks. A744-fixture constructor probe verifies scalar/triple loads, masks, immediate offsets and cache reuse. The source cache is block-local: sibling-block SSA values cannot be reused without dominance. This is a conservative source safeguard, not evidence of a demonstrated GameSir crash.

Thirty-five real GLSL/SPIR-V modules pass glslang and spirv-val;315 import/preprocess/IR3 cases cover A630/A730/A830, each forced primitive class, float vectors, flat signed/unsigned integers, constant/dynamic indices and mixed ordinary/per-vertex/barycentric inputs. Their LDLV words agree with the previously recovered original encoder formula. These are compiler/CPU tests, not GPU input-value/rendering validation. Broader16/64-bit and extension-limit policy remain open.

The tests exposed an existing common NIR IO error in this base: flat per-vertex integer input lowered to load_per_vertex_input while carrying interp_explicit_strict, which the NIR validator correctly rejects. The source now routes flat per-vertex variables through the explicit-vertex path. The old function, rebuilt against current headers, reproduces the validator failure; eight isolated normal/per-vertex IO contracts and all315 imported cases pass after the fix. A standalone final-audit patch is saved as outputs/stage66-validation/0005-flat-pervertex-nir-io.patch and apply-checks against stage66. Exact blame attribution was unavailable due to a missing promisor blob; do not infer the original author's identity.

Current-ABI regressions pass:324 coordinate compiler cases plus90 imported SPIR-V cases,42 matrix cases/100,352 numerical elements, six indirect-image FS compilations, all63 global shader compilations (maximum22,016/32,768 bytes), and12 delay tests.

Recovery also identifies the second GameSir FS key byte (key+0x2a) as last-provoking-vertex mode: vk_graphics_pipeline_state_fill copies VkPipelineRasterizationProvokingVertexStateCreateInfoEXT (sType1000254001/0x3b9eaa31) into rasterization-state+0x1c; the pipeline builder compares it with1, writes FS key bit25, and tu_compile_shaders copies that bit to IR3 key+0x2a. GameSir's A8xx backend uses this together with per-vertex input presence to read ordinary flat inputs with LDLV at class-1. This special ordinary-flat path is NOT yet ported. The current local rasterization-state layout differs, so copying its numeric struct offset would be wrong. Evidence is saved in provoking-vertex-recovery.json.

Vulkan pipeline/GPL/dynamic-topology primitive and provoking-vertex integration, input-limit policy and hardware validation remain mandatory. KHR barycentric remains hidden under SDK33 strict. Mesh/task and the other mandatory capability work remain open. The device still has230817c8; this local checkpoint is not installed.


## Last-provoking-vertex flat reads (2026-10-18)

The recovered A8xx backend guard is now ported: fs_provoking_vtx_last plus a live per-vertex input makes ordinary flat fragment inputs use LDLV at primitive_class-1. It is not enabled for A6xx/A7xx, smooth inputs, coordinate-only shaders or shaders whose per-vertex input was eliminated. The final NIR is scanned once before emission, so an ordinary flat load preceding an explicit per-vertex load receives the same treatment as the reverse order. Explicit per-vertex indices retain their own clamp and are not replaced by the provoking vertex.

The IR3 key participates in FS invalidation and variant equality; its used-key mask keeps it only for A8xx+ per-vertex shaders. The selected flat input is marked per-vertex for the recovered repacking and VPC-padding paths. This is backend support only: the Vulkan frontend does not yet set this flag or fs_primitive. GPL/non-LTO linking, dynamic states and input limits must be handled before advertising complete support. In particular, a separately compiled fragment library may not know input-assembly/rasterization state; blindly copying that missing state into a fixed compile key is insufficient.

Six real GLSL/SPIR-V modules pass glslang/spirv-val. Across A630/A730/A830, three forced primitive classes and both provoking modes,108 import/preprocess/compiler/cache cases pass. Cases include both ordinary/per-vertex declaration orders, smooth inputs, a dead per-vertex input and coordinate-only shaders. Tests inspect actual input flags, LDLV encoded locations/sizes, disassembly, key masking, FS/VS invalidation and cache reuse. The paired compiled ISA is identical when the guard is inapplicable; applicable mixed-flat A8xx cases change from flat.b to LDLV at the correct vertex. No GPU vertex-value/rendering test has run. Regression:315 prior per-vertex SPIR-V cases,324 coordinate cases plus their boundary checks, and12 delay tests pass.

The frontend integration is deliberately still incomplete and no extension is newly exposed. A backend port and successful host tests do not establish FPS, temperature, stutter or rendering improvement. Evidence: outputs/gamesir-preliminary/provoking-spirv-probe.log and provoking-vertex-recovery.json; reproducible fixtures: work/gamesir-audit/generate_provoking_spirv.py and check_provoking_spirv.py.


## Static Vulkan fragment key and producer topology (2026-10-18)

The Vulkan shader key now carries the primitive class and last-provoking-vertex flag to IR3. Static input-assembly/rasterization state is applied after importing LTO library keys and before hashing shaders, so stale imported keys cannot overwrite the merged state. Missing state blocks and dynamic topology/provoking state remain unspecified instead of reading their ignored values. The fragment key is part of the existing hashed TU key; no build-ID/cache override is introduced.

After tessellation execution modes are merged, a present geometry shader determines the delivered primitive class; otherwise TES point mode, isolines, triangles/quads determine it; only without either producer does the input-assembly class apply. This producer override is a Vulkan-semantic adaptation, not a claim that the same source code was recovered from GameSir. The original builder's input-assembly lookup agrees for the ten non-patch topologies. PATCH_LIST is intentionally left unspecified until TES/GS metadata is available rather than copying GameSir's generic triangle fallback. Mesh producer support is not yet implemented.

Actual source helper/key/state and the extracted production TU-to-IR3 bridge pass1,012 host cases:704 topology/missing/dynamic/stale-key combinations,280 producer-priority/key-transfer combinations, and28 checks from seven actual geometry/tessellation GLSL/SPIR-V modules. All seven pass glslang/spirv-val and Mesa import/NIR validation. A further108 fragment SPIR-V compiler/cache cases take their keys through the new static Vulkan-state helper and production bridge, checking resulting LDLV locations/sizes, flat-input flags and paired ISA. This does not instantiate a complete Vulkan pipeline, exercise application caches, link actual GPL objects or execute shaders on hardware. Android SDK33 KGSL compilation succeeds.

This checkpoint implements the static monolithic/LTO path only. Non-LTO library reuse and dynamic topology class changes are still unsupported for the new capability. Zero unspecified still uses the previously documented backend placeholder/fallback; it does NOT establish correct dynamic behavior. Neither KHR barycentric nor other new extensions are newly advertised.

The original GameSir pipeline builder explicitly scans linked fragment variants for has_per_vertex(+0x572)/has_barycentric_coord(+0x570). It uses a separate flag to enter the library-NIR cloning/key import path even without the normal LTO request, then reapplies rasterization/input-assembly state after importing keys. The relevant decompiled regions are lines400..412,2402..2567 and2568..2585 of tu_pipeline_builder_compile_shaders. Automatic retained-NIR/cache/lifetime handling for this path remains to port and validate. Merely importing its boolean condition would not supply missing retained NIR.

References for the producer override and dynamic class distinction: https://docs.vulkan.org/spec/latest/chapters/vertexpostproc.html and https://docs.vulkan.org/spec/latest/chapters/drawing.html. The full requested package, GPU validation, mesh/task, extended resolves/CCU and final audit remain incomplete. Device still230817c8.


## Automatic primitive-dependent GPL relink (2026-10-18)

Fragment shaders with live per-vertex reads or vertex-coordinate operations now retain a pre-IR3 NIR snapshot when built into a library, even without RETAIN_LTO. Ordinary shaders and dead per-vertex declarations do not add such snapshots. The same predicate is recorded in the serialized FS metadata, independently of triangle VPC basis allocation: point/line coordinate shaders also require relinking. A full user-requested LTO snapshot still retains all supplied stages.

A linked library with this fragment predicate imports its immutable fragment NIR and key even without LINK_TIME_OPTIMIZATION. Actual lowering/compilation uses a clone on a cache miss. Missing required retained NIR returns INITIALIZATION_FAILED; it is not silently reused under the old primitive key. Compiled library GS/TES metadata supplies the output class when those producer shaders are not part of the new NIR compilation. Imported keys are overwritten with merged static state before hashing. This is a source adaptation of the recovered GameSir relink path, not an assertion of identical binary structure or full pipeline validation.

Class-dependent complete shader libraries keep the snapshot as well and do not bypass linking by copying their already-built program. Ordinary complete shader libraries retain their existing program-reuse path. A cached class-dependent fragment shader without the required snapshot is treated as a partial cache hit and cannot complete a retained-library build without recovering NIR. Fragment-only snapshots use the NIR-cache tag P, full LTO snapshots N; partial NIR must not satisfy a full-stage LTO request or deduplicate a complete snapshot under the same key. Fresh snapshots have a separate owned array, transferred and cleared on cache adoption; errors free only still-owned snapshots.

The new path exposed an existing unchecked allocation in tu_pipeline_builder_compile_shaders: tu_nir_shaders_init could return NULL and the following loop dereferenced it. The original stage66 block reproduces a host fault under injected failure. The standalone correction returns OUT_OF_HOST_MEMORY and releases both test snapshots. It is saved as outputs/stage66-validation/0006-nir-cache-allocation-failure.patch and apply-checks against stage66; no blame attribution is inferred. The experimental implementation uses the same error result plus its separate owned-snapshot cleanup.

Validation:216 actual-source import/cache/ownership/compiled-producer cases with mock caches/resources, including missing NIR, allocation failure and deduplication. There are also216 selective snapshot policies over real imported/preprocessed fragment NIR:288 real NIR serialization/deserialization/validation roundtrips and288 independent snapshot destructors pass. Dead declarations do not trigger retention. Those shaders still pass108 state-to-IR3/ISA/cache compilations across A630/A730/A830, three classes and both provoking modes. The prior1,012 key/producer bridge checks pass. Android KGSL SDK33 compilation succeeds. These are host/source-component checks: no complete Vulkan GPL objects, application-cache device roundtrip, GPU execution, relowering through the entire TU pipeline, or performance result is established.

Dynamic topology/provoking selection, input limits and hardware validation remain pending. The new capability remains unadvertised. Mesh/task, extended resolves/CCU and other mandatory scope items remain open; the full package and final combined audit are not complete. Phone remains230817c8.


## Combined GPL-state follow-up and A8xx PC/VPC ordering (2026-10-18)

Further review found an omission in our preceding non-LTO GPL port: although a class-dependent fragment shader was recompiled and complete-library program copying was disabled, set_combined_state could still treat the old library as supplying the complete shader group. That suppressed both program/VPC emission and the related command-space/load-state reservation. The shared predicate now ignores inherited FS-containing combined groups when that library's FS needs primitive relinking. This also covers the FS/output ordering group. Groups unrelated to FS and ordinary libraries keep their previous decisions. This is a correction to our experimental port, not an inherited Mesa or demonstrated GameSir bug.

The actual old/current predicate is exercised over73,728 full/partial-library/group/two-library/fragment-kind combinations. The old helper suppresses5,095 decisions that need rebuilding under the new relink path; other decisions agree. Production allocation and program/ordering emission call the same predicate. These are source-component decisions, not complete Vulkan pipeline creation or proof of actual BO sizing on hardware.

GameSir's A8xx draw_common contains additional PC/VPC vertex ordering for variants with has_per_vertex_input or triangle basis coordinates. The source now emits the recovered pair: first-provoking vertex uses D3D_VERTEX_ORDERING(bit2) in both controls; last-provoking uses bit1 in PC but bit2 with bit1 clear in VPC. Explicit last-vertex flat attributes are already read via LDLV. Restart stays in bit0; unrelated bits are preserved. Ordinary shaders and A6xx/A7xx retain identical register words. A cached last-ordering boolean triggers re-emission only when the fragment ordering requirement changes, or existing restart/provoking/restore dirty conditions require it. It is reset with the existing complete command-state memset.

A bounded original GameSir machine-code range ELF0x97d414..0x97d55c (328bytes), from tu6_draw_common<A8XX>, was run on the host with mock records/command storage, CS mode1/error0. Its arithmetic, guards and packet stores are unmodified; a return trampoline is appended, and external calls/error targets are inaccessible guard pages. All96 presence/flags/provoking/restart/indexed combinations match our source PC/VPC values and original packet addresses0x9b01/0x930f. No Android-driver initialization or GPU execution occurs. Another2,048 actual-source word cases preserve other bits;32 actual dirty-guard transitions include leaving the special mode. Evidence includes the original range/branch manifest, byte array and reproducible probe.

Dynamic investigation: GameSir's property dispatcher maps EXT_extended_dynamic_state3 sType1000455001 to physical-device+0xf18; its initialization writes1 there. This confirms the advertised unrestricted-topology property, not a complete shader-variant switching implementation. The newly recovered draw_common code handles vertex ordering, not primitive-class shader recompilation. No complete dynamic class/mode selection has yet been identified or ported; do not infer either its absence everywhere or correct dynamic rendering from the property bit. A future selector also needs complete program/VPC, constlen and FS-dependent state handling.

Regressions:216 GPL import/cache/ownership/producer cases,216 selective snapshot policies/288 real NIR roundtrips and destructors, and108 fragment state/IR3/ISA/cache cases pass after this follow-up. Android KGSL SDK33 compilation passes. Full TU relowering/GPL pipeline/cache/GPU validation remains pending, along with dynamic selection, limits, mesh/task and other mandatory capabilities. No extension exposure change. The installed phone checkpoint remains2e9c8f5f; this follow-up is local only.


## Extended resolve shader reconstruction (2026-10-18)

Recovered the separate GameSir sysmem resolve builder at ELF0x8f6404 and its five actual initializations in global slots9..13: float MIN/MAX/AVERAGE and unsigned MIN/MAX. The new `tu_nir_resolve.c` preserves sample0 initialization, dynamic texture-sample query, loop from1, unsigned loop termination, per-component reductions, and float division by the signed-to-float sample count. The original ALU names are read from its own metadata, not inferred from our enum numbers. Integer AVERAGE and other modes are rejected by the source API because the recovered initializer never requests them. This is not a signed-color resolve implementation.

The builder is compiled into the source project but deliberately not registered or called by command recording yet. Existing GMEM color-average shaders, command behavior and advertised depth/stencil modes are unchanged. Five generic variants are not the complete resolve path: GameSir also routes MIN for VkFormat125(X8_D24_UNORM_PACK32) and129(D24_UNORM_S8_UINT) through flag0x400 to a separate global shader14. That shader converts normalized bytes to a24-bit integer before comparison and handles stencil separately. Applying generic per-channel float MIN to that byte-encoded representation would be incorrect. Source format/view remapping, this D24 path, mode selection and the third CCU state remain to port together before enabling extended resolves.

Validation:15 real IR3 compilations across A630/A730/A830 pass NIR validation and produce shader binaries. Another480 source-built NIR specialization cases cover all five variants, sample counts1/2/4/8, unsigned high-bit values, finite signed float values, both zero signs, NaNs and infinities. The test replaces the texture query/data, unrolls the actual NIR loop, removes dead break iterations, verifies each live sample index is in bounds and exactly one fetch per sample remains, and compares the constant output against independent reductions. Zero-sign bit preservation and every NaN payload/rounding case are not established. Android KGSL SDK33 compiles the new source. These are not original GameSir ISA comparisons or GPU resolve tests.

Dynamic primitive investigation remains unresolved: GameSir class0 follows the generic VPC basis/LDLV path rather than the point/line constant specializations. Therefore the need for a runtime CPU variant selector is a hypothesis, not an established missing implementation. Conversely, generic class0 correctness for unrestricted dynamic point/line/triangle rendering is not proved. Full TU/GPL relowering, dynamic provoking behavior and GPU tests remain required; no new extension was exposed.

The phone remains on2e9c8f5f. The full requested package, mesh/task and final combined audit remain incomplete.


## D24 byte-encoded MIN resolve shader (2026-10-18)

The separate GameSir global shader14 is now reconstructed as `tu_nir_resolve_d24_min_fs`. It converts normalized RGBA to unsigned bytes with fround_even(value*255), reconstructs depth as R|(G<<8)|(B<<16), reduces the24-bit depth and8-bit stencil independently with unsigned MIN, then repacks the winning depth bytes and independent stencil as normalized floats. The reciprocal literal at ELF0xb0568 is0.003921568859368563 (the F32 reciprocal of255 promoted to F64); the source preserves that F32 constant. Numeric ALU names, including fround_even/f2u32/ior/ishl/ushr/u2f32, are confirmed from the original binary metadata. A shared NIR loop builder serves this and the five generic modes; the public D24 entrypoint permits only this specialized MIN behavior.

Validation: the new shader compiles through actual IR3 for A630/A730/A830 (1,152bytes each).4,032 actual source-built NIR specialization cases validate16,128 output bytes across1/2/4/8 samples. Inputs cover all256 byte values at single-sample roundtrip, cross-byte depth boundaries, equal-depth/different-stencil and equal-stencil/different-depth cases, and deterministic random values.2,820 fixtures differ from an incorrect per-channel MIN. The test unrolls the real loop, removes dead break iterations, checks bounded live sample indices/exact fetch counts, and compares output against independent integer depth/stencil reductions followed by byte quantization. The generic five-mode regression still passes15 real IR3 compilations and480 NIR cases after the refactor. Android KGSL SDK33 compiles the source. No original GameSir ISA equality or GPU texture/RT quantization test is claimed.

The shader remains unregistered and disconnected from recording until matching source/destination view mappings, resolve-mode selection and CCU state are recovered. Existing GMEM color-average behavior and feature advertisement are unchanged; this is not a completed extended resolve implementation. The original A8 CCU configuration-selection range ELF0x9352a4..0x93532c (136bytes) was subsequently executed with mock records in6,144 native host cases. All12 words match the recovered model for numeric states0/1/2, num_ccu1..8 and alignment-boundary/random field patterns. State1 constructs a temporary fd6_gmem_config: color offset0x1068000, depth offset num_ccu*gmem.attr_size, attr offset0, position size aligned16KiB and position offset0x1068000+num_ccu*sysmem.color_cache_size; BV fields come from gmem and cache fractions/sizes from sysmem. State0/2 choose the two stored records. Only internal branches are permitted in the copied range; guard pages surround executable storage, a RET is appended, and no driver initialization or GPU execution occurs. Disassembly also corrects Ghidra's phantom extra parameter: the real ABI is x0=cmd,x1=cs,w2=state. Call-site state selection and hardware/address eligibility are still unresolved. No CCU source change or guessed address policy was applied. The phone retains2e9c8f5f.


## Resolve shader registration/selection and CCU scope correction (2026-10-18)

All six reconstructed resolve builders are now registered in device global-shader storage and selectable from r3d_common. The MIN/MAX/AVERAGE/UINT/D24 mode flags occupy the recovered GameSir bits6..10. Earlier experimental indirect-copy flags move from6/7 to the original GameSir bits11/12; every producer and consumer uses the named flags, and the extracted indirect command regression still passes108 cases. Existing global-shader indices are preserved by appending the six new slots. The resolver selection precedes the old GMEM average prototype only when explicit resolve flags are present; normal copies, single-sample sources, Z-scale, clears and indirect types retain their earlier choices. Old GMEM average shaders keep their debug gate. Depth/stencil feature advertisement is unchanged.

An isolated GameSir selector range ELF0x91d844..0x91d888(68bytes) is executed natively with register-only fixtures:8,192 mode/unrelated-bit cases match the source selector, including conflicting-flag priority and UINT-only fallback. Internal branches are unchanged, RET is appended, executable memory has guard pages, and no Android driver initialization or GPU execution occurs. The native range starts after common single-sample/Z-scale/indirect guards. Those production guards are checked separately using the actual extracted r3d_common source:20,480 old/current ordinary selection cases agree, and634,880 flag/priority/source-count/type cases pass. The old complete source fixture is preserved so this regression does not depend on a dangling amended Git object.

The actual global initialization and shader compilers, with a mock BO allocation, compile all27 shaders on A630/A730/A830, including optional old prototype variants. Occupancy is25,088/24,448/25,216bytes respectively, within32,768bytes. The new registration adds3,200bytes on each generation compared with the previous21-shader configuration. This validates storage bounds and compiler construction, not device startup time, GPU rendering or performance. Android KGSL SDK33 builds the changed device/blitter code.

**Correction to the preceding resolve/CCU dependency assumption:** the identified writer of cmd+0x4df2 is tu_mesh_draw_begin<A8XX> (ELF0x97c438): its store at0x97c460 writes1 before mesh setup. The render/barrier paths later pass this byte to CCU state selection; tile rendering explicitly selects2 and ordinary outside-pass blits select0. The mesh dispatch packet also uses0x1068000 for its GMEM regions. This establishes a mesh-related activation of the third state; it does not prove the state is needed by ordinary depth/stencil resolves. The prior notes grouping the third CCU state as an obligatory resolve prerequisite were too strong. Mesh reset/lifetime, hardware/address eligibility and the complete mesh path remain open; no CCU source changes were applied.

The new modes are connected to the internal 3D selector, but render-pass commands do not yet produce the new flags: depth/stencil modes must still be carried through pass state, matching format/aspect/view remapping and sysmem/GMEM recording. Thus this checkpoint does not implement end-to-end extended resolves or expose new modes. The phone still has2e9c8f5f; no deployment occurs in this continuation.


## D24 MAX/AVERAGE source corrections before command integration (2026-10-18)

The next format/view review exposed a potential correctness problem in the original route: resolve_sysmem<A8XX> patches D24-class source descriptors to normalized RGBA bytes, while r3d_common selects generic float MAX/AVERAGE for those modes. The original explicit D24 special shader handles only MIN. The property dispatcher maps DEPTH_STENCIL_RESOLVE_PROPERTIES to physical+0xc30/c38; initialization chooses literal47/37(depth/stencil) for gen>=8 versus33/33 for older chips, with independentResolve/None false. This is static binary/decompilation/literal evidence, not a native property-query test. In particular, the presence of an unsigned MAX builder does not imply advertised stencil MAX.

Actual reconstructed NIR demonstrates why encoded bytes cannot be reduced independently: MAX(0x00ffff,0x010000) produces0x01ffff in the generic-byte model. AVERAGE gives0x008080 after byte quantization, versus exact average65535.5 rounded to even0x010000. Across five directed pairs each, three MAX and four AVERAGE controls disagree with independent integer-depth arithmetic. This does not execute the original Android driver or establish an observed GameSir rendering defect on hardware; texture/render-target behavior still needs GPU confirmation. It does establish a hazard that should not be copied into the new command path without verification.

Source correction: `tu_nir_resolve_d24_fs` now accepts MIN/MAX/AVERAGE, with public invalid-mode rejection. MIN remains the recovered unsigned depth/stencil reduction. MAX reduces the reconstructed unsigned24-bit depth instead of three bytes (its stencil result must not be treated as advertised stencil MAX support). AVERAGE accumulates depth in unsigned32 bits and rounds the rational sum/count to nearest, ties even using quotient/remainder; it avoids losing low depth bits in an F32 sum. At64 samples the maximum sum is64*(2^24-1)<2^30, so it fits32 bits. AVERAGE preserves sample0 stencil and is not stencil averaging. The future command path must honor per-aspect mode and write-mask restrictions. The chosen exact integer rounding policy is source behavior, not a claim of bit-identical original GameSir averaging or GPU validation.

The two new source-correction shaders are appended and registered, preserving previous global indices. New internal flag R3D_RESOLVE_D24(bit13) selects depth-aware modes; original D24 MIN(bit10) retains its recovered priority. No existing render-pass caller produces the new flags yet, and capability advertisement remains unchanged.

Validation:9 D24 IR3 compilations(MIN/MAX/AVERAGE onA630/A730/A830) and12,096 actual NIR specialization cases validate48,384 output bytes with exact independent integer reference and bounded/exact sample fetches. Ties include odd/even quotient classes below and above0x800000. The five generic-mode regression remains15 IR3 compilations/480 NIR cases. The directed counterexample probe checks20 actual NIR evaluations and all10 corrected depths match the integer reference. The preserved native original selector still matches8,192 cases;20,480 ordinary old/current choices agree,1,269,760 actual-source guard cases and32 corrected-D24 selector cases pass. All29 global shaders fit32KiB, including optional old GMEM prototypes:27,904/27,264/28,160bytes onA630/A730/A830. Android KGSL SDK33 builds the changes.

Render-pass mode propagation and the matching format/aspect/view sysmem/GMEM recording are still pending: this correctness detour does not complete that planned connection. Shader builders/selection are source-validated, not GPU-executed. Mesh CCU remains separate. Phone retains2e9c8f5f; no deployment.


## Pass-mode propagation and sysmem resolve recording (2026-10-18)

The new modes are now connected from legacy VkSubpassDescriptionDepthStencilResolve and dynamic VkRenderingAttachmentInfo through tu_subpass and the end-of-subpass sysmem resolve producer. Fields for absent format aspects are normalized to NONE. Implicit MSRTSS defaults remain SAMPLE_ZERO; custom resolves retain their existing route. The end-of-subpass producer forwards the modes only for the final depth/stencil resolve attachment; color resolves and unresolves retain the previous default arguments.

For explicit MIN/MAX/AVERAGE, tu_resolve_sysmem selects the existing depth/stencil views and emits shader blits with the matching mode flags. D32S8 uses separate D32 and S8 planes; NONE skips that plane. Packed D24 can combine identical modes into one write or use independent aspect masks when modes differ. D24 MAX/AVERAGE choose the corrected integer-depth shaders. These internal control paths do not expand advertised stencil modes/independent resolve support; some structural test tuples intentionally exceed current Vulkan feature/format/sample/layer constraints. The current command producer uses the existing view records for same-format resolves. GameSir's additional cross-format/linear-view reinterpretation logic is not claimed to be fully ported.

New-mode depth writes disable destination LRZ when present. Since the FS writes through the color CCU, a conservative boundary cleans color CCU, invalidates color/depth CCUs and UCHE, then waits for idle on the same command stream. This avoids relying solely on the previous depth-attachment cache domain after a shader resolve. It is source-level synchronization policy, not proven GPU ordering or a performance optimization. Existing nonshader-mode paths do not get this boundary.

Until GMEM resolve-mode recording is recovered, any subpass requiring extended depth/stencil modes selects sysmem rendering before ordinary render-mode heuristics. This is a conservative source adaptation, not GameSir's complete GMEM implementation or demonstrated performance parity. Interaction with GPU-specific render-mode workarounds still needs device validation. Feature properties remain SAMPLE_ZERO/CUSTOM and independentResolve/None false; no new mode is advertised merely because CPU recording compiles.

Validation: actual production private/public resolve bodies and the original layer iterator are exercised with mocked blit operations and image-view records on all3 generation templates.32,256 old/current ordinary command traces agree;145,152 extended-mode control cases check flags, aspects, D32S8 plane pointer selection with both primary-plane arrangements, UBWC forwarding, multiview including bit31, per-layer rectangles, NONE skips, LRZ handling and exact cache-boundary call order. These are CPU recording/control checks, not real descriptor-byte construction, address-bound checks or GPU packets. The complete pre-change source fixture is preserved independently of amended Git objects.

Actual-source pass-mode components pass432 legacy assignment/aspect-normalization cases,1,944 dynamic pointer/handle/mode cases,512 whole-pass fallback cases and432 mode-forwarding cases. This executes extracted source assignments/branches with mock records, not complete VkRenderPass2/vkCmdBeginRendering objects or an application-cache/device roundtrip. Android SDK33 KGSL builds the real modified pass, command-buffer and blitter translation units. The previously validated shader arithmetic is unchanged and is not needlessly re-tested as GPU evidence.

The same-format sysmem source path is connected; native GMEM extended resolves, full view remapping, complete Vulkan/GPU rendering/synchronization validation and exposure policy remain pending. Mesh/task and other open capabilities are unchanged. Phone retains2e9c8f5f; no deployment.


## A8xx non-FDM GMEM extended resolves

Recovered the original A8xx GMEM store mode forwarding (GameSir ELF function at `0x90cd80`) and its 3D-store helper contract. The shader store requires independent source/destination sample counts, MIN/MAX/AVERAGE mode flags, a separate S8 plane for D32S8, and reserved color CCU space. The existing 3D-store helper already performs GMEM cache invalidation, WFI, color clean and register restoration; these operations were retained.

This checkpoint connects the recorded depth/stencil modes to GMEM stores on A8xx without FDM. It selects `AVOID_CCU` layout before tiling and prevents the CP blit fast path for extended modes. D24 packed mixed modes use aspect masks; D32S8 uses only depth in its common plane and only stencil in its separate plane. An intermediate double-stencil emission bug in the uncommitted port was caught and corrected before checkpointing. Explicit SAMPLE_ZERO prevents a mixed-mode job from selecting the old optional GMEM averaging prototype. Destination depth LRZ is disabled when written.

Validation uses extracted current source with mocked GPU operations:

- 3,888 extended GMEM branch cases: modes, NONE/sample-zero companions, six formats, plane stores, 2/4/8 source samples, multiview and per-layer rectangles.
- 480 old/new 3D-store traces: existing calls retain sample selection, plane views, cache/WFI and register restore order; new arguments change only the intended setup fields.
- 432 legacy pass mode, 1,944 dynamic mode, 3,072 generation/FDM fallback and 432 forwarding cases for each sysmem/GMEM producer.
- 32,256 ordinary sysmem recording comparisons and 145,152 extended controls remain passing.
- Original bounded GameSir selector: 8,192 native cases. Source selection: 20,480 ordinary comparisons, 1,269,760 priority guards, 32 D24 corrections and 2,560 explicit sample-zero cases.

Android SDK33 strict compilation passes. No Vulkan device/render-pass construction, rendering, GPU cache correctness or performance is established by these component tests. Existing SAMPLE_ZERO/CUSTOM property advertisement is unchanged. FDM/older-GPU extended resolves keep the conservative sysmem path; complete view reinterpretation and the full requested GameSir package remain unfinished. The phone still has checkpoint `7ccda658`; this continuation does not install a new driver.


## Resolve view reconstruction and skip-transfer flag

GameSir's private sysmem resolve (ELF `0x923c64`) rebuilds FDL views when the operation format differs from the image view format. Its public wrapper (ELF `0x905478`) optionally replaces sRGB formats with their linear counterparts. The boolean is **not** the attachment's GMEM allocation flag: both `tu_CreateRenderPass2` (ELF `0x9bd21c`) and dynamic pass setup (ELF `0x9bfa98`) copy attachment flag bit1 into record+0x22; the sysmem resolve producer (ELF `0x970f1c`) forwards that byte. The dynamic setup explicitly reads `vk_get_rendering_attachment_flags`. Local registry/runtime sources identify bit1 as maintenance10 `RESOLVE_SKIP_TRANSFER_FUNCTION`.

The preliminary port now retains that flag in legacy attachments, dynamic color resolve destinations and MSRTSS user attachments, and forwards it to sysmem. Both source and destination formats become linear when requested. The reused copy-view constructor selects the plane, identity swizzle, base mip and base array layer. Temporary views are also used for the new shader depth/stencil resolves. Existing ordinary resolves retain their old plane-view path unless the skip-transfer flag is explicitly requested. Skip-transfer passes conservatively force sysmem until GMEM handling is ported. Unresolve does not request the flag, matching the recovered GameSir caller.

Validation:

- 1,296 real FDL view-construction cases across A630/A730/A830: D32S8 plane views, D24 byte interpretation, sRGB-to-linear descriptors, 1/2/4/8 samples, mip and array-layer bases, tiling and UBWC. The linear descriptor differs from the sRGB descriptor while retaining image addresses. These test the actual host FDL library and extracted source helpers, using a small Vulkan-format mapping fixture; they do not instantiate a Vulkan image on a device.
- 48 extracted-source legacy/dynamic flag cases with missing/chained pNext records, both initial boolean values and SKIP versus ENABLE bits. The runtime flag helper is extracted unchanged; the pNext walker is a fixture.
- 192 skip-transfer recording cases with mocked GPU commands; the real descriptor constructor is checked separately above. Existing 32,256 ordinary and 145,152 extended sysmem controls still pass. Pass-mode coverage includes 16 skip-transfer fallback controls and forwarding of the destination flag.
- Android SDK33 strict driver compilation passes.

This is a maintenance10 **subset**, not full extension support. GMEM skip-transfer, ENABLE-transfer semantics, explicit resolve-image mode/flag controls, additional entrypoints/properties and their interaction with other capabilities still require audit. The extension advertisement has not changed. Full view-format parity, FDM extended GMEM and hardware validation remain open; no device performance result is claimed. The phone remains on checkpoint `7ccda658`.


## Explicit ResolveImage2 modes and depth/stencil aspects

Recovered the mode-info branch and aspect handling in GameSir `tu_CmdResolveImage2<A8xx>` (ELF `0x904914`, pNext structure tag `0x3ba466f4` = `1000630004`, `VkResolveImageModeInfoKHR`). It selects source/destination plane formats, handles skip-transfer for color, forwards actual source samples, and uses a shader path for explicit modes and DS resolves. D24 paired sample-zero/MIN may share one packed operation; otherwise aspects are handled independently.

The preliminary driver now routes explicit mode-info commands and default DS resolves through `tu_resolve_image_region`. The ordinary color command without mode-info retains the previous A2D implementation. New recording:

- honors resolveMode/stencilResolveMode per selected aspect and skips NONE;
- rebuilds source/destination plane views per region, preserving independent mip, array-layer and z offsets;
- selects the existing MIN/MAX/AVERAGE and sample-zero shaders using the actual source sample count;
- reuses our depth-aware D24 MAX/AVERAGE corrections instead of componentwise byte reduction;
- uses linear source and destination color views when SKIP_TRANSFER_FUNCTION is requested;
- drains shader color writes at one conservative final cache boundary, and disables destination LRZ only if depth was written.

This adapts recovered behavior to our shader numbering and sample-zero flag. It is not a byte-for-byte command-stream equivalence claim: our shader-source D24 corrections, NONE handling and conservative cache boundary remain explicit source choices. ENABLE-transfer semantics and full maintenance10 support are still pending. No feature/extension advertisement changed.

Validation: 648 actual-source old/new ordinary command recording comparisons, 23,085 explicit-mode/aspect/sample/flag cases, 18 default-DS/remaining-layer controls, and nine empty/NONE/multi-region controls across A6/A7/A8. GPU operations and FDL-view construction are mocked in these command tests; real FDL subresource/format construction was checked separately in the preceding 1,296-case probe. Android SDK33 strict compilation passes. These checks do not execute the command stream or prove device synchronization, image contents or performance.

GMEM skip-transfer was reviewed but not activated in this checkpoint: the source/destination descriptor SRGB interpretation must both be overridden, not just the pipeline's pipe formats. The conservative sysmem fallback remains. Mesh/task and the other mandatory open work are unchanged. The phone remains on checkpoint `7ccda658`.


## A8xx non-FDM GMEM skip-transfer

This supersedes the preceding checkpoint's held GMEM skip-transfer note. In GameSir `tu_store_gmem_attachment<A8xx>` (ELF `0x90cd80`), the attachment flag is effective only for a resolve into an sRGB format. The original copies/rebuilds both image views with linear formats before issuing the store; the pipe formats are also replaced. Its ordinary store branch does not apply this reinterpretation.

The preliminary GMEM path now follows that contract on A8xx without FDM:

- gated by a real resolve, destination SKIP flag and sRGB destination;
- reserves AVOID_CCU layout and avoids the CP blit fast path;
- reuses the generic AVERAGE resolve shader with actual source samples and a single-sample destination;
- builds both linear FDL views using the existing subresource-preserving resolve helper, then uses the ordinary 3D-store cache/WFI/register-restore sequence;
- retains sysmem for older GPUs and FDM; a SKIP bit on a non-sRGB attachment does not itself force sysmem;
- excludes SKIP sRGB from the debug-only old GMEM-to-GMEM prototype, which otherwise could overwrite the new result with its original sRGB interpretation. Normal store/load round trips remain available.

Descriptor inspection confirms why replacing the pipe format alone was insufficient: A6/A7 texture SRGB is stored in descriptor word0, while A8 SRGB is in word4; `tu_desc_set_format` only replaces the format field (A8 word3). Rebuilding the descriptor clears the separate SRGB bit before GMEM address/format patching. This is a source/host-descriptor conclusion, not GPU validation.

Validation:

- 324 integration cases with the actual host FDL library and extracted current 3D-store helper, across A6/A7/A8. Both linear views match independent FDL reference descriptors for RGBA/BGRA sRGB, mip/array bases, sample counts and UBWC; image addresses and cache/register operation order remain unchanged. GPU upload operations are mocked. A6/A7 exercise only the helper; the rendering policy still falls back to sysmem there.
- 480 prior old/new ordinary 3D-store traces and 3,888 extended DS branch cases remain passing.
- 36 skip-transfer color branch controls check AVERAGE flags, source/destination samples, view-reinterpret requests, layer masks, render areas and skipped stores.
- 400 actual-source gating/layout/prototype controls and 192 generation/FDM/SRGB/flag fallback controls pass. The original 3,072 DS fallback cases and mode-forwarding tests remain passing.
- Android SDK33 strict compilation passes.

No feature/extension advertisement changed; this remains an incomplete maintenance10/GameSir package. FDM GMEM, complete ENABLE-transfer semantics, remaining entrypoints/properties, mesh/task and hardware rendering/synchronization/performance validation remain open. The phone remains on `7ccda658`.


## Ordinary A8xx color resolve reconstruction

An additional difference in GameSir A8xx store recording was not covered by the skip-transfer-only port: for a real color resolve, its non-integer format predicate forces the shader path, even without any new attachment flags. This predicate has not been recovered in the corresponding A6/A7 store functions; their existing paths remain unchanged here. Our existing `blit_can_resolve` comment already notes that the blit event averages encoded unsigned values and can differ from the 2D draw resolve for sRGB; keeping the CP_EVENT fast path was therefore not complete parity with the recovered A8xx path.

`tu_gmem_color_resolve_needs_shader` now selects shader averaging for A8xx non-integer color resolves without FDM. Both the store decision and AVOID_CCU layout use this shared policy. Ordinary non-resolve stores and integer resolves retain their previous paths, as do older generations and FDM. Skip-transfer still uses both linear views; default sRGB keeps the original sRGB views, with decode on texture input and encode in the existing RB/SP sRGB controls. This can change command cost and GPU work; no speed or temperature improvement is claimed.

Validation:

- Executed a bounded 28-byte original A8xx color-policy range at ELF `0x90d21c` on the host:16 aspect/integer controls. The two format-query calls are explicitly mocked; only this branch fragment executes, with relocated local branch/return blocks, not the complete driver.
- 1,104 source classifications use the actual Mesa Vulkan/pipe format utilities (184 core VkFormat values, three generations and two FDM states). These test selection, not attachment format support or rendered output.
- 500 source gating/layout/prototype controls and216 ordinary/skip color recording cases pass, alongside3,888 DS branch controls. The source recording uses mocked GPU operations.
- Android SDK33 strict compilation passes. Existing AVERAGE shaders/slots are reused; no shader builder, feature or extension advertisement changed.

Maintenance10 audit also found already-present support: `vk_common_CmdSetSampleMaskEXT` checks NULL and initializes all supported mask bits; the generated dispatch table aliases `CmdEndRendering2KHR` and `CmdEndRendering2EXT` in one union slot. No duplicate patch was added for either. Registry/spec review additionally identified vertex sRGB semantics, local-read feedback control, format-copy queue flags and properties as requirements still to verify before extension advertisement. Recovered GameSir property setup stores gen>=8 into all three maintenance10 property fields; this alone is not proof our full implementation meets those properties, and they are not copied blindly.

Reference: [Vulkan maintenance10 requirements](https://docs.vulkan.org/refpages/latest/refpages/source/VK_KHR_maintenance10.html). [Attachment feedback/resolve flags](https://docs.vulkan.org/refpages/latest/refpages/source/VkRenderingAttachmentFlagBitsKHR.html).

User testing of the installed `6031b938` preliminarily reports a hotter driver, without numerical thermal/FPS measurements or a confirmed cause. Thermal/performance comparison is deferred until the full reconstruction is understood. This ordinary-color follow-up was not installed and cannot explain that observation. The phone remains on `6031b938`; retained artifacts/backup enable later comparisons.


## Task AQE compiler reconstruction

Recovered and ported task/mesh preparation and the complete task intrinsic callback into `tu_nir_lower_mesh.cc`. The task ABI uses constants0/1 for chunk index/size,2..3 for the metadata pointer,4..5 for the payload pointer,6..8 for workgroup dimensions and32 for draw ID. Payload uses raw hardware group X and `align(max(payload_size,1),4)` bytes per group; logical workgroup IDs include the chunk offset. The first local invocation writes the recovered24-byte AQE record, including zero-launch handling and logical task XYZ. Task/output barrier modes become global while unrelated modes, semantics and scopes are preserved.

The recovered compiler order is prepare after SPIR-V/disassembly, explicit payload/shared I/O, retained NIR, then task normalization/AQE lowering. The first direct array hooks attempted to apply this order prematurely and have been removed after the startup crash; pipeline stage storage still has only six entries. The generic normalizer sends payload atomics and small types through shared memory before copying payload to global memory. Compute-only base-workgroup lowering remains compute-only.

Intentional adaptation: logical workgroup coordinates use exact `nir_udiv`, lowered by existing IR3 integer division passes, instead of the recovered float reciprocal plus one-step correction. The original reciprocal input-range assumption is unverified; this does not establish a GameSir bug or a performance gain. Mesh shader preparation is present, but full mesh output lowering and backend/pipeline/dispatch support are still missing; no mesh/task feature is exposed.

Validation:1,296 actual NIR fixtures cover prepare+lower and direct lowering, chunk/global versus raw indices, dimensions, draw ID, payload addresses/alignment/access/write masks, first-invocation conditions, zero launches and24-byte launch metadata, including indices near UINT32_MAX. Eight generic-normalization cases cover8/16/32-bit payload and atomics, with/without explicit launch; two std430 task/mesh explicit-I/O fixtures use the actual driver type callback, plus empty-payload stride and mesh preparation checks. NIR validity/SSA dominance checks run on generic fixtures. These are host compiler/constant-folding tests, not original native builder execution, a complete Turnip mesh pipeline test or GPU validation. Android SDK33 strict compilation passes. Evidence: `task-reconstruction-evidence.json`, `task-lowering-probe.log`, `check_task_lowering.py`.

The requested full GameSir package remains incomplete. Phone stays on6031b938, with the user's preliminary hotter-driver feedback preserved; no new installation was requested or performed during this compiler step.


## Mesh input and output I/O reconstruction

Mesh input lowering now uses the recovered stage-specific ABI: producer task index14 for payload addressing, dimensions15..17 for logical workgroup IDs/counts,32 for draw ID and33 for multiview index. The chunk/global workgroup formula is shared with task, while payload uses its independent producer-task index. Mesh view masks are retained alongside geometry view masks in `tu_lower_nir`. Preparation and mesh-input passes remain callable and tested, but are detached from pipeline compilation pending the coordinated eight-stage integration. The mesh capability remains unavailable.

Added a callable mesh-output pass with an explicit layout subset. Ordinary outputs are stored in both shared memory and per-group AQE vertex/primitive buffers; readback uses shared memory. Indirect varying slots advance16 bytes and components4 bytes. The original shared-base/stride/capacity/slot-map offsets are recorded in `mesh-io-reconstruction-evidence.json`; the source structure deliberately does not claim to be the complete600-byte original layout ABI. The pass additionally handles packed16-bit point/line/triangle indices, multiview flags, layer/viewport/shading-rate read-modify-write flags and conditional atomic-AND cull bitmap updates. Output barriers map shader-out to shared and retain other modes/scopes/semantics. This output pass is not yet connected to shader compilation: layout construction, count publication, metadata initialization/finalization and runtime state must be ported together. Primitive-index read unpacking is still unresolved if such reads can reach this path.

Corrected the earlier task payload port's alignment metadata. Actual Mesa explicit-I/O lowering supplies256-byte relative alignment at a variable base; the core explicitly requires backends to clamp this when necessary. With a real std430 array of two uvec3 values the used size is28 bytes, so an AQE payload stride of28 breaks that alignment for group1. Task and mesh payload global accesses now conservatively cap alignment at4 bytes, preserving smaller alignments and reducing align_offset modulo the new alignment. This changes compiler metadata, not the recovered address/stride ABI. Four actual std430 regression fixtures demonstrate the incorrect previous promise and the corrected one; no device fault or performance change is claimed.

Validation:6,912 actual mesh NIR fixtures exercise preparation/input lowering followed by output lowering, independent raw/logical/producer-task indices,64-bit pointers,8/16/32/64-bit payload loads, payload size/alignments, view/draw IDs, shared readback, dual global/shared stores and component/array offsets. Another576 packed-output cases cover point/line/triangle, raw groups, bitmap boundaries0/31/32/255, view/volatile/cull toggles and prior flag patterns. Prior global flag reads are explicitly mocked; host arithmetic and NIR structure are checked, not GPU memory ordering/rendering. NIR validity and SSA dominance checks pass. The1,296 task fixtures and8 generic normalization cases still pass, alongside std430/empty-payload/preparation cases. Android SDK33 strict compilation passes. Reproduction: `check_mesh_io.py`, `check_task_lowering.py`; evidence: `mesh-io-reconstruction-evidence.json`, `mesh-io-probe.log`, `task-lowering-probe.log`.

Next recovered but not yet ported: original `set_vertex_and_primitive_count` stores static counts in layout0x3c/0x40, or initializes per-primitive metadata and publishes dynamic counts to const10 at raw*4 and (raw+const1)*4 under local invocation0. The wrapper also performs initialization/finalization and barriers; copying only that callback would be incomplete. Local Mesa has a third decomposed-count source, but SPIR-V SetMeshOutputsEXT supplies undef there, as confirmed in spirv_to_nir.c. Full layout calculation, this wrapper, backend/pipeline/VPC, direct/indirect/count dispatch and CCU lifetime remain mandatory before exposure. Phone remains6031b938; its preliminary hotter-driver feedback is unchanged and unrelated to these uninstalled follow-ups.


## Startup crash regression: six-stage pipeline storage

User testing and a native Xendroid tombstone confirm e034d70c crashes before a game opens; GameHub reports immediate graphics-pipeline creation failure. Xendroid loads its own extracted e034 library, not the GameHub slot. The crash is in `nir_lower_compute_system_values+4` with x0=1 / fault0x71. Mapping LR to the exact installed SO identifies the call in `tu_nir_prepare_task` at ELF0x9a2018.

This was our integration error: local `MESA_SHADER_STAGES` is6 (VS through CS), while TASK=6 and MESH=7. Both callers and the compiler's arrays/loops still use six stages. The direct task/mesh array hooks introduced in19dcceaf and retained in e034 therefore read outside caller storage even for ordinary pipelines. Host tests previously validated individual passes, not this caller integration; successful Android compilation did not protect against the error.

Removed all four premature array-based task/mesh hooks, leaving the restored passes callable and their per-shader I/O groundwork intact. Simply enlarging a local array would not repair caller storage, cache keys, pipeline shader arrays, command state or IR3 backend arrays. `mesh-stage-capacity-inventory.json` records the sites for coordinated integration. No feature exposure is added.

Regression validation extracts the complete old and corrected `tu_compile_shaders` function bodies, uses actual Mesa/NIR stage definitions and real NIR allocation/cloning, and places all six-entry caller arrays against protected pages. The original read is reproduced in a child process.4,992 corrected compiler control-flow cases pass with ASan/UBSan: valid stage combinations, cache/disassembly/retained NIR and parser/backend failure paths. SPIR-V parsing, lowering/link and GPU backend operations are explicitly mocked; this is compiler-array regression coverage, not full Vulkan/GPU validation. Exact reproduction: `check_compile_stage_bounds.py`; evidence `startup-stage-bounds-evidence.json`, `compile-stage-bounds-probe.log`, filtered `xendroid-e034-native-crash.log`.

The task/mesh source restoration is retained. Future work must expand complete pipeline/backend storage and stage iteration together before reconnecting these passes. Mesh layout, counts/publication/metadata initialization and dispatch restoration remain pending. No further feature port is mixed into this startup hotfix.


## EXT mesh counts and metadata initialization

Added a standalone `tu_nir_lower_mesh_counts` pass. At the workgroup-uniform SetMeshOutputs site, the first local invocation initializes each primitive's volatile u16 metadata flag, optionally initializes the32-byte cull bitmap to all ones, and writes primitive/vertex counts to the recovered const10 buffer at rawGroup×4 and(rawGroup+const1)×4. The third local Mesa intrinsic operand is the GS decomposed-primitive count; SPIR-V SetMeshOutputsEXT supplies undef and mesh publication consumes only sources0/1. The pass lowers its local loop counter to SSA and is idempotent.

Conservative adaptations, explicitly not a byte-identical GameSir wrapper: initialization/counts stay at each original count site, including conditional constant sites, rather than assuming the last constant represents the entire shader. Cull initialization is done for both constant and dynamic EXT counts when needed; the original full-pipeline initialization guarantee for every dynamic path remains unproved. A workgroup execution/global-memory ACQ_REL barrier follows first-invocation initialization for multi-invocation groups, so other invocations cannot update flags/cull before initialization. Equivalent original hardware ordering is not established; no performance claim is made. A one-invocation group omits this extra barrier.

Validation executes actual lowered NIR through a local CFG/phi interpreter using Mesa's real ALU evaluator:5,040 cases with count0/1/7/32/256, point/line/triangle, raw group0/3/31, group size1/32/128, first/non-first invocations, constant/dynamic/conditional counts, cull and branch toggles. It verifies every metadata address/value, bitmap words, count-array separation, no duplicate initialization by other invocations, and barrier-before-output event order. NIR validity, dominance and idempotence pass. Twelve A830 compilations of compute-only surrogates (after actual compute sysval lowering) verify the common instructions/loops/barriers compile; group128 retains a backend barrier. This is NOT mesh ABI compilation or GPU execution/synchronization validation. Reproduction: `check_mesh_counts.py`; evidence `mesh-counts-reconstruction-evidence.json`, `mesh-counts-probe.log`.

The installed phone driver remains the separate5c4d1b00 startup fix; these count changes were not installed. Task/mesh pipeline hooks remain detached, with six-entry compiler bounds covered by the4,992-case regression. Full layout/VPC, NV/shared count fallback if needed, eight-stage pipeline/cache/backend state, AQE direct/indirect/count commands and CCU lifetime remain open. The requested complete GameSir package is not ready.


## Mesh layout kernel and migration order (2026-10-18)

The user agreed to finish GameSir reconstruction in this separate branch first. Afterwards create a clean checkout/branch from a freshly fetched official Mesa main, port our changes with cherry-pick/adaptation while checking upstream equivalents, and then port this GameSir series. Do not merge the old tree wholesale, blindly replay upstream fixes, or rebase this evidence branch now. The historical audit fixes are also to be preserved and reviewed during migration. Fresh-main snapshot today:9a1e58dabb9c; missing-upstream-patch review is stored outside this checkout under upstream-review-20261018. No upstream candidate has been applied here.

New tu_mesh_layout.h/.cc reconstruct the complete600-byte layout arithmetic kernel, from precollected I/O maps/qualifier keys. The pure kernel packs3/2/1-component variable outputs into16-byte registers only within identical qualifier groups, handles the unpacked-variable priority mask, primitive outputs, optional no-FS primitive-ID reservation, position/point size, clipping masks and aligned shared regions. Metadata flags at original+0x53/+0x54 remain explicitly opaque; their system-value meaning and the producer of interpolation/per-view qualifier keys still need recovery. No raw original nir_shader ABI offsets are used in production source.

A bounded native comparison executes original ELF0x9ad478..0x9adc74, ending before the NIR pass call. It uses mock precollected I/O and shader records. The original memset call is replaced by host prezero, the topology table address is relocated to its original20 bytes, and the unused next-pass callback address is replaced by zero. The wrapper restores its return address because the original non-leaf code uses w30 as a scratch register. The first wrapper attempt crashed in that harness and was corrected; this was not a driver/device failure. All direct branch destinations are range-checked; no Android driver is initialized.

All600 output bytes and the shared-size update agree in12000 deterministic native original-versus-source cases. Coverage includes three topologies, zero/maximum tested capacities, packed/unpacked outputs, qualifier groups, builtins, clip fallback, view/layer/viewport/cull/rate flags, no-FS primitive-ID reservation and shared alignment boundaries. The kernel clears primitive-used slot24 before generic packing while retaining its component metadata: shading rate uses its separate shared area. This resolves the earlier ambiguity about possible duplicate allocation.

This is a source port of the arithmetic kernel, not complete mesh support. NIR I/O discovery/qualifier production, conversion to the existing output-lowering view, NV/shared count fallback, eight-stage pipeline/cache/IR3 storage and linking, AQE draws/dispatch and CCU/resource lifetime remain pending. The new builder has no production caller. Mesh/task hooks remain detached; no extension exposure or device installation changes.


## NIR mesh I/O discovery and recovery corrections (2026-10-18)

The source now collects mesh output usage/components from all four per-vertex/per-primitive load/store intrinsics, recognizes special primitive flags/indices, handles compact clip/cull slot spans, collects the FS explicit-per-vertex mask and builds the layout from actual NIR. A separate helper adapts the full layout to the existing output-lowering view and expands shared_size. Packed-index volatility is an explicit caller parameter: the full original publication wrapper policy remains unresolved, so the helper does not silently choose it. Neither helper is attached to the ordinary six-stage pipeline.

Correction to the earlier layout section: qualifier-key low bytes are the output store's SRC_TYPE, NOT interpolation mode; high bit8 means the linked FS interpolates that slot, NOT per-view. Original intrinsic descriptor+0x3f selects store index5, SRC_TYPE. Original shader+0xb0/+0xb8 are perspective_varyings/linear_varyings: gather_func_info writes them for load_interpolated_input, and tu_compile_shaders copies both from the FS to the prior stage. The layout arithmetic was already generic and remains valid, but its earlier semantic labels were wrong. The source now names both system-value flags: original bitset begins at shader+0xa0, and the original gl_system_value_name table identifies bit17 as DRAW_ID and bit50 as NUM_WORKGROUPS. No guessed raw offset is used in the source collector.

Before the user's workflow change,2000 actual NIR mesh/FS fixtures passed24000 bounded native original I/O-record executions, comparing every used/component/key slot and special primitive flag. Coverage includes scalar and vectorized original slot loops, repeated/overlapping stores, mixed source types, clip/cull span conversion, loaded outputs, two sysvalue flags, FS priority-mask boundaries, the adapter and actual output-lowering NIR/SSA validity. The12000 native layout comparisons still pass after the naming correction. These are structural NIR fixtures, not imported SPIR-V/Vulkan pipelines or GPU tests. An initial generated fixture supplied only two primitive indices to a triangle output; its output validation correctly failed, and the fixture was corrected to the topology's index count. No production change was needed for that fixture error.

Workflow change requested by the user: prioritize transferring related blocks and then perform a coordinated audit of the complete package. During transfer use compilation and short checks for concrete integration hazards; do not repeat every old regression suite after each isolated addition. Track exact recovery, adaptations, unresolved policy and unvalidated behavior in the ledger. Rerun an old test only when its source/dependencies change or a new failure warrants it. The new/current preliminary package remains incomplete and unvalidated on GPU. Next port blocks include the original NV/shared/static count wrapper, eight-stage pipeline/cache/IR3 linking, AQE direct/indirect/count dispatch and resource lifetime.


## Task/mesh AQE batch entrypoints and shader-cache payload (2026-10-18)

Related transfer batch, per the user's request to postpone full validation:

- tu_nir_lower_task_aqe composes preparation, generic task payload lowering with atomics/small-type shared fallback, AQE task accesses, remaining compute-system-value lowering and integer division lowering. It requires already explicit payload/shared I/O, as established by tu_lower_nir. It is not attached to the ordinary pipeline.
- tu_nir_lower_mesh_aqe preflights the supported EXT path, composes layout discovery/building, mesh preparation, logical input lowering BEFORE raw count/output operations, count-site publication, output lowering and integer division lowering. It returns layout metadata for future AQE state setup. Unsupported NV shaders, primitive-index readback, remaining generic/per-view output I/O, output dereferences and missing count sites fail before mutation.
- Constant entry-block counts from exactly one SetMeshOutputs site populate static_vertices/static_primitives. All other forms are conservatively marked dynamic. Publication stays at the original count instruction, including conditional cases; this intentionally avoids GameSir's last-constant global hoist. counts_initialized records emitted publication code, not proof that conditional execution will reach it. The existing conservative count-site barrier remains. No original whole-pipeline ordering equivalence is claimed.
- Shader creation now copies the full mesh layout from tu_shader_info into the mesh stage's tu_shader union. Mesh serialization/deserialization write/read all600 bytes, matching original tu_shader_serialize0xb08174 and its reader. The original tu_shader_create also copies600 bytes from shader-info+4, independently confirming the stage-specific payload. Other stage cache payload formats remain conditional on their existing stage. This carries metadata for future integration; the current compiler still does not produce mesh pipelines.

The original wrapper's shared-count fallback needs scope clarification: in the inspected EXT callback, dynamic_counts is set alongside counts_initialized. The outer wrapper exits early when counts_initialized is set, so its dynamic shared-count tail is not reached by that EXT path. A separate NV/shared count producer has not been identified. Do not invent one or describe unsupported NV support as a confirmed missing GameSir capability. This branch remains catalogued for later review.

Validation for this NEW batch: Android SDK33 compilation/link only. No new native equivalence suite, NIR/runtime execution, shader-cache round trip, full Vulkan pipeline or GPU test was run. Broader validation remains deferred until the package is assembled. Main integration still requires eight-stage caller storage and ownership, proper mesh-to-FS linking, IR3 task/mesh compute-like stage handling, AQE state/commands and resource lifetime. The startup-fix prohibition on premature array hooks is retained.


## Eight-stage storage and IR3 task/mesh execution resources (2026-10-18)

Transferred as one related infrastructure batch:

- TU pipeline/library/retained-NIR/program/command shader arrays, stage feedback, temporary compiler arrays and stage bit iteration now use MESA_SHADER_MESH_STAGES (eight). The public common Mesa GL count is not changed. All internal tu_compile_shaders callers now provide eight slots. The old six-stage hooks are not re-enabled.
- Varying linkage follows explicit VS/TCS/TES/GS/MS/FS producer order; TS payload and CS are excluded from the graphics varying chain. The previous numeric enum walk would put FS before MS. The existing compute-only fallback is retained.
- Retained-NIR cache records now contain eight stage-presence entries. A new graphics pipeline hash schema marker separates them from six-entry records, including builds with a build-ID override. Stage-specific mesh binary metadata remains600 bytes as ported earlier.
- Local IR3 classification includes TS/MS in compute execution-resource decisions: shared/local memory, workgroup metadata, subgroup lowering/parameters, threadsize choice, RA input handling, maximum constants and sysval register extraction. Stage identity remains TASK/MESH in NIR and variants. Both are excluded from ordinary store_output setup; the existing alias.tex exclusion already covered them. Indirect-output support covers all eight stages. New task/mesh shader objects capture NIR shared_size for compute constant/local-memory budgeting. This is a source adaptation across local backend interfaces, not proof that every original backend detail has been recovered.
- Keep ir3_trim_constlen's six-entry legacy input contract unchanged: Gallium callers still supply six entries, and TS/MS's compute-resource constant budgeting is separate. Full AQE program handling of that budgeting is still pending.

The common TU compiler explicitly returns VK_ERROR_FEATURE_NOT_PRESENT when TASK/MESH is supplied. This is temporary staging until AQE graphics-program/state/dispatch are ready; it prevents prematurely treating new storage as working mesh pipeline support. No feature/extension was enabled. In particular, complete mesh program emission, pre-raster/GPL mesh keys, MS-to-FS hardware VPC state and compute-resource binding still require porting.

Focused integration smoke only, not the deferred full audit: two minimal actual TASK and MESH shaders pass their composed AQE lowering and compile to A830 ISA while retaining their stage IDs, local size and shared-memory metadata. The first MESH compile exposed an unlowered local_invocation_index introduced by count initialization. The original tu_compile_shaders calls nir_lower_compute_system_values after tu_nir_lower_mesh; that missing post-mesh step is now in the wrapper, and both samples compile. This is a concrete integration correction to the prior batch, not an FPS or GPU correctness result. Four ASan/UBSan cases with protected eight-entry arrays pass (empty, VS+FS with retained NIR/disassembly, TASK rejection, MESH rejection), with parser/backend explicitly mocked. The historical full bounds recipe was updated to the new eight-entry API while retaining its old six-entry crash reproducer, but the full sweep was not rerun.

Remaining: AQE graphics program/state and direct/indirect/count commands, pre-raster/GPL integration, compute-resource constants/descriptors/private memory, rasterizer/VPC mesh layout use and CCU lifetime, final cache/ownership/performance and GPU validation. The user-requested fresh-main migration remains a later step.

## A8 AQE packet construction and compute-like shader upload (2026-10-18)

Related transfer batch, full audit deferred as requested:

- tu6_emit_variant routes TS/MS through the existing compute register/configuration upload path via ir3_stage_is_compute, preserving variant stage identity. Original A8 helper ELF0xb12834 classifies bits5/6/7/14 as compute-like. No unrelated CS configuration bits were changed. This closes the previous unknown-stage upload switch for future AQE variants; full descriptor/private-memory/program binding still needs integration.
- tu_mesh_dispatch.h/.cc reconstruct standalone A8 packet builders from original ELF0x97ccb4 (direct/indirect opcode0x7a,32/31 total dwords) and0x97ca84 (per-view state,2/8 dwords). The port includes resource sizing, special-feature budget selection, point-output clamp, even capacity division, large direct grid split, all resource/GMEM address words and ordinal offset. The ordinal's32-bit wrapping before IOVA addition is independently confirmed by original instructions0x97cf20(add w14,w14,w7,lsl16) and0x97cf44(add x14,x11,x14). The repeated upper opaque-header word is intentionally retained.
- Original command-buffer fields+0x1a20 and byte+0x5734 remain explicitly opaque inputs. Resource allocation IOVA and GMEM base are supplied as typed inputs rather than copied raw ABI offsets. The A8 register addressesab78/ab79 and packet opcode0x7a have no known names in our XML; recovered literal headers/parity are kept. Known viewmask addresses9b0d/90c1 agree with current A8 XML. No guessed field semantics are introduced.
- The builders only construct bytes. They have NO Vulkan caller and do not allocate/pin resources, reserve command stream space, synchronize or submit packets. Do not connect them until AQE resource ownership, draw begin/end, task scheduling and program/rasterizer/CCU state are ported. Ordinary graphics cannot reach these builders; the compiler TASK/MESH staging rejection remains enabled.

Validation: Android SDK33 incremental compile/link passed.20 small ASan/UBSan packet-construction fixtures check direct/indirect word counts, sentinels, headers against Mesa PM4 helpers, IOVA word/carry layout, selected sizing branches and view-index shift boundaries. The initial host probe compile used the wrong generated-header directory; the harness include path was corrected, with no production source correction needed. This is NOT native original equivalence, a Vulkan pipeline, GPU execution or performance validation. No old full suites were repeated. Evidence/reproduction: mesh-packet-smoke.log and check_mesh_packet_smoke.py.

Device checkpoint35526a837c3a was installed before this batch, with matching ZIP/SO hashes and an old-slot backup; deployment.json records that installation. This new local batch is not installed. Full requested package remains incomplete; next AQE blocks are resource allocation/lifetime, draw begin/end, task scheduling and direct/indirect/count command integration, plus mesh VPC/rasterizer and pre-raster/GPL state.

## AQE storage, draw state and dispatch loops (2026-10-18)

Continued related transfer batch; full semantic/GPU audit remains deferred.

- tu_get_mesh_aqe_bo reconstructs the lazy device-owned mesh_aqe_memory allocation from original mesh_draw_begin ELF0x97c438: size0x2073000, device-local/host-visible/coherent memory, ALLOW_DUMP|INTERNAL_RESOURCE, map and initial zero-fill under the existing device mutex. The last0x20000 bytes hold two ordinal regions, selected by dispatch ordinal&1 in the original command loops. The source uses named local BO interfaces and a device member, not original field offsets. Device teardown releases the BO, matching original DestroyDevice ELF0x98dfb8.
- Deliberate allocation adaptation: create/map/initialize a temporary BO before publishing the device member; map failure frees the temporary and returns the error with NULL output. The original publishes the BO before mapping, so a failed map could leave a non-NULL uninitialized allocation for the next call. This failure-path adaptation was source-inspected and compiled, not fault-injection tested. The shared BO's GPU synchronization/cross-queue reuse policy is still unresolved; device lifetime alone does not establish safe reuse.
- Added standalone begin/end packet builders for the hardware tail of original mesh_draw_begin and mesh_draw_end ELF0x97d0ac: original program-group selection, PC/VPC provoking/order bits, stereo setup/viewmask, mesh layer/viewport/shading-rate VPC flags, point-mode VFD state and restoration of the original draw-state group. Begin explicitly excludes allocation, draw_common, trace/debug, cache/CCU and command dirty-state responsibilities. End excludes trace/debug. The original cmd+0x3ec mode source is still an explicit opaque boolean input; do not claim a mapped Vulkan dynamic-state source.
- Critical integration boundary: original state groups2/29 are retained in these detached raw builders. The local ordinary draw-state group assignment differs, so these IDs must be mapped as part of AQE program/group ownership before connecting any builder. There is no production Vulkan caller and no TASK/MESH feature exposure.
- Added dispatch-only direct and indirect loops in tu_mesh_draw.cc, preserving ascending view-bit order, non-multiview single iteration, draw index passed through ab78, ordinal&1 across draws/views and64-bit indirect address/stride arithmetic. Indirect draw_count0 emits nothing. Indirect-count composes the recovered GPU CP_COND_EXEC + one-word CP_NOP swallowing the per-view/dispatch body. The condition's always-true dword is an explicit command-buffer-owned UINT32_MAX input for the future caller; there is no CPU count-buffer read.
- Reserve each complete41/47-word indirect-count sequence before emission so its NOP body remains in one IB. These loops still require a caller to supply prepared AQE shaders/descriptors, BO ownership, begin/end, command-buffer error propagation, trace, cache/CCU and synchronization. They are internal detached helpers, not a working Vulkan draw implementation.

Validation: Android SDK33 incremental compile/link passed. An initial compile required an explicit tu_bo_alloc_flags cast for the combined enum flags; corrected. The existing packet probe was extended only because its implementation changed:24 short ASan/UBSan fixtures now also check maximum begin/end output bounds, original state headers and indirect-count conditional-body lengths against Mesa PM4 headers. No original-native comparison, BO failure/concurrency test, complete command-stream/lifecycle execution, Vulkan pipeline, GPU or performance test was run. No broad old suites were rerun. Full audit and GPU validation remain for the end.

Remaining mesh integration includes AQE program and pre-raster/GPL keys, stage groups, MS-to-FS VPC/rasterizer state, compute descriptors/constants/private memory, exact mapping of opaque command inputs, command-buffer ownership/error/trace/dirty/cache/CCU paths and cross-queue resource synchronization. NV/shared count support remains unresolved as previously recorded. Installed phone checkpoint remains35526a837c3a; this new batch is local only.

## Additional source search and AQE program-state mapping (2026-10-18)

A bounded second public-source search did not find the exact GameSir source. Exact tu_nir_lower_mesh/mesh_aqe_memory GitHub commit searches returned0; broader rutabaga+turnip results led to binary feeds/packaging, and mesa+rutabaga fork hits concern the unrelated virtualization utilities. GameSir+Mesa repository search returned0; the legacy whitebelyash branch API query was rate-limited403, so that lookup was not completed. Saved responses live outside the checkout under source-origin-search-20261018. This remains negative evidence for the checked locations, not proof of private authorship or an exhaustive source search. Return to binary reconstruction as requested.

Resumed original tu_emit_program_state A8 ELF0x9e9070. Its initial variant-state slots are zeroed, then selected normal/safe uploads for TS/MS are preserved separately. Original assignments at decompiled tail param_2[0x12..0x15] represent the16-byte task state at+0x90 and mesh state at+0xa0, following the fragment state at+0x80. Added corresponding ts_state/ms_state members and selected-state assignments to the local program. Explicitly zero-initialize the local draw_states array so absent stages produce empty states, matching the original rather than copying indeterminate local slots. Existing eight-stage descriptor linkage, dynamic descriptor-size merging and shader hash collection already process TS/MS, so those are not duplicated.

This transfers program ownership of the separate stage uploads only. It does not connect AQE dispatch, resolve original group numbering or supply MS VPC/rasterizer configuration. TASK/MESH compile guard remains, no feature exposure. Full audit remains deferred; this small related change receives Android compilation only. No new ZIP or device installation.

## Passthrough VS program, primitive FS provenance and AQE metadata — source only (2026-10-18)

User changed the workflow: do NOT build after each transfer. This checkpoint has source changes only. No compiler, shader probe, sanitizer suite, GPU test, ZIP or installation was run. The last built local artifact is57888b6d1ae6; the phone remains35526a837c3a. Do not relabel that existing SO as this unbuilt source revision.

Transferred a related mesh-program block from the mesh branch of tu_emit_program_state A8 ELF0x9e9070:

- Added tu_mesh_program.h/.cc: output-register component extents, packed SP_VS_OUTPUT and VPC destination maps, resource configuration inherited from the mesh variant, position/point/clip/cull/layer/viewport/shading-rate controls, stereo/viewmask, primitive input span and base, VFD formats/shared offsets/destinations, and the original128-byte passthrough VS at ELF0xc1154 (END plus padding). The four-entry fetch-format table comes from ELF0xae650. Raw recovered register values are retained without inventing fields missing from current XML.
- Added an upload helper that allocates the passthrough code, constructs the main setup state, builds the primitive-override-disabled binning copy, and stores the20-word AQE descriptor. It returns mapped setup/split information to a future VPC caller. The constructor accepts explicitly selected task/mesh upload references and local sizes; there is no original shader-struct offset in production source. Invalid/unsupported inputs return an error before upload. Output publication is delayed until the helper succeeds; already reserved storage remains owned by the parent sub-CS.
- Added the original PM4 splice order: setup prefix, real FS state if present, generated mesh VPC state, setup suffix containing VFD fetch. Its output is an internal state result; a future caller must publish the final VS_BINNING state and clear the displaced ordinary VS/FS/VPC groups as the original does. This does not yet implement the remaining generated MS->FS VPC register/linkage body or activate an AQE pipeline.
- Added named IR3 FS per_primitive provenance, routed load_per_primitive_input into existing FS input setup, and excluded primitive inputs from the last-vertex flat-input fallback. Existing per_vertex bit placement is preserved locally; per_primitive is a separate bit. The collector copies live varying records by semantic names, never original flag-byte offsets. The original consumer's flag bit5 is used for primitive-span classification; the producer mapping in local source uses NIR's explicit per-primitive intrinsic. Full mixed per-vertex/per-primitive location/interpolation validation remains deferred. Graphics pipeline hash schema advances to0x47533802, separating the changed FS cached metadata even with a fixed build-ID override.
- Staging limits/adaptations: the detached program builder currently rejects more than32 output attributes and packed fetch widths over4, corresponding to the map ranges available in our XML/format table. Full capability limits need reconciliation before exposure. Variable local sizes and absent/invalid stage identities are rejected. These restrictions must not be reported as complete original capability equivalence.

Recovery corrections, superseding earlier speculative labels:

- Original CmdBindPipeline writes eight shader pointers at cmd+0x1930..0x1968 and copies program state to cmd+0x1970 (decompiled lines70..119). The twenty-word data allocation is stored at program+0xb0 by tu_emit_program_state, so cmd+0x1a20 in the dispatch packet is its metadata IOVA. The dispatch input field is now metadata_iova; the duplicated upper address word remains unchanged. Only byte cmd+0x5734 remains opaque. tu_program_state now has mesh_metadata after TS/MS state slots. The original shared-constant state at program+0xc0 independently agrees with this slot ordering.
- The earlier statement that original groups2/29 differ from our enum assignment was too strong. Local TU_DRAW_STATE_VS_BINNING is2 and TU_DRAW_STATE_DYNAMIC+TU_DYNAMIC_STATE_VERTEX_INPUT is29. Original draw_common emits its cmd+0x54d0 state as group29. Raw begin/end builders retain these numbers pending typed caller wiring; no renumbering has been applied or proved necessary. Matching numeric values alone is not a lifecycle or rendering validation.

No ordinary Vulkan caller invokes the new program/upload/splice helpers. TASK/MESH compiler rejection and strict SDK33 feature exposure remain unchanged. Remaining: generated mesh VPC/FS linkage and varying modes; pre-raster/GPL mesh keys; compute descriptors/constants/private memory; command dirty/cache/CCU and exact opaque-mode source; command lifecycle/error/trace/BO synchronization; coordinated final build/audit/cache/GPU/performance checks. Probe recipes were only updated for the renamed metadata field and were NOT executed.


## Mesh-to-FS linkage preparation and packed VPC maps — source only (2026-10-18)

No build, test/probe, sanitizer, GPU run, ZIP or device installation in this batch. Full validation remains deferred by user request. The new helpers are detached; the ordinary Vulkan pipeline still does not call them, and TASK/MESH compilation remains guarded.

- Added tu_mesh_vpc.h/.cc with semantic reconstruction of the mesh branch of original tu_emit_program_state (ELF0x9e9070, decomp lines2500..2710). Collect the synthetic producer's output records (layer first, viewport in component1, then the64-slot vertex layout). Preserve clip/cull masks, compute the layer/viewport map exclusion mask and the initial position location for the per-vertex primitive-ID case.
- Prepare two borrowed copies of the real FS: full_fs remaps vertex inputs to packed vertex registers and primitive inputs to primitive_base plus primitive byte offset /4; absent producer outputs keep original FS locations. vertex_fs filters primitive, handled primitive-ID, layer and viewport records and recomputes ranges from fetched varyings. These are metadata views, not newly compiled or independently owned shaders. The original shader is not modified. Named local per_vertex presence is recomputed; the original's separate cleared flag byte has NOT been equated with any other local flag without evidence. Barycentric coordinate placement still needs coordinated handling in the final emitter.
- Corrected the previous tu_mesh_program primitive-base fallback: original shader+0x26f is layout.vertex_attributes_before_specials, NOT the adjacent primitive_id_attributes. The primitive-ID fallback is (vertex_attributes_before_specials + primitive_attributes)*4. The original +0x26e primitive count and +0x26d vertex count distinguish these fields. This is a source recovery correction, not a measured speed improvement.
- Recovered packed SP output/destination map generation from original tu6_emit_vpc A8 (ELF0x9c6d74, decomp lines1390..1443). Merge masks for all semantic outputs sharing a physical vec4, shift by each component offset, exclude requested special slots, emit in ascending physical-register order. This avoids treating packed scalar aliases as separate output records. The helper returns maps; the generic VPC emitter does not yet consume them.
- Extended the existing varying-mode helper with optional single-component or packed four-component overrides, applied before per-vertex padding, with replacement modes cleared for overridden lanes. The recovered layer/viewport pattern0xe9 means flat/zero/zero/one. Existing callers use the default disabled override. A mesh emitter caller and exact selection policy remain pending.

Explicit staging adaptations: FS must be a fragment variant with input count fitting the local array; semantic slots<64, masks<=0xf and remapped locations including padded lanes must fit128 components. Producer metadata has66 output records separately from the smaller cached IR3 variant.outputs array; no cached variant layout/schema changed in this batch. Map generation rejects physical registers>=32 and component masks crossing vec4 boundaries, rather than silently truncating them as the original does. These are current restrictions, not established GameSir capability limits. The future generic VPC adapter must resolve the output/linkage capacities, fixed-function output location/varmask policy and hardware map ranges before accepting wider layouts. Do not memcpy these66 records into the current34-entry variant output array.

Still pending in this block: opt-in generic VPC adapter with source-register locations and compact maps; final primitive/rasterizer/SP controls; barycentric padding and layer/viewport override selection; upload/splice/publication into program state. Do not infer a complete mesh pipeline from detached planning helpers. After that remain pre-raster/GPL keys, compute resources, typed command dirty/cache/CCU/error/trace/lifetime/BO synchronization and final coordinated validation.


## Generic mesh VPC adapter, final register body and linked upload — source only (2026-10-18)

Continue source recovery only, per user request. No build, compiler check, shader probe, sanitizer/GPU/performance test, ZIP or device installation. This batch does not activate Vulkan TASK/MESH compilation, draw entrypoints or advertised features.

- Split tu6_emit_vpc into an internal optional-mesh implementation and the existing six-argument public wrapper. Ordinary shader callers keep their original interface and pass no mesh plan. The mesh adapter reads the66-record synthetic producer list directly instead of resizing or copying into the cached variant.outputs[34]. The small dummy VS contains only layer/viewport presence needed by the shared interpolation helper.
- Runtime ir3_shader_linkage.var capacity grows32→66 for semantic aliases plus fixed outputs; this is NOT serialized variant data. No variant cache layout/schema change in this batch. Normal HW output maps retain their32-entry assertion. Mesh aliases are merged by physical vec4 using the recovered map generator before SP output counts/arrays are emitted.
- Mesh linkage uses source-register component positions, the recovered initial position location, and barycentric/per-vertex alignment. Inputs without a real producer do not inflate mesh max_loc (the original resets it after linking). Layer/viewport skip-mask outputs retain their physical special locations without duplicate map entries. Position/point size/shading rate/clip/cull locations follow the mesh register layout. Only the mesh option changes these policies; normal calls retain the prior path. Runtime unused-location bitsets are not claimed byte-identical to the original reset sequence; complete packet/linkage comparison remains deferred.
- Added tu_a8_emit_mesh_vpc: shared VPC state followed by recovered PC_VS synth-layer/shading-rate controls; synthetic layer SIV state; no-FS register resets and tu6_emit_fs(NULL); fixed A8 controls; primitive span/base/stride, primitive override and ViewIndex location; SP fragment input count; final layer/viewport four-component interpolation overrides or clip/cull fallback. The original slot0x1e is named VARYING_SLOT_VIEW_INDEX here. Nonzero primitive span remains enabled even when full FS total_in exceeds primitive_base+span; total range is then expanded, not disabled. This corrects a possible earlier reading of decompiler branch labels; no previous active path used that tentative interpretation.
- Added unpublished VPC upload (current conservative reserve1024 dwords) and tu_a8_upload_linked_mesh_program. The latter derives setup input records from the same real FS (including non-bary non-sysval records), uploads setup + VPC, then calls the existing prefix → real FS state → VPC → suffix splice. Final output is published to the helper result only after all steps succeed. On failure, allocated storage remains owned by parent sub-CS and partial state is not exposed. Errors from a direct emit require discarding the entire unpublished stream. The original program-state group assignment/ordinary group clearing is still the future caller's job.

Raw original data was read for provenance only: ELF0xaf6b0 = [0x48924001,0xe0] clip/cull fallback;0xaf4e0 = [0x489b0701,1];0xb04c0 = [0x409b0601,0]. No-FS pairs come from ELF0xae380,0xaef10,0xae890,0xae010,0xaf9d8. This was a data read, not execution/equivalence validation.

Current staging restrictions remain32 physical maps, one-vec4 masks and128 component locations. The emitter additionally rejects location overflow in fixed outputs, barycentric coordinates, primitive ranges or four-lane special overrides. Its caller must check CS allocation/status. Final validation must reconcile hardware ranges, reserve bounds, mixed primitive/per-vertex interpolation, alignment/default-output handling and the original cleared FS metadata flag before feature exposure. No GPU correctness/performance result is established.

Next integration: program state group publication and displacement of ordinary groups; pre-raster/GPL mesh keys; coordinated TASK/MESH resources/constants/private memory and AQE command state/cache/CCU/error/trace/BO synchronization. NV/shared count path remains unresolved. The compiler guard and strict feature exposure remain in place. Last built local artifact remains57888b6d1ae6; phone remains35526a837c3a.


## Mesh program publication and initial pre-raster/GPL keys — source only (2026-10-18)

No build, compiler invocation, functional tests/probes, sanitizer, GPU/performance test, ZIP or installation in this batch. Only source whitespace checking. The existing tu_compile_shaders TASK/MESH rejection and strict feature exposure remain in place: the newly wired program path must not be interpreted as a usable mesh implementation.

- tu_emit_program_state now has a mesh branch using the selected TS/MS uploads and shader.mesh layout. The real FS state is mapped from its owning shader suballocation after checking mapping presence, IOVA alignment and the complete normal/safe-state byte range. The linked upload's combined stream is assigned to vs_binning_state (original program+0x20), while vs_state/fs_state/vpc_state are cleared exactly as the recovered tail does. AQE metadata is assigned to mesh_metadata (original program+0xb0). TS/MS uploads remain separately owned by their shader allocations; generated code/maps/metadata belong to pipeline sub-CS.
- Reject mesh programs on non-A8xx, mesh mixed with real legacy geometry stages, and TASK without MESH. Avoid absent-VS/GS binning-state dereferences in the mesh path. Program viewport flags select MS as the rasterizer producer, mesh shading-rate output comes from its layout, and absent FS reads_shading_rate is false.
- tu6_emit_program_config uses the original mesh policy for SP_UPDATE_CNTL: regular graphics stage-update bits are zero in mesh mode (original A8 ELF0x9e9070 decomp1058..1075, register0xab1f). Common graphics shader configuration remains shared; do not duplicate TS/MS compute configuration in the regular VPC stage list.
- Program construction returns VkResult and checks sub-stream allocation/status. The pipeline builder propagates failure and destroys the partial pipeline. Helpers publish combined program state only after successful construction; a failed pipeline and its generated allocations are discarded by the owner. Existing shader references/BO lifetimes remain with the pipeline/cache ownership model.
- Pipeline CS allocation reserves mesh-only slack8192 dwords plus max(normal FS state,safe FS state) for the copied fragment state. Current source arithmetic budgets setup<=512, binning<=518, VPC reserve1024, composite<=512+1024+FS and code/alignment/metadata, in addition to ordinary config/state allocation. This is a conservative staging allocation, not a validated final bound or memory optimization. The final check must include allocator accounting/error paths and maximum-stage descriptors/constants.
- Initial pre-raster key adaptation: TS/MS receive multiview masks; MS is the last rasterizer producer for the existing FDM-per-layer key. Cached mesh producers imported through GPL supply FS primitive class from layout.vertices_per_primitive; fresh linked mesh NIR supplies it from info.mesh.primitive_type. FS input-assembly topology is therefore not incorrectly substituted for point/line/triangle mesh output topology. These are semantic adaptations to local keys, not a claim of complete binary-identical GPL behavior. Ordinary non-mesh key paths retain their prior selection.

Recovery clarification: reread original ir3_trim_constlen (ELF0xaa7aec, Ghidra0xba7aec). It accesses legacy entries0..5; the six-entry contract was correctly retained previously. Do NOT expand it to eight or add a second copy just because the Turnip shader arrays now have eight entries. Program TS/MS upload selection is separate; per-stage AQE constants/resources still need their own recovered command path. This is evidence from source reading, not an execution test.

Remaining: full mesh/FS interface relinking and retained-NIR policy across GPL combinations, task/mesh preparation/lowering activation, compute descriptor/constant/private-memory scheduling, typed AQE command state/dirty/cache/CCU/error/trace/BO synchronization, NV/shared counts/limits and final coordinated build/audit/cache/GPU/performance checks. Source wiring is farther along; TASK/MESH execution and extension exposure remain blocked intentionally until that work is complete. Last local built SO/meta remain57888b6d1ae6; installed phone35526a837c3a.


## TS/MS constants, graphics/compute descriptors and query accounting — source only (2026-10-18)

No build, compilation, functional tests/probes, sanitizers, GPU/performance test, ZIP or installation. Only git diff --check for source whitespace. TASK/MESH compiler guard and strict feature exposure remain unchanged.

- Original A8 tu_emit_consts (ELF0x97eaa4, decomp156..174) explicitly emits push constants, inline UBOs and bindless base addresses for stages6/7 using graphics descriptor state. tu6_const_size (ELF0x97fbb4) likewise includes their bound shader metadata. Extended local graphics sizing and emission through MESA_SHADER_MESH_STAGES, skipping absent stages and the separately bound COMPUTE stage. Both use the selected program linkage metadata for consistent size/emission accounting, including safe-variant constlen. The compute bind-point path remains separate. Explicitly skipping COMPUTE is a local bind-point adaptation: original scans all8 pointers, with no compute linkage in a graphics program.
- Recovered per-stage push-constant packets (ELF0x980810) use CP_LOAD_STATE6_FRAG and SB6_CS_SHADER for both TASK and MESH. Fixed tu6_stage2opcode/texsb/shadersb to map workgroup stages (COMPUTE/KERNEL/TASK/MESH) to compute opcode/state banks, preserving API shader identity. Previous enum addition would have produced invalid TS/MS bank indices14/15. This corrects a detached path before activation; it is not a measured FPS fix. Graphics FS and legacy geometry mappings remain unchanged.
- Original graphics tu6_emit_descriptor_sets A8 (ELF0x97efdc, decomp60..75 and112..245) duplicates bound descriptor addresses and the reserved dynamic-descriptor set into graphics bank0xab0a and compute bank0xa9e8 when MS is bound. Added the same conditional duplication with matching draw-state allocation size. No new descriptor allocation/state model is introduced. Raw original SP_UPDATE pair ELF0xb05b0 is [0x48ab1f01,0], agreeing with existing A8 named field emission; bytes read only, not code execution.
- Local dirty-state adaptation for the shared compute descriptor bank: building a mesh graphics descriptor state marks COMPUTE_DESC_SETS dirty; ordinary compute dispatch with a bound mesh graphics pipeline marks graphics DESC_SETS and SHADER_CONSTS dirty. This is conservative source integration, not original packet equivalence or a complete compute/AQE context switch. Restoring compute shader/config/private-memory state after AQE execution and ordered graphics replay still need explicit lifecycle handling before activation.
- Identified original byte cmd+0x5734. Original BeginCommandBuffer (ELF0x9376d8, decomp58..63) initializes it from inherited pipelineStatistics!=0. BeginQuery A8 sets it for pipeline statistics and primitive-generated queries, including the mesh primitive query branch. No EndQuery write clearing this byte was found in the recovered query functions; it is a conservative latch rather than the current nesting count. Added statistics_query_may_be_running to command state, initialized for secondary inheritance and set by existing supported stat/primitive query begins. It resets with command state, not at query end. Renamed packet input opaque_mode to statistics_enabled. The byte's provenance is recovered; full new mesh-query support is not implemented/advertised.
- Added tu_a8_prepare_mesh_dispatch_state to capture typed resource IOVA, program.mesh_metadata IOVA, supplied GMEM base and the statistics latch. It checks A8 generation, MS identity, AQE BO size and20-word aligned metadata state. The caller must still retain/pin the BOs and reserve GMEM; this helper does not allocate, synchronize, record a draw or submit work. Other mode input (original cmd+0x3ec) remains unresolved.

Reuse clarification: TS/MS shader uploads already use tu6_emit_cs_config and the existing private-memory BO selection/retention. Do not emit those uploads again as independent ordinary graphics state groups: AQE metadata references the selected upload streams. Constant/descriptor address preparation is now source-wired, while full AQE scheduling, shared-bank isolation between TS/MS and compute, shader/private-memory restore, driver parameters, command ownership/cache/CCU/trace/error/synchronization and query counters remain unfinished. Full GPL mesh/FS interface relinking, NIR lowering activation, NV/shared-count/limits and final coordinated validation also remain. Last local built SO/meta57888b6d1ae6; installed phone35526a837c3a.


## Typed mesh draw boundaries, entrypoints and mesh sysmem CCU — source only (2026-10-18)

This continuation is source-only. No new build, compiler/test/probe/sanitizer/GPU run, ZIP or installation. The previously tested/installed checkpoint is cdeb1ae503ba (ordinary A830 graphics pipeline/cache +4096-pixel GPU readback passed there). Those results do NOT validate the new code below. TASK/MESH compilation and feature exposure remain guarded.

- Resolved original cmd+0x3ec as dynamic provoking-vertex mode. In original draw_common A8 (ELF0x97d104 region/decomp135..195) the same field determines PC/VPC provoking-last controls; existing local ordinary code already uses rs.provoking_vertex. Renamed detached packet input original_mode_is_one→provoking_vertex_last, and populate it from the named dynamic field. Last=1 is statically asserted. Also renamed restore_dirty→restore_writeable: original saved state word bit16 maps local tu_draw_state.writeable, which forces the CP_SET_DRAW_STATE dirty bit; it is not the ordinary cmd dirty mask.
- Added typed tu_a8_begin_mesh_draw/end_mesh_draw. Begin checks prior recording errors, A8/MS/program/metadata identity and recovered GMEM eligibility `(gmem_size>>21)>8`, obtains the existing device-owned initialized AQE BO, captures dispatch inputs, marks render-pass mesh use before draw_common, then emits the recovered begin packet. Snapshot the selected program and dynamic vertex-input restore state AFTER common state generation. Numeric groups2/29 are statically checked against local enums. End emits the recovered six-word restore tail, balances trace_end_draw, invalidates cached provoking-vertex state and propagates CS errors. Session data borrows shader/program allocations only until matching end; do not rebind inside that interval. Existing shader/cache/device ownership is reused, not duplicated.
- Connected direct, indirect and indirect-count Vulkan mesh command wrappers to begin → existing per-view dispatch loops → end. Non-A8 paths return a recording error. Indirect uses existing draw_wfm handling and vk_buffer_address. Count uses a command-buffer-owned UINT32_MAX dword like the existing indirect compute predicate; count remains a GPU read. Existing loops reserve each conditional NOP+dispatch body together. Zero counts follow the original begin/end behavior. No pipeline can validly exercise these wrappers until TASK/MESH preparation/lowering/GPL/feature integration is complete; the compiler guard has NOT been removed.
- Added render-pass has_mesh tracking and merge with secondary/suspend state. Original mesh_begin writes cmd+0x4df2=1 before common draw; sysmem render later uses it for CCU selection. Added a symbolic local TU_CMD_CCU_MESH_SYSMEM mode without changing prior enum values; do NOT assume its local numeric value equals original state1. Ordinary SYSMEM/GMEM remain on their prior configurations.
- Ported the previously recovered12-word mesh CCU arithmetic by named fd6_gmem_config fields in fd6_calc_mesh_gmem_config: color base0x1068000; depth base=num_ccu*GMEM attr size; GMEM attr size atoffset0; aligned16KiB GMEM pos size; pos base0x1068000+num_ccu*SYSMEM color cache size; GMEM BV-pos size/base; cache fractions/sizes retained from SYSMEM. This corresponds to original tu_emit_cache_flush_ccu A8 ELF0x93520c (original numerical state1); prior original-native6144 cases are historical evidence, NOT rerun or claimed as validation of this new helper.
- Shared RB/VPC cache emitters accept an optional config. Mesh sysmem replay chooses that config, retaining existing CCU clean/invalidate, thread control and patchpoint handling; ordinary defaults and tiled GMEM config remain unchanged. Full address/size eligibility, barriers and tile/binning interaction must still be reconciled before exposure. This is a mesh carveout, not a global resolve/ordinary-game optimization.

Correction to earlier compute-restore wording: original mesh_end ELF0x97d0ac emits the VPC/group29 restore and tracing, with no explicit compute shader/config/private-memory replay in the inspected routine. Original tu_dispatch A8 similarly re-emits shader state only under its existing stomping/debug condition. Therefore a new unconditional restore or duplicate private-memory allocator is NOT an established GameSir change and has NOT been added. Whether AQE firmware preserves the ordinary CS context, and whether this local command replay needs a conservative targeted invalidation, remains an integration/validation question. The preceding notes saying such a restore was definitely required were too strong.

Remaining: complete mesh/FS interface and retained-NIR policy across GPL, hook preparation/lowering activation, full TASK/MESH shader-stage access/barrier mapping, AQE/compute shared-bank isolation and driver parameters, renderpass/binning/CCU eligibility and resource synchronization, full mesh query counters, NV/shared count handling/limits, extended trace shader metadata, and final coordinated compilation/cache/GPU/game/performance validation. No complete GameSir capability parity is claimed.


## Explicit TASK/MESH synchronization scopes — source only (2026-10-18)

Original vk2tu_access ELF0x959808 builds shader stage masks0x40000188f8 and0x70120188fc, neither containing TASK0x80000 nor MESH0x100000. This is an observed omission, not a recovered GameSir improvement. The local SHADER_STAGES mask now includes both bits so explicit shader read/write, uniform, descriptor and generic memory access scopes enter the existing cache accounting. This is a local integration adaptation; no new cache policy or extension exposure is introduced. Aggregate ALL_GRAPHICS/ALL_COMMANDS behavior already existed.

Do not move TASK/MESH into the ordinary BV-stage list merely because they are pre-raster shaders. Original tu_barrier decomp lines285..335 classifies both explicit bits as BR for source and destination. The destination's BV branch covers only legacy geometry/input, transform-feedback and pre-raster aggregate stages. Local BR fallback already matches it and is retained with a comment. This corrects the preceding remaining-work wording: explicit TASK/MESH stage classification is accounted for; full AQE/barrier/shared-bank ordering and hardware validation remain open. The existing A750 UBWC coherency quirk is not extended by this batch; mesh activation remains A8-only and guarded.

Only source whitespace checking, no build or tests. User feedback for the installed cdeb1ae503ba checkpoint: smoothness good, no worse than their baseline; temperature not evaluated yet. Subjective feedback is not a measured performance or thermal result.


## Mesh-to-fragment NIR linkage and preparation hooks — source only (2026-10-18)

Recovered tu_compile_shaders decomp377..383 prepares TASK and MESH before common tu_lower_nir. In the mesh branch (decomp511..628), the original bypasses nir_opt_varyings_bulk, recomputes IO bases, increments only FS load_per_primitive_input.base by1, gathers shader info and copies FS known_interpolation_qualifiers/perspective_varyings/linear_varyings to the mesh producer. Original intrinsic499 is load_per_primitive_input, resolved from the previously saved original intrinsic-name table; no numeric original NIR opcode is used in production.

Added tu_nir_link_mesh_fragment in the existing mesh lowering module and routed tu_link_shaders through it only when MESH is present. The helper recomputes bases on every link before adding the reserved per-primitive slot, so a retained NIR snapshot does not receive another cumulative increment. Other FS input classes retain their recomputed bases. No ordinary varying compaction is run for mesh. Task IO bases/info are gathered separately; task payload is not passed through mesh-to-FS varying linkage. Existing non-mesh linkage and cache policy remain unchanged. The local API rejects mixed legacy/mesh programs later during program construction; complete validation of invalid combinations remains deferred.

Connected tu_nir_prepare_task/mesh immediately before common lowering in tu_compile_shaders, still BELOW the existing TASK/MESH rejection guard. Thus these source hooks and mesh-only linkage cannot execute yet. Post-link AQE task/mesh output/count lowering and IR3 mesh-key setup remain disconnected. Existing helpers are not being called prematurely with guessed layout/volatility options.

Original retains TS/MS snapshots after linkage and before final AQE lowering (decomp634..645). Current retain_all_nir snapshots already occur at that position relative to the new link helper, but full GPL policy remains pending: library producers and FS must be relinked as a coordinated interface when compiling non-LTO combinations, and A8 pre-lowered FS snapshots must not be confused with mesh-modified snapshots. No unconditional retention/recompilation cost has been added to ordinary graphics libraries in this batch. No cache-format change yet: feature guard remains, and no new mesh binary is emitted.

No build, compiler invocation, tests/probes, sanitizers, device access or ZIP in this batch. Only git diff --check for whitespace. Full mesh GPL retention/relink policy, final AQE lowering/key setup, shared-bank/CCU/driver-parameter ordering, NV/shared counts, limits/queries and complete validation remain open.


## Mesh GPL interface snapshots and coordinated non-LTO relink — source only (2026-10-18)

The original compiler retains TASK/MESH after linkage and before AQE lowering (tu_compile_shaders decomp634..645); it preserves an A8 FS snapshot before common lowering when there is no mesh producer (decomp370..378), and transfers retained snapshots into pipeline-cache/library ownership. The original builder uses eight-stage library cloning/cache slots (builder decomp2400..2545,2750..2930). These are recovery evidence; the policy below adapts them to local GPL and the existing primitive-dependent FS relink rather than claiming identical original predicates.

- Added explicit retain_mesh_interface_nir to tu_compile_shaders. FS is snapshotted before common lowering and before mesh-dependent base/layout modifications; TASK/MESH snapshots are captured after mesh linkage and before final AQE lowering. Existing retain_all_nir and primitive-dependent retention remain separate. Snapshot clone failures propagate VK_ERROR_OUT_OF_HOST_MEMORY and are released by the existing nir_out cleanup path. Borrowed library NIR is cloned on a cache miss; clone failure now propagates instead of turning an imported stage into a null pointer.
- Local library policy is gated by A8+ AND enabled_features.meshShader AND graphics-library type. This stores the unmodified consumer even for an FS-only library whose eventual producer is unknown. Currently mesh is unadvertised and the compiler guard remains: no new retention/recompile cost is activated in the installed ordinary-game path. Devices with mesh enabled in a future complete implementation will pay the necessary snapshot memory cost; this has not been measured.
- When the merged pipeline active stages contain MESH, import retained TASK/MESH/FS from their active library stages even without LTO. Missing required NIR is a recording/build error, not a fallback to silently using mismatched compiled stages. Imported NIR is borrowed only for hashing and then cloned on a miss. Selected keys are restored from owners before merged mesh/FS keys are applied.
- Complete mesh library programs are no longer inherited wholesale: the same pipeline_requires_shader_relink predicate controls parse-time program reuse and combined-state allocation/emission for states containing FS. This complements existing primitive-dependent FS behavior. Complete graphics libraries retain mesh-interface snapshots when the new policy is active, so later libraries/pipelines can still link them.
- NIR cache domains M (mesh interface) and B (mesh interface plus full LTO) are distinct from prior P/N domains. An old post-lowered FS snapshot must not satisfy an unmodified-consumer lookup. Required active FS/TASK/MESH presence is checked on mesh-interface cache hits. A lookup reference for a partial NIR hit is released before creating a replacement. No serialized struct format changes in this batch; compiled shader identity still uses the existing stage schema, with mesh compilation blocked.

This is source integration, not a complete GPL validation result. Still open: final post-link AQE lowering/IR3 key hookup, repeated-lowering and snapshot idempotence through all library combinations, safe cache-insert dedup behavior with malformed/incomplete entries, BO/context/driver-parameter and CCU ordering, NV/shared counts/limits/queries, and final compilation/API/cache/GPU/performance checks. External compiler harness calls will need the new retain_mesh_interface_nir argument when final tests are run. No build, tests/probes, sanitizers, device access or ZIP; only git diff --check.


## Post-link AQE lowering and immutable mesh FS input options — source only (2026-10-18)

Connected tu_nir_lower_task_aqe and tu_nir_lower_mesh_aqe after retained NIR snapshots, matching original compiler decomp634..645,648..655,765..790. Mesh layout is published into tu_shader_info.mesh for existing shader/program ownership. Unsupported NV/shared-count/index-read forms return an error rather than reaching IR3 unlowered. Packed index stores currently use an explicit conservative volatile=true local staging policy: the original conditional publication policy remains to reconcile. This is not a proven original optimization. The top-level TASK/MESH rejection remains unchanged.

Recovered additional FS placement inputs: original compiler decomp770..790 encodes the primitive base and layer/viewport base; decomp790..1870 forms generic/clip/primitive-ID maps using both vertex locations and primitive offsets. Original tu_shader_create decomp588..643 copies these into immutable IR3 shader options before ir3_shader_from_nir, and original emit_intrinsic decomp1021..1069 and pack_inlocs decomp143..185 consume the same placement rules. These do not fit in the local fs_primitive bits, which already encode point/line/triangle behavior. Added named ir3_mesh_fs_inputs to immutable shader options instead of overwriting local global key bits.

The typed planner uses primitive_base=(primitive_attributes+vertex_attributes_before_specials)*4 when PrimitiveID is present, otherwise vertex_attributes*4+4 for primitive attributes (subtract4 when base+primitive_stride/4 exceeds128). Layer/viewport base is vertex_attributes*4. Nonzero mapped locations encode physical component+1, using primitive_base+primitive_offset/4 for per-primitive storage or vertex_register*4+vertex_component for vertex storage. Missing entries retain original fallback rules. Locations beyond the current128-component staging bound fail. Enabled flag/maps travel through FS tu_shader_info into IR3 options; non-FS options remain zero.

Local integration deliberately delays applying physical locations until pack_inlocs: logical input indices survive IR optimization, then surviving input metadata and IR immediates (including FLAT_B pairs, LDLV and texture-prefetch offsets) are mapped to AQE physical locations. This differs from original emission-time remapping and must be checked for equivalence, aliases and optimizer assumptions. Mesh enables this final pass on A8, preserves the reserved barycentric pseudo-inputs for the existing later fixup, accounts for repeated inputs and bypasses ordinary per-vertex compaction so it does not undo physical mesh placement. Ordinary non-mesh call conditions remain unchanged. Existing mesh VPC metadata remapping must be reconciled with already-physical FS locations during final validation, especially sparse primitive offsets; do not assume the old detached VPC tests cover this new chain.

IR3 disk keys hash each named mesh-option field and map separately, avoiding padding. Turnip stage schema bumped0x47533802→0x47533803 because variant shader_options gained the mesh interface record. Old pipeline-cache artifacts must not cross this schema even with a build-ID override. Cache invalidation can cause first-run recompilation and is not a performance result. No new feature exposure or guard removal.

Remaining: original-to-local map/key equivalence and layout range proofs, emission-time versus late remap behavior, mesh VPC double-remap/sparse primitive-base reconciliation, final TS/MS resources/driver params/context/CCU/BO synchronization, NV/shared counts/queries/limits, all GPL/cache idempotence and full final build/API/compiler/GPU/performance validation. No build, tests/probes/sanitizers, device access, ZIP or install. Only git diff --check.


## Compiled physical FS locations versus VPC metadata remap — source only (2026-10-18)

Original tu_emit_program_state A8 decomp2552..2655 chooses a primitive base from the first FS per-primitive/PrimitiveID input, then remaps its borrowed FS copy using that base plus layout offsets. Its filtered copy excludes primitive/special records before ordinary VPC emission. This reproduces the earlier detached VPC helper, but combining it with the newly wired immutable FS physical maps would apply a nonzero leading primitive offset twice. This is a local integration problem; it is not evidence that a running original GameSir game has this bug.

Added explicit fs_locations_are_physical in the VPC plan, derived from fs.shader_options.mesh_fs_inputs.enabled. For this path, rebuild the expected map/base inputs from the selected mesh layout and compare named fields/maps to the compiled FS options. Mismatched producer/consumer layout fails before emission. Live bary input locations must also agree with the immutable input-location selector, using the retained logical array index as fallback. Copies still borrow all variant pointers and never change cached/live FS metadata.

For a physical FS, preserve its input.inloc rather than remapping again. The primitive stream base comes from the same immutable primitive_base supplied to compilation when primitive attributes exist, not the first live consumer (which may already include a sparse offset). Primitive span is measured from that base through the consumer end, including leading holes. A per-vertex PrimitiveID below the primitive stream base contributes no negative span. Existing primitive-ID minimum-span rules and no-FS branch remain as before. Untagged legacy/detached FS fixtures retain the old recovered remap path. Ordinary non-mesh pipeline behavior is not changed.

Example requiring final validation: primitive base20, first live primitive slot offset8 bytes: the compiled FS location is22. The physical path keeps22 and base20 instead of deriving base22 and remapping to24. Multiple sparse primitive slots, unused producer outputs, mixed PrimitiveID/per-primitive/special inputs and fragment barycentric/per-vertex combinations require final compiler+VPC+packet/GPU checks. The source change removes the identified double-offset dataflow; no equivalence or rendering test was run.

Remaining: validate the late-remap strategy against original instruction emission and packing; reconcile special PrimitiveID-only/no-primitive-attribute cases, repeated loads and sparse ranges, address/size/qualifier limits; continue original volatility/driver params/resources/AQE context/CCU synchronization and NV/shared-count/query/limit reconstruction. Existing full TASK/MESH guard and feature policy remain unchanged. No build/tests/probes/sanitizers/device/ZIP/install, only git diff --check.


## Remaining-work registry and FS Layer/Viewport demand — source only (2026-10-18)

Added GAMESIR_REMAINING.md and outputs/gamesir-preliminary/remaining-work-registry.json:18 explicit work blocks across mesh, other capability ports, unconfirmed original capabilities and final validation. The20 inherited scope areas are listed separately, without claiming they were re-audited this turn. No percent completion is inferred from the count. NV parity and symbol-only AMDX/CUDA features must first be confirmed as working/advertised in the original; shared-count fallback code or ELF symbols alone do not establish that requirement.

Transferred a missing producer/consumer connection: original tu_compile_shaders decomp660..716 scans FS IO semantics for Layer/Viewport and sets those bits in the mesh per-primitive interface before tu_nir_lower_mesh. Original lower_mesh decomp363..438 derives layer/viewport/cull/shading-rate layout flags from this metadata. Local collection now uses info.per_primitive_outputs for these four primitive fields, and includes FS Layer/Viewport IO demand while collecting its existing per-vertex/unpacked mask. This derives layout inputs without mutating borrowed FS NIR or retaining a second copy. It applies even when MS has no explicit write of the requested special output. Metadata initialization/default handling still requires the integrated final test.

Correction of earlier recovery terminology: original nir_shader+0xc8 is matched to per_primitive_outputs by the neighbouring known sysvalue/interpolation/per-view fields and the bit uses in lower_mesh; earlier notes calling it outputs_read were inaccurate. Current local shader_info declares per_primitive_inputs/outputs directly after the two interpolation masks, and nir_gather_info sets per_primitive_outputs for store_per_primitive_output. No raw original offset is used in production. The old pure layout fixture counts remain historical and do not validate this newly connected FS-demand policy.

No build/tests/probes/sanitizers/device/ZIP/install; only git diff --check. TASK/MESH guard and feature advertisement unchanged. Next source work remains M4/M5/M6 and the open equivalence cases from M1/M2/M3 in the registry; final validation stays V1.


## Build/install checkpoint0e3f1d29 and conditional packed-index writes — 2026-10-18

At the user's request checkpoint0e3f1d29e475 was built on Android SDK33 and installed to the phone Download/GameHub slot. Driver SHA062d46a7ef42149a8488229fa96ced1e9f44265666e3e89d3c11b4cf62ff12ec; ZIP SHA0abe8049470a94fa1a80f3969bbb0b9be65817cb31c7ae077758ca006569600d. A830 ordinary graphics cold/warm/serialized-cache pipelines and GPU triangle/transfer/readback4096pixels passed. Compiler/sanitizer suites were not rerun; no game/performance or TASK/MESH execution claim. Backup oldSO: /sdcard/Download/gamehub-before-gs-preliminary-0e3f1d29-20261018-195210.so. Source below is a subsequent unbuilt transfer, not included in that installation.

Recovered original lower_mesh_intrinsic decomp502..517: packed-index store access is4 iff layout flags+0x4d/+0x4e/+0x4f/+0x51 are set, otherwise0. They map to has_layer/synthesize_layer/has_viewport/has_shading_rate; static offset assertions added. The store wrapper decomp49..52 forwards indices+0x0c to the intrinsic access field; ACCESS_VOLATILE is bit2. This policy concerns only the packed global index store. Flag reads and other metadata accesses retain their existing flags. Cull bitmap is not in the predicate.

Added tu_mesh_indices_require_volatile(layout) and compiler entry tu_nir_lower_mesh_aqe_recovered deriving the access policy after layout construction. The earlier explicit-bool helper remains as a probe override, through the same implementation. Replaced the compiler's conservative always-volatile=true staging choice with the recovered predicate. No second layout construction, duplicated lowering or extra allocator. This closes the source-recovery part of that predicate in M5; it does not close shared-count/index-read semantics or publication ordering validation. This path remains below the existing TASK/MESH rejection guard and no feature advertisement changed.

No rebuild/tests/probes/sanitizers/device changes after this transfer; only git diff --check. Final source-vs-original compiler/packet/GPU verification remains V1.


## Count wrapper reachability and constant entry publication — source only (2026-10-18)

Traced original tu_nir_lower_mesh and lower_mesh_intrinsic through decompilation and static disassembly. Original layout is memset600 at ELF0x9ad478..484. The count callback's nonconstant path sets dynamic_counts(+0x55)=1 at0x9aeb38 and counts_initialized(+0x56)=1 at0x9afd04. The wrapper tests+0x56 at0x9adc78..80 and skips fallback publication if set. Constant counts update static_vertices/static_primitives; the remaining wrapper emits their publication at shader entry. Thus the shared-count load branch guarded by dynamic_counts has no demonstrated reachable producer with counts_initialized=false in this traced entry/callback path. This is a control-flow recovery finding, not an execution test or proof about untraced aliases/call paths. Do not implement an inferred NV ABI merely because that fallback code exists. NV support remains unconfirmed and locally rejected.

Refactored the existing local primitive-flag/cull initialization and AQE count writes into mesh_publish_counts, shared by dynamic/site and static/entry paths. The recovered production entry now hoists publication for exactly one constant SetMeshOutputs site in the original entry block, using fresh constants before the impl. It then removes the original count intrinsic, invalidates changed CFG metadata and lowers the local loop counter to SSA. The narrower entry-block restriction is a deliberate local adaptation: the original caches constant counts more broadly, but this transfer does not speculate about conditionally executed constant sites. Other sites retain the existing in-place lowering. The explicit-bool probe entry keeps its previous site behavior. No second publication is inserted on either branch.

Both paths retain the local conservative execution+memory barrier after invocation0 initialization for multi-invocation workgroups. This barrier is not claimed as original packet/code equivalence or a performance gain; final hardware ordering validation may establish whether it can be narrowed. counts_initialized in the local resulting layout marks emitted publication, including the hoisted path; no GPU consumer of that layout byte has been identified locally. Preserve that distinction when comparing600-byte fixture layouts. The compiler TASK/MESH rejection and extension policy stay unchanged.

M5 registry now distinguishes the source-connected static/dynamic publication paths from unsupported index reads and final ordering validation. The shared fallback is classified as no reachable producer demonstrated in the traced path, not a confirmed missing feature. Original evidence: mesh-count-wrapper-original-disasm.txt. No build/compiler/tests/probes/sanitizers/device/ZIP/install this batch; only git diff --check. Last built/installed remains0e3f1d29e475.


## Continuous source transfer: bind/raster/query/cache and indexed diagnostics — source only (2026-10-18)

Continued the requested uninterrupted half-hour source pass from15:12:58 UTC; this section records transfers during it, not a build or a completed audit. The TASK/MESH compilation guard and feature advertisement remain unchanged. Last built and installed stays0e3f1d29e475.

M3 command-state integration: original CmdBindPipeline copies TASK and MESH pointers at command-buffer+0x1960/+0x1968 from pipeline+0x280/+0x288, including null for legacy pipelines. These assignments were missing locally despite descriptor/constants/dispatch consumers already using the array. Added them, plus local dirty-state adaptation for changes of the mesh stage and protection against eliding a graphics program when its mesh/legacy execution mask changes. Both original pipeline-bind and draw_common use group2 word0x02610000 for MESH versus0x02110000 for legacy binning VS: GMEM|SYSMEM instead of BINNING, with DIRTY retained. A shared internal emitter now selects the appropriate mask at both sites. The independent mesh-begin packet already used the mesh mask. No global change to other callers. Dynamic vertex fetch now skips absent VS and retains the prior state for the mesh begin/end disable/restore pair; later legacy VS binding dirties it again. This is a null-safe local adaptation to common draw-state code, not a claim of byte-identical original branching.

M3 mesh point raster/blend: original shader+0x26c corresponds to600-byte mesh layout+0x44 (vertices_per_primitive), +0x25c/+0x260 to layout+0x34/+0x38 (max_vertices/max_primitives). Added layout assertions. Point mesh chooses POLYMODE6_POINTS. For point mesh with max_primitives<=max_vertices, original disables per-MRT color/alpha blending bits; global destination-read/blend masks remain unchanged. Added both static and dynamic source paths. Mesh stage changes dirty raster and blend. A partial GPL blend state is deferred when mesh is enabled and the full shader interface is not known, avoiding caching an FS-only decision for an unknown producer. The underlying reason and full rendering correctness of this original policy still require final validation; no performance gain is asserted.

M1 setup-prefix integration: both the setup prefix and final VPC now validate a tagged physical FS map against the selected layout using one named-field comparison helper. The linked uploader supplies the actual FS immutable options. For physical inputs, setup uses the compiler primitive base and includes leading sparse holes in the primitive span, matching the prior physical-VPC adaptation. Untagged detached inputs retain the recovered legacy placement. This resolves another local prefix/tail inconsistency; original emission-time versus local late physical remapping equivalence, PrimitiveID-only cases and full range validation remain open.

M6 query source transfer: original pipeline-stat slot is0x158 bytes, with14 uint64 counters each for results/begin/end. Original GetQueryPoolResults maps TASK bit0x800 to byte offset0x58 and MESH bit0x1000 to0x60 within results (indices11/12); XML names the registers ASINVOCATIONS/MSINVOCATIONS. Original primitive-counter mask0x1b7f includes both new stage bits. Added the mapping, mask and storage. A6/A7 retain eleven valid register reads. Local adaptation also retains eleven-counter work for ordinary A8 requests and uses fourteen only when TASK/MESH stats are requested; this differs from original always-fourteen snapshots and needs final query validation. Added MESH_PRIMITIVES_GENERATED to create/reset/result-count/host-read/GPU-copy/begin/end routes and nested-query stop accounting. Original groups query types0x3b9fcb40 (mesh) and0x3ba09e30 (legacy) into the same primitive-query path; original dataELF0xaf6b8 is[0x703e8003,0x4008038a], proving CINVOCATIONS0x38a, not a guessed MSPRIMITIVES substitution. Existing renderpass/multiview/availability logic is shared. Feature exposure remains disabled. Original start_draw still passes only the five legacy hashes; missing TS/MS trace fields alone are not a missing original feature.

M2 local cache correction: a partial NIR cache hit was rejected before compilation, but add_object could deduplicate its complete replacement back to the old incomplete object. Retain a private reference to the complete candidate until checking returned stage presence/identity. If dedup returns an incomplete object, release that returned reference and keep the valid private candidate for library ownership. Never mutate NIR borrowed by other libraries/threads. A bad published entry is not evicted through private runtime internals, so later lookups may still recompile; cache recovery/eviction and concurrency remain final-test cases. Cache-hit checks now cover all actually required retained stages, including full LTO and primitive FS. Deserialization rejects a null/wrong-stage snapshot or detected overrun and releases prior snapshots; this is not a general hardened NIR parser. The NIR record format itself is unchanged; the later C5 compiler metadata change bumps the stage schema as described below.

C5 indexed draw diagnostic recovery: original env strings are TU_DIAGNOSTIC_SPLIT_INDEXED and TU_DIAGNOSTIC_INDEXED_CHUNK; chunk clamps3..0xc000 and rounds down to a multiple of3. Default chunk0x600, or0x1800 with the original application default. Original default originates from drirc option tu_split_indexed_draws (instance drirc base0x2d8 + option0x4d =0x325), gated for default purposes by device property has_indexed_draw_split_quirk (original props+0x262). The diagnostic env override is evaluated on A8 even when that property is false. Recovered the hardware and application defaults as well: all79 fd_dev_recs records were inspected statically; only the three A830/A830v1 records71..73 have props+0x262=1. Added the named property only to the matching A830 local GPUInfo. The original two-device driconf table (one global and one turnip), including all engine/application options, has exactly one tu_split_indexed_draws override: Cyberpunk 2077, application_name_match=^[Cc][Yy][Bb][Ee][Rr][Pp][Uu][Nn][Kk]2077[.][Ee][Xx][Ee]$. Added that exact Turnip config rule and a defaultfalse generated option. Device initialization combines the hardware property and drirc value, then applies the diagnostic environment override. The default chunk is0x1800 only when this application+hardware default is true, otherwise0x600. Guard: one instance, count>chunk, TRIANGLE_LIST, no primitive restart, no TCS/GS, existing VS with has_no_side_effects and no stream output, FS absent or not reads_primid. Original VS+0x5ee is has_no_side_effects (compiler derives !writes_memory), +0x6f4 is stream_output.num_outputs, FS+0x560 is reads_primid (VARYING_SLOT_PRIMITIVE_ID input). Each chunk advances firstIndex, preserves vertexOffset/firstInstance and index buffer range, emits CP_WAIT_FOR_IDLE including after the last chunk, and uses one logical draw trace/common setup. No indirect/multidraw behavior is inferred. It is not enabled as a general throughput optimization. The local IR3 compiler formerly initialized has_no_side_effects only for FS, which would leave the restored VS guard permanently false. Moved its !writes_memory assignment outside the FS-only block, as in the original. Turnip stage-cache schema bumped0x47533803→0x47533804 to prevent old VS metadata from satisfying a hit even with TU_BUILD_ID_OVERRIDE. IR3 disk cache independently uses the actual ELF build ID. Old pipeline cache entries may therefore recompile; this is not a measured performance improvement. The separate use-indirect-draw hardware path remains outside this split transfer. Hardware/config evidence: indexed-split-original-hardware.json and indexed-split-original-config.json.

M7/C10 advertised-feature scope: recovered original vk_device_extensions at ELF0x8ab7a8,431 VkExtensionProperties records of260 bytes; supported table base is physical_device+0x58 (vk_physical_device_init ELF0xa56b34 copies431 bytes there). vk_common_EnumerateDeviceExtensionProperties ELF0xa56c1c requires the supported byte AND Android allowlist byte both equal1. NV_mesh_shader is index376, physical+0x1d0: the regular TU initializer explicitly zeroes eight bytes at+0x1cf. AMDX_dense_geometry_format/enqueue are indices270/271 at+0x166/+0x167: store32(+0x165,1) sets only+0x165 and clears both. NV cooperative matrix/matrix2/decode/vector/CUDA are indices343/344/345/346/350 at+0x1af..0x1b6, explicitly zeroed by store64(+0x1af,0). The Android allowlist is1 for these names, so the driver's false bytes are decisive. No later setter of these bytes appears in the regular initializer. NV task/mesh is therefore excluded from advertised-feature parity obligations for this binary; rejecting NV locally remains appropriate. Internal lowering/stubs are not demonstrated advertised capabilities. Listed AMDX/NV families are likewise excluded; other symbol-only entries still need individual classification. This is static recovery of regular initialization/enumeration, not a runtime probe or a claim about modified/future binaries. Evidence: original-unadvertised-features-evidence.json plus initializer/enumerator disassembly artifacts.

M5 index-read scope follow-up: local SPIR-V frontend vtn_variables.c:1435..1442 explicitly documents EXT_mesh_shader primitive point/line/triangle indices as a write-only array of vectors, versus NV read/write flat arrays. The original lower_mesh_intrinsic routes the primitive-indices semantic to the packed write sequence; it does not demonstrate a valid EXT read capability. Together with the original NV advertisement=false, a new primitive-index read/unpack implementation is not a missing advertised capability for this package. Keep the recovered entry rejecting such reads before mutation. Updated comments and remaining-work criterion accordingly; count publication ordering and static/dynamic integration still require final validation. No new read backend or NV ABI was invented.

Only source reads, static binary inspection, editing and git diff --check in this pass. No compiler, build, test/probe, sanitizer, GPU/device access or ZIP. Binary/metadata/deployment records remain tied to the last installed checkpoint. Evidence excerpts: continuous-pass-original-evidence.txt. Full integrated validation remains V1; the package is still incomplete.


## AQE constant prefix and mesh multiview FS — source only (2026-10-18)

Original `tu_shader_create` ELF0xa094a0..0xa094f4 zeroes the allocation record
at x29-0xf0 and stores9 at x29-0x20, its max-constant cursor at offset0xd0,
for stage6/task and7/mesh. This precedes push/inline/dynamic-descriptor
allocation. The absolute AQE loads at scalar slots0..33 therefore require
nine vec4s (36 scalar words) at the beginning of constant space. This is a
fixed ABI reservation, not the separately aligned IR3 driver-param allocation.

The same original branch applies to FS with shader-key byte4==1. The regular
pipeline builder constructs that byte as multiview enabled AND MS present
(NIR or stage-info). Original `tu_lower_nir` then replaces FS load_view_index
with load_const_ir3(base33) after input-attachment lowering and before ordinary
sysval-to-varying conversion. Intrinsic table confirms opcodes0x2dc and0x157.
The alternate inherited-state construction at builder decomp2582 is recorded;
no blanket change to ordinary non-mesh multiview is inferred from it.

Source transfer: named nine-vec4 prefix, bound assertion for AQE helper,
reservation before tu_lower_io for TS/MS and mesh-multiview FS, and matching
FS ViewIndex lowering before varying conversion. A transient tu_shader_info
flag carries producer context; the existing retained FS snapshot precedes the
consumer-dependent lowering, preserving later GPL relinking. Cache schema is
47533805 so build-ID overrides do not reuse the previous constant layout.

These are unbuilt source changes. TASK/MESH rejection and feature gates remain.
No compiler, test executable, build or device access in this follow-up. Full
validation, AQE context/shared-bank preservation, slot33 hardware publication
and cross-queue BO synchronization remain open. Last installed driver is
8cf7cc15750a, not this source update. No performance claim.


## Thirty-minute source pass: shader shuffle, limits and depth/device policies (2026-10-18)

This pass continues from source9bb6a9eac2a6 and the installed8cf7cc15750a.
No build, compiler/sanitizer suite, native test executable, GPU access or
installation was requested or run. Evidence below is static source/ELF recovery.

**Uniform-rotate shader optimization.** Original shuffle_rotate_offset
ELF0xa82ef8 recognizes scalar32 load_subgroup_invocation recursively through
uniform additions/subtractions and AND masks retaining every low bit of the
maximum subgroup width. Recovered op-info names confirm0x100=iadd,
0x103=iand and0x132=isub. Added the bounded8-node matcher before the existing
simple relative matcher. A matching absolute shuffle becomes a uniform rotate,
including wrapped/nested expressions; unknown forms retain the existing
waterfall. Existing uniform broadcasts and simple XOR matching remain. This
has a concrete avoided-loop mechanism but no FPS/temperature or ISA result.
Turnip stage schema is47533806, including the previous AQE prefix change.

**M6 limit table and eligibility.** Recovered all36 scalar fields of
VkPhysicalDeviceMeshShaderPropertiesEXT through original Properties2 case
0x3b9fcb41 and initializer constants. TASK:1024 invocations,16KiB payload,
32KiB shared,48KiB combined; MESH:256 invocations,28KiB shared/payload+shared,
32KiB output and48KiB payload+output. Both total counts are2^22 and each
count dimension65535. Outputs:128 components,256 vertices/primitives/layers,
6 multiview views,32 granularity; preferred invocations1024/256, preference
flagsfalse. Reporting the table does not enable the extension. The named
hardware eligibility helper and recovered defaultfalse tu_disable_mesh_shader
option now gate both AQE allocation and mesh-begin consistently. It retains the
local A8-only generation bound plus original GMEM>>21>8 condition; future
generations are not enabled by copying the original gen>=8 comparison.

**C6 unrestricted-depth policy.** The unknown device+0x110 byte is proven to be
enabled_extensions.EXT_depth_range_unrestricted: enabled table begins at0x78,
original extension index152, supported bytephysical+0xf0 is explicitly1.
A7 GRAS/RB clamp is API-clamp OR !extension. A8 keeps clamp enabled and clears
O_DEPTH_01_CLAMP_EN when unrestricted; with API clampoff its viewport interval
is[-FLT_MAX,+FLT_MAX] rather than[0,1]. Explicit user clamp ranges retain priority.
Added these state rules, leaving advertisementfalse until integrated rendering
validation. Ordinary extension-disabled behavior remains the previous path.
The separate A6/A7 sample-center polarity candidate remains held.

**C8 A850 guarded mechanisms.** Static comparison of full0x324-byte original
A840/A850 records identifies only two magic-register differences and five
named properties. Registers:0x980b changes00800280->00c00280;0x8816=1 is added.
The five properties are native U64 image UMAX, hardware VS draw params, TPL1
mode bit16, hardware array-index rounding, and CAT4-repeat quirk. Recovered121
named original property offsets from the debug-option parser for future work.
Four independent policies are now source-connected: only bindless-image U64
UMAX bypasses atomic CAS arithmetic lowering; TPL1 bit16 and array-round mode2;
no software half-bias when array rounding is native; CAT4-only repeat expansion
before legalization while debugEXPANDRPT still expands all eligible categories.
All four flags are explicitlyfalse on the existing A8 records. A850 device ID
is NOT added/enabled: the complete VS hardware draw-parameter path is still
required (compiler sysvals/binning liveness, VFD0xa00a, direct-as-indirect
packets, indirect/multidraw/bytecount handling). Do not count this as full A850
support or an A830 optimization.

**C4 maintenance10 metadata.** Recovered original feature bytephysical+0x263,
extension index55/supportedbyte0x8f and Properties2 case0x3ba466f1. All three
properties at0xaf0/af4/af8 are chip>=8: rgba4OpaqueBlackSwizzled, default sRGB
transfer function, transfer-function override support. Copied property values
without enabling the feature/extension. The Vulkan copy-command specification
(https://docs.vulkan.org/spec/latest/chapters/copies.html) confirms ENABLE requests
transfer conversion; on a compliant default-true path this is already the
default, whereas SKIP requires the existing separate override. Therefore the
absence of an ENABLE-specific toggle alone is not a missing algorithm. Full
FDM/default/override consistency and the remaining maintenance10 requirements
remain mandatory validation work; the copied properties are not proof of them.

**M4 context boundary and C7 scope.** Original mesh-end clearsVPC_UNKNOWN_CNTL
and restoresgroup29; no CS program IB restore exists in that helper. The
ordinary original dispatch repeats the program on its debug/stomp guard only.
This is evidence against inventing an unconditional restore, not proof of
hardware context preservation across every path. Local bin prefixes currently
use OFFSET packets with delayed visibility-stream patchpoints; copying original
absolute addresses into temporary CPU-built prefix buffers would need correct
GPU-address/patchpoint ownership. No speculative packet substitution was made.

Evidence: mesh-limits-and-context-source-evidence.json,
shuffle-rotate-source-evidence.json + original disassembly,
unrestricted-depth-source-evidence.json, a850-policy-source-evidence.json,
original-device-property-offsets.json and maintenance10-property-source-evidence.json.
All previously reported executable validation counts remain historical.


C2 follow-up within the timed pass: the earlier A8-wide extension restriction
was conservative, not faithful. Original supported extension physical+0x61 is
set by store64(+0x60,0x0101010101010101); Features2 case0x3ba32a88 reads
physical+0x2ae/2af, both explicitly set1 by store16(+0x2ae,0x0101). Restored
unconditional KHR_copy_memory_indirect support with feature bits following it.
The separate A8 destination-format whitelist remains unchanged. SDK33 Android
allowlist filtering remains; no build, GPU or older-device validation was run.


C9 final source follow-up in this pass: original timeline wait-many recomputes
the remaining duration from the absolute deadline on each EINTR/EAGAIN retry.
Local safe_ioctl previously retried the same relative timeout. Added a bounded
absolute-deadline wrapper for both native timeline and binary-timeline waits,
retaining existing millisecond rounding and wait-any/all semantics. Empty waits
now return success before ioctl/allocation. No other KGSL fatal-error policy
is changed; error/device-lost classification remains for the final audit.
No wait or runtime test was executed in this source pass.


## Requested build integration (2026-10-18)

The SDK33 build caught four uses of the obsolete `nir_def::parent_instr` field in the newly recovered recursive shuffle matcher. Replace them with this checkout's `nir_def_instr()` accessor. This changes API access only; the expression matching and fallback rules remain unchanged. The preceding source-only checkpoint was not build validated. Requested build/device smoke results are recorded separately in artifact metadata.


### 2026-10-18 — Предварительная регрессия 578df20e

Пользователь: «последний прогон менее плавный (фпс меньше в thief и плавность меньше в alice madness)». Сборка578df20e; предполагаемый reference — предыдущий установленный8cf7cc15 (сравнение не уточнено). Это качественный игровой отзыв без frame-time/temperature capture, не доказательство конкретной причины. Сборку578df20e не считать новой базой. Сохранены обе ZIP/SO и backup слота. Во время следующего переноса устройство не меняем. При финальной проверке изолировать обычный A830 путь: recursive shuffle matcher и A7/A8 depth state; mesh/A850 flags остаются gated.


## KGSL fatal errors and guarded A850 draw inputs (2026-10-18)

Original native timeline signal (ELF0xa1ec08), query (0xa1ece8) and wait-many (0xa1ee00) retry EINTR/EAGAIN, then mark the Vulkan device lost on fatal errors. The local native callbacks now call vk_device_set_lost; EBUSY/ETIMEDOUT waits still return VK_TIMEOUT. Native finish (ELF0xa1eb74) now ignores zero IDs and clears the payload after destroy, as the original does; initialization failures remain VK_ERROR_OUT_OF_HOST_MEMORY (original0xa1ea8c), not device-lost. The runtime's zero-count signal_many fast return already existed; no invented missing batching optimization. Legacy timestamp/syncobj fallback, imported sync-file errors and local binary timeline callback policies are separate and remain unchanged. Removed a redundant zero-count return after the binary imported-fence scan (the same check already precedes it).

The last missing A850 property has_hw_vs_draw_params is added but explicitly false in a8xx_base. No A850 device record is enabled. Recovered emit_intrinsic paths use FIRST_VERTEX (both load_base_vertex and load_first_vertex), BASE_INSTANCE and DRAW_ID as cached sysval inputs instead of driver constants. The binning VS preserves the non-binning VS's vertex/instance/first/base/draw input set, matching original keep mask0x32600. The shared shader-state sizing helper reserves its additional two dwords for regular, binning and safe-const VS variants. VFD register0xa00a packs DRAW_ID/FIRST_VERTEX/BASE_INSTANCE/unused into successive bytes; absent inputs and final byte use regid63.x (0xfc).

Recovered direct draws serialize a four-word VkDrawIndirectCommand or five-word VkDrawIndexedIndirectCommand into the command substream and execute one CP_DRAW_INDIRECT_MULTI with opcode2/4, no constant destination. Existing indirect/count paths obtain zero from vs_params_offset under the disabled property. Compact CPU multi-draw uses original opcode9/10, two/three-word {first,count[,vertexOffset]} records, common instanceCount/firstInstance, and eight/eleven-word packets. The original non-indexed compact initiator uses 32-bit index-size encoding; without tessellation both compact modes use the ISOLINES patch encoding. Common pVertexOffset overrides the per-record field. Checked overflow of drawCount times record words before allocating the substream. This is source reconstruction, not a claim that the device can execute it.

CP_DRAW_AUTO follows the original existing direct-parameter and six-word packet route; unlike normal direct draws the original has no dedicated hardware-feature branch there. The original ir3_nir_scan_driver_consts (ELF0xa7e408) explicitly counts opcodes0x135/0x136/0x172/0x17b regardless of the hardware flag. ir3_get_driver_param_info (0xa7e210), ir3_alloc_driver_params (0xa7e8e8) and ir3_setup_const_state (0xa7ddc0) retain this reservation. Keeping local conservative slots therefore matches this recovered allocation policy; there is no constant-pressure optimization to claim. Sideband/limits, hardware CP_DRAW_AUTO results, firmware and full integrated validation remain open. Stage cache schema0x47533807 separates the new compiler representation under build-ID overrides.

No compilation, compiler/API/GPU/performance tests or device changes in this pass. Last installed578df20e passed ordinary smoke but user reports lower Thief FPS and less smooth Alice: Madness Returns. The preceding8cf7cc15 is the provisional comparison reference; neither this observation nor the source analysis isolates the cause. New A850 paths remain disabled on A830, so they cannot explain the installed build's regression.


## Empty KGSL timeline submission CPU path (2026-10-18)

Recovered from GameSir kgsl_queue_submit (ELF0xa202f8): if both command and bind lists are empty, API wait_count is zero, signal_count is nonzero and every signal uses the native KGSL timeline sync type, the original polls the previous queue timestamp fence with timeout0. If absent or complete it calls kgsl_signal_timelines_cpu (ELF0xa2146c, branch0xa20e20) instead of submitting a GPU auxiliary command. A timeout keeps the GPU path; a fatal completion-check/signal error is returned. The all-native/nonempty guard is visible at0xa20bc4; the zero-wait branch reaches it from0xa20a54. This is not a CPU shortcut for outstanding API waits or binary/imported-fence payloads.

The local stage66 source already had native timelines and GPU aux submissions, but only a comment proposing the CPU fast path. The current branch history has no kgsl_signal_timelines_cpu implementation to restore verbatim. The source port implements the original narrow eligibility policy on the existing backend: poll its private queue timeline's previous point with timeout0; only after success CPU-signal all requested native timeline points plus the new private queue point in one ioctl. Publish queue->kgsl_queue_timeline_seqno only after successful signaling. This explicitly adapts original timestamp-based queue ordering to local private-timeline ordering; it is not a byte-identical original backend. EBUSY/ETIMEDOUT keeps the existing GPU wait/signal path, fatal errors set device-lost, and binary/imported waits, API waits, commands, bind work and empty signal lists retain their previous routes. No CPU blocking wait is introduced. Saved u_trace/profiling behavior remains the existing empty-submission policy.

This can avoid a synchronization-only GPU aux job and GPU wake-up for eligible empty submissions; it adds a nonblocking completion ioctl, so CPU cost and real game frequency must be measured. No FPS, temperature or frame-time improvement is claimed. Compiler/build/API/GPU/game tests are deferred; only source whitespace checks and original ELF/decompilation reads were used. Last built/installed578df20e is unchanged, with its qualitative Thief/Alice regression still unisolated.

A850 source clarification: original tu6_emit_vs<A8> computes PC_VS_INPUT_CNTL.SIDEBAND_CNT only from VERTEX_ID(slot9), INSTANCE_ID(slot10) and VIEW_INDEX(slot0x38), each regid !=0xfc. FIRST_VERTEX/BASE_INSTANCE/DRAW_ID go to the separate0xa00a register and are not added to the sideband count. The current local sideband count already matches; do not widen it just because three new compiler sysvals were recovered. No sideband source edit was needed. Full input allocation, firmware and rendering validation remain open. Original instruction excerpts saved in kgsl-empty-submit-and-sideband-original.asm.


## Requested build integration for empty-submit checkpoint (2026-10-18)

The SDK33 integration build found that this register generator emits the VFD_DRAW_PARAMS packing function only through the C++ variant dispatcher. Use VFD_DRAW_PARAMS(CHIP, ...) rather than the generated A8XX macro whose pack_A8XX symbol is absent. Register fields and feature gating are unchanged. Build and device validation results are saved in the artifact report.


## Selected VSC addresses in the inlined bin prefix (2026-10-18)

Correction to the earlier C7 description: the original standalone tu6_emit_tile_select<A8> (ELF0x972efc) emits CP_SET_BIN_DATA5_OFFSET (opcode0x2e) and three per-pipe offsets, matching the existing local path. The original inlined CP_START_BIN prefix in tu_cmd_render_tiles<A8> (ELF0x96d14c) uses a different sequence: CP_SET_PSEUDO_REG (0x56, payload9) supplies pseudo IDs8/9/10 with this pipe's draw/size/primitive addresses, then CP_SET_UNK_BIN_DATA (0x2d, payload7). That is not a direct replacement with CP_SET_BIN_DATA5 (0x2f).

ELF constants0xaffb0/0xaf920 decode to {0x70d68009,8} and {0,9}; original patchpoint structures carry renderpass index, CPU pointer, GPU IOVA and buffer offset, like local tu_vis_stream_patchpoint. The three offsets are draw_base+pipe*draw_pitch, size_base+pipe*4, prim_base+pipe*prim_pitch. The 0x2d payload is control(pipe_size|slot<<22|ABS_MASK), absolute tile mask, and FIVE reserved zero dwords. It is seven payload words total, not three trailing zero qwords. Instructions0x96ed10..0x96ed4c prove the header, zero writes and32-byte pointer advance. Unknown pseudo IDs11–15 are a separate question; their earlier raw values are retained, their interpretation is not newly established.

The source adds a local emitter and relative scratch-fixup records. Measuring and staging prefixes never publish fake GPU IOVAs or pointers into temporary CPU memory. Only after copying the complete prefix array into an owned substream are its three per-tile patchpoints registered with final mapped CPU pointers and GPU addresses. The existing queue patcher then handles selected visibility BOs/renderpass offsets and CPU versus GPU patching. For SIMULTANEOUS_USE the prefix allocation is writable; the previous substream writeability is restored after allocation/emission. Allocation or prefix-size fallback frees temporary metadata and publishes no patchpoints. This reuses the existing secondary replay and visibility submit machinery; it does not establish their full correctness without runtime validation.

The variant is deliberately OFF by default, selected only by TU_GAMESIR_BIN_DATA=true on A8 with absolute-mask support and the existing bin-loop eligibility guards. Ordinary tile-select remains OFFSET. This is an unbuilt source recovery candidate, not an activated FPS/temperature improvement. Need command-layout checks and coordinated normal/simultaneous/secondary/preemption/multi-renderpass execution before making it a default or declaring C7 complete. No build, compiler/API/GPU/game tests or device work in this pass. Last installed4bc6bcd3 (ordinary rendering and empty-timeline queue-order smoke passed) remains unchanged.


## Sample-center diagnostic source connection and finite handoff (2026-10-18)

Original lowerer at ELF0xa8c4c8 and ALU-source recognizer at ELF0xa8cdfc use intrinsic600 (load_sample_pos_from_id_ir3), ALU0x2b (ball_fequal2), and select_barycentric_at_center_ir3 sources predicate/computed/pixel. The original SEL_B32 constructor at ELF0xabdfb8 preserves computed/condition/pixel source order; ordinary bcsel uses the same ordering. This does not resolve the surprising predicate polarity. The existing backend is retained, without claiming an original bug or an optimization.

The caller is now connected only behind IR3_GAMESIR_SAMPLE_CENTER=false and gen<8. The ALU-only recognizer is locally bounded at depth64 and1024 visits; an incomplete match falls back to ordinary derivative lowering. This cap deliberately differs from the unbounded original recursion. Both the IR3 disk key and TU graphics shader-cache key include the diagnostic setting because this lowering occurs after the IR3 cache lookup; TU schema advances to0x47533808. Treat the process environment as immutable during compilation. The old sample-center-activation-unvalidated.patch is historical and superseded by this guarded source variant; do not apply it on top.

No build, compiler probe or device execution was performed for this follow-up. Installed c7bf6afa remains unchanged. GAMESIR_REMAINING.md now separates confirmed missing device data, unresolved source comparisons, existing source awaiting validation, excluded unadvertised families, and the final coordinated validation. It is a finite handoff list, not a completeness certificate or ETA.


## Emission-time mesh inputs, per-vertex metadata, A850 and advertised feature inventory — source only (2026-10-19)

Mesh FS placement now follows original emit_intrinsic (ELF0xab8abc): assign physical input_base before BARY/FLAT/LDLV creation, mark PrimitiveID per-primitive when primitive_base is nonzero, and retain its physical input record. Removed the mesh-only post-RA pack_inlocs adaptation; original compiler calls that pass only for gen<6. Original create_frag_input (ELF0xac4998) uses coordinate-backed FLAT_B for mesh flat inputs, source2 wrmask3, before ordinary flat_bypass. Legacy packing rewrites source2 only when it is immediate.

The original compiler also runs fixup_per_vertex_input_inlocs (ELF0xaae20c) on mesh consumers. That call is restored. AQE setup scans the compact per-vertex records, while VPC remaps a borrowed copy through the producer layout, as in tu_emit_program_state (ELF0x9e9070 decomp2552..2655). The new fs_inputs_repacked planning flag separates those records from already-physical non-per-vertex FS metadata. Compiled-map consistency remains checked in both cases. Sparse primitive ranges, default inputs, prefetch coordinates and complete LD(L)V/VPC hardware semantics remain for the coordinated validation; source equivalence is not certified. TU schema0x47533809 separates changed compiled/cache behavior.

Added the recovered A850 record76 (chip0xffff440514ff, infoELF0x897408) using A840 geometry/CCU parameters, the five recovered properties, PC_UNKNOWN_980B=0x00c00280 and RB_RESOLVE_CR_CNTL(0x8816)=1. Its raw-register list is copied rather than mutating the A8 base. A830/A840 keep the new flagsfalse. The original DRAW_AUTO path (ELF0x9514c8) has no special HW-property packet branch: tu6_emit_vs_params (ELF0x982ca0) still writes VFD offsets and skips only constants. Existing local code already matches that boundary. Original input count reject>43 matches local44-element capacity and <44 guard. This does not validate A850 hardware execution.

Recovered a bounded advertised-extension inventory from all431 original extension-byte stores plus Android allow bytes:143 literal enabled,238 disabled,50 symbolic/conditional. Symbolic RHS and local initializer presence do not establish runtime support. The parser records literal provenance and overlapping stores; the TU memset at+0x205 also clears the final two extension bytes. Platform/SDK filters and runtime enumeration are deferred.

ASTC decode mode was an actual missing advertised path: the original A8 image-view pNext selects FP16 for R16G16B16A16_SFLOAT, UNORM8 for R8G8B8A8_UNORM, and default FP32 otherwise. Original fdl6_view_init (ELF0xc73618) sets descriptor word5 bit26/28 respectively. Added named XML bits, an appended zero-default FDL argument, Vulkan image-view routing and A8-only extension advertisement. Shared-exponent decoding remainsfalse, matching original feature byte+0x2a4=0. Unrequested decode precision leaves descriptors unchanged.

Added original unconditional EXT_frame_boundary/feature metadata (supported+0x107 and feature+0x3ba) and EXT_pipeline_robustness advertisement (supported+0x132); robustness feature/property/per-pipeline compiler support already existed locally. These metadata changes are unbuilt and unvalidated. Three original A8 shader-family decisions remain: maximal reconvergence, quad control, and fragment interlock. Generic SPIR-V/NIR quad helper propagation and IR3 SP/RB controls already exist, but the full API/compiler contract is not established. Interlock begin/end no-ISA cases (original opcodes0x32/0x97, emit_intrinsic ELF0xab5d58) are source-transferred; raster-order/memory/sample semantics remain open and the extension is unadvertised.

No build, compiler probe, test or device call in this source pass. TASK/MESH compilation rejection and the earlier sample-center/bin-prefix diagnostic defaults remain. Installed build c7bf6afa, SO/meta/ZIP and deployment.json remain unchanged. All new source awaits the final coordinated build/compiler/API/cache/GPU/game/temperature validation.


## A8 FDM shader-store route and platform-hidden metadata — source only (2026-10-19)

Original A8 tu_store_gmem_attachment (ELF0x90d218/0x90d7e4) selects shader stores for non-integer color and extended modes without excluding FDM. Its store_3d_blit helper (ELF0x931540) uses ordinary r3d coordinates and the same offset-aware GMEM descriptor; no extra shader-store coordinate patchpoint was found there. The original sysmem selector (ELF0x963c18) contains no local FDM/extended-mode forced-sysmem override. Removed those local FDM-only overrides and the shader-store non-FDM assertion; pre-A8 fallback remains. This is source route recovery, not validated FDM rendering or a performance result. The separate in-place GMEM-to-GMEM FDM resolve route is not activated or claimed reconstructed by this change.

The original431-entry inventory distinguishes original TU support from Android enumeration policy. maintenance10 and untyped pointers have supported byte1 but Android allow byte0. This is different from the NV/AMDX families whose supported bytes are0. Untyped pointer type/variable/access-chain lowering already exists in the shared SPIR-V frontend; transferred the missing KHR_shader_untyped_pointers and shaderUntypedPointers metadata, matching original feature byte+0x428=1 (sType0x3ba0b1b8). This is not a claim that SDK33 advertises the extension or that its full IR3 execution has been validated.

No compilation, tests/probes or device work. Current installed c7bf6afa is unchanged. Full coordinated compiler/API/cache/GPU/FDM/game validation remains at the end. The earlier source-policy probe expecting !has_fdm is historical and is not evidence about the recovered original route.


## A8 interlock raster-order chain and bounded source decisions — source only (2026-10-19)

No build, compiler probe, test, device operation or performance measurement in this source batch. Full validation stays deferred.

- Recovered the missing interlock metadata chain: original `spirv_to_nir` stores six execution-mode bits16..21 at NIR+0x1a0; `tu_shader_create` (ELF0xa09304) stores their OR at shader+0x22a. Added the two missing shading-rate interlock execution modes and frontend capability, then persisted `fs.uses_interlock` through the existing whole-FS serialized payload.
- Original A8 `CreateGraphicsPipelines` ELF0x9cce1c selects GRAS_SC_CNTL raw8 (FLUSH_PER_OVERLAP_AND_OVERWRITE) for interlock in GMEM, raw24 for ordinary attachment raster order, zero for neither. Sysmem static/dynamic predicates also include shader+0x22a and latch single-primitive mode. Connected both policies; retained the original coherent-UBWC gating of sysmem state. Begin/end no-ISA compiler cases were already transferred. TU graphics cache schema is now0x4753380a.
- EXT_fragment_shader_interlock remains unadvertised until the integrated validation. This is recovered source, not proof of sample/shading-rate behavior, memory ordering, cache/GPL behavior or Vulkan conformance.
- Quad control's frontend execution modes, subgroup/full-quad gather policy, IR3 need_pixlod/need_full_quad and SP/RB state already exist locally and match the inspected original anchors. No duplicate implementation was added. Maximal-reconvergence interval-tree/worklist/branchstack algorithm also matches the inspected original body. Their activation/contracts still need coordinated validation; an extension bit alone does not prove semantics.
- Corrected the residual FDM scope: the original normal A8 subpass resolve path (tu6_emit_gmem_stores ELF0x97123c decomp91..132) stores to system memory, emits CLEAN_BLIT, and reloads if the destination must remain in GMEM. It has no direct-resolve branch in this inspected path. The local opt-in GMEM_RESOLVE_PROTOTYPE is our older experimental path and remains disabled by default, including its FDM rejection. Reconstructing an in-place FDM resolve is not justified as a missing GameSir feature. Actual FDM format/store/load behavior remains for final validation.

Evidence: a8-interlock-quad-resolve-source-transfer.json and interlock-quad-resolve-original-excerpts.txt. Only source bookkeeping/whitespace checks; last installed c7bf6afa is unchanged.


## Mesh empty-output publication and source-boundary closure — source only (2026-10-19)

- Transferred the original zero-output publication path for a mesh shader with no SetMeshOutputs site. Original layout memset leaves static vertex/primitive counts zero, and the wrapper publishes them at entry. The local adapter no longer rejects this case; selected and detached paths both use the existing metadata/count helper with fresh zero constants.
- Bounded the shared-count fallback question to the actual original EXT wrapper/callback chain: dynamic_counts is set only by the dynamic count branch, which also unconditionally sets counts_initialized. The wrapper exits on initialized before its shared-load branch. Constant/no-count cases remain static. Thus that branch has no producer in this chain; no NV/shared ABI is invented. NV remains unadvertised in the original.
- Original indexed indirect/count entrypoints ELF0x94fe84/0x950df0 emit ordinary CP_DRAW_INDIRECT_MULTI payload9/11, subopcodes4/7; WFM and hardware-VS-constant suppression already match locally. No separate direct-style splitting is missing from these entrypoints.
- Original prefetch uses logical NIR coordinates and device eligibility without a mesh-only remap/gate, matching the local source boundary. Sparse/packed/pervertex/prefetch-only behavior is a final validation case, not grounds to invent a new original optimization.
- Sparse physical FS inputs still have an explicit local adaptation: immutable compiler primitive_base avoids applying offsets twice. Original copied-FS remap scans the first live primitive consumer. The exact sparse equivalence and hardware behavior are not proved; preserve this difference for final validation. Corrected the comment about missing outputs: unwritten varyings are not promised defined default values.

No build/tests/probes/device work. Evidence: mesh-empty-indirect-prefetch-source-transfer.json and mesh-empty-indirect-prefetch-original-excerpts.txt. Last installed c7bf6afa unchanged.


## Finite activation handoff and feature-store inventory — source only (2026-10-19)

This batch preserves source-recovery decisions and a finite final-validation handoff. It does not enable unvalidated extensions, build the driver or claim a full feature/API audit.

- Added `recover_feature_getter_inventory.py`:327 scalar getter stores (including simple byte temporaries/common tails) mapped to registry field names, with original initializer-store provenance and explicit NV alias renames. Complex/SIMD/core getter forms are outside this inventory. Symbolic RHS and conditional literals are not treated as executed support policy. The mapped original explicit-store/local-absence cases are the known barycentric, quad/reconvergence, three interlock fields, meshShaderQueries and maintenance10; no new implementation family was inferred.
- Recorded static original/upstream reconvergence body listings (428/431 instructions). The diff retains structure offsets/register allocation and normalizes direct addresses only. It is evidence of the comparison, not a semantic-equivalence proof. The inspected interval-tree/worklist/physical-edge/branchstack algorithm is already present; no duplicate algorithm patch.
- Original `tu_emit_vsc<A8>` ELF0x9716d0 constant tail decodes to pseudo-register pairs11→0,12→0,13→1,14→4,15→0, matching existing local code. Unknown firmware names/side effects are not missing source instructions. Packet/concurrency/preemption behavior still needs final validation.
- maintenance10's shared `CmdEndRendering2KHR/EXT` alias is already defined in the local registry and dispatched through existing TU/runtime code. The original normal A8 subpass path remains store/flush/load. No speculative in-place FDM implementation added.
- Narrowly checked the public resolve-mode contract against Khronos documentation to avoid reproducing invalid-input behavior: explicit color resolves require AVERAGE for noninteger formats or SAMPLE_ZERO for integer formats; a referenced depth/stencil aspect cannot use NONE. SKIP/ENABLE transfer controls require AVERAGE and transfer-control support. Thus the original's modeNONE fallback is not a required valid-command route, and no patch was added for it. This check does not establish complete maintenance10 conformance. Sources: [VkResolveImageInfo2](https://docs.vulkan.org/refpages/latest/refpages/source/VkResolveImageInfo2.html), [VkResolveImageModeInfoKHR](https://docs.vulkan.org/refpages/latest/refpages/source/VkResolveImageModeInfoKHR.html).

`source-to-activation-handoff.json` lists seven concrete activation groups and their behavior checks. Historical broad labels in this journal do not mean every item needs another new implementation. Sparse physical mesh-base handling remains an explicit local adaptation for the final comparison; AQE slot33/firmware publication remains a hardware validation boundary. Final validation is deferred, last installed c7bf6afa unchanged.

Source-comment handoff correction: mesh view packets now name draw_index/view_index consistently with the recovered userspace ABI; headers describe the connected typed caller and VPC splice instead of obsolete future-caller TODOs. Firmware execution remains unvalidated. No packet encoding changed by this naming/comment cleanup.


## 2026-10-19 — Cooperative policy and getter source handoff

Restored original cooperativeMatrixRobustBufferAccess eligibility with the already-present scalar matrix memory path. Added an explicit source comment for the local dynamic primitive-key adaptation. Full API/robustness/FP/GPU validation is deferred. Static getter recovery now maps660 stores across274 selector values plus55 core fields; predicate/platform evaluation remains separate. Installed a69bd574 is unchanged and has NOT been tested by the user; prior smoothness feedback was retracted.


### Additional recovered compatibility options

Transferred the two remaining original drirc options: Wave32 capability compatibility and Uncharted legacy A6/A7 policy. Both defaultfalse as in the original. Preserved the original three process/application rules, native single-wave execution, A7 SSBO-range eligibility and A8 exclusion from the Uncharted legacy changes. No build or API/GPU/game validation in this pass.


The full23-profile embedded Turnip driconf data is now statically reconciled: six newly missing compatibility/mesh-disable rules transferred, original match attributes and option values preserved; additional local profiles retained. No runtime matcher or GPU check performed.


## 2026-10-19 — Device-profile source transfer (unbuilt)

Прочитаны все79 записей `fd_dev_recs` (ELF0xd5d2f0, stride32),121 ранее восстановленное именованное поле и массивы64 пар `{reg,value}` для общих профилей A6+. Сопоставление использует декларативные Python-профили текущей ветки, не исполнение оригинального драйвера. Все исходные raw-register пары общих A6+ присутствуют с теми же значениями; у A8 совпадает и порядок. Дополнительные локальные RB_RBP_CNTL/RB_DBG_ECO_CNTL на A6/A7 не удалены.

Найден и перенесён реальный пропуск: `fdm_requires_full_density` (info+0x309) включён в оригинальных A8-профилях. Оригинальный `tu_calc_frag_area` при нём заполняет fragment area значениями1×1 вместо чтения плотности FDM. Добавлено поле и такой же потребитель в `tu_tile_config.cc`. Это отдельно от ранее перенесённого GMEM store/resolve; не доказательство in-place FDM resolve и не измеренное ускорение. Два соседних флага0x30a/0x30b уже реализованы локальными gen8-условиями MSAA в `tu_cmd_buffer.cc`.

Политика `has_64b_image_atomics`: оригинал включает её начиная с A7 gen2/gen3 и у производных A8. Локальный False происходит из прежней AK-правки a801126b02ce (gralloc/atomics эксперимент), а не из отсутствия backend. В предварительной ветке полного переноса восстановлен True; существующие EXT_shader_image_atomic_int64, R64_UINT/SINT format features и IR3 lowering уже присутствуют. Это осознанное отличие от AK-базы для пакета всех возможностей, а не подтверждённое исправление или выигрыш FPS. Полная проверка атомарных операций/форматов/дескрипторов/GPU остаётся на конец.

Две «недостающие» A702-записи оригинала содержат старые ошибочные chip ID. Mesa-коммит ffe0b7f4567e3cbf93c29eacb7d98bb9454c74c7 исправил переставленные цифры; текущие исправленные ID сохранены. Старые опечатки не возвращаем. После переноса FDM и atomics не осталось различий значений среди121 сопоставленного поля на общих A6+ профилях, кроме двух уже заменённых gen8-предикатами полей (это ограниченная таблицей сверка, не полная валидация драйвера).

Доказательства: `outputs/gamesir-preliminary/device-profile-static-comparison.json`, `device-profile-static-summary.json`; воспроизведение: `work/gamesir-audit/recover_device_profile_comparison.py`. Новые исходники пока не собраны, на телефоне остаётся a69bd574, **пользователь ещё не тестировал её**.


## 2026-10-19 — Бинарные shader overrides и Wine cache path (source-only)

Найдены и перенесены ещё два отсутствовавших механизма Gamesir. Наличие штатного `.asm` override само по себе не покрывало изменения в `compile_variant` оригинала (ELF0xaa99f0,size2488). Теперь при существующем `IR3_SHADER_OVERRIDE_PATH` до подмены сохраняются `<hash>.orig.bin`, `.orig.asm` и `.meta` (blake3,stage,size,double_threadsize), существующие файлы сохраняются. Сначала читается `<hash>.bin` точного исходного размера; если его нет — прежний `.asm`. Неверный размер/неполное чтение завершают debug-процесс с ошибкой, как у оригинала. Binary replacement не меняет метаданные варианта и исходный hash. Обычный путь без переменной не создаёт файлов. Allocation/seek ошибки обработаны явно. Это диагностическая возможность, не заявленное ускорение игр. Наличие TU/application cache может исключить саму компиляцию; обход уже кешированного pipeline не обещается.

В `disk_cache_generate_cache_dir` (ELF0xb27e94) оригинал переводит Windows drive-absolute пути **только** из явно заданных MESA_SHADER_CACHE_DIR/MESA_GLSL_CACHE_DIR: при непустом WINEPREFIX — в `<prefix>/drive_<lowercase drive>/<rest>`. Без WINEPREFIX распознаёт точный `C:\Program Files (x86)\Steam\` и восстанавливает Steam root по первому `/steamapps/` в cwd. После удачного перевода backslash заменяется на slash. Перенесён такой же helper; XDG/HOME/default и непризнанные пути сохраняют прежнюю политику. Не создавались cache-файлы, не запускались probes, сборки или GPU. Польза для реальных игр и корректность всех container/path/cache случаев пока не проверены.

Статический разбор свойств Vulkan добавил567 прямых/packed copy-записей с ABI-картой129 structs. Это ограниченная инвентаризация, **не** исполненная таблица свойств: shared-tail goto, CONCAT, динамические массивы и core-блок здесь ещё не покрыты. Среди полей таких structs, связанных с исходно неотключёнными extensions, не найден новый отсутствующий payload: `triStripVertexOrderIndependentOfProvokingVertex` оригинал задаёт0 (локально исходный zero default), UUID пишется существующим memcpy. Не переносить возможности NV/AMDX по одним getter symbols.

Evidence: `shader-binary-override-source-evidence.json`, `wine-cache-path-source-evidence.json`, `original-property-getter-inventory.json`; scripts `record_device_debug_cache_source.py`, `recover_property_getter_inventory.py`. Весь новый source не собран; на телефоне всё ещё a69bd574, пользователь её пока НЕ проверял.


## 2026-10-19 — Android runtime mapper и FullyCoveredEXT (source-only)

Восстановлен отсутствовавший `u_gralloc_mapper_runtime` (ELF create0xc84494, get_info0xc84748,destroy0xc84a00). Загрузчик ищет android_load_sphal_library в default namespace, затем libmain_hook.so (NOLOAD→normal); открывает mapper.qti.so и AIMapper_loadIMapper. При неподходящем v5 использует GraphicBufferMapper singleton/FourCC/modifier из libui/default namespace. Legacy fallback создаётся напрямую и даёт planes/fds/strides; runtime заменяет **только** FourCC+modifier и только при успехе обеих optional queries, иначе сохраняются fallback-значения. Декодер original StandardMetadataType принимает1..128 bytes, name53 bytes,type offset61,value offset69: FourCC type7/4 bytes (minimum73), modifier type8/8 bytes (minimum77). Не путать этот ограниченный backend с полной plane-layout поддержкой IMapper. Более богатый build-time IMapper сохранён первым на platform builds; SDK33 получает runtime вариант до LIBDRM/QCOM/legacy. API callback/return types дополнительно сверены с [AOSP IMapper.h](https://android.googlesource.com/platform/hardware/interfaces/+/refs/heads/main/graphics/mapper/stable-c/include/android/hardware/graphics/mapper/IMapper.h), blob23de7b9b71a889954a5bf361cfc186cfc8076ce4; runtime/GPU пока не проверены.

Property getter inventory позволил найти ещё один реальный пропуск: `fullyCoveredFragmentShaderInputVariable` original+0xb6c = chip>=8. В оригинальном emit_intrinsic opcode0x199 — load_fully_covered, sysval0x5b, unsigned compare !=0. Перенесены IR3 input/variant flag, FS destination selection (fully-covered input вместо обычного samplemask при наличии), sysval count, консервативная растеризация RB inner bit12 и GRAS bits3+4=0x18, FS-параметр в static/dynamic raster emission и обновление по dirty FS. **Уточнение по условию:** original NIR+0xa4 bit0 — system_values_read[SAMPLE_MASK_IN=0x20], не uses_sample_shading. При чтении обоих builtins sample-mask input создаётся до fully-covered input. Публикация свойства остаётся false до конечной compiler/cache/GPU проверки.

Чтобы не читать старые variant records, изменена схема0x4753380b в TU graphics/compute и IR3 shader cache keys. Старые записи о0x4753380a относятся к прежнему interlock checkpoint; это история, не актуальная схема. Отдельно записаны original A8 свойства `meshAndTaskShaderDerivatives` (+0xc6c) и `fragmentShadingRateWithFragmentShaderInterlock` (+0xf78); они пока gated вместе с соответствующими execution chains, а не считаются уже проверенными возможностями.

Дополнен default-off TU_AC4_DIAGNOSTIC_LOG: thread-safe init и атомарные первые8 сообщений stage/extra_dwords. Существующая VPC reservation64/320 dwords уже совпадала с оригиналом и **не увеличивалась повторно**. Это только диагностика.

Evidence: `runtime-mapper-source-evidence.json`, `fully-covered-source-evidence.json`, `safe-const-diagnostic-source-evidence.json`. Полная сборка/тесты/probes/установка не запускались. На устройстве a69bd574; пользователь пока НЕ тестировал её.


## 2026-10-19 — Current hour source handoff

Transferred this hour: A8 full-density FDM tile policy; existing image atomic64 capability policy restored to original in the isolated experiment; binary-first shader override plus original dumps/metadata; Wine cache path translation; runtime AIMapper/libui FourCC+modifier backend; original default-off AC4 diagnostics; FullyCoveredEXT compiler/FS/raster chain with exposure held. Full package and validation are still incomplete. The runtime mapper, debug/cache tools and fully-covered path are now explicit C11/C12/C13 validation rows.

The observed outer original IR3 emit_intrinsic switch has159 named cases; current switch has156. The remaining three are original no-ISA mesh markers (set_vertex_and_primitive_count, store_per_vertex_output, store_per_primitive_output) removed earlier by our mesh lowering. Do not add silent no-ops merely to match case counts; final integrated mesh validation must prove no side-effecting leftovers. This inventory does not establish semantic equivalence of all handlers.

Local adaptation for fully-covered state: dirty RAST only when the reads_fully_covered flag changes at FS bind, avoiding a new redundant raster-state emission for ordinary FS changes. Native comparison/register placement/API/GPU behavior awaits final validation. Cache schema is0x4753380b in TU graphics/compute and IR3 shader keys.

Last installed binary remains a69bd574 and is NOT_TESTED_BY_USER. The prior smoothness report was sent in error and retracted. All subsequent source remains unbuilt, with no new tests/probes/device work in this hour.


## 2026-10-19 — Shared atomic increment source transfer (unbuilt)

Original `emit_intrinsic_atomic_shared` (Ghidra0xbc11c4) selects opcode0x313/ATOMIC_INC for iadd with32-bit result and constant unsigned value1. Local code previously always selected ATOMIC_ADD. Transferred this bounded selection using the existing INSTR2(ATOMIC_INC) builder; retained both operands and common type/barrier/conflict/keeps processing. No additional hardware-generation gate appears in the inspected original branch. Other operations and nonconstant/non-one/non32bit fallback remain unchanged. Cache schema0x4753380c belongs to the same unbuilt source checkpoint. No compiler/assembler/GPU execution or performance result; final checks include wraparound, result used/unused, shader memory ordering and all supported GPU generations. Evidence: shared-atomic-increment-source-evidence.json.

Getter inventory follow-up: six bounded shared tails plus four mesh CONCAT32 copies add10copy/21field records (88field bytes), disjoint from the prior direct-copy inventory. The four mesh output fields are already assigned locally. FDM2/NV/ARM getter presence is not evidence that those extensions are supported; no exposure was enabled. Initializer predicates/value equivalence remain unproved. Evidence: original-property-shared-tail-inventory.json.


2026-10-19 requested build: fixed decoder-generator symbol collisions by giving the split ATOMIC_B and BAR/FENCE leaves explicit generation tags. Effective inherited generation ranges and instruction patterns remain unchanged; raw leaf gen_min is also used in generated symbol names. First build failure retained in build-dfafced7.log.


## 2026-10-19 — Подтверждённая регрессия7e9ea0c8: потеря static MSAA state

Виноват наш перенос FSR/MSAA зависимости, а не доказанный дефект gralloc/ISA. `tu_fragment_shading_rate_state` стал потреблять MS_RASTERIZATION_SAMPLES. `emit_pipeline_state` удалял этот static state из копируемого при bind pipeline, хотя `tu6_update_msaa` и `tu_emit_fs_params` читают его при draw. Изображение2x/4x могло рисоваться как1x/default либо с устаревшим числом samples.

Исправление: добавить MS_RASTERIZATION_SAMPLES в keep-set в `tu_pipeline_builder_emit_state`. FSR shape/16coverage policy и dirty dependency остаются. Ничего из ISA, gralloc или capabilities для этой коррекции не откатывалось. Это одна строка функционального исправления плюс комментарий.

Android/A830: тест257×257 с цветовым узором, MSAA и resolve в imported AHB воспроизводит ошибку. На7e9ea0c8:4x —238794/248474ошибок каналов вGMEM/sysmem;2x —223216/229610. После keep-fix оба режима проходят66049pixels RGBA с допуском1. Dynamic4x также проходит. a69bd574 проходит static4x, но пользовательское игровое тестирование a69 ранее отозвано — не объявлять её игровой базой.1x AHB и shared/SSBO/typed atomics проходят old/new и сами по себе не ловят эту регрессию.

Исходники тестов, скриншот, логи, минимальный patch и findings: `outputs/gamesir-preliminary/regression-7e9ea0c8/`. Полная валидация переноса впереди. Исправление воспроизведённой GPU-регрессии подтверждено; исправление именно пользовательской игровой сцены требует повторной проверки пользователем. Слот пока остаётся7e9ea0c8.


## 2026-10-19 — Imported signaled sync-file adaptation, source only

GameSir legacy syncobj merge skips state1 (signaled), and its wait builder emits no binary wait for that payload. Our legacy backend and CPU binary waits already agree. The separate native-timeline binary backend imported fd=-1 as a signaled FENCE payload but GPU submission still emitted it as a KGSL fence descriptor. Skip these payloads in both counting and emission; retain the private prior-queue timeline. This is a semantic adaptation to our backend, not a wholesale replacement with GameSir legacy syncobjs.

Actual source blocks were extracted and tested with host payload mocks: all1365 sequences of up to5 native timeline/binary/real-fd/signaled-fd waits; old builder fails1001 cases, corrected builder passes all. This verifies array contents/order/counts, not ioctls/GPU scheduling/API conformance. No driver build or device change. Evidence: kgsl-signaled-wait-source-evidence.json and kgsl-signaled-wait/result.json.

ISA follow-up: manually compared the finite87 callback formulas to source on valid fields; no further discrepancy identified. Missing-field/decode-error behavior, encoder/decoder execution and formal body equivalence remain unproved. Width-sensitive shifts were considered only on the encoded1bit W domain. Evidence: isa-expression-normal-domain-review.json.

Installed3ae2867c rendering is user-confirmed; FPS/temperature remain unconfirmed. TASK/MESH rejection and other existing feature gates stay in place. Complete GameSir package and final validation are still unfinished.


## 2026-10-19 — Mesh FS pipeline cache integration and bounded failure recovery

The reconstructed immutable mesh producer map lived in ir3_shader_variant.shader_options.mesh_fs_inputs, before VARIANT_CACHE_START. Gallium initializes those options from its shader, but TU reconstructs variants with ir3_retrieve_variant. Consequently TU cache hits lost enabled/base/location fields and could select the unmarked VPC remap. The outer IR3 variant record now explicitly writes/reads this pointer-free map. This repairs our local compiler/VPC integration; it is not evidence of a newly identified original GameSir algorithm or GPU rendering bug. Graphics/compute TU stage keys and IR3 shader keys use schema0x4753380d. Earlier0x4753380c records are historical.

Bounded inherited cache fixes: read and size-check the full binary/constant/immediate payload before allocating or publishing it; handle variant/const-state/binary/immediate/binning allocation failure; return NULL on failed outer variant read. TU rejects failed variants, unsupported main stage, mismatched safe-constant stage, truncation and trailing bytes before GPU upload. Direct IR3 disk-cache reads restore both draw/binning records and shared const_state after either read fails. Valid wire format changes only by the explicit mesh-map addition; not a general adversarial-cache parser claim.

Focused host evidence, no driver rebuild:8448real IR3 cache roundtrips (old4224mesh-map mismatches, new0);17036truncated payloads plus19injected allocation failures rejected;16417disk-cache read/rollback cases including9allocation failures;83011TU reader control-flow cases pass with TU objects/upload mocked. Whole ir3_disk_cache.c syntax check passes without diagnostics. Existing libraries are used only to link isolated component fixtures; Android SO/ZIP/slot remain3ae2867c.

Separately, four descriptor commands inherited graphics mask0x1f from GameSir and missed TASK/MESH-only stageFlags. They now use Mesa's graphics-bind-point helper. This is a local bug correction, not a missing GameSir optimization. Original ARM64 wrappers/masks are saved in descriptor-stage-original-wrappers.asm.

Finite source inventory cleanup:54historical new-function instances with no source-name mention now have explicit alias/generated/split-helper routes. The entire184new-instance table is not a semantic equivalence proof; changed existing functions and final execution/activation checks remain independent. TASK/MESH rejection remains. Next full build/API/GPU validation must cover mesh/GPL/cache/AQE and other held feature chains. New schema requires warm-cache comparisons for any later game/FPS/temperature test.


Cache feedback follow-up: common vk_pipeline_cache_lookup_object removes a rejected raw object/disk record and returns NULL, so TU can recompile. Its application-hit flag may already be true for a raw entry. Graphics TU also broke out before combining a missing shader's hit flag. Require cache_hit for graphics and a non-NULL shader for compute before reporting APPLICATION_PIPELINE_CACHE_HIT. This covers ordinary misses, rejected serialized records and incomplete retained NIR. A small reporting correction, not an FPS optimization or a new GameSir algorithm; no separate mirrored Boolean test was added.


## 2026-10-19 — Recover the real-FS predicate for mesh AQE/VPC

GameSir tu_emit_program_state excludes both device-owned placeholder FS objects before deciding whether the mesh path has a fragment shader. In the original builder, device+0x27b8/0x27c0 supply the empty normal/FDM FS; the program builder compares the shader object with both, then uses the resulting real-FS predicate for AQE metadata bit1, the VPC no-FS branch and inclusion of the FS command stream in the combined program. This is a recovered caller contract, not an extra empty shader implementation.

The local pipeline builder also inserts empty_fs/empty_fs_fdm for an absent application FS, but the reconstructed mesh caller previously passed their non-NULL compiled variants to tu_a8_upload_linked_mesh_program. Consequently has_fragment_shader was true, AQE metadata bit1 was clear and the no-FS VPC reset was unreachable through the production caller. The detached helpers already supported NULL FS correctly; the missing part was normalization at the pipeline-to-mesh boundary.

The mesh branch now selects mesh_fs only for a shader object different from both device placeholders. The same selected variant controls fs_dwords and the linked program upload; NULL selects the existing metadata/no-FS VPC behavior and omits the placeholder FS stream. Ordinary pipeline shader arrays remain intact for shared bookkeeping and state. An application-provided empty shader is still a real FS: this is object identity, not a heuristic on input/output/instruction counts. Selected safe-constant variants remain paired with their draw state for a real FS.

Source-only transfer. No driver/component build, tests, sanitizer, GPU execution, ZIP or installation in this pass; only source/decompiler inspection and git diff --check. The TASK/MESH compilation rejection and feature gates remain. Shader serialization and shader key inputs did not change, so the cache schema remains0x4753380d; combined mesh program state is regenerated when constructing a pipeline. 39fddd2d remains the last built/installed provisional baseline candidate. Full mesh-without-FS execution (including FDM, primitives/queries, multiview and real-but-empty application FS) remains for integrated validation.


## 2026-10-19 — A8 16-bit UBO storage feature policy (source-only)

Recovered another concrete feature-policy omission from GameSir physical-device initialization. ELF0x988d3c loads info+0x268 (storage_16bit), ELF0x988d44 loads the generation byte, and0x988d5c..0x988d78 stores `(generation > 7) & storage_16bit` at physical+0x252. The mapped Vulkan11/16bitStorage getters return this byte as uniformAndStorageBuffer16BitAccess. The local inherited unconditionalfalse is now replaced by chip>=8 && storage_16bit. Existing KHR_16bit_storage extension eligibility is retained.

This is an experimental capability metadata transfer, not a measured FPS/temperature improvement. IR3 already runs nir_lower_mem_access_bit_sizes for UBOs with 32-bit access sizing before UBO range promotion and LDC lowering; packed16-bit values are extracted from32-bit loads rather than requiring native16-bit LDC. No second UBO backend was invented. Runtime robustness1/2, descriptor/range edges, scalar layout, nonuniform/dynamic indexing, push/inline-constant promotion and cold/warm/GPL/cache behavior remain for final validation.

The adjacent original pool ELF0xaf760 starts00 00 01 01: storagePushConstant16 and storageInputOutput16 remainfalse, while shaderSubgroupExtendedTypes and samplerYcbcrConversion remaintrue. Those already matched. Full feature initializer policy is not yet audited by this bounded recovery.

No driver/component build, test suite, GPU execution or device upload in this continuation. Cache record format unchanged (schema0x4753380d). TASK/MESH rejection and the other held capability gates remain. Last installed39fddd2d is still only a provisional baseline candidate. Native sparse FS address/mode/transfer-mask worksheet is saved separately; it does not justify an isolated ISA/padding/LM-mask patch.


### UBO16 prerequisite: RANGE_BASE in widened unaligned memory loads

Found an inherited Mesa defect while following the newly exposed UBO16 path. nir_lower_mem_access_bit_sizes handles align_mul<requested.align by cloning an aligned load, then assumes every RANGE intrinsic also has BASE. load_ubo has RANGE_BASE/RANGE and no BASE, so a legal align_mul2 UBO16 load widened to32bits reaches that assertion. The existing BASE path comes from Mesa2e653c36407b.

Added a separate RANGE_BASE branch: publish the documented conservative unknown range `{range_base=0, range=UINT32_MAX}` for the widened clone. Keep the actual aligned address, shifts/extraction, source access flags and the existing BASE-backed range calculation unchanged. This avoids inventing a tight bound for dynamic offsets and can reduce UBO range promotion on that path; it is a correctness prerequisite, not a claimed optimization. This is a local inherited-Mesa correction, not a claim that GameSir contains the same generic source fix.

Full compiler execution is deferred with the rest of validation. Required focused cases: align_mul2 UBO16 loads at offsets0/2/14 and dynamic halfword offsets, vector crossings, explicit RANGE_BASE and unknown range, promotion/LDC plus robustness/descriptor range ends. Also regress BASE-backed load_constant/push_constant widening. No test or build run in this pass.


## 2026-10-19 — Capture/replay eligibility and bounded initializer reconciliation (unbuilt)

Original physical-init ARM64 uses `has_set_iova && (!has_sparse || has_vm_bind)` for BDA and acceleration-structure capture/replay, and additionally gates descriptor-buffer capture/replay by descriptorBuffer. Restored this shared eligibility in tu_get_features. Ordinary bufferDeviceAddress stays true; allocation backends are unchanged. This restricts the three capture/replay feature bits on sparse-capable KGSL without VM_BIND, matching the original. The original allocator already supports the REPLAYABLE/client_iova path; its extra feature restriction is not proof of a missing backend or of a defect in current Mesa. Kernel-specific motivation and complete capture/replay behavior remain for final validation. No performance/thermal claim.

Static reconciliation now accounts for the571 distinct mapped original feature bytes:244 explicit literals match;249 original default-zero bytes match local false/default;43 conditional expressions match;8 native packed-constant bytes match;2 indirect-copy feature bytes depend on the already enabled extension. Original policy transfers account for1 prior UBO16 byte and3 capture/replay bytes in this pass. Eleven feature bytes plus maintenance10 remain held for final activation validation. Nine WSI fields are conditional on TU_USE_WSI_PLATFORM (Wayland/XCB/XLIB/DISPLAY), absent in the original Android TU stores. This is initializer/getter source coverage, not a full API/backend/GPU audit or live feature enumeration.

All50 conditional original extension-byte predicates were separately classified:45 current source expressions match;5 mesh/barycentric/interlock/quad-control/reconvergence extension activations remain held. A historical descriptor-buffer annotation was stale: current source already has the Uncharted restriction. The new current-source report corrects it without inventing a duplicate patch. maintenance10 remains Android-filtered in the original; its complete API/resolve contract is still open.

Evidence: capture-replay-feature-policy-transfer.json; feature-predicate-source-classification.json; feature-initializer-source-boundary.json; extension-predicate-current-source-classification.json. No build, compiler execution, driver tests or device installation in this pass. TASK/MESH guard and cache schema0x4753380d retained. Last installed39fddd2d is only a provisional baseline candidate.


### Additional source boundaries in this continuation

All26 original TU parser options have current declarations, literal defaults and direct consumer routes recorded in drirc-current-consumer-source-boundary.json. This is stronger than a name-only match, but does not establish complete option semantics or runtime profile matching. The originalfalse/localtrue cached-coherent replacement default is an intentional AK change from6e9a7ffd53c9 and is retained. No default changed in this pass. A suspected NULL string default was rejected: DrircString(None) emits DRI_CONF_OPT_S_NODEF, whose C macro stores an empty string; original force-device-name fallback likewise checks the first byte.

ISA decoder continuation: four shared helper bodies and four representative callback missing-field paths inspected against native ARM64 and source. Missing fields diagnose and substitute numericzero, matching the current shared isa_decode_field contract. In expr_anon_49 a decompiler temporary looks uninitialized, but native failure sets the comparison result totrue (zero==zero); no speculative patch. Native exports and limits: feature-decode-native/export-provenance.json and isa-decode-error-source-boundary.json. This does not close all87 callbacks' malformed-input combinations, allocator/depth/recursion safety, generated codec behavior or GPU instruction execution.

## 2026-10-19 — Общая граница ABI кеша (перенос без сборки)

В оригинальном `tu_device_get_cache_uuid` найден дополнительный маркер формата кеша: 62 байта по ELF0xbed88, hash-update в ARM64 по0x988174..0x98818c. У нас схема0x4753380d уже входит в ключи graphics/compute/IR3, но ещё не входила в общий UUID кеша устройства. Добавлены собственный маркер AK и существующий номер схемы после build ID. Оригинальный маркер не копируется буквально: наш сериализованный формат не объявляется совместимым с GameSir.

UUID используется в заголовке кеша приложения и в пространстве имён дискового кеша TU. Общий загрузчик сравнивает заголовок до импорта записей. Это сохраняет разделение форматов и при фиксированном `TU_BUILD_ID_OVERRIDE`. Старые ключи отдельных записей уже были разделены; ошибочное использование старых шейдеров этой находкой не доказано. Содержимое записей не менялось, схема остаётся0d; теперь её учитывают четыре места.

Следующая сборка получит новый UUID и может потребовать повторного прогрева кеша. Ускорение компиляции, рост FPS или снижение температуры не измерены. Проверки старого/нового кеша, повторного запуска, обычного и фиксированного build ID остаются на конец. Доказательства: `cache-abi-uuid-source-transfer.json`, `feature-decode-native/cache-uuid-abi-update.asm`.

В очереди properties уточнены213 исторических записей «нет прямого присваивания»: шесть заполняются копированием. У остальных207 в ограниченной карте нет ненулевого прямого TU-store:205 без наблюдаемой записи, две с нулевой записью. Это не207 отсутствующих возможностей и не доказательство полноты анализа инициализации. Карта: `property-missing-assignment-source-boundary.json`.

AQE no-op caller restoration (2026-10-24): both indirect mesh entrypoints now return for zero drawCount/maxDrawCount before setup/allocation, matching original0x94a1a0/0x94a2ac. Direct mesh entry unchanged. Policy0x47533810; unbuilt.

2026-10-24 flat-input native selection restoration: the historical !per_primitive exclusion is superseded. Compute last_vertex_flat from the original coord before rasterflat changes it; retain independent primitive provenance as the original does. Native/source evidence sparse-aqe-hour-20261024/flat-input-selection-source-evidence.json. Current unbuilt compiler policy0x47533810; integrated LDLV/VPC/GPU verification remains deferred.



### 2026-10-24 — Новый час sparse address/AQE: shared special stores

Начало11:34:56UTC, работа минимум до12:34:56UTC; без сборки/запуска shader compiler/устройства. Новый coordinated policy0x47533811 (все четыре key/UUID sites). Перенесён native source gap: lower_mesh_intrinsic ELF0x9ae8b0 и0x9aec6c передают shared-store access4(ACCESS_VOLATILE) для CullPrimitive и PrimitiveShadingRate. Layer/Viewport shared mirror(0x9af420), ordinary output(0x9af714) и packed-index mirror(0x9afa6c) access0. Local lower_mesh_special_output прежде терял access для всех special mirrors; теперь volatile назначается ровно двум native случаям. Global flag reads/writes сохраняют прежнюю policy, новых барьеров не добавлено. Полная GPU-валидация отложена.

Получены10новых typed C/high-Pcode exports helpers/callbacks и3уточнённых memory-index exports. Имена и index_map для18NIR intrinsics восстановлены из original ELF nir_intrinsic_index_names/infos; номера enum локальной Mesa не подставлялись. Неопределённые native поля не считаются доказанными layouts. Источник: native-complete/sparse-address-aqe-hour-20261024/shared-special-access-source-evidence.json, original-address-aqe-index-abi.json и raw ARM64.

Рассмотренный get_barycentric(0) у mesh flat inputs НЕ новый gap: и native ir3_compile_shader_nir, и local emit_instructions заранее создают IJ_PERSP_PIXEL для FS. Условный native вызов повторно проверяет уже существующий cache entry; дублирующая инициализация не добавлена.


## 2026-10-24 — Restore A8 single-subpass MSAA GMEM policy

GameSir get_optimal_mode ELF0x8eae64..0x8eaea8 returns GMEM for chip>=8, exactly one subpass and any attachment with samples>1. This branch follows FDM/MSRTSS rules and precedes tuner configuration/history allocation. The reconstructed source lacked it. The same early predicate is now connected; the local trace reason is set explicitly. use_sysmem_rendering still rejects unsupported/impossible GMEM cases before invoking the autotuner. Existing AK depth-only policy remains, but the recovered MSAA early rule takes precedence when eligible, as the original does.

Source and native evidence: docs/gamesir-recovery/a8-msaa-autotune.json; recovery/native-complete/rutabaga-operand-hour-20261024/. No build, shader execution, GPU test or measured performance/thermal result. Cache schema0x47533814 is unchanged: no shader format/code changes. Final validation must cover caller restrictions, single/multiple subpasses, dynamic/legacy rendering, all MSAA counts and autotuner/debug configurations.


## 2026-10-24 — TASK/MESH WorkgroupID delivery bundle

Restored three coupled native compiler policies absent from the recovered source: emit_intrinsic ELF0xab7ff0..0xab80d8 creates vector TYPE_U32 xyz copies of the shared WorkgroupID input for TASK/MESH; ir3_cp ELF0xacdd20..0xacdd84 plus instr_cp retains shared-to-vector copies from META_SPLIT(META_INPUT(WORKGROUP_ID)) for MESH; ir3_compile_shader_nir ELF0xaad100..0xaad150 precolors that input at IR3 shared register0xcf before cleanup_rpt/RA. The same eligibility helper serves ordinary copy propagation and kept-output moves. TASK receives the copies and precolor, but not the MESH-only CP exception.

Original sysval47 and intrinsic0x2f0 are independently named by ELF tables as SYSTEM_VALUE_WORKGROUP_ID and load_workgroup_id; numeric local-ABI assumptions are not used. Shared register0xcf is not a CP shared-GFX constant address. The ordinary RA handles precolored input allocation, but the separate shared RA also needs the native META_INPUT preference. That missing get_reg branch was restored in the subsequent policy1c continuation; runtime validation remains deferred. All four TU UUID/graphics/compute/IR3 cache policy sites move together to0x47533815, because generated code/register placement changes. TASK/MESH API/compiler gates stay held. No build, shader execution, GPU test or performance claim. Evidence and final cases: docs/gamesir-recovery/workgroup-id-copies.json.


## 2026-10-24 — Subgroup execution-mode/dispatch source transfer

Original spirv_to_nir writes shader-info byte0x196 bit5 for SubgroupUniformControlFlowKHR(mode4421) and bit4 for MaximallyReconvergesKHR(mode6023). In original ir3_nir_post_finalize, COMPUTE/TASK/MESH/KERNEL select linear dispatch when maximally_reconverges OR (subgroup_uniform_control_flow AND has_dual_wave_dispatch). The existing derivative_group==LINEAR rule is separate. Native property info+0x2bd is independently named has_dual_wave_dispatch by the debug-parser string/field route. Earlier notes labeling this QUADS behavior were incorrect.

The local frontend previously ignored SubgroupUniformControlFlowKHR. Add a named shader_info boolean, retain the execution mode and restore the native IR3 dispatch predicate. Whole-struct NIR clone/serialization already carry this metadata; no duplicate serializer path is added. Four TU/IR3 UUID/key sites advance together from15 to16 because both generated code and serialized shader_info bit layout can change. No feature-gate activation, driver/shader build, GPU test or measured performance claim. Final cases and original anchors: docs/gamesir-recovery/subgroup-dispatch.json.


2026-10-24: compute/fragment helper reconciliation is recorded in docs/gamesir-recovery/compute-frag-stages.json and the current GAMESIR_REMAINING.md section. Source-only transfer, policy17, feature gates held.


## Late FS input hoisting: native transfer, policy0x4753381d (unbuilt)

Восстановлен `ir3_nir_move_varying_inputs` (ELF0xa93918), его рекурсивная проверка зависимостей0xa93b98 и перенос0xa93e3c. Объявление уже было в ir3_nir.h, но реализации, Meson-источника и позднего вызова не было. Вызов теперь после lowering barycentric-at-offset, до framebuffer/layer lowering; ранний generic move-to-top это не заменяет. Оригинальная таблица intrinsic_infos независимо дала три цели:0x1c2 load_input,0x1cb load_interpolated_input,0x1f3 load_per_primitive_input.

Сначала проверяется весь shader: зависимости вне entry могут быть ALU/deref/constant/undef либо reorderable intrinsic; phi/texture/непереставляемая операция отменяет весь проход до изменений. Затем зависимости и входы добавляются в entry в порядке зависимостей. Сохраняется control_flow metadata0x83. Локальная адаптация явно отделена от оригинала: возвращается фактический progress, тогда как native функция возвращаетfalse даже после nir_progress. Четыре cache/compiler markers1c→1d.

Доказательства: docs/gamesir-recovery/fs-input-hoist.json и native-complete/rutabaga-execution-hour-20261024/fs-input-hoist-native.arm64.txt. Только статическое восстановление и diff --check; без сборки, исполнения NIR/шейдеров и GPU. Полная проверка на конец: условные/циклические и sparse/per-primitive FS входы, phi/texture abort без частичного переноса, ordering/metadata, давление регистров, prefetch/VPC и игры. Ускорение пока не измерено.
