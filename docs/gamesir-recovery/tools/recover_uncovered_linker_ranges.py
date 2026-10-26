"""Classify executable bytes outside sized functions, without target execution.

Byte patterns and ELF relocation records identify PLT slots; zero padding and
an unnamed BTI/RET stub are preserved as data, not invented driver features.
The two unsized libunwind routines have a separate readable assembly record.
"""
from pathlib import Path
import json,struct,hashlib
from index_native_coverage import ROOT,elf
from compare_native_versions import snapshot
R=ROOT;E=R/'native-complete/rutabaga-remaining-two-hours-20261027';W=R/'mesa-gamesir-recovered';D=W/'docs/gamesir-recovery';raw,sections,functions=elf(R/'originals/gs_rutabaga_v3.so');assert hashlib.sha256(raw).hexdigest()=='90c056d7919a1da8ef0fd3131403729b81b05fe3c7373e772e3ceaa522ad397e'
def at(address,n):
 s=next(s for s in sections if s['type']!=8 and s['address']<=address and address+n<=s['address']+s['size']);offset=s['offset']+address-s['address'];return raw[offset:offset+n]
def symbol(section,index):
 offset=section['offset']+index*section['entsize'];name,info,other,shndx,value,size=struct.unpack_from('<IBBHQQ',raw,offset);strings=sections[section['link']];start=strings['offset']+name;name=raw[start:raw.index(b'\0',start)].decode();return dict(name=name,type=info&15,binding=info>>4,section_index=shndx,value=hex(value))
reloc=next(s for s in sections if s['name']=='.rela.plt');dynamic_symbols=sections[reloc['link']];entries={}
for offset in range(reloc['offset'],reloc['offset']+reloc['size'],reloc['entsize']):
 address,info,addend=struct.unpack_from('<QQq',raw,offset);assert address not in entries;assert info&0xffffffff==0x402;entries[address]=dict(symbol=symbol(dynamic_symbols,info>>32),type=hex(info&0xffffffff),addend=addend)
plt=next(s for s in sections if s['name']=='.plt');assert plt['size']>=32 and(plt['size']-32)%16==0;slots=[]
for pc in range(plt['address']+32,plt['address']+plt['size'],16):
 words=struct.unpack('<IIII',at(pc,16));a,l,d,b=words
 assert a&0x9f00001f==0x90000010 and l&0xffc003ff==0xf9400211 and d&0xffc003ff==0x91000210 and b==0xd61f0220,(hex(pc),[hex(w)for w in words])
 imm=((a>>29)&3)|(((a>>5)&0x7ffff)<<2);imm-=1<<21 if imm&(1<<20)else 0;page=(pc&~4095)+(imm<<12);off=((l>>10)&0xfff)*8;add=(d>>10)&0xfff;assert off==add
 got=page+off;assert got in entries;slots.append(dict(elf=hex(pc),got_slot=hex(got),**entries[got]))
assert len(slots)==len(entries)and len({s['got_slot']for s in slots})==len(entries)
header=list(struct.unpack('<8I',at(plt['address'],32)));assert header[0]==0xa9bf7bf0 and header[4]==0xd61f0220 and header[5:]==[0xd503201f]*3
ranges=[]
for r in snapshot(R/'originals/gs_rutabaga_v3.so')['uncovered_executable_ranges']:
 address=int(r['address'],16);body=at(address,r['bytes']);assert hashlib.sha256(body).hexdigest()==r['sha256']
 if r['section']=='.plt':kind='ELF PLT resolver32bytes +234relocation-backed16byte entries';assert r['bytes']==3776
 elif address==0xce5094:kind='Two independent unsized libunwind entry spans; readable assembly saved';assert r['bytes']==316
 elif body==struct.pack('<II',0xd503245f,0xd65f03c0):kind='Unnamed BTI c / RET stub; no symbol identity inferred';assert address==0x8e1050
 else:assert not any(body);kind='All-zero inter-function padding/UDF words; no callable semantics inferred'
 ranges.append(dict(**r,classification=kind))
result=dict(original_binary_sha256=hashlib.sha256(raw).hexdigest(),uncovered_ranges=ranges,unclassified_ranges=0,PLT_entries=len(slots),relocation_type='0x402',PLT_header_words=[hex(w)for w in header],PLT_slots=slots,production_driver_changed=False,driver_build=False,limits=__doc__.splitlines()+['No dynamic linking/import resolution or library/runtime behavior was executed.','Zero-size labels sharing a sized start are spatial overlap only; not all are semantic aliases.','This accounts for executable byte ranges, not dataflow/source equivalence of every driver routine.'])
(E/'uncovered-linker-ranges.json').write_text(json.dumps(result,indent=2)+'\n')
(D/'original-PLT-slots.tsv').write_text('ELF\tGOT-slot\timport-symbol\n'+''.join(f"{r['elf']}\t{r['got_slot']}\t{r['symbol']['name']}\n"for r in slots))
compact={k:v for k,v in result.items()if k!='PLT_slots'};compact['PLT_map']='original-PLT-slots.tsv';compact['PLT_map_sha256']=hashlib.sha256((D/'original-PLT-slots.tsv').read_bytes()).hexdigest();(D/'uncovered-linker-ranges.json').write_text(json.dumps(compact,indent=2)+'\n');print(dict(uncovered_ranges=len(ranges),unclassified=0,PLT_entries=len(slots),padding_bytes=sum(r['bytes']for r in ranges if 'padding'in r['classification'])))
