#!/usr/bin/env python3

"""
Usage: python3 make_teachables.py SOURCE_DIR
       python3 make_teachables.py --tutors SOURCE_DIR
       python3 make_teachables.py --preparation

Build a C-header defining the set of teachable moves for each configured-on
species-family based on the learnable moves defined in SOURCE_DIR/all_learnables.json.

A move is "teachable" if it is:
    1. Can be taught by some Move Tutor in the overworld, which is identified by
       using the ChooseMonForMoveTutor special in a script and setting VAR_0x8005
       to the offered MOVE constant. (e.g., MOVE_SWAGGER)
    2. Assigned to some TM or HM in include/constants/tms_hms.h using the
       FOREACH_TM macro.
    3. A universal move, as defined by universalMoves in
       src/data/pokemon/special_movesets.json

For a given species, a move is considered teachable to that species if:
    1. The species can learn the move via *any* method within any Expansion-
       supported game.
    2. The species rule defined by their teachingType in the species_info folder
"""

from itertools import chain
from textwrap import dedent

import json
import pathlib
import re
import sys
import typing


CONFIG_ENABLED_PAT = re.compile(r"^#define P_LEARNSET_HELPER_TEACHABLE\s+(?P<cfg_val>[^ ]*)", flags=re.MULTILINE)
ALPHABETICAL_ORDER_ENABLED_PAT = re.compile(r"^#define HGSS_SORT_TMS_BY_NUM\s+(?P<cfg_val>[^ ]*)", flags=re.MULTILINE)
TM_LITERACY_PAT = re.compile(r"^#define P_TM_LITERACY\s+GEN_(?P<cfg_val>[^ ]*)", flags=re.MULTILINE)
TMHM_MACRO_PAT = re.compile(r"F\((\w+)\)")
SNAKIFY_PAT = re.compile(r"(?!^)([A-Z]+)")
MOVE_CONST_PAT = re.compile(r"^MOVE_[A-Z0-9_]+$")
PREPARATION_OUTPUT = pathlib.Path("./src/data/pokemon/emerald_champions_preparation_learnsets.h")
PREPARATION_FORM_LEARNSETS = pathlib.Path("./data/emerald_champions/emerald_champions_preparation_form_learnsets.json")
PREPARATION_SPECIES_ALIASES = {
    "FLABÃ©BÃ©": "FLABEBE",
    "MEOWSTIC": "MEOWSTIC_M",
    "HOOPA": "HOOPA_CONFINED",
    "TOXTRICITY": "TOXTRICITY_AMPED",
    "INDEEDEE": "INDEEDEE_M",
    "OINKOLOGNE": "OINKOLOGNE_M",
}

def preparation_species_key(species: str) -> str:
    species = PREPARATION_SPECIES_ALIASES.get(species, species)
    return re.sub(r"[^A-Z0-9]+", "_", species.replace("'", "").replace(".", "")).strip("_")

def enabled() -> bool:
    """
    Check if the user has explicitly enabled this opt-in helper.
    """
    with open("./include/config/pokemon.h", "r") as cfg_pokemon_fp:
        cfg_pokemon = cfg_pokemon_fp.read()
        cfg_defined = CONFIG_ENABLED_PAT.search(cfg_pokemon)
        return cfg_defined is not None and cfg_defined.group("cfg_val") in ("TRUE", "1")

def extract_repo_tms() -> typing.Generator[str, None, None]:
    """
    Yield MOVE constants assigned to a TM or HM in the user's repo.
    """
    with open("./include/constants/tms_hms.h", "r") as tmshms_fp:
        tmshms = tmshms_fp.read()
        match_it = TMHM_MACRO_PAT.finditer(tmshms)
        if not match_it:
            return

        for match in match_it:
            yield f"MOVE_{match.group(1)}"

def extract_tm_litteracy_config() -> bool:
    config = False
    with open("./include/config/pokemon.h", "r") as cfg_pokemon_fp:
        cfg_pokemon = cfg_pokemon_fp.read()
        cfg_defined = TM_LITERACY_PAT.search(cfg_pokemon)
        if cfg_defined:
            cfg_val = cfg_defined.group("cfg_val")
            if ((cfg_val == "LATEST") or (int(cfg_val) > 6)):
                config = True
    return config

