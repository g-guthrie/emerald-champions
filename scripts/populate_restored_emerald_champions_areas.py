#!/usr/bin/env python3
"""Populate restored side areas with progression, story, and encounters."""

from __future__ import annotations

import json
from pathlib import Path

from restore_poke_vial_quest import check as check_poke_vial_quest
from restore_poke_vial_quest import write as restore_poke_vial_quest


ROOT = Path(__file__).resolve().parents[1]
OLD = Path("/private/tmp/emerald-battle-set.O3NA1S/repo")


def load(path: Path):
    return json.loads(path.read_text())


def save(path: Path, payload) -> None:
    path.write_text(json.dumps(payload, indent=2) + "\n")


def obj(gfx, x, y, script, flag="0", local_id=None, movement="MOVEMENT_TYPE_FACE_DOWN"):
    row = {
        "graphics_id": gfx,
        "x": x,
        "y": y,
        "elevation": 3,
        "movement_type": movement,
        "movement_range_x": 0,
        "movement_range_y": 0,
        "trainer_type": "TRAINER_TYPE_NONE",
        "trainer_sight_or_berry_tree_id": "0",
        "script": script,
        "flag": flag,
    }
    if local_id:
        row["local_id"] = local_id
    return row


def item(x, y, item_id, flag, gfx=None):
    if gfx is None:
        if item_id.endswith("ITE") or item_id.endswith("ITE_Y"):
            gfx = "OBJ_EVENT_GFX_ITEM_BALL"
        elif "FOSSIL" in item_id or item_id == "ITEM_OLD_AMBER":
            gfx = "OBJ_EVENT_GFX_FOSSIL"
        elif item_id == "ITEM_RARE_CANDY":
            gfx = "OBJ_EVENT_GFX_ITEM_BALL"
        else:
            gfx = "OBJ_EVENT_GFX_ITEM_BALL"
    row = obj(gfx, x, y, "Common_EventScript_FindItem", flag)
    row["trainer_sight_or_berry_tree_id"] = item_id
    return row


def old_obstacles(map_name):
    payload = load(OLD / "data" / "maps" / map_name / "map.json")
    allowed = {"EventScript_RockSmash", "EventScript_CutTree", "EventScript_StrengthBoulder"}
    return [event for event in payload.get("object_events", []) if event.get("script") in allowed]


def base_script(map_name):
    return f"{map_name}_MapScripts::\n\t.byte 0\n\n"


def preserve_poke_vial_script(path: Path) -> str:
    """Keep the authored Ashen Woods chase while regenerating map content."""
    text = path.read_text()
    begin = "@ BEGIN EMERALD CHAMPIONS POKE VIAL QUEST"
    end = "@ END EMERALD CHAMPIONS POKE VIAL QUEST"
    if text.count(begin) != 1 or text.count(end) != 1:
        raise SystemExit("AshenWoods: missing unique Poke Vial preservation markers")
    return text[text.index(begin):text.index(end) + len(end)] + "\n\n"


def static_legend_script(map_name, local_id, species, flag, offset, title):
    return f"""{map_name}_EventScript_{title}::
\tlock
\tfaceplayer
\twaitse
\tplaymoncry SPECIES_{species}, CRY_MODE_ENCOUNTER
\tdelay 40
\twaitmoncry
\tsetvar VAR_0x8004, SPECIES_{species}
\tsetvar VAR_0x8005, {offset}
\tspecial CreateEmeraldChampionsStaticLegendaryEncounter
\tsetflag FLAG_SYS_CTRL_OBJ_DELETE
\tspecial BattleSetup_StartLegendaryBattle
\tclearflag FLAG_SYS_CTRL_OBJ_DELETE
\tspecialvar VAR_RESULT, GetBattleOutcome
\tgoto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, {map_name}_EventScript_{title}Caught
\tmsgbox {map_name}_Text_{title}Remains, MSGBOX_DEFAULT
\trelease
\tend

{map_name}_EventScript_{title}Caught::
\tsetflag {flag}
\tremoveobject {local_id}
\trelease
\tend

{map_name}_Text_{title}Remains:
\t.string "{species.replace('_', ' ')} accepts the challenge, but the\\n"
\t.string "sanctuary will hold its place for you.$"

"""


