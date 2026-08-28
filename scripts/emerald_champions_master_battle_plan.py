#!/usr/bin/env python3
"""Bootstrap and validate the single editable master battle-design document.

The TXT file is the authoring surface.  Bootstrap is intentionally one-shot:
after creation, designers edit the TXT directly and this tool only validates
coverage and structure.  It never regenerates or overwrites authored work.
"""

from __future__ import annotations

import argparse
import importlib.util
import json
import re
from pathlib import Path

import verdant_custom_teams as custom
import verdant_doubles_conversion as doubles
import verdant_team_polish as polish


ROOT = Path(__file__).resolve().parents[1]
MASTER = ROOT / "docs/emerald_champions_master_battle_design.txt"
ATLAS = ROOT / "docs/verdant_physical_encounter_atlas.json"
DESIGNS = ROOT / "docs/verdant_bespoke_battle_designs.json"
LEDGER = ROOT / "docs/verdant_battle_experience_ledger.json"
TRAINERS = ROOT / "src/data/trainers.h"
PARTIES = ROOT / "src/data/trainer_parties.h"
DRAFT_BATCH = ROOT / "scripts/emerald_champions_battles144_155.py"


def load_module(path: Path):
    spec = importlib.util.spec_from_file_location(f"master_{path.stem}", path)
    if spec is None or spec.loader is None:
        raise ValueError(f"cannot import {path}")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def one_line(value) -> str:
    if value is None:
        return "PENDING"
    if isinstance(value, (list, tuple)):
        return "; ".join(one_line(item) for item in value) or "NONE"
    if isinstance(value, dict):
        return json.dumps(value, ensure_ascii=False, sort_keys=True)
    return re.sub(r"\s+", " ", str(value)).strip() or "PENDING"


def source_teams() -> tuple[dict[str, list[dict]], dict[str, str]]:
    trainers = TRAINERS.read_text()
    parties = PARTIES.read_text()
    blocks = doubles.trainer_blocks(trainers)
    teams = {}
    formats = {}
    for trainer_id, match in blocks.items():
        block = match.group(0)
        formats[trainer_id] = "double" if ".doubleBattle = TRUE" in block else "single"
        party_name = doubles.party_name(block)
        if party_name == "NULL":
            continue
        party = doubles.party_match(parties, party_name)
        if party:
            teams[trainer_id] = [
                polish.parse_entry(entry)
                for entry in custom.party_entries(party.group(2))
            ]
    return teams, formats


def current_design(group: dict, designs: dict, ledger: dict, draft_by_main: dict):
    sequence_index = group.get("sequenceIndex")
    if isinstance(sequence_index, int) and sequence_index <= 143:
        encounter_id = group["groupId"]
        design = designs.get(encounter_id, {})
        row = ledger.get(sequence_index, {})
        return {
            "campaign_order": sequence_index,
            "proposed_id": encounter_id,
            "status": "implemented_closed_runtime_unplayed",
            "difficulty": row.get("target_difficulty", design.get("manual_difficulty")),
            "question": design.get("primary_player_question", row.get("primary_player_question")),
            "theme": design.get("team_intent", row.get("tempo")),
            "weakness": design.get("intended_counterplay", row.get("intentional_weakness")),
            "lesson": design.get("first_loss_lesson", row.get("first_loss_lesson")),
            "strongest": design.get("author_self_check", {}).get("strongest_part"),
            "weakest": design.get("author_self_check", {}).get("weakest_link"),
            "references": row.get("historic_reference_ids", []),
            "teams": design.get("source_teams"),
            "dialogue_status": "implemented_and_width_checked",
            "reservation_status": one_line(design.get("campaign_reservations")),
        }
    trainer_ids = group.get("resolvedOpponentTrainerIds", [])
    for trainer_id in trainer_ids:
        if trainer_id in draft_by_main:
            config, module = draft_by_main[trainer_id]
            design = module.design(config)
            return {
                "campaign_order": config["index"],
                "proposed_id": config["id"],
                "status": "design_draft_not_applied",
                "difficulty": config["target"],
                "question": config["question"],
                "theme": config["tempo"],
                "weakness": config["weakness"],
                "lesson": config["lesson"],
                "strongest": config["tempo"],
                "weakest": config["weakness"],
                "references": config["refs"],
                "teams": config["teams"],
                "dialogue_status": "draft_exact_lines_not_applied",
                "reservation_status": one_line(design.get("campaign_reservations")),
            }
    return {
        "campaign_order": None,
        "proposed_id": None,
        "status": "design_pending_source_baseline_only",
        "difficulty": None,
        "question": None,
        "theme": None,
        "weakness": None,
        "lesson": None,
        "strongest": None,
        "weakest": None,
        "references": [],
        "teams": None,
        "dialogue_status": "audit_pending",
        "reservation_status": "audit_pending",
    }


