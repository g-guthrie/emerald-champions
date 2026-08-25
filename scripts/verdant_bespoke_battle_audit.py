#!/usr/bin/env python3
"""Direct invariants for manually closed Verdant battle encounters."""

from __future__ import annotations

import json
import re
from pathlib import Path

import verdant_custom_teams as custom
import verdant_doubles_conversion as doubles
import verdant_team_polish as polish


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


def party_build(trainer_id: str, trainers_text: str, parties_text: str) -> dict:
    block = doubles.trainer_blocks(trainers_text)[trainer_id].group(0)
    body = doubles.party_match(parties_text, doubles.party_name(block)).group(2)
    entries = custom.party_entries(body)
    if len(entries) != 1:
        raise ValueError(f"{trainer_id}: Route 103 rival must have exactly one Pokémon")
    return polish.parse_entry(entries[0])


def party_builds(trainer_id: str, trainers_text: str, parties_text: str) -> list[dict]:
    block = doubles.trainer_blocks(trainers_text)[trainer_id].group(0)
    body = doubles.party_match(parties_text, doubles.party_name(block)).group(2)
    return [polish.parse_entry(entry) for entry in custom.party_entries(body)]


def tutor_move_indices(source: str) -> dict[str, int]:
    body = source.split("const u16 gTutorMoves[] =", 1)[1].split("};", 1)[0]
    moves = []
    for line in body.splitlines():
        match = re.search(r"\[TUTOR_MOVE_[A-Z0-9_]+\]\s*=\s*(MOVE_[A-Z0-9_]+)", line)
        if match:
            moves.append(match.group(1))
    return {move: index for index, move in enumerate(moves)}


