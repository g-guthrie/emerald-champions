#!/usr/bin/env python3
"""Verify generated battle-set data and current wild-table coverage."""

from __future__ import annotations

import json
import collections
import re
from pathlib import Path

from verify_trainer_ability_legality import (
    SPECIES_MARKER,
    configured_species_abilities,
    preprocess_species_info,
    resolve_species,
    species_aliases,
)


ROOT = Path(__file__).resolve().parents[1]

LOWERS_ATTACK = {
    "NATURE_BOLD", "NATURE_MODEST", "NATURE_CALM", "NATURE_TIMID",
}
LOWERS_SP_ATTACK = {
    "NATURE_ADAMANT", "NATURE_IMPISH", "NATURE_CAREFUL", "NATURE_JOLLY",
}
LOCKED_ITEMS = {
    "ITEM_CHOICE_BAND", "ITEM_CHOICE_SCARF", "ITEM_CHOICE_SPECS", "ITEM_ASSAULT_VEST",
}
CHOICE_COMPATIBLE_STATUS = {"MOVE_TRANSFORM", "MOVE_TRICK", "MOVE_SWITCHEROO"}
SCREEN_MOVES = {"MOVE_REFLECT", "MOVE_LIGHT_SCREEN", "MOVE_AURORA_VEIL"}
CHARGE_MOVES = {
    "MOVE_SOLAR_BEAM", "MOVE_SOLAR_BLADE", "MOVE_SKY_ATTACK", "MOVE_GEOMANCY",
    "MOVE_METEOR_BEAM", "MOVE_ELECTRO_SHOT", "MOVE_SKULL_BASH",
    "MOVE_FREEZE_SHOCK", "MOVE_ICE_BURN",
}
SELF_LOWERING_MOVES = {
    "MOVE_CLOSE_COMBAT", "MOVE_SUPERPOWER", "MOVE_OVERHEAT", "MOVE_DRACO_METEOR",
    "MOVE_LEAF_STORM", "MOVE_PSYCHO_BOOST", "MOVE_V_CREATE", "MOVE_HAMMER_ARM",
    "MOVE_ICE_HAMMER", "MOVE_SHELL_SMASH", "MOVE_HEADLONG_RUSH", "MOVE_ARMOR_CANNON",
    "MOVE_MAKE_IT_RAIN",
}
# Damage is not why these moves occupy a support set, or their damage ignores
# the user's corresponding Attack stat.  They must not create false nature
# failures in the strategic-coherence audit.
UTILITY_DAMAGE_MOVES = {
    "MOVE_ACID_SPRAY", "MOVE_BODY_PRESS", "MOVE_CLEAR_SMOG", "MOVE_COUNTER",
    "MOVE_ELECTROWEB", "MOVE_ENDEAVOR", "MOVE_FAKE_OUT", "MOVE_FEINT",
    "MOVE_FINAL_GAMBIT", "MOVE_FOUL_PLAY", "MOVE_ICY_WIND", "MOVE_METAL_BURST",
    "MOVE_MIRROR_COAT", "MOVE_NIGHT_SHADE", "MOVE_NUZZLE", "MOVE_RAPID_SPIN",
    "MOVE_RUINATION", "MOVE_SALT_CURE", "MOVE_SEISMIC_TOSS", "MOVE_SNARL",
    "MOVE_SONIC_BOOM", "MOVE_STRUGGLE_BUG", "MOVE_SUPER_FANG",
}


def move_metadata() -> tuple[dict[str, str], set[str]]:
    text = (ROOT / "src/data/moves_info.h").read_text()
    markers = list(re.finditer(r"(?m)^\s*\[(MOVE_[A-Z0-9_]+)\]\s*=\s*\{", text))
    categories: dict[str, str] = {}
    sound_moves: set[str] = set()
    for index, marker in enumerate(markers):
        end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
        body = text[marker.end():end]
        category = re.search(r"\.category\s*=\s*(DAMAGE_CATEGORY_[A-Z]+)", body)
        categories[marker.group(1)] = category.group(1) if category else "DAMAGE_CATEGORY_STATUS"
        if ".soundMove = TRUE" in body:
            sound_moves.add(marker.group(1))
    return categories, sound_moves


