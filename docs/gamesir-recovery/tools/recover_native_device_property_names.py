"""Recover debug-option field names/offsets from the original bounded parser.
Provenance crosswalk for analysis; not a guessed local C-struct layout.
"""
from pathlib import Path
import re,json,hashlib
from index_native_coverage import ROOT,elf
F=ROOT/'native-complete';E=F/'rutabaga-two-hours-20261027'
raw,secs,_=elf(ROOT/'originals/gs_rutabaga_v3.so')
p=next((F/'decomp-gamesir').glob('d688d0-*.c'));code=p.read_text()
def readstr(a):
 s=next(s for s in secs if s['type']!=8 and s['address']<=a<s['address']+s['size']);o=s['offset']+a-s['address'];return raw[o:raw.index(b'\0',o)].decode('ascii')
compares=list(re.finditer(r'func_0x00de5400\s*\([^;]*?&\s*UNK_00([0-9a-f]+)\)\s*;',code))
rows=[];unresolved=[]
for i,m in enumerate(compares):
 end=compares[i+1].start() if i+1<len(compares) else len(code);chunk=code[m.end():end];parse=re.search(r'debug_parse_(bool|num)_option\s*\(',chunk)
 if not parse:continue
 offsets=re.findall(r'\*\((?:byte|undefined[1248]|uint|int|float)\s*\*\)\(param_1\s*\+\s*(0x[0-9a-f]+|\d+)\s*\)\s*=',chunk)
 name=readstr(int(m[1],16)-0x100000)
 if len(set(offsets))!=1:
  unresolved.append(dict(name=name,line=code.count('\n',0,m.start())+1,offsets=offsets));continue
 offset=int(offsets[0],0);assert re.fullmatch(r'\w+',name)
 rows.append(dict(name=name,native_info_offset=hex(offset),parser=parse[1],string_elf=hex(int(m[1],16)-0x100000),source_C_line=code.count('\n',0,m.start())+1))
assert len({x['name'] for x in rows})==len(rows)
result=dict(original_parser_elf='0xc688d0',original_parser_size=5540,original_binary_sha256=hashlib.sha256(raw).hexdigest(),canonical_C=str(p.relative_to(ROOT)),canonical_C_sha256=hashlib.sha256(p.read_bytes()).hexdigest(),named_fields=rows,unresolved=unresolved,limits=['Names from original ELF strings and field stores in its bounded decompiled parser.','No current Mesa header offset or profile value is inferred by this crosswalk.','C exports remain analysis evidence, not a semantic-equivalence certificate.'])
(E/'native-device-property-names.json').write_text(json.dumps(result,indent=2)+'\n');print('named',len(rows),'unresolved',unresolved)
for x in rows:
 if x['name'] in ['storage_8bit','storage_16bit','has_hw_multiview','is_a702','has_getfiberid','has_ray_intersection','has_sw_fuse','has_image_processing','has_per_view_viewport','has_persistent_counter']:print(x['name'],x['native_info_offset'])
