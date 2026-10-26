# Static GameSir reconstruction tools

These tools inspect the original ARM64 ELF and current source as data. They do
not load the driver, run shaders, build Mesa, enable features or contact a device.
They are the source for the recovery workspace scripts; no binary/C backups are
included here. Original SHA256:
`90c056d7919a1da8ef0fd3131403729b81b05fe3c7373e772e3ceaa522ad397e`.

Set `TURNIP_RECOVERY_ROOT` to the recovery directory containing
`originals/gs_rutabaga_v3.so`, `native-complete/` and
`mesa-gamesir-recovered/`. The byte-verified `gamesir.arm64.txt`, registry XML,
Ghidra export reports and named evidence directories are explicit inputs. Most
reconstruction helpers intentionally target this original binary/date/address
map. They are not generic parsers for an unverified future binary.

- `recover_native_device_property_names.py` recovers native debug names/offsets;
  `recover_native_device_records.py` reads79records/35infos and emits143typed
  header/property field locations. Neither imports the device generator.
- `compare_native_profile_declarations.py` reads a restricted profile AST;
  register constants are input metadata extracted from the existing register
  XML enum generator. Unknown syntax remains unresolved.
- `recover_native_feature_getter.py` follows the exact leaf getter with symbolic
  boolean inputs and concrete registry selectors. Every instruction word is
  checked against the ELF. All274selectors/660member records are mapped;
  byte0x3cc remains unnamed. This does not evaluate support/initialization.
- `recover_native_property_getter.py` preserves full-byte symbolic provenance,
  AArch64 field layouts and scalar/SIMD copies, including lane insertion. Four
  caller-array cases in the original inventory fail closed without assumptions;
  its explicit null-array records remain historical. The follow-up
  `recover_native_property_arrays.py` supplies symbolic input arrays and concrete
  counts/pointers for6routes in4structs,620bounded static cases. This includes
  unsigned32 capacities and independent paired pointers; no arbitrary aliasing
  or pointer-lifetime completeness is claimed. The646 preliminary field count was
  superseded by649 after correcting SIMD lane handling. No general CPU emulator
  or arbitrary dynamic-path completeness is claimed.
- `NativePhysicalExtensionTypes.java` applies the recovered original431extension,
  571feature,143device-info and649property field maps to analysis-only types.
  Ghidra addresses use the verified original image bias0x100000. The inspected
  physical prefix through0x20c0 is not a sizeof claim for a full derived object.
- `NativeExtensionStoreGraph.java` exports bounded SSA stores and control blocks.
  Default range is extensions0x58..0x207; optional range207..443 selects features.
  Constant-folding helpers preserve symbolic inputs, all writes and branch
  records. A last lexical write is not a general path/dominance proof.
- `NativeSymbolExport.java` and the positive tailcall/no-return helpers improve
  C export boundaries without changing ELF bytes. Successful decompilation and
  absence of outside labels do not establish source or GPU equivalence.

The remaining8failed C exports and3boundary-label cases are explicit. The
original machine listing is authoritative; do not fabricate successful C or
mark generic returning builders no-return to remove warnings. Original field
names/offsets must never be copied into the current Mesa ABI as numeric offsets.

For a future release, `compare_native_versions.py --old OLD.so --new NEW.so
--out NEW_EMPTY_DIRECTORY` produces named raw/branch-normalized comparisons,
a changed-function candidate queue, direct-caller context and allocated data
section changes. It does not erase ADRP/literal differences or equate ambiguous
names. Changed tables/callees/ABI can affect byte-identical callers; stripped
symbols/LTO/compiler changes can require broader matching. Reuse confirmed
unchanged material, then verify changed contracts and their dependencies.

Current outputs: the parent directory's complete getter maps, original-ABI TSVs,
`native-tables-20261027.json`, and each source-transfer proof. Larger raw/SSA
outputs remain in the recovery workspace, with digests in the compact manifest.
Final compiler/API/cache/GPU/game and performance validation remains deferred.

The2026-10-27 continuation also saves `recover_native_feature_checker.py`,
`reconcile_checker_guards.py`, `emit_native_checker_capsule.py` and
`NativeFeatureCheckerTypes.java`:715routes,275eligibility types, original
diagnostic order, explicit synthetic query replies and chain protocol. Seven
manual format helpers and three classified boundaries are recorded by the
corresponding `record_*` tools. Typed Ghidra C timeouts remain failures; no
further identical retries are required. The reconstructed C is analysis only.

`cleanup_recovery_runs.py` verifies the duplicate baseline against its Git tree
and preserves canonical selected C before removing superseded runs. It requires
`TURNIP_RECOVERY_ROOT` when invoked from this tracked tools directory. A compact
cleanup report records exactly what was removed; original binaries, current
source, full listings, unique SSA and Git history stay protected.

`record_feature_checker_site_coverage.py` plus
`classify_feature_checker_residuals.py` keep interpreted sites separate from
statically classified post-error/stack-cycle guards. They never fake corruption
to call an unexecuted path covered. `record_property_getter_site_closure.py`
records the unknown-sType skip remaining after caller-array recovery.

The incremental version tool now queues identical branch words that resolve
to different named destinations. `check_native_version_triage.py` checks moved
layout with same/different targets using in-memory instruction/symbol data; its
temporary report directories are automatically removed. No target code runs.

`recover_unsized_runtime_entries.py` preserves the two independent libunwind
assembly spans, while `recover_uncovered_linker_ranges.py` classifies all9
executable gaps and resolves234PLT entries to actual ELF relocation symbols.
Zero-sized labels at other sized starts are not automatically semantic aliases.
The incremental tool now emits `uncovered-executable-ranges.tsv` separately
from named C jobs, so unsized assembly/PLT/padding changes cannot vanish from
the comparison. Five static regression cases include a changed unsized span and identical
data bytes moved to another section address.

Allocated data sections retain their addresses as well as size/content hashes.
A moved section remains dependency-review evidence even when its bytes match;
these classes do not certify behavior of unchanged-looking code.

Fixed feature/property ABI models enforce original binary and registry hashes.
The feature checker also verifies its saved getter-map registry dependency;
regenerate and reconcile those inputs before adapting to another SDK layout.
The generic incremental ELF comparator remains usable on future binaries.
