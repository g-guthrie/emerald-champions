#!/usr/bin/env python3
"""Small, deterministic GPT-player benchmark loop for Emerald Champions.

The emulator process is deliberately short-lived.  Every command consumes one
exact mGBA state and atomically produces the next, so a crash cannot corrupt
the last accepted checkpoint.  This is slower than a socket daemon but much
easier to reproduce and audit.
"""

from __future__ import annotations

import argparse
import contextlib
import fcntl
import hashlib
import json
import os
import re
import shlex
import shutil
import subprocess
import sys
import tempfile
import time
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
from rom_artifacts import verify_rom_elf_pair

RUNNER_SOURCE = ROOT / "tests/headless/emerald_champions_mgba_runner.c"
DEFAULT_RUNNER = ROOT / "build/headless/emerald_champions_mgba_runner"
BUTTONS = {"A", "B", "START", "SELECT", "UP", "DOWN", "LEFT", "RIGHT", "L", "R", "WAIT"}
PREP_MUTATIONS = {"roster", "catch", "level", "moves", "preset", "nature", "ability", "item", "stat_points"}
SEMANTIC_EVENTS = {
    "battle_attempt": "battle_attempts",
    "battle_turn": "battle_turns",
    "battle_success": "battle_successes",
    "whiteout": "whiteouts",
    "catch": "catches",
    "team_change": "team_changes",
    "move_tutor": "tutor_uses",
    "preset_change": "preset_uses",
    "vendor_use": "vendor_uses",
    "ability_change": "ability_changes",
    "stat_points_change": "stat_point_changes",
    "item_change": "item_changes",
    "level_to_cap": "leveler_uses",
    "center_visit": "center_visits",
    "adaptation": "adaptations",
}
RESULT_RE = re.compile(r"^RESULT frames=(?P<frames>\d+).*$", re.MULTILINE)
READ_RE = re.compile(
    r"^READ width=(?P<width>\d+) address=(?P<address>[0-9a-fA-F]+) value=(?P<value>[0-9a-fA-F]+)$",
    re.MULTILINE,
)


