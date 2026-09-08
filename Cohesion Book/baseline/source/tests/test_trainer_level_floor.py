"""Exercise the production level/EXP/HP adjustment against badge progress."""
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

from test_difficulty_integrity import ROOT, function


class TrainerLevelFloor(unittest.TestCase):
    def test_opponent_levels_and_exemptions(self):
        source = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
#include "constants/pokemon.h"
#include "constants/battle.h"
#include "constants/map_types.h"
#define PARTY_SIZE 6
#define MAX_LEVEL 100
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
enum Species {SPECIES_NONE, SPECIES_TEST, SPECIES_EGG};
enum {MON_DATA_SPECIES, MON_DATA_LEVEL, MON_DATA_EXP, MON_DATA_MAX_HP, MON_DATA_HP};
struct Pokemon { enum Species species; u8 level; u32 exp; u16 hp, maxHp; };
static u8 reduction, cap;
static u32 gBattleTypeFlags;
static enum MapBattleScene scene;
static struct { u8 growthRate; } gSpeciesInfo[3];
static u32 gExperienceTables[1][101];
static u8 GetTrainerLevelReduction(void) { return reduction; }
static u32 GetCurrentLevelCap(void) { return cap; }
static enum MapBattleScene GetCurrentMapBattleScene(void) { return scene; }
static u32 GetMonData(struct Pokemon *p,int field) {
    if(field==MON_DATA_SPECIES)return p->species;
    if(field==MON_DATA_LEVEL)return p->level;
    assert(field==MON_DATA_MAX_HP);return p->maxHp;
}
static void SetMonData(struct Pokemon *p,int field,const void *value) {
    if(field==MON_DATA_EXP)p->exp=*(const u32 *)value;
    else if(field==MON_DATA_LEVEL)p->level=*(const u8 *)value;
    else {assert(field==MON_DATA_HP);p->hp=*(const u16 *)value;}
}
static void CalculateMonStats(struct Pokemon *p) {p->maxHp=10+3*p->level;}
'''
        source += function("src/difficulty.c", "ApplyTrainerLevelDifficulty")
        source += r'''
int main(void) {
    const u8 caps[]={14,20,30,40,45,55,60,70,80,100};
    const u32 exclusions[]={BATTLE_TYPE_FRONTIER,BATTLE_TYPE_LINK,BATTLE_TYPE_RECORDED,BATTLE_TYPE_TRAINER_HILL};
    for(int i=0;i<=100;i++)gExperienceTables[0][i]=i*i*i;
    for(unsigned ci=0;ci<sizeof(caps);ci++)for(reduction=0;reduction<=4;reduction+=2) {
        cap=caps[ci];scene=MAP_BATTLE_SCENE_NORMAL;
        gBattleTypeFlags=BATTLE_TYPE_TRAINER|BATTLE_TYPE_DOUBLE;
        struct Pokemon party[6]={{SPECIES_TEST,12},{SPECIES_TEST,100},{SPECIES_EGG,1}};
        ApplyTrainerLevelDifficulty(party);
        assert(party[0].level==cap-reduction);
        assert(party[1].level==100-reduction);
        assert(party[0].exp==gExperienceTables[0][cap-reduction]);
        assert(party[0].hp==10+3*(cap-reduction));
        assert(party[1].hp==party[1].maxHp);
        assert(party[2].level==1 && party[2].hp==0);
        assert(party[3].species==SPECIES_NONE && party[3].level==0);
        // Regeneration on Retry produces the same level, not a second offset.
        party[0]=(struct Pokemon){SPECIES_TEST,12};
        ApplyTrainerLevelDifficulty(party);
        assert(party[0].level==cap-reduction);
        scene=MAP_BATTLE_SCENE_GYM;
        party[0]=(struct Pokemon){SPECIES_TEST,12};
        ApplyTrainerLevelDifficulty(party);assert(party[0].level==12-reduction);
        scene=MAP_BATTLE_SCENE_NORMAL;gBattleTypeFlags=BATTLE_TYPE_TRAINER;
        party[0]=(struct Pokemon){SPECIES_TEST,13};
        ApplyTrainerLevelDifficulty(party);assert(party[0].level==13-reduction);
        for(unsigned j=0;j<sizeof(exclusions)/sizeof(exclusions[0]);j++) {
            gBattleTypeFlags=BATTLE_TYPE_TRAINER|BATTLE_TYPE_DOUBLE|exclusions[j];
            party[0]=(struct Pokemon){SPECIES_TEST,12};
            ApplyTrainerLevelDifficulty(party);assert(party[0].level==12-reduction);
        }
    }
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "floor.c"
            binary = Path(directory) / "floor"
            path.write_text(source)
            compiled = subprocess.run([shutil.which("cc"), "-std=c11", "-fsanitize=undefined",
                            "-I", str(ROOT / "include"), str(path), "-o", str(binary)],
                           capture_output=True, text=True)
            self.assertEqual(compiled.returncode, 0, compiled.stderr)
            subprocess.run([str(binary)], check=True, capture_output=True, text=True)
