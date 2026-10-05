#!/usr/bin/env python3
"""Per-Mega-Stone register plus the campaign's story-state reachability model.

Two things live here:

1. The Mega Stone register (enabled form(s), world acquisition source, every
   trainer holder with class and whether mega_slots authorizes battle
   evolution, a signature owner, secondary owners, and the first milestone at
   which the player can reach the base family). It is a design-review index:
   "mega_slots permits evolution" is the authored bitmask, not a claim that
   the AI will Mega Evolve. Signature owner is the design tier
   elite/leader > gym > admin/boss > rival > ace > brain > regular/casual/grunt,
   ties broken by data/emerald_champions/trainer-review-index.json order.

2. `Story`: the per-milestone reachability model that scripts/reference_pool.py
   (tuning availability pools and the encounter list) is built on. See the
   "Story-state world model" section below; every hand-written gate there
   carries its source citation.
"""
from __future__ import annotations

import json
import re
import struct
from collections import defaultdict, deque
from functools import lru_cache
from pathlib import Path

import emerald_champions_teams as teams
from generate_emerald_champions_mega_archive import stones
from verify_mega_stone_rewards import world_reward_sources
from verify_trainer_ability_legality import preprocess_species_info

ROOT = Path(__file__).resolve().parents[1]
FORM_TABLES = ROOT / "src/data/pokemon/form_change_tables.h"
MASTER = ROOT / "data/emerald_champions/emerald_champions_master_battle_design.txt"
REVIEW_INDEX = ROOT / "data/emerald_champions/trainer-review-index.json"
WILD = ROOT / "src/data/wild_encounters.json"
LEGENDARY_SIGNS = ROOT / "src/data/pokemon/legendary_signs.h"
MAPS_DIR = ROOT / "data/maps"
LAYOUTS = ROOT / "data/layouts/layouts.json"
CAPS_C = ROOT / "src/caps.c"
METATILE_BEHAVIORS_H = ROOT / "include/constants/metatile_behaviors.h"
METATILE_BEHAVIOR_C = ROOT / "src/metatile_behavior.c"

# Design tier used only to pick a "signature" holder among several.
TIER = {
    "elite": 6, "leader": 6,
    "gym": 5,
    "admin": 4, "boss": 4,
    "rival": 3,
    "ace": 2,
    "brain": 1,
    "regular": 0, "casual": 0, "grunt": 0,
}

SPECIAL_FORM_NOTES = {
    "SPECIES_RAYQUAZA_MEGA": "Mega Rayquaza -- move-triggered (Dragon Ascent), not one of the item-based stones; "
                             "the player's Rayquaza may not Mega Evolve before FLAG_IS_CHAMPION "
                             "(src/emerald_champions_battle_plan.c:43-51).",
    "SPECIES_MEOWSTIC_F_MEGA": "female Meowstic form of Meowsticite (shares the stone with SPECIES_MEOWSTIC_M_MEGA).",
    "SPECIES_MAGEARNA_ORIGINAL_MEGA": "original-color Magearna form of Magearnite (shares the stone with SPECIES_MAGEARNA_MEGA).",
    "SPECIES_TATSUGIRI_CURLY_MEGA": "Tatsugiri Curly form of Tatsugirinite.",
    "SPECIES_TATSUGIRI_DROOPY_MEGA": "Tatsugiri Droopy form of Tatsugirinite.",
    "SPECIES_TATSUGIRI_STRETCHY_MEGA": "Tatsugiri Stretchy form of Tatsugirinite.",
}


def pretty(token: str) -> str:
    return re.sub(r"^(SPECIES|ITEM|MOVE)_", "", token).replace("_", " ").title()


def item_forms() -> dict[str, list[str]]:
    """ITEM_x -> every SPECIES_x_MEGA[...] it triggers (some items cover more
    than one form: Meowsticite, Magearnite, Tatsugirinite)."""
    text = FORM_TABLES.read_text()
    forms: dict[str, list[str]] = defaultdict(list)
    for species, item in re.findall(
        r"FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM,\s*(SPECIES_[A-Z0-9_]+),\s*(ITEM_[A-Z0-9_]+)", text
    ):
        forms[item].append(species)
    return dict(forms)


def move_triggered_forms() -> dict[str, str]:
    """SPECIES_x_MEGA -> MOVE_x for the non-item Mega triggers (Rayquaza)."""
    text = FORM_TABLES.read_text()
    return dict(re.findall(
        r"FORM_CHANGE_BATTLE_MEGA_EVOLUTION_MOVE,\s*(SPECIES_[A-Z0-9_]+),\s*(MOVE_[A-Z0-9_]+)", text
    ))


def base_species(mega_species: str) -> str:
    """Strip a Mega/Mega-X/Y/Z suffix back to the base species token."""
    return re.sub(r"_MEGA(_[XYZ])?$", "", mega_species)


def evolution_edges() -> dict[str, set[str]]:
    """Undirected adjacency over configured evolution relationships."""
    text = preprocess_species_info()
    text = text[text.index("const struct SpeciesInfo gSpeciesInfo[]"):]
    marks = list(re.finditer(r"\[(SPECIES_\w+)\]\s*=\s*\{", text))
    adjacency: dict[str, set[str]] = defaultdict(set)
    for i, m in enumerate(marks):
        body = text[m.end():marks[i + 1].start() if i + 1 < len(marks) else len(text)]
        evo = re.search(r"\.evolutions\s*=\s*(.*?)(?:\n\s*\.\w+\s*=|\n\s*\},?\s*\Z)", body, re.S)
        if not evo:
            continue
        for target in re.findall(r"SPECIES_\w+", evo[1]):
            adjacency[m[1]].add(target)
            adjacency[target].add(m[1])
    return adjacency


def family_of(species: str, adjacency: dict[str, set[str]]) -> set[str]:
    if species not in adjacency:
        return {species}
    seen = {species}
    stack = [species]
    while stack:
        cur = stack.pop()
        for nxt in adjacency[cur]:
            if nxt not in seen:
                seen.add(nxt)
                stack.append(nxt)
    return seen


def map_dir_to_id() -> dict[str, str]:
    result = {}
    for path in sorted(MAPS_DIR.glob("*/map.json")):
        try:
            result[path.parent.name] = json.loads(path.read_text())["id"]
        except (OSError, ValueError, KeyError):
            continue
    return result


# ---------------------------------------------------------------------------
# Story-state world model
# ---------------------------------------------------------------------------
#
# Nodes are walkable tile components, not whole maps. Every layout
# (data/layouts/*/map.bin + tileset metatile_attributes.bin) is split into
# land and water components, so the river splitting Route 118 or the sea half
# of Route 104 is a real barrier. Edges:
#   - 4-neighbour steps inside a component;
#   - land <-> water: Surf;
#   - MB_JUMP_* ledges: one way;
#   - MB_WATERFALL runs: Waterfall;
#   - MB_*_CURRENT tiles: one way, downstream, across map connections
#     (Routes 132-134 and Seafloor Cavern);
#   - EventScript_CutTree / EventScript_RockSmash / EventScript_StrengthBoulder
#     object events: their field move;
#   - map.json connections, when both edge tiles are walkable;
#   - warp_events whose source tile carries a warp behaviour (plain-floor
#     warps are arrival points or script-opened entrances,
#     src/field_control_avatar.c:1005-1019);
#   - dive/emerge connections and `setdivewarp` map scripts: Dive;
#   - the hand-written STORY_* tables below (scripted transport, blocking
#     NPCs, coord-event blockers, key items), and every map load script that
#     `setmetatile`s a wall under an unset flag or opens one under a set flag
#     (parsed generically; see Geometry._load_script_gates).
#
# A milestone window is the set of save states whose campaign cap is that
# milestone's cap (src/caps.c GetCampaignLevelCap). For each window the model
# computes a closure: walk from Littleroot, fire every story event whose tile
# is reachable and whose prerequisites hold, but allow a milestone flag only
# if adding it keeps the cap at or below the window's cap (the window exists
# when its own flag was reached). That reproduces
# src/caps.c exactly, including its Groudon special case: Groudon may be woken
# inside the badge-5 window (without Winona's badge the cap stays 55) but never
# inside the badge-6 window (badge 6 + Groudon = 65). Juan does not require
# Winona's badge either (SootopolisCity_Gym_1F/scripts.inc:72-74 battles first
# and only then points to Fortree), so badge 7 and badge 8 windows are also
# reachable without it; only the League door checks all eight
# (EverGrandeCity_PokemonLeague_1F/scripts.inc:45-52).
#
# Deliberate upper-bound simplifications: elevation, bike-only terrain,
# mud/sand slopes, Flash darkness (walkable in Gen 3), moving NPCs and
# ordinary non-blocking NPCs are ignored; a trainer/item/wild table is
# reachable when any tile of its component is. Field moves are granted when
# licence + badge hold; "a party member able to learn it" is assumed.

MILESTONE_NAMES = {
    None: "start",
    "FLAG_BADGE01_GET": "badge1", "FLAG_BADGE02_GET": "badge2", "FLAG_BADGE03_GET": "badge3",
    "FLAG_BADGE04_GET": "badge4", "FLAG_BADGE05_GET": "badge5", "FLAG_BADGE06_GET": "badge6",
    "FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT": "groudon",
    "FLAG_BADGE07_GET": "badge7", "FLAG_BADGE08_GET": "badge8",
    "FLAG_IS_CHAMPION": "champion",
}
MILESTONE_FLAGS = [f for f in MILESTONE_NAMES if f]


def campaign_milestones() -> list[tuple[str, str | None, int]]:
    """[(name, flag, cap)] in src/caps.c order, starting with the pre-badge
    fallback returned at the end of GetCampaignLevelCap's flag-list branch."""
    text = CAPS_C.read_text()
    table = text.split("sCampaignMilestones[] =", 1)[1].split("};", 1)[0]
    rows = re.findall(r"\{\s*(FLAG_\w+)\s*,\s*(\d+)\s*\}", table)
    base = int(re.search(r"return (\d+);\s*\}\s*else if \(B_LEVEL_CAP_TYPE == LEVEL_CAP_VARIABLE\)", text)[1])
    return [("start", None, base)] + [(MILESTONE_NAMES.get(f, f.lower()), f, int(c)) for f, c in rows]


def cap_of_flags(flags, milestones=None) -> int:
    """GetCampaignLevelCap (src/caps.c:34-52): the highest milestone flag held
    sets the cap, except that the Groudon step only counts once Winona's
    badge is held."""
    milestones = milestones or campaign_milestones()
    for _name, flag, cap in reversed(milestones[1:]):
        if flag == "FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT" and "FLAG_BADGE06_GET" not in flags:
            continue
        if flag in flags:
            return cap
    return milestones[0][2]


@lru_cache(maxsize=1)
def flag_aliases() -> dict[str, str]:
    """FLAG_x defined as another FLAG_y in include/constants/flags.h (e.g.
    FLAG_RECEIVED_HM01 -> FLAG_RECEIVED_HM_CUT); requirement tokens are
    compared in their canonical (numbered) spelling."""
    text = (ROOT / "include/constants/flags.h").read_text()
    return dict(re.findall(r"#define\s+(FLAG_\w+)\s+(FLAG_\w+)\s*(?:$|//)", text, re.M))


