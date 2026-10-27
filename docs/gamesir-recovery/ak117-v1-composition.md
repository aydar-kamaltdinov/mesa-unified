# AK1.17 + recovered GameSir V1 composition

Release branch/tag: `ak-turnip-1.17-gs-rutabaga`.

The recovered V1 source starts at an independent reconstructed stage66 snapshot,
then includes the GameSir V1 transfer and subsequent source-audit fixes. It
already contains the tested AK1.17 Stage21/22/29/34/41/42 work. It is not a clean
vendor source checkout, and its additional baseline changes are retained.

All88 reachable commits by our recorded author in AK1.17 were accounted for.
34 source patches are already present literally,25 are adapted/hardened in the
newer V1 code,2 empty historical commits have identical trees to their parent,
and8 old release-metadata changes are replaced. Superseded/reverted historical
experiments are evaluated against the final AK1.17 tree rather than replayed.
See `ak117-v1-composition.json` for every commit and the exact limits.

Two missing changes were cherry-picked with their original provenance:

- `b1ff928eb59` → `24acb9f6512`: remove the10% anti-GMEM bandwidth margin, restoring
  the final AK1.17 formula while preserving V1's newer depth-only pass heuristic.
- `88ad63ec154` → `d6cd3e5f69e`: restore A730 PC_MODE_CNTL to0x3f. Device testing of
  this package is limited to Adreno830; the A730 register change is untested here.

The existing corrected shader ownership, deadline retries, sparse-FS fallback,
depth/stencil resolves, cache keys and feature gates stay in the V1 base. No V2
compute-mesh/ring/cache-format changes are included. This accounting is not a
new exhaustive proof of every historical patch or full Vulkan conformance.

Build uses the existing locked SDK33 strict/API29 KGSL/Oryon-1 recipe. Binary
hashes, runtime checks, device deployment and the final English release message
are recorded with the release artifact and in Obsidian after the build.
