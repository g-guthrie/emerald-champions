"""Run the production tutor collector against the real generated move/preset tables.

The whole of src/emerald_champions_battle_sets.c and src/move_relearner.c is
compiled on the host. Only pokemon.c's boundary is stubbed: BoxPokemon storage
accessors and the species form tables (taken from the configured species data).
This checks move access, not native menu rendering or move replacement.
"""
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tests"))
import host_c


# pokemon.c boundary: the harness owns each BoxPokemon's species and moves.
BOUNDARY = r'''
#include "host_mon.h"
u32 GetBoxMonData2(struct BoxPokemon *boxMon, s32 field)
{
    assert(field == MON_DATA_SPECIES);
    return ((struct HostMon *)boxMon)->species;
}
bool8 BoxMonKnowsMove(struct BoxPokemon *boxMon, enum Move move)
{
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        if (((struct HostMon *)boxMon)->moves[i] == move)
            return TRUE;
    return FALSE;
}
'''

HOST_MON = r'''
#include <assert.h>
struct HostMon { struct BoxPokemon box; enum Species species; u16 moves[MAX_MON_MOVES]; };
bool32 HostHasRelearnerAllMoves(struct BoxPokemon *boxMon);
'''

RELEARNER = r'''
bool32 HostHasRelearnerAllMoves(struct BoxPokemon *boxMon)
{
    return HasRelearnerAllMoves(boxMon);
}
'''

HARNESS = r'''
#include <stdio.h>
#include <string.h>
#include "constants/move_relearner.h"
#include "host_mon.h"
static u16 moves[MAX_RELEARNER_MOVES];
static bool8 offered[MOVES_COUNT_ALL];
static u32 collect(struct HostMon *mon) {
    memset(offered, 0, sizeof(offered));
    u32 count = GetEmeraldChampionsPreparationMovesToLearn(&mon->box, moves);
    assert(count <= MAX_RELEARNER_MOVES);
    assert(count == GetEmeraldChampionsPreparationMovesToLearn(&mon->box, NULL));
    assert(HostHasRelearnerAllMoves(&mon->box) == (count != 0));
    for (u32 i = 0; i < count; i++) {
        assert(moves[i] > MOVE_NONE && moves[i] < MOVES_COUNT_ALL);
        assert(!offered[moves[i]]);
        assert(!BoxMonKnowsMove(&mon->box, moves[i]));
        offered[moves[i]] = TRUE;
    }
    return count;
}
int main(void) {
    u32 checked = 0;
    for (u32 species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++) {
        struct HostMon mon = {.species = species};
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
            assert(offered[move] == (expected[move] && !BoxMonKnowsMove(&mon.box, move)));
    }
    struct HostMon smeargle = {.species = SPECIES_SMEARGLE, .moves = {MOVE_SKETCH}};
    collect(&smeargle);
    assert(HostHasRelearnerAllMoves(&smeargle.box)); // Old has-moves path incorrectly returned FALSE.
    assert(offered[MOVE_SPORE] && offered[MOVE_GEOMANCY] && offered[MOVE_DECORATE]);
    assert(!offered[MOVE_SKETCH] && !offered[MOVE_THUNDERBOLT]);
    struct HostMon rotom = {.species = SPECIES_ROTOM_WASH};
    collect(&rotom);
    assert(offered[MOVE_HYDRO_PUMP] && !offered[MOVE_OVERHEAT]);
    struct HostMon starmie = {.species = SPECIES_STARMIE};
    collect(&starmie);
    assert(offered[MOVE_ICE_SPINNER]); // Mega preset move access does not require its item.
    printf("PASS: every species/form, %u direct presets, known-move filtering and Smeargle boundary\n", checked);
}
'''


class PreparationMoveIntegrity(unittest.TestCase):
    def test_historical_and_preset_moves_are_individually_learnable(self):
        subprocess.run([sys.executable, "tools/learnset_helpers/make_teachables.py", "--preparation"],
                       cwd=ROOT, check=True, timeout=60)
        boundary = host_c.species_form_boundary() + BOUNDARY
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)
            (path / "host_mon.h").write_text(HOST_MON)
            executable = host_c.build(path, {
                "battle_sets.c": host_c.production("src/emerald_champions_battle_sets.c") + HARNESS,
                "move_relearner.c": host_c.production("src/move_relearner.c") + RELEARNER,
                "pokemon_boundary.c": boundary,
            }, flags=("-iquote", str(path)))
            result = host_c.run(executable, timeout=120)
            self.assertIn("PASS: every species/form", result.stdout)


if __name__ == "__main__":
    unittest.main()