def render_member(member: dict) -> str:
    return (
        f"{member['species']} @ {member['item']} | level={member['level']} | "
        f"ability_slot={member['ability_slot']} | {member['spread']} | "
        f"moves={','.join(member['moves'])}"
    )


def bootstrap_text() -> str:
    atlas = json.loads(ATLAS.read_text())
    designs = json.loads(DESIGNS.read_text())["designs"]
    ledger = {
        row["index"]: row
        for row in json.loads(LEDGER.read_text())["entries"]
    }
    baseline_teams, baseline_formats = source_teams()
    draft_by_main = {}
    if DRAFT_BATCH.exists():
        module = load_module(DRAFT_BATCH)
        for config in module.CONFIGS:
            draft_by_main[config["main"]] = (config, module)

    rows = []
    for group in atlas["physicalGroups"]:
        design = current_design(group, designs, ledger, draft_by_main)
        rows.append((group, design))
    rows.sort(
        key=lambda pair: (
            pair[1]["campaign_order"] is None,
            pair[1]["campaign_order"] if pair[1]["campaign_order"] is not None else pair[0].get("chapterRank", 999),
            pair[0].get("atlasOrdinal", 9999),
        )
    )

    total_branches = sum(
        len(set(group.get("resolvedOpponentTrainerIds", [])))
        for group, _ in rows
    )
    counts = {}
    for _, design in rows:
        counts[design["status"]] = counts.get(design["status"], 0) + 1

    lines = [
        "EMERALD CHAMPIONS — MASTER BATTLE DESIGN",
        "VERSION: 1",
        "AUTHORING_STATUS: ACTIVE — THIS TXT IS THE CAMPAIGN DESIGN SOURCE OF TRUTH",
        "REGENERATION_RULE: Bootstrap once. Edit this TXT directly. Never overwrite authored blocks from source.",
        "",
        "SCOPE",
        f"physical_encounter_groups: {len(rows)}",
        f"resolved_opponent_branches_in_physical_groups: {total_branches}",
        f"proven_reachable_script_invocations: {atlas['totals']['provenReachableInvocations']}",
        f"script_battle_declarations: {atlas['totals']['scriptBattleDeclarations']}",
        f"resolved_opponent_trainer_ids: {atlas['totals']['resolvedOpponentTrainerIdsIncludingRematches']}",
        f"status_counts: {json.dumps(counts, sort_keys=True)}",
        "",
        "DESIGN THESIS",
        "Every encounter should be bespoke, native, legible, and worth remembering.",
        "Quality target 10/10 does not mean difficulty target 10/10.",
        "The player receives broad competitive tools; difficulty should come from interesting decisions, not grinding.",
        "Hard is authored. Medium subtracts 2 opponent levels. Easy subtracts 4. Teams and AI remain identical.",
        "Bosses may be near-lethal. Ordinary trainers must stay challenging without turning every route into a gauntlet.",
        "",
        "DIFFICULTY PACING — PROVISIONAL AUDIT BANDS",
        "ordinary_route: 7.5-8.4",
        "notable_optional_or_route_ace: 8.5-9.0",
        "mini_boss_or_faction_admin: 9.1-9.6",
        "gym_leader_major_rival_faction_boss: 9.7-10.0",
        "elite_four_champion_superboss: 10.0",
        "These are advisory bands, not quotas. Judge each battle and the previous 8-10 encounters.",
        "",
        "FATIGUE PRINCIPLE",
        "A lower-pressure fight is not filler. It still needs a clean idea, legal set, real counterplay, and memorable identity.",
        "Relief comes from fewer simultaneous demands, shorter teams, simpler information, or wider counterplay—not dumb AI.",
        "Do not solve fatigue by making all later battles easier. Rebalance the whole campaign here before implementation.",
        "",
        "WORKFLOW",
        "1. Establish true campaign order for every PENDING block, including optional and backtrack encounters.",
        "2. Protect marquee anchors and reservations.",
        "3. Author exact teams, branches, dialogue intent, AI needs, difficulty, strongest part, and weakest link in this TXT.",
        "4. Audit rolling fatigue, species/mechanic repetition, specialty fidelity, progression, legality, and rewards globally.",
        "5. Only after the document passes, implement in 100-battle batches and compile once per batch.",
        "",
        "STATUS LEGEND",
        "implemented_closed_runtime_unplayed = already in source, but still eligible for master rebalance.",
        "design_draft_not_applied = authored proposal only; no trainer source change.",
        "design_pending_source_baseline_only = current ROM data shown solely as a baseline, not an approved design.",
        "",
    ]

    for planning_ordinal, (group, design) in enumerate(rows, 1):
        maps = sorted({source.get("map") for source in group.get("sources", []) if source.get("map")})
        trainer_ids = list(group.get("resolvedOpponentTrainerIds", []))
        teams = design["teams"] or {}
        lines += [
            f"=== ENCOUNTER {planning_ordinal:04d} ===",
            f"physical_group_id: {group['groupId']}",
            f"proposed_encounter_id: {one_line(design['proposed_id'])}",
            f"campaign_order: {one_line(design['campaign_order'])}",
            f"atlas_ordinal: {group.get('atlasOrdinal', 'PENDING')}",
            f"chapter: {one_line(group.get('primaryChapter'))}",
            f"strict_cap: {one_line(group.get('levelCap'))}",
            f"location: {one_line(maps)}",
            f"requirement: {one_line(group.get('campaignCategory'))}",
            f"status: {design['status']}",
            "quality_target: 10",
            f"difficulty_target: {one_line(design['difficulty'])}",
            "difficulty_observed: UNPLAYED",
            "fatigue_role: PENDING_AUDIT",
            f"primary_question: {one_line(design['question'])}",
            f"theme_and_tempo: {one_line(design['theme'])}",
            f"intentional_weakness: {one_line(design['weakness'])}",
            f"first_loss_lesson: {one_line(design['lesson'])}",
            f"strongest_part: {one_line(design['strongest'])}",
            f"weakest_link: {one_line(design['weakest'])}",
            f"competitive_references: {one_line(design['references'])}",
            f"dialogue_status: {design['dialogue_status']}",
            f"reservation_status: {design['reservation_status']}",
            f"trainer_ids: {one_line(trainer_ids)}",
            "branches:",
        ]
        branch_ids = list(dict.fromkeys([*trainer_ids, *teams.keys()]))
        if not branch_ids:
            lines.append("--- BRANCH NONE ---")
            lines.append("trainer_id: NONE")
            lines.append("format: NONE")
            lines.append("team: NONE")
        for trainer_id in branch_ids:
            team = teams.get(trainer_id, baseline_teams.get(trainer_id, []))
            lines += [
                f"--- BRANCH {trainer_id} ---",
                f"trainer_id: {trainer_id}",
                f"format: {baseline_formats.get(trainer_id, 'PENDING')}",
                "team:",
            ]
            if team:
                lines.extend(
                    f"  {slot}. {render_member(member)}"
                    for slot, member in enumerate(team, 1)
                )
            else:
                lines.append("  PENDING")
        lines += ["source_note: Existing source is evidence, never automatic approval.", "=== END ENCOUNTER ===", ""]
    return "\n".join(lines)