def visible_sign_script(map_name, local_id, species, sign_id, flag, required_name, place_name):
    title = species.title().replace("_", "")
    return f""".set EC_SIGN_{species}_ID, {sign_id}

{map_name}_EventScript_{title}::
\tlock
\tfaceplayer
\tsetvar VAR_0x8004, EC_SIGN_{species}_ID
\tspecial TryUnlockSelectedLegendarySign
\tgoto_if_eq VAR_RESULT, 0, {map_name}_EventScript_{title}Dormant
\tgoto_if_eq VAR_RESULT, 1, {map_name}_EventScript_{title}NeedsPartner
\tgoto_if_eq VAR_RESULT, 4, {map_name}_EventScript_{title}Cleanup
\tmsgbox {map_name}_Text_{title}Awakens, MSGBOX_DEFAULT
\twaitse
\tplaymoncry SPECIES_{species}, CRY_MODE_ENCOUNTER
\tdelay 40
\twaitmoncry
\tsetvar VAR_0x8004, EC_SIGN_{species}_ID
\tspecial CreateSelectedLegendarySignEncounter
\tsetflag FLAG_SYS_CTRL_OBJ_DELETE
\tspecial BattleSetup_StartLegendaryBattle
\tclearflag FLAG_SYS_CTRL_OBJ_DELETE
\tspecialvar VAR_RESULT, GetBattleOutcome
\tgoto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, {map_name}_EventScript_{title}Caught
\tmsgbox {map_name}_Text_{title}Remains, MSGBOX_DEFAULT
\trelease
\tend

{map_name}_EventScript_{title}Dormant::
\tmsgbox {map_name}_Text_{title}Dormant, MSGBOX_DEFAULT
\trelease
\tend

{map_name}_EventScript_{title}NeedsPartner::
\tmsgbox {map_name}_Text_{title}NeedsPartner, MSGBOX_DEFAULT
\trelease
\tend

{map_name}_EventScript_{title}Caught::
\tsetflag {flag}
{map_name}_EventScript_{title}Cleanup::
\tsetflag {flag}
\tremoveobject {local_id}
\trelease
\tend

{map_name}_Text_{title}Dormant:
\t.string "A CHAMPION'S SIGN is carved here.\\p"
\t.string "Its light is dormant. Hoenn's story\\n"
\t.string "has not yet reached this place.$"

{map_name}_Text_{title}NeedsPartner:
\t.string "The SIGN sketches {required_name}.\\p"
\t.string "Bring that Pokémon's family here, and\\n"
\t.string "the hidden challenger may answer.$"

{map_name}_Text_{title}Awakens:
\t.string "The CHAMPION'S SIGN erupts with light!\\n"
\t.string "{species.replace('_', ' ')} answers the challenge!$"

{map_name}_Text_{title}Remains:
\t.string "The SIGN remains bright. {species.replace('_', ' ')}\\n"
\t.string "will accept another challenge.$"

"""


ITEMS = {
    "AlteringCave_1F": [
        item(33, 12, "ITEM_PRISON_BOTTLE", "FLAG_EC_ITEM_PRISON_BOTTLE"),
        item(29, 16, "ITEM_MASTER_BALL", "FLAG_EC_ITEM_MASTER_BALL"),
        item(9, 18, "ITEM_BEAST_BALL", "FLAG_EC_ITEM_ALTERING_BEAST_BALL"),
    ],
    "AshenWoods": [
        item(10, 5, "ITEM_PINSIRITE", "FLAG_EC_ITEM_ASHEN_PINSIRITE"),
        item(26, 5, "ITEM_DUSK_STONE", "FLAG_EC_ITEM_ASHEN_DUSK_STONE"),
        item(26, 43, "ITEM_RARE_CANDY", "FLAG_EC_ITEM_ASHEN_RARE_CANDY"),
    ],
    "DewfordManor_1F": [
        item(1, 11, "ITEM_SABLENITE", "FLAG_EC_ITEM_MANOR_SABLENITE"),
        item(13, 5, "ITEM_REAPER_CLOTH", "FLAG_EC_ITEM_MANOR_REAPER_CLOTH"),
    ],
    "DewfordMeadow": [
        item(21, 14, "ITEM_MAWILITE", "FLAG_EC_ITEM_MEADOW_MAWILITE"),
        item(27, 1, "ITEM_SHINY_STONE", "FLAG_EC_ITEM_MEADOW_SHINY_STONE"),
        item(4, 13, "ITEM_RARE_CANDY", "FLAG_EC_ITEM_MEADOW_RARE_CANDY"),
    ],
    "EmberPath": [
        item(36, 2, "ITEM_BLAZIKENITE", "FLAG_EC_ITEM_EMBER_BLAZIKENITE"),
        item(12, 10, "ITEM_MAGMARIZER", "FLAG_EC_ITEM_EMBER_MAGMARIZER"),
        item(25, 35, "ITEM_RARE_CANDY", "FLAG_EC_ITEM_EMBER_RARE_CANDY"),
    ],
    "PetalburgWoods_2": [
        item(36, 5, "ITEM_SUN_STONE", "FLAG_EC_ITEM_WOODS2_SUN_STONE"),
    ],
    "PetalburgWoods_3": [
        item(38, 9, "ITEM_BEEDRILLITE", "FLAG_EC_ITEM_WOODS3_BEEDRILLITE"),
        item(22, 26, "ITEM_SWEET_APPLE", "FLAG_EC_ITEM_WOODS3_SWEET_APPLE"),
        item(22, 16, "ITEM_TART_APPLE", "FLAG_EC_ITEM_WOODS3_TART_APPLE"),
    ],
    "Route111_RuinsExterior": [
        item(10, 22, "ITEM_STEELIXITE", "FLAG_EC_ITEM_RUINS_STEELIXITE"),
    ],
    "SandstrewnRuins": [
        item(5, 56, "ITEM_GARCHOMPITE", "FLAG_EC_ITEM_RUINS_GARCHOMPITE"),
        item(14, 18, "ITEM_SAIL_FOSSIL", "FLAG_EC_ITEM_RUINS_SAIL_FOSSIL"),
        item(2, 29, "ITEM_BLACK_AUGURITE", "FLAG_EC_ITEM_RUINS_BLACK_AUGURITE"),
        item(14, 43, "ITEM_ARMOR_FOSSIL", "FLAG_EC_ITEM_RUINS_ARMOR_FOSSIL"),
        item(3, 58, "ITEM_PLUME_FOSSIL", "FLAG_EC_ITEM_RUINS_PLUME_FOSSIL"),
        item(10, 71, "ITEM_SKULL_FOSSIL", "FLAG_EC_ITEM_RUINS_SKULL_FOSSIL"),
        item(2, 85, "ITEM_COVER_FOSSIL", "FLAG_EC_ITEM_RUINS_COVER_FOSSIL"),
        item(7, 102, "ITEM_HELIX_FOSSIL", "FLAG_EC_ITEM_RUINS_HELIX_FOSSIL"),
        item(3, 118, "ITEM_DOME_FOSSIL", "FLAG_EC_ITEM_RUINS_DOME_FOSSIL"),
        item(4, 2, "ITEM_JAW_FOSSIL", "FLAG_EC_ITEM_RUINS_JAW_FOSSIL"),
        item(3, 14, "ITEM_ODD_KEYSTONE", "FLAG_EC_ITEM_RUINS_ODD_KEYSTONE"),
        item(12, 113, "ITEM_RARE_CANDY", "FLAG_EC_ITEM_RUINS_RARE_CANDY"),
    ],
    "ScorchedSlab_B2F": [
        item(23, 22, "ITEM_CHARIZARDITE_X", "FLAG_EC_ITEM_SCORCHED_CHARIZARDITE_X"),
    ],
    "Seaspray_Cave": [
        item(6, 25, "ITEM_BLASTOISINITE", "FLAG_EC_ITEM_SEASPRAY_BLASTOISINITE"),
        item(5, 5, "ITEM_DAWN_STONE", "FLAG_EC_ITEM_SEASPRAY_DAWN_STONE"),
        item(10, 24, "ITEM_LURE_BALL", "FLAG_EC_ITEM_SEASPRAY_LURE_BALL"),
        item(46, 18, "ITEM_RARE_CANDY", "FLAG_EC_ITEM_SEASPRAY_RARE_CANDY"),
    ],
    "Seaspray_Cave_B1F": [
        item(46, 15, "ITEM_KINGS_ROCK", "FLAG_EC_ITEM_SEASPRAY_KINGS_ROCK"),
        item(11, 12, "ITEM_SLOWBRONITE", "FLAG_EC_ITEM_SEASPRAY_SLOWBRONITE"),
        item(20, 24, "ITEM_ICE_STONE", "FLAG_EC_ITEM_SEASPRAY_ICE_STONE"),
    ],
    "VerdanturfMeadow": [
        item(7, 10, "ITEM_RARE_CANDY", "FLAG_EC_ITEM_VERDANTURF_RARE_CANDY"),
    ],
}


