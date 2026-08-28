#!/usr/bin/env python3
"""Generate/check Battle 92, Lao's first identity-and-type deception single."""

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

TEAM = [
    {
        "level": 1,
        "species": "SPECIES_ZOROARK",
        "item": "ITEM_BLACK_GLASSES",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
        "moves": ["MOVE_NIGHT_DAZE", "MOVE_FLAMETHROWER", "MOVE_FOCUS_BLAST", "MOVE_SLUDGE_BOMB"],
    },
    {
        "level": 2,
        "species": "SPECIES_GRENINJA",
        "item": "ITEM_LIFE_ORB",
        "ability_slot": 2,
        "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
        "moves": ["MOVE_HYDRO_PUMP", "MOVE_ICE_BEAM", "MOVE_DARK_PULSE", "MOVE_U_TURN"],
    },
    {
        "level": 3,
        "species": "SPECIES_CINDERACE",
        "item": "ITEM_WIDE_LENS",
        "ability_slot": 2,
        "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
        "moves": ["MOVE_PYRO_BALL", "MOVE_HIGH_JUMP_KICK", "MOVE_IRON_HEAD", "MOVE_U_TURN"],
    },
]

REFERENCES = [
    "showdown:gen8randomdoublesbattle:008",
    "smogon:gen7ou:001",
    "showdown:gen9randombattle:003",
]

NEXT = {
    "index": 93,
    "encounter_id": "BATTLE_093_ROUTE_113_DILLON",
    "location": "Route113",
    "category": "optional west-ash Youngster double",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_DILLON"],
    "access_note": (
        "Dillon is the next westbound physical encounter after buried Lao: a Youngster at (21,11), facing "
        "down/left with three-tile sight. His source record forces a four-member double but the map script is "
        "currently a single opcode, so Battle 93 must verify and repair the one-Pokemon guard path."
    ),
}


