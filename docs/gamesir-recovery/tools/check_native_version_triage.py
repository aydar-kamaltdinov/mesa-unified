"""Static regression cases for version triage; no ELF/driver/code execution.

Constructs in-memory instruction words and symbol records, then checks output
classification/queueing. Only temporary JSON/TSV/Markdown reports are written,
and TemporaryDirectory removes them. No driver or component build is involved.
"""
from pathlib import Path
from unittest.mock import patch
from tempfile import TemporaryDirectory
import struct,sys,json,hashlib
import compare_native_versions as m

def fixture(base,callee):
 # f: BL to base+16; RET. At base+16 a named target containing RET.
 raw=struct.pack('<II',0x94000004,0xd65f03c0)+bytes(8)+struct.pack('<I',0xd65f03c0)
 functions=[dict(name='f',address=base,size=8,section='.text',binding=1,source_file=None,sha256=m.sha(raw[:8])),dict(name=callee,address=base+16,size=4,section='.text',binding=1,source_file=None,sha256=m.sha(raw[16:20]))]
 sections=[dict(name='.text',address=base,offset=0,size=len(raw),flags=6,type=1),dict(name='.symtab',address=0,offset=len(raw),size=0,flags=0,type=2)]
 return raw,sections,functions

def snap(base,callee,path):
 with patch.object(m,'elf',return_value=fixture(base,callee)):return m.snapshot(Path(path))
a=snap(0x1000,'same_target','old');identical=snap(0x1000,'same_target','identical');same=snap(0x2000,'same_target','same');changed=snap(0x2000,'different_target','changed')
assert a['bykey'][('','f')][0]['sha256']==changed['bykey'][('','f')][0]['sha256']
assert a['bykey'][('','f')][0]['normalized']==same['bykey'][('','f')][0]['normalized']
assert a['bykey'][('','f')][0]['normalized']!=changed['bykey'][('','f')][0]['normalized']
# A changed unsized assembly span must remain visible even when every sized
# function body is unchanged. Keep its symbol metadata separate from C jobs.
raw,sections,functions=fixture(0x1000,'same_target');unsized=dict(name='runtime_unsized',address=0x1008,size=0,section='.text',binding=0,source_file='runtime.S',sha256=m.sha(b''))
functions.append(unsized)
with patch.object(m,'elf',return_value=(raw,sections,functions)):unsized_old=m.snapshot(Path('unsized-old'))
mutated=raw[:8]+struct.pack('<I',0xd503201f)+raw[12:]
with patch.object(m,'elf',return_value=(mutated,sections,functions)):unsized_new=m.snapshot(Path('unsized-new'))
# Identical data bytes at a different section address are still dependency
# evidence. Do not equate data layout solely by content hashes.
base_raw,base_sections,base_functions=fixture(0x1000,'same_target');base_sections=base_sections+[dict(name='.rodata',address=0x4000,offset=len(base_raw),size=8,flags=2,type=1)];with_data=base_raw+b'constant'
with patch.object(m,'elf',return_value=(with_data,base_sections,base_functions)):data_old=m.snapshot(Path('data-old'))
new_sections=[dict(s,address=0x5000)if s['name']=='.rodata'else s for s in base_sections]
with patch.object(m,'elf',return_value=(with_data,new_sections,base_functions)):data_new=m.snapshot(Path('data-new'))
results=[]
for name,old,new,expected,jobs,range_changed in [('identical-input',a,identical,'same-code-bytes',0,False),('same-target-moved-layout',a,same,'same-code-bytes',0,True),('different-target-identical-words',a,changed,'same-code-bytes-different-branch-targets',2,True),('changed-unsized-assembly',unsized_old,unsized_new,'same-code-bytes',0,True),('data-address-changed-same-bytes',data_old,data_new,'same-code-bytes',0,False)]:
 with TemporaryDirectory(prefix='rutabaga-version-triage-')as directory:
  out=Path(directory)/'out'
  with patch.object(m,'snapshot',side_effect=[old,new]),patch.object(sys,'argv',['compare_native_versions.py','--old','old','--new','new','--out',str(out)]):m.main()
  result=json.loads((out/'comparison.json').read_text());f=next(r for r in result['rows']if r['name']=='f');assert f['status']==expected,(name,f)
  assert result['unique_decompilation_jobs']==jobs,(name,result['unique_decompilation_jobs'])
  assert result['uncovered_executable_range_changes']['changed']==range_changed
  if name=='changed-unsized-assembly':
   assert any(r['unsized_entries']for r in result['uncovered_executable_range_changes']['new'])
   assert (out/'uncovered-executable-ranges.tsv').read_text()
  if name=='data-address-changed-same-bytes':
   assert len(result['data_section_changes'])==1
   changed=result['data_section_changes'][0];assert changed['old']['sha256']==changed['new']['sha256']and changed['old']['address']!=changed['new']['address']
  results.append(dict(name=name,status=f['status'],candidate_jobs=jobs,uncovered_executable_ranges_changed=range_changed,data_sections_changed=len(result['data_section_changes'])))
print(json.dumps(dict(cases=results,no_driver_build=True,no_target_execution=True,temporary_reports_removed=True),indent=2))
