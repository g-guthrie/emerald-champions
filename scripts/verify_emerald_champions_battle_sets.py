#!/usr/bin/env python3
"""Verify generated battle-set data and current wild-table coverage."""

from __future__ import annotations

import json
import collections
import itertools
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
SYNTHETIC_SETUP_MOVES = {
    "MOVE_BULK_UP", "MOVE_CALM_MIND", "MOVE_COIL", "MOVE_DRAGON_DANCE",
    "MOVE_GEOMANCY", "MOVE_HONE_CLAWS", "MOVE_HOWL", "MOVE_NASTY_PLOT",
    "MOVE_QUIVER_DANCE", "MOVE_SHELL_SMASH", "MOVE_SHIFT_GEAR",
    "MOVE_SWORDS_DANCE", "MOVE_TAIL_GLOW", "MOVE_TIDY_UP",
    "MOVE_VICTORY_DANCE",
}
UNBURDEN_ITEMS = {
    "ITEM_ELECTRIC_SEED", "ITEM_FOCUS_SASH", "ITEM_GRASSY_SEED",
    "ITEM_MISTY_SEED", "ITEM_POWER_HERB", "ITEM_PSYCHIC_SEED",
    "ITEM_SITRUS_BERRY", "ITEM_WEAKNESS_POLICY", "ITEM_WHITE_HERB",
}
TRIAGE_ATTACKS = {
    "MOVE_DRAIN_PUNCH", "MOVE_DRAINING_KISS", "MOVE_GIGA_DRAIN",
    "MOVE_HORN_LEECH", "MOVE_LEECH_LIFE", "MOVE_PARABOLIC_CHARGE",
}


def move_metadata() -> tuple[dict[str, str], set[str], set[str]]:
    text = (ROOT / "src/data/moves_info.h").read_text()
    markers = list(re.finditer(r"(?m)^\s*\[(MOVE_[A-Z0-9_]+)\]\s*=\s*\{", text))
    categories: dict[str, str] = {}
    sound_moves: set[str] = set()
    punching_moves: set[str] = set()
    for index, marker in enumerate(markers):
        end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
        body = text[marker.end():end]
        category = re.search(r"\.category\s*=\s*(DAMAGE_CATEGORY_[A-Z]+)", body)
        categories[marker.group(1)] = category.group(1) if category else "DAMAGE_CATEGORY_STATUS"
        if ".soundMove = TRUE" in body:
            sound_moves.add(marker.group(1))
        if ".punchingMove = TRUE" in body:
            punching_moves.add(marker.group(1))
    return categories, sound_moves, punching_moves


