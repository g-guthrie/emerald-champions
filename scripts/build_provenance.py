#!/usr/bin/env python3
"""Immutable build provenance: one identifier compiled into the ROM and stamped beside it.

The Makefile runs this twice per linked ROM:

    prepare   before linking. Identifies the source (Git commit plus the content
              of every uncommitted build input; a whole-tree digest when there
              is no Git checkout), the build configuration and the toolchain,
              hashes that description into a build id and writes an assembly
              file that places ``ECBUILD1:<build id>`` in the ROM as the kept
              symbol ``gEcBuildProvenance``. Unchanged descriptions are not
              rewritten, so an unchanged tree does not relink.
    finalize  after objcopy/gbafix. Checks that the ROM and ELF carry exactly
              that identifier, derives the save-compatibility id from the
              compiled save structures (DWARF of SaveBlock1/2/3, PokemonStorage
              and SaveSector, plus the compiled flag/var numbering), rechecks
              the source identity and writes ``<rom>.provenance.json`` binding
              the ROM and ELF bytes.

Consumers call ``verify(rom, elf)``: a ROM is "verified" only when its embedded
identifier, the stamp's recomputed build id and the artifact hashes agree and the
source did not change while it was built. Anything else is "unverified" (with a
reason); a ROM with neither an embedded identifier nor a stamp is "legacy".
This records what was compiled; it does not authenticate the toolchain.
"""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys
import tempfile
import time

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(Path(__file__).resolve().parent))
import stamp_release_inputs as inputs  # noqa: E402

SCHEMA = 1
MAGIC = b"ECBUILD1:"
SYMBOL = "gEcBuildProvenance"
ROM_BASE = 0x08000000
EMBEDDED = re.compile(re.escape(MAGIC) + rb"([0-9a-f]{64})\0")
LAYOUT_ROOTS = ("SaveBlock1", "SaveBlock2", "SaveBlock3", "PokemonStorage", "SaveSector")
LAYOUT_HEADERS = ("global.h", "pokemon_storage_system.h", "save.h")
ID_HEADERS = ("constants/flags.h", "constants/vars.h")
# Files that shape the stamp itself, beyond stamp_release_inputs' generator list.
EXTRA_INPUTS = ("scripts/build_provenance.py",)


def canonical(value) -> bytes:
    return json.dumps(value, sort_keys=True, separators=(",", ":"), ensure_ascii=False).encode()


def sha256_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1 << 20), b""):
            digest.update(block)
    return digest.hexdigest()


def build_id(description: dict) -> str:
    return sha256_bytes(canonical(description))


# --------------------------------------------------------------------------- source


def _tool_dirs(root: Path) -> tuple[str, ...]:
    try:
        text = (root / "make_tools.mk").read_text()
    except OSError:
        return inputs.GENERATOR_DIRS
    match = re.search(r"(?m)^TOOL_NAMES\s*:?=\s*(.+)$", text)
    return tuple(match.group(1).split() if match else ()) + tuple(inputs.GENERATOR_DIRS)


def is_build_input(relative: str, tool_dirs: tuple[str, ...]) -> bool:
    """Mirror stamp_release_inputs.build_inputs() for one path Git reports as changed."""
    parts = relative.split("/")
    if parts[-1].startswith("._"):
        return False
    if len(parts) == 1:
        return relative in inputs.INPUT_FILES or Path(relative).suffix in inputs.INPUT_SUFFIXES
    if parts[0] in inputs.INPUT_DIRS:
        return True
    if relative in inputs.GENERATOR_INPUTS or relative in EXTRA_INPUTS:
        return True
    if parts[0] == "tools" and len(parts) > 2 and parts[1] in tool_dirs:
        if any(part in {"build", "__pycache__", ".git"} for part in parts[2:]):
            return False
        return parts[-1] == "Makefile" or Path(relative).suffix in inputs.TOOL_SOURCE_SUFFIXES
    return False


def _git(root: Path, *args: str) -> bytes | None:
    try:
        result = subprocess.run(["git", "--no-optional-locks", "-C", str(root), *args],
                                capture_output=True, timeout=60)
    except (OSError, subprocess.TimeoutExpired):
        return None
    return result.stdout if result.returncode == 0 else None


