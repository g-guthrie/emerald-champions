"""Run the production tutor collector against the real generated move/preset tables.

Only BoxPokemon storage is stubbed. Form pointers come from configured species
data; this checks move access, not native menu rendering or move replacement.
"""
import re
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
from verify_trainer_ability_legality import preprocess_species_info, SPECIES_MARKER


class PreparationMoveIntegrity(unittest.TestCase):
    def test_historical_and_preset_moves_are_individually_learnable(self):
        subprocess.run([sys.executable, "tools/learnset_helpers/make_teachables.py", "--preparation"],
                       cwd=ROOT, check=True, timeout=60)
        species = preprocess_species_info().split("const struct SpeciesInfo gSpeciesInfo[]", 1)[1]
        markers = list(SPECIES_MARKER.finditer(species))
        pointers = []
        for index, marker in enumerate(markers):
            end = markers[index + 1].start() if index + 1 < len(markers) else len(species)
            form = re.search(r"\.formSpeciesIdTable\s*=\s*(s\w+)", species[marker.end():end])
            if form:
                pointers.append(f"[{marker[1]}] = {{{form[1]}}},")

        header = (ROOT / "include/emerald_champions_battle_sets.h").read_text()
        structs = header[header.index("struct EmeraldChampionsBattleSet\n"):
                         header.index("// EVs are shown")]
        sets = (ROOT / "src/emerald_champions_battle_sets.c").read_text()
        lookup = sets[sets.index("static bool32 IsValidBattleFormat("):
                      sets.index("static bool32 IsVisiblePreset(")]
        tutor = (ROOT / "src/move_relearner.c").read_text()
        collector = sets[sets.index("const u16 *GetEmeraldChampionsPreparationMoves("):
                         sets.index("bool32 CanSpeciesKeepEmeraldChampionsUnfusionMove(")]
        has_moves = tutor[tutor.rindex("static bool32 HasRelearnerAllMoves("):
                          tutor.rindex("static bool32 IsLevelUpMoveRelearnerActive(")]

        harness = r'''
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
typedef uint8_t u8, bool8;
typedef uint16_t u16;
typedef uint32_t u32, bool32;
#define TRUE 1
#define FALSE 0
#define _(text) text
#define STATIC_ASSERT(condition, name) _Static_assert(condition, #name)
#include "constants/global.h"
#include "constants/pokemon.h"
#include "constants/species.h"
#include "constants/moves.h"
#include "constants/items.h"
#include "constants/abilities.h"
#include "constants/emerald_champions.h"
#include "constants/move_relearner.h"
#define FORM_SPECIES_END (0xffff)
#include "data/pokemon/form_species_tables.h"
struct BoxPokemon { enum Species species; u16 moves[MAX_MON_MOVES]; };
#define MON_DATA_SPECIES 0
static u32 GetBoxMonData(struct BoxPokemon *mon, u32 field) { return mon->species; }
static bool32 BoxMonKnowsMove(struct BoxPokemon *mon, u16 move) {
    for (u32 i = 0; i < MAX_MON_MOVES; i++) if (mon->moves[i] == move) return TRUE;
    return FALSE;
}
'''
        harness += "static const struct { const u16 *formSpeciesIdTable; } gSpeciesInfo[NUM_SPECIES] = {\n"
        harness += "\n".join(pointers) + "\n};\n"
        harness += r'''
#define GET_BASE_SPECIES_ID(s) (gSpeciesInfo[s].formSpeciesIdTable ? gSpeciesInfo[s].formSpeciesIdTable[0] : (s))
'''
        harness += structs
        harness += '#include "data/pokemon/emerald_champions_battle_sets.h"\n'
        harness += '#include "data/pokemon/emerald_champions_preparation_learnsets.h"\n'
        harness += lookup + collector + has_moves
        harness += r'''
static u16 moves[MAX_RELEARNER_MOVES];
static bool8 offered[MOVES_COUNT_ALL];
static u32 collect(struct BoxPokemon *mon) {
    memset(offered, 0, sizeof(offered));
    u32 count = GetEmeraldChampionsPreparationMovesToLearn(mon, moves);
    assert(count <= MAX_RELEARNER_MOVES);
    assert(count == GetEmeraldChampionsPreparationMovesToLearn(mon, NULL));
    assert(HasRelearnerAllMoves(mon) == (count != 0));
    for (u32 i = 0; i < count; i++) {
        assert(moves[i] > MOVE_NONE && moves[i] < MOVES_COUNT_ALL);
        assert(!offered[moves[i]]);
        assert(!BoxMonKnowsMove(mon, moves[i]));
        offered[moves[i]] = TRUE;
    }
    return count;
}
int main(void) {
    u32 checked = 0;
    for (u32 species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++) {
        struct BoxPokemon mon = {.species = species};
        bool8 expected[MOVES_COUNT_ALL] = {FALSE};
        const u16 *historical = GetEmeraldChampionsPreparationMoves(species);
        for (u32 i = 0; historical[i] != MOVE_UNAVAILABLE; i++) expected[historical[i]] = TRUE;
        // Read the compiled tables directly, independently of the raw lookup API.
        for (u32 format = 0; format < EC_BATTLE_FORMAT_COUNT; format++) {
            const struct EmeraldChampionsBattleSetRange *ranges = gEmeraldChampionsBattleSetRanges[format];
            u32 owner = species;
            if (ranges[owner].count == 0 && gSpeciesInfo[species].formSpeciesIdTable) {
                const u16 *forms = gSpeciesInfo[species].formSpeciesIdTable;
                for (u32 i = 0; forms[i] != FORM_SPECIES_END; i++) {
                    if (ranges[forms[i]].count != 0) { owner = forms[i]; break; }
                }
            }
            if (ranges[owner].count == 0) continue;
            for (u32 raw = 0; raw < ranges[owner].count; raw++) {
                const struct EmeraldChampionsBattleSet *preset = &gEmeraldChampionsBattleSets[ranges[owner].offset + raw].preset;
                if (owner == species) checked++;
                for (u32 slot = 0; slot < MAX_MON_MOVES; slot++) expected[preset->moves[slot]] = TRUE;
            }
        }
        expected[MOVE_NONE] = FALSE;
        u32 count = collect(&mon);
        for (u32 move = 1; move < MOVES_COUNT_ALL; move++) assert(offered[move] == expected[move]);
        // Learning any four offered moves must remove only those four entries.
        for (u32 slot = 0; slot < MAX_MON_MOVES && slot < count; slot++) mon.moves[slot] = moves[slot];
        collect(&mon);
        for (u32 move = 1; move < MOVES_COUNT_ALL; move++)
            assert(offered[move] == (expected[move] && !BoxMonKnowsMove(&mon, move)));
    }
    struct BoxPokemon smeargle = {.species = SPECIES_SMEARGLE, .moves = {MOVE_SKETCH}};
    collect(&smeargle);
    assert(HasRelearnerAllMoves(&smeargle)); // Old has-moves path incorrectly returned FALSE.
    assert(offered[MOVE_SPORE] && offered[MOVE_GEOMANCY] && offered[MOVE_DECORATE]);
    assert(!offered[MOVE_SKETCH] && !offered[MOVE_THUNDERBOLT]);
    struct BoxPokemon rotom = {.species = SPECIES_ROTOM_WASH};
    collect(&rotom);
    assert(offered[MOVE_HYDRO_PUMP] && !offered[MOVE_OVERHEAT]);
    struct BoxPokemon starmie = {.species = SPECIES_STARMIE};
    collect(&starmie);
    assert(offered[MOVE_ICE_SPINNER]); // Mega preset move access does not require its item.
    printf("PASS: every species/form, %u direct presets, known-move filtering and Smeargle boundary\n", checked);
}
'''
        compiler = shutil.which("cc")
        self.assertIsNotNone(compiler, "host C compiler required")
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)
            (path / "test.c").write_text(harness)
            subprocess.run([compiler, "-std=c11", "-fsanitize=undefined", "-O1",
                            "-I", str(ROOT / "include"), "-I", str(ROOT / "src"),
                            str(path / "test.c"), "-o", str(path / "test")], check=True, timeout=60)
            subprocess.run([str(path / "test")], check=True, timeout=60)


if __name__ == "__main__":
    unittest.main()
