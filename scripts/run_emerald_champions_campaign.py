#!/usr/bin/env python3
"""Run deterministic, checkpointed Emerald Champions campaign segments in mGBA."""

from __future__ import annotations

import argparse
import ast
import fcntl
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile

import render_emerald_champions_ui as ui
import verify_emerald_champions_campaign_capture_paths as capture_paths
import verify_emerald_champions_campaign_prerequisites as prerequisites
from rom_artifacts import verify_rom_elf_pair


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_MANIFEST = ROOT / "tests/campaign/playthrough.json"
DEFAULT_ROM = ROOT / "pokeemerald-playthrough.gba"
DEFAULT_ELF = ROOT / "pokeemerald-playthrough.elf"
DEFAULT_OUT = ROOT / "work/campaign-playthrough/current"
SCENARIO_NAME = "EC_HEADLESS_SCENARIO_CAMPAIGN_AUTOWIN"
TELEMETRY_SYMBOLS = (
    "gEcHeadlessFixtureSetupResult",
    "gEcHeadlessCampaignBattleSerial",
    "gEcHeadlessCampaignLastBattleType",
    "gEcHeadlessCampaignLastOpponentA",
    "gEcHeadlessCampaignLastOpponentB",
    "gEcHeadlessCampaignCaptureSerial",
    "gEcHeadlessCampaignLastCapturedSpecies",
    "gEcHeadlessCampaignLastCaptureResult",
    "gEcHeadlessCampaignLastResolution",
    "gEcHeadlessCampaignMapId",
    "gEcHeadlessCampaignMapGroup",
    "gEcHeadlessCampaignMapNum",
    "gEcHeadlessCampaignPlayerX",
    "gEcHeadlessCampaignPlayerY",
    "gEcHeadlessCampaignPlayerFacing",
    "gEcHeadlessCampaignControlsLocked",
    "gEcHeadlessCampaignScriptEnabled",
    "gEcHeadlessCampaignInBattle",
    "gEcHeadlessCampaignQueryKind",
    "gEcHeadlessCampaignQueryId",
    "gEcHeadlessCampaignQueryValue",
    "gEcHeadlessCampaignQueryObjectActive",
    "gEcHeadlessCampaignQueryObjectX",
    "gEcHeadlessCampaignQueryObjectY",
)
WATCH_PATTERN = re.compile(
    r"^WATCH frame=(?P<frame>\d+) address=(?P<address>[0-9a-f]{8}) "
    r"value=(?P<value>[0-9a-f]{8}) screenshot=(?P<path>.+)$",
    re.MULTILINE,
)


def fail(message: str) -> None:
    raise RuntimeError(message)


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def write_json_atomic(path: Path, payload: object) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    descriptor, temporary_name = tempfile.mkstemp(prefix=f".{path.name}-", dir=path.parent)
    os.close(descriptor)
    temporary = Path(temporary_name)
    try:
        temporary.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n")
        os.replace(temporary, path)
    finally:
        temporary.unlink(missing_ok=True)


def update_run_index(path: Path, run_id: str, summary: dict[str, object]) -> None:
    """Serialize targeted-run index updates so parallel branches cannot clobber one another."""
    path.parent.mkdir(parents=True, exist_ok=True)
    lock_path = path.with_suffix(path.suffix + ".lock")
    with lock_path.open("a+") as lock:
        fcntl.flock(lock.fileno(), fcntl.LOCK_EX)
        if path.is_file():
            index = json.loads(path.read_text())
            if not isinstance(index, dict) or index.get("schema_version") != 1:
                fail(f"targeted-run index has incompatible schema: {path}")
        else:
            index = {"schema_version": 1, "runs": {}}
        runs = index.setdefault("runs", {})
        if not isinstance(runs, dict):
            fail(f"targeted-run index has invalid run table: {path}")
        runs[run_id] = summary
        write_json_atomic(path, index)
        fcntl.flock(lock.fileno(), fcntl.LOCK_UN)


def snapshot_artifact(source: Path, directory: Path, label: str) -> tuple[Path, dict[str, object]]:
    """Take one immutable content-addressed copy for the lifetime of a run."""
    directory.mkdir(parents=True, exist_ok=True)
    source = source.resolve()
    before_stat = source.stat()
    source_hash_before = sha256(source)
    descriptor, temporary_name = tempfile.mkstemp(
        prefix=f".{label}-", suffix=source.suffix, dir=directory
    )
    os.close(descriptor)
    temporary = Path(temporary_name)
    try:
        shutil.copy2(source, temporary)
        digest = sha256(temporary)
        after_stat = source.stat()
        source_hash_after_copy = sha256(source)
        if source_hash_before != source_hash_after_copy:
            fail(f"{label} changed while its immutable run snapshot was being copied: {source}")
        if digest != source_hash_before:
            fail(f"{label} snapshot does not match its source: {source}")
        destination = directory / f"{label}-{digest}{source.suffix}"
        if destination.is_file():
            require_hash = sha256(destination)
            if require_hash != digest:
                fail(f"content-addressed artifact has wrong hash: {destination}")
            temporary.unlink()
        else:
            os.replace(temporary, destination)
        destination.chmod(0o444)
        return destination, {
            "label": label,
            "source": str(source),
            "source_size": before_stat.st_size,
            "source_mtime_ns_before": before_stat.st_mtime_ns,
            "source_mtime_ns_after_copy": after_stat.st_mtime_ns,
            "source_sha256_before": source_hash_before,
            "source_sha256_after_copy": source_hash_after_copy,
            "snapshot": str(destination),
            "snapshot_size": destination.stat().st_size,
            "snapshot_sha256": digest,
            "copy_race_detected": False,
        }
    except BaseException:
        temporary.unlink(missing_ok=True)
        raise


def verify_artifact_snapshot(evidence: dict[str, object]) -> dict[str, object]:
    """Prove that neither the input nor the content-addressed copy drifted."""
    source = Path(str(evidence["source"]))
    snapshot = Path(str(evidence["snapshot"]))
    expected = str(evidence["snapshot_sha256"])
    source_final = sha256(source)
    snapshot_final = sha256(snapshot)
    if source_final != evidence["source_sha256_before"]:
        fail(f"{evidence['label']} source changed during the campaign run: {source}")
    if snapshot_final != expected:
        fail(f"immutable {evidence['label']} snapshot changed during the campaign run: {snapshot}")
    result = dict(evidence)
    result.update({
        "source_sha256_after_run": source_final,
        "snapshot_sha256_after_run": snapshot_final,
        "run_race_detected": False,
        "verified_immutable": True,
    })
    return result


def parse_scenario_id() -> int:
    text = (ROOT / "include/emerald_champions_headless.h").read_text()
    region = text.split("enum EmeraldChampionsHeadlessScenario", 1)[1].split("};", 1)[0]
    names = re.findall(r"EC_HEADLESS_SCENARIO_[A-Z0-9_]+", region)
    if SCENARIO_NAME not in names:
        fail(f"missing campaign scenario: {SCENARIO_NAME}")
    return names.index(SCENARIO_NAME)


def parse_map_ids() -> dict[str, int]:
    text = (ROOT / "include/constants/map_groups.h").read_text()
    rows = re.findall(
        r"^\s*(MAP_[A-Z0-9_]+)\s*=\s*\((\d+)\s*\|\s*\((\d+)\s*<<\s*8\)\)",
        text,
        re.MULTILINE,
    )
    return {name: (int(group) << 8) | int(number) for name, number, group in rows}


