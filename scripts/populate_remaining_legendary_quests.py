#!/usr/bin/env python3
"""Restore and author legendary quests on retained Hoenn maps."""

from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BEGIN = "@ BEGIN EMERALD CHAMPIONS LEGENDARY ROOTS"
END = "@ END EMERALD CHAMPIONS LEGENDARY ROOTS"


def load(path: Path):
    return json.loads(path.read_text())


def save(path: Path, payload) -> None:
    path.write_text(json.dumps(payload, indent=2) + "\n")


def obj(species: str, x: int, y: int, script: str, flag: str, local_id: str) -> dict:
    return {
        "local_id": local_id,
        "graphics_id": f"OBJ_EVENT_GFX_SPECIES({species})",
        "x": x,
        "y": y,
        "elevation": 3,
        "movement_type": "MOVEMENT_TYPE_FACE_DOWN",
        "movement_range_x": 0,
        "movement_range_y": 0,
        "trainer_type": "TRAINER_TYPE_NONE",
        "trainer_sight_or_berry_tree_id": "0",
        "script": script,
        "flag": flag,
    }


QUESTS = {
    "ShoalCave_LowTideIceRoom": [
        ("ARTICUNO", 8, 8, 53, "FROSLASS", "FLAG_EC_CAUGHT_ARTICUNO", "the ice altar"),
    ],
    "NewMauville_Inside": [
        ("ZAPDOS", 33, 15, 69, "MANECTRIC", "FLAG_EC_CAUGHT_ZAPDOS", "the silent generator"),
        ("MELTAN", 37, 7, 60, "MAGNETON", "FLAG_EC_CAUGHT_MELTAN", "the liquid-metal conduit"),
    ],
    "RustboroCity_DevonCorp_2F": [
        ("MAGEARNA", 8, 7, 58, "DIANCIE", "FLAG_EC_CAUGHT_MAGEARNA", "DEVON's sealed prototype"),
    ],
}

CUSTOM_OBJECTS = {
    "MtPyre_6F": [
        obj("PECHARUNT", 8, 8, "MtPyre_6F_EventScript_Pecharunt", "FLAG_EC_CAUGHT_PECHARUNT", "LOCALID_EC_PECHARUNT"),
    ],
    "SealedChamber_InnerRoom": [
        obj("REGIGIGAS", 10, 12, "SealedChamber_InnerRoom_EventScript_RegigigasEC", "FLAG_EC_CAUGHT_REGIGIGAS", "LOCALID_EC_REGIGIGAS"),
    ],
}


def title(species: str) -> str:
    return species.title().replace("_", "")


def visible_script(map_name: str, species: str, sign_id: int, required: str, flag: str, place: str) -> str:
    name = title(species)
    local_id = f"LOCALID_EC_{species}"
    return f""".set EC_SIGN_{species}_ID, {sign_id}

{map_name}_EventScript_{name}::
\tlock
\tfaceplayer
\tsetvar VAR_0x8004, EC_SIGN_{species}_ID
\tspecial TryUnlockSelectedLegendarySign
\tgoto_if_eq VAR_RESULT, 0, {map_name}_EventScript_{name}Dormant
\tgoto_if_eq VAR_RESULT, 1, {map_name}_EventScript_{name}NeedsPartner
\tgoto_if_eq VAR_RESULT, 4, {map_name}_EventScript_{name}Cleanup
\tmsgbox {map_name}_Text_{name}Awakens, MSGBOX_DEFAULT
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
\tgoto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, {map_name}_EventScript_{name}Caught
\tmsgbox {map_name}_Text_{name}Remains, MSGBOX_DEFAULT
\trelease
\tend

{map_name}_EventScript_{name}Dormant::
\tmsgbox {map_name}_Text_{name}Dormant, MSGBOX_DEFAULT
\trelease
\tend

{map_name}_EventScript_{name}NeedsPartner::
\tmsgbox {map_name}_Text_{name}NeedsPartner, MSGBOX_DEFAULT
\trelease
\tend

{map_name}_EventScript_{name}Caught::
\tsetflag {flag}
{map_name}_EventScript_{name}Cleanup::
\tsetflag {flag}
\tremoveobject {local_id}
\trelease
\tend

{map_name}_Text_{name}Dormant:
\t.string "A CHAMPION'S SIGN marks {place}.\\p"
\t.string "Its light is dormant. HOENN's story\\n"
\t.string "has not yet reached this place.$"

{map_name}_Text_{name}NeedsPartner:
\t.string "The SIGN sketches {required}.\\p"
\t.string "Bring that POKéMON's family here, and\\n"
\t.string "the hidden challenger may answer.$"

{map_name}_Text_{name}Awakens:
\t.string "The CHAMPION'S SIGN erupts with light!\\n"
\t.string "{species.replace('_', ' ')} answers the challenge!$"

{map_name}_Text_{name}Remains:
\t.string "The SIGN remains bright. {species.replace('_', ' ')}\\n"
\t.string "will accept another challenge.$"

"""