def validate() -> None:
    if not MASTER.exists():
        raise SystemExit("FAIL: master TXT does not exist; run --bootstrap")
    text = MASTER.read_text()
    expected_groups = json.loads(ATLAS.read_text())["physicalGroups"]
    blocks = re.findall(r"^=== ENCOUNTER \d{4} ===$(.*?)^=== END ENCOUNTER ===$", text, re.M | re.S)
    if len(blocks) != len(expected_groups):
        raise SystemExit(f"FAIL: master has {len(blocks)} encounter blocks, expected {len(expected_groups)}")
    ids = []
    required = [
        "physical_group_id:", "campaign_order:", "status:", "quality_target:",
        "difficulty_target:", "fatigue_role:", "primary_question:",
        "strongest_part:", "weakest_link:", "trainer_ids:", "branches:",
    ]
    for index, block in enumerate(blocks, 1):
        for field in required:
            if field not in block:
                raise SystemExit(f"FAIL: encounter {index} missing {field}")
        match = re.search(r"^physical_group_id:\s*(\S+)", block, re.M)
        if not match:
            raise SystemExit(f"FAIL: encounter {index} missing group ID")
        ids.append(match.group(1))
    expected_ids = {group["groupId"] for group in expected_groups}
    if set(ids) != expected_ids or len(ids) != len(set(ids)):
        raise SystemExit("FAIL: master physical-group coverage is not exact and unique")
    atlas_branches = sum(len(dict.fromkeys(group.get("resolvedOpponentTrainerIds", []))) for group in expected_groups)
    document_branches = len(re.findall(r"^--- BRANCH TRAINER_", text, re.M))
    if document_branches < atlas_branches:
        raise SystemExit(f"FAIL: master has {document_branches} trainer branches, below atlas minimum {atlas_branches}")
    print(f"PASS: master TXT covers all {len(blocks)} physical encounters and {document_branches} trainer branches")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--bootstrap", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.bootstrap and not args.check:
        parser.error("choose --bootstrap or --check")
    if args.bootstrap:
        if MASTER.exists():
            raise SystemExit(f"REFUSING: {MASTER.name} already exists; bootstrap never overwrites authored work")
        MASTER.write_text(bootstrap_text())
        print(f"WROTE: {MASTER}")
    if args.check:
        validate()


if __name__ == "__main__":
    main()
