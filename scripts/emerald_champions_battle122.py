#!/usr/bin/env python3
"""Generate and verify Battle 122, Jeff's Contrary flashover double."""

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
    {"level": 2, "species": "SPECIES_SPINDA", "item": "ITEM_SITRUS_BERRY", "ability_slot": 2, "spread": "SPREAD_31_IV_HP_SPEED_TIMID", "moves": ["MOVE_SKILL_SWAP", "MOVE_FAKE_OUT", "MOVE_ICY_WIND", "MOVE_HELPING_HAND"]},
    {"level": 3, "species": "SPECIES_PYROAR", "item": "ITEM_IRON_BALL", "ability_slot": 1, "spread": "SPREAD_31_IV_HP_SPATK_MODEST", "moves": ["MOVE_OVERHEAT", "MOVE_HYPER_VOICE", "MOVE_DARK_PULSE", "MOVE_PROTECT"]},
    {"level": 4, "species": "SPECIES_CINDERACE", "item": "ITEM_EXPERT_BELT", "ability_slot": 2, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY", "moves": ["MOVE_PYRO_BALL", "MOVE_HIGH_JUMP_KICK", "MOVE_SUCKER_PUNCH", "MOVE_TAUNT"]},
    {"level": 5, "species": "SPECIES_CENTISKORCH", "item": "ITEM_ASSAULT_VEST", "ability_slot": 1, "spread": "SPREAD_31_IV_HP_ATK_ADAMANT", "moves": ["MOVE_FIRE_LASH", "MOVE_POWER_WHIP", "MOVE_LEECH_LIFE", "MOVE_KNOCK_OFF"]},
]

REFERENCES = [
    "showdown:gen7randomdoublesbattle:018",
    "showdown:gen9championsrandomdoublesbattle:021",
    "showdown:gen9randomdoublesbattle:001",
    "showdown:gen8randombattle:009",
]

NEXT = {
    "index": 123,
    "encounter_id": "BATTLE_123_LAVARIDGE_GYM_FLANNERY",
    "location": "LavaridgeTown_Gym_1F",
    "category": "required Heat Badge Gym Leader and rematch family",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_FLANNERY_1", "TRAINER_FLANNERY_2", "TRAINER_FLANNERY_3", "TRAINER_FLANNERY_4"],
    "access_note": "Flannery is the required Gym Leader at (13,9). Her physical source group owns the main fight and rematch family; the doubles/no-legends script additionally reaches TRAINER_FLANNERY_5.",
}


