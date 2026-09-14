#!/usr/bin/env python3
"""Call the official Deus stdio MCP server; preserve the real JSON-RPC response.

Use the stock v0.9.0 release in runtimes without local sockets. v0.10.x requires
its daemon even for CLI calls. No agent installation, socket workaround, or
changes to the server's security checks are made by this adapter.
"""
import argparse
import json
import os
from pathlib import Path
import select
import subprocess
import time

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--binary', type=Path, required=True)
p.add_argument('--cache', type=Path, required=True)
p.add_argument('--out', type=Path, required=True)
p.add_argument('--timeout', type=int, default=300)
p.add_argument('tool')
p.add_argument('arguments', nargs='?', default='{}')
a = p.parse_args()
arguments = json.loads(a.arguments)
a.out.parent.mkdir(parents=True, exist_ok=True)
env = os.environ.copy()
env['CBM_CACHE_DIR'] = str(a.cache.resolve())
with a.out.with_suffix('.stderr.log').open('w') as log:
    proc = subprocess.Popen([str(a.binary.resolve()), '--ui=false'],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=log, env=env, bufsize=0)
    pending = bytearray()
    transcript = []
    def send(value):
        proc.stdin.write((json.dumps(value) + '\n').encode())
    def receive(ident):
        deadline = time.monotonic() + a.timeout
        while time.monotonic() < deadline:
            while b'\n' in pending:
                line, _, rest = pending.partition(b'\n')
                pending[:] = rest
                if not line.strip():
                    continue
                value = json.loads(line)
                transcript.append(value)
                if value.get('id') == ident:
                    return value
            if select.select([proc.stdout], [], [], 1)[0]:
                data = os.read(proc.stdout.fileno(), 65536)
                if not data:
                    raise RuntimeError('MCP server exited before responding; inspect stderr log')
                pending.extend(data)
        raise TimeoutError(f'MCP response {ident} timed out')
    try:
        send({'jsonrpc': '2.0', 'id': 1, 'method': 'initialize', 'params': {
            'protocolVersion': '2024-11-05', 'capabilities': {},
            'clientInfo': {'name': 'emerald-champions-audit', 'version': '1'}}})
        initialized = receive(1)
        if 'error' in initialized:
            raise RuntimeError(initialized['error'])
        send({'jsonrpc': '2.0', 'method': 'notifications/initialized'})
        send({'jsonrpc': '2.0', 'id': 2,
              'method': 'tools/list' if a.tool == '__list_tools__' else 'tools/call',
              'params': {} if a.tool == '__list_tools__' else {'name': a.tool, 'arguments': arguments}})
        result = receive(2)
        a.out.write_text(json.dumps({'transport': 'real stdio MCP',
            'binary': str(a.binary.resolve()), 'tool': a.tool,
            'arguments': arguments, 'messages': transcript}, indent=2) + '\n')
        print(json.dumps(result))
        if 'error' in result or result.get('result', {}).get('isError'):
            raise SystemExit(1)
    finally:
        proc.stdin.close()
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            proc.terminate()
            proc.wait(timeout=5)