def custom_scripts() -> dict[str, str]:
    return {
        "MtPyre_6F": """.set EC_SIGN_PECHARUNT_ID, 65

MtPyre_6F_EventScript_Pecharunt::
\tlock
\tfaceplayer
\tsetvar VAR_0x8004, 64
\tspecial GetSelectedLegendarySignState
\tgoto_if_ne VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntDormant
\tsetvar VAR_0x8004, 63
\tspecial GetSelectedLegendarySignState
\tgoto_if_ne VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntDormant
\tsetvar VAR_0x8004, 56
\tspecial GetSelectedLegendarySignState
\tgoto_if_ne VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntDormant
\tsetvar VAR_0x8004, EC_SIGN_PECHARUNT_ID
\tspecial TryUnlockSelectedLegendarySign
\tmsgbox MtPyre_6F_Text_PecharuntAwakens, MSGBOX_DEFAULT
\tsetvar VAR_0x8004, EC_SIGN_PECHARUNT_ID
\tspecial CreateSelectedLegendarySignEncounter
\tsetflag FLAG_SYS_CTRL_OBJ_DELETE
\tspecial BattleSetup_StartLegendaryBattle
\tclearflag FLAG_SYS_CTRL_OBJ_DELETE
\tspecialvar VAR_RESULT, GetBattleOutcome
\tgoto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, MtPyre_6F_EventScript_PecharuntCaught
\tmsgbox MtPyre_6F_Text_PecharuntRemains, MSGBOX_DEFAULT
\trelease
\tend

MtPyre_6F_EventScript_PecharuntDormant::
\tmsgbox MtPyre_6F_Text_PecharuntDormant, MSGBOX_DEFAULT
\trelease
\tend

MtPyre_6F_EventScript_PecharuntCaught::
\tsetflag FLAG_EC_CAUGHT_PECHARUNT
\tremoveobject LOCALID_EC_PECHARUNT
\trelease
\tend

MtPyre_6F_Text_PecharuntDormant:
\t.string "A violet SIGN shows three masks: a dog,\\n"
\t.string "a monkey, and a brilliant bird.\\p"
\t.string "Catch all three challengers before\\n"
\t.string "returning to this poisoned shrine.$"

MtPyre_6F_Text_PecharuntAwakens:
\t.string "The three masks flare, and the shrine's\\n"
\t.string "hidden master rolls into view!$"

MtPyre_6F_Text_PecharuntRemains:
\t.string "PECHARUNT settles back onto the shrine.\\n"
\t.string "The three masks remain lit.$"

""",
        "SealedChamber_InnerRoom": """.set EC_SIGN_REGIGIGAS_ID, 66

SealedChamber_InnerRoom_EventScript_RegigigasEC::
\tlock
\tfaceplayer
\tgoto_if_unset FLAG_BADGE07_GET, SealedChamber_InnerRoom_EventScript_RegigigasECDormant
\tsetvar VAR_0x8004, SPECIES_REGICE
\tspecial DoesPlayerPartyHaveSelectedSpeciesFamily
\tgoto_if_eq VAR_RESULT, FALSE, SealedChamber_InnerRoom_EventScript_RegigigasECNeedsRegis
\tsetvar VAR_0x8004, SPECIES_REGIROCK
\tspecial DoesPlayerPartyHaveSelectedSpeciesFamily
\tgoto_if_eq VAR_RESULT, FALSE, SealedChamber_InnerRoom_EventScript_RegigigasECNeedsRegis
\tsetvar VAR_0x8004, SPECIES_REGISTEEL
\tspecial DoesPlayerPartyHaveSelectedSpeciesFamily
\tgoto_if_eq VAR_RESULT, FALSE, SealedChamber_InnerRoom_EventScript_RegigigasECNeedsRegis
\tsetvar VAR_0x8004, EC_SIGN_REGIGIGAS_ID
\tspecial TryUnlockSelectedLegendarySign
\tmsgbox SealedChamber_InnerRoom_Text_RegigigasECAwakens, MSGBOX_DEFAULT
\tsetvar VAR_0x8004, EC_SIGN_REGIGIGAS_ID
\tspecial CreateSelectedLegendarySignEncounter
\tsetflag FLAG_SYS_CTRL_OBJ_DELETE
\tspecial BattleSetup_StartLegendaryBattle
\tclearflag FLAG_SYS_CTRL_OBJ_DELETE
\tspecialvar VAR_RESULT, GetBattleOutcome
\tgoto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, SealedChamber_InnerRoom_EventScript_RegigigasECCaught
\tmsgbox SealedChamber_InnerRoom_Text_RegigigasECRemains, MSGBOX_DEFAULT
\trelease
\tend

SealedChamber_InnerRoom_EventScript_RegigigasECDormant::
\tmsgbox SealedChamber_InnerRoom_Text_RegigigasECDormant, MSGBOX_DEFAULT
\trelease
\tend

SealedChamber_InnerRoom_EventScript_RegigigasECNeedsRegis::
\tmsgbox SealedChamber_InnerRoom_Text_RegigigasECNeedsRegis, MSGBOX_DEFAULT
\trelease
\tend

SealedChamber_InnerRoom_EventScript_RegigigasECCaught::
\tsetflag FLAG_EC_CAUGHT_REGIGIGAS
\tremoveobject LOCALID_EC_REGIGIGAS
\trelease
\tend

SealedChamber_InnerRoom_Text_RegigigasECDormant:
\t.string "The giant statue is silent. Seven BADGES\\n"
\t.string "are needed to read its final seal.$"

SealedChamber_InnerRoom_Text_RegigigasECNeedsRegis:
\t.string "Braille names REGIROCK, REGICE, and\\n"
\t.string "REGISTEEL. Bring all three in your party.$"

SealedChamber_InnerRoom_Text_RegigigasECAwakens:
\t.string "The three REGIS answer together!\\n"
\t.string "The colossal statue begins to move!$"

SealedChamber_InnerRoom_Text_RegigigasECRemains:
\t.string "REGIGIGAS returns to its stone stance.\\n"
\t.string "The three seals remain open.$"

""",
    }