def canon(token: str) -> str:
    aliases = flag_aliases()
    seen = 0
    while token in aliases and seen < 4:
        token = aliases[token]
        seen += 1
    return token


def as_req(value) -> tuple:
    """Requirement normaliser: None -> free; 'X' -> X; ('A', 'B') -> A and B;
    [('A',), ('B', 'C')] -> A, or B and C. 'BADGES>=n' counts badge flags."""
    if not value:
        return ((),)
    if isinstance(value, tuple) and all(isinstance(alt, tuple) for alt in value):
        return value
    if isinstance(value, str):
        return ((canon(value),),)
    if isinstance(value, list):
        return tuple((canon(alt),) if isinstance(alt, str) else tuple(canon(t) for t in alt) for alt in value)
    return (tuple(canon(t) for t in value),)


def badge_count(flags) -> int:
    return sum(1 for i in range(1, 9) if f"FLAG_BADGE0{i}_GET" in flags)


def holds(req: tuple, flags) -> bool:
    for alt in req:
        if all((badge_count(flags) >= int(t[8:])) if t.startswith("BADGES>=") else (t in flags) for t in alt):
            return True
    return False


# Field moves (src/field_move.c:28-38 licences, :187-276 badges; the check is
# HasBadgeForFieldMove :60-69 plus a capable party member :115-158).
FIELD_MOVES = {
    "CAN_CUT": ("FLAG_RECEIVED_HM01", "FLAG_BADGE01_GET"),
    "CAN_ROCK_SMASH": ("FLAG_RECEIVED_HM06", "FLAG_BADGE03_GET"),
    "CAN_STRENGTH": ("FLAG_RECEIVED_HM04", "FLAG_BADGE04_GET"),
    "CAN_SURF": ("FLAG_RECEIVED_HM03", "FLAG_BADGE05_GET"),
    "CAN_DIVE": ("FLAG_RECEIVED_HM08", "FLAG_BADGE07_GET"),
    "CAN_WATERFALL": ("FLAG_RECEIVED_HM07", "FLAG_BADGE08_GET"),
}

# Story events: (flag, map, (x, y) or None, requirement, citation). A flag
# fires once its tile is reachable and its requirement holds. Pseudo-flags
# (CAN_*, HAS_*, EV_*) stand for states the game tracks elsewhere.
STORY_EVENTS: list[tuple] = [
    # Field-move licences (FLAG_RECEIVED_HM0x aliases, include/constants/flags.h:1461-1468).
    ("FLAG_RECEIVED_HM01", "RustboroCity_CuttersHouse", None, None, "RustboroCity_CuttersHouse/scripts.inc:9"),
    ("FLAG_RECEIVED_HM05", "GraniteCave_1F", None, None, "GraniteCave_1F/scripts.inc:9"),
    ("FLAG_RECEIVED_HM06", "MauvilleCity_House1", None, None, "MauvilleCity_House1/scripts.inc:9"),
    ("FLAG_RECEIVED_HM04", "RusturfTunnel", (24, 4), "CAN_ROCK_SMASH",
     "RusturfTunnel/scripts.inc:71 after a tunnel rock is smashed (src/field_specials.c:2022-2039)"),
    ("FLAG_RECEIVED_HM03", "PetalburgCity_WallysHouse", None, "FLAG_BADGE05_GET",
     "PetalburgCity_WallysHouse/scripts.inc:24, sent there after Norman (PetalburgCity_Gym/scripts.inc:538)"),
    ("FLAG_RECEIVED_HM02", "Route119", (25, 31), "CAN_SURF", "Route119/scripts.inc:149 (rival)"),
    ("FLAG_RECEIVED_HM08", "MossdeepCity_StevensHouse", None, "FLAG_DEFEATED_MAGMA_SPACE_CENTER",
     "MossdeepCity_StevensHouse/scripts.inc:29-44 (VAR_STEVENS_HOUSE_STATE 1, SpaceCenter_2F:316)"),
    ("FLAG_RECEIVED_HM07", "SootopolisCity", None, "FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE", "SootopolisCity/scripts.inc:1412"),
    *[(k, None, None, v, "src/field_move.c:28-69") for k, v in FIELD_MOVES.items()],
    # Mom gives the Old Rod with the shoes after Birch's post-rival sendoff
    # (VAR_LITTLEROOT_TOWN_STATE = 3), not on the first visit to Littleroot.
    ("HAS_OLD_ROD", "LittlerootTown", None, "FLAG_ADVENTURE_STARTED", "LittlerootTown/scripts.inc:951-959 (Mom)"),
    ("HAS_GOOD_ROD", "Route114", None, None, "Route114/scripts.inc:35-38"),
    ("HAS_SUPER_ROD", "MossdeepCity_House3", None, None, "MossdeepCity_House3/scripts.inc:12-15"),
    # Opening and badge 1.
    ("FLAG_DEFEATED_RIVAL_ROUTE103", "Route103", (10, 3), None, "Route103/scripts.inc:227"),
    ("FLAG_ADVENTURE_STARTED", "LittlerootTown_ProfessorBirchsLab", None, "FLAG_DEFEATED_RIVAL_ROUTE103",
     "LittlerootTown_ProfessorBirchsLab/scripts.inc:388"),
    ("FLAG_BADGE01_GET", "RustboroCity_Gym", None, None, "RustboroCity_Gym/scripts.inc:17"),
    ("FLAG_DEVON_GOODS_STOLEN", "RustboroCity", None, "FLAG_BADGE01_GET", "RustboroCity/scripts.inc:309-315"),
    ("FLAG_RECOVERED_DEVON_GOODS", "RusturfTunnel", (14, 5), "FLAG_DEVON_GOODS_STOLEN", "RusturfTunnel/scripts.inc:317"),
    ("FLAG_RECEIVED_POKENAV", "RustboroCity_DevonCorp_3F", None, "FLAG_RECOVERED_DEVON_GOODS",
     "RustboroCity_DevonCorp_3F/scripts.inc:61-70 (Letter, Briney appears)"),
    ("FLAG_DELIVERED_STEVEN_LETTER", "GraniteCave_StevensRoom", None, "FLAG_RECEIVED_POKENAV",
     "GraniteCave_StevensRoom/scripts.inc:13"),
    ("FLAG_HIDE_SLATEPORT_CITY_BRAWLY", "SlateportCity", (19, 26), None,
     "SlateportCity/scripts.inc:1004-1022 (removeobject Brawly in the Museum queue)"),
    ("FLAG_BADGE02_GET", "DewfordTown_Gym", None, None,
     "DewfordTown_Gym/scripts.inc:160-161 (also sets FLAG_HIDE_SLATEPORT_CITY_TEAM_AQUA)"),
    ("FLAG_HIDE_ROUTE_110_TEAM_AQUA", "SlateportCity_OceanicMuseum_2F", None, None,
     "SlateportCity_OceanicMuseum_2F/scripts.inc:85-101 (Archie)"),
    ("FLAG_DEFEATED_WALLY_MAUVILLE", "MauvilleCity", (8, 7), None, "MauvilleCity/scripts.inc:185"),
    ("FLAG_BADGE03_GET", "MauvilleCity_Gym", None, None, "MauvilleCity_Gym/scripts.inc:94"),
    ("FLAG_RECEIVED_BIKE", "MauvilleCity_BikeShop", None, None, "MauvilleCity_BikeShop/scripts.inc:24-26 (Acro Bike, no condition)"),
    ("FLAG_HIDE_ROUTE_112_TEAM_MAGMA", "MeteorFalls_1F_1R", (14, 18), None, "MeteorFalls_1F_1R/scripts.inc:101-104"),
    ("FLAG_MET_ARCHIE_METEOR_FALLS", "MeteorFalls_1F_1R", (14, 18), None, "MeteorFalls_1F_1R/scripts.inc:102"),
    ("FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY", "MtChimney", (13, 7), None, "MtChimney/scripts.inc:80 (Maxie)"),
    ("FLAG_BADGE04_GET", "LavaridgeTown_Gym_1F", None, None, "LavaridgeTown_Gym_1F/scripts.inc:63"),
    ("FLAG_RECEIVED_GO_GOGGLES", "LavaridgeTown", None, "FLAG_BADGE04_GET",
     "LavaridgeTown/scripts.inc:81-92 (rival, VAR_LAVARIDGE_TOWN_STATE 1 from LavaridgeTown_Gym_1F:70)"),
    ("FLAG_SYS_RECEIVED_KEYSTONE", "MauvilleCity_Gym", None, "FLAG_BADGE03_GET",
     "MauvilleCity_Gym/scripts.inc: MauvilleCity_Gym_EventScript_GiveMegaRing (after Wattson)"),
    ("FLAG_BADGE05_GET", "PetalburgCity_Gym", None, "FLAG_BADGE04_GET",
     "PetalburgCity_Gym/scripts.inc:448-459"),
    ("FLAG_GOT_BASEMENT_KEY_FROM_WATTSON", "MauvilleCity", (29, 10), "FLAG_BADGE05_GET",
     "MauvilleCity/scripts.inc:445-453 (Wattson is put outside after Norman, PetalburgCity_Gym:462-463)"),
    ("EV_NEW_MAUVILLE_GENERATOR_OFF", "NewMauville_Inside", None, None, "NewMauville_Inside generator (VAR_NEW_MAUVILLE_STATE)"),
    ("FLAG_GOT_TM24_FROM_WATTSON", "MauvilleCity", (29, 10), "EV_NEW_MAUVILLE_GENERATOR_OFF", "MauvilleCity/scripts.inc:445-468"),
    ("FLAG_HIDE_ROUTE_119_TEAM_AQUA", "Route119_WeatherInstitute_2F", None, None,
     "Route119_WeatherInstitute_2F/scripts.inc:73-77 (Shelly)"),
    ("FLAG_VISITED_FORTREE_CITY", "FortreeCity", None, None, "FortreeCity/scripts.inc:7"),
    ("FLAG_RECEIVED_DEVON_SCOPE", "Route120", (13, 16), None, "Route120/scripts.inc:255-260 (Steven)"),
    ("FLAG_BADGE06_GET", "FortreeCity_Gym", None, None, "FortreeCity_Gym/scripts.inc:31"),
    ("FLAG_RECEIVED_RED_OR_BLUE_ORB", "MtPyre_Summit", None, None,
     "MtPyre_Summit/scripts.inc:69-82 (Magma Emblem, Orb, Jagged Pass guard hidden); no badge check"),
    ("FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT", "MagmaHideout_4F", (16, 22), None, "MagmaHideout_4F/scripts.inc:88 (Maxie)"),
    ("FLAG_MET_TEAM_AQUA_HARBOR", "SlateportCity_Harbor", None, "FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT",
     "SlateportCity_Harbor/scripts.inc:71-85 (VAR_SLATEPORT_HARBOR_STATE 1 from MagmaHideout_4F:89)"),
    ("FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE", "AquaHideout_B2F", None, None, "AquaHideout_B2F/scripts.inc:52-53 (Matt)"),
    ("FLAG_BADGE07_GET", "MossdeepCity_Gym", None, None, "MossdeepCity_Gym/scripts.inc:65"),
    ("FLAG_DEFEATED_MAGMA_SPACE_CENTER", "MossdeepCity_SpaceCenter_2F", None, "FLAG_BADGE07_GET",
     "MossdeepCity_SpaceCenter_2F/scripts.inc:311 (Magma appear on MossdeepCity_Gym:73-76)"),
    ("FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN", "SeafloorCavern_Room9", None, None, "SeafloorCavern_Room9/scripts.inc:133-149"),
    ("FLAG_WALLACE_GOES_TO_SKY_PILLAR", "CaveOfOrigin_B1F", None, "FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN",
     "CaveOfOrigin_B1F/scripts.inc:56"),
    ("EV_RAYQUAZA_AWAKE", "SkyPillar_Top", None, "FLAG_WALLACE_GOES_TO_SKY_PILLAR",
     "SkyPillar_Top/scripts.inc:133-134 (VAR_SOOTOPOLIS_CITY_STATE RAYQUAZA_AWAKE)"),
    ("FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE", "SootopolisCity", None, "EV_RAYQUAZA_AWAKE", "SootopolisCity/scripts.inc:1493"),
    ("FLAG_BADGE08_GET", "SootopolisCity_Gym_1F", None, None,
     "SootopolisCity_Gym_1F/scripts.inc:72-86 (no badge-6 check before the battle)"),
    ("FLAG_DEFEATED_WALLY_VICTORY_ROAD", "VictoryRoad_1F", None, None, "VictoryRoad_1F/scripts.inc:46-50"),
    # Each Elite Four battle sets its member's flag, which opens that room's
    # exit (STORY_WARP_GATES below).
    *[(f"FLAG_DEFEATED_ELITE_4_{room.upper()}", f"EverGrandeCity_{room}sRoom", None, "BADGES>=8",
       f"EverGrandeCity_{room}sRoom/scripts.inc (setflag after the battle)")
      for room in ("Sidney", "Phoebe", "Glacia", "Drake")],
    ("FLAG_IS_CHAMPION", "EverGrandeCity_ChampionsRoom", None, "BADGES>=8",
     "data/scripts/hall_of_fame.inc:3 (FLAG_SYS_GAME_CLEAR: src/post_battle_event_funcs.c:30)"),
    ("FLAG_SYS_GAME_CLEAR", None, None, "FLAG_IS_CHAMPION", "src/post_battle_event_funcs.c:30 (same Hall of Fame)"),
    # Finale (src/emerald_champions_story.c:178-199 GetEmeraldChampionsFinaleStage).
    ("EV_FINALE_WALLY_DONE", "VictoryRoad_1F", None, "FLAG_SYS_GAME_CLEAR",
     "VictoryRoad_1F/scripts.inc:89 TRAINER_WALLY_VR_2 (shown by hall_of_fame.inc:14)"),
    ("EV_FINALE_VOYAGE_DONE", "SSTidalRooms", None, "EV_FINALE_WALLY_DONE", "SSTidalRooms/scripts.inc:37-75 (five cabins)"),
    ("FLAG_ENABLE_SHIP_BIRTH_ISLAND", "MeteorFalls_StevensCave", None, "EV_FINALE_VOYAGE_DONE",
     "MeteorFalls_StevensCave/scripts.inc:8-9,43-46 (Steven, Aurora Ticket)"),
    ("FLAG_EC_FINALE_DEOXYS_RESOLVED", "BirthIsland_Exterior", None, "FLAG_ENABLE_SHIP_BIRTH_ISLAND",
     "BirthIsland_Exterior/scripts.inc:91,107"),
    ("EV_BUFFEL_DONE", "LilycoveCity_CoveLilyMotel_2F", None, "FLAG_EC_FINALE_DEOXYS_RESOLVED",
     "LilycoveCity_CoveLilyMotel_2F/scripts.inc:38-47"),
    # Post-Champion side content that does not wait for Buffel.
    ("FLAG_ENABLE_SHIP_SOUTHERN_ISLAND", "Route103", None, "FLAG_SYS_GAME_CLEAR", "Route103/scripts.inc:150-180"),
    ("FLAG_ENABLE_SHIP_NAVEL_ROCK", "MossdeepCity_House1", None, "FLAG_SYS_GAME_CLEAR", "MossdeepCity_House1/scripts.inc:56-58"),
    ("FLAG_ENABLE_SHIP_FARAWAY_ISLAND", "AbandonedShip_HiddenFloorRooms", None, None,
     "AbandonedShip_HiddenFloorRooms/scripts.inc:126-130 (Old Sea Map)"),
    # Regi chain.
    ("FLAG_SYS_BRAILLE_DIG", "SealedChamber_OuterRoom", None, None, "SealedChamber_OuterRoom (Dig on the braille wall)"),
    ("FLAG_REGI_DOORS_OPENED", "SealedChamber_InnerRoom", None, "BADGES>=7",
     "SealedChamber_InnerRoom/scripts.inc:14-23,74"),
    ("FLAG_SYS_REGIROCK_PUZZLE_COMPLETED", "DesertRuins", None, None, "DesertRuins braille puzzle"),
    ("FLAG_SYS_BRAILLE_REGICE_COMPLETED", "IslandCave", None, None, "IslandCave braille puzzle"),
    ("FLAG_SYS_REGISTEEL_PUZZLE_COMPLETED", "AncientTomb", None, None, "AncientTomb braille puzzle"),
    # Abnormal weather (Terra/Marine Cave) after the skies calm.
    ("EV_ABNORMAL_WEATHER", "Route119_WeatherInstitute_2F", None, "FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE",
     "Route119_WeatherInstitute_2F/scripts.inc:144,170-190; src/field_specials.c:4143-4165"),
    # Out-of-scope markers are never set.
]