class HarnessError(RuntimeError):
    pass


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def atomic_json(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_name(path.name + ".tmp")
    temporary.write_text(json.dumps(value, indent=2, sort_keys=True) + "\n")
    os.replace(temporary, path)


def append_jsonl(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a", encoding="utf-8") as handle:
        handle.write(json.dumps(value, sort_keys=True, separators=(",", ":")) + "\n")
        handle.flush()
        os.fsync(handle.fileno())


def load_json(path: Path) -> dict[str, Any]:
    try:
        value = json.loads(path.read_text())
    except (OSError, json.JSONDecodeError) as error:
        raise HarnessError(f"cannot read JSON {path}: {error}") from error
    if not isinstance(value, dict):
        raise HarnessError(f"expected a JSON object: {path}")
    return value


def config_sha256(config: dict[str, Any]) -> str:
    return hashlib.sha256(json.dumps(config, sort_keys=True, separators=(",", ":")).encode()).hexdigest()


def repo_path(value: str, config_path: Path) -> Path:
    path = Path(value)
    if path.is_absolute():
        return path
    # Run configs are portable repository artifacts, so relative paths are
    # repository-relative rather than dependent on the caller's cwd.
    return (ROOT / path).resolve()


def require_file(path: Path, label: str) -> Path:
    if not path.is_file():
        raise HarnessError(f"{label} is missing: {path}")
    stat = path.stat()
    if stat.st_size == 0 or (stat.st_size and stat.st_blocks == 0):
        raise HarnessError(f"{label} is empty or offloaded: {path}")
    return path


def build_runner() -> Path:
    output = DEFAULT_RUNNER
    if output.is_file() and output.stat().st_mtime_ns >= RUNNER_SOURCE.stat().st_mtime_ns:
        return output
    output.parent.mkdir(parents=True, exist_ok=True)
    command = [os.environ.get("CC", "cc"), "-std=c11", "-O2", "-Wall", "-Wextra", "-Werror", str(RUNNER_SOURCE)]
    pkg_config = shutil.which("pkg-config")
    flags: list[str] = []
    if pkg_config:
        result = subprocess.run([pkg_config, "--cflags", "--libs", "mgba"], text=True, capture_output=True)
        if result.returncode == 0:
            flags = shlex.split(result.stdout)
    if not flags:
        prefixes = [Path(os.environ["MGBA_PREFIX"])] if os.environ.get("MGBA_PREFIX") else []
        prefixes += [Path("/opt/homebrew/opt/mgba"), Path("/usr/local/opt/mgba"), Path("/usr")]
        for prefix in prefixes:
            if (prefix / "include/mgba/core/core.h").is_file():
                flags = [f"-I{prefix / 'include'}", f"-L{prefix / 'lib'}", "-lmgba", f"-Wl,-rpath,{prefix / 'lib'}"]
                break
    if not flags:
        raise HarnessError("native libmGBA development files unavailable; set MGBA_PREFIX")
    command += flags + ["-o", str(output)]
    result = subprocess.run(command, text=True, capture_output=True)
    if result.returncode != 0:
        raise HarnessError(f"failed to build native runner:\n{result.stdout}{result.stderr}")
    return output


def find_nm() -> str:
    candidates = [
        shutil.which("arm-none-eabi-nm"),
        str(ROOT / "tools/binutils/bin/arm-none-eabi-nm"),
        "/opt/homebrew/bin/arm-none-eabi-nm",
    ]
    for candidate in candidates:
        if candidate and Path(candidate).is_file():
            return candidate
    raise HarnessError("arm-none-eabi-nm is required for symbol probes")


def symbols(elf: Path) -> dict[str, int]:
    result = subprocess.run([find_nm(), "-S", str(elf)], text=True, capture_output=True)
    if result.returncode != 0:
        raise HarnessError(f"cannot inspect ELF symbols: {result.stderr.strip()}")
    found: dict[str, int] = {}
    for line in result.stdout.splitlines():
        fields = line.split()
        if len(fields) >= 4:
            try:
                found[fields[-1]] = int(fields[0], 16)
            except ValueError:
                pass
    return found


def resolve_game_constants(names: list[str]) -> dict[str, int]:
    if not names:
        return {}
    if any(not re.fullmatch(r"(?:SPECIES|MOVE|ABILITY|ITEM|NATURE)_[A-Z0-9_]+", name) for name in names):
        raise HarnessError("preparation request contains a malformed game constant")
    compiler = shutil.which("cc") or shutil.which("clang")
    if not compiler:
        raise HarnessError("host C compiler is required to resolve game constants")
    unique = list(dict.fromkeys(names))
    prints = "\n".join(f'printf("{name}=%u\\n", (unsigned){name});' for name in unique)
    source = f'''#include <stdio.h>
#include "constants/species.h"
#include "constants/moves.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/pokemon.h"
int main(void) {{ {prints} return 0; }}
'''
    with tempfile.TemporaryDirectory(prefix="ec-agent-constants-") as raw:
        executable = Path(raw) / "constants"
        built = subprocess.run([compiler, "-Iinclude", "-I.", "-x", "c", "-", "-o", str(executable)], cwd=ROOT, input=source, text=True, capture_output=True)
        if built.returncode:
            raise HarnessError(f"cannot resolve requested constants:\n{built.stderr}")
        output = subprocess.run([str(executable)], text=True, capture_output=True, check=True).stdout
    return {name: int(value) for name, value in re.findall(r"^([A-Z0-9_]+)=(\d+)$", output, re.M)}


def normalized_config(path: Path) -> dict[str, Any]:
    config = load_json(path)
    if config.get("schema_version") != 1:
        raise HarnessError("run config schema_version must be 1")
    mode = config.get("observation_mode")
    if mode not in {"vision_only", "instrumented"}:
        raise HarnessError("observation_mode must be vision_only or instrumented")
    config.setdefault("model", {"provider": "manual", "name": "manual-policy"})
    config.setdefault("benchmark_mode", "campaign_play")
    if config["benchmark_mode"] not in {"campaign_play", "battle_lab"}:
        raise HarnessError("benchmark_mode must be campaign_play or battle_lab")
    config.setdefault("player_context", "tools/agent_player/player_context.md")
    config.setdefault("rtc_epoch", 946684800)
    config.setdefault("seed", 0)
    config.setdefault("rng_delay_frames", 0)
    config.setdefault("boot_frames", 120)
    config.setdefault("step", {"press_frames": 1, "release_frames": 15})
    config.setdefault("budgets", {})
    config["budgets"].setdefault("max_actions", 1000)
    config["budgets"].setdefault("max_frames", 100000)
    config["budgets"].setdefault("timeout_seconds", 3600)
    config["budgets"].setdefault("step_timeout_seconds", 30)
    config["budgets"].setdefault("max_deaths", 100)
    config.setdefault("probes", [])
    config.setdefault("init_writes", [])
    if not isinstance(config["probes"], list):
        raise HarnessError("probes must be a list")
    if not isinstance(config["init_writes"], list):
        raise HarnessError("init_writes must be a list")
    return config


class Session:
    def __init__(self, config_path: Path, run_dir_override: Path | None = None):
        self.config_path = config_path.resolve()
        self.config = normalized_config(self.config_path)
        self.rom = require_file(repo_path(str(self.config["rom"]), self.config_path), "ROM")
        self.elf = require_file(repo_path(str(self.config["elf"]), self.config_path), "ELF") if self.config.get("elf") else None
        runner_value = self.config.get("runner")
        self.runner = require_file(repo_path(str(runner_value), self.config_path), "runner") if runner_value else build_runner()
        configured_run_dir = repo_path(str(self.config.get("run_dir", "work/agent-player/default")), self.config_path)
        self.run_dir = (run_dir_override or configured_run_dir).resolve()
        self.player_context = require_file(repo_path(str(self.config["player_context"]), self.config_path), "player context")
        self.state_path = self.run_dir / "current.ss1"
        self.meta_path = self.run_dir / "session.json"
        self.log_path = self.run_dir / "events.jsonl"
        self.probes = self._resolve_probes()
        self.init_writes = self._resolve_records(self.config["init_writes"], "init write")
        self.arsenal: dict[str, Any] | None = None
        if self.config["benchmark_mode"] == "battle_lab":
            if not self.config.get("start_checkpoint"):
                raise HarnessError("battle_lab requires a deterministic start_checkpoint")
            if not self.init_writes:
                raise HarnessError("battle_lab requires init_writes that disable fixture battle automation")
            if not any(write.get("symbol") == "gEcHeadlessFixtureActiveScenario" and int(write.get("value", -1)) == 0 for write in self.init_writes):
                raise HarnessError("battle_lab must set gEcHeadlessFixtureActiveScenario to 0 before play")
            if not self.config.get("arsenal_manifest"):
                raise HarnessError("battle_lab requires a checkpoint-bound arsenal_manifest")
            self.arsenal_path = require_file(repo_path(str(self.config["arsenal_manifest"]), self.config_path), "arsenal manifest")
            self.arsenal = load_json(self.arsenal_path)
            self._validate_arsenal(self.arsenal)

    def _validate_arsenal(self, arsenal: dict[str, Any]) -> None:
        if arsenal.get("schema_version") != 1:
            raise HarnessError("arsenal manifest schema_version must be 1")
        if arsenal.get("battle_id") != self.config.get("battle_id"):
            raise HarnessError("arsenal battle_id differs from battle-lab config")
        if arsenal.get("rom_sha256") != sha256(self.rom):
            raise HarnessError("arsenal manifest belongs to a different ROM")
        checkpoint = self.config["start_checkpoint"]
        state = require_file(repo_path(str(checkpoint["state"]), self.config_path), "start checkpoint")
        if arsenal.get("checkpoint_state_sha256") != sha256(state):
            raise HarnessError("arsenal manifest belongs to a different checkpoint")
        if not isinstance(arsenal.get("level_cap"), int) or arsenal["level_cap"] <= 0:
            raise HarnessError("arsenal manifest needs a positive level_cap")
        if not isinstance(arsenal.get("pokemon"), list) or not isinstance(arsenal.get("items"), list):
            raise HarnessError("arsenal manifest needs pokemon and items lists")
        if not isinstance(arsenal.get("generation_evidence"), list) or not arsenal["generation_evidence"]:
            raise HarnessError("arsenal manifest needs source generation_evidence")
        for category in ("pokemon", "items"):
            for entry in arsenal[category]:
                if not isinstance(entry, dict) or not entry.get("source_justification"):
                    raise HarnessError(f"every arsenal {category} entry needs source_justification")
        mega = arsenal.get("mega_access")
        if not isinstance(mega, dict) or not isinstance(mega.get("bracelet"), bool) or not isinstance(mega.get("legal_stones"), list):
            raise HarnessError("arsenal manifest needs explicit Mega bracelet and legal_stones state")
        if not isinstance(arsenal.get("opponent_dossier"), dict):
            raise HarnessError("battle puzzle needs a generated full opponent_dossier")

    @contextlib.contextmanager
    def lock(self):
        self.run_dir.mkdir(parents=True, exist_ok=True)
        descriptor = os.open(self.run_dir / ".lock", os.O_CREAT | os.O_RDWR, 0o600)
        try:
            try:
                fcntl.flock(descriptor, fcntl.LOCK_EX | fcntl.LOCK_NB)
            except BlockingIOError as error:
                raise HarnessError(f"session is already being modified: {self.run_dir}") from error
            yield
        finally:
            os.close(descriptor)

    def _resolve_probes(self) -> list[dict[str, Any]]:
        return self._resolve_records(self.config["probes"], "probe")

    def _resolve_records(self, records: list[dict[str, Any]], label: str) -> list[dict[str, Any]]:
        table = symbols(self.elf) if self.elf and records else {}
        result = []
        for raw in records:
            probe = dict(raw)
            if "address" not in probe:
                name = str(probe.get("symbol", ""))
                if name not in table:
                    if probe.get("optional", False):
                        continue
                    raise HarnessError(f"required {label} symbol missing from ELF: {name}")
                probe["address"] = table[name]
            probe.setdefault("width", 4)
            if probe["width"] not in (1, 2, 4):
                raise HarnessError(f"invalid {label} width: {probe}")
            probe.setdefault("name", probe.get("symbol", f"0x{int(probe['address']):x}"))
            probe.setdefault("role", "value")
            result.append(probe)
        return result

    def _base_meta(self) -> dict[str, Any]:
        result = {
            "schema_version": 1,
            "config": str(self.config_path),
            "config_sha256": config_sha256(self.config),
            "rom": str(self.rom),
            "rom_sha256": sha256(self.rom),
            "elf": str(self.elf) if self.elf else None,
            "elf_sha256": sha256(self.elf) if self.elf else None,
            "runner": str(self.runner),
            "runner_sha256": sha256(self.runner),
            "observation_mode": self.config["observation_mode"],
            "benchmark_mode": self.config["benchmark_mode"],
            "battle_id": self.config.get("battle_id"),
            "model": self.config["model"],
            "player_context": str(self.player_context),
            "player_context_sha256": sha256(self.player_context),
            "rtc_epoch": int(self.config["rtc_epoch"]),
            "seed": int(self.config["seed"]),
            "rng_delay_frames": int(self.config["rng_delay_frames"]),
            # Seed labels and delays vary across a cohort; these settings must
            # remain comparable. Existing artifact hashes bind the inputs.
            "comparison_protocol": {
                key: self.config[key]
                for key in ("schema_version", "benchmark_mode", "battle_id", "observation_mode", "model",
                            "rtc_epoch", "boot_frames", "step", "budgets", "init_writes", "probes")
                if key in self.config
            },
            "started_at_epoch": time.time(),
            "action_count": 0,
            "frame_count": 0,
            "metrics": {
                "attempts": 1,
                "deaths": 0,
                "battles": 0,
                "progress_events": 0,
                **{metric: 0 for metric in SEMANTIC_EVENTS.values()},
                "retries": 0,
            },
            "battle_attempts_by_id": {},
            "last_probe_values": {},
            "status": "active",
        }
        if self.arsenal is not None:
            result["arsenal_manifest"] = str(self.arsenal_path)
            result["arsenal_manifest_sha256"] = sha256(self.arsenal_path)
            result["level_cap"] = self.arsenal["level_cap"]
        return result

    def load_meta(self) -> dict[str, Any]:
        meta = load_json(self.meta_path)
        identities = {
            "config": config_sha256(self.config),
            "ROM": sha256(self.rom),
            "ELF": sha256(self.elf) if self.elf else None,
            "runner": sha256(self.runner),
            "player context": sha256(self.player_context),
        }
        fields = {
            "config": "config_sha256",
            "ROM": "rom_sha256",
            "ELF": "elf_sha256",
            "runner": "runner_sha256",
            "player context": "player_context_sha256",
        }
        if self.arsenal is not None:
            identities["arsenal manifest"] = sha256(self.arsenal_path)
            fields["arsenal manifest"] = "arsenal_manifest_sha256"
        for label, current in identities.items():
            if meta.get(fields[label]) != current:
                raise HarnessError(f"session {label} hash differs from the current run identity; start a new run")
        return meta

    def _budget_check(self, meta: dict[str, Any], frames: int) -> None:
        budget = self.config["budgets"]
        if meta["action_count"] >= int(budget["max_actions"]):
            raise HarnessError("action budget exhausted")
        if meta["frame_count"] + frames > int(budget["max_frames"]):
            raise HarnessError("frame budget exhausted")
        if time.time() - float(meta["started_at_epoch"]) > float(budget["timeout_seconds"]):
            raise HarnessError("wall-clock budget exhausted")
        if meta["metrics"]["deaths"] >= int(budget["max_deaths"]):
            raise HarnessError("death budget exhausted")

    def _run_step(self, frames: int, screenshot: Path, state_in: Path | None, key: str | None, writes: list[dict[str, Any]] | None = None, extra_reads: list[dict[str, Any]] | None = None) -> tuple[int, dict[str, int]]:
        self.run_dir.mkdir(parents=True, exist_ok=True)
        temporary_state = self.run_dir / f".next-{os.getpid()}.ss1"
        temporary_shot = self.run_dir / f".next-{os.getpid()}.png"
        for temporary in (temporary_state, temporary_shot):
            temporary.unlink(missing_ok=True)
        command = [str(self.runner), "--rom", str(self.rom), "--frames", str(frames), "--rtc", str(int(self.config["rtc_epoch"])), "--state-out", str(temporary_state), "--screenshot", str(temporary_shot)]
        if state_in:
            command += ["--state-in", str(state_in)]
        if key and key != "WAIT":
            command += ["--key", f"0:{int(self.config['step']['press_frames'])}:{key}"]
        for write in writes or []:
            command += ["--write", f"0:{write['width']}:0x{int(write['address']):x}:{int(write['value'])}"]
        reads = self.probes + (extra_reads or [])
        for probe in reads:
            command += ["--read", f"{probe['width']}:0x{int(probe['address']):x}"]
        probe_addresses = {int(probe["address"]) for probe in reads}
        for write in writes or []:
            if int(write["address"]) not in probe_addresses:
                command += ["--read", f"{write['width']}:0x{int(write['address']):x}"]
        timeout = float(self.config["budgets"]["step_timeout_seconds"])
        try:
            completed = subprocess.run(command, text=True, capture_output=True, timeout=timeout)
        except subprocess.TimeoutExpired as error:
            raise HarnessError(f"emulator step exceeded {timeout:g}s") from error
        if completed.returncode != 0:
            raise HarnessError(f"emulator step failed ({completed.returncode}):\n{completed.stdout}{completed.stderr}")
        match = RESULT_RE.search(completed.stdout)
        if not match or not temporary_state.is_file() or not temporary_shot.is_file():
            raise HarnessError(f"emulator step returned incomplete artifacts:\n{completed.stdout}")
        png_header = temporary_shot.read_bytes()[:24]
        if png_header[:8] != b"\x89PNG\r\n\x1a\n" or png_header[12:16] != b"IHDR":
            raise HarnessError("emulator screenshot is not a PNG")
        width, height = int.from_bytes(png_header[16:20], "big"), int.from_bytes(png_header[20:24], "big")
        if (width, height) != (240, 160):
            raise HarnessError(f"emulator screenshot is {width}x{height}, expected 240x160")
        values_by_address = {int(row.group("address"), 16): int(row.group("value"), 16) for row in READ_RE.finditer(completed.stdout)}
        for write in writes or []:
            if not write.get("verify_after", True):
                continue
            actual = values_by_address.get(int(write["address"]))
            if actual != int(write["value"]):
                raise HarnessError(f"init write verification failed for {write['name']}: got {actual!r}")
        values = {str(probe["name"]): values_by_address[int(probe["address"])] for probe in reads}
        screenshot.parent.mkdir(parents=True, exist_ok=True)
        os.replace(temporary_shot, screenshot)
        os.replace(temporary_state, self.state_path)
        return int(match.group("frames")), values

    def _update_metrics(self, meta: dict[str, Any], values: dict[str, int]) -> None:
        previous = meta.get("last_probe_values", {})
        for probe in self.probes:
            name, role = str(probe["name"]), str(probe["role"])
            if name not in values:
                continue
            old, new = previous.get(name), values[name]
            if old is not None:
                if role == "battle_counter" and new > old:
                    meta["metrics"]["battles"] += new - old
                elif role == "battle_active" and old == 0 and new != 0:
                    meta["metrics"]["battles"] += 1
                elif role == "death_counter" and new > old:
                    meta["metrics"]["deaths"] += new - old
                elif role == "death_active" and old == 0 and new != 0:
                    meta["metrics"]["deaths"] += 1
                elif role == "progress_counter" and new > old:
                    meta["metrics"]["progress_events"] += new - old
                elif role == "progress_value" and new != old:
                    meta["metrics"]["progress_events"] += 1
        meta["last_probe_values"] = values

    def init(self, replace: bool = False) -> dict[str, Any]:
        if self.meta_path.exists() and not replace:
            raise HarnessError(f"session already exists: {self.run_dir} (use --replace or a new run dir)")
        if self.elf is not None:
            try:
                verify_rom_elf_pair(self.rom, self.elf)
            except (ValueError, OSError, subprocess.TimeoutExpired) as error:
                raise HarnessError(f"ROM/ELF pair verification failed: {error}") from error
        self.run_dir.mkdir(parents=True, exist_ok=True)
        if replace:
            # Only these harness-owned leaf files are replaced. Historical
            # checkpoints and observations remain available for diagnosis.
            self.meta_path.unlink(missing_ok=True)
            self.log_path.unlink(missing_ok=True)
            self.state_path.unlink(missing_ok=True)
        meta = self._base_meta()
        start_state: Path | None = None
        if self.config.get("start_checkpoint"):
            checkpoint = self.config["start_checkpoint"]
            if not isinstance(checkpoint, dict) or "state" not in checkpoint:
                raise HarnessError("start_checkpoint must contain a state path")
            start_state = require_file(repo_path(str(checkpoint["state"]), self.config_path), "start checkpoint")
            default_metadata = str(checkpoint["state"]) + ".json"
            metadata_path = repo_path(str(checkpoint.get("metadata", default_metadata)), self.config_path)
            metadata = load_json(metadata_path)
            if metadata.get("rom_sha256") != sha256(self.rom):
                raise HarnessError("start checkpoint belongs to a different ROM")
            if metadata.get("state_sha256") and metadata["state_sha256"] != sha256(start_state):
                raise HarnessError("start checkpoint state hash mismatch")
            meta["start_checkpoint"] = str(start_state)
            meta["start_checkpoint_sha256"] = sha256(start_state)
        shot = self.run_dir / "observations/000000.png"
        initial_frames = int(self.config["boot_frames"]) + int(self.config["rng_delay_frames"])
        frames, values = self._run_step(initial_frames, shot, start_state, None, self.init_writes)
        meta["frame_count"] = frames
        self._update_metrics(meta, values)
        atomic_json(self.meta_path, meta)
        event = self._event("init", None, shot, frames, values, meta)
        append_jsonl(self.log_path, event)
        return event

    def _event(self, kind: str, button: str | None, shot: Path, frames: int, values: dict[str, int], meta: dict[str, Any]) -> dict[str, Any]:
        agent_observation: dict[str, Any] = {"screenshot": str(shot), "screenshot_sha256": sha256(shot)}
        if self.config["observation_mode"] == "instrumented":
            agent_observation["telemetry"] = values
        return {
            "schema_version": 1,
            "timestamp_epoch": time.time(),
            "kind": kind,
            "action_index": meta["action_count"],
            "action": {"button": button, "frames": frames} if button else None,
            "agent_observation": agent_observation,
            "evaluator": {"telemetry": values, "metrics": dict(meta["metrics"])},
            "state": {"path": str(self.state_path), "sha256": sha256(self.state_path)},
            "totals": {"actions": meta["action_count"], "frames": meta["frame_count"]},
        }

    def step(self, button: str) -> dict[str, Any]:
        button = button.upper()
        if button not in BUTTONS:
            raise HarnessError(f"unknown button {button!r}; choose one of {', '.join(sorted(BUTTONS))}")
        meta = self.load_meta()
        frames = int(self.config["step"]["press_frames"]) + int(self.config["step"]["release_frames"])
        self._budget_check(meta, frames)
        index = meta["action_count"] + 1
        shot = self.run_dir / f"observations/{index:06d}.png"
        frames_run, values = self._run_step(frames, shot, self.state_path, button)
        meta["action_count"] = index
        meta["frame_count"] += frames_run
        self._update_metrics(meta, values)
        atomic_json(self.meta_path, meta)
        event = self._event("step", button, shot, frames_run, values, meta)
        append_jsonl(self.log_path, event)
        return event

    def checkpoint(self, name: str) -> Path:
        if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9_.-]{0,63}", name):
            raise HarnessError("checkpoint name must be 1-64 safe filename characters")
        meta = self.load_meta()
        directory = self.run_dir / "checkpoints" / name
        if directory.exists():
            raise HarnessError(f"checkpoint already exists: {name}")
        directory.mkdir(parents=True)
        shutil.copyfile(self.state_path, directory / "state.ss1")
        snapshot = dict(meta)
        snapshot["checkpoint_state_sha256"] = sha256(directory / "state.ss1")
        atomic_json(directory / "metadata.json", snapshot)
        return directory

    def restore(self, name: str) -> None:
        if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9_.-]{0,63}", name):
            raise HarnessError("checkpoint name must be 1-64 safe filename characters")
        meta = self.load_meta()
        directory = self.run_dir / "checkpoints" / name
        snapshot = load_json(directory / "metadata.json")
        state = require_file(directory / "state.ss1", "checkpoint state")
        if snapshot.get("rom_sha256") != sha256(self.rom) or snapshot.get("checkpoint_state_sha256") != sha256(state):
            raise HarnessError("checkpoint ROM or state hash mismatch")
        for field in ("config_sha256", "elf_sha256", "runner_sha256", "player_context_sha256",
                      "arsenal_manifest_sha256", "start_checkpoint_sha256", "started_at_epoch"):
            if snapshot.get(field) != meta.get(field):
                raise HarnessError(f"checkpoint {field} differs from the current session")
        temporary = self.run_dir / ".restore.ss1"
        shutil.copyfile(state, temporary)
        os.replace(temporary, self.state_path)
        # Only emulator state rewinds. Resource budgets, elapsed time, semantic
        # attempts, and the observation sequence belong to the whole run.
        meta["metrics"]["attempts"] += 1
        meta["last_probe_values"] = snapshot.get("last_probe_values", {})
        atomic_json(self.meta_path, meta)
        append_jsonl(self.log_path, {
            "schema_version": 1, "timestamp_epoch": time.time(), "kind": "restore", "checkpoint": name,
            "metrics": meta["metrics"], "totals": {"actions": meta["action_count"], "frames": meta["frame_count"]},
            "state": {"path": str(self.state_path), "sha256": sha256(self.state_path)},
        })

    def record(self, event: str, battle_id: str | None, rationale: str | None, detail: str | None) -> dict[str, Any]:
        """Record evaluator/model-declared semantics that cannot be inferred safely.

        We never guess team edits or whiteouts from image hashes. A provider
        driver should emit these declarations when it observes/completes the
        corresponding native UI operation.
        """
        if event not in SEMANTIC_EVENTS:
            raise HarnessError(f"unknown semantic event {event!r}")
        if event in {"battle_attempt", "battle_turn", "battle_success", "whiteout"} and not battle_id:
            raise HarnessError(f"{event} requires --battle-id")
        if event == "adaptation" and not rationale:
            raise HarnessError("adaptation requires --rationale")
        meta = self.load_meta()
        metric = SEMANTIC_EVENTS[event]
        meta["metrics"][metric] += 1
        if event == "whiteout":
            meta["metrics"]["deaths"] += 1
        if event == "battle_attempt":
            attempts = meta.setdefault("battle_attempts_by_id", {})
            if int(attempts.get(battle_id, 0)) > 0:
                meta["metrics"]["retries"] += 1
            attempts[battle_id] = int(attempts.get(battle_id, 0)) + 1
        atomic_json(self.meta_path, meta)
        row = {
            "schema_version": 1,
            "timestamp_epoch": time.time(),
            "kind": "semantic",
            "event": event,
            "battle_id": battle_id,
            "rationale": rationale,
            "detail": detail,
            "action_index": meta["action_count"],
            "frame_count": meta["frame_count"],
            "elapsed_seconds": time.time() - float(meta["started_at_epoch"]),
            "metrics": dict(meta["metrics"]),
        }
        append_jsonl(self.log_path, row)
        return row

    def prep(self, mutation: str, target: str, value: str, status: str, source: str | None, rationale: str | None) -> dict[str, Any]:
        if self.arsenal is None:
            raise HarnessError("prep ledger is available only in battle_lab mode")
        if mutation not in PREP_MUTATIONS:
            raise HarnessError(f"unknown prep mutation: {mutation}")
        if status not in {"requested", "applied", "rejected"}:
            raise HarnessError("prep status must be requested, applied, or rejected")
        if status in {"applied", "rejected"} and not source:
            raise HarnessError(f"{status} prep needs --source justification")
        meta = self.load_meta()
        metric = f"prep_{status}"
        meta["metrics"][metric] = int(meta["metrics"].get(metric, 0)) + 1
        typed_metric = f"prep_{mutation}_{status}"
        meta["metrics"][typed_metric] = int(meta["metrics"].get(typed_metric, 0)) + 1
        atomic_json(self.meta_path, meta)
        row = {
            "schema_version": 1,
            "timestamp_epoch": time.time(),
            "kind": "prep",
            "mutation": mutation,
            "target": target,
            "value": value,
            "status": status,
            "source_justification": source,
            "rationale": rationale,
            "battle_id": self.config.get("battle_id"),
            "action_index": meta["action_count"],
            "frame_count": meta["frame_count"],
            "elapsed_seconds": time.time() - float(meta["started_at_epoch"]),
        }
        append_jsonl(self.log_path, row)
        return row

    def apply_preparation(self, request_path: Path) -> dict[str, Any]:
        if self.arsenal is None or self.elf is None:
            raise HarnessError("canonical preparation requires battle_lab mode and an ELF")
        request = load_json(request_path)
        roster = request.get("roster")
        if request.get("schema_version") != 1 or not isinstance(roster, list) or not 1 <= len(roster) <= 6:
            raise HarnessError("preparation request needs schema 1 and a roster of 1-6")
        available = {row.get("species"): row for row in self.arsenal["pokemon"] if isinstance(row, dict)}
        legal_items = {row.get("item") for row in self.arsenal["items"] if isinstance(row, dict)} | {"ITEM_NONE"}
        constants_needed: list[str] = []
        normalized: list[dict[str, Any]] = []
        cap = int(self.arsenal["level_cap"])
        for slot, raw in enumerate(roster):
            if not isinstance(raw, dict) or raw.get("species") not in available:
                raise HarnessError(f"slot {slot}: species is not in the checkpoint arsenal")
            species = str(raw["species"])
            source = available[species]
            level = int(raw.get("level", cap))
            if level != cap:
                raise HarnessError(f"slot {slot}: level must equal active Hard cap {cap}")
            preset = raw.get("preset")
            if preset is not None:
                if not isinstance(preset, dict) or preset not in source.get("presets", []):
                    raise HarnessError(f"slot {slot}: preset is not a generated legal choice")
            moves = raw.get("moves")
            if moves is not None:
                if not isinstance(moves, list) or len(moves) != 4 or any(move not in source.get("legal_moves", []) for move in moves):
                    raise HarnessError(f"slot {slot}: move list is not four generated legal moves")
            ability = raw.get("ability")
            if ability is not None and ability not in source.get("legal_abilities", []):
                raise HarnessError(f"slot {slot}: Ability is not legal for the species")
            item = raw.get("item")
            if item is not None and item not in legal_items:
                raise HarnessError(f"slot {slot}: item is not contemporaneously available")
            if item and item in self.arsenal.get("mega_access", {}).get("legal_stones", []) and not self.arsenal["mega_access"]["bracelet"]:
                raise HarnessError(f"slot {slot}: Mega bracelet is not accessible")
            nature = raw.get("nature")
            if nature is not None and not re.fullmatch(r"NATURE_[A-Z0-9_]+", str(nature)):
                raise HarnessError(f"slot {slot}: invalid nature")
            points = raw.get("stat_points")
            if points is not None and (not isinstance(points, list) or len(points) != 6 or any(not isinstance(value, int) or not 0 <= value <= 32 for value in points) or sum(points) != 66):
                raise HarnessError(f"slot {slot}: Stat Points must total 66 with 0-32 per stat")
            constants_needed += [species]
            constants_needed += moves or []
            constants_needed += [value for value in (ability, item, nature) if value is not None]
            normalized.append({"species": species, "level": level, "preset": preset, "moves": moves, "ability": ability, "item": item, "nature": nature, "stat_points": points})
        ids = resolve_game_constants(constants_needed)
        symbol_table = symbols(self.elf)
        names = ["gEcAgentPrepCommand", "gEcAgentPrepResult", "gEcAgentPrepErrorSlot", "gEcAgentPrepPartyCount", "gEcAgentPrepSpecies", "gEcAgentPrepPreset", "gEcAgentPrepFormat", "gEcAgentPrepLevel", "gEcAgentPrepMoves", "gEcAgentPrepNature", "gEcAgentPrepAbility", "gEcAgentPrepItem", "gEcAgentPrepStatPoints", "gEcHeadlessFixtureActiveScenario"]
        missing = [name for name in names if name not in symbol_table]
        if missing:
            raise HarnessError(f"agent-preparation fixture symbols missing from ELF: {missing}")
        keep = 0xFFFFFFFF
        writes: list[dict[str, Any]] = []
        def write(name: str, offset: int, value: int, verify: bool = True) -> None:
            writes.append({"name": f"{name}+{offset}", "address": symbol_table[name] + offset, "width": 4, "value": value, "verify_after": verify})
        write("gEcHeadlessFixtureActiveScenario", 0, 0)
        write("gEcAgentPrepResult", 0, 0, False)
        write("gEcAgentPrepPartyCount", 0, len(normalized))
        for slot in range(6):
            mon = normalized[slot] if slot < len(normalized) else None
            write("gEcAgentPrepSpecies", slot * 4, ids[mon["species"]] if mon else 0)
            write("gEcAgentPrepLevel", slot * 4, mon["level"] if mon else 0)
            preset = mon["preset"] if mon else None
            write("gEcAgentPrepPreset", slot * 4, int(preset["choice"]) if preset else keep)
            write("gEcAgentPrepFormat", slot * 4, 1 if preset and preset["format"] == "singles" else 0)
            for move_slot in range(4):
                move = mon["moves"][move_slot] if mon and mon["moves"] else None
                write("gEcAgentPrepMoves", (slot * 4 + move_slot) * 4, ids[move] if move else keep)
            for name, key in (("gEcAgentPrepNature", "nature"), ("gEcAgentPrepAbility", "ability"), ("gEcAgentPrepItem", "item")):
                value = mon[key] if mon else None
                write(name, slot * 4, ids[value] if value else keep)
            for stat in range(6):
                value = mon["stat_points"][stat] if mon and mon["stat_points"] else keep
                write("gEcAgentPrepStatPoints", (slot * 6 + stat) * 4, value)
        write("gEcAgentPrepCommand", 0, 1, False)
        result_reads = [
            {"name": "prep_result", "address": symbol_table["gEcAgentPrepResult"], "width": 4},
            {"name": "prep_error_slot", "address": symbol_table["gEcAgentPrepErrorSlot"], "width": 4},
        ]
        meta = self.load_meta()
        shot = self.run_dir / f"observations/prepared-{meta.get('prep_revision_count', 0) + 1:04d}.png"
        frames, values = self._run_step(2, shot, self.state_path, None, writes, result_reads)
        if values["prep_result"] != 1:
            row = self.prep("roster", "party", json.dumps(normalized, sort_keys=True), "rejected", f"fixture result {values['prep_result']} slot {values['prep_error_slot']}", request.get("rationale"))
            raise HarnessError(f"canonical preparation rejected: result={values['prep_result']} slot={values['prep_error_slot']}; logged {row['timestamp_epoch']}")
        meta = self.load_meta()
        meta["frame_count"] += frames
        meta["prep_revision_count"] = int(meta.get("prep_revision_count", 0)) + 1
        atomic_json(self.meta_path, meta)
        row = self.prep("roster", "party", json.dumps(normalized, sort_keys=True), "applied", "fixture canonical game functions plus checkpoint arsenal validation", request.get("rationale"))
        row["prepared_state_sha256"] = sha256(self.state_path)
        row["screenshot"] = str(shot)
        append_jsonl(self.log_path, {**row, "kind": "preparation_artifact"})
        return row


