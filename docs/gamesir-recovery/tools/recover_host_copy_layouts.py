"""Read the original host-image-copy layout table; no driver execution."""
from pathlib import Path
import json,re,struct,xml.etree.ElementTree as ET,hashlib
from index_native_coverage import ROOT,elf
E=ROOT/'native-complete/rutabaga-two-hours-20261027';W=ROOT/'mesa-gamesir-recovered'
raw,secs,_=elf(ROOT/'originals/gs_rutabaga_v3.so');objects=json.loads((ROOT/'native-complete/gamesir-objects.json').read_text());obj=next(x for x in objects if x['name'].endswith('E17supported_layouts'))
a=obj['address'];n=obj['size'];section=next(s for s in secs if s['type']!=8 and s['address']<=a and a+n<=s['address']+s['size']);o=section['offset']+a-section['address'];blob=raw[o:o+n];values=struct.unpack('<'+str(n//4)+'I',blob)
s=(W/'src/freedreno/vulkan/tu_device.cc').read_text();table=s.split('static const VkImageLayout supported_layouts[] = {',1)[1].split('};',1)[0];names=re.findall(r'VK_IMAGE_LAYOUT_[A-Z0-9_]+',table)
root=ET.parse(W/'src/vulkan/registry/vk.xml').getroot();enums={};aliases={}
for e in root.findall("./enums[@name='VkImageLayout']/enum"):
 if e.get('value'):enums[e.get('name')]=int(e.get('value'),0)
 if e.get('alias'):aliases[e.get('name')]=e.get('alias')
for extension in root.findall('./extensions/extension'):
 for e in extension.findall('.//enum'):
  if e.get('extends')!='VkImageLayout':continue
  name=e.get('name')
  if e.get('alias'):aliases[name]=e.get('alias')
  elif e.get('value'):enums[name]=int(e.get('value'),0)
  elif e.get('offset'):enums[name]=1000000000+(int(e.get('extnumber')or extension.get('number'))-1)*1000+int(e.get('offset'))
for e in root.iter('enum'):
 name=e.get('name','')
 if name.startswith('VK_IMAGE_LAYOUT_'):
  if e.get('alias'):aliases[name]=e.get('alias')
  elif e.get('value'):enums[name]=int(e.get('value'),0)
  elif e.get('offset') and e.get('extnumber'):enums[name]=1000000000+(int(e.get('extnumber'))-1)*1000+int(e.get('offset'))
for _ in range(len(aliases)):
 for name,target in aliases.items():
  if target in enums:enums[name]=enums[target]
assert len(names)==len(values)==20
rows=[dict(name=name,native_value=value,current_value=enums[name],equal=value==enums[name])for name,value in zip(names,values)];assert all(r['equal']for r in rows)
result=dict(original_binary_sha256=hashlib.sha256(raw).hexdigest(),native_object=obj['name'],elf=hex(a),bytes=n,array_sha256=hashlib.sha256(blob).hexdigest(),rows=rows,source='src/freedreno/vulkan/tu_device.cc supported_layouts',finding='Both20-entry arrays and order already match; no duplicate transfer.',generic_array_source='src/vulkan/util/vk_physical_device_properties_gen.py OUT_ARRAYS/ARRAY_COPY_TEMPLATE',native_capacity_example='ELF0xa43f84..0xa44020: null output returns available count; non-null takes unsigned min(requested,available), copies entries then publishes copied count.',limits=['Static table and inspected count/copy protocol only.','Caller alias/capacity/vector/tail and runtime API/GPU behavior remain unvalidated.','No feature exposure, driver build or device action.'])
(E/'host-copy-layout-table.json').write_text(json.dumps(result,indent=2)+'\n');print('Original/current host-copy layouts:20entries and order equal')