def strip_generated_block(text: str) -> str:
    pattern = re.compile(rf"\n?{re.escape(BEGIN)}.*?{re.escape(END)}\n?", re.S)
    return pattern.sub("\n", text).rstrip() + "\n"


def main() -> None:
    layout_rows = {row["id"]: row for row in load(ROOT / "data/layouts/layouts.json")["layouts"]}
    custom = custom_scripts()
    maps = set(QUESTS) | set(CUSTOM_OBJECTS)
    object_count = 0

    for map_name in sorted(maps):
        map_path = ROOT / "data/maps" / map_name / "map.json"
        payload = load(map_path)
        remove_ids = {
            f"LOCALID_EC_{species}"
            for species, *_ in QUESTS.get(map_name, [])
        } | {row["local_id"] for row in CUSTOM_OBJECTS.get(map_name, [])}
        payload["object_events"] = [
            row for row in payload.get("object_events", [])
            if row.get("local_id") not in remove_ids
        ]
        for species, x, y, sign_id, required, flag, place in QUESTS.get(map_name, []):
            payload["object_events"].append(obj(
                species, x, y, f"{map_name}_EventScript_{title(species)}", flag, f"LOCALID_EC_{species}"
            ))
        payload["object_events"].extend(CUSTOM_OBJECTS.get(map_name, []))

        layout = layout_rows[payload["layout"]]
        for row in payload["object_events"]:
            if not (0 <= row["x"] < layout["width"] and 0 <= row["y"] < layout["height"]):
                raise SystemExit(f"{map_name}: object outside layout: {row}")
        save(map_path, payload)
        object_count += len(QUESTS.get(map_name, [])) + len(CUSTOM_OBJECTS.get(map_name, []))

        scripts_path = map_path.parent / "scripts.inc"
        scripts = strip_generated_block(scripts_path.read_text())
        generated = [BEGIN]
        for species, x, y, sign_id, required, flag, place in QUESTS.get(map_name, []):
            generated.append(visible_script(map_name, species, sign_id, required, flag, place))
        if map_name in custom:
            generated.append(custom[map_name])
        generated.append(END)
        scripts_path.write_text(scripts + "\n" + "\n".join(generated) + "\n")

    print(f"retained_map_legendary_objects={object_count}")
    print("retained_map_legendary_maps=" + ",".join(sorted(maps)))


if __name__ == "__main__":
    main()