# Tiles that stay solid until a requirement holds: blocking story NPCs and
# push-back coord events. (map, x, y) -> (requirement, citation).
STORY_TILE_GATES: dict[tuple[str, int, int], tuple] = {}


def _gate_tiles(map_name, tiles, req, cite):
    for x, y in tiles:
        STORY_TILE_GATES[(map_name, x, y)] = (req, cite)


_gate_tiles("OldaleTown", [(0, 10), (1, 11)], "FLAG_ADVENTURE_STARTED",
            "OldaleTown/scripts.inc:12,21-24 (footprints man at (1,11), push-back coord at (0,10))")
_gate_tiles("RusturfTunnel", [(14, 4), (14, 5)], "FLAG_RECOVERED_DEVON_GOODS",
            "RusturfTunnel/map.json Peeko+grunt (default hidden until RustboroCity/scripts.inc:315)")
_gate_tiles("DewfordTown", [(8, 18)], "FLAG_HIDE_SLATEPORT_CITY_BRAWLY",
            "DewfordTown/map.json gym guide flag FLAG_HIDE_SLATEPORT_CITY_BRAWLY blocks the gym door")
_gate_tiles("SlateportCity", [(31, 27), (30, 27), (29, 27), (22, 27), (23, 27), (24, 27), (21, 26), (20, 26),
                              (26, 27), (28, 27), (25, 27)], "FLAG_BADGE02_GET",
            "SlateportCity/map.json Museum queue (FLAG_HIDE_SLATEPORT_CITY_TEAM_AQUA, set at DewfordTown_Gym:161)")
_gate_tiles("Route110", [(7, 83), (8, 83), (9, 83), (10, 83), (8, 82)], "FLAG_HIDE_ROUTE_110_TEAM_AQUA",
            "Route110/map.json Aqua line (FLAG_HIDE_ROUTE_110_TEAM_AQUA)")
_gate_tiles("MauvilleCity", [(8, 6), (9, 6)], "FLAG_DEFEATED_WALLY_MAUVILLE",
            "MauvilleCity/map.json Wally and uncle in front of the gym")
_gate_tiles("Route112", [(26, 30), (27, 30)], "FLAG_HIDE_ROUTE_112_TEAM_MAGMA", "Route112/map.json cable-car grunts")
_gate_tiles("MtChimney", [(13, 6), (12, 11), (32, 5), (28, 12), (19, 39), (29, 5), (31, 12), (22, 39), (23, 19),
                          (23, 18), (23, 17), (23, 20), (22, 19), (23, 21), (21, 19), (30, 12), (29, 12), (30, 5),
                          (31, 5), (20, 39), (21, 39), (24, 19), (13, 16)], "FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY",
            "MtChimney/map.json Magma/Aqua objects (hidden by MtChimney/scripts.inc:80)")
_gate_tiles("Route119", [(13, 33), (13, 34)], "FLAG_HIDE_ROUTE_119_TEAM_AQUA", "Route119/map.json bridge grunts")
_gate_tiles("Route119", [(31, 6), (25, 15)], "FLAG_RECEIVED_DEVON_SCOPE", "Route119/map.json Kecleon (data/scripts/kecleon.inc)")
_gate_tiles("FortreeCity", [(25, 8)], "FLAG_RECEIVED_DEVON_SCOPE", "FortreeCity/scripts.inc:55-84 (gym Kecleon)")
_gate_tiles("Route120", [(12, 16), (20, 11), (27, 2), (4, 77), (7, 51), (19, 48)], "FLAG_RECEIVED_DEVON_SCOPE",
            "Route120/map.json Kecleon (bridge Kecleon removed at Route120/scripts.inc:248-249)")
_gate_tiles("LilycoveCity", [(73, 15), (46, 12), (45, 12), (38, 9)], "FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE",
            "LilycoveCity/map.json Aqua grunts (FLAG_HIDE_LILYCOVE_CITY_AQUA_GRUNTS, set at AquaHideout_B2F/scripts.inc:53)")
_gate_tiles("AquaHideout_1F", [(13, 11), (14, 11)], "FLAG_MET_TEAM_AQUA_HARBOR",
            "AquaHideout_1F/map.json entrance-blocking grunts (hidden at SlateportCity_Harbor/scripts.inc:84-85)")
_gate_tiles("JaggedPass", [(16, 19)], "FLAG_RECEIVED_RED_OR_BLUE_ORB",
            "JaggedPass/map.json hideout guard (hidden at MtPyre_Summit/scripts.inc:82)")
_gate_tiles("EverGrandeCity_PokemonLeague_1F", [(9, 2), (10, 2)], "BADGES>=8",
            "EverGrandeCity_PokemonLeague_1F/scripts.inc:45-52 door guards")
_gate_tiles("Route110_SeasideCyclingRoadNorthEntrance", [(7, 4)], "FLAG_RECEIVED_BIKE",
            "Route110_SeasideCyclingRoadNorthEntrance BikeCheck (GetPlayerAvatarBike)")
_gate_tiles("Route110_SeasideCyclingRoadSouthEntrance", [(7, 4)], "FLAG_RECEIVED_BIKE",
            "Route110_SeasideCyclingRoadSouthEntrance/scripts.inc:23-33 BikeCheck (GetPlayerAvatarBike)")