def parse_numeric_constants() -> dict[str, int]:
    """Resolve the configured flag, variable, and direction constants."""
    compiler = shutil.which("cc") or shutil.which("clang") or shutil.which("gcc")
    if compiler is None:
        fail("a host C preprocessor is required for flag/variable assertions")
    command = [
        compiler, "-dM", "-E", "-Iinclude", "-Isrc", "-I.",
        "-DTRUE=1", "-DFALSE=0",
        "-include", "constants/flags.h",
        "-include", "constants/vars.h",
        "-include", "constants/event_objects.h",
        "-x", "c", "/dev/null",
    ]
    result = subprocess.run(command, cwd=ROOT, text=True, capture_output=True, check=False)
    if result.returncode != 0:
        fail("failed to preprocess campaign constants:\n" + result.stderr)
    expressions = dict(re.findall(r"^#define\s+([A-Z][A-Za-z0-9_]+)\s+(.+)$", result.stdout, re.M))
    resolved: dict[str, int] = {}

    def evaluate_node(node: ast.AST, stack: tuple[str, ...]) -> int:
        if isinstance(node, ast.Constant) and isinstance(node.value, int):
            return node.value
        if isinstance(node, ast.Name):
            return evaluate(node.id, stack)
        if isinstance(node, ast.UnaryOp) and isinstance(node.op, (ast.UAdd, ast.USub, ast.Invert)):
            value = evaluate_node(node.operand, stack)
            if isinstance(node.op, ast.UAdd):
                return value
            if isinstance(node.op, ast.USub):
                return -value
            return ~value
        if isinstance(node, ast.BinOp) and isinstance(
            node.op, (ast.Add, ast.Sub, ast.Mod, ast.BitOr, ast.BitAnd, ast.LShift, ast.RShift)
        ):
            left = evaluate_node(node.left, stack)
            right = evaluate_node(node.right, stack)
            if isinstance(node.op, ast.Add): return left + right
            if isinstance(node.op, ast.Sub): return left - right
            if isinstance(node.op, ast.Mod): return left % right
            if isinstance(node.op, ast.BitOr): return left | right
            if isinstance(node.op, ast.BitAnd): return left & right
            if isinstance(node.op, ast.LShift): return left << right
            return left >> right
        raise ValueError(ast.dump(node))

    def evaluate(name: str, stack: tuple[str, ...] = ()) -> int:
        if name in resolved:
            return resolved[name]
        if name not in expressions or name in stack:
            raise ValueError(name)
        expression = re.sub(r"\b(?:u|U|l|L)+\b", "", expressions[name])
        value = evaluate_node(ast.parse(expression, mode="eval").body, stack + (name,))
        resolved[name] = value
        return value

    for name in expressions:
        if name.startswith(("FLAG_", "VAR_", "DIR_")):
            try:
                evaluate(name)
            except (SyntaxError, ValueError):
                pass
    return resolved


def load_manifest(path: Path) -> dict[str, object]:
    data = json.loads(path.read_text())
    if data.get("schema_version") != 1:
        fail("campaign manifest schema_version must be 1")
    segments = data.get("segments")
    if not isinstance(segments, list) or not segments:
        fail("campaign manifest has no segments")
    ids = [row.get("id") for row in segments if isinstance(row, dict)]
    if len(ids) != len(segments) or len(ids) != len(set(ids)) or not all(isinstance(x, str) for x in ids):
        fail("campaign segment IDs must be unique strings")
    prior: set[str] = set()
    for segment in segments:
        parent = segment.get("parent")
        if parent is not None and parent not in prior:
            fail(f"{segment['id']}: parent {parent!r} must name an earlier segment")
        if not isinstance(segment.get("frames"), int) or segment["frames"] <= 0:
            fail(f"{segment['id']}: frames must be positive")
        semantic_actions = segment.get("semantic_actions", [])
        if not isinstance(semantic_actions, list):
            fail(f"{segment['id']}: semantic_actions must be a list")
        allowed_actions = {
            "press", "wait_stable_overworld", "advance_dialogue",
            "advance_until_battle", "face", "step", "walk_to", "screenshot",
            "interact_object", "interact_tile",
        }
        for index, action in enumerate(semantic_actions):
            if not isinstance(action, dict) or action.get("type") not in allowed_actions:
                fail(f"{segment['id']}: invalid semantic action {index}")
        expected = segment.get("expected", {})
        if not isinstance(expected, dict):
            fail(f"{segment['id']}: expected must be an object")
        for field in ("flags", "vars"):
            if field in expected and not isinstance(expected[field], dict):
                fail(f"{segment['id']}: expected.{field} must be an object")
        prior.add(segment["id"])
    return data


def segment_ancestry(segment_id: str, by_id: dict[str, dict[str, object]]) -> list[str]:
    result = []
    current: str | None = segment_id
    while current is not None:
        result.append(current)
        parent = by_id[current].get("parent")
        current = str(parent) if parent is not None else None
    return list(reversed(result))


def select_segments(
    segments: list[dict[str, object]], start: str | None, through: str | None,
    suite: str | None = None,
) -> list[dict[str, object]]:
    by_id = {str(segment["id"]): segment for segment in segments}
    if suite is not None:
        selected_ids: set[str] = set()
        for segment in segments:
            declared = segment.get("suite", segment.get("suites", []))
            declared_suites = [declared] if isinstance(declared, str) else declared
            if isinstance(declared_suites, list) and suite in declared_suites:
                selected_ids.update(segment_ancestry(str(segment["id"]), by_id))
        if not selected_ids:
            fail(f"unknown or empty campaign suite: {suite}")
        return [segment for segment in segments if segment["id"] in selected_ids]
    if through is not None:
        chain = segment_ancestry(through, by_id)
        if start is not None:
            if start not in chain:
                fail(f"--start {start} is not an ancestor of --through {through}")
            chain = chain[chain.index(start):]
        return [by_id[segment_id] for segment_id in chain]
    if start is None:
        return segments
    descendants = []
    for segment in segments:
        ancestry = segment_ancestry(str(segment["id"]), by_id)
        if start in ancestry:
            descendants.append(segment)
    return descendants


def safe_run_slug(value: str) -> str:
    if not re.fullmatch(r"[a-z0-9][a-z0-9._-]{0,79}", value):
        fail(f"unsafe run/suite name: {value!r}")
    return value


def validate_manifest_symbols(
    segments: list[dict[str, object]], map_ids: dict[str, int], constants: dict[str, int]
) -> None:
    for segment in segments:
        expected = segment.get("expected", {})
        map_name = expected.get("map")
        if map_name is not None and map_name not in map_ids:
            fail(f"{segment['id']}: unknown map {map_name}")
        for field, prefix in (("flags", "FLAG_"), ("vars", "VAR_")):
            for name, wanted in expected.get(field, {}).items():
                if not isinstance(name, str) or not name.startswith(prefix) or name not in constants:
                    fail(f"{segment['id']}: unknown {field[:-1]} constant {name}")
                if field == "flags" and wanted not in (False, True, 0, 1):
                    fail(f"{segment['id']}: flag assertion {name} must be boolean")
                if field == "vars" and not isinstance(wanted, int):
                    fail(f"{segment['id']}: variable assertion {name} must be an integer")


def state_metadata_path(state: Path) -> Path:
    return Path(str(state) + ".json")


def validate_state_input(
    state: Path, rom_hash: str, *, expected_segment: str | None = None,
    artifact_evidence: dict[str, dict[str, object]] | None = None,
) -> None:
    if not state.is_file():
        fail(f"checkpoint is missing: {state}")
    metadata_path = state_metadata_path(state)
    if not metadata_path.is_file():
        fail(f"checkpoint metadata is missing: {metadata_path}")
    metadata = json.loads(metadata_path.read_text())
    if not isinstance(metadata, dict) or metadata.get("schema_version") != 1:
        fail(f"checkpoint {state.name} has invalid metadata schema; regenerate it")
    state_hash = metadata.get("state_sha256")
    if not isinstance(state_hash, str) or not re.fullmatch(r"[0-9a-f]{64}", state_hash):
        fail(f"checkpoint {state.name} lacks a valid state hash; regenerate it")
    if sha256(state) != state_hash:
        fail(f"checkpoint {state.name} contents do not match its recorded state hash")
    expected_segment = expected_segment if expected_segment is not None else state.stem
    if metadata.get("segment") != expected_segment:
        fail(f"checkpoint {state.name} is segment {metadata.get('segment')!r}, expected parent {expected_segment!r}")
    if metadata.get("rom_sha256") != rom_hash:
        fail(
            f"checkpoint {state.name} belongs to ROM {metadata.get('rom_sha256')}, "
            f"not {rom_hash}"
        )
    recorded = metadata.get("artifact_evidence")
    if not isinstance(recorded, dict):
        fail(f"checkpoint {state.name} lacks artifact provenance; regenerate it")
    for label in ("rom", "elf", "manifest"):
        evidence = recorded.get(label)
        digest = evidence.get("snapshot_sha256") if isinstance(evidence, dict) else None
        if not isinstance(digest, str) or not re.fullmatch(r"[0-9a-f]{64}", digest):
            fail(f"checkpoint {state.name} lacks valid {label} provenance; regenerate it")
        if any(evidence.get(field) != digest for field in ("source_sha256_before", "source_sha256_after_copy")):
            fail(f"checkpoint {state.name} has inconsistent {label} provenance")
        if label == "rom" and digest != rom_hash:
            fail(f"checkpoint {state.name} ROM provenance disagrees with its ROM identity")
        if artifact_evidence is not None and digest != artifact_evidence.get(label, {}).get("snapshot_sha256"):
            if label != "manifest":
                fail(f"checkpoint {state.name} belongs to different {label} inputs; regenerate the parent checkpoint")
    if "parent" not in metadata:
        fail(f"checkpoint {state.name} lacks recorded parent lineage; regenerate it")
    current_manifest = artifact_evidence.get("manifest", {}) if artifact_evidence is not None else recorded["manifest"]
    validate_checkpoint_ancestry(expected_segment, recorded["manifest"], current_manifest, metadata["parent"])