def source_identity(root: Path = ROOT) -> dict:
    """Commit plus the bytes of every uncommitted build input, or a whole-tree digest."""
    head = _git(root, "rev-parse", "--verify", "HEAD")
    top = _git(root, "rev-parse", "--show-toplevel")
    if head is None or top is None or Path(top.decode().strip()).resolve() != root.resolve():
        try:
            digest, count = inputs.digest_tree()
        except (ValueError, OSError) as error:
            return {"kind": "unknown", "reason": str(error)}
        return {"kind": "tree", "inputs_sha256": digest, "input_count": count}
    status = _git(root, "status", "--porcelain=v1", "-z", "--untracked-files=all", "--no-renames")
    if status is None:
        return {"kind": "unknown", "reason": "git status failed"}
    tool_dirs = _tool_dirs(root)
    dirty: dict[str, str | None] = {}
    for entry in status.decode("utf-8", "surrogateescape").split("\0"):
        if len(entry) < 4:
            continue
        relative = entry[3:]
        if not is_build_input(relative, tool_dirs):
            continue
        path = root / relative
        dirty[relative] = sha256_file(path) if path.is_file() else None
    return {"kind": "git", "commit": head.decode().strip(), "dirty_inputs": dict(sorted(dirty.items()))}


def source_label(source: dict) -> str:
    kind = source.get("kind")
    if kind == "git":
        count = len(source.get("dirty_inputs") or {})
        return source["commit"][:12] + (f" + {count} uncommitted input file{'s' if count != 1 else ''}" if count else " (clean)")
    if kind == "tree":
        return "tree digest " + str(source.get("inputs_sha256", ""))[:12]
    return "unknown source"


# --------------------------------------------------------------------------- toolchain


def tool_identity(command: str) -> dict:
    words = shlex.split(command)
    executable = shutil.which(words[0]) if words else None
    if executable is None:
        return {"command": command, "missing": True}
    path = Path(executable).resolve()
    shown = path.relative_to(ROOT).as_posix() if path.is_relative_to(ROOT) else str(path)
    identity = {"command": Path(words[0]).name, "path": shown, "sha256": sha256_file(path)}
    if path.name.endswith("gcc"):
        try:
            version = subprocess.run([str(path), "--version"], capture_output=True, text=True, timeout=30).stdout
            identity["version"] = version.splitlines()[0] if version else ""
        except (OSError, subprocess.TimeoutExpired):
            pass
    return identity


# --------------------------------------------------------------------------- layout


def _parse_dwarf(text: str) -> dict[int, dict]:
    """objdump --dwarf=info text -> {offset: {tag, attrs, children}} for one object."""
    dies: dict[int, dict] = {}
    stack: list[tuple[int, int]] = []
    current = None
    header = re.compile(r"^\s*<(\d+)><([0-9a-f]+)>: Abbrev Number: (\d+)(?: \((DW_TAG_\w+)\))?")
    attribute = re.compile(r"^\s*<[0-9a-f]+>\s+(DW_AT_\w+)\s*:\s*(.*)$")
    for line in text.splitlines():
        match = header.match(line)
        if match:
            depth, offset, tag = int(match[1]), int(match[2], 16), match[4]
            if tag is None:  # abbrev 0: end of a sibling chain
                current = None
                continue
            current = {"tag": tag, "attrs": {}, "children": []}
            dies[offset] = current
            while stack and stack[-1][0] >= depth:
                stack.pop()
            if stack:
                dies[stack[-1][1]]["children"].append(offset)
            stack.append((depth, offset))
            continue
        match = attribute.match(line)
        if match and current is not None:
            current["attrs"][match[1]] = match[2].strip()
    return dies


def _name(value: str | None) -> str | None:
    if value is None:
        return None
    # "(indirect string, offset: 0x12): SaveBlock1", "(strx1) SaveBlock1" or "SaveBlock1".
    return re.sub(r"^\([^)]*\):?\s*", "", value).strip()


def _number(value: str | None) -> int | None:
    if value is None:
        return None
    match = re.search(r"(?:DW_OP_plus_uconst:\s*)(\d+)", value) or re.match(r"^(-?(?:0x[0-9a-fA-F]+|\d+))", value)
    return int(match[1], 0) if match else None


