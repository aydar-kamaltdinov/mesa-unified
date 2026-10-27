Current validation result (2026-10-27): **source/static review complete with execution/activation blockers**, policy0x4753381f, UNBUILT. V001–V011 are fixed in source; V007 includes both lazy-option readers and V009 includes ioctl and FD-poll retries. The bounded127-path inventory, fixes, rejected suspicions and mandatory next build/NIR/ISA/API/GPU/thermal checks are recorded in [the review report](docs/gamesir-recovery/validation-source-review-20261027.md) and validation-20261027.json. **Integrated validation remains incomplete**; held feature gates stay closed. Earlier dated summaries/checklist below are historical, not current proof of correctness.

Previous continuation snapshot before validation (2026-10-27): policy0x4753381d, UNBUILT. The 8 previously failed-C units now have bounded source accounting:7 readable format helpers and a715-field/275-type feature-checker capsule, plus 3 classified boundaries. Four caller-array property structures now have nonempty-buffer contracts (6 routes/620 static cases). Existing Mesa generators already implement these paths; no redundant production backend was added. Automatic-C metrics remain 7683/7691, 8 failures/3 boundary-label cases. Full integrated validation and feature gates remain deferred. Earlier dated sections below are historical.

# GameSir reconstruction — remaining work

Pre-validation snapshot2026-10-27: branch `rutabaga`, policy **0x4753381d, UNBUILT**. The six source transfers through503ca36a6db are followed by the three source bundles described above. The machine listing covers all sized code; recovered C has7683/7691 successful exports,8failures and3boundary-label cases. These are analysis quality counts, not behavioral coverage.

The restored original getter maps contain274feature selectors/660member records/571physical bytes; property routes include122strict structures,659member records,118core fields and649distinct contiguous physical fields. The original getter map records four null-query-only cases; the separate property-caller-array-contracts.json now recovers their bounded nonempty-buffer semantics in6routes/620static cases. Arbitrary aliasing/lifetime and real initialization remain deferred. Original static device tables match121named properties and18header values on55common A6–A8 records; all9A8 raw magic sequences match including order. No extra profile defaults or feature activation inferred.

TASK/MESH compilation/exposure and other held gates stay closed. Full integrated compiler/API/cache/GPU/game validation remains at the end. The current source has not been built or installed. See `docs/gamesir-recovery/native-tables-20261027.json`, the full getter maps, the three source-transfer proofs and the analysis-tool README.

Historical transfer checklist, superseded for current source-review status by the2026-10-27 report above. Its execution/activation criteria remain applicable.

