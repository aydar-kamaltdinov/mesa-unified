"""Independent contract models for the second GameSir source audit.

No C/NIR/ISA, kernel ioctl, driver or GPU is executed. Source markers bind
the checks to the reviewed edits, without claiming compiled equivalence.
"""

import hashlib
import itertools
import json
import math
from pathlib import Path
import random
import struct

ROOT = Path(__file__).resolve().parents[3]


def f32(value):
    return struct.unpack('<f', struct.pack('<f', value))[0]


def reciprocal_neighbor(value, ulps):
    bits = struct.unpack('<I', struct.pack('<f', value))[0]
    return struct.unpack('<f', struct.pack('<I', bits + ulps))[0]


# AQE's documented index domain is [0,2^22). Check the two corrections
# against integer division, including +/-1 ULP around a rounded reciprocal.
# This does not establish the actual GPU reciprocal error bound.
rng = random.Random(0x4753381f)
division_cases = 0
for denominator in [1, 2, 3, 7, 31, 64, 255, 256, 1023, 8191, 65535]:
    numerators = [0, 1, (1 << 22) - 1, (1 << 22) - 2]
    numerators += [rng.randrange(1 << 22) for _ in range(256)]
    for numerator in numerators:
        for ulps in [-1, 0, 1]:
            reciprocal = reciprocal_neighbor(f32(1.0 / denominator), ulps)
            estimate = math.trunc(f32(f32(numerator) * reciprocal))
            if numerator < estimate * denominator:
                estimate -= 1
            if numerator >= (estimate + 1) * denominator:
                estimate += 1
            assert estimate == numerator // denominator
            division_cases += 1


# Resource addresses come from a single0x2073000-byte device BO. Enumerate
# both alternating banks, rather than treating ordinal as an unbounded index.
bo_size = 0x2073000
ranges = [(0, 0x1800000), (0x1800000, 0x40000),
          (0x1850000, 0x800000), (0x2050000, 0x3000)]
ranges += [(0x2053000 + bank * 0x10000, 0x10000) for bank in range(2)]
for start, size in ranges:
    assert 0 <= start <= start + size <= bo_size
for (start_a, size_a), (start_b, size_b) in itertools.combinations(ranges, 2):
    assert start_a + size_a <= start_b or start_b + size_b <= start_a
assert max(start + size for start, size in ranges) == bo_size