def _ref(value: str | None) -> int | None:
    match = re.search(r"<0x([0-9a-f]+)>", value or "")
    return int(match[1], 16) if match else None


class Layout:
    """Canonical, name-bearing binary layout of compiled types."""

    def __init__(self, dies: dict[int, dict]):
        self.dies = dies
        self.memo: dict[int, object] = {}

    def describe(self, offset: int | None):
        if offset is None:
            return ["void"]
        if offset in self.memo:
            return self.memo[offset]
        die = self.dies[offset]
        tag, attrs = die["tag"], die["attrs"]
        size = _number(attrs.get("DW_AT_byte_size"))
        if tag in ("DW_TAG_typedef", "DW_TAG_const_type", "DW_TAG_volatile_type",
                   "DW_TAG_restrict_type", "DW_TAG_atomic_type"):
            result = self.describe(_ref(attrs.get("DW_AT_type")))
        elif tag == "DW_TAG_base_type":
            result = ["base", size, _number(attrs.get("DW_AT_encoding"))]
        elif tag in ("DW_TAG_pointer_type", "DW_TAG_subroutine_type"):
            result = ["pointer", size or 4]
        elif tag == "DW_TAG_enumeration_type":
            result = ["enum", size]
        elif tag == "DW_TAG_array_type":
            bounds = []
            for child in die["children"]:
                sub = self.dies[child]["attrs"]
                count = _number(sub.get("DW_AT_count"))
                upper = _number(sub.get("DW_AT_upper_bound"))
                bounds.append(count if count is not None else (upper + 1 if upper is not None else -1))
            result = ["array", bounds, self.describe(_ref(attrs.get("DW_AT_type")))]
        elif tag in ("DW_TAG_structure_type", "DW_TAG_union_type"):
            if "DW_AT_declaration" in attrs:
                raise ValueError(f"incomplete type in save layout: {_name(attrs.get('DW_AT_name'))}")
            self.memo[offset] = ["recursive", _name(attrs.get("DW_AT_name"))]
            members = []
            for child in die["children"]:
                member = self.dies[child]
                if member["tag"] != "DW_TAG_member":
                    continue
                m = member["attrs"]
                members.append([_name(m.get("DW_AT_name")), _number(m.get("DW_AT_data_member_location")),
                                _number(m.get("DW_AT_data_bit_offset")), _number(m.get("DW_AT_bit_offset")),
                                _number(m.get("DW_AT_bit_size")), self.describe(_ref(m.get("DW_AT_type")))])
            result = ["struct" if tag == "DW_TAG_structure_type" else "union", size, members]
        else:
            raise ValueError(f"unsupported DWARF type in save layout: {tag}")
        self.memo[offset] = result
        return result


def _run(command: list[str], **kwargs) -> subprocess.CompletedProcess:
    result = subprocess.run(command, capture_output=True, text=True, timeout=300, **kwargs)
    if result.returncode:
        raise ValueError(f"{' '.join(command[:3])} ... failed:\n{(result.stdout + result.stderr)[-3000:]}")
    return result


def compile_flags(flags: str) -> list[str]:
    # DWARF only exists in non-LTO objects; the layout does not depend on -O.
    return [flag for flag in shlex.split(flags)
            if not flag.startswith(("-flto", "-fno-fat-lto", "-O", "-ffunction-sections", "-fdata-sections"))]


