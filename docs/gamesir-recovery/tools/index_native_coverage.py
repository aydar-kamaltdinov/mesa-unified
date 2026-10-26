"""Static ELF64 inventory. Never loads or executes the target shared library.

Exact bytes are useful positive matches; differing bytes are NOT semantic diffs
(addresses, compiler decisions and relocations also change instructions).
Ghidra export presence is not a claim that its recovered C is correct or audited.
"""
import hashlib
import os
import json
from pathlib import Path
import re
import struct

ROOT = Path(os.environ.get("TURNIP_RECOVERY_ROOT", Path(__file__).resolve().parents[1])).resolve()
DECOMP = Path('/Users/aydar.kamaltdinov/Documents/claude/tools/decomp')


def elf(path):
    raw = path.read_bytes()
    assert raw[:6] == b'\x7fELF\x02\x01'
    assert struct.unpack_from('<H', raw, 18)[0] == 183
    off = struct.unpack_from('<Q', raw, 40)[0]
    ent, count, names_idx = struct.unpack_from('<HHH', raw, 58)
    headers = [struct.unpack_from('<IIQQQQIIQQ', raw, off + i * ent)
               for i in range(count)]
    nh = headers[names_idx]
    names = raw[nh[4]:nh[4] + nh[5]]

    def string(data, pos):
        return data[pos:data.index(b'\0', pos)].decode('utf8', 'replace')

    sections = [dict(name=string(names, h[0]), type=h[1], flags=h[2],
                     address=h[3], offset=h[4], size=h[5], link=h[6],
                     entsize=h[9]) for h in headers]
    functions = []
    # Prefer the complete symtab. Dynsym would duplicate exported symbols.
    for section in sections:
        if section['type'] != 2:
            continue
        strings = sections[section['link']]
        strings = raw[strings['offset']:strings['offset'] + strings['size']]
        source_file = None
        for offset in range(section['offset'], section['offset'] + section['size'],
                            section['entsize']):
            name, info, other, shndx, value, size = struct.unpack_from('<IBBHQQ', raw, offset)
            if info & 15 == 4:
                source_file = string(strings, name)
            if info & 15 != 2 or not 0 < shndx < len(sections):
                continue
            target = sections[shndx]
            if not target['flags'] & 4:
                continue
            start = target['offset'] + value - target['address']
            assert target['address'] <= value <= target['address'] + target['size']
            assert value + size <= target['address'] + target['size']
            functions.append(dict(name=string(strings, name), address=value,
                                  size=size, section=target['name'],
                                  binding=info >> 4,
                                  source_file=source_file if info >> 4 == 0 else None,
                                  sha256=hashlib.sha256(raw[start:start + size]).hexdigest()))
    return raw, sections, functions


def main():
    result = {'limits': [
        'All executable sections are exported by llvm-objdump; embedded data is retained.',
        'Function boundaries use ELF STT_FUNC symbols, not a proof that all code is symbolized.',
        'Ghidra C presence only; no semantic audit or source-port completeness inferred.',
        'Different raw hashes do not establish a GameSir-specific change.',
    ]}
    indexes = {}
    for label, filename, directory in [('gamesir', 'gs_rutabaga_v3.so', 'full_gs'),
                                        ('upstream', 'up_gsbase.so', 'full_up')]:
        raw, sections, functions = elf(ROOT / 'originals' / filename)
        exports = {}
        for path in sorted((DECOMP / directory).glob('*.c')):
            head = path.read_text(errors='replace')[:2048]
            match = re.search(r'// (.+?) @ ([0-9a-fA-F]+)', head)
            if match:
                exports.setdefault(int(match[2], 16) - 0x100000, []).append(str(path))
        for fn in functions:
            fn['ghidra_exports'] = exports.get(fn['address'], [])
        functions.sort(key=lambda fn: (fn['address'], fn['name']))
        indexes[label] = functions
        executable = [s for s in sections if s['flags'] & 4]
        intervals = sorted((f['address'], f['address'] + f['size']) for f in functions if f['size'])
        merged = []
        for lo, hi in intervals:
            if merged and lo <= merged[-1][1]:
                merged[-1][1] = max(merged[-1][1], hi)
            else:
                merged.append([lo, hi])
        result[label] = dict(binary_sha256=hashlib.sha256(raw).hexdigest(),
                             executable_sections=executable,
                             executable_bytes=sum(s['size'] for s in executable),
                             function_symbols=len(functions),
                             unique_function_addresses=len({f['address'] for f in functions}),
                             symbol_function_bytes=sum(hi-lo for lo, hi in merged),
                             symbols_with_ghidra_export=sum(bool(f['ghidra_exports']) for f in functions),
                             ghidra_exports_matched_addresses=len(exports))
        (ROOT / 'native-complete' / (label + '-functions.json')).write_text(
            json.dumps(functions, indent=2) + '\n')
    def identity(fn):
        return (fn['source_file'], fn['name'])
    upstream = {}
    for fn in indexes['upstream']:
        upstream.setdefault(identity(fn), []).append(fn)
    counts = {}
    frontier = []
    for fn in indexes['gamesir']:
        matches = upstream.get(identity(fn), [])
        state = ('no_same_named_baseline_symbol' if not matches else
                 'identical_bytes' if fn['size'] and any(
                     f['size'] == fn['size'] and f['sha256'] == fn['sha256'] for f in matches)
                 else 'different_bytes_semantics_unresolved')
        fn['raw_comparison'] = state
        counts[state] = counts.get(state, 0) + 1
        if not fn['ghidra_exports'] and re.search(
            r'mesh|task|aqe|ir3_|tu6_|tu7_|tu8_|tu_nir|pipeline|shader', fn['name'], re.I):
            frontier.append(fn)
    frontier.sort(key=lambda f: -f['size'])
    result['raw_symbol_comparison'] = counts
    result['comparison_identity'] = 'STT_FILE plus name for local definitions; name for global/weak definitions'
    (ROOT / 'native-complete' / 'gamesir-functions.json').write_text(json.dumps(indexes['gamesir'], indent=2)+'\n')
    result['unexported_named_frontier_count'] = len(frontier)
    (ROOT / 'native-complete' / 'unexported-frontier.json').write_text(json.dumps(frontier, indent=2)+'\n')
    (ROOT / 'native-complete' / 'coverage.json').write_text(json.dumps(result, indent=2)+'\n')
    print(json.dumps(result, indent=2))
    print('Largest relevant functions lacking a matching saved Ghidra export:')
    for fn in frontier[:25]:
        print(hex(fn['address']), fn['size'], fn['name'], fn['raw_comparison'])


if __name__ == '__main__':
    main()
