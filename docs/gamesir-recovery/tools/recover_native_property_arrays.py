"""Recover the four original caller-array property routes as bounded ARM dataflow.

No SO/driver code is executed. Concrete counts/pointers select original branches;
array bytes remain independent symbolic inputs. Nonoverlapping valid little-endian
buffers only. memcpy is an explicit byte-copy abstraction. This does not validate
allocation/lifetime, arbitrary aliasing, GPU use or real property initialization.
"""
from pathlib import Path
import json,hashlib,itertools
import recover_native_property_getter as m
E=m.ROOT/'native-complete/rutabaga-remaining-two-hours-20261027'
A=0x500000000;D=0x600000000
specs=[
 dict(name='VkPhysicalDeviceHostImageCopyProperties',arrays=[('pCopySrcLayouts','copySrcLayoutCount',0xec4,0xec8,4),('pCopyDstLayouts','copyDstLayoutCount',0xed0,0xed8,4)]),
 dict(name='VkPhysicalDeviceVulkan14Properties',arrays=[('pCopySrcLayouts','copySrcLayoutCount',0xec4,0xec8,4),('pCopyDstLayouts','copyDstLayoutCount',0xed0,0xed8,4)]),
 dict(name='VkPhysicalDeviceLayeredApiPropertiesListKHR',arrays=[('pLayeredApis','layeredApiCount',0xadc,0xae0,288)]),
 dict(name='VkPhysicalDeviceGpaPropertiesAMD',arrays=[('pPerfBlocks','perfBlockCount',0xf00,0xf08,28)])]
rows=[];sites=set()
def seed(cells,address,value,n):
 for i in range(n):cells[address+i]=m.bits((value>>(8*i))&255,8)
def check(spec,total,capacity,present,mask=None):
 st=next(t for t in m.structs if t['name']==spec['name']);fields={f['name']:f for f in st['fields']};cells={};details=[];ranges=[]
 for index,(ptr,count,po,pp,stride)in enumerate(spec['arrays']):
  count_field=fields[count];ptr_field=fields[ptr];source=A+index*0x10000;dest=D+index*0x10000
  pointer_present=present if mask is None else bool(mask&(1<<index));expected=min(total,capacity)if pointer_present else total
  seed(cells,m.P+po,total,4);seed(cells,m.P+pp,source,8)
  seed(cells,m.C+count_field['offset'],capacity,4);seed(cells,m.C+ptr_field['offset'],dest if pointer_present else 0,8)
  for byte in range(total*stride):cells[source+byte]=tuple(f'array{index}:{byte}:{bit}'for bit in range(8))
  # Written range plus canaries; do not allocate UINT32_MAX capacity.
  allocation=max(total,min(capacity,64))*stride;seed(cells,dest-16,0xa5a5a5a5a5a5a5a5,8)
  for byte in range(-16,allocation+16):cells[dest+byte]=m.bits(0xa5,8)
  ranges.append((dest-16,dest+allocation+16));details.append(dict(pointer=ptr,count=count,count_offset=count_field['offset'],pointer_offset=ptr_field['offset'],source_count_offset=hex(po),source_pointer_offset=hex(pp),stride=stride,expected_count=expected,present=pointer_present,source=source,destination=dest,allocation=allocation,index=index))
 partial={};stores,trace=m.recover(st['stype'],partial,True,inputs=cells,output_ranges=ranges);sites.update(trace)
 for d in details:
  memory=partial['memory'];at=m.C+d['count_offset'];observed=m.integer(sum((memory[at+i]for i in range(4)),()))
  assert observed==d['expected_count'],(spec['name'],total,capacity,present,d,observed)
  copied=d['expected_count']*d['stride']if d['present']else 0;dest=d['destination'];source=d['source']
  for byte in range(-16,d['allocation']+16):
   expect=cells[source+byte]if 0<=byte<copied else m.bits(0xa5,8)
   assert memory[dest+byte]==expect,(spec['name'],total,capacity,d['pointer'],byte)
  for i in range(8):assert memory[m.C+d['pointer_offset']+i]==cells[m.C+d['pointer_offset']+i]
 return dict(struct=spec['name'],total=total,capacity=capacity,array_presence_mask=mask if mask is not None else((1<<len(spec['arrays']))-1 if present else 0),model_steps=len(trace),arrays=[{k:v for k,v in d.items()if k not in['source','destination','allocation','index']}for d in details])
for spec in specs:
 for total in[0,1,7,8,9,16,17]:
  for capacity in[0,1,3,7,8,9,16,17,19,0x80000000,0xffffffff]:
   for present in[False,True]:rows.append(check(spec,total,capacity,present))
 if len(spec['arrays'])==2:
  for mask in[1,2]:rows.append(check(spec,17,9,True,mask))
source=m.W/'src/vulkan/util/vk_physical_device_properties_gen.py';text=source.read_text()
assert all(repr(p) in text and repr(c) in text for spec in specs for p,c,*_ in spec['arrays'])
assert 'uint32_t count = MIN2(${dst_count}, ${src_count});'in text and '${dst_ptr}[i] = ${src_ptr}[i];'in text and '${dst_count} = ${src_count};'in text
report=dict(original_binary_sha256=hashlib.sha256(m.raw).hexdigest(),getter_elf=hex(m.lo),structures=4,array_routes=6,model_cases=len(rows),rows=rows,unique_instruction_sites=len(sites),visited_instruction_addresses=[hex(pc)for pc in sorted(sites)],source_template_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),source_template_matches=True,production_driver_changed=False,limits=__doc__.splitlines())
(E/'property-caller-array-routes.json').write_text(json.dumps(report,indent=2)+'\n')
print(dict(structures=4,array_routes=6,cases=len(rows),unique_sites=len(sites),source_matches=True))