def save_layout(cc: str, objdump: str, flags: str, work: Path) -> dict:
    """Compile the save structures with the ROM's compiler and flags and fingerprint their layout."""
    work.mkdir(parents=True, exist_ok=True)
    base = [*shlex.split(cc), *compile_flags(flags), "-O0"]
    ids_source = work / "save_ids_macros.c"
    ids_source.write_text("".join(f'#include "{h}"\n' for h in ID_HEADERS))
    macros = _run([*base, "-E", "-dM", str(ids_source)], cwd=ROOT).stdout
    names = sorted(set(re.findall(r"(?m)^#define ((?:FLAG|VAR)_[A-Za-z0-9_]+) \S", macros)))
    probe = work / "save_layout_probe.c"
    text = "".join(f'#include "{h}"\n' for h in LAYOUT_HEADERS)
    text += "".join(f"struct {name} gEcLayoutProbe{name};\n" for name in LAYOUT_ROOTS)
    text += "enum EcLayoutProbeIds {\n" + "".join(f"    EC_LAYOUT_ID_{n} = ({n}),\n" for n in names)
    text += "    EC_LAYOUT_ID_END_\n};\nenum EcLayoutProbeIds gEcLayoutProbeIds;\n"
    probe.write_text(text)
    obj = work / "save_layout_probe.o"
    _run([*base, "-g", "-gno-strict-dwarf", "-c", str(probe), "-o", str(obj)], cwd=ROOT)
    dies = _parse_dwarf(_run([*shlex.split(objdump), "--dwarf=info", str(obj)]).stdout)
    layout = Layout(dies)
    variables = {_name(d["attrs"].get("DW_AT_name")): d for d in dies.values() if d["tag"] == "DW_TAG_variable"}
    roots = {}
    for name in LAYOUT_ROOTS:
        variable = variables.get(f"gEcLayoutProbe{name}")
        if variable is None:
            raise ValueError(f"no DWARF for struct {name}")
        described = layout.describe(_ref(variable["attrs"].get("DW_AT_type")))
        roots[name] = {"size": described[1], "sha256": sha256_bytes(canonical(described))}
    enum_die = next((d for d in dies.values() if d["tag"] == "DW_TAG_enumeration_type"
                     and _name(d["attrs"].get("DW_AT_name")) == "EcLayoutProbeIds"), None)
    if enum_die is None:
        raise ValueError("no DWARF for the flag/var numbering probe")
    values = {}
    for child in enum_die["children"]:
        attrs = dies[child]["attrs"]
        name = _name(attrs.get("DW_AT_name")) or ""
        if name.startswith("EC_LAYOUT_ID_") and name != "EC_LAYOUT_ID_END_":
            values[name.removeprefix("EC_LAYOUT_ID_")] = _number(attrs.get("DW_AT_const_value"))
    if set(values) != set(names):
        raise ValueError("flag/var numbering probe lost constants")
    ids = {"flags": sum(n.startswith("FLAG_") for n in values), "vars": sum(n.startswith("VAR_") for n in values),
           "sha256": sha256_bytes(canonical(values))}
    fingerprint = sha256_bytes(canonical({"roots": {k: v["sha256"] for k, v in roots.items()}, "ids": ids["sha256"]}))
    return {"schema": 1, "id": "layout1:" + fingerprint, "roots": roots, "ids": ids,
            "method": "DWARF of the ROM compiler's own build of the save structures; compiled flag/var values"}


# --------------------------------------------------------------------------- files


def write_if_changed(path: Path, data: bytes) -> bool:
    if path.is_file() and path.read_bytes() == data:
        return False
    # GNU make 3.81 compares whole seconds: a changed prerequisite must be
    # strictly newer than outputs built just before this call.
    now = time.time()
    time.sleep(int(now) + 1 - now)
    write_atomic(path, data)
    return True


def write_atomic(path: Path, data: bytes) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(dir=path.parent, prefix=path.name + ".", delete=False) as stream:
        temporary = Path(stream.name)
        stream.write(data)
    temporary.replace(path)


def embedded_ids(rom: bytes) -> list[str]:
    return [match[1].decode() for match in EMBEDDED.finditer(rom)]


def stamp_path(rom: Path) -> Path:
    return rom.with_name(rom.stem + ".provenance.json")


def elf_symbol(elf: Path, nm: str | None = None) -> int | None:
    nm = nm or shutil.which("arm-none-eabi-nm")
    if nm is None:
        raise ValueError("arm-none-eabi-nm is required to read the provenance symbol")
    out = _run([*shlex.split(nm), str(elf)]).stdout
    match = re.search(rf"(?m)^([0-9a-fA-F]+) [A-Za-z] {SYMBOL}$", out)
    return int(match[1], 16) if match else None


# --------------------------------------------------------------------------- commands