def prepare_output(all_learnables: dict[str, set[str]], tms: list[str], tutors: list[str], special_movesets, repo_teaching_types, header: str) -> str:
    """
    Build the file content for teachable_learnsets.h.
    """

    tm_litteracy_config = extract_tm_litteracy_config()

    cursor = 0
    new = header + dedent("""
    static const u16 sNoneTeachableLearnset[] = {
        MOVE_UNAVAILABLE,
    };
    """)

    joinpat = ",\n    "
    for species_data in repo_teaching_types:
        if isinstance(species_data, str):
            new += (species_data)
            continue
        species = species_data["name"]
        teaching_type = species_data["teaching_type"]
        new += f"static const u16 s{species}TeachableLearnset[] = "
        new += "{\n"
        species_upper =  SNAKIFY_PAT.sub(r"_\1", species).upper()
        if teaching_type == "ALL_TEACHABLES":
            part1 = list(filter(lambda m: m not in special_movesets["signatureTeachables"], tms))
            part2 = list(filter(lambda m: m not in special_movesets["signatureTeachables"], tutors))
        else:
            if teaching_type == "TM_ILLITERATE":
                learnables = all_learnables[species_upper]
                if not tm_litteracy_config:
                    learnables = filter(lambda m: m not in special_movesets["universalMoves"], learnables)
            else:
                learnables = all_learnables[species_upper] + special_movesets["universalMoves"]
            part1 = list(filter(lambda m: m in learnables, tms))
            part2 = list(filter(lambda m: m in learnables, tutors))


        repo_species_teachables = part1 + part2
        if species_upper == "TERAPAGOS":
             repo_species_teachables = filter(lambda m: m != "MOVE_TERA_BLAST", repo_species_teachables)

        repo_species_teachables = list(dict.fromkeys(repo_species_teachables))
        new += "\n".join([
            f"    {joinpat.join(chain(repo_species_teachables, ('MOVE_UNAVAILABLE',)))},",
            "};\n",
        ])

    return new

def prepare_emerald_champions_preparation_output(all_learnables: dict[str, list[str]]) -> tuple[str, dict[str, int]]:
    """Build the Center-only, all-generation legal preparation corpus."""
    lines = [
        "// DO NOT MODIFY THIS FILE! It is auto-generated by tools/learnset_helpers/make_teachables.py",
        "// This corpus is exclusive to Emerald Champions' All Legal Moves service.",
        "// Ordinary TM/tutor compatibility and random move generation must keep using teachable_learnsets.h.",
        "",
        "static const u16 sEmeraldChampionsPreparationMoves_None[] = { MOVE_UNAVAILABLE };",
        "",
    ]
    normalized: dict[str, list[str]] = {}
    for species, source_moves in all_learnables.items():
        species_key = preparation_species_key(species)
        assert re.fullmatch(r"[A-Z0-9_]+", species_key), species
        moves = list(dict.fromkeys(source_moves))
        assert len(moves) == len(source_moves), f"duplicate preparation move for {species}"
        assert all(MOVE_CONST_PAT.fullmatch(move) for move in moves), species
        merged = normalized.setdefault(species_key, [])
        merged.extend(move for move in moves if move not in merged)

    total_moves = sum(len(moves) for moves in normalized.values())
    max_species, max_species_moves = max(normalized.items(), key=lambda entry: len(entry[1]))
    max_moves = len(max_species_moves)
    for species_key, moves in normalized.items():
        lines.append(f"static const u16 sEmeraldChampionsPreparationMoves_{species_key}[] = {{")
        lines.extend(f"    {move}," for move in moves)
        lines.extend(("    MOVE_UNAVAILABLE,", "};", ""))

    lines.extend((
        f"#define EMERALD_CHAMPIONS_PREPARATION_SPECIES_COUNT {len(normalized)}",
        f"#define EMERALD_CHAMPIONS_PREPARATION_MOVE_COUNT {total_moves}",
        f"#define EMERALD_CHAMPIONS_PREPARATION_MAX_MOVES {max_moves}",
        f"#define EMERALD_CHAMPIONS_PREPARATION_MAX_SPECIES SPECIES_{max_species}",
        "STATIC_ASSERT(EMERALD_CHAMPIONS_PREPARATION_MAX_MOVES <= MAX_RELEARNER_MOVES, PreparationMoveListExceedsRuntimeBuffer);",
        "",
        "static const u16 *const sEmeraldChampionsPreparationLearnsets[NUM_SPECIES] =",
        "{",
    ))
    lines.extend(
        f"    [SPECIES_{species}] = sEmeraldChampionsPreparationMoves_{species},"
        for species in normalized
    )
    lines.extend(("};", ""))
    return "\n".join(lines), {
        "species": len(normalized),
        "moves": total_moves,
        "max_moves": max_moves,
    }

