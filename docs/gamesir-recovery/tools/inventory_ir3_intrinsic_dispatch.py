"""Name original IR3 dispatch cases from its own ELF intrinsic table, no execution."""
from pathlib import Path
import os
import json,re,struct
from index_native_coverage import elf
R=Path(os.environ.get('TURNIP_RECOVERY_ROOT',str(Path(__file__).resolve().parents[1])));W=R/'mesa-gamesir-recovered';E=R/'native-complete/rutabaga-linkage-hour-20261025'
raw,ss,fs=elf(R/'originals/gs_rutabaga_v3.so');objs={x['name']:x for x in json.loads((R/'native-complete/gamesir-objects.json').read_text())}
def off(a):return next(s['offset']+a-s['address'] for s in ss if s['type']!=8 and s['address']<=a<s['address']+s['size'])
def string(a):p=off(a);return raw[p:raw.index(b'\0',p)].decode()
s=(R/'native-complete/decomp-gamesir/bb5d58-emit_intrinsic.c').read_text();ordinals=[int(n,0) for n in re.findall(r'^  case (0x[0-9a-f]+|\d+):',s,re.M)];rows=[dict(ordinal=hex(n),name=string(struct.unpack_from('<Q',raw,off(objs['nir_intrinsic_infos']['address']+n*136))[0])) for n in ordinals]
s=(W/'src/freedreno/ir3/ir3_compiler_nir.c').read_text();a=s.index('\nemit_intrinsic(struct');b=s.index('\nstatic ',a);body=s[a:b];names=set(re.findall(r'case nir_intrinsic_(\w+):',body))
missing=[r for r in rows if r['name'] not in names];source_only=sorted(names-{r['name'] for r in rows})
d=dict(scope='Top-level decompiled outer dispatch labels only, independently named by original ELF table; case-name presence is not semantic equivalence',native_function='emit_intrinsic ELF0xab5d58',native_cases=rows,source_named_cases=sorted(names),native_names_without_same_source_case=missing,source_only_names=source_only,build=False,full_validation_deferred=True)
(E/'ir3-intrinsic-dispatch-name-inventory.json').write_text(json.dumps(d,indent=2)+'\n');print('Original named cases',len(rows),'source named cases',len(names));print('Original without same source case:',missing);print('Source only:',source_only)
