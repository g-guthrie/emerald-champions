"""Execute the actual Continue script refresher on minimal host map/save fixtures."""
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
PRELUDE = r'''
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
typedef uint8_t u8;
typedef uint32_t u32;
typedef int32_t s32;
#define OBJECT_EVENT_TEMPLATES_COUNT 64
#define OBJ_KIND_CLONE 255
struct ObjectEventTemplate {
    u8 localId, kind, targetLocalId, targetMapNum, targetMapGroup, movementType;
    int16_t x, y;
    const unsigned char *script;
};
struct MapEvents { u32 objectEventCount; const struct ObjectEventTemplate *objectEvents; };
struct MapHeader { const struct MapEvents *events; } gMapHeader, targetMap;
struct Save { struct ObjectEventTemplate objectEventTemplates[64]; } save, *gSaveBlock1Ptr=&save;
static const unsigned char currentScript[]="current", oldScript[]="old", poisonScript[]="poison", cloneScript[]="clone";
static const struct MapHeader *Overworld_GetMapHeaderByGroupAndId(u8 group, u8 num) {
    assert(group==3 && num==4); return &targetMap;
}
'''
CASES = r'''
int main(int argc, char **argv) {
    unsigned count = (unsigned)atoi(argv[1]);
    int clone = argc > 2 && strcmp(argv[2], "-") != 0;
    // Sentinel entries model adjacent memory beyond the declared object count.
    // A short actual allocation also exercises zero/one-entry bounds below.
    unsigned allocated = count > 64 ? count : (argc > 3 ? 64 : count);
    struct ObjectEventTemplate *source = allocated ? calloc(allocated,sizeof(*source)) : NULL;
    struct MapEvents events = {count,source};
    struct ObjectEventTemplate target = {.localId=7, .script=cloneScript};
    struct MapEvents targetEvents = {1,&target};
    struct ObjectEventTemplate preserved[64];
    targetMap.events=&targetEvents; gMapHeader.events=&events;
    for (unsigned i=0;i<allocated;i++) source[i].script = i<count ? currentScript : poisonScript;
    for (unsigned i=0;i<64;i++) {
        save.objectEventTemplates[i]=(struct ObjectEventTemplate){
            .localId=i+1,.kind=2,.movementType=10,.x=100+i,.y=-20,
            .targetLocalId=8,.targetMapNum=9,.targetMapGroup=10,.script=oldScript};
    }
    if (clone) {
        source[0].kind=OBJ_KIND_CLONE; source[0].targetMapGroup=3; source[0].targetMapNum=4;
        source[0].targetLocalId=(u8)atoi(argv[2]); source[0].script=poisonScript;
    }
    memcpy(preserved,save.objectEventTemplates,sizeof(preserved));
    LoadSaveblockObjEventScripts();
    for (unsigned i=0;i<64;i++) {
        const unsigned char *expected=i<count ? currentScript : NULL;
        if (clone && i==0) expected=source[0].targetLocalId==1 ? cloneScript : NULL;
        assert(save.objectEventTemplates[i].script==expected);
        preserved[i].script=expected;
        assert(memcmp(&preserved[i],&save.objectEventTemplates[i],sizeof(preserved[i]))==0);
    }
    free(source);return 0;
}
'''


class ContinueObjectScripts(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory()
        cls.addClassCleanup(cls.temp.cleanup)
        cls.directory = Path(cls.temp.name)
        source = (ROOT / 'src/overworld.c').read_text()
        cls.function = re.search(r'void LoadSaveblockObjEventScripts\(void\)\n\{.*?\n\}', source, re.S)[0]
        cls.fixed = cls.compile(cls.function, 'fixed')

    @classmethod
    def compile(cls, function, name):
        compiler = shutil.which('cc')
        if compiler is None:
            raise RuntimeError('host C compiler required')
        source = cls.directory / (name + '.c')
        executable = cls.directory / name
        source.write_text(PRELUDE + function + CASES)
        subprocess.run([compiler, '-std=c11', '-Wall', '-Wextra', str(source), '-o', str(executable)], check=True, capture_output=True)
        return executable

    def check(self, *args):
        result = subprocess.run([str(self.fixed), *map(str,args)], capture_output=True, text=True, timeout=10)
        self.assertEqual(result.returncode, 0, result.stderr)

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
        old = '''void LoadSaveblockObjEventScripts(void) {
            const struct ObjectEventTemplate *mapHeaderObjTemplates=gMapHeader.events->objectEvents;
            struct ObjectEventTemplate *savObjTemplates=gSaveBlock1Ptr->objectEventTemplates;
            for (s32 i=0;i<OBJECT_EVENT_TEMPLATES_COUNT;i++) savObjTemplates[i].script=mapHeaderObjTemplates[i].script;
        }'''
        executable = self.compile(old, 'original')
        # Padded memory makes the wrong-read reproduction deterministic without
        # relying on a platform sanitizer runtime or undefined allocation bounds.
        result = subprocess.run([str(executable), '1', '-', 'padded'], capture_output=True, text=True, timeout=10)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('assert', result.stderr.lower())


if __name__ == '__main__':
    unittest.main()
