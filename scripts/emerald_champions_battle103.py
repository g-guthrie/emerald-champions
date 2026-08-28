#!/usr/bin/env python3
"""Generate and verify Battle 103, Lenny's four-part mountain echo."""

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
        "level": 3,
        "species": "SPECIES_NOIVERN",
        "item": "ITEM_FOCUS_SASH",
        "ability_slot": 2,
        "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
        "moves": ["MOVE_TAILWIND", "MOVE_BOOMBURST", "MOVE_HURRICANE", "MOVE_DRAGON_PULSE"],
    },
    {
        "level": 2,
        "species": "SPECIES_EXPLOUD",
        "item": "ITEM_CHOICE_SPECS",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
        "moves": ["MOVE_BOOMBURST", "MOVE_FOCUS_BLAST", "MOVE_FIRE_BLAST", "MOVE_ICE_BEAM"],
    },
    {
        "level": 3,
        "species": "SPECIES_KOMMO_O",
        "item": "ITEM_ASSAULT_VEST",
        "ability_slot": 1,
        "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
        "moves": ["MOVE_CLOSE_COMBAT", "MOVE_POISON_JAB", "MOVE_ROCK_SLIDE", "MOVE_DRAGON_CLAW"],
    },
    {
        "level": 4,
        "species": "SPECIES_ALTARIA",
        "item": "ITEM_ALTARIANITE",
        "ability_slot": 0,
        "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
        "moves": ["MOVE_HYPER_VOICE", "MOVE_FIRE_BLAST", "MOVE_ROOST", "MOVE_PROTECT"],
    },
]

REFERENCES = [
    "showdown:gen6randomdoublesbattle:004",
    "showdown:gen7randomdoublesbattle:008",
    "showdown:gen7randomdoublesbattle:003",
    "showdown:gen9championsrandomdoublesbattle:004",
]

NEXT = {
    "index": 104,
    "encounter_id": "BATTLE_104_ROUTE_114_ANGELINA_LUCAS",
    "location": "Route114",
    "category": "optional final lower-route Picnicker and Hiker pair",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_ANGELINA", "TRAINER_LUCAS_1"],
    "access_note": (
        "Angelina at (26,72) and Lucas at (30,72) occupy the final lower Route 114 shelf. Their two physical "
        "trainer invocations are reviewed together because the player can engage them separately or from the shared lane."
    ),
}


