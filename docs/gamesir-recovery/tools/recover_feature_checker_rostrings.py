"""Recover bounded direct RO string-address construction in the generated checker.
No SSA/path/call-argument association or complete semantic equivalence inferred.
"""
from pathlib import Path
import json,re,xml.etree.ElementTree as ET,hashlib
from index_native_coverage import ROOT,elf
E=ROOT/'native-complete/rutabaga-two-hours-20261027';F=ROOT/'native-complete';W=ROOT/'mesa-gamesir-recovered'
raw,secs,fs=elf(ROOT/'originals/gs_rutabaga_v3.so');fn=next(f for f in fs if f['name']=='vk_physical_device_check_device_features');lo=fn['address'];hi=lo+fn['size'];words=[]
for line in (F/'gamesir.arm64.txt').read_text().splitlines():
 m=re.match(r'\s*([0-9a-f]+): ([0-9a-f]{8})\s+(\S+)\s*(.*)',line)
 if m and lo<=int(m[1],16)<hi:words.append((int(m[1],16),int(m[2],16),m[3],m[4].split('//')[0].strip()))
def string_at(a):
 for s in secs:
  if s['flags']&2 and not s['flags']&1 and s['type']!=8 and s['address']<=a<s['address']+s['size']:
   o=s['offset']+a-s['address'];end=raw.find(b'\0',o,min(o+300,s['offset']+s['size']))
   if end<0:return None
   b=raw[o:end]
   return b.decode('ascii') if b and all(32<=x<127 for x in b) else None
 return None
known={};refs=[]
for pc,w,op,args in words:
 if op.startswith('b') or op in ['cbz','cbnz','tbz','tbnz','ret']:known.clear();continue
 dest=args.split(',')[0]
 value=None
 if w&0x9f000000 in [0x90000000,0x10000000]:
  imm=((w>>29)&3)|(((w>>5)&0x7ffff)<<2);imm=imm-(1<<21) if imm&(1<<20) else imm
  value=((pc&~4095)+(imm<<12)) if w&0x80000000 else pc+imm
  known[dest]=(value,pc)
 elif op=='add':
  m=re.fullmatch(r'(x\d+), (x\d+), #0x([0-9a-f]+)',args)
  if m and m[2] in known:
   a,start=known[m[2]];value=a+int(m[3],16);known[m[1]]=(value,start)
   text=string_at(value)
   if text:refs.append(dict(adrp_or_adr_elf=hex(start),address_constructed_elf=hex(pc),string_elf=hex(value),text=text,register=m[1]))
  else:known.pop(dest,None)
 elif op not in ['str','strb','strh','stur','stp','cmp','cmn','tst','nop']:
  known.pop(dest,None)
 if value is not None and op=='adr':
  text=string_at(value)
  if text:refs.append(dict(adrp_or_adr_elf=hex(pc),address_constructed_elf=hex(pc),string_elf=hex(value),text=text,register=dest))
xml=W/'src/vulkan/registry/vk.xml';root=ET.parse(xml).getroot();names={t.get('name')or t.findtext('name') for t in root.findall('./types/type')};types=sorted({r['text'] for r in refs if re.fullmatch(r'VkPhysicalDevice\w*Features\w*(?:\.features)?',r['text'])});missing=[n for n in types if n.split('.')[0]not in names]
result=dict(function=fn['name'],elf=hex(lo),size=fn['size'],reference_count=len(refs),feature_structure_strings=types,not_in_current_vk_xml=missing,source_registry_sha256=hashlib.sha256(xml.read_bytes()).hexdigest(),references=refs,limits=['Positive direct address-construction references only; absence is not a completeness proof.','No inferred association between neighboring strings and vk_errorf arguments.','Structure name presence does not establish a supported/exposed driver feature.','Generated feature checker C export still fails; original full ARM remains authoritative.'])
(E/'feature-checker-rostrings.json').write_text(json.dumps(result,indent=2)+'\n');print('positive RO refs',len(refs),'structure strings',len(types),'missing XML names',missing)
