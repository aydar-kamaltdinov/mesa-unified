"""Remove only verified duplicate/superseded recovery work after the requested pass.

The original libraries, current source worktree, machine listings, selected C,
unique SSA and recovery history are protected. No archives are created.
"""
from pathlib import Path
from datetime import datetime,timezone
import argparse,json,hashlib,os,stat,shutil,subprocess,re
R=Path(os.environ.get('TURNIP_RECOVERY_ROOT',Path(__file__).resolve().parents[1])).resolve();W=R/'mesa-gamesir-recovered';F=R/'native-complete';E=F/'rutabaga-remaining-two-hours-20261027';PRE=F/'rutabaga-two-hours-20261027';BASE='afec90a36cc9e433bc4c95b3bd6f42dda960c77b'
def digest(data):return hashlib.sha256(data).hexdigest()
def git(*args):return subprocess.check_output(['git',*args],cwd=W).decode().strip()
def files(path):return sorted(p for p in path.rglob('*')if p.is_file()or p.is_symlink())
def data(path):return os.readlink(path).encode()if path.is_symlink()else path.read_bytes()
def inventory(path):return[dict(path=str(p.relative_to(R)),bytes=len(data(p)),sha256=digest(data(p)))for p in files(path)]
def baseline_comparison():
 C=R/'source-baseline-candidate'
 if not C.exists():return dict(path=str(C.relative_to(R)),exists=False,removable=False)
 assert C.is_dir()and not C.is_symlink();assert git('merge-base',BASE,'rutabaga')==BASE
 tree=git('rev-parse',BASE+'^{tree}');entries=subprocess.check_output(['git','ls-tree','-rz',BASE],cwd=W).split(b'\0');known={}
 for e in entries:
  if e:
   meta,name=e.split(b'\t',1);mode,kind,blob=meta.decode().split();assert kind=='blob';known[name.decode()]=(mode,blob)
 rows=[];different=[];seen=set();finder=[]
 for p in files(C):
  rel=p.relative_to(C).as_posix();blob=data(p);sha=hashlib.sha1(b'blob '+str(len(blob)).encode()+b'\0'+blob).hexdigest();mode='120000'if p.is_symlink()else'100755'if p.stat().st_mode&stat.S_IXUSR else'100644';row=[rel,mode,sha,len(blob)];rows.append(row);seen.add(rel)
  if known.get(rel)!=(mode,sha):
   if p.name=='.DS_Store'and rel not in known:finder.append(rel)
   else:different.append(row)
 missing=sorted(set(known)-seen);canonical=json.dumps(rows,separators=(',',':')).encode()
 return dict(path=str(C.relative_to(R)),exists=True,baseline_commit=BASE,baseline_tree=tree,tracked_entries=len(known),candidate_entries=len(rows),logical_bytes=sum(row[3]for row in rows),finder_extras=finder,differences=different,missing=missing,inventory_sha256=digest(canonical),removable=not different and not missing)

