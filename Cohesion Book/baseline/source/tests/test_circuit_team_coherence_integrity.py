"""Execute production Circuit field/lead rules with small move/species fixtures."""
import re
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def function(source, name):
    start = re.search(r'static [^\n]+\b' + name + r'\(', source).start()
    return source[start:source.index('\n}', start) + 2] + '\n'


class CircuitTeamCoherenceTests(unittest.TestCase):
    def test_fields_and_leads_execute_production_rules(self):
        compiler = shutil.which('cc')
        self.assertIsNotNone(compiler)
        source = (ROOT / 'src/champions_circuit.c').read_text()
        names = ['MoveInList', 'SetHasMove', 'TeamHasMove', 'SetHasMoveFromList',
                 'IsDamagingMove', 'GetCircuitSetAbility', 'UpdateTeamDetails',
                 'TeamHasSpecies', 'TeamIsCoherent', 'LeadSupportScore', 'ChooseCoherentLeads']
        functions = '\n'.join(function(source, name) for name in names)
        structures = '\n'.join(re.search(r'struct ' + name + r'\n\{.*?\n\};', source, re.S)[0]
                               for name in ['CircuitTeamDetails', 'CircuitGeneratedSet', 'CircuitTeamState'])
        main = r'''
static struct CircuitTeamState Team(void) {
    struct CircuitTeamState t = {0}; t.count = PARTY_SIZE;
    for (unsigned i = 0; i < PARTY_SIZE; i++) {
        gShowdownCircuitVariants[i].formSpecies = i;
        gShowdownCircuitVariants[i].partySpecies = i;
        gShowdownCircuitVariants[i].requiredItem = ITEM_NONE;
        gSpeciesInfo[i].baseSpeed = 50;
        gSpeciesInfo[i].abilities[0] = ABILITY_NONE;
        t.sets[i].variantIndex = i;
        t.sets[i].moves[0] = i % 2 ? MOVE_PSYCHIC : MOVE_TACKLE;
    }
    t.sets[0].moves[1] = MOVE_TAILWIND;
    return t;
}
static void Fields(struct CircuitTeamState *t) {
    memset(&t->details, 0, sizeof(t->details));
    for (unsigned i = 0; i < t->count; i++) UpdateTeamDetails(&t->details, &t->sets[i]);
}
static int Leads(const struct CircuitTeamState *t, unsigned a, unsigned b) {
    unsigned x=t->sets[5].variantIndex,y=t->sets[4].variantIndex;
    return (x==a && y==b) || (x==b && y==a);
}
int main(void) {
    struct CircuitTeamState t = Team(); Fields(&t); assert(TeamIsCoherent(&t));
    t.sets[1].ability = ABILITY_SURGE_SURFER;
    t.sets[2].ability = ABILITY_MISTY_SURGE;
    Fields(&t); assert(!TeamIsCoherent(&t)); // the wrong terrain is not support
    t.sets[2].ability = ABILITY_ELECTRIC_SURGE;
    Fields(&t); assert(TeamIsCoherent(&t));
    t.sets[1].ability = ABILITY_NONE;
    t.sets[1].moves[0] = MOVE_EXPANDING_FORCE;
    Fields(&t); assert(!TeamIsCoherent(&t));
    t.sets[2].ability = ABILITY_PSYCHIC_SURGE;
    Fields(&t); assert(TeamIsCoherent(&t));
    t.sets[1].moves[0] = MOVE_GRASSY_GLIDE;
    Fields(&t); assert(!TeamIsCoherent(&t));
    t.sets[2].ability = ABILITY_NONE;
    t.sets[2].moves[1] = MOVE_GRASSY_TERRAIN;
    Fields(&t); assert(TeamIsCoherent(&t)); // manually set terrain counts
    t = Team(); t.sets[1].ability = ABILITY_SAND_RUSH;
    Fields(&t); assert(!TeamIsCoherent(&t));
    t.sets[2].moves[1] = MOVE_SANDSTORM;
    Fields(&t); assert(TeamIsCoherent(&t));
    t.sets[2].moves[1] = MOVE_NONE;
    gShowdownCircuitVariants[2].requiredItem = ITEM_MEGA_STONE;
    gSpeciesInfo[2].abilities[0] = ABILITY_SAND_STREAM;
    Fields(&t); assert(TeamIsCoherent(&t)); // use the actual Mega ability
    t = Team(); t.sets[1].ability = ABILITY_SCREEN_CLEANER;
    Fields(&t); assert(TeamIsCoherent(&t));
    t.sets[2].moves[1] = MOVE_REFLECT;
    Fields(&t); assert(!TeamIsCoherent(&t));
    t.sets[2].moves[1] = MOVE_LIGHT_SCREEN;
    Fields(&t); assert(!TeamIsCoherent(&t));
    t.sets[2].moves[1] = MOVE_AURORA_VEIL;
    t.sets[3].ability = ABILITY_SNOW_WARNING;
    Fields(&t); assert(!TeamIsCoherent(&t));
    // No regular attack in the first pair; this is also a zero-score tie.
    t = Team(); memset(t.sets[0].moves,0,sizeof(t.sets[0].moves));
    memset(t.sets[1].moves,0,sizeof(t.sets[1].moves));
    t.sets[0].moves[0]=MOVE_HELPING_HAND; t.sets[1].moves[0]=MOVE_PROTECT;
    ChooseCoherentLeads(&t); assert(!Leads(&t,0,1));
    t = Team(); t.sets[0].moves[0]=MOVE_FAKE_OUT;
    t.sets[1].moves[0]=MOVE_FAKE_OUT; t.sets[1].moves[1]=MOVE_TAILWIND;
    ChooseCoherentLeads(&t); assert(!Leads(&t,0,1));
    // Weather partners beat a non-complementary generic support pairing.
    t = Team(); t.sets[0].moves[1]=MOVE_NONE;
    t.sets[2].ability=ABILITY_DRIZZLE; t.sets[3].ability=ABILITY_SWIFT_SWIM;
    ChooseCoherentLeads(&t); assert(Leads(&t,2,3));
    t = Team(); t.sets[0].moves[1]=MOVE_NONE;
    t.sets[2].ability=ABILITY_ELECTRIC_SURGE; t.sets[3].ability=ABILITY_SURGE_SURFER;
    ChooseCoherentLeads(&t); assert(Leads(&t,2,3));
    t = Team(); t.sets[0].moves[1]=MOVE_NONE;
    t.sets[2].ability=ABILITY_PSYCHIC_SURGE; t.sets[3].moves[0]=MOVE_EXPANDING_FORCE;
    ChooseCoherentLeads(&t); assert(Leads(&t,2,3));
    return 0;
}
'''
        text = functions + structures + main
        enums = []
        for kind, prefix, zero in [('Move','MOVE_','MOVE_NONE'), ('Ability','ABILITY_','ABILITY_NONE'),
                                   ('Species','SPECIES_','SPECIES_NONE'), ('Item','ITEM_','ITEM_NONE'),
                                   ('CircuitDependency','CIRCUIT_DEPENDENCY_','CIRCUIT_DEPENDENCY_NONE'),
                                   ('BattleTerrain','B_TERRAIN_','B_TERRAIN_NONE')]:
            values = sorted(set(re.findall(r'\b' + prefix + r'[A-Z0-9_]+', text)) - {zero})
            enums.append('enum ' + kind + ' {' + ','.join([zero, *values]) + '};')
        prefix = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