def main() -> None:
    manifest = json.loads((ROOT / "docs" / "emerald_champions_battle_sets.json").read_text())
    move_access_review = json.loads(
        (ROOT / "docs/emerald_champions_move_access_review.json").read_text()
    )
    defaults = manifest["defaults"]
    alternatives = manifest["alternatives"]
    singles_defaults = manifest["singles_defaults"]
    singles_alternatives = manifest["singles_alternatives"]
    doubles_entries = defaults + alternatives
    singles_entries = singles_defaults + singles_alternatives
    entries = doubles_entries + singles_entries

    assert manifest["source_commit"] == "0b2bc96c7d6480187f70f5b83a705c081780983e"
    assert manifest["default_count"] == len(defaults) == 1258
    assert manifest["alternative_count"] == len(alternatives) == 1361
    assert manifest["singles_default_count"] == len(singles_defaults) == 1258
    assert manifest["singles_alternative_count"] == len(singles_alternatives) == 1434
    assert manifest["set_count"] == len(entries) == 5311
    assert len({entry["species"] for entry in defaults}) == len(defaults)
    assert len({entry["species"] for entry in singles_defaults}) == len(singles_defaults)
    assert all(entry["required_item"] == "ITEM_NONE" for entry in defaults), (
        "a Doubles wild/evolution default is gated behind a progression item",
        [(entry["species"], entry["name"], entry["required_item"])
         for entry in defaults if entry["required_item"] != "ITEM_NONE"],
    )
    scovillain_default = next(entry for entry in defaults if entry["species"] == "SPECIES_SCOVILLAIN")
    assert scovillain_default["name"] == "Rage Powder Support"
    assert scovillain_default["required_item"] == "ITEM_NONE"
    assert any(
        entry["species"] == "SPECIES_SCOVILLAIN"
        and entry["required_item"] == "ITEM_SCOVILLAINITE"
        for entry in alternatives
    ), "Scovillain's Mega role disappeared instead of becoming a gated alternative"
    showdown_singles = json.loads(
        (ROOT / "docs/showdown_champions_random_singles.json").read_text()
    )
    assert showdown_singles["source_commit"] == "bb179fbf8449e3c31632bd56f671ffb4404fa6e7"
    assert showdown_singles["source_sha256"] == "7b189d6de33367aca7191e484069b74757097fc34fed0402b52bb6fa41447421"
    showdown_gen9_singles = json.loads(
        (ROOT / "docs/showdown_gen9_random_singles.json").read_text()
    )
    assert showdown_gen9_singles["source_commit"] == showdown_singles["source_commit"]
    assert showdown_gen9_singles["source_sha256"] == "d18992314222060dda9a2a9bea09331478991d469babd95662517668099669f9"
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
    retained_move_pairs = {
        (species, move)
        for species, _, move in retained_review
    }
    assert len(retained_review) == 65
    extension_rows = [
        row for row in move_access_review["assignments"]
        if row.get("requires_tutor_extension", False)
    ]
    assert len(extension_rows) == 58
    assert len({(row["teachable_species"], row["move"]) for row in extension_rows}) == 38

    by_species: dict[str, list[dict]] = collections.defaultdict(list)
    for entry in doubles_entries:
        by_species[entry["species"]].append(entry)
    assert all(
        sum(entry["required_item"] == "ITEM_NONE" for entry in choices) >= 2
        for choices in by_species.values()
    ), "a direct species/form has fewer than two pre-Mega orientations"
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
        non_mega = [entry for entry in choices if entry["required_item"] == "ITEM_NONE"]
        for first, second in itertools.combinations(non_mega, 2):
            assert (
                frozenset(first["moves"]) != frozenset(second["moves"])
                or first["ability"] != second["ability"]
                or first["nature"] != second["nature"]
                or tuple(first["stat_points"]) != tuple(second["stat_points"])
            ), (
                species,
                "superficial move-order/item-only duplicate",
                first["name"],
                second["name"],
            )

    singles_by_species: dict[str, list[dict]] = collections.defaultdict(list)
    for entry in singles_entries:
        singles_by_species[entry["species"]].append(entry)
    assert set(singles_by_species) == set(by_species)
    for species, choices in singles_by_species.items():
        names = [entry["name"] for entry in choices]
        assert len(names) == len(set(names)), (species, "duplicate Singles names", names)
        assert sum(entry["required_item"] == "ITEM_NONE" for entry in choices) >= 2

    synthesized = [
        entry for entry in alternatives
        if entry["source"].startswith("Emerald Champions legal doubles role synthesis")
    ]
    assert len(synthesized) == 1086

    by_identity = {(entry["species"], entry["name"]): entry for entry in doubles_entries}
    default_by_species = {entry["species"]: entry for entry in defaults}
    for row in move_access_review["assignments"]:
        if row["action"] == "replace":
            current = (
                default_by_species[row["species"]]
                if row["replacement_role"] == "Recommended"
                else by_identity[(row["species"], row["replacement_role"])]
            )
            assert row["move"] not in current["moves"], row
            assert row["replacement_move"] in current["moves"], row
            assert row["authoritative_provenance"] == [], row
            continue
        current = (
            default_by_species[row["species"]]
            if row["role"] == "Recommended"
            else by_identity[(row["species"], row["role"])]
        )
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
        expected_moves = {"SPECIES_DITTO": 1, "SPECIES_UNOWN": 2}.get(entry["species"], 4)
        assert len(entry["moves"]) == expected_moves, (entry["species"], entry["name"])
        assert len(entry["moves"]) == len(set(entry["moves"])), entry["species"]
        assert len(entry["name"]) <= 23, entry["name"]
        assert entry["name"] != "Recommended", entry["species"]
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
    mega_entries = [entry for entry in doubles_entries if entry["required_item"] != "ITEM_NONE"]
    singles_mega_entries = [entry for entry in singles_entries if entry["required_item"] != "ITEM_NONE"]
    mega_archive = set(re.findall(
        r"ITEM_[A-Z0-9_]+",
        (ROOT / "src/data/emerald_champions_mega_stones.h").read_text(),
    ))
    assert len(mega_archive) == 92
    assert len(mega_entries) == 95
    assert len({(entry["species"], entry["required_item"]) for entry in mega_entries}) == 95
    assert {entry["required_item"] for entry in mega_entries} == mega_archive
    assert len(singles_mega_entries) == 125
    assert len({(entry["species"], entry["required_item"]) for entry in singles_mega_entries}) == 95
    assert {entry["required_item"] for entry in singles_mega_entries} == mega_archive
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
    for entry in singles_mega_entries:
        targets = mega_targets.get(entry["required_item"], set())
        legal_mega_abilities = set().union(*(
            configured_abilities.get(resolve_species(target, aliases), frozenset())
            for target in targets
        ))
        assert entry["ability"] in legal_mega_abilities, entry
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

    categories, sound_moves, punching_moves = move_metadata()
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
        if entry["ability"] in {"ABILITY_BLITZ_BOXER", "ABILITY_IRON_FIST"} and not moves & punching_moves:
            failures.append(f"{tag}: punching Ability has no punching move")
        if entry["ability"] == "ABILITY_HARVEST" and not item.endswith("_BERRY"):
            failures.append(f"{tag}: Harvest has no Berry")
        if entry["source"].startswith("Emerald Champions legal doubles role synthesis"):
            if entry["ability"] == "ABILITY_CONTRARY" and moves & SYNTHETIC_SETUP_MOVES:
                failures.append(f"{tag}: Contrary reverses its synthesized setup move")
            if entry["ability"] == "ABILITY_GORILLA_TACTICS" and status_moves:
                failures.append(f"{tag}: Gorilla Tactics synthesized a status-locked role")
            if entry["ability"] == "ABILITY_GUTS" and item != "ITEM_FLAME_ORB":
                failures.append(f"{tag}: synthesized Guts role has no Flame Orb")
            if entry["ability"] in {"ABILITY_QUICK_FEET", "ABILITY_TOXIC_BOOST"} and item != "ITEM_TOXIC_ORB":
                failures.append(f"{tag}: synthesized status Ability has no Toxic Orb")
            if entry["ability"] == "ABILITY_UNBURDEN" and item not in UNBURDEN_ITEMS:
                failures.append(f"{tag}: synthesized Unburden role cannot consume its item")
            if entry["ability"] == "ABILITY_TRIAGE" and not moves & TRIAGE_ATTACKS:
                failures.append(f"{tag}: synthesized Triage role has no priority healing attack")
    assert not failures, "battle-set coherence failures:\n" + "\n".join(failures)

    # These two formerly inert White Herbs were individually reviewed. White
    # Herb itself is not globally rejected without a self-drop because it can
    # be deliberate anti-Intimidate/speed-control tech in doubles.
    by_identity = {(entry["species"], entry["name"]): entry for entry in doubles_entries}
    assert default_by_species["SPECIES_GROUDON"]["item"] == "ITEM_CLEAR_AMULET"
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
    assert generated.count(".statPoints =") == 5311
    assert "gEmeraldChampionsDefaultBattleSets[NUM_SPECIES]" in generated
    assert "gEmeraldChampionsBattleSetAlternatives[]" in generated
    assert "gEmeraldChampionsSinglesDefaultBattleSets[NUM_SPECIES]" in generated
    assert "gEmeraldChampionsSinglesBattleSetAlternatives[]" in generated
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
    preparation_source = {species: list(moves) for species, moves in all_learnables.items()}
    for row in extension_rows:
        moves = preparation_source[row["teachable_species"]]
        if row["move"] not in moves:
            moves.append(row["move"])
    preparation: dict[str, list[str]] = {}
    for species, source_moves in preparation_source.items():
        species = {
            "FLABÃ©BÃ©": "FLABEBE",
            "MEOWSTIC": "MEOWSTIC_M",
            "HOOPA": "HOOPA_CONFINED",
            "TOXTRICITY": "TOXTRICITY_AMPED",
            "INDEEDEE": "INDEEDEE_M",
            "OINKOLOGNE": "OINKOLOGNE_M",
        }.get(species, species)
        species_key = re.sub(r"[^A-Z0-9]+", "_", species.replace("'", "").replace(".", "")).strip("_")
        moves = preparation.setdefault(species_key, [])
        moves.extend(move for move in source_moves if move not in moves)
    preparation_counts = {species: len(moves) for species, moves in preparation.items()}
    assert len(preparation) == 1108
    assert sum(preparation_counts.values()) == 88073
    assert max(preparation_counts.items(), key=lambda entry: entry[1]) == ("MEW", 372)
    preparation_rom_data_bytes = 2 * (sum(preparation_counts.values()) + len(preparation)) + 4 * 1573
    assert preparation_rom_data_bytes == 184654
    assert preparation_rom_data_bytes < 200000
    assert max(preparation_counts.values()) + 1 < 4096
    assert len(preparation["MEW"]) == len(set(preparation["MEW"]))
    assert {"MOVE_TAILWIND", "MOVE_WILL_O_WISP"} <= set(preparation["MEW"])
    move_constants = set(re.findall(r"\bMOVE_[A-Z0-9_]+\b", (ROOT / "include/constants/moves.h").read_text()))
    species_constants = set(re.findall(r"\bSPECIES_[A-Z0-9_]+\b", (ROOT / "include/constants/species.h").read_text()))
    assert not set().union(*(set(moves) for moves in preparation.values())).difference(move_constants)
    assert not {f"SPECIES_{species}" for species in preparation}.difference(species_constants)

    tm_moves = re.findall(r"F\(([A-Z0-9_]+)\)", (ROOT / "include/constants/tms_hms.h").read_text())
    tutor_moves: set[str] = set()
    tutor_sources = list((ROOT / "data/scripts").glob("*.inc"))
    tutor_sources.extend((ROOT / "data/maps").glob("*/scripts.inc"))
    for path in tutor_sources:
        source = path.read_text()
        if "special ChooseMonForMoveTutor" not in source and "chooseboxmon SELECT_PC_MON_MOVE_TUTOR" not in source:
            continue
        tutor_moves.update(re.findall(r"setvar VAR_0x8005, (MOVE_[A-Z0-9_]+)", source))
        tutor_moves.update(re.findall(r"move_tutor (MOVE_[A-Z0-9_]+)", source))
    tutor_moves.update(row["move"] for row in extension_rows)
    special_movesets = json.loads((ROOT / "src/data/pokemon/special_movesets.json").read_text())
    signature_moves = set(special_movesets["signatureTeachables"])
    current_mew_teachables = {
        *(f"MOVE_{move}" for move in tm_moves),
        *tutor_moves,
        *special_movesets["extraTutors"],
    }.difference(signature_moves)
    mew_level_block = (ROOT / "src/data/pokemon/level_up_learnsets/gen_9.h").read_text().split(
        "static const struct LevelUpMove sMewLevelUpLearnset[]", 1
    )[1].split("LEVEL_UP_END", 1)[0]
    mew_level_moves = set(re.findall(r"\bMOVE_[A-Z0-9_]+\b", mew_level_block))
    assert len(current_mew_teachables) == 110
    assert len(current_mew_teachables | mew_level_moves) == 119
    assert {"MOVE_TAILWIND", "MOVE_WILL_O_WISP"}.isdisjoint(current_mew_teachables)

    preparation_generator = (ROOT / "tools/learnset_helpers/make_teachables.py").read_text()
    makefile = (ROOT / "Makefile").read_text()
    move_relearner = (ROOT / "src/move_relearner.c").read_text()
    preparation_population = move_relearner.rsplit(
        "u32 GetEmeraldChampionsPreparationMovesToLearn", 1
    )[1].split("void Special_HasMoveToRelearn", 1)[0]
    preparation_has_check = move_relearner.rsplit(
        "static bool32 HasRelearnerAllMoves", 1
    )[1].split("static bool32 IsLevelUpMoveRelearnerActive", 1)[0]
    assert "--preparation" in preparation_generator
    assert "emerald_champions_preparation_learnsets.h" in preparation_generator
    assert "EC_PREPARATION_LEARNSETS" in makefile and "--preparation" in makefile
    assert '#include "data/pokemon/emerald_champions_preparation_learnsets.h"' in move_relearner
    assert "GetEmeraldChampionsPreparationMoves(species)" in preparation_population
    assert "CanLearnTeachableMove" not in preparation_population
    assert "GetSpeciesLevelUpLearnset" not in preparation_population
    assert "GetEmeraldChampionsPreparationMoves(species)" in preparation_has_check
    assert "u16 numMenuChoices;" in move_relearner
    assert "#define MAX_RELEARNER_MOVES MOVES_COUNT_ALL" in (
        ROOT / "include/constants/move_relearner.h"
    ).read_text()
    assert "u32 totalItems:12;" in (ROOT / "include/list_menu.h").read_text()
    ordinary_random = (ROOT / "src/random_mon_generation.c").read_text()
    assert "GetSpeciesTeachableLearnset" in ordinary_random
    assert "EmeraldChampionsPreparation" not in ordinary_random
    pokemon_source = (ROOT / "src/pokemon.c").read_text()
    can_teach = pokemon_source.split("bool32 CanLearnTeachableMove", 1)[1].split("u16 SpeciesToPokedexNum", 1)[0]
    assert "GetSpeciesTeachableLearnset(species)" in can_teach
    assert "EmeraldChampionsPreparation" not in can_teach
    runtime_tests = (ROOT / "test/emerald_champions.c").read_text()
    assert "canonicalCount, 372" in runtime_tests
    assert "MOVE_TAILWIND" in runtime_tests and "MOVE_WILL_O_WISP" in runtime_tests
    assert "GetEmeraldChampionsPreparationMovesToLearn" in runtime_tests

    direct_gaps: set[tuple[str, str, str]] = set()
    reviewed_recommended_species = {
        row["species"]
        for row in move_access_review["assignments"]
        if row["role"] == "Recommended"
    }
    for entry in entries:
        resolved_species = resolve_species(entry["species"], aliases)
        keys = {
            entry["species"].removeprefix("SPECIES_"),
            resolved_species.removeprefix("SPECIES_"),
            pointer_keys.get(entry["species"], ""),
            pointer_keys.get(resolved_species, ""),
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
                review_name = (
                    "Recommended"
                    if entry in defaults and entry["species"] in reviewed_recommended_species
                    else entry["name"]
                )
                direct_gaps.add((entry["species"], review_name, move))
    unreviewed_gaps = {
        gap for gap in direct_gaps
        if gap not in retained_review and (gap[0], gap[2]) not in retained_move_pairs
    }
    assert not unreviewed_gaps, sorted(unreviewed_gaps)
    # These rows are now directly reachable through inherited or shared-form
    # learnset pointers; retaining the explicit extension is harmless and
    # preserves the pinned migration disposition.
    expected_resolved_review = {
        ("SPECIES_ALCREMIE_SALTED_CREAM", "Recommended", "MOVE_DAZZLING_GLEAM"),
        ("SPECIES_ALCREMIE_SALTED_CREAM", "Recommended", "MOVE_DECORATE"),
        ("SPECIES_ALCREMIE_SALTED_CREAM", "Recommended", "MOVE_HELPING_HAND"),
        ("SPECIES_ALCREMIE_SALTED_CREAM", "Recommended", "MOVE_PROTECT"),
        ("SPECIES_RABOOT", "Recommended", "MOVE_HIGH_JUMP_KICK"),
        ("SPECIES_CINDERACE", "Choice Attacker", "MOVE_HIGH_JUMP_KICK"),
        ("SPECIES_CINDERACE", "Offensive", "MOVE_HIGH_JUMP_KICK"),
        ("SPECIES_LEAVANNY", "Offensive", "MOVE_STICKY_WEB"),
    }
    resolved_review = retained_review.difference(direct_gaps)
    assert resolved_review == expected_resolved_review, (
        sorted(resolved_review), sorted(expected_resolved_review)
    )

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
    evolution = (ROOT / "src/evolution_scene.c").read_text()
    assert evolution.count("ApplyEmeraldChampionsRecommendedEvolutionSet") == 3
    assert "CreateShedinja" in evolution
    assert "preserveProtectedItemInPlace" in runtime
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
    print(f"doubles_non_mega_orientations={len(doubles_entries) - len(mega_entries)}; minimum=2 for {len(defaults)} species/forms")
    print(f"singles_sets={len(singles_entries)}; minimum=2 non-Mega roles for {len(singles_defaults)} species/forms")
    print(f"legal_role_syntheses={len(synthesized)}")
    print(f"mega_roles={len(mega_entries)} across {len(mega_archive)} stones")
    print("move_access_review=65 retained + 7 replaced; 38 unique tutor extensions")
    print(f"wild_species_with_presets={len(wild_species)}")
    print(f"new_campaign_species_with_two_sets={len(modern_campaign_species)}")


if __name__ == "__main__":
    main()
