"""Merge completed static exports in place, keeping compact provenance, no C backups.

Analysis only: API completion and bounded-label count are not semantic validation.
Usage: python3 scripts/merge_native_c_refresh.py OUTPUT_DIR REFRESH_JSON
"""
from pathlib import Path
import sys,json,hashlib,re,csv,datetime
from index_native_coverage import ROOT

folder=ROOT/'native-complete'
source=Path(sys.argv[1]).resolve(); proof=Path(sys.argv[2]).resolve()
assert not proof.exists(), 'Do not overwrite historical refresh evidence'
jobs={a:(int(n,16),name) for a,n,name in (s.split('\t',2) for s in (folder/'gamesir-export-jobs.tsv').read_text().splitlines())}
registry_path=folder/'canonical-code-refreshes.json'
registry=json.loads(registry_path.read_text()) if registry_path.exists() else {'limits':['Static C reconstruction only; no source or GPU equivalence inferred.'],'functions':{}}
rows=[]
def sha(b): return hashlib.sha256(b).hexdigest()
def outside(code,a,size):
 return len({int(m[1],16) for m in re.finditer(r'(?:code_r0x|joined_r0x|LAB_)([0-9a-fA-F]+)',code) if not a<=int(m[1],16)<a+size})
for entry in csv.DictReader((source/'results.tsv').open(),delimiter='\t'):
 a=entry['address']; status=entry['status']; filename=(a+'-'+re.sub(r'[^A-Za-z0-9_]','_',jobs[a][1]))[:190]+'.c'
 row=dict(address=a,file=filename,status=status)
 if status=='completed':
  p=source/entry['file']; target=folder/'decomp-gamesir'/filename
  b=p.read_bytes(); code=b.decode(); old=target.read_bytes() if target.exists() else b''
  assert '// FAILED' not in code and '\n{' in code
  before=outside(old.decode(),int(a,16),jobs[a][0]); after=outside(code,int(a,16),jobs[a][0])
  if after > before:
   row.update(status='retained-original',reason='new export has more outside-function labels',old_outside=before,new_outside=after)
   rows.append(row)
   continue
  row.update(old_sha256=sha(old),new_sha256=sha(b),old_bytes=len(old),new_bytes=len(b),old_outside=before,new_outside=after)
  target.write_bytes(b)
  registry['functions'][a]=dict(file=str(target.relative_to(ROOT)),sha256=sha(b),report=str(proof.relative_to(ROOT)),status='completed',analysis_only=True)
 rows.append(row)
result=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),source_report=str((source/'results.tsv').relative_to(ROOT)),functions=rows,limits=['No source semantic equivalence asserted.','Original failed result rows retained; new successful export is a separate analysis attempt.'])
proof.write_text(json.dumps(result,indent=2)+'\n'); registry_path.write_text(json.dumps(registry,indent=2)+'\n')
# Keep reports and remove redundant generated C only. Original ARM and canonical C remain.
for p in source.glob('*.c'): p.unlink()
print(json.dumps({'merged':sum(x['status']=='completed' for x in rows),'failed':sum(x['status']!='completed' for x in rows),'proof':str(proof)}))
