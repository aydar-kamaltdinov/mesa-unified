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
  caller-array cases fail closed without assumptions; explicit zero-capacity,
  null-array cases are recorded separately. The646 preliminary field count was
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