EXTRA_OBJECTS = {
    "AlteringCave_1F": [
        obj("OBJ_EVENT_GFX_SPECIES(HOOPA)", 6, 8, "AlteringCave_1F_EventScript_Hoopa", "FLAG_EC_CAUGHT_HOOPA", "LOCALID_EC_HOOPA"),
    ],
    "AlteringCave_B1F": [
        obj("OBJ_EVENT_GFX_SPECIES(MEWTWO)", 7, 13, "AlteringCave_B1F_EventScript_Mewtwo", "FLAG_EC_CAUGHT_MEWTWO", "LOCALID_EC_MEWTWO"),
        obj("OBJ_EVENT_GFX_LEAF", 21, 16, "AlteringCave_B1F_EventScript_Leaf", "0", "LOCALID_EC_LEAF"),
    ],
    "AshenWoods": [
        obj("OBJ_EVENT_GFX_HIKER", 10, 13, "AshenWoods_EventScript_Roman"),
        obj("OBJ_EVENT_GFX_PICNICKER", 12, 25, "AshenWoods_EventScript_Alannah"),
        obj("OBJ_EVENT_GFX_CAMPER", 12, 22, "AshenWoods_EventScript_Martin"),
        obj("OBJ_EVENT_GFX_MANIAC", 6, 33, "AshenWoods_EventScript_Elmer"),
        obj("OBJ_EVENT_GFX_COOK", 19, 39, "AshenWoods_EventScript_Caretaker"),
        obj("OBJ_EVENT_GFX_SPECIES(OKIDOGI)", 14, 29, "AshenWoods_EventScript_Okidogi", "FLAG_EC_CAUGHT_OKIDOGI", "LOCALID_EC_OKIDOGI"),
    ],
    "CaveOfOrigin_DianciesRoom": [
        obj("OBJ_EVENT_GFX_SPECIES(DIANCIE)", 9, 9, "CaveOfOrigin_DianciesRoom_EventScript_Diancie", "FLAG_EC_CAUGHT_DIANCIE", "LOCALID_EC_DIANCIE"),
        obj("OBJ_EVENT_GFX_SPECIES(TERAPAGOS)", 7, 9, "CaveOfOrigin_DianciesRoom_EventScript_Terapagos", "FLAG_EC_CAUGHT_TERAPAGOS", "LOCALID_EC_TERAPAGOS"),
        obj("OBJ_EVENT_GFX_WALLACE", 13, 9, "CaveOfOrigin_DianciesRoom_EventScript_WallaceExhibition", "0", "LOCALID_EC_WALLACE"),
    ],
    "DewfordManor_1F": [
        obj("OBJ_EVENT_GFX_OLD_MAN", 15, 8, "DewfordManor_1F_EventScript_Historian"),
        obj("OBJ_EVENT_GFX_SPECIES(MELOETTA)", 8, 8, "DewfordManor_1F_EventScript_Meloetta", "FLAG_EC_CAUGHT_MELOETTA", "LOCALID_EC_MELOETTA"),
        obj("OBJ_EVENT_GFX_SPECIES(MUNKIDORI)", 13, 8, "DewfordManor_1F_EventScript_Munkidori", "FLAG_EC_CAUGHT_MUNKIDORI", "LOCALID_EC_MUNKIDORI"),
    ],
    "DewfordMeadow": [obj("OBJ_EVENT_GFX_GIRL_2", 12, 9, "DewfordMeadow_EventScript_Warden")],
    "EmberPath": [
        obj("OBJ_EVENT_GFX_SPECIES(MOLTRES)", 21, 14, "EmberPath_EventScript_Moltres", "FLAG_EC_CAUGHT_MOLTRES", "LOCALID_EC_MOLTRES"),
        obj("OBJ_EVENT_GFX_HIKER", 8, 36, "EmberPath_EventScript_Warden"),
    ],
    "MeteorFalls_JirachisRoom": [
        obj("OBJ_EVENT_GFX_SPECIES(JIRACHI)", 7, 6, "MeteorFalls_JirachisRoom_EventScript_Jirachi", "FLAG_EC_CAUGHT_JIRACHI", "LOCALID_EC_JIRACHI"),
        obj("OBJ_EVENT_GFX_SPECIES(COSMOG)", 4, 6, "MeteorFalls_JirachisRoom_EventScript_Cosmog", "FLAG_EC_CAUGHT_COSMOG", "LOCALID_EC_COSMOG"),
    ],
    "MirageTower_B1F": [obj("OBJ_EVENT_GFX_OLD_MAN", 4, 7, "MirageTower_B1F_EventScript_Inscription")],
    "PetalburgWoods_2": [
        obj("OBJ_EVENT_GFX_SPECIES(VIRIZION)", 42, 6, "PetalburgWoods_2_EventScript_Virizion", "FLAG_EC_CAUGHT_VIRIZION", "LOCALID_EC_VIRIZION"),
        obj("OBJ_EVENT_GFX_SPECIES(WO_CHIEN)", 5, 5, "PetalburgWoods_2_EventScript_WoChien", "FLAG_EC_CAUGHT_WO_CHIEN", "LOCALID_EC_WO_CHIEN"),
        obj("OBJ_EVENT_GFX_BUG_CATCHER", 18, 31, "PetalburgWoods_2_EventScript_Ranger"),
    ],
    "PetalburgWoods_3": [
        obj("OBJ_EVENT_GFX_SPECIES(CELEBI)", 44, 41, "PetalburgWoods_3_EventScript_Celebi", "FLAG_EC_CAUGHT_CELEBI", "LOCALID_EC_CELEBI"),
        obj("OBJ_EVENT_GFX_CAMPER", 8, 32, "PetalburgWoods_3_EventScript_Ranger"),
    ],
    "Route111_RuinsExterior": [
        obj("OBJ_EVENT_GFX_SPECIES(LANDORUS)", 9, 10, "Route111_RuinsExterior_EventScript_Landorus", "FLAG_EC_CAUGHT_LANDORUS", "LOCALID_EC_LANDORUS"),
        obj("OBJ_EVENT_GFX_MANIAC", 8, 20, "Route111_RuinsExterior_EventScript_Archaeologist"),
    ],
    "SandstrewnRuins": [obj("OBJ_EVENT_GFX_MANIAC", 8, 128, "SandstrewnRuins_EventScript_Archaeologist")],
    "SandstrewnRuins_B1F": [obj("OBJ_EVENT_GFX_SPECIES(ZYGARDE)", 34, 6, "SandstrewnRuins_B1F_EventScript_Zygarde", "FLAG_EC_CAUGHT_ZYGARDE", "LOCALID_EC_ZYGARDE")],
    "ScorchedSlab_B2F": [
        obj("OBJ_EVENT_GFX_SPECIES(RESHIRAM)", 8, 5, "ScorchedSlab_B2F_EventScript_Reshiram", "FLAG_EC_CAUGHT_RESHIRAM", "LOCALID_EC_RESHIRAM"),
        obj("OBJ_EVENT_GFX_HIKER", 12, 28, "ScorchedSlab_B2F_EventScript_Warden"),
    ],
    "ScorchedSlab_HeatransRoom": [obj("OBJ_EVENT_GFX_SPECIES(HEATRAN)", 10, 12, "ScorchedSlab_HeatransRoom_EventScript_Heatran", "FLAG_EC_CAUGHT_HEATRAN", "LOCALID_EC_HEATRAN")],
    "Seaspray_Cave": [obj("OBJ_EVENT_GFX_SAILOR", 8, 32, "Seaspray_Cave_EventScript_Explorer")],
    "Seaspray_Cave_B1F": [obj("OBJ_EVENT_GFX_SPECIES(PALKIA)", 40, 7, "Seaspray_Cave_B1F_EventScript_Palkia", "FLAG_EC_CAUGHT_PALKIA", "LOCALID_EC_PALKIA")],
    "VerdanturfMeadow": [
        obj("OBJ_EVENT_GFX_SPECIES(SHAYMIN)", 10, 14, "VerdanturfMeadow_EventScript_Shaymin", "FLAG_EC_CAUGHT_SHAYMIN", "LOCALID_EC_SHAYMIN"),
        obj("OBJ_EVENT_GFX_SPECIES(ENAMORUS)", 4, 12, "VerdanturfMeadow_EventScript_Enamorus", "FLAG_EC_CAUGHT_ENAMORUS", "LOCALID_EC_ENAMORUS"),
        obj("OBJ_EVENT_GFX_SPECIES(FEZANDIPITI)", 13, 12, "VerdanturfMeadow_EventScript_Fezandipiti", "FLAG_EC_CAUGHT_FEZANDIPITI", "LOCALID_EC_FEZANDIPITI"),
        obj("OBJ_EVENT_GFX_GIRL_2", 12, 9, "VerdanturfMeadow_EventScript_Warden"),
    ],
}


