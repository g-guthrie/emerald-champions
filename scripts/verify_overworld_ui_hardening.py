#!/usr/bin/env python3
"""Focused static regression checks for Verdant's nonbattle engine fixes."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
checks = []


def source(path):
    return (ROOT / path).read_text()


def check(name, condition):
    if not condition:
        raise AssertionError(name)
    checks.append(name)
    print(f"PASS: {name}")


main = source("src/main.c")
check("L=A key repeat compares raw held keys", "gMain.heldKeysRaw == keyInput" in main)

contest_ai = source("src/contest_ai.c")
check(
    "contest AI compares scripted contest effects",
    "gContestMoves[gContestMons[eContestAI.contestantId].moves[i]].effect" in contest_ai,
)

popup = source("src/map_name_popup.c")
check("map popup rejects WINDOW_NONE", "GetMapNamePopUpWindowId() != WINDOW_NONE" in popup)

landmarks = source("src/landmark.c")
route122_1 = landmarks.index("{MAPSEC_ROUTE_122, 1, Landmarks_Route122_0}")
route123_0 = landmarks.index("{MAPSEC_ROUTE_123, 0, Landmarks_Route123_0}")
check("PokeNav landmarks remain sorted", route122_1 < route123_0)

objects = source("src/event_object_movement.c")
check("object palette table has canonical sentinel", "{NULL,                                  OBJ_EVENT_PAL_TAG_NONE}" in objects)
check("missing object palettes do not index 0xFF", "if (i != 0xFF)\n        LoadSpritePaletteIfTagExists" in objects)
check("camera cannot spawn ground effects", objects.count("objEvent->localId != OBJ_EVENT_ID_CAMERA") == 3)

avatar = source("src/field_player_avatar.c")
check("blocked rotating gates use normal collision feedback", "if (adjustedCollision > 2)" in avatar)
check("surf dismount refreshes covering effects", "playerObjEvent->triggerGroundEffectsOnMove = TRUE;" in avatar)

cable_car = source("src/cable_car.c")
check("all cable-car cameo sprites are reachable", "rval % ARRAY_COUNT(hikerGraphicsIds)" in cable_car)

pokenav = source("src/pokenav_menu_handler_2.c")
check("PokeNav glow overwrites stale WIN0V", "SetGpuReg(REG_OFFSET_WIN0V, DISPLAY_HEIGHT);" in pokenav)

script_mon = source("src/script_pokemon_util.c")
check("scripted party index rejects PARTY_SIZE", "if (monIndex >= PARTY_SIZE)" in script_mon)

contest = source("src/contest.c")
check("right single quote is accepted in link contest names", "|| *nickname == CHAR_SGL_QUOT_RIGHT)" in contest)

egg = source("src/egg_hatch.c")
third_crack = egg.index("// Show the final cracked-egg frame")
check("egg hatch uses the third crack frame", "StartSpriteAnim(sprite, 3);" in egg[third_crack:third_crack + 220])

pokeball = source("src/pokeball.c")
timer_case = pokeball.index("case BALL_TIMER:")
check("Timer Ball keeps its unique open frame", pokeball.rfind("switch (ballId)", 0, timer_case) != -1)

dodrio = source("src/dodrio_berry_picking.c")
pal = dodrio.index("static const u16 sBg_Pal[]")
check("Dodrio minigame palettes are ordered correctly", "tree_border.gbapal" in dodrio[pal:pal + 180])

wireless = source("src/wireless_communication_status_screen.c")
check("wireless headers use the array count", "ARRAY_COUNT(sHeaderTexts) - 2" in wireless)
check("wireless search activity cannot index group 0xFF", "group_type(i) == (u8)GROUPTYPE_NONE" in wireless)

bag = source("src/item_menu.c")
bag_icons = source("src/item_menu_icons.c")
check("bag cursor destroys the previous icon slot", "RemoveBagItemIconSprite(gBagMenu->itemIconSlot ^ 1);" in bag)
check("bag icon reuse hides before palette replacement", "spriteId[id ^ 1]" in bag_icons and "DestroySpriteAndFreeResources" in bag_icons)

route111 = source("data/maps/Route111/scripts.inc")
upgrade = route111[route111.index("Route111_EventScript_UpgradeVialHideNurse::"):route111.index("Route111_EventScript_PlayerFaceUp::")]
check("Route 111 vial nurse is removed in place", "removeobject LOCALID_NURSE" in upgrade and "applymovement" not in upgrade)

print(f"{len(checks)} overworld/UI checks passed")
