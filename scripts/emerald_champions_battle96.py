#!/usr/bin/env python3
"""Generate and verify Battle 96, Kai and Charlotte's native pair."""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path

import verdant_battle_set_presets as presets
import verdant_custom_teams as custom
import verdant_doubles_conversion as doubles
import verdant_team_polish as polish


ROOT = Path(__file__).resolve().parents[1]
DESIGNS = ROOT / "docs/verdant_bespoke_battle_designs.json"
LEDGER = ROOT / "docs/verdant_battle_experience_ledger.json"
SEQUENCE = ROOT / "docs/verdant_battle_sequence.json"
OS_PATH = ROOT / "docs/emerald_champions_battle_design_operating_system.json"
CORPUS = ROOT / "docs/competitive_team_index.jsonl"

KAI_TEAM = [
    {
        "level": 1,
        "species": "SPECIES_WHISCASH",
        "item": "ITEM_RINDO_BERRY",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_HP_ATK_BRAVE",
        "moves": ["MOVE_EARTHQUAKE", "MOVE_WATERFALL", "MOVE_ROCK_SLIDE", "MOVE_PROTECT"],
    },
    {
        "level": 2,
        "species": "SPECIES_GRAPPLOCT",
        "item": "ITEM_EXPERT_BELT",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_HP_ATK_BRAVE",
        "moves": ["MOVE_OCTOLOCK", "MOVE_DRAIN_PUNCH", "MOVE_ICE_PUNCH", "MOVE_PROTECT"],
    },
    {
        "level": 4,
        "species": "SPECIES_WALREIN",
        "item": "ITEM_WEAKNESS_POLICY",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_HP_SPATK_QUIET",
        "moves": ["MOVE_SURF", "MOVE_ICE_BEAM", "MOVE_SUPER_FANG", "MOVE_PROTECT"],
    },
]

CHARLOTTE_TEAM = [
    {
        "level": 2,
        "species": "SPECIES_BEHEEYEM",
        "item": "ITEM_MENTAL_HERB",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_HP_SPATK_QUIET",
        "moves": ["MOVE_TRICK_ROOM", "MOVE_PSYCHIC", "MOVE_THUNDERBOLT", "MOVE_PROTECT"],
    },
    {
        "level": 1,
        "species": "SPECIES_KOMALA",
        "item": "ITEM_LIFE_ORB",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_HP_ATK_BRAVE",
        "moves": ["MOVE_SWORDS_DANCE", "MOVE_RETURN", "MOVE_KNOCK_OFF", "MOVE_SUCKER_PUNCH"],
    },
    {
        "level": 3,
        "species": "SPECIES_SHIINOTIC",
        "item": "ITEM_BIG_ROOT",
        "ability_slot": 1,
        "spread": "SPREAD_31_IV_HP_SPATK_QUIET",
        "moves": ["MOVE_SPORE", "MOVE_DREAM_EATER", "MOVE_MOONBLAST", "MOVE_STRENGTH_SAP"],
    },
]

REFERENCES = [
    "showdown:gen6randomdoublesbattle:022",
    "smogon:gen4nu:004",
    "showdown:gen5randomdoublesbattle:027",
    "showdown:gen7randomdoublesbattle:001",
]

NEXT = {
    "index": 97,
    "encounter_id": "BATTLE_097_ROUTE_114_CLAUDE",
    "location": "Route114",
    "category": "optional mid-route direct-interaction Fisherman single",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_CLAUDE"],
    "access_note": (
        "Claude stands at (19,26) with sight range zero immediately south-west of the Kai/Charlotte pond lane. "
        "He is the next north-to-south optional encounter before Nancy and the Tyra/Ivy pair."
    ),
}


