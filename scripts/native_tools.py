"""Shared native runner construction, ELF symbols, and read-only game queries."""
from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import struct
import tempfile


class NativeToolError(RuntimeError):
    pass


async def read_game_query(core, kind: int, ident: int, advance):
    """Read an acknowledged query; the caller records each frame via advance()."""
    if kind <= 0:
        raise ValueError("Native queries need a positive kind")
    pending = core.syms["gEcHeadlessCampaignQueryKind"]
    value = core.syms["gEcHeadlessCampaignQueryValue"]
    await core.write([(core.syms["gEcHeadlessCampaignQueryId"], ident), (pending, kind)])
    for _ in range(6):
        await advance()
        raw = await core.rpc(4, struct.pack("<II", pending, 4))
        if struct.unpack("<I", raw)[0] == 0:
            raw = await core.rpc(4, struct.pack("<II", value, 4))
            return struct.unpack("<I", raw)[0]
    raise NativeToolError(f"Native query (kind {kind}, id {ident}) was not acknowledged; no result was accepted")


def run(command: list[str]) -> subprocess.CompletedProcess[str]:
    try:
        result = subprocess.run(command, text=True, capture_output=True, timeout=120)
    except (OSError, subprocess.TimeoutExpired) as error:
        raise NativeToolError(str(error)) from error
    if result.returncode:
        raise NativeToolError(f"command failed ({result.returncode}): {' '.join(command)}\n{(result.stdout + result.stderr)[-6000:]}")
    return result


def find_mgba_prefix() -> Path:
    candidates = [Path(os.environ['MGBA_PREFIX'])] if os.environ.get('MGBA_PREFIX') else []
    candidates.append(Path.home() / '.local/share/mgba-prefix')
    brew = shutil.which('brew')
    if brew:
        try:
            prefix = run([brew, '--prefix', 'mgba']).stdout.strip()
            if prefix:
                candidates.append(Path(prefix))
        except NativeToolError:
            pass
    candidates.extend(map(Path, ('/opt/homebrew/opt/mgba', '/usr/local/opt/mgba', '/usr')))
    for prefix in candidates:
        libraries = list((prefix / 'lib').glob('libmgba*')) + list((prefix / 'lib').glob('*/libmgba*'))
        if (prefix / 'include/mgba/core/core.h').is_file() and libraries:
            return prefix.resolve()
    raise NativeToolError('native libmGBA headers/library are unavailable; set MGBA_PREFIX')


def mgba_flags() -> list[str]:
    pkg_config = shutil.which('pkg-config')
    # An explicit prefix beats whatever pkg-config finds (Homebrew's bottle can
    # lag its ffmpeg dependency and fail to load).
    # The local native build avoids that FFmpeg dependency entirely.
    local_library = Path.home() / '.local/share/mgba-prefix/lib/libmgba.dylib'
    if pkg_config and not os.environ.get('MGBA_PREFIX') and not local_library.is_file():
        try:
            flags = shlex.split(run([pkg_config, '--cflags', '--libs', 'mgba']).stdout)
            if flags:
                return flags
        except NativeToolError:
            pass
    prefix = find_mgba_prefix()
    return [f'-I{prefix / "include"}', f'-L{prefix / "lib"}', '-lmgba', f'-Wl,-rpath,{prefix / "lib"}']


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def build_runner(source: Path, output: Path) -> Path:
    command = [os.environ.get('CC', 'cc'), '-std=c11', '-O2', '-Wall', '-Wextra', '-Werror', str(source), *mgba_flags()]
    # Content identity catches preserved timestamps and builder changes. The
    # resolved flags/CC/environment also distinguish native toolchain choices.
    identity = {'source': digest(source), 'builder': digest(Path(__file__)),
                'command': command, 'environment': {key: os.environ.get(key, '') for key in
                    ('PATH', 'MGBA_PREFIX', 'CPATH', 'C_INCLUDE_PATH', 'LIBRARY_PATH', 'SDKROOT', 'MACOSX_DEPLOYMENT_TARGET')}}
    stamp = output.with_name(output.name + '.inputs.json')
    if output.is_file() and stamp.is_file():
        try:
            if json.loads(stamp.read_text()) == {'inputs': identity, 'output': digest(output)}:
                return output
        except (ValueError, OSError):
            pass
    output.parent.mkdir(parents=True, exist_ok=True)
    # Failed builds leave the previous executable and stamp intact.
    with tempfile.TemporaryDirectory(prefix='native-runner-', dir=output.parent) as temp:
        built = Path(temp) / output.name
        run([*command, '-o', str(built)])
        built.replace(output)
    stamp.write_text(json.dumps({'inputs': identity, 'output': digest(output)}, sort_keys=True) + '\n')
    return output


def find_nm(root: Path) -> str:
    for candidate in (shutil.which('arm-none-eabi-nm'), root / 'tools/binutils/bin/arm-none-eabi-nm', '/opt/homebrew/bin/arm-none-eabi-nm'):
        if candidate and Path(candidate).is_file():
            return str(candidate)
    raise NativeToolError('arm-none-eabi-nm is required for symbol probes')


def parse_symbols(text: str, *, first: bool = False) -> dict[str, int]:
    found = {}
    # GNU nm -S emits either address/size/type/name or address/type/name.
    # Undefined symbols have no address; reject malformed records explicitly.
    pattern = r'^([0-9a-fA-F]+)\s+(?:[0-9a-fA-F]+\s+)?[A-Za-z?]\s+(\S+)$'
    for line in text.splitlines():
        match = re.fullmatch(pattern, line.strip())
        if match:
            if not first or match[2] not in found:
                found[match[2]] = int(match[1], 16)
    return found


def symbols(elf: Path, root: Path, *, first: bool = False) -> dict[str, int]:
    return parse_symbols(run([find_nm(root), '-S', str(elf)]).stdout, first=first)
