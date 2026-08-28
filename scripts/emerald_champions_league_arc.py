#!/usr/bin/env python3
"""Generate and verify the connected Elite Four and Wallace attrition arc."""

from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DESIGNS = ROOT / "docs/verdant_marquee_battle_designs.json"
COLLISIONS = ROOT / "docs/verdant_marquee_collision_report.json"
OUTPUT_JSON = ROOT / "docs/emerald_champions_league_attrition_arc.json"
OUTPUT_MD = ROOT / "docs/emerald_champions_league_attrition_arc.md"

ORDER = [
    "ELITE_FOUR_SIDNEY",
    "ELITE_FOUR_PHOEBE",
    "ELITE_FOUR_GLACIA",
    "ELITE_FOUR_DRAKE",
    "CHAMPION_WALLACE",
]

CARRY_FORWARD = {
    "ELITE_FOUR_SIDNEY": [
        "Direct HP loss and faints",
        "Will-O-Wisp burn",
        "Revive and healing inventory pressure before Phoebe",
    ],
    "ELITE_FOUR_PHOEBE": [
        "Direct HP loss and faints from the Perish and cleanup phases",
        "Spore sleep and Will-O-Wisp burn",
        "Awakening, Full Heal, Revive, and recovery pressure before Glacia",
    ],
    "ELITE_FOUR_GLACIA": [
        "Direct HP loss and faints",
        "Toxic status, Infestation switching pressure, and item disruption",
        "Status, held-item, and preserved anti-Ice resource pressure before Drake",
    ],
    "ELITE_FOUR_DRAKE": [
        "Direct HP loss and faints",
        "Stealth Rock and forced-switch damage during the duel",
        "Pressure on the special wall, priority, Fairy, Ice, and hazard answers preserved for Wallace",
    ],
    "CHAMPION_WALLACE": [
        "Final conversion of remaining healing, weather, speed-control, spread-mitigation, Taunt, and physical coverage resources",
        "No later main-story battle is reserved; this is the attrition payoff",
    ],
}

COLLISION_DISPOSITIONS = [
    {
        "advisory": "SIGNATURE_MOVE_REPETITION",
        "disposition": "retain",
        "reason": "Repeated common competitive moves occupy materially different jobs: Tailwind is Sidney's midgame Dark tempo, Drake's visible singles handoff, and Wallace's lead speed mode; Icy Wind is Darkrai control, Iron Bundle control, and a conditional non-Trick-Room Milotic option; Knock Off belongs to position theft, trap transition, and proactive slow-mode pressure. Close Combat, Shadow Ball, Taunt, and U-turn are role-appropriate coverage or utility rather than repeated primary questions.",
    },
    {
        "advisory": "HISTORIC_REFERENCE_REUSE",
        "disposition": "retain",
        "reason": "The repeated references are used for different evidence. Sidney imports positioning and a single Kingambit endgame from Turin; Phoebe uses only survival-clock lessons. The shared random-doubles record informs Glacia's mode discipline and Drake's differentiated role sequencing, not an exact duplicated roster or core.",
    },
    {
        "advisory": "MODE_CLUSTER",
        "disposition": "retain-with-implementation-review",
        "reason": "Only Wallace owns active rain. Choice pressure is limited to one readable commitment in Sidney, Glacia, and Drake. Fast control appears in four anchors but at different moments and with different answers: a contestable middle Tailwind, active Icy Wind, a sacrificial singles handoff, and a lead Tailwind that may be reversed by Trick Room. Recheck the final Victory Road previous-ten window before implementation.",
    },
]


def load(path: Path) -> dict:
    return json.loads(path.read_text())


