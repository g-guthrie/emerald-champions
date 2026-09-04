#!/usr/bin/env python3
"""Static transactional gates for the Mauville competitive Pokemon archive."""

from __future__ import annotations

import re
from pathlib import Path

from run_emerald_champions_runtime_gates import RUNTIME_GATES, TEST_DECLARATION, curated_test_sources, filter_matches


ROOT = Path(__file__).resolve().parents[1]
STARTERS = (
    "BULBASAUR", "CHARMANDER", "SQUIRTLE",
    "CHIKORITA", "CYNDAQUIL", "TOTODILE",
    "TREECKO", "TORCHIC", "MUDKIP",
    "TURTWIG", "CHIMCHAR", "PIPLUP",
    "SNIVY", "TEPIG", "OSHAWOTT",
    "CHESPIN", "FENNEKIN", "FROAKIE",
    "ROWLET", "LITTEN", "POPPLIO",
    "GROOKEY", "SCORBUNNY", "SOBBLE",
    "SPRIGATITO", "FUECOCO", "QUAXLY",
)
SPECIAL_PRIZES = ("GENESECT", "POIPOLE")
REGIONS = ("KANTO", "JOHTO", "HOENN", "SINNOH", "UNOVA", "KALOS", "ALOLA", "GALAR", "PALDEA")
SAVE_SAFE_FLAG_IDS = {
    *range(0x4A2, 0x4B1),
    0x4B4,
    0x4B5,
    0x4C7,
    0x4C8,
    0x4C9,
    0x4D0,
    0x4D1,
    0x4D2,
    0x4D3,
    0x4D4,
    0x4D5,
    0x4D6,
}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def function_block(source: str, name: str) -> str:
    match = re.search(rf"(?:static\s+)?(?:void|u8)\s+{name}\([^)]*\)\s*\{{", source)
    require(match is not None, f"missing function {name}")
    start = match.start()
    next_function = re.search(r"\n(?:static\s+)?(?:void|u8)\s+[A-Za-z0-9_]+\([^)]*\)", source[match.end():])
    end = match.end() + next_function.start() if next_function else len(source)
    return source[start:end]


def strip_controls(text: str) -> str:
    return re.sub(r"\{[^}]+\}", "", text)


