"""Host regressions executing extracted game functions, not copies of their logic.

The fixture uses actual Pokemon/ItemSlot/BagPocket declarations and constants,
plus actual bag pointer/key-rotation functions. Save-block surroundings and party
count calculation are stubbed. This does not execute GBA callbacks, Pokemon
encryption/accessors, the restart UI, or a complete battle in an emulator.
"""

from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]


def declaration(source, signature):
    """Extract a complete, brace-balanced declaration/function from game code."""
    pattern = r"\s+".join(re.escape(part) for part in signature.split()) + r"\s*\{"
    match = re.search(pattern, source)
    if match is None:
        raise AssertionError(f"cannot locate declaration: {signature}")
    start = match.start()
    opening = match.end() - 1
    depth = 1
    cursor = opening + 1
    while depth:
        depth += (source[cursor] == "{") - (source[cursor] == "}")
        cursor += 1
    if source[cursor:cursor + 1] == ";":
        cursor += 1
    return source[start:cursor] + "\n"


def fixture_source(battle_source=None):
    battle = battle_source or (ROOT / "src/battle_main.c").read_text()
    pokemon = (ROOT / "include/pokemon.h").read_text()
    item = (ROOT / "src/item.c").read_text()
    snapshot_start = battle.index("static EWRAM_DATA struct Pokemon sEcRestartParty")
    snapshot_end = battle.index("void CB2_InitBattle(void)", snapshot_start)
    snapshot = battle[snapshot_start:snapshot_end]
    pokemon_start = pokemon.index("struct PokemonSubstruct0\n{")
    pokemon_end = pokemon.index("struct MonSpritesGfxManager\n{")
    structs = pokemon[pokemon_start:pokemon_end]
    structs += declaration((ROOT / "include/global.h").read_text(), "struct ItemSlot")
    structs += declaration((ROOT / "include/item.h").read_text(), "struct ALIGNED(2) BagPocket")
    config = re.search(r"^#define B_RUN_TRAINER_BATTLE\s+\S+", (ROOT / "include/config/battle.h").read_text(), re.MULTILINE)
    if config is None:
        raise AssertionError("B_RUN_TRAINER_BATTLE configuration declaration missing")
    return (
        PREAMBLE + config.group() + "\n" + structs + ENVIRONMENT
        + declaration((ROOT / "src/load_save.c").read_text(), "void ApplyNewEncryptionKeyToHword(u16 *hWord, u32 newKey)")
        + declaration(item, "static inline struct ItemSlot *NONNULL BagPocket_GetSlotPointer(struct BagPocket *pocket, u32 pocketPos)")
        + declaration(item, "void ApplyNewEncryptionKeyToBagItems(u32 newKey)")
        + declaration(battle, "bool32 CanPlayerForfeitNormalTrainerBattle(void)")
        + snapshot + SCENARIOS
    )


PREAMBLE = r"""
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef u8 bool8;
typedef u32 bool32;
#define TRUE 1
#define FALSE 0
#define EWRAM_DATA
#define NONNULL
#define ALIGNED(n) __attribute__((aligned(n)))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#include "constants/global.h"
#include "constants/species.h"
#include "constants/pokemon.h"
#include "constants/items.h"
#include "constants/item.h"
#include "constants/battle.h"
"""

ENVIRONMENT = r"""
_Static_assert(sizeof(struct Pokemon) == 100, "host Pokemon layout differs from GBA fixture assumptions");
_Static_assert(sizeof(struct ItemSlot) == 4, "host ItemSlot layout differs from GBA fixture assumptions");
static struct Pokemon gParties[MAX_BATTLE_TRAINERS][PARTY_SIZE];
#define gPlayerParty gParties[B_TRAINER_PLAYER]
static u32 gBattleTypeFlags;
// Party-count implementation is outside this extracted snapshot fixture.
static u8 CalculatePlayerPartyCount(void) { return PARTY_SIZE; }
static struct { u32 encryptionKey; } save2;
static typeof(save2) *gSaveBlock2Ptr = &save2;
static struct {
    struct ItemSlot bagPocketMedicine[BAG_MEDICINE_COUNT];
    struct ItemSlot bagPocketBattle[BAG_BATTLE_COUNT];
} save3;
static typeof(save3) *gSaveBlock3Ptr = &save3;
static struct BagPocket gBagPockets[POCKETS_COUNT];
static struct ItemSlot primarySlots[POCKETS_COUNT][2];
static struct ItemSlot overflowSlots[POCKETS_COUNT][2];
#define CHECK(condition, message) do { if (!(condition)) { fprintf(stderr, "%s\n", message); return 1; } } while (0)
"""