| ID | Area | Historical transfer status | Present in source | Execution/activation closure criterion |
|---|---|---|---|---|
| M1 | Mesh EXT compiler/FS/VPC | source-connected-unvalidated | Physical placement moved to setup_input before instruction creation; PrimitiveID tagging and coordinate-based FLAT_B restored. Compiler per-vertex compaction and separate borrowed VPC metadata remap connected; native input-record prescan, filtered-copy flag preservation and A8 VPC0x925a now transferred. Native FS constant/RT-alias/preamble restrictions and pre-rasterflat last-vertex selection now restored. Late FS input/dependency hoisting after barycentric lowering is connected. | Validate compiler/VPC/LDLV execution, sparse primitive ranges, absent-producer/unwritten varyings, prefetch logical/physical coordinates and barycentric/limit cases. |
| M2 | Mesh GPL and cache | source-connected-unvalidated | GPL snapshots/import plus required-stage checks and private complete-candidate fallback after incomplete NIR dedup.  Explicit mesh FS producer-map serialization and clean truncated/OOM read fallback transferred; schema0x47533810. | Validate snapshot idempotence, GPL combinations, cache ownership/dedup concurrency and repeated malformed-entry recovery. |
| M3 | Mesh draw/CCU | source-connected-unvalidated | Draw/CCU source plus TASK/MESH bind pointers, mesh program group masks, absent-VS handling and point raster/MRT policy. | Finish address/size eligibility, renderpass/binning/secondary replay and full point/blend behavior validation. |
| M4 | AQE resources/context/parameters | partial | AQE BO, metadata, descriptors/constants and query latch source-wired. Both indirect zero-draw caller exits restored; metadata/prefix/packet address units recorded separately. Recovered 9-vec4 absolute constant prefix, mesh-multiview FS slot33 lowering and original reciprocal/two-correction AQE group-coordinate division. | DrawID/ViewIndex preamble copies, multi-STSC expansion and raw WorkgroupID shared-input0xcf/vector-copy/CP bundle, including separate shared-RA fixed-input preference, are now restored. Validate prefix/API/driver-parameter allocation and publication ordering; trace context/shared-bank/BO synchronization; do not assume an unconditional compute restore is required. |
| M5 | Count publication/indices | partial | Static/dynamic/absent-site zero-count publication, conditional packed-index volatility and native CullPrimitive/PrimitiveShadingRate shared-mirror volatility source-connected. EXT indices documented write-only; NV read/write form is unadvertised. Original zeroed EXT wrapper/callback chain has no producer for dynamic_counts && !counts_initialized. | Validate count publication ordering, constant/dynamic CFG and limits; Shared fallback excluded for the inspected EXT chain; do not re-open without a concrete producer. No extra advertised MeshEXT Output-read backend is required (VUID-StandaloneSpirv-MeshEXT-07107); existing recovered helpers retained. |
| M6 | Mesh queries/limits/trace | partial | Mesh primitive-query API routes, TASK/MESH statistics indices/storage/mask and query latch source-connected. Original trace has only legacy hashes. Exact original EXT task/mesh limit table source-connected; exposure remains gated. | Validate exact limit table against compiler/layout bounds and counters/nesting/multiview/availability; TS/MS trace metadata is optional local observability. |
| M7 | NV task/mesh parity | excluded-original-unadvertised | Original NV_mesh_shader supported byte at physical+0x1d0 is explicitly zero; enumeration requires1. Internal fallback/stubs do not create advertised support. | No NV backend required for this binary advertised-feature parity; confirm complete original/local feature inventory in final validation. |
| C1 | Cooperative matrix | source-connected-unvalidated | Original13-operation lowering, properties and ordinary buffer I/O path transferred. Both cooperativeMatrix feature bits now use the original64-lane eligibility instead of an unconditionalfalse robustness bit. | Final descriptor/robustness1/2, pipeline overrides, required subgroup-size and FP edge behavior; Vulkan/GPU validation. |
| C2 | Indirect memory/image copy | partial | Commands, shaders, formats and queue policy transferred. Original extension/features are unconditional; A8-only image-destination format guard remains separate. | Full API/GPU behavior and SDK exposure filters. |
| C3 | Barycentric/per-vertex/GPL | partial | Static IR3 paths and primitive-dependent NIR retention/relink transferred; common A6/A7/A8 VPC alignment/barycentric fourth-component override now restored beyond mesh callers. | Dynamic topology/provoking behavior, library/cache lifetime and complete rendering validation. |
| C4 | Depth/stencil/transfer resolves | source-connected-FDM-store-unvalidated | Extended sysmem and A8 GMEM shader stores transferred; original non-integer-color/extended shader-store policy now includes FDM. Removed local FDM-only whole-pass sysmem override; existing offset-aware GMEM descriptor reused. Original A8 full-density FDM tile-area policy now transferred (info+0x309). | Validate FDM/subsampled/per-view stores, format/LRZ/synchronization and the original store/load round trip. The local opt-in in-place prototype is not a demonstrated original feature. maintenance10 TU support is Android-filtered in the original; do not confuse that with an unimplemented stub or claim full API parity. |
| C5 | Draw and application capability compatibility | source-connected-unvalidated | A8 single-subpass MSAA forced-GMEM autotune rule, direct indexed split+WFI and Cyberpunk rule transferred. Original Wave32/Uncharted options and compiler/capability predicates transferred; all23 embedded Turnip profiles now represented, including mesh-disable rules. | Validate compiler VS side-effect metadata, default/override matching, eligible draw semantics and performance; indexed indirect/count ordinary packet path is already equivalent; no separate direct-style split in those original entrypoints. |
| C6 | Sample-center and unrestricted depth | partial | Sample-center caller source-connected behind IR3_GAMESIR_SAMPLE_CENTER=false; original operand order retained, bounded ALU walk and both cache keys separated. Device+0x110 decoded as EXT_depth_range_unrestricted; A7/A8 clamp and viewport rules plus D32 2D/shader/event raw-clear selection and separate sysmem clear clamp policy source-connected, exposure disabled. | Resolve sample-center polarity and validate unrestricted depth/explicit clamp ranges on A7/A8. |
| C7 | CP_START_BIN/bin-data policy | source-connected-disabled-unvalidated | Standalone OFFSET path already matches. Selected-pipe pseudo8/9/10 + opcode0x2d prefix and final-buffer address fixups transferred behind defaultfalse TU_GAMESIR_BIN_DATA. | Validate packets, prefix ownership, SIMULTANEOUS_USE/secondary/multi-renderpass/preemption and activation policy; pseudo11–15 values already match; their unknown names alone are not a missing implementation. |
| C8 | A850/device properties | source-connected-unvalidated | Separate recovered A850 chip0xffff440514ff profile added with five enabled properties and raw0x980b/0x8816 changes; A830/A840 flags remainfalse. HW VS sysvals, direct/indirect/count/compact multi-draw source paths connected. DRAW_AUTO already preserves VFD offset writes while skipping only constants. All79 original device records/121 named properties statically reconciled; old A702 typo IDs intentionally excluded. Existing A7 gen2/gen3 image atomic64 policy restored for this isolated full-capability experiment. | Validate original driver-constant reservation, VFD/packet layouts, DRAW_AUTO sysvals, shader-input limits, firmware execution and GPL/cache on A850; source profile presence is not hardware validation. |
| C9 | KGSL timeline semantic differences | partial | Native KGSL timelines already existed locally. Absolute deadline retry and native fatal signal/query/wait device-lost handling transferred. Narrow empty native-timeline CPU-signal path now source-connected, preserving local private queue timeline ordering. Already-signaled imported sync-file payloads are now omitted from native timeline GPU fence waits; private queue ordering is retained. | Device c7bf6afa empty-submit/host-wait and pending-queue order checks passed. CPU ioctl selection, failure injection, resource failures and performance remain; compare binary/imported-fence and legacy fallback separately. Host extracted wait-builder cases pass; real KGSL mixed imported/native waits and Vulkan sync conformance remain pending. |
| C10 | Advertised features / symbol-only entries | shader-policy-source-connected-activation-unvalidated | 431 original extension store bytes classified statically:143 literal enabled,238 disabled,50 symbolic/conditional. ASTC decode path, frame-boundary/pipeline-robustness and untyped-pointer metadata transferred; the latter is Android-filtered in the original. NV/AMDX listed unadvertised families remain excluded. | Interlock six-mode frontend/FS metadata and GMEM/sysmem raster-order source chain transferred; exposure stays gated. Inspected quad-control/reconvergence core algorithms already exist locally; missing SubgroupUniformControlFlowKHR metadata and native compute-class linear-dispatch rules are now restored. Validate complete contracts/activation, including ordered/sample/shading-rate memory behavior. Mesh/barycentric/depth exposure remains gated. SDK/platform and integrated behavior await final validation. |
| C11 | Android runtime mapper metadata | source-connected-unbuilt-unvalidated | Recovered AIMapper v5/libui runtime backend, optional FourCC+modifier override over private legacy fallback; richer build-time mapper retained. | SDK/ABI/loader ownership, fallback errors and AHB/UBWC/linear/YUV imports on actual device. |
| C12 | Shader debug tools and Wine cache path | source-connected-unbuilt-unvalidated | Original binary-first override and original dumps/metadata; Wine drive/Steam cache path translation; default-off first8 AC4 VPC diagnostics. | File/malformed override behavior, cache bypass versus existing application pipelines, Wine/prefix/Steam paths and actual cache reuse; not a claimed FPS improvement. |
| C13 | FullyCoveredEXT and additional original A8 property contracts | source-connected-exposure-held | IR3 sysval/variant flag, sample-mask companion ordering, A8 FS input selection/count and conservative raster controls; coordinated compiler policy/schema0x47533810. Exact original A8 derivative/interlock property predicates recorded. | Compiler/RA register placement/GPU conservative/sample mask combination, static/dynamic/GPL/cache and activation; task/mesh derivatives and shading-rate interlock contracts also require final validation. |
| C14 | A8 fragment shading rate / MSAA | source-connected-unbuilt-unvalidated | Original A8 shape/16-coverage-sample policy restored; static/dynamic sample-count parameter and MS dirty dependency connected. | Final rate/combiner/MSAA/static/dynamic/GPL/GPU checks and performance; smaller fragments can increase work. |
| C15 | A8 instruction encodings / assembly tooling | source-connected-unbuilt-unvalidated | Generation-specific BAR/FENCE bit49, nine ATOMIC_B bit52 patterns, fixed BAR_SET/BAR_SYNC and A8 ALIAS reserved bits restored. All399 original descriptor masks/ranges and669 field-layout records match source declarations; All87 callback bodies manually reviewed against XML (not formal proof); alias operand mapping reconstructed. A8 FLAT_B immediate-src2 assembly rewrite (destination register+SY; restore IR) now transferred, policy0x4753380f. Selected cat3/WMM/STSC/LDLV/LDC/ALIAS operand routes are also source-reconciled; other operands and full assembler/GPU validation remain. No automatic new opcode insertion. | Final generators/assembler/encode/decode across A6/A7/A8, real barrier/atomic scope/memory ordering/deadlocks and game performance. |
| V1 | Final integrated validation | final-validation-deferred | Recovered e0ef6c3c artifact is byte-matched to the October19 installation. Whole historical source identity is not proved; current policy1d source snapshot is unbuilt. | One coordinated build/compiler/API/cache/GPU/game/performance pass after source transfer; fix regressions before enabling features. |