_gate_tiles("Route111", [(11, 61), (12, 61), (13, 61), (14, 61), (12, 44), (13, 43), (14, 42), (16, 40),
                         (17, 39), (18, 38), (7, 63), (7, 64)], "FLAG_RECEIVED_GO_GOGGLES",
            "Route111/scripts.inc:200-217 ViciousSandstormTrigger* (checkitem ITEM_GO_GOGGLES)")

# Scripted transport and script-opened doors with no walkable map.json edge:
# (source, destination, requirement, citation); a source/destination is a
# map directory (every component) or (map, x, y).
STORY_EXTRA_EDGES: list[tuple] = [
    ("Route104_MrBrineysHouse", ("DewfordTown", 11, 9), "FLAG_RECEIVED_POKENAV",
     "Route104_MrBrineysHouse/scripts.inc:26-31,78-88; Route104/scripts.inc:382"),
    ("DewfordTown", ("Route104", 13, 51), "FLAG_RECEIVED_POKENAV", "DewfordTown/scripts.inc:35-38,117-132"),
    ("DewfordTown", ("Route109", 21, 23), "FLAG_DELIVERED_STEVEN_LETTER", "DewfordTown/scripts.inc:24,153"),
    ("Route109", ("DewfordTown", 11, 9), "FLAG_DELIVERED_STEVEN_LETTER", "Route109/scripts.inc:51"),
    *[("Route110_TrickHouseEntrance", (f"Route110_TrickHousePuzzle{n}", 0, 21), None,
       f"Route110_TrickHouseEntrance/scripts.inc:{527 + 6 * n} (puzzle door; badge gates in STORY_MAP_GATES)")
      for n in range(1, 9)],
    ("Route112_CableCarStation", "MtChimney_CableCarStation", None, "cable car special (Route112_CableCarStation/scripts.inc)"),
    ("MtChimney_CableCarStation", "Route112_CableCarStation", None, "cable car special (MtChimney_CableCarStation/scripts.inc)"),
    (("JaggedPass", 16, 18), "MagmaHideout_1F", "FLAG_RECEIVED_RED_OR_BLUE_ORB",
     "JaggedPass/scripts.inc:16-23,52-73 (Magma Emblem opens the hideout; emblem from MtPyre_Summit:78)"),
    (("NewMauville_Entrance", 4, 1), "NewMauville_Inside", "FLAG_GOT_BASEMENT_KEY_FROM_WATTSON",
     "NewMauville_Entrance/scripts.inc:24-40 (Basement Key)"),
    # Ferries: FLAG_SYS_GAME_CLEAR at both harbors (SlateportCity_Harbor/scripts.inc:212;
    # LilycoveCity_Harbor/scripts.inc:17). The Battle Frontier stop is out of scope.
    ("SlateportCity_Harbor", "SSTidalCorridor", "FLAG_SYS_GAME_CLEAR", "SlateportCity_Harbor/scripts.inc:212-251"),
    ("LilycoveCity_Harbor", "SSTidalCorridor", "FLAG_SYS_GAME_CLEAR", "LilycoveCity_Harbor/scripts.inc:17,96"),
    ("SSTidalCorridor", "SlateportCity_Harbor", None, "SSTidalCorridor/scripts.inc:9-13"),
    ("SSTidalCorridor", "LilycoveCity_Harbor", None, "SSTidalCorridor/scripts.inc:9-13"),
    ("LilycoveCity_Harbor", "SouthernIsland_Exterior", ("FLAG_SYS_GAME_CLEAR", "FLAG_ENABLE_SHIP_SOUTHERN_ISLAND"),
     "LilycoveCity_Harbor/scripts.inc:62,195"),
    ("LilycoveCity_Harbor", "NavelRock_Harbor", ("FLAG_SYS_GAME_CLEAR", "FLAG_ENABLE_SHIP_NAVEL_ROCK"),
     "LilycoveCity_Harbor/scripts.inc:69"),
    ("LilycoveCity_Harbor", "BirthIsland_Harbor", ("FLAG_SYS_GAME_CLEAR", "FLAG_ENABLE_SHIP_BIRTH_ISLAND"),
     "LilycoveCity_Harbor/scripts.inc:74-78,122-131"),
    ("LilycoveCity_Harbor", "FarawayIsland_Entrance", ("FLAG_SYS_GAME_CLEAR", "FLAG_ENABLE_SHIP_FARAWAY_ISLAND"),
     "LilycoveCity_Harbor/scripts.inc:85"),
    ("SouthernIsland_Exterior", "LilycoveCity_Harbor", None, "SouthernIsland_Exterior/scripts.inc:26"),
    ("NavelRock_Harbor", "LilycoveCity_Harbor", None, "NavelRock_Harbor/scripts.inc:21"),
    ("BirthIsland_Harbor", "LilycoveCity_Harbor", None, "BirthIsland_Harbor/scripts.inc:25"),
    ("FarawayIsland_Entrance", "LilycoveCity_Harbor", None, "FarawayIsland_Entrance/scripts.inc:36"),
]

# Warp pairs whose source tile is plain floor until a script opens it, or
# that need a requirement of their own. (source map, destination map) ->
# (requirement, citation).
STORY_WARP_GATES: dict[tuple[str, str], tuple] = {
    **{(m, "TerraCave_Entrance"): ("EV_ABNORMAL_WEATHER", "src/field_specials.c:4143-4165 (Terra Cave mouth)")
       for m in ("Route113", "Route114", "Route115", "Route116", "Route118")},
    **{(m, "Underwater_MarineCave"): ("EV_ABNORMAL_WEATHER", "src/field_specials.c:4143-4165 (Marine Cave)")
       for m in ("Underwater_Route105", "Underwater_Route125", "Underwater_Route127", "Underwater_Route129")},
    ("MeteorFalls_1F_1R", "MeteorFalls_StevensCave"): ("FLAG_SYS_GAME_CLEAR", "MeteorFalls_1F_1R OpenStevensCave on load"),
    ("SkyPillar_Outside", "SkyPillar_1F"): ("FLAG_WALLACE_GOES_TO_SKY_PILLAR", "SkyPillar_Outside/scripts.inc:25-30"),
    # Norman's gym rooms: the doors run `compare VAR_PETALBURG_GYM_STATE, 6`
    # (four gym wins) and warp inside the same map (PetalburgCity_Gym/scripts.inc:812-890).
    ("PetalburgCity_Gym", "PetalburgCity_Gym"): ("FLAG_BADGE04_GET", "PetalburgCity_Gym/scripts.inc:812-846"),
    # Each Elite Four room's exit opens when its own member is beaten
    # (setflag FLAG_DEFEATED_ELITE_4_x in EverGrandeCity_<Member>sRoom/scripts.inc);
    # the run starts at the eight-badge door (EverGrandeCity_PokemonLeague_1F/scripts.inc:45-52).
    **{(f"EverGrandeCity_{room}sRoom", hall): (f"FLAG_DEFEATED_ELITE_4_{room.upper()}",
                                               f"EverGrandeCity_{room}sRoom/scripts.inc (exit after the battle)")
       for room, hall in (("Sidney", "EverGrandeCity_Hall1"), ("Phoebe", "EverGrandeCity_Hall2"),
                          ("Glacia", "EverGrandeCity_Hall3"), ("Drake", "EverGrandeCity_Hall4"))},
    # The Champion's door is the one Wallace's defeat script opens and walks the
    # player through (EverGrandeCity_ChampionsRoom/scripts.inc:51-54, 144); the
    # manifest clears this token only once that script's battle is won.
    ("EverGrandeCity_ChampionsRoom", "EverGrandeCity_HallOfFame"): (
        "SCRIPT_OPEN:EverGrandeCity_ChampionsRoom:6:2", "EverGrandeCity_ChampionsRoom/scripts.inc:51-54"),
}

# Whole-map entry requirements. (map prefix match) -> (requirement, citation).
STORY_MAP_GATES: dict[str, tuple] = {
    # Trick House puzzles (Route110_TrickHouseEntrance/scripts.inc:55-100).
    "Route110_TrickHousePuzzle2": ("FLAG_BADGE03_GET", "Route110_TrickHouseEntrance/scripts.inc:76"),
    "Route110_TrickHousePuzzle3": ("FLAG_BADGE04_GET", "Route110_TrickHouseEntrance/scripts.inc:80"),
    "Route110_TrickHousePuzzle4": ("FLAG_BADGE05_GET", "Route110_TrickHouseEntrance/scripts.inc:84"),
    "Route110_TrickHousePuzzle5": ("FLAG_BADGE06_GET", "Route110_TrickHouseEntrance/scripts.inc:88"),
    "Route110_TrickHousePuzzle6": ("FLAG_BADGE07_GET", "Route110_TrickHouseEntrance/scripts.inc:92"),
    "Route110_TrickHousePuzzle7": ("FLAG_BADGE08_GET", "Route110_TrickHouseEntrance/scripts.inc:96"),
    "Route110_TrickHousePuzzle8": ("FLAG_SYS_GAME_CLEAR", "Route110_TrickHouseEntrance/scripts.inc:100"),
    # Scope: the Frontier (and the Champions Circuit behind its desks) and
    # Trainer Hill never count before Buffel. Nothing sets this token.
    "BattleFrontier_": ("OUT_OF_SCOPE", "AGENTS.md scope: Frontier/Circuit after the finale"),
    "TrainerHill_": ("OUT_OF_SCOPE", "reached only from the Battle Frontier"),
    "ArtisanCave": ("OUT_OF_SCOPE", "Battle Frontier only"),
}

# Map load scripts whose setmetatile walls are puzzles or cosmetics, not
# progression gates; the generic load-script parser skips them.
LOAD_GATE_SKIP_MAPS = {"MossdeepCity_Gym", "Route110_TrickHousePuzzle7", "ShoalCave_LowTideInnerRoom",
                       "LilycoveCity_LilycoveMuseum_2F", "SandstrewnRuins", "MossdeepCity_StevensHouse"}
LOAD_GATE_SKIP_PREFIXES = ("AbandonedShip_",)

# Gym and Trick House interiors whose barriers are switch/door/ice puzzles
# (setmetatile at runtime, e.g. MauvilleGymPressSwitch): solvable once the
# map is entered, so every component inside is linked (upper bound).
PUZZLE_MAPS = {"MauvilleCity_Gym", "MossdeepCity_Gym", "SootopolisCity_Gym_1F", "SootopolisCity_Gym_B1F",
               "FortreeCity_Gym", "LavaridgeTown_Gym_1F", "LavaridgeTown_Gym_B1F", "RustboroCity_Gym",
               "DewfordTown_Gym", *[f"Route110_TrickHousePuzzle{n}" for n in range(1, 9)]}

SEED_TILES = [("LittlerootTown", 10, 10)]
CURRENT_BEHAVIORS = {"MB_EASTWARD_CURRENT": (1, 0), "MB_WESTWARD_CURRENT": (-1, 0),
                     "MB_NORTHWARD_CURRENT": (0, -1), "MB_SOUTHWARD_CURRENT": (0, 1)}