typedef uint8_t u8; typedef uint16_t u16; typedef uint32_t u32; typedef int bool32; typedef uint8_t bool8;
#define TRUE 1
#define FALSE 0
#define PARTY_SIZE 6
#define CIRCUIT_TEAM_SIZE 6
#define MAX_MON_MOVES 4
#define NUM_STATS 6
#define NUMBER_OF_MON_TYPES 18
#define ARRAY_COUNT(a) (sizeof(a)/sizeof(*(a)))
#define DAMAGE_CATEGORY_STATUS 0
#define DAMAGE_CATEGORY_PHYSICAL 1
#define DAMAGE_CATEGORY_SPECIAL 2
'''
        fixtures = r'''
struct ShowdownCircuitVariant { enum Species formSpecies,partySpecies; enum Item requiredItem; };
static struct ShowdownCircuitVariant gShowdownCircuitVariants[16];
static struct { unsigned baseSpeed; enum Ability abilities[3]; } gSpeciesInfo[16];
static int IsMegaVariant(const struct ShowdownCircuitVariant *v) {return v->requiredItem!=ITEM_NONE;}
static unsigned GetSpeciesBaseStat(enum Species s,unsigned n) {(void)s;(void)n;return 90;}
static unsigned GetMoveCategory(enum Move m) {
    if(m==MOVE_TACKLE || m==MOVE_FAKE_OUT || m==MOVE_GRASSY_GLIDE) return DAMAGE_CATEGORY_PHYSICAL;
    if(m==MOVE_PSYCHIC || m==MOVE_EXPANDING_FORCE) return DAMAGE_CATEGORY_SPECIAL;
    return DAMAGE_CATEGORY_STATUS;
}
static const enum Move sSpeedControlMoves[] = {MOVE_TAILWIND,MOVE_TRICK_ROOM};
'''
        code = prefix + '\n'.join(enums) + structures + fixtures + functions + main
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)
            (path/'test.c').write_text(code)
            subprocess.run([compiler,'-std=c11','-Wall','-Wextra','-Werror','-fsanitize=undefined',str(path/'test.c'),'-o',str(path/'test')],check=True,timeout=30)
            subprocess.run([str(path/'test')],check=True,timeout=30)


if __name__ == '__main__':
    unittest.main()