def design() -> dict:
    return {
        "guide_order": 103,
        "trainer_ids": ["TRAINER_LENNY"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": (
            "Optional six-tile Hiker double at (15,65), after the Steve/Bernie Match Call families and immediately before "
            "the final Angelina/Lucas shelf. Full Center preparation, the leveler, legal-move teacher, free ordinary held "
            "items, and Mega access are available."
        ),
        "runtime_branches": [
            "Guarded four-member double when the player has at least two usable party members.",
            "Native refusal path when the player cannot legally field two Pokemon.",
        ],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 mature lower-mountain double",
            "effective_levels": "43, 42, 43, and 44",
            "eligible_ratio": "4/4",
            "mega_access": True,
            "status": "pass",
            "reason": (
                "Noivern evolves at 48 in the base games but the source permits rare mature exceptions after four badges; "
                "Exploud evolves at 40, Kommo-o at 45, and Altaria at 35. Noivern and Kommo-o are deliberate late-blooming "
                "mountain previews rather than a claim that every wild peer has already evolved."
            ),
        },
        "manual_quality": 10,
        "manual_difficulty": 9.3,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [
                {"reference_id": REFERENCES[0], "decision": "Noivern role selected; donor roster rejected", "reason": "The reproducible doubles set validates fast special pressure; Telepathy and Tailwind are local partner-safe adaptations."},
                {"reference_id": REFERENCES[1], "decision": "Exploud role selected; donor roster rejected", "reason": "The exact doubles set validates Boomburst, broad coverage, and bulky special offense."},
                {"reference_id": REFERENCES[2], "decision": "Kommo-o role selected; donor roster rejected", "reason": "The exact doubles set validates physical Fighting/Dragon pressure; Z-Move dependencies are removed."},
                {"reference_id": REFERENCES[3], "decision": "Mega Altaria set selected", "reason": "The Champions generator supplies the exact native Mega Hyper Voice, Roost, Protect, and speed-support shell."},
            ],
            "decision": (
                "All 1005 indexed references were available for review. Four exact-species doubles references support the "
                "sets; Lenny's echo ordering and mutual sound immunity are locally authored from his yodel dialogue."
            ),
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Noivern becomes the Focus Sash Tailwind lead and uses Telepathy so Exploud's Boomburst cannot hit it."},
            {"reference_id": REFERENCES[1], "adaptation": "Exploud keeps four-attack special coverage but uses Soundproof so Noivern's Boomburst cannot hit it."},
            {"reference_id": REFERENCES[2], "adaptation": "Kommo-o becomes the Assault Vest physical bridge; Soundproof preserves the echo immunity while Poison Jab covers Fairy answers."},
            {"reference_id": REFERENCES[3], "adaptation": "Mega Altaria preserves the exact Hyper Voice/Roost/Protect identity, with Fire Blast replacing redundant Tailwind coverage."},
        ],
        "ordering": {
            "lead": ["SPECIES_NOIVERN", "SPECIES_EXPLOUD"],
            "reserves": ["SPECIES_KOMMO_O", "SPECIES_ALTARIA"],
            "reason": (
                "The lead proves two-way partner immunity immediately. Soundproof Kommo-o is the physical bridge when either "
                "lead falls; source-last Mega Altaria is the cloud-top answering voice rather than a second opening gimmick."
            ),
        },
        "team_intent": (
            "Telepathy Focus Sash Noivern and Soundproof Choice Specs Exploud can both choose Boomburst without damaging one "
            "another; Noivern also owns the single Tailwind clock. Assault Vest Soundproof Kommo-o changes to a physical axis "
            "with Fighting, Poison, Rock, and Dragon coverage. Mega Altaria closes through Pixilate Hyper Voice, Fire coverage, "
            "Roost, and Protect. The roster uses one Mega, one speed field, no weather, no sleep, no trap, and no manual setup."
        ),
        "intended_counterplay": (
            "Wide Guard, Soundproof, priority, Fake Out, Taunt, Trick Room, opposing Tailwind, Sash chip, special bulk, and "
            "Choice-lock exploitation all attack the lead. Fairy pressure sharply checks Kommo-o; Steel, Poison, Ice, Rock, "
            "item removal, Encore, and focused damage answer Mega Altaria. Three special attackers make special walls valuable, "
            "while Kommo-o's physical coverage prevents one-category autopilot. No exact catch or scripted move order is required."
        ),
        "bespoke_ai": (
            "Lenny uses smart switching, partner awareness, HP awareness, Combo Setup, and Speed Control. Native foe/partner "
            "scoring understands Telepathy and Soundproof before selecting Boomburst, uses Tailwind only when speed state gains "
            "value, respects Choice Specs, Mega Evolves Altaria through standard trainer Mega logic, and uses Protect/Roost only "
            "from visible board state. No target, move, or switch is forced."
        ),
        "uniqueness": (
            "Noivern, Exploud, Kommo-o, and Altaria are new to the first 102 physical encounters; Mega Altaria is unspent and "
            "not protected by the anchor board. Toxtricity and every earlier sound-team species remain unused here. This is the "
            "first mutual Boomburst-immunity puzzle and the first Mega on Route 114, after nine straight encounters without one."
        ),
        "story_logic": (
            "Lenny remains a yodeling Hiker, but every line now describes the actual four voices and their echo rules. The "
            "post-battle explanation names Telepathy, Soundproof, Boomburst, Mega Altaria, Pixilate, and Hyper Voice. His long "
            "sight line, optional status, prize money, and no-reward flow remain unchanged."
        ),
        "reward_logic": "EXP and prize money only; Lenny grants no item, flag, Match Call registration, or progression reward.",
        "campaign_reservations": {
            "spends": ["first mutual Telepathy/Soundproof Boomburst lead", "first Kommo-o", "first Exploud", "first Noivern", "first Mega Altaria"],
            "preserves": ["all legendary sound identities", "Toxtricity rematch identities", "Kommo-o setup/Z-Move identities", "Winona's reserved Mega Pidgeot", "Drake's Dragon puzzle"],
            "repeat_rule": "These four species should not recur soon; later sound teams must change the immunity structure, format, or central win condition.",
        },
        "author_self_check": {
            "strongest_part": "The original yodel joke now communicates a mechanically exact four-voice battle whose lead can safely unleash two Boombursts.",
            "weakest_link": "Three special attackers can be compressed by special bulk. Choice lock, Wide Guard, and sound immunity are intentionally strong answers; the physical Kommo-o bridge, Tailwind, Mega closer, coverage, and +2 to +4 levels keep the encounter severe without hiding that seam.",
        },
        "closure": (
            "Battle 103 is source-closed at quality 10 and target difficulty 9.3: a guarded cap-40 double uses four legal "
            "cap+2 to +4 species, four distinct items, one unspent Mega, exact source ordering, partner-safe AI, four indexed "
            "references, native-width truthful dialogue, broad counterplay, and zero reward debt. Runtime remains unplayed."
        ),
    }