def prepare(args) -> int:
    description = {
        "schema": SCHEMA,
        "source": source_identity(),
        "config": dict(sorted(value.split("=", 1) for value in args.value)),
        "toolchain": [tool_identity(command) for command in args.tool],
    }
    identifier = build_id(description)
    record = json.dumps({"build_id": identifier, "description": description}, indent=2, sort_keys=True).encode() + b"\n"
    if not args.json.is_file() or args.json.read_bytes() != record:
        write_atomic(args.json, record)  # not a make prerequisite; only the assembly needs the time guard
    asm = (f"@ Generated by scripts/build_provenance.py; see {args.json.name}.\n"
           '\t.section .text.consts,"a",%progbits\n\t.align 2\n'
           f"\t.global {SYMBOL}\n\t.type {SYMBOL}, %object\n{SYMBOL}:\n"
           f'\t.asciz "{MAGIC.decode()}{identifier}"\n\t.size {SYMBOL}, .-{SYMBOL}\n\t.align 2\n')
    write_if_changed(args.asm, asm.encode())
    return 0


def finalize(args) -> int:
    prepared = json.loads(args.prepared.read_text())
    identifier, description = prepared["build_id"], prepared["description"]
    if build_id(description) != identifier:
        raise ValueError(f"{args.prepared} does not hash to its build id")
    rom_bytes = args.rom.read_bytes()
    found = embedded_ids(rom_bytes)
    if found != [identifier]:
        raise ValueError(f"{args.rom.name} embeds {found or 'no build id'}, expected {identifier}")
    address = elf_symbol(args.elf, args.nm)
    offset = rom_bytes.find(MAGIC + identifier.encode())
    if address is None or address - ROM_BASE != offset:
        raise ValueError(f"{args.elf.name} does not place {SYMBOL} at the ROM's embedded identifier")
    try:
        layout = save_layout(args.cc, args.objdump, args.flags, args.work)
    except (ValueError, OSError, KeyError, IndexError, TypeError, subprocess.TimeoutExpired) as error:
        print(f"WARNING: save layout unavailable for {args.rom.name}: {error}", file=sys.stderr)
        layout = {"schema": 1, "id": None, "error": str(error)[-2000:]}
    after = source_identity()
    stamp = {
        "schema": SCHEMA,
        "build_id": identifier,
        "embedded": {"symbol": SYMBOL, "address": f"0x{address:08x}", "text": MAGIC.decode() + identifier},
        "description": description,
        "source_label": source_label(description["source"]),
        "source_stable": after == description["source"],
        "save_layout": layout,
        "artifacts": {
            "rom": {"name": args.rom.name, "sha256": sha256_bytes(rom_bytes), "size": len(rom_bytes)},
            "elf": {"name": args.elf.name, "sha256": sha256_file(args.elf), "size": args.elf.stat().st_size},
        },
        "built_at": datetime.now(timezone.utc).isoformat(timespec="seconds"),
    }
    if not stamp["source_stable"]:
        stamp["source_after_build"] = after
        print(f"WARNING: build inputs changed while {args.rom.name} was built; "
              "its provenance is recorded as unverified. Build again.", file=sys.stderr)
    write_atomic(args.out, json.dumps(stamp, indent=2, sort_keys=True).encode() + b"\n")
    print(f"provenance {identifier[:12]} ({stamp['source_label']}; save layout "
          f"{(layout.get('id') or 'unavailable')[:20]}) -> {args.out.name}")
    return 0


# --------------------------------------------------------------------------- consumers