def apply_reviewed_tutor_extensions(all_learnables, tutor_extensions) -> None:
    for row in tutor_extensions:
        species = row["teachable_species"]
        move = row["move"]
        assert species in all_learnables, (species, move)
        if move not in all_learnables[species]:
            all_learnables[species].append(move)

def apply_reviewed_form_learnsets(all_learnables) -> None:
    """Add explicit legal pools for forms absent from all_learnables.json."""
    with open(PREPARATION_FORM_LEARNSETS, "r") as file:
        review = json.load(file)
    assert review["schema_version"] == 1
    assert review["source"] == {
        "repository": "smogon/pokemon-showdown",
        "commit": "bb179fbf8449e3c31632bd56f671ffb4404fa6e7",
        "path": "data/learnsets.ts",
    }
    resolved = {}
    for species, row in review["forms"].items():
        if "inherits" in row:
            inherited_species = row["inherits"]
            inherited_moves = resolved.get(inherited_species, all_learnables.get(inherited_species))
            assert inherited_moves is not None, (species, inherited_species)
            moves = [*inherited_moves, *row.get("adds", [])]
        else:
            moves = list(row["moves"])
        assert len(moves) == len(set(moves)), species
        all_learnables[species] = resolved[species] = moves

def prepare_header(h_align: int, tmshms: list[str], tutors: list[str], universals: list[str]) -> str:
    universals_title = "Near-universal moves found in data/special_movesets.json:"
    tmhm_title = "TM/HM moves found in \"include/constants/tms_hms.h\":"
    tutor_title = "Tutor moves found from map scripts:"
    h_align = max(h_align, len(universals_title), len(tmhm_title), len(tutor_title))

    lines = [
         "//",
         "// DO NOT MODIFY THIS FILE! It is auto-generated by tools/learnset_helpers/make_teachables.py",
         "//",
         "",
        f"// {'*' * h_align} //",
        f"// {tmhm_title: >{h_align}} //",
    ]
    lines.extend([f"// - {move: <{h_align - 2}} //" for move in tmshms])
    lines.extend([
        f"// {'*' * h_align} //",
        f"// {tutor_title: <{h_align}} //",
    ])
    lines.extend([f"// - {move: <{h_align - 2}} //" for move in sorted(tutors)])
    lines.extend([
        f"// {'*' * h_align} //",
        f"// {universals_title: <{h_align}} //",
    ])
    lines.extend([f"// - {move: <{h_align - 2}} //" for move in universals])
    lines.extend([
        f"// {'*' * h_align} //",
         "",
    ])

    return "\n".join(lines)

def create_tutor_moves_array(tutors):
    """
    Generate gTutorMoves[].
    """
    header = dedent("""\
        // DO NOT MODIFY THIS FILE! It is auto-generated by tools/learnset_helpers/make_teachables.py
        // Needed by tutor moves relearner!

        const u16 gTutorMoves[] = {
    """)

    lines = [f"    {move}," for move in tutors]
    lines.append("    MOVE_UNAVAILABLE\n};\n")
    with open("./src/data/tutor_moves.h", "w") as f:
        f.write(header + "\n".join(lines))

def make_move_tutors(build_dir, special_movesets):
    SOURCE_TUTORS_JSON = build_dir / "all_tutors.json"

    assert SOURCE_TUTORS_JSON.exists(), f"{SOURCE_TUTORS_JSON=} does not exist"
    assert SOURCE_TUTORS_JSON.is_file(), f"{SOURCE_TUTORS_JSON=} is not a file"

    with open(SOURCE_TUTORS_JSON, "r") as fp:
        repo_tutors = json.load(fp)

    repo_tutors = sorted(list(set(repo_tutors + special_movesets["extraTutors"])))
    create_tutor_moves_array(repo_tutors)

    return repo_tutors

