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


def tm_move_indices(source: str) -> dict[str, int]:
    body = source.split("static const u16 sTMHMMoves[] =", 1)[1].split("};", 1)[0]
    moves = re.findall(r"MOVE_[A-Z0-9_]+", body)
    return {move: index for index, move in enumerate(moves)}


def species_has_gen9_tm_move(source: str, indices: dict[str, int], species: str, move: str) -> bool:
    match = re.search(rf"\[SPECIES_{species}\]\s*=\s*\{{([^}}]+)\}}", source)
    if not match or move not in indices:
        return False
    words = [int(value, 16) for value in re.findall(r"0x[0-9A-Fa-f]+", match.group(1))]
    index = indices[move]
    return index // 32 < len(words) and bool(words[index // 32] & (1 << (index % 32)))


def level_up_body(source: str, species_name: str) -> str:
    match = re.search(
        rf"s(?:VerdantGen9)?{species_name}LevelUpLearnset\[\]\s*=\s*\{{(.*?)\}};",
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
    gen9_tm_source = read("src/data/pokemon/verdant_gen9_tmhm_learnsets.h")
    tm_indices = tm_move_indices(read("src/data/party_menu.h"))
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

    level_source = read("src/data/pokemon/level_up_learnsets.h") + "\n" + read("src/data/pokemon/verdant_gen9_level_up_learnsets.h")
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

    lyle = designs["BATTLE_009_PETALBURG_WOODS_LYLE"]
    expected_lyle = [
        {
            "level": 3, "species": "SPECIES_PINECO", "item": "ITEM_NORMAL_GEM", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_BRAVE",
            "moves": ["MOVE_EXPLOSION", "MOVE_GYRO_BALL", "MOVE_BUG_BITE", "MOVE_PROTECT"],
        },
        {
            "level": 1, "species": "SPECIES_DOTTLER", "item": "ITEM_LIGHT_CLAY", "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPDEF_CALM",
            "moves": ["MOVE_REFLECT", "MOVE_LIGHT_SCREEN", "MOVE_PSYCHIC", "MOVE_STRUGGLE_BUG"],
        },
        {
            "level": 2, "species": "SPECIES_DWEBBLE", "item": "ITEM_EVIOLITE", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_ROCK_SLIDE", "MOVE_KNOCK_OFF", "MOVE_X_SCISSOR", "MOVE_PROTECT"],
        },
        {
            "level": 3, "species": "SPECIES_JOLTIK", "item": "ITEM_LIFE_ORB", "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_THUNDER", "MOVE_BUG_BUZZ", "MOVE_ENERGY_BALL", "MOVE_PROTECT"],
        },
    ]
    if lyle["trainer_ids"] != ["TRAINER_LYLE"]:
        problems.append("Battle 9: closure is not attached only to Lyle")
    if party_builds("TRAINER_LYLE", trainers_text, parties_text) != expected_lyle:
        problems.append("Battle 9: Lyle's source party differs from the closed design")
    if [build["level"] for build in expected_lyle] != [3, 1, 2, 3]:
        problems.append("Battle 9: Lyle must use the authored 17/15/16/17 progression")
    if lyle.get("evolution_stage_fit", {}).get("status") != "pass":
        problems.append("Battle 9: Lyle's evolution-stage closure is not passing")

    lyle_block = trainer_blocks["TRAINER_LYLE"].group(0)
    for token in (
        ".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HP_AWARE",
        "AI_FLAG_WILL_SUICIDE", "AI_FLAG_FIELD_CONTROL",
    ):
        if token not in lyle_block:
            problems.append(f"Battle 9: Lyle is missing {token}")
    for token in (
        "AI_FLAG_SPEED_CONTROL", "AI_FLAG_SETUP_FIRST_TURN", "AI_FLAG_HELP_PARTNER",
        "AI_FLAG_COMBO_SETUP", "AI_FLAG_RISKY",
    ):
        if token in lyle_block:
            problems.append(f"Battle 9: Lyle has an unrelated AI profile: {token}")

    woods_source = read("data/maps/PetalburgWoods/scripts.inc")
    if "trainerbattle_double TRAINER_LYLE" not in woods_source or "PetalburgWoods_Text_LyleNotEnoughPokemon" not in woods_source:
        problems.append("Battle 9: Lyle lacks the explicit doubles command or two-mon guard")

    lyle_abilities = {
        "SPECIES_PINECO": (0, "ABILITY_STURDY"),
        "SPECIES_DOTTLER": (2, "ABILITY_TELEPATHY"),
        "SPECIES_DWEBBLE": (0, "ABILITY_STURDY"),
        "SPECIES_JOLTIK": (0, "ABILITY_COMPOUND_EYES"),
    }
    for species, (slot, ability) in lyle_abilities.items():
        slots = ability_slots.get(species, [])
        if len(slots) <= slot or slots[slot] != ability:
            problems.append(f"Battle 9: {species} slot {slot} is not {ability}: {slots}")

    pineco_level = level_up_body(level_source, "Pineco")
    pineco_tmhm = species_tmhm_body(tmhm_source, "PINECO")
    for move, tm in (("MOVE_EXPLOSION", "TM64_EXPLOSION"), ("MOVE_GYRO_BALL", "TM74_GYRO_BALL"), ("MOVE_PROTECT", "TM17_PROTECT")):
        if move not in pineco_level and tm not in pineco_tmhm:
            problems.append(f"Battle 9: Pineco cannot legally learn {move}")
    if not species_has_tutor_move(tutor_source, indices, "PINECO", "MOVE_BUG_BITE"):
        problems.append("Battle 9: Pineco cannot legally learn Bug Bite")

    dottler_level = level_up_body(level_source, "Dottler")
    dottler_tmhm = species_tmhm_body(tmhm_source, "DOTTLER")
    for move, tm in (
        ("MOVE_REFLECT", "TM33_REFLECT"),
        ("MOVE_LIGHT_SCREEN", "TM16_LIGHT_SCREEN"),
        ("MOVE_PSYCHIC", "TM29_PSYCHIC"),
        ("MOVE_STRUGGLE_BUG", "TM76_STRUGGLE_BUG"),
    ):
        if move not in dottler_level and tm not in dottler_tmhm:
            problems.append(f"Battle 9: Dottler cannot legally learn {move}")

    dwebble_level = level_up_body(level_source, "Dwebble")
    dwebble_tmhm = species_tmhm_body(tmhm_source, "DWEBBLE")
    for move, tm in (
        ("MOVE_ROCK_SLIDE", "TM63_ROCK_SLIDE"),
        ("MOVE_X_SCISSOR", "TM81_X_SCISSOR"),
        ("MOVE_PROTECT", "TM17_PROTECT"),
    ):
        if move not in dwebble_level and tm not in dwebble_tmhm:
            problems.append(f"Battle 9: Dwebble cannot legally learn {move}")
    if not species_has_tutor_move(tutor_source, indices, "DWEBBLE", "MOVE_KNOCK_OFF"):
        problems.append("Battle 9: Dwebble cannot legally learn Knock Off")

    joltik_level = level_up_body(level_source, "Joltik")
    joltik_tmhm = species_tmhm_body(tmhm_source, "JOLTIK")
    for move, tm in (
        ("MOVE_THUNDER", "TM25_THUNDER"),
        ("MOVE_ENERGY_BALL", "TM53_ENERGY_BALL"),
        ("MOVE_PROTECT", "TM17_PROTECT"),
    ):
        if move not in joltik_level and tm not in joltik_tmhm:
            problems.append(f"Battle 9: Joltik cannot legally learn {move}")
    if not species_has_tutor_move(tutor_source, indices, "JOLTIK", "MOVE_BUG_BUZZ"):
        problems.append("Battle 9: Joltik cannot legally learn Bug Buzz")

    battle_moves = read("src/data/battle_moves.h")
    explosion = re.search(r"\[MOVE_EXPLOSION\]\s*=\s*\{(.*?)\n\s*\},", battle_moves, re.S)
    if not explosion or "MOVE_TARGET_FOES_AND_ALLY" not in explosion.group(1):
        problems.append("Battle 9: Explosion no longer reaches the ally slot")
    battle_util = read("src/battle_util.c")
    ai_main = read("src/battle_ai_main.c")
    if "GetBattlerAbility(battlerDef) == ABILITY_TELEPATHY && battlerDef == BATTLE_PARTNER(battlerAtk)" not in battle_util:
        problems.append("Battle 9: Telepathy no longer nullifies allied spread damage")
    if "AI_DATA->atkPartnerAbility != ABILITY_TELEPATHY" not in ai_main:
        problems.append("Battle 9: AI no longer recognizes a Telepathy-safe spread move")

    lyle_dialogue = woods_source.split("PetalburgWoods_Text_GoBugPokemonTeam:", 1)[1].split("PetalburgWoods_Text_InstantlyPopularWithBugPokemon:", 1)[0]
    for truthful in ("explosions cannot split us", "Telepathy ignores", "Ghosts and Wide Guard", "two healthy Pokémon"):
        if truthful not in lyle_dialogue:
            problems.append(f"Battle 9: Lyle dialogue does not explain {truthful}")
    for line in re.findall(r'\.string "([^"]*)"', lyle_dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("$", "")
        if len(visible) > 36:
            problems.append(f"Battle 9: Lyle dialogue line is too long: {visible}")

    lyle_donors = (
        ("docs/showdown_gen7_random_doubles_30.json", 3, "4,31676,6269,59824", "Forretress"),
        ("docs/showdown_gen8_random_doubles_30.json", 12, "13,37412,20156,14182", "Orbeetle"),
        ("docs/showdown_gen5_random_singles_30.json", 22, "23,51067,35586,65412", "Dwebble"),
        ("docs/showdown_gen7_random_doubles_30.json", 17, "18,11472,27871,39797", "Galvantula"),
    )
    for path, index, seed, species in lyle_donors:
        sample = json.loads(read(path))["samples"][index]
        names = {mon.get("name") for mon in sample.get("team", [])}
        if sample.get("seed") != seed or species not in names:
            problems.append(f"Battle 9: {species} donor sample drifted in {path}")

    battles_1_to_8 = battles_1_to_7 | {build["species"] for build in expected_cindy}
    if battles_1_to_8 & {build["species"] for build in expected_lyle}:
        problems.append("Battle 9: Lyle repeats a species from Battles 2-8")

    grunt = designs["BATTLE_010_PETALBURG_WOODS_AQUA_GRUNT"]
    expected_grunt = [
        {
            "level": 0, "species": "SPECIES_PURRLOIN", "item": "ITEM_FOCUS_SASH", "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_FAKE_TEARS", "MOVE_THUNDER_WAVE", "MOVE_ENCORE", "MOVE_KNOCK_OFF"],
        },
        {
            "level": 1, "species": "SPECIES_SKRELP", "item": "ITEM_EVIOLITE", "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_SLUDGE_BOMB", "MOVE_SCALD", "MOVE_ICY_WIND", "MOVE_PROTECT"],
        },
        {
            "level": 2, "species": "SPECIES_CORPHISH", "item": "ITEM_CHOICE_BAND", "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_ADAMANT",
            "moves": ["MOVE_CRABHAMMER", "MOVE_KNOCK_OFF", "MOVE_AQUA_JET", "MOVE_SUPERPOWER"],
        },
        {
            "level": 2, "species": "SPECIES_INKAY", "item": "ITEM_LIFE_ORB", "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_ADAMANT",
            "moves": ["MOVE_SUPERPOWER", "MOVE_PSYCHO_CUT", "MOVE_KNOCK_OFF", "MOVE_ROCK_SLIDE"],
        },
    ]
    if grunt["trainer_ids"] != ["TRAINER_GRUNT_PETALBURG_WOODS"]:
        problems.append("Battle 10: closure is not attached only to the first Aqua Grunt")
    if party_builds("TRAINER_GRUNT_PETALBURG_WOODS", trainers_text, parties_text) != expected_grunt:
        problems.append("Battle 10: Aqua Grunt source party differs from the closed design")
    if [build["level"] for build in expected_grunt] != [0, 1, 2, 2]:
        problems.append("Battle 10: Aqua Grunt must use the authored 14/15/16/16 progression")
    if grunt.get("evolution_stage_fit", {}).get("status") != "pass":
        problems.append("Battle 10: Aqua Grunt evolution-stage closure is not passing")

    grunt_block = trainer_blocks["TRAINER_GRUNT_PETALBURG_WOODS"].group(0)
    for token in (
        ".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING",
        "AI_FLAG_SETUP_FIRST_TURN", "AI_FLAG_SPEED_CONTROL",
    ):
        if token not in grunt_block:
            problems.append(f"Battle 10: Aqua Grunt is missing {token}")
    for token in (
        "AI_FLAG_HELP_PARTNER", "AI_FLAG_COMBO_SETUP", "AI_FLAG_FIELD_CONTROL",
        "AI_FLAG_HP_AWARE", "AI_FLAG_WILL_SUICIDE",
    ):
        if token in grunt_block:
            problems.append(f"Battle 10: Aqua Grunt has an unrelated AI profile: {token}")
    if woods_source.count("trainerbattle_no_intro TRAINER_GRUNT_PETALBURG_WOODS") != 2:
        problems.append("Battle 10: both Devon-researcher approach branches must use the same Aqua Grunt")

    grunt_abilities = {
        "SPECIES_PURRLOIN": (2, "ABILITY_PRANKSTER"),
        "SPECIES_SKRELP": (2, "ABILITY_ADAPTABILITY"),
        "SPECIES_CORPHISH": (2, "ABILITY_ADAPTABILITY"),
        "SPECIES_INKAY": (0, "ABILITY_CONTRARY"),
    }
    for species, (slot, ability) in grunt_abilities.items():
        slots = ability_slots.get(species, [])
        if len(slots) <= slot or slots[slot] != ability:
            problems.append(f"Battle 10: {species} slot {slot} is not {ability}: {slots}")

    egg_source = read("src/data/pokemon/egg_moves.h")
    purrloin_level = level_up_body(level_source, "Purrloin")
    purrloin_tmhm = species_tmhm_body(tmhm_source, "PURRLOIN")
    purrloin_eggs = re.search(r"egg_moves\(PURRLOIN,(.*?)\)", egg_source, re.S)
    for move in ("MOVE_FAKE_TEARS", "MOVE_ENCORE"):
        if move not in purrloin_level and (not purrloin_eggs or move not in purrloin_eggs.group(1)):
            problems.append(f"Battle 10: Purrloin cannot legally learn {move}")
    if "TM73_THUNDER_WAVE" not in purrloin_tmhm:
        problems.append("Battle 10: Purrloin cannot legally learn Thunder Wave")
    if not species_has_tutor_move(tutor_source, indices, "PURRLOIN", "MOVE_KNOCK_OFF"):
        problems.append("Battle 10: Purrloin cannot legally learn Knock Off")

    skrelp_level = level_up_body(level_source, "Skrelp")
    skrelp_tmhm = species_tmhm_body(tmhm_source, "SKRELP")
    for move, tm in (
        ("MOVE_SLUDGE_BOMB", "TM36_SLUDGE_BOMB"),
        ("MOVE_SCALD", "TM55_SCALD"),
        ("MOVE_PROTECT", "TM17_PROTECT"),
    ):
        if move not in skrelp_level and tm not in skrelp_tmhm:
            problems.append(f"Battle 10: Skrelp cannot legally learn {move}")
    if not species_has_tutor_move(tutor_source, indices, "SKRELP", "MOVE_ICY_WIND"):
        problems.append("Battle 10: Skrelp cannot legally learn Icy Wind")

    corphish_level = level_up_body(level_source, "Corphish")
    corphish_eggs = re.search(r"egg_moves\(CORPHISH,(.*?)\)", egg_source, re.S)
    if "MOVE_CRABHAMMER" not in corphish_level:
        problems.append("Battle 10: Corphish cannot legally learn Crabhammer")
    if "MOVE_AQUA_JET" not in corphish_level and (not corphish_eggs or "MOVE_AQUA_JET" not in corphish_eggs.group(1)):
        problems.append("Battle 10: Corphish cannot legally learn Aqua Jet")
    for move in ("MOVE_KNOCK_OFF", "MOVE_SUPERPOWER"):
        if not species_has_tutor_move(tutor_source, indices, "CORPHISH", move):
            problems.append(f"Battle 10: Corphish cannot legally learn {move}")

    inkay_level = level_up_body(level_source, "Inkay")
    inkay_tmhm = species_tmhm_body(tmhm_source, "INKAY")
    if "MOVE_PSYCHO_CUT" not in inkay_level:
        problems.append("Battle 10: Inkay cannot legally learn Psycho Cut")
    if "TM63_ROCK_SLIDE" not in inkay_tmhm:
        problems.append("Battle 10: Inkay cannot legally learn Rock Slide")
    for move in ("MOVE_KNOCK_OFF", "MOVE_SUPERPOWER"):
        if move not in inkay_level and not species_has_tutor_move(tutor_source, indices, "INKAY", move):
            problems.append(f"Battle 10: Inkay cannot legally learn {move}")

    grunt_dialogue = woods_source.split("PetalburgWoods_Text_NoOneCrossesTeamAqua:", 1)[1].split("PetalburgWoods_Text_ThatWasAwfullyClose:", 1)[0]
    for truthful in ("Team Aqua", "protect him", "out of", "Rustboro"):
        if truthful not in grunt_dialogue:
            problems.append(f"Battle 10: mandatory story dialogue lost: {truthful}")
    for line in re.findall(r'\.string "([^"]*)"', grunt_dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("$", "")
        if len(visible) > 36:
            problems.append(f"Battle 10: Aqua Grunt dialogue line is too long: {visible}")

    grunt_donors = (
        ("docs/showdown_champions_random_doubles_30.json", 17, "18,11472,27871,39797", "Liepard"),
        ("docs/showdown_gen9_random_doubles_30.json", 6, "7,55433,10898,22765", "Dragalge"),
        ("docs/showdown_gen6_random_doubles_30.json", 26, "27,17208,41758,59690", "Crawdaunt"),
        ("docs/showdown_gen6_random_doubles_30.json", 9, "10,13655,15527,51241", "Malamar"),
    )
    for path, index, seed, species in grunt_donors:
        sample = json.loads(read(path))["samples"][index]
        names = {mon.get("name") for mon in sample.get("team", [])}
        if sample.get("seed") != seed or species not in names:
            problems.append(f"Battle 10: {species} donor sample drifted in {path}")

    battles_1_to_9 = battles_1_to_8 | {build["species"] for build in expected_lyle}
    if battles_1_to_9 & {build["species"] for build in expected_grunt}:
        problems.append("Battle 10: Aqua Grunt repeats a species from Battles 2-9")

    james = designs["BATTLE_011_PETALBURG_WOODS_JAMES"]
    expected_james = [
        {
            "level": 0, "species": "SPECIES_CELEBI", "item": "ITEM_SITRUS_BERRY", "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_PERISH_SONG", "MOVE_PSYCHIC", "MOVE_EARTH_POWER", "MOVE_PROTECT"],
        },
        {
            "level": 2, "species": "SPECIES_SHELMET", "item": "ITEM_EVIOLITE", "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_DEF_SPDEF_SASSY",
            "moves": ["MOVE_INFESTATION", "MOVE_ENCORE", "MOVE_STRUGGLE_BUG", "MOVE_PROTECT"],
        },
        {
            "level": 3, "species": "SPECIES_NINCADA", "item": "ITEM_FOCUS_SASH", "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_X_SCISSOR", "MOVE_DIG", "MOVE_TOXIC", "MOVE_PROTECT"],
        },
        {
            "level": 1, "species": "SPECIES_HERACROSS", "item": "ITEM_CHOICE_SCARF", "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_MEGAHORN", "MOVE_KNOCK_OFF", "MOVE_ROCK_SLIDE", "MOVE_CLOSE_COMBAT"],
        },
    ]
    if james["trainer_ids"] != ["TRAINER_JAMES_1"]:
        problems.append("Battle 11: closure is not attached only to James's first encounter")
    if party_builds("TRAINER_JAMES_1", trainers_text, parties_text) != expected_james:
        problems.append("Battle 11: James's source party differs from the closed design")
    if [build["level"] for build in expected_james] != [0, 2, 3, 1]:
        problems.append("Battle 11: James must use the authored 14/16/17/15 progression")
    if james.get("evolution_stage_fit", {}).get("status") != "pass":
        problems.append("Battle 11: James's evolution-stage closure is not passing")

    james_block = trainer_blocks["TRAINER_JAMES_1"].group(0)
    for token in (".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_PERISH_TRAP"):
        if token not in james_block:
            problems.append(f"Battle 11: James is missing {token}")
    for token in (
        "AI_FLAG_SETUP_FIRST_TURN", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL",
        "AI_FLAG_COMBO_SETUP", "AI_FLAG_HP_AWARE", "AI_FLAG_WILL_SUICIDE",
    ):
        if token in james_block:
            problems.append(f"Battle 11: James has an unrelated AI profile: {token}")
    if "trainerbattle_double TRAINER_JAMES_1" not in woods_source or "PetalburgWoods_Text_JamesNotEnoughPokemon" not in woods_source:
        problems.append("Battle 11: James lacks the explicit doubles command or two-mon guard")

    james_abilities = {
        "SPECIES_CELEBI": (0, "ABILITY_NATURAL_CURE"),
        "SPECIES_SHELMET": (2, "ABILITY_OVERCOAT"),
        "SPECIES_NINCADA": (0, "ABILITY_COMPOUND_EYES"),
        "SPECIES_HERACROSS": (2, "ABILITY_MOXIE"),
    }
    for species, (slot, ability) in james_abilities.items():
        slots = ability_slots.get(species, [])
        if len(slots) <= slot or slots[slot] != ability:
            problems.append(f"Battle 11: {species} slot {slot} is not {ability}: {slots}")

    celebi_level = level_up_body(level_source, "Celebi")
    celebi_tmhm = species_tmhm_body(tmhm_source, "CELEBI")
    if "MOVE_PERISH_SONG" not in celebi_level:
        problems.append("Battle 11: Celebi cannot legally learn Perish Song")
    for tm in ("TM17_PROTECT", "TM29_PSYCHIC"):
        if tm not in celebi_tmhm:
            problems.append(f"Battle 11: Celebi is missing {tm}")
    if not species_has_tutor_move(tutor_source, indices, "CELEBI", "MOVE_EARTH_POWER"):
        problems.append("Battle 11: Celebi cannot legally learn Earth Power")

    shelmet_level = level_up_body(level_source, "Shelmet")
    shelmet_tmhm = species_tmhm_body(tmhm_source, "SHELMET")
    shelmet_eggs = re.search(r"egg_moves\(SHELMET,(.*?)\)", egg_source, re.S)
    if not species_has_tutor_move(tutor_source, indices, "SHELMET", "MOVE_INFESTATION"):
        problems.append("Battle 11: Shelmet cannot legally learn Infestation")
    if (
        "MOVE_ENCORE" not in shelmet_level
        and (not shelmet_eggs or "MOVE_ENCORE" not in shelmet_eggs.group(1))
        and not species_has_tutor_move(tutor_source, indices, "SHELMET", "MOVE_ENCORE")
    ):
        problems.append("Battle 11: Shelmet cannot legally learn Encore")
    if "MOVE_STRUGGLE_BUG" not in shelmet_level and "TM76_STRUGGLE_BUG" not in shelmet_tmhm:
        problems.append("Battle 11: Shelmet cannot legally learn Struggle Bug")
    if "MOVE_PROTECT" not in shelmet_level and "TM17_PROTECT" not in shelmet_tmhm:
        problems.append("Battle 11: Shelmet cannot legally learn Protect")

    nincada_level = level_up_body(level_source, "Nincada")
    nincada_tmhm = species_tmhm_body(tmhm_source, "NINCADA")
    for move, tm in (
        ("MOVE_X_SCISSOR", "TM81_X_SCISSOR"),
        ("MOVE_DIG", "TM28_DIG"),
        ("MOVE_TOXIC", "TM06_TOXIC"),
        ("MOVE_PROTECT", "TM17_PROTECT"),
    ):
        if move not in nincada_level and tm not in nincada_tmhm:
            problems.append(f"Battle 11: Nincada cannot legally learn {move}")

    heracross_level = level_up_body(level_source, "Heracross")
    heracross_tmhm = species_tmhm_body(tmhm_source, "HERACROSS")
    for move in ("MOVE_MEGAHORN", "MOVE_CLOSE_COMBAT"):
        if move not in heracross_level and not species_has_tutor_move(tutor_source, indices, "HERACROSS", move):
            problems.append(f"Battle 11: Heracross cannot legally learn {move}")
    if not species_has_tutor_move(tutor_source, indices, "HERACROSS", "MOVE_KNOCK_OFF"):
        problems.append("Battle 11: Heracross cannot legally learn Knock Off")
    if "TM63_ROCK_SLIDE" not in heracross_tmhm:
        problems.append("Battle 11: Heracross cannot legally learn Rock Slide")

    if not all(
        token in ai_main
        for token in (
            "static s16 AI_PerishTrap", "HasTrappingMoveEffect(partner)",
            "IsTrappingMoveEffect(effect) && (targetPerishing || partnerStartingPerish)",
        )
    ):
        problems.append("Battle 11: reusable Perish-trap coordination is incomplete")
    ai_switch = read("src/battle_ai_switch_items.c")
    for token in ("perishSongTimer <= 1", "CountUsablePartyMons(gActiveBattler) > 0", "!IsBattlerTrapped(gActiveBattler, TRUE)"):
        if token not in ai_switch:
            problems.append(f"Battle 11: Perish switch safety is missing {token}")

    james_dialogue = woods_source.split("PetalburgWoods_Text_InstantlyPopularWithBugPokemon:", 1)[1].split("PetalburgWoods_Text_StayOutOfTallGrass:", 1)[0]
    for truthful in ("forest spirit", "fading song", "Infestation traps", "Perish Song", "two healthy Pokémon", "grow up"):
        if truthful not in james_dialogue:
            problems.append(f"Battle 11: James dialogue does not explain or preserve {truthful}")
    for line in re.findall(r'\.string "([^"]*)"', james_dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("$", "")
        if len(visible) > 36:
            problems.append(f"Battle 11: James dialogue line is too long: {visible}")

    for token in (
        "special HealPlayerParty", "addpcitem ITEM_EXP_SHARE, 1",
        "Your Bag is full, so I sent the", "Exp. Share to your PC.",
    ):
        if token not in woods_source:
            problems.append(f"Battle 11: post-Grunt preparation guarantee is missing {token}")

    james_donors = (
        ("docs/showdown_gen8_random_doubles_30.json", 11, "12,29493,18613,48380", "Celebi"),
        ("docs/showdown_gen6_random_doubles_30.json", 8, "9,5736,13984,19904", "Accelgor"),
        ("docs/showdown_gen5_random_doubles_30.json", 22, "123,56547,58816,53432", "Nincada"),
        ("docs/showdown_champions_random_doubles_30.json", 16, "17,3553,26328,8460", "Heracross"),
    )
    for path, index, seed, species in james_donors:
        sample = json.loads(read(path))["samples"][index]
        names = {mon.get("name") for mon in sample.get("team", [])}
        if sample.get("seed") != seed or species not in names:
            problems.append(f"Battle 11: {species} donor sample drifted in {path}")

    battles_1_to_10 = battles_1_to_9 | {build["species"] for build in expected_grunt}
    if battles_1_to_10 & {build["species"] for build in expected_james}:
        problems.append("Battle 11: James repeats a species from Battles 2-10")

    winston = designs["BATTLE_012_ROUTE_104_WINSTON"]
    expected_winston = [
        {
            "level": 1, "species": "SPECIES_STONJOURNER", "item": "ITEM_FOCUS_SASH", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_ROCK_SLIDE", "MOVE_BODY_PRESS", "MOVE_WIDE_GUARD", "MOVE_PROTECT"],
        },
        {
            "level": 1, "species": "SPECIES_PORYGON", "item": "ITEM_EVIOLITE", "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_TRI_ATTACK", "MOVE_ICE_BEAM", "MOVE_THUNDERBOLT", "MOVE_PROTECT"],
        },
        {
            "level": 2, "species": "SPECIES_DEDENNE", "item": "ITEM_SITRUS_BERRY", "ability_slot": 2,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_SUPER_FANG", "MOVE_NUZZLE", "MOVE_DAZZLING_GLEAM", "MOVE_THUNDERBOLT"],
        },
        {
            "level": 3, "species": "SPECIES_HONEDGE", "item": "ITEM_AIR_BALLOON", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_BRAVE",
            "moves": ["MOVE_SWORDS_DANCE", "MOVE_IRON_HEAD", "MOVE_SACRED_SWORD", "MOVE_SHADOW_SNEAK"],
        },
    ]
    if winston["trainer_ids"] != ["TRAINER_WINSTON_1"]:
        problems.append("Battle 12: closure is not attached only to Winston's first encounter")
    if party_builds("TRAINER_WINSTON_1", trainers_text, parties_text) != expected_winston:
        problems.append("Battle 12: Winston's source party differs from the closed design")
    if [build["level"] for build in expected_winston] != [1, 1, 2, 3]:
        problems.append("Battle 12: Winston must use the authored 15/15/16/17 progression")
    if winston.get("evolution_stage_fit", {}).get("status") != "pass":
        problems.append("Battle 12: Winston's evolution-stage closure is not passing")

    winston_block = trainer_blocks["TRAINER_WINSTON_1"].group(0)
    for token in (".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_SPEED_CONTROL"):
        if token not in winston_block:
            problems.append(f"Battle 12: Winston is missing {token}")
    for token in (
        "AI_FLAG_SETUP_FIRST_TURN", "AI_FLAG_HELP_PARTNER", "AI_FLAG_COMBO_SETUP",
        "AI_FLAG_FIELD_CONTROL", "AI_FLAG_HP_AWARE", "AI_FLAG_WILL_SUICIDE",
    ):
        if token in winston_block:
            problems.append(f"Battle 12: Winston has an unrelated AI profile: {token}")
    route104 = read("data/maps/Route104/scripts.inc")
    if "trainerbattle_double TRAINER_WINSTON_1" not in route104 or "Route104_Text_WinstonNotEnoughPokemon" not in route104:
        problems.append("Battle 12: Winston lacks the explicit doubles command or two-mon guard")

    winston_abilities = {
        "SPECIES_STONJOURNER": (0, "ABILITY_POWER_SPOT"),
        "SPECIES_PORYGON": (1, "ABILITY_DOWNLOAD"),
        "SPECIES_DEDENNE": (2, "ABILITY_CHEEK_POUCH"),
        "SPECIES_HONEDGE": (0, "ABILITY_NO_GUARD"),
    }
    for species, (slot, ability) in winston_abilities.items():
        slots = ability_slots.get(species, [])
        if len(slots) <= slot or slots[slot] != ability:
            problems.append(f"Battle 12: {species} slot {slot} is not {ability}: {slots}")

    stonjourner_level = level_up_body(level_source, "Stonjourner")
    stonjourner_tmhm = species_tmhm_body(tmhm_source, "STONJOURNER")
    for move, tm in (("MOVE_ROCK_SLIDE", "TM63_ROCK_SLIDE"), ("MOVE_PROTECT", "TM17_PROTECT")):
        if move not in stonjourner_level and tm not in stonjourner_tmhm:
            problems.append(f"Battle 12: Stonjourner cannot legally learn {move}")
    for move in ("MOVE_BODY_PRESS", "MOVE_WIDE_GUARD"):
        if move not in stonjourner_level and not species_has_tutor_move(tutor_source, indices, "STONJOURNER", move):
            problems.append(f"Battle 12: Stonjourner cannot legally learn {move}")

    porygon_level = level_up_body(level_source, "Porygon")
    porygon_tmhm = species_tmhm_body(tmhm_source, "PORYGON")
    if "MOVE_TRI_ATTACK" not in porygon_level:
        problems.append("Battle 12: Porygon cannot legally learn Tri Attack")
    for tm in ("TM13_ICE_BEAM", "TM17_PROTECT", "TM24_THUNDERBOLT"):
        if tm not in porygon_tmhm:
            problems.append(f"Battle 12: Porygon is missing {tm}")

    dedenne_level = level_up_body(level_source, "Dedenne")
    dedenne_tmhm = species_tmhm_body(tmhm_source, "DEDENNE")
    if "MOVE_NUZZLE" not in dedenne_level:
        problems.append("Battle 12: Dedenne cannot legally learn Nuzzle")
    for tm in ("TM24_THUNDERBOLT", "TM99_DAZZLING_GLEAM"):
        if tm not in dedenne_tmhm:
            problems.append(f"Battle 12: Dedenne is missing {tm}")
    if not species_has_tutor_move(tutor_source, indices, "DEDENNE", "MOVE_SUPER_FANG"):
        problems.append("Battle 12: Dedenne cannot legally learn Super Fang")

    honedge_level = level_up_body(level_source, "Honedge")
    honedge_tmhm = species_tmhm_body(tmhm_source, "HONEDGE")
    honedge_eggs = re.search(r"egg_moves\(HONEDGE,(.*?)\)", egg_source, re.S)
    for move in ("MOVE_SWORDS_DANCE", "MOVE_IRON_HEAD", "MOVE_SACRED_SWORD", "MOVE_SHADOW_SNEAK"):
        legal = (
            move in honedge_level
            or (honedge_eggs is not None and move in honedge_eggs.group(1))
            or (move in indices and species_has_tutor_move(tutor_source, indices, "HONEDGE", move))
            or move.removeprefix("MOVE_") in honedge_tmhm
        )
        if not legal:
            problems.append(f"Battle 12: Honedge cannot legally learn {move}")

    power_spot = re.search(r"case ABILITY_POWER_SPOT:(.*?)break;", battle_util, re.S)
    if not power_spot or "UQ_4_12(1.3)" not in power_spot.group(1):
        problems.append("Battle 12: Power Spot no longer supplies its 1.3x partner multiplier")

    winston_dialogue = read("data/text/trainers.inc").split("Route104_Text_WinstonIntro:", 1)[1].split("Route104_Text_CindyIntro:", 1)[0]
    for truthful in ("rarest Pokémon", "Power Spot", "not victory", "two healthy Pokémon"):
        if truthful not in winston_dialogue:
            problems.append(f"Battle 12: Winston dialogue does not explain or preserve {truthful}")
    for line in re.findall(r'\.string "([^"]*)"', winston_dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("$", "")
        if len(visible) > 36:
            problems.append(f"Battle 12: Winston dialogue line is too long: {visible}")

    winston_donors = (
        ("docs/showdown_gen8_random_doubles_30.json", 14, "15,53250,23242,11321", "Stonjourner"),
        ("docs/showdown_gen8_random_doubles_30.json", 13, "14,45331,21699,45519", "Porygon2"),
        ("docs/showdown_gen9_random_doubles_30.json", 5, "6,47514,9355,56963", "Dedenne"),
        ("docs/showdown_gen8_random_singles_30.json", 14, "15,53250,23242,11321", "Doublade"),
    )
    for path, index, seed, species in winston_donors:
        sample = json.loads(read(path))["samples"][index]
        names = {mon.get("name") for mon in sample.get("team", [])}
        if sample.get("seed") != seed or species not in names:
            problems.append(f"Battle 12: {species} donor sample drifted in {path}")

    battles_1_to_11 = battles_1_to_10 | {build["species"] for build in expected_james}
    if battles_1_to_11 & {build["species"] for build in expected_winston}:
        problems.append("Battle 12: Winston repeats a species from Battles 2-11")

    haley = designs["BATTLE_013_ROUTE_104_HALEY"]
    expected_haley = [
        {
            "level": 1, "species": "SPECIES_EEVEE", "item": "ITEM_EVIOLITE", "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_YAWN", "MOVE_PROTECT", "MOVE_EXTREME_SPEED", "MOVE_BITE"],
        },
        {
            "level": 2, "species": "SPECIES_RALTS", "item": "ITEM_FOCUS_SASH", "ability_slot": 1,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_ENCORE", "MOVE_PSYCHIC", "MOVE_DAZZLING_GLEAM", "MOVE_THUNDERBOLT"],
        },
        {
            "level": 2, "species": "SPECIES_SNORUNT", "item": "ITEM_CHOICE_SPECS", "ability_slot": 0,
            "spread": "SPREAD_HP_FIGHTING_TIMID",
            "moves": ["MOVE_ICE_BEAM", "MOVE_HIDDEN_POWER", "MOVE_SHADOW_BALL", "MOVE_WATER_PULSE"],
        },
        {
            "level": 3, "species": "SPECIES_MORPEKO", "item": "ITEM_LIFE_ORB", "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_AURA_WHEEL", "MOVE_PARTING_SHOT", "MOVE_SEED_BOMB", "MOVE_PROTECT"],
        },
    ]
    if haley["trainer_ids"] != ["TRAINER_HALEY_1"]:
        problems.append("Battle 13: closure is not attached only to Haley's first encounter")
    if party_builds("TRAINER_HALEY_1", trainers_text, parties_text) != expected_haley:
        problems.append("Battle 13: Haley's source party differs from the closed design")
    if [build["level"] for build in expected_haley] != [1, 2, 2, 3]:
        problems.append("Battle 13: Haley must use the authored 15/16/16/17 progression")
    if haley.get("evolution_stage_fit", {}).get("status") != "pass":
        problems.append("Battle 13: Haley's evolution-stage closure is not passing")

    haley_block = trainer_blocks["TRAINER_HALEY_1"].group(0)
    for token in (".doubleBattle = FALSE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_SETUP_FIRST_TURN", "AI_FLAG_HP_AWARE"):
        if token not in haley_block:
            problems.append(f"Battle 13: Haley is missing {token}")
    for token in (
        "AI_FLAG_SPEED_CONTROL", "AI_FLAG_HELP_PARTNER", "AI_FLAG_COMBO_SETUP",
        "AI_FLAG_FIELD_CONTROL", "AI_FLAG_PERISH_TRAP", "AI_FLAG_WILL_SUICIDE",
    ):
        if token in haley_block:
            problems.append(f"Battle 13: Haley has an unrelated AI profile: {token}")
    if "trainerbattle_single TRAINER_HALEY_1" not in route104:
        problems.append("Battle 13: Haley is not preserved as the intended singles pacing battle")

    haley_abilities = {
        "SPECIES_EEVEE": (1, "ABILITY_ADAPTABILITY"),
        "SPECIES_RALTS": (1, "ABILITY_TRACE"),
        "SPECIES_SNORUNT": (0, "ABILITY_INNER_FOCUS"),
        "SPECIES_MORPEKO": (0, "ABILITY_HUNGER_SWITCH"),
    }
    for species, (slot, ability) in haley_abilities.items():
        slots = ability_slots.get(species, [])
        if len(slots) <= slot or slots[slot] != ability:
            problems.append(f"Battle 13: {species} slot {slot} is not {ability}: {slots}")

    eevee_level = level_up_body(level_source, "Eevee")
    eevee_tmhm = species_tmhm_body(tmhm_source, "EEVEE")
    eevee_eggs = re.search(r"egg_moves\(EEVEE,(.*?)\)", egg_source, re.S)
    for move in ("MOVE_YAWN", "MOVE_EXTREME_SPEED", "MOVE_BITE"):
        legal = (
            move in eevee_level
            or (eevee_eggs is not None and move in eevee_eggs.group(1))
            or (move in indices and species_has_tutor_move(tutor_source, indices, "EEVEE", move))
        )
        if not legal:
            problems.append(f"Battle 13: Eevee cannot legally learn {move}")
    if "TM17_PROTECT" not in eevee_tmhm:
        problems.append("Battle 13: Eevee cannot legally learn Protect")

    ralts_level = level_up_body(level_source, "Ralts")
    ralts_tmhm = species_tmhm_body(tmhm_source, "RALTS")
    ralts_eggs = re.search(r"egg_moves\(RALTS,(.*?)\)", egg_source, re.S)
    if (
        "MOVE_ENCORE" not in ralts_level
        and (not ralts_eggs or "MOVE_ENCORE" not in ralts_eggs.group(1))
        and not ("MOVE_ENCORE" in indices and species_has_tutor_move(tutor_source, indices, "RALTS", "MOVE_ENCORE"))
    ):
        problems.append("Battle 13: Ralts cannot legally learn Encore")
    for tm in ("TM24_THUNDERBOLT", "TM29_PSYCHIC", "TM99_DAZZLING_GLEAM"):
        if tm not in ralts_tmhm:
            problems.append(f"Battle 13: Ralts is missing {tm}")

    snorunt_level = level_up_body(level_source, "Snorunt")
    snorunt_tmhm = species_tmhm_body(tmhm_source, "SNORUNT")
    snorunt_eggs = re.search(r"egg_moves\(SNORUNT,(.*?)\)", egg_source, re.S)
    for move, tm in (
        ("MOVE_ICE_BEAM", "TM13_ICE_BEAM"),
        ("MOVE_HIDDEN_POWER", "TM10_HIDDEN_POWER"),
        ("MOVE_SHADOW_BALL", "TM30_SHADOW_BALL"),
        ("MOVE_WATER_PULSE", "TM03_WATER_PULSE"),
    ):
        legal = (
            move in snorunt_level
            or (snorunt_eggs is not None and move in snorunt_eggs.group(1))
            or (tm is not None and tm in snorunt_tmhm)
            or (move in indices and species_has_tutor_move(tutor_source, indices, "SNORUNT", move))
        )
        if not legal:
            problems.append(f"Battle 13: Snorunt cannot legally learn {move}")

    morpeko_level = level_up_body(level_source, "Morpeko")
    morpeko_tmhm = species_tmhm_body(tmhm_source, "MORPEKO")
    morpeko_eggs = re.search(r"egg_moves\(MORPEKO,(.*?)\)", egg_source, re.S)
    for move in ("MOVE_AURA_WHEEL", "MOVE_PARTING_SHOT"):
        if move not in morpeko_level and (not morpeko_eggs or move not in morpeko_eggs.group(1)):
            problems.append(f"Battle 13: Morpeko cannot legally learn {move}")
    if "TM17_PROTECT" not in morpeko_tmhm:
        problems.append("Battle 13: Morpeko cannot legally learn Protect")
    if not species_has_tutor_move(tutor_source, indices, "MORPEKO", "MOVE_SEED_BOMB"):
        problems.append("Battle 13: Morpeko cannot legally learn Seed Bomb")

    haley_dialogue = read("data/text/trainers.inc").split("Route104_Text_HaleyIntro:", 1)[1].split("Route104_Text_WinstonIntro:", 1)[0]
    for truthful in ("team will help me choose", "wrong choice", "Morpeko changes", "changing my mind"):
        if truthful not in haley_dialogue:
            problems.append(f"Battle 13: Haley dialogue does not explain {truthful}")
    for line in re.findall(r'\.string "([^"]*)"', haley_dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("$", "")
        if len(visible) > 36:
            problems.append(f"Battle 13: Haley dialogue line is too long: {visible}")
    if 'gText_MatchCallLass_Haley_Strategy[] = _("I keep my options open!")' not in read("src/data/text/match_call_messages.h"):
        problems.append("Battle 13: Haley's Match Call strategy is stale")

    haley_donors = (
        ("docs/showdown_gen5_random_singles_30.json", 0, "1,7919,1640,31348", "Eevee"),
        ("docs/showdown_gen4_random_doubles_30.json", 27, "128,30607,996,13512", "Ralts"),
        ("docs/showdown_gen4_random_singles_30.json", 17, "18,11472,27871,39797", "Snorunt"),
        ("docs/showdown_gen9_random_doubles_30.json", 0, "1,7919,1640,31348", "Morpeko"),
    )
    for path, index, seed, species in haley_donors:
        sample = json.loads(read(path))["samples"][index]
        names = {mon.get("name") for mon in sample.get("team", [])}
        if sample.get("seed") != seed or species not in names:
            problems.append(f"Battle 13: {species} donor sample drifted in {path}")

    battles_1_to_12 = battles_1_to_11 | {build["species"] for build in expected_winston}
    if battles_1_to_12 & {build["species"] for build in expected_haley}:
        problems.append("Battle 13: Haley repeats a species from Battles 2-12")

    gina_mia = designs["BATTLE_014_ROUTE_104_GINA_MIA"]
    expected_gina_mia = [
        {
            "level": 2, "species": "SPECIES_ORICORIO", "item": "ITEM_FLYING_GEM", "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_HASTY",
            "moves": ["MOVE_ACROBATICS", "MOVE_REVELATION_DANCE", "MOVE_ROOST", "MOVE_PROTECT"],
        },
        {
            "level": 1, "species": "SPECIES_AXEW", "item": "ITEM_EVIOLITE", "ability_slot": 1,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_DRAGON_DANCE", "MOVE_DRAGON_CLAW", "MOVE_POISON_JAB", "MOVE_PROTECT"],
        },
        {
            "level": 2, "species": "SPECIES_CUTIEFLY", "item": "ITEM_FOCUS_SASH", "ability_slot": 2,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_QUIVER_DANCE", "MOVE_BUG_BUZZ", "MOVE_DAZZLING_GLEAM", "MOVE_PROTECT"],
        },
        {
            "level": 3, "species": "SPECIES_ODDISH", "item": "ITEM_BLACK_SLUDGE", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_PETAL_DANCE", "MOVE_SLUDGE_BOMB", "MOVE_GIGA_DRAIN", "MOVE_PROTECT"],
        },
    ]
    if gina_mia["trainer_ids"] != ["TRAINER_GINA_AND_MIA_1"]:
        problems.append("Battle 14: closure is not attached to the shared twin encounter")
    if party_builds("TRAINER_GINA_AND_MIA_1", trainers_text, parties_text) != expected_gina_mia:
        problems.append("Battle 14: Gina & Mia's source party differs from the closed design")
    if [build["level"] for build in expected_gina_mia] != [2, 1, 2, 3]:
        problems.append("Battle 14: Gina & Mia must use the authored 16/15/16/17 progression")
    if gina_mia.get("evolution_stage_fit", {}).get("status") != "pass":
        problems.append("Battle 14: Gina & Mia's evolution-stage closure is not passing")

    gina_block = trainer_blocks["TRAINER_GINA_AND_MIA_1"].group(0)
    for token in (
        ".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING",
        "AI_FLAG_SETUP_FIRST_TURN", "AI_FLAG_COMBO_SETUP",
    ):
        if token not in gina_block:
            problems.append(f"Battle 14: Gina & Mia are missing {token}")
    for token in (
        "AI_FLAG_SPEED_CONTROL", "AI_FLAG_PERISH_TRAP", "AI_FLAG_WILL_SUICIDE",
        "AI_FLAG_HP_AWARE", "AI_FLAG_FIELD_CONTROL",
    ):
        if token in gina_block:
            problems.append(f"Battle 14: Gina & Mia have an unrelated AI profile: {token}")
    if route104.count("trainerbattle_double TRAINER_GINA_AND_MIA_1") != 2:
        problems.append("Battle 14: both overworld twins do not share the native doubles guard")

    gina_abilities = {
        "SPECIES_ORICORIO": (0, "ABILITY_DANCER"),
        "SPECIES_AXEW": (1, "ABILITY_MOLD_BREAKER"),
        "SPECIES_CUTIEFLY": (2, "ABILITY_SWEET_VEIL"),
        "SPECIES_ODDISH": (0, "ABILITY_CHLOROPHYLL"),
    }
    for species, (slot, ability) in gina_abilities.items():
        slots = ability_slots.get(species, [])
        if len(slots) <= slot or slots[slot] != ability:
            problems.append(f"Battle 14: {species} slot {slot} is not {ability}: {slots}")

    oricorio_level = level_up_body(level_source, "Oricorio")
    oricorio_tmhm = species_tmhm_body(tmhm_source, "ORICORIO")
    for move in ("MOVE_REVELATION_DANCE", "MOVE_ROOST"):
        if move not in oricorio_level and not species_has_tutor_move(tutor_source, indices, "ORICORIO", move):
            problems.append(f"Battle 14: Oricorio cannot legally learn {move}")
    for tm in ("TM17_PROTECT", "TM62_ACROBATICS"):
        if tm not in oricorio_tmhm:
            problems.append(f"Battle 14: Oricorio is missing {tm}")

    axew_level = level_up_body(level_source, "Axew")
    axew_tmhm = species_tmhm_body(tmhm_source, "AXEW")
    if "MOVE_DRAGON_DANCE" not in axew_level:
        problems.append("Battle 14: Axew cannot legally learn Dragon Dance")
    for tm in ("TM02_DRAGON_CLAW", "TM17_PROTECT", "TM84_POISON_JAB"):
        if tm not in axew_tmhm:
            problems.append(f"Battle 14: Axew is missing {tm}")

    cutiefly_level = level_up_body(level_source, "Cutiefly")
    cutiefly_tmhm = species_tmhm_body(tmhm_source, "CUTIEFLY")
    for move in ("MOVE_QUIVER_DANCE", "MOVE_BUG_BUZZ"):
        if move not in cutiefly_level:
            problems.append(f"Battle 14: Cutiefly cannot legally learn {move}")
    for tm in ("TM17_PROTECT", "TM99_DAZZLING_GLEAM"):
        if tm not in cutiefly_tmhm:
            problems.append(f"Battle 14: Cutiefly is missing {tm}")

    oddish_level = level_up_body(level_source, "Oddish")
    oddish_tmhm = species_tmhm_body(tmhm_source, "ODDISH")
    for move in ("MOVE_PETAL_DANCE", "MOVE_GIGA_DRAIN"):
        if move not in oddish_level:
            problems.append(f"Battle 14: Oddish cannot legally learn {move}")
    for tm in ("TM17_PROTECT", "TM36_SLUDGE_BOMB"):
        if tm not in oddish_tmhm:
            problems.append(f"Battle 14: Oddish is missing {tm}")

    combo_ai = read("src/battle_ai_main.c")
    if not all(token in combo_ai for token in (
        "partnerAbility == ABILITY_DANCER",
        "TestMoveFlags(move, FLAG_DANCE)",
        "score += 12",
    )):
        problems.append("Battle 14: the combo AI does not value a dance beside Dancer")

    gina_dialogue = read("data/text/trainers.inc").split("Route104_Text_GinaIntro:", 1)[1].split("Route104_Text_IvanIntro:", 1)[0]
    for truthful in ("Dragon Dance", "Quiver Dance", "follows each dance", "Taunt or focused attacks"):
        if truthful not in gina_dialogue:
            problems.append(f"Battle 14: twin dialogue does not explain {truthful}")
    for line in re.findall(r'\.string "([^"]*)"', gina_dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("$", "")
        if len(visible) > 36:
            problems.append(f"Battle 14: twin dialogue line is too long: {visible}")

    gina_donors = (
        ("docs/showdown_gen9_random_doubles_30.json", 21, "22,43148,34043,34075", {"Oricorio", "Lilligant"}),
        ("docs/showdown_gen9_random_doubles_30.json", 19, "20,27310,30957,36936", {"Oricorio"}),
        ("docs/showdown_gen5_random_doubles_30.json", 2, "103,29237,27956,16507", {"Axew"}),
    )
    for path, index, seed, species in gina_donors:
        sample = json.loads(read(path))["samples"][index]
        names = {mon.get("name") for mon in sample.get("team", [])}
        if sample.get("seed") != seed or not species <= names:
            problems.append(f"Battle 14: donor sample drifted in {path} sample {index + 1}")

    battles_1_to_13 = battles_1_to_12 | {build["species"] for build in expected_haley}
    if battles_1_to_13 & {build["species"] for build in expected_gina_mia}:
        problems.append("Battle 14: Gina & Mia repeat a species from Battles 2-13")
    if "regional-merida-2025 / regional-curitiba-2024 / regional-liverpool-2023" not in json.dumps(gina_mia):
        problems.append("Battle 14: the rejected Dondozo/Tatsugiri concept is not preserved for later")

    ivan = designs["BATTLE_015_ROUTE_104_IVAN"]
    expected_ivan = [
        {
            "level": 2, "species": "SPECIES_LUVDISC", "item": "ITEM_FOCUS_SASH", "ability_slot": 1,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_SURF", "MOVE_ENDEAVOR", "MOVE_FLIP_TURN", "MOVE_ICE_BEAM"],
        },
        {
            "level": 2, "species": "SPECIES_STUNFISK", "item": "ITEM_LEFTOVERS", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_DISCHARGE", "MOVE_EARTH_POWER", "MOVE_SCALD", "MOVE_PAIN_SPLIT"],
        },
        {
            "level": 3, "species": "SPECIES_TENTACOOL", "item": "ITEM_EVIOLITE", "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_SLUDGE_BOMB", "MOVE_SCALD", "MOVE_ICE_BEAM", "MOVE_GIGA_DRAIN"],
        },
        {
            "level": 6, "species": "SPECIES_WISHIWASHI", "item": "ITEM_ASSAULT_VEST", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_MIXED",
            "moves": ["MOVE_LIQUIDATION", "MOVE_ICE_BEAM", "MOVE_EARTHQUAKE", "MOVE_U_TURN"],
        },
    ]
    if ivan["trainer_ids"] != ["TRAINER_IVAN"]:
        problems.append("Battle 15: closure is not attached only to Ivan")
    if party_builds("TRAINER_IVAN", trainers_text, parties_text) != expected_ivan:
        problems.append("Battle 15: Ivan's source party differs from the closed design")
    if [build["level"] for build in expected_ivan] != [2, 2, 3, 6]:
        problems.append("Battle 15: Ivan must use the authored 16/16/17/20 progression")
    if ivan.get("evolution_stage_fit", {}).get("status") != "pass":
        problems.append("Battle 15: Ivan's evolution-stage closure is not passing")

    ivan_block = trainer_blocks["TRAINER_IVAN"].group(0)
    for token in (".doubleBattle = FALSE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HP_AWARE"):
        if token not in ivan_block:
            problems.append(f"Battle 15: Ivan is missing {token}")
    for token in (
        "AI_FLAG_SPEED_CONTROL", "AI_FLAG_SETUP_FIRST_TURN", "AI_FLAG_HELP_PARTNER",
        "AI_FLAG_COMBO_SETUP", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_PERISH_TRAP", "AI_FLAG_WILL_SUICIDE",
    ):
        if token in ivan_block:
            problems.append(f"Battle 15: Ivan has an unrelated AI profile: {token}")
    if "trainerbattle_single TRAINER_IVAN" not in route104:
        problems.append("Battle 15: Ivan is not preserved as the intended talk-only single")

    ivan_abilities = {
        "SPECIES_LUVDISC": (1, "ABILITY_SOUL_HEART"),
        "SPECIES_STUNFISK": (0, "ABILITY_STATIC"),
        "SPECIES_TENTACOOL": (1, "ABILITY_LIQUID_OOZE"),
        "SPECIES_WISHIWASHI": (0, "ABILITY_SCHOOLING"),
    }
    for species, (slot, ability) in ivan_abilities.items():
        slots = ability_slots.get(species, [])
        if len(slots) <= slot or slots[slot] != ability:
            problems.append(f"Battle 15: {species} slot {slot} is not {ability}: {slots}")

    luvdisc_tmhm = species_tmhm_body(tmhm_source, "LUVDISC")
    for tm in ("TM13_ICE_BEAM", "HM03_SURF"):
        if tm not in luvdisc_tmhm:
            problems.append(f"Battle 15: Luvdisc is missing {tm}")
    for move in ("MOVE_ENDEAVOR", "MOVE_FLIP_TURN"):
        if not species_has_tutor_move(tutor_source, indices, "LUVDISC", move):
            problems.append(f"Battle 15: Luvdisc cannot legally learn {move}")

    stunfisk_level = level_up_body(level_source, "Stunfisk")
    stunfisk_tmhm = species_tmhm_body(tmhm_source, "STUNFISK")
    if "MOVE_DISCHARGE" not in stunfisk_level:
        problems.append("Battle 15: Stunfisk cannot legally learn Discharge")
    if "TM55_SCALD" not in stunfisk_tmhm:
        problems.append("Battle 15: Stunfisk is missing TM55_SCALD")
    for move in ("MOVE_EARTH_POWER", "MOVE_PAIN_SPLIT"):
        if not species_has_tutor_move(tutor_source, indices, "STUNFISK", move):
            problems.append(f"Battle 15: Stunfisk cannot legally learn {move}")

    tentacool_tmhm = species_tmhm_body(tmhm_source, "TENTACOOL")
    for tm in ("TM13_ICE_BEAM", "TM19_GIGA_DRAIN", "TM36_SLUDGE_BOMB", "TM55_SCALD"):
        if tm not in tentacool_tmhm:
            problems.append(f"Battle 15: Tentacool is missing {tm}")

    wishiwashi_tmhm = species_tmhm_body(tmhm_source, "WISHIWASHI")
    for tm in ("TM13_ICE_BEAM", "TM26_EARTHQUAKE", "TM89_U_TURN"):
        if tm not in wishiwashi_tmhm:
            problems.append(f"Battle 15: Wishiwashi is missing {tm}")
    if not species_has_tutor_move(tutor_source, indices, "WISHIWASHI", "MOVE_LIQUIDATION"):
        problems.append("Battle 15: Wishiwashi cannot legally learn Liquidation")

    schooling = read("src/battle_util.c")
    for token in (
        "{ABILITY_SCHOOLING, SPECIES_WISHIWASHI_SCHOOL, SPECIES_WISHIWASHI, 4}",
        "if (gBattleMons[battler].level < 20)",
        "gBattleMons[battler].hp <= gBattleMons[battler].maxHP / forms[i][3]",
    ):
        if token not in schooling:
            problems.append(f"Battle 15: Schooling threshold rule drifted: {token}")

    ivan_dialogue = read("data/text/trainers.inc").split("Route104_Text_IvanIntro:", 1)[1].split("Route104_Text_BillyIntro:", 1)[0]
    for truthful in ("right lure", "Electric bite", "hooks Grass", "schools at level 20", "one-quarter HP"):
        if truthful not in ivan_dialogue:
            problems.append(f"Battle 15: Ivan dialogue does not explain {truthful}")
    for line in re.findall(r'\.string "([^"]*)"', ivan_dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("$", "")
        if len(visible) > 36:
            problems.append(f"Battle 15: Ivan dialogue line is too long: {visible}")

    ivan_donors = (
        ("docs/showdown_gen9_random_singles_30.json", 7, "8,63352,12441,54102", {"Luvdisc"}),
        ("docs/showdown_gen6_random_singles_30.json", 11, "12,29493,18613,48380", {"Luvdisc", "Stunfisk"}),
        ("docs/showdown_gen4_random_singles_30.json", 29, "30,40965,46387,22631", {"Tentacruel"}),
        ("docs/showdown_gen6_random_singles_30.json", 4, "5,39595,7812,25626", {"Tentacruel"}),
        ("docs/showdown_gen7_random_singles_30.json", 13, "14,45331,21699,45519", {"Wishiwashi"}),
    )
    for path, index, seed, species in ivan_donors:
        sample = json.loads(read(path))["samples"][index]
        names = {mon.get("name") for mon in sample.get("team", [])}
        if sample.get("seed") != seed or not species <= names:
            problems.append(f"Battle 15: donor sample drifted in {path} sample {index + 1}")

    battles_1_to_14 = battles_1_to_13 | {build["species"] for build in expected_gina_mia}
    if battles_1_to_14 & {build["species"] for build in expected_ivan}:
        problems.append("Battle 15: Ivan repeats a species from Battles 2-14")
    if "regional-merida-2025 / regional-curitiba-2024 / regional-liverpool-2023" not in json.dumps(ivan):
        problems.append("Battle 15: the reserved Dondozo/Tatsugiri concept was lost")

    josh = designs["BATTLE_016_RUSTBORO_GYM_JOSH"]
    expected_josh = [
        {
            "level": 1, "species": "SPECIES_SHUCKLE", "item": "ITEM_MENTAL_HERB", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_DEF_BOLD",
            "moves": ["MOVE_GUARD_SPLIT", "MOVE_HELPING_HAND", "MOVE_ROCK_TOMB", "MOVE_PROTECT"],
        },
        {
            "level": 3, "species": "SPECIES_CRANIDOS", "item": "ITEM_SITRUS_BERRY", "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_ROCK_SLIDE", "MOVE_ZEN_HEADBUTT", "MOVE_FIRE_PUNCH", "MOVE_PROTECT"],
        },
        {
            "level": 2, "species": "SPECIES_LILEEP", "item": "ITEM_EVIOLITE", "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_GIGA_DRAIN", "MOVE_ANCIENT_POWER", "MOVE_EARTH_POWER", "MOVE_STOCKPILE"],
        },
        {
            "level": 2, "species": "SPECIES_GLIMMET", "item": "ITEM_FOCUS_SASH", "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_POWER_GEM", "MOVE_SLUDGE_BOMB", "MOVE_ROCK_POLISH", "MOVE_PROTECT"],
        },
    ]
    if josh["trainer_ids"] != ["TRAINER_JOSH"]:
        problems.append("Battle 16: closure is not attached only to Josh")
    if party_builds("TRAINER_JOSH", trainers_text, parties_text) != expected_josh:
        problems.append("Battle 16: Josh's source party differs from the closed design")
    if [build["level"] for build in expected_josh] != [1, 3, 2, 2]:
        problems.append("Battle 16: Josh must use the authored 15/17/16/16 progression")
    if josh.get("evolution_stage_fit", {}).get("status") != "pass":
        problems.append("Battle 16: Josh's evolution-stage closure is not passing")

    josh_block = trainer_blocks["TRAINER_JOSH"].group(0)
    for token in (
        ".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER",
        "AI_FLAG_SETUP_FIRST_TURN", "AI_FLAG_COMBO_SETUP", "AI_FLAG_SPEED_CONTROL",
    ):
        if token not in josh_block:
            problems.append(f"Battle 16: Josh is missing {token}")
    for token in ("AI_FLAG_HP_AWARE", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_PERISH_TRAP", "AI_FLAG_WILL_SUICIDE"):
        if token in josh_block:
            problems.append(f"Battle 16: Josh has an unrelated AI profile: {token}")

    rustboro_gym = read("data/maps/RustboroCity_Gym/scripts.inc")
    if "trainerbattle_double TRAINER_JOSH" not in rustboro_gym or "RustboroCity_Gym_Text_JoshNotEnoughPokemon" not in rustboro_gym:
        problems.append("Battle 16: Josh lacks the native doubles command or two-mon guard")

    josh_abilities = {
        "SPECIES_SHUCKLE": (0, "ABILITY_STURDY"),
        "SPECIES_CRANIDOS": (2, "ABILITY_SHEER_FORCE"),
        "SPECIES_LILEEP": (2, "ABILITY_STORM_DRAIN"),
        "SPECIES_GLIMMET": (0, "ABILITY_TOXIC_DEBRIS"),
    }
    for species, (slot, ability) in josh_abilities.items():
        slots = ability_slots.get(species, [])
        if len(slots) <= slot or slots[slot] != ability:
            problems.append(f"Battle 16: {species} slot {slot} is not {ability}: {slots}")

    shuckle_level = level_up_body(level_source, "Shuckle")
    shuckle_tmhm = species_tmhm_body(tmhm_source, "SHUCKLE")
    if "MOVE_GUARD_SPLIT" not in shuckle_level:
        problems.append("Battle 16: Shuckle cannot legally learn Guard Split")
    if not species_has_tutor_move(tutor_source, indices, "SHUCKLE", "MOVE_HELPING_HAND"):
        problems.append("Battle 16: Shuckle cannot legally learn Helping Hand")
    for tm in ("TM17_PROTECT", "TM39_ROCK_TOMB"):
        if tm not in shuckle_tmhm:
            problems.append(f"Battle 16: Shuckle is missing {tm}")

    cranidos_tmhm = species_tmhm_body(tmhm_source, "CRANIDOS")
    for tm in ("TM17_PROTECT", "TM63_ROCK_SLIDE"):
        if tm not in cranidos_tmhm:
            problems.append(f"Battle 16: Cranidos is missing {tm}")
    for move in ("MOVE_ZEN_HEADBUTT", "MOVE_FIRE_PUNCH"):
        if not species_has_tutor_move(tutor_source, indices, "CRANIDOS", move):
            problems.append(f"Battle 16: Cranidos cannot legally learn {move}")

    lileep_level = level_up_body(level_source, "Lileep")
    lileep_tmhm = species_tmhm_body(tmhm_source, "LILEEP")
    for move in ("MOVE_ANCIENT_POWER", "MOVE_GIGA_DRAIN", "MOVE_STOCKPILE"):
        if move not in lileep_level:
            problems.append(f"Battle 16: Lileep cannot legally learn {move}")
    if not species_has_tutor_move(tutor_source, indices, "LILEEP", "MOVE_EARTH_POWER"):
        problems.append("Battle 16: Lileep cannot legally learn Earth Power")
    if "TM19_GIGA_DRAIN" not in lileep_tmhm:
        problems.append("Battle 16: Lileep is missing TM19_GIGA_DRAIN")

    glimmet_level = level_up_body(level_source, "Glimmet")
    for move in ("MOVE_POWER_GEM", "MOVE_ROCK_POLISH"):
        if move not in glimmet_level:
            problems.append(f"Battle 16: Glimmet cannot legally learn {move}")
    for move in ("MOVE_SLUDGE_BOMB", "MOVE_PROTECT"):
        if not species_has_gen9_tm_move(gen9_tm_source, tm_indices, "GLIMMET", move):
            problems.append(f"Battle 16: Glimmet cannot legally learn {move}")

    guard_split_ai = read("src/battle_ai_main.c")
    for token in (
        "A defensive donor can deliberately lend bulk to a frail",
        "case EFFECT_GUARD_SPLIT:",
        "> gBattleMons[battlerDef].defense + gBattleMons[battlerDef].spDefense",
        "score += 12",
    ):
        if token not in guard_split_ai:
            problems.append(f"Battle 16: Guard Split partner AI drifted: {token}")

    josh_dialogue = rustboro_gym.split("RustboroCity_Gym_Text_JoshIntro:", 1)[1].split("RustboroCity_Gym_Text_TommyIntro:", 1)[0]
    for truthful in ("share your defenses", "Guard Split lends", "Storm Drain", "Toxic Spikes", "two healthy Pokémon"):
        if truthful not in josh_dialogue:
            problems.append(f"Battle 16: Josh dialogue does not explain {truthful}")
    for line in re.findall(r'\.string "([^"]*)"', josh_dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("$", "")
        if len(visible) > 36:
            problems.append(f"Battle 16: Josh dialogue line is too long: {visible}")

    josh_donors = (
        ("docs/showdown_gen8_random_doubles_30.json", 15, "16,61169,24785,42658", {"Shuckle"}),
        ("docs/showdown_champions_random_doubles_30.json", 22, "23,51067,35586,65412", {"Rampardos"}),
        ("docs/showdown_gen9_random_doubles_30.json", 23, "24,58986,37129,31214", {"Rampardos"}),
        ("docs/showdown_gen4_random_doubles_30.json", 3, "104,37156,29499,47844", {"Lileep"}),
        ("docs/showdown_gen5_random_doubles_30.json", 7, "108,3297,35671,42122", {"Cradily"}),
    )
    for path, index, seed, species in josh_donors:
        sample = json.loads(read(path))["samples"][index]
        names = {mon.get("name") for mon in sample.get("team", [])}
        if sample.get("seed") != seed or not species <= names:
            problems.append(f"Battle 16: donor sample drifted in {path} sample {index + 1}")

    smogon = json.loads(read("docs/smogon_gen4_9_ou_uu_nu_sample_teams.json"))["formats"]["gen9ou"]
    for index in (0, 1):
        if "Glimmora" not in {mon.get("species") for mon in smogon[index].get("data", [])}:
            problems.append(f"Battle 16: Smogon gen9ou sample {index + 1} lost Glimmora")

    battles_1_to_15 = battles_1_to_14 | {build["species"] for build in expected_ivan}
    if battles_1_to_15 & {build["species"] for build in expected_josh}:
        problems.append("Battle 16: Josh repeats a species from Battles 2-15")

    tommy = designs["BATTLE_017_RUSTBORO_GYM_TOMMY"]
    expected_tommy = [
        {
            "level": 1, "species": "SPECIES_ORANGURU", "item": "ITEM_SITRUS_BERRY", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_DEF_BOLD",
            "moves": ["MOVE_INSTRUCT", "MOVE_PSYCHIC", "MOVE_FOUL_PLAY", "MOVE_PROTECT"],
        },
        {
            "level": 3, "species": "SPECIES_TYRUNT", "item": "ITEM_LIFE_ORB", "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_ROCK_SLIDE", "MOVE_DRAGON_CLAW", "MOVE_FIRE_FANG", "MOVE_PROTECT"],
        },
        {
            "level": 2, "species": "SPECIES_NOSEPASS", "item": "ITEM_EVIOLITE", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_SPATK_MODEST",
            "moves": ["MOVE_WIDE_GUARD", "MOVE_THUNDER_WAVE", "MOVE_POWER_GEM", "MOVE_EARTH_POWER"],
        },
        {
            "level": 3, "species": "SPECIES_BINACLE", "item": "ITEM_EXPERT_BELT", "ability_slot": 0,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_RAZOR_SHELL", "MOVE_CROSS_CHOP", "MOVE_ROCK_SLIDE", "MOVE_PROTECT"],
        },
    ]
    if tommy["trainer_ids"] != ["TRAINER_TOMMY"]:
        problems.append("Battle 17: closure is not attached only to Tommy")
    if party_builds("TRAINER_TOMMY", trainers_text, parties_text) != expected_tommy:
        problems.append("Battle 17: Tommy's source party differs from the closed design")
    if [build["level"] for build in expected_tommy] != [1, 3, 2, 3]:
        problems.append("Battle 17: Tommy must use the authored 15/17/16/17 progression")
    if tommy.get("evolution_stage_fit", {}).get("status") != "pass":
        problems.append("Battle 17: Tommy's evolution-stage closure is not passing")

    tommy_block = trainer_blocks["TRAINER_TOMMY"].group(0)
    for token in (
        ".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER",
        "AI_FLAG_COMBO_SETUP", "AI_FLAG_SPEED_CONTROL",
    ):
        if token not in tommy_block:
            problems.append(f"Battle 17: Tommy is missing {token}")
    for token in (
        "AI_FLAG_SETUP_FIRST_TURN", "AI_FLAG_HP_AWARE", "AI_FLAG_FIELD_CONTROL",
        "AI_FLAG_PERISH_TRAP", "AI_FLAG_WILL_SUICIDE",
    ):
        if token in tommy_block:
            problems.append(f"Battle 17: Tommy has an unrelated AI profile: {token}")
    if "trainerbattle_double TRAINER_TOMMY" not in rustboro_gym or "RustboroCity_Gym_Text_TommyNotEnoughPokemon" not in rustboro_gym:
        problems.append("Battle 17: Tommy lacks the native doubles command or two-mon guard")

    tommy_abilities = {
        "SPECIES_ORANGURU": (0, "ABILITY_INNER_FOCUS"),
        "SPECIES_TYRUNT": (0, "ABILITY_STRONG_JAW"),
        "SPECIES_NOSEPASS": (0, "ABILITY_STURDY"),
        "SPECIES_BINACLE": (0, "ABILITY_TOUGH_CLAWS"),
    }
    for species, (slot, ability) in tommy_abilities.items():
        slots = ability_slots.get(species, [])
        if len(slots) <= slot or slots[slot] != ability:
            problems.append(f"Battle 17: {species} slot {slot} is not {ability}: {slots}")

    oranguru_level = level_up_body(level_source, "Oranguru")
    oranguru_tmhm = species_tmhm_body(tmhm_source, "ORANGURU")
    for move in ("MOVE_INSTRUCT", "MOVE_PSYCHIC", "MOVE_FOUL_PLAY"):
        if move not in oranguru_level:
            problems.append(f"Battle 17: Oranguru cannot legally learn {move}")
    if "TM17_PROTECT" not in oranguru_tmhm:
        problems.append("Battle 17: Oranguru is missing TM17_PROTECT")

    tyrunt_level = level_up_body(level_source, "Tyrunt")
    tyrunt_tmhm = species_tmhm_body(tmhm_source, "TYRUNT")
    if "MOVE_DRAGON_CLAW" not in tyrunt_level:
        problems.append("Battle 17: Tyrunt cannot legally learn Dragon Claw")
    if not species_has_tutor_move(tutor_source, indices, "TYRUNT", "MOVE_FIRE_FANG"):
        problems.append("Battle 17: Tyrunt cannot legally learn Fire Fang")
    for tm in ("TM17_PROTECT", "TM63_ROCK_SLIDE"):
        if tm not in tyrunt_tmhm:
            problems.append(f"Battle 17: Tyrunt is missing {tm}")

    nosepass_level = level_up_body(level_source, "Nosepass")
    nosepass_eggs = re.search(r"egg_moves\(NOSEPASS,(.*?)\)", egg_source, re.S)
    for move in ("MOVE_THUNDER_WAVE", "MOVE_POWER_GEM", "MOVE_EARTH_POWER"):
        if move not in nosepass_level:
            problems.append(f"Battle 17: Nosepass cannot legally learn {move}")
    if not nosepass_eggs or "MOVE_WIDE_GUARD" not in nosepass_eggs.group(1):
        problems.append("Battle 17: Nosepass cannot legally learn Wide Guard")

    binacle_level = level_up_body(level_source, "Binacle")
    binacle_tmhm = species_tmhm_body(tmhm_source, "BINACLE")
    for move in ("MOVE_RAZOR_SHELL", "MOVE_CROSS_CHOP"):
        if move not in binacle_level:
            problems.append(f"Battle 17: Binacle cannot legally learn {move}")
    for tm in ("TM17_PROTECT", "TM63_ROCK_SLIDE"):
        if tm not in binacle_tmhm:
            problems.append(f"Battle 17: Binacle is missing {tm}")

    instruct_ai = read("src/battle_ai_main.c")
    for token in (
        "case EFFECT_INSTRUCT:",
        "gBattleMoves[instructedMove].target & (MOVE_TARGET_BOTH | MOVE_TARGET_FOES_AND_ALLY)",
        "effect == EFFECT_INSTRUCT",
        "gBattleMoves[AI_DATA->partnerMove].target & (MOVE_TARGET_BOTH | MOVE_TARGET_FOES_AND_ALLY)",
    ):
        if token not in instruct_ai:
            problems.append(f"Battle 17: Instruct partner AI drifted: {token}")

    tommy_dialogue = rustboro_gym.split("RustboroCity_Gym_Text_TommyIntro:", 1)[1].split("RustboroCity_Gym_Text_MarcIntro:", 1)[0]
    for truthful in ("worth repeating", "make it do that again", "Instruct makes my partner repeat", "Wide Guard", "two healthy Pokémon"):
        if truthful not in tommy_dialogue:
            problems.append(f"Battle 17: Tommy dialogue does not explain {truthful}")
    for line in re.findall(r'\.string "([^"]*)"', tommy_dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("$", "")
        if len(visible) > 36:
            problems.append(f"Battle 17: Tommy dialogue line is too long: {visible}")

    tommy_donors = (
        ("docs/showdown_gen8_random_doubles_30.json", 11, "12,29493,18613,48380", {"Oranguru"}),
        ("docs/showdown_gen6_random_doubles_30.json", 26, "27,17208,41758,59690", {"Tyrantrum"}),
        ("docs/showdown_gen7_random_doubles_30.json", 1, "2,15838,3183,62685", {"Probopass"}),
        ("docs/showdown_gen6_random_doubles_30.json", 23, "24,58986,37129,31214", {"Barbaracle"}),
    )
    for path, index, seed, species in tommy_donors:
        sample = json.loads(read(path))["samples"][index]
        names = {mon.get("name") for mon in sample.get("team", [])}
        if sample.get("seed") != seed or not species <= names:
            problems.append(f"Battle 17: donor sample drifted in {path} sample {index + 1}")

    battles_1_to_16 = battles_1_to_15 | {build["species"] for build in expected_josh}
    if battles_1_to_16 & {build["species"] for build in expected_tommy}:
        problems.append("Battle 17: Tommy repeats a species from Battles 2-16")

    marc = designs["BATTLE_018_RUSTBORO_GYM_MARC"]
    expected_marc = [
        {
            "level": 1, "species": "SPECIES_SHIELDON", "item": "ITEM_ROCKY_HELMET", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_DEF_SPDEF_SASSY",
            "moves": ["MOVE_STEALTH_ROCK", "MOVE_METAL_BURST", "MOVE_ROCK_TOMB", "MOVE_IRON_HEAD"],
        },
        {
            "level": 2, "species": "SPECIES_WOOBAT", "item": "ITEM_CHOICE_SCARF", "ability_slot": 0,
            "spread": "SPREAD_31_IV_SPATK_SPEED_TIMID",
            "moves": ["MOVE_TRICK", "MOVE_U_TURN", "MOVE_PSYCHIC", "MOVE_AIR_SLASH"],
        },
        {
            "level": 2, "species": "SPECIES_CORSOLA_GALARIAN", "item": "ITEM_EVIOLITE", "ability_slot": 2,
            "spread": "SPREAD_31_IV_HP_DEF_BOLD",
            "moves": ["MOVE_STRENGTH_SAP", "MOVE_WILL_O_WISP", "MOVE_HEX", "MOVE_HAZE"],
        },
        {
            "level": 3, "species": "SPECIES_AERODACTYL", "item": "ITEM_LIFE_ORB", "ability_slot": 2,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_STONE_EDGE", "MOVE_EARTHQUAKE", "MOVE_DUAL_WINGBEAT", "MOVE_AQUA_TAIL"],
        },
    ]
    if marc["trainer_ids"] != ["TRAINER_MARC"]:
        problems.append("Battle 18: closure is not attached only to Marc")
    if party_builds("TRAINER_MARC", trainers_text, parties_text) != expected_marc:
        problems.append("Battle 18: Marc's source party differs from the closed design")
    if [build["level"] for build in expected_marc] != [1, 2, 2, 3]:
        problems.append("Battle 18: Marc must use the authored 15/16/16/17 progression")
    if marc.get("evolution_stage_fit", {}).get("status") != "pass":
        problems.append("Battle 18: Marc's evolution-stage closure is not passing")

    marc_block = trainer_blocks["TRAINER_MARC"].group(0)
    for token in (".doubleBattle = FALSE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_SETUP_FIRST_TURN", "AI_FLAG_HP_AWARE"):
        if token not in marc_block:
            problems.append(f"Battle 18: Marc is missing {token}")
    for token in (
        "AI_FLAG_HELP_PARTNER", "AI_FLAG_COMBO_SETUP", "AI_FLAG_SPEED_CONTROL",
        "AI_FLAG_FIELD_CONTROL", "AI_FLAG_PERISH_TRAP", "AI_FLAG_WILL_SUICIDE",
    ):
        if token in marc_block:
            problems.append(f"Battle 18: Marc has an unrelated AI profile: {token}")
    if "trainerbattle_single TRAINER_MARC" not in rustboro_gym:
        problems.append("Battle 18: Marc is not preserved as the intended singles pacing battle")

    marc_abilities = {
        "SPECIES_SHIELDON": (0, "ABILITY_STURDY"),
        "SPECIES_WOOBAT": (0, "ABILITY_UNAWARE"),
        "SPECIES_CORSOLA_GALARIAN": (2, "ABILITY_CURSED_BODY"),
        "SPECIES_AERODACTYL": (2, "ABILITY_UNNERVE"),
    }
    for species, (slot, ability) in marc_abilities.items():
        slots = ability_slots.get(species, [])
        if len(slots) <= slot or slots[slot] != ability:
            problems.append(f"Battle 18: {species} slot {slot} is not {ability}: {slots}")

    shieldon_level = level_up_body(level_source, "Shieldon")
    shieldon_tmhm = species_tmhm_body(tmhm_source, "SHIELDON")
    for move in ("MOVE_METAL_BURST",):
        if move not in shieldon_level:
            problems.append(f"Battle 18: Shieldon cannot legally learn {move}")
    for move in ("MOVE_STEALTH_ROCK", "MOVE_ROCK_TOMB"):
        if move.removeprefix("MOVE_") not in shieldon_tmhm:
            problems.append(f"Battle 18: Shieldon cannot legally learn {move}")
    if not species_has_tutor_move(tutor_source, indices, "SHIELDON", "MOVE_IRON_HEAD"):
        problems.append("Battle 18: Shieldon cannot legally learn Iron Head")

    woobat_level = level_up_body(level_source, "Woobat")
    woobat_tmhm = species_tmhm_body(tmhm_source, "WOOBAT")
    for move in ("MOVE_PSYCHIC", "MOVE_AIR_SLASH"):
        if move not in woobat_level:
            problems.append(f"Battle 18: Woobat cannot legally learn {move}")
    if not species_has_tutor_move(tutor_source, indices, "WOOBAT", "MOVE_TRICK"):
        problems.append("Battle 18: Woobat cannot legally learn Trick")
    if "TM89_U_TURN" not in woobat_tmhm:
        problems.append("Battle 18: Woobat is missing TM89_U_TURN")

    corsola_level = level_up_body(level_source, "CorsolaGalarian")
    corsola_tmhm = species_tmhm_body(tmhm_source, "CORSOLA_GALARIAN")
    corsola_eggs = re.search(r"egg_moves\(CORSOLA_GALARIAN,(.*?)\)", egg_source, re.S)
    for move in ("MOVE_STRENGTH_SAP", "MOVE_HEX"):
        if move not in corsola_level:
            problems.append(f"Battle 18: Galarian Corsola cannot legally learn {move}")
    if "TM61_WILL_O_WISP" not in corsola_tmhm:
        problems.append("Battle 18: Galarian Corsola is missing TM61_WILL_O_WISP")
    if not corsola_eggs or "MOVE_HAZE" not in corsola_eggs.group(1):
        problems.append("Battle 18: Galarian Corsola cannot legally learn Haze")

    aerodactyl_tmhm = species_tmhm_body(tmhm_source, "AERODACTYL")
    for tm in ("TM26_EARTHQUAKE", "TM71_STONE_EDGE"):
        if tm not in aerodactyl_tmhm:
            problems.append(f"Battle 18: Aerodactyl is missing {tm}")
    for move in ("MOVE_DUAL_WINGBEAT", "MOVE_AQUA_TAIL"):
        if not species_has_tutor_move(tutor_source, indices, "AERODACTYL", move):
            problems.append(f"Battle 18: Aerodactyl cannot legally learn {move}")

    marc_dialogue = rustboro_gym.split("RustboroCity_Gym_Text_MarcIntro:", 1)[1].split("RustboroCity_Gym_Text_RoxanneIntro:", 1)[0]
    for truthful in ("layer underfoot", "Shieldon scatters sharp stones", "trades away its Scarf", "Cursed Body seals"):
        if truthful not in marc_dialogue:
            problems.append(f"Battle 18: Marc dialogue does not explain {truthful}")
    for line in re.findall(r'\.string "([^"]*)"', marc_dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            problems.append(f"Battle 18: Marc dialogue line is too long: {visible}")

    marc_donors = (
        ("docs/showdown_gen5_random_doubles_30.json", 16, "117,9033,49558,62015", {"Grumpig", "Golett"}),
        ("docs/showdown_gen8_random_singles_30.json", 27, "28,25127,43301,25492", {"Corsola"}),
        ("docs/showdown_gen4_random_singles_30.json", 28, "29,33046,44844,56829", {"Meditite"}),
    )
    for path, index, seed, species in marc_donors:
        sample = json.loads(read(path))["samples"][index]
        names = {mon.get("name") for mon in sample.get("team", [])}
        if sample.get("seed") != seed or not species <= names:
            problems.append(f"Battle 18: donor sample drifted in {path} sample {index + 1}")

    for fmt, index in (("gen8nu", 3), ("gen7nu", 2)):
        sample = json.loads(read("docs/smogon_gen4_9_ou_uu_nu_sample_teams.json"))["formats"][fmt][index]
        if "Aerodactyl" not in {mon.get("species") for mon in sample.get("data", [])}:
            problems.append(f"Battle 18: Smogon {fmt} sample {index + 1} lost Aerodactyl")

    battles_1_to_17 = battles_1_to_16 | {build["species"] for build in expected_tommy}
    if battles_1_to_17 & {build["species"] for build in expected_marc}:
        problems.append("Battle 18: Marc repeats a species from Battles 2-17")

    roxanne = designs["BATTLE_019_RUSTBORO_GYM_ROXANNE"]
    expected_roxanne = [
        {
            "level": 3, "species": "SPECIES_KLEFKI", "item": "ITEM_MENTAL_HERB", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_DEF_SPDEF_SASSY",
            "moves": ["MOVE_SAFEGUARD", "MOVE_SWAGGER", "MOVE_FOUL_PLAY", "MOVE_PROTECT"],
        },
        {
            "level": 3, "species": "SPECIES_ROCKRUFF", "item": "ITEM_EVIOLITE", "ability_slot": 1,
            "spread": "SPREAD_31_IV_ATK_SPEED_JOLLY",
            "moves": ["MOVE_ROCK_SLIDE", "MOVE_PSYCHIC_FANGS", "MOVE_SUCKER_PUNCH", "MOVE_PROTECT"],
        },
        {
            "level": 4, "species": "SPECIES_MUDBRAY", "item": "ITEM_SITRUS_BERRY", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_HIGH_HORSEPOWER", "MOVE_ROCK_SLIDE", "MOVE_HEAVY_SLAM", "MOVE_PROTECT"],
        },
        {
            "level": 4, "species": "SPECIES_BONSLY", "item": "ITEM_LIFE_ORB", "ability_slot": 1,
            "spread": "SPREAD_31_IV_HP_ATK_BRAVE",
            "moves": ["MOVE_ROCK_SLIDE", "MOVE_DOUBLE_EDGE", "MOVE_LOW_KICK", "MOVE_SUCKER_PUNCH"],
        },
        {
            "level": 4, "species": "SPECIES_MARACTUS", "item": "ITEM_ASSAULT_VEST", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_SEED_BOMB", "MOVE_SUCKER_PUNCH", "MOVE_DRAIN_PUNCH", "MOVE_POISON_JAB"],
        },
        {
            "level": 5, "species": "SPECIES_REGIROCK", "item": "ITEM_EXPERT_BELT", "ability_slot": 0,
            "spread": "SPREAD_31_IV_HP_ATK_ADAMANT",
            "moves": ["MOVE_ROCK_SLIDE", "MOVE_DRAIN_PUNCH", "MOVE_THUNDER_PUNCH", "MOVE_IRON_HEAD"],
        },
    ]
    if roxanne["trainer_ids"] != ["TRAINER_ROXANNE_1"]:
        problems.append("Battle 19: closure is not attached only to Roxanne's first battle")
    if party_builds("TRAINER_ROXANNE_1", trainers_text, parties_text) != expected_roxanne:
        problems.append("Battle 19: Roxanne's source party differs from the closed design")
    if [build["level"] for build in expected_roxanne] != [3, 3, 4, 4, 4, 5]:
        problems.append("Battle 19: Roxanne must use the authored 17/17/18/18/18/19 progression")
    if roxanne.get("evolution_stage_fit", {}).get("status") != "pass":
        problems.append("Battle 19: Roxanne's evolution-stage closure is not passing")
    if roxanne.get("manual_difficulty") != 10.0:
        problems.append("Battle 19: Roxanne is not closed at boss difficulty 10/10")

    roxanne_block = trainer_blocks["TRAINER_ROXANNE_1"].group(0)
    for token in (
        ".doubleBattle = TRUE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_HELP_PARTNER",
        "AI_FLAG_SETUP_FIRST_TURN", "AI_FLAG_COMBO_SETUP", "AI_FLAG_SPEED_CONTROL",
    ):
        if token not in roxanne_block:
            problems.append(f"Battle 19: Roxanne is missing {token}")
    for token in ("AI_FLAG_HP_AWARE", "AI_FLAG_FIELD_CONTROL", "AI_FLAG_PERISH_TRAP", "AI_FLAG_WILL_SUICIDE"):
        if token in roxanne_block:
            problems.append(f"Battle 19: Roxanne has an unrelated AI profile: {token}")
    if "trainerbattle_double TRAINER_ROXANNE_1" not in rustboro_gym or "RustboroCity_Gym_Text_RoxanneNotEnoughPokemon" not in rustboro_gym:
        problems.append("Battle 19: Roxanne lacks the native doubles command or two-mon guard")

    roxanne_abilities = {
        "SPECIES_KLEFKI": (0, "ABILITY_PRANKSTER"),
        "SPECIES_ROCKRUFF": (1, "ABILITY_VITAL_SPIRIT"),
        "SPECIES_MUDBRAY": (0, "ABILITY_OWN_TEMPO"),
        "SPECIES_BONSLY": (1, "ABILITY_ROCK_HEAD"),
        "SPECIES_MARACTUS": (0, "ABILITY_WATER_ABSORB"),
        "SPECIES_REGIROCK": (0, "ABILITY_CLEAR_BODY"),
    }
    for species, (slot, ability) in roxanne_abilities.items():
        slots = ability_slots.get(species, [])
        if len(slots) <= slot or slots[slot] != ability:
            problems.append(f"Battle 19: {species} slot {slot} is not {ability}: {slots}")

    roxanne_move_sources = {
        "KLEFKI": {
            "tm": ("TM17_PROTECT", "TM20_SAFEGUARD"),
            "tutor": ("MOVE_SWAGGER", "MOVE_FOUL_PLAY"),
        },
        "ROCKRUFF": {
            "tm": ("TM17_PROTECT", "TM63_ROCK_SLIDE", "TM94_SUCKER_PUNCH"),
            "tutor": ("MOVE_PSYCHIC_FANGS",),
        },
        "MUDBRAY": {
            "tm": ("TM17_PROTECT", "TM63_ROCK_SLIDE"),
            "level": ("MOVE_HIGH_HORSEPOWER", "MOVE_HEAVY_SLAM"),
        },
        "BONSLY": {
            "tm": ("TM63_ROCK_SLIDE", "TM94_SUCKER_PUNCH"),
            "level": ("MOVE_DOUBLE_EDGE", "MOVE_LOW_KICK"),
        },
        "MARACTUS": {
            "tm": ("TM60_DRAIN_PUNCH", "TM84_POISON_JAB", "TM94_SUCKER_PUNCH"),
            "tutor": ("MOVE_SEED_BOMB",),
        },
        "REGIROCK": {
            "tm": ("TM60_DRAIN_PUNCH", "TM63_ROCK_SLIDE"),
            "tutor": ("MOVE_THUNDER_PUNCH", "MOVE_IRON_HEAD"),
        },
    }
    for species, sources in roxanne_move_sources.items():
        tmhm = species_tmhm_body(tmhm_source, species)
        level = level_up_body(level_source, species.title().replace("_", ""))
        for tm in sources.get("tm", ()):
            if tm not in tmhm:
                problems.append(f"Battle 19: {species} is missing {tm}")
        for move in sources.get("level", ()):
            if move not in level:
                problems.append(f"Battle 19: {species} cannot legally learn {move}")
        for move in sources.get("tutor", ()):
            if not species_has_tutor_move(tutor_source, indices, species, move):
                problems.append(f"Battle 19: {species} cannot legally learn {move}")

    ai_util = read("src/battle_ai_util.c")
    for token in (
        "gSideStatuses[GetBattlerSide(battler)] & SIDE_STATUS_SAFEGUARD",
        "HasMoveEffect(battlerAtk, EFFECT_SWAGGER)",
        "effect == EFFECT_SWAGGER",
        "partnerAbility == ABILITY_OWN_TEMPO",
        "score += 15",
    ):
        if token not in ai_util and token not in guard_split_ai:
            problems.append(f"Battle 19: Safeguard/Swagger AI drifted: {token}")
    setup_body = guard_split_ai.rsplit("static s16 AI_SetupFirstTurn(u8 battlerAtk", 1)[-1].split("static s16 AI_Risky", 1)[0]
    if "case EFFECT_SAFEGUARD:" not in setup_body:
        problems.append("Battle 19: first-turn setup AI does not value Safeguard")

    roxanne_dialogue = rustboro_gym.split("RustboroCity_Gym_Text_RoxanneIntro:", 1)[1].split("RustboroCity_Gym_Text_GymStatue:", 1)[0]
    for truthful in ("examination is confidence", "turn Swagger into strength", "Safeguard prevents confusion", "two healthy Pokémon"):
        if truthful not in roxanne_dialogue:
            problems.append(f"Battle 19: Roxanne dialogue does not explain {truthful}")
    for line in re.findall(r'\.string "([^"]*)"', roxanne_dialogue):
        visible = line.replace("\\n", "").replace("\\l", "").replace("\\p", "").replace("$", "")
        if len(visible) > 36:
            problems.append(f"Battle 19: Roxanne dialogue line is too long: {visible}")
    gym_tips = rustboro_gym.split("RustboroCity_Gym_Text_Tips:", 1)[1].split("RustboroCity_Gym_Text_GymGuideAdvice:", 1)[0]
    if "evolved once" in gym_tips or not all(token in gym_tips for token in ("Attack with Swagger", "remove boosts", "spread attacks")):
        problems.append("Battle 19: Gym Guide advice is stale or does not hint at Roxanne's real exam")

    roxanne_donors = (
        ("docs/showdown_gen6_random_doubles_30.json", 11, "12,29493,18613,48380", {"Klefki"}),
        ("docs/showdown_champions_random_doubles_30.json", 5, "6,47514,9355,56963", {"Mudsdale"}),
        ("docs/showdown_champions_random_doubles_30.json", 7, "8,63352,12441,54102", {"Mudsdale"}),
        ("docs/showdown_gen8_random_doubles_30.json", 0, "1,7919,1640,31348", {"Regirock"}),
        ("docs/showdown_champions_random_doubles_30.json", 4, "5,39595,7812,25626", {"Klefki", "Aerodactyl"}),
    )
    for path, index, seed, species in roxanne_donors:
        sample = json.loads(read(path))["samples"][index]
        names = {mon.get("name") for mon in sample.get("team", [])}
        if sample.get("seed") != seed or not species <= names:
            problems.append(f"Battle 19: donor sample drifted in {path} sample {index + 1}")
    regirock_smogon = json.loads(read("docs/smogon_gen4_9_ou_uu_nu_sample_teams.json"))["formats"]["gen5nu"][0]
    if "Regirock" not in {mon.get("species") for mon in regirock_smogon.get("data", [])}:
        problems.append("Battle 19: Smogon gen5nu sample 1 lost Regirock")

    reward_source = read("src/item.c")
    if "{ITEM_EXPERT_BELT,      DISCOVERY_ONLY}" not in reward_source:
        problems.append("Battle 19: Expert Belt is no longer discovery-unlocked")
    for token in ("giveitem ITEM_EXPERT_BELT", "setflag FLAG_RECEIVED_TM39", "That Expert Belt strengthens moves"):
        if token not in rustboro_gym:
            problems.append(f"Battle 19: Roxanne reward flow drifted: {token}")

    battles_1_to_18 = battles_1_to_17 | {build["species"] for build in expected_marc}
    if battles_1_to_18 & {build["species"] for build in expected_roxanne}:
        problems.append("Battle 19: Roxanne repeats a species from Battles 2-18")

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
    print("PASS: Battle 9 Lyle Telepathy detonation, legal young Bug stages, contextual suicide AI, and doubles guard")
    print("PASS: Battle 10 Aqua Grunt smash-and-grab doubles, four legal first stages, theft AI, and story branches")
    print("PASS: Battle 11 James forest-circle Perish trap, mythical showcase, legal sets, guard, and post-Grunt recovery")
    print("PASS: Battle 12 Winston Power Spot collection, legal rare stages, conditional speed AI, and doubles guard")
    print("PASS: Battle 13 Haley branching-choice singles, legal young forms, adaptive AI, and rematch identity")
    print("PASS: Battle 14 Gina & Mia three-dance recital, legal stages/sets, Dancer combo AI, twin guards, and native dialogue")
    print("PASS: Battle 15 Ivan lure-sinker-hook-school singles, legal sets, Schooling threshold, adaptive AI, and native dialogue")
    print("PASS: Battle 16 Josh Guard Split geology lab, legal young sets, one-use transfer AI, doubles guard, and native dialogue")
    print("PASS: Battle 17 Tommy Instruct repetition lesson, legal young sets, contextual partner AI, doubles guard, and native dialogue")
    print("PASS: Battle 18 Marc shifting-strata singles, legal young sets, deterministic hazard order, adaptive AI, and native dialogue")
    print("PASS: Battle 19 Roxanne protected-confidence boss, six legal stages, Safeguard/Swagger AI, doubles guard, truthful hints, and Expert Belt reward")
    print(f"PASS: all {len(designs)} closed encounters record a full 983-team corpus fit decision")


if __name__ == "__main__":
    main()
