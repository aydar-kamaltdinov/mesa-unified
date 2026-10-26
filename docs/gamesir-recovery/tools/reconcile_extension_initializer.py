"""Index original extension support writes against current source declarations.

Only declaration presence/literal comparisons. Does not evaluate CPP, the generator,
SDK filters, debug options, symbolic predicates or runtime feature legality.
"""
import json,re,hashlib,collections
from index_native_coverage import ROOT
E=ROOT/'native-complete/rutabaga-two-hours-20261027'
p=ROOT/'mesa-gamesir-recovered/src/freedreno/vulkan/tu_device.cc'
s=p.read_text();body=s[s.index('static void\nget_device_extensions'):s.index('static void\ntu_get_features')]
assignments={}
for m in re.finditer(r'^\s*\.(\w+)\s*=\s*',body,re.M):
 name=m[1];assert name not in assignments,name
 at=m.end();start=at;depth=0
 while at<len(body):
  char=body[at]
  if body.startswith('/*',at):at=body.index('*/',at+2)+2;continue
  if char in '([{':depth+=1
  elif char in ')]}':depth-=1
  elif char==',' and depth==0:break
  at+=1
 assert at<len(body)and depth==0,name
 assignments[name]=re.sub(r'\s+',' ',body[start:at].strip())
native=json.loads((E/'original-extension-store-inventory.json').read_text());rows=[]
for row in native['rows']:
 name=row['name'][3:];expr=assignments.get(name)
 tag=('literal-true-declaration' if expr=='true' else 'literal-false-declaration' if expr=='false' else 'expression-declaration' if expr is not None else 'absent-zero-initialized')
 rows.append(dict(name=row['name'],native_classification=row['classification'],native_elf=row['final_write']['elf'],current_expression=expr,source_classification=tag))
counts={key:dict(collections.Counter(r['source_classification'] for r in rows if r['native_classification']==key)) for key in ['literal-enabled','conditional-or-symbolic','literal-disabled']}
result=dict(original_binary_sha256=native['binary_sha256'],current_source_sha256=hashlib.sha256(p.read_bytes()).hexdigest(),counts=counts,rows=rows,limits=['Declaration inventory only; CPP/platform/SDK filtering not evaluated.','Source expression presence is not semantic equivalence to original conditions.','Held original features are not enabled by this index.','No driver build, shader execution or GPU/runtime enumeration.'])
(E/'extension-source-declaration-comparison.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(counts))
for r in rows:
 if r['native_classification']!='literal-disabled' and r['source_classification'] in ['absent-zero-initialized','literal-false-declaration']:print(r['name'],r['native_classification'],r['source_classification'])
