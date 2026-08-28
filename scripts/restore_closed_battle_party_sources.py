#!/usr/bin/env python3
"""Restore canonical source arrays for closed battles after legacy generators run."""

from __future__ import annotations

import argparse
import importlib.util
from pathlib import Path
import re
import subprocess
import sys

import verdant_custom_teams as custom
import verdant_doubles_conversion as doubles
import verdant_team_polish as polish

ROOT = Path(__file__).resolve().parents[1]
PARTIES = ROOT / "src/data/trainer_parties.h"
TRAINERS = ROOT / "src/data/trainers.h"

# Battles 73-120 are source-closed by durable generator modules. Loading their
# exact teams here makes the old broad ordinary-team generator harmless: any
# accidental filler append is replaced by the closure, and the rendered
# ``Verdant polish`` marker prevents a later broad pass from touching it again.
CLOSURE_GENERATORS = [
    *(ROOT / "scripts" / f"emerald_champions_battle{index}.py" for index in range(73, 77)),
    *(ROOT / "scripts" / f"emerald_champions_battle{index}.py" for index in range(81, 124)),
]

# Exact records reported by the closed-battle audit after the parallel preset rewrite.
# Battle 48 is deliberately absent because it has a new story-correct source closure.
TRAINER_IDS = [
    "TRAINER_MAY_ROUTE_103_TREECKO", "TRAINER_MAY_ROUTE_103_TORCHIC", "TRAINER_MAY_ROUTE_103_MUDKIP",
    "TRAINER_BRENDAN_ROUTE_103_TREECKO", "TRAINER_BRENDAN_ROUTE_103_TORCHIC", "TRAINER_BRENDAN_ROUTE_103_MUDKIP",
    "TRAINER_ALLEN", "TRAINER_TIANA", "TRAINER_DARIAN", "TRAINER_LYLE", "TRAINER_GINA_AND_MIA_1",
    "TRAINER_JOSH", "TRAINER_TOMMY", "TRAINER_JOEY", "TRAINER_DEVAN", "TRAINER_SARAH", "TRAINER_DAWSON",
    "TRAINER_JANICE", "TRAINER_JERRY_1", "TRAINER_ELLIOT_1", "TRAINER_EDMOND", "TRAINER_DWAYNE",
    "TRAINER_SIMON", "TRAINER_ISABEL_1", "TRAINER_KALEB", "TRAINER_ROBIN", "TRAINER_EDWARD",
    "TRAINER_ALYSSA", "TRAINER_JACLYN", "TRAINER_WALLY_MAUVILLE", "TRAINER_VIVIAN", "TRAINER_KIRK",
    "TRAINER_WATTSON_1",
]


def git_head(path: str) -> str:
    return subprocess.run(
        ["git", "show", f"HEAD:{path}"], cwd=ROOT, check=True, text=True, stdout=subprocess.PIPE
    ).stdout


def trainer_party_names(trainers_source: str) -> dict[str, str]:
    result = {}
    for trainer_id in TRAINER_IDS:
        match = re.search(
            rf"^\s*\[{re.escape(trainer_id)}\]\s*=\s*\{{(.*?)(?=^\s*\[TRAINER_|\Z)",
            trainers_source,
            re.M | re.S,
        )
        if not match:
            raise ValueError(f"missing trainer record {trainer_id}")
        party = re.search(r"\.party\s*=\s*\{\.ItemCustomMoves\s*=\s*([A-Za-z0-9_]+)\}", match.group(1))
        if not party:
            raise ValueError(f"missing custom party for {trainer_id}")
        result[trainer_id] = party.group(1)
    return result


def party_block(source: str, party_name: str) -> str:
    match = re.search(
        rf"^static const struct TrainerMonItemCustomMoves\s+{re.escape(party_name)}\[\]\s*=\s*\{{.*?^\}};",
        source,
        re.M | re.S,
    )
    if not match:
        raise ValueError(f"missing party array {party_name}")
    return match.group(0)


def load_module(path: Path):
    spec = importlib.util.spec_from_file_location(f"closed_{path.stem}", path)
    if spec is None or spec.loader is None:
        raise ValueError(f"cannot load {path}")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def generated_closure_teams() -> dict[str, list[dict]]:
    result: dict[str, list[dict]] = {}
    for path in CLOSURE_GENERATORS:
        module = load_module(path)
        if hasattr(module, "TEAMS"):
            result.update(module.TEAMS)
            continue
        trainer_ids = module.design()["trainer_ids"]
        if hasattr(module, "TEAM"):
            if len(trainer_ids) != 1:
                raise ValueError(f"ambiguous TEAM owner in {path.name}: {trainer_ids}")
            result[trainer_ids[0]] = module.TEAM
            continue
        index = int(path.stem.removeprefix("emerald_champions_battle"))
        if index == 96:
            named = {"TRAINER_KAI": module.KAI_TEAM, "TRAINER_CHARLOTTE": module.CHARLOTTE_TEAM}
        elif index == 104:
            named = {"TRAINER_ANGELINA": module.ANGELINA_TEAM, "TRAINER_LUCAS_1": module.LUCAS_TEAM}
        elif index == 105:
            named = {"TRAINER_COURTNEY_METEOR_FALLS": module.COURTNEY_TEAM, "TRAINER_GRUNT_METEOR_FALLS": module.GRUNT_TEAM}
        elif index == 108:
            named = {"TRAINER_GRUNT_MT_CHIMNEY_1": module.GRUNT1_TEAM, "TRAINER_GRUNT_MT_CHIMNEY_2": module.GRUNT2_TEAM}
        else:
            named = None
        if named is None:
            raise ValueError(f"no closure team mapping for {path.name}")
        result.update(named)

    winstrate = load_module(ROOT / "scripts" / "emerald_champions_winstrate_arc.py")
    for config in winstrate.CONFIGS:
        result[config["trainer_id"]] = config["team"]
    heat_epilogue = load_module(ROOT / "scripts" / "emerald_champions_battles124_133.py")
    result.update(heat_epilogue.ALL_TEAMS)
    route111_north = load_module(ROOT / "scripts" / "emerald_champions_battles134_143.py")
    result.update(route111_north.ALL_TEAMS)
    return result


def expected_source() -> tuple[str, list[str]]:
    current = PARTIES.read_text()
    baseline_parties = git_head("src/data/trainer_parties.h")
    baseline_trainers = git_head("src/data/trainers.h")
    names = sorted(set(trainer_party_names(baseline_trainers).values()))
    for name in names:
        current_block = party_block(current, name)
        baseline_block = party_block(baseline_parties, name)
        current = current.replace(current_block, baseline_block, 1)

    current_trainers = TRAINERS.read_text()
    blocks = doubles.trainer_blocks(current_trainers)
    generated = generated_closure_teams()
    for trainer_id, team in generated.items():
        party_name = doubles.party_name(blocks[trainer_id].group(0))
        entries = [polish.render(build, trainer_id) for build in team]
        current = custom.replace_party_body(current, party_name, entries)
    return current, names + sorted(generated)


def main():
    parser = argparse.ArgumentParser(); parser.add_argument("--write", action="store_true"); parser.add_argument("--check", action="store_true"); args = parser.parse_args()
    if not args.write and not args.check: parser.error("choose --write or --check")
    expected, names = expected_source()
    if args.write:
        PARTIES.write_text(expected)
    if args.check and PARTIES.read_text() != expected:
        raise SystemExit("FAIL: one or more closed Battle 1-68 party arrays still drift from checkpoint")
    print(f"PASS: {len(names)} canonical closed-battle party arrays match their checkpoints and generators")


if __name__ == "__main__": main()
