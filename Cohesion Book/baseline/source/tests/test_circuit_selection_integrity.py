"""Host execution of Circuit selection; move data and culling are fixtures.

Production membership, type conversion, reservoir selection and AddMove execute.
The culling stub records calls and removes a marker, so mutation order is checked.
"""
import re
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def function(source, name):
    start = re.search(r'static [^\n]+\b' + name + r'\(', source).start()
    end = source.index('\n}', start) + 2
    return source[start:end] + '\n'


def harness(source):
    code = r'''
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
typedef uint8_t u8;
typedef uint32_t u32;
typedef int bool32;
#define TRUE 1
#define FALSE 0
#define MAX_MON_MOVES 4
#define ITEM_NONE 0
#define DAMAGE_CATEGORY_STATUS 0
#define ARRAY_COUNT(a) (sizeof(a) / sizeof *(a))
enum Move {MOVE_NONE, NORMAL, WATER, STATUS, NO_STAB, SOUND, MARKER};
enum Type {TYPE_NONE, TYPE_NORMAL, TYPE_WATER, TYPE_FLYING, TYPE_ELECTRIC, TYPE_FAIRY, TYPE_ICE};
enum Ability {ABILITY_NONE, ABILITY_AERILATE, ABILITY_GALVANIZE, ABILITY_NORMALIZE, ABILITY_PIXILATE, ABILITY_REFRIGERATE, ABILITY_LIQUID_VOICE};
struct CircuitGeneratedSet {enum Move moves[4];};
struct CircuitMovePool {enum Move moves[8]; u8 count;};
struct ShowdownCircuitTemplate {enum Ability abilities[3]; unsigned abilityCount;};
struct ShowdownCircuitVariant {unsigned requiredItem, formSpecies;};
static struct {enum Ability abilities[3];} gSpeciesInfo[2];
static const enum Move sNoStabMoves[] = {NO_STAB};
static unsigned rngState, draws, bounds[8], culls, typeCalls;
static unsigned CircuitRandomUniform(unsigned lo, unsigned hi) {
    assert(lo == 0 && draws < 8); bounds[draws++] = hi;
    rngState = rngState * 1664525u + 1013904223u;
    return rngState % (hi + 1);
}
static unsigned GetMoveCategory(enum Move move) {return move == STATUS ? DAMAGE_CATEGORY_STATUS : 1;}
static enum Type GetMoveType(enum Move move) {typeCalls++; return move == WATER ? TYPE_WATER : TYPE_NORMAL;}
static bool32 IsSoundMove(enum Move move) {return move == SOUND;}
'''
    for name in ['MoveInList', 'SetHasMove', 'SetMoveCount', 'PoolContains',
                 'RemovePoolIndex', 'RemovePoolMove', 'IsDamagingMove',
                 'GetTemplateMoveType', 'IsNoStabMove']:
        code += function(source, name)
    code += r'''
static void CullSelectedIncompatibilities(struct CircuitGeneratedSet *set, struct CircuitMovePool *pool,
    const struct ShowdownCircuitTemplate *template, const struct ShowdownCircuitVariant *variant) {
    culls++;
    RemovePoolMove(pool, MARKER);
}
'''
    code += function(source, 'AddMove')
    code += function(source, 'AddRandomDamagingMove')
    if 'static bool32 AddRandomStabMove(' in source:
        code += function(source, 'AddRandomStabMove')
        code += r'''
static int Select(struct CircuitGeneratedSet *set, struct CircuitMovePool *pool, enum Type type, int match,
    const struct ShowdownCircuitTemplate *template, const struct ShowdownCircuitVariant *variant) {
    return match ? AddRandomStabMove(set,pool,type,template,variant) : AddRandomDamagingMove(set,pool,type,template,variant);
}
'''
    else:
        code += '#define Select AddRandomDamagingMove\n'
    return code