class Geometry:
    """Tile components and their edges over every data/maps layout."""

    OBJECT_GATES = {"EventScript_CutTree": "CAN_CUT", "EventScript_RockSmash": "CAN_ROCK_SMASH",
                    "EventScript_StrengthBoulder": "CAN_STRENGTH"}
    DIRS = ((1, 0), (-1, 0), (0, 1), (0, -1))
    # Warp sources the engine never triggers by walking (IsWarpMetatileBehavior
    # and the arrow/door checks, src/field_control_avatar.c:1005-1100).
    INERT_WARP_BEHAVIORS = ("MB_NORMAL", "MB_MOUNTAIN_TOP", "MB_ICE", "MB_INDOOR_ENCOUNTER")

    def __init__(self):
        enum = METATILE_BEHAVIORS_H.read_text().split("enum {", 1)[1].split("};", 1)[0]
        enum = re.sub(r"//[^\n]*|/\*.*?\*/", "", enum, flags=re.S)
        self.mb = {name: i for i, name in enumerate(re.findall(r"\bMB_\w+\b", enum))}
        rows = re.findall(r"\[(MB_\w+)\]\s*=\s*([^,\n]+)", METATILE_BEHAVIOR_C.read_text())
        self.surfable = {self.mb[n] for n, f in rows if "TILE_FLAG_SURFABLE" in f and n in self.mb}
        self.encounter_tiles = {self.mb[n] for n, f in rows if "TILE_FLAG_HAS_ENCOUNTERS" in f and n in self.mb}
        self.waterfall = self.mb["MB_WATERFALL"]
        # MetatileBehavior_IsDiveable, src/metatile_behavior.c:814-822
        self.diveable = {self.mb[n] for n in ("MB_INTERIOR_DEEP_WATER", "MB_DEEP_WATER", "MB_SOOTOPOLIS_DEEP_WATER")}
        self.jumps = {self.mb["MB_JUMP_EAST"]: (1, 0), self.mb["MB_JUMP_WEST"]: (-1, 0),
                      self.mb["MB_JUMP_NORTH"]: (0, -1), self.mb["MB_JUMP_SOUTH"]: (0, 1)}
        self.currents = {self.mb[n]: v for n, v in CURRENT_BEHAVIORS.items()}
        # Walkable bridges over water (MB_BRIDGE_OVER_*): elevation 15 tiles
        # walked on top at the bridge's height and surfed under at the water's.
        self.bridges = {i for n, i in self.mb.items() if n.startswith("MB_BRIDGE_OVER")}
        self.cracked = {self.mb[n] for n in ("MB_CRACKED_FLOOR", "MB_CRACKED_FLOOR_HOLE") if n in self.mb}
        self.counter = self.mb.get("MB_COUNTER")
        self.inert_warp = {self.mb[n] for n in self.INERT_WARP_BEHAVIORS}
        self.maps: dict[str, dict] = {}
        for path in sorted(MAPS_DIR.glob("*/map.json")):
            try:
                self.maps[path.parent.name] = json.loads(path.read_text())
            except (OSError, ValueError):
                continue
        self.id_to_dir = {m["id"]: d for d, m in self.maps.items() if "id" in m}
        self.dir_to_id = {d: i for i, d in self.id_to_dir.items()}
        self.underwater = {d for d, m in self.maps.items() if m.get("map_type") == "MAP_TYPE_UNDERWATER"}
        layouts = {l["id"]: l for l in json.loads(LAYOUTS.read_text())["layouts"] if "id" in l}
        attrs: dict[tuple[str, str], bytes] = {}

        def attr(kind: str, name: str) -> bytes:
            if (kind, name) not in attrs:
                folder = re.sub(r"(?<!^)(?=[A-Z])", "_", name.removeprefix("gTileset_")).lower()
                p = ROOT / "data/tilesets" / kind / folder / "metatile_attributes.bin"
                attrs[kind, name] = p.read_bytes() if p.exists() else b""
            return attrs[kind, name]

        self.grid: dict[str, tuple[int, int, tuple, list]] = {}
        for d, m in self.maps.items():
            l = layouts.get(self._effective_layout(d, m.get("layout")))
            if not l or "blockdata_filepath" not in l:
                continue
            w, h = l["width"], l["height"]
            words = struct.unpack("<" + "H" * (w * h), (ROOT / l["blockdata_filepath"]).read_bytes()[:2 * w * h])
            prim, sec = attr("primary", l["primary_tileset"]), attr("secondary", l["secondary_tileset"])
            beh = []
            for word in words:
                t = word & 0x3FF
                src, idx = (prim, t) if t < 512 else (sec, t - 512)
                beh.append(struct.unpack_from("<H", src, 2 * idx)[0] & 0xFF if 2 * idx + 2 <= len(src) else 0)
            self.grid[d] = (w, h, words, beh)
        # Tile gates: story blockers, generic load-script walls, field-move objects.
        self.tile_gates: dict[tuple[str, int, int], tuple] = {}
        self.tile_gate_cites: dict[tuple[str, int, int], str] = {}
        for key, (req, cite) in STORY_TILE_GATES.items():
            self.tile_gates[key] = as_req(req)
            self.tile_gate_cites[key] = cite
        for key, req, cite in self._load_script_gates():
            self.tile_gates.setdefault(key, req)
            self.tile_gate_cites.setdefault(key, cite)
        for d, m in self.maps.items():
            for o in m.get("object_events") or []:
                g = self.OBJECT_GATES.get(o.get("script"))
                if g and isinstance(o.get("x"), int):
                    self.tile_gates.setdefault((d, o["x"], o["y"]), ((g,),))
                    self.tile_gate_cites.setdefault((d, o["x"], o["y"]), f"{d}/map.json {o['script']}")
        self._build()

    @staticmethod
    def _effective_layout(d: str, layout: str | None) -> str | None:
        """A layout the map's transition script always swaps in with an
        unconditional `setmaplayoutindex` (e.g. Route131 always loads
        LAYOUT_ROUTE131_SKY_PILLAR, Route131/scripts.inc:5-13)."""
        path = MAPS_DIR / d / "scripts.inc"
        if not path.exists():
            return layout
        text = path.read_text()
        entry = re.search(r"map_script\s+MAP_SCRIPT_ON_TRANSITION\s*,\s*(\w+)", text)
        if not entry:
            return layout
        bodies = {}
        for m in re.finditer(r"(?m)^(\w+)::?[^\n]*\n(.*?)(?=^\w+::?|\Z)", text, re.S):
            bodies[m[1]] = m[2]
        label, seen = entry[1], set()
        while label in bodies and label not in seen:
            seen.add(label)
            nxt = None
            for line in bodies[label].splitlines():
                line = line.split("@")[0].strip()
                m = re.match(r"setmaplayoutindex\s+(LAYOUT_\w+)", line)
                if m:
                    return m[1]
                m = re.match(r"(call|goto)\s+(\w+)$", line)
                if m and m[2] in bodies:
                    sub = bodies[m[2]]
                    lm = re.search(r"^\s*setmaplayoutindex\s+(LAYOUT_\w+)", sub, re.M)
                    if lm and not re.search(r"^\s*(?:goto_if|call_if|compare)", sub, re.M):
                        return lm[1]
                    if m[1] == "goto":
                        nxt = m[2]
                        break
                if line in ("end", "return"):
                    break
            label = nxt
        return layout

    def _load_script_gates(self):
        """Walls a map's load/transition scripts place under a flag:
        `call_if_unset FLAG, L` whose L setmetatiles a solid tile blocks it
        until FLAG; `call_if_set FLAG, L` whose L opens a tile (collision 0)
        opens it only with FLAG (e.g. Route103 Altering Cave on
        FLAG_SYS_GAME_CLEAR, Route103/scripts.inc:13-19)."""
        label_re = re.compile(r"^([A-Za-z_]\w*)::?")
        for d in self.grid:
            if d in LOAD_GATE_SKIP_MAPS or d.startswith(LOAD_GATE_SKIP_PREFIXES):
                continue
            path = MAPS_DIR / d / "scripts.inc"
            if not path.exists():
                continue
            text = path.read_text()
            bodies: dict[str, list[tuple[int, str]]] = {}
            cur = None
            for n, raw in enumerate(text.splitlines(), 1):
                line = raw.split("@")[0]
                if line and not line[0].isspace():
                    m = label_re.match(line.strip())
                    if m:
                        cur = m[1]
                        bodies[cur] = []
                        continue
                if cur and line.strip():
                    bodies[cur].append((n, line.strip()))

            def tiles(label, depth=0, seen=None):
                seen = seen if seen is not None else set()
                if label not in bodies or label in seen or depth > 3:
                    return []
                seen.add(label)
                out = []
                for n, line in bodies[label]:
                    m = re.match(r"setmetatile\s+(\d+),\s*(\d+),\s*\w+,\s*(\w+)", line)
                    if m:
                        out.append((int(m[1]), int(m[2]), m[3], n))
                    m = re.match(r"(?:call|goto)\s+(\w+)$", line)
                    if m:
                        out += tiles(m[1], depth + 1, seen)
                return out

            entries = re.findall(r"map_script\s+MAP_SCRIPT_ON_(?:LOAD|TRANSITION|RESUME)\s*,\s*(\w+)", text)
            seen, stack = set(), list(entries)
            while stack:
                label = stack.pop()
                if label in seen or label not in bodies:
                    continue
                seen.add(label)
                for _n, line in bodies[label]:
                    m = re.match(r"(call_if_set|call_if_unset|goto_if_set|goto_if_unset)\s+(FLAG_\w+),\s*(\w+)", line)
                    if m:
                        opens = m[1].endswith("_set")
                        for x, y, coll, n in tiles(m[3]):
                            if opens and coll in ("0", "FALSE"):
                                yield (d, x, y), as_req(m[2]), f"{d}/scripts.inc:{n} ({m[1]} {m[2]})"
                            elif not opens and coll not in ("0", "FALSE"):
                                yield (d, x, y), as_req(m[2]), f"{d}/scripts.inc:{n} ({m[1]} {m[2]})"
                    m = re.match(r"(?:call|goto)\s+(\w+)$", line)
                    if m:
                        stack.append(m[1])

    def kind(self, d: str, x: int, y: int) -> str | None:
        """L land, W water, B bridge over water (walkable on top, surfable
        underneath), F waterfall, J ledge, C current, None solid."""
        w, h, words, beh = self.grid[d]
        if not (0 <= x < w and 0 <= y < h):
            return None
        b = beh[y * w + x]
        if b == self.waterfall:
            return "F"
        if b in self.jumps:
            return "J"
        if b in self.currents:
            return "C"
        if words[y * w + x] & 0xC00:
            return None
        if d in self.underwater:
            return "W"  # every open tile of an underwater map is swum (Dive)
        if b in self.bridges:
            return "B"
        return "W" if b in self.surfable else "L"

    def behavior(self, d: str, x: int, y: int) -> int | None:
        w, h, _words, beh = self.grid[d]
        return beh[y * w + x] if 0 <= x < w and 0 <= y < h else None

    def elevation(self, d: str, x: int, y: int) -> int:
        w, h, words, _beh = self.grid[d]
        return words[y * w + x] >> 12 if 0 <= x < w and 0 <= y < h else 0

    def _cross(self, d: str, x: int, y: int):
        """(map, x, y) for a step that leaves map d at (x, y) through a
        map.json connection, else None."""
        w, h, _w, _b = self.grid[d]
        for conn in self.maps[d].get("connections") or []:
            o = self.id_to_dir.get(conn.get("map"))
            if not o or o not in self.grid:
                continue
            ow, oh, _ow, _ob = self.grid[o]
            direction, off = conn["direction"], int(conn.get("offset", 0))
            if direction == "up" and y < 0:
                return o, x - off, oh + y
            if direction == "down" and y >= h:
                return o, x - off, y - h
            if direction == "left" and x < 0:
                return o, ow + x, y - off
            if direction == "right" and x >= w:
                return o, x - w, y - off
        return None

    @staticmethod
    def step_z(z: int, ne: int) -> int | None:
        """Elevation after stepping onto a tile of elevation ne, or None when
        IsElevationMismatchAt blocks it (src/event_object_movement.c:9433-9449;
        the object's elevation follows the tile except on multi-level 15,
        ObjectEventUpdateElevation :9496-9513)."""
        if z != 0 and ne not in (0, 15) and ne != z:
            return None
        return z if ne == 15 else ne

    def _build(self) -> None:
        # Nodes are (map, x, y, elevation, mode) states grouped into
        # components; mode is "L" on foot or "W" surfing.
        state: dict[tuple, int] = {}
        nodes: list[tuple[str, str]] = []
        tile_nodes: dict[tuple[str, int, int], list[int]] = defaultdict(list)
        gates = self.tile_gates
        ok = {"L": ("L", "B"), "W": ("W", "B")}
        for d, (w, h, _words, _beh) in self.grid.items():
            for y in range(h):
                for x in range(w):
                    k = self.kind(d, x, y)
                    if k not in ("L", "W") or (d, x, y) in gates:
                        continue
                    z = self.elevation(d, x, y)
                    if z == 15:
                        continue
                    if (d, x, y, z, k) in state:
                        continue
                    nid = len(nodes)
                    nodes.append((d, k))
                    state[d, x, y, z, k] = nid
                    queue = deque([(x, y, z)])
                    while queue:
                        cx, cy, cz = queue.popleft()
                        for dx, dy in self.DIRS:
                            nx, ny = cx + dx, cy + dy
                            if (d, nx, ny) in gates or self.kind(d, nx, ny) not in ok[k]:
                                continue
                            nz = self.step_z(cz, self.elevation(d, nx, ny))
                            if nz is None or (d, nx, ny, nz, k) in state:
                                continue
                            state[d, nx, ny, nz, k] = nid
                            queue.append((nx, ny, nz))
        for (d, x, y, _z, _k), nid in state.items():
            if nid not in tile_nodes[d, x, y]:
                tile_nodes[d, x, y].append(nid)
        self.state, self.nodes, self.tile_nodes = state, nodes, tile_nodes
        self.by_map: dict[str, list[int]] = defaultdict(list)
        for i, (d, _k) in enumerate(nodes):
            self.by_map[d].append(i)
        # edges[node] = [(target, requirement, via)]
        self.edges: dict[int, list[tuple[int, tuple, str]]] = defaultdict(list)
        free: tuple = ((),)
        surf = (("CAN_SURF",),)

        def add(a, b, req=free, via="", both=True):
            if a is None or b is None or a == b:
                return
            self.edges[a].append((b, req, via))
            if both:
                self.edges[b].append((a, req, via))

        def modes(d, x, y, mode):
            return [n for n in tile_nodes.get((d, x, y), []) if nodes[n][1] == mode]

        for d, (w, h, _words, beh) in self.grid.items():
            for y in range(h):
                for x in range(w):
                    k = self.kind(d, x, y)
                    if k in ("L", "B"):
                        # Surf from elevation-3 ground onto facing water and
                        # back (IsPlayerFacingSurfableFishableWater,
                        # src/field_player_avatar.c:1640-1650; CanStopSurfing :1020-1034).
                        if self.elevation(d, x, y) != 3:
                            continue
                        for a in modes(d, x, y, "L"):
                            for dx, dy in self.DIRS:
                                for o in modes(d, x + dx, y + dy, "W"):
                                    add(a, o, surf, "surf")
                    elif k == "J":
                        dx, dy = self.jumps[beh[y * w + x]]
                        for a in modes(d, x - dx, y - dy, "L"):
                            for b in modes(d, x + dx, y + dy, "L"):
                                add(a, b, via="ledge", both=False)
                    elif k == "C":
                        exits = self._follow_current(d, x, y)
                        for dx, dy in self.DIRS:
                            entry = modes(d, x + dx, y + dy, "W")
                            if self.elevation(d, x + dx, y + dy) == 3:
                                entry += modes(d, x + dx, y + dy, "L")
                            for o in entry:
                                for e in exits:
                                    add(o, e, surf, "current", both=False)
            for x in range(w):
                y = 0
                while y < h:
                    if self.kind(d, x, y) != "F":
                        y += 1
                        continue
                    y0 = y
                    while y < h and self.kind(d, x, y) == "F":
                        y += 1
                    for a in modes(d, x, y0 - 1, "W"):
                        for b in modes(d, x, y, "W"):
                            add(a, b, (("CAN_WATERFALL",),), "waterfall")
        # A cluster of adjacent gate tiles (a wall of Wailmer, a queue of
        # grunts, a Rock Smash + Strength puzzle) opens as a whole once every
        # requirement in it holds: link every component around it.
        seen_gate: set = set()
        for key in gates:
            d = key[0]
            if d not in self.grid or key in seen_gate:
                continue
            region, queue = {key}, deque([key])
            while queue:
                _d, x, y = queue.popleft()
                for dx, dy in self.DIRS:
                    nk = (d, x + dx, y + dy)
                    if nk not in region and nk in gates:
                        region.add(nk)
                        queue.append(nk)
            seen_gate |= region
            req = ((),)
            for r in dict.fromkeys(gates[k] for k in region):
                req = req_and(req, r)
            around = {n for (_d, x, y) in region for dx, dy in self.DIRS
                      for n in tile_nodes.get((d, x + dx, y + dy), [])}
            for a in around:
                for b in around:
                    if nodes[a][1] == nodes[b][1]:
                        add(a, b, req, f"gate@{d}:{key[1]},{key[2]}", both=False)
        for d, m in self.maps.items():
            if d not in self.grid:
                continue
            w, h, _words, beh = self.grid[d]
            for warp in m.get("warp_events") or []:
                dest = self.id_to_dir.get(warp.get("dest_map"))
                try:
                    wid = int(warp.get("dest_warp_id"))
                except (TypeError, ValueError):
                    continue
                if not dest or dest not in self.grid:
                    continue
                targets = self.maps[dest].get("warp_events") or []
                if not 0 <= wid < len(targets):
                    continue
                key = (d, warp["x"], warp["y"])
                req = free
                if (d, dest) in STORY_WARP_GATES:
                    req = as_req(STORY_WARP_GATES[d, dest][0])
                elif key in gates:
                    req = gates[key]
                elif self.behavior(*key) in self.inert_warp:
                    continue
                for a, areq in self.approach(d, warp["x"], warp["y"]):
                    for b in self.near(dest, targets[wid]["x"], targets[wid]["y"])[:1]:
                        add(a, b, req_and(req, areq), f"warp {d}->{dest}", both=False)
            for conn in m.get("connections") or []:
                o = self.id_to_dir.get(conn.get("map"))
                if not o or o not in self.grid:
                    continue
                ow, oh, _ow, _ob = self.grid[o]
                direction, off = conn["direction"], int(conn.get("offset", 0))
                if direction in ("dive", "emerge"):
                    for yy in range(min(h, oh)):
                        for xx in range(min(w, ow)):
                            if direction == "dive" and beh[yy * w + xx] not in self.diveable:
                                continue
                            for a in tile_nodes.get((d, xx, yy), []):
                                for b in tile_nodes.get((o, xx, yy), [])[:1]:
                                    add(a, b, (("CAN_DIVE",),), "dive", both=False)
                    continue
                if direction in ("up", "down"):
                    sy, ty = (0, oh - 1) if direction == "up" else (h - 1, 0)
                    pairs = [((xx, sy), (xx - off, ty)) for xx in range(w)]
                else:
                    sx, tx = (0, ow - 1) if direction == "left" else (w - 1, 0)
                    pairs = [((sx, yy), (tx, yy - off)) for yy in range(h)]
                for (ax, ay), (bx, by) in pairs:
                    for mode in ("L", "W"):
                        src = [(z, n) for (z, n) in self._states_at(d, ax, ay, mode)]
                        if not src:
                            continue
                        ne = self.elevation(o, bx, by) if 0 <= bx < ow and 0 <= by < oh else 0
                        for z, a in src:
                            nz = self.step_z(z, ne)
                            if nz is None:
                                continue
                            b = state.get((o, bx, by, nz, mode))
                            if b is None:
                                cands = modes(o, bx, by, mode)
                                b = cands[0] if cands else None
                            add(a, b, surf if mode == "W" else free, "connection", both=False)
            if d in PUZZLE_MAPS:
                inside = [n for n in self.by_map.get(d, []) if nodes[n][1] == "L"]
                for a in inside:
                    for b in inside:
                        add(a, b, via="puzzle", both=False)
            script = MAPS_DIR / d / "scripts.inc"
            if script.exists():
                # Cracked floors drop to the setholewarp map at the same
                # coordinates (SkyPillar_2F/4F, GraniteCave_B1F).
                for target in re.findall(r"setholewarp\s+(MAP_\w+)", script.read_text()):
                    o = self.id_to_dir.get(target)
                    if not o or o not in self.grid:
                        continue
                    for yy in range(h):
                        for xx in range(w):
                            if beh[yy * w + xx] not in self.cracked:
                                continue
                            for a in tile_nodes.get((d, xx, yy), []):
                                for b in self.near(o, xx, yy)[:1]:
                                    add(a, b, via="hole", both=False)
                for target, x, y in re.findall(r"setdivewarp\s+(MAP_\w+),\s*\d+,\s*(\d+),\s*(\d+)", script.read_text()):
                    o = self.id_to_dir.get(target)
                    if not o or o not in self.grid:
                        continue
                    underwater = m.get("map_type") == "MAP_TYPE_UNDERWATER"
                    sources = {n for (sd, xx, yy), ns in tile_nodes.items() if sd == d
                               and (underwater or beh[yy * w + xx] in self.diveable) for n in ns}
                    for s_ in sources:
                        for t in self.near(o, int(x), int(y))[:1]:
                            add(s_, t, (("CAN_DIVE",),), "setdivewarp", both=False)

    def _states_at(self, d: str, x: int, y: int, mode: str) -> list[tuple[int, int]]:
        if not hasattr(self, "_state_index"):
            index: dict[tuple, list[tuple[int, int]]] = defaultdict(list)
            for (sd, sx, sy, sz, sk), n in self.state.items():
                index[sd, sx, sy, sk].append((sz, n))
            self._state_index = index
        return self._state_index.get((d, x, y, mode), [])

    def _follow_current(self, d: str, x: int, y: int) -> list[int]:
        """Water components a current carries the swimmer to: the first calm
        tile downstream, or, where the current runs into a wall, the calm
        water beside the tile it stalls on."""
        for _ in range(400):
            b = self.behavior(d, x, y)
            if b not in self.currents:
                return [n for n in self.tile_nodes.get((d, x, y), []) if self.nodes[n][1] == "W"]
            dx, dy = self.currents[b]
            nx, ny, nd = x + dx, y + dy, d
            w, h, _w, _b = self.grid[d]
            if not (0 <= nx < w and 0 <= ny < h):
                crossed = self._cross(d, nx, ny)
                nd, nx, ny = crossed if crossed else (d, nx, ny)
            k = self.kind(nd, nx, ny) if nd in self.grid else None
            if k == "L" and self.elevation(nd, nx, ny) == 3:
                # Forced onto elevation-3 ground: the surfer dismounts.
                return list(self.tile_nodes.get((nd, nx, ny), []))
            if k not in ("W", "C", "B"):
                # Stalled against a wall: free to move off sideways.
                out = []
                for ex, ey in self.DIRS:
                    for n in self.tile_nodes.get((d, x + ex, y + ey), []):
                        if self.nodes[n][1] == "W" or self.elevation(d, x + ex, y + ey) == 3:
                            out.append(n)
                return out
            d, x, y = nd, nx, ny
        return []

    def near(self, d: str, x: int, y: int) -> list[int]:
        """Components a player can stand on to use (x, y): the tile itself
        and its neighbours (object tiles are solid, so an NPC is talked to
        from beside it), and across a shop counter."""
        if d not in self.grid:
            return []
        out = list(self.tile_nodes.get((d, x, y), []))
        for dx, dy in ((0, 1), (0, -1), (1, 0), (-1, 0)):
            for c in self.tile_nodes.get((d, x + dx, y + dy), []):
                if c not in out:
                    out.append(c)
            # Clerks are talked to across a counter (MB_COUNTER).
            if self.behavior(d, x + dx, y + dy) == self.counter:
                for c in self.tile_nodes.get((d, x + 2 * dx, y + 2 * dy), []):
                    if c not in out:
                        out.append(c)
        return out

    def approach(self, d: str, x: int, y: int) -> list[tuple[int, tuple]]:
        """Like near(), but a neighbour that is itself a gated tile (an NPC
        standing on a door's approach tile) contributes the components
        around it together with its gate requirement."""
        free = ((),)
        if d not in self.grid:
            return []
        here = self.tile_nodes.get((d, x, y), [])
        if here:
            return [(n, free) for n in here]
        out = []
        for dx, dy in ((0, 1), (0, -1), (1, 0), (-1, 0)):
            nx, ny = x + dx, y + dy
            for c in self.tile_nodes.get((d, nx, ny), []):
                out.append((c, free))
            gate = self.tile_gates.get((d, nx, ny))
            if gate is not None:
                for ex, ey in self.DIRS:
                    if (nx + ex, ny + ey) == (x, y):
                        continue
                    for c in self.tile_nodes.get((d, nx + ex, ny + ey), []):
                        out.append((c, gate))
        return out