def validate_checkpoint_ancestry(
    segment: str, recorded: dict[str, object], current: dict[str, object], recorded_parent: object,
) -> None:
    """Permit unrelated manifest extensions without trusting changed ancestry."""
    chains = []
    manifests = (("current", current),) if recorded.get("snapshot_sha256") == current.get("snapshot_sha256") else (("original", recorded), ("current", current))
    for label, evidence in manifests:
        path_value = evidence.get("snapshot")
        if not isinstance(path_value, str) or not path_value:
            fail(f"checkpoint {segment}: {label} manifest snapshot is unavailable; cannot verify ancestry, regenerate checkpoint")
        path = Path(path_value)
        if not path.is_file() or sha256(path) != evidence.get("snapshot_sha256"):
            fail(f"checkpoint {segment}: {label} manifest snapshot is missing or changed; regenerate checkpoint")
        manifest = load_manifest(path)
        by_id = {row["id"]: row for row in manifest["segments"]}
        if segment not in by_id:
            fail(f"checkpoint {segment}: segment is absent from {label} manifest")
        chains.append([by_id[name] for name in segment_ancestry(segment, by_id)])
    if len(chains) == 2 and chains[0] != chains[1]:
        fail(f"checkpoint {segment}: manifest ancestry changed; regenerate the parent checkpoint")
    if recorded_parent != chains[-1][-1].get("parent"):
        fail(f"checkpoint {segment}: recorded parent disagrees with its manifest definition")


def select_parent_checkpoint(
    parent: str, *, completed_segments: set[str], run_out: Path, out: Path,
    parent_run_id: str | None,
) -> Path:
    # A parent completed in this invocation is authoritative. Otherwise an
    # explicit --parent-run must be honored, even when its file is missing.
    # Existing local files from an earlier invocation cannot override it.
    local = run_out / "checkpoints" / f"{parent}.ss1"
    if parent in completed_segments:
        return local
    if parent_run_id is not None:
        return out / "runs" / parent_run_id / "checkpoints" / f"{parent}.ss1"
    return local if local.is_file() else out / "checkpoints" / f"{parent}.ss1"


def telemetry_from_stdout(stdout: str, addresses: dict[str, int]) -> dict[str, int]:
    reads = {
        int(address, 16): int(value, 16)
        for address, value in ui.READ_PATTERN.findall(stdout)
    }
    missing = [name for name, address in addresses.items() if address not in reads]
    if missing:
        fail(f"runner omitted telemetry reads: {missing}")
    return {name: reads[address] for name, address in addresses.items()}


def is_stable_overworld(telemetry: dict[str, int]) -> bool:
    return (
        telemetry["gEcHeadlessFixtureSetupResult"] == 1
        and telemetry["gEcHeadlessCampaignControlsLocked"] == 0
        and telemetry["gEcHeadlessCampaignScriptEnabled"] == 0
        and telemetry["gEcHeadlessCampaignInBattle"] == 0
    )


def validate_expected(
    segment: dict[str, object], telemetry: dict[str, int], map_ids: dict[str, int]
) -> None:
    expected = segment.get("expected", {})
    if not isinstance(expected, dict):
        fail(f"{segment['id']}: expected must be an object")
    map_name = expected.get("map")
    if map_name is not None:
        if map_name not in map_ids:
            fail(f"{segment['id']}: unknown map {map_name}")
        actual = telemetry["gEcHeadlessCampaignMapId"]
        if actual != map_ids[map_name]:
            fail(f"{segment['id']}: expected {map_name}, observed map ID 0x{actual:04x}")
    if "position" in expected:
        position = expected["position"]
        actual = [
            telemetry["gEcHeadlessCampaignPlayerX"],
            telemetry["gEcHeadlessCampaignPlayerY"],
        ]
        if actual != position:
            fail(f"{segment['id']}: expected position {position}, observed {actual}")
    if "facing" in expected:
        directions = {"SOUTH": 1, "NORTH": 2, "WEST": 3, "EAST": 4}
        facing = expected["facing"]
        wanted = directions.get(str(facing).removeprefix("DIR_"), facing)
        actual = telemetry["gEcHeadlessCampaignPlayerFacing"]
        if actual != wanted:
            fail(f"{segment['id']}: expected facing {facing}, observed direction {actual}")
    minimum = expected.get("battle_serial_min")
    if minimum is not None and telemetry["gEcHeadlessCampaignBattleSerial"] < minimum:
        fail(
            f"{segment['id']}: expected battle serial >= {minimum}, observed "
            f"{telemetry['gEcHeadlessCampaignBattleSerial']}"
        )
    capture_minimum = expected.get("capture_serial_min")
    if capture_minimum is not None and telemetry["gEcHeadlessCampaignCaptureSerial"] < capture_minimum:
        fail(
            f"{segment['id']}: expected capture serial >= {capture_minimum}, observed "
            f"{telemetry['gEcHeadlessCampaignCaptureSerial']}"
        )
    if "captured_species" in expected:
        actual = telemetry["gEcHeadlessCampaignLastCapturedSpecies"]
        if actual != expected["captured_species"]:
            fail(
                f"{segment['id']}: expected captured species {expected['captured_species']}, "
                f"observed {actual}"
            )
    if "capture_result" in expected:
        actual = telemetry["gEcHeadlessCampaignLastCaptureResult"]
        if actual != expected["capture_result"]:
            fail(
                f"{segment['id']}: expected capture result {expected['capture_result']}, "
                f"observed {actual}"
            )
    if expected.get("stable_overworld"):
        actual = (
            telemetry["gEcHeadlessCampaignControlsLocked"],
            telemetry["gEcHeadlessCampaignScriptEnabled"],
            telemetry["gEcHeadlessCampaignInBattle"],
        )
        if actual != (0, 0, 0):
            fail(f"{segment['id']}: expected stable overworld, observed lock/script/battle={actual}")


def run_state_chunk(
    *,
    runner: Path,
    rom: Path,
    state: Path,
    addresses: dict[str, int],
    frames: int,
    keys: list[tuple[int, int, str]] | None = None,
    writes: list[tuple[int, int, int, int]] | None = None,
    screenshot: Path | None = None,
    save_out: Path | None = None,
) -> tuple[dict[str, int], str]:
    """Advance one exact-ROM checkpoint and atomically replace it."""
    next_state = state.with_name(state.stem + ".next" + state.suffix)
    command = [
        str(runner), "--rom", str(rom), "--frames", str(frames),
        "--state-in", str(state), "--state-out", str(next_state),
    ]
    for frame, duration, key_names in keys or []:
        command.extend(("--key", f"{frame}:{duration}:{key_names}"))
    for frame, width, address, value in writes or []:
        command.extend(("--write", f"{frame}:{width}:0x{address:x}:{value}"))
    if screenshot is not None:
        screenshot.parent.mkdir(parents=True, exist_ok=True)
        command.extend(("--screenshot", str(screenshot)))
    if save_out is not None:
        save_out.parent.mkdir(parents=True, exist_ok=True)
        command.extend(("--save-out", str(save_out)))
    for name in TELEMETRY_SYMBOLS:
        command.extend(("--read", f"4:0x{addresses[name]:x}"))
    completed = subprocess.run(command, cwd=ROOT, text=True, capture_output=True, check=False)
    if completed.returncode != 0:
        next_state.unlink(missing_ok=True)
        fail(
            f"adaptive runner failed with {completed.returncode}\n"
            f"stdout:\n{completed.stdout}\nstderr:\n{completed.stderr}"
        )
    telemetry = telemetry_from_stdout(
        completed.stdout, {name: addresses[name] for name in TELEMETRY_SYMBOLS}
    )
    os.replace(next_state, state)
    return telemetry, completed.stdout


def query_campaign_value(
    *,
    kind: int,
    identifier: int,
    runner: Path,
    rom: Path,
    state: Path,
    addresses: dict[str, int],
) -> tuple[int, dict[str, int]]:
    telemetry, _ = run_state_chunk(
        runner=runner,
        rom=rom,
        state=state,
        addresses=addresses,
        frames=2,
        writes=[
            (0, 4, addresses["gEcHeadlessCampaignQueryId"], identifier),
            (0, 4, addresses["gEcHeadlessCampaignQueryKind"], kind),
        ],
    )
    return telemetry["gEcHeadlessCampaignQueryValue"], telemetry


