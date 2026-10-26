"""Record exact linker/undefined-domain boundary evidence without changing flow."""
from pathlib import Path
import json,re,hashlib
from index_native_coverage import ROOT,elf
F=ROOT/'native-complete';E=F/'rutabaga-remaining-two-hours-20261027';W=ROOT/'mesa-gamesir-recovered'
raw,secs,functions=elf(ROOT/'originals/gs_rutabaga_v3.so');instructions={}
assert hashlib.sha256(raw).hexdigest()=='90c056d7919a1da8ef0fd3131403729b81b05fe3c7373e772e3ceaa522ad397e', 'This reconstruction model is bound to the original v3 binary'
for line in(F/'gamesir.arm64.txt').read_text().splitlines():
 m=re.match(r'\s*([a-f0-9]+): ([a-f0-9]{8})\s+(\S+)\s*(.*)',line)
 if m:instructions[int(m[1],16)]=dict(word=int(m[2],16),op=m[3],args=m[4].split('//')[0].strip(),line=line)
def at(a):
 r=instructions[a];s=next(s for s in secs if s['type']!=8 and s['address']<=a<s['address']+s['size']);o=s['offset']+a-s['address'];assert raw[o:o+4]==r['word'].to_bytes(4,'little');return r
def source(p,needle):
 path=W/p;s=path.read_text();assert needle in s
 return dict(path=p,sha256=hashlib.sha256(path.read_bytes()).hexdigest(),line=s[:s.index(needle)].count('\n')+1)
def span(a,b):return[dict(elf=hex(pc),**at(pc))for pc in range(a,b,4)]
# Exact raw branch predicates; no no-return contract is applied to a builder.
assert at(0xb862d0)['args']=='w21, #0x6';assert at(0xb862d8)['op']=='b.gt'and at(0xb862d8)['args'].startswith('0xb8650c ')
assert at(0xb86510)['args']=='w21, #0x7';assert at(0xb86514)['op']=='b.ne'and at(0xb86514)['args'].startswith('0xb869bc ')
assert at(0xb85fc0)['args']=='w21, w2';assert at(0xb85ff8)['args']=='w8, w21, [sp, #0x18]'
assert at(0xb869c8)['args'].startswith('0xb582e8 <nir_build_alu1>')
assert at(0xce51d0)['args']=='x8, [x8, #0x18]';assert at(0xce51d4)['op']=='b'and at(0xce51d4)['args'].startswith('0x96b288 ')
assert at(0x96b284)['args'].startswith('0xce51d0 ');assert 0x96b288==0x96b284+4
report=dict(original_binary_sha256=hashlib.sha256(raw).hexdigest(),boundaries=[dict(name='CortexA53 erratum843419 veneer',elf='0xce51d0',classification='linker-generated displaced load; no missing driver feature',evidence=span(0xce51d0,0xce51d8)+span(0x96b278,0x96b298),contract='Caller B→veneer LDRx8[x8+24]→caller+4; no call/stack change.',source_change=False),dict(name='nir_lower_explicit_io terminal builder',elf='0xb85f94',classification='outside supported SSBO array-length address-format domain',original_constraints=['w21 initially receives format w2; copied to immutable SP+0x1c and restored around other loop uses.','Array-length conversion branches signed format>6 at0xb862d8, then!=7 at0xb86514; terminal path therefore format>=8.','Valid sized SSBO formats3/4 and index formats5/6/7 are handled earlier; address-to-index rejects other formats.'],evidence=span(0xb85fbc,0xb86000)+span(0xb862d0,0xb862e0)+span(0xb8650c,0xb86518)+span(0xb869bc,0xb869d0),source=[source('src/compiler/nir/nir_lower_explicit_io.c','addr_to_index('),source('src/compiler/nir/nir_lower_explicit_io.c','lower_explicit_io_array_length(')],limits=['Native bytes do fall into the following symbol on this invalid input path.','No site-specific fake RET, byte patch or global no-return builder contract applied.','This accounts for the boundary; it does not prove the entire NIR function equivalent or validate malformed NIR.'],source_change=False),dict(name='nir_lower_mediump_vars terminal diagnostic',elf='0xb9c43c',classification='unsupported dereference kind; diagnostic followed by source UNREACHABLE',evidence=span(0xb9c7cc,0xb9c7e4)+span(0xb9c848,0xb9c850)+span(0xb9c878,0xb9c884)+span(0xb9cb98,0xb9cbb4),source=source('src/compiler/nir/nir_lower_mediump.c','UNREACHABLE("unsupported deref type")'),limits=['Native normal return is0xb9cb94, before the cold diagnostic block.','nir_print_instr itself returns for normal callers; it is not marked globally no-return.','Boundary matches the unsupported-input source route; whole-function/GPU equivalence unproved.'],source_change=False)],raw_boundary_label_count_retained=3,flow_changes=False,build=False,full_validation_deferred=True)
(E/'native-boundary-classification.json').write_text(json.dumps(report,indent=2)+'\n');print('Recorded three raw/source boundary classifications; no flow or source patch.')