SCRIPTS = {
    "AlteringCave_1F": """AlteringCave_1F_EventScript_Researcher::
\tmsgbox AlteringCave_1F_Text_Researcher, MSGBOX_NPC
\tend

AlteringCave_1F_Text_Researcher:
\t.string "DEVON called this cave an error in\\n"
\t.string "nature. I think it is a doorway.\\p"
\t.string "The same CHAMPION'S SIGN appears in\\n"
\t.string "every shape this cavern assumes.$"
""",
    "AlteringCave_B1F": """AlteringCave_B1F_EventScript_Leaf::
\ttrainerbattle_double TRAINER_LEAF_ALTERING_CAVE, AlteringCave_B1F_Text_LeafIntro, AlteringCave_B1F_Text_LeafDefeat, EmeraldChampions_Text_NeedTwoPokemon
\tcheckitem ITEM_CATCHING_CHARM, 1
\tgoto_if_eq VAR_RESULT, TRUE, AlteringCave_B1F_EventScript_LeafAfter
\tgiveitem ITEM_CATCHING_CHARM
\tgoto_if_eq VAR_RESULT, FALSE, AlteringCave_B1F_EventScript_LeafBagFull
AlteringCave_B1F_EventScript_LeafAfter::
\tmsgbox AlteringCave_B1F_Text_LeafAfter, MSGBOX_AUTOCLOSE
\tend

AlteringCave_B1F_EventScript_LeafBagFull::
\tmsgbox AlteringCave_B1F_Text_LeafBagFull, MSGBOX_DEFAULT
\trelease
\tend

AlteringCave_B1F_Text_LeafIntro:
\t.string "I followed the distortions here from\\n"
\t.string "Kanto. They react to strong teams.\\p"
\t.string "Show me whether Hoenn chose well.$"

AlteringCave_B1F_Text_LeafDefeat:
\t.string "It did. That was a Champion's answer.$"

AlteringCave_B1F_Text_LeafAfter:
\t.string "Take this CATCHING CHARM. It rewards\\n"
\t.string "the patience these distortions demand.$"
AlteringCave_B1F_Text_LeafBagFull:
\t.string "Make room for the CATCHING CHARM.\\n"
\t.string "Your reward will wait here.$"
""",
    "AshenWoods": """AshenWoods_EventScript_Alannah::
\ttrainerbattle_double TRAINER_ALANNAH, AshenWoods_Text_AlannahIntro, AshenWoods_Text_AlannahDefeat, EmeraldChampions_Text_NeedTwoPokemon
\tmsgbox AshenWoods_Text_AlannahAfter, MSGBOX_AUTOCLOSE
\tend
AshenWoods_EventScript_Martin::
\ttrainerbattle_double TRAINER_MARTIN, AshenWoods_Text_MartinIntro, AshenWoods_Text_MartinDefeat, EmeraldChampions_Text_NeedTwoPokemon
\tmsgbox AshenWoods_Text_MartinAfter, MSGBOX_AUTOCLOSE
\tend
AshenWoods_EventScript_Roman::
\ttrainerbattle_double TRAINER_ROMAN, AshenWoods_Text_RomanIntro, AshenWoods_Text_RomanDefeat, EmeraldChampions_Text_NeedTwoPokemon
\tmsgbox AshenWoods_Text_RomanAfter, MSGBOX_AUTOCLOSE
\tend
AshenWoods_EventScript_Elmer::
\ttrainerbattle_double TRAINER_ELMER, AshenWoods_Text_ElmerIntro, AshenWoods_Text_ElmerDefeat, EmeraldChampions_Text_NeedTwoPokemon
\tmsgbox AshenWoods_Text_ElmerAfter, MSGBOX_AUTOCLOSE
\tend
AshenWoods_EventScript_Caretaker::
\tmsgbox AshenWoods_Text_Caretaker, MSGBOX_NPC
\tend

AshenWoods_Text_AlannahIntro:
\t.string "The ash feeds roots tougher than stone.\\n"
\t.string "Can your team outlast mine?$"
AshenWoods_Text_AlannahDefeat:
\t.string "Your answer grew through the ash.$"
AshenWoods_Text_AlannahAfter:
\t.string "The forest rewards adaptation, not one\\n"
\t.string "perfect type chart.$"
AshenWoods_Text_MartinIntro:
\t.string "A firebird circles whenever the sun\\n"
\t.string "breaks through. Face the whole blaze!$"
AshenWoods_Text_MartinDefeat:
\t.string "You extinguished the wildfire cleanly.$"
AshenWoods_Text_MartinAfter:
\t.string "MOLTRES waits beyond EMBER PATH.\\n"
\t.string "Strength alone will not reach it.$"
AshenWoods_Text_RomanIntro:
\t.string "One drop of water can wake a mountain.\\n"
\t.string "Interrupt the engine if you can!$"
AshenWoods_Text_RomanDefeat:
\t.string "You stopped the engine before it ran.$"
AshenWoods_Text_RomanAfter:
\t.string "The best doubles plans are visible.\\n"
\t.string "The hard part is answering in time.$"
AshenWoods_Text_ElmerIntro:
\t.string "Every insect here wins differently.\\n"
\t.string "Show me four different answers!$"
AshenWoods_Text_ElmerDefeat:
\t.string "You changed pace with every swarm.$"
AshenWoods_Text_ElmerAfter:
\t.string "A Champion needs a toolbox, not a\\n"
\t.string "single unbeatable hammer.$"
AshenWoods_Text_Caretaker:
\t.string "GROUDON's heat burned this place.\\n"
\t.string "Rain returned it to life.\\p"
\t.string "The old wardens say RAYQUAZA is not\\n"
\t.string "a conqueror, but Hoenn's balance.$"
""",
    "CaveOfOrigin_DianciesRoom": """CaveOfOrigin_DianciesRoom_EventScript_WallaceExhibition::
\tlock
\tfaceplayer
\tgoto_if_unset FLAG_SYS_GAME_CLEAR, CaveOfOrigin_DianciesRoom_EventScript_WallaceNotReady
\tgoto_if_defeated TRAINER_WALLACE_DOUBLES_LEGENDS, CaveOfOrigin_DianciesRoom_EventScript_WallaceReward
\ttrainerbattle_double TRAINER_WALLACE_DOUBLES_LEGENDS, CaveOfOrigin_DianciesRoom_Text_WallaceIntro, CaveOfOrigin_DianciesRoom_Text_WallaceDefeat, EmeraldChampions_Text_NeedTwoPokemon

CaveOfOrigin_DianciesRoom_EventScript_WallaceReward::
\tcheckitem ITEM_OVAL_CHARM, 1
\tgoto_if_eq VAR_RESULT, TRUE, CaveOfOrigin_DianciesRoom_EventScript_WallaceAfter
\tgiveitem ITEM_OVAL_CHARM
\tgoto_if_eq VAR_RESULT, FALSE, CaveOfOrigin_DianciesRoom_EventScript_WallaceBagFull

CaveOfOrigin_DianciesRoom_EventScript_WallaceAfter::
\tmsgbox CaveOfOrigin_DianciesRoom_Text_WallaceAfter, MSGBOX_DEFAULT
\trelease
\tend

CaveOfOrigin_DianciesRoom_EventScript_WallaceNotReady::
\tmsgbox CaveOfOrigin_DianciesRoom_Text_WallaceNotReady, MSGBOX_DEFAULT
\trelease
\tend

CaveOfOrigin_DianciesRoom_EventScript_WallaceBagFull::
\tmsgbox CaveOfOrigin_DianciesRoom_Text_WallaceBagFull, MSGBOX_DEFAULT
\trelease
\tend

CaveOfOrigin_DianciesRoom_Text_WallaceIntro:
\t.string "This crystal chamber records HOENN's\\n"
\t.string "deepest answer to pressure.\\p"
\t.string "As Champion, face my unrestricted rain\\n"
\t.string "exhibition. Nothing here is ceremonial.$"

CaveOfOrigin_DianciesRoom_Text_WallaceDefeat:
\t.string "You found calm inside impossible rain.$"

CaveOfOrigin_DianciesRoom_Text_WallaceAfter:
\t.string "Take this OVAL CHARM. Let the next\\n"
\t.string "generation inherit what you learned.$"

CaveOfOrigin_DianciesRoom_Text_WallaceNotReady:
\t.string "First defeat me at the POKéMON LEAGUE.\\n"
\t.string "Then return for the unrestricted battle.$"

CaveOfOrigin_DianciesRoom_Text_WallaceBagFull:
\t.string "Make room for the OVAL CHARM and return.\\n"
\t.string "A Champion's reward will wait.$"
""",
    "DewfordManor_1F": """DewfordManor_1F_EventScript_Historian::
\tmsgbox DewfordManor_1F_Text_Historian, MSGBOX_NPC
\tend
DewfordManor_1F_Text_Historian:
\t.string "This manor housed Hoenn's first study\\n"
\t.string "of MEGA EVOLUTION.\\p"
\t.string "The researchers found that a stone is\\n"
\t.string "only a key. Trust supplies the power.\\p"
\t.string "Their final entry mentions a crystal\\n"
\t.string "chamber beneath SOOTOPOLIS.$"
""",
    "DewfordMeadow": """DewfordMeadow_EventScript_Warden::
\tmsgbox DewfordMeadow_Text_Warden, MSGBOX_NPC
\tend
DewfordMeadow_Text_Warden:
\t.string "STEVEN reopened the old manor when the\\n"
\t.string "CHAMPION'S SIGNS began shining.\\p"
\t.string "He says the stones, the legends, and\\n"
\t.string "the shaking at MT. CHIMNEY are linked.$"
""",
    "EmberPath": """EmberPath_EventScript_Warden::
\tmsgbox EmberPath_Text_Warden, MSGBOX_NPC
\tend
EmberPath_Text_Warden:
\t.string "TEAM MAGMA's machine woke every fault\\n"
\t.string "beneath this mountain.\\p"
\t.string "MOLTRES came to test the heat. Farther\\n"
\t.string "east, something heavier answered.$"
""",
    "MirageTower_B1F": """MirageTower_B1F_EventScript_Inscription::
\tmsgbox MirageTower_B1F_Text_Inscription, MSGBOX_NPC
\tend
MirageTower_B1F_Text_Inscription:
\t.string "The inscription reads:\\p"
\t.string "WHEN LAND, SEA, AND SKY CONTEST, THE\\n"
\t.string "CELLS OF ORDER GATHER BELOW.$"
""",
    "PetalburgWoods_2": """PetalburgWoods_2_EventScript_Ranger::
\tmsgbox PetalburgWoods_2_Text_Ranger, MSGBOX_NPC
\tend
PetalburgWoods_2_Text_Ranger:
\t.string "These paths returned when the first\\n"
\t.string "CHAMPION'S SIGN lit up.\\p"
\t.string "A swordsman of the forest appears only\\n"
\t.string "beside a fully grown BRELOOM.$"
""",
    "PetalburgWoods_3": """PetalburgWoods_3_EventScript_Ranger::
\tmsgbox PetalburgWoods_3_Text_Ranger, MSGBOX_NPC
\tend
PetalburgWoods_3_Text_Ranger:
\t.string "Time folds strangely in this grove.\\p"
\t.string "The old stories pair CELEBI with a\\n"
\t.string "kind healer whose line ends in BLISSEY.$"
""",
    "Route111_RuinsExterior": """Route111_RuinsExterior_EventScript_Archaeologist::
\tmsgbox Route111_RuinsExterior_Text_Archaeologist, MSGBOX_NPC
\tend
Route111_RuinsExterior_Text_Archaeologist:
\t.string "The desert was a proving ground for\\n"
\t.string "Hoenn's ancient Champions.\\p"
\t.string "Bring CASTFORM to the summit. The land\\n"
\t.string "itself may recognize a weather master.$"
""",
    "SandstrewnRuins": """SandstrewnRuins_EventScript_Archaeologist::
\tmsgbox SandstrewnRuins_Text_Archaeologist, MSGBOX_NPC
\tend
SandstrewnRuins_Text_Archaeologist:
\t.string "Every fossil here records a different\\n"
\t.string "answer to survival. Take them; build.\\p"
\t.string "Below us, cells of green light wait for\\n"
\t.string "the guardian of the land.$"
""",
    "ScorchedSlab_B2F": """ScorchedSlab_B2F_EventScript_Warden::
\tmsgbox ScorchedSlab_B2F_Text_Warden, MSGBOX_NPC
\tend
ScorchedSlab_B2F_Text_Warden:
\t.string "The slab is a lock between two fires.\\p"
\t.string "HEATRAN guards the earth below. A white\\n"
\t.string "dragon answers only after time itself.$"
""",
    "Seaspray_Cave": """Seaspray_Cave_EventScript_Explorer::
\tmsgbox Seaspray_Cave_Text_Explorer, MSGBOX_NPC
\tend
Seaspray_Cave_Text_Explorer:
\t.string "AQUA's tides exposed this cave, but they\\n"
\t.string "did not create it.\\p"
\t.string "At the frozen floor, space bends around\\n"
\t.string "a SIGN shaped like KINGDRA.$"
""",
    "VerdanturfMeadow": """VerdanturfMeadow_EventScript_Warden::
\tmsgbox VerdanturfMeadow_Text_Warden, MSGBOX_NPC
\tend
VerdanturfMeadow_Text_Warden:
\t.string "This meadow heals after every storm.\\p"
\t.string "Bring ROSERADE after you master the heat.\\n"
\t.string "A tiny guardian may reveal itself.$"
""",
}


