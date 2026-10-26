"""Account for the last unvisited getter instruction: unknown-sType skip.

Instruction-site closure is not proof of all values, paths or API/GPU behavior.
"""
import json,hashlib
import recover_native_property_getter as m
E=m.ROOT/'native-complete/rutabaga-remaining-two-hours-20261027'
previous=json.loads((m.E/'recovered-property-getter-fields.json').read_text());arrays=json.loads((E/'property-caller-array-routes.json').read_text())
assert previous['original_binary_sha256']==arrays['original_binary_sha256']==hashlib.sha256(m.raw).hexdigest()
unknown=0x3ba14a10;assert all(st['stype']!=unknown for st in m.structs)
inputs={m.C+i:m.bits(0xa5,8)for i in range(16,256)};partial={};stores,trace=m.recover(unknown,partial,inputs=inputs)
assert 0xa43c10 in trace
assert not any(m.C<=at<m.C+0x1000 for at in stores),'Unknown payload must be skipped'
assert all(partial['memory'][at]==cell for at,cell in inputs.items())
remaining=set(int(a,16)for a in previous['unvisited_instruction_addresses'])-set(int(a,16)for a in arrays['visited_instruction_addresses'])-set(trace)
assert not remaining
result=dict(original_binary_sha256=hashlib.sha256(m.raw).hexdigest(),getter_elf=hex(m.lo),getter_instruction_sites=len(m.code),previous_sites=previous['visited_static_instructions'],array_model_sites=arrays['unique_instruction_sites'],unknown_sType=hex(unknown),last_skip_elf='0xa43c10',unknown_payload_untouched=True,unknown_trace=[hex(pc)for pc in trace],unvisited_instruction_sites=len(remaining),production_driver_changed=False,limits=__doc__.splitlines())
(E/'property-getter-site-closure.json').write_text(json.dumps(result,indent=2)+'\n');print({k:v for k,v in result.items()if k not in['unknown_trace','limits']})