A block closes only after its stated behavior is reconstructed or shown to be already present/unimplemented in the original. Source-connected does not mean runnable, validated, or fast. V1 is mandatory for the final package. NV and symbol-only entries remain evidence questions, not automatically required new implementations.

The inherited scope.json area list is retained separately in outputs/gamesir-preliminary/remaining-work-registry.json; it has not been independently re-audited by this inventory.

2026-10-18 user feedback: installed578df20e has lower FPS in Thief and reduced smoothness in Alice: Madness Returns. Qualitative report, cause not isolated; do not promote as baseline. Keep prior8cf7cc15 artifact/slot backup for later comparison. No build/device work during this source pass.

## Finite handoff: source gaps versus validation (2026-10-18)

This is a routing of the existing 18 rows, not a completed audit or an estimate of percentage. Historical `partial` labels above do not each imply a missing implementation.

| Work type | Existing rows | Concrete next deliverable |
|---|---|---|
| Recovered device data, awaiting execution validation | C8 | A850 record is now source-connected; existing A830/A840 flags remainfalse. Validate actual packet/sysval/hardware behavior in the coordinated pass. |
| Remaining source-equivalence decisions | M1, M4, M5, C4, C5, C10 | Resolve FS remap equivalence and AQE context/view publication; establish fallback reachability; finish FDM/resolve routing and activation/format decisions. Indexed indirect/count packet source boundary already matches. The advertised inventory is now recovered; three A8 shader-policy decisions remain explicit in C10. These are bounded questions, not a request to translate every ELF symbol. |
| Source already connected; runtime/API/compiler closure outstanding | M1–M6, C1–C3, C5–C9 | Validate against the per-row criteria. C6 sample-center and C7 selected-bin variants stay default-off. M4/M5 still overlap the source-equivalence decisions above. No claim that source presence proves completeness. |
| Explicitly excluded from this binary's advertised feature parity | M7; listed families in C10 | NV mesh, AMDX dense/enqueue, NV cooperative matrix/vector/CUDA have original supported bytes zero. Do not implement them merely because symbols exist. |
| Final coordinated validation | V1 | Build once after the source decisions, run compiler/cache/API/GPU coverage and games; isolate reported FPS/heat regressions before promoting a baseline. |

