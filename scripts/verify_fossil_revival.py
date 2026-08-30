#!/usr/bin/env python3
"""Verify the complete, repeatable Devon fossil-revival contract."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DEVON = ROOT / "data/maps/RustboroCity_DevonCorp_2F/scripts.inc"
MANIAC = ROOT / "data/maps/Route114_FossilManiacsTunnel/scripts.inc"
ROUTE = ROOT / "data/maps/Route114/scripts.inc"
ACQUISITION_SOURCES = {
    "ITEM_ROOT_FOSSIL": ("data/maps/MirageTower_4F/scripts.inc", "data/maps/DesertUnderpass/scripts.inc"),
    "ITEM_CLAW_FOSSIL": ("data/maps/MirageTower_4F/scripts.inc", "data/maps/DesertUnderpass/scripts.inc"),
    "ITEM_OLD_AMBER": ("data/maps/RustboroCity_Gym/scripts.inc",),
    **{
        item: ("data/maps/SandstrewnRuins/map.json",)
        for item in (
            "ITEM_HELIX_FOSSIL", "ITEM_DOME_FOSSIL", "ITEM_ARMOR_FOSSIL", "ITEM_SKULL_FOSSIL",
            "ITEM_COVER_FOSSIL", "ITEM_PLUME_FOSSIL", "ITEM_JAW_FOSSIL", "ITEM_SAIL_FOSSIL",
        )
    },
}

FOSSILS = (
    ("ITEM_ROOT_FOSSIL", 1, "SPECIES_LILEEP", "RootFossil", "Lileep"),
    ("ITEM_CLAW_FOSSIL", 2, "SPECIES_ANORITH", "ClawFossil", "Anorith"),
    ("ITEM_HELIX_FOSSIL", 3, "SPECIES_OMANYTE", "HelixFossil", "Omanyte"),
    ("ITEM_DOME_FOSSIL", 4, "SPECIES_KABUTO", "DomeFossil", "Kabuto"),
    ("ITEM_OLD_AMBER", 5, "SPECIES_AERODACTYL", "OldAmber", "Aerodactyl"),
    ("ITEM_ARMOR_FOSSIL", 6, "SPECIES_SHIELDON", "ArmorFossil", "Shieldon"),
    ("ITEM_SKULL_FOSSIL", 7, "SPECIES_CRANIDOS", "SkullFossil", "Cranidos"),
    ("ITEM_COVER_FOSSIL", 8, "SPECIES_TIRTOUGA", "CoverFossil", "Tirtouga"),
    ("ITEM_PLUME_FOSSIL", 9, "SPECIES_ARCHEN", "PlumeFossil", "Archen"),
    ("ITEM_JAW_FOSSIL", 10, "SPECIES_TYRUNT", "JawFossil", "Tyrunt"),
    ("ITEM_SAIL_FOSSIL", 11, "SPECIES_AMAURA", "SailFossil", "Amaura"),
)


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def block(text: str, label: str) -> str:
    marker = f"{label}::"
    require(marker in text, f"missing script label: {label}")
    body = text.split(marker, 1)[1]
    next_label = body.find("\n\n")
    return body if next_label < 0 else body[:next_label]


def main() -> None:
    devon = DEVON.read_text()
    maniac = MANIAC.read_text()
    route = ROUTE.read_text()

    scientist = block(devon, "RustboroCity_DevonCorp_2F_EventScript_FossilScientist")
    notice = block(devon, "RustboroCity_DevonCorp_2F_EventScript_NoticeFossil")
    ready = block(devon, "RustboroCity_DevonCorp_2F_EventScript_FossilMonReady")
    begin = block(devon, "RustboroCity_DevonCorp_2F_EventScript_BeginFossilRevival")
    receive = block(devon, "RustboroCity_DevonCorp_2F_EventScript_ReceiveFossilMon")
    finish = block(devon, "RustboroCity_DevonCorp_2F_EventScript_FinishReceivingFossilMon")
    maniac_talk = block(maniac, "Route114_FossilManiacsTunnel_EventScript_FossilManiac")

    require("MULTI_FOSSIL" not in devon, "Devon still uses the Root/Claw-only static menu")
    require(
        "dynmultistack 0, 1, FALSE, 5, FALSE, 0, DYN_MULTICHOICE_CB_SHOW_ITEM" in notice,
        "fossil selection is not the native scrolling item menu",
    )
    require(
        "removeitem VAR_0x8004" in begin
        and begin.index("removeitem VAR_0x8004")
        < begin.index("goto_if_eq VAR_RESULT, FALSE")
        < begin.index("setvar VAR_FOSSIL_RESURRECTION_STATE, 1"),
        "revival state can advance without a successful fossil removal",
    )
    require(
        "copyvar VAR_0x8004, VAR_TEMP_TRANSFERRED_SPECIES" in receive
        and "setvar VAR_0x8005, 20" in receive
        and "special GiveEmeraldChampionsPreparedPokemon" in receive
        and "MON_GIVEN_TO_PARTY" in receive
        and "MON_GIVEN_TO_PC" in receive
        and "Common_EventScript_NoMoreRoomForPokemon" in receive,
        "revived Pokemon is not prepared and delivered across party, PC, and no-room outcomes",
    )
    for line in (
        "setvar VAR_FOSSIL_RESURRECTION_STATE, 0",
        "setvar VAR_WHICH_FOSSIL_REVIVED, 0",
        "setflag FLAG_RECEIVED_REVIVED_FOSSIL_MON",
    ):
        require(line in finish, f"revival completion is missing: {line}")

    for item, revival_id, species, choice_suffix, ready_suffix in FOSSILS:
        require(
            any(item in (ROOT / source).read_text() for source in ACQUISITION_SOURCES[item]),
            f"{item} has no verified Hoenn acquisition source",
        )
        require(f"checkitem {item}" in scientist, f"Devon does not recognize {item}")
        require(
            f"call_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_Push{choice_suffix}"
            in notice,
            f"owned {item} is not added to the selection menu",
        )
        require(
            f"case {item}, RustboroCity_DevonCorp_2F_EventScript_Choose{choice_suffix}" in notice,
            f"{item} menu result is not handled",
        )
        choose = block(devon, f"RustboroCity_DevonCorp_2F_EventScript_Choose{choice_suffix}")
        require(f"setvar VAR_0x8004, {item}" in choose, f"{item} does not select its own Bag item")
        require(
            f"setvar VAR_WHICH_FOSSIL_REVIVED, {revival_id}" in choose,
            f"{item} has the wrong persistent revival ID",
        )
        require(
            f"goto_if_eq VAR_WHICH_FOSSIL_REVIVED, {revival_id}, "
            f"RustboroCity_DevonCorp_2F_EventScript_Set{ready_suffix}Ready" in ready,
            f"revival ID {revival_id} has no ready dispatch",
        )
        set_ready = block(devon, f"RustboroCity_DevonCorp_2F_EventScript_Set{ready_suffix}Ready")
        require(
            f"setvar VAR_TEMP_TRANSFERRED_SPECIES, {species}" in set_ready,
            f"{item} revives the wrong species (expected {species})",
        )
        require(f"checkitem {item}" in maniac_talk, f"Fossil Maniac does not recognize {item}")

    require(
        maniac_talk.index("checkitem ITEM_SAIL_FOSSIL")
        < maniac_talk.index("goto_if_set FLAG_RECEIVED_REVIVED_FOSSIL_MON"),
        "Fossil Maniac ignores newly held fossils after the first revival",
    )
    require("from every era" in devon, "Devon dialogue still implies limited fossil support")
    require("every ancient era" in maniac, "Fossil Maniac dialogue still implies limited fossil support")
    require("Fossils from every era studied!" in route, "Route 114 sign does not match fossil support")

    print("PASS: all 11 complete fossils revive repeatably with native selection and safe delivery")


if __name__ == "__main__":
    main()