# The capacity formula bounds the combined vertex/primitive GMEM allocation
# by15MiB. Exhaust the scalar-stride/256-element envelope independently.
capacity_cases = 0
for total in range(1, (512 + 1024) * 256 + 1):
    capacity = 0x2000 if total < 0x781 else (0xf00000 // total) & ~1
    assert total * capacity <= 0xf00000
    assert capacity % 2 == 0
    capacity_cases += 1
assert 0x1068000 >= 0xf00000


# A per-vertex input moves ahead of UV, while a legacy prefetch would retain
# UV's old offset. The selected fallback keeps normal BARY/texture operations.
old_uv, new_uv = 0, 4
assert old_uv != new_uv
prefetch_cases = 0
for hardware, mesh, per_vertex in itertools.product([False, True], repeat=3):
    allowed = hardware and not mesh and not per_vertex
    if mesh or per_vertex:
        assert not allowed
    elif hardware:
        assert allowed
    prefetch_cases += 1

basis_cases = 0
for prefetch in range(0, 126):
    base = (prefetch + 2 + 3) & ~3
    if base + 3 > 128:
        continue  # the compiler rejects the transfer-mask overflow
    assert not (set(range(prefetch, prefetch + 2)) & set(range(base, base + 3)))
    basis_cases += 1
assert set(range(0, 2)) & set(range(0, 3))  # old prefetch-only counterexample


# The wait attempts below are scripted outcomes, not real ioctls. An expired
# finite wait must terminate after a retryable error, preserving success and
# fatal errors returned by the first nonblocking attempt.
def deadline_wait(deadline, attempts):
    for now_after_ioctl, result in attempts:
        if result not in ['EINTR', 'EAGAIN']:
            return result
        if deadline < (1 << 63) and now_after_ioctl >= deadline:
            return 'ETIMEDOUT'
    return 'retry'


wait_scenarios = [
    (0, [(10, 'EINTR')], 'ETIMEDOUT'),
    (0, [(10, 'EAGAIN')], 'ETIMEDOUT'),
    (0, [(10, 'success')], 'success'),
    (0, [(10, 'EIO')], 'EIO'),
    (100, [(25, 'EINTR'), (99, 'EAGAIN'), (100, 'EINTR')], 'ETIMEDOUT'),
    (100, [(99, 'EAGAIN'), (105, 'success')], 'success'),
    ((1 << 64) - 1, [(200, 'EINTR'), (300, 'EAGAIN'), (400, 'success')], 'success'),
]
for deadline, attempts, expected in wait_scenarios:
    assert deadline_wait(deadline, attempts) == expected


# NONE must preserve the unused aspect, including packed D24S8 and separate
# D32S8 planes. A single present attachment is valid regardless of the
# independentResolveNone restriction on two present attachments (VUID06104).
resolve_cases = 0
for depth_enabled, stencil_enabled in [(True, False), (False, True), (True, True)]:
    for old_depth in [0, 1, 0x00ffff, 0xffffff]:
        for old_stencil in [0, 1, 127, 255]:
            packed = old_depth | old_stencil << 24
            source_depth, source_stencil = 0x123456, 0xab
            mask = (0xffffff if depth_enabled else 0) | (0xff000000 if stencil_enabled else 0)
            source = source_depth | source_stencil << 24
            output = (packed & ~mask) | (source & mask)
            assert output & 0xffffff == (source_depth if depth_enabled else old_depth)
            assert output >> 24 == (source_stencil if stencil_enabled else old_stencil)
            separate = [old_depth, old_stencil]
            if depth_enabled:
                separate[0] = source_depth
            if stencil_enabled:
                separate[1] = source_stencil
            assert separate == [output & 0xffffff, output >> 24]
            resolve_cases += 1


# Nonzero byte BASE must be included before alignment. This independently
# compares byte-addressed extraction with the aligned window, including
# wrap at the 32-bit address boundary. It is not execution of NIR lowering.
base_alignment_cases = 0
for alignment in [2, 4, 8, 16]:
    for base in range(64):
        for offset in list(range(64)) + [0xfffffffd, 0xfffffffe, 0xffffffff]:
            low = base & (alignment - 1)
            new_base = base - low
            chunk = (offset + low) & 0xffffffff
            window = (new_base + (chunk & ~(alignment - 1))) & 0xffffffff
            address = (base + offset) & 0xffffffff
            assert (window + (chunk & (alignment - 1))) & 0xffffffff == address
            assert window % alignment == 0
            base_alignment_cases += 1

# Modulo a power-of-two subgroup, a mask retaining every lane bit and
# arbitrary uniform adds/subtracts preserve a rotate, even across uint32
# overflow. Compare expression evaluation to the accumulated offset oracle.
shuffle_cases = 0
for subgroup in [32, 64, 128]:
    for _ in range(256):
        operations = []
        offset = 0
        for _ in range(rng.randrange(1, 8)):
            op = rng.choice(['add', 'sub', 'mask'])
            value = rng.randrange(1 << 32)
            if op == 'mask':
                value |= subgroup - 1
            elif op == 'add':
                offset += value
            else:
                offset -= value
            operations.append((op, value))
        for lane in range(subgroup):
            index = lane
            for op, value in operations:
                if op == 'mask':
                    index &= value
                elif op == 'add':
                    index = (index + value) & 0xffffffff
                else:
                    index = (index - value) & 0xffffffff
            assert index % subgroup == (lane + offset) % subgroup
            shuffle_cases += 1

# Sparse physical producer locations and compact LDLV locations deliberately
# differ. The consumer permutation must be bijective within its 128-scalar
# envelope and keep every component's relative position. VPC retains the
# physical semantic map; it is not required to numerically equal LDLV.
sparse_cases = 0
for count in range(1, 33):
    for _ in range(32):
        physical = rng.sample(range(32), count)
        per_vertex = [rng.choice([False, True]) for _ in range(count)]
        order = sorted(range(count), key=lambda i: not per_vertex[i])
        compact = {i: n * 4 for n, i in enumerate(order)}
        mapped = []
        for i in range(count):
            components = rng.randrange(1, 5)
            for component in range(components):
                original = physical[i] * 4 + component
                relocated = compact[i] + (original - physical[i] * 4)
                assert 0 <= relocated < 128
                assert relocated % 4 == component
                mapped.append(relocated)
        assert len(mapped) == len(set(mapped))
        sparse_cases += 1


paths = ['src/freedreno/ir3/ir3_context.c',
         'src/freedreno/ir3/ir3_compiler_nir.c',
         'src/freedreno/ir3/ir3_a6xx.c',
         'src/freedreno/vulkan/tu_knl_kgsl.cc',
         'src/freedreno/vulkan/tu_clear_blit.cc',
         'src/freedreno/vulkan/tu_mesh_dispatch.h',
         'src/freedreno/vulkan/tu_mesh_dispatch.cc']
source = {path: (ROOT / path).read_text() for path in paths}
assert '!has_per_vertex_fragment_inputs(ctx->s)' in source[paths[0]]
assert '!so->shader_options.mesh_fs_inputs.enabled' in source[paths[0]]
assert 'end = MAX2(end, instr->prefetch.input_offset + 2);' in source[paths[1]]
assert 'a8_mesh && bit_size >= 16' in source[paths[2]]
assert source[paths[3]].count('if (get_relative_ns(abs_timeout_ns) == 0)') == 2
assert 'store_separate_stencil &= stencil_mode != VK_RESOLVE_MODE_NONE;' in source[paths[4]]
assert 'dst->format == VK_FORMAT_D24_UNORM_S8_UINT && depth_mode != stencil_mode' in source[paths[4]]
assert '#define TU_A8_MESH_AQE_SIZE 0x2073000u' in source[paths[5]]
policy_paths = ['src/freedreno/vulkan/tu_device.cc',
                'src/freedreno/vulkan/tu_pipeline.cc',
                'src/freedreno/ir3/ir3_disk_cache.c']
assert sum((ROOT / path).read_text().count('0x4753381f') for path in policy_paths) == 4

result = {
    'aqe_division_F32_and_neighbor_cases': division_cases,
    'aqe_reciprocal_hardware_accuracy_proved': False,
    'aqe_nonoverlapping_BO_ranges': len(ranges),
    'aqe_capacity_bound_cases': capacity_cases,
    'prefetch_gate_cases': prefetch_cases,
    'prefetch_basis_disjoint_cases': basis_cases,
    'scripted_wait_cases': len(wait_scenarios),
    'resolve_aspect_preservation_cases': resolve_cases,
    'byte_base_alignment_cases': base_alignment_cases,
    'shuffle_modulo_lane_cases': shuffle_cases,
    'sparse_semantic_permutations': sparse_cases,
    'source_sha256': {path: hashlib.sha256((ROOT / path).read_bytes()).hexdigest()
                      for path in paths + policy_paths},
    'driver_or_C_executed': False,
    'build': False,
    'limits': __doc__.splitlines(),
}
(ROOT / 'docs/gamesir-recovery/validation-interface-results.json').write_text(
    json.dumps(result, indent=2) + '\n')
print(json.dumps({k: v for k, v in result.items()
                  if k not in ['source_sha256', 'limits']}, indent=2))