def design() -> dict:
    return {
        "guide_order": 92,
        "trainer_ids": ["TRAINER_LAO_1"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Optional buried Ninja Boy single at (29,6), after the twins and before Dillon. This dossier closes Lao's "
            "first reachable battle and native Match Call registration, while later rematch tiers remain reserved."
        ),
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 mature ninja deception single",
            "effective_levels": "41, 42, and 43",
            "eligible_ratio": "3/3",
            "mega_access": True,
            "status": "pass",
            "reason": "Zoroark, Greninja, and Cinderace are all natural final forms by this campaign phase.",
        },
        "manual_quality": 10,
        "manual_difficulty": 8.6,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {
                    "reference_id": reference_id,
                    "decision": "selected exact role; full donor rejected",
                    "reason": (
                        "The exact public set supplies one identity/type-changing attacker. Lao's source-last Illusion "
                        "ordering and three-member no-setup progression are hand-authored."
                    ),
                }
                for reference_id in REFERENCES
            ],
            "decision": (
                "Three exact competitive references support Illusion, Protean, and Libero roles. Setup, Protect, "
                "Mat Block, hazards, priority spam, and Z-Move dependencies are removed."
            ),
        },
        "competitive_references": [
            {
                "reference_id": REFERENCES[0],
                "adaptation": "Illusion Zoroark keeps special Dark/Fire pressure but drops Nasty Plot, Protect, and recovery for four immediate legal attacks.",
            },
            {
                "reference_id": REFERENCES[1],
                "adaptation": "Protean Greninja retains Life Orb Hydro Pump/Ice Beam pressure, adding legal Dark Pulse and one U-turn without hazards.",
            },
            {
                "reference_id": REFERENCES[2],
                "adaptation": "Libero Cinderace retains Pyro Ball/High Jump Kick and receives Wide Lens plus legal Iron Head/U-turn instead of Court Change.",
            },
        ],
        "ordering": {
            "intended_lead": ["SPECIES_ZOROARK"],
            "source_order": ["SPECIES_ZOROARK", "SPECIES_GRENINJA", "SPECIES_CINDERACE"],
            "illusion_target": "SPECIES_CINDERACE",
            "reason": (
                "The engine scans backward for the last living non-egg reserve, so lead Zoroark appears as source-last "
                "Cinderace. Greninja then reveals Protean before the real Libero Cinderace arrives."
            ),
        },
        "team_intent": (
            "Black Glasses Illusion Zoroark presents Cinderace's sprite while attacking specially through Dark/Fire/"
            "Fighting/Poison coverage. Life Orb Protean Greninja changes its own defensive type with Water/Ice/Dark or "
            "pivots. Wide Lens Libero Cinderace closes physically with more reliable Pyro Ball/High Jump Kick, Steel "
            "coverage, or U-turn. There is no setup, field, recovery, or status clock."
        ),
        "intended_counterplay": (
            "Do not commit a Cinderace-specific answer until the first damaging hit breaks Illusion. Fairy/Fighting/Bug "
            "and special bulk answer Zoroark; priority, Life Orb recoil, Electric/Grass/Fighting/Fairy by current Protean "
            "type, and strong neutral attacks answer Greninja; Water/Ground/Rock, Intimidate, burn, recoil, and Libero "
            "type tracking answer Cinderace. Hazards punish the two U-turns."
        ),
        "bespoke_ai": (
            "Lao uses smart switching and HP awareness. Illusion is automatic from exact source order. Protean and Libero "
            "activate through the native pre-move type hook; the AI scores the resulting STAB and matchup normally. Life "
            "Orb, Wide Lens, recoil, U-turn, and revealed current typing are public. No sleep, Toxic, explosion, setup, "
            "Protect, priority chain, evasion, or hidden input read is present."
        ),
        "uniqueness": (
            "Greninja and Cinderace are new to the first 91 closed encounters. Zoroark last appeared 45 battles ago as "
            "a museum cleanup reserve; here it is source-first and deliberately disguised as a physical Fire ace. Battle "
            "2 taught juvenile Illusion 90 encounters earlier, making this the mature identity-and-type exam."
        ),
        "story_logic": (
            "Lao's buried ash ambush now has a real deception. Intro and post-battle text truthfully hint that the first "
            "face is false and name Illusion's target, Protean, and Libero. Registration remains native; later rematch "
            "dialogue and teams remain separate work."
        ),
        "reward_logic": (
            "EXP, prize money, and native Match Call registration only. No item reward. Later Lao rematches remain "
            "explicitly unclaimed by this first-battle closure."
        ),
        "campaign_reservations": {
            "spends": [
                "mature Illusion lesson",
                "Protean-to-Libero type relay",
                "Zoroark disguised as Cinderace",
            ],
            "preserves": [
                "Mega Greninja",
                "Battle Bond Greninja",
                "setup Illusion teams",
                "Lao rematch escalation",
                "other mature starter showcases",
            ],
            "repeat_rule": (
                "These exact three should not recur soon. Later identity teams must alter source order and defensive "
                "questions; Lao rematches must evolve this deception rather than restore Toxic/explosion filler."
            ),
        },
        "author_self_check": {
            "strongest_part": (
                "The buried NPC, source order, visible false ace, and two type-changing reserves all express one idea "
                "without a custom menu or opaque script."
            ),
            "weakest_link": (
                "All three are fast offensive Pokemon with limited longevity. +1/+2/+3 levels, three items, broad "
                "coverage, Illusion tempo, and type changes create difficulty, while priority/hazards/recoil remain "
                "healthy broad counterplay."
            ),
        },
        "closure": (
            "Battle 92 is source-closed at quality 10 and target difficulty 8.6: three legal levels 41-43, three distinct "
            "items, proven source-last Illusion target, two fresh species, one distant role-changed repeat, exact native "
            "abilities, three competitive references, Match Call routing, rewritten width-safe dialogue, broad "
            "counterplay, and no sleep/Toxic/explosion/setup/Protect debt. Runtime remains unplayed; rematches stay reserved."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 92,
        "encounter_id": "BATTLE_092_ROUTE_113_LAO",
        "identity": {
            "location": "Route113",
            "category": "optional buried Ninja Boy first battle",
            "format": "single",
            "strict_cap": 40,
            "memory_hook": (
                "A Zoroark wearing Cinderace's face opens from the ash, then Protean Greninja and the real Libero "
                "Cinderace change type with every attack."
            ),
        },
        "primary_player_question": (
            "Can the player withhold an ace-specific commitment until Illusion breaks, then track Protean and Libero "
            "types rather than attacking yesterday's weakness?"
        ),
        "tempo": (
            "Three-member no-setup offensive single: one false identity, one special type-changing Life Orb pivot, and "
            "one physical Wide Lens type-changing closer."
        ),
        "pressure_sources": [
            "level-41 Black Glasses Illusion Zoroark disguised as Cinderace",
            "level-42 Life Orb Protean Greninja",
            "level-43 Wide Lens Libero Cinderace",
            "twelve immediate coverage moves across special and physical axes",
        ],
        "intentional_opening": "Zoroark is fixed and copies the last living reserve, source-last Cinderace.",
        "intentional_weakness": (
            "Three frail offensive bodies, no recovery/setup/field/Protect, Life Orb and attack recoil, hazard-sensitive "
            "U-turns, and public type changes after each selected move."
        ),
        "first_loss_lesson": (
            "The first Cinderace was not Cinderace. Break Illusion with a safe neutral line, then re-evaluate weakness "
            "after every Protean or Libero activation instead of repeating one attack."
        ),
        "revealed_information": [
            "cap 40",
            "levels 41-43",
            "intentional single",
            "source-last Cinderace Illusion target",
            "Protean and Libero",
            "two fresh species",
            "native Match Call registration",
            "later rematches reserved",
        ],
        "counterplay_classes": [
            "safe neutral Illusion break",
            "priority and hazards",
            "special bulk into Zoroark/Greninja",
            "Intimidate/burn into Cinderace",
            "Life Orb/recoil and Choice-free pivot reads",
            "current-type tracking",
        ],
        "target_difficulty": 8.6,
        "difficulty_rationale": (
            "Three optimized levels 41-43, one false identity, two native type changers, full items, and mixed coverage "
            "meet the serious-contender floor. Frailty, recoil, no setup/field, and public post-move typing keep it fair."
        ),
        "tuning_knob": "Tune Cinderace from +3 to +2 first; preserve source order, abilities, species, and all three items.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": [
            "route-single", "ninja", "illusion", "protean", "libero", "false-ace", "source-order", "mixed-offense",
            "no-sleep", "no-toxic", "no-explosion", "no-setup", "no-protect", "no-mega", "no-legendary",
            "match-call-first-battle",
        ],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {
            "status": "complete-current-review",
            "pool_size": 1005,
            "selection": "Three exact competitive identities recomposed around proven local Illusion source order.",
        },
        "author_self_check": {
            "strongest_part": "The battle's deception is visible, source-proven, and mechanically unified.",
            "weakest_link": "Frail offense can fold to priority/hazards; that is the intended fairness valve.",
        },
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_092_ROUTE_113_LAO"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [entry for entry in ledger["entries"] if entry["index"] != 92] + [ledger_entry()]
    ledger["entries"].sort(key=lambda entry: entry["index"])
    sequence = json.loads(SEQUENCE.read_text())
    sequence["entries"] = [entry for entry in sequence["entries"] if entry["index"] != 93] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda entry: entry["index"])
    for entry in sequence["entries"]:
        if entry["index"] <= 92:
            entry["status"] = "closed"
        elif entry["index"] == 93:
            entry["status"] = "next"
        else:
            entry["status"] = "queued"
    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update(
        {
            "closed_encounters": 92,
            "next_index": 93,
            "next_encounter_id": "BATTLE_093_ROUTE_113_DILLON",
            "queued_sequence_entries": 0,
            "canonical_sequence_groups": 93,
            "physical_encounter_groups": 529,
            "unordered_physical_groups": 436,
        }
    )
    return designs, ledger, sequence, operating_system


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    block = doubles.trainer_blocks(trainers)["TRAINER_LAO_1"].group(0)
    body = doubles.party_match(parties, doubles.party_name(block)).group(2)
    actual = [polish.parse_entry(entry) for entry in custom.party_entries(body)]
    if actual != TEAM:
        raise SystemExit("FAIL: Battle 92 source party differs")
    for token in (".doubleBattle = FALSE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HP_AWARE"):
        if token not in block:
            raise SystemExit(f"FAIL: Battle 92 missing {token}")

    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    for member in TEAM:
        illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
        if illegal:
            raise SystemExit(f"FAIL: Battle 92 illegal moves for {member['species']}: {illegal}")
        if member["ability_slot"] >= len(slots[member["species"]]):
            raise SystemExit(f"FAIL: Battle 92 invalid ability slot for {member['species']}")
    for forbidden in ("MOVE_PROTECT", "MOVE_HYPNOSIS", "MOVE_TOXIC", "MOVE_EXPLOSION", "MOVE_SELF_DESTRUCT"):
        if forbidden in body:
            raise SystemExit(f"FAIL: Battle 92 retained forbidden move {forbidden}")

    illusion = (ROOT / "src/battle_util.c").read_text()
    for token in (
        "for (i = PARTY_SIZE - 1; i >= 0; i--)",
        "GetMonData(&party[id], MON_DATA_HP)",
        "&party[id] != mon",
    ):
        if token not in illusion:
            raise SystemExit(f"FAIL: Battle 92 Illusion ordering proof missing {token}")
    if TEAM[-1]["species"] != "SPECIES_CINDERACE" or TEAM[0]["species"] != "SPECIES_ZOROARK":
        raise SystemExit("FAIL: Battle 92 Illusion source order drifted")
    type_hook = (ROOT / "src/battle_script_commands.c").read_text()
    if "ABILITY_PROTEAN || GetBattlerAbility(gBattlerAttacker) == ABILITY_LIBERO" not in type_hook:
        raise SystemExit("FAIL: Battle 92 Protean/Libero hook missing")

    scripts = (ROOT / "data/maps/Route113/scripts.inc").read_text()
    for token in (
        "trainerbattle_single TRAINER_LAO_1",
        "register_matchcall TRAINER_LAO_1",
        "Route113_EventScript_RematchLao",
    ):
        if token not in scripts:
            raise SystemExit(f"FAIL: Battle 92 routing missing {token}")
    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]["TRAINER_LAO_1"]
    if manifest != {
        "format": "single",
        "target_size": 3,
        "archetype": "Identity-and-type deception",
        "difficulty": 86,
        "partner_interaction": False,
        "level_offset": 2,
        "location": "Route 113",
    }:
        raise SystemExit("FAIL: Battle 92 format manifest stale")

    dialogue_file = (ROOT / "data/text/trainers.inc").read_text()
    dialogue = dialogue_file.split("Route113_Text_LaoIntro:", 1)[1].split("Route113_Text_LungIntro:", 1)[0]
    for cue in (
        "first face you see is false",
        "Zoroark wears Cinderace's face",
        "Protean",
        "Libero",
        "PokéNav registration",
    ):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 92 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 92 overlong dialogue: {visible}")

    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in corpus_ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 92 competitive reference missing from corpus")


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
                raise SystemExit(f"FAIL: Battle 92 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())
        entry = next(row for row in guide["entries"] if row["trainerId"] == "TRAINER_LAO_1")
        if entry["designStatus"] != "closed" or entry["format"] != "single":
            raise SystemExit("FAIL: Battle 92 guide status/format stale")
        if [member["speciesId"] for member in entry["party"]] != [member["species"] for member in TEAM]:
            raise SystemExit("FAIL: Battle 92 guide party stale")
    print("PASS: Battle 92 Lao first-battle identity deception single is source-closed")


if __name__ == "__main__":
    main()