def print_json(value: Any) -> None:
    print(json.dumps(value, indent=2, sort_keys=True))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", type=Path, default=Path(__file__).with_name("example_run.json"))
    parser.add_argument("--run-dir", type=Path, help="override config run_dir")
    sub = parser.add_subparsers(dest="command", required=True)
    init_parser = sub.add_parser("init")
    init_parser.add_argument("--replace", action="store_true")
    step_parser = sub.add_parser("step")
    step_parser.add_argument("button")
    run_parser = sub.add_parser("run-manual", help="run a fixed keyless smoke/manual policy")
    run_parser.add_argument("buttons", nargs="*", default=["START", "A", "WAIT", "B"])
    check_parser = sub.add_parser("checkpoint")
    check_parser.add_argument("name")
    restore_parser = sub.add_parser("restore")
    restore_parser.add_argument("name")
    record_parser = sub.add_parser("record", help="record a scored semantic event")
    record_parser.add_argument("event", choices=sorted(SEMANTIC_EVENTS))
    record_parser.add_argument("--battle-id")
    record_parser.add_argument("--rationale")
    record_parser.add_argument("--detail")
    prep_parser = sub.add_parser("prep", help="record a requested/applied/rejected legal prep mutation")
    prep_parser.add_argument("mutation", choices=sorted(PREP_MUTATIONS))
    prep_parser.add_argument("target")
    prep_parser.add_argument("value")
    prep_parser.add_argument("--status", choices=("requested", "applied", "rejected"), default="requested")
    prep_parser.add_argument("--source")
    prep_parser.add_argument("--rationale")
    sub.add_parser("arsenal", help="print the checkpoint-bound legal preparation arsenal")
    apply_parser = sub.add_parser("apply-prep", help="validate and canonically apply a complete team request")
    apply_parser.add_argument("request", type=Path)
    sub.add_parser("summary")
    args = parser.parse_args()
    try:
        session = Session(args.config, args.run_dir)
        with session.lock():
            if args.command == "init":
                print_json(session.init(args.replace))
            elif args.command == "step":
                print_json(session.step(args.button))
            elif args.command == "run-manual":
                if not session.meta_path.exists():
                    session.init()
                for button in args.buttons:
                    event = session.step(button)
                    print(f"{event['action_index']:06d} {button} {event['agent_observation']['screenshot']}")
                print_json(session.load_meta())
            elif args.command == "checkpoint":
                print(session.checkpoint(args.name))
            elif args.command == "restore":
                session.restore(args.name)
                print_json(session.load_meta())
            elif args.command == "record":
                print_json(session.record(args.event, args.battle_id, args.rationale, args.detail))
            elif args.command == "prep":
                print_json(session.prep(args.mutation, args.target, args.value, args.status, args.source, args.rationale))
            elif args.command == "arsenal":
                if session.arsenal is None:
                    raise HarnessError("arsenal is available only in battle_lab mode")
                print_json(session.arsenal)
            elif args.command == "apply-prep":
                print_json(session.apply_preparation(args.request))
            elif args.command == "summary":
                print_json(session.load_meta())
    except HarnessError as error:
        print(f"agent-player: FAIL: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