def req_and(a: tuple, b: tuple) -> tuple:
    """Conjunction of two alternative-requirement tuples."""
    return tuple(tuple(dict.fromkeys(x + y)) for x in a for y in b)


class Story:
    """Per-milestone closures over Geometry and the STORY_* tables."""

    def __init__(self, geometry: Geometry | None = None):
        self.milestones = campaign_milestones()
        self.geo = geometry or Geometry()
        self.extra_edges: dict[int, list[tuple[int, tuple, str]]] = defaultdict(list)
        for src, dst, req, cite in STORY_EXTRA_EDGES:
            dsts = self.locate(dst)[:1] if isinstance(dst, tuple) else self.locate(dst)
            for a in self.locate(src):
                for b in dsts:
                    self.extra_edges[a].append((b, as_req(req), "script: " + cite))
        self._abnormal_weather_dive_spots()
        self.map_gates = {}
        for d in self.geo.grid:
            for prefix, (req, _cite) in STORY_MAP_GATES.items():
                if d == prefix or (prefix.endswith("_") and d.startswith(prefix)) or d.startswith(prefix + "_"):
                    self.map_gates[d] = as_req(req)
        self.events = [(canon(f), m, xy, as_req(r), c) for f, m, xy, r, c in STORY_EVENTS]
        self.seeds = [n for d, x, y in SEED_TILES for n in self.geo.near(d, x, y)]
        self.windows: list[tuple[str, str | None, int]] = []
        self.closure: dict[str, dict] = {}
        self._cache: dict[tuple, dict] = {}
        for name, flag, cap in self.milestones:
            c = self.close(cap)
            if self.realizes(name, flag, c["flags"]):
                self.windows.append((name, flag, cap))
                self.closure[name] = c

    @staticmethod
    def realizes(name, flag, flags) -> bool:
        if flag is None:
            return True
        if flag not in flags:
            return False
        return not (flag == "FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT" and "FLAG_BADGE06_GET" not in flags)

    def locate(self, where) -> list[int]:
        if isinstance(where, tuple):
            return self.geo.near(*where)
        return list(self.geo.by_map.get(where, []))

    def _abnormal_weather_dive_spots(self) -> None:
        """Marine Cave: while the abnormal weather is on, a dive spot opens on
        the surface above each Underwater_RouteN warp into
        Underwater_MarineCave (src/field_specials.c:4143-4241)."""
        for d in list(self.geo.grid):
            if not d.startswith("Underwater_Route"):
                continue
            surface = d[len("Underwater_"):]
            for warp in self.geo.maps[d].get("warp_events") or []:
                if warp.get("dest_map") != "MAP_UNDERWATER_MARINE_CAVE" or surface not in self.geo.grid:
                    continue
                for a in self.geo.near(surface, warp["x"], warp["y"]):
                    for b in self.geo.near(d, warp["x"], warp["y"])[:1]:
                        self.extra_edges[a].append((b, (("EV_ABNORMAL_WEATHER", "CAN_DIVE"),),
                                                    "script: Marine Cave dive spot (src/field_specials.c:4143-4241)"))

    def _edge_ok(self, a: int, b: int, req: tuple, flags) -> bool:
        if not holds(req, flags):
            return False
        mb = self.geo.nodes[b][0]
        if self.geo.nodes[a][0] != mb:
            gate = self.map_gates.get(mb)
            if gate is not None and not holds(gate, flags):
                return False
        return True

    def _walk(self, flags, reached: dict[int, int], beat: int) -> None:
        queue = deque(reached)
        while queue:
            n = queue.popleft()
            for b, req, _via in self.geo.edges[n] + self.extra_edges.get(n, []):
                if b in reached or not self._edge_ok(n, b, req, flags):
                    continue
                reached[b] = beat
                queue.append(b)

    def close(self, cap: int, forbid: frozenset = frozenset()) -> dict:
        """Closure for one cap: flags fired, nodes reached, and the closure
        round (beat) in which each node/flag first held."""
        key = (cap, forbid)
        if key in self._cache:
            return self._cache[key]
        flags: set[str] = set()
        fbeats: dict[str, int] = {}
        reached: dict[int, int] = {s: 0 for s in self.seeds}
        self._walk(flags, reached, 0)
        beat = 0
        while True:
            beat += 1
            new = []
            for flag, map_, xy, req, _cite in self.events:
                if flag in flags or flag in forbid or not holds(req, flags):
                    continue
                if map_ is not None and not any(n in reached for n in self.locate((map_, *xy) if xy else map_)):
                    continue
                if flag in MILESTONE_FLAGS and cap_of_flags(flags | {flag}, self.milestones) > cap:
                    continue
                new.append(flag)
            if not new:
                break
            for f in new:
                flags.add(f)
                fbeats[f] = beat
            self._walk(flags, reached, beat)
        result = dict(flags=flags, reached=reached, fbeats=fbeats)
        self._cache[key] = result
        return result

    # -- queries ---------------------------------------------------------
    def window_names(self) -> list[str]:
        return [n for n, _f, _c in self.windows]

    def cap(self, window: str) -> int:
        return next(c for n, _f, c in self.windows if n == window)

    def flag_of(self, window: str):
        return next(f for n, f, _c in self.windows if n == window)

    def reachable(self, window: str, where, requires=None, closure: dict | None = None) -> bool:
        c = closure or self.closure[window]
        if not holds(as_req(requires), c["flags"]):
            return False
        if where is None:
            return True
        return any(n in c["reached"] for n in self.locate(where))

    def first_window(self, where, requires=None) -> str | None:
        for name in self.window_names():
            if self.reachable(name, where, requires):
                return name
        return None

    def beat(self, window: str, where, requires=None, closure: dict | None = None) -> int:
        """Closure round in which `where` (and `requires`) first held inside
        the window: a coarse story-order index within the window."""
        c = closure or self.closure[window]
        nodes = self.locate(where) if where is not None else []
        b = min((c["reached"][n] for n in nodes if n in c["reached"]), default=0 if where is None else 10 ** 6)
        best_alt = None
        for alt in as_req(requires):
            if all((badge_count(c["flags"]) >= int(t[8:])) if t.startswith("BADGES>=") else t in c["flags"] for t in alt):
                v = max([0] + [c["fbeats"].get(t, 0) for t in alt if not t.startswith("BADGES>=")])
                best_alt = v if best_alt is None else min(best_alt, v)
        return max(b, best_alt or 0)

    def map_first_windows(self) -> dict[str, str]:
        out = {}
        for name in self.window_names():
            reached = self.closure[name]["reached"]
            for n in reached:
                out.setdefault(self.geo.nodes[n][0], name)
        return out

    def unresolved_gate_tokens(self) -> list[str]:
        """Requirement tokens that no event can ever set (diagnostic)."""
        settable = {canon(f) for f, *_ in STORY_EVENTS}
        tokens = set()
        for reqs in list(self.geo.tile_gates.values()) + list(self.map_gates.values()):
            for alt in reqs:
                tokens.update(t for t in alt if not t.startswith("BADGES>="))
        for _s, _d, req, _c in STORY_EXTRA_EDGES:
            for alt in as_req(req):
                tokens.update(alt)
        for req, _c in STORY_WARP_GATES.values():
            for alt in as_req(req):
                tokens.update(alt)
        return sorted(t for t in tokens if t not in settable and t != "OUT_OF_SCOPE" and not t.startswith("BADGES>="))