def main():
    if not enabled():
        quit()

    tutor_mode = False
    preparation_mode = len(sys.argv) == 2 and sys.argv[1] == "--preparation"
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print("Invalid number of arguments", file=sys.stderr)
        print(__doc__, file=sys.stderr)
        quit(1)

    if len(sys.argv) == 3:
        if sys.argv[1] != "--tutors":
            print("Unknown make_teachables mode", file=sys.stderr)
            quit(1)
        tutor_mode = True

    SOURCE_DIR = None if preparation_mode else pathlib.Path(sys.argv[-1])

    with open("src/data/pokemon/special_movesets.json", "r") as file:
        special_movesets = json.load(file)

    # Emerald Champions keeps a small, source-reviewed compatibility layer for
    # historical event moves and deliberate Inclement learnset extensions.
    # The manifest is authoritative: unsupported imports are replaced there,
    # and only explicitly reviewed retained rows enter the native tutor path.
    MOVE_ACCESS_REVIEW = pathlib.Path("./data/emerald_champions/emerald_champions_move_access_review.json")
    with open(MOVE_ACCESS_REVIEW, "r") as file:
        move_access_review = json.load(file)
    tutor_extensions = [
        row for row in move_access_review["assignments"]
        if row.get("requires_tutor_extension", False)
    ]
    with open("./src/data/pokemon/all_learnables.json", "r") as source_fp:
        all_learnables = json.load(source_fp)
    apply_reviewed_form_learnsets(all_learnables)
    apply_reviewed_tutor_extensions(all_learnables, tutor_extensions)

    if preparation_mode:
        content, counts = prepare_emerald_champions_preparation_output(all_learnables)
        expected_species = len({preparation_species_key(species) for species in all_learnables})
        assert counts["species"] == expected_species, counts
        assert counts["moves"] >= counts["species"], counts
        assert counts["max_moves"] > 0, counts
        PREPARATION_OUTPUT.write_text(content)
        return

    for row in tutor_extensions:
        if row["move"] not in special_movesets["extraTutors"]:
            special_movesets["extraTutors"].append(row["move"])

    repo_tutors = make_move_tutors(SOURCE_DIR, special_movesets)
    if tutor_mode:
        quit(0)

    SOURCE_LEARNSETS_JSON = pathlib.Path("./src/data/pokemon/all_learnables.json")
    SOURCE_TEACHING_TYPES_JSON = SOURCE_DIR / "all_teaching_types.json"

    assert SOURCE_LEARNSETS_JSON.exists(), f"{SOURCE_LEARNSETS_JSON=} does not exist"
    assert SOURCE_LEARNSETS_JSON.is_file(), f"{SOURCE_LEARNSETS_JSON=} is not a file"

    assert SOURCE_TEACHING_TYPES_JSON.exists(), f"{SOURCE_TEACHING_TYPES_JSON=} does not exist"
    assert SOURCE_TEACHING_TYPES_JSON.is_file(), f"{SOURCE_TEACHING_TYPES_JSON=} is not a file"

    repo_tms = list(extract_repo_tms())

    with open("./include/config/pokedex_plus_hgss.h", "r") as cfg_pokemon_fp:
        cfg_pokemon = cfg_pokemon_fp.read()
        cfg_defined = ALPHABETICAL_ORDER_ENABLED_PAT.search(cfg_pokemon)
        if cfg_defined is None or cfg_defined.group("cfg_val") in ("FALSE", "0"):
            repo_tms = sorted(repo_tms)

    h_align = max(map(lambda move: len(move), chain(special_movesets["universalMoves"], repo_tms, repo_tutors))) + 2
    header = prepare_header(h_align, repo_tms, repo_tutors, special_movesets["universalMoves"])

    with open(SOURCE_TEACHING_TYPES_JSON, "r") as source_fp:
        repo_teaching_types = json.load(source_fp)

    content = prepare_output(all_learnables, repo_tms, repo_tutors, special_movesets, repo_teaching_types, header)
    with open("./src/data/pokemon/teachable_learnsets.h", "w") as teachables_fp:
        teachables_fp.write(content)

if __name__ == "__main__":
    main()
