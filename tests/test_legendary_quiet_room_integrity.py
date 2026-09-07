"""Execute the production quiet-room prefix and shared Sign override on the host.

The StandardWildEncounter fragment ends before Battle Pike handling: the fixture
closes that block and returns FALSE for all remaining paths. Thus a nonempty
header test proves only that it bypasses this fallback, not ordinary encounters.
Research, save state, terrain, RNG and level calculation are controlled APIs;
this does not prove Devon research acquisition or the authored encounter table.
"""
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


class LegendaryQuietRoomIntegrity(unittest.TestCase):
    def test_researched_quiet_room_encounter_guards(self):
        wild = (ROOT / 'src/wild_encounter.c').read_text()
        signs = (ROOT / 'src/legendary_signs.c').read_text()
        start = wild.index('bool8 StandardWildEncounter(')
        stop = wild.index('        if (gMapHeader.mapLayoutId == LAYOUT_BATTLE_FRONTIER_BATTLE_PIKE_ROOM_WILD_MONS)', start)
        prefix = wild[start:stop] + '        return FALSE;\n    }\n    return FALSE;\n}\n'
        override = signs[signs.index('bool32 TryGetLegendarySignWildOverride('):signs.index('void TryUnlockSelectedLegendarySign(')]
        harness = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef unsigned bool8;
typedef unsigned bool32;
#define TRUE 1
#define FALSE 0
#define HEADER_NONE UINT16_MAX
#define FLAG_EC_REPEL_SPRAY_ACTIVE 42
#define RNG_NONE 0
#define LAND_TILE 7
#define QUIET_MAP 0x1234

enum Species { SPECIES_NONE, SPECIES_TERAPAGOS = 1024 };
enum TimeOfDay { DAY };
enum WildPokemonArea { WILD_AREA_LAND, WILD_AREA_WATER };
enum LegendarySignId { LEGENDARY_SIGN_TERAPAGOS, LEGENDARY_SIGN_COUNT };
enum LegendarySignSource { LEGENDARY_SOURCE_CONDITIONAL_WILD, LEGENDARY_SOURCE_OTHER };
struct LegendarySignDefinition {
    enum Species species;
    u16 mapId;
    enum LegendarySignSource source;
    enum WildPokemonArea area;
    unsigned chance;
    int levelOffset;
};
static struct LegendarySignDefinition gLegendarySignDefinitions[LEGENDARY_SIGN_COUNT];
static struct { struct { u8 mapGroup, mapNum; } location; } save;
#define gSaveBlock1Ptr (&save)
static unsigned sWildEncountersDisabled, repel, researched, caught, lost, roll;
static unsigned createCount, battleCount, stateChecks, rngCalls;
static u32 headerId;
static enum Species createdSpecies;
static u8 createdLevel;
static bool32 FlagGet(unsigned flag) { assert(flag == FLAG_EC_REPEL_SPRAY_ACTIVE); return repel; }
static u32 GetCurrentMapWildMonHeaderId(void) { return headerId; }
static bool32 MetatileBehavior_IsLandWildEncounter(u16 tile) { return tile == LAND_TILE; }
static bool32 IsLegendarySignCaught(enum LegendarySignId sign) {
    assert(sign == LEGENDARY_SIGN_TERAPAGOS); stateChecks++; return caught;
}
static bool32 IsLegendaryEncounterLost(enum Species species) {
    assert(species == SPECIES_TERAPAGOS); return lost;
}
static bool32 IsLegendarySignUnlocked(enum LegendarySignId sign) {
    assert(sign == LEGENDARY_SIGN_TERAPAGOS); return researched;
}
static unsigned RandomUniform(unsigned rng, unsigned low, unsigned high) {
    assert(rng == RNG_NONE && low == 0 && high == 99); rngCalls++; return roll;
}
static u8 GetSignLevel(int offset) { assert(offset == 2); return 62; }
static void CreateWildMon(enum Species species, u8 level) {
    assert(createCount == 0 && battleCount == 0);
    createdSpecies = species; createdLevel = level; createCount++;
}
static void BattleSetup_StartWildBattle(void) { assert(createCount == 1 && battleCount == 0); battleCount++; }
'''
        harness += override + prefix
        harness += r'''
static void Reset(void) {
    gLegendarySignDefinitions[0] = (struct LegendarySignDefinition) {
        SPECIES_TERAPAGOS, QUIET_MAP, LEGENDARY_SOURCE_CONDITIONAL_WILD,
        WILD_AREA_LAND, 20, 2
    };
    save.location.mapGroup = QUIET_MAP >> 8;
    save.location.mapNum = QUIET_MAP & 255;
    researched = 1;
    caught = lost = repel = sWildEncountersDisabled = 0;
    createCount = battleCount = stateChecks = rngCalls = 0;
    createdSpecies = SPECIES_NONE; createdLevel = 0;
    headerId = HEADER_NONE; roll = 19;
}
static void Rejected(u16 tile) {
    assert(StandardWildEncounter(tile, tile) == FALSE);
    assert(createCount == 0 && battleCount == 0);
    assert(createdSpecies == SPECIES_NONE && createdLevel == 0);
}
int main(void) {
    Reset();
    assert(StandardWildEncounter(LAND_TILE, LAND_TILE) == TRUE);
    assert(createCount == 1 && battleCount == 1);
    assert(createdSpecies == SPECIES_TERAPAGOS && createdLevel == 62);
    assert(rngCalls == 1);

    Reset(); researched = 0; Rejected(LAND_TILE); assert(rngCalls == 0);
    Reset(); caught = 1; Rejected(LAND_TILE); assert(rngCalls == 0);
    Reset(); lost = 1; Rejected(LAND_TILE); assert(!caught && lost && rngCalls == 0);
    Reset(); save.location.mapGroup++; Rejected(LAND_TILE); assert(rngCalls == 0);
    Reset(); save.location.mapNum++; Rejected(LAND_TILE); assert(rngCalls == 0);
    Reset(); Rejected(LAND_TILE + 1); assert(stateChecks == 0);
    Reset(); sWildEncountersDisabled = TRUE; Rejected(LAND_TILE); assert(stateChecks == 0);
    Reset(); repel = TRUE; Rejected(LAND_TILE); assert(stateChecks == 0);
    Reset(); headerId = 0; Rejected(LAND_TILE); assert(stateChecks == 0);
    Reset(); headerId = 1; Rejected(LAND_TILE); assert(stateChecks == 0);
    Reset(); roll = 20; Rejected(LAND_TILE); assert(rngCalls == 1);
    Reset(); roll = 99; Rejected(LAND_TILE); assert(rngCalls == 1);
    Reset(); gLegendarySignDefinitions[0].chance = 0; roll = 0; Rejected(LAND_TILE);
    Reset(); gLegendarySignDefinitions[0].source = LEGENDARY_SOURCE_OTHER; Rejected(LAND_TILE);
    Reset(); gLegendarySignDefinitions[0].area = WILD_AREA_WATER; Rejected(LAND_TILE);
    return 0;
}
'''
        compiler = shutil.which('cc')
        self.assertIsNotNone(compiler, 'host C compiler required')
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp)
            (path / 'test.c').write_text(harness)
            subprocess.run([compiler, '-std=c11', '-fsanitize=undefined',
                            '-fno-sanitize-recover=undefined', str(path / 'test.c'),
                            '-o', str(path / 'test')], check=True, timeout=30)
            subprocess.run([str(path / 'test')], check=True, timeout=30)


if __name__ == '__main__':
    unittest.main()