def main():
 parser=argparse.ArgumentParser();parser.add_argument('--apply',action='store_true');args=parser.parse_args();now=datetime.now(timezone.utc)
 if args.apply and(E/'cleanup-applied.json').is_file():
  previous=json.loads((E/'cleanup-applied.json').read_text())
  if previous.get('applied'):
   print(json.dumps(dict(already_applied=True,logical_bytes_removed=previous['logical_bytes_removed'],report=str(E/'cleanup-applied.json'))));return
 if args.apply:
  start=json.loads((E/'session-start.json').read_text());assert now>=datetime.fromisoformat(start['earliest_finish_utc']), 'Finish the requested two-hour pass first'
 manifest=json.loads((F/'decompilation-manifest.json').read_text());selected={r['address']:r['preferred']for r in manifest['gamesir']['records']if r['preferred']};registry=json.loads((F/'canonical-code-refreshes.json').read_text())['functions']
 # All successful current preferred bodies must now be in the protected directory.
 for address,p in selected.items():
  target=F/'decomp-gamesir'/Path(p['file']).name;assert target.is_file()
  if Path(p['file']).parent.name!='decomp-gamesir':assert digest(target.read_bytes())==p['sha256'],address
 for address,entry in registry.items():assert digest((R/entry['file']).read_bytes())==entry['sha256'],address
 baseline=baseline_comparison();candidates=[F/name for name in ['decomp-gamesir-retry','decomp-gamesir-conservative','decomp-gamesir-contracts']]
 candidates +=[p for parent in[PRE,E]for p in parent.iterdir()if p.is_dir()and p.name.endswith('-output')]
 history=[];directories=[];skipped=[]
 for folder in candidates:
  if not folder.exists():continue
  assert folder.resolve().is_relative_to(F.resolve())and folder.name not in['decomp-gamesir','decomp-upstream']
  bad=[]
  for p in folder.glob('*.c'):
   text=p.read_text();m=re.match(r'([0-9a-f]+)-',p.name)
   if '// FAILED'in text or text.startswith('// ERROR'):continue
   if not m or m[1]not in selected:bad.append(p.name);continue
   target=F/'decomp-gamesir'/p.name
   if not target.exists()or'// FAILED'in target.read_text():bad.append(p.name)
  if bad:skipped.append(dict(path=str(folder.relative_to(R)),reason='Unindexed/unique C requires explicit review',files=bad));continue
  report={}
  for p in files(folder):
   if p.suffix=='.tsv':report[str(p.relative_to(folder))]=p.read_text()
  inv=inventory(folder);history.append(dict(path=str(folder.relative_to(R)),reports=report,files=inv));directories.append(folder)
 logs=[]
 for parent in[PRE,E]:
  for p in sorted(parent.glob('*.log')):
   text=p.read_text(errors='replace');lines=text.splitlines();diagnostics=[line for line in lines if any(token in line for token in ['ERROR','WARN','FAILED','timeout','Exception','> NativeSymbolExport done','> Applied','> Typed'])]
   logs.append(dict(path=str(p.relative_to(R)),bytes=p.stat().st_size,sha256=digest(p.read_bytes()),diagnostics=diagnostics,tail=lines[-4:]))
 temporary_reports=[E/name for name in['checker-single-route.json','property-getter-remaining-sites.json']]
 temporary_inventory=[dict(path=str(p.relative_to(R)),bytes=p.stat().st_size,sha256=digest(p.read_bytes()),reason='Temporary/superseded intermediate; final all-route/source-closure evidence retained')for p in temporary_reports if p.is_file()]
 experiments=[R/'scripts'/name for name in['NativeAlternateCStyle.java','NativeCOnlyRetry.java','NativeLowTransformRetry.java']]
 experiment_inventory=[dict(path=str(p.relative_to(R)),bytes=p.stat().st_size,sha256=digest(p.read_bytes()),reason='Failed-only retry helper; strategy/results retained in compact report; no extracted source data')for p in experiments if p.is_file()]
 caches=[R/'scripts/__pycache__',W/'docs/gamesir-recovery/tools/__pycache__',F/'ghidra-cache']
 caches +=list((F/'ghidra-settings').glob('**/osgi/felixcache'))
 caches=[p for p in caches if p.exists()]
 report=dict(created_utc=now.isoformat(),applied=False,baseline=baseline,superseded_run_directories=[str(p.relative_to(R))for p in directories],run_history=history,old_logs=logs,failed_only_helpers=experiment_inventory,temporary_reports=temporary_inventory,caches=[dict(path=str(p.relative_to(R)),bytes=sum(r['bytes']for r in inventory(p)))for p in caches],skipped=skipped,protected=['originals/','device/','history/','mesa-gamesir-recovered/ tracked source','native-complete/decomp-gamesir/','native-complete/decomp-upstream/','full verified ARM listings','unique SSA and transfer evidence'],limits=['Logical file bytes, not a guarantee of physical APFS space freed.','Historical report/status text retained compactly; superseded C/verbose launcher logs are removed.','No driver build, device action, new archive or source-history deletion.'])
 output=E/('cleanup-applied.json'if args.apply else'cleanup-plan.json')
 if args.apply:
  removed=[]
  if baseline['removable']:
   shutil.rmtree(R/baseline['path']);removed.append(dict(path=baseline['path'],bytes=baseline['logical_bytes'],reason='Verified exact tracked baseline plus Finder extras; recoverable from Git root/tree'))
  for folder in directories:
   size=sum(p['bytes']for p in inventory(folder));shutil.rmtree(folder);removed.append(dict(path=str(folder.relative_to(R)),bytes=size,reason='Superseded export runs; selected C/data and compact reports retained'))
  for entry in logs:
   p=R/entry['path'];assert digest(p.read_bytes())==entry['sha256'];p.unlink();removed.append(dict(path=entry['path'],bytes=entry['bytes'],reason='Verbose run log condensed into diagnostics'))
  for entry in experiment_inventory+temporary_inventory:
   p=R/entry['path'];assert digest(p.read_bytes())==entry['sha256'];p.unlink();removed.append(dict(path=entry['path'],bytes=entry['bytes'],reason=entry['reason']))
  for folder in caches:
   size=sum(r['bytes']for r in inventory(folder));shutil.rmtree(folder);removed.append(dict(path=str(folder.relative_to(R)),bytes=size,reason='Rebuildable analysis cache'))
  finder=[]
  tracked=set(git('ls-files').splitlines())
  for p in sorted(R.rglob('.DS_Store')):
   if p.is_file():
    if p.is_relative_to(W)and p.relative_to(W).as_posix()in tracked:continue
    finder.append(dict(path=str(p.relative_to(R)),bytes=p.stat().st_size));p.unlink()
  report.update(applied=True,removed=removed,finder_removed=finder,logical_bytes_removed=sum(r['bytes']for r in removed)+sum(r['bytes']for r in finder))
  for p in[E/'cleanup-baseline-comparison.json',E/'cleanup-plan.json']:
   if p.is_file():p.unlink()
  for p in[Path('/private/tmp/rutabaga-20261027-commit-message.txt')]:
   if p.is_file():p.unlink()
 output.write_text(json.dumps(report,indent=2)+'\n')
 print(json.dumps(dict(applied=args.apply,baseline_removable=baseline['removable'],baseline_logical_bytes=baseline.get('logical_bytes'),superseded_directories=len(directories),logs=len(logs),failed_helpers=len(experiment_inventory),skipped=skipped,logical_bytes_removed=report.get('logical_bytes_removed'),output=str(output)),indent=2))
if __name__=='__main__':main()