def query_campaign_object(
    *,
    local_id: int,
    runner: Path,
    rom: Path,
    state: Path,
    addresses: dict[str, int],
) -> tuple[bool, tuple[int, int], dict[str, int]]:
    telemetry, _ = run_state_chunk(
        runner=runner,
        rom=rom,
        state=state,
        addresses=addresses,
        frames=2,
        writes=[
            (0, 4, addresses["gEcHeadlessCampaignQueryId"], local_id),
            (0, 4, addresses["gEcHeadlessCampaignQueryKind"], 3),
        ],
    )
    return (
        telemetry["gEcHeadlessCampaignQueryObjectActive"] != 0,
        (
            telemetry["gEcHeadlessCampaignQueryObjectX"],
            telemetry["gEcHeadlessCampaignQueryObjectY"],
        ),
        telemetry,
    )


def apply_semantic_actions(
    segment: dict[str, object],
    *,
    runner: Path,
    rom: Path,
    state: Path,
    addresses: dict[str, int],
    initial: dict[str, int],
    screenshot_dir: Path,
) -> tuple[dict[str, int], list[dict[str, object]]]:
    telemetry = initial
    trace: list[dict[str, object]] = []
    adaptive_trace_path = screenshot_dir / "adaptive-trace.jsonl"
    adaptive_trace_path.unlink(missing_ok=True)
    directions = {
        "UP": (0, -1, 2), "DOWN": (0, 1, 1),
        "LEFT": (-1, 0, 3), "RIGHT": (1, 0, 4),
    }

    def advance(
        label: str, frames: int, keys: str | None = None, key_duration: int = 2
    ) -> None:
        nonlocal telemetry
        before = telemetry.copy()
        telemetry, _ = run_state_chunk(
            runner=runner, rom=rom, state=state, addresses=addresses, frames=frames,
            keys=[] if keys is None else [(1, key_duration, keys)],
        )
        row = {"action": label, "before": before, "after": telemetry.copy()}
        trace.append(row)
        with adaptive_trace_path.open("a") as handle:
            handle.write(json.dumps(row, sort_keys=True) + "\n")

    for index, action in enumerate(segment.get("semantic_actions", [])):
        if not isinstance(action, dict) or not isinstance(action.get("type"), str):
            fail(f"{segment['id']}: semantic action {index} must have a type")
        action_type = action["type"]
        if action_type == "press":
            keys = action.get("keys")
            if not isinstance(keys, list) or not keys:
                fail(f"{segment['id']}: press action {index} needs keys")
            advance(
                f"press:{','.join(keys)}",
                int(action.get("frames", 30)),
                ",".join(keys),
                int(action.get("duration", 2)),
            )
        elif action_type == "wait_stable_overworld":
            consecutive = 0
            for _ in range(int(action.get("max_chunks", 40))):
                advance("wait-stable", int(action.get("chunk_frames", 30)))
                consecutive = consecutive + 1 if is_stable_overworld(telemetry) else 0
                if consecutive >= 2:
                    break
            else:
                fail(f"{segment['id']}: overworld did not become stably controllable")
        elif action_type == "advance_dialogue":
            if is_stable_overworld(telemetry) and not action.get("allow_initial_stable", False):
                fail(
                    f"{segment['id']}: advance_dialogue began from stable overworld; "
                    "the expected conversation was not active"
                )
            for press in range(int(action.get("max_presses", 40))):
                advance(f"dialogue-A-{press + 1}", int(action.get("frames_per_press", 90)), "A")
                if is_stable_overworld(telemetry):
                    advance("dialogue-stability-confirm", int(action.get("settle_frames", 30)))
                    if is_stable_overworld(telemetry):
                        break
            else:
                fail(f"{segment['id']}: dialogue did not return to stable overworld")
        elif action_type == "advance_until_battle":
            if telemetry["gEcHeadlessCampaignInBattle"] != 0:
                fail(f"{segment['id']}: advance_until_battle began inside a battle")
            for press in range(int(action.get("max_presses", 80))):
                advance(
                    f"battle-entry-A-{press + 1}",
                    int(action.get("frames_per_press", 12)),
                    "A",
                )
                if telemetry["gEcHeadlessCampaignInBattle"] != 0:
                    name = str(action.get("screenshot", "native-battle-entry"))
                    shot = screenshot_dir / f"{name}.png"
                    telemetry, _ = run_state_chunk(
                        runner=runner, rom=rom, state=state, addresses=addresses,
                        frames=int(action.get("screenshot_delay_frames", 1)), screenshot=shot,
                    )
                    row = {
                        "action": f"screenshot:{name}",
                        "path": str(shot),
                        "after": telemetry.copy(),
                    }
                    trace.append(row)
                    with adaptive_trace_path.open("a") as handle:
                        handle.write(json.dumps(row, sort_keys=True) + "\n")
                    break
            else:
                fail(f"{segment['id']}: input sequence never entered a battle")
        elif action_type == "face":
            direction = str(action.get("direction", "")).upper()
            if direction not in directions:
                fail(f"{segment['id']}: invalid facing direction {direction}")
            advance(f"face:{direction}", int(action.get("frames", 24)), direction)
            if telemetry["gEcHeadlessCampaignPlayerFacing"] != directions[direction][2]:
                fail(f"{segment['id']}: failed to face {direction}")
        elif action_type == "step":
            direction = str(action.get("direction", "")).upper()
            if direction not in directions:
                fail(f"{segment['id']}: invalid step direction {direction}")
            advance("step-settle", int(action.get("settle_frames", 30)))
            start_map = telemetry["gEcHeadlessCampaignMapId"]
            start_position = (
                telemetry["gEcHeadlessCampaignPlayerX"],
                telemetry["gEcHeadlessCampaignPlayerY"],
            )
            if telemetry["gEcHeadlessCampaignPlayerFacing"] != directions[direction][2]:
                advance(
                    f"step-face:{direction}",
                    int(action.get("face_frames", 24)),
                    direction,
                    int(action.get("face_duration", 2)),
                )
                faced_position = (
                    telemetry["gEcHeadlessCampaignPlayerX"],
                    telemetry["gEcHeadlessCampaignPlayerY"],
                )
                if telemetry["gEcHeadlessCampaignMapId"] != start_map or faced_position != start_position:
                    fail(f"{segment['id']}: facing phase of step {direction} displaced the player")
                if telemetry["gEcHeadlessCampaignPlayerFacing"] != directions[direction][2]:
                    fail(f"{segment['id']}: facing phase of step {direction} did not turn the player")
            advance(
                f"step-move:{direction}",
                int(action.get("move_frames", 30)),
                direction,
                int(action.get("move_duration", 2)),
            )
            allow_map_change = bool(action.get("allow_map_change", False))
            if allow_map_change:
                for _ in range(int(action.get("max_transition_chunks", 20))):
                    if telemetry["gEcHeadlessCampaignMapId"] != start_map:
                        break
                    advance("step-wait-map-change", int(action.get("transition_chunk_frames", 30)))
                else:
                    fail(f"{segment['id']}: step {direction} did not cross the expected map boundary")
            else:
                dx, dy, _ = directions[direction]
                expected_position = (start_position[0] + dx, start_position[1] + dy)
                actual_position = (
                    telemetry["gEcHeadlessCampaignPlayerX"],
                    telemetry["gEcHeadlessCampaignPlayerY"],
                )
                if telemetry["gEcHeadlessCampaignMapId"] != start_map or actual_position != expected_position:
                    fail(
                        f"{segment['id']}: step {direction} expected {expected_position} "
                        f"on map 0x{start_map:04x}, observed {actual_position} on "
                        f"0x{telemetry['gEcHeadlessCampaignMapId']:04x}"
                    )
        elif action_type == "walk_to":
            target = action.get("position")
            if not isinstance(target, list) or len(target) != 2 or not all(isinstance(v, int) for v in target):
                fail(f"{segment['id']}: walk_to action {index} needs [x, y]")
            order = str(action.get("order", "xy"))
            if sorted(order) != ["x", "y"]:
                fail(f"{segment['id']}: walk_to order must be xy or yx")
            maximum = int(action.get("max_steps", 256))
            attempts_per_step = int(action.get("max_attempts_per_step", 3))
            allow_battle_interruptions = bool(action.get("allow_battle_interruptions", False))
            steps = 0
            starting_map = telemetry["gEcHeadlessCampaignMapId"]
            while [telemetry["gEcHeadlessCampaignPlayerX"], telemetry["gEcHeadlessCampaignPlayerY"]] != target:
                if not is_stable_overworld(telemetry):
                    fail(f"{segment['id']}: lost stable overworld while walking to {target}")
                x = telemetry["gEcHeadlessCampaignPlayerX"]
                y = telemetry["gEcHeadlessCampaignPlayerY"]
                axis = next(axis for axis in order if (x if axis == "x" else y) != target[0 if axis == "x" else 1])
                if axis == "x":
                    direction = "RIGHT" if x < target[0] else "LEFT"
                else:
                    direction = "DOWN" if y < target[1] else "UP"
                before = (x, y)
                battle_serial_before = telemetry["gEcHeadlessCampaignBattleSerial"]
                for attempt in range(attempts_per_step):
                    advance(
                        f"walk:{direction}:attempt-{attempt + 1}",
                        int(action.get("frames_per_step", 24)),
                        direction,
                    )
                    if not is_stable_overworld(telemetry):
                        if not allow_battle_interruptions:
                            fail(
                                f"{segment['id']}: lost stable overworld walking "
                                f"{direction} at {before} toward {target}"
                            )
                        consecutive = 0
                        for interruption_press in range(
                            int(action.get("max_interruption_presses", 40))
                        ):
                            if is_stable_overworld(telemetry):
                                advance(
                                    f"walk-interruption-stability-{interruption_press + 1}",
                                    int(action.get("interruption_settle_frames", 30)),
                                )
                                consecutive = consecutive + 1 if is_stable_overworld(telemetry) else 0
                            else:
                                advance(
                                    f"walk-interruption-A-{interruption_press + 1}",
                                    int(action.get("interruption_frames_per_press", 90)),
                                    "A",
                                )
                                consecutive = 1 if is_stable_overworld(telemetry) else 0
                            if consecutive >= 2:
                                break
                        else:
                            fail(
                                f"{segment['id']}: battle interruption did not return "
                                "to stable overworld"
                            )
                        if telemetry["gEcHeadlessCampaignBattleSerial"] <= battle_serial_before:
                            fail(
                                f"{segment['id']}: allowed walking interruption did not "
                                "resolve a battle"
                            )
                    after = (
                        telemetry["gEcHeadlessCampaignPlayerX"],
                        telemetry["gEcHeadlessCampaignPlayerY"],
                    )
                    if after != before:
                        break
                else:
                    fail(f"{segment['id']}: blocked walking {direction} at {before} toward {target}")
                if telemetry["gEcHeadlessCampaignMapId"] != starting_map:
                    fail(f"{segment['id']}: walk_to crossed a map boundary before reaching {target}")
                old_distance = abs(before[0] - target[0]) + abs(before[1] - target[1])
                new_distance = abs(after[0] - target[0]) + abs(after[1] - target[1])
                if new_distance >= old_distance:
                    fail(
                        f"{segment['id']}: walking {direction} moved from {before} to {after} "
                        f"without approaching {target}"
                    )
                steps += 1
                if steps >= maximum:
                    fail(f"{segment['id']}: exceeded {maximum} steps walking to {target}")
        elif action_type == "interact_object":
            local_id = action.get("local_id")
            candidates = action.get("candidates")
            if not isinstance(local_id, int) or not 0 < local_id < 256:
                fail(f"{segment['id']}: interact_object needs a byte-sized positive local_id")
            if not isinstance(candidates, list) or not candidates:
                fail(f"{segment['id']}: interact_object needs declared adjacent candidates")
            normalized = []
            for candidate in candidates:
                if not isinstance(candidate, dict):
                    fail(f"{segment['id']}: interact_object candidate must be an object")
                position = candidate.get("position")
                waypoints = candidate.get("waypoints", [position])
                if (
                    not isinstance(position, list) or len(position) != 2
                    or not all(isinstance(value, int) for value in position)
                    or not isinstance(waypoints, list) or not waypoints
                    or any(
                        not isinstance(point, list) or len(point) != 2
                        or not all(isinstance(value, int) for value in point)
                        for point in waypoints
                    )
                ):
                    fail(f"{segment['id']}: invalid interact_object candidate geometry")
                order = str(candidate.get("order", "xy"))
                if sorted(order) != ["x", "y"]:
                    fail(f"{segment['id']}: interact_object candidate order must be xy or yx")
                normalized.append((position, waypoints, order))

            starting_map = telemetry["gEcHeadlessCampaignMapId"]
            failed_pairs: set[tuple[tuple[int, int], tuple[int, int]]] = set()
            allow_battle_interruptions = bool(action.get("allow_battle_interruptions", False))
            for relocation in range(int(action.get("max_relocations", 8))):
                active, object_position, telemetry = query_campaign_object(
                    local_id=local_id, runner=runner, rom=rom, state=state,
                    addresses=addresses,
                )
                if not active:
                    fail(f"{segment['id']}: local object {local_id} is not active on the current map")
                eligible = [
                    row for row in normalized
                    if abs(row[0][0] - object_position[0]) + abs(row[0][1] - object_position[1]) == 1
                    and (
                        bool(action.get("retry_failed_pairs", False))
                        or (object_position, tuple(row[0])) not in failed_pairs
                    )
                ]
                if not eligible:
                    fail(
                        f"{segment['id']}: object {local_id} moved to {object_position}, "
                        "outside every declared adjacent candidate"
                    )
                player_position = (
                    telemetry["gEcHeadlessCampaignPlayerX"],
                    telemetry["gEcHeadlessCampaignPlayerY"],
                )
                eligible.sort(key=lambda row: abs(row[0][0] - player_position[0]) + abs(row[0][1] - player_position[1]))
                candidate_position, waypoints, order = eligible[0]
                blocked = False
                for waypoint in waypoints:
                    steps = 0
                    while [telemetry["gEcHeadlessCampaignPlayerX"], telemetry["gEcHeadlessCampaignPlayerY"]] != waypoint:
                        if not is_stable_overworld(telemetry):
                            fail(f"{segment['id']}: lost field control approaching object {local_id}")
                        x = telemetry["gEcHeadlessCampaignPlayerX"]
                        y = telemetry["gEcHeadlessCampaignPlayerY"]
                        axis = next(
                            axis for axis in order
                            if (x if axis == "x" else y) != waypoint[0 if axis == "x" else 1]
                        )
                        direction = (
                            "RIGHT" if axis == "x" and x < waypoint[0]
                            else "LEFT" if axis == "x"
                            else "DOWN" if y < waypoint[1]
                            else "UP"
                        )
                        before = (x, y)
                        battle_serial_before = telemetry["gEcHeadlessCampaignBattleSerial"]
                        for attempt in range(int(action.get("max_attempts_per_step", 3))):
                            advance(
                                f"object-{local_id}-walk:{direction}:attempt-{attempt + 1}",
                                int(action.get("frames_per_step", 24)), direction,
                            )
                            if not is_stable_overworld(telemetry):
                                if not allow_battle_interruptions:
                                    fail(
                                        f"{segment['id']}: lost field control approaching "
                                        f"object {local_id} at {before}"
                                    )
                                consecutive = 0
                                for interruption_press in range(
                                    int(action.get("max_interruption_presses", 40))
                                ):
                                    if is_stable_overworld(telemetry):
                                        advance(
                                            f"object-{local_id}-interruption-stability-"
                                            f"{interruption_press + 1}",
                                            int(action.get("interruption_settle_frames", 30)),
                                        )
                                        consecutive = (
                                            consecutive + 1
                                            if is_stable_overworld(telemetry) else 0
                                        )
                                    else:
                                        advance(
                                            f"object-{local_id}-interruption-A-"
                                            f"{interruption_press + 1}",
                                            int(action.get("interruption_frames_per_press", 90)),
                                            "A",
                                        )
                                        consecutive = 1 if is_stable_overworld(telemetry) else 0
                                    if consecutive >= 2:
                                        break
                                else:
                                    fail(
                                        f"{segment['id']}: battle interruption while approaching "
                                        f"object {local_id} did not return to stable overworld"
                                    )
                                if telemetry["gEcHeadlessCampaignBattleSerial"] <= battle_serial_before:
                                    fail(
                                        f"{segment['id']}: allowed object-approach interruption "
                                        "did not resolve a battle"
                                    )
                            after = (
                                telemetry["gEcHeadlessCampaignPlayerX"],
                                telemetry["gEcHeadlessCampaignPlayerY"],
                            )
                            if after != before:
                                break
                        else:
                            blocked = True
                            break
                        if telemetry["gEcHeadlessCampaignMapId"] != starting_map:
                            fail(f"{segment['id']}: object approach crossed a map boundary")
                        steps += 1
                        if steps >= int(action.get("max_steps_per_waypoint", 128)):
                            fail(f"{segment['id']}: object approach exceeded its step budget")
                    if blocked:
                        break
                if blocked:
                    failed_pairs.add((object_position, tuple(candidate_position)))
                    continue

                active, live_position, telemetry = query_campaign_object(
                    local_id=local_id, runner=runner, rom=rom, state=state,
                    addresses=addresses,
                )
                player_position = (
                    telemetry["gEcHeadlessCampaignPlayerX"],
                    telemetry["gEcHeadlessCampaignPlayerY"],
                )
                delta = (live_position[0] - player_position[0], live_position[1] - player_position[1])
                facing_by_delta = {(0, -1): "UP", (0, 1): "DOWN", (-1, 0): "LEFT", (1, 0): "RIGHT"}
                if not active or delta not in facing_by_delta:
                    failed_pairs.add((object_position, tuple(candidate_position)))
                    continue
                direction = facing_by_delta[delta]
                if telemetry["gEcHeadlessCampaignPlayerFacing"] != directions[direction][2]:
                    before_face = player_position
                    advance(
                        f"object-{local_id}-face:{direction}",
                        int(action.get("face_frames", 24)),
                        direction,
                    )
                    after_face = (
                        telemetry["gEcHeadlessCampaignPlayerX"],
                        telemetry["gEcHeadlessCampaignPlayerY"],
                    )
                    if after_face != before_face:
                        fail(f"{segment['id']}: facing object {local_id} displaced the player")
                active, live_position, telemetry = query_campaign_object(
                    local_id=local_id, runner=runner, rom=rom, state=state,
                    addresses=addresses,
                )
                player_position = (
                    telemetry["gEcHeadlessCampaignPlayerX"],
                    telemetry["gEcHeadlessCampaignPlayerY"],
                )
                delta = (live_position[0] - player_position[0], live_position[1] - player_position[1])
                if not active or facing_by_delta.get(delta) != direction:
                    failed_pairs.add((object_position, tuple(candidate_position)))
                    continue
                advance(
                    f"object-{local_id}-interact",
                    int(action.get("press_frames", 12)), "A",
                )
                if is_stable_overworld(telemetry):
                    failed_pairs.add((object_position, tuple(candidate_position)))
                    continue
                if player_position != (
                    telemetry["gEcHeadlessCampaignPlayerX"],
                    telemetry["gEcHeadlessCampaignPlayerY"],
                ):
                    fail(f"{segment['id']}: interacting with object {local_id} displaced the player")
                name = str(action.get("screenshot", f"object-{local_id}-interaction"))
                shot = screenshot_dir / f"{name}.png"
                telemetry, _ = run_state_chunk(
                    runner=runner, rom=rom, state=state, addresses=addresses,
                    frames=1, screenshot=shot,
                )
                row = {
                    "action": f"screenshot:{name}", "path": str(shot),
                    "object_local_id": local_id, "object_position": list(live_position),
                    "player_position": list(player_position), "after": telemetry.copy(),
                }
                trace.append(row)
                with adaptive_trace_path.open("a") as handle:
                    handle.write(json.dumps(row, sort_keys=True) + "\n")
                break
            else:
                fail(f"{segment['id']}: could not interact with moving object {local_id}")
        elif action_type == "interact_tile":
            target = action.get("target")
            if (
                not isinstance(target, list) or len(target) != 2
                or not all(isinstance(value, int) for value in target)
            ):
                fail(f"{segment['id']}: interact_tile needs target [x, y]")
            if not is_stable_overworld(telemetry):
                fail(f"{segment['id']}: interact_tile began without stable field control")
            player_position = (
                telemetry["gEcHeadlessCampaignPlayerX"],
                telemetry["gEcHeadlessCampaignPlayerY"],
            )
            delta = (target[0] - player_position[0], target[1] - player_position[1])
            facing_by_delta = {
                (0, -1): "UP", (0, 1): "DOWN", (-1, 0): "LEFT", (1, 0): "RIGHT",
            }
            if delta not in facing_by_delta:
                fail(
                    f"{segment['id']}: interact_tile target {target} is not adjacent "
                    f"to player {player_position}"
                )
            direction = facing_by_delta[delta]
            if telemetry["gEcHeadlessCampaignPlayerFacing"] != directions[direction][2]:
                advance(
                    f"tile-{target[0]}-{target[1]}-face:{direction}",
                    int(action.get("face_frames", 24)),
                    direction,
                )
                after_face = (
                    telemetry["gEcHeadlessCampaignPlayerX"],
                    telemetry["gEcHeadlessCampaignPlayerY"],
                )
                if after_face != player_position:
                    fail(
                        f"{segment['id']}: facing open interaction tile {target} "
                        f"displaced player from {player_position} to {after_face}; "
                        "approach the tile from the intended direction"
                    )
                if telemetry["gEcHeadlessCampaignPlayerFacing"] != directions[direction][2]:
                    fail(f"{segment['id']}: failed to face interaction tile {target}")
            advance(
                f"tile-{target[0]}-{target[1]}-interact",
                int(action.get("press_frames", 12)), "A",
            )
            if is_stable_overworld(telemetry):
                fail(f"{segment['id']}: interaction tile {target} did not activate a script")
            if player_position != (
                telemetry["gEcHeadlessCampaignPlayerX"],
                telemetry["gEcHeadlessCampaignPlayerY"],
            ):
                fail(f"{segment['id']}: interacting with tile {target} displaced the player")
            if "screenshot" in action:
                name = str(action["screenshot"])
                shot = screenshot_dir / f"{name}.png"
                telemetry, _ = run_state_chunk(
                    runner=runner, rom=rom, state=state, addresses=addresses,
                    frames=1, screenshot=shot,
                )
                row = {
                    "action": f"screenshot:{name}", "path": str(shot),
                    "target": target, "player_position": list(player_position),
                    "after": telemetry.copy(),
                }
                trace.append(row)
                with adaptive_trace_path.open("a") as handle:
                    handle.write(json.dumps(row, sort_keys=True) + "\n")
        elif action_type == "screenshot":
            name = str(action.get("name", f"semantic-{index:03d}"))
            shot = screenshot_dir / f"{name}.png"
            telemetry, _ = run_state_chunk(
                runner=runner, rom=rom, state=state, addresses=addresses,
                frames=1, screenshot=shot,
            )
            row = {"action": f"screenshot:{name}", "path": str(shot), "after": telemetry.copy()}
            trace.append(row)
            with adaptive_trace_path.open("a") as handle:
                handle.write(json.dumps(row, sort_keys=True) + "\n")
        else:
            fail(f"{segment['id']}: unknown semantic action type {action_type}")
    return telemetry, trace