def design() -> dict:
    return {
        "guide_order": 96,
        "trainer_ids": ["TRAINER_KAI", "TRAINER_CHARLOTTE"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Optional opposing sight lines at Route 114's east pond after Nolan. Kai and Charlotte can join as a "
            "six-member native double or be fought separately as two complete three-member singles."
        ),
        "runtime_branches": [
            "Kai+Charlotte native-pair double: Whiscash and Beheeyem lead, all six source members available.",
            "Kai split single: Whiscash, Grapploct, Walrein.",
            "Charlotte split single: Beheeyem, Komala, Shiinotic.",
        ],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 mature east-pond branch puzzle",
            "effective_levels": "Kai 41/42/44; Charlotte 42/41/43",
            "eligible_ratio": "6/6",
            "mega_access": True,
            "status": "pass",
            "reason": (
                "Whiscash evolves at 30; Grapploct evolves after learning Taunt; Walrein first becomes legal at its exact "
                "level 44 here; Beheeyem evolves at its exact level 42; Komala is single-stage; Shiinotic evolves at 24."
            ),
        },
        "manual_quality": 10,
        "manual_difficulty": 9.3,
        "branch_difficulty": {"joint_double": 9.3, "kai_single": 8.6, "charlotte_single": 8.5},
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {
                    "reference_id": REFERENCES[0],
                    "decision": "Whiscash role selected; full donor rejected",
                    "reason": "The reproducible doubles set validates Earthquake/Waterfall/Rock coverage, but this branch needs no rain or unrelated Mew/Dragonite shell.",
                },
                {
                    "reference_id": REFERENCES[1],
                    "decision": "Whiscash competitive legitimacy corroborated; full donor rejected",
                    "reason": "Published NU offense supports Whiscash as a real breaker rather than route filler.",
                },
                {
                    "reference_id": REFERENCES[2],
                    "decision": "Beheeyem coverage selected; full donor rejected",
                    "reason": "The reproducible set supports Psychic/Thunderbolt; local doubles evidence adds Telepathy and Trick Room.",
                },
                {
                    "reference_id": REFERENCES[3],
                    "decision": "Komala setup role selected; full donor rejected",
                    "reason": "The reproducible doubles set supplies Comatose, Swords Dance, Return, and a public setup plan.",
                },
            ],
            "decision": (
                "All 1005 references were reviewed. Four indexed donors establish Whiscash, Beheeyem, and Komala roles; "
                "the all-species doubles handbook supplies Walrein, Grapploct, and Shiinotic evidence. No complete donor "
                "matches the physical joint/split branch, so the six-member composition is transparently hand-authored."
            ),
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Whiscash keeps Earthquake/Waterfall/Rock pressure, trades Dragon Dance for Protect, and uses local Adaptability plus Rindo as a Trick Room-compatible lead."},
            {"reference_id": REFERENCES[1], "adaptation": "The published Whiscash breaker confirms that a familiar route fish can carry a serious competitive job."},
            {"reference_id": REFERENCES[2], "adaptation": "Psychic and Thunderbolt remain; handbook Telepathy/Trick Room turns Beheeyem into the legal joint-branch hinge."},
            {"reference_id": REFERENCES[3], "adaptation": "Komala keeps Comatose, Swords Dance, and Return, adding Knock Off and Sucker Punch for autonomous split and joint play."},
            {"source": "docs/battle_set_reviews/040_hoenn.json", "adaptation": "Walrein's Thick Fat bulky-control chassis becomes Surf/Super Fang/Policy pressure at its first legal evolution level."},
            {"source": "docs/battle_set_reviews/085_galar.json", "adaptation": "Grapploct keeps the source-backed Octolock, Drain Punch, Ice Punch, and Protect trap plan."},
            {"source": "docs/battle_set_reviews/080_alola.json", "adaptation": "Shiinotic's wall evidence becomes one finite Spore into Dream Eater/Strength Sap sustain line."},
        ],
        "ordering": {
            "joint_lead": ["SPECIES_WHISCASH", "SPECIES_BEHEEYEM"],
            "kai_source_order": [member["species"] for member in KAI_TEAM],
            "charlotte_source_order": [member["species"] for member in CHARLOTTE_TEAM],
            "reason": (
                "Beheeyem's Telepathy makes Kai's lead Earthquake safe and its Trick Room favors every slow member. "
                "Grapploct and Komala change to physical trap/setup pressure; the exact level-44 Walrein and sleep-feeding "
                "Shiinotic close each source half without depending on the other trainer."
            ),
        },
        "team_intent": (
            "The joint branch opens Adaptability Rindo Whiscash beside Mental Herb Telepathy Beheeyem. Earthquake is "
            "partner-safe and Trick Room flips six naturally slow bodies. Grapploct introduces Octolock sustain; Komala's "
            "Comatose creates a status-immune Swords Dance threat; first-legal Walrein turns common weakness pressure into "
            "a Policy decision while Surf remains Telepathy-safe; Big Root Shiinotic creates one public Spore, Dream Eater, "
            "and Strength Sap loop. In either split, every member retains four useful moves and a complete win condition."
        ),
        "intended_counterplay": (
            "Taunt after Mental Herb, Imprison, reversing Trick Room, fast focus into Beheeyem, priority, or Protect-stalling "
            "the five turns answer speed control. Grass pressure breaks Whiscash after its one Rindo; Flying/Psychic/Fairy, "
            "Ghost immunity, pivoting, or phazing answer Octolock; burn/Intimidate and setup denial answer Komala; Grass, "
            "Electric, Fighting, Rock, item removal, or non-super-effective damage avoid gifting Walrein's Policy; Grass "
            "types, Overcoat, Safety Goggles, terrain, Lum, Taunt, or immediate focus answer Shiinotic. No precise catch or "
            "single scripted turn is required."
        ),
        "bespoke_ai": (
            "Both records remain native singles so every physical branch works. Kai uses smart switching, partner awareness, "
            "and HP awareness; Charlotte adds Combo Setup, Speed Control, and Field Control. Existing AI treats Telepathy "
            "Earthquake/Surf as safe, sets Trick Room only when the speed board benefits, values Protect and Sucker Punch "
            "normally, uses Octolock against switchable targets, and attempts Spore only against legal awake targets. No "
            "target, turn, item proc, switch, or Trick Room action is forced."
        ),
        "uniqueness": (
            "All six species are new to the first 95 encounters and absent from every protected marquee anchor. This is the "
            "first native pair whose cross-trainer hinge is simultaneously Telepathy-safe Earthquake/Surf and Trick Room, "
            "and the first rolling-window sleep puzzle after ten explicitly sleep-free encounters. It follows Nolan's one-slot "
            "Water mechanic with six active bodies, field reversal, trap, setup, Policy, and sleep rather than repeating Commander."
        ),
        "story_logic": (
            "Kai's rewritten fisherman dialogue names three slow giant catches and Octolock. Charlotte's existing cute-quirk "
            "identity now names Beheeyem's room, Komala's Comatose, and Shiinotic's dream theft. Post-battle text explains the "
            "actual joint synergies without claiming the other trainer is required. Neither trainer owns a reward, rematch, or story flag."
        ),
        "reward_logic": "EXP and prize money only; both optional records own no item, shop, legendary, Mega Stone, rematch, or progression reward.",
        "campaign_reservations": {
            "spends": ["first Telepathy plus Trick Room native pair", "Whiscash/Grapploct/Walrein giant-catch trio", "Beheeyem/Komala/Shiinotic cute-quirk trio", "first rolling-window Spore/Dream Eater lesson"],
            "preserves": ["Hatterene and Aromatisse Trick Room anchors", "Wailord faction anchor", "Araquanid Juan anchor", "every Mega/legendary", "weather and Tailwind speed modes"],
            "repeat_rule": "These six species should not recur soon; future Trick Room teams must change the setter, payoff, and physical branch structure.",
        },
        "author_self_check": {
            "strongest_part": "The sight-line double is meaningfully stronger than concatenation—Telepathy and Trick Room change Kai's exact moves—while both split singles remain complete.",
            "weakest_link": "Both halves are intentionally slow and the joint leans on one Trick Room setter. Mental Herb buys one attempt, but focus, reversal, priority, and five-turn stalling are broad; +1 to +4 levels preserve difficulty if the field is denied.",
        },
        "closure": (
            "Battle 96 is source-closed at quality 10 and target difficulty 9.3: every joint/split branch is indexed; six "
            "fresh, unreserved, legal mature species appear at levels 41-44 with six distinct items; exact AI, source ordering, "
            "geometry, four indexed competitive references, three handbook roles, native-width dialogue, broad field/status/"
            "type counterplay, and zero reward debt are proven. Runtime playtesting remains required before difficulty is observed."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 96,
        "encounter_id": "BATTLE_096_ROUTE_114_KAI_CHARLOTTE",
        "identity": {
            "location": "Route114",
            "category": "optional east-pond native-pair cluster",
            "format": "native-pair double or two split singles",
            "strict_cap": 40,
            "memory_hook": "Telepathy Whiscash and Trick Room Beheeyem join three giant catches to three cute quirks; split approaches retain two complete singles.",
        },
        "primary_player_question": "Can the player deny or reverse the one Trick Room hinge, then change answers across Octolock, Comatose setup, Weakness Policy, and Spore without assuming both trainers were engaged?",
        "tempo": "Six-member joint Trick Room double with Telepathy-safe spread, trap, status-immune setup, Policy, and sleep; or two autonomous three-member singles.",
        "pressure_sources": [
            "level-41 Rindo Adaptability Whiscash Earthquake lead",
            "level-42 Mental Herb Telepathy Beheeyem Trick Room lead",
            "level-42 Expert Belt Octolock Grapploct",
            "level-41 Life Orb Comatose Komala setup",
            "level-44 Weakness Policy Thick Fat Walrein with Surf/Super Fang",
            "level-43 Big Root Effect Spore Shiinotic with Spore/Dream Eater/Strength Sap",
        ],
        "intentional_opening": "Joint opens Whiscash+Beheeyem; splits open the same source-first member. Mental Herb protects one Trick Room attempt, not a forced success.",
        "intentional_weakness": "One field setter, finite five-turn room, shared slow pace, Whiscash's 4x Grass seam, Grapploct's modest bulk, Komala setup turn, avoidable Policy, and many sleep immunities/items.",
        "first_loss_lesson": "The two sight lines changed the board. Stop or reverse Beheeyem's room before trading into slow pressure, and bring an explicit sleep answer for source-last Shiinotic.",
        "revealed_information": ["cap 40", "joint and two split branches", "levels 41-44", "Telepathy Earthquake/Surf", "Mental Herb Trick Room", "Octolock", "Comatose", "Weakness Policy", "Spore plus Dream Eater", "six fresh species", "no reward/rematch"],
        "counterplay_classes": ["Taunt/Imprison/Trick Room reversal/focus", "priority and Protect stalling", "Grass into Whiscash", "burn/Intimidate/setup denial", "pivot/phazing/Ghost immunity around Octolock", "Policy avoidance/item removal", "Grass/Overcoat/Goggles/terrain/Lum/Taunt into Spore"],
        "target_difficulty": 9.3,
        "difficulty_rationale": "The joint has six optimized fresh levels 41-44 and five distinct pressure modes, while each split has three complete optimized members. One field setter and broad typed/status/item answers keep the puzzle severe but learnable.",
        "tuning_knob": "Tune Walrein +4 to +3 first, then Shiinotic +3 to +2; preserve species, source ordering, Telepathy/Trick Room hinge, items, and all physical branches.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["route-cluster", "native-pair-double", "split-singles", "trick-room", "telepathy-spread", "whiscash", "grapploct", "walrein", "beheeyem", "komala", "shiinotic", "octolock", "comatose", "weakness-policy", "spore", "dream-eater", "six-fresh-species", "no-weather", "no-mega", "no-legendary"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Four indexed Whiscash/Beheeyem/Komala references plus exact all-species handbook reviews for Walrein, Grapploct, and Shiinotic."},
        "author_self_check": {"strongest_part": "The native pair creates real cross-trainer mechanics while each split remains a full battle.", "weakest_link": "One Trick Room setter is focusable; levels, Mental Herb, priority, trap, and autonomous reserves compensate without hiding the answer."},
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_096_ROUTE_114_KAI_CHARLOTTE"] = design()

    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 96] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])

    sequence = json.loads(SEQUENCE.read_text())
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 97] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        if row["index"] <= 96:
            row["status"] = "closed"
        elif row["index"] == 97:
            row["status"] = "next"
        else:
            row["status"] = "queued"

    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update(
        {
            "closed_encounters": 96,
            "next_index": 97,
            "next_encounter_id": NEXT["encounter_id"],
            "queued_sequence_entries": 0,
            "canonical_sequence_groups": 97,
            "physical_encounter_groups": 527,
            "unordered_physical_groups": 430,
        }
    )
    return designs, ledger, sequence, operating_system


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    blocks = doubles.trainer_blocks(trainers)
    for trainer_id, expected_team, required_flags in (
        ("TRAINER_KAI", KAI_TEAM, ("AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE")),
        ("TRAINER_CHARLOTTE", CHARLOTTE_TEAM, ("AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_COMBO_SETUP", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL")),
    ):
        block_text = blocks[trainer_id].group(0)
        body = doubles.party_match(parties, doubles.party_name(block_text)).group(2)
        actual = [polish.parse_entry(entry) for entry in custom.party_entries(body)]
        if actual != expected_team:
            raise SystemExit(f"FAIL: Battle 96 source party differs for {trainer_id}")
        if ".doubleBattle = FALSE" not in block_text:
            raise SystemExit(f"FAIL: Battle 96 {trainer_id} is not native-pair compatible")
        for flag in required_flags:
            if flag not in block_text:
                raise SystemExit(f"FAIL: Battle 96 {trainer_id} missing {flag}")

    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    for member in KAI_TEAM + CHARLOTTE_TEAM:
        illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
        if illegal:
            raise SystemExit(f"FAIL: Battle 96 illegal moves for {member['species']}: {illegal}")
        if member["ability_slot"] >= len(slots[member["species"]]):
            raise SystemExit(f"FAIL: Battle 96 invalid ability slot for {member['species']}")
    if len({member["species"] for member in KAI_TEAM + CHARLOTTE_TEAM}) != 6:
        raise SystemExit("FAIL: Battle 96 species are not unique")
    if len({member["item"] for member in KAI_TEAM + CHARLOTTE_TEAM}) != 6:
        raise SystemExit("FAIL: Battle 96 items are not unique")

    map_data = json.loads((ROOT / "data/maps/Route114/map.json").read_text())["object_events"]
    geometry = {
        row["script"]: (row["x"], row["y"], row["movement_type"], str(row["trainer_sight_or_berry_tree_id"]))
        for row in map_data
        if row.get("script") in {"Route114_EventScript_Kai", "Route114_EventScript_Charlotte"}
    }
    expected_geometry = {
        "Route114_EventScript_Kai": (28, 16, "MOVEMENT_TYPE_FACE_DOWN_AND_LEFT", "3"),
        "Route114_EventScript_Charlotte": (28, 20, "MOVEMENT_TYPE_FACE_UP", "3"),
    }
    if geometry != expected_geometry:
        raise SystemExit("FAIL: Battle 96 native-pair geometry drifted")

    route_script = (ROOT / "data/maps/Route114/scripts.inc").read_text()
    for trainer_id in ("TRAINER_KAI", "TRAINER_CHARLOTTE"):
        if f"trainerbattle_single {trainer_id}" not in route_script:
            raise SystemExit(f"FAIL: Battle 96 split source missing {trainer_id}")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]
    expected_manifest = {
        "TRAINER_KAI": {"format": "single", "target_size": 3, "archetype": "Giant slow catches", "difficulty": 86, "partner_interaction": True, "level_offset": 2, "location": "Route 114"},
        "TRAINER_CHARLOTTE": {"format": "single", "target_size": 3, "archetype": "Cute Trick Room quirks", "difficulty": 85, "partner_interaction": True, "level_offset": 2, "location": "Route 114"},
    }
    for trainer_id, expected in expected_manifest.items():
        if manifest[trainer_id] != expected:
            raise SystemExit(f"FAIL: Battle 96 manifest stale for {trainer_id}")

    dialogue = (ROOT / "data/text/trainers.inc").read_text().split("Route114_Text_KaiIntro:", 1)[1].split("Route114_Text_AngelinaIntro:", 1)[0]
    for cue in ("three giants", "move slow", "Octolock", "cute team", "twists the room", "never wakes", "steals", "Telepathy", "Comatose", "Dream Eater"):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 96 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 96 overlong dialogue: {visible}")

    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in corpus_ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 96 competitive reference missing")
    for path, cue in (
        ("docs/battle_set_reviews/040_hoenn.json", "Walrein uses Thick Fat"),
        ("docs/battle_set_reviews/085_galar.json", "Grapploct while Octolock"),
        ("docs/battle_set_reviews/080_alola.json", "Shiinotic Physical Wall"),
    ):
        if cue not in (ROOT / path).read_text():
            raise SystemExit(f"FAIL: Battle 96 handbook evidence missing from {path}")

    protected = "\n".join(
        path.read_text()
        for path in list((ROOT / "docs").glob("emerald_champions_*anchor_designs.json"))
        + list((ROOT / "docs/dossier_packets").glob("*.json"))
    )
    for species in ("Whiscash", "Walrein", "Grapploct", "Beheeyem", "Komala", "Shiinotic"):
        if re.search(rf'"{species}"', protected):
            raise SystemExit(f"FAIL: Battle 96 spends protected anchor species {species}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.write and not args.check:
        parser.error("choose --write or --check")

    payloads = expected_payloads()
    paths = (DESIGNS, LEDGER, SEQUENCE, OS_PATH)
    expected_text = [json.dumps(payload, indent=2, ensure_ascii=False) + "\n" for payload in payloads]
    if args.write:
        for path, text in zip(paths, expected_text):
            path.write_text(text)
    if args.check:
        for path, text in zip(paths, expected_text):
            if path.read_text() != text:
                raise SystemExit(f"FAIL: Battle 96 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())
        entries = [row for row in guide["entries"] if row["trainerId"] in {"TRAINER_KAI", "TRAINER_CHARLOTTE"}]
        if len(entries) != 2 or any(row["designStatus"] != "closed" for row in entries):
            raise SystemExit("FAIL: Battle 96 guide status stale")
        if {row["partySize"] for row in entries} != {3}:
            raise SystemExit("FAIL: Battle 96 guide parties stale")
    print("PASS: Battle 96 Kai/Charlotte joint and split branches are source-closed")


if __name__ == "__main__":
    main()