STATIC_LEGENDS = {
    "CaveOfOrigin_DianciesRoom": ("LOCALID_EC_DIANCIE", "DIANCIE", "FLAG_EC_CAUGHT_DIANCIE", 2, "Diancie"),
    "MeteorFalls_JirachisRoom": ("LOCALID_EC_JIRACHI", "JIRACHI", "FLAG_EC_CAUGHT_JIRACHI", 2, "Jirachi"),
    "EmberPath": ("LOCALID_EC_MOLTRES", "MOLTRES", "FLAG_EC_CAUGHT_MOLTRES", 1, "Moltres"),
    "ScorchedSlab_HeatransRoom": ("LOCALID_EC_HEATRAN", "HEATRAN", "FLAG_EC_CAUGHT_HEATRAN", 2, "Heatran"),
}

VISIBLE_SIGNS = {
    "AlteringCave_1F": ("LOCALID_EC_HOOPA", "HOOPA", 17, "FLAG_EC_CAUGHT_HOOPA", "UNOWN", "the distortion"),
    "AlteringCave_B1F": ("LOCALID_EC_MEWTWO", "MEWTWO", 61, "FLAG_EC_CAUGHT_MEWTWO", "DITTO", "the deepest distortion"),
    "AshenWoods": ("LOCALID_EC_OKIDOGI", "OKIDOGI", 64, "FLAG_EC_CAUGHT_OKIDOGI", "MIGHTYENA", "this ash grove"),
    "CaveOfOrigin_DianciesRoom": ("LOCALID_EC_TERAPAGOS", "TERAPAGOS", 67, "FLAG_EC_CAUGHT_TERAPAGOS", "DIANCIE", "the crystal chamber"),
    "DewfordManor_1F": ("LOCALID_EC_MELOETTA", "MELOETTA", 59, "FLAG_EC_CAUGHT_MELOETTA", "EXPLOUD", "the abandoned ballroom"),
    "MeteorFalls_JirachisRoom": ("LOCALID_EC_COSMOG", "COSMOG", 54, "FLAG_EC_CAUGHT_COSMOG", "JIRACHI", "the wishing chamber"),
    "PetalburgWoods_3": ("LOCALID_EC_CELEBI", "CELEBI", 5, "FLAG_EC_CAUGHT_CELEBI", "BLISSEY", "the timeless grove"),
    "PetalburgWoods_2": ("LOCALID_EC_VIRIZION", "VIRIZION", 43, "FLAG_EC_CAUGHT_VIRIZION", "BRELOOM", "the deep forest"),
    "Route111_RuinsExterior": ("LOCALID_EC_LANDORUS", "LANDORUS", 20, "FLAG_EC_CAUGHT_LANDORUS", "CASTFORM", "the desert summit"),
    "SandstrewnRuins_B1F": ("LOCALID_EC_ZYGARDE", "ZYGARDE", 52, "FLAG_EC_CAUGHT_ZYGARDE", "LANDORUS", "the cells below"),
    "ScorchedSlab_B2F": ("LOCALID_EC_RESHIRAM", "RESHIRAM", 32, "FLAG_EC_CAUGHT_RESHIRAM", "DIALGA", "the white flame"),
    "Seaspray_Cave_B1F": ("LOCALID_EC_PALKIA", "PALKIA", 25, "FLAG_EC_CAUGHT_PALKIA", "KINGDRA", "the frozen tide"),
    "VerdanturfMeadow": ("LOCALID_EC_SHAYMIN", "SHAYMIN", 33, "FLAG_EC_CAUGHT_SHAYMIN", "ROSERADE", "the healing meadow"),
}