Mesh/task remains the largest incomplete end-to-end chain and remains rejected at pipeline compilation. C1–C3 have transferred paths but their full requirements are not yet proved. Pseudo11–15 values match original tu_emit_vsc; unknown firmware names are a validation knowledge gap. The guarded selected-bin packet stream is not validated. This finite list may change only with specific new evidence, recorded here, rather than expanding by symbol count.

## 2026-10-19 source handoff update

The confirmed A850 record gap is transferred. M1 now follows the original emission-time placement rather than a local post-RA mesh rewrite, including the original mesh-inclusive per-vertex fixup and a separate remapped VPC metadata copy. These are unbuilt source changes, not a statement that mesh can run.

C10 is no longer an unbounded symbol search: original-extension-store-inventory.json records all431 entries, initial byte stores and Android allow bytes. Its50 symbolic/conditional RHS values are not treated as enabled. Newly identified advertised obligations are maximal reconvergence, quad control and fragment interlock on A8; they may partly use existing generic Mesa paths but their complete driver contracts are not established. ASTC decode, frame boundary and pipeline robustness metadata source transfers are recorded separately. Final platform/SDK filtering and live enumeration are still required.


2026-10-19 source update: A8 FDM shader-store policy now follows original tu_store_gmem_attachment and store_3d_blit. This changes the GMEM-to-sysmem store route, not the still-open in-place GMEM resolve route. Original TU enables maintenance10 and untyped pointers, but their Android allow bytes are0; they are platform-hidden implementations, distinct from explicitly unsupported NV/AMDX. Untyped pointer frontend code already exists in Mesa and its missing metadata is now transferred. maintenance10 complete API/feature obligations and all SDK filters remain explicitly unvalidated.


2026-10-19 finite source correction: original A8 subpass resolve always uses store→CLEAN_BLIT→load for GMEM-resident destinations. In-place FDM resolve is a local opt-in prototype, not an established Gamesir source gap. C10 interlock raster-order chain is now transferred (six modes, serialized FS flag, both rendering modes); quad-control/reconvergence inspected algorithms already exist. Their complete contracts and feature activation remain deferred to final validation.


2026-10-19 source boundary: no-count mesh shaders now publish zero counts instead of being rejected. Shared-count fallback is excluded for the original zeroed EXT wrapper/callback chain by its flag writes. Indirect indexed/count packets already match; logical prefetch offsets match the original inspected path. Sparse physical FS primitive-base handling is explicitly a local adaptation awaiting the final compiler/GPU decision, not claimed original-equivalent.


## Next work is bounded (2026-10-19, latest source pass)

Source handoff: interlock frontend/metadata/raster-order and no-count mesh zero publication are transferred. Reconvergence/quad-helper algorithms, indexed indirect/count packets, logical prefetch offsets, pseudo11–15 values and the original GMEM store/load route are already present on the inspected paths. Do not repeat their reconstruction unless a concrete new discrepancy appears.

Remaining concrete decisions are in source-to-activation-handoff.json: sparse physical mesh-base adaptation; end-to-end task/mesh/AQE/queries/limits and activation; valid resolve/format/FDM paths and maintenance10 activation; existing guarded sample-center, depth-range and selected-bin paths; complete shader API/cache/GPL/GPU behavior. The327 scalar getter-store inventory is a partial static aid, not a full feature audit. No build or full validation has started for this source.


## 2026-10-19 — Cooperative memory policy and complete getter field map

Source-only follow-up after the a69bd574 device build. Original feature getter0x3ba28290 returns physical bytes0x3cd/0x3ce, both set by threadsize_base==64 && !expose_double_threadsize. The local unconditionalfalse cooperativeMatrixRobustBufferAccess is replaced by the same cooperativeMatrix predicate. This restores original feature metadata; it does not prove robust buffer behavior. Ordinary matrix memory lowering and buffer robustness-key selection already follow the inspected original paths; no separate matrix descriptor backend was found there. Required subgroup-size interaction, OOB/robustness1/2/pipeline overrides, FP/rounding/NaNs and GPU/API behavior remain for final validation.