def execute(code):
    compiler = shutil.which('cc')
    if compiler is None:
        raise RuntimeError('host C compiler required')
    with tempfile.TemporaryDirectory() as temp:
        path = Path(temp)
        (path / 'test.c').write_text(code)
        subprocess.run([compiler, '-std=c11', '-fsanitize=undefined', str(path / 'test.c'), '-o', str(path / 'test')], check=True, timeout=30)
        return subprocess.check_output([str(path / 'test')], text=True, timeout=30)


class CircuitSelectionIntegrity(unittest.TestCase):
    def test_selection_boundaries_and_converted_types(self):
        code = harness((ROOT / 'src/champions_circuit.c').read_text())
        code += r'''
int main(void) {
    struct ShowdownCircuitTemplate template = {{ABILITY_NONE}, 1};
    struct ShowdownCircuitVariant variant = {0};
    struct CircuitGeneratedSet set = {{MOVE_NONE, WATER, MOVE_NONE, SOUND}};
    struct CircuitMovePool pool = {{NORMAL, NORMAL, MOVE_NONE}, 3};
    assert(SetHasMove(&set, SOUND)); // later slots count even after a hole
    assert(SetHasMove(&set, MOVE_NONE));
    assert(PoolContains(&pool, MOVE_NONE));
    pool.count = 0;
    assert(!PoolContains(&pool, NORMAL));
    assert(!Select(&set, &pool, TYPE_NONE, FALSE, &template, &variant));
    assert(draws == 0 && culls == 0);
    set = (struct CircuitGeneratedSet){{0}};
    pool = (struct CircuitMovePool){{MOVE_NONE, STATUS, NO_STAB, NORMAL, MARKER}, 5};
    // MARKER is eligible too; this seed selects NORMAL, then culling removes MARKER.
    rngState = 1;
    assert(Select(&set, &pool, TYPE_NONE, FALSE, &template, &variant));
    assert(draws == 2 && bounds[0] == 0 && bounds[1] == 1);
    assert(typeCalls == 0); // TYPE_NONE exclusion bypasses type lookup
    assert(set.moves[0] == NORMAL && culls == 1);
    assert(!PoolContains(&pool, NORMAL) && !PoolContains(&pool, MARKER));
    draws = culls = typeCalls = 0;
    set = (struct CircuitGeneratedSet){{NORMAL, WATER, SOUND, NO_STAB}};
    pool = (struct CircuitMovePool){{WATER}, 1};
    assert(!Select(&set, &pool, TYPE_WATER, TRUE, &template, &variant));
    assert(draws == 1 && culls == 0 && pool.count == 1); // selection consumes RNG before AddMove rejects
    set = (struct CircuitGeneratedSet){{WATER}};
    draws = 0;
    assert(!Select(&set, &pool, TYPE_WATER, TRUE, &template, &variant));
    assert(draws == 1 && culls == 0 && pool.count == 1); // duplicate remains
    const enum Type converted[] = {TYPE_NORMAL, TYPE_FLYING, TYPE_ELECTRIC, TYPE_NORMAL, TYPE_FAIRY, TYPE_ICE, TYPE_WATER};
    for (unsigned ability = ABILITY_AERILATE; ability <= ABILITY_LIQUID_VOICE; ability++) {
        for (unsigned mega = 0; mega < 2; mega++) {
            template.abilities[0] = mega ? ABILITY_NONE : ability;
            gSpeciesInfo[0].abilities[0] = ability;
            variant.requiredItem = mega;
            set = (struct CircuitGeneratedSet){{0}};
            pool = (struct CircuitMovePool){{SOUND}, 1};
            draws = culls = typeCalls = 0;
            assert(Select(&set, &pool, converted[ability], TRUE, &template, &variant));
            assert(set.moves[0] == SOUND && draws == 1 && culls == 1 && typeCalls == 1);
        }
    }
    return 0;
}
'''
        execute(code)


if __name__ == '__main__':
    unittest.main()
