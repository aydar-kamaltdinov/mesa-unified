"""Summarize actual export results, preserving failures and quality warnings."""
import hashlib
import json
from pathlib import Path
import re
from index_native_coverage import ROOT

folder=ROOT/'native-complete'
refresh_path=folder/'rutabaga-execution-hour-20261024/compiler-export-refresh.json'
refreshed={}
if refresh_path.exists():
    refresh=json.loads(refresh_path.read_text())
    refreshed={label:{x['file'] for x in rows} for label,rows in refresh['results'].items()}
exit_refresh_path=folder/'rutabaga-execution-hour-20261024/exit-contract-refresh.json'
if exit_refresh_path.exists():
    exit_refresh=json.loads(exit_refresh_path.read_text())
    refreshed.setdefault('gamesir',set()).update(x['path'] for x in exit_refresh['functions'] if 'new_sha256' in x)
tail_refresh_path=folder/'rutabaga-linkage-hour-20261025/tu-tailcall-refresh.json'
if tail_refresh_path.exists():
    tail_refresh=json.loads(tail_refresh_path.read_text())
    refreshed.setdefault('gamesir',set()).update(x['file'] for x in tail_refresh['functions'] if 'new_sha256' in x)
ir3_tail_refresh_path=folder/'rutabaga-linkage-hour-20261025/ir3-tailcall-refresh.json'
if ir3_tail_refresh_path.exists():
    ir3_tail_refresh=json.loads(ir3_tail_refresh_path.read_text())
    refreshed.setdefault('gamesir',set()).update(x['file'] for x in ir3_tail_refresh['functions'] if 'new_sha256' in x)
typed_refresh_path=folder/'rutabaga-linkage-hour-20261025/fs-output-typed-refresh.json'
if typed_refresh_path.exists():
    typed_refresh=json.loads(typed_refresh_path.read_text())
    refreshed.setdefault('gamesir',set()).update(x['file'] for x in typed_refresh['functions'] if 'new_sha256' in x)
registry_path=folder/'canonical-code-refreshes.json'
registry=json.loads(registry_path.read_text())['functions'] if registry_path.exists() else {}
result={}
for label in ('gamesir','upstream'):
    jobs={}
    for line in (folder/(label+'-export-jobs.tsv')).read_text().splitlines():
        address,size,name=line.split('\t',2)
        jobs[address]=dict(address=address,size=int(size,16),name=name)
    reports=['decomp-'+label]
    if label=='gamesir':
        reports+=['decomp-gamesir-retry','decomp-gamesir-conservative','decomp-gamesir-contracts']
    candidates={address:[] for address in jobs}
    for dirname in reports:
        directory=folder/dirname
        report=directory/'results.tsv'
        if not report.exists():
            continue
        # Old error text contained embedded newlines. A job header is the
        # authoritative row start; continuation lines cannot become new jobs.
        for line in report.read_text().splitlines():
            fields=line.split('\t')
            if len(fields)<3 or fields[0] not in jobs:
                continue
            address=fields[0]
            filename=address+'-'+re.sub(r'[^A-Za-z0-9_]','_',jobs[address]['name'])
            filename=filename[:190]+'.c'
            path=directory/filename
            complete=fields[2]=='completed' and path.exists()
            content=path.read_text() if path.exists() else ''
            complete=complete and '// FAILED' not in content and '\n{' in content
            outside=[]
            for match in re.finditer(r'(?:code_r0x|joined_r0x|LAB_)([0-9a-fA-F]+)',content):
                a=int(match[1],16)
                if not int(address,16)<=a<int(address,16)+jobs[address]['size']:
                    outside.append(hex(a))
            candidates[address].append(dict(status=fields[2],completed=complete,
                file=str(path.relative_to(ROOT)),bytes=path.stat().st_size if path.exists() else 0,
                sha256=hashlib.sha256(path.read_bytes()).hexdigest() if path.exists() else None,
                outside_function_labels=sorted(set(outside)),warnings=content.count('WARNING'),
                analysis_contracts=('contracts' in dirname or (dirname=='decomp-'+label and filename in refreshed.get(label,set()))),
                conservative=('conservative' in dirname or 'contracts' in dirname or (dirname=='decomp-'+label and filename in refreshed.get(label,set())))))
    # Preserve earlier report statuses; a successful refresh after a FAILED
    # export is a new attempt with a verified canonical-file digest.
    if label == 'gamesir':
        for address, refresh in registry.items():
            path=ROOT/refresh['file']
            data=path.read_bytes()
            assert hashlib.sha256(data).hexdigest() == refresh['sha256'], path
            content=data.decode()
            lo=int(address,16); hi=lo+jobs[address]['size']
            outside=sorted({hex(int(m[1],16)) for m in re.finditer(
                r'(?:code_r0x|joined_r0x|LAB_)([0-9a-fA-F]+)',content)
                if not lo <= int(m[1],16) < hi})
            candidates[address].append(dict(status='completed-refresh',
                completed='// FAILED' not in content and '\n{' in content,
                file=refresh['file'],bytes=len(data),sha256=refresh['sha256'],
                outside_function_labels=outside,warnings=content.count('WARNING'),
                analysis_contracts=True,conservative=True,refresh_report=refresh['report']))
    records=[]
    for address,job in jobs.items():
        history=candidates[address]
        good=[c for c in history if c['completed']]
        preferred=next((c for c in reversed(good) if not c['outside_function_labels']),good[-1] if good else None)
        records.append(dict(job,preferred=preferred,attempts=history,
                            c_semantics_audited=False))
    result[label]=dict(expected_nonzero_function_addresses=len(jobs),
        completed_C_exports=sum(bool(r['preferred']) for r in records),
        with_out_of_boundary_labels=sum(bool(r['preferred'] and r['preferred']['outside_function_labels']) for r in records),
        failed=[dict(address=r['address'],name=r['name']) for r in records if not r['preferred']],
        records=records)
result['limits']=['Completed is only the decompiler API status plus output presence.',
                  'No semantic equivalence inferred; warnings/indirect targets/boundaries still need interpretation.',
                  'All executable bytes remain available in independently byte-verified ARM64 listings.',
                  'Two zero-size libunwind entry addresses have no ELF size and are excluded from C jobs.']
(folder/'decompilation-manifest.json').write_text(json.dumps(result,indent=2)+'\n')
for label in ('gamesir','upstream'):
    r=result[label]
    print(label,{k:v for k,v in r.items() if k!='records'})