def design() -> dict:
    return {
        "guide_order": 122,
        "trainer_ids": ["TRAINER_JEFF"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": "Final optional buried B1F Kindler at (13,17), immediately before the route to Flannery.",
        "runtime_branches": ["One guarded four-member single-trainer double at levels 42-45."],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 mature final Gym drill",
            "effective_levels": "42-45",
            "eligible_ratio": "4/4",
            "mega_access": True,
            "status": "pass",
            "reason": "Spinda is single-stage; Pyroar evolves at 35; Cinderace at 35; Centiskorch at 28. No Mega or Primal is used.",
        },
        "manual_quality": 10,
        "manual_difficulty": 9.4,
        "observed_difficulty": None,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {"reference_id": REFERENCES[0], "decision": "Contrary Spinda role adapted", "reason": "The exact random-doubles set validates Contrary support and priority. Jeff replaces generic Superpower/Trick Room with a legal Skill Swap handoff, Fake Out, Icy Wind, and Helping Hand."},
                {"reference_id": REFERENCES[1], "decision": "Pyroar role adapted; Mega rejected", "reason": "The Champions doubles set validates Hyper Voice, special Fire pressure, and Protect. Jeff rejects Pyroarite and uses Iron Ball so the visible ability handoff resolves before Overheat."},
                {"reference_id": REFERENCES[2], "decision": "Cinderace role adapted", "reason": "The exact doubles set validates Pyro Ball, High Jump Kick, and Protect-family tempo. Libero, Sucker Punch, and Taunt create the fast physical transition without importing Court Change."},
                {"reference_id": REFERENCES[3], "decision": "Centiskorch role adapted", "reason": "The generated set validates Fire Lash, Power Whip, and Knock Off. Assault Vest, White Smoke, and Leech Life remove another Coil setup and make the last body an immediate physical anchor."},
            ],
            "decision": "Four exact-species references cover every role. No complete donor has this ability-transfer order, so the flashover sequence is transparently local rather than presented as a historic team.",
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Contrary Spinda becomes a fast one-action technician that can still Fake Out, slow both foes, or amplify its partner."},
            {"reference_id": REFERENCES[1], "adaptation": "Iron Ball Pyroar deliberately yields move order to Spinda, then turns Overheat's drawback into a two-stage gain after Skill Swap."},
            {"reference_id": REFERENCES[2], "adaptation": "Expert Belt Libero Cinderace changes the second act to fast physical coverage and priority."},
            {"reference_id": REFERENCES[3], "adaptation": "Assault Vest White Smoke Centiskorch closes with four immediate attacks and a four-times Rock seam."},
        ],
        "ordering": {
            "lead": ["SPECIES_SPINDA", "SPECIES_PYROAR"],
            "reserves": ["SPECIES_CINDERACE", "SPECIES_CENTISKORCH"],
            "reason": "Timid max-Speed Spinda moves before Iron Ball Pyroar, allowing a visible Contrary Skill Swap before Overheat. The reserves deliberately change from a special ability puzzle to two direct physical attackers.",
        },
        "team_intent": "Spinda can trade Contrary to Pyroar before its Overheat, turning a nominal drawback into escalating special pressure. The player may interrupt that handoff, remove either lead, exploit Pyroar's Iron Ball, or wait out a Protect. Cinderace and Centiskorch then abandon ability transfer for Libero coverage, priority, Fire Lash defense drops, item removal, and Grass/Bug coverage.",
        "primary_player_question": "Can the player interrupt or exploit the Contrary handoff, then switch from special containment to the correct physical answers for two unlike reserves?",
        "intended_counterplay": "Fake Out, faster Taunt, redirection, focus into Spinda, Skill Swap, Worry Seed, Gastro Acid, Protect, Haze, Clear Smog, Unaware, special bulk, Water/Ground/Rock/Fighting, hazards, priority, burn, Intimidate after White Smoke is identified, and accurate pressure into Cinderace all work. Spinda is frail, Skill Swap spends one action, Iron Ball exposes Pyroar's speed, Centiskorch is four-times weak to Rock, and there is no weather, room, terrain, Mega, legendary, redirection, or recovery move.",
        "first_loss_lesson": "Do not let the animation read as unexplained snowballing: stop Spinda or remove Contrary from Pyroar, then preserve Rock or fast special pressure for Centiskorch and a reliable answer to Libero Cinderace.",
        "bespoke_ai": "Jeff uses smart switching, partner awareness, HP awareness, and Combo Setup. Reusable AI rewards Contrary Skill Swap into a visible Overheat partner and rewards Overheat when the partner has selected that transfer; ordinary scoring still chooses the move and target. Iron Ball supplies deterministic move order without a forced turn. No action, switch, or target is scripted.",
        "uniqueness": "This is the campaign's first Contrary-to-Overheat transfer and the first time a deliberately slowing item enables an ally before empowering the recipient. Spinda showcases an underused species without weakening the battle. Three of four members are Fire type, while all four species have been absent for at least twenty encounters. It does not repeat the recent Gravity, Dancer, Flash Fire, Eruption, consumable-item, trap, or contact-deterrent engines.",
        "story_logic": "Jeff's old claim that flames are waiting to blaze becomes a literal delayed flashover. Intro and post-battle text name Contrary, Iron Ball move order, Skill Swap, Overheat, and both physical reserves rather than generic bravado.",
        "reward_logic": "Optional EXP and prize money only; Jeff sets only his native trainer flag and grants no item, heal, callback, or progression gate.",
        "campaign_reservations": {
            "spends": ["first Contrary-to-Overheat ability handoff", "underused Spinda technician", "final pre-Flannery flashover drill"],
            "preserves": ["Flannery's Torkoal-Lilligant After You sun", "Flannery's Delphox-Skeledirge slow mode", "Flannery's Heatran bridge", "Flannery's Mega Emboar", "Maxie's Mega Houndoom/Mega Malamar anchors"],
            "repeat_rule": "Do not repeat Contrary Skill Swap into a stat-dropping attack or Iron Ball as ally-order enablement soon.",
        },
        "author_self_check": {
            "strongest_part": "The support Pokémon, held item, move order, ability, dialogue, and Overheat animation all express the same one-turn flashover idea.",
            "weakest_link": "If Spinda is removed immediately, the opening becomes ordinary special Fire pressure. That is earned broad counterplay; two optimized cap-plus physical reserves keep the battle above the floor without hiding a second mandatory trick.",
        },
        "closure": "Battle 122 is source-closed at quality 10 and target difficulty 9.4: exact final-trainer topology, four legal cap+2 to +5 sets, deterministic handoff order, four indexed references, mostly Fire identity, reusable Contrary AI, truthful native-width dialogue, broad counterplay, and no reward debt are proven. Runtime remains unplayed.",
    }


