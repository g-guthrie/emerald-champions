#!/usr/bin/env python3
"""Static gates for the nine-generation starter and rival contract."""

from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TRIOS = [
    ("BULBASAUR", "CHARMANDER", "SQUIRTLE"),
    ("CHIKORITA", "CYNDAQUIL", "TOTODILE"),
    ("TREECKO", "TORCHIC", "MUDKIP"),
    ("TURTWIG", "CHIMCHAR", "PIPLUP"),
    ("SNIVY", "TEPIG", "OSHAWOTT"),
    ("CHESPIN", "FENNEKIN", "FROAKIE"),
    ("ROWLET", "LITTEN", "POPPLIO"),
    ("GROOKEY", "SCORBUNNY", "SOBBLE"),
    ("SPRIGATITO", "FUECOCO", "QUAXLY"),
]
EVOLUTIONS = [
    ("IVYSAUR", "CHARMELEON", "WARTORTLE", "VENUSAUR", "CHARIZARD", "BLASTOISE"),
    ("BAYLEEF", "QUILAVA", "CROCONAW", "MEGANIUM", "TYPHLOSION", "FERALIGATR"),
    ("GROVYLE", "COMBUSKEN", "MARSHTOMP", "SCEPTILE", "BLAZIKEN", "SWAMPERT"),
    ("GROTLE", "MONFERNO", "PRINPLUP", "TORTERRA", "INFERNAPE", "EMPOLEON"),
    ("SERVINE", "PIGNITE", "DEWOTT", "SERPERIOR", "EMBOAR", "SAMUROTT"),
    ("QUILLADIN", "BRAIXEN", "FROGADIER", "CHESNAUGHT", "DELPHOX", "GRENINJA"),
    ("DARTRIX", "TORRACAT", "BRIONNE", "DECIDUEYE", "INCINEROAR", "PRIMARINA"),
    ("THWACKEY", "RABOOT", "DRIZZILE", "RILLABOOM", "CINDERACE", "INTELEON"),
    ("FLORAGATO", "CROCALOR", "QUAXWELL", "MEOWSCARADA", "SKELEDIRGE", "QUAQUAVAL"),
]


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def main() -> None:
    chooser = (ROOT / "src" / "starter_choose.c").read_text()
    setup = (ROOT / "src" / "battle_setup.c").read_text()
    field = (ROOT / "src" / "field_specials.c").read_text()
    story = (ROOT / "data" / "scripts" / "emerald_champions.inc").read_text()
    vars_header = (ROOT / "include" / "constants" / "vars.h").read_text()

    for generation, trio in enumerate(TRIOS, 1):
        row = "[{}] = {{{}}}".format(
            generation,
            ", ".join(f"SPECIES_{species}" for species in trio),
        )
        compact_chooser = re.sub(r"\s+", "", chooser)
        require(re.sub(r"\s+", "", row) in compact_chooser, f"generation {generation} starter trio drifted")
    for family in EVOLUTIONS:
        for species in family:
            require(f"SPECIES_{species}" in chooser, f"starter evolution mapping lacks {species}")

    require("VAR_STARTER_GEN" in vars_header and "0x4083" in vars_header, "starter-generation save var is missing")
    require("SCROLL_MULTI_STARTER_REGIONS" in field, "native region selector is missing")
    for region in ("Kanto", "Johto", "Hoenn", "Sinnoh", "Unova", "Kalos", "Alola", "Galar", "Paldea"):
        require(f'COMPOUND_STRING("{region}")' in field, f"region selector lacks {region}")
        require(f"StarterRegion{region}" in story, f"story buffer lacks {region}")
    require(set(re.findall(r"setvar VAR_STARTER_GEN, (\d+)", story)) == {str(i) for i in range(1, 10)},
            "starter selection does not map exactly to generations 1-9")

    house_scripts = [
        ROOT / "data/maps/LittlerootTown_MaysHouse_1F/scripts.inc",
        ROOT / "data/maps/LittlerootTown_MaysHouse_2F/scripts.inc",
        ROOT / "data/maps/LittlerootTown_BrendansHouse_1F/scripts.inc",
        ROOT / "data/maps/LittlerootTown_BrendansHouse_2F/scripts.inc",
    ]
    require(sum(path.read_text().count("call Common_EventScript_ChooseStarterRegion") for path in house_scripts) == 10,
            "not every valid rival introduction opens the region selector")
    require("ApplyRegionalRivalStarter" in setup, "rival starter override is missing")
    require("GetMiddleEvolutionForStarter" in setup and "GetFinalEvolutionForStarter" in setup,
            "later rivals do not advance the selected starter family")
    require("ApplyEmeraldChampionsOpponentSet(&gParties[B_TRAINER_PLAYER][0], 0)" in setup,
            "the player's starter is not battle-ready")

    presets = json.loads((ROOT / "data/emerald_champions/emerald_champions_battle_sets.json").read_text())
    preset_species = {entry["species"].removeprefix("SPECIES_") for entry in presets["defaults"]}
    all_stages = {species for trio in TRIOS for species in trio} | {species for family in EVOLUTIONS for species in family}
    require(all_stages <= preset_species, f"starter stages lack battle sets: {sorted(all_stages - preset_species)}")

    print("PASS: all nine regional starter trios are selectable")
    print("PASS: all 81 starter stages have competitive presets")
    print("PASS: every rival milestone follows the chosen counter-starter family")


if __name__ == "__main__":
    main()