Static getter mapping expanded from327 scalar records by333 packed/core/shared-tail records:660 member-store records, all274 selector values,55 core feature fields and571 distinct physical bytes. No named registry field is missing for those selector values. Physical byte0x3cc has no observed getter and is not assigned a feature name. This is complete coverage of the mapped getter-store forms, not an executed supported-feature inventory: initializer predicates, SDK/platform filtering, APIs and GPU semantics are not proved. Evidence: current-feature-getter-source-map.json, original-packed-feature-getters.json.

Primitive-key source correction: inspected original builder copies IA class and provoking value without the local dynamic-bit gates; inspected A8 draw_common only consumes pervertex/barycentric metadata for PC/VPC ordering. An original runtime variant selector has not been established. Keep the local unspecified dynamic key and producer-derived GS/TES classes as explicit adaptations for final evaluation, without claiming a missing original selector must be invented.

User correction2026-10-19: installed a69bd574 has NOT been tested yet. The earlier smoothness message was sent by mistake and is retracted. This source pass leaves that installed build unchanged.


2026-10-19 additional confirmed transfer: the26-option original drirc parser inventory still lacked tu_advertise_wave32_compat and tu_uncharted_legacy_adreno_compat. Both original description defaults arefalse (ELF0xcebe88/0xcebff8). Transferred Wave32 compatibility property/key policy (not native Wave32), A6/A7 Uncharted FP16/descriptor-buffer suppression and A7 SSBO fallback, plus the exact two Uncharted process/app rules and007 First Light restrict64+Wave32 rule from embedded driconf. No global enabling or guessed application matching. These compatibility paths remain unbuilt and unvalidated. Evidence: wave32-uncharted-source-transfer.json, wave32-uncharted-original-config.json.


2026-10-19 profile completion: recovered all23 embedded Turnip profiles (16application+7engine). Added the three missing mesh-disable rules for Ninja Gaiden2Black and Onimusha process/app names. Static rule/options comparison now finds no missing/different original Turnip profile, while additional local rules remain. This is not matcher/runtime/game validation. All26 original parser option names now exist locally; option implementation completeness is not inferred solely from their names.


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


2026-10-24 sparse FS/AQE continuation: restored original FS address-base guard around CP immediate-to-constant lowering, RT aliases and immediate-constant-to-preamble conversion. Predicate is FRAGMENT && (primitive_base || layer_viewport_base), not mesh-map-enabled. Ordinary CP and texture register aliasing remain. Coordinated policy0x47533810 supersedes0f; source only. See sparse-aqe-hour-20261024/mesh-fs-constant-policy-source-evidence.json. Full compiler/GPU/ABI verification stays deferred.

2026-10-24 AQE caller continuation: native drawCount/maxDrawCount zero exits restored before draw setup/resource/predicate allocations in both indirect entrypoints. Direct mesh dispatch and nonzero paths unchanged. Evidence sparse-aqe-hour-20261024/aqe-zero-draw-source-evidence.json; policy0x47533810, no build/tests.

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

Original sysval47 and intrinsic0x2f0 are independently named by ELF tables as SYSTEM_VALUE_WORKGROUP_ID and load_workgroup_id; numeric local-ABI assumptions are not used. Shared register0xcf is not a CP shared-GFX constant address. Ordinary RA handles precolor; the separate shared-RA input preference was missing and is restored in the subsequent policy1c continuation. Runtime validation remains deferred. All four TU UUID/graphics/compute/IR3 cache policy sites move together to0x47533815, because generated code/register placement changes. TASK/MESH API/compiler gates stay held. No build, shader execution, GPU test or performance claim. Evidence and final cases: docs/gamesir-recovery/workgroup-id-copies.json.


## 2026-10-24 — Subgroup execution-mode/dispatch source transfer

Original spirv_to_nir writes shader-info byte0x196 bit5 for SubgroupUniformControlFlowKHR(mode4421) and bit4 for MaximallyReconvergesKHR(mode6023). In original ir3_nir_post_finalize, COMPUTE/TASK/MESH/KERNEL select linear dispatch when maximally_reconverges OR (subgroup_uniform_control_flow AND has_dual_wave_dispatch). The existing derivative_group==LINEAR rule is separate. Native property info+0x2bd is independently named has_dual_wave_dispatch by the debug-parser string/field route. Earlier notes labeling this QUADS behavior were incorrect.

The local frontend previously ignored SubgroupUniformControlFlowKHR. Add a named shader_info boolean, retain the execution mode and restore the native IR3 dispatch predicate. Whole-struct NIR clone/serialization already carry this metadata; no duplicate serializer path is added. Four TU/IR3 UUID/key sites advance together from15 to16 because both generated code and serialized shader_info bit layout can change. No feature-gate activation, driver/shader build, GPU test or measured performance claim. Final cases and original anchors: docs/gamesir-recovery/subgroup-dispatch.json.


## 2026-10-24 — Reconcile the compute/fragment execution-class helper