def main() -> None:
    flags = (ROOT / "include/constants/flags.h").read_text()
    field = (ROOT / "src/field_specials.c").read_text()
    specials = (ROOT / "data/specials.inc").read_text()
    script = (ROOT / "data/maps/MauvilleCity_GameCorner/scripts.inc").read_text()
    menus = (ROOT / "src/data/script_menu.h").read_text()
    menu_constants = (ROOT / "include/constants/script_menu.h").read_text()
    sets = (ROOT / "src/data/pokemon/emerald_champions_battle_sets.h").read_text()
    tests = (ROOT / "test/emerald_champions.c").read_text()

    flag_rows = {}
    for species in STARTERS:
        name = f"FLAG_EC_STARTER_ARCHIVE_{species}"
        match = re.search(rf"#define\s+{name}\s+(0x[0-9A-Fa-f]+)", flags)
        require(match is not None, f"missing persistent claim flag for {species}")
        flag_rows[species] = int(match.group(1), 0)
    require(len(set(flag_rows.values())) == len(STARTERS), "starter archive claim flags are not unique")
    require(set(flag_rows.values()) == SAVE_SAFE_FLAG_IDS,
            "starter claims moved outside the audited formerly-unused save-safe flag IDs")

    table = re.search(
        r"sEmeraldChampionsGameCornerPokemonPrizes\[\]\s*=\s*\{(.*?)\n\};",
        field,
        re.S,
    )
    require(table is not None, "Game Corner Pokemon prize table is missing")
    table_species = re.findall(r"\{SPECIES_([A-Z0-9_]+),", table.group(1))
    require(table_species == [*STARTERS, *SPECIAL_PRIZES], "Game Corner Pokemon prize table drifted")
    for species in STARTERS:
        require(f"{{SPECIES_{species}," in table.group(1), f"archive table lacks {species}")
        require(f"FLAG_EC_STARTER_ARCHIVE_{species}" in table.group(1), f"archive table flag mismatch for {species}")
    require("FLAG_RECEIVED_GAME_CORNER_GENESECT" in table.group(1), "Genesect one-time flag was lost")
    require("FLAG_RECEIVED_GAME_CORNER_POIPOLE" in table.group(1), "Poipole one-time flag was lost")

    claimed = function_block(field, "IsEmeraldChampionsGameCornerPokemonClaimed")
    give_wrapper = function_block(field, "GiveEmeraldChampionsGameCornerPokemon")
    give = function_block(field, "TryGiveEmeraldChampionsGameCornerPokemon")
    prepared = function_block(field, "TryGiveEmeraldChampionsPreparedPokemon")
    require("IsEmeraldChampionsInitialStarter" in claimed, "initial starter is not treated as already claimed")
    require("TryGiveEmeraldChampionsGameCornerPokemon" in give_wrapper and "TRUE" in give_wrapper,
            "give path can bypass initial-starter rejection")
    require("GetStarterPokemonForGeneration" in field and "VAR_STARTER_MON" in field and "VAR_STARTER_GEN" in field,
            "initial starter identity is not derived from persistent selection state")

    ordered_tokens = (
        "CreateRandomMon(&mon",
        "ApplyEmeraldChampionsRandomNonMegaSet(&mon)",
        "GiveScriptedMonToPlayer(&mon, PARTY_SIZE)",
    )
    positions = [prepared.find(token) for token in ordered_tokens]
    require(all(position >= 0 for position in positions), "competitive prize transaction is incomplete")
    require(positions == sorted(positions), "preset/delivery/claim transaction order is unsafe")
    require("giveResult == MON_CANT_GIVE" in prepared, "full party and PC are not retry-safe")
    require("RecordPlayerPartyMonHeldItemForRestoration" in prepared,
            "party-delivered preset item lacks restoration baseline")
    require("MarkLegendarySignCaughtBySpecies(species)" in give,
            "Genesect/Poipole acquisition does not update Legendary Signs")
    require(give.find("TryGiveEmeraldChampionsPreparedPokemon") < give.find("FlagSet(flag)"),
            "Game Corner claim flag can advance before prepared delivery")

    for name in ("IsEmeraldChampionsGameCornerPokemonClaimed", "GiveEmeraldChampionsGameCornerPokemon"):
        require(f"def_special {name}" in specials, f"{name} is not exposed to map scripts")
    require("def_special GiveEmeraldChampionsPreparedPokemon" in specials,
            "prepared story-gift service is not exposed to map scripts")
    require("givemon VAR_TEMP_1, 30" not in script, "old unprepared Genesect/Poipole gift path remains")
    give_call = script.index("special GiveEmeraldChampionsGameCornerPokemon")
    remove_coins = script.index("removecoins VAR_0x8006", give_call)
    require(give_call < remove_coins, "Coins are removed before prize delivery succeeds")
    require("goto_if_eq VAR_RESULT, MON_CANT_GIVE" in script[give_call:remove_coins],
            "no-room result is not handled before payment")
    require("goto_if_eq VAR_RESULT, EC_GAME_CORNER_PRIZE_SET_FAILED" in script[give_call:remove_coins],
            "preset failure is not handled before payment")
    require("setvar VAR_0x8006, 500" in script, "starter archive price is not 500 Coins")
    require("setvar VAR_0x8006, 7500" in script and "setvar VAR_0x8006, 6500" in script,
            "Genesect/Poipole prices drifted")

    for species in STARTERS:
        require(f"setvar VAR_0x8004, SPECIES_{species}" in script,
                f"map script cannot select {species}")
    for region in REGIONS:
        menu_id = f"MULTI_EC_STARTER_ARCHIVE_{region}"
        require(menu_id in menu_constants, f"missing menu ID {menu_id}")
        require(f"[{menu_id}]" in menus, f"missing menu table {menu_id}")
        require(
            f"multichoice 11, 0, {menu_id}, FALSE" in script,
            f"{menu_id} is not separated from the native Coins box",
        )
    require("STARTER ARCHIVE" in menus, "top-level prize menu lacks Starter Archive")
    require(menus.count("{CLEAR_TO 72}500 COINS") == len(STARTERS),
            "starter menu row count or 500-Coin price drifted")

    for species in (*STARTERS, *SPECIAL_PRIZES):
        block = re.search(rf"\[SPECIES_{species}\]\s*=\s*\{{(.*?)\n\s*\}},", sets, re.S)
        require(block is not None and ".requiredItem = ITEM_NONE" in block.group(1),
                f"{species} lacks a guaranteed non-Mega competitive preset")

    required_runtime_tests = (
        "rejects the initially chosen starter",
        "delivers a prepared alternate starter transactionally",
        "rejects a repeated archive claim",
        "keeps a full-storage claim retryable",
        "rejects invalid or presetless prizes",
    )
    for test_name in required_runtime_tests:
        require(f'Emerald Champions Game Corner {test_name}' in tests,
                f"missing Game Corner runtime test: {test_name}")
    for test_name in (
        "story gifts arrive battle-ready with restoration baselines",
        "prepared story gifts preserve PC delivery and no-room retries",
    ):
        require(f'Emerald Champions {test_name}' in tests,
                f"missing prepared-gift runtime test: {test_name}")

    gift_scripts = {
        "Weather Institute Castform": ROOT / "data/maps/Route119_WeatherInstitute_2F/scripts.inc",
        "Steven Beldum": ROOT / "data/maps/MossdeepCity_StevensHouse/scripts.inc",
        "Devon fossil revival": ROOT / "data/maps/RustboroCity_DevonCorp_2F/scripts.inc",
    }
    for label, path in gift_scripts.items():
        source = path.read_text()
        require("givemon" not in source, f"{label} still uses an unprepared raw givemon path")
        require("special GiveEmeraldChampionsPreparedPokemon" in source,
                f"{label} does not use the prepared-gift transaction")
        require("goto_if_eq VAR_RESULT, MON_GIVEN_TO_PARTY" in source
                and "goto_if_eq VAR_RESULT, MON_GIVEN_TO_PC" in source,
                f"{label} lost party/PC delivery branches")
        require("Common_EventScript_NoMoreRoomForPokemon" in source,
                f"{label} no-room retry path disappeared")
    castform = gift_scripts["Weather Institute Castform"].read_text()
    beldum = gift_scripts["Steven Beldum"].read_text()
    fossil = gift_scripts["Devon fossil revival"].read_text()
    require("setvar VAR_0x8004, SPECIES_CASTFORM_NORMAL" in castform
            and "setvar VAR_0x8005, 25" in castform,
            "Castform prepared-gift parameters drifted")
    require("setflag FLAG_RECEIVED_CASTFORM" in castform,
            "Castform finite claim flag disappeared")
    require(castform.count("giveitem ITEM_REVEAL_GLASS") == 2
            and "checkitem ITEM_REVEAL_GLASS, 1" in castform
            and "Route119_WeatherInstitute_2F_EventScript_RevealGlassBagFull" in castform,
            "Weather Institute Reveal Glass delivery lost its initial/retry-safe paths")
    altering_cave = (ROOT / "data/maps/AlteringCave_1F/map.json").read_text()
    require("ITEM_REVEAL_GLASS" not in altering_cave
            and "ITEM_BEAST_BALL" in altering_cave,
            "postgame Altering Cave still delays or duplicates the Reveal Glass")
    require("setvar VAR_0x8004, SPECIES_BELDUM" in beldum
            and "setvar VAR_0x8005, 5" in beldum,
            "Beldum prepared-gift parameters drifted")
    require("setflag FLAG_RECEIVED_BELDUM" in beldum,
            "Beldum finite claim flag disappeared")
    require("copyvar VAR_0x8004, VAR_TEMP_TRANSFERRED_SPECIES" in fossil
            and "setvar VAR_0x8005, 20" in fossil,
            "fossil prepared-gift parameters drifted")
    fossil_receive = fossil.split(
        "RustboroCity_DevonCorp_2F_EventScript_ReceiveFossilMon::", 1
    )[1].split("RustboroCity_DevonCorp_2F_EventScript_MatchCallScientist::", 1)[0]
    require(fossil_receive.find("special GiveEmeraldChampionsPreparedPokemon")
            < fossil_receive.find("setvar VAR_FOSSIL_RESURRECTION_STATE, 0"),
            "fossil resurrection state can clear before delivery succeeds")
    test_path = "test/emerald_champions.c"
    required_tests = {
        "Emerald Champions Game Corner " + behavior
        for behavior in (
            "rejects the initially chosen starter",
            "delivers a prepared alternate starter transactionally",
            "rejects a repeated archive claim",
            "keeps a full-storage claim retryable",
            "rejects invalid or presetless prizes",
        )
    }
    declarations = set(TEST_DECLARATION.findall((ROOT / test_path).read_text()))
    require(required_tests <= declarations, "required Game Corner runtime cases are missing")
    require(test_path in curated_test_sources(), "Game Corner tests are not compiled by the curated suite")
    require(all(any(gate.filter == test_path or filter_matches(gate.filter, name)
                    for gate in RUNTIME_GATES) for name in required_tests),
            "required Game Corner cases are not selected for runtime execution")

    # Native message boxes safely fit roughly 36 monospace characters.  Keep
    # new prize dialogue at 34 visible characters to preserve a margin.
    for literal in re.findall(r'\.string\s+"([^"]*)"', script):
        if not any(token in literal for token in ("competitive", "regional starter", "No Coins", "Which region")):
            continue
        for line in re.split(r"\\[npl]", literal):
            visible = strip_controls(line).rstrip("$")
            require(len(visible) <= 34, f"Game Corner dialogue line is too wide: {visible!r}")
    for species in STARTERS:
        require(len(species) + len(" 500 COINS") <= 24, f"starter menu row is too wide: {species}")

    print("PASS: all 27 regional starters have independent persistent claim flags")
    print("PASS: initial starter, no-room, preset-failure, payment, and one-time paths are transactional")
    print("PASS: all 29 Pokemon prizes receive a non-Mega competitive preset before party/PC delivery")
    print("PASS: Castform, Beldum, and all Devon fossil gifts use the retry-safe prepared transaction")
    print("PASS: all starter menus and new dialogue fit their native windows")


if __name__ == "__main__":
    main()