_STORY: Story | None = None


def story() -> Story:
    global _STORY
    if _STORY is None:
        _STORY = Story()
    return _STORY


def map_cap_index() -> dict[str, int]:
    """MAP_x -> cap of the first milestone window in which any part of the
    map is reachable (Story model above). Unreachable maps are omitted."""
    s = story()
    return {s.geo.dir_to_id[d]: s.cap(w) for d, w in s.map_first_windows().items() if d in s.geo.dir_to_id}


def explain_map_window(mapdir: str) -> str:
    """First window of a map directory plus the gates that were closed in the
    window before it (for scripts/reference_pool.py --explain)."""
    s = story()
    first = s.map_first_windows().get(mapdir)
    if first is None:
        return f"{mapdir}: never reachable before Buffel under the story model."
    names = s.window_names()
    lines = [f"{mapdir}: first window {first} (cap {s.cap(first)})"]
    idx = names.index(first)
    if idx:
        prev = s.closure[names[idx - 1]]
        now = s.closure[first]
        lines.append(f"  flags new in {first}: " + ", ".join(sorted(now['flags'] - prev['flags'])))
    return "\n".join(lines)


# ---------------------------------------------------------------------------
# Species source helpers (Mega register)
# ---------------------------------------------------------------------------

def species_wild_maps() -> dict[str, set[str]]:
    """SPECIES_x -> set of MAP_x ids where it appears in the wild table."""
    data = json.loads(WILD.read_text())
    result: dict[str, set[str]] = defaultdict(set)
    for group in data["wild_encounter_groups"]:
        if not group.get("for_maps"):
            continue
        for entry in group.get("encounters", []):
            mapid = entry.get("map")
            if not mapid:
                continue
            for field in entry.values():
                if isinstance(field, dict) and "mons" in field:
                    for mon in field["mons"]:
                        result[mon["species"]].add(mapid)
    return result


