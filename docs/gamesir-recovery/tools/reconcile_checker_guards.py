"""Compare recovered original eligibility formulas with current registry metadata.
Imports only the XML requirement resolver; no C/device generator or driver runs.
"""
from pathlib import Path
import sys,json,hashlib,xml.etree.ElementTree as ET
from index_native_coverage import ROOT
E=ROOT/'native-complete/rutabaga-remaining-two-hours-20261027';W=ROOT/'mesa-gamesir-recovered';U=W/'src/vulkan/util';sys.path.insert(0,str(U))
from vk_extensions import get_all_required
xml=W/'src/vulkan/registry/vk.xml';required=get_all_required(ET.parse(xml).getroot(),'type','vulkan','true');native=json.loads((E/'checker-eligibility-guards.json').read_text());rows=[]
for r in native['rows']:
 if r['name']=='VkPhysicalDeviceFeatures2':
  rows.append(dict(name=r['name'],classification='explicit unconditional core-features2 validation case',matches=r['unconditional']and not r['extension_bytes']));continue
 req=required.get(r['name'])
 if req is None:rows.append(dict(name=r['name'],classification='not required by current Vulkan XML metadata',matches=False));continue
 version=req.core_version
 current_min=(version.major<<22)|(version.minor<<12)|(version.patch or 0)if version is not None else None
 original_min=int(r['minimum_core_version_from_standard_thresholds'],16)if r['minimum_core_version_from_standard_thresholds']is not None else None
 old_ext=sorted(x['name']for x in r['extension_bytes']);new_ext=sorted(x.name for x in req.extensions)
 thresholds=r.get('api_comparisons',[]);precise=not original_min or bool(thresholds)and all(c['other_value']in[original_min,original_min-1]for c in thresholds)
 row=dict(name=r['name'],original_core_min=hex(original_min)if original_min is not None else None,current_core_min=hex(current_min)if current_min is not None else None,original_extensions=old_ext,current_extensions=new_ext,original_exact_threshold_sites=thresholds,thresholds_precise=precise,phase_dependencies_equal=r['phase_dependencies']['query_chain']==r['phase_dependencies']['validation'],matches=original_min==current_min and old_ext==new_ext and precise and r['each_extension_alone_enables']);rows.append(row)
result=dict(original_binary_sha256=native['original_binary_sha256'],current_registry_sha256=hashlib.sha256(xml.read_bytes()).hexdigest(),resolver_sha256=hashlib.sha256((U/'vk_extensions.py').read_bytes()).hexdigest(),generator_sha256=hashlib.sha256((U/'vk_physical_device_features_gen.py').read_bytes()).hexdigest(),rows=rows,equal=sum(r['matches']for r in rows),differences=[r for r in rows if not r['matches']],limits=['Static original guard/metadata reconciliation, not live device support or extension activation.','Core-feature2 is the explicit generator case, separate from generic requirements.','One-bit/all-zero query routing and read-site thresholds are bounded original-code models, not final API/GPU validation.'])
(E/'checker-current-guard-comparison.json').write_text(json.dumps(result,indent=2)+'\n');print('equal guards',result['equal'],'differences',len(result['differences']));print([{k:v for k,v in r.items()if k!='original_exact_threshold_sites'}for r in result['differences'][:5]])
