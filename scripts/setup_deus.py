#!/usr/bin/env python3
"""Fetch the stock, checksum-pinned Deus release with direct stdio MCP support."""
import argparse
import hashlib
from pathlib import Path
import platform
import tarfile
import tempfile
import urllib.request

VERSION = 'v0.9.0'
ARCHIVE = 'codebase-memory-mcp-linux-amd64-portable.tar.gz'
SHA256 = '8459d5c9d1457f2c82de3de307ffc7641ecbba2dde893427be1e62eca8ef9b25'
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--directory', type=Path, required=True)
a = p.parse_args()
if platform.system() != 'Linux' or platform.machine() not in ('x86_64', 'amd64'):
    raise SystemExit('This pin is Linux x86_64 only; select the official asset for your platform.')
a.directory.mkdir(parents=True, exist_ok=True)
if (a.directory / 'codebase-memory-mcp').exists():
    raise SystemExit('Binary already exists; use it or choose a new empty directory.')
url = f'https://github.com/DeusData/codebase-memory-mcp/releases/download/{VERSION}/{ARCHIVE}'
with tempfile.TemporaryDirectory(prefix='deus-download-') as temp:
    archive = Path(temp) / ARCHIVE
    urllib.request.urlretrieve(url, archive)
    if hashlib.sha256(archive.read_bytes()).hexdigest() != SHA256:
        raise SystemExit('Official archive checksum mismatch; nothing installed.')
    with tarfile.open(archive) as source:
        members = source.getmembers()
        expected = {'codebase-memory-mcp', 'LICENSE', 'install.sh', 'THIRD_PARTY_NOTICES.md'}
        if {m.name for m in members} != expected or any(not m.isfile() for m in members):
            raise SystemExit('Unexpected archive members; nothing installed.')
        source.extractall(a.directory, filter='data')
print(a.directory.resolve() / 'codebase-memory-mcp')
