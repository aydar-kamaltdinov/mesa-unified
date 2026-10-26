"""Emit readable original field/eligibility metadata and isolated check kernel.
This is an analysis artifact; it does not replace the existing Mesa generator.
"""
from pathlib import Path
import json,hashlib,re
from index_native_coverage import ROOT
E=ROOT/'native-complete/rutabaga-remaining-two-hours-20261027';W=ROOT/'mesa-gamesir-recovered';D=W/'docs/gamesir-recovery/reconstructed';D.mkdir(exist_ok=True)
priority=json.loads((E/'checker-field-priority.json').read_text());guard_doc=json.loads((E/'checker-eligibility-guards.json').read_text());assert priority['original_binary_sha256']==guard_doc['original_binary_sha256']=='90c056d7919a1da8ef0fd3131403729b81b05fe3c7373e772e3ceaa522ad397e';guards={r['stype']:r for r in guard_doc['rows']};fields=[];descs=[];core=None
for group in priority['rows']:
 first=len(fields)
 for field in group['order']:
  assert re.fullmatch(r'[A-Za-z0-9_]+',field['member']);fields.append(dict(offset=field['api_offset'],name=field['member']))
 if group['core']:g=dict(unconditional=True,minimum_core_version_from_standard_thresholds=None,extension_bytes=[])
 else:g=guards[group['stype']]
 ext=[int(x['physical_offset'],16)-0x58 for x in g['extension_bytes']];assert len(ext)<=2 and all(0<=v<431 for v in ext)
 descriptor=dict(stype=group['stype'],name=group['native_diagnostic_name'],first=first,count=len(group['order']),unconditional=g['unconditional'],core_min=int(g['minimum_core_version_from_standard_thresholds'],16)if g['minimum_core_version_from_standard_thresholds']is not None else 0,extensions=ext)
 if group['core']:core=descriptor
 else:descs.append(descriptor)
assert len(fields)==715 and len(descs)==275 and core is not None
header='''/* SPDX-License-Identifier: MIT
 * Original GameSir eligibility metadata, diagnostic order and field kernel.
 * Derived from715native routes and275paired query/validation guards.
 * Analysis artifact: not linked into Mesa, not a complete vkCreateDevice
 * replacement and not a successful automatic C decompilation.
 * Public API field offsets are little-endian VkBool32 views. Extension IDs
 * refer ONLY to the original431-byte extension table, with canonical0/1 flags;
 * they must not be copied as numeric offsets into the current Mesa ABI.
 * Query-chain construction/ownership/duplicates remain in the existing Mesa
 * generator. Features2 uses its already-owned root during queries; the entry
 * below describes its explicit validation view, not another appended root.
 * Existing Mesa's newer KHR pipeline-library-group-handles alias is preserved
 * in production; this capsule records the original EXT-only eligibility.
 * No driver build, C execution or final API/GPU validation performed.
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct gs_native_feature_field {
   uint16_t api_offset;
   const char *name;
};
struct gs_native_feature_description {
   uint32_t sType;
   const char *diagnostic_name;
   uint16_t first_field, field_count;
   uint32_t minimum_core_api;
   bool unconditional;
   uint16_t extension_ids[2];
   uint8_t extension_count;
};

static const struct gs_native_feature_field gs_native_fields[] = {
'''
parts=[header]
for f in fields:parts.append(f'   {{{f["offset"]}, "{f["name"]}"}},\n')
parts.append('};\n\nstatic const struct gs_native_feature_description gs_native_types[] = {\n')
def initializer(d):
 ext=(d['extensions']+[0,0])[:2];assert re.fullmatch(r'[A-Za-z0-9_.]+',d['name'])
 return f'   {{{d["stype"]}u, "{d["name"]}", {d["first"]}, {d["count"]}, 0x{d["core_min"]:x}u, '+('true'if d['unconditional']else'false')+f', {{{ext[0]}, {ext[1]}}}, {len(d["extensions"])}'+ '}'
for d in descs:parts.append(initializer(d)+',\n')
parts.append('};\n\n/* pEnabledFeatures has no sType; this object is not in the sType lookup. */\nstatic const struct gs_native_feature_description gs_native_core =\n'+initializer(core)+';\n')
parts.append('''
const struct gs_native_feature_description *
gs_native_find_feature_type(uint32_t sType)
{
   for (unsigned i = 0; i < sizeof(gs_native_types) / sizeof(gs_native_types[0]); i++)
      if (gs_native_types[i].sType == sType)
         return &gs_native_types[i];
   return NULL;
}

const struct gs_native_feature_description *
gs_native_core_feature_description(void)
{
   return &gs_native_core;
}

bool
gs_native_validation_eligible(const struct gs_native_feature_description *d,
                              uint32_t api_version,
                              const uint8_t native_extension_flags[431])
{
   if (!d)
      return false;
   if (d->unconditional || (d->minimum_core_api && api_version >= d->minimum_core_api))
      return true;
   for (unsigned i = 0; i < d->extension_count; i++)
      if (native_extension_flags[d->extension_ids[i]])
         return true;
   return false;
}

static uint32_t
gs_load_api_bool(const uint8_t *p)
{
   return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
          ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* For an eligible known view, req/supported are valid corresponding API
 * structures. A returned field identifies the original first unsupported
 * request, which maps to VK_ERROR_FEATURE_NOT_PRESENT(-8). This kernel does
 * not query a device, follow pNext, allocate memory, log or enable features.
 */
const struct gs_native_feature_field *
gs_native_first_unsupported(const struct gs_native_feature_description *d,
                            const void *req, const void *supported)
{
   if (!d)
      return NULL;
   for (unsigned i = 0; i < d->field_count; i++) {
      const struct gs_native_feature_field *f = &gs_native_fields[d->first_field + i];
      if (gs_load_api_bool((const uint8_t *)req + f->api_offset) &&
          !gs_load_api_bool((const uint8_t *)supported + f->api_offset))
         return f;
   }
   return NULL;
}
''')
p=D/'feature-checker.c';p.write_text(''.join(parts));report=dict(artifact=str(p.relative_to(W)),sha256=hashlib.sha256(p.read_bytes()).hexdigest(),field_records=len(fields),type_descriptions=len(descs),separate_core_description=True,original_extension_table_entries=431,query_or_driver_replacement=False,build=False,limits=['Readable original data/kernel reconstruction; current generic Mesa generator is retained.','Full API/query-chain/callback ownership and arbitrary runtime behavior remain for final validation.'])
(E/'checker-capsule.json').write_text(json.dumps(report,indent=2)+'\n');print('Emitted715field records,275type guards, separate55field core view and isolated C kernels.')