Native ir3_shader_get_subgroup_size includes FS4/CS5/TASK6/MESH7/KERNEL14 for SINGLE_OR_DOUBLE. The local is_compute_or_frag helper still excluded TASK/MESH even though ir3_should_double_threadsize already included them. The same native set also guards nir_opt_phi_precision, the doubled soft-SY scheduling estimates and exclusion from the legacy SAMGQ workaround. Restore the shared helper using mesa_shader_stage_uses_workgroup(type) || FRAGMENT. This preserves the ordinary stage controls while reconnecting all affected TASK/MESH consumers. Four cache ABI/key markers advance16→17 because compilation/scheduling decisions change. No gates enabled, build or GPU execution. Original anchors and final cases: docs/gamesir-recovery/compute-frag-stages.json.


## 2026-10-24 — BAR and split-barrier legalize accounting

Native ir3_required_sync_flags ELF0xad604c forces SS|SY for A8 BAR in addition to SHPE, and returns zero for BAR_SET/BAR_SYNC. Native sync_update ELF0xad7bfc retires both SS/SY dependency masks for the split barriers without requiring flags, and excludes them from ordinary next-instruction barrier forcing. Native ir3_update_legalize_state ELF0xad7b0c clears fixed ALU/non-ALU/predicate/address readiness while retaining the current block cycle. Restore this coupled policy using named fields. Existing fixed ISA encodings stay unchanged; no new split barrier insertion or feature activation. Four cache policy sites advance17→18. Source-only; final compiler/API/cache/GPU validation deferred. See docs/gamesir-recovery/split-barrier-sync.json.


## 2026-10-24 — Preserve MESH output lowering and A8 store selection

Native ir3_nir_lower_io_vars_to_temporaries ELF0xa7c2d8 excludes MESH in addition to TCS from ordinary output-temporary lowering. Restore the missing MESH exclusion so its arrays/per-primitive data reach their dedicated lowering intact. Native emit_intrinsic_store_global ELF0xac8ce8 uses STG_A for every A8 MESH global write and chooses S16 for bit16, S32 otherwise. For a previously immediate-only address the original transfers the entire immediate into the register offset, and uses zero shift and zero STG_A immediate; preserve that instead of truncating a 13-bit STG immediate into the 8-bit STG_A field. TASK/ordinary stages and older gens keep their original local policy. Four cache markers18→19. No builds/tests/GPU or capability activation. Evidence/final cases: docs/gamesir-recovery/mesh-output-store.json.


## 2026-10-24 — KGSL GPU timestamp reader

Original kgsl_device_get_gpu_timestamp ELF0xa1effc issues IOCTL_KGSL_READ_CALIBRATED_TIMESTAMPS0xc0200960 with one DEVICE-domain source, caller output, zero deviation and padding, and safe EINTR/EAGAIN retries. The named 32-byte ABI already exists in msm_kgsl.h, but the recovered backend callback was still UNREACHABLE. Restore the actual request through safe_ioctl. Existing common timestamp hook, extension predicates and unsupported-kernel error propagation remain unchanged. Compiler/cache policy19 unchanged. No build/kernel/API execution. Evidence and final cases: docs/gamesir-recovery/kgsl-timestamp.json.


## 2026-10-24 — Host image-copy mapping fallback

Original CopyMemoryToImageEXT ELF0x8fd9a8, CopyImageToMemoryEXT0x90117c and CopyImageToImageEXT0x901a9c ensure image.map before accessing region memory. Missing mem/BO returns MEMORY_MAP_FAILED; a missing BO map calls tu_bo_map and propagates its result; successful image pointer includes mem_offset. Image-image checks source then destination. Restore this shared helper at all three entrypoints. BindImageMemory already maps ordinary HOST_TRANSFER/FDM images, so the missing fallback alone is not proof of a valid-API rendering bug. Copy formats/pitches/cache/LRZ behavior and mapping ownership remain unchanged; no cache schema bump. Final mapping/imported-memory/host-synchronization/API validation deferred. See docs/gamesir-recovery/host-image-map.json.


## 2026-10-24 — TU workgroup preprocessing bundle

Native tu_lower_nir ELF0xa0842c excludes all workgroup stages (CS/TASK/MESH/KERNEL mask0x40e0) from layered FDM, not only COMPUTE. Its shared-memory preprocessing reconciles declared size against load_shared/store_shared/shared_atomic/shared_atomic_swap offset upper bounds before zero initialization: consider known end addresses only up to min(initial_size*4, device.cs_shared_mem_size), grow only, align to16. Original intrinsic ordinals were named from the ELF table; no local ordinal substitution. Its second compute-system-values pass runs for all workgroup stages with has_base_workgroup_id=(stage!=MESH), after zero initialization. Restore these coupled policies. Existing TASK/MESH prepare pass is retained; that earlier pass does not replace the later post-zero-init stage. Four compiler/cache keys19→1a; no build, shader/GPU execution or feature activation. Exact anchors, limits and deferred cases: docs/gamesir-recovery/tu-workgroup-preprocess.json.


## 2026-10-24 — Independent carry correction, not a GameSir transfer

