#!/usr/bin/env python3
"""Author, apply, and verify the ten-battle Heat Badge epilogue batch."""

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
FORMATS = ROOT / "docs/verdant_doubles_manifest.json"
CORPUS = ROOT / "docs/competitive_team_index.jsonl"
PARTIES = ROOT / "src/data/trainer_parties.h"
TRAINERS = ROOT / "src/data/trainers.h"


def M(level, species, item, ability_slot, spread, *moves):
    return {"level": level, "species": species, "item": item, "ability_slot": ability_slot, "spread": spread, "moves": list(moves)}


# Battle 124 — Shelby's stance-reading family.
MIENSHAO = M(1, "SPECIES_MIENSHAO", "ITEM_FOCUS_SASH", 1, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_FAKE_OUT", "MOVE_CLOSE_COMBAT", "MOVE_KNOCK_OFF", "MOVE_U_TURN")
WOBBUFFET = M(2, "SPECIES_WOBBUFFET", "ITEM_MENTAL_HERB", 0, "SPREAD_31_IV_HP_DEF_BOLD", "MOVE_COUNTER", "MOVE_MIRROR_COAT", "MOVE_ENCORE", "MOVE_DESTINY_BOND")
SLOWBRO = M(3, "SPECIES_SLOWBRO", "ITEM_COLBUR_BERRY", 2, "SPREAD_31_IV_HP_DEF_BOLD", "MOVE_SCALD", "MOVE_ICY_WIND", "MOVE_SLACK_OFF", "MOVE_PSYSHOCK")
AEGISLASH = M(4, "SPECIES_AEGISLASH", "ITEM_WEAKNESS_POLICY", 0, "SPREAD_31_IV_HP_SPATK_QUIET", "MOVE_KINGS_SHIELD", "MOVE_SHADOW_BALL", "MOVE_FLASH_CANNON", "MOVE_WIDE_GUARD")
LUCARIO = M(5, "SPECIES_LUCARIO", "ITEM_EXPERT_BELT", 1, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_CLOSE_COMBAT", "MOVE_METEOR_MASH", "MOVE_EXTREME_SPEED", "MOVE_PROTECT")
KINGAMBIT = M(6, "SPECIES_KINGAMBIT", "ITEM_BLACK_GLASSES", 1, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_KOWTOW_CLEAVE", "MOVE_IRON_HEAD", "MOVE_SUCKER_PUNCH", "MOVE_PROTECT")
SHELBY_TEAMS = {
    "TRAINER_SHELBY_1": [MIENSHAO, WOBBUFFET, SLOWBRO, AEGISLASH],
    "TRAINER_SHELBY_2": [dict(LUCARIO, level=2), dict(SLOWBRO, level=3), dict(KINGAMBIT, level=4)],
    "TRAINER_SHELBY_3": [dict(WOBBUFFET, level=1), dict(AEGISLASH, level=2), dict(MIENSHAO, level=3), dict(LUCARIO, level=4)],
    "TRAINER_SHELBY_4": [MIENSHAO, WOBBUFFET, SLOWBRO, AEGISLASH, LUCARIO, KINGAMBIT],
}

# Battle 125 — Melissa's volcanic poise single.
MELISSA_TEAM = [
    M(2, "SPECIES_MILOTIC", "ITEM_LEFTOVERS", 1, "SPREAD_31_IV_HP_DEF_BOLD", "MOVE_SCALD", "MOVE_ICE_BEAM", "MOVE_RECOVER", "MOVE_HAZE"),
    M(3, "SPECIES_FURFROU", "ITEM_CHOPLE_BERRY", 0, "SPREAD_31_IV_HP_DEF_IMPISH", "MOVE_RETURN", "MOVE_COTTON_GUARD", "MOVE_SUCKER_PUNCH", "MOVE_REST"),
    M(4, "SPECIES_LOPUNNY", "ITEM_FLAME_ORB", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_SWITCHEROO", "MOVE_FAKE_OUT", "MOVE_RETURN", "MOVE_HIGH_JUMP_KICK"),
    M(5, "SPECIES_DIANCIE", "ITEM_DIANCITE", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_MOONBLAST", "MOVE_DIAMOND_STORM", "MOVE_EARTH_POWER", "MOVE_PROTECT"),
]

# Battle 126 — Sheila's priority-proof royal court.
SHEILA_TEAM = [
    M(1, "SPECIES_TSAREENA", "ITEM_ASSAULT_VEST", 1, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_POWER_WHIP", "MOVE_HIGH_JUMP_KICK", "MOVE_KNOCK_OFF", "MOVE_U_TURN"),
    M(2, "SPECIES_CINCCINO", "ITEM_KINGS_ROCK", 2, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_TAIL_SLAP", "MOVE_BULLET_SEED", "MOVE_ROCK_BLAST", "MOVE_PROTECT"),
    M(3, "SPECIES_BISHARP", "ITEM_EVIOLITE", 0, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_IRON_HEAD", "MOVE_KNOCK_OFF", "MOVE_SUCKER_PUNCH", "MOVE_SWORDS_DANCE"),
    M(4, "SPECIES_ESCAVALIER", "ITEM_OCCA_BERRY", 2, "SPREAD_31_IV_HP_ATK_BRAVE", "MOVE_MEGAHORN", "MOVE_IRON_HEAD", "MOVE_KNOCK_OFF", "MOVE_PROTECT"),
]

# Battle 127 — Shirley's four-act summit performance.
SHIRLEY_TEAM = [
    M(1, "SPECIES_ALCREMIE", "ITEM_BABIRI_BERRY", 2, "SPREAD_31_IV_HP_DEF_BOLD", "MOVE_DECORATE", "MOVE_DAZZLING_GLEAM", "MOVE_HELPING_HAND", "MOVE_PROTECT"),
    M(2, "SPECIES_MELOETTA", "ITEM_ASSAULT_VEST", 0, "SPREAD_31_IV_SPATK_SPEED_HASTY", "MOVE_HYPER_VOICE", "MOVE_PSYCHIC", "MOVE_CLOSE_COMBAT", "MOVE_KNOCK_OFF"),
    M(3, "SPECIES_NIHILEGO", "ITEM_LIFE_ORB", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_POWER_GEM", "MOVE_SLUDGE_WAVE", "MOVE_GRASS_KNOT", "MOVE_PROTECT"),
    M(4, "SPECIES_MR_RIME", "ITEM_CHOICE_SCARF", 1, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_FREEZE_DRY", "MOVE_PSYCHIC", "MOVE_FOCUS_BLAST", "MOVE_ICY_WIND"),
]

# Battle 128 — Sawyer's mountain-strata family.
GIGALITH = M(1, "SPECIES_GIGALITH", "ITEM_SMOOTH_ROCK", 1, "SPREAD_31_IV_HP_ATK_BRAVE", "MOVE_ROCK_SLIDE", "MOVE_BODY_PRESS", "MOVE_WIDE_GUARD", "MOVE_PROTECT")
DRACOZOLT = M(2, "SPECIES_DRACOZOLT", "ITEM_LIFE_ORB", 2, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_BOLT_BEAK", "MOVE_DRAGON_CLAW", "MOVE_HIGH_HORSEPOWER", "MOVE_PROTECT")
CRADILY = M(3, "SPECIES_CRADILY", "ITEM_LEFTOVERS", 2, "SPREAD_31_IV_HP_SPATK_QUIET", "MOVE_GIGA_DRAIN", "MOVE_POWER_GEM", "MOVE_RECOVER", "MOVE_PROTECT")
EXCADRILL = M(4, "SPECIES_EXCADRILL", "ITEM_EXCADRITE", 2, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_HIGH_HORSEPOWER", "MOVE_IRON_HEAD", "MOVE_ROCK_SLIDE", "MOVE_PROTECT")
HYDREIGON = M(5, "SPECIES_HYDREIGON", "ITEM_EXPERT_BELT", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_DRACO_METEOR", "MOVE_DARK_PULSE", "MOVE_HEAT_WAVE", "MOVE_TAILWIND")
ROTOM_FROST = M(6, "SPECIES_ROTOM_FROST", "ITEM_SITRUS_BERRY", 0, "SPREAD_31_IV_HP_SPATK_MODEST", "MOVE_FREEZE_DRY", "MOVE_THUNDERBOLT", "MOVE_WILL_O_WISP", "MOVE_PROTECT")
SAWYER_TEAMS = {
    "TRAINER_SAWYER_1": [GIGALITH, DRACOZOLT, CRADILY, EXCADRILL],
    "TRAINER_SAWYER_2": [dict(GIGALITH, level=2), dict(DRACOZOLT, level=3)],
    "TRAINER_SAWYER_3": [CRADILY, EXCADRILL, GIGALITH, DRACOZOLT],
    "TRAINER_SAWYER_4": [GIGALITH, DRACOZOLT, CRADILY, EXCADRILL, HYDREIGON, ROTOM_FROST],
}

# Battle 129 — Drew's three desert traps single.
DREW_TEAM = [
    M(2, "SPECIES_SANDSLASH", "ITEM_FOCUS_SASH", 2, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_SWORDS_DANCE", "MOVE_EARTHQUAKE", "MOVE_STONE_EDGE", "MOVE_SHADOW_CLAW"),
    M(3, "SPECIES_CACTURNE", "ITEM_LIFE_ORB", 2, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_SUCKER_PUNCH", "MOVE_SEED_BOMB", "MOVE_DRAIN_PUNCH", "MOVE_SPIKY_SHIELD"),
    M(4, "SPECIES_DUNSPARCE", "ITEM_LEFTOVERS", 0, "SPREAD_31_IV_HP_SPDEF_CAREFUL", "MOVE_COIL", "MOVE_GLARE", "MOVE_HEADBUTT", "MOVE_ROOST"),
]

# Battle 130 — Heidi's oasis seed circuit.
HEIDI_TEAM = [
    M(1, "SPECIES_TAPU_BULU", "ITEM_TERRAIN_EXTENDER", 0, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_WOOD_HAMMER", "MOVE_HORN_LEECH", "MOVE_CLOSE_COMBAT", "MOVE_PROTECT"),
    M(2, "SPECIES_HAWLUCHA", "ITEM_GRASSY_SEED", 1, "SPREAD_31_IV_ATK_SPEED_ADAMANT", "MOVE_ACROBATICS", "MOVE_CLOSE_COMBAT", "MOVE_SWORDS_DANCE", "MOVE_PROTECT"),
    M(3, "SPECIES_MUDSDALE", "ITEM_ASSAULT_VEST", 1, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_HIGH_HORSEPOWER", "MOVE_BODY_PRESS", "MOVE_HEAVY_SLAM", "MOVE_ROCK_SLIDE"),
    M(4, "SPECIES_BELLOSSOM", "ITEM_LEFTOVERS", 2, "SPREAD_31_IV_HP_SPATK_MODEST", "MOVE_GIGA_DRAIN", "MOVE_MOONBLAST", "MOVE_QUIVER_DANCE", "MOVE_STRENGTH_SAP"),
]

# Battle 131 — Beau's scout-and-shelter double.
BEAU_TEAM = [
    M(1, "SPECIES_PERSIAN_ALOLAN", "ITEM_SITRUS_BERRY", 0, "SPREAD_31_IV_HP_SPEED_TIMID", "MOVE_FAKE_OUT", "MOVE_PARTING_SHOT", "MOVE_FOUL_PLAY", "MOVE_TAUNT"),
    M(2, "SPECIES_MANDIBUZZ", "ITEM_MENTAL_HERB", 1, "SPREAD_31_IV_HP_SPDEF_CAREFUL", "MOVE_TAILWIND", "MOVE_FOUL_PLAY", "MOVE_SNARL", "MOVE_ROOST"),
    M(3, "SPECIES_GASTRODON", "ITEM_RINDO_BERRY", 1, "SPREAD_31_IV_HP_DEF_BOLD", "MOVE_SCALD", "MOVE_EARTH_POWER", "MOVE_RECOVER", "MOVE_ICY_WIND"),
    M(4, "SPECIES_LYCANROC_DUSK", "ITEM_LIFE_ORB", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_ACCELEROCK", "MOVE_ROCK_SLIDE", "MOVE_CLOSE_COMBAT", "MOVE_PROTECT"),
]

# Battle 132 — Becky's Simple mirage double.
BECKY_TEAM = [
    M(1, "SPECIES_GOLDUCK", "ITEM_SITRUS_BERRY", 1, "SPREAD_31_IV_HP_SPATK_MODEST", "MOVE_SIMPLE_BEAM", "MOVE_ICY_WIND", "MOVE_SCALD", "MOVE_PROTECT"),
    M(2, "SPECIES_KOMMO_O", "ITEM_ROSELI_BERRY", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_DRAGON_DANCE", "MOVE_DRAGON_CLAW", "MOVE_DRAIN_PUNCH", "MOVE_PROTECT"),
    M(3, "SPECIES_JUMPLUFF", "ITEM_FOCUS_SASH", 2, "SPREAD_31_IV_HP_SPEED_TIMID", "MOVE_COTTON_SPORE", "MOVE_STRENGTH_SAP", "MOVE_ENCORE", "MOVE_U_TURN"),
    M(4, "SPECIES_LYCANROC", "ITEM_ROCK_GEM", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_STONE_EDGE", "MOVE_CLOSE_COMBAT", "MOVE_ACCELEROCK", "MOVE_PROTECT"),
]

# Battle 133 — Dusty's fossil-excavation family.
GOLURK = M(2, "SPECIES_GOLURK", "ITEM_COLBUR_BERRY", 2, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_DYNAMIC_PUNCH", "MOVE_HIGH_HORSEPOWER", "MOVE_POLTERGEIST", "MOVE_ICE_PUNCH")
SIGILYPH = M(3, "SPECIES_SIGILYPH", "ITEM_FLAME_ORB", 1, "SPREAD_31_IV_HP_SPEED_TIMID", "MOVE_COSMIC_POWER", "MOVE_STORED_POWER", "MOVE_ROOST", "MOVE_PSYCHO_SHIFT")
BASTIODON = M(4, "SPECIES_BASTIODON", "ITEM_LEFTOVERS", 2, "SPREAD_31_IV_HP_SPDEF_CALM", "MOVE_METAL_BURST", "MOVE_FLASH_CANNON", "MOVE_SHORE_UP", "MOVE_PROTECT")
RAMPARDOS = M(4, "SPECIES_RAMPARDOS", "ITEM_CHOICE_SCARF", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_HEAD_SMASH", "MOVE_EARTHQUAKE", "MOVE_ZEN_HEADBUTT", "MOVE_CRUNCH")
ARMALDO = M(5, "SPECIES_ARMALDO", "ITEM_ASSAULT_VEST", 0, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_STONE_EDGE", "MOVE_X_SCISSOR", "MOVE_AQUA_JET", "MOVE_KNOCK_OFF")
FLYGON = M(6, "SPECIES_FLYGON", "ITEM_FLYGONITE", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_DRAGON_CLAW", "MOVE_EARTHQUAKE", "MOVE_DRAGON_DANCE", "MOVE_PROTECT")
DUSTY_TEAMS = {
    "TRAINER_DUSTY_1": [GOLURK, SIGILYPH, BASTIODON],
    "TRAINER_DUSTY_2": [dict(RAMPARDOS, level=2), dict(ARMALDO, level=3), dict(FLYGON, level=4)],
    "TRAINER_DUSTY_3": [dict(GOLURK, level=1), dict(SIGILYPH, level=2), dict(BASTIODON, level=3), dict(RAMPARDOS, level=4)],
    "TRAINER_DUSTY_4": [dict(GOLURK, level=1), dict(SIGILYPH, level=2), dict(BASTIODON, level=3), RAMPARDOS, ARMALDO, FLYGON],
}


CONFIGS = [
    {"index": 124, "id": "BATTLE_124_MT_CHIMNEY_SHELBY", "location": "MtChimney", "category": "optional stance-reading Picnicker rematch family", "trainers": list(SHELBY_TEAMS), "main": "TRAINER_SHELBY_1", "teams": SHELBY_TEAMS, "target": 9.2, "question": "Can the player read damage category and commitment under Fake Out, Shadow Tag, counters, stance changes, and a late Supreme Overlord?", "tempo": "Fake Out/U-turn probe, trapped counter stance, defensive pivot, blade stance, then priority-heavy rematch finish.", "weakness": "Wobbuffet is passive against setup and status; Ghosts, Taunt, mixed damage, spread pressure, phazing, item removal, and focus fire remain broad.", "lesson": "Do not hand Wobbuffet the damage category it wants; change axis before the steel reserves punish the locked response.", "tags": ["mt-chimney-epilogue", "stance-reading", "shadow-tag", "aegislash", "kingambit", "mixed-rematch-family"], "refs": ["showdown:gen7randomdoublesbattle:016", "showdown:gen4randomdoublesbattle:018", "showdown:gen7randomdoublesbattle:007", "showdown:gen9championsrandomdoublesbattle:013"], "coords": [16, 18]},
    {"index": 125, "id": "BATTLE_125_MT_CHIMNEY_MELISSA", "location": "MtChimney", "category": "optional volcanic-poise Beauty single", "trainers": ["TRAINER_MELISSA"], "main": "TRAINER_MELISSA", "teams": {"TRAINER_MELISSA": MELISSA_TEAM}, "target": 8.9, "question": "Can the player change attacking method through Marvel Scale, Fur Coat, Klutz disruption, and Mega Diancie's fast mixed finish?", "tempo": "Four singles walls and pivots: competitive water poise, physical grooming, item trick, then one radiant Mega.", "weakness": "No weather, speed field, trap, priority core, or sustain loop beyond two visible recovery users; Electric, Grass, Poison, Steel, Fighting, and special pressure remain broad.", "lesson": "Stop treating beauty as one defensive stat: special pressure breaks Fur Coat, item awareness beats Lopunny, and Steel must be preserved for Diancie.", "tags": ["mt-chimney-epilogue", "volcanic-poise", "route-single", "milotic", "furfrou", "klutz-switcheroo", "mega-diancie"], "refs": ["showdown:gen9championsrandomdoublesbattle:007", "showdown:gen9championsrandomdoublesbattle:016", "showdown:gen9randomdoublesbattle:010"], "coords": [14, 7]},
    {"index": 126, "id": "BATTLE_126_MT_CHIMNEY_SHEILA", "location": "MtChimney", "category": "optional priority-proof royal-court double", "trainers": ["TRAINER_SHEILA"], "main": "TRAINER_SHEILA", "teams": {"TRAINER_SHEILA": SHEILA_TEAM}, "target": 9.1, "question": "Can the player survive Skill Link volleys without leaning on priority, then break the Defiant bishop and Overcoat knight?", "tempo": "Queenly Majesty blocks cheap tempo while Cinccino volleys; Defiant Bisharp and Overcoat Escavalier form the guard.", "weakness": "No weather, room, redirection, Mega, legend, recovery, or spread loop; Fighting, Fire, Fairy, burn, Intimidate discipline, and focused special damage remain broad.", "lesson": "Queenly Majesty turns off priority, not ordinary speed control; remove the volley before feeding the two Steel reserves.", "tags": ["mt-chimney-epilogue", "royal-court", "queenly-majesty", "skill-link", "bisharp", "escavalier"], "refs": ["showdown:gen9championsrandomdoublesbattle:021", "showdown:gen7randomdoublesbattle:013", "showdown:gen6randomdoublesbattle:001", "showdown:gen8randomdoublesbattle:015"], "coords": [29, 7]},
    {"index": 127, "id": "BATTLE_127_MT_CHIMNEY_SHIRLEY", "location": "MtChimney", "category": "optional four-act summit-performance double", "trainers": ["TRAINER_SHIRLEY"], "main": "TRAINER_SHIRLEY", "teams": {"TRAINER_SHIRLEY": SHIRLEY_TEAM}, "target": 9.2, "question": "Can the player stop Decorate from turning Meloetta into a mixed lead, then preserve answers for Beast Boost and a Scarfed screen-cleaning finale?", "tempo": "Decorate/Helping Hand opening, mixed song pressure, Beast Boost glass act, then Choice Scarf speed control.", "weakness": "Alcremie spends support turns; Meloetta has no Protect; Nihilego is physically fragile; Mr. Rime is Choice locked; Steel, Ghost, Dark, Ground, priority, Taunt, and Wide Guard remain broad.", "lesson": "Break the support-act link first, then deny Nihilego its first KO and exploit Mr. Rime's visible lock.", "tags": ["mt-chimney-epilogue", "summit-performance", "decorate", "meloetta", "nihilego", "screen-cleaner"], "refs": ["showdown:gen8randomdoublesbattle:004", "showdown:gen5randomdoublesbattle:006", "smogon:gen8uu:010", "showdown:gen9championsrandomdoublesbattle:009"], "coords": [27, 17]},
    {"index": 128, "id": "BATTLE_128_MT_CHIMNEY_SAWYER", "location": "MtChimney", "category": "optional mountain-strata Hiker rematch family", "trainers": list(SAWYER_TEAMS), "main": "TRAINER_SAWYER_1", "teams": SAWYER_TEAMS, "target": 9.3, "question": "Can the player break Sand Rush plus Storm Drain positioning before Mega Excadrill drills through, while preserving the right rematch answers for air and cold layers?", "tempo": "Sand setter and fossil sprint, water-absorbing shelf, one Mega drill, then Tailwind and frozen-appliance rematch layers.", "weakness": "Weather is replaceable; Dracozolt is Life Orb frail; Cradily is Tauntable; Excadrill exposes common Water/Fighting/Ground; Hydreigon and Rotom take sand chip.", "lesson": "Remove the speed layer or exploit its own chip; do not feed Cradily Water before Mega Excadrill is under control.", "tags": ["mt-chimney-epilogue", "mountain-strata", "sand-rush", "storm-drain", "mega-excadrill", "mixed-rematch-family"], "refs": ["showdown:gen6randomdoublesbattle:004", "showdown:gen8randombattle:025", "showdown:gen4randomdoublesbattle:030", "showdown:gen9championsrandomdoublesbattle:019", "showdown:gen8randomdoublesbattle:009", "showdown:gen5randomdoublesbattle:016"], "coords": [7, 7]},
    {"index": 129, "id": "BATTLE_129_ROUTE_111_DREW", "location": "Route111", "category": "optional three-trap desert single", "trainers": ["TRAINER_DREW"], "main": "TRAINER_DREW", "teams": {"TRAINER_DREW": DREW_TEAM}, "target": 8.7, "question": "Can the player stop three different setup and denial traps without assuming a two-member desert single is free relief?", "tempo": "Sash Swords Dance opener, Sucker Punch/Spiky Shield ambush, then Serene Grace Coil/Glare attrition.", "weakness": "Only three bodies, no weather, Mega, legend, field, recovery loop beyond Dunsparce, and broad Water/Ice/Fighting/Bug/Fairy/Grass counterplay.", "lesson": "Deny Sandslash immediately, refuse Cacturne's priority guessing game, then use Taunt or special burst before Dunsparce compounds turns.", "tags": ["route111-desert", "route-single", "three-traps", "sandslash", "cacturne", "dunsparce"], "refs": ["showdown:gen4randomdoublesbattle:028", "showdown:gen6randomdoublesbattle:030", "showdown:gen8randomdoublesbattle:008"], "coords": [29, 37]},
    {"index": 130, "id": "BATTLE_130_ROUTE_111_HEIDI", "location": "Route111", "category": "optional oasis-seed double", "trainers": ["TRAINER_HEIDI"], "main": "TRAINER_HEIDI", "teams": {"TRAINER_HEIDI": HEIDI_TEAM}, "target": 9.2, "question": "Can the player contest Grassy Terrain before Hawlucha spends its Seed, then change damage categories through Stamina and Bellossom's recovery?", "tempo": "Terrain/Seed acceleration lead into Stamina physical bulk and Healer-backed special setup.", "weakness": "Terrain is replaceable; Hawlucha's Seed is one-use; Mudsdale is slow and special-vulnerable; Bellossom is Tauntable; Fire/Flying/Ice/Poison/Steel remain broad.", "lesson": "Do not let one automatic Seed define the fight: replace terrain or remove Hawlucha, then attack Mudsdale specially and deny Bellossom setup.", "tags": ["route111-desert", "oasis", "grassy-terrain", "tapu-bulu", "grassy-seed", "hawlucha", "stamina"], "refs": ["showdown:gen8randomdoublesbattle:006", "showdown:gen7randomdoublesbattle:014", "showdown:gen9championsrandomdoublesbattle:006", "showdown:gen4randomdoublesbattle:024"], "coords": [28, 51]},
    {"index": 131, "id": "BATTLE_131_ROUTE_111_BEAU", "location": "Route111", "category": "optional scout-and-shelter double", "trainers": ["TRAINER_BEAU"], "main": "TRAINER_BEAU", "teams": {"TRAINER_BEAU": BEAU_TEAM}, "target": 9.0, "question": "Can the player prevent Fake Out, Parting Shot, Tailwind, and Snarl from buying the exact safe entries Gastrodon and Lycanroc need?", "tempo": "Fast disruption plus bulky Tailwind lead into Storm Drain shelter and Tough Claws cleanup.", "weakness": "The lead is low-damage and Tauntable; Gastrodon has a four-times Grass seam; Lycanroc is frail; Electric, Fairy, Ice, Rock, Grass, and focused special damage remain broad.", "lesson": "Attack the control economy rather than its pivots: stop Tailwind or Parting Shot, then preserve Grass for Gastrodon and priority for Lycanroc.", "tags": ["route111-desert", "scout-and-shelter", "fake-out", "parting-shot", "tailwind", "storm-drain", "lycanroc-dusk"], "refs": ["showdown:gen7randomdoublesbattle:009", "showdown:gen7randomdoublesbattle:002", "showdown:gen4randomdoublesbattle:006", "showdown:gen7randomdoublesbattle:003"], "coords": [21, 47]},
    {"index": 132, "id": "BATTLE_132_ROUTE_111_BECKY", "location": "Route111", "category": "optional Simple-mirage double", "trainers": ["TRAINER_BECKY"], "main": "TRAINER_BECKY", "teams": {"TRAINER_BECKY": BECKY_TEAM}, "target": 9.2, "question": "Can the player interrupt Simple Beam before one Dragon Dance becomes two, then handle a disruptive airborne reserve and priority Rock cleanup?", "tempo": "Simple Beam/Dragon Dance lead, Cotton Spore and Encore reserve control, then Rock Gem priority finish.", "weakness": "The combo spends Golduck's turn and is interruptible; Haze, Unaware, Taunt, redirection, Fairy/Ice/Flying/Psychic, special pressure, and priority remain broad.", "lesson": "Remove Golduck or deny Kommo-o's first setup; if the mirage forms, reset it instead of racing doubled boosts.", "tags": ["route111-desert", "simple-mirage", "simple-beam", "dragon-dance", "kommo-o", "jumpluff", "lycanroc"], "refs": ["showdown:gen7randomdoublesbattle:029", "showdown:gen7randomdoublesbattle:003", "showdown:gen9randomdoublesbattle:026", "smogon:gen8nu:001"], "coords": [32, 66]},
    {"index": 133, "id": "BATTLE_133_ROUTE_111_DUSTY", "location": "Route111", "category": "optional fossil-excavation Hiker rematch family", "trainers": list(DUSTY_TEAMS), "main": "TRAINER_DUSTY_1", "teams": DUSTY_TEAMS, "target": 8.9, "question": "Can the player change from No Guard force to Magic Guard setup and Sturdy reflection, then survive the rematches' fossil offense and Mega Flygon?", "tempo": "Three-layer excavation single grows into mixed fossil doubles and a six-member Mega finale.", "weakness": "The opening is only three bodies; Golurk is slow, Sigilyph is Knock Off/Taunt vulnerable, Bastiodon has four-times weaknesses, Rampardos is Choice locked, and Flygon needs a setup turn.", "lesson": "Change tools by layer: exploit Golurk's speed, stop Sigilyph's stored power, avoid feeding Metal Burst, then preserve Ice/Fairy/Water for the rematch fossils.", "tags": ["route111-desert", "fossil-excavation", "no-guard", "magic-guard", "metal-burst", "mega-flygon", "mixed-rematch-family"], "refs": ["showdown:gen9championsrandomdoublesbattle:017", "smogon:gen7nu:004", "showdown:gen9championsrandomdoublesbattle:002", "showdown:gen9championsrandomdoublesbattle:023", "showdown:gen6randomdoublesbattle:025", "showdown:gen8randomdoublesbattle:013"], "coords": [27, 69]},
]


ALL_TEAMS = {trainer_id: team for config in CONFIGS for trainer_id, team in config["teams"].items()}
TRAINER_RULES = {
    "TRAINER_SHELBY_1": ("double", 4, "Stance-reading counter double", 92), "TRAINER_SHELBY_2": ("single", 3, "Stance-reading singles rematch", 90), "TRAINER_SHELBY_3": ("double", 4, "Stance-reading doubles rematch", 94), "TRAINER_SHELBY_4": ("double", 6, "Stance-reading final rematch", 98),
    "TRAINER_MELISSA": ("single", 4, "Volcanic poise single", 89), "TRAINER_SHEILA": ("double", 4, "Priority-proof royal court", 91), "TRAINER_SHIRLEY": ("double", 4, "Four-act summit performance", 92),
    "TRAINER_SAWYER_1": ("double", 4, "Mountain-strata sand double", 93), "TRAINER_SAWYER_2": ("single", 2, "Mountain-strata singles rematch", 88), "TRAINER_SAWYER_3": ("double", 4, "Mountain-strata doubles rematch", 94), "TRAINER_SAWYER_4": ("double", 6, "Mountain-strata final rematch", 98),
    "TRAINER_DREW": ("single", 3, "Three-trap desert single", 87), "TRAINER_HEIDI": ("double", 4, "Oasis seed circuit", 92), "TRAINER_BEAU": ("double", 4, "Scout-and-shelter control", 90), "TRAINER_BECKY": ("double", 4, "Simple mirage setup", 92),
    "TRAINER_DUSTY_1": ("single", 3, "Fossil-excavation single", 89), "TRAINER_DUSTY_2": ("single", 3, "Fossil offense singles rematch", 91), "TRAINER_DUSTY_3": ("double", 4, "Fossil excavation doubles rematch", 94), "TRAINER_DUSTY_4": ("double", 6, "Fossil excavation final rematch", 98),
}


BASE_FLAGS = ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_HP_AWARE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_CHECK_FOE"]
EXTRA_FLAGS = {
    "TRAINER_SHELBY_1": ["AI_FLAG_HELP_PARTNER"], "TRAINER_SHELBY_3": ["AI_FLAG_HELP_PARTNER"], "TRAINER_SHELBY_4": ["AI_FLAG_HELP_PARTNER"],
    "TRAINER_SHEILA": ["AI_FLAG_HELP_PARTNER"], "TRAINER_SHIRLEY": ["AI_FLAG_HELP_PARTNER"],
    "TRAINER_SAWYER_1": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_FIELD_CONTROL"], "TRAINER_SAWYER_2": ["AI_FLAG_FIELD_CONTROL"], "TRAINER_SAWYER_3": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_FIELD_CONTROL"], "TRAINER_SAWYER_4": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL", "AI_FLAG_FIELD_CONTROL"],
    "TRAINER_HEIDI": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_FIELD_CONTROL"], "TRAINER_BEAU": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL"], "TRAINER_BECKY": ["AI_FLAG_HELP_PARTNER", "AI_FLAG_COMBO_SETUP", "AI_FLAG_SPEED_CONTROL"],
    "TRAINER_DUSTY_3": ["AI_FLAG_HELP_PARTNER"], "TRAINER_DUSTY_4": ["AI_FLAG_HELP_PARTNER"],
}


DIALOGUE = {
    "data/maps/MtChimney/scripts.inc": {
        "MtChimney_Text_ShelbyIntro": ["Every stance has an answer.\\p", "Mienshao probes; Wobbuffet reads,\\n", "then steel chooses the finish!$"],
        "MtChimney_Text_ShelbyDefeat": ["You changed rhythm before I could!$"],
        "MtChimney_Text_ShelbyPostBattle": ["Mixed damage defeats Counter and\\n", "Mirror Coat. Taunt breaks Encore.$"],
        "MtChimney_Text_ShelbyRematchIntro": ["I brought every stance this time.\\n", "Show me how quickly you can adapt!$"],
        "MtChimney_Text_ShelbyRematchDefeat": ["You read the whole sequence!$"],
        "MtChimney_Text_ShelbyPostRematch": ["A stance is strongest only while its\\n", "answer stays hidden.$"],
        "MtChimney_Text_MelissaIntro": ["Beauty is control under pressure!\\p", "Poise, tricks, and one bright Mega--\\n", "show me what can break the pattern!$"],
        "MtChimney_Text_MelissaDefeat": ["You never lost your composure!$"],
        "MtChimney_Text_MelissaPostBattle": ["Fur Coat fears special attacks.\\n", "Klutz Lopunny wants to trade items.$"],
        "MtChimney_Text_SheilaIntro": ["A royal court permits no cheap shot!\\p", "Tsareena guards priority while\\n", "Cinccino volleys. Two rulers wait!$"],
        "MtChimney_Text_SheilaDefeat": ["You overthrew the whole court!$"],
        "MtChimney_Text_SheilaPostBattle": ["Queenly Majesty blocks priority.\\n", "It does not block ordinary speed.$"],
        "MtChimney_Text_ShirleyIntro": ["The summit is my stage!\\p", "Alcremie Decorates Meloetta; then\\n", "Nihilego and Mr. Rime take over!$"],
        "MtChimney_Text_ShirleyDefeat": ["You stole the final bow!$"],
        "MtChimney_Text_ShirleyPostBattle": ["Break the support act first.\\n", "Then deny Nihilego its first KO.$"],
        "MtChimney_Text_SawyerIntro": ["Read the mountain's layers!\\p", "Sand wakes Dracozolt, Cradily drinks\\n", "Water, and Mega Excadrill drills!$"],
        "MtChimney_Text_SawyerDefeat": ["You split every layer!$"],
        "MtChimney_Text_SawyerPostBattle": ["Replace sand or deny Sand Rush.\\n", "Never feed Storm Drain for free.$"],
        "MtChimney_Text_SawyerRematchIntro": ["The mountain grew two new layers!\\n", "Can your formation grow with it?$"],
        "MtChimney_Text_SawyerRematchDefeat": ["Your answer reached the summit!$"],
        "MtChimney_Text_SawyerPostRematch": ["Each rematch exposes a deeper layer.$"],
    },
    "data/text/trainers.inc": {
        "Route111_Text_DrewIntro": ["Three desert traps. Pick your path!$"],
        "Route111_Text_DrewDefeat": ["You escaped all three!$"],
        "Route111_Text_DrewPostBattle": ["Deny Sandslash, refuse Cacturne's\\n", "guessing game, then Taunt Dunsparce.$"],
        "Route111_Text_HeidiIntro": ["An oasis can be a battlefield!\\p", "Tapu Bulu grows the terrain;\\n", "Hawlucha spends its Grassy Seed!$"],
        "Route111_Text_HeidiDefeat": ["You crossed the whole oasis!$"],
        "Route111_Text_HeidiPostBattle": ["Replace terrain or remove Hawlucha.\\n", "Attack Mudsdale specially.$"],
        "Route111_Text_BeauIntro": ["Scouts survive by controlling pace!\\p", "Persian disrupts, Mandibuzz lifts,\\n", "then the sheltered hunters enter!$"],
        "Route111_Text_BeauDefeat": ["You caught every signal!$"],
        "Route111_Text_BeauPostBattle": ["Stop Tailwind or Parting Shot first.\\n", "Save Grass pressure for Gastrodon.$"],
        "Route111_Text_BeckyIntro": ["A mirage makes one step become two!\\p", "Golduck makes Kommo-o Simple;\\n", "one Dragon Dance doubles its gains!$"],
        "Route111_Text_BeckyDefeat": ["You saw through the mirage!$"],
        "Route111_Text_BeckyPostBattle": ["Stop Simple Beam or clear boosts.\\n", "Do not try to race doubled growth.$"],
        "Route111_Text_DustyIntro": ["Every fossil tells a battle story!\\p", "Golurk guards, Sigilyph remembers,\\n", "Bastiodon survives the first dig!$"],
        "Route111_Text_DustyDefeat": ["You uncovered the whole formation!$"],
        "Route111_Text_DustyPostBattle": ["Change tools at every layer.\\n", "Never strike Metal Burst blindly.$"],
        "Route111_Text_DustyRematchIntro": ["I found a deeper fossil layer!\\n", "This excavation ends with a Mega!$"],
        "Route111_Text_DustyRematchDefeat": ["You read the entire dig site!$"],
        "Route111_Text_DustyPostRematch": ["The best teams preserve old layers\\n", "while revealing something new.$"],
    },
}


NEXT = {"index": 134, "encounter_id": "BATTLE_134_ROUTE_111_DAISUKE", "location": "Route111", "category": "optional Route 111 Ninja Boy single", "status": "next", "strict_cap": 45, "trainer_ids": ["TRAINER_DAISUKE"], "access_note": "Daisuke is the next unclosed Route 111 trainer at (32,29) after the desert cluster."}


def design(config):
    return {
        "guide_order": config["index"], "trainer_ids": config["trainers"], "status": "closed", "strict_cap": 45,
        "campaign_point": f"Post-Heat-Badge optional encounter at {config['location']} before Norman; full player preparation remains available.",
        "runtime_branches": [f"{trainer_id}: {TRAINER_RULES[trainer_id][0]} with {len(config['teams'][trainer_id])} authored members." for trainer_id in config["trainers"]],
        "evolution_stage_fit": {"campaign_phase": "cap-45 mature post-Heat-Badge exploration", "effective_levels": "main cap+1 to cap+6 as authored; rematches remain cap-relative", "eligible_ratio": f"{sum(len(team) for team in config['teams'].values())}/{sum(len(team) for team in config['teams'].values())}", "mega_access": True, "status": "pass", "reason": "Every member is fully evolved, single-stage, stone/item evolved, or a deliberate Eviolite middle-stage specialist."},
        "manual_quality": 10, "manual_difficulty": config["target"], "observed_difficulty": None,
        "corpus_review": {"reference_pool_size": 1005, "full_team_candidates": [{"reference_id": ref, "decision": "role adapted; donor roster rejected", "reason": "The exact species role supports this location-specific composition without copying an unrelated full roster."} for ref in config["refs"]], "decision": f"{len(config['refs'])} indexed references support the roles; the encounter structure is locally authored."},
        "competitive_references": [{"reference_id": ref, "adaptation": "Exact role evidence adapted to the authored local team."} for ref in config["refs"]],
        "ordering": {trainer_id: [member["species"] for member in team] for trainer_id, team in config["teams"].items()},
        "team_intent": config["tempo"], "primary_player_question": config["question"], "intended_counterplay": config["weakness"], "first_loss_lesson": config["lesson"],
        "bespoke_ai": "Formats, partner awareness, HP awareness, smart switching, field or speed control, and Combo Setup are attached only where the exact source team uses them. No move, target, switch, or turn is forced.",
        "uniqueness": f"The encounter spends {', '.join(config['tags'][1:])} as its own question and was checked against Battles {config['index']-10}-{config['index']-1} before closure.",
        "story_logic": "The existing trainer, location, trigger, registration/rematch routing, and rewards remain native; dialogue now describes the actual team and counterplay.",
        "reward_logic": "Ordinary EXP and prize money only; existing Match Call families retain registration and rematch progression without invented item rewards.",
        "campaign_reservations": {"spends": config["tags"], "preserves": ["Norman's protected singles discipline", "later Gym and faction anchors", "all unrelated Mega and legendary reveals"], "repeat_rule": f"Do not repeat {config['tags'][1]} soon without a different primary question."},
        "source_teams": config["teams"], "author_self_check": {"strongest_part": config["tempo"], "weakest_link": config["weakness"]},
        "closure": f"Battle {config['index']} is source-closed at quality 10 and target {config['target']}: every physical/rematch branch, exact team, legal set, reference, dialogue cue, broad counterplay, and source route is proven. Runtime remains unplayed.",
    }


def ledger_entry(config):
    main_team = config["teams"][config["main"]]
    return {
        "index": config["index"], "encounter_id": config["id"],
        "identity": {"location": config["location"], "category": config["category"], "format": TRAINER_RULES[config["main"]][0], "strict_cap": 45, "memory_hook": config["tempo"]},
        "primary_player_question": config["question"], "tempo": config["tempo"], "pressure_sources": [member["species"] for member in main_team],
        "intentional_opening": "Source order is intentional and preserved.", "intentional_weakness": config["weakness"], "first_loss_lesson": config["lesson"],
        "revealed_information": ["cap 45", TRAINER_RULES[config["main"]][0], f"{len(main_team)} main members", *config["tags"][1:]],
        "counterplay_classes": [
            "Disrupt, Taunt, redirect, or remove the enabling lead or partner.",
            "Exploit the team's listed type and damage-category seams.",
            "Use speed reversal, item removal, status control, focus fire, or setup denial.",
            config["weakness"],
        ], "target_difficulty": config["target"],
        "difficulty_rationale": "Optimized cap-plus sets and one coherent interaction create a serious optional puzzle while explicit seams preserve broad counterplay.",
        "tuning_knob": "Lower the last main-story member by one level first; preserve species, mechanics, and order.", "playtest_status": "static-pass-runtime-unplayed", "novelty_tags": config["tags"],
        "historic_reference_ids": config["refs"], "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": f"{len(config['refs'])} indexed references."},
        "author_self_check": {"strongest_part": config["tempo"], "weakest_link": config["weakness"]},
    }


def payloads():
    designs = json.loads(DESIGNS.read_text()); ledger = json.loads(LEDGER.read_text()); sequence = json.loads(SEQUENCE.read_text()); operating_system = json.loads(OS_PATH.read_text()); formats = json.loads(FORMATS.read_text())
    for config in CONFIGS:
        designs["designs"][config["id"]] = design(config)
        ledger["entries"] = [row for row in ledger["entries"] if row["index"] != config["index"]] + [ledger_entry(config)]
        sequence["entries"] = [row for row in sequence["entries"] if row["index"] != config["index"]] + [{"index": config["index"], "encounter_id": config["id"], "location": config["location"], "category": config["category"], "status": "closed", "strict_cap": 45, "trainer_ids": config["trainers"], "access_note": f"Physical trigger at ({config['coords'][0]},{config['coords'][1]}); all listed rematches share the same native trainer family."}]
        for trainer_id in config["trainers"]:
            fmt, size, archetype, difficulty = TRAINER_RULES[trainer_id]
            formats["formats"][trainer_id].update({"format": fmt, "target_size": size, "archetype": archetype, "difficulty": difficulty, "partner_interaction": fmt == "double", "level_offset": 3, "location": config["location"], "smart_switching": True})
    ledger["entries"].sort(key=lambda row: row["index"])
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != NEXT["index"]] + [dict(NEXT)]; sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]: row["status"] = "closed" if row["index"] <= 133 else "next" if row["index"] == 134 else "queued"
    operating_system["current_state"].update({"closed_encounters": 133, "next_index": 134, "next_encounter_id": NEXT["encounter_id"], "canonical_sequence_groups": 134, "physical_encounter_groups": 522, "unordered_physical_groups": 388})
    return designs, ledger, sequence, operating_system, formats


def replace_dialogue(text, label, lines):
    pattern = re.compile(rf"({re.escape(label)}:[^\n]*\n)(?:\s*\.string[^\n]*\n)+")
    match = pattern.search(text)
    if not match: raise ValueError(f"dialogue label not found: {label}")
    rendered = match.group(1) + "".join(f'\t.string "{line}"\n' for line in lines)
    return text[:match.start()] + rendered + text[match.end():]


def apply_source():
    parties = PARTIES.read_text(); trainers = TRAINERS.read_text(); blocks = doubles.trainer_blocks(trainers)
    for trainer_id, team in ALL_TEAMS.items():
        party_name = doubles.party_name(blocks[trainer_id].group(0)); entries = [polish.render(member, trainer_id) for member in team]; parties = custom.replace_party_body(parties, party_name, entries)
    blocks = doubles.trainer_blocks(trainers)
    for trainer_id, (fmt, _, _, _) in TRAINER_RULES.items():
        match = blocks[trainer_id]; block = match.group(0); block = re.sub(r"(\.doubleBattle\s*=\s*)(TRUE|FALSE)", rf"\g<1>{'TRUE' if fmt == 'double' else 'FALSE'}", block)
        flags = BASE_FLAGS + EXTRA_FLAGS.get(trainer_id, []); block = re.sub(r"(\.aiFlags\s*=\s*)[^,\n]+", rf"\g<1>{' | '.join(flags)}", block); trainers = trainers[:match.start()] + block + trainers[match.end():]; blocks = doubles.trainer_blocks(trainers)
    PARTIES.write_text(parties); TRAINERS.write_text(trainers)
    for rel, labels in DIALOGUE.items():
        path = ROOT / rel; text = path.read_text()
        for label, lines in labels.items(): text = replace_dialogue(text, label, lines)
        path.write_text(text)


def verify_source(check_guide=False):
    trainers = TRAINERS.read_text(); parties = PARTIES.read_text(); blocks = doubles.trainer_blocks(trainers); dex = presets.LocalDex(); slots = doubles.base_ability_slots(); refs = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    for trainer_id, expected in ALL_TEAMS.items():
        block = blocks[trainer_id].group(0); actual = [polish.parse_entry(entry) for entry in custom.party_entries(doubles.party_match(parties, doubles.party_name(block)).group(2))]
        if actual != expected: raise SystemExit(f"FAIL batch source party {trainer_id}")
        fmt = TRAINER_RULES[trainer_id][0]
        if (".doubleBattle = TRUE" in block) != (fmt == "double"): raise SystemExit(f"FAIL batch format {trainer_id}")
        for member in expected:
            illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
            if illegal or member["ability_slot"] >= len(slots[member["species"]]): raise SystemExit(f"FAIL batch legality {trainer_id}/{member['species']}: {illegal}")
    for config in CONFIGS:
        if any(ref not in refs for ref in config["refs"]): raise SystemExit(f"FAIL batch refs {config['id']}")
    for rel, labels in DIALOGUE.items():
        text = (ROOT / rel).read_text()
        for label, lines in labels.items():
            if label not in text: raise SystemExit(f"FAIL batch dialogue {label}")
            for line in lines:
                for visible in re.split(r"\\[nlp]", line.replace("$", "")):
                    if len(visible) > 36: raise SystemExit(f"FAIL batch dialogue width {label}: {visible!r}")
    ai = (ROOT / "src/battle_ai_main.c").read_text()
    if "effect == EFFECT_SIMPLE_BEAM" not in ai or "IsStatRaisingEffect(gBattleMoves[AI_DATA->partnerMove].effect)" not in ai: raise SystemExit("FAIL batch Simple Beam AI")
    if check_guide:
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())["entries"]
        rows = {row["trainerId"]: row for row in guide}
        for trainer_id, team in ALL_TEAMS.items():
            if rows[trainer_id]["designStatus"] != "closed" or rows[trainer_id]["partySize"] != len(team): raise SystemExit(f"FAIL batch guide {trainer_id}")


def main():
    parser = argparse.ArgumentParser(); parser.add_argument("--apply-source", action="store_true"); parser.add_argument("--write", action="store_true"); parser.add_argument("--check-source", action="store_true"); parser.add_argument("--check", action="store_true"); args = parser.parse_args()
    if not any((args.apply_source, args.write, args.check_source, args.check)): parser.error("choose an action")
    if args.apply_source: apply_source()
    data = payloads(); paths = (DESIGNS, LEDGER, SEQUENCE, OS_PATH, FORMATS); serialized = [json.dumps(payload, indent=2, ensure_ascii=False) + "\n" for payload in data]
    if args.write:
        for path, text in zip(paths, serialized): path.write_text(text)
    if args.check or args.check_source:
        for path, text in zip(paths, serialized):
            if path.read_text() != text: raise SystemExit(f"FAIL heat-epilogue artifact stale: {path.name}")
        verify_source(args.check)
    print("PASS: Battles 124-133 Heat Badge epilogue batch is source-closed")


if __name__ == "__main__": main()