def run_segment(
    segment: dict[str, object],
    *,
    runner: Path,
    rom: Path,
    rom_hash: str,
    addresses: dict[str, int],
    scenario_id: int,
    map_ids: dict[str, int],
    constants: dict[str, int],
    artifact_evidence: dict[str, dict[str, object]],
    out: Path,
    state_in: Path | None,
) -> tuple[Path, dict[str, object]]:
    segment_id = str(segment["id"])
    checkpoint_dir = out / "checkpoints"
    screenshot_dir = out / "screenshots" / segment_id
    save_dir = out / "saves"
    checkpoint_dir.mkdir(parents=True, exist_ok=True)
    screenshot_dir.mkdir(parents=True, exist_ok=True)
    save_dir.mkdir(parents=True, exist_ok=True)
    state_out = checkpoint_dir / f"{segment_id}.ss1"
    save_out = save_dir / f"{segment_id}.sav"
    final_screenshot = screenshot_dir / "final.png"

    command = [
        str(runner),
        "--rom",
        str(rom),
        "--frames",
        str(segment["frames"]),
        "--rtc",
        str(segment.get("rtc", 946684800)),
        "--state-out",
        str(state_out),
        "--save-out",
        str(save_out),
        "--screenshot",
        str(final_screenshot),
    ]
    if state_in is None:
        command.extend(
            (
                "--write",
                f"60:4:0x{addresses['gEcHeadlessFixtureScenario']:x}:{scenario_id}",
            )
        )
    else:
        validate_state_input(
            state_in, rom_hash, expected_segment=str(segment["parent"]),
            artifact_evidence=artifact_evidence,
        )
        command.extend(("--state-in", str(state_in)))

    for action in segment.get("actions", []):
        command.extend(
            (
                "--key",
                f"{action['at']}:{action.get('duration', 2)}:{','.join(action['keys'])}",
            )
        )
    for shot in segment.get("screenshots", []):
        command.extend(
            (
                "--screenshot-at",
                f"{shot['at']}:{screenshot_dir / (shot['name'] + '.png')}",
            )
        )
    command.extend(
        (
            "--screenshot-on-change",
            f"4:0x{addresses['gEcHeadlessCampaignMapId']:x}:400:{screenshot_dir / 'map'}",
            "--screenshot-on-change",
            f"4:0x{addresses['gEcHeadlessCampaignBattleSerial']:x}:2:{screenshot_dir / 'battle'}",
        )
    )
    for name in TELEMETRY_SYMBOLS:
        command.extend(("--read", f"4:0x{addresses[name]:x}"))

    completed = subprocess.run(command, text=True, capture_output=True, check=False)
    if completed.returncode != 0:
        fail(
            f"{segment_id}: runner failed with {completed.returncode}\n"
            f"stdout:\n{completed.stdout}\nstderr:\n{completed.stderr}"
        )
    telemetry = telemetry_from_stdout(
        completed.stdout,
        {name: addresses[name] for name in TELEMETRY_SYMBOLS},
    )
    semantic_trace: list[dict[str, object]] = []
    if segment.get("semantic_actions"):
        telemetry, semantic_trace = apply_semantic_actions(
            segment,
            runner=runner,
            rom=rom,
            state=state_out,
            addresses=addresses,
            initial=telemetry,
            screenshot_dir=screenshot_dir,
        )

    expected = segment.get("expected", {})
    assertion_results: dict[str, dict[str, dict[str, object]]] = {"flags": {}, "vars": {}}
    for kind_name, query_kind in (("flags", 1), ("vars", 2)):
        requested = expected.get(kind_name, {}) if isinstance(expected, dict) else {}
        if not isinstance(requested, dict):
            fail(f"{segment_id}: expected.{kind_name} must be an object")
        for constant_name, wanted in requested.items():
            if constant_name not in constants:
                fail(f"{segment_id}: unknown {kind_name[:-1]} constant {constant_name}")
            actual, telemetry = query_campaign_value(
                kind=query_kind,
                identifier=constants[constant_name],
                runner=runner,
                rom=rom,
                state=state_out,
                addresses=addresses,
            )
            wanted_value = int(wanted) if kind_name == "flags" else wanted
            assertion_results[kind_name][constant_name] = {
                "id": constants[constant_name],
                "expected": wanted_value,
                "actual": actual,
                "passed": actual == wanted_value,
            }
            if actual != wanted_value:
                fail(
                    f"{segment_id}: expected {constant_name}={wanted_value}, observed {actual}"
                )
    validate_expected(segment, telemetry, map_ids)

    if segment.get("semantic_actions"):
        telemetry, _ = run_state_chunk(
            runner=runner,
            rom=rom,
            state=state_out,
            addresses=addresses,
            frames=1,
            screenshot=final_screenshot,
            save_out=save_out,
        )

    screenshots = [final_screenshot]
    screenshots.extend(screenshot_dir / f"{shot['name']}.png" for shot in segment.get("screenshots", []))
    watched = []
    for match in WATCH_PATTERN.finditer(completed.stdout):
        path = Path(match.group("path"))
        screenshots.append(path)
        watched.append(
            {
                "frame": int(match.group("frame")),
                "address": f"0x{int(match.group('address'), 16):08x}",
                "value": int(match.group("value"), 16),
                "screenshot": str(path),
            }
        )
    screenshots.extend(
        Path(row["path"])
        for row in semantic_trace
        if row["action"].startswith("screenshot:")
    )
    image_rows = []
    for screenshot in dict.fromkeys(screenshots):
        if not screenshot.is_file():
            fail(f"{segment_id}: screenshot is missing: {screenshot}")
        image_rows.append(
            {
                "path": str(screenshot),
                "png_sha256": sha256(screenshot),
                "pixel_sha256": ui.validate_screenshot_png(screenshot),
            }
        )

    metadata = {
        "schema_version": 1,
        "segment": segment_id,
        "parent": segment.get("parent"),
        "rom": str(rom),
        "rom_sha256": rom_hash,
        "artifact_evidence": artifact_evidence,
        "state": str(state_out),
        "state_sha256": sha256(state_out),
        "save": str(save_out),
        "save_sha256": sha256(save_out),
        "telemetry": telemetry,
        "watched_transitions": watched,
        "semantic_trace": semantic_trace,
        "assertions": assertion_results,
        "screenshots": image_rows,
        "coverage": segment.get("coverage", {}),
    }
    state_metadata_path(state_out).write_text(json.dumps(metadata, indent=2, sort_keys=True) + "\n")
    return state_out, metadata


