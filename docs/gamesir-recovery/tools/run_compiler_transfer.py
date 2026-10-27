"""Build/run debug NIR fixtures against the configured native component build.

First build src/compiler/nir/nir_tests and src/freedreno/ir3/ir3_delay_test in
that directory. Shipping Android configuration is not changed by this runner.
"""
import argparse
import json
from pathlib import Path
import shlex
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('build_directory', type=Path)
args = parser.parse_args()
build = args.build_directory.resolve()
root = Path(__file__).resolve().parents[3]
options = json.loads((build / 'meson-info/intro-buildoptions.json').read_text())
values = {item['name']: item['value'] for item in options}
assert values['b_ndebug'] == 'false', 'NIR assertions must be enabled'
commands = json.loads((build / 'compile_commands.json').read_text())
entry = next(x for x in commands if x['file'].endswith('/ir3_nir_move_varying_inputs.c'))
command = entry.get('arguments') or shlex.split(entry['command'])
# Do not overwrite Ninja's dependency files for the production library object.
for option in ['-MF', '-MQ', '-MT']:
    if option in command:
        idx = command.index(option)
        del command[idx:idx + 2]
command = [x for x in command if x not in ['-MD', '-MMD']]
out_dir = build / 'transfer-fixtures'
out_dir.mkdir(exist_ok=True)
objects = []
for source in [root / 'docs/gamesir-recovery/tools/check_compiler_transfer.c',
               root / 'src/freedreno/vulkan/tu_nir_lower_cooperative_matrix.c']:
    obj = out_dir / (source.stem + '.o')
    cmd = command.copy()
    cmd[cmd.index('-o') + 1] = str(obj)
    cmd[cmd.index('-c') + 1] = str(source)
    subprocess.run(cmd, cwd=entry['directory'], check=True)
    objects.append(str(obj))
link = subprocess.check_output(
    ['ninja', '-C', str(build), '-t', 'commands',
     'src/freedreno/ir3/ir3_delay_test'], text=True).splitlines()[-1]
cmd = shlex.split(link)
old = 'src/freedreno/ir3/ir3_delay_test.p/tests_delay.c.o'
assert cmd.count(old) == 1
idx = cmd.index(old)
cmd[idx:idx + 1] = objects
executable = out_dir / 'compiler-transfer-check'
cmd[cmd.index('-o') + 1] = str(executable)
subprocess.run(cmd, cwd=build, check=True)
subprocess.run([str(executable)], cwd=build, check=True)