ADDITIONAL_VISIBLE_SIGNS = {
    "DewfordManor_1F": [
        ("LOCALID_EC_MUNKIDORI", "MUNKIDORI", 63, "FLAG_EC_CAUGHT_MUNKIDORI", "ORANGURU", "the sealed study"),
    ],
    "PetalburgWoods_2": [
        ("LOCALID_EC_WO_CHIEN", "WO_CHIEN", 68, "FLAG_EC_CAUGHT_WO_CHIEN", "TREVENANT", "the ruined shrine"),
    ],
    "VerdanturfMeadow": [
        ("LOCALID_EC_ENAMORUS", "ENAMORUS", 55, "FLAG_EC_CAUGHT_ENAMORUS", "SHAYMIN", "the recovering meadow"),
        ("LOCALID_EC_FEZANDIPITI", "FEZANDIPITI", 56, "FLAG_EC_CAUGHT_FEZANDIPITI", "ALTARIA", "the highland breeze"),
    ],
}


def main() -> None:
    maps = load(ROOT / "data" / "maps" / "map_groups.json")["gMapGroup_EmeraldChampionsExpansion"]
    layout_rows = {layout["id"]: layout for layout in load(ROOT / "data" / "layouts" / "layouts.json")["layouts"]}
    for map_name in maps:
        path = ROOT / "data" / "maps" / map_name / "map.json"
        payload = load(path)
        payload["object_events"] = old_obstacles(map_name) + ITEMS.get(map_name, []) + EXTRA_OBJECTS.get(map_name, [])
        payload["bg_events"] = []
        layout = layout_rows[payload["layout"]]
        for event in payload["object_events"]:
            if not (0 <= event["x"] < layout["width"] and 0 <= event["y"] < layout["height"]):
                raise SystemExit(f"{map_name}: object outside layout: {event}")
        save(path, payload)
        if map_name == "AshenWoods":
            script = preserve_poke_vial_script(path.parent / "scripts.inc")
        else:
            script = base_script(map_name)
        script += SCRIPTS.get(map_name, "")
        if map_name in STATIC_LEGENDS:
            script += static_legend_script(map_name, *STATIC_LEGENDS[map_name])
        if map_name in VISIBLE_SIGNS:
            script += visible_sign_script(map_name, *VISIBLE_SIGNS[map_name])
        for sign in ADDITIONAL_VISIBLE_SIGNS.get(map_name, []):
            script += visible_sign_script(map_name, *sign)
        (path.parent / "scripts.inc").write_text(script)
    # Expansion-map population replaces object and coordinate arrays wholesale.
    # Restore the cross-map chase after that reset and prove it survived.
    restore_poke_vial_quest()
    check_poke_vial_quest()
    print(f"populated_restored_maps={len(maps)}")
    print(f"item_objects={sum(len(rows) for rows in ITEMS.values())}")
    extra_signs = sum(len(rows) for rows in ADDITIONAL_VISIBLE_SIGNS.values())
    print(f"visible_legendary_objects={len(STATIC_LEGENDS) + len(VISIBLE_SIGNS) + extra_signs}")


if __name__ == "__main__":
    main()