Both native ir3_nir_lower_const_global_loads ELF0xa87e08 and the recovered copy_global_to_uniform compared the old low32 address with start before adding start to it. That is not the carry of unsigned addition: e.g. low0xfffffff0+start0x800 wraps to0x7f0 with carry1, while the old comparison returns0. Conversely low0x100+0x800 has no carry but the old comparison returns1. Compute low32 sum first, then compare the wrapped sum with start and add the carry to high32. Keep this explicitly separate from native parity; the original shares the defect. Four cache markers1a→1b. Cases are manually derived arithmetic, not executed shader/GPU tests; full validation remains deferred. See docs/gamesir-recovery/global-preamble-carry.json.


## 2026-10-24 — Complete shared-allocator precolor preservation

Native shared get_reg ELF0xae4d44 first tries a META_INPUT requested register when it is not INVALID_REG, fits the register file and get_reg_specified accepts it. That branch was absent locally. Ordinary ir3_ra has its own precolored-input handling, but ir3_ra_shared can run first for shared vectors or pressure and replace num; the ordinary path alone was not sufficient evidence for WorkgroupID0xcf. Restore the native preference and its unchanged generic fallback. Correct the previous overly broad RA statement. Four cache markers1b→1c, source-only; gates unchanged and final RA/compiler/GPU checks deferred. See docs/gamesir-recovery/shared-input-precolor.json.


## Late FS input hoisting: native transfer, policy0x4753381d (unbuilt)

Восстановлен `ir3_nir_move_varying_inputs` (ELF0xa93918), его рекурсивная проверка зависимостей0xa93b98 и перенос0xa93e3c. Объявление уже было в ir3_nir.h, но реализации, Meson-источника и позднего вызова не было. Вызов теперь после lowering barycentric-at-offset, до framebuffer/layer lowering; ранний generic move-to-top это не заменяет. Оригинальная таблица intrinsic_infos независимо дала три цели:0x1c2 load_input,0x1cb load_interpolated_input,0x1f3 load_per_primitive_input.

Сначала проверяется весь shader: зависимости вне entry могут быть ALU/deref/constant/undef либо reorderable intrinsic; phi/texture/непереставляемая операция отменяет весь проход до изменений. Затем зависимости и входы добавляются в entry в порядке зависимостей. Сохраняется control_flow metadata0x83. Локальная адаптация явно отделена от оригинала: возвращается фактический progress, тогда как native функция возвращаетfalse даже после nir_progress. Четыре cache/compiler markers1c→1d.

Доказательства: docs/gamesir-recovery/fs-input-hoist.json и native-complete/rutabaga-execution-hour-20261024/fs-input-hoist-native.arm64.txt. Только статическое восстановление и diff --check; без сборки, исполнения NIR/шейдеров и GPU. Полная проверка на конец: условные/циклические и sparse/per-primitive FS входы, phi/texture abort без частичного переноса, ordering/metadata, давление регистров, prefetch/VPC и игры. Ускорение пока не измерено.


## 25 октября — A8 next-subpass synchronization (без сборки)

Native CmdNextSubpass2<A8> ELF0x949b9c/768 байт после feedback scheduling выдаёт pending renderpass flushes, затем SUBPASS_FENCE(0x35) → WFI → CACHE_INVALIDATE7(0x33), до next renderpass state/subpass begin. Оригинальные пары данных: ELF0xb0688=(0x70460001,0x35),0xaf8e0=(0x70460001,0x33); WFI header0x70268000. Восстановлено в draw_cs только для A8; SUBPASS_SLICE_FENCE(0x36) не подставляем. Existing feedback helper только ставит условные cache/stage flags и не заменяет эту последовательность.

Это перенос native совместимости, не доказанное исправление игры или ускорение: full WFI/invalidate может снижать throughput. Остальные entry points не менялись. Cache policy1d сохранена: shader/code/serialization не меняются. Доказательство docs/gamesir-recovery/next-subpass-sync.json + compact ARM excerpt. Финальная проверка позже: GMEM/sysmem, input/no-input/feedback, pending/debug flushes, secondary replay, A6/A7 и latency/нагрев.


## 25 октября — A8 MRT layers / raw GRAS_CL_ARRAY_SIZE

Native tu6_emit_mrt<A8> ELF0x97ac9c/1016: max(framebuffer.layers,bit_length(multiview_mask|1),non-null non-unused color/depth-stencil view.layer_count), затем raw register0x8204=count без -1. A6/A7 native0x8004=count-1 остаётся отдельным контролем. Поля независимо привязаны к производителям: tu_CreateFramebuffer сохраняет API layers в+0x48; vk_image_view_init пишет resolved layerCount в+0x7c (включая REMAINING_ARRAY_LAYERS и 2D-view-of3D), это не заранее уменьшенные значения.

Восстановлены оба A8 отличия вместе; raw .dword сохраняет original unmasked write. Особенно спорная граница2048: XML11bit, но native не обрезает count2048; аппаратная причина и корректность не доказаны, это не общий off-by-one fix. Выделенный коммит позволит сравнить/откатить отдельно после полной проверки. Shader cache1d не меняется; без сборки/GPU/feature activation. Доказательства docs/gamesir-recovery/mrt-layers.json + bounded A6/A7/A8 ARM. Финальные кейсы: framebuffer/view mismatch, multiview holes, unused remap/null views, combined DS и max layers.


