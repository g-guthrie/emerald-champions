#include "global.h"
#include "pokemon_storage_system.h"
#include "test/test.h"

// Preserve the legacy prefixes used by Bag migration. SaveBlock1 may grow
// through its trailing Mega Stone array; src/save.c enforces sector bounds.
#define T_SAVEBLOCK1_LEGACY_SIZE 15568
#define T_SAVEBLOCK2_LEGACY_SIZE 3884
#define T_SAVEBLOCK3_LEGACY_SIZE 4
#define T_SAVEBLOCK2_SIZE 3928
#define T_SAVEBLOCK3_SIZE 1624
#define T_POKEMONSTORAGE_SIZE 34144

TEST("SaveBlock1 preserves the legacy prefix before its Bag extension")
{
    EXPECT_EQ(offsetof(struct SaveBlock1, bagExtension), T_SAVEBLOCK1_LEGACY_SIZE);
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
#undef T_SAVEBLOCK2_SIZE
#undef T_SAVEBLOCK3_SIZE
#undef T_POKEMONSTORAGE_SIZE
