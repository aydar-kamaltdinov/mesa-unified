# Remaining native-function source handoff, 2026-10-27

The original ARM64 machine listing is complete. This is not a whole-driver
equivalence claim. The eight failed automatic-C exports now have bounded source
accounting: seven format helpers in `format-unpack.c`, and the generic feature
checker through 715 diagnostic routes, supported branches, 275 eligibility
formulas, diagnostic order and query-chain behavior. These paths already exist
in Mesa; their current generators are retained.

`feature-checker.c` stores the original field tables and isolated eligibility
and field-check kernels. It is an analysis artifact, not a build input or a
replacement for device creation. Its numeric extension IDs describe only the
original 431-byte table. Production keeps Mesa's newer KHR pipeline-library-
group-handles alias. Original diagnostic names and current SDK layout aliases
remain separate.

`property-arrays.c` records six array routes in four public structures. The
static ARM model recovers null/non-null count behavior, bounded copies,
unsigned capacities and independent paired output pointers. This supplements
the older null-query inventory; arbitrary aliasing, lifetime and real property
initialization remain outside the model.

Three symbol-boundary cases are classified: the Cortex-A53 displaced-load
veneer, the mediump unsupported-deref diagnostic and the explicit-I/O
unsupported SSBO-array-length address format. Returning generic helpers were
not marked no-return to suppress decompiler warnings.

Instruction accounting separates 23,232 observed checker sites from 1,153
unexecuted but statically classified sites: 605 post-diagnostic return jumps
and 274 pairs of internal query-stack cycle-guard returns. Unknown-site count
zero is an inventory result, not proof of every value, path or API behavior.
The property getter accounts for all 2,245 sites, including unknown-sType skip.

`libunwind-unsized.S` preserves 79 verified instructions from two independent
zero-size-symbol runtime spans. Their ends follow consecutive entries and
terminal RET instructions; original ELF sizes remain zero. Eight other
addresses have zero labels sharing sized starts. No semantic aliases or
capabilities are inferred from those labels. Other executable gaps are an
unnamed BTI/RET stub, 56 zero-padding bytes and a PLT resolver with 234
relocation-backed import entries. These are runtime/linker output, not missing
GPU algorithms or a reason to replace Mesa's compiler runtime.

Automatic-C metrics remain 7,683/7,691 successful exports, eight failures and
three outside-label cases. Readable manual C/ASM does not count as successful
automatic decompilation or formal equivalence. Models state their normal-memory,
little-endian and timing limits. Feature queries and diagnostic-only helpers
use explicit synthetic replies; no original SO, driver, shader or GPU ran.

No driver/component build was performed. Full compiler/API/cache/GPU/game and
performance validation of the existing transfer package remains deferred.
TASK/MESH and other held gates remain unchanged. See the parent handoff JSON
and each detailed proof for precise scope.
