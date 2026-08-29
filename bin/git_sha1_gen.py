"""
Generate the contents of the git_sha1.h file.
"""

import argparse
import os
import os.path
import subprocess
import sys


def get_git_sha1():
    """Try to get the git SHA1 with git rev-parse."""
    git_dir = os.path.join(os.path.dirname(sys.argv[0]), '..', '.git')
    try:
        git_sha1 = subprocess.check_output([
            'git',
            '--git-dir=' + git_dir,
            'rev-parse',
            'HEAD',
        ], stderr=open(os.devnull, 'w')).decode("ascii").strip()
    except Exception:
        # don't print anything if it fails
        return ''

    git_sha1 = git_sha1[:10]

    # AK: HEAD alone is ambiguous while iterating with uncommitted changes
    # (our normal workflow) -- every build sharing a HEAD reports identical
    # driverInfo/driverVersion to apps even though the actual compiled code
    # differs, which lets a Vulkan app/loader that caches anything keyed by
    # driver identity (seen documented for Eden, and a real risk for our own
    # iterative on-device testing) serve stale state across genuinely
    # different driver builds. Fold a hash of the dirty diff (tracked files
    # only, matching what actually gets compiled) into the identifier so it
    # changes whenever the real source content does, not just on commit.
    # Applied here (before the caller's own truncation) so the suffix
    # survives instead of being cut off by it.
    try:
        work_tree = os.path.join(os.path.dirname(sys.argv[0]), '..')
        diff = subprocess.check_output([
            'git',
            '--git-dir=' + git_dir,
            '--work-tree=' + work_tree,
            'diff',
            'HEAD',
        ], stderr=open(os.devnull, 'w'))
        if diff:
            import hashlib
            git_sha1 = git_sha1 + '-dirty' + hashlib.sha1(diff).hexdigest()[:8]
    except Exception:
        pass

    return git_sha1


def write_if_different(contents):
    """
    Avoid touching the output file if it doesn't need modifications
    Useful to avoid triggering rebuilds when nothing has changed.
    """
    if os.path.isfile(args.output):
        with open(args.output, 'r') as file:
            if file.read() == contents:
                return
    with open(args.output, 'w') as file:
        file.write(contents)


parser = argparse.ArgumentParser()
parser.add_argument('--output', help='File to write the #define in',
                    required=True)
args = parser.parse_args()

env_override = os.environ.get('MESA_GIT_SHA1_OVERRIDE')
git_sha1 = env_override[:10] if env_override is not None else get_git_sha1()
if git_sha1:
    write_if_different('#define MESA_GIT_SHA1 " (git-' + git_sha1 + ')"')
else:
    write_if_different('#define MESA_GIT_SHA1 ""')
