#!/usr/bin/env python3
"""Generate and verify Battle 116, Lucy's Lavaridge engineered-odds preview."""
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
FRONTIER = ROOT / "docs/emerald_champions_frontier_brain_designs.json"

TEAM = [
    {"level": 1, "species": "SPECIES_GALVANTULA", "item": "ITEM_FOCUS_SASH", "ability_slot": 0, "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID", "moves": ["MOVE_THUNDER", "MOVE_BUG_BUZZ", "MOVE_ENERGY_BALL", "MOVE_STICKY_WEB"]},
    {"level": 2, "species": "SPECIES_AMBIPOM", "item": "ITEM_NORMAL_GEM", "ability_slot": 0, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY", "moves": ["MOVE_FAKE_OUT", "MOVE_DOUBLE_HIT", "MOVE_KNOCK_OFF", "MOVE_U_TURN"]},
    {"level": 2, "species": "SPECIES_CINCCINO", "item": "ITEM_WIDE_LENS", "ability_slot": 2, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY", "moves": ["MOVE_TAIL_SLAP", "MOVE_BULLET_SEED", "MOVE_ROCK_BLAST", "MOVE_KNOCK_OFF"]},
    {"level": 3, "species": "SPECIES_DRAPION", "item": "ITEM_SCOPE_LENS", "ability_slot": 1, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY", "moves": ["MOVE_CROSS_POISON", "MOVE_NIGHT_SLASH", "MOVE_ICE_FANG", "MOVE_PROTECT"]},
    {"level": 3, "species": "SPECIES_CLEFABLE", "item": "ITEM_LIFE_ORB", "ability_slot": 1, "spread": "SPREAD_31_IV_HP_SPATK_MODEST", "moves": ["MOVE_MOONBLAST", "MOVE_DAZZLING_GLEAM", "MOVE_FLAMETHROWER", "MOVE_THUNDERBOLT"]},
    {"level": 4, "species": "SPECIES_ABSOL", "item": "ITEM_ABSOLITE", "ability_slot": 1, "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY", "moves": ["MOVE_SUCKER_PUNCH", "MOVE_KNOCK_OFF", "MOVE_PLAY_ROUGH", "MOVE_PROTECT"]},
]

REFERENCES = [
    "showdown:gen7randomdoublesbattle:018",
    "showdown:gen6randomdoublesbattle:025",
    "showdown:gen8randomdoublesbattle:024",
    "showdown:gen5randomdoublesbattle:011",
    "showdown:gen6randomdoublesbattle:005",
    "showdown:gen6randomdoublesbattle:022",
    "smogon:gen7uu:007",
    "elite:wolfe:indianapolis-2026",
]

NEXT = {
    "index": 117,
    "encounter_id": "BATTLE_117_LAVARIDGE_GYM_COLE_GERALD",
    "location": "LavaridgeTown_Gym_1F",
    "category": "optional buried first-chamber Cole/Gerald native pair",
    "status": "next",
    "strict_cap": 40,
    "trainer_ids": ["TRAINER_COLE", "TRAINER_GERALD"],
    "access_note": "Cole at (3,14) and Gerald at (2,15) are adjacent buried trainers in the first lower Gym chamber; source explicitly identifies Gerald as Cole's double partner. Their continue-script routing can produce the native pair or either separately.",
}


def design() -> dict:
    return {
        "guide_order": 116,
        "trainer_ids": ["TRAINER_LUCY_LAVARIDGE"],
        "status": "closed",
        "strict_cap": 40,
        "campaign_point": "First Lavaridge Pokémon Center entry before the Gym. A coordinate trigger at (2,2) summons Lucy when VAR_LAVARIDGE_LUCY_STATE is zero; the player may accept, refuse and retry by direct interaction, lose and retry, or win and retry the three-Bottle-Cap reward if the Bag is full.",
        "runtime_branches": ["Automatic entrance plus YES starts the guarded six-member double.", "NO moves Lucy beside the PC and preserves direct-interaction retry.", "Fewer than two healthy Pokémon refuses safely without setting the defeat flag.", "Loss preserves the challenge.", "Victory with Bag space grants exactly three Bottle Caps, sets state 2, and removes Lucy.", "Victory with a full Bag leaves state 1 and the defeated trainer available for reward retry without another battle."],
        "evolution_stage_fit": {
            "campaign_phase": "cap-40 named Frontier Brain preview with one legal Mega",
            "effective_levels": "41, 42, 42, 43, 43, and 44",
            "eligible_ratio": "6/6",
            "mega_access": True,
            "status": "pass",
            "reason": "Galvantula, Ambipom, Cinccino, Drapion, and Clefable are naturally mature by these levels; Absol is single-stage and Mega access has existed since cap 20. Exactly one Absolite is used.",
        },
        "manual_quality": 10,
        "manual_difficulty": 10,
        "corpus_review": {
            "reference_pool_size": 1005,
            "full_team_candidates": [{"reference_id": reference, "decision": "role or reservation evidence selected; full donor rejected", "reason": "Each reference supports one engineered-odds mechanic or protects Lucy's future Pike identity; the six-member campaign preview is locally authored."} for reference in REFERENCES],
            "decision": "All 1005 references, six authored species reviews, and Lucy's protected postgame Brain dossier were checked. Showdown and Smogon supply exact roles; Wolfe's Mega Steelix roster is retained only as evidence for the future Pike battle, not copied here.",
        },
        "competitive_references": [
            {"reference_id": REFERENCES[0], "adaptation": "Compound Eyes Galvantula keeps accurate Thunder and Bug Buzz; Focus Sash buys one public Sticky Web attempt."},
            {"reference_id": REFERENCES[1], "adaptation": "Technician Ambipom keeps Fake Out and U-turn, with one Normal Gem burst and direct Knock Off/Double Hit fallbacks."},
            {"reference_id": REFERENCES[2], "adaptation": "Skill Link Cinccino keeps deterministic multihit coverage; Wide Lens improves accuracy rather than adding another raw power item."},
            {"reference_id": REFERENCES[3], "adaptation": "Drapion's physical coverage is recast as Scope Lens Sniper pressure with two public high-critical attacks."},
            {"reference_id": REFERENCES[4], "adaptation": "Magic Guard Clefable validates recoil-free Life Orb pressure, adapted to four attacks rather than a passive screen/redirection loop."},
            {"reference_id": REFERENCES[5], "adaptation": "Random doubles confirms Absolite and mixed offensive legitimacy; local Absol uses a clean physical priority closer."},
            {"reference_id": REFERENCES[6], "adaptation": "Published Mega Absol offense validates Absolite, Knock Off, and Sucker Punch while retired Z-Move/Tera assumptions remain absent."},
            {"reference_id": REFERENCES[7], "adaptation": "Wolfe's Mega Steelix evidence remains reserved for postgame Lucy's six-serpent Pike team; no species or Mega is imported into this preview."},
        ],
        "ordering": {
            "lead": ["SPECIES_GALVANTULA", "SPECIES_AMBIPOM"],
            "reserves": ["SPECIES_CINCCINO", "SPECIES_DRAPION", "SPECIES_CLEFABLE", "SPECIES_ABSOL"],
            "reason": "Fake Out creates a visible chance for Sash Galvantula to establish Web or attack. Deterministic Skill Link and Sniper odds occupy the middle; Magic Guard removes recoil variance; Mega Absol closes by reflecting careless status or hazards. Smart switching may respond to the board without changing the lead lesson.",
        },
        "team_intent": "Lucy does not pray for favorable rolls—she engineers them. Compound Eyes raises accuracy, Technician fixes low-power moves, Skill Link fixes hit count, Sniper rewards public critical moves, Magic Guard fixes Life Orb cost, and Mega Absol turns opposing status/hazards back through Magic Bounce. Every probability has a visible item or ability and direct fallback.",
        "primary_player_question": "Can the player break Lucy's Fake Out/Sash lead before Web, then identify which deterministic odds layer is active without feeding the final Mega Absol a reflected status or hazard turn?",
        "intended_counterplay": "Protect, Inner Focus, priority, multihit, Wide Guard, Defog/Rapid Spin, Magic Bounce, burn, Intimidate, Rocky Helmet, physical walls, Ground, Fire, Rock, Fighting, Fairy, Bug, Steel, Poison, Taunt, item removal, Choice/priority baiting, and focused damage all answer different layers. Galvantula depends on Sash, Ambipom spends its Gem, Cinccino lacks a power item, Drapion has no setup, Clefable is slow, and Mega Absol is frail with common weaknesses.",
        "bespoke_ai": "Lucy uses smart switching, partner awareness, HP awareness, Field Control, and Combo Setup. Native AI values Fake Out from board state, establishes Sticky Web only when useful, recognizes Skill Link, Sniper, Magic Guard, items, priority, and coverage, and evaluates Absol as Mega Absol before move/switch decisions. Native trainer logic Mega Evolves Absol on its first legal attack; no opener, Web turn, critical, switch, target, or Mega turn is scripted.",
        "uniqueness": "All six species are new to the first 115 closed encounters and absent from every protected campaign and Frontier anchor. This is the first engineered-probability team. It explicitly avoids Seviper, Arbok, Serperior, Sandaconda, Zygarde, and Mega Steelix so postgame Lucy retains the definitive six-serpent Pike identity.",
        "story_logic": "Lucy now says she stacks odds rather than trusting luck, permits preparation and retry, names the six loaded chances, acknowledges a player-created win, awards the actual three Bottle Caps, and foreshadows that the Pike will test chance with a different team. No postgame serpent reveal is spoiled.",
        "reward_logic": "Three Bottle Caps remain a finite, retry-safe pre-Gym reward. Space is checked before the gift, state 2 is set only after successful delivery, and a full Bag cannot lose or duplicate the reward.",
        "campaign_reservations": {
            "spends": ["Lavaridge engineered-odds Lucy preview", "Compound Eyes Galvantula", "Technician Ambipom", "Skill Link Cinccino", "Sniper Drapion", "Magic Guard Clefable", "Mega Absol"],
            "preserves": ["postgame Lucy's six serpents", "Zygarde reveal", "Mega Steelix", "Wolfe's exact Mega Steelix reference", "all other Frontier Brain identities"],
            "repeat_rule": "Do not repeat the exact six engineered-odds layers or Mega Absol closer. The postgame Lucy must use her already protected serpent roster instead of upgrading this team.",
        },
        "author_self_check": {
            "strongest_part": "Every member expresses Lucy's luck language through a different public mechanic, while her true Pike identity remains completely protected.",
            "weakest_link": "Sticky Web plus Fake Out is conventional. The later fixed-hit, critical, recoil, and reflection layers—and the one-time Sash/Gem costs—make that familiar opening a readable entry to a novel full fight rather than the whole puzzle.",
        },
        "closure": "Battle 116 is source-closed at quality 10 and target difficulty 10: every trigger/refusal/retry/loss/reward branch, guarded six-member double, six legal levels 41-44, six fresh unreserved species and distinct items, one Mega, eight indexed Showdown/Smogon/Wolfe references, protected Pike reconciliation, retry-safe Bottle Caps, native-width dialogue, broad counterplay, and zero story/reward debt are proven. Runtime remains unplayed.",
    }


def ledger_entry() -> dict:
    return {
        "index": 116,
        "encounter_id": "BATTLE_116_LAVARIDGE_POKECENTER_LUCY",
        "identity": {"location": "LavaridgeTown_PokemonCenter_1F", "category": "optional pre-Gym Frontier Brain preview", "format": "guarded six-member double", "strict_cap": 40, "memory_hook": "Lucy loads six different odds, then Mega Absol reflects the player's attempt to cheat chance."},
        "primary_player_question": "Can the player break Fake Out plus Sash before Web, then answer fixed accuracy, hits, criticals, recoil, and Magic Bounce without treating them as random luck?",
        "tempo": "Fake Out/Web odds lead, deterministic multihit and critical middle, recoil-free special bridge, then Mega Absol reflection closer.",
        "pressure_sources": ["Compound Eyes Sash Galvantula", "Normal Gem Technician Fake Out Ambipom", "Wide Lens Skill Link Cinccino", "Scope Lens Sniper Drapion", "Magic Guard Life Orb Clefable", "Magic Bounce Mega Absol"],
        "intentional_opening": "Galvantula plus Ambipom is fixed; Fake Out may buy Web or direct Thunder/Bug pressure but is never forced.",
        "intentional_weakness": "No weather, terrain, room, redirection, healing loop, legendary, or second transformation; every member exposes an item, speed, type, or bulk seam and the lead spends Sash/Gem resources once.",
        "first_loss_lesson": "Lucy removes variance rather than relying on it: break the enabler for each fixed outcome, clear Web, and stop using status/hazards carelessly when Mega Absol appears.",
        "revealed_information": ["cap 40", "optional named double", "levels 41-44", "six engineered odds mechanics", "one Mega", "three Bottle Caps", "future Pike team differs"],
        "counterplay_classes": ["Protect/Inner Focus/priority/multihit", "Wide Guard/Defog/Rapid Spin/Magic Bounce", "burn/Intimidate/Helmet/physical walls", "Ground/Fire/Rock/Fighting/Fairy/Bug/Steel/Poison", "Taunt/item removal/priority baiting/focused damage"],
        "target_difficulty": 10,
        "difficulty_rationale": "A named six-member cap-plus-one-to-four optional Brain preview layers public, deterministic mechanics and one Mega. Each layer is independently answerable, but the full sequence demands adaptation worthy of target 10.",
        "tuning_knob": "Reduce Mega Absol +4 to +3 first, then Clefable/Drapion +3 to +2; preserve species, items, lead, reward, and future-Pike separation.",
        "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": ["lavaridge", "frontier-brain-preview", "engineered-odds", "galvantula", "ambipom", "cinccino", "drapion", "clefable", "mega-absol", "compound-eyes", "technician", "skill-link", "sniper", "magic-guard", "magic-bounce", "six-fresh-species", "one-mega", "no-legendary"],
        "historic_reference_ids": REFERENCES,
        "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": "Seven role references plus Wolfe's protected future-Pike evidence."},
        "author_self_check": {"strongest_part": "Luck becomes six visible mechanics rather than hidden RNG.", "weakest_link": "The familiar Web opening is only the doorway to the full odds progression."},
    }


def payloads() -> tuple[dict, dict, dict, dict]:
    designs = json.loads(DESIGNS.read_text())
    designs["designs"]["BATTLE_116_LAVARIDGE_POKECENTER_LUCY"] = design()
    ledger = json.loads(LEDGER.read_text())
    ledger["entries"] = [row for row in ledger["entries"] if row["index"] != 116] + [ledger_entry()]
    ledger["entries"].sort(key=lambda row: row["index"])
    sequence = json.loads(SEQUENCE.read_text())
    for row in sequence["entries"]:
        if row["index"] == 116:
            row.update({"category": "optional pre-Gym Pokémon Center Frontier Brain preview", "trainer_ids": ["TRAINER_LUCY_LAVARIDGE"], "access_note": "The first walk to (2,2) at state zero triggers Lucy's entrance and yes/no challenge. Refusal and insufficient-party branches preserve direct retry; victory grants three retry-safe Bottle Caps and removes her."})
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != 117] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        row["status"] = "closed" if row["index"] <= 116 else "next" if row["index"] == 117 else "queued"
    os_data = json.loads(OS_PATH.read_text())
    os_data["current_state"].update({"closed_encounters": 116, "next_index": 117, "next_encounter_id": NEXT["encounter_id"], "queued_sequence_entries": 0, "canonical_sequence_groups": 117, "physical_encounter_groups": 523, "unordered_physical_groups": 406})
    return designs, ledger, sequence, os_data


def protected_campaign_species() -> set[str]:
    protected: set[str] = set()
    for path in ROOT.glob("docs/emerald_champions_*anchor_designs.json"):
        for anchor in json.loads(path.read_text()).get("designs", {}).values():
            for member in anchor.get("team", []):
                if isinstance(member, dict) and isinstance(member.get("species"), str):
                    protected.add(member["species"])
    return protected


def verify_source() -> None:
    trainers = (ROOT / "src/data/trainers.h").read_text()
    parties = (ROOT / "src/data/trainer_parties.h").read_text()
    block = doubles.trainer_blocks(trainers)["TRAINER_LUCY_LAVARIDGE"].group(0)
    actual = [polish.parse_entry(entry) for entry in custom.party_entries(doubles.party_match(parties, doubles.party_name(block)).group(2))]
    if actual != TEAM:
        raise SystemExit("FAIL: Battle 116 party")
    for token in (".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER", "AI_FLAG_HP_AWARE", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_COMBO_SETUP"):
        if token not in block:
            raise SystemExit(f"FAIL: Battle 116 trainer missing {token}")
    if len({member["species"] for member in TEAM}) != 6 or len({member["item"] for member in TEAM}) != 6 or sum(member["item"] == "ITEM_ABSOLITE" for member in TEAM) != 1:
        raise SystemExit("FAIL: Battle 116 party invariants")
    dex = presets.LocalDex()
    ability_slots = doubles.base_ability_slots()
    for member in TEAM:
        illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
        if illegal or member["ability_slot"] >= len(ability_slots[member["species"]]):
            raise SystemExit(f"FAIL: Battle 116 legality {member['species']} {illegal}")
    if {member["species"] for member in TEAM} & protected_campaign_species():
        raise SystemExit("FAIL: Battle 116 campaign anchor collision")

    frontier_lucy = json.loads(FRONTIER.read_text())["designs"]["LUCY"]
    future_species = {member["species"] for member in frontier_lucy["team"]}
    if future_species != {"SPECIES_SEVIPER", "SPECIES_ARBOK", "SPECIES_SERPERIOR", "SPECIES_SANDACONDA", "SPECIES_ZYGARDE", "SPECIES_STEELIX"} or future_species & {member["species"] for member in TEAM}:
        raise SystemExit("FAIL: Battle 116 future Pike identity collision")

    script = (ROOT / "data/maps/LavaridgeTown_PokemonCenter_1F/scripts.inc").read_text()
    battle = script.split("LavaridgeTown_PokemonCenter_1F_EventScript_LucyBattle::", 1)[1].split("LavaridgeTown_PokemonCenter_1F_EventScript_LucyReward::", 1)[0]
    for token in ("goto_if_defeated TRAINER_LUCY_LAVARIDGE", "HasEnoughMonsForDoubleBattle", "PLAYER_HAS_TWO_USABLE_MONS", "LucyNeedTwoMons", "trainerbattle_no_intro TRAINER_LUCY_LAVARIDGE"):
        if token not in battle:
            raise SystemExit(f"FAIL: Battle 116 battle branch missing {token}")
    reward = script.split("LavaridgeTown_PokemonCenter_1F_EventScript_LucyReward::", 1)[1].split("LavaridgeTown_PokemonCenter_1F_EventScript_Lucy::", 1)[0]
    for token in ("checkitemspace ITEM_BOTTLE_CAP, 3", "giveitem ITEM_BOTTLE_CAP, 3", "setvar VAR_LAVARIDGE_LUCY_STATE, 2"):
        if token not in reward:
            raise SystemExit(f"FAIL: Battle 116 reward missing {token}")
    if reward.index("giveitem ITEM_BOTTLE_CAP, 3") > reward.index("setvar VAR_LAVARIDGE_LUCY_STATE, 2") or "LucyGiveChoiceBand" in script:
        raise SystemExit("FAIL: Battle 116 reward order or stale label")
    map_data = json.loads((ROOT / "data/maps/LavaridgeTown_PokemonCenter_1F/map.json").read_text())
    trigger = next(row for row in map_data["coord_events"] if row.get("script") == "LavaridgeTown_PokemonCenter_1F_EventScript_LucyTrigger")
    if (trigger["x"], trigger["y"], trigger["var"], str(trigger["var_value"])) != (2, 2, "VAR_LAVARIDGE_LUCY_STATE", "0"):
        raise SystemExit("FAIL: Battle 116 trigger")
    dialogue = script.split("LavaridgeTown_PokemonCenter_1F_Text_LucyYouThere:", 1)[1].split("LavaridgeTown_PokemonCenter_1F_Text_WantToBuyMoomooMilk:", 1)[0]
    for cue in ("stack the odds", "Six loaded chances", "beat the odds", "three Bottle Caps", "Pike tests chance", "two healthy"):
        if cue not in dialogue:
            raise SystemExit(f"FAIL: Battle 116 dialogue missing {cue}")
    for raw_line in re.findall(r'\.string "([^"]*)"', dialogue):
        visible = raw_line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            raise SystemExit(f"FAIL: Battle 116 overlong dialogue: {visible}")

    manifest = json.loads((ROOT / "docs/verdant_doubles_manifest.json").read_text())["formats"]["TRAINER_LUCY_LAVARIDGE"]
    if manifest != {"format": "double", "target_size": 6, "archetype": "Engineered-odds Frontier preview", "difficulty": 100, "partner_interaction": True, "level_offset": 3, "location": "Lavaridge Town Pokemon Center 1 F"}:
        raise SystemExit("FAIL: Battle 116 manifest")
    corpus_ids = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    if any(reference not in corpus_ids for reference in REFERENCES):
        raise SystemExit("FAIL: Battle 116 reference")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.write and not args.check:
        parser.error("choose --write or --check")
    generated = payloads()
    paths = (DESIGNS, LEDGER, SEQUENCE, OS_PATH)
    texts = [json.dumps(payload, indent=2, ensure_ascii=False) + "\n" for payload in generated]
    if args.write:
        for path, text in zip(paths, texts):
            path.write_text(text)
    if args.check:
        for path, text in zip(paths, texts):
            if path.read_text() != text:
                raise SystemExit(f"FAIL: Battle 116 stale {path.name}")
        verify_source()
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())["entries"]
        entry = next(row for row in guide if row["trainerId"] == "TRAINER_LUCY_LAVARIDGE")
        if entry["designStatus"] != "closed" or entry["format"] != "double" or entry["partySize"] != 6:
            raise SystemExit("FAIL: Battle 116 guide")
    print("PASS: Battle 116 Lucy engineered-odds preview is source-closed")


if __name__ == "__main__":
    main()