def build() -> dict:
    payload = load(DESIGNS)
    designs = payload["designs"]
    collision = load(COLLISIONS)
    mechanics = payload["mechanics_baselines"]["pokemon_league_main_story"]
    phases = []

    for position, anchor_id in enumerate(ORDER, 1):
        dossier = designs[anchor_id]
        phases.append({
            "position": position,
            "anchor_id": anchor_id,
            "trainer_ids": dossier["runtime"]["trainer_ids"],
            "format": dossier["runtime"]["canonical_format"],
            "cap": dossier["campaign_state"]["strict_cap"],
            "level_offsets": [mon["level_offset"] for mon in dossier["team"]],
            "lead": dossier["ordering"]["intended_lead"],
            "team": [
                {
                    "species": mon["species"],
                    "item": mon["item"],
                    "ability": mon["ability"],
                    "moves": mon["moves"],
                    "role": mon["role"],
                    "mega_candidate": mon["mega_candidate"],
                }
                for mon in dossier["team"]
            ],
            "memory_hook": dossier["identity"]["memory_hook"],
            "primary_question": dossier["identity"]["primary_player_question"],
            "strongest_part": dossier["author_self_check"]["strongest_part"],
            "weakest_link": dossier["author_self_check"]["weakest_link"],
            "first_loss_lesson": dossier["counterplay"]["first_loss_lesson"],
            "broad_counterplay": dossier["counterplay"]["classes"],
            "target_difficulty": dossier["difficulty"]["target"],
            "observed_difficulty": dossier["difficulty"]["observed"],
            "resource_tax": dossier["difficulty"]["resource_tax"],
            "carry_forward": CARRY_FORWARD[anchor_id],
            "ai_must_execute": dossier["ai"]["custom_requirements"],
            "selected_reference_ids": dossier["competitive_research"]["selected_reference_ids"],
            "spends": dossier["campaign_reservations"]["spends"],
            "preserves": dossier["campaign_reservations"]["preserves"],
            "guide_summary": dossier["presentation"]["guide_summary"],
            "status": dossier["status"],
        })

    return {
        "version": 1,
        "title": "Emerald Champions main-story League attrition arc",
        "status": {
            "design": "design-complete",
            "source": "unimplemented",
            "static": "design-validated",
            "runtime": "unplayed",
        },
        "mechanics": {
            "strict_cap": mechanics["strict_cap"],
            "authored_difficulty": mechanics["authored_difficulty_setting"],
            "medium_level_delta": mechanics["medium_trainer_level_delta"],
            "easy_level_delta": mechanics["easy_trainer_level_delta"],
            "automatic_healing_between_members": mechanics["automatic_healing_between_members"],
            "manual_overworld_bag_between_members": mechanics["manual_overworld_bag_between_members"],
            "pokemon_menu_between_members": mechanics["pokemon_menu_between_members"],
            "save_menu_between_members": mechanics["save_menu_between_members"],
            "items_during_trainer_battles": mechanics["items_during_trainer_battles"],
            "party_composition_locked_after_entry": mechanics["party_composition_locked_after_entry"],
            "allowed_battle_transformations": mechanics["allowed_battle_transformations"],
            "forbidden_battle_transformations": mechanics["forbidden_battle_transformations"],
        },
        "arc_question": "Can one fixed six-Pokemon party survive five distinct near-impossible questions by spending carried recovery deliberately and preserving the right strategic classes for later rooms?",
        "arc_shape": [
            "Sidney steals position and turns visible losses into one final king.",
            "Phoebe changes the resource from tempo to turns, exits, sleep, burn, and survival.",
            "Glacia punishes static play with a readable detonation, trapping, item loss, and a late snowball.",
            "Drake changes pace to a singles preservation duel with hazards, one Choice lock, and a visible Mega handoff.",
            "Wallace cashes out the surviving field-control and physical answers through fast rain, one denyable reversal, and Mega Milotic.",
        ],
        "phases": phases,
        "collision_review": {
            "hard_errors": collision["hard_errors"],
            "advisories": collision["advisories"],
            "dispositions": COLLISION_DISPOSITIONS,
            "unique_species": collision["species_count"],
            "unique_mega_species": sorted(collision["mega_species"]),
        },
        "completion_truth": "The connected arc and all five exact teams are design-complete and statically legal. Game source, exact final dialogue, guide replacement, AI state machines, previous-ten implementation context, real-ROM behavior, and observed difficulty remain explicitly open until chronological implementation.",
    }