def ledger_entry() -> dict:
    return {
        "index": 103,
        "encounter_id": "BATTLE_103_ROUTE_114_LENNY",
        "identity": {
            "location": "Route114",
            "category": "optional lower-route Hiker double",
            "format": "guarded double",
            "strict_cap": 40,
            "memory_hook": "Telepathy Noivern and Soundproof Exploud trade safe Boombursts before Kommo-o and Mega Altaria answer the mountain echo.",
        },
        "primary_player_question": "Can the player break the mutual-immunity Boomburst lead before Tailwind, then change category and type answers for Kommo-o and Mega Altaria?",
        "tempo": "Four-part echo double: fast speed-control spread lead, slow Choice spread cannon, physical Soundproof bridge, then Pixilate Mega closer.",
        "pressure_sources": ["Focus Sash Telepathy Noivern Tailwind/Boomburst", "Choice Specs Soundproof Exploud Boomburst", "Assault Vest physical Soundproof Kommo-o", "Pixilate Mega Altaria Hyper Voice sustain"],
        "intentional_opening": "Noivern and Exploud are fixed together; each is immune to the other's Boomburst through a different ability.",
        "intentional_weakness": "Three special attackers, public Choice lock, one fragile Sash speed setter, Kommo-o's Fairy seam, Wide Guard/sound immunity, and no weather, sleep, trap, hazard, or manual setup.",
        "first_loss_lesson": "Do not race two safe spread attacks blindly: deny Tailwind or block sound, exploit Exploud's lock, then save Fairy and Steel/Poison/Ice pressure for the two reserves.",
        "revealed_information": ["cap 40", "guarded double", "levels 42-44", "mutual Telepathy/Soundproof Boomburst", "Tailwind", "Choice Specs", "physical Kommo-o", "Mega Altaria", "Pixilate Hyper Voice", "four fresh species", "no reward"],
        "counterplay_classes": ["Wide Guard/Soundproof", "Fake Out/Taunt/priority/Sash chip", "Trick Room or speed reversal", "special bulk and Choice exploitation", "Fairy into Kommo-o", "Steel/Poison/Ice/Rock into Altaria", "item removal/Encore/focused damage"],
        "target_difficulty": 9.3,
        "difficulty_rationale": "Four optimized levels 42-44, a partner-safe spread lead, Tailwind, one physical bridge, and a source-last Mega demand adaptation. Multiple broad immunity, speed, type, and item answers keep it below boss difficulty.",
        "tuning_knob": "Tune Mega Altaria +4 to +3 first, then Noivern/Kommo-o +3 to +2; preserve the lead, abilities, items, and four-part order.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["route-double", "hiker", "mountain-echo", "boomburst", "telepathy", "soundproof", "tailwind", "choice-specs", "noivern", "exploud", "kommo-o", "mega-altaria", "pixilate", "hyper-voice", "four-fresh-species", "no-weather", "no-sleep", "no-trap", "no-hazards", "no-manual-setup", "no-legendary"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Four exact doubles references; mutual echo ordering is local."},
        "author_self_check": {"strongest_part": "The yodel dialogue, lead abilities, spread moves, reserve ordering, and Mega climax all tell one story.", "weakest_link": "Special-bulk compression is real and deliberately answerable; Kommo-o and level pressure prevent it from solving every turn."},
    }