## 25 октября — RGB32 float format query exception

Native format query ELF0x998b7c/1568 отдельно пропускает VkFormat106=R32G32B32_SFLOAT при texture support, несмотря на общую NPOT ветку. ELF0x998ce0..0x998d10 добавляет sampled|transfer_src|transfer_dst (0xc001) и для float linear filter (0xd001). Восстановлена только эта ветка; не добавляются blit/minmax/storage/HIC и не расширяется поддержка всех RGB/NPOT форматов. Native tiling_possible не исключает RGB32, поэтому local linear/optimal publication сохранена. Depth/HIC проверка совпадает с util_format_has_depth, отдельного пропущенного stencil fix нет.

Это перенос feature advertisement, не доказанная работоспособность всех RGB32 путей. На финал остаются layout/stride/mips/arrays/3D, sampling/filtering/copies, A702 и UINT/SINT controls, CTS/игры. Shader/cache1d неизменен; сборки и GPU нет. Доказательства docs/gamesir-recovery/rgb32-format.json + bounded ARM excerpt.


## 25 октября — KGSL timeline backend probe

Native kgsl_native_timeline_supported ELF0xa1d8bc/284 требует успешных CREATE(0xc0100958) и DESTROY(0x4004095d), оба с EINTR/EAGAIN retry. Локально результат DESTROY игнорировался и backend выбирался только по CREATE. Восстановлен named helper и оба условия; existing legacy fallback/private queue timeline ordering остаются. Это не полноценная signal/wait проверка возможностей, не новый backend и не ускорение. При ошибке kernel destroy утечка probe до fd close возможна и в original — дополнительную семантику очистки не выдумываем.

Policy1d неизменна; без сборки/тестов/устройства. На финал: комбинации ioctl success/failure/retry, cleanup и реальные KGSL waits/signals. Доказательство docs/gamesir-recovery/kgsl-timeline-probe.json и bounded ARM.


## 25 октября — dual-source FS output register fallback

Native tu6_emit_fs_outputs A6/A7/A8 имеет одинаковую дополнительную ветку: dual_src && reg0==0xfc && reg1!=0xfc -> reg0=reg1 перед emission. Точки ELF0xa14330/0xa15da4/0xa1796c. Byte variant+0x302 независимо привязан к dual-source через SP_PS_OUTPUT_CNTL0xa98c bit0 и RB_PS_OUTPUT_CNTL0x880b bit3. Локальный ir3_alias guard без DATA0 уже есть, но не заменяет эту настройку аппаратных выходов. Перенесено только копирование packed regid с half-bit; alias nibbles/output count не меняются, render-component masks вычисляются после fallback как в original.

Соседний original stencil lookup проверяет raw slot13 после1; meaning/producer13 не установлен (native names table покрывает0..12). Не подставляем локальный enum по догадке; это отдельный оставшийся вопрос. Policy1d сохранена: shader/serialized metadata не меняются. Без сборки/исполнения/GPU. Финал: dual-source only-source1/both, half/full registers, blending/color masks/GPL, A6/A7/A8. Доказательства docs/gamesir-recovery/dual-source-output.json и ARM excerpts.


## 26 октября — original secondary stencil reader

Закрыта оставшаяся ветка reader в tu6_emit_fs_outputs A6/A7/A8: сначала искать metadata slot1, только при отсутствии записи — raw slot13, затем regid либоINVALID0xfc. Наличие slot1 с invalid regid НЕ включает fallback. После typed original exports контракт reader однозначен; его восстановление не требует выдумывать frontend meaning13. В NIR/SPIR-V enums/API/features ничего не добавлено, writes_stencilref не меняется. Происхождение и достижимость raw13 остаются вопросом финальной проверки, но сам original reader перенесён.

Policy1d сохранена; без сборки/GPU. Доказательства docs/gamesir-recovery/stencil-fallback-reader.json и ранее сохранённый dual-source-output-native.arm64.txt.


## 2026-10-27 — Device-policy and mesh-output follow-up

Restore the original VPC writer order: test the merged mask byte for zero, then emit only its low nibble; a high-only byte still emits a zero-compmask record. Keep the32record bounds supported by the native producer scratch. Restore independent MSAA policy flags instead of replacing them with generation predicates; A8 defaults true/true and older defaults false/false, matching original tables. Restore eight original FD_DEV_FEATURES spellings as aliases to existing Mesa QCTDD quirk fields. No shader/cache payload changes; policy1d retained.

Complete machine listing remains available. C-export failures are a generated feature checker and seven format utilities, whose lane arithmetic/source families were inspected without pretending to recover successful C. Three boundary-label cases remain: a CortexA53 erratum veneer, an invalid-deref diagnostic boundary, and an explicit-I/O terminal call whose path contract is not fully established. Do not fabricate a no-return builder merely to improve export counts. Full row/alias/tail checks, symbolic branch feasibility, caller-array paths and integrated driver validation remain open.
