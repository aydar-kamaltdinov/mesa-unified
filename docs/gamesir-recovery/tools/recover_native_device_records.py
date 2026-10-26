"""Recover original device records and named native property values, no execution."""
from pathlib import Path
import struct,json,re,hashlib
from index_native_coverage import ROOT,elf
F=ROOT/'native-complete';E=F/'rutabaga-two-hours-20261027'
raw,secs,_=elf(ROOT/'originals/gs_rutabaga_v3.so');objects={}
def cstr(b,o):return b[o:b.index(b'\0',o)].decode()
for s in secs:
 if s['type']!=2:continue
 st=secs[s['link']];names=raw[st['offset']:st['offset']+st['size']]
 for o in range(s['offset'],s['offset']+s['size'],s['entsize']):
  n,info,other,index,value,size=struct.unpack_from('<IBBHQQ',raw,o);name=cstr(names,n)
  if info&15==1 and (name=='fd_dev_recs' or re.fullmatch(r'__info\d+',name)):objects[name]=dict(address=value,size=size)
def data_at(a,n):
 s=next(s for s in secs if s['type']!=8 and s['address']<=a and a+n<=s['address']+s['size']);o=s['offset']+a-s['address'];return raw[o:o+n]
def string_at(a):
 s=next(s for s in secs if s['type']!=8 and s['address']<=a<s['address']+s['size']);return cstr(raw,s['offset']+a-s['address'])
props=json.loads((E/'native-device-property-names.json').read_text())['named_fields'];table=objects['fd_dev_recs'];assert table['size']%32==0
records=[]
for i in range(table['size']//32):
 b=data_at(table['address']+i*32,32);gpu,pad,chip,name,info=struct.unpack('<IIQQQ',b);blob=data_at(info,0x324);values={}
 for p in props:
  o=int(p['native_info_offset'],16)
  if p['parser']=='bool':value=blob[o];assert value in [0,1]
  elif p['name'] in ['line_width_min','line_width_max']:value=struct.unpack_from('<f',blob,o)[0]
  else:value=struct.unpack_from('<I',blob,o)[0]
  values[p['name']]=value
 header_names=['tile_align_w','tile_align_h','gmem_align_w','gmem_align_h','tile_max_w','tile_max_h','num_vsc_pipes','cs_shared_mem_size','wave_granularity','highest_bank_bit','ubwc_swizzle','macrotile_mode','fibers_per_sp','threadsize_base','max_waves','compute_lb_size','num_sp_cores','num_slices']
 header=dict(zip(header_names,struct.unpack_from('<18I',blob,4)))
 magic=[dict(reg=hex(reg),value=hex(value)) for reg,value in struct.iter_unpack('<II',blob[0x58:0x258]) if reg or value]
 legacy=dict(zip(['RB_DBG_ECO_CNTL','RB_DBG_ECO_CNTL_blit','RB_RBP_CNTL'],struct.unpack_from('<3I',blob,0x4c)))
 records.append(dict(index=i,gpu_id=gpu,chip_id=hex(chip),name=string_at(name),info_elf=hex(info),chip=blob[0],info_sha256=hashlib.sha256(blob).hexdigest(),header=header,legacy_magic=legacy,raw_magic=magic,properties=values))
result=dict(binary_sha256=hashlib.sha256(raw).hexdigest(),table=table,record_count=len(records),named_properties=len(props),records=records,limits=['Raw original static records and named fields only.','Kernel-derived GMEM, runtime debug overrides and driver activation are separate.','No assumption that local current fd_dev_info has this original layout.'])
(E/'native-device-records.json').write_text(json.dumps(result,indent=2)+'\n');print('records',len(records),'unique_info',len(set(x['info_elf'] for x in records)),'named_props',len(props))
for rec in records:
 if rec['chip']>=8:print(rec['name'],rec['chip_id'],{n:rec['properties'][n] for n in ['multi_subpass_msaa_no_input_requires_sysmem','msaa_input_d32s8_requires_gmem','fdm_requires_full_density']})

fields=[(0,'u8','chip')]+[(4+4*i,'u32',n)for i,n in enumerate(header_names)]
fields +=[(0x4c+4*i,'u32',n)for i,n in enumerate(['RB_DBG_ECO_CNTL','RB_DBG_ECO_CNTL_blit','RB_RBP_CNTL'])]
fields +=[(int(f['native_info_offset'],16),'bool'if f['parser']=='bool'else'f32'if f['name']in['line_width_min','line_width_max']else'u32',f['name'])for f in props]
(E/'original-device-info-fields.tsv').write_text(''.join(f'{o:x}\t{t}\t{n}\n'for o,t,n in sorted(fields)))