def species_has_tutor_move(source: str, indices: dict[str, int], species: str, move: str) -> bool:
    match = re.search(rf"\[SPECIES_{species}\]\s*=\s*\{{([^}}]+)\}}", source)
    if not match:
        return False
    words = [int(value, 16) for value in re.findall(r"0x[0-9A-Fa-f]+", match.group(1))]
    index = indices[move]
    return bool(words[index // 32] & (1 << (index % 32)))


def species_tmhm_body(source: str, species: str) -> str:
    match = re.search(
        rf"\[SPECIES_{species}\]\s*=\s*TMHM_LEARNSET\s*\((.*?)(?=\n\s*\[SPECIES_|\Z)",
        source,
        re.S,
    )
    return match.group(1) if match else ""


def level_up_body(source: str, species_name: str) -> str:
    match = re.search(
        rf"s{species_name}LevelUpLearnset\[\]\s*=\s*\{{(.*?)\}};",
        source,
        re.S,
    )
    return match.group(1) if match else ""


def main() -> None:
    designs = json.loads(read("docs/verdant_bespoke_battle_designs.json"))["designs"]
    trainers_text = read("src/data/trainers.h")
    parties_text = read("src/data/trainer_parties.h")
    trainer_blocks = doubles.trainer_blocks(trainers_text)
    problems = []

    for encounter_id, design in designs.items():
        if design["status"] == "closed" and design["manual_quality"] != 10:
            problems.append(f"{encounter_id}: closed quality is not 10/10")
        if design["status"] == "closed" and design["manual_difficulty"] < 6.5:
            problems.append(f"{encounter_id}: closed difficulty is below 6.5")
        if design["status"] == "closed":
            corpus = design.get("corpus_review", {})
            if corpus.get("reference_pool_size") != 983:
                problems.append(f"{encounter_id}: full 983-team corpus review is not recorded")
            if not corpus.get("full_team_candidates") or not corpus.get("decision"):
                problems.append(f"{encounter_id}: full-team fit decision is missing")
            if not design.get("competitive_references"):
                problems.append(f"{encounter_id}: no concrete competitive provenance is recorded")
            if design.get("strict_cap") == 14:
                stage = design.get("evolution_stage_fit", {})
                if stage.get("status") != "pass":
                    problems.append(f"{encounter_id}: early evolution-stage fit is not closed as pass")
                if stage.get("mega_access") is not False:
                    problems.append(f"{encounter_id}: pre-Stone design does not record Mega access as false")
        for trainer_id in design.get("trainer_ids", []):
            if trainer_id not in trainer_blocks:
                problems.append(f"{encounter_id}: unknown trainer {trainer_id}")

    battle = designs["BATTLE_001_ROUTE_103_RIVAL"]
    expected_ids = {
        f"TRAINER_{rival}_ROUTE_103_{starter}"
        for rival in ("MAY", "BRENDAN")
        for starter in ("TREECKO", "TORCHIC", "MUDKIP")
    }
    if set(battle["trainer_ids"]) != expected_ids:
        problems.append("Battle 1 does not cover all six gender/starter source records")

    expected_by_starter = {
        "TREECKO": ("SPECIES_TORCHIC", "MOVE_FIRE_PLEDGE"),
        "TORCHIC": ("SPECIES_MUDKIP", "MOVE_WATER_PLEDGE"),
        "MUDKIP": ("SPECIES_TREECKO", "MOVE_GRASS_PLEDGE"),
    }
    for starter, (species, pledge) in expected_by_starter.items():
        may = party_build(f"TRAINER_MAY_ROUTE_103_{starter}", trainers_text, parties_text)
        brendan = party_build(f"TRAINER_BRENDAN_ROUTE_103_{starter}", trainers_text, parties_text)
        expected = {
            "level": 0,
            "species": species,
            "item": "ITEM_LIFE_ORB",
            "ability_slot": 0,
            "spread": "SPREAD_31_IV_0_EV",
            "moves": [pledge, "MOVE_RETURN", "MOVE_TOXIC", "MOVE_PROTECT"],
        }
        if may != brendan:
            problems.append(f"Battle 1 {starter}: May and Brendan branches differ")
        if may != expected:
            problems.append(f"Battle 1 {starter}: source set differs from the closed design")

    battle_main = read("src/battle_main.c")
    starter_choose = read("src/starter_choose.c")
    dynamic_tokens = (
        "IsRoute103RivalTrainer(trainerNum)",
        "GetStarterPokemonForGeneration((VarGet(VAR_STARTER_MON) + 1) % 3, VarGet(VAR_STARTER_GEN))",
        "CreateMon(&party[i], species, level, 31",
    )
    for token in dynamic_tokens:
        if token not in battle_main:
            problems.append(f"Battle 1 dynamic counterpart rule missing: {token}")

    trio_names = ("Kanto", "Johto", "Hoenn", "Sinnoh", "Unova", "Kalos", "Alola")
    trios = {}
    for name in trio_names:
        match = re.search(rf"sStarterMon{name}\[STARTER_MON_COUNT\]\s*=\s*\{{(.*?)\}};", starter_choose, re.S)
        species = re.findall(r"SPECIES_([A-Z0-9_]+)", match.group(1)) if match else []
        if len(species) != 3:
            problems.append(f"Battle 1: {name} starter trio is not complete")
        trios[name] = species

    tutor_source = read("src/data/pokemon/tutor_learnsets.h")
    tmhm_source = read("src/data/pokemon/tmhm_learnsets.h")
    indices = tutor_move_indices(tutor_source)
    pledge_by_slot = ("MOVE_GRASS_PLEDGE", "MOVE_FIRE_PLEDGE", "MOVE_WATER_PLEDGE")
    for trio in trios.values():
        for slot, species in enumerate(trio):
            if not species_has_tutor_move(tutor_source, indices, species, pledge_by_slot[slot]):
                problems.append(f"Battle 1: {species} cannot legally learn {pledge_by_slot[slot]}")
            tmhm = species_tmhm_body(tmhm_source, species)
            for move in ("TM06_TOXIC", "TM17_PROTECT", "TM27_RETURN"):
                if move not in tmhm:
                    problems.append(f"Battle 1: {species} is missing {move}")

    oldale = read("data/maps/OldaleTown_PokemonCenter_1F/map.json")
    shop = read("src/shop.c")
    if not all(token in oldale for token in ("PKMN_Center_Move_Tutor", "General_Mart_Script")):
        problems.append("Battle 1: Oldale preparation NPCs are not both accessible")
    if not all(token in shop for token in ("ITEM_RARE_CANDY", "ITEM_LIFE_ORB", "ITEM_FOCUS_SASH", "ITEM_EVIOLITE", "ITEM_LEFTOVERS")):
        problems.append("Battle 1: documented core preparation items are not in normal Mart stock")
    if "SetMoney(&gSaveBlock1Ptr->money, 25000);" not in read("src/new_game.c"):
        problems.append("Battle 1: documented starting money is not $25,000")

    calvin = designs["BATTLE_002_ROUTE_102_CALVIN"]
    expected_calvin = [
        {
            "level": 0, "species": "SPECIES_ZORUA", "item": "ITEM_EXPERT_BELT", "ability_slot": 0,
            "spread": "SPREAD_HP_FIGHTING_TIMID",
            "moves": ["MOVE_DARK_PULSE", "MOVE_EXTRASENSORY", "MOVE_SUCKER_PUNCH", "MOVE_HIDDEN_POWER"],
        },
        {
            "level": 0, "species": "SPECIES_JIGGLYPUFF", "item": "ITEM_EVIOLITE", "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_DEF_SPDEF_SASSY",
            "moves": ["MOVE_ICY_WIND", "MOVE_HELPING_HAND", "MOVE_DAZZLING_GLEAM", "MOVE_PROTECT"],
        },
        {
            "level": 1, "species": "SPECIES_SMEARGLE", "item": "ITEM_FOCUS_SASH", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPEED_JOLLY",
            "moves": ["MOVE_FAKE_OUT", "MOVE_FOLLOW_ME", "MOVE_HELPING_HAND", "MOVE_ENDEAVOR"],
        },
        {
            "level": 1, "species": "SPECIES_MEW", "item": "ITEM_SITRUS_BERRY", "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_PSYSHOCK", "MOVE_NASTY_PLOT", "MOVE_AURA_SPHERE", "MOVE_WILL_O_WISP"],
        },
    ]
    if calvin["trainer_ids"] != ["TRAINER_CALVIN_1"]:
        problems.append("Battle 2: closure is not attached only to Calvin's first battle")
    if party_builds("TRAINER_CALVIN_1", trainers_text, parties_text) != expected_calvin:
        problems.append("Battle 2: Calvin's source party differs from the closed design")

    calvin_block = trainer_blocks["TRAINER_CALVIN_1"].group(0)
    for token in (".doubleBattle = TRUE", "AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_SMART_SWITCHING"):
        if token not in calvin_block:
            problems.append(f"Battle 2: Calvin is missing {token}")
    illusion_source = read("src/battle_util.c")
    if not all(token in illusion_source for token in ("for (i = PARTY_SIZE - 1; i >= 0; i--)", "ABILITY_ILLUSION", "gBattleStruct->illusion[battlerId].mon = &party[id]")):
        problems.append("Battle 2: Zorua's last-slot Mew disguise is not supported by the engine")
    fighting_spread_match = re.search(
        r"\[SPREAD_HP_FIGHTING_TIMID\]\s*=\s*\{(.*?)(?=\n\s*\[SPREAD_)",
        read("src/data/trainer_spreads.h"),
        re.S,
    )
    fighting_spread = fighting_spread_match.group(1) if fighting_spread_match else ""
    if ".nature = NATURE_TIMID" not in fighting_spread:
        problems.append("Battle 2: HP Fighting Timid spread does not actually use a Timid nature")
    route102 = read("data/maps/Route102/scripts.inc")
    if "trainerbattle_double TRAINER_CALVIN_1" not in route102 or "trainerbattle_rematch_double TRAINER_CALVIN_1" not in route102:
        problems.append("Battle 2: Calvin does not use native doubles script guards")
    if "Route102_Text_CalvinNotEnoughPokemon" not in route102:
        problems.append("Battle 2: Calvin has no native two-healthy-Pokémon guard text")

    zorua_tmhm = species_tmhm_body(tmhm_source, "ZORUA")
    for token in ("TM10_HIDDEN_POWER", "TM94_SUCKER_PUNCH", "TM97_DARK_PULSE"):
        if token not in zorua_tmhm:
            problems.append(f"Battle 2: Zorua is missing {token}")
    if "MOVE_EXTRASENSORY" not in read("src/data/pokemon/egg_moves.h").split("egg_moves(ZORUA", 1)[1].split("),", 1)[0]:
        problems.append("Battle 2: Zorua cannot legally learn Extrasensory")

    jiggly_tmhm = species_tmhm_body(tmhm_source, "JIGGLYPUFF")
    for token in ("TM17_PROTECT", "TM99_DAZZLING_GLEAM"):
        if token not in jiggly_tmhm:
            problems.append(f"Battle 2: Jigglypuff is missing {token}")
    for move in ("MOVE_ICY_WIND", "MOVE_HELPING_HAND"):
        if not species_has_tutor_move(tutor_source, indices, "JIGGLYPUFF", move):
            problems.append(f"Battle 2: Jigglypuff cannot legally learn {move}")

    mew_tmhm = species_tmhm_body(tmhm_source, "MEW")
    for token in ("TM54_PSYSHOCK", "TM61_WILL_O_WISP"):
        if token not in mew_tmhm:
            problems.append(f"Battle 2: Mew is missing {token}")
    for move in ("MOVE_NASTY_PLOT", "MOVE_AURA_SPHERE"):
        if not species_has_tutor_move(tutor_source, indices, "MEW", move):
            problems.append(f"Battle 2: Mew cannot legally learn {move}")
    if "LEVEL_UP_MOVE( 1, MOVE_SKETCH)" not in read("src/data/pokemon/level_up_learnsets.h"):
        problems.append("Battle 2: Smeargle's Sketch legality is missing")

    calvin_dialogue = read("data/text/trainers.inc").split("Route102_Text_CalvinIntro:", 1)[1].split("Route102_Text_CalvinRegister:", 1)[0]
    for line in re.findall(r'\.string "([^"]*)"', calvin_dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("$", "")
        if len(visible) > 36:
            problems.append(f"Battle 2: Calvin dialogue line is too long: {visible}")

    rick = designs["BATTLE_003_ROUTE_102_RICK"]
    expected_rick = [
        {
            "level": 0, "species": "SPECIES_DEWPIDER", "item": "ITEM_FOCUS_SASH", "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_MODEST",
            "moves": ["MOVE_STICKY_WEB", "MOVE_SCALD", "MOVE_ICE_BEAM", "MOVE_GIGA_DRAIN"],
        },
        {
            "level": 1, "species": "SPECIES_ANORITH", "item": "ITEM_CHOICE_BAND", "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_ADAMANT",
            "moves": ["MOVE_X_SCISSOR", "MOVE_ROCK_SLIDE", "MOVE_AQUA_JET", "MOVE_KNOCK_OFF"],
        },
        {
            "level": 1, "species": "SPECIES_KARRABLAST", "item": "ITEM_EVIOLITE", "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_MEGAHORN", "MOVE_DRILL_RUN", "MOVE_KNOCK_OFF", "MOVE_SWORDS_DANCE"],
        },
        {
            "level": 2, "species": "SPECIES_LARVESTA", "item": "ITEM_LIFE_ORB", "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_FLARE_BLITZ", "MOVE_U_TURN", "MOVE_WILD_CHARGE", "MOVE_MORNING_SUN"],
        },
    ]
    if rick["trainer_ids"] != ["TRAINER_RICK"]:
        problems.append("Battle 3: closure is not attached only to Rick")
    if party_builds("TRAINER_RICK", trainers_text, parties_text) != expected_rick:
        problems.append("Battle 3: Rick's source party differs from the closed design")
    if sum(build["item"] == "ITEM_FOCUS_SASH" for build in expected_rick) != 1:
        problems.append("Battle 3: Rick must have exactly one Focus Sash")
    if sum(move == "MOVE_SWORDS_DANCE" for build in expected_rick for move in build["moves"]) != 1:
        problems.append("Battle 3: Rick must have exactly one setup win condition")
    if rick.get("evolution_stage_fit", {}).get("status") != "pass":
        problems.append("Battle 3: Rick's evolution-stage closure is not passing")

    rick_block = trainer_blocks["TRAINER_RICK"].group(0)
    if ".doubleBattle = FALSE" not in rick_block:
        problems.append("Battle 3: Rick no longer provides the intended singles pacing contrast")
    for token in ("AI_FLAG_SETUP_FIRST_TURN", "AI_FLAG_SMART_SWITCHING"):
        if token not in rick_block:
            problems.append(f"Battle 3: Rick is missing {token}")
    for token in ("AI_FLAG_SPEED_CONTROL", "AI_FLAG_COMBO_SETUP"):
        if token in rick_block:
            problems.append(f"Battle 3: Rick has unrelated scripted pressure from {token}")

    level_source = read("src/data/pokemon/level_up_learnsets.h")
    dewpider_eggs = re.search(r"egg_moves\(DEWPIDER,(.*?)\)", read("src/data/pokemon/egg_moves.h"), re.S)
    if not dewpider_eggs or "MOVE_STICKY_WEB" not in dewpider_eggs.group(1):
        problems.append("Battle 3: Dewpider cannot legally inherit Sticky Web")
    dewpider_tmhm = species_tmhm_body(tmhm_source, "DEWPIDER")
    for move in ("TM13_ICE_BEAM", "TM19_GIGA_DRAIN", "TM55_SCALD"):
        if move not in dewpider_tmhm:
            problems.append(f"Battle 3: Dewpider is missing {move}")

    anorith_level = level_up_body(level_source, "Anorith")
    if "MOVE_X_SCISSOR" not in anorith_level:
        problems.append("Battle 3: Anorith cannot legally learn X-Scissor")
    anorith_tmhm = species_tmhm_body(tmhm_source, "ANORITH")
    for move in ("TM54_ROCK_SLIDE",):
        if move not in anorith_tmhm and "TM63_ROCK_SLIDE" not in anorith_tmhm:
            problems.append("Battle 3: Anorith cannot legally learn Rock Slide")
    anorith_eggs = re.search(r"egg_moves\(ANORITH,(.*?)\)", read("src/data/pokemon/egg_moves.h"), re.S)
    for move in ("MOVE_AQUA_JET", "MOVE_KNOCK_OFF"):
        if not anorith_eggs or move not in anorith_eggs.group(1):
            problems.append(f"Battle 3: Anorith cannot legally inherit {move}")

    karrablast_level = level_up_body(level_source, "Karrablast")
    if "MOVE_SWORDS_DANCE" not in karrablast_level:
        problems.append("Battle 3: Karrablast cannot legally learn Swords Dance")
    karrablast_eggs = re.search(r"egg_moves\(KARRABLAST,(.*?)\)", read("src/data/pokemon/egg_moves.h"), re.S)
    for move in ("MOVE_MEGAHORN", "MOVE_DRILL_RUN", "MOVE_KNOCK_OFF"):
        if not karrablast_eggs or move not in karrablast_eggs.group(1):
            problems.append(f"Battle 3: Karrablast cannot legally inherit {move}")

    larvesta_level = level_up_body(level_source, "Larvesta")
    if "MOVE_FLARE_BLITZ" not in larvesta_level:
        problems.append("Battle 3: Larvesta cannot legally learn Flare Blitz")
    larvesta_tmhm = species_tmhm_body(tmhm_source, "LARVESTA")
    for move in ("TM89_U_TURN", "TM93_WILD_CHARGE"):
        if move not in larvesta_tmhm:
            problems.append(f"Battle 3: Larvesta is missing {move}")
    larvesta_eggs = re.search(r"egg_moves\(LARVESTA,(.*?)\)", read("src/data/pokemon/egg_moves.h"), re.S)
    if not larvesta_eggs or "MOVE_MORNING_SUN" not in larvesta_eggs.group(1):
        problems.append("Battle 3: Larvesta cannot legally inherit Morning Sun")

    rick_dialogue = read("data/text/trainers.inc").split("Route102_Text_RickIntro:", 1)[1].split("Route102_Text_TianaIntro:", 1)[0]
    for line in re.findall(r'\.string "([^"]*)"', rick_dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("$", "")
        if len(visible) > 36:
            problems.append(f"Battle 3: Rick dialogue line is too long: {visible}")

    allen = designs["BATTLE_004_ROUTE_102_ALLEN"]
    expected_allen = [
        {
            "level": 0, "species": "SPECIES_PIKACHU", "item": "ITEM_LIGHT_BALL", "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_FAKE_OUT", "MOVE_PROTECT", "MOVE_VOLT_TACKLE", "MOVE_ELECTROWEB"],
        },
        {
            "level": 1, "species": "SPECIES_TAILLOW", "item": "ITEM_TOXIC_ORB", "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_PROTECT", "MOVE_QUICK_ATTACK", "MOVE_BRAVE_BIRD", "MOVE_FACADE"],
        },
        {
            "level": 1, "species": "SPECIES_PARAS", "item": "ITEM_EVIOLITE", "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_DEF_BOLD",
            "moves": ["MOVE_SPORE", "MOVE_PROTECT", "MOVE_SEED_BOMB", "MOVE_RAGE_POWDER"],
        },
        {
            "level": 0, "species": "SPECIES_ZIGZAGOON", "item": "ITEM_SITRUS_BERRY", "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_BELLY_DRUM", "MOVE_EXTREME_SPEED", "MOVE_SEED_BOMB", "MOVE_PROTECT"],
        },
    ]
    if allen["trainer_ids"] != ["TRAINER_ALLEN"]:
        problems.append("Battle 4: closure is not attached only to Allen")
    if party_builds("TRAINER_ALLEN", trainers_text, parties_text) != expected_allen:
        problems.append("Battle 4: Allen's source party differs from the closed design")
    if sum(build["item"] == "ITEM_FOCUS_SASH" for build in expected_allen) != 0:
        problems.append("Battle 4: Allen's back-line redirection must not be guaranteed by Focus Sash")
    if sum(move == "MOVE_BELLY_DRUM" for build in expected_allen for move in build["moves"]) != 1:
        problems.append("Battle 4: Allen must have exactly one setup win condition")
    if allen.get("evolution_stage_fit", {}).get("status") != "pass":
        problems.append("Battle 4: Allen's evolution-stage closure is not passing")

    allen_block = trainer_blocks["TRAINER_ALLEN"].group(0)
    for token in (".doubleBattle = TRUE", "AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL"):
        if token not in allen_block:
            problems.append(f"Battle 4: Allen is missing {token}")
    for token in ("AI_FLAG_COMBO_SETUP", "AI_FLAG_SETUP_FIRST_TURN"):
        if token in allen_block:
            problems.append(f"Battle 4: Allen has an unnecessary forced-combo flag: {token}")
    ai_main_source = read("src/battle_ai_main.c")
    for token in ("[11] = AI_HelpPartner", "effect == EFFECT_FOLLOW_ME && (partnerChoosingSetup || PartnerHasSetupMove(partner))", "score += partnerChoosingSetup ? 12 : 6"):
        if token not in ai_main_source:
            problems.append(f"Battle 4: partner-support preference is not active: {token}")
    if "trainerbattle_double TRAINER_ALLEN" not in route102 or "Route102_Text_AllenNotEnoughPokemon" not in route102:
        problems.append("Battle 4: Allen does not use the native doubles party-size guard")

    paras_level = level_up_body(level_source, "Paras")
    for move in ("MOVE_SPORE", "MOVE_RAGE_POWDER"):
        if move not in paras_level:
            problems.append(f"Battle 4: Paras cannot legally learn {move}")
    if "TM17_PROTECT" not in species_tmhm_body(tmhm_source, "PARAS"):
        problems.append("Battle 4: Paras cannot legally learn Protect")
    if not species_has_tutor_move(tutor_source, indices, "PARAS", "MOVE_SEED_BOMB"):
        problems.append("Battle 4: Paras cannot legally learn Seed Bomb")

    zigzagoon_level = level_up_body(level_source, "Zigzagoon")
    if "MOVE_BELLY_DRUM" not in zigzagoon_level:
        problems.append("Battle 4: Zigzagoon cannot legally learn Belly Drum")
    zigzagoon_eggs = read("src/data/pokemon/egg_moves.h").split("egg_moves(ZIGZAGOON", 1)[1].split("),", 1)[0]
    if "MOVE_EXTREME_SPEED" not in zigzagoon_eggs:
        problems.append("Battle 4: Zigzagoon cannot legally learn Extreme Speed")
    if "TM17_PROTECT" not in species_tmhm_body(tmhm_source, "ZIGZAGOON"):
        problems.append("Battle 4: Zigzagoon cannot legally learn Protect")
    if not species_has_tutor_move(tutor_source, indices, "ZIGZAGOON", "MOVE_SEED_BOMB"):
        problems.append("Battle 4: Zigzagoon cannot legally learn Seed Bomb")

    pichu_eggs = read("src/data/pokemon/egg_moves.h").split("egg_moves(PICHU", 1)[1].split("),", 1)[0]
    for move in ("MOVE_FAKE_OUT",):
        if move not in pichu_eggs:
            problems.append(f"Battle 4: Pikachu cannot legally inherit {move}")
    if not species_has_tutor_move(tutor_source, indices, "PIKACHU", "MOVE_ELECTROWEB"):
        problems.append("Battle 4: Pikachu cannot legally learn Electroweb")
    if "TM17_PROTECT" not in species_tmhm_body(tmhm_source, "PIKACHU"):
        problems.append("Battle 4: Pikachu cannot legally learn Protect")
    pokemon_source = read("src/pokemon.c")
    if "if (eggSpecies == SPECIES_PICHU)" not in pokemon_source or "AddMoveIfLegalAndNew(MOVE_VOLT_TACKLE" not in pokemon_source:
        problems.append("Battle 4: all-legal-moves teacher omits the Pichu-line Volt Tackle special case")

    taillow_level = level_up_body(level_source, "Taillow")
    if "MOVE_QUICK_ATTACK" not in taillow_level:
        problems.append("Battle 4: Taillow cannot legally learn Quick Attack")
    taillow_eggs = read("src/data/pokemon/egg_moves.h").split("egg_moves(TAILLOW", 1)[1].split("),", 1)[0]
    if "MOVE_BRAVE_BIRD" not in taillow_eggs:
        problems.append("Battle 4: Taillow cannot legally learn Brave Bird")
    taillow_tmhm = species_tmhm_body(tmhm_source, "TAILLOW")
    for move in ("TM17_PROTECT", "TM42_FACADE"):
        if move not in taillow_tmhm:
            problems.append(f"Battle 4: Taillow is missing {move}")

    allen_dialogue = read("data/text/trainers.inc").split("Route102_Text_AllenIntro:", 1)[1].split("Route102_Text_RickIntro:", 1)[0]
    for line in re.findall(r'\.string "([^"]*)"', allen_dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("$", "")
        if len(visible) > 36:
            problems.append(f"Battle 4: Allen dialogue line is too long: {visible}")

    tiana = designs["BATTLE_005_ROUTE_102_TIANA"]
    expected_tiana = [
        {
            "level": 0, "species": "SPECIES_AMAURA", "item": "ITEM_FOCUS_SASH", "ability_slot": 2,
            "spread": "SPREAD_31_IV_SPATK_SPEED_MODEST",
            "moves": ["MOVE_AURORA_VEIL", "MOVE_BLIZZARD", "MOVE_EARTH_POWER", "MOVE_PROTECT"],
        },
        {
            "level": 1, "species": "SPECIES_SANDSHREW_ALOLAN", "item": "ITEM_EVIOLITE", "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_ICICLE_CRASH", "MOVE_IRON_HEAD", "MOVE_BRICK_BREAK", "MOVE_PROTECT"],
        },
        {
            "level": 1, "species": "SPECIES_SWINUB", "item": "ITEM_CHOICE_BAND", "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_ADAMANT",
            "moves": ["MOVE_ICICLE_CRASH", "MOVE_SUPERPOWER", "MOVE_ICE_SHARD", "MOVE_ROCK_SLIDE"],
        },
        {
            "level": 2, "species": "SPECIES_ARCTOZOLT", "item": "ITEM_EXPERT_BELT", "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_BOLT_BEAK", "MOVE_ICICLE_CRASH", "MOVE_STOMPING_TANTRUM", "MOVE_PROTECT"],
        },
    ]
    if tiana["trainer_ids"] != ["TRAINER_TIANA"]:
        problems.append("Battle 5: closure is not attached only to Tiana")
    if party_builds("TRAINER_TIANA", trainers_text, parties_text) != expected_tiana:
        problems.append("Battle 5: Tiana's source party differs from the closed design")
    if [build["level"] for build in expected_tiana] != [0, 1, 1, 2]:
        problems.append("Battle 5: Tiana must use the authored 14/15/15/16 progression")
    if tiana.get("evolution_stage_fit", {}).get("status") != "pass":
        problems.append("Battle 5: Tiana's evolution-stage closure is not passing")

    tiana_block = trainer_blocks["TRAINER_TIANA"].group(0)
    for token in (".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_SETUP_FIRST_TURN", "AI_FLAG_FIELD_CONTROL"):
        if token not in tiana_block:
            problems.append(f"Battle 5: Tiana is missing {token}")
    if "AI_FLAG_SPEED_CONTROL" in tiana_block:
        problems.append("Battle 5: Tiana retains a speed-control profile despite having no active speed-control move")
    if "trainerbattle_double TRAINER_TIANA" not in route102 or "Route102_Text_TianaNotEnoughPokemon" not in route102:
        problems.append("Battle 5: Tiana does not use the native doubles party-size guard")

    amaura_tmhm = species_tmhm_body(tmhm_source, "AMAURA")
    for move in ("TM14_BLIZZARD", "TM17_PROTECT", "TM70_AURORA_VEIL"):
        if move not in amaura_tmhm:
            problems.append(f"Battle 5: Amaura is missing {move}")
    if not species_has_tutor_move(tutor_source, indices, "AMAURA", "MOVE_EARTH_POWER"):
        problems.append("Battle 5: Amaura cannot legally learn Earth Power")

    sandshrew_eggs = re.search(r"egg_moves\(SANDSHREW_ALOLAN,(.*?)\)", read("src/data/pokemon/egg_moves.h"), re.S)
    if not sandshrew_eggs or "MOVE_ICICLE_CRASH" not in sandshrew_eggs.group(1):
        problems.append("Battle 5: Alolan Sandshrew cannot inherit Icicle Crash")
    sandshrew_level = level_up_body(level_source, "SandshrewAlolan")
    if "MOVE_IRON_HEAD" not in sandshrew_level and not species_has_tutor_move(tutor_source, indices, "SANDSHREW_ALOLAN", "MOVE_IRON_HEAD"):
        problems.append("Battle 5: Alolan Sandshrew cannot legally learn Iron Head")
    sandshrew_tmhm = species_tmhm_body(tmhm_source, "SANDSHREW_ALOLAN")
    for move in ("TM17_PROTECT", "TM31_BRICK_BREAK"):
        if move not in sandshrew_tmhm:
            problems.append(f"Battle 5: Alolan Sandshrew is missing {move}")

    swinub_level = level_up_body(level_source, "Swinub")
    if "MOVE_ICE_SHARD" not in swinub_level:
        problems.append("Battle 5: Swinub cannot legally learn Ice Shard")
    swinub_eggs = read("src/data/pokemon/egg_moves.h").split("egg_moves(SWINUB", 1)[1].split("),", 1)[0]
    if "MOVE_ICICLE_CRASH" not in swinub_eggs:
        problems.append("Battle 5: Swinub cannot legally inherit Icicle Crash")
    if "TM63_ROCK_SLIDE" not in species_tmhm_body(tmhm_source, "SWINUB"):
        problems.append("Battle 5: Swinub cannot legally learn Rock Slide")
    if not species_has_tutor_move(tutor_source, indices, "SWINUB", "MOVE_SUPERPOWER"):
        problems.append("Battle 5: Swinub cannot legally learn Superpower")

    arctozolt_level = level_up_body(level_source, "Arctozolt")
    for move in ("MOVE_BOLT_BEAK", "MOVE_ICICLE_CRASH"):
        if move not in arctozolt_level:
            problems.append(f"Battle 5: Arctozolt cannot legally learn {move}")
    if "TM17_PROTECT" not in species_tmhm_body(tmhm_source, "ARCTOZOLT"):
        problems.append("Battle 5: Arctozolt cannot legally learn Protect")
    if not species_has_tutor_move(tutor_source, indices, "ARCTOZOLT", "MOVE_STOMPING_TANTRUM"):
        problems.append("Battle 5: Arctozolt cannot legally learn Stomping Tantrum")

    tiana_dialogue = read("data/text/trainers.inc").split("Route102_Text_TianaIntro:", 1)[1].split("Route103_Text_DaisyIntro:", 1)[0]
    if "team's young" not in tiana_dialogue or "fastest pair" not in tiana_dialogue:
        problems.append("Battle 5: Tiana's post-battle weather dialogue is stale")
    for line in re.findall(r'\.string "([^"]*)"', tiana_dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("$", "")
        if len(visible) > 36:
            problems.append(f"Battle 5: Tiana dialogue line is too long: {visible}")

    billy = designs["BATTLE_006_ROUTE_104_BILLY"]
    expected_billy = [
        {
            "level": 0, "species": "SPECIES_DITTO", "item": "ITEM_CHOICE_SCARF", "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_DEF_SPDEF_SASSY",
            "moves": ["MOVE_TRANSFORM", "MOVE_NONE", "MOVE_NONE", "MOVE_NONE"],
        },
        {
            "level": 1, "species": "SPECIES_WIMPOD", "item": "ITEM_FOCUS_SASH", "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_TAUNT", "MOVE_SPIKES", "MOVE_AQUA_JET", "MOVE_LEECH_LIFE"],
        },
        {
            "level": 1, "species": "SPECIES_SANDYGAST", "item": "ITEM_EVIOLITE", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_SHORE_UP", "MOVE_EARTH_POWER", "MOVE_SHADOW_BALL", "MOVE_TOXIC"],
        },
        {
            "level": 2, "species": "SPECIES_TIRTOUGA", "item": "ITEM_WEAKNESS_POLICY", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_LIQUIDATION", "MOVE_STONE_EDGE", "MOVE_AQUA_JET", "MOVE_KNOCK_OFF"],
        },
    ]
    if billy["trainer_ids"] != ["TRAINER_BILLY"]:
        problems.append("Battle 6: closure is not attached only to Billy")
    if party_builds("TRAINER_BILLY", trainers_text, parties_text) != expected_billy:
        problems.append("Battle 6: Billy's source party differs from the closed design")
    if [build["level"] for build in expected_billy] != [0, 1, 1, 2]:
        problems.append("Battle 6: Billy must use the authored 14/15/15/16 progression")
    if billy.get("evolution_stage_fit", {}).get("status") != "pass":
        problems.append("Battle 6: Billy's evolution-stage closure is not passing")

    billy_block = trainer_blocks["TRAINER_BILLY"].group(0)
    for token in (".doubleBattle = FALSE", "AI_FLAG_SMART_SWITCHING"):
        if token not in billy_block:
            problems.append(f"Battle 6: Billy is missing {token}")
    for token in ("AI_FLAG_SPEED_CONTROL", "AI_FLAG_COMBO_SETUP", "AI_FLAG_FIELD_CONTROL"):
        if token in billy_block:
            problems.append(f"Battle 6: Billy has an unrelated AI profile: {token}")

    ditto_level = level_up_body(level_source, "Ditto")
    if "MOVE_TRANSFORM" not in ditto_level:
        problems.append("Battle 6: Ditto cannot legally learn Transform")
    ditto_stats = re.search(r"\[SPECIES_DITTO\]\s*=\s*\{(.*?)\n\s*\},", read("src/data/pokemon/base_stats.h"), re.S)
    if not ditto_stats or "ABILITY_IMPOSTER" not in ditto_stats.group(1):
        problems.append("Battle 6: Ditto does not expose Imposter")

    wimpod_tmhm = species_tmhm_body(tmhm_source, "WIMPOD")
    for move in ("TM12_TAUNT", "TM56_LEECH_LIFE"):
        if move not in wimpod_tmhm:
            problems.append(f"Battle 6: Wimpod is missing {move}")
    wimpod_eggs = re.search(r"egg_moves\(WIMPOD,(.*?)\)", read("src/data/pokemon/egg_moves.h"), re.S)
    for move in ("MOVE_SPIKES", "MOVE_AQUA_JET"):
        if not wimpod_eggs or move not in wimpod_eggs.group(1):
            problems.append(f"Battle 6: Wimpod cannot legally inherit {move}")

    sandygast_level = level_up_body(level_source, "Sandygast")
    for move in ("MOVE_SHORE_UP", "MOVE_EARTH_POWER"):
        if move not in sandygast_level:
            problems.append(f"Battle 6: Sandygast cannot legally learn {move}")
    sandygast_tmhm = species_tmhm_body(tmhm_source, "SANDYGAST")
    for move in ("TM06_TOXIC", "TM30_SHADOW_BALL"):
        if move not in sandygast_tmhm:
            problems.append(f"Battle 6: Sandygast is missing {move}")

    tirtouga_level = level_up_body(level_source, "Tirtouga")
    if "MOVE_AQUA_JET" not in tirtouga_level:
        problems.append("Battle 6: Tirtouga cannot legally learn Aqua Jet")
    if "TM71_STONE_EDGE" not in species_tmhm_body(tmhm_source, "TIRTOUGA"):
        problems.append("Battle 6: Tirtouga cannot legally learn Stone Edge")
    for move in ("MOVE_LIQUIDATION", "MOVE_KNOCK_OFF"):
        if not species_has_tutor_move(tutor_source, indices, "TIRTOUGA", move):
            problems.append(f"Battle 6: Tirtouga cannot legally learn {move}")

    billy_dialogue = read("data/text/trainers.inc").split("Route104_Text_BillyIntro:", 1)[1].split("Route104_Text_HaleyIntro:", 1)[0]
    for line in re.findall(r'\.string "([^"]*)"', billy_dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("$", "")
        if len(visible) > 36:
            problems.append(f"Battle 6: Billy dialogue line is too long: {visible}")

    darian = designs["BATTLE_007_ROUTE_104_DARIAN"]
    expected_darian = [
        {
            "level": 3, "species": "SPECIES_REMORAID", "item": "ITEM_CHOICE_SCARF", "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_HASTY",
            "moves": ["MOVE_WATER_SPOUT", "MOVE_SEED_BOMB", "MOVE_FIRE_BLAST", "MOVE_HIDDEN_POWER"],
        },
        {
            "level": 0, "species": "SPECIES_BRUXISH", "item": "ITEM_EXPERT_BELT", "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_PSYCHIC_FANGS", "MOVE_CRUNCH", "MOVE_TAUNT", "MOVE_PROTECT"],
        },
        {
            "level": 0, "species": "SPECIES_QWILFISH", "item": "ITEM_SITRUS_BERRY", "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_THUNDER_WAVE", "MOVE_PROTECT", "MOVE_POISON_JAB", "MOVE_LIQUIDATION"],
        },
        {
            "level": 2, "species": "SPECIES_CLAMPERL", "item": "ITEM_FOCUS_SASH", "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_MODEST",
            "moves": ["MOVE_SHELL_SMASH", "MOVE_MUDDY_WATER", "MOVE_ICE_BEAM", "MOVE_PROTECT"],
        },
    ]
    if darian["trainer_ids"] != ["TRAINER_DARIAN"]:
        problems.append("Battle 7: closure is not attached only to Darian")
    if party_builds("TRAINER_DARIAN", trainers_text, parties_text) != expected_darian:
        problems.append("Battle 7: Darian's source party differs from the closed design")
    if [build["level"] for build in expected_darian] != [3, 0, 0, 2]:
        problems.append("Battle 7: Darian must use the authored 17/14/14/16 two-wave progression")
    if darian.get("evolution_stage_fit", {}).get("status") != "pass":
        problems.append("Battle 7: Darian's evolution-stage closure is not passing")

    darian_block = trainer_blocks["TRAINER_DARIAN"].group(0)
    for token in (".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_SETUP_FIRST_TURN", "AI_FLAG_HP_AWARE", "AI_FLAG_SPEED_CONTROL"):
        if token not in darian_block:
            problems.append(f"Battle 7: Darian is missing {token}")
    for token in ("AI_FLAG_HELP_PARTNER", "AI_FLAG_PERISH_TRAP", "AI_FLAG_COMBO_SETUP", "AI_FLAG_FIELD_CONTROL"):
        if token in darian_block:
            problems.append(f"Battle 7: Darian has an unrelated AI profile: {token}")

    remoraid_eggs = re.search(r"egg_moves\(REMORAID,(.*?)\)", read("src/data/pokemon/egg_moves.h"), re.S)
    if not remoraid_eggs or "MOVE_WATER_SPOUT" not in remoraid_eggs.group(1):
        problems.append("Battle 7: Remoraid cannot legally inherit Water Spout")
    if not species_has_tutor_move(tutor_source, indices, "REMORAID", "MOVE_SEED_BOMB"):
        problems.append("Battle 7: Remoraid cannot legally learn Seed Bomb")
    remoraid_tmhm = species_tmhm_body(tmhm_source, "REMORAID")
    for move in ("TM10_HIDDEN_POWER", "TM38_FIRE_BLAST"):
        if move not in remoraid_tmhm:
            problems.append(f"Battle 7: Remoraid is missing {move}")

    bruxish_level = level_up_body(level_source, "Bruxish")
    for move in ("MOVE_PSYCHIC_FANGS", "MOVE_CRUNCH"):
        if move not in bruxish_level and not species_has_tutor_move(tutor_source, indices, "BRUXISH", move):
            problems.append(f"Battle 7: Bruxish cannot legally learn {move}")
    bruxish_tmhm = species_tmhm_body(tmhm_source, "BRUXISH")
    for move in ("TM12_TAUNT", "TM17_PROTECT"):
        if move not in bruxish_tmhm:
            problems.append(f"Battle 7: Bruxish is missing {move}")

    qwilfish_tmhm = species_tmhm_body(tmhm_source, "QWILFISH")
    for move in ("TM17_PROTECT", "TM73_THUNDER_WAVE", "TM84_POISON_JAB"):
        if move not in qwilfish_tmhm:
            problems.append(f"Battle 7: Qwilfish is missing {move}")
    if not species_has_tutor_move(tutor_source, indices, "QWILFISH", "MOVE_LIQUIDATION"):
        problems.append("Battle 7: Qwilfish cannot legally learn Liquidation")

    clamperl_level = level_up_body(level_source, "Clamperl")
    if "MOVE_SHELL_SMASH" not in clamperl_level:
        problems.append("Battle 7: Clamperl cannot legally learn Shell Smash")
    clamperl_eggs = re.search(r"egg_moves\(CLAMPERL,(.*?)\)", read("src/data/pokemon/egg_moves.h"), re.S)
    if not clamperl_eggs or "MOVE_MUDDY_WATER" not in clamperl_eggs.group(1):
        problems.append("Battle 7: Clamperl cannot legally inherit Muddy Water")
    clamperl_tmhm = species_tmhm_body(tmhm_source, "CLAMPERL")
    for move in ("TM13_ICE_BEAM", "TM17_PROTECT"):
        if move not in clamperl_tmhm:
            problems.append(f"Battle 7: Clamperl is missing {move}")

    route104 = read("data/maps/Route104/scripts.inc")
    if "trainerbattle_double TRAINER_DARIAN" not in route104 or "Route104_Text_DarianNotEnoughMons" not in route104:
        problems.append("Battle 7: Darian does not use the native doubles script and two-mon guard")
    darian_dialogue = read("data/text/trainers.inc").split("Route104_Text_DarianIntro:", 1)[1].split("Route105_Text_FosterIntro:", 1)[0]
    for stale in ("Magikarp", "magical quality"):
        if stale in darian_dialogue:
            problems.append(f"Battle 7: Darian dialogue still contains stale text: {stale}")
    for truthful in ("strange little fish", "both of my tricks", "Water Spout", "Clamperl", "two healthy Pokémon"):
        if truthful not in darian_dialogue:
            problems.append(f"Battle 7: Darian dialogue does not explain {truthful}")
    for line in re.findall(r'\.string "([^"]*)"', darian_dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("$", "")
        if len(visible) > 36:
            problems.append(f"Battle 7: Darian dialogue line is too long: {visible}")

    remoraid_sample = json.loads(read("docs/showdown_gen4_random_doubles_30.json"))["samples"][2]
    remoraid_donor = next((mon for mon in remoraid_sample["team"] if mon.get("name") == "Remoraid"), None)
    if not remoraid_donor or remoraid_sample.get("seed") != "103,29237,27956,16507":
        problems.append("Battle 7: exact Remoraid donor sample is missing")
    tabled = json.loads(read("docs/verdant_tabled_mature_battle_concepts.json"))["concepts"]
    if not any(concept.get("former_encounter") == "BATTLE_007_ROUTE_104_DARIAN" for concept in tabled):
        problems.append("Battle 7: mature OCIC quartet was not preserved for later")

    earlier_species = {
        build["species"]
        for trainer_id in (
            "TRAINER_CALVIN_1", "TRAINER_RICK", "TRAINER_ALLEN", "TRAINER_TIANA", "TRAINER_BILLY",
        )
        for build in party_builds(trainer_id, trainers_text, parties_text)
    }
    if earlier_species & {build["species"] for build in expected_darian}:
        problems.append("Battle 7: Darian repeats a species from Battles 2-6")

    cindy = designs["BATTLE_008_ROUTE_104_CINDY"]
    expected_cindy = [
        {
            "level": 2, "species": "SPECIES_BUNEARY", "item": "ITEM_EVIOLITE", "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_FAKE_OUT", "MOVE_RETURN", "MOVE_DRAIN_PUNCH", "MOVE_ENCORE"],
        },
        {
            "level": 1, "species": "SPECIES_GOTHITA", "item": "ITEM_FOCUS_SASH", "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_SPATK_QUIET",
            "moves": ["MOVE_TRICK_ROOM", "MOVE_PSYCHIC", "MOVE_THUNDERBOLT", "MOVE_PROTECT"],
        },
        {
            "level": 3, "species": "SPECIES_MAWILE", "item": "ITEM_LIFE_ORB", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_BRAVE",
            "moves": ["MOVE_PLAY_ROUGH", "MOVE_IRON_HEAD", "MOVE_FIRE_FANG", "MOVE_SUCKER_PUNCH"],
        },
        {
            "level": 2, "species": "SPECIES_FURFROU_DEBUTANTE_TRIM", "item": "ITEM_CHOICE_BAND", "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_DOUBLE_EDGE", "MOVE_U_TURN", "MOVE_ZEN_HEADBUTT", "MOVE_SUCKER_PUNCH"],
        },
    ]
    if cindy["trainer_ids"] != ["TRAINER_CINDY_1"]:
        problems.append("Battle 8: closure is not attached only to Cindy's first battle")
    if party_builds("TRAINER_CINDY_1", trainers_text, parties_text) != expected_cindy:
        problems.append("Battle 8: Cindy's source party differs from the closed design")
    if [build["level"] for build in expected_cindy] != [2, 1, 3, 2]:
        problems.append("Battle 8: Cindy must use the authored 16/15/17/16 progression")
    if cindy.get("evolution_stage_fit", {}).get("status") != "pass":
        problems.append("Battle 8: Cindy's evolution-stage closure is not passing")

    cindy_block = trainer_blocks["TRAINER_CINDY_1"].group(0)
    for token in (".doubleBattle = FALSE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_SPEED_CONTROL"):
        if token not in cindy_block:
            problems.append(f"Battle 8: Cindy is missing {token}")
    for token in ("AI_FLAG_SETUP_FIRST_TURN", "AI_FLAG_HELP_PARTNER", "AI_FLAG_COMBO_SETUP", "AI_FLAG_FIELD_CONTROL"):
        if token in cindy_block:
            problems.append(f"Battle 8: Cindy has an unrelated AI profile: {token}")
    if "trainerbattle_single TRAINER_CINDY_1" not in route104:
        problems.append("Battle 8: Cindy is not preserved as the intended singles pacing battle")

    ability_slots = doubles.base_ability_slots()
    expected_abilities = {
        "SPECIES_BUNEARY": (2, "ABILITY_LIMBER"),
        "SPECIES_GOTHITA": (1, "ABILITY_COMPETITIVE"),
        "SPECIES_MAWILE": (0, "ABILITY_HUGE_POWER"),
        "SPECIES_FURFROU_DEBUTANTE_TRIM": (0, "ABILITY_FUR_COAT"),
    }
    for species, (slot, ability) in expected_abilities.items():
        slots = ability_slots.get(species, [])
        if len(slots) <= slot or slots[slot] != ability:
            problems.append(f"Battle 8: {species} slot {slot} is not {ability}: {slots}")

    egg_source = read("src/data/pokemon/egg_moves.h")
    buneary_eggs = re.search(r"egg_moves\(BUNEARY,(.*?)\)", egg_source, re.S)
    if not buneary_eggs or "MOVE_FAKE_OUT" not in buneary_eggs.group(1):
        problems.append("Battle 8: Buneary cannot legally inherit Fake Out")
    buneary_egg_moves = buneary_eggs.group(1) if buneary_eggs else ""
    buneary_tmhm = species_tmhm_body(tmhm_source, "BUNEARY")
    for move in ("TM27_RETURN", "TM60_DRAIN_PUNCH"):
        if move not in buneary_tmhm:
            problems.append(f"Battle 8: Buneary is missing {move}")
    buneary_level = level_up_body(level_source, "Buneary")
    buneary_has_encore = (
        "MOVE_ENCORE" in buneary_level
        or "MOVE_ENCORE" in buneary_egg_moves
        or (
            "MOVE_ENCORE" in indices
            and species_has_tutor_move(tutor_source, indices, "BUNEARY", "MOVE_ENCORE")
        )
    )
    if not buneary_has_encore:
        problems.append("Battle 8: Buneary cannot legally learn Encore")

    gothita_tmhm = species_tmhm_body(tmhm_source, "GOTHITA")
    for move in ("TM17_PROTECT", "TM24_THUNDERBOLT", "TM29_PSYCHIC", "TM92_TRICK_ROOM"):
        if move not in gothita_tmhm:
            problems.append(f"Battle 8: Gothita is missing {move}")

    mawile_level = level_up_body(level_source, "Mawile")
    mawile_tmhm = species_tmhm_body(tmhm_source, "MAWILE")
    mawile_eggs = re.search(r"egg_moves\(MAWILE,(.*?)\)", egg_source, re.S)
    for move in ("MOVE_PLAY_ROUGH", "MOVE_IRON_HEAD", "MOVE_FIRE_FANG", "MOVE_SUCKER_PUNCH"):
        legal = (
            move in mawile_level
            or (mawile_eggs is not None and move in mawile_eggs.group(1))
            or (move in indices and species_has_tutor_move(tutor_source, indices, "MAWILE", move))
            or move.removeprefix("MOVE_") in mawile_tmhm
        )
        if not legal:
            problems.append(f"Battle 8: Mawile cannot legally learn {move}")

    furfrou_level = level_up_body(level_source, "Furfrou")
    furfrou_tmhm = species_tmhm_body(tmhm_source, "FURFROU_DEBUTANTE_TRIM")
    for move in ("MOVE_DOUBLE_EDGE", "MOVE_U_TURN", "MOVE_ZEN_HEADBUTT", "MOVE_SUCKER_PUNCH"):
        legal = (
            move in furfrou_level
            or (
                move in indices
                and species_has_tutor_move(tutor_source, indices, "FURFROU_DEBUTANTE_TRIM", move)
            )
            or move.removeprefix("MOVE_") in furfrou_tmhm
        )
        if not legal:
            problems.append(f"Battle 8: Debutante Furfrou cannot legally learn {move}")

    cindy_dialogue = read("data/text/trainers.inc").split("Route104_Text_CindyIntro:", 1)[1].split("Route104_Text_CindyRegister1:", 1)[0]
    for truthful in ("partners are dressed", "time turns", "Mawile move first", "Furfrou takes over"):
        if truthful not in cindy_dialogue:
            problems.append(f"Battle 8: Cindy's native singles dialogue lost: {truthful}")
    for line in re.findall(r'\.string "([^"]*)"', cindy_dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("$", "")
        if len(visible) > 36:
            problems.append(f"Battle 8: Cindy dialogue line is too long: {visible}")

    donor_checks = (
        ("docs/showdown_champions_random_doubles_30.json", 6, "7,55433,10898,22765", "Lopunny"),
        ("docs/showdown_gen5_random_singles_30.json", 28, "29,33046,44844,56829", "Gothita"),
        ("docs/showdown_gen7_random_singles_30.json", 23, "24,58986,37129,31214", "Mawile"),
        ("docs/showdown_gen7_random_singles_30.json", 9, "10,13655,15527,51241", "Furfrou"),
    )
    for path, index, seed, species in donor_checks:
        sample = json.loads(read(path))["samples"][index]
        names = {mon.get("name") for mon in sample.get("team", [])}
        if sample.get("seed") != seed or species not in names:
            problems.append(f"Battle 8: {species} donor sample drifted in {path}")

    battles_1_to_7 = {
        build["species"]
        for trainer_id in (
            "TRAINER_CALVIN_1", "TRAINER_RICK", "TRAINER_ALLEN", "TRAINER_TIANA",
            "TRAINER_BILLY", "TRAINER_DARIAN",
        )
        for build in party_builds(trainer_id, trainers_text, parties_text)
    }
    if battles_1_to_7 & {build["species"] for build in expected_cindy}:
        problems.append("Battle 8: Cindy repeats a species from Battles 2-7")

    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))
    print("PASS: Battle 1 groups all six source branches into one encounter")
    print("PASS: all 21 Gen 1-7 starters receive the legal same-trio counter-starter")
    print("PASS: May/Brendan parity, cap-relative level, item, IVs, moves, and Oldale preparation access")
    print("PASS: Battle 2 Calvin party, Illusion ordering, true Timid spread, move legality, native doubles guards, AI flags, and dialogue")
    print("PASS: Battle 3 Rick party, legal donor adaptations, single-Sash/single-setup restraint, singles AI, and dialogue")
    print("PASS: Battle 4 Allen party, partner-aware native doubles, move legality including Volt Tackle, and dialogue")
    print("PASS: Battle 5 Tiana young snow core, legal stages/sets, contextual field/setup AI, native doubles, and dialogue")
    print("PASS: Battle 6 Billy young shoreline mirror, legal stages/sets, reactive-item AI, and dialogue")
    print("PASS: Battle 7 Darian two-cast young fish team, legal stages/sets, Water Spout/Shell Smash AI, and dialogue")
    print("PASS: Battle 8 Cindy temporal singles sequence, legal stages/sets, conditional Trick Room AI, and native dialogue")
    print(f"PASS: all {len(designs)} closed encounters record a full 983-team corpus fit decision")


if __name__ == "__main__":
    main()