def markdown(payload: dict) -> str:
    lines = [
        "# Emerald Champions main-story League attrition arc",
        "",
        f"Status: design `{payload['status']['design']}`, source `{payload['status']['source']}`, runtime `{payload['status']['runtime']}`.",
        "",
        "## Confirmed native rules",
        "",
        f"- Strict cap: {payload['mechanics']['strict_cap']} on Hard.",
        f"- Live difficulty: Medium {payload['mechanics']['medium_level_delta']}, Easy {payload['mechanics']['easy_level_delta']} opposing levels only.",
        "- No automatic healing between League members.",
        "- Manual Bag, Pokémon, held-item, and Save access returns between rooms.",
        "- Bag items cannot be used during trainer battles.",
        "- Party composition is locked after League entry.",
        "- Mega Evolution and intentional Primal Reversion are supported; Tera, Z-Moves, Dynamax, and Gigantamax are not.",
        "",
        "## Arc question",
        "",
        payload["arc_question"],
        "",
        "## Five-stage shape",
        "",
    ]
    lines.extend(f"{index}. {text}" for index, text in enumerate(payload["arc_shape"], 1))
    for phase in payload["phases"]:
        lines.extend([
            "",
            f"## {phase['position']}. {phase['anchor_id']}",
            "",
            f"- Format: {phase['format']}; cap {phase['cap']}; offsets {phase['level_offsets']}.",
            f"- Primary question: {phase['primary_question']}",
            f"- Strongest part: {phase['strongest_part']}",
            f"- Weakest link: {phase['weakest_link']}",
            f"- First-loss lesson: {phase['first_loss_lesson']}",
            f"- Resource tax: {phase['resource_tax']}",
            f"- References: {', '.join(f'`{ref}`' for ref in phase['selected_reference_ids'])}",
            "- Exact team:",
        ])
        for mon in phase["team"]:
            mega = "; Mega" if mon["mega_candidate"] else ""
            lines.append(
                f"  - `{mon['species']}` — `{mon['item']}`, `{mon['ability']}`{mega}; "
                + ", ".join(f"`{move}`" for move in mon["moves"])
            )
    lines.extend(["", "## Collision dispositions", ""])
    for row in payload["collision_review"]["dispositions"]:
        lines.append(f"- **{row['advisory']} — {row['disposition']}:** {row['reason']}")
    lines.extend(["", "## Completion truth", "", payload["completion_truth"], ""])
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.write and not args.check:
        parser.error("choose --write or --check")
    payload = build()
    expected_json = json.dumps(payload, indent=2, ensure_ascii=False) + "\n"
    expected_md = markdown(payload)
    if args.write:
        OUTPUT_JSON.write_text(expected_json)
        OUTPUT_MD.write_text(expected_md)
    if args.check:
        if not OUTPUT_JSON.exists() or OUTPUT_JSON.read_text() != expected_json:
            raise SystemExit("FAIL: League attrition JSON is missing or stale")
        if not OUTPUT_MD.exists() or OUTPUT_MD.read_text() != expected_md:
            raise SystemExit("FAIL: League attrition Markdown is missing or stale")
        if payload["collision_review"]["hard_errors"]:
            raise SystemExit("FAIL: League attrition arc has hard collision errors")
        if len(payload["phases"]) != 5 or any(phase["target_difficulty"] != 10 for phase in payload["phases"]):
            raise SystemExit("FAIL: League attrition arc is incomplete or not target 10")
        if any(phase["observed_difficulty"] is not None for phase in payload["phases"]):
            raise SystemExit("FAIL: unplayed League battle claims observed difficulty")
        species = [mon["species"] for phase in payload["phases"] for mon in phase["team"]]
        if len(species) != len(set(species)):
            raise SystemExit("FAIL: League arc repeats a species without a written exception")
    print("PASS: five exact League teams form one source-honest connected attrition arc")
    print("PASS: 30 unique species, 5 unique Megas, 5 distinct primary questions, and 0 hard collisions")


if __name__ == "__main__":
    main()