def expected_payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_103_ROUTE_114_LENNY"] = design()

    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 103] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])

    sequence = json.loads(SEQUENCE.read_text())
    for row in sequence["entries"]:
        if row["index"] == 103:
            row.update({
                "category": "optional lower-route Hiker echo double",
                "trainer_ids": ["TRAINER_LENNY"],
                "access_note": "Lenny faces right at (15,65) with six-tile sight. His guarded four-member double is the last solo trainer before the final lower shelf.",
            })
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 104] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        row["status"] = "closed" if row["index"] <= 103 else "next" if row["index"] == 104 else "queued"

    operating_system = json.loads(OS_PATH.read_text())
    operating_system["current_state"].update({
        "closed_encounters": 103,
        "next_index": 104,
        "next_encounter_id": NEXT["encounter_id"],
        "queued_sequence_entries": 0,
        "canonical_sequence_groups": 104,
        "physical_encounter_groups": 526,
        "unordered_physical_groups": 422,
    })
    return designs, ledger, sequence, operating_system


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    block = doubles.trainer_blocks(trainers)["TRAINER_LENNY"].group(0)
    actual = [polish.parse_entry(entry) for entry in custom.party_entries(doubles.party_match(parties, doubles.party_name(block)).group(2))]
    if actual != TEAM:
        raise SystemExit("FAIL: Battle 103 Lenny source party differs")
    for token in (".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_COMBO_SETUP", "AI_FLAG_SPEED_CONTROL"):
        if token not in block:
            raise SystemExit(f"FAIL: Battle 103 Lenny missing {token}")

    dex = presets.LocalDex()
    slots = doubles.base_ability_slots()
    for member in TEAM:
        illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
        if illegal:
            raise SystemExit(f"FAIL: Battle 103 illegal moves for {member['species']}: {illegal}")
        if member["ability_slot"] >= len(slots[member["species"]]):
            raise SystemExit(f"FAIL: Battle 103 invalid ability slot for {member['species']}")
    if len({member["species"] for member in TEAM}) != 4 or len({member["item"] for member in TEAM}) != 4:
        raise SystemExit("FAIL: Battle 103 species/items are not unique")

    route = (ROOT / "data/maps/Route114/scripts.inc").read_text()
    if "trainerbattle_double TRAINER_LENNY" not in route or "Route114_Text_LennyNotEnoughMons" not in route:
        raise SystemExit("FAIL: Battle 103 guarded double routing missing")
    obj = next(row for row in json.loads((ROOT / "data/maps/Route114/map.json").read_text())["object_events"] if row.get("script") == "Route114_EventScript_Lenny")
    if (obj["x"], obj["y"], obj["movement_type"], str(obj["trainer_sight_or_berry_tree_id"])) != (15, 65, "MOVEMENT_TYPE_FACE_RIGHT", "6"):
        raise SystemExit("FAIL: Battle 103 Lenny geometry drifted")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]["TRAINER_LENNY"]
    expected_manifest = {"format": "double", "target_size": 4, "archetype": "Four-part mountain echo", "difficulty": 93, "partner_interaction": True, "level_offset": 4, "location": "Route 114"}
    if manifest != expected_manifest:
        raise SystemExit("FAIL: Battle 103 manifest stale")

    dialogue = (ROOT / "data/text/trainers.inc").read_text().split("Route114_Text_LennyIntro:", 1)[1].split("Route114_Text_LucasIntro:", 1)[0]
    for cue in ("Four voices", "Noivern, Exploud, Kommo-o, Altaria", "Telepathy and Soundproof", "Boomburst safely", "Mega Altaria", "Pixilate Hyper Voice", "two ready Pokémon"):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 103 dialogue missing {cue}")
    for line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 103 overlong dialogue: {visible}")

    ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference_id not in ids for reference_id in REFERENCES):
        raise SystemExit("FAIL: Battle 103 competitive reference missing")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.write and not args.check:
        parser.error("choose --write or --check")

    payloads = expected_payloads()
    paths = (DESIGNS, LEDGER, SEQUENCE, OS_PATH)
    texts = [json.dumps(payload, indent=2, ensure_ascii=False) + "\n" for payload in payloads]
    if args.write:
        for path, text in zip(paths, texts):
            path.write_text(text)
    if args.check:
        for path, text in zip(paths, texts):
            if path.read_text() != text:
                raise SystemExit(f"FAIL: Battle 103 generated artifact stale: {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())
        entry = next(row for row in guide["entries"] if row["trainerId"] == "TRAINER_LENNY")
        if entry["designStatus"] != "closed" or entry["format"] != "double" or entry["partySize"] != 4:
            raise SystemExit("FAIL: Battle 103 guide stale")
    print("PASS: Battle 103 Lenny four-part mountain echo is source-closed")


if __name__ == "__main__":
    main()
