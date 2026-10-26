"""Recover original VkExtensionProperties and Android allow bytes by ELF symbols.
Static extraction only; these properties are names, not support claims.
"""
from pathlib import Path
import struct,json,hashlib
from index_native_coverage import ROOT,elf
raw,secs,_=elf(ROOT/'originals/gs_rutabaga_v3.so')
def string(blob,pos):return blob[pos:blob.index(b'\0',pos)].decode()
objects={}
for sec in secs:
 if sec['type']!=2:continue
 st=secs[sec['link']];names=raw[st['offset']:st['offset']+st['size']]
 for off in range(sec['offset'],sec['offset']+sec['size'],sec['entsize']):
  name,info,other,idx,value,size=struct.unpack_from('<IBBHQQ',raw,off)
  n=string(names,name)
  if 'device_extensions' in n and info&15==1:
   objects[n]=dict(address=value,size=size,section=secs[idx]['name'])
def bytes_at(a,n):
 for sec in secs:
  if sec['address']<=a and a+n<=sec['address']+sec['size']:
   o=sec['offset']+a-sec['address'];return raw[o:o+n]
 raise ValueError(hex(a))
table=objects['vk_device_extensions'];assert table['size']%260==0
rows=[]
for i in range(table['size']//260):
 b=bytes_at(table['address']+260*i,260);name=string(b,0);assert name.startswith('VK_');rows.append(dict(index=i,physical_device_offset=hex(0x58+i),name=name,specVersion=struct.unpack_from('<I',b,256)[0]))
for name,obj in objects.items():
 if name=='vk_device_extensions':continue
 if obj['size']==len(rows):
  allowed=bytes_at(obj['address'],obj['size'])
  for i,row in enumerate(rows):row[name]=allowed[i]
p=ROOT/'native-complete/rutabaga-two-hours-20261027/original-extension-table.json';p.write_text(json.dumps(dict(binary_sha256=hashlib.sha256(raw).hexdigest(),objects=objects,rows=rows,limits=['Extension name table is not supported-extension enumeration.','Original regular TU writes and platform allow bytes must be evaluated separately.']),indent=2)+'\n');print('objects',objects,'rows',len(rows))
