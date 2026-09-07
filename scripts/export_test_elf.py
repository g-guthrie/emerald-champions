#!/usr/bin/env python3
"""Export one filtered test ELF from the immutable linked test image."""
import argparse
from pathlib import Path
import json
import shutil
import shlex
import subprocess
import tempfile

from update_build_config import digest, tool_identity, write_config


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--receipt', required=True, type=Path)
    parser.add_argument('--patchelf', required=True)
    parser.add_argument('--filter', default='')
    args = parser.parse_args()
    identity = {'source': digest(args.source), 'filter': args.filter,
                'tool': tool_identity(args.patchelf), 'exporter': digest(Path(__file__))}
    if args.output.is_file() and args.receipt.is_file():
        try:
            if json.loads(args.receipt.read_text()) == {'inputs': identity, 'output': digest(args.output)}:
                return
        except (ValueError, OSError):
            pass
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='.test-export-', dir=args.output.parent) as temp:
        target = Path(temp) / args.output.name
        shutil.copyfile(args.source, target)
        subprocess.run([args.patchelf, str(target), 'gTestRunnerArgv', args.filter + r'\0'], check=True)
        target.replace(args.output)
    write_config(args.receipt, {'inputs': identity, 'output': digest(args.output)})


if __name__ == '__main__':
    main()