def ledger_entry() -> dict:
    return {
        "index": 122,
        "encounter_id": "BATTLE_122_LAVARIDGE_GYM_JEFF",
        "identity": {"location": "LavaridgeTown_Gym_B1F", "category": "optional final buried Kindler", "format": "double", "strict_cap": 40, "memory_hook": "A fast Spinda hands Contrary to an Iron Ball Pyroar before two physical flames replace the special flashover."},
        "primary_player_question": "Can the Contrary handoff be interrupted or exploited before the player changes answers for Cinderace and Centiskorch?",
        "tempo": "Contrary Skill Swap and Overheat lead into two direct physical reserves.",
        "pressure_sources": ["Contrary Skill Swap", "Iron Ball move order", "Overheat snowball", "Fake Out/Icy Wind/Helping Hand", "Libero Cinderace", "White Smoke Assault Vest Centiskorch"],
        "intentional_opening": "Spinda and Iron Ball Pyroar lead; the AI values but never forces Skill Swap plus Overheat.",
        "intentional_weakness": "Frail support, one setup action, slow recipient, shared Water/Ground/Rock pressure, four-times Rock closer, no field/Mega/legend/recovery.",
        "first_loss_lesson": "Stop Spinda or strip Contrary, then preserve reliable fast pressure for Cinderace and Rock/special pressure for Centiskorch.",
        "revealed_information": ["cap 40", "double", "levels 42-45", "Contrary", "Skill Swap", "Iron Ball", "Overheat", "Libero", "White Smoke", "no reward"],
        "counterplay_classes": ["Fake Out/Taunt/redirection/focus", "ability removal or swap", "Haze/Clear Smog/Unaware", "Water/Ground/Rock/Fighting", "hazards/priority/burn/special bulk"],
        "target_difficulty": 9.4,
        "difficulty_rationale": "A deterministic but interruptible ability transfer creates a dangerous special lead, then two cap+4/+5 physical reserves demand a category change. No boss transformation or persistent field is spent.",
        "tuning_knob": "Lower Centiskorch +5 to +4 first, then Pyroar +3 to +2; preserve the transfer, Iron Ball, four species, and physical transition.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["lavaridge-gym", "final-gym-trainer", "contrary-flashover", "skill-swap-overheat", "spinda", "pyroar", "cinderace", "centiskorch", "iron-ball-ordering", "libero", "white-smoke", "three-of-four-fire", "no-weather", "no-room", "no-mega", "no-legendary"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Four exact-species generated references; local ability-transfer composition disclosed."},
        "author_self_check": {"strongest_part": "Every opening component communicates delayed ignition.", "weakest_link": "Removing Spinda simplifies the first act by design."},
    }


def payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_122_LAVARIDGE_GYM_JEFF"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 122] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])
    sequence = json.loads(SEQUENCE.read_text())
    for row in sequence["entries"]:
        if row["index"] == 122:
            row.update({
                "category": "optional final B1F Contrary-flashover double",
                "trainer_ids": ["TRAINER_JEFF"],
                "access_note": "Jeff is the standalone buried trainer at (13,17) and the final ordinary Lavaridge Gym encounter before Flannery.",
            })
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 123] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        row["status"] = "closed" if row["index"] <= 122 else "next" if row["index"] == 123 else "queued"
    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update({
        "closed_encounters": 122,
        "next_index": 123,
        "next_encounter_id": NEXT["encounter_id"],
        "canonical_sequence_groups": 123,
        "physical_encounter_groups": 522,
        "unordered_physical_groups": 399,
    })
    return designs, ledger, sequence, operating_system


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    block = doubles.trainer_blocks(trainers)["TRAINER_JEFF"].group(0)
    actual = [
        polish.parse_entry(entry)
        for entry in custom.party_entries(doubles.party_match(parties, doubles.party_name(block)).group(2))
    ]
    if actual != TEAM:
        raise SystemExit("FAIL Battle 122 source party")
    for fragment in (
        ".doubleBattle = TRUE",
        "AI_FLAG_HP_AWARE",
        "AI_FLAG_HELP_PARTNER",
        "AI_FLAG_SMART_SWITCHING",
        "AI_FLAG_COMBO_SETUP",
    ):
        if fragment not in block:
            raise SystemExit(f"FAIL Battle 122 trainer flag {fragment}")

    local_dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    for member in TEAM:
        illegal = [move for move in member["moves"] if move not in local_dex.legal_moves(member["species"])]
        if illegal or member["ability_slot"] >= len(slots[member["species"]]):
            raise SystemExit(f"FAIL Battle 122 legality {member['species']}: {illegal}")

    map_data = json.loads((ROOT / "data/maps/LavaridgeTown_Gym_B1F/map.json").read_text())
    event = next((row for row in map_data["object_events"] if row["script"] == "LavaridgeTown_Gym_B1F_EventScript_Jeff"), None)
    if event is None or (event["x"], event["y"]) != (13, 17) or event["trainer_type"] != "TRAINER_TYPE_BURIED":
        raise SystemExit("FAIL Battle 122 source topology")

    ai = (ROOT / "src/battle_ai_main.c").read_text()
    for fragment in (
        "AI_DATA->partnerMove == MOVE_SKILL_SWAP",
        "partnerAbility == ABILITY_CONTRARY",
        "effect == EFFECT_OVERHEAT",
        "AI_DATA->atkAbility == ABILITY_CONTRARY",
        "HasMove(battlerDef, MOVE_OVERHEAT)",
    ):
        if fragment not in ai:
            raise SystemExit(f"FAIL Battle 122 Contrary AI {fragment}")

    battle_util = (ROOT / "src/battle_util.c").read_text()
    battle_main = (ROOT / "src/battle_main.c").read_text()
    items = (ROOT / "src/data/items.h").read_text()
    if "HOLD_EFFECT_IRON_BALL" not in items or "holdEffect == HOLD_EFFECT_IRON_BALL" not in battle_main or "GetBattlerHoldEffect(battlerId, TRUE) == HOLD_EFFECT_IRON_BALL" not in battle_util:
        raise SystemExit("FAIL Battle 122 Iron Ball ordering mechanics")

    scripts = (ROOT / "data/maps/LavaridgeTown_Gym_1F/scripts.inc").read_text()
    dialogue = scripts.split("LavaridgeTown_Gym_B1F_Text_JeffIntro:", 1)[1].split("LavaridgeTown_Gym_B1F_Text_EliIntro:", 1)[0]
    for cue in ("Contrary", "Pyroar", "Overheat", "Iron Ball", "Skill Swap", "Cinderace", "Centiskorch"):
        if cue not in dialogue:
            raise SystemExit(f"FAIL Battle 122 dialogue {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL Battle 122 dialogue width {visible!r}")

    reference_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    missing = [reference for reference in REFERENCES if reference not in reference_ids]
    if missing:
        raise SystemExit(f"FAIL Battle 122 corpus references {missing}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    expected = payloads()
    paths = (DESIGNS, LEDGER, SEQUENCE, OS_PATH)
    serialized = [json.dumps(payload, indent=2, ensure_ascii=False) + "\n" for payload in expected]
    if args.write:
        for path, text in zip(paths, serialized):
            path.write_text(text)
    if args.check:
        for path, text in zip(paths, serialized):
            if path.read_text() != text:
                raise SystemExit(f"FAIL Battle 122 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())["entries"]
        row = next(entry for entry in guide if entry["trainerId"] == "TRAINER_JEFF")
        if row["designStatus"] != "closed" or row["partySize"] != 4:
            raise SystemExit("FAIL Battle 122 guide row")
    print("PASS: Battle 122 Jeff Contrary-flashover double is source-closed")


if __name__ == "__main__":
    main()
