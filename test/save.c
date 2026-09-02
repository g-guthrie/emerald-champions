#include "global.h"
#include "pokemon_storage_system.h"
#include "test/test.h"

// Preserve every legacy byte offset while pinning the intentional append-only
// segmented-Bag extensions and the sector-size safety contract.
#define T_SAVEBLOCK1_LEGACY_SIZE 15568
#define T_SAVEBLOCK2_LEGACY_SIZE 3884
#define T_SAVEBLOCK3_LEGACY_SIZE 4
#define T_SAVEBLOCK1_SIZE 15836
#define T_SAVEBLOCK2_SIZE 3928
#define T_SAVEBLOCK3_SIZE 1624
#define T_POKEMONSTORAGE_SIZE 34144

TEST("SaveBlock1 is backwards compatible")
{
    EXPECT_EQ(offsetof(struct SaveBlock1, bagExtension), T_SAVEBLOCK1_LEGACY_SIZE);
    EXPECT_EQ(sizeof(struct SaveBlock1), T_SAVEBLOCK1_SIZE);
}

TEST("SaveBlock2 is backwards compatible")
{
    EXPECT_EQ(offsetof(struct SaveBlock2, bagPocketPokeBalls), T_SAVEBLOCK2_LEGACY_SIZE);
    EXPECT_EQ(sizeof(struct SaveBlock2), T_SAVEBLOCK2_SIZE);
}

TEST("SaveBlock3 is backwards compatible")
{
    EXPECT_EQ(offsetof(struct SaveBlock3, bagPocketLayoutMagic), T_SAVEBLOCK3_LEGACY_SIZE);
    EXPECT_EQ(sizeof(struct SaveBlock3), T_SAVEBLOCK3_SIZE);
}

TEST("PokemonStorage is backwards compatible")
{
    EXPECT_EQ(sizeof(struct PokemonStorage), T_POKEMONSTORAGE_SIZE);
}

#undef T_SAVEBLOCK1_LEGACY_SIZE
#undef T_SAVEBLOCK2_LEGACY_SIZE
#undef T_SAVEBLOCK3_LEGACY_SIZE
#undef T_SAVEBLOCK1_SIZE
#undef T_SAVEBLOCK2_SIZE
#undef T_SAVEBLOCK3_SIZE
#undef T_POKEMONSTORAGE_SIZE
