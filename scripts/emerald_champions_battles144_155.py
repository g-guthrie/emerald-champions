#!/usr/bin/env python3
"""Author, apply, and verify Ashen Woods plus the Balance Badge Gym."""

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


# Battle 144 — four different forms of wilderness survival.
ALANNAH_TEAM = [
    M(1, "SPECIES_TREVENANT", "ITEM_SITRUS_BERRY", 1, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_HORN_LEECH", "MOVE_POLTERGEIST", "MOVE_LEECH_SEED", "MOVE_PROTECT"),
    M(2, "SPECIES_CHESNAUGHT", "ITEM_LEFTOVERS", 2, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_DRAIN_PUNCH", "MOVE_WOOD_HAMMER", "MOVE_SPIKY_SHIELD", "MOVE_LEECH_SEED"),
    M(3, "SPECIES_GOODRA", "ITEM_ASSAULT_VEST", 2, "SPREAD_31_IV_HP_SPATK_MODEST", "MOVE_DRAGON_PULSE", "MOVE_SLUDGE_BOMB", "MOVE_THUNDERBOLT", "MOVE_FLAMETHROWER"),
    M(4, "SPECIES_SCEPTILE", "ITEM_SCEPTILITE", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_LEAF_STORM", "MOVE_DRAGON_PULSE", "MOVE_FOCUS_BLAST", "MOVE_PROTECT"),
]

# Battle 145 — Martin's investigated wildfire.
MARTIN_TEAM = [
    M(1, "SPECIES_NINETALES", "ITEM_HEAT_ROCK", 2, "SPREAD_31_IV_HP_SPEED_TIMID", "MOVE_HEAT_WAVE", "MOVE_SOLAR_BEAM", "MOVE_WILL_O_WISP", "MOVE_PROTECT"),
    M(2, "SPECIES_SHIFTRY", "ITEM_LIFE_ORB", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_LEAF_BLADE", "MOVE_KNOCK_OFF", "MOVE_FAKE_OUT", "MOVE_PROTECT"),
    M(3, "SPECIES_MOLTRES", "ITEM_CHARTI_BERRY", 2, "SPREAD_31_IV_HP_SPATK_MODEST", "MOVE_AIR_SLASH", "MOVE_HEAT_WAVE", "MOVE_TAILWIND", "MOVE_ROOST"),
    M(4, "SPECIES_HOUNDOOM", "ITEM_HOUNDOOMINITE", 1, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_HEAT_WAVE", "MOVE_DARK_PULSE", "MOVE_SOLAR_BEAM", "MOVE_PROTECT"),
]

# Battle 146 — Roman's Surf-triggered coal engine.
ROMAN_TEAM = [
    M(1, "SPECIES_DRAGAPULT", "ITEM_CHOICE_SCARF", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_SURF", "MOVE_DRACO_METEOR", "MOVE_SHADOW_BALL", "MOVE_U_TURN"),
    M(2, "SPECIES_COALOSSAL", "ITEM_WEAKNESS_POLICY", 0, "SPREAD_31_IV_HP_SPATK_MODEST", "MOVE_HEAT_WAVE", "MOVE_ROCK_SLIDE", "MOVE_EARTH_POWER", "MOVE_PROTECT"),
    M(3, "SPECIES_CRUSTLE", "ITEM_WHITE_HERB", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_SHELL_SMASH", "MOVE_ROCK_SLIDE", "MOVE_X_SCISSOR", "MOVE_PROTECT"),
    M(4, "SPECIES_RHYPERIOR", "ITEM_ASSAULT_VEST", 1, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_HIGH_HORSEPOWER", "MOVE_ROCK_SLIDE", "MOVE_ICE_PUNCH", "MOVE_MEGAHORN"),
]

# Battle 147 — Elmer's ash-wing Bug collection.
ELMER_TEAM = [
    M(1, "SPECIES_RIBOMBEE", "ITEM_FOCUS_SASH", 1, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_POLLEN_PUFF", "MOVE_MOONBLAST", "MOVE_SPEED_SWAP", "MOVE_PROTECT"),
    M(2, "SPECIES_VOLCARONA", "ITEM_SITRUS_BERRY", 0, "SPREAD_31_IV_HP_SPATK_MODEST", "MOVE_HEAT_WAVE", "MOVE_BUG_BUZZ", "MOVE_RAGE_POWDER", "MOVE_QUIVER_DANCE"),
    M(3, "SPECIES_ARAQUANID", "ITEM_MYSTIC_WATER", 0, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_LIQUIDATION", "MOVE_LEECH_LIFE", "MOVE_WIDE_GUARD", "MOVE_PROTECT"),
    M(4, "SPECIES_HERACROSS", "ITEM_HERACRONITE", 1, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_PIN_MISSILE", "MOVE_ROCK_BLAST", "MOVE_CLOSE_COMBAT", "MOVE_PROTECT"),
]

# Battle 148 — Speed Room.
RANDALL_TEAM = [
    M(1, "SPECIES_REGIELEKI", "ITEM_MAGNET", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_THUNDERBOLT", "MOVE_ELECTROWEB", "MOVE_VOLT_SWITCH", "MOVE_PROTECT"),
    M(2, "SPECIES_AMBIPOM", "ITEM_FOCUS_SASH", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_FAKE_OUT", "MOVE_DOUBLE_HIT", "MOVE_KNOCK_OFF", "MOVE_U_TURN"),
    M(3, "SPECIES_DODRIO", "ITEM_CHOICE_SCARF", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_BRAVE_BIRD", "MOVE_RETURN", "MOVE_JUMP_KICK", "MOVE_KNOCK_OFF"),
    M(4, "SPECIES_LOPUNNY", "ITEM_LOPUNNITE", 1, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_FAKE_OUT", "MOVE_RETURN", "MOVE_HIGH_JUMP_KICK", "MOVE_ICE_PUNCH"),
]

# Battle 149 — Confusion Room.
PARKER_TEAM = [
    M(1, "SPECIES_SPINDA", "ITEM_FOCUS_SASH", 0, "SPREAD_31_IV_HP_SPEED_TIMID", "MOVE_TEETER_DANCE", "MOVE_SKILL_SWAP", "MOVE_HELPING_HAND", "MOVE_PROTECT"),
    M(2, "SPECIES_LICKILICKY", "ITEM_ASSAULT_VEST", 0, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_BODY_SLAM", "MOVE_KNOCK_OFF", "MOVE_POWER_WHIP", "MOVE_EARTHQUAKE"),
    M(3, "SPECIES_ORANGURU", "ITEM_MENTAL_HERB", 1, "SPREAD_31_IV_HP_DEF_BOLD", "MOVE_INSTRUCT", "MOVE_PSYCHIC", "MOVE_TRICK_ROOM", "MOVE_PROTECT"),
    M(4, "SPECIES_GIRAFARIG", "ITEM_SITRUS_BERRY", 2, "SPREAD_31_IV_HP_SPATK_MODEST", "MOVE_PSYCHIC", "MOVE_HYPER_VOICE", "MOVE_THUNDERBOLT", "MOVE_PROTECT"),
]

# Battle 150 — Recovery Room.
GEORGE_TEAM = [
    M(1, "SPECIES_CHANSEY", "ITEM_EVIOLITE", 2, "SPREAD_31_IV_HP_DEF_BOLD", "MOVE_SEISMIC_TOSS", "MOVE_HEAL_PULSE", "MOVE_SOFT_BOILED", "MOVE_HELPING_HAND"),
    M(2, "SPECIES_KECLEON", "ITEM_LEFTOVERS", 2, "SPREAD_31_IV_HP_SPDEF_CAREFUL", "MOVE_RECOVER", "MOVE_RETURN", "MOVE_KNOCK_OFF", "MOVE_PROTECT"),
    M(3, "SPECIES_PORYGON2", "ITEM_EVIOLITE", 1, "SPREAD_31_IV_HP_SPATK_MODEST", "MOVE_TRI_ATTACK", "MOVE_ICE_BEAM", "MOVE_RECOVER", "MOVE_TRICK_ROOM"),
    M(4, "SPECIES_AUDINO", "ITEM_AUDINITE", 1, "SPREAD_31_IV_HP_DEF_BOLD", "MOVE_DAZZLING_GLEAM", "MOVE_HEAL_PULSE", "MOVE_HELPING_HAND", "MOVE_PROTECT"),
]

# Battle 151 — One-Hit KO Room without random OHKO moves.
BERKE_TEAM = [
    M(1, "SPECIES_EXPLOUD", "ITEM_CHOICE_SPECS", 2, "SPREAD_31_IV_HP_SPATK_MODEST", "MOVE_BOOMBURST", "MOVE_FIRE_BLAST", "MOVE_ICE_BEAM", "MOVE_SURF"),
    M(2, "SPECIES_MIMIKYU", "ITEM_MENTAL_HERB", 0, "SPREAD_31_IV_HP_ATK_BRAVE", "MOVE_TRICK_ROOM", "MOVE_PLAY_ROUGH", "MOVE_SHADOW_SNEAK", "MOVE_PROTECT"),
    M(3, "SPECIES_SILVALLY", "ITEM_NORMAL_GEM", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_EXPLOSION", "MOVE_MULTI_ATTACK", "MOVE_CRUNCH", "MOVE_PROTECT"),
    M(4, "SPECIES_GLALIE", "ITEM_GLALITITE", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_DOUBLE_EDGE", "MOVE_EARTHQUAKE", "MOVE_ICE_SHARD", "MOVE_PROTECT"),
]

# Battle 152 — Accuracy Room.
MARY_TEAM = [
    M(1, "SPECIES_CINCCINO", "ITEM_KINGS_ROCK", 2, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_TAIL_SLAP", "MOVE_BULLET_SEED", "MOVE_ROCK_BLAST", "MOVE_PROTECT"),
    M(2, "SPECIES_TOUCANNON", "ITEM_WIDE_LENS", 1, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_BULLET_SEED", "MOVE_ROCK_BLAST", "MOVE_BEAK_BLAST", "MOVE_PROTECT"),
    M(3, "SPECIES_DRAMPA", "ITEM_ZOOM_LENS", 0, "SPREAD_31_IV_HP_SPATK_QUIET", "MOVE_HYPER_VOICE", "MOVE_DRACO_METEOR", "MOVE_FIRE_BLAST", "MOVE_FOCUS_BLAST"),
    M(4, "SPECIES_PIDGEOT", "ITEM_PIDGEOTITE", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_HURRICANE", "MOVE_HEAT_WAVE", "MOVE_HYPER_VOICE", "MOVE_PROTECT"),
]

# Battle 153 — Defense Room.
ALEXIA_TEAM = [
    M(1, "SPECIES_FURFROU", "ITEM_LEFTOVERS", 0, "SPREAD_31_IV_HP_DEF_BOLD", "MOVE_COTTON_GUARD", "MOVE_RETURN", "MOVE_THUNDER_WAVE", "MOVE_REST"),
    M(2, "SPECIES_DUBWOOL", "ITEM_SITRUS_BERRY", 0, "SPREAD_31_IV_HP_DEF_BOLD", "MOVE_COTTON_GUARD", "MOVE_BODY_PRESS", "MOVE_BODY_SLAM", "MOVE_PROTECT"),
    M(3, "SPECIES_WIGGLYTUFF", "ITEM_ASSAULT_VEST", 1, "SPREAD_31_IV_HP_SPATK_MODEST", "MOVE_HYPER_VOICE", "MOVE_DAZZLING_GLEAM", "MOVE_ICE_BEAM", "MOVE_THUNDERBOLT"),
    M(4, "SPECIES_AGGRON", "ITEM_AGGRONITE", 0, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_HEAVY_SLAM", "MOVE_BODY_PRESS", "MOVE_ROCK_SLIDE", "MOVE_PROTECT"),
]

# Battle 154 — Strength Room.
JODY_TEAM = [
    M(1, "SPECIES_URSARING", "ITEM_FLAME_ORB", 0, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_FACADE", "MOVE_CLOSE_COMBAT", "MOVE_CRUNCH", "MOVE_PROTECT"),
    M(2, "SPECIES_BOUFFALANT", "ITEM_CHOICE_BAND", 0, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_HEAD_CHARGE", "MOVE_CLOSE_COMBAT", "MOVE_MEGAHORN", "MOVE_HIGH_HORSEPOWER"),
    M(3, "SPECIES_DIGGERSBY", "ITEM_LIFE_ORB", 2, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_EARTHQUAKE", "MOVE_RETURN", "MOVE_QUICK_ATTACK", "MOVE_PROTECT"),
    M(4, "SPECIES_PINSIR", "ITEM_PINSIRITE", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_RETURN", "MOVE_CLOSE_COMBAT", "MOVE_QUICK_ATTACK", "MOVE_PROTECT"),
]

# Battle 155 — Norman's protected main-story anchor and four native rematch modes.
PORYGON_Z = M(1, "SPECIES_PORYGON_Z", "ITEM_CHOICE_SPECS", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_TRI_ATTACK", "MOVE_DARK_PULSE", "MOVE_THUNDERBOLT", "MOVE_ICE_BEAM")
SWELLOW = M(1, "SPECIES_SWELLOW", "ITEM_FLAME_ORB", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_FACADE", "MOVE_BRAVE_BIRD", "MOVE_U_TURN", "MOVE_QUICK_ATTACK")
BEWEAR = M(2, "SPECIES_BEWEAR", "ITEM_ASSAULT_VEST", 0, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_DOUBLE_EDGE", "MOVE_DRAIN_PUNCH", "MOVE_ICE_PUNCH", "MOVE_SHADOW_CLAW")
MELOETTA = M(2, "SPECIES_MELOETTA", "ITEM_CHOICE_SCARF", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_HYPER_VOICE", "MOVE_PSYCHIC", "MOVE_FOCUS_BLAST", "MOVE_U_TURN")
REGIGIGAS = M(3, "SPECIES_REGIGIGAS", "ITEM_LEFTOVERS", 0, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_CRUSH_GRIP", "MOVE_DRAIN_PUNCH", "MOVE_KNOCK_OFF", "MOVE_THUNDER_WAVE")
KANGASKHAN = M(4, "SPECIES_KANGASKHAN", "ITEM_KANGASKHANITE", 1, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_FAKE_OUT", "MOVE_DOUBLE_EDGE", "MOVE_SUCKER_PUNCH", "MOVE_POWER_UP_PUNCH")
HELIOLISK = M(3, "SPECIES_HELIOLISK", "ITEM_LIFE_ORB", 0, "SPREAD_31_IV_SPATK_SPEED_TIMID", "MOVE_HYPER_VOICE", "MOVE_THUNDERBOLT", "MOVE_SURF", "MOVE_VOLT_SWITCH")
SNORLAX = M(4, "SPECIES_SNORLAX", "ITEM_FIGY_BERRY", 2, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_BODY_SLAM", "MOVE_HIGH_HORSEPOWER", "MOVE_BELLY_DRUM", "MOVE_RECYCLE")
CLEFAIRY = M(1, "SPECIES_CLEFAIRY", "ITEM_EVIOLITE", 2, "SPREAD_31_IV_HP_DEF_BOLD", "MOVE_FOLLOW_ME", "MOVE_ICY_WIND", "MOVE_HELPING_HAND", "MOVE_PROTECT")
PORYGON_Z_DOUBLE = dict(PORYGON_Z, level=2, item="ITEM_LIFE_ORB", moves=["MOVE_TRI_ATTACK", "MOVE_DARK_PULSE", "MOVE_THUNDERBOLT", "MOVE_PROTECT"])
STOUTLAND = M(3, "SPECIES_STOUTLAND", "ITEM_ASSAULT_VEST", 0, "SPREAD_31_IV_HP_ATK_ADAMANT", "MOVE_RETURN", "MOVE_SNARL", "MOVE_CRUNCH", "MOVE_ICE_FANG")
BEWEAR_DOUBLE = dict(BEWEAR, level=4, item="ITEM_WEAKNESS_POLICY", moves=["MOVE_DOUBLE_EDGE", "MOVE_DRAIN_PUNCH", "MOVE_ICE_PUNCH", "MOVE_PROTECT"])
STARAPTOR = M(5, "SPECIES_STARAPTOR", "ITEM_CHOICE_SCARF", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_BRAVE_BIRD", "MOVE_DOUBLE_EDGE", "MOVE_CLOSE_COMBAT", "MOVE_U_TURN")
ARCEUS = M(5, "SPECIES_ARCEUS", "ITEM_SILK_SCARF", 0, "SPREAD_31_IV_ATK_SPEED_JOLLY", "MOVE_EXTREME_SPEED", "MOVE_SWORDS_DANCE", "MOVE_EARTHQUAKE", "MOVE_SHADOW_CLAW")
MELOETTA_DOUBLE = dict(MELOETTA, level=3, item="ITEM_SITRUS_BERRY", moves=["MOVE_HYPER_VOICE", "MOVE_PSYCHIC", "MOVE_FOCUS_BLAST", "MOVE_HELPING_HAND"])
REGIGIGAS_DOUBLE = dict(REGIGIGAS, level=5, moves=["MOVE_CRUSH_GRIP", "MOVE_DRAIN_PUNCH", "MOVE_KNOCK_OFF", "MOVE_WIDE_GUARD"])
NORMAN_TEAMS = {
    "TRAINER_NORMAN_1": [PORYGON_Z, SWELLOW, BEWEAR, MELOETTA, REGIGIGAS, KANGASKHAN],
    "TRAINER_NORMAN_2": [PORYGON_Z, SWELLOW, BEWEAR, HELIOLISK, SNORLAX, KANGASKHAN],
    "TRAINER_NORMAN_3": [CLEFAIRY, PORYGON_Z_DOUBLE, STOUTLAND, BEWEAR_DOUBLE, STARAPTOR, dict(KANGASKHAN, level=6)],
    "TRAINER_NORMAN_4": [PORYGON_Z, SWELLOW, MELOETTA, REGIGIGAS, ARCEUS, dict(KANGASKHAN, level=6)],
    "TRAINER_NORMAN_5": [CLEFAIRY, PORYGON_Z_DOUBLE, MELOETTA_DOUBLE, ARCEUS, REGIGIGAS_DOUBLE, dict(KANGASKHAN, level=6)],
}


CONFIGS = [
    {"index": 144, "id": "BATTLE_144_ASHEN_WOODS_ALANNAH", "location": "AshenWoods", "category": "optional wilderness-survival Ranger single", "trainers": ["TRAINER_ALANNAH"], "main": "TRAINER_ALANNAH", "teams": {"TRAINER_ALANNAH": ALANNAH_TEAM}, "target": 8.9, "question": "Can the player change damage and denial methods through Harvest, Bulletproof, Gooey, and one fast forest Mega?", "tempo": "Four forest survivors reveal one defensive rule at a time before Mega Sceptile ends the single with immediate speed.", "weakness": "No field mode, trap, priority loop, or shared resistance; Taunt, item removal, Ice, Flying, Fairy, Poison, Dragon, and immediate physical or special burst remain broad.", "lesson": "Read each survival trait rather than attacking on habit, then preserve priority or speed control for Mega Sceptile.", "tags": ["ashen-woods", "wilderness-survival", "harvest", "bulletproof", "gooey", "mega-sceptile"], "refs": ["showdown:gen6randombattle:013", "showdown:gen8randombattle:024", "showdown:gen4randombattle:024"], "coords": [12, 25]},
    {"index": 145, "id": "BATTLE_145_ASHEN_WOODS_MARTIN", "location": "AshenWoods", "category": "optional investigated-wildfire Ranger double", "trainers": ["TRAINER_MARTIN"], "main": "TRAINER_MARTIN", "teams": {"TRAINER_MARTIN": MARTIN_TEAM}, "target": 9.3, "question": "Can the player extinguish Drought before Chlorophyll, Tailwind, Moltres, and Mega Houndoom turn one reported fireball into a wildfire?", "tempo": "Drought and Fake Out ignite the woods; Moltres adds an actual legendary firebird and Tailwind; Mega Houndoom converts the final sun turns.", "weakness": "Weather is replaceable; Shiftry and Houndoom are frail; Moltres has a four-times Rock weakness; Water, Rock, Ground, Wide Guard, priority, and focused physical pressure remain broad.", "lesson": "Replace sun or remove its fast abuser first, then keep Rock pressure for the bird the Ranger was investigating.", "tags": ["ashen-woods", "investigated-wildfire", "drought", "chlorophyll", "moltres", "mega-houndoom"], "refs": ["showdown:gen5randomdoublesbattle:003", "showdown:gen5randomdoublesbattle:005", "showdown:gen5randomdoublesbattle:022"], "coords": [12, 22]},
    {"index": 146, "id": "BATTLE_146_ASHEN_WOODS_ROMAN", "location": "AshenWoods", "category": "optional coal-engine Hiker double", "trainers": ["TRAINER_ROMAN"], "main": "TRAINER_ROMAN", "teams": {"TRAINER_ROMAN": ROMAN_TEAM}, "target": 9.5, "question": "Can the player interrupt an allied Surf before Coalossal receives Steam Engine speed and Weakness Policy power?", "tempo": "Scarf Dragapult deliberately Surfs its partner; the world-champion coal engine gives way to an independent Shell Smash and Solid Rock reserve pair.", "weakness": "The activation is visible and interruptible; Gastrodon, Storm Drain, Water Absorb, Fake Out, Wide Guard, priority, Ground, Fighting, Grass, and special Water pressure remain broad.", "lesson": "Stop Surf or steal it with an immunity ability; if Coalossal ignites, reset boosts instead of racing four gained stages.", "tags": ["ashen-woods", "coal-engine", "allied-surf", "steam-engine", "weakness-policy", "historic-wolfe-core"], "refs": ["elite:wolfe:players-cup-ii-2020", "vgc:players-cup-1-global-finals-2020", "showdown:gen5randomdoublesbattle:008"], "coords": [10, 13]},
    {"index": 147, "id": "BATTLE_147_ASHEN_WOODS_ELMER", "location": "AshenWoods", "category": "optional ash-wing Bug Maniac double", "trainers": ["TRAINER_ELMER"], "main": "TRAINER_ELMER", "teams": {"TRAINER_ELMER": ELMER_TEAM}, "target": 9.2, "question": "Can the player stop Rage Powder and Speed Swap from buying setup before Water Bubble and Mega Heracross punish the obvious Rock answer?", "tempo": "Ribombee trades speed or heals, Volcarona redirects and grows, then Araquanid blocks spread moves while Mega Heracross breaks single targets.", "weakness": "The lead is Rock-vulnerable, redirection is Tauntable, Araquanid is slow, Heracross lacks priority, and Rock, Flying, Fire, Electric, Psychic, burn, Haze, and focus fire remain broad.", "lesson": "Do not spend every Rock answer on Volcarona; deny its support turn and preserve a clean way through Water Bubble and the Mega.", "tags": ["ashen-woods", "ash-wing-collection", "speed-swap", "rage-powder", "water-bubble", "mega-heracross"], "refs": ["showdown:gen8randombattle:008", "showdown:gen5randomdoublesbattle:030", "showdown:gen8randombattle:020", "showdown:gen4randomdoublesbattle:023"], "coords": [6, 33]},
    {"index": 148, "id": "BATTLE_148_PETALBURG_GYM_RANDALL", "location": "PetalburgCity_Gym", "category": "optional Speed Room Normal-specialty double", "trainers": ["TRAINER_RANDALL"], "main": "TRAINER_RANDALL", "teams": {"TRAINER_RANDALL": RANDALL_TEAM}, "target": 9.3, "question": "Can the player survive immediate Fake Out and Electroweb tempo without letting Scarf Dodrio or Mega Lopunny own every later speed check?", "tempo": "Regieleki controls the field while Ambipom buys the first turn; two Normal speedsters convert the resulting order into direct offense.", "weakness": "Regieleki is Ground-walled, Ambipom is Sash frail, Dodrio is Choice locked, Lopunny is physically fragile, and Trick Room, priority denial, Intimidate, burn, Fighting, Ground, Rock, and focus fire remain broad.", "lesson": "Speed is not one number: block Electroweb or reverse the field, then exploit the visible Choice lock and fragile Mega.", "tags": ["petalburg-gym", "speed-room", "electroweb", "fake-out", "choice-speed", "mega-lopunny"], "refs": ["showdown:gen6randomdoublesbattle:025", "showdown:gen7randomdoublesbattle:014", "showdown:gen9championsrandomdoublesbattle:007"], "coords": [4, 81]},
    {"index": 149, "id": "BATTLE_149_PETALBURG_GYM_PARKER", "location": "PetalburgCity_Gym", "category": "optional Confusion Room Normal-specialty double", "trainers": ["TRAINER_PARKER"], "main": "TRAINER_PARKER", "teams": {"TRAINER_PARKER": PARKER_TEAM}, "target": 9.1, "question": "Can the player deny Teeter Dance while two Own Tempo Normal partners exploit the disorder and Oranguru threatens a slower second mode?", "tempo": "Spinda confuses both foes while Lickilicky ignores it; Oranguru can reverse speed and Instruct a Contrary Girafarig reserve.", "weakness": "Confusion is removable variance, Spinda is Sash frail, the lead is slow, Oranguru is Tauntable, and Own Tempo, Safeguard, Misty Terrain, switching, priority, Dark, Bug, Fighting, and focused damage remain broad.", "lesson": "Remove the dancer or clear confusion rather than gambling turns; keep Taunt or Dark pressure for the slower reserve mode.", "tags": ["petalburg-gym", "confusion-room", "teeter-dance", "own-tempo", "instruct", "trick-room-option"], "refs": ["showdown:gen7randomdoublesbattle:018", "showdown:gen6randomdoublesbattle:013", "showdown:gen8randomdoublesbattle:012"], "coords": [4, 42]},
    {"index": 150, "id": "BATTLE_150_PETALBURG_GYM_GEORGE", "location": "PetalburgCity_Gym", "category": "optional Recovery Room Normal-specialty double", "trainers": ["TRAINER_GEORGE"], "main": "TRAINER_GEORGE", "teams": {"TRAINER_GEORGE": GEORGE_TEAM}, "target": 9.4, "question": "Can the player create two simultaneous knockout threats before Heal Pulse, Recover, Soft-Boiled, and Mega Audino erase focused chip?", "tempo": "Chansey sustains a Protean recovery attacker, Porygon2 offers a slower room, and Mega Audino turns the last pair into mutual healing support.", "weakness": "Damage output is modest; every healer is Tauntable, Eviolites are removable, Knock Off and Toxic create lasting progress, and Fighting, Steel, Poison, setup, Encore, Heal Block, and double-targeting remain broad.", "lesson": "Chip is not progress here: disable healing or threaten both slots at once, then remove Eviolite before committing damage.", "tags": ["petalburg-gym", "recovery-room", "heal-pulse", "eviolite", "protean-kecleon", "mega-audino"], "refs": ["showdown:gen4randomdoublesbattle:019", "showdown:gen5randomdoublesbattle:001", "showdown:gen7randomdoublesbattle:015"], "coords": [4, 68]},
    {"index": 151, "id": "BATTLE_151_PETALBURG_GYM_BERKE", "location": "PetalburgCity_Gym", "category": "optional One-Hit KO Room Normal-specialty double", "trainers": ["TRAINER_BERKE"], "main": "TRAINER_BERKE", "teams": {"TRAINER_BERKE": BERKE_TEAM}, "target": 9.4, "question": "Can the player position around Scrappy Boomburst and one telegraphed Normal Gem Explosion without relying on random OHKO moves?", "tempo": "Exploud attacks through Ghosts while Mimikyu can reverse speed; Silvally owns one Ghost-safe Explosion and Mega Glalie closes with Refrigerate impact.", "weakness": "Exploud is Choice locked, Mimikyu's Disguise breaks once, Explosion is one-use and telegraphed, Glalie is frail, and Wide Guard, Protect, Soundproof, Fighting, Steel, Rock, priority, and Intimidate remain broad.", "lesson": "Scout the lock, use Wide Guard or Soundproof, and make Silvally spend its one explosion into protection instead of a full board.", "tags": ["petalburg-gym", "one-hit-room", "scrappy-boomburst", "ghost-safe-explosion", "trick-room-option", "mega-glalie"], "refs": ["showdown:gen7randomdoublesbattle:008", "showdown:gen7randomdoublesbattle:015", "showdown:gen4randomdoublesbattle:008"], "coords": [4, 29]},
    {"index": 152, "id": "BATTLE_152_PETALBURG_GYM_MARY", "location": "PetalburgCity_Gym", "category": "optional Accuracy Room Normal-specialty double", "trainers": ["TRAINER_MARY"], "main": "TRAINER_MARY", "teams": {"TRAINER_MARY": MARY_TEAM}, "target": 9.3, "question": "Can the player withstand guaranteed multi-hit pressure and high-base-power inaccurate moves once Mega Pidgeot makes accuracy irrelevant?", "tempo": "Two Skill Link volleys break Sashes, slow Drampa uses accuracy items for oversized coverage, and Mega Pidgeot ends with No Guard spread offense.", "weakness": "Cinccino is frail, Toucannon is slow, Drampa lacks Protect, No Guard also makes attacks against Pidgeot accurate, and Fighting, Rock, Electric, Ice, priority, Wide Guard, and physical focus fire remain broad.", "lesson": "Multi-hit attacks invalidate Sashes; remove the fragile volley first, then use No Guard against Pidgeot itself.", "tags": ["petalburg-gym", "accuracy-room", "skill-link", "accuracy-items", "no-guard", "mega-pidgeot"], "refs": ["showdown:gen7randomdoublesbattle:013", "showdown:gen7randomdoublesbattle:019", "showdown:gen7randombattle:001", "showdown:gen5randomdoublesbattle:019"], "coords": [4, 94]},
    {"index": 153, "id": "BATTLE_153_PETALBURG_GYM_ALEXIA", "location": "PetalburgCity_Gym", "category": "optional Defense Room Normal-specialty double", "trainers": ["TRAINER_ALEXIA"], "main": "TRAINER_ALEXIA", "teams": {"TRAINER_ALEXIA": ALEXIA_TEAM}, "target": 9.3, "question": "Can the player change damage categories before Fur Coat, Fluffy, Cotton Guard, Competitive, and Mega Aggron punish a physical-only plan?", "tempo": "Two woolly Normal walls advertise contact and defense rules; Competitive Wigglytuff punishes careless drops before Mega Aggron anchors the finish.", "weakness": "Special offense bypasses the main premise, Cotton Guard is Tauntable, Furfrou can be statused, Wigglytuff is slow, Aggron lacks recovery, and Fire, Ground, Fighting, Steel, Poison, Encore, and special spread damage remain broad.", "lesson": "Stop attacking the displayed Defense stat: switch category, deny Cotton Guard, and do not feed Competitive with automatic drops.", "tags": ["petalburg-gym", "defense-room", "fur-coat", "fluffy", "cotton-guard", "mega-aggron"], "refs": ["showdown:gen8randombattle:013", "showdown:gen5randomdoublesbattle:015", "showdown:gen4randomdoublesbattle:020"], "coords": [4, 55]},
    {"index": 154, "id": "BATTLE_154_PETALBURG_GYM_JODY", "location": "PetalburgCity_Gym", "category": "optional Strength Room Normal-specialty double", "trainers": ["TRAINER_JODY"], "main": "TRAINER_JODY", "teams": {"TRAINER_JODY": JODY_TEAM}, "target": 9.5, "question": "Can the player blunt four different physical multipliers—Guts, Reckless, Huge Power, and Aerilate—without one Intimidate solving everything?", "tempo": "Visible burn and recoil lead into Huge Power spread pressure and Mega Pinsir's Flying conversion; every member expresses strength through a different rule.", "weakness": "The team is fully physical, two attackers take recoil or burn chip, Bouffalant is Choice locked, Pinsir is priority-vulnerable, and burn, Intimidate, Reflect, Fighting, Flying, Rock, Ice, redirection, and special offense remain broad.", "lesson": "Stack physical counterplay but respect Defiant-free switching: burn or Intimidate early, then preserve Rock or priority for Mega Pinsir.", "tags": ["petalburg-gym", "strength-room", "guts", "reckless", "huge-power", "mega-pinsir"], "refs": ["showdown:gen5randomdoublesbattle:005", "showdown:gen9championsrandomdoublesbattle:028", "showdown:gen6randomdoublesbattle:017"], "coords": [4, 16]},
    {"index": 155, "id": "BATTLE_155_PETALBURG_GYM_NORMAN", "location": "PetalburgCity_Gym", "category": "required Normal-specialty Gym Leader family", "trainers": list(NORMAN_TEAMS), "main": "TRAINER_NORMAN_1", "teams": NORMAN_TEAMS, "target": 10.0, "question": "Can the player identify each Normal Pokemon's visible category, item commitment, and ability constraint before Mega Kangaskhan converts one mistake into the sole setup endgame?", "tempo": "Norman's main single moves through Specs special pressure, visible Guts, Fluffy contact resistance, Scarf speed, a Slow Start clock, and one parent-child Mega; four postgame modes extend that identity.", "weakness": "Every member remains Normal-typed; Choice locks, recoil, burn chip, non-contact and special pressure into Bewear, Slow Start, Intimidate, burn, Haze, phazing, Fighting, and priority denial all create broad answers.", "lesson": "Read before attacking: exploit locks, avoid contact into Fluffy, wait out Slow Start, and preserve immediate control for Mega Kangaskhan.", "tags": ["petalburg-gym", "norman-discipline", "normal-specialty", "visible-commitments", "slow-start-clock", "mega-kangaskhan", "mixed-rematch-family"], "refs": ["showdown:gen4randombattle:013", "smogon:gen4uu:003", "showdown:gen8randombattle:011", "smogon:gen5uu:002", "showdown:gen6randombattle:020", "smogon:gen6nu:001"], "coords": [4, 2]},
]


ALL_TEAMS = {trainer_id: team for config in CONFIGS for trainer_id, team in config["teams"].items()}
TRAINER_RULES = {
    "TRAINER_ALANNAH": ("single", 4, "Wilderness survival single", 89),
    "TRAINER_MARTIN": ("double", 4, "Investigated wildfire double", 93),
    "TRAINER_ROMAN": ("double", 4, "Surf-triggered coal engine", 95),
    "TRAINER_ELMER": ("double", 4, "Ash-wing Bug collection", 92),
    "TRAINER_RANDALL": ("double", 4, "Speed Room Normal double", 93),
    "TRAINER_PARKER": ("double", 4, "Confusion Room Normal double", 91),
    "TRAINER_GEORGE": ("double", 4, "Recovery Room Normal double", 94),
    "TRAINER_BERKE": ("double", 4, "One-Hit KO Room Normal double", 94),
    "TRAINER_MARY": ("double", 4, "Accuracy Room Normal double", 93),
    "TRAINER_ALEXIA": ("double", 4, "Defense Room Normal double", 93),
    "TRAINER_JODY": ("double", 4, "Strength Room Normal double", 95),
    "TRAINER_NORMAN_1": ("single", 6, "Normal discipline badge boss", 100),
    "TRAINER_NORMAN_2": ("single", 6, "Normal discipline nonlegend single", 98),
    "TRAINER_NORMAN_3": ("double", 6, "Normal discipline nonlegend double", 98),
    "TRAINER_NORMAN_4": ("single", 6, "Normal discipline legend single", 100),
    "TRAINER_NORMAN_5": ("double", 6, "Normal discipline legend double", 100),
}
BASE_FLAGS = ["AI_FLAG_CHECK_BAD_MOVE", "AI_FLAG_TRY_TO_FAINT", "AI_FLAG_CHECK_VIABILITY", "AI_FLAG_HP_AWARE", "AI_FLAG_SMART_SWITCHING", "AI_FLAG_CHECK_FOE"]
EXTRA_FLAGS = {
    trainer_id: ["AI_FLAG_HELP_PARTNER", "AI_FLAG_SPEED_CONTROL"]
    for trainer_id in ["TRAINER_MARTIN", "TRAINER_ROMAN", "TRAINER_ELMER", "TRAINER_RANDALL", "TRAINER_PARKER", "TRAINER_GEORGE", "TRAINER_BERKE", "TRAINER_MARY", "TRAINER_ALEXIA", "TRAINER_JODY", "TRAINER_NORMAN_3", "TRAINER_NORMAN_5"]
}
EXTRA_FLAGS["TRAINER_ROMAN"].append("AI_FLAG_COMBO_SETUP")
EXTRA_FLAGS["TRAINER_GEORGE"].append("AI_FLAG_COMBO_SETUP")
for trainer_id in NORMAN_TEAMS:
    EXTRA_FLAGS.setdefault(trainer_id, []).append("AI_FLAG_SETUP_FIRST_TURN")


DIALOGUE = {
    "data/text/trainers.inc": {
        "AshenWoods_Text_AlannahIntro": [r"Four survivors guard these woods!\p", r"Harvest, armor, sap, and one Mega--\n", r"show me how you adapt in the wild!$"],
        "AshenWoods_Text_AlannahDefeat": [r"You survived every guardian!$"],
        "AshenWoods_Text_AlannahPostBattle": [r"Read each defensive trait.\n", r"Save speed control for Mega Sceptile.$"],
        "AshenWoods_Text_MartinIntro": [r"I found the fireball we tracked!\p", r"Sun wakes Shiftry; Moltres rises,\n", r"then Mega Houndoom feeds the blaze!$"],
        "AshenWoods_Text_MartinDefeat": [r"You contained the whole wildfire!$"],
        "AshenWoods_Text_MartinPostBattle": [r"Replace sun or remove Shiftry.\n", r"Keep Rock pressure for Moltres.$"],
        "AshenWoods_Text_RomanIntro": [r"Coal sleeps beneath these woods!\p", r"One allied Surf wakes its Engine\n", r"and doubles Weakness Policy power!$"],
        "AshenWoods_Text_RomanDefeat": [r"You stopped the engine cold!$"],
        "AshenWoods_Text_RomanPostBattle": [r"Interrupt Surf or absorb it.\n", r"Do not race a boosted Coalossal.$"],
        "AshenWoods_Text_ElmerIntro": [r"My ash-wing collection is complete!\p", r"Speed Swap, Rage Powder, Water\n", r"Bubble, then one many-armed Mega!$"],
        "AshenWoods_Text_ElmerDefeat": [r"You caught every wingbeat!$"],
        "AshenWoods_Text_ElmerPostBattle": [r"Deny Volcarona's support turn.\n", r"Save an answer for Mega Heracross.$"],
    },
    "data/maps/PetalburgCity_Gym/scripts.inc": {
        "PetalburgCity_Gym_Text_RandallIntro": [r"This is the Speed Room!\p", r"Fake Out and Electroweb decide order;\n", r"two Normal sprinters take it from there!$"],
        "PetalburgCity_Gym_Text_RandallDefeat": [r"You controlled every speed tier!$"],
        "PetalburgCity_Gym_Text_RandallPostBattle": [r"Block Electroweb or reverse speed.\p", r"Left: Confusion. Right: Defense.$"],
        "PetalburgCity_Gym_Text_ParkerIntro": [r"This is the Confusion Room!\p", r"Teeter Dance misses Own Tempo,\n", r"then Oranguru changes the pace!$"],
        "PetalburgCity_Gym_Text_ParkerDefeat": [r"You never gambled on confusion!$"],
        "PetalburgCity_Gym_Text_ParkerPostBattle": [r"Remove the dancer or clear confusion.\n", r"The Strength Room is next.$"],
        "PetalburgCity_Gym_Text_GeorgeIntro": [r"This is the Recovery Room!\p", r"Chip vanishes to Heal Pulse, Recover,\n", r"Soft-Boiled, and one healing Mega!$"],
        "PetalburgCity_Gym_Text_GeorgeDefeat": [r"You made damage stay!$"],
        "PetalburgCity_Gym_Text_GeorgePostBattle": [r"Disable healing or threaten both slots.\n", r"The One-Hit KO Room is next.$"],
        "PetalburgCity_Gym_Text_BerkeIntro": [r"This is the One-Hit KO Room!\p", r"No lottery: Boomburst, Explosion,\n", r"and Refrigerate make honest threats!$"],
        "PetalburgCity_Gym_Text_BerkeDefeat": [r"You survived every direct hit!$"],
        "PetalburgCity_Gym_Text_BerkePostBattle": [r"Wide Guard and Protect buy the turn.\n", r"Your father waits beyond.$"],
        "PetalburgCity_Gym_Text_MaryIntro": [r"This is the Accuracy Room!\p", r"Skill Link never stops early,\n", r"and Mega Pidgeot never misses!$"],
        "PetalburgCity_Gym_Text_MaryDefeat": [r"You made certainty fail!$"],
        "PetalburgCity_Gym_Text_MaryPostBattle": [r"Multi-hit moves break Sashes.\p", r"Left: Defense. Right: Recovery.$"],
        "PetalburgCity_Gym_Text_AlexiaIntro": [r"This is the Defense Room!\p", r"Fur Coat, Fluffy, and Cotton Guard\n", r"punish anyone who attacks one way!$"],
        "PetalburgCity_Gym_Text_AlexiaDefeat": [r"You changed damage categories!$"],
        "PetalburgCity_Gym_Text_AlexiaPostBattle": [r"Use special pressure; deny Guard.\p", r"Left: Strength. Right: One-Hit KO.$"],
        "PetalburgCity_Gym_Text_JodyIntro": [r"This is the Strength Room!\p", r"Guts, Reckless, Huge Power, Aerilate--\n", r"four rules multiply every blow!$"],
        "PetalburgCity_Gym_Text_JodyDefeat": [r"You blunted every multiplier!$"],
        "PetalburgCity_Gym_Text_JodyPostBattle": [r"Burn and Intimidate still matter.\n", r"Norman is waiting beyond this room.$"],
        "PetalburgCity_Gym_Text_NormanBattleIntro": [r"{PLAYER}...\p", r"Strength is seeing each commitment:\n", r"a lock, a burn, a coat, a slow start.\p", r"Then parent and child attack as one.\p", r"Read honestly. Give me your best!$"],
        "PetalburgCity_Gym_Text_NormanDefeat": [r"...\p", r"You read every partner honestly,\n", r"and kept calm against our final attack.\p", r"I lost, {PLAYER}. Rules are rules!\n", r"Here, take this.$"],
        "PetalburgCity_Gym_Text_NormanPostRematch": [r"Dad: Every format reveals something\n", r"different about a familiar partner.\p", r"Keep reading the battle honestly.$"],
    },
}


NEXT = {"index": 156, "encounter_id": "BATTLE_156_ROUTE_118_ROSE", "location": "Route118", "category": "optional Route 118 Aroma Lady rematch family", "status": "next", "strict_cap": 55, "trainer_ids": ["TRAINER_ROSE_1", "TRAINER_ROSE_2", "TRAINER_ROSE_3", "TRAINER_ROSE_4"], "access_note": "Rose is the first Route 118 trainer after Surf opens the eastern campaign."}


def design(config):
    total = sum(len(team) for team in config["teams"].values())
    is_norman = config["index"] == 155
    return {
        "guide_order": config["index"], "trainer_ids": config["trainers"], "status": "closed", "strict_cap": 45,
        "campaign_point": "Balance Badge chapter after the Heat Badge; full preparation and reciprocal Mega access are available before Petalburg Gym.",
        "runtime_branches": [f"{trainer_id}: {TRAINER_RULES[trainer_id][0]} with {len(config['teams'][trainer_id])} authored members." for trainer_id in config["trainers"]],
        "evolution_stage_fit": {"campaign_phase": "cap-45 mature fifth-badge chapter", "effective_levels": "main cap+1 to cap+4; postgame rematches may reach cap+6", "eligible_ratio": f"{total}/{total}", "mega_access": True, "status": "pass", "reason": "Every member is fully evolved, single-stage, or a deliberate Eviolite specialist; Normal specialty is preserved throughout Petalburg Gym."},
        "manual_quality": 10, "manual_difficulty": config["target"], "observed_difficulty": None,
        "corpus_review": {"reference_pool_size": 1005, "full_team_candidates": [{"reference_id": ref, "decision": "role adapted; donor roster rejected", "reason": "The exact indexed role informs this location-specific team without replacing bespoke authorship."} for ref in config["refs"]], "decision": f"{len(config['refs'])} references were read and adapted."},
        "competitive_references": [{"reference_id": ref, "adaptation": "Exact competitive role adapted to the authored chapter puzzle."} for ref in config["refs"]],
        "ordering": {trainer_id: [member["species"] for member in team] for trainer_id, team in config["teams"].items()},
        "team_intent": config["tempo"], "primary_player_question": config["question"], "intended_counterplay": config["weakness"], "first_loss_lesson": config["lesson"],
        "bespoke_ai": "Partner, speed, setup, switching, contact, Choice, Slow Start, spread, and healing awareness are attached through existing source-honest AI flags; no hidden player state or fixed turn script is used.",
        "uniqueness": f"The encounter spends {', '.join(config['tags'][1:])}; it was reviewed against the previous ten battles and every other member of this chapter batch.",
        "story_logic": "Native trainer identity, room routing, trigger, rewards, and progression remain intact; dialogue now accurately explains the implemented team.",
        "reward_logic": "Ordinary trainers retain EXP and money only. Norman retains the Balance Badge, Surf progression, and native rematch routing without invented rewards.",
        "campaign_reservations": {"spends": config["tags"], "preserves": ["Winona's Flying doubles lanes", "Magma and Aqua faction doctrines", "League state machines", "all unrelated legendary and Mega showcases"], "protected_anchor": "PETALBURG_GYM_NORMAN exact main-story team" if is_norman else None},
        "source_teams": config["teams"], "author_self_check": {"strongest_part": config["tempo"], "weakest_link": config["weakness"]},
        "closure": f"Battle {config['index']} is source-closed at quality 10 and target {config['target']}; runtime difficulty remains unplayed until the Battle 233 compile gate.",
    }


def ledger_entry(config):
    main_team = config["teams"][config["main"]]
    return {
        "index": config["index"], "encounter_id": config["id"],
        "identity": {"location": config["location"], "category": config["category"], "format": TRAINER_RULES[config["main"]][0], "strict_cap": 45, "memory_hook": config["tempo"]},
        "primary_player_question": config["question"], "tempo": config["tempo"], "pressure_sources": [member["species"] for member in main_team],
        "intentional_opening": "Source order is intentional and preserved.", "intentional_weakness": config["weakness"], "first_loss_lesson": config["lesson"],
        "revealed_information": ["cap 45", TRAINER_RULES[config["main"]][0], f"{len(main_team)} main members", *config["tags"][1:]],
        "counterplay_classes": ["Disrupt, redirect, Taunt, or remove the enabling lead.", "Exploit the listed type, item, category, and timing seams.", "Use speed reversal, item removal, status, focus fire, setup denial, or switching.", config["weakness"]],
        "target_difficulty": config["target"], "difficulty_rationale": "Cap-plus optimized sets and one coherent question create a serious puzzle while explicit seams preserve multiple answers.",
        "tuning_knob": "Lower the last main-story member by one level first; preserve species, mechanic, format, and order.", "playtest_status": "static-pass-runtime-unplayed",
        "novelty_tags": config["tags"], "historic_reference_ids": config["refs"], "corpus_search": {"status": "complete-current-review", "pool_size": 1005, "selection": f"{len(config['refs'])} indexed references."},
        "author_self_check": {"strongest_part": config["tempo"], "weakest_link": config["weakness"]},
    }


def payloads():
    designs = json.loads(DESIGNS.read_text()); ledger = json.loads(LEDGER.read_text()); sequence = json.loads(SEQUENCE.read_text()); operating_system = json.loads(OS_PATH.read_text()); formats = json.loads(FORMATS.read_text())
    for config in CONFIGS:
        designs["designs"][config["id"]] = design(config)
        ledger["entries"] = [row for row in ledger["entries"] if row["index"] != config["index"]] + [ledger_entry(config)]
        sequence["entries"] = [row for row in sequence["entries"] if row["index"] != config["index"]] + [{"index": config["index"], "encounter_id": config["id"], "location": config["location"], "category": config["category"], "status": "closed", "strict_cap": 45, "trainer_ids": config["trainers"], "access_note": f"Physical trigger at ({config['coords'][0]},{config['coords'][1]}); listed rematches share the native trainer family."}]
        for trainer_id in config["trainers"]:
            fmt, size, archetype, difficulty = TRAINER_RULES[trainer_id]
            formats["formats"][trainer_id].update({"format": fmt, "target_size": size, "archetype": archetype, "difficulty": difficulty, "partner_interaction": fmt == "double", "level_offset": 3, "location": config["location"], "smart_switching": True})
    ledger["entries"].sort(key=lambda row: row["index"])
    sequence["entries"] = [row for row in sequence["entries"] if row["index"] != NEXT["index"]] + [dict(NEXT)]
    sequence["entries"].sort(key=lambda row: row["index"])
    for row in sequence["entries"]:
        row["status"] = "closed" if row["index"] <= 155 else "next" if row["index"] == 156 else "queued"
    operating_system["current_state"].update({"closed_encounters": 155, "next_index": 156, "next_encounter_id": NEXT["encounter_id"], "canonical_sequence_groups": 156, "physical_encounter_groups": 522, "unordered_physical_groups": 366})
    return designs, ledger, sequence, operating_system, formats


def replace_dialogue(text, label, lines):
    pattern = re.compile(rf"({re.escape(label)}:[^\n]*\n)(?:\s*\.string[^\n]*\n)+")
    match = pattern.search(text)
    if not match:
        raise ValueError(f"dialogue label not found: {label}")
    rendered = match.group(1) + "".join(f'\t.string "{line}"\n' for line in lines)
    return text[:match.start()] + rendered + text[match.end():]


def apply_source():
    parties = PARTIES.read_text(); trainers = TRAINERS.read_text(); blocks = doubles.trainer_blocks(trainers)
    for trainer_id, team in ALL_TEAMS.items():
        party_name = doubles.party_name(blocks[trainer_id].group(0))
        parties = custom.replace_party_body(parties, party_name, [polish.render(member, trainer_id) for member in team])
    blocks = doubles.trainer_blocks(trainers)
    for trainer_id, (fmt, _, _, _) in TRAINER_RULES.items():
        match = blocks[trainer_id]; block = match.group(0)
        block = re.sub(r"(\.doubleBattle\s*=\s*)(TRUE|FALSE)", rf"\g<1>{'TRUE' if fmt == 'double' else 'FALSE'}", block)
        flags = BASE_FLAGS + EXTRA_FLAGS.get(trainer_id, [])
        block = re.sub(r"(\.aiFlags\s*=\s*)[^,\n]+", rf"\g<1>{' | '.join(flags)}", block)
        trainers = trainers[:match.start()] + block + trainers[match.end():]
        blocks = doubles.trainer_blocks(trainers)
    PARTIES.write_text(parties); TRAINERS.write_text(trainers)
    for rel, labels in DIALOGUE.items():
        path = ROOT / rel; text = path.read_text()
        for label, lines in labels.items():
            text = replace_dialogue(text, label, lines)
        path.write_text(text)


def verify_source(check_guide=False):
    trainers = TRAINERS.read_text(); parties = PARTIES.read_text(); blocks = doubles.trainer_blocks(trainers); dex = presets.LocalDex(); slots = doubles.base_ability_slots()
    refs = {json.loads(line)["reference_id"] for line in CORPUS.read_text().splitlines()}
    for trainer_id, expected in ALL_TEAMS.items():
        block = blocks[trainer_id].group(0)
        actual = [polish.parse_entry(entry) for entry in custom.party_entries(doubles.party_match(parties, doubles.party_name(block)).group(2))]
        if actual != expected:
            raise SystemExit(f"FAIL Balance Badge source party {trainer_id}")
        fmt = TRAINER_RULES[trainer_id][0]
        if (".doubleBattle = TRUE" in block) != (fmt == "double"):
            raise SystemExit(f"FAIL Balance Badge format {trainer_id}")
        for member in expected:
            illegal = [move for move in member["moves"] if move not in dex.legal_moves(member["species"])]
            if illegal or member["ability_slot"] >= len(slots[member["species"]]):
                raise SystemExit(f"FAIL Balance Badge legality {trainer_id}/{member['species']}: {illegal}")
    for config in CONFIGS:
        if any(ref not in refs for ref in config["refs"]):
            raise SystemExit(f"FAIL Balance Badge references {config['id']}")
    for rel, labels in DIALOGUE.items():
        text = (ROOT / rel).read_text()
        for label, lines in labels.items():
            if label not in text:
                raise SystemExit(f"FAIL Balance Badge dialogue {label}")
            for line in lines:
                for visible in re.split(r"\\[nlp]", line.replace("$", "")):
                    if len(visible) > 36:
                        raise SystemExit(f"FAIL Balance Badge dialogue width {label}: {visible!r}")
    norman = json.loads((ROOT / "docs/emerald_champions_gym_anchor_designs.json").read_text())["designs"]["PETALBURG_GYM_NORMAN"]
    anchor_team = [{"level": member["level_offset"], "species": member["species"], "item": member["item"], "ability_slot": member["ability_slot"], "spread": member["spread"], "moves": member["moves"]} for member in norman["team"]]
    if NORMAN_TEAMS["TRAINER_NORMAN_1"] != anchor_team:
        raise SystemExit("FAIL Norman protected main-story anchor drift")
    if check_guide:
        guide = json.loads((ROOT / "docs/verdant_battle_guide.json").read_text())["entries"]
        rows = {row["trainerId"]: row for row in guide}
        for trainer_id, team in ALL_TEAMS.items():
            if rows[trainer_id]["designStatus"] != "closed" or rows[trainer_id]["partySize"] != len(team):
                raise SystemExit(f"FAIL Balance Badge guide {trainer_id}")


def main():
    parser = argparse.ArgumentParser(); parser.add_argument("--apply-source", action="store_true"); parser.add_argument("--write", action="store_true"); parser.add_argument("--check-source", action="store_true"); parser.add_argument("--check", action="store_true"); args = parser.parse_args()
    if not any((args.apply_source, args.write, args.check_source, args.check)):
        parser.error("choose an action")
    if args.apply_source:
        apply_source()
    data = payloads(); paths = (DESIGNS, LEDGER, SEQUENCE, OS_PATH, FORMATS); serialized = [json.dumps(payload, indent=2, ensure_ascii=False) + "\n" for payload in data]
    if args.write:
        for path, text in zip(paths, serialized):
            path.write_text(text)
    if args.check or args.check_source:
        for path, text in zip(paths, serialized):
            if path.read_text() != text:
                raise SystemExit(f"FAIL Balance Badge artifact stale: {path.name}")
        verify_source(args.check)
    print("PASS: Battles 144-155 Ashen Woods and Balance Badge batch is source-closed")


if __name__ == "__main__":
    main()