def verify(rom: Path, elf: Path | None = None, stamp: Path | None = None) -> dict:
    """Validate a ROM (and optionally its ELF) against its stamp; never raises for bad data.

    Returns a summary suitable for evidence records: status is "verified",
    "unverified" (with a reason) or "legacy" (built before provenance existed).
    """
    stamp = stamp or stamp_path(rom)
    rom_bytes = rom.read_bytes()
    rom_hash = sha256_bytes(rom_bytes)
    found = embedded_ids(rom_bytes)
    summary: dict = {"status": "unverified", "rom_sha256": rom_hash, "embedded_build_id": found[0] if len(found) == 1 else None}
    if not stamp.is_file():
        if not found:
            summary.update(status="legacy", reason="built before build provenance: no embedded identifier or stamp")
        else:
            summary["reason"] = f"embedded build id present but its stamp {stamp.name} is missing"
        return summary
    try:
        raw = stamp.read_bytes()
        data = json.loads(raw)
        summary["stamp_sha256"] = sha256_bytes(raw)
        identifier, description = data["build_id"], data["description"]
        source = description.get("source", {})
        summary.update(build_id=identifier, source=source, source_label=source_label(source),
                       config=description.get("config", {}),
                       compiler=next((t.get("version") for t in description.get("toolchain", []) if t.get("version")), None),
                       save_layout=(data.get("save_layout") or {}).get("id"), built_at=data.get("built_at"))
        problems = []
        if data.get("schema") != SCHEMA:
            problems.append(f"unsupported stamp schema {data.get('schema')!r}")
        if build_id(description) != identifier:
            problems.append("stamp description does not hash to its build id")
        if found != [identifier]:
            problems.append(f"ROM embeds {found or 'no build id'}, stamp says {identifier}")
        artifacts = data.get("artifacts", {})
        if artifacts.get("rom", {}).get("sha256") != rom_hash:
            problems.append("ROM bytes differ from the stamp")
        if elf is not None and artifacts.get("elf", {}).get("sha256") != sha256_file(elf):
            problems.append("ELF bytes differ from the stamp")
        if data.get("source_stable") is not True:
            problems.append("build inputs changed while this ROM was being built")
        if source.get("kind") not in ("git", "tree"):
            problems.append("source identity unavailable at build time")
    except (OSError, ValueError, KeyError, TypeError, AttributeError) as error:
        problems = [f"unreadable stamp: {error}"]
    if problems:
        summary["reason"] = "; ".join(problems)
    else:
        summary["status"] = "verified"
    return summary


def carry(source_rom: Path, copy_rom: Path, copy_elf: Path | None = None) -> dict:
    """Give a byte-identical ROM copy its source's stamp, then verify the copy.

    The stamp is copied only when it proves the copy, so a snapshot never picks
    up the provenance of a different build that later replaced the source ROM.
    """
    source_stamp, target = stamp_path(source_rom), stamp_path(copy_rom)
    if source_stamp.is_file() and source_stamp.resolve() != target.resolve() \
            and verify(copy_rom, copy_elf, source_stamp)["status"] == "verified" \
            and (not target.is_file() or verify(copy_rom, copy_elf)["status"] != "verified"):
        write_atomic(target, source_stamp.read_bytes())
    return verify(copy_rom, copy_elf)


def label(summary: dict | None) -> str:
    """One human line for any provenance summary, including records made before it existed."""
    if not summary:
        return "legacy provenance (recorded before build stamps; source not proven)"
    status = summary.get("status")
    if status == "verified":
        return f"verified build {summary['build_id'][:12]} · {summary.get('source_label', '')}"
    if status == "legacy":
        return "legacy provenance (ROM built before build stamps; source not proven)"
    return "UNVERIFIED build: " + str(summary.get("reason", "unknown"))


def short_label(summary: dict | None) -> str:
    """Compact form for footers and contact-sheet headers; the reason lives in the record."""
    if not summary or summary.get("status") == "legacy":
        return "legacy provenance"
    if summary.get("status") == "verified":
        return f"verified {summary['build_id'][:10]} · {summary.get('source_label', '')}"
    return "UNVERIFIED build"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="command", required=True)
    p = sub.add_parser("prepare")
    p.add_argument("--asm", type=Path, required=True)
    p.add_argument("--json", type=Path, required=True)
    p.add_argument("--value", action="append", default=[])
    p.add_argument("--tool", action="append", default=[])
    f = sub.add_parser("finalize")
    f.add_argument("--prepared", type=Path, required=True)
    f.add_argument("--rom", type=Path, required=True)
    f.add_argument("--elf", type=Path, required=True)
    f.add_argument("--out", type=Path, required=True)
    f.add_argument("--cc", required=True)
    f.add_argument("--objdump", required=True)
    f.add_argument("--nm", required=True)
    f.add_argument("--flags", default="")
    f.add_argument("--work", type=Path, required=True)
    v = sub.add_parser("verify")
    v.add_argument("rom", type=Path)
    v.add_argument("--elf", type=Path)
    args = parser.parse_args()
    if args.command == "prepare":
        return prepare(args)
    if args.command == "finalize":
        return finalize(args)
    summary = verify(args.rom, args.elf)
    print(json.dumps(summary, indent=2, sort_keys=True))
    print(label(summary), file=sys.stderr)
    return 0 if summary["status"] == "verified" else 1


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (ValueError, OSError) as error:
        sys.exit(f"build provenance: {error}")
