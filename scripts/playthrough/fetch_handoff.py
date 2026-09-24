#!/usr/bin/env python3
"""Download verified handoff assets without overwriting a later local session.

No GitHub account or token is needed for this public repository's release.
The default is the complete current earned session. --evidence adds the earlier
native play/scene/regression archive; --roms adds player and historical ROMs.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import tarfile
import tempfile
import urllib.request

ROOT = Path(__file__).resolve().parents[2]


def digest(path):
    result = hashlib.sha256()
    with path.open('rb') as source:
        for block in iter(lambda: source.read(1024 * 1024), b''):
            result.update(block)
    return result.hexdigest()


def destination(name):
    path = Path(name)
    if path.is_absolute() or '..' in path.parts or path.parts[0] not in ('work', 'release'):
        raise ValueError(f'Unsafe artifact path: {name}')
    target = ROOT / path
    if not target.resolve().is_relative_to(ROOT):
        raise ValueError(f'Artifact path escapes checkout: {name}')
    return target


def install(source, target, expected=None, mode=0o644):
    target.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(dir=target.parent, prefix='.handoff-', delete=False) as temp:
        temporary = Path(temp.name)
        try:
            shutil.copyfileobj(source, temp)
            temp.flush()
            actual = digest(temporary)
            if expected and actual != expected:
                raise ValueError(f'Payload hash mismatch: {target}')
            if target.exists():
                if not target.is_file() or digest(target) != actual:
                    raise FileExistsError(f'Preserving different existing file: {target}')
            else:
                os.chmod(temporary, mode & 0o777)
                os.replace(temporary, target)
        finally:
            temporary.unlink(missing_ok=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--manifest', type=Path, default=ROOT / 'handoff/assets.json')
    parser.add_argument('--list', action='store_true')
    parser.add_argument('--evidence', action='store_true')
    parser.add_argument('--roms', action='store_true')
    parser.add_argument('--all', action='store_true')
    parser.add_argument('--asset', action='append', help='download a particular asset name')
    args = parser.parse_args()
    manifest = json.loads(args.manifest.read_text())
    groups = {'resume'}
    if args.evidence or args.all:
        groups.add('evidence')
    if args.roms or args.all:
        groups.update(('rom', 'historical'))
    selected = [a for a in manifest['assets'] if a['name'] in args.asset] if args.asset else [
        a for a in manifest['assets'] if a['group'] in groups]
    if args.list:
        for asset in manifest['assets']:
            print(f'{asset["group"]:10} {asset["size"]/1024/1024:8.1f} MiB {asset["name"]}')
        return
    if not selected:
        raise SystemExit('No matching assets.')
    blobs = {}
    if any(asset['format'] == 'blobs' for asset in selected):
        index = json.loads((ROOT / 'handoff/evidence-index.json').read_text())
        for entry in index['files']:
            blobs.setdefault(entry['sha256'], []).append(entry)
    cache = ROOT / 'work/handoff-downloads'
    cache.mkdir(parents=True, exist_ok=True)
    for asset in selected:
        url = f'https://github.com/{manifest["repository"]}/releases/download/{manifest["tag"]}/{asset["name"]}'
        path = cache / asset['name']
        print(f'Download/verify {asset["name"]}', flush=True)
        if not path.is_file() or digest(path) != asset['sha256']:
            request = urllib.request.Request(url, headers={'User-Agent': 'EmeraldChampions-Handoff/1'})
            with urllib.request.urlopen(request, timeout=120) as response:
                with tempfile.NamedTemporaryFile(dir=cache, delete=False) as temp:
                    temporary = Path(temp.name)
                    try:
                        shutil.copyfileobj(response, temp)
                        temp.flush()
                        if temporary.stat().st_size != asset['size'] or digest(temporary) != asset['sha256']:
                            raise ValueError(f'Download hash/size mismatch: {asset["name"]}')
                        os.replace(temporary, path)
                    finally:
                        temporary.unlink(missing_ok=True)
        if asset['format'] == 'file':
            with path.open('rb') as source:
                install(source, destination(asset['destination']), asset['sha256'])
        else:
            with tarfile.open(path, 'r:gz') as archive:
                for member in archive:
                    if member.isdir():
                        continue
                    if not member.isfile():
                        raise ValueError(f'Unsupported archive member: {member.name}')
                    if asset['format'] == 'tar':
                        with archive.extractfile(member) as source:
                            install(source, destination(member.name), mode=member.mode)
                    elif asset['format'] == 'blobs':
                        key = member.name.removeprefix('blobs/')
                        entries = blobs.get(key, [])
                        if not entries or any(e['asset'] != asset['name'] for e in entries):
                            raise ValueError(f'Unknown blob: {member.name}')
                        first = destination(entries[0]['path'])
                        with archive.extractfile(member) as source:
                            install(source, first, key, entries[0]['mode'])
                        for entry in entries[1:]:
                            with first.open('rb') as source:
                                install(source, destination(entry['path']), key, entry['mode'])
                    else:
                        raise ValueError(f'Unknown asset format: {asset["format"]}')
        path.unlink()  # Installed bytes remain; avoid keeping another full archive copy.
        print(f'Installed {asset["name"]}', flush=True)
    print('Verified handoff assets installed.')


if __name__ == '__main__':
    main()