def species_gift_maps() -> dict[str, set[str]]:
    """SPECIES_x -> set of MAP_x ids where a givemon/giveegg script grants it."""
    dir_to_id = map_dir_to_id()
    result: dict[str, set[str]] = defaultdict(set)
    for path in sorted(MAPS_DIR.glob("*/scripts.inc")):
        mapid = dir_to_id.get(path.parent.name)
        if not mapid:
            continue
        for species in re.findall(r"\b(?:givemon|giveegg)\s+(SPECIES_\w+)", path.read_text()):
            result[species].add(mapid)
    return result


def legendary_gates() -> dict[str, dict]:
    """SPECIES_x -> gate row from src/data/pokemon/legendary_signs.h
    (GATE(mon, badges, flag, reqMon, kind) / VISITOR(mon, badges, flag, id,
    map, habitat, weather); struct at include/legendary_signs.h:25-39)."""
    text = LEGENDARY_SIGNS.read_text()
    rows: dict[str, dict] = {}
    for n, line in enumerate(text.splitlines(), 1):
        m = re.match(r"GATE\((\w+),\s*(\d+),\s*(\w+),\s*(\w+),\s*(\w+)\)", line.strip())
        if m:
            rows["SPECIES_" + m[1]] = dict(badges=int(m[2]), flag=None if m[3] == "0" else m[3],
                                           req_species=None if m[4] == "NONE" else "SPECIES_" + m[4],
                                           kind=m[5], visitor=None, line=n)
            continue
        m = re.match(r"VISITOR\((\w+),\s*(\d+),\s*(\w+),\s*(\d+),\s*(\w+),\s*(\w+),\s*(\w+)\)", line.strip())
        if m:
            rows["SPECIES_" + m[1]] = dict(badges=int(m[2]), flag=None if m[3] == "0" else m[3], req_species=None,
                                           kind="WILD", visitor=dict(id=int(m[4]), map="MAP_" + m[5],
                                                                     habitat=m[6], weather=m[7]), line=n)
    return rows


def species_legendary_sign_maps() -> dict[str, set[str]]:
    """SPECIES_x -> MAP_x of each storm visitor's home map (GATE rows carry
    no map; their residents are ordinary wild slots)."""
    result: dict[str, set[str]] = defaultdict(set)
    for species, row in legendary_gates().items():
        if row["visitor"]:
            result[species].add(row["visitor"]["map"])
    return result


def review_index_order() -> dict[str, int]:
    """TRAINER_x -> review_index, from data/emerald_champions/trainer-review-index.json."""
    if not REVIEW_INDEX.exists():
        return {}
    data = json.loads(REVIEW_INDEX.read_text())
    order: dict[str, int] = {}
    for encounter in data["encounters"]:
        for trainer_id in encounter["trainer_ids"]:
            if encounter.get("review_index") is not None:
                order[trainer_id] = encounter["review_index"]
    return order


def build_register() -> list[dict]:
    all_stones = stones()
    forms = item_forms()
    rewards = world_reward_sources()
    branches = teams.read_teams()
    order = review_index_order()
    adjacency = evolution_edges()
    cap_by_map = map_cap_index()
    wild_maps = species_wild_maps()
    gift_maps = species_gift_maps()
    sign_maps = species_legendary_sign_maps()

    rows = []
    for item in all_stones:
        holders = []
        for branch in branches:
            for index, mon in enumerate(branch.mons):
                if "ITEM_" + mon.item != item:
                    continue
                can_evolve = bool(branch.mega_slots is not None and (branch.mega_slots >> index) & 1)
                holders.append(dict(
                    trainer=branch.trainer, encounter=f"E{branch.encounter:04d}",
                    cls=branch.cls, can_evolve=can_evolve,
                    review_index=order.get(branch.trainer),
                ))
        if holders:
            def sort_key(h):
                tier = TIER.get(h["cls"], 0)
                access = h["review_index"] if h["review_index"] is not None else 10 ** 9
                return (-tier, access)
            holders.sort(key=sort_key)
            signature = holders[0]
            secondary = holders[1:]
        else:
            signature = None
            secondary = []

        species_forms = forms.get(item, [])
        family_caps = []
        unmapped_species = []
        for form_species in species_forms:
            root = base_species(form_species)
            family = family_of(root, adjacency)
            maps = set()
            for sp in family:
                maps |= wild_maps.get(sp, set())
                maps |= gift_maps.get(sp, set())
                maps |= sign_maps.get(sp, set())
            caps = [cap_by_map[m] for m in maps if m in cap_by_map]
            if caps:
                family_caps.append(min(caps))
            else:
                unmapped_species.append(root)

        rows.append(dict(
            stone=item,
            forms=species_forms,
            world_source=rewards.get(item, []),
            holders=holders,
            signature_owner=signature,
            secondary_owners=secondary,
            player_family_access=(min(family_caps) if family_caps else None),
            player_family_access_unmapped=sorted(set(unmapped_species)) if not family_caps else [],
            no_trainer_holder=not holders,
        ))
    return rows


def special_form_rows() -> list[dict]:
    branches = teams.read_teams()
    moves = move_triggered_forms()
    rows = []
    for species, move in moves.items():
        root = base_species(species)
        owners = []
        for branch in branches:
            for mon in branch.mons:
                if mon.species != root[len("SPECIES_"):]:
                    continue
                has_move = move[len("MOVE_"):] in mon.moves
                owners.append(dict(trainer=branch.trainer, cls=branch.cls, encounter=f"E{branch.encounter:04d}",
                                   has_trigger_move=has_move))
        rows.append(dict(kind="move-triggered", species=species, trigger=move,
                         note=SPECIAL_FORM_NOTES.get(species, ""), owners=owners))
    for species, note in SPECIAL_FORM_NOTES.items():
        if species in moves:
            continue
        rows.append(dict(kind="shared-stone-form", species=species, trigger=None, note=note, owners=[]))
    return rows


def render_lines() -> tuple[list[str], list[dict], list[dict]]:
    rows = build_register()
    specials = special_form_rows()
    no_holder = [r["stone"] for r in rows if r["no_trainer_holder"]]
    lines = [
        "\nMEGA REGISTER",
        f"{len(rows)} item-based Mega Stones. Signature owner = highest design tier "
        "(elite/leader>gym>admin/boss>rival>ace>brain>regular/casual/grunt), ties broken "
        "by earlier data/emerald_champions/trainer-review-index.json review_index. mega_slots=yes means the "
        "holder's authored Mega permission bitmask covers that party position. player_family_access is "
        "the cap of the first story milestone (Story model) in which a map with the base family in its "
        "wild table, a givemon/giveegg grant or a storm-visitor home is reachable; scripts/reference_pool.py "
        "is the full availability model.",
        f"NO TRAINER HOLDER rows: {len(no_holder)}" + (f" -- {', '.join(no_holder)}" if no_holder else " (none)"),
    ]
    for row in rows:
        forms_str = ", ".join(pretty(f) for f in row["forms"]) or "(no form found in form_change_tables.h)"
        source_str = "; ".join(row["world_source"]) or "NO WORLD SOURCE FOUND"
        lines.append(f"\n{pretty(row['stone'])} ({row['stone']}) -> {forms_str}")
        lines.append(f"  World source: {source_str}")
        if row["no_trainer_holder"]:
            lines.append("  NO TRAINER HOLDER")
        else:
            sig = row["signature_owner"]
            lines.append(f"  Signature owner: {sig['trainer']} ({sig['encounter']}, class={sig['cls']}, "
                         f"mega_slots evolves this slot={'yes' if sig['can_evolve'] else 'no'})")
            if row["secondary_owners"]:
                lines.append("  Secondary owners: " + "; ".join(
                    f"{h['trainer']} ({h['encounter']}, class={h['cls']}, "
                    f"mega_slots evolves this slot={'yes' if h['can_evolve'] else 'no'})"
                    for h in row["secondary_owners"]))
        if row["player_family_access"] is not None:
            lines.append(f"  Player family access: cap {row['player_family_access']}")
        else:
            lines.append("  Player family access: unmapped" + (
                f" ({', '.join(pretty(s) for s in row['player_family_access_unmapped'])})"
                if row["player_family_access_unmapped"] else ""))
    lines.append("\nSPECIAL FORMS")
    for row in specials:
        if row["kind"] == "move-triggered":
            owners = row["owners"]
            if owners:
                status = "; ".join(f"{o['trainer']} ({o['encounter']}, class={o['cls']}, "
                                   f"has {pretty(row['trigger'])}={'yes' if o['has_trigger_move'] else 'no'})"
                                   for o in owners)
            else:
                status = "NO TRAINER HOLDER of the base species"
            lines.append(f"{pretty(row['species'])} -- move-triggered by {pretty(row['trigger'])}: {status}")
        else:
            lines.append(f"{pretty(row['species'])} -- {row['note']}")
    return lines, rows, specials


def generate(root: Path = ROOT) -> tuple[list[str], dict, set[Path]]:
    lines, rows, specials = render_lines()
    catalog = dict(mega_register=rows, special_forms=specials)
    paths = {
        Path(__file__), FORM_TABLES, MASTER, REVIEW_INDEX, WILD, LEGENDARY_SIGNS,
        root / "src/data/emerald_champions_mega_stones.h",
        teams.TEAMS,
    }
    return lines, catalog, paths


def main() -> None:
    lines, catalog, _paths = generate()
    print("\n".join(lines))
    out = ROOT / "work/mega-register.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(catalog, indent=2) + "\n")
    no_holder = [r["stone"] for r in catalog["mega_register"] if r["no_trainer_holder"]]
    print(f"\n{'PASS' if not no_holder else 'FAIL'}: {len(catalog['mega_register'])} stones; "
          f"{len(no_holder)} with NO TRAINER HOLDER")


if __name__ == "__main__":
    main()
