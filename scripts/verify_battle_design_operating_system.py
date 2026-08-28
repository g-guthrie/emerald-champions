#!/usr/bin/env python3
"""Prove the Emerald Champions battle-design operating-system contract."""

from __future__ import annotations

import json
from pathlib import Path
import sys


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

import verdant_battle_dossier as dossier_packets  # noqa: E402
import verdant_marquee_design_audit as marquee_audit  # noqa: E402


OS_PATH = ROOT / "docs/emerald_champions_battle_design_operating_system.json"


def load(path: str | Path) -> dict:
    path = Path(path)
    if not path.is_absolute():
        path = ROOT / path
    return json.loads(path.read_text())


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def require_keys(value: dict, required: list[str], label: str) -> None:
    missing = sorted(set(required) - set(value))
    require(not missing, f"{label} missing fields: {missing}")
    for key in required:
        item = value[key]
        if key in {"observed", "observed_difficulty", "mechanics_proposal", "previous_encounters"}:
            continue
        require(item not in (None, "", []), f"{label}.{key} is empty")


def main() -> None:
    operating_system = load(OS_PATH)
    sources = operating_system["authoritative_sources"]
    sequence = load(sources["canonical_sequence"])
    atlas = load(sources["physical_atlas"])
    guide = load(sources["battle_guide"])
    designs_payload = load(sources["marquee_designs"])
    corpus_meta = load(sources["competitive_index_meta"])
    state = operating_system["current_state"]

    entries = sequence["entries"]
    closed = [entry for entry in entries if entry.get("status") == "closed"]
    next_entries = [entry for entry in entries if entry.get("status") == "next"]
    queued = [entry for entry in entries if entry.get("status") == "queued"]
    totals = atlas["totals"]
    guide_meta = guide["meta"]

    require(len(closed) == state["closed_encounters"], "closed encounter count drifted")
    require(len(next_entries) == 1, "canonical sequence must have exactly one next encounter")
    require(next_entries[0]["index"] == state["next_index"], "next battle index drifted")
    require(next_entries[0]["encounter_id"] == state["next_encounter_id"], "next encounter identity drifted")
    require(len(queued) == state["queued_sequence_entries"], "queued sequence count drifted")
    require(len(entries) == state["canonical_sequence_groups"], "canonical sequence boundary drifted")
    require(totals["physicalEncounterGroups"] == state["physical_encounter_groups"], "physical group count drifted")
    require(totals["unorderedFoundationGroups"] == state["unordered_physical_groups"], "unordered physical count drifted")
    require(guide_meta["reachableBattleDefinitions"] == state["reachable_battle_definitions"], "reachable definition count drifted")
    require(guide_meta["internalUnusedDefinitions"] == state["internal_unreachable_definitions"], "unreachable definition count drifted")
    require(guide_meta["qualityAudit"]["trainer_records"] == state["trainer_records"], "trainer record count drifted")
    require(guide_meta["bespokeClosed"] == state["closed_encounters"], "guide closed count drifted")
    require(state["campaign_complete"] is False, "campaign cannot be complete while physical groups remain unordered or open")

    policy = operating_system["policy"]
    require(policy["advisory_not_allocator"] is True, "operating system became an allocator")
    require(policy["global_quotas_forbidden"] is True, "global quotas are no longer forbidden")
    require(policy["marquee_design_order"] == "backward", "marquee design order drifted")
    require(policy["implementation_order"] == "forward", "implementation order drifted")

    expected_lifecycle = [
        "blueprint", "authored-draft", "author-self-checked", "design-complete",
        "source-implemented", "static-validated", "source-closed",
        "runtime-playtested", "release-closed",
    ]
    require(operating_system["lifecycle"] == expected_lifecycle, "battle lifecycle drifted")

    contract = operating_system["dossier_contract"]
    require(set(contract["required_top_level"]) == marquee_audit.REQUIRED_DOSSIER_FIELDS,
            "machine contract and marquee validator disagree on top-level dossier fields")
    require(set(contract["mon_required"]) == marquee_audit.REQUIRED_MON_FIELDS,
            "machine contract and marquee validator disagree on Pokemon fields")

    designs = designs_payload["designs"]
    require(set(designs) == set(designs_payload["expected_phase_anchors"]), "League anchor set drifted")
    require(len(designs) == state["main_story_league_dossiers"], "League dossier count drifted")
    design_complete = 0
    for anchor_id, dossier in designs.items():
        require_keys(dossier, contract["required_top_level"], anchor_id)
        require_keys(dossier["campaign_state"], contract["campaign_state_required"], f"{anchor_id}.campaign_state")
        require_keys(dossier["runtime"], contract["runtime_required"], f"{anchor_id}.runtime")
        require_keys(dossier["rolling_context"], contract["rolling_context_required"], f"{anchor_id}.rolling_context")
        require_keys(dossier["identity"], contract["identity_required"], f"{anchor_id}.identity")
        require_keys(dossier["difficulty"], contract["difficulty_required"], f"{anchor_id}.difficulty")
        require_keys(dossier["ordering"], contract["ordering_required"], f"{anchor_id}.ordering")
        require_keys(dossier["ai"], contract["ai_required"], f"{anchor_id}.ai")
        require_keys(dossier["counterplay"], contract["counterplay_required"], f"{anchor_id}.counterplay")
        require_keys(dossier["competitive_research"], contract["competitive_research_required"], f"{anchor_id}.competitive_research")
        require_keys(dossier["campaign_reservations"], contract["reservations_required"], f"{anchor_id}.campaign_reservations")
        require_keys(dossier["presentation"], contract["presentation_required"], f"{anchor_id}.presentation")
        require_keys(dossier["verification"], contract["verification_required"], f"{anchor_id}.verification")
        require_keys(dossier["author_self_check"], contract["author_self_check_required"], f"{anchor_id}.author_self_check")

        require(dossier["status"] == {
            "design": "design-complete",
            "source": "unimplemented",
            "static": "design-validated",
            "runtime": "unplayed",
        }, f"{anchor_id} overstates or understates its status")
        require(dossier["difficulty"]["target"] == 10, f"{anchor_id} is not target 10")
        require(dossier["difficulty"]["observed"] is None, f"{anchor_id} claims unplayed observed difficulty")
        require(dossier["verification"]["observed_difficulty"] is None, f"{anchor_id} verification claims observed difficulty")
        require(len(dossier["team"]) == 6, f"{anchor_id} does not have six exact Pokemon")
        require(sum(bool(mon["mega_candidate"]) for mon in dossier["team"]) == 1,
                f"{anchor_id} must reserve exactly one Mega candidate")
        for index, mon in enumerate(dossier["team"], 1):
            require_keys(mon, contract["mon_required"], f"{anchor_id}.team[{index}]")
            require(len(mon["moves"]) == 4, f"{anchor_id}.team[{index}] lacks four moves")
        if not dossier["rolling_context"]["available"]:
            require(dossier["rolling_context"]["previous_encounters"] == [],
                    f"{anchor_id} fabricates future previous-ten context")
        design_complete += 1

        packet_path = ROOT / "docs/dossier_packets" / f"{anchor_id.lower()}.json"
        require(packet_path.exists(), f"{anchor_id} authoring packet is missing")
        packet = load(packet_path)
        filters = packet.get("competitive_filters", {})
        expected_packet = dossier_packets.build(
            anchor_id,
            packet.get("competitive_queries", []),
            filters.get("tags", []),
            filters.get("pokemon", []),
            filters.get("limit", 12),
        )
        require(packet == expected_packet, f"{anchor_id} authoring packet is stale")

    require(design_complete == state["main_story_league_design_complete"], "League design-complete count drifted")
    expected_corpus = {key: corpus_meta[key] for key in ("version", "record_count", "sha256")}
    actual_corpus = {key: designs_payload["corpus_identity"][key] for key in expected_corpus}
    require(actual_corpus == expected_corpus, "League dossiers do not bind the current competitive corpus")

    difficulty = operating_system["difficulty_contract"]
    require(difficulty["authored_setting"] == "Hard", "authored difficulty is not Hard")
    require(difficulty["medium_level_delta"] == -2 and difficulty["easy_level_delta"] == -4,
            "live difficulty deltas drifted")
    require(difficulty["observed_is_null_until_real_rom_playtest"] is True,
            "observed difficulty may be claimed without a playtest")

    philosophy = (ROOT / sources["philosophy"]).read_text()
    production = (ROOT / sources["production_contract"]).read_text()
    require("not a global team allocator" in philosophy, "philosophy lost advisory ownership")
    require("previous ten physical encounters" in production, "production contract lost previous-ten context")
    require("design-complete` must never be described as implemented" in production,
            "production contract permits status inflation")
    require("Exact source implementation continues from the unique sequence entry marked" in production,
            "production contract lost forward closure")

    print(f"Battle Design OS: {len(designs)} League anchors complete; source remains honestly unimplemented/unplayed")
    print(f"Chronological frontier: {len(closed)} closed, Battle {state['next_index']} next, {state['unordered_physical_groups']} physical groups still unordered")
    print("Backward design, forward closure, previous-ten context, competitive evidence, and advisory ownership are proven")
    print("Emerald Champions Battle Design Operating System release gate: PASS")


if __name__ == "__main__":
    main()
