"""Preserve two unsized libunwind spans as readable ARM64, never execute them.

Boundaries use consecutive independent entries plus terminal RET instructions,
not manufactured ELF sizes. Other zero-size labels overlap sized starts; this
is spatial evidence only, not callable aliases or an advertised capability.
"""
from pathlib import Path
import re,json,hashlib,collections
from index_native_coverage import ROOT,elf
from compare_native_versions import snapshot
R=ROOT;F=R/'native-complete';E=F/'rutabaga-remaining-two-hours-20261027';W=R/'mesa-gamesir-recovered';D=W/'docs/gamesir-recovery';raw,sections,functions=elf(R/'originals/gs_rutabaga_v3.so')
assert hashlib.sha256(raw).hexdigest()=='90c056d7919a1da8ef0fd3131403729b81b05fe3c7373e772e3ceaa522ad397e'
spans=[('__libunwind_Registers_arm64_jumpto',0xce5094,0xce5134,'gs_restored_unwind_jumpto'),('unw_getcontext',0xce5134,0xce51d0,'gs_restored_unw_getcontext')];code={}
for line in(F/'gamesir.arm64.txt').read_text().splitlines():
 m=re.match(r'\s*([a-f0-9]+): ([a-f0-9]{8})\s+(\S+)\s*(.*)',line)
 if m and any(lo<=int(m[1],16)<hi for _,lo,hi,_ in spans):
  pc=int(m[1],16);section=next(s for s in sections if s['type']!=8 and s['address']<=pc and pc+4<=s['address']+s['size']);offset=section['offset']+pc-section['address'];word=int(m[2],16);assert raw[offset:offset+4]==word.to_bytes(4,'little');code[pc]=(word,m[3],m[4].split('//')[0].strip())
text=['''/* Original GameSir libunwind assembly reconstruction, analysis only.
 * Not linked into Mesa; bundled compiler runtime remains authoritative.
 * Exact instruction words are checked against SHA-bound original ELF bytes.
 * Ranges are entry-to-next-entry spans with terminal RET, not ELF st_size.
 * GCS/BTI instruction words remain exact, including the conditional sequence;
 * no target execution, assembler build or runtime/GPU validation performed.
 */
.text
.p2align 2
'''];rows=[]
for name,lo,hi,label in spans:
 assert any(f['name']==name and f['address']==lo and not f['size']for f in functions)
 assert code[hi-4][1]=='ret';assert (hi-lo)//4==len([pc for pc in code if lo<=pc<hi]);text.append(f'\n.type {label}, %function\n{label}:\n');transfers=[]
 for pc in range(lo,hi,4):
  word,op,args=code[pc]
  if pc==0xce5130:text.append('.Lgs_unwind_skip_gcs:\n')
  if op in['chkfeat','gcspushm','bti']:body=f'.inst 0x{word:08x} /* {op} {args} */'
  elif op=='cbnz':assert pc==0xce5128 and args.startswith('x16, 0xce5130');body='cbnz x16, .Lgs_unwind_skip_gcs'
  else:body=op+' '+args
  text.append(f'   {body:<45} // ELF0x{pc:x},word0x{word:08x}\n')
  if op in['ldr','ldp','str','stp']:
   match=re.fullmatch(r'([xd][0-9]+)(?:, ([xd][0-9]+))?, \[x0(?:, #(0x[0-9a-f]+))?\]',args);assert match,(hex(pc),op,args)
   offset=int(match[3],16)if match[3]else 0
   transfers.extend(dict(register=register,context_byte=offset+8*i,bytes=8,operation='load'if op in['ldr','ldp']else'store',elf=hex(pc))for i,register in enumerate([match[1]]+([match[2]]if match[2]else[])))
 text.append(f'.size {label}, .-{label}\n');rows.append(dict(native_name=name,elf=hex(lo),end=hex(hi),span_bytes=hi-lo,instructions=(hi-lo)//4,ELF_size=0,range_is_inferred_not_ELF_size=True,restored_label=label,transfers=transfers))
source=D/'reconstructed/libunwind-unsized.S';source.write_text(''.join(text))
by=collections.defaultdict(list)
for f in functions:by[f['address']].append(f)
labels=[]
for address,group in sorted(by.items()):
 zero=[f['name']for f in group if not f['size']]
 if zero:labels.append(dict(elf=hex(address),zero_size_labels=zero,sized_starts=[dict(name=f['name'],bytes=f['size'])for f in group if f['size']],classification='spatial overlap only; no semantic alias or feature claim'if any(f['size']for f in group)else'independent unsized runtime entry; explicit span recovered'))
meta=snapshot(R/'originals/gs_rutabaga_v3.so');report=dict(original_binary_sha256=hashlib.sha256(raw).hexdigest(),ranges=rows,zero_size_labels=labels,source=str(source.relative_to(W)),source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),uncovered_executable_ranges=meta['uncovered_executable_ranges'],context_notes=['getcontext stores GPR x0..x30, SP at0xf8, PC(x30)at0x100; lower64 bits of d0..d31 at0x110..0x208.','jumpto restores x2..x15/x18..x29, x30 fromPC0x100, d0..d31, SP from0xf8, and x0/x1 last; x16/x17 are not restored as a pair.','Optional guarded-control-stack instruction sequence is byte-preserved; branch/architectural feature behavior is not executed or inferred here.','No new Mesa GPU/backend behavior or runtime replacement was added.'],automatic_C_metrics_unchanged=True,driver_build=False,limits=__doc__.splitlines())
(E/'unsized-runtime-entries.json').write_text(json.dumps(report,indent=2)+'\n');(D/'unsized-runtime-entries.json').write_text(json.dumps(report,indent=2)+'\n');print(dict(independent_spans=len(rows),instructions=len(code),overlapping_label_addresses=sum(bool(r['sized_starts'])for r in labels),all_zero_size_symbol_labels=sum(len(r['zero_size_labels'])for r in labels),uncovered_exec_ranges=len(meta['uncovered_executable_ranges'])))