def write_failure_bundle(
    *, out: Path, segment: dict[str, object], error: BaseException, rom_hash: str
) -> Path:
    segment_id = str(segment.get("id", "unknown"))
    failure_dir = out / "failures" / segment_id
    failure_dir.mkdir(parents=True, exist_ok=True)
    checkpoint = out / "checkpoints" / f"{segment_id}.ss1"
    final_screenshot = out / "screenshots" / segment_id / "final.png"
    save = out / "saves" / f"{segment_id}.sav"
    adaptive_trace = out / "screenshots" / segment_id / "adaptive-trace.jsonl"
    copied: dict[str, str] = {}
    for label, source, destination in (
        ("state", checkpoint, failure_dir / "last-state.ss1"),
        ("save", save, failure_dir / "last-save.sav"),
        ("screenshot", final_screenshot, failure_dir / "last-frame.png"),
        ("adaptive_trace", adaptive_trace, failure_dir / "adaptive-trace.jsonl"),
    ):
        if source.is_file():
            shutil.copy2(source, destination)
            copied[label] = str(destination)
    payload = {
        "schema_version": 1,
        "segment": segment_id,
        "rom_sha256": rom_hash,
        "error_type": type(error).__name__,
        "error": str(error),
        "segment_spec": segment,
        "artifacts": copied,
    }
    (failure_dir / "failure.json").write_text(
        json.dumps(payload, indent=2, sort_keys=True) + "\n"
    )
    return failure_dir


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--manifest", type=Path, default=DEFAULT_MANIFEST)
    parser.add_argument("--rom", type=Path, default=DEFAULT_ROM)
    parser.add_argument("--elf", type=Path, default=DEFAULT_ELF)
    parser.add_argument("--out", type=Path, default=DEFAULT_OUT)
    parser.add_argument("--start", help="start at this segment using its parent checkpoint")
    parser.add_argument("--through", help="stop after this segment")
    parser.add_argument("--suite", help="run every segment in a named suite plus its ancestry")
    parser.add_argument("--run-name", help="safe output name for a targeted run")
    parser.add_argument(
        "--parent-run",
        help="read missing parent checkpoints from this named targeted run",
    )
    parser.add_argument("--list", action="store_true")
    parser.add_argument("--validate-only", action="store_true")
    args = parser.parse_args()

    source_manifest = ui.require_resident_file(args.manifest, "campaign playthrough manifest")
    manifest_snapshot: Path | None = None
    manifest_artifact: dict[str, object] | None = None
    if args.list or args.validate_only:
        manifest = load_manifest(source_manifest)
    else:
        args.out.mkdir(parents=True, exist_ok=True)
        manifest_snapshot, manifest_artifact = snapshot_artifact(
            source_manifest, args.out / "artifacts" / "manifests", "manifest"
        )
        manifest = load_manifest(manifest_snapshot)
    capture_audit = capture_paths.audit()
    if capture_audit["failures"]:
        fail("campaign capture-path audit failed: " + "; ".join(capture_audit["failures"]))
    prerequisite_audit = prerequisites.audit()
    if prerequisite_audit["failures"]:
        fail("campaign prerequisite audit failed: " + "; ".join(prerequisite_audit["failures"]))
    segments = manifest["segments"]
    ids = [segment["id"] for segment in segments]
    if args.list:
        print("\n".join(ids))
        return 0
    if args.start is not None and args.start not in ids:
        fail(f"unknown start segment: {args.start}")
    if args.through is not None and args.through not in ids:
        fail(f"unknown through segment: {args.through}")
    if args.suite is not None and (args.start is not None or args.through is not None):
        fail("--suite cannot be combined with --start or --through")
    if args.run_name is not None and args.start is None and args.through is None and args.suite is None:
        fail("--run-name is only valid for a targeted run")
    if args.parent_run is not None and args.start is None and args.through is None and args.suite is None:
        fail("--parent-run is only valid for a targeted run")
    map_ids = parse_map_ids()
    constants = parse_numeric_constants()
    validate_manifest_symbols(segments, map_ids, constants)
    if args.validate_only:
        summary = capture_audit["summary"]
        print(
            f"PASS: {len(segments)} campaign segments have valid structure and symbols; "
            f"capture paths={summary['legendary_auto_capture']} legendary/"
            f"{summary['ordinary_static_won_safe']} ordinary WON-safe; "
            f"prerequisites={prerequisite_audit['summary']['total']}"
        )
        return 0

    selected_segments = select_segments(segments, args.start, args.through, args.suite)
    full_run = args.start is None and args.through is None and args.suite is None
    if full_run:
        run_id = "full"
        run_out = args.out
    else:
        automatic_name = (
            f"suite-{args.suite}" if args.suite is not None
            else f"from-{args.start}-through-{args.through}" if args.start and args.through
            else f"from-{args.start}" if args.start
            else f"through-{args.through}"
        )
        run_id = safe_run_slug(args.run_name or automatic_name)
        run_out = args.out / "runs" / run_id
    parent_run_id = safe_run_slug(args.parent_run) if args.parent_run is not None else None
    if parent_run_id == run_id:
        fail("--parent-run must differ from the destination run ID")

    source_rom = ui.require_resident_file(args.rom, "campaign playthrough ROM")
    source_elf = ui.require_resident_file(args.elf, "campaign playthrough ELF")
    run_out.mkdir(parents=True, exist_ok=True)
    artifact_dir = run_out / "artifacts"
    rom, rom_artifact = snapshot_artifact(source_rom, artifact_dir, "rom")
    elf, elf_artifact = snapshot_artifact(source_elf, artifact_dir, "elf")
    try:
        verify_rom_elf_pair(rom, elf)
    except (ValueError, subprocess.TimeoutExpired) as error:
        fail(f"campaign ROM/ELF correspondence failed: {error}")
    if manifest_snapshot is None or manifest_artifact is None:
        fail("campaign manifest snapshot was not initialized")
    manifest_hash = str(manifest_artifact["snapshot_sha256"])
    rom_hash = str(rom_artifact["snapshot_sha256"])
    elf_hash = str(elf_artifact["snapshot_sha256"])
    runner = ui.build_runner()
    addresses = {name: ui.resolve_symbol(elf, name) for name in TELEMETRY_SYMBOLS}
    addresses["gEcHeadlessFixtureScenario"] = ui.resolve_symbol(
        elf, "gEcHeadlessFixtureScenario"
    )
    scenario_id = parse_scenario_id()
    trace_path = run_out / "trace.jsonl"
    trace_path.unlink(missing_ok=True)
    rows = []
    for segment in selected_segments:
        parent = segment.get("parent")
        if parent is None:
            state_in = None
        else:
            state_in = select_parent_checkpoint(
                str(parent), completed_segments={str(row["segment"]) for row in rows},
                run_out=run_out, out=args.out, parent_run_id=parent_run_id,
            )
        try:
            _state, row = run_segment(
                segment,
                runner=runner,
                rom=rom,
                rom_hash=rom_hash,
                addresses=addresses,
                scenario_id=scenario_id,
                map_ids=map_ids,
                constants=constants,
                artifact_evidence={
                    "rom": rom_artifact, "elf": elf_artifact,
                    "manifest": manifest_artifact,
                },
                out=run_out,
                state_in=state_in,
            )
        except (OSError, RuntimeError, ValueError) as error:
            failed_state = run_out / "checkpoints" / f"{segment['id']}.ss1"
            if failed_state.is_file():
                try:
                    run_state_chunk(
                        runner=runner,
                        rom=rom,
                        state=failed_state,
                        addresses=addresses,
                        frames=1,
                        screenshot=run_out / "screenshots" / str(segment["id"]) / "final.png",
                        save_out=run_out / "saves" / f"{segment['id']}.sav",
                    )
                except (OSError, RuntimeError, ValueError):
                    pass
            failure_dir = write_failure_bundle(
                out=run_out, segment=segment, error=error, rom_hash=rom_hash
            )
            failure_summary = {
                "schema_version": 1,
                "run_id": run_id,
                "run_kind": "full" if full_run else "targeted",
                "selection": {
                    "start": args.start,
                    "through": args.through,
                    "suite": args.suite,
                    "parent_run": parent_run_id,
                    "segment_ids": [item["id"] for item in selected_segments],
                    "complete_manifest": full_run,
                },
                "rom_sha256": rom_hash,
                "elf_sha256": elf_hash,
                "manifest_sha256": manifest_hash,
                "artifact_snapshots_verified_immutable": False,
                "completed_segment_count": len(rows),
                "failed_segment": segment["id"],
                "failure_bundle": str(failure_dir),
                "status": "fail",
            }
            write_json_atomic(run_out / "run-summary.json", failure_summary)
            if not full_run:
                update_run_index(args.out / "runs" / "index.json", run_id, failure_summary)
            fail(f"{segment['id']}: failure bundle written to {failure_dir}: {error}")
        rows.append(row)
        with trace_path.open("a") as trace:
            trace.write(json.dumps(row, sort_keys=True) + "\n")
        telemetry = row["telemetry"]
        print(
            f"PASS {segment['id']}: map=0x{telemetry['gEcHeadlessCampaignMapId']:04x} "
            f"pos=({telemetry['gEcHeadlessCampaignPlayerX']},"
            f"{telemetry['gEcHeadlessCampaignPlayerY']}) "
            f"battles={telemetry['gEcHeadlessCampaignBattleSerial']}"
        )
    try:
        rom_artifact = verify_artifact_snapshot(rom_artifact)
        elf_artifact = verify_artifact_snapshot(elf_artifact)
        manifest_artifact = verify_artifact_snapshot(manifest_artifact)
    except (OSError, RuntimeError) as error:
        race_summary = {
            "schema_version": 1,
            "run_id": run_id,
            "run_kind": "full" if full_run else "targeted",
            "selection": {
                "start": args.start, "through": args.through, "suite": args.suite,
                "parent_run": parent_run_id,
                "segment_ids": [item["id"] for item in selected_segments],
                "complete_manifest": full_run,
            },
            "rom_sha256": rom_hash,
            "elf_sha256": elf_hash,
            "manifest_sha256": manifest_hash,
            "artifact_snapshots_verified_immutable": False,
            "completed_segment_count": len(rows),
            "error": str(error),
            "status": "artifact-race-fail",
        }
        write_json_atomic(run_out / "run-summary.json", race_summary)
        if not full_run:
            update_run_index(args.out / "runs" / "index.json", run_id, race_summary)
        raise
    final_artifact_evidence = {
        "rom": rom_artifact, "elf": elf_artifact, "manifest": manifest_artifact,
    }
    for row in rows:
        row["artifact_evidence"] = final_artifact_evidence
        write_json_atomic(state_metadata_path(Path(str(row["state"]))), row)
    latest_run = run_out / "latest-run.json"
    run_payload = {
                "schema_version": 1,
                "manifest": str(manifest_snapshot),
                "source_manifest": str(source_manifest),
                "manifest_sha256": manifest_hash,
                "run_id": run_id,
                "run_kind": "full" if full_run else "targeted",
                "selection": {
                    "start": args.start,
                    "through": args.through,
                    "suite": args.suite,
                    "parent_run": parent_run_id,
                    "segment_ids": [segment["id"] for segment in selected_segments],
                    "complete_manifest": full_run,
                },
                "source_rom": str(source_rom),
                "source_elf": str(source_elf),
                "rom": str(rom),
                "rom_sha256": rom_hash,
                "elf": str(elf),
                "elf_sha256": elf_hash,
                "artifact_evidence": final_artifact_evidence,
                "segments": rows,
            }
    write_json_atomic(latest_run, run_payload)
    report_script = ROOT / "scripts" / "render_emerald_champions_campaign_report.py"
    if report_script.is_file():
        report = subprocess.run(
            [
                sys.executable,
                str(report_script),
                "--manifest",
                str(manifest_snapshot),
                "--run",
                str(latest_run),
                "--out",
                str(run_out / "report.html"),
            ],
            cwd=ROOT,
            text=True,
            capture_output=True,
            check=False,
        )
        if report.returncode != 0:
            fail("campaign report renderer failed:\n" + report.stdout + report.stderr)
        print(report.stdout.strip())
    summary = {
        "schema_version": 1,
        "run_id": run_id,
        "run_kind": run_payload["run_kind"],
        "selection": run_payload["selection"],
        "manifest": str(manifest_snapshot),
        "source_manifest": str(source_manifest),
        "manifest_sha256": manifest_hash,
        "rom_sha256": rom_hash,
        "elf_sha256": elf_hash,
        "artifact_snapshots_verified_immutable": True,
        "segment_count": len(rows),
        "latest_run": str(latest_run),
        "report": str(run_out / "report.html"),
        "status": "pass",
    }
    write_json_atomic(run_out / "run-summary.json", summary)
    if not full_run:
        update_run_index(args.out / "runs" / "index.json", run_id, summary)
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError) as error:
        print(f"campaign playthrough: FAIL: {error}", file=sys.stderr)
        raise SystemExit(1)
