"""Execute the actual Continue script refresher on minimal host map/save fixtures.

The whole of src/overworld.c is compiled on the host with the real map and
object-template types; the harness supplies only the save block and the map
group table of other units.
"""
from pathlib import Path
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tests'))
import host_c

# load_save.c and generated map-group boundary: one target map at group 3, map 4.
BOUNDARY = r'''
#include "global.h"
''' + host_c.ASSERTS + r'''
static struct SaveBlock1 sSave;
struct SaveBlock1 *gSaveBlock1Ptr = &sSave;
struct MapHeader gMapHeader;
struct MapHeader gHostTargetMap;
static const struct MapHeader *const sGroup3[] = {NULL, NULL, NULL, NULL, &gHostTargetMap};
const struct MapHeader *const *const gMapGroups[] = {NULL, NULL, NULL, sGroup3};
'''

CASES = r'''
#include <stdlib.h>
#include <string.h>
extern struct MapHeader gHostTargetMap;
static const unsigned char currentScript[]="current", oldScript[]="old", poisonScript[]="poison", cloneScript[]="clone";
#ifdef HOST_ORIGINAL
#define Refresh OriginalLoadSaveblockObjEventScripts
#else
#define Refresh LoadSaveblockObjEventScripts
#endif
int main(int argc, char **argv) {
    unsigned count = (unsigned)atoi(argv[1]);
    int clone = argc > 2 && strcmp(argv[2], "-") != 0;
    // Sentinel entries model adjacent memory beyond the declared object count.
    // A short actual allocation also exercises zero/one-entry bounds below.
    unsigned allocated = count > 64 ? count : (argc > 3 ? 64 : count);
    struct ObjectEventTemplate *source = allocated ? calloc(allocated,sizeof(*source)) : NULL;
    struct MapEvents events = {.objectEventCount = count, .objectEvents = source};
    struct ObjectEventTemplate target = {.localId=7, .script=cloneScript};
    struct MapEvents targetEvents = {.objectEventCount = 1, .objectEvents = &target};
    struct ObjectEventTemplate preserved[OBJECT_EVENT_TEMPLATES_COUNT];
    assert(OBJECT_EVENT_TEMPLATES_COUNT == 64);
    gHostTargetMap.events=&targetEvents; gMapHeader.events=&events;
    for (unsigned i=0;i<allocated;i++) source[i].script = i<count ? currentScript : poisonScript;
    for (unsigned i=0;i<64;i++) {
        gSaveBlock1Ptr->objectEventTemplates[i]=(struct ObjectEventTemplate){
            .localId=i+1,.graphicsId=13,.kind=2,.x=100+i,.y=-20,.elevation=3,.movementType=10,
            .trainerType=11,.trainerRange_berryTreeId=12,.flagId=14,.script=oldScript};
    }
    if (clone) {
        source[0].kind=OBJ_KIND_CLONE; source[0].targetMapGroup=3; source[0].targetMapNum=4;
        source[0].targetLocalId=(u8)atoi(argv[2]); source[0].script=poisonScript;
    }
    memcpy(preserved,gSaveBlock1Ptr->objectEventTemplates,sizeof(preserved));
    Refresh();
    for (unsigned i=0;i<64;i++) {
        const unsigned char *expected=i<count ? currentScript : NULL;
        if (clone && i==0) expected=source[0].targetLocalId==1 ? cloneScript : NULL;
        assert(gSaveBlock1Ptr->objectEventTemplates[i].script==expected);
        preserved[i].script=expected;
        assert(memcmp(&preserved[i],&gSaveBlock1Ptr->objectEventTemplates[i],sizeof(preserved[i]))==0);
    }
    free(source);return 0;
}
'''

ORIGINAL = '''static void OriginalLoadSaveblockObjEventScripts(void) {
    const struct ObjectEventTemplate *mapHeaderObjTemplates=gMapHeader.events->objectEvents;
    struct ObjectEventTemplate *savObjTemplates=gSaveBlock1Ptr->objectEventTemplates;
    for (s32 i=0;i<OBJECT_EVENT_TEMPLATES_COUNT;i++) savObjTemplates[i].script=mapHeaderObjTemplates[i].script;
}'''


class ContinueObjectScripts(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory()
        cls.addClassCleanup(cls.temp.cleanup)
        cls.directory = Path(cls.temp.name)
        cls.fixed = cls.compile('fixed')

    @classmethod
    def compile(cls, name, original=False):
        directory = cls.directory / name
        directory.mkdir()
        # The pre-fix function runs against the same real types and globals.
        return host_c.build(directory, {
            'overworld.c': host_c.production('src/overworld.c') + ORIGINAL + CASES,
            'boundary.c': BOUNDARY,
        }, name=name, defines={'HOST_ORIGINAL': '1'} if original else None)

    def check(self, *args):
        host_c.run(self.fixed, *args, timeout=10)

    def test_zero_short_and_full_maps_preserve_non_script_state(self):
        for count in (0, 1, 2, 63, 64, 65):
            with self.subTest(count=count):
                self.check(count)

    def test_clone_refreshes_target_script_not_clone_header_pointer(self):
        self.check(1, 1)

    def test_invalid_clone_indices_clear_script_without_reading_target(self):
        self.check(1, 0)
        self.check(1, 2)

    def test_original_bug_reproduces_poison_from_beyond_declared_map_count(self):
        executable = self.compile('original', original=True)
        # Padded memory makes the wrong-read reproduction deterministic without
        # relying on a platform sanitizer runtime or undefined allocation bounds.
        with self.assertRaises(AssertionError) as failure:
            host_c.run(executable, '1', '-', 'padded', timeout=10)
        self.assertIn('assert', str(failure.exception).lower())


if __name__ == '__main__':
    unittest.main()