SCENARIOS = r"""
static void SeedParty(struct Pokemon *party, unsigned salt)
{
    for (unsigned i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &party[i];
        memset(mon, salt + i, sizeof(*mon));
        mon->box.personality = salt * 1000 + i;
        mon->hp = 40 + i;
        mon->status = 1u << i;
        mon->box.secure.substructs[0].type0.heldItem = ITEM_ORAN_BERRY + i;
        mon->box.secure.substructs[1].type1.pp1 = 10 + i;
        mon->box.secure.substructs[1].type1.pp2 = 15 + i;
        mon->box.secure.substructs[1].type1.pp3 = 20 + i;
        mon->box.secure.substructs[1].type1.pp4 = 25 + i;
    }
}

static void DamageAndReorderParty(struct Pokemon *party)
{
    struct Pokemon first = party[0];
    memmove(party, party + 1, (PARTY_SIZE - 1) * sizeof(*party));
    party[PARTY_SIZE - 1] = first;
    for (unsigned i = 0; i < PARTY_SIZE; i++)
    {
        party[i].hp = 0;
        party[i].status = 0x12345678;
        party[i].box.secure.substructs[0].type0.heldItem = ITEM_NONE;
        party[i].box.secure.substructs[1].type1.pp1 = 0;
        party[i].box.secure.substructs[1].type1.pp2 = 0;
        party[i].box.secure.substructs[1].type1.pp3 = 0;
        party[i].box.secure.substructs[1].type1.pp4 = 0;
    }
}

static void SeedBag(void)
{
    save2.encryptionKey = 0x87651234;
    for (unsigned pocket = 0; pocket < POCKETS_COUNT; pocket++)
    {
        gBagPockets[pocket] = (struct BagPocket) {
            .itemSlots = primarySlots[pocket], .overflowSlots = overflowSlots[pocket],
            .capacity = 4, .primaryCapacity = 2, .id = pocket,
        };
    }
    gBagPockets[POCKET_MEDICINE] = (struct BagPocket) {
        .itemSlots = save3.bagPocketMedicine, .capacity = BAG_MEDICINE_COUNT,
        .primaryCapacity = BAG_MEDICINE_COUNT, .id = POCKET_MEDICINE,
    };
    gBagPockets[POCKET_BATTLE] = (struct BagPocket) {
        .itemSlots = save3.bagPocketBattle, .capacity = BAG_BATTLE_COUNT,
        .primaryCapacity = BAG_BATTLE_COUNT, .id = POCKET_BATTLE,
    };
    for (unsigned pocket = 0; pocket < POCKETS_COUNT; pocket++)
        for (unsigned i = 0; i < gBagPockets[pocket].capacity; i++)
            *BagPocket_GetSlotPointer(&gBagPockets[pocket], i) = (struct ItemSlot) {
                .itemId = ITEM_POTION + pocket,
                .quantity = (u16)(i + 1) ^ (u16)save2.encryptionKey,
            };
}

static int PartyScenario(bool32 partner, bool32 repeat)
{
    struct Pokemon originalPlayer[PARTY_SIZE], originalPartner[PARTY_SIZE];
    struct Pokemon opponentA[PARTY_SIZE], opponentB[PARTY_SIZE], untouchedPartner[PARTY_SIZE];
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
    if (partner)
        gBattleTypeFlags |= BATTLE_TYPE_INGAME_PARTNER | BATTLE_TYPE_MULTI | BATTLE_TYPE_TWO_OPPONENTS;
    for (unsigned trainer = 0; trainer < MAX_BATTLE_TRAINERS; trainer++)
        SeedParty(gParties[trainer], 11 + trainer);
    memcpy(originalPlayer, gPlayerParty, sizeof(originalPlayer));
    memcpy(originalPartner, gParties[B_TRAINER_PARTNER], sizeof(originalPartner));
    EcSnapshotForRestart();
    for (unsigned trial = 0; trial < (repeat ? 8u : 1u); trial++)
    {
        for (unsigned trainer = 0; trainer < MAX_BATTLE_TRAINERS; trainer++)
            DamageAndReorderParty(gParties[trainer]);
        memcpy(opponentA, gParties[B_TRAINER_OPPONENT_A], sizeof(opponentA));
        memcpy(opponentB, gParties[B_TRAINER_OPPONENT_B], sizeof(opponentB));
        memcpy(untouchedPartner, gParties[B_TRAINER_PARTNER], sizeof(untouchedPartner));
        EcRestoreForRestart();
        CHECK(memcmp(originalPlayer, gPlayerParty, sizeof(originalPlayer)) == 0,
              "restart did not restore exact player HP/status/PP/items/order/other bytes");
        CHECK(memcmp(partner ? originalPartner : untouchedPartner, gParties[B_TRAINER_PARTNER], sizeof(originalPartner)) == 0,
              "restart did not restore exact AI partner state, or touched a nonpartner party");
        CHECK(memcmp(opponentA, gParties[B_TRAINER_OPPONENT_A], sizeof(opponentA)) == 0,
              "snapshot restore unexpectedly mutated opponent A");
        CHECK(memcmp(opponentB, gParties[B_TRAINER_OPPONENT_B], sizeof(opponentB)) == 0,
              "snapshot restore unexpectedly mutated opponent B");
        // CB2_InitBattle takes another snapshot after a restart restores state.
        EcSnapshotForRestart();
    }
    return 0;
}

static int BagKeyScenario(void)
{
    static const u32 keys[] = {0x9876abcd, 0xffff0000, 0, 0x1357ffff, 0x24681234};
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
    SeedBag();
    EcSnapshotForRestart();
    for (unsigned trial = 0; trial < sizeof(keys) / sizeof(keys[0]); trial++)
    {
        // Execute the real item encryption rotation, then publish its new key
        // in the same order used by MoveSaveBlocks_ResetHeap's save migration.
        ApplyNewEncryptionKeyToBagItems(keys[trial]);
        save2.encryptionKey = keys[trial];
        typeof(save3) beforeSave3 = save3;
        struct ItemSlot beforePrimary[POCKETS_COUNT][2], beforeOverflow[POCKETS_COUNT][2];
        memcpy(beforePrimary, primarySlots, sizeof(beforePrimary));
        memcpy(beforeOverflow, overflowSlots, sizeof(beforeOverflow));
        EcRestoreForRestart();
        CHECK(save2.encryptionKey == keys[trial], "restart changed the live save encryption key");
        CHECK(memcmp(&beforeSave3, &save3, sizeof(save3)) == 0,
              "restart overwrote encrypted medicine/battle bag bytes after key rotation");
        CHECK(memcmp(beforePrimary, primarySlots, sizeof(beforePrimary)) == 0,
              "restart mutated a primary bag pocket");
        CHECK(memcmp(beforeOverflow, overflowSlots, sizeof(beforeOverflow)) == 0,
              "restart mutated an overflow bag pocket");
        for (unsigned pocket = 0; pocket < POCKETS_COUNT; pocket++)
            for (unsigned i = 0; i < gBagPockets[pocket].capacity; i++)
            {
                struct ItemSlot slot = *BagPocket_GetSlotPointer(&gBagPockets[pocket], i);
                CHECK((slot.quantity ^ (u16)save2.encryptionKey) == i + 1,
                      "restart changed a decoded bag quantity across a key rotation");
                CHECK(slot.itemId == ITEM_POTION + pocket, "restart changed a bag item identity");
            }
        EcSnapshotForRestart();
    }
    return 0;
}

static int EligibilityScenario(void)
{
    const u32 allowed[] = {
        BATTLE_TYPE_TRAINER,
        BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE,
        BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_INGAME_PARTNER | BATTLE_TYPE_MULTI,
        BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_TWO_OPPONENTS,
    };
    const u32 denied[] = {
        BATTLE_TYPE_LINK, BATTLE_TYPE_SAFARI, BATTLE_TYPE_FIRST_BATTLE,
        BATTLE_TYPE_CATCH_TUTORIAL, BATTLE_TYPE_ROAMER, BATTLE_TYPE_EREADER_TRAINER,
        BATTLE_TYPE_LEGENDARY, BATTLE_TYPE_RECORDED, BATTLE_TYPE_TRAINER_HILL,
        BATTLE_TYPE_SECRET_BASE, BATTLE_TYPE_BATTLE_TOWER, BATTLE_TYPE_DOME,
        BATTLE_TYPE_PALACE, BATTLE_TYPE_ARENA, BATTLE_TYPE_FACTORY,
        BATTLE_TYPE_PIKE, BATTLE_TYPE_PYRAMID,
    };
    for (unsigned i = 0; i < sizeof(allowed) / sizeof(allowed[0]); i++)
    {
        gBattleTypeFlags = allowed[i];
        CHECK(CanPlayerForfeitNormalTrainerBattle(), "normal/doubles/partner trainer retry eligibility lost");
        gBattleTypeFlags &= ~BATTLE_TYPE_TRAINER;
        CHECK(!CanPlayerForfeitNormalTrainerBattle(), "nontrainer battle incorrectly eligible");
    }
    for (unsigned i = 0; i < sizeof(denied) / sizeof(denied[0]); i++)
    {
        gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE | denied[i];
        CHECK(!CanPlayerForfeitNormalTrainerBattle(), "excluded link/Frontier/tutorial/context incorrectly eligible");
    }
    return 0;
}

static int NewBattleScenario(void)
{
    struct Pokemon expected[PARTY_SIZE];
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_INGAME_PARTNER;
    SeedParty(gPlayerParty, 10);
    SeedParty(gParties[B_TRAINER_PARTNER], 20);
    EcSnapshotForRestart();
    // A later trainer battle must replace, not reuse, the previous snapshot.
    SeedParty(gPlayerParty, 30);
    SeedParty(gParties[B_TRAINER_PARTNER], 40);
    memcpy(expected, gPlayerParty, sizeof(expected));
    EcSnapshotForRestart();
    DamageAndReorderParty(gPlayerParty);
    DamageAndReorderParty(gParties[B_TRAINER_PARTNER]);
    EcRestoreForRestart();
    CHECK(memcmp(expected, gPlayerParty, sizeof(expected)) == 0, "new trainer reused an old player snapshot");
    CHECK(gParties[B_TRAINER_PARTNER][0].box.personality == 40000, "new trainer reused an old partner snapshot");
    // Follow a partner battle with a normal battle: stale partner snapshot
    // must not touch unrelated partner storage in the normal context.
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    EcSnapshotForRestart();
    SeedParty(gParties[B_TRAINER_PARTNER], 50);
    memcpy(expected, gParties[B_TRAINER_PARTNER], sizeof(expected));
    EcRestoreForRestart();
    CHECK(memcmp(expected, gParties[B_TRAINER_PARTNER], sizeof(expected)) == 0, "normal battle restored stale partner state");
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!strcmp(argv[1], "player")) return PartyScenario(FALSE, FALSE);
    if (!strcmp(argv[1], "partner")) return PartyScenario(TRUE, FALSE);
    if (!strcmp(argv[1], "repeated-player")) return PartyScenario(FALSE, TRUE);
    if (!strcmp(argv[1], "repeated-partner")) return PartyScenario(TRUE, TRUE);
    if (!strcmp(argv[1], "bag-key")) return BagKeyScenario();
    if (!strcmp(argv[1], "eligibility")) return EligibilityScenario();
    if (!strcmp(argv[1], "new-battle")) return NewBattleScenario();
    return 2;
}
"""


class RestartIntegrityTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        compiler = shutil.which("cc")
        if compiler is None:
            raise AssertionError("host C compiler required for restart integrity tests")
        cls.directory = tempfile.TemporaryDirectory(prefix="ec-restart-integrity-")
        cls.addClassCleanup(cls.directory.cleanup)
        directory = Path(cls.directory.name)
        source = directory / "restart.c"
        source.write_text(fixture_source())
        cls.executable = directory / "restart"
        result = subprocess.run(
            [compiler, "-std=gnu11", "-Wall", "-Wextra", "-Wno-unused-variable",
             "-I", str(ROOT / "include"), str(source), "-o", str(cls.executable)],
            capture_output=True, text=True,
        )
        if result.returncode:
            raise AssertionError(f"actual-source host fixture failed to compile:\n{result.stdout}{result.stderr}")

    def scenario(self, name):
        result = subprocess.run([str(self.executable), name], capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, f"{name}: {result.stdout}{result.stderr}")

    def test_player_exact_restoration_does_not_touch_nonpartner(self):
        self.scenario("player")

    def test_partner_exact_restoration(self):
        self.scenario("partner")

    def test_repeated_player_retries(self):
        self.scenario("repeated-player")

    def test_repeated_partner_retries(self):
        self.scenario("repeated-partner")

    def test_bag_quantities_survive_live_encryption_key_changes(self):
        self.scenario("bag-key")

    def test_actual_eligibility_excludes_link_frontier_and_tutorial(self):
        self.scenario("eligibility")

    def test_new_battle_replaces_snapshot_and_does_not_reuse_partner_state(self):
        self.scenario("new-battle")


if __name__ == "__main__":
    unittest.main()