def main() -> None:
    manifest = json.loads((ROOT / "docs" / "emerald_champions_battle_sets.json").read_text())
    move_access_review = json.loads(
        (ROOT / "docs/emerald_champions_move_access_review.json").read_text()
    )
    defaults = manifest["defaults"]
    alternatives = manifest["alternatives"]
    entries = defaults + alternatives

    assert manifest["source_commit"] == "0b2bc96c7d6480187f70f5b83a705c081780983e"
    assert manifest["default_count"] == len(defaults) == 1258
    assert manifest["alternative_count"] == len(alternatives) == 276
    assert manifest["set_count"] == len(entries) == 1534
    assert len({entry["species"] for entry in defaults}) == len(defaults)
    assert move_access_review["reviewed_assignment_count"] == 72
    assert len(move_access_review["assignments"]) == 72
    assert move_access_review["provenance_counts"] == {
        "inclement_custom_extension": 41,
        "official_historical_event": 2,
        "official_inherited": 22,
        "replaced": 7,
    }
    reviewed_identities = [
        (row["species"], row["role"], row["move"])
        for row in move_access_review["assignments"]
    ]
    assert len(reviewed_identities) == len(set(reviewed_identities))
    retained_review = {
        identity
        for identity, row in zip(reviewed_identities, move_access_review["assignments"])
        if row["action"] != "replace"
    }
    assert len(retained_review) == 65
    extension_rows = [
        row for row in move_access_review["assignments"]
        if row.get("requires_tutor_extension", False)
    ]
    assert len(extension_rows) == 58
    assert len({(row["teachable_species"], row["move"]) for row in extension_rows}) == 38

    by_species: dict[str, list[dict]] = collections.defaultdict(list)
    for entry in entries:
        by_species[entry["species"]].append(entry)
    assert all(
        any(entry["required_item"] == "ITEM_NONE" for entry in choices)
        for choices in by_species.values()
    ), "a species has no non-Mega wild-eligible preset"
    for species, choices in by_species.items():
        names = [entry["name"] for entry in choices]
        assert len(names) == len(set(names)), (species, "duplicate role names", names)
        orientations = [(
            tuple(entry["moves"]), entry["item"], entry["required_item"],
            entry["nature"], entry["ability"], tuple(entry["stat_points"]),
        ) for entry in choices]
        assert len(orientations) == len(set(orientations)), (
            species, "duplicate complete orientations",
        )

    by_identity = {(entry["species"], entry["name"]): entry for entry in entries}
    for row in move_access_review["assignments"]:
        if row["action"] == "replace":
            current = by_identity[(row["species"], row["replacement_role"])]
            assert row["move"] not in current["moves"], row
            assert row["replacement_move"] in current["moves"], row
            assert row["authoritative_provenance"] == [], row
            continue
        current = by_identity[(row["species"], row["role"])]
        assert row["move"] in current["moves"], row
        assert row["authoritative_provenance"], row
        if row["action"] == "retain_inclement_custom_extension":
            assert row.get("requires_tutor_extension") is True, row
            assert all(
                source["repository"] == "jrci-dev/inclementemerald"
                and source["commit"] == "93e96730fb3c771f7f08f9623ae36f0643c52d3d"
                for source in row["authoritative_provenance"]
            ), row
        else:
            assert all(
                source["repository"] == "smogon/pokemon-showdown"
                and source["commit"] == "bb179fbf8449e3c31632bd56f671ffb4404fa6e7"
                for source in row["authoritative_provenance"]
            ), row

    for entry in entries:
        assert 1 <= len(entry["moves"]) <= 4, entry["species"]
        assert len(entry["moves"]) == len(set(entry["moves"])), entry["species"]
        assert len(entry["name"]) <= 23, entry["name"]
        assert len(entry["stat_points"]) == 6
        assert sum(entry["stat_points"]) == 66, entry["species"]
        assert max(entry["stat_points"]) <= 32, entry["species"]
        assert not entry["item"].endswith("ITE") or entry["item"] == "ITEM_EVIOLITE", entry["item"]

    configured_abilities = configured_species_abilities()
    aliases = species_aliases()
    for entry in entries:
        # Mega presets deliberately name the transformed Ability; ApplyPreset
        # keeps a legal base Ability until the stone transforms the Pokemon.
        if entry["required_item"] != "ITEM_NONE":
            continue
        species = resolve_species(entry["species"], aliases)
        assert entry["ability"] in configured_abilities.get(species, frozenset()), (
            entry["species"],
            entry["ability"],
            sorted(configured_abilities.get(species, frozenset())),
        )

    # Every ladder-backed Mega in the supplied handbook exists in this engine.
    # Its role must therefore use the transformed Ability, remain progression
    # gated, and leave the supplied-item field empty.
    form_changes = (ROOT / "src/data/pokemon/form_change_tables.h").read_text()
    mega_targets: dict[str, set[str]] = collections.defaultdict(set)
    for species, item in re.findall(
        r"FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM,\s*(SPECIES_[A-Z0-9_]+),\s*(ITEM_[A-Z0-9_]+)",
        form_changes,
    ):
        mega_targets[item].add(species)
    mega_entries = [entry for entry in entries if entry["required_item"] != "ITEM_NONE"]
    mega_archive = set(re.findall(
        r"ITEM_[A-Z0-9_]+",
        (ROOT / "src/data/emerald_champions_mega_stones.h").read_text(),
    ))
    assert len(mega_archive) == 92
    assert len(mega_entries) == 95
    assert len({(entry["species"], entry["required_item"]) for entry in mega_entries}) == 95
    assert {entry["required_item"] for entry in mega_entries} == mega_archive
    source_counts = collections.Counter(entry["source"] for entry in mega_entries)
    assert source_counts == {
        "Pokemon Champions doubles handbook: M-B ladder data": 75,
        "Emerald Champions custom Mega extension: current form data and authored doubles corpus": 20,
    }, source_counts
    for entry in mega_entries:
        targets = mega_targets.get(entry["required_item"], set())
        assert targets, (entry["species"], entry["required_item"])
        legal_mega_abilities = set().union(*(
            configured_abilities.get(resolve_species(target, aliases), frozenset())
            for target in targets
        ))
        assert entry["ability"] in legal_mega_abilities, (
            entry["species"], entry["required_item"], entry["ability"],
            sorted(legal_mega_abilities),
        )
        assert entry["item"] == "ITEM_NONE", entry
        assert entry["name"].startswith("Mega "), entry

    mega_identities = {
        (entry["species"], entry["required_item"])
        for entry in mega_entries
    }
    assert {
        ("SPECIES_MAGEARNA", "ITEM_MAGEARNITE"),
        ("SPECIES_MAGEARNA_ORIGINAL", "ITEM_MAGEARNITE"),
    } <= mega_identities
    assert ("SPECIES_ZYGARDE", "ITEM_ZYGARDITE") not in mega_identities
    assert ("SPECIES_ZYGARDE_50_POWER_CONSTRUCT", "ITEM_ZYGARDITE") in mega_identities
    assert (
        "FORM_CHANGE_BATTLE_HP_PERCENT_TURN_END, SPECIES_ZYGARDE_COMPLETE, "
        "ABILITY_POWER_CONSTRUCT, HP_LOWER_EQ_THAN, 50"
    ) in form_changes
    assert (
        "FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM,    SPECIES_ZYGARDE_MEGA, ITEM_ZYGARDITE"
    ) in form_changes

    evolution_items = set(re.findall(
        r"ITEM_[A-Z0-9_]+",
        (ROOT / "src/data/emerald_champions_evolution_items.h").read_text(),
    ))
    assert len(evolution_items) == 44
    assert not evolution_items.intersection(entry["item"] for entry in entries)
    assert not evolution_items.intersection(entry["required_item"] for entry in entries)

    categories, sound_moves = move_metadata()
    failures: list[str] = []
    for entry in entries:
        tag = f"{entry['species']}/{entry['name']}"
        moves = set(entry["moves"])
        status_moves = {
            move for move in moves
            if categories.get(move, "DAMAGE_CATEGORY_STATUS") == "DAMAGE_CATEGORY_STATUS"
        }
        physical = {
            move for move in moves
            if categories.get(move) == "DAMAGE_CATEGORY_PHYSICAL"
            and move not in UTILITY_DAMAGE_MOVES
        }
        special = {
            move for move in moves
            if categories.get(move) == "DAMAGE_CATEGORY_SPECIAL"
            and move not in UTILITY_DAMAGE_MOVES
        }
        item = entry["item"]

        if physical and not special and entry["nature"] in LOWERS_ATTACK:
            failures.append(f"{tag}: nature lowers its only meaningful attack category")
        if special and not physical and entry["nature"] in LOWERS_SP_ATTACK:
            failures.append(f"{tag}: nature lowers its only meaningful attack category")
        if "WALLBREAKER" in entry["role"].upper() and not (
            entry["stat_points"][1] == 32 or entry["stat_points"][3] == 32
        ):
            failures.append(f"{tag}: wallbreaker has no offensive Stat Point maximum")
        if item in LOCKED_ITEMS and status_moves:
            if not (
                item.startswith("ITEM_CHOICE_")
                and status_moves <= CHOICE_COMPATIBLE_STATUS
            ):
                failures.append(f"{tag}: {item} cannot use {sorted(status_moves)}")
        if item == "ITEM_LIGHT_CLAY" and not moves & SCREEN_MOVES:
            failures.append(f"{tag}: Light Clay has no screen")
        if item == "ITEM_POWER_HERB" and not moves & CHARGE_MOVES:
            failures.append(f"{tag}: Power Herb has no charge move")
        if item == "ITEM_EJECT_PACK" and not moves & SELF_LOWERING_MOVES:
            failures.append(f"{tag}: Eject Pack has no self-lowering move")
        if item == "ITEM_THROAT_SPRAY" and not moves & sound_moves:
            failures.append(f"{tag}: Throat Spray has no sound move")
        if entry["ability"] == "ABILITY_POISON_HEAL" and item != "ITEM_TOXIC_ORB":
            failures.append(f"{tag}: Poison Heal has no Toxic Orb")
        if entry["ability"] == "ABILITY_FLARE_BOOST" and item != "ITEM_FLAME_ORB":
            failures.append(f"{tag}: Flare Boost has no Flame Orb")
        if entry["ability"] == "ABILITY_HARVEST" and not item.endswith("_BERRY"):
            failures.append(f"{tag}: Harvest has no Berry")
    assert not failures, "battle-set coherence failures:\n" + "\n".join(failures)

    # These two formerly inert White Herbs were individually reviewed. White
    # Herb itself is not globally rejected without a self-drop because it can
    # be deliberate anti-Intimidate/speed-control tech in doubles.
    by_identity = {(entry["species"], entry["name"]): entry for entry in entries}
    assert by_identity[("SPECIES_GROUDON", "Recommended")]["item"] == "ITEM_CLEAR_AMULET"
    assert "MOVE_SHELL_SMASH" in by_identity[("SPECIES_DREDNAW", "Shell Smash")]["moves"]

    wild_text = (ROOT / "src" / "data" / "wild_encounters.json").read_text()
    wild_species = set(re.findall(r"SPECIES_[A-Z0-9_]+", wild_text))
    default_species = {entry["species"] for entry in defaults}
    wild_species_for_sets = {
        "SPECIES_GIMMIGHOUL" if species == "SPECIES_GIMMIGHOUL_CHEST" else species
        for species in wild_species
    }
    missing = sorted(wild_species_for_sets - default_species)
    assert not missing, f"Current wild tables lack presets: {missing}"

    generated = (ROOT / "src" / "data" / "pokemon" / "emerald_champions_battle_sets.h").read_text()
    assert generated.count(".statPoints =") == 1534
    assert "gEmeraldChampionsDefaultBattleSets[NUM_SPECIES]" in generated
    assert "gEmeraldChampionsBattleSetAlternatives[]" in generated
    reviewed_header = (
        ROOT / "src/data/pokemon/emerald_champions_move_access_review.h"
    ).read_text()
    assert reviewed_header.count("{SPECIES_") == 65
    tutor_generator = (ROOT / "tools/learnset_helpers/make_teachables.py").read_text()
    makefile = (ROOT / "Makefile").read_text()
    assert "emerald_champions_move_access_review.json" in tutor_generator
    assert "MOVE_ACCESS_REVIEW_JSON" in makefile

    starter_species = {
        "SPECIES_GROOKEY", "SPECIES_SCORBUNNY", "SPECIES_SOBBLE",
        "SPECIES_SPRIGATITO", "SPECIES_FUECOCO", "SPECIES_QUAXLY",
        "SPECIES_THWACKEY", "SPECIES_RABOOT", "SPECIES_DRIZZILE",
        "SPECIES_FLORAGATO", "SPECIES_CROCALOR", "SPECIES_QUAXWELL",
        "SPECIES_RILLABOOM", "SPECIES_CINDERACE", "SPECIES_INTELEON",
        "SPECIES_MEOWSCARADA", "SPECIES_SKELEDIRGE", "SPECIES_QUAQUAVAL",
    }
    assert starter_species <= default_species, f"Gen 8/9 starter presets missing: {sorted(starter_species - default_species)}"

    sign_text = (ROOT / "src" / "data" / "pokemon" / "legendary_signs.h").read_text()
    sign_species = {
        "SPECIES_" + species
        for species in re.findall(r"(?:WILD|VISIBLE|OTHER)_SIGN\([^,]+,\s*([A-Z0-9_]+)", sign_text)
    }
    assert sign_species <= default_species, f"Legendary roots lack presets: {sorted(sign_species - default_species)}"

    modern_campaign_species = {
        "SPECIES_BASCULIN_WHITE_STRIPED", "SPECIES_CAPSAKID", "SPECIES_CHARCADET",
        "SPECIES_FLITTLE", "SPECIES_GIMMIGHOUL", "SPECIES_GREAVARD",
        "SPECIES_GROWLITHE_HISUI", "SPECIES_ORTHWORM", "SPECIES_POLTCHAGEIST",
        "SPECIES_QWILFISH_HISUI", "SPECIES_SNEASEL_HISUI", "SPECIES_TADBULB",
        "SPECIES_TANDEMAUS", "SPECIES_TAUROS_PALDEA_AQUA",
        "SPECIES_TAUROS_PALDEA_BLAZE", "SPECIES_TAUROS_PALDEA_COMBAT",
        "SPECIES_TINKATINK", "SPECIES_ZORUA_HISUI",
    }
    set_counts = {
        species: sum(entry["species"] == species for entry in entries)
        for species in modern_campaign_species
    }
    assert all(count >= 2 for count in set_counts.values()), set_counts

    pinned_payload = json.loads(
        (ROOT / "docs" / "showdown_champions_learnsets.json").read_text()
    )
    pinned = pinned_payload["learnsets"]
    assert pinned_payload["source_commit"] == "bb179fbf8449e3c31632bd56f671ffb4404fa6e7"
    assert "pre-evolution chains" in pinned_payload["policy"]
    showdown_ids = {
        "SPECIES_GIMMIGHOUL": "gimmighoul",
        **{
            species: re.sub(r"[^a-z0-9]", "", species.removeprefix("SPECIES_").lower())
            for species in modern_campaign_species
            if species != "SPECIES_GIMMIGHOUL"
        },
    }
    move_ids = {
        move: re.sub(r"[^a-z0-9]", "", move.removeprefix("MOVE_").lower())
        for move in set(re.findall(r"MOVE_[A-Z0-9_]+", (ROOT / "include/constants/moves.h").read_text()))
    }
    for entry in entries:
        if entry["species"] not in modern_campaign_species:
            continue
        legal = set(pinned[showdown_ids[entry["species"]]])
        illegal = {move for move in entry["moves"] if move_ids[move] not in legal}
        assert not illegal, (entry["species"], sorted(illegal))

    # Reproduce the former direct-form check and fail on any gap not present in
    # the exact reviewed manifest. Four reviewed HJK/Sticky Web rows now resolve
    # directly because the pinned projection inherits pre-evolution learnsets.
    species_text = preprocess_species_info()
    species_text = species_text[species_text.find("const struct SpeciesInfo gSpeciesInfo[]"):]
    species_markers = list(SPECIES_MARKER.finditer(species_text))
    pointer_keys: dict[str, str] = {}
    for index, marker in enumerate(species_markers):
        end = species_markers[index + 1].start() if index + 1 < len(species_markers) else len(species_text)
        block = species_text[marker.start():end]
        match = re.search(
            r"\.teachableLearnset\s*=\s*s([A-Za-z0-9]+)TeachableLearnset",
            block,
        )
        if match:
            pointer_keys[marker.group(1)] = re.sub(
                r"([a-z0-9])([A-Z])", r"\1_\2", match.group(1)
            ).upper()
    all_learnables = json.loads(
        (ROOT / "src/data/pokemon/all_learnables.json").read_text()
    )
    direct_gaps: set[tuple[str, str, str]] = set()
    for entry in entries:
        keys = {
            entry["species"].removeprefix("SPECIES_"),
            pointer_keys.get(entry["species"], ""),
        }
        configured = set().union(*(set(all_learnables.get(key, [])) for key in keys))
        showdown_id = re.sub(
            r"[^a-z0-9]", "", entry["species"].removeprefix("SPECIES_").lower()
        )
        projected = set(pinned.get(showdown_id, []))
        for move in entry["moves"]:
            move_id = re.sub(r"[^a-z0-9]", "", move.removeprefix("MOVE_").lower())
            if (
                entry["species"] != "SPECIES_SMEARGLE"
                and move not in configured
                and move_id not in projected
            ):
                direct_gaps.add((entry["species"], entry["name"], move))
    assert not direct_gaps.difference(retained_review), sorted(
        direct_gaps.difference(retained_review)
    )
    assert retained_review.difference(direct_gaps) == {
        ("SPECIES_RABOOT", "Recommended", "MOVE_HIGH_JUMP_KICK"),
        ("SPECIES_CINDERACE", "Choice Attacker", "MOVE_HIGH_JUMP_KICK"),
        ("SPECIES_CINDERACE", "Offensive", "MOVE_HIGH_JUMP_KICK"),
        ("SPECIES_LEAVANNY", "Offensive", "MOVE_STICKY_WEB"),
    }

    runtime = (ROOT / "src" / "emerald_champions_battle_sets.c").read_text()
    wild_function = runtime.split("u8 ApplyEmeraldChampionsRandomWildSet", 1)[1].split(
        "u8 ApplyEmeraldChampionsRandomNonMegaSet", 1
    )[0]
    assert "ApplyEmeraldChampionsRandomNonMegaSet(mon)" in wild_function
    assert "ResolveVisibleChoice" not in wild_function
    non_mega_function = runtime.split("u8 ApplyEmeraldChampionsRandomNonMegaSet", 1)[1].split(
        "u8 ApplyEmeraldChampionsOpponentSet", 1
    )[0]
    assert "preset->requiredItem != ITEM_NONE" in non_mega_function
    assert "RandomUniform(RNG_NONE, 0, ++matches - 1)" in non_mega_function
    assert "ResolveBattleSetSpecies" in runtime and "formSpeciesIdTable" in runtime
    assert '#include "data/emerald_champions_evolution_items.h"' in runtime
    assert "ARRAY_COUNT(sEmeraldChampionsEvolutionItems)" in runtime
    ordinary_wild_gate = runtime.split("bool32 IsEmeraldChampionsOrdinaryWildSpecies", 1)[1].split("static bool32 FindAbilitySlot", 1)[0]
    assert ".isUltraBeast" not in ordinary_wild_gate, "random-table Ultra Beasts bypass competitive presets"
    assert ".isParadox" not in ordinary_wild_gate, "random-table Paradox Pokemon bypass competitive presets"

    handbook_species = {
        entry["species"] for entry in defaults
        if entry["source"].startswith("Pokemon Champions doubles handbook")
    }
    assert len(handbook_species) == 80, len(handbook_species)

    print("battle_set_static_checks=PASS")
    print(f"sets={len(entries)}")
    print(f"mega_roles={len(mega_entries)} across {len(mega_archive)} stones")
    print("move_access_review=65 retained + 7 replaced; 38 unique tutor extensions")
    print(f"wild_species_with_presets={len(wild_species)}")
    print(f"new_campaign_species_with_two_sets={len(modern_campaign_species)}")


if __name__ == "__main__":
    main()
