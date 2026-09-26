#include "global.h"
#include "move.h"
#include "champions_circuit.h"
#include "malloc.h"
#include "apprentice.h"
#include "battle.h"
#include "battle_ai_util.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_message.h"
#include "battle_pike.h"
#include "battle_pyramid.h"
#include "battle_setup.h"
#include "battle_tower.h"
#include "battle_z_move.h"
#include "caps.h"
#include "data.h"
#include "daycare.h"
#include "dexnav.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "evolution_scene.h"
#include "field_player_avatar.h"
#include "field_specials.h"
#include "field_weather.h"
#include "fishing.h"
#include "follower_npc.h"
#include "frontier_util.h"
#include "graphics.h"
#include "item.h"
#include "link.h"
#include "m4a.h"
#include "main.h"
#include "mail.h"
#include "move_relearner.h"
#include "naming_screen.h"
#include "overworld.h"
#include "ow_abilities.h"
#include "party_menu.h"
#include "pokedex.h"
#include "pokeblock.h"
#include "pokemon.h"
#include "pokemon_animation.h"
#include "pokemon_icon.h"
#include "pokemon_summary_screen.h"
#include "pokemon_storage_system.h"
#include "pokerus.h"
#include "random.h"
#include "random_mon_generation.h"
#include "recorded_battle.h"
#include "regions.h"
#include "rtc.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "test_runner.h"
#include "text.h"
#include "trainer.h"
#include "trainer_hill.h"
#include "util.h"
#include "constants/abilities.h"
#include "constants/battle_frontier.h"
#include "constants/battle_move_effects.h"
#include "constants/battle_partner.h"
#include "constants/battle_script_commands.h"
#include "constants/battle_string_ids.h"
#include "constants/cries.h"
#include "constants/event_objects.h"
#include "constants/form_change_types.h"
#include "constants/item_effects.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/moves.h"
#include "constants/party_menu.h"
#include "constants/regions.h"
#include "constants/songs.h"
#include "constants/trainers.h"
#include "constants/union_room.h"
#include "constants/weather.h"

extern enum Item gSpecialVar_ItemId;

struct SpeciesItem
{
    enum Species species;
    enum Item item;
};

struct MonSpritesGfxManager
{
    u32 numSprites:4;
    u32 numFrames:8;
    u32 active:8;
    u32 dataSize:4;
    u32 mode:4; // MON_SPR_GFX_MODE_*
    void *spriteBuffer;
    u8 **spritePointers;
    struct SpriteTemplate *templates;
    struct SpriteFrameImage *frameImages;
};

static u16 CalculateBoxMonChecksum(struct BoxPokemon *boxMon);
static u16 CalculateBoxMonChecksumDecrypt(struct BoxPokemon *boxMon);
static u16 CalculateBoxMonChecksumReencrypt(struct BoxPokemon *boxMon);
static union PokemonSubstruct *GetSubstruct(struct BoxPokemon *boxMon, u32 personality, enum SubstructType substructType);
static void EncryptBoxMon(struct BoxPokemon *boxMon);
static void DecryptBoxMon(struct BoxPokemon *boxMon);
static void Task_PlayMapChosenOrBattleBGM(u8 taskId);
void TrySpecialOverworldEvo();

EWRAM_DATA static u8 sLearningMoveTableID = 0;
EWRAM_DATA u8 gPartiesCount[MAX_BATTLE_TRAINERS] = {0};
EWRAM_DATA struct Pokemon gParties[MAX_BATTLE_TRAINERS][PARTY_SIZE] = {0};
EWRAM_DATA struct SpriteTemplate gMultiuseSpriteTemplate = {0};
EWRAM_DATA static struct MonSpritesGfxManager *sMonSpritesGfxManagers[MON_SPR_GFX_MANAGERS_COUNT] = {NULL};
EWRAM_DATA u8 gTriedEvolving = 0;
EWRAM_DATA u16 gFollowerSteps = 0;

struct Pokemon (*const gPlayerPartyPtr)[6] = &gParties[B_TRAINER_PLAYER];
u8 (*const gPlayerPartyCountPtr) = &gPartiesCount[B_TRAINER_PLAYER];
struct Pokemon (*const gEnemyPartyPtr)[6] = &gParties[B_TRAINER_OPPONENT_A];
u8 (*const gEnemyPartyCountPtr) = &gPartiesCount[B_TRAINER_OPPONENT_A];

#include "data/abilities.h"

// Inclement layer: Inclement Emerald's base stat changes and extra Abilities,
// read only by Pokemon that are not trainer-owned (see IsMonTrainerOwned).
struct InclementSpeciesLayer
{
    bool8 hasBaseStats;
    u8 baseStats[NUM_STATS];
    u16 addedAbilities[NUM_INCLEMENT_ABILITY_SLOTS]; // Offered as ABILITY_SLOT_INCLEMENT + i.
};

#include "data/pokemon/inclement_layer.h"

// Used in an unreferenced function in RS.
// Unreferenced here and in FRLG.
struct CombinedMove
{
    enum Move move1;
    enum Move move2;
    enum Move newMove;
};

static const struct CombinedMove sCombinedMoves[2] =
{
    {MOVE_EMBER, MOVE_GUST, MOVE_HEAT_WAVE},
    {0xFFFF, 0xFFFF, 0xFFFF}
};

// NOTE: The order of the elements in the array below is irrelevant.
// To reorder the pokedex, see the values in include/constants/pokedex.h.

#define KANTO_TO_NATIONAL(name)     [KANTO_DEX_##name - 1] = NATIONAL_DEX_##name,
#define HOENN_TO_NATIONAL(name)     [HOENN_DEX_##name - 1] = NATIONAL_DEX_##name,

static const enum NationalDexOrder sKantoToNationalOrder[KANTO_DEX_COUNT - 1] =
{
    FOREACH_SPECIES_IN_KANTO_DEX_ORDER(KANTO_TO_NATIONAL)
};


// Assigns all Hoenn Dex Indexes to a National Dex Index
static const enum NationalDexOrder sHoennToNationalOrder[HOENN_DEX_COUNT - 1] =
{
    FOREACH_SPECIES_IN_HOENN_DEX_ORDER(HOENN_TO_NATIONAL)
};

// In Battle Palace, moves are chosen based on the Pokémon's nature rather than by the player
// Moves are grouped into "Attack", "Defense", or "Support" (see PALACE_MOVE_GROUP_*)
// Each nature has a certain percent chance of selecting a move from a particular group
// and a separate percent chance for each group when at or below 50% HP
// The table below doesn't list percentages for Support because you can subtract the other two
// Support percentages are listed in comments off to the side instead
#define PALACE_STYLE(atk, def, atkLow, defLow) {atk, atk + def, atkLow, atkLow + defLow}

const struct NatureInfo gNaturesInfo[NUM_NATURES] =
{
    [NATURE_HARDY] =
    {
        .name = COMPOUND_STRING("Hardy"),
        .statUp = STAT_ATK,
        .statDown = STAT_ATK,
        .backAnim = 0,
        .pokeBlockAnim = {ANIM_HARDY, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlAttackHighAttackLow,
        .battlePalacePercents = PALACE_STYLE(61, 7, 61, 7), //32% support >= 50% HP, 32% support < 50% HP
        .battlePalaceFlavorText = B_MSG_EAGER_FOR_MORE,
        .battlePalaceSmokescreen = PALACE_TARGET_STRONGER,
    },
    [NATURE_LONELY] =
    {
        .name = COMPOUND_STRING("Lonely"),
        .statUp = STAT_ATK,
        .statDown = STAT_DEF,
        .backAnim = 2,
        .pokeBlockAnim = {ANIM_LONELY, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlSupportHighAttackLow,
        .battlePalacePercents = PALACE_STYLE(20, 25, 84, 8), //55%,  8%
        .battlePalaceFlavorText = B_MSG_GLINT_IN_EYE,
        .battlePalaceSmokescreen = PALACE_TARGET_STRONGER,
    },
    [NATURE_BRAVE] =
    {
        .name = COMPOUND_STRING("Brave"),
        .statUp = STAT_ATK,
        .statDown = STAT_SPEED,
        .backAnim = 0,
        .pokeBlockAnim = {ANIM_BRAVE, AFFINE_TURN_UP},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlAttackHighDefenseLow,
        .battlePalacePercents = PALACE_STYLE(70, 15, 32, 60), //15%, 8%
        .battlePalaceFlavorText = B_MSG_GETTING_IN_POS,
        .battlePalaceSmokescreen = PALACE_TARGET_WEAKER,
    },
    [NATURE_ADAMANT] =
    {
        .name = COMPOUND_STRING("Adamant"),
        .statUp = STAT_ATK,
        .statDown = STAT_SPATK,
        .backAnim = 0,
        .pokeBlockAnim = {ANIM_ADAMANT, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlAttackHighAttackLow,
        .battlePalacePercents = PALACE_STYLE(38, 31, 70, 15), //31%, 15%
        .battlePalaceFlavorText = B_MSG_GLINT_IN_EYE,
        .battlePalaceSmokescreen = PALACE_TARGET_STRONGER,
    },
    [NATURE_NAUGHTY] =
    {
        .name = COMPOUND_STRING("Naughty"),
        .statUp = STAT_ATK,
        .statDown = STAT_SPDEF,
        .backAnim = 0,
        .pokeBlockAnim = {ANIM_NAUGHTY, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlDefenseHighAttackLow,
        .battlePalacePercents = PALACE_STYLE(20, 70, 70, 22), //10%, 8%
        .battlePalaceFlavorText = B_MSG_GLINT_IN_EYE,
        .battlePalaceSmokescreen = PALACE_TARGET_WEAKER,
    },
    [NATURE_BOLD] =
    {
        .name = COMPOUND_STRING("Bold"),
        .statUp = STAT_DEF,
        .statDown = STAT_ATK,
        .backAnim = 1,
        .pokeBlockAnim = {ANIM_BOLD, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlSupportHighDefenseLow,
        .battlePalacePercents = PALACE_STYLE(30, 20, 32, 58), //50%, 10%
        .battlePalaceFlavorText = B_MSG_GETTING_IN_POS,
        .battlePalaceSmokescreen = PALACE_TARGET_WEAKER,
    },
    [NATURE_DOCILE] =
    {
        .name = COMPOUND_STRING("Docile"),
        .statUp = STAT_DEF,
        .statDown = STAT_DEF,
        .backAnim = 1,
        .pokeBlockAnim = {ANIM_DOCILE, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlAttackHighAttackLow,
        .battlePalacePercents = PALACE_STYLE(56, 22, 56, 22), //22%, 22%
        .battlePalaceFlavorText = B_MSG_EAGER_FOR_MORE,
        .battlePalaceSmokescreen = PALACE_TARGET_RANDOM,
    },
    [NATURE_RELAXED] =
    {
        .name = COMPOUND_STRING("Relaxed"),
        .statUp = STAT_DEF,
        .statDown = STAT_SPEED,
        .backAnim = 1,
        .pokeBlockAnim = {ANIM_RELAXED, AFFINE_TURN_UP_AND_DOWN},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlSupportHighAttackLow,
        .battlePalacePercents = PALACE_STYLE(25, 15, 75, 15), //60%, 10%
        .battlePalaceFlavorText = B_MSG_GLINT_IN_EYE,
        .battlePalaceSmokescreen = PALACE_TARGET_STRONGER,
    },
    [NATURE_IMPISH] =
    {
        .name = COMPOUND_STRING("Impish"),
        .statUp = STAT_DEF,
        .statDown = STAT_SPATK,
        .backAnim = 0,
        .pokeBlockAnim = {ANIM_IMPISH, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlAttackHighDefenseLow,
        .battlePalacePercents = PALACE_STYLE(69, 6, 28, 55), //25%, 17%
        .battlePalaceFlavorText = B_MSG_GETTING_IN_POS,
        .battlePalaceSmokescreen = PALACE_TARGET_STRONGER,
    },
    [NATURE_LAX] =
    {
        .name = COMPOUND_STRING("Lax"),
        .statUp = STAT_DEF,
        .statDown = STAT_SPDEF,
        .backAnim = 1,
        .pokeBlockAnim = {ANIM_LAX, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlSupportHighSupportLow,
        .battlePalacePercents = PALACE_STYLE(35, 10, 29, 6), //55%, 65%
        .battlePalaceFlavorText = B_MSG_GROWL_DEEPLY,
        .battlePalaceSmokescreen = PALACE_TARGET_STRONGER,
    },
    [NATURE_TIMID] =
    {
        .name = COMPOUND_STRING("Timid"),
        .statUp = STAT_SPEED,
        .statDown = STAT_ATK,
        .backAnim = 2,
        .pokeBlockAnim = {ANIM_TIMID, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlAttackHighSupportLow,
        .battlePalacePercents = PALACE_STYLE(62, 10, 30, 20), //28%, 50%
        .battlePalaceFlavorText = B_MSG_GROWL_DEEPLY,
        .battlePalaceSmokescreen = PALACE_TARGET_WEAKER,
    },
    [NATURE_HASTY] =
    {
        .name = COMPOUND_STRING("Hasty"),
        .statUp = STAT_SPEED,
        .statDown = STAT_DEF,
        .backAnim = 0,
        .pokeBlockAnim = {ANIM_HASTY, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlAttackHighAttackLow,
        .battlePalacePercents = PALACE_STYLE(58, 37, 88, 6), //5%, 6%
        .battlePalaceFlavorText = B_MSG_GLINT_IN_EYE,
        .battlePalaceSmokescreen = PALACE_TARGET_WEAKER,
    },
    [NATURE_SERIOUS] =
    {
        .name = COMPOUND_STRING("Serious"),
        .statUp = STAT_SPEED,
        .statDown = STAT_SPEED,
        .backAnim = 1,
        .pokeBlockAnim = {ANIM_SERIOUS, AFFINE_TURN_DOWN},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlSupportHighSupportLow,
        .battlePalacePercents = PALACE_STYLE(34, 11, 29, 11), //55%, 60%
        .battlePalaceFlavorText = B_MSG_EAGER_FOR_MORE,
        .battlePalaceSmokescreen = PALACE_TARGET_WEAKER,
    },
    [NATURE_JOLLY] =
    {
        .name = COMPOUND_STRING("Jolly"),
        .statUp = STAT_SPEED,
        .statDown = STAT_SPATK,
        .backAnim = 0,
        .pokeBlockAnim = {ANIM_JOLLY, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlSupportHighDefenseLow,
        .battlePalacePercents = PALACE_STYLE(35, 5, 35, 60), //60%, 5%
        .battlePalaceFlavorText = B_MSG_GETTING_IN_POS,
        .battlePalaceSmokescreen = PALACE_TARGET_STRONGER,
    },
    [NATURE_NAIVE] =
    {
        .name = COMPOUND_STRING("Naive"),
        .statUp = STAT_SPEED,
        .statDown = STAT_SPDEF,
        .backAnim = 0,
        .pokeBlockAnim = {ANIM_NAIVE, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlAttackHighAttackLow,
        .battlePalacePercents = PALACE_STYLE(56, 22, 56, 22), //22%, 22%
        .battlePalaceFlavorText = B_MSG_EAGER_FOR_MORE,
        .battlePalaceSmokescreen = PALACE_TARGET_RANDOM,
    },
    [NATURE_MODEST] =
    {
        .name = COMPOUND_STRING("Modest"),
        .statUp = STAT_SPATK,
        .statDown = STAT_ATK,
        .backAnim = 2,
        .pokeBlockAnim = {ANIM_MODEST, AFFINE_TURN_DOWN_SLOW},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlDefenseHighDefenseLow,
        .battlePalacePercents = PALACE_STYLE(35, 45, 34, 60), //20%, 6%
        .battlePalaceFlavorText = B_MSG_GETTING_IN_POS,
        .battlePalaceSmokescreen = PALACE_TARGET_WEAKER,
    },
    [NATURE_MILD] =
    {
        .name = COMPOUND_STRING("Mild"),
        .statUp = STAT_SPATK,
        .statDown = STAT_DEF,
        .backAnim = 2,
        .pokeBlockAnim = {ANIM_MILD, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlDefenseHighSupportLow,
        .battlePalacePercents = PALACE_STYLE(44, 50, 34, 6), //6%, 60%
        .battlePalaceFlavorText = B_MSG_GROWL_DEEPLY,
        .battlePalaceSmokescreen = PALACE_TARGET_STRONGER,
    },
    [NATURE_QUIET] =
    {
        .name = COMPOUND_STRING("Quiet"),
        .statUp = STAT_SPATK,
        .statDown = STAT_SPEED,
        .backAnim = 2,
        .pokeBlockAnim = {ANIM_QUIET, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlAttackHighAttackLow,
        .battlePalacePercents = PALACE_STYLE(56, 22, 56, 22), //22%, 22%
        .battlePalaceFlavorText = B_MSG_EAGER_FOR_MORE,
        .battlePalaceSmokescreen = PALACE_TARGET_WEAKER,
    },
    [NATURE_BASHFUL] =
    {
        .name = COMPOUND_STRING("Bashful"),
        .statUp = STAT_SPATK,
        .statDown = STAT_SPATK,
        .backAnim = 2,
        .pokeBlockAnim = {ANIM_BASHFUL, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlDefenseHighDefenseLow,
        .battlePalacePercents = PALACE_STYLE(30, 58, 30, 58), //12%, 12%
        .battlePalaceFlavorText = B_MSG_EAGER_FOR_MORE,
        .battlePalaceSmokescreen = PALACE_TARGET_WEAKER,
    },
    [NATURE_RASH] =
    {
        .name = COMPOUND_STRING("Rash"),
        .statUp = STAT_SPATK,
        .statDown = STAT_SPDEF,
        .backAnim = 1,
        .pokeBlockAnim = {ANIM_RASH, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlSupportHighSupportLow,
        .battlePalacePercents = PALACE_STYLE(30, 13, 27, 6), //57%, 67%
        .battlePalaceFlavorText = B_MSG_GROWL_DEEPLY,
        .battlePalaceSmokescreen = PALACE_TARGET_STRONGER,
    },
    [NATURE_CALM] =
    {
        .name = COMPOUND_STRING("Calm"),
        .statUp = STAT_SPDEF,
        .statDown = STAT_ATK,
        .backAnim = 1,
        .pokeBlockAnim = {ANIM_CALM, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlDefenseHighDefenseLow,
        .battlePalacePercents = PALACE_STYLE(40, 50, 25, 62), //10%, 13%
        .battlePalaceFlavorText = B_MSG_GETTING_IN_POS,
        .battlePalaceSmokescreen = PALACE_TARGET_STRONGER,
    },
    [NATURE_GENTLE] =
    {
        .name = COMPOUND_STRING("Gentle"),
        .statUp = STAT_SPDEF,
        .statDown = STAT_DEF,
        .backAnim = 2,
        .pokeBlockAnim = {ANIM_GENTLE, AFFINE_TURN_DOWN_SLIGHT},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlDefenseHighAttackLow,
        .battlePalacePercents = PALACE_STYLE(18, 70, 90, 5), //12%, 5%
        .battlePalaceFlavorText = B_MSG_GLINT_IN_EYE,
        .battlePalaceSmokescreen = PALACE_TARGET_STRONGER,
    },
    [NATURE_SASSY] =
    {
        .name = COMPOUND_STRING("Sassy"),
        .statUp = STAT_SPDEF,
        .statDown = STAT_SPEED,
        .backAnim = 1,
        .pokeBlockAnim = {ANIM_SASSY, AFFINE_TURN_UP_HIGH},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlAttackHighSupportLow,
        .battlePalacePercents = PALACE_STYLE(88, 6, 22, 20), //6%, 58%
        .battlePalaceFlavorText = B_MSG_GROWL_DEEPLY,
        .battlePalaceSmokescreen = PALACE_TARGET_WEAKER,
    },
    [NATURE_CAREFUL] =
    {
        .name = COMPOUND_STRING("Careful"),
        .statUp = STAT_SPDEF,
        .statDown = STAT_SPATK,
        .backAnim = 2,
        .pokeBlockAnim = {ANIM_CAREFUL, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlDefenseHighSupportLow,
        .battlePalacePercents = PALACE_STYLE(42, 50, 42, 5), //8%, 53%
        .battlePalaceFlavorText = B_MSG_GROWL_DEEPLY,
        .battlePalaceSmokescreen = PALACE_TARGET_WEAKER,
    },
    [NATURE_QUIRKY] =
    {
        .name = COMPOUND_STRING("Quirky"),
        .statUp = STAT_SPDEF,
        .statDown = STAT_SPDEF,
        .backAnim = 1,
        .pokeBlockAnim = {ANIM_QUIRKY, AFFINE_NONE},
        .natureGirlMessage = BattleFrontier_Lounge5_Text_NatureGirlAttackHighAttackLow,
        .battlePalacePercents = PALACE_STYLE(56, 22, 56, 22), //22%, 22%
        .battlePalaceFlavorText = B_MSG_EAGER_FOR_MORE,
        .battlePalaceSmokescreen = PALACE_TARGET_STRONGER,
    },
};

#include "data/graphics/pokemon.h"

#include "data/pokemon/trainer_class_lookups.h"
#include "data/pokemon/experience_tables.h"

#if P_LVL_UP_LEARNSETS >= GEN_9
#include "data/pokemon/level_up_learnsets/gen_9.h" // Scarlet/Violet
#elif P_LVL_UP_LEARNSETS >= GEN_8
#include "data/pokemon/level_up_learnsets/gen_8.h" // Sword/Shield
#elif P_LVL_UP_LEARNSETS >= GEN_7
#include "data/pokemon/level_up_learnsets/gen_7.h" // Ultra Sun/Ultra Moon
#elif P_LVL_UP_LEARNSETS >= GEN_6
#include "data/pokemon/level_up_learnsets/gen_6.h" // Omega Ruby/Alpha Sapphire
#elif P_LVL_UP_LEARNSETS >= GEN_5
#include "data/pokemon/level_up_learnsets/gen_5.h" // Black 2/White 2
#elif P_LVL_UP_LEARNSETS >= GEN_4
#include "data/pokemon/level_up_learnsets/gen_4.h" // HeartGold/SoulSilver
#elif P_LVL_UP_LEARNSETS >= GEN_3
#include "data/pokemon/level_up_learnsets/gen_3.h" // Ruby/Sapphire/Emerald
#elif P_LVL_UP_LEARNSETS >= GEN_2
#include "data/pokemon/level_up_learnsets/gen_2.h" // Crystal
#elif P_LVL_UP_LEARNSETS >= GEN_1
#include "data/pokemon/level_up_learnsets/gen_1.h" // Yellow
#endif

#include "data/pokemon/teachable_learnsets.h"
#include "data/pokemon/egg_moves.h"
#include "data/pokemon/form_species_tables.h"
#include "data/pokemon/form_change_tables.h"
#include "data/pokemon/form_change_table_pointers.h"
#include "data/pokemon/wild_encounter_ow_behavior.h"
#include "data/object_events/object_event_pic_tables_followers.h"

#include "data/pokemon/species_info.h"

#define PP_UP_SHIFTS(val)           val,        (val) << 2,        (val) << 4,        (val) << 6
#define PP_UP_SHIFTS_INV(val) (u8)~(val), (u8)~((val) << 2), (u8)~((val) << 4), (u8)~((val) << 6)

// PP Up bonuses are stored for a Pokémon as a single byte.
// There are 2 bits (a value 0-3) for each move slot that
// represent how many PP Ups have been applied.
// The following arrays take a move slot id and return:
// gPPUpGetMask - A mask to get the number of PP Ups applied to that move slot
// gPPUpClearMask - A mask to clear the number of PP Ups applied to that move slot
// gPPUpAddValues - A value to add to the PP Bonuses byte to apply 1 PP Up to that move slot
const u8 gPPUpGetMask[MAX_MON_MOVES]   = {PP_UP_SHIFTS(3)};
const u8 gPPUpClearMask[MAX_MON_MOVES] = {PP_UP_SHIFTS_INV(3)};
const u8 gPPUpAddValues[MAX_MON_MOVES] = {PP_UP_SHIFTS(1)};

const u8 gStatStageRatios[MAX_STAT_STAGE + 1][2] =
{
    {10, 40}, // -6, MIN_STAT_STAGE
    {10, 35}, // -5
    {10, 30}, // -4
    {10, 25}, // -3
    {10, 20}, // -2
    {10, 15}, // -1
    {10, 10}, //  0, DEFAULT_STAT_STAGE
    {15, 10}, // +1
    {20, 10}, // +2
    {25, 10}, // +3
    {30, 10}, // +4
    {35, 10}, // +5
    {40, 10}, // +6, MAX_STAT_STAGE
};

// The classes used by other players in the Union Room.
// These should correspond with the overworld graphics in sUnionRoomObjGfxIds
const u16 gUnionRoomFacilityClasses[NUM_UNION_ROOM_CLASSES * GENDER_COUNT] =
{
    // Male classes
    FACILITY_CLASS_COOLTRAINER_M,
    FACILITY_CLASS_BLACK_BELT,
    FACILITY_CLASS_CAMPER,
    FACILITY_CLASS_YOUNGSTER,
    FACILITY_CLASS_PSYCHIC_M,
    FACILITY_CLASS_BUG_CATCHER,
    FACILITY_CLASS_PKMN_BREEDER_M,
    FACILITY_CLASS_GUITARIST,
    // Female classes
    FACILITY_CLASS_COOLTRAINER_F,
    FACILITY_CLASS_HEX_MANIAC,
    FACILITY_CLASS_PICNICKER,
    FACILITY_CLASS_LASS,
    FACILITY_CLASS_PSYCHIC_F,
    FACILITY_CLASS_BATTLE_GIRL,
    FACILITY_CLASS_PKMN_BREEDER_F,
    FACILITY_CLASS_BEAUTY
};

const struct SpriteTemplate gBattlerSpriteTemplates[MAX_BATTLERS_COUNT] =
{
    [B_POSITION_PLAYER_LEFT] = {
        .tileTag = TAG_NONE,
        .paletteTag = 0,
        .oam = &gOamData_BattleSpritePlayerSide,
        .anims = NULL,
        .images = gBattlerPicTable_PlayerLeft,
        .affineAnims = gAffineAnims_BattleSpritePlayerSide,
        .callback = SpriteCB_BattleSpriteStartSlideLeft,
    },
    [B_POSITION_OPPONENT_LEFT] = {
        .tileTag = TAG_NONE,
        .paletteTag = 0,
        .oam = &gOamData_BattleSpriteOpponentSide,
        .anims = NULL,
        .images = gBattlerPicTable_OpponentLeft,
        .affineAnims = gAffineAnims_BattleSpriteOpponentSide,
        .callback = SpriteCB_WildMon,
    },
    [B_POSITION_PLAYER_RIGHT] = {
        .tileTag = TAG_NONE,
        .paletteTag = 0,
        .oam = &gOamData_BattleSpritePlayerSide,
        .anims = NULL,
        .images = gBattlerPicTable_PlayerRight,
        .affineAnims = gAffineAnims_BattleSpritePlayerSide,
        .callback = SpriteCB_BattleSpriteStartSlideLeft,
    },
    [B_POSITION_OPPONENT_RIGHT] = {
        .tileTag = TAG_NONE,
        .paletteTag = 0,
        .oam = &gOamData_BattleSpriteOpponentSide,
        .anims = NULL,
        .images = gBattlerPicTable_OpponentRight,
        .affineAnims = gAffineAnims_BattleSpriteOpponentSide,
        .callback = SpriteCB_WildMon
    },
};

static const struct SpriteTemplate sTrainerBackSpriteTemplate =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0,
    .oam = &gOamData_BattleSpritePlayerSide,
    .anims = NULL,
    .affineAnims = gAffineAnims_BattleSpritePlayerSide,
    .callback = SpriteCB_BattleSpriteStartSlideLeft,
};

#define NUM_SECRET_BASE_CLASSES 5
static const u8 sSecretBaseFacilityClasses[GENDER_COUNT][NUM_SECRET_BASE_CLASSES] =
{
    [MALE] = {
        FACILITY_CLASS_YOUNGSTER,
        FACILITY_CLASS_BUG_CATCHER,
        FACILITY_CLASS_RICH_BOY,
        FACILITY_CLASS_CAMPER,
        FACILITY_CLASS_COOLTRAINER_M
    },
    [FEMALE] = {
        FACILITY_CLASS_LASS,
        FACILITY_CLASS_SCHOOL_KID_F,
        FACILITY_CLASS_LADY,
        FACILITY_CLASS_PICNICKER,
        FACILITY_CLASS_COOLTRAINER_F
    }
};

static const u8 sGetMonDataEVConstants[] =
{
    MON_DATA_HP_EV,
    MON_DATA_ATK_EV,
    MON_DATA_DEF_EV,
    MON_DATA_SPEED_EV,
    MON_DATA_SPDEF_EV,
    MON_DATA_SPATK_EV
};

// For stat-raising items
static const enum Stat sStatsToRaise[] =
{
    STAT_ATK, STAT_ATK, STAT_DEF, STAT_SPEED, STAT_SPATK, STAT_SPDEF, STAT_ACC
};

// 3 modifiers each for how much to change friendship for different ranges
// 0-99, 100-199, 200+
static const s8 sFriendshipEventModifiers[][3] =
{
    [FRIENDSHIP_EVENT_GROW_LEVEL]      = { 5,  3,  2},
    [FRIENDSHIP_EVENT_VITAMIN]         = { 5,  3,  2},
    [FRIENDSHIP_EVENT_BATTLE_ITEM]     = { 1,  1,  0},
    [FRIENDSHIP_EVENT_LEAGUE_BATTLE]   = { 3,  2,  1},
    [FRIENDSHIP_EVENT_LEARN_TMHM]      = { 1,  1,  0},
    [FRIENDSHIP_EVENT_WALKING]         = { 1,  1,  1},
    [FRIENDSHIP_EVENT_FAINT_SMALL]     = {-1, -1, -1},
    [FRIENDSHIP_EVENT_FAINT_FIELD_PSN] = {-5, -5, -10},
    [FRIENDSHIP_EVENT_FAINT_LARGE]     = {-5, -5, -10},
    [FRIENDSHIP_EVENT_MASSAGE]         = { 3,  3,  3 },
};

static const struct SpeciesItem sAlteringCaveWildMonHeldItems[] =
{
    {SPECIES_NONE,      ITEM_NONE},
    {SPECIES_MAREEP,    ITEM_GANLON_BERRY},
    {SPECIES_PINECO,    ITEM_APICOT_BERRY},
    {SPECIES_HOUNDOUR,  ITEM_BIG_MUSHROOM},
    {SPECIES_TEDDIURSA, ITEM_PETAYA_BERRY},
    {SPECIES_AIPOM,     ITEM_BERRY_JUICE},
    {SPECIES_SHUCKLE,   ITEM_BERRY_JUICE},
    {SPECIES_STANTLER,  ITEM_PETAYA_BERRY},
    {SPECIES_SMEARGLE,  ITEM_SALAC_BERRY},
};

static const struct OamData sOamData_64x64 =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x64),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(64x64),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const struct SpriteTemplate sSpriteTemplate_64x64 =
{
    .tileTag = TAG_NONE,
    .paletteTag = TAG_NONE,
    .oam = &sOamData_64x64,
};

// NOTE: Reordering this array will break compatibility with existing
// saves.
static const u32 sCompressedStatuses[] =
{
    STATUS1_NONE,
    STATUS1_SLEEP_TURN(1),
    STATUS1_SLEEP_TURN(2),
    STATUS1_SLEEP_TURN(3),
    STATUS1_SLEEP_TURN(4),
    STATUS1_SLEEP_TURN(5),
    STATUS1_POISON,
    STATUS1_BURN,
    STATUS1_FREEZE,
    STATUS1_PARALYSIS,
    STATUS1_TOXIC_POISON,
    STATUS1_FROSTBITE,
};

// Attempt to detect situations where the BoxPokemon struct is unable to
// contain all the values.
// TODO: Is it possible to compute:
// - The maximum experience.
// - The maximum PP.
// - The maximum HP.
// - The maximum form countdown.

// The following definition of sBoxPokemonConstantsFit and STATIC_ASSERTS
// will prevent developers from compiling the game if the value of the
// constant on the left does not fit within the number of bits defined
// in BoxPokemon or its substructs (currently located in include/pokemon.h).

// To successfully compile, developers will need to do one of the following:
// 1) Decrease the size of the constant.
// 2) Increase the number of bits both on the struct AND in the corresponding assert. This will likely break user's saves unless there is free space after the member that is being adjsted.
// 3) Repurpose unused IDs.

// EXAMPLES
// If a developer has added enough new items so that ITEMS_COUNT now equals 1200, they could...
// 1) remove new items until ITEMS_COUNT is 1023, the max value that will fit in 10 bits.
// 2) change heldItem:10 to heldItem:11 AND change the below assert for ITEMS_COUNT to check for (1 << 11).
// 3) repurpose IDs from other items that aren't being used, like ITEM_GOLD_TEETH or ITEM_SS_TICKET until ITEMS_COUNT equals 1023, the max value that will fit in 10 bits.

UNUSED static const struct BoxPokemon sBoxPokemonConstantsFit =
{
    .language = NUM_LANGUAGES - 1,
    .hiddenNatureModifier = NUM_NATURES - 1,
    .compressedStatus = ARRAY_COUNT(sCompressedStatuses) - 1,
    .secure.substructs[0].type0 = {
        .species = NUM_SPECIES - 1,
        .heldItem = ITEMS_COUNT - 1,
        .pokeball = POKEBALL_COUNT - 1,
    },
    .secure.substructs[1].type1 = {
        .move1 = MOVES_COUNT_ALL - 1,
        .move2 = MOVES_COUNT_ALL - 1,
        .move3 = MOVES_COUNT_ALL - 1,
        .move4 = MOVES_COUNT_ALL - 1,
    },
    .secure.substructs[2].type2 = {
        .hpEV = MAX_PER_STAT_EVS,
        .attackEV = MAX_PER_STAT_EVS,
        .defenseEV = MAX_PER_STAT_EVS,
        .speedEV = MAX_PER_STAT_EVS,
        .spAttackEV = MAX_PER_STAT_EVS,
        .spDefenseEV = MAX_PER_STAT_EVS,
    },
    .secure.substructs[3].type3 = {
        .metLocation = min(MAPSEC_COUNT, min(METLOC_SPECIAL_EGG, min(METLOC_IN_GAME_TRADE, METLOC_FATEFUL_ENCOUNTER))),
        .metLevel = MAX_LEVEL,
        .metGame = NUM_VERSIONS, // NOTE: NUM_VERSIONS is inclusive!
        .dynamaxLevel = MAX_DYNAMAX_LEVEL,
        .otGender = GENDER_COUNT - 1,
        .hpIV = MAX_PER_STAT_IVS,
        .attackIV = MAX_PER_STAT_IVS,
        .defenseIV = MAX_PER_STAT_IVS,
        .speedIV = MAX_PER_STAT_IVS,
        .spAttackIV = MAX_PER_STAT_IVS,
        .spDefenseIV = MAX_PER_STAT_IVS,
        .abilityNum = NUM_ABILITY_SLOTS - 1,
    },
};

STATIC_ASSERT(MAX_LEVEL <= 100, PokemonSubstruct0_experience_PotentiallyTooSmall); // Maximum of ~2 million exp.

static u32 CompressStatus(u32 status)
{
    s32 i;
    for (i = 0; i < ARRAY_COUNT(sCompressedStatuses); i++)
    {
        if (sCompressedStatuses[i] == status)
            return i;
    }
    return 0; // STATUS1_NONE
}

static u32 UncompressStatus(u32 compressedStatus)
{
    if (compressedStatus < ARRAY_COUNT(sCompressedStatuses))
        return sCompressedStatuses[compressedStatus];
    else
        return STATUS1_NONE;
}

void ZeroBoxMonData(struct BoxPokemon *boxMon)
{
    memset(boxMon, 0, sizeof(*boxMon));
}

void ZeroMonData(struct Pokemon *mon)
{
    memset(mon, 0, sizeof(*mon));
    mon->mail = MAIL_NONE;
}

void ZeroPartyMons(struct Pokemon *party)
{
    for (s32 i = 0; i < PARTY_SIZE; i++)
        ZeroMonData(&party[i]);
}

void ZeroPlayerPartyMons(void)
{
    ZeroPartyMons(gParties[B_TRAINER_PLAYER]);
    gPartiesCount[B_TRAINER_PLAYER] = 0;
}

void ZeroEnemyPartyMons(void)
{
    ZeroPartyMons(gParties[B_TRAINER_OPPONENT_A]);
    ZeroPartyMons(gParties[B_TRAINER_OPPONENT_B]);
    gPartiesCount[B_TRAINER_OPPONENT_A] = 0;
    gPartiesCount[B_TRAINER_OPPONENT_B] = 0;
}

void CreateRandomMon(struct Pokemon *mon, enum Species species, u8 level)
{
    CreateRandomMonWithIVs(mon, species, level, USE_RANDOM_IVS);
}

void CreateRandomMonWithIVs(struct Pokemon *mon, enum Species species, u8 level, u8 fixedIv)
{
    CreateMonWithIVs(mon, species, level, Random32(), OTID_STRUCT_PLAYER_ID, fixedIv);
    GiveMonInitialMoveset(mon);
}

void CreateMon(struct Pokemon *mon, enum Species species, u8 level, u32 personality, struct OriginalTrainerId trainerId)
{
    u32 mail;
    ZeroMonData(mon);
    CreateBoxMon(&mon->box, species, level, personality, trainerId);
    SetMonData(mon, MON_DATA_LEVEL, &level);
    mail = MAIL_NONE;
    SetMonData(mon, MON_DATA_MAIL, &mail);
}

void CreateMonWithIVs(struct Pokemon *mon, enum Species species, u8 level, u32 personality, struct OriginalTrainerId trainerId, u8 fixedIV)
{
    CreateMon(mon, species, level, personality, trainerId);
    SetBoxMonIVs(&mon->box, fixedIV);
    CalculateMonStats(mon);
}

bool32 ComputePlayerShinyOdds(u32 personality, u32 value)
{
    if (FlagGet(P_FLAG_FORCE_NO_SHINY))
        return FALSE;

    if (FlagGet(P_FLAG_FORCE_SHINY))
        return TRUE;

    if (P_ONLY_OBTAINABLE_SHINIES && (CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE || (FlagGet(WE_FLAG_NO_CATCHING))))
        return FALSE;

    if (P_NO_SHINIES_WITHOUT_POKEBALLS && !HasAtLeastOnePokeBall() && FlagGet(FLAG_SYS_POKEDEX_GET))
        return FALSE;

    u32 totalRerolls = 0;

    if (CheckBagHasItem(ITEM_SHINY_CHARM, 1))
        totalRerolls += I_SHINY_CHARM_ADDITIONAL_ROLLS;

    if (LURE_STEP_COUNT != 0)
        totalRerolls += 1;

    totalRerolls += CalculateChainFishingShinyRolls();

    if (gDexNavSpecies)
        totalRerolls += CalculateDexNavShinyRolls();

    while (GET_SHINY_VALUE(value, personality) >= SHINY_ODDS && totalRerolls > 0)
    {
        personality = Random32();
        totalRerolls--;
    }

    return GET_SHINY_VALUE(value, personality) < SHINY_ODDS;
}

void SetBoxMonIVs(struct BoxPokemon *mon, u8 fixedIV)
{
    u32 ivs;

    if (fixedIV <= MAX_PER_STAT_IVS)
    {
        u32 value = fixedIV;
        ivs = value | (value << 5) | (value << 10) | (value << 15)
            | (value << 20) | (value << 25);
    }
    else
    {
        // Inclement Emerald: natural acquisitions have random IVs with three
        // distinct perfect stats. Explicit trainer IVs remain untouched.
        u8 stats[NUM_STATS] = {0, 1, 2, 3, 4, 5};
        ivs = Random32() & 0x3FFFFFFF;
        for (u32 i = 0; i < 3; i++)
        {
            u32 chosen = i + Random() % (NUM_STATS - i);
            u8 stat = stats[chosen];
            stats[chosen] = stats[i];
            ivs |= (u32)MAX_PER_STAT_IVS << (stat * 5);
        }
    }
    SetBoxMonData(mon, MON_DATA_IVS, &ivs);
}

void CreateBoxMon(struct BoxPokemon *boxMon, enum Species species, u8 level, u32 personality, struct OriginalTrainerId trainerId)
{
    u8 speciesName[POKEMON_NAME_LENGTH + 1];
    u32 value;
    u16 checksum;
    bool32 isShiny;

    ZeroBoxMonData(boxMon);
    // Determine original trainer ID
    if (trainerId.method == OT_ID_RANDOM_NO_SHINY)
    {
        value = Random32();
        isShiny = FALSE;
    }
    else if (trainerId.method == OT_ID_PRESET)
    {
        value = trainerId.value;
        isShiny = GET_SHINY_VALUE(value, personality) < SHINY_ODDS;
    }
    else // Player is the OT
    {
        value = READ_OTID_FROM_SAVE;
        isShiny = ComputePlayerShinyOdds(personality, value);
    }

    SetBoxMonData(boxMon, MON_DATA_PERSONALITY, &personality);
    SetBoxMonData(boxMon, MON_DATA_OT_ID, &value);

    checksum = CalculateBoxMonChecksum(boxMon);
    SetBoxMonData(boxMon, MON_DATA_CHECKSUM, &checksum);
    EncryptBoxMon(boxMon);
    SetBoxMonData(boxMon, MON_DATA_IS_SHINY, &isShiny);
    // The nickname setter copies the full field, including bytes after EOS.
    memset(speciesName, EOS, sizeof(speciesName));
    StringCopy(speciesName, GetSpeciesName(species));
    SetBoxMonData(boxMon, MON_DATA_NICKNAME, speciesName);
    SetBoxMonData(boxMon, MON_DATA_LANGUAGE, &gGameLanguage);
    SetBoxMonData(boxMon, MON_DATA_OT_NAME, gSaveBlock2Ptr->playerName);
    SetBoxMonData(boxMon, MON_DATA_SPECIES, &species);
    SetBoxMonData(boxMon, MON_DATA_EXP, &gExperienceTables[gSpeciesInfo[species].growthRate][level]);
    SetBoxMonData(boxMon, MON_DATA_FRIENDSHIP, &gSpeciesInfo[species].friendship);
    value = GetCurrentRegionMapSectionId();
    SetBoxMonData(boxMon, MON_DATA_MET_LOCATION, &value);
    SetBoxMonData(boxMon, MON_DATA_MET_LEVEL, &level);
    SetBoxMonData(boxMon, MON_DATA_MET_GAME, &gGameVersion);
    value = BALL_POKE;
    SetBoxMonData(boxMon, MON_DATA_POKEBALL, &value);
    SetBoxMonData(boxMon, MON_DATA_OT_GENDER, &gSaveBlock2Ptr->playerGender);

    // Gen 3-4 formula over the normal slots: slot 1 when the species has one,
    // plus the Inclement slot. The hidden slot stays as rare as before; a
    // trainer-owned Pokemon never keeps the Inclement slot (SetMonTrainerOwned).
    value = RollNormalAbilitySlot(species, boxMon->personality);
    if (value != 0)
        SetBoxMonData(boxMon, MON_DATA_ABILITY_NUM, &value);
    SetBoxMonIVs(boxMon, MAX_PER_STAT_IVS);
}

static bool32 IsValidGender(u32 gender)
{
    switch (gender)
    {
    case MON_MALE:
    case MON_FEMALE:
    case MON_GENDERLESS:
    case MON_GENDER_RANDOM:
        return TRUE;
    default:
        return FALSE;
    }
}

static void CleanIncompatibleGenderSpecies(enum Species species, u8 *gender)
{
    switch (gSpeciesInfo[species].genderRatio)
    {
    case MON_MALE:
    case MON_FEMALE:
    case MON_GENDERLESS:
        *gender = MON_GENDER_RANDOM;
        return;
    }
    if (*gender == MON_GENDERLESS)
        *gender = MON_GENDER_RANDOM;
}

u32 GetMonPersonality(enum Species species, u8 gender, u8 nature, u8 unownLetter)
{
    u32 personality, actualLetter;

    assertf(IsValidGender(gender), "invalid gender: %d", gender)
    {
        gender = MON_GENDER_RANDOM;
    }

    assertf(nature <= NATURE_RANDOM, "invalid nature: %d", nature)
    {
        nature = NATURE_RANDOM;
    }

    assertf(unownLetter <= NUM_UNOWN_FORMS, "invalid letter: %d", unownLetter)
    {
        unownLetter = RANDOM_UNOWN_LETTER;
    }

    CleanIncompatibleGenderSpecies(species, &gender);
    do
    {
        personality = Random32();
        actualLetter = GET_UNOWN_LETTER(personality);
    }
    while ((nature != GetNatureFromPersonality(personality) && nature != NATURE_RANDOM)
            || (gender != MON_GENDER_RANDOM && gender != GetGenderFromSpeciesAndPersonality(species, personality))
            || ((actualLetter != unownLetter - 1) && unownLetter > 0));
    return personality;
}

// This is only used to create Wally's Ralts.
void CreateMaleMon(struct Pokemon *mon, enum Species species, u8 level)
{
    u32 personality = GetMonPersonality(species, MON_MALE, NATURE_RANDOM, RANDOM_UNOWN_LETTER);
    CreateMonWithIVs(mon, species, level, personality, OTID_STRUCT_PLAYER_ID, USE_RANDOM_IVS);
    GiveMonInitialMoveset(mon);
}

void CreateMonWithIVsPersonality(struct Pokemon *mon, enum Species species, u8 level, u32 ivs, u32 personality)
{
    CreateMon(mon, species, level, personality, OTID_STRUCT_PLAYER_ID);
    SetMonData(mon, MON_DATA_IVS, &ivs);
    CalculateMonStats(mon);
    GiveMonInitialMoveset(mon);
}

void CreateBattleTowerMon(struct Pokemon *mon, struct BattleTowerPokemon *src)
{
    s32 i;
    u8 nickname[max(32, POKEMON_NAME_BUFFER_SIZE)];
    enum Language language;
    u8 value;

    CreateMon(mon, src->species, src->level, src->personality, OTID_STRUCT_PRESET(src->otId));

    for (i = 0; i < MAX_MON_MOVES; i++)
        SetMonMoveSlot(mon, src->moves[i], i);

    SetMonData(mon, MON_DATA_PP_BONUSES, &src->ppBonuses);
    SetMonData(mon, MON_DATA_HELD_ITEM, &src->heldItem);
    SetMonData(mon, MON_DATA_FRIENDSHIP, &src->friendship);

    memset(nickname, EOS, sizeof(nickname));
    StringCopy(nickname, src->nickname);

    if (nickname[0] == EXT_CTRL_CODE_BEGIN && nickname[1] == EXT_CTRL_CODE_JPN)
    {
        language = LANGUAGE_JAPANESE;
        StripExtCtrlCodes(nickname);
    }
    else
    {
        language = GAME_LANGUAGE;
    }

    SetMonData(mon, MON_DATA_LANGUAGE, &language);
    SetMonData(mon, MON_DATA_NICKNAME, nickname);
    SetMonData(mon, MON_DATA_HP_EV, &src->hpEV);
    SetMonData(mon, MON_DATA_ATK_EV, &src->attackEV);
    SetMonData(mon, MON_DATA_DEF_EV, &src->defenseEV);
    SetMonData(mon, MON_DATA_SPEED_EV, &src->speedEV);
    SetMonData(mon, MON_DATA_SPATK_EV, &src->spAttackEV);
    SetMonData(mon, MON_DATA_SPDEF_EV, &src->spDefenseEV);
    value = src->abilityNum;
    SetMonData(mon, MON_DATA_ABILITY_NUM, &value);
    value = src->hpIV;
    SetMonData(mon, MON_DATA_HP_IV, &value);
    value = src->attackIV;
    SetMonData(mon, MON_DATA_ATK_IV, &value);
    value = src->defenseIV;
    SetMonData(mon, MON_DATA_DEF_IV, &value);
    value = src->speedIV;
    SetMonData(mon, MON_DATA_SPEED_IV, &value);
    value = src->spAttackIV;
    SetMonData(mon, MON_DATA_SPATK_IV, &value);
    value = src->spDefenseIV;
    SetMonData(mon, MON_DATA_SPDEF_IV, &value);
    MonRestorePP(mon);
    CalculateMonStats(mon);
}

void CreateBattleTowerMon_HandleLevel(struct Pokemon *mon, struct BattleTowerPokemon *src, bool8 lvl50)
{
    s32 i;
    u8 nickname[max(32, POKEMON_NAME_BUFFER_SIZE)];
    u8 level;
    enum Language language;
    u8 value;

    if (gSaveBlock2Ptr->frontier.lvlMode != FRONTIER_LVL_50)
        level = GetFrontierEnemyMonLevel(gSaveBlock2Ptr->frontier.lvlMode);
    else if (lvl50)
        level = FRONTIER_MAX_LEVEL_50;
    else
        level = src->level;

    CreateMon(mon, src->species, level, src->personality, OTID_STRUCT_PRESET(src->otId));

    for (i = 0; i < MAX_MON_MOVES; i++)
        SetMonMoveSlot(mon, src->moves[i], i);

    SetMonData(mon, MON_DATA_PP_BONUSES, &src->ppBonuses);
    SetMonData(mon, MON_DATA_HELD_ITEM, &src->heldItem);
    SetMonData(mon, MON_DATA_FRIENDSHIP, &src->friendship);

    memset(nickname, EOS, sizeof(nickname));
    StringCopy(nickname, src->nickname);

    if (nickname[0] == EXT_CTRL_CODE_BEGIN && nickname[1] == EXT_CTRL_CODE_JPN)
    {
        language = LANGUAGE_JAPANESE;
        StripExtCtrlCodes(nickname);
    }
    else
    {
        language = GAME_LANGUAGE;
    }

    SetMonData(mon, MON_DATA_LANGUAGE, &language);
    SetMonData(mon, MON_DATA_NICKNAME, nickname);
    SetMonData(mon, MON_DATA_HP_EV, &src->hpEV);
    SetMonData(mon, MON_DATA_ATK_EV, &src->attackEV);
    SetMonData(mon, MON_DATA_DEF_EV, &src->defenseEV);
    SetMonData(mon, MON_DATA_SPEED_EV, &src->speedEV);
    SetMonData(mon, MON_DATA_SPATK_EV, &src->spAttackEV);
    SetMonData(mon, MON_DATA_SPDEF_EV, &src->spDefenseEV);
    value = src->abilityNum;
    SetMonData(mon, MON_DATA_ABILITY_NUM, &value);
    value = src->hpIV;
    SetMonData(mon, MON_DATA_HP_IV, &value);
    value = src->attackIV;
    SetMonData(mon, MON_DATA_ATK_IV, &value);
    value = src->defenseIV;
    SetMonData(mon, MON_DATA_DEF_IV, &value);
    value = src->speedIV;
    SetMonData(mon, MON_DATA_SPEED_IV, &value);
    value = src->spAttackIV;
    SetMonData(mon, MON_DATA_SPATK_IV, &value);
    value = src->spDefenseIV;
    SetMonData(mon, MON_DATA_SPDEF_IV, &value);
    MonRestorePP(mon);
    CalculateMonStats(mon);
}

void CreateApprenticeMon(struct Pokemon *mon, const struct Apprentice *src, u8 monId)
{
    s32 i;
    u16 evAmount;
    u8 language;
    u32 otId = gApprentices[src->id].otId;
    u32 personality = ((gApprentices[src->id].otId >> 8) | ((gApprentices[src->id].otId & 0xFF) << 8))
                    + src->party[monId].species + src->number;

    CreateMonWithIVs(mon,
              src->party[monId].species,
              GetFrontierEnemyMonLevel(src->lvlMode - 1),
              personality,
              OTID_STRUCT_PRESET(otId),
              MAX_PER_STAT_IVS);
    SetMonData(mon, MON_DATA_HELD_ITEM, &src->party[monId].item);
    for (i = 0; i < MAX_MON_MOVES; i++)
        SetMonMoveSlot(mon, src->party[monId].moves[i], i);

    evAmount = MAX_TOTAL_EVS / NUM_STATS;
    for (i = 0; i < NUM_STATS; i++)
        SetMonData(mon, MON_DATA_HP_EV + i, &evAmount);

    language = src->language;
    SetMonData(mon, MON_DATA_LANGUAGE, &language);
    SetMonData(mon, MON_DATA_OT_NAME, GetApprenticeNameInLanguage(src->id, language));
    CalculateMonStats(mon);
}

void ConvertPokemonToBattleTowerPokemon(struct Pokemon *mon, struct BattleTowerPokemon *dest)
{
    s32 i;
    enum Item heldItem;

    dest->species = GetMonData(mon, MON_DATA_SPECIES);
    heldItem = GetMonData(mon, MON_DATA_HELD_ITEM);

    if (heldItem == ITEM_ENIGMA_BERRY_E_READER)
        heldItem = ITEM_NONE;

    dest->heldItem = heldItem;

    for (i = 0; i < MAX_MON_MOVES; i++)
        dest->moves[i] = GetMonData(mon, MON_DATA_MOVE1 + i);

    dest->level = GetMonData(mon, MON_DATA_LEVEL);
    dest->ppBonuses = GetMonData(mon, MON_DATA_PP_BONUSES);
    dest->otId = GetMonData(mon, MON_DATA_OT_ID);
    dest->hpEV = GetMonData(mon, MON_DATA_HP_EV);
    dest->attackEV = GetMonData(mon, MON_DATA_ATK_EV);
    dest->defenseEV = GetMonData(mon, MON_DATA_DEF_EV);
    dest->speedEV = GetMonData(mon, MON_DATA_SPEED_EV);
    dest->spAttackEV = GetMonData(mon, MON_DATA_SPATK_EV);
    dest->spDefenseEV = GetMonData(mon, MON_DATA_SPDEF_EV);
    dest->friendship = GetMonData(mon, MON_DATA_FRIENDSHIP);
    dest->hpIV = GetMonData(mon, MON_DATA_HP_IV);
    dest->attackIV = GetMonData(mon, MON_DATA_ATK_IV);
    dest->defenseIV = GetMonData(mon, MON_DATA_DEF_IV);
    dest->speedIV  = GetMonData(mon, MON_DATA_SPEED_IV);
    dest->spAttackIV  = GetMonData(mon, MON_DATA_SPATK_IV);
    dest->spDefenseIV  = GetMonData(mon, MON_DATA_SPDEF_IV);
    dest->abilityNum = GetMonData(mon, MON_DATA_ABILITY_NUM);
    dest->personality = GetMonData(mon, MON_DATA_PERSONALITY);
    GetMonData(mon, MON_DATA_NICKNAME10, dest->nickname);
}

static void CreateEventMon(struct Pokemon *mon, enum Species species, u8 level, u32 personality, struct OriginalTrainerId otId)
{
    bool32 isModernFatefulEncounter = TRUE;

    CreateMonWithIVs(mon, species, level, personality, otId, USE_RANDOM_IVS);
    GiveMonInitialMoveset(mon);
    SetMonData(mon, MON_DATA_MODERN_FATEFUL_ENCOUNTER, &isModernFatefulEncounter);
    CalculateMonStats(mon);
}

enum TrainerPicID GetUnionRoomTrainerPic(void)
{
    u8 linkId;
    u32 arrId;

    if (gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK)
        linkId = gRecordedBattleMultiplayerId ^ 1;
    else
        linkId = GetMultiplayerId() ^ 1;

    arrId = gLinkPlayers[linkId].trainerId % NUM_UNION_ROOM_CLASSES;
    arrId |= gLinkPlayers[linkId].gender * NUM_UNION_ROOM_CLASSES;
    return FacilityClassToPicIndex(gUnionRoomFacilityClasses[arrId]);
}

enum TrainerClassID GetUnionRoomTrainerClass(void)
{
    u8 linkId;
    u32 arrId;

    if (gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK)
        linkId = gRecordedBattleMultiplayerId ^ 1;
    else
        linkId = GetMultiplayerId() ^ 1;

    arrId = gLinkPlayers[linkId].trainerId % NUM_UNION_ROOM_CLASSES;
    arrId |= gLinkPlayers[linkId].gender * NUM_UNION_ROOM_CLASSES;
    return gFacilityClassToTrainerClass[gUnionRoomFacilityClasses[arrId]];
}

void CreateEnemyEventMon(void)
{
    s32 species = gSpecialVar_0x8004;
    s32 level = gSpecialVar_0x8005;
    s32 itemId = gSpecialVar_0x8006;

    ZeroEnemyPartyMons();

    CreateEventMon(&gParties[B_TRAINER_OPPONENT_A][0], species, level, Random32(), OTID_STRUCT_PLAYER_ID);
    if (itemId)
    {
        u8 heldItem[2];
        heldItem[0] = itemId;
        heldItem[1] = itemId >> 8;
        SetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HELD_ITEM, heldItem);
    }
}

static u16 CalculateBoxMonChecksum(struct BoxPokemon *boxMon)
{
    u32 checksum = 0;

    for (u32 i = 0; i < ARRAY_COUNT(boxMon->secure.raw); i++)
        checksum += boxMon->secure.raw[i] + (boxMon->secure.raw[i] >> 16);

    return checksum;
}

static u16 CalculateBoxMonChecksumDecrypt(struct BoxPokemon *boxMon)
{
    u32 checksum = 0;

    for (u32 i = 0; i < ARRAY_COUNT(boxMon->secure.raw); i++)
    {
        boxMon->secure.raw[i] ^= (boxMon->otId ^ boxMon->personality);
        checksum += boxMon->secure.raw[i] + (boxMon->secure.raw[i] >> 16);
    }

    return checksum;
}

static u16 CalculateBoxMonChecksumReencrypt(struct BoxPokemon *boxMon)
{
    u32 checksum = 0;

    for (u32 i = 0; i < ARRAY_COUNT(boxMon->secure.raw); i++)
    {
        checksum += boxMon->secure.raw[i] + (boxMon->secure.raw[i] >> 16);
        boxMon->secure.raw[i] ^= (boxMon->otId ^ boxMon->personality);
    }

    return checksum;
}

// Single owner of the game's Gen 9 stat formula, including authored IVs and
// Shedinja. Party recalculation and facility previews use this same function.
u32 CalculateSpeciesStat(enum Species species, u32 nature, enum Stat stat, u32 level, u32 evs, u32 iv)
{
    return CalculateSpeciesStatForOwner(species, nature, stat, level, evs, iv, TRUE);
}

// Trainer-owned Pokemon read gSpeciesInfo; everyone else reads the Inclement layer.
u32 CalculateSpeciesStatForOwner(enum Species species, u32 nature, enum Stat stat, u32 level, u32 evs, u32 iv, bool32 trainerOwned)
{
    u32 value;

    if (stat == STAT_HP && HasShedinjaHPHandling(species))
        return 1;
    value = ((2 * GetSpeciesBaseStatForOwner(species, stat, trainerOwned) + min(iv, MAX_PER_STAT_IVS) + evs / 4) * level) / 100;
    if (stat == STAT_HP)
        return value + level + 10;
    return ModifyStatByNature(nature, value + 5, stat);
}

void CalculateMonStats(struct Pokemon *mon)
{
    CalculateMonStatsCont(mon, TRUE);
}

static bool32 IsOverlevelTrainerOpponent(const struct Pokemon *mon)
{
    if (!(gBattleTypeFlags & BATTLE_TYPE_TRAINER))
        return FALSE;
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        if (mon == &gParties[B_TRAINER_OPPONENT_A][slot]
         || mon == &gParties[B_TRAINER_OPPONENT_B][slot])
            return TRUE;
    return FALSE;
}

void CalculateMonStatsCont(struct Pokemon *mon, bool32 updateSpeedStat)
{
    s32 oldMaxHP = GetMonData(mon, MON_DATA_MAX_HP);
    s32 currentHP = GetMonData(mon, MON_DATA_HP);
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    u8 friendship = GetMonData(mon, MON_DATA_FRIENDSHIP);
    s32 level = GetLevelFromMonExp(mon);
    s32 newMaxHP;
    bool32 trainerOwned = IsMonTrainerOwned(mon);

    // Trainer/Circuit opponents have battle-only levels above the boxed EXP ceiling.
    // Preserve them through Mega/form stat recalculation without changing the
    // save layout, player leveling, experience tables, or ordinary wild battles.
    if (mon->level > MAX_LEVEL
     && (IsOverlevelTrainerOpponent(mon) || IsChampionsCircuitOpponent(mon)))
        level = mon->level;

    u8 nature = GetMonData(mon, MON_DATA_HIDDEN_NATURE);

    SetMonData(mon, MON_DATA_LEVEL, &level);

    bool32 hyperTrained[NUM_STATS]; //In a battle test, hyper training flag indicates a fixed stat
    s32 ev[NUM_STATS];
    for (u32 i = 0; i < NUM_STATS; i++)
    {
        hyperTrained[i] = GetMonData(mon, MON_DATA_HYPER_TRAINED_HP + i);
        ev[i] = GetMonData(mon, MON_DATA_HP_EV + i);

        if (hyperTrained[i])
        {
        #if TESTING
            if (gMain.inBattle)
                continue;
        #endif
        }

        if (i == STAT_HP)
            continue;

        s32 n = CalculateSpeciesStatForOwner(species, nature, i, level, ev[i],
                                             GetMonData(mon, MON_DATA_HP_IV + i), trainerOwned);
        if (B_FRIENDSHIP_BOOST == TRUE)
            n = n + ((n * 10 * friendship) / (MAX_FRIENDSHIP * 100));
        SetMonData(mon, MON_DATA_MAX_HP + i, &n);
    }

#if TESTING
    if (hyperTrained[STAT_HP] && gMain.inBattle)
        return;
#endif

    newMaxHP = CalculateSpeciesStatForOwner(species, nature, STAT_HP, level, ev[STAT_HP],
                                            GetMonData(mon, MON_DATA_HP_IV), trainerOwned);

    gBattleScripting.levelUpHP = newMaxHP - oldMaxHP;
    if (gBattleScripting.levelUpHP == 0)
        gBattleScripting.levelUpHP = 1;
    SetMonData(mon, MON_DATA_MAX_HP, &newMaxHP);

    // Since a Pokémon's maxHP data could either not have
    // been initialized at this point or this Pokémon is
    // just fainted, the check for oldMaxHP is important.
    if (currentHP == 0 && oldMaxHP != 0)
    {
        // Keep boxed damage consistent when a fainted mon's max HP changes.
        SetMonData(mon, MON_DATA_HP, &currentHP);
        return;
    }

    // Only add to currentHP if newMaxHP went up.
    if (newMaxHP > oldMaxHP)
        currentHP += newMaxHP - oldMaxHP;

    // Ensure currentHP does not surpass newMaxHP.
    if (currentHP > newMaxHP)
        currentHP = newMaxHP;

    SetMonData(mon, MON_DATA_HP, &currentHP);
}

// Ownership rule for species data. Trainer-owned Pokemon use gSpeciesInfo
// exactly as before; every other Pokemon (the player's, wild ones, gifts,
// eggs, trades) reads the Inclement layer (sInclementLayer). The trainer mark
// is explicit, stored in the Pokemon, and set in three places only:
// GenerateMonFromTrainerMon (every trainer and NPC partner party), Champions
// Circuit team generation, and battle start, which marks both opponent
// parties of a trainer or Circuit battle and any partner party. Wild
// battles never mark, so a caught Pokemon simply keeps the layer. Anything
// the player receives (GiveMonToPartyOrPC) is unmarked.
bool32 IsBoxMonTrainerOwned(const struct BoxPokemon *boxMon)
{
    return boxMon->isTrainerOwned;
}

bool32 IsMonTrainerOwned(const struct Pokemon *mon)
{
    return mon->box.isTrainerOwned;
}

void SetMonTrainerOwned(struct Pokemon *mon, bool32 trainerOwned)
{
    enum Species species;
    u32 hp, maxHP;

    if (mon->box.isTrainerOwned == (trainerOwned != FALSE))
        return;
    mon->box.isTrainerOwned = (trainerOwned != FALSE);
    species = GetMonData(mon, MON_DATA_SPECIES);
    // Trainer-owned Pokemon can never use an Inclement slot.
    if (trainerOwned && GetMonData(mon, MON_DATA_ABILITY_NUM) >= ABILITY_SLOT_INCLEMENT)
    {
        u32 slot = (GetSpeciesAbility(species, 1) != ABILITY_NONE) ? (mon->box.personality & 1) : 0;
        SetMonData(mon, MON_DATA_ABILITY_NUM, &slot);
    }
    maxHP = GetMonData(mon, MON_DATA_MAX_HP);
    // Nothing to redo before the first stat calculation or without a layered stat line.
    if (species == SPECIES_NONE || maxHP == 0 || !sInclementLayer[SanitizeSpeciesId(species)].hasBaseStats)
        return;
    // A full-HP Pokemon stays at full HP; otherwise keep its HP within the new maximum.
    hp = GetMonData(mon, MON_DATA_HP);
    CalculateMonStats(mon);
    if (hp == maxHP)
        hp = GetMonData(mon, MON_DATA_MAX_HP);
    else
        hp = min(hp, GetMonData(mon, MON_DATA_MAX_HP));
    SetMonData(mon, MON_DATA_HP, &hp);
}

static void MarkPartyTrainerOwned(enum BattleTrainer trainer)
{
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
    {
        struct Pokemon *mon = &gParties[trainer][slot];
        if (GetMonData(mon, MON_DATA_SPECIES) != SPECIES_NONE)
            SetMonTrainerOwned(mon, TRUE);
    }
}

// Battle start: both opponent parties of a trainer (or Champions Circuit)
// battle and any NPC partner party are trainer-owned. Wild foes are not.
void MarkTrainerBattlePartiesOwned(void)
{
    if ((gBattleTypeFlags & BATTLE_TYPE_TRAINER) || IsChampionsCircuitBattle())
    {
        MarkPartyTrainerOwned(B_TRAINER_OPPONENT_A);
        MarkPartyTrainerOwned(B_TRAINER_OPPONENT_B);
    }
    MarkPartyTrainerOwned(B_TRAINER_PARTNER);
}

void BoxMonToMon(const struct BoxPokemon *src, struct Pokemon *dest)
{
    u32 value = 0;
    dest->box = *src;
    dest->status = GetBoxMonData(&dest->box, MON_DATA_STATUS);
    dest->hp = 0;
    dest->maxHP = 0;
    value = MAIL_NONE;
    SetMonData(dest, MON_DATA_MAIL, &value);
    value = GetBoxMonData(&dest->box, MON_DATA_HP_LOST);
    CalculateMonStats(dest);
    value = GetMonData(dest, MON_DATA_MAX_HP) - value;
    SetMonData(dest, MON_DATA_HP, &value);
}

u8 GetLevelFromMonExp(struct Pokemon *mon)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    u32 exp = GetMonData(mon, MON_DATA_EXP);
    s32 level = 1;

    while (level <= MAX_LEVEL && gExperienceTables[gSpeciesInfo[species].growthRate][level] <= exp)
        level++;

    return level - 1;
}

u8 GetLevelFromBoxMonExp(struct BoxPokemon *boxMon)
{
    enum Species species = GetBoxMonData(boxMon, MON_DATA_SPECIES);
    u32 exp = GetBoxMonData(boxMon, MON_DATA_EXP);
    s32 level = 1;

    while (level <= MAX_LEVEL && gExperienceTables[gSpeciesInfo[species].growthRate][level] <= exp)
        level++;

    return level - 1;
}

u16 GiveMoveToMon(struct Pokemon *mon, enum Move move)
{
    u16 result = GiveMoveToBoxMon(&mon->box, move);

    if (result == move)
        TryFormChangeOnMove(mon, move, B_TRAINER_PLAYER);
    return result;
}

u16 GiveMoveToBoxMon(struct BoxPokemon *boxMon, enum Move move)
{
    s32 i;
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        enum Move existingMove = GetBoxMonData(boxMon, MON_DATA_MOVE1 + i);
        if (existingMove == MOVE_NONE)
        {
            u32 pp = GetMoveMaxPP(move);
            SetBoxMonData(boxMon, MON_DATA_MOVE1 + i, &move);
            SetBoxMonData(boxMon, MON_DATA_PP1 + i, &pp);
            return move;
        }
        if (existingMove == move)
            return MON_ALREADY_KNOWS_MOVE;
    }
    return MON_HAS_MAX_MOVES;
}

void SetMonMoveSlot(struct Pokemon *mon, enum Move move, u8 slot)
{
    enum Move oldMove = GetMonData(mon, MON_DATA_MOVE1 + slot);

    SetBoxMonMoveSlot(&mon->box, move, slot);
    if (oldMove != move)
    {
        TryFormChangeOnMove(mon, oldMove, B_TRAINER_PLAYER);
        TryFormChangeOnMove(mon, move, B_TRAINER_PLAYER);
    }
}

void SetBoxMonMoveSlot(struct BoxPokemon *mon, enum Move move, u8 slot)
{
    SetBoxMonData(mon, MON_DATA_MOVE1 + slot, &move);
    u32 pp = GetMoveMaxPP(move);
    SetBoxMonData(mon, MON_DATA_PP1 + slot, &pp);
}

void SwapBoxMonMoves(struct BoxPokemon *mon, u8 slotTo, u8 slotFrom)
{
    if (slotTo == slotFrom)
        return;

    enum Move move1 = GetBoxMonData(mon, MON_DATA_MOVE1 + slotTo);
    enum Move move0 = GetBoxMonData(mon, MON_DATA_MOVE1 + slotFrom);
    u8 pp1 = GetBoxMonData(mon, MON_DATA_PP1 + slotTo);
    u8 pp0 = GetBoxMonData(mon, MON_DATA_PP1 + slotFrom);
    u8 ppBonuses = GetBoxMonData(mon, MON_DATA_PP_BONUSES);
    u8 ppBonusMask1 = gPPUpGetMask[slotTo];
    u8 ppBonusMove1 = (ppBonuses & ppBonusMask1) >> (slotTo * 2);
    u8 ppBonusMask2 = gPPUpGetMask[slotFrom];
    u8 ppBonusMove2 = (ppBonuses & ppBonusMask2) >> (slotFrom * 2);
    ppBonuses &= ~ppBonusMask1;
    ppBonuses &= ~ppBonusMask2;
    ppBonuses |= (ppBonusMove1 << (slotFrom * 2)) + (ppBonusMove2 << (slotTo * 2));
    SetBoxMonData(mon, MON_DATA_MOVE1 + slotTo, &move0);
    SetBoxMonData(mon, MON_DATA_MOVE1 + slotFrom, &move1);
    SetBoxMonData(mon, MON_DATA_PP1 + slotTo, &pp0);
    SetBoxMonData(mon, MON_DATA_PP1 + slotFrom, &pp1);
    SetBoxMonData(mon, MON_DATA_PP_BONUSES, &ppBonuses);
}

void DeleteMove(struct Pokemon *mon, enum Move move)
{
    struct BoxPokemon *boxMon = &mon->box;
    u32 i, j;

    if (move != MOVE_NONE)
    {
        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            u32 existingMove = GetBoxMonData(boxMon, MON_DATA_MOVE1 + i);
            if (existingMove == move)
            {
                SetMonMoveSlot(mon, MOVE_NONE, i);
                RemoveMonPPBonus(mon, i);
                for (j = i; j < MAX_MON_MOVES - 1; j++)
                    SwapBoxMonMoves(&mon->box, j, j + 1);
                break;
            }
        }
    }
}

static void SetMonMoveSlot_KeepPP(struct Pokemon *mon, enum Move move, u8 slot)
{
    u8 currPP = GetMonData(mon, MON_DATA_PP1 + slot);
    u8 newPP = GetMoveMaxPP(move);
    u16 finalPP = min(currPP, newPP);

    SetMonData(mon, MON_DATA_MOVE1 + slot, &move);
    SetMonData(mon, MON_DATA_PP1 + slot, &finalPP);
}

void GiveMonInitialMoveset(struct Pokemon *mon)
{
    GiveBoxMonInitialMoveset(&mon->box);
}

void GiveBoxMonInitialMoveset(struct BoxPokemon *boxMon) //Credit: AsparagusEduardo
{
    enum Species species = GetBoxMonData(boxMon, MON_DATA_SPECIES);
    s32 level = GetLevelFromBoxMonExp(boxMon);
    s32 i;
    enum Move moves[MAX_MON_MOVES] = {MOVE_NONE};
    u8 addedMoves = 0;
    const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(species);

    for (i = 0; learnset[i].move != LEVEL_UP_MOVE_END; i++)
    {
        s32 j;
        bool32 alreadyKnown = FALSE;

        if (learnset[i].level > level)
            break;
        if (learnset[i].level == 0)
            continue;

        for (j = 0; j < addedMoves; j++)
        {
            if (moves[j] == learnset[i].move)
            {
                alreadyKnown = TRUE;
                break;
            }
        }

        if (!alreadyKnown)
        {
            if (addedMoves < MAX_MON_MOVES)
            {
                moves[addedMoves] = learnset[i].move;
                addedMoves++;
            }
            else
            {
                for (j = 0; j < MAX_MON_MOVES - 1; j++)
                    moves[j] = moves[j + 1];
                moves[MAX_MON_MOVES - 1] = learnset[i].move;
            }
        }
    }
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        SetBoxMonData(boxMon, MON_DATA_MOVE1 + i, &moves[i]);
        u32 pp = GetMoveMaxPP(moves[i]);
        SetBoxMonData(boxMon, MON_DATA_PP1 + i, &pp);
    }
}

enum Move MonTryLearningNewMoveAtLevel(struct Pokemon *mon, bool32 firstMove, u32 level)
{
    if (!P_LEVEL_UP_MOVE_LEARNING)
        return MOVE_NONE;

    enum Move retVal = MOVE_NONE;
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(species);

    // since you can learn more than one move per level
    // the game needs to know whether you decided to
    // learn it or keep the old set to avoid asking
    // you to learn the same move over and over again
    if (firstMove)
    {
        sLearningMoveTableID = 0;

        while (learnset[sLearningMoveTableID].level != level)
        {
            sLearningMoveTableID++;
            if (learnset[sLearningMoveTableID].move == LEVEL_UP_MOVE_END)
                return MOVE_NONE;
        }
    }

    //  Handler for Pokémon whose moves change upon form change.
    //  For example, if Zacian or Zamazenta should learn Iron Head,
    //  they're prevented from doing if they have Behemoth Blade/Bash,
    //  since it transforms into them while in their Crowned forms.
    const struct FormChange *formChanges = GetSpeciesFormChanges(species);

    for (u32 i = 0; formChanges != NULL && formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        if (formChanges[i].method == FORM_CHANGE_END_BATTLE
            && learnset[sLearningMoveTableID].move == formChanges[i].param3)
        {
            for (u32 j = 0; j < MAX_MON_MOVES; j++)
            {
                if (formChanges[i].param2 == GetMonData(mon, MON_DATA_MOVE1 + j))
                {
                    sLearningMoveTableID++;
                    return MON_ALREADY_KNOWS_MOVE;
                }
            }
        }
    }

    if (learnset[sLearningMoveTableID].level == level)
    {
        gMoveToLearn = learnset[sLearningMoveTableID].move;
        sLearningMoveTableID++;
        retVal = GiveMoveToMon(mon, gMoveToLearn);
    }

    return retVal;
}

void DeleteFirstMoveAndGiveMoveToMon(struct Pokemon *mon, enum Move move)
{
    s32 i;
    enum Move moves[MAX_MON_MOVES];
    u8 pp[MAX_MON_MOVES];
    u8 ppBonuses;

    for (i = 0; i < MAX_MON_MOVES - 1; i++)
    {
        moves[i] = GetMonData(mon, MON_DATA_MOVE2 + i);
        pp[i] = GetMonData(mon, MON_DATA_PP2 + i);
    }

    ppBonuses = GetMonData(mon, MON_DATA_PP_BONUSES);
    ppBonuses >>= 2;
    moves[MAX_MON_MOVES - 1] = move;
    pp[MAX_MON_MOVES - 1] = GetMoveMaxPP(move);

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        SetMonData(mon, MON_DATA_MOVE1 + i, &moves[i]);
        SetMonData(mon, MON_DATA_PP1 + i, &pp[i]);
    }

    SetMonData(mon, MON_DATA_PP_BONUSES, &ppBonuses);
}

u32 CountAliveMonsInBattle(u8 caseId, enum BattlerId battler)
{
    enum BattlerId i;
    u32 aliveMonCount = 0;

    switch (caseId)
    {
    case BATTLE_ALIVE_EXCEPT_BATTLER:
        for (i = 0; i < gBattlersCount; i++)
        {
            if (i != battler && IsBattlerAlive(i))
                aliveMonCount++;
        }
        break;
    case BATTLE_ALIVE_EXCEPT_BATTLER_SIDE:
        for (i = 0; i < gBattlersCount; i++)
        {
            if (i != battler && i != GetPartnerBattler(battler) && IsBattlerAlive(i))
                aliveMonCount++;
        }
        break;
    case BATTLE_ALIVE_SIDE:
        for (i = 0; i < gBattlersCount; i++)
        {
            if (IsBattlerAlly(i, battler) && IsBattlerAlive(i))
                aliveMonCount++;
        }
        break;
    }

    return aliveMonCount;
}

u8 GetMonGender(struct Pokemon *mon)
{
    return GetBoxMonGender(&mon->box);
}

u8 GetBoxMonGender(struct BoxPokemon *boxMon)
{
    enum Species species = GetBoxMonData(boxMon, MON_DATA_SPECIES);
    u32 personality = GetBoxMonData(boxMon, MON_DATA_PERSONALITY);

    switch (gSpeciesInfo[species].genderRatio)
    {
    case MON_MALE:
    case MON_FEMALE:
    case MON_GENDERLESS:
        return gSpeciesInfo[species].genderRatio;
    }

    if (gSpeciesInfo[species].genderRatio > (personality & 0xFF))
        return MON_FEMALE;
    else
        return MON_MALE;
}

u8 GetGenderFromSpeciesAndPersonality(enum Species species, u32 personality)
{
    switch (gSpeciesInfo[species].genderRatio)
    {
    case MON_MALE:
    case MON_FEMALE:
    case MON_GENDERLESS:
        return gSpeciesInfo[species].genderRatio;
    }

    if (gSpeciesInfo[species].genderRatio > (personality & 0xFF))
        return MON_FEMALE;
    else
        return MON_MALE;
}

bool32 IsPersonalityFemale(enum Species species, u32 personality)
{
    return GetGenderFromSpeciesAndPersonality(species, personality) == MON_FEMALE;
}

enum Species GetUnownSpeciesId(u32 personality)
{
    u16 unownLetter = GetUnownLetterByPersonality(personality);

    if (unownLetter == 0)
        return SPECIES_UNOWN;
    return unownLetter + SPECIES_UNOWN_B - 1;
}

void SetMultiuseSpriteTemplateToPokemon(enum Species speciesTag, enum BattlerPosition battlerPosition)
{
    if (gMonSpritesGfxPtr != NULL)
        gMultiuseSpriteTemplate = gMonSpritesGfxPtr->templates[battlerPosition];
    else if (sMonSpritesGfxManagers[MON_SPR_GFX_MANAGER_A])
        gMultiuseSpriteTemplate = sMonSpritesGfxManagers[MON_SPR_GFX_MANAGER_A]->templates[battlerPosition];
    else if (sMonSpritesGfxManagers[MON_SPR_GFX_MANAGER_B])
        gMultiuseSpriteTemplate = sMonSpritesGfxManagers[MON_SPR_GFX_MANAGER_B]->templates[battlerPosition];
    else
        gMultiuseSpriteTemplate = gBattlerSpriteTemplates[battlerPosition];

    gMultiuseSpriteTemplate.paletteTag = speciesTag;
    if (battlerPosition == B_POSITION_PLAYER_LEFT || battlerPosition == B_POSITION_PLAYER_RIGHT)
        gMultiuseSpriteTemplate.anims = gAnims_MonPic;
    else
    {
        if (speciesTag > SPECIES_SHINY_TAG)
            speciesTag = speciesTag - SPECIES_SHINY_TAG;

        speciesTag = SanitizeSpeciesId(speciesTag);
        if (gSpeciesInfo[speciesTag].frontAnimFrames != NULL)
            gMultiuseSpriteTemplate.anims = gSpeciesInfo[speciesTag].frontAnimFrames;
        else
            gMultiuseSpriteTemplate.anims = gSpeciesInfo[SPECIES_NONE].frontAnimFrames;
    }
}

void SetMultiuseSpriteTemplateToTrainerBack(enum TrainerPicID trainerPicId, enum BattlerPosition battlerPosition)
{
    gMultiuseSpriteTemplate.paletteTag = GetTrainerPicTag(trainerPicId, FALSE);
    if (battlerPosition == B_POSITION_PLAYER_LEFT || battlerPosition == B_POSITION_PLAYER_RIGHT)
    {
        gMultiuseSpriteTemplate = sTrainerBackSpriteTemplate;
        gMultiuseSpriteTemplate.images = GetTrainerBackPicImage(trainerPicId);
        gMultiuseSpriteTemplate.anims = GetTrainerBackPicAnims(trainerPicId);
    }
    else
    {
        if (gMonSpritesGfxPtr != NULL)
            gMultiuseSpriteTemplate = gMonSpritesGfxPtr->templates[battlerPosition];
        else
            gMultiuseSpriteTemplate = gBattlerSpriteTemplates[battlerPosition];
        gMultiuseSpriteTemplate.anims = gAnims_Trainer;
    }
}

void SetMultiuseSpriteTemplateToTrainerFront(enum TrainerPicID trainerPicId, enum BattlerPosition battlerPosition)
{
    if (gMonSpritesGfxPtr != NULL)
        gMultiuseSpriteTemplate = gMonSpritesGfxPtr->templates[battlerPosition];
    else
        gMultiuseSpriteTemplate = gBattlerSpriteTemplates[battlerPosition];

    gMultiuseSpriteTemplate.paletteTag = GetTrainerPicTag(trainerPicId, TRUE);
    gMultiuseSpriteTemplate.anims = gAnims_Trainer;
}

static void EncryptBoxMon(struct BoxPokemon *boxMon)
{
    for (u32 i = 0; i < ARRAY_COUNT(boxMon->secure.raw); i++)
    {
        boxMon->secure.raw[i] ^= boxMon->personality;
        boxMon->secure.raw[i] ^= boxMon->otId;
    }
}

static void DecryptBoxMon(struct BoxPokemon *boxMon)
{
    for (u32 i = 0; i < ARRAY_COUNT(boxMon->secure.raw); i++)
    {
        boxMon->secure.raw[i] ^= boxMon->otId;
        boxMon->secure.raw[i] ^= boxMon->personality;
    }
}

static const u8 sSubstructOffsets[4][24] =
{
    [SUBSTRUCT_TYPE_0] = {0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 2, 3, 1, 1, 2, 3, 2, 3, 1, 1, 2, 3, 2, 3},
    [SUBSTRUCT_TYPE_1] = {1, 1, 2, 3, 2, 3, 0, 0, 0, 0, 0, 0, 2, 3, 1, 1, 3, 2, 2, 3, 1, 1, 3, 2},
    [SUBSTRUCT_TYPE_2] = {2, 3, 1, 1, 3, 2, 2, 3, 1, 1, 3, 2, 0, 0, 0, 0, 0, 0, 3, 2, 3, 2, 1, 1},
    [SUBSTRUCT_TYPE_3] = {3, 2, 3, 2, 1, 1, 3, 2, 3, 2, 1, 1, 3, 2, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0},
};

ARM_FUNC NOINLINE static u32 ConstantMod24(u32 a) { return a % 24; }

static union PokemonSubstruct *GetSubstruct(struct BoxPokemon *boxMon, u32 personality, enum SubstructType substructType)
{
    return &boxMon->secure.substructs[sSubstructOffsets[substructType][ConstantMod24(personality)]];
}

/* GameFreak called GetMonData with either 2 or 3 arguments, for type
 * safety we have a GetMonData macro (in include/pokemon.h) which
 * dispatches to either GetMonData2 or GetMonData3 based on the number
 * of arguments. */
u32 GetMonData3(struct Pokemon *mon, s32 field, u8 *data)
{
    u32 ret;

    switch (field)
    {
    case MON_DATA_STATUS:
        ret = mon->status;
        break;
    case MON_DATA_LEVEL:
        ret = mon->level;
        break;
    case MON_DATA_HP:
        ret = mon->hp;
        break;
    case MON_DATA_MAX_HP:
        ret = mon->maxHP;
        break;
    case MON_DATA_ATK:
        ret = mon->attack;
        break;
    case MON_DATA_DEF:
        ret = mon->defense;
        break;
    case MON_DATA_SPEED:
        ret = mon->speed;
        break;
    case MON_DATA_SPATK:
        ret = mon->spAttack;
        break;
    case MON_DATA_SPDEF:
        ret = mon->spDefense;
        break;
    case MON_DATA_MAIL:
        ret = mon->mail;
        break;
    default:
        ret = GetBoxMonData(&mon->box, field, data);
        break;
    }
    return ret;
}

u32 GetMonData2(struct Pokemon *mon, s32 field)
{
    return GetMonData3(mon, field, NULL);
}

bool8 MonKnowsMove(struct Pokemon *mon, enum Move move)
{
    return BoxMonKnowsMove(&mon->box, move);
}

bool8 BoxMonKnowsMove(struct BoxPokemon *boxMon, enum Move move)
{
    u8 i;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (GetBoxMonData(boxMon, MON_DATA_MOVE1 + i) == move)
            return TRUE;
    }
    return FALSE;
}

union EvolutionTracker
{
    u16 combinedValue:10;
    struct {
        u16 tracker1: 5;
        u16 tracker2: 5;
    };
};

static ALWAYS_INLINE struct PokemonSubstruct0 *GetSubstruct0(struct BoxPokemon *boxMon)
{
    return &(GetSubstruct(boxMon, boxMon->personality, SUBSTRUCT_TYPE_0)->type0);
}

static ALWAYS_INLINE struct PokemonSubstruct1 *GetSubstruct1(struct BoxPokemon *boxMon)
{
    return &(GetSubstruct(boxMon, boxMon->personality, SUBSTRUCT_TYPE_1)->type1);
}

static ALWAYS_INLINE struct PokemonSubstruct2 *GetSubstruct2(struct BoxPokemon *boxMon)
{
    return &(GetSubstruct(boxMon, boxMon->personality, SUBSTRUCT_TYPE_2)->type2);
}

static ALWAYS_INLINE struct PokemonSubstruct3 *GetSubstruct3(struct BoxPokemon *boxMon)
{
    return &(GetSubstruct(boxMon, boxMon->personality, SUBSTRUCT_TYPE_3)->type3);
}

static bool32 IsBadEgg(struct BoxPokemon *boxMon)
{
    if (boxMon->isBadEgg)
        return TRUE;

    if (CalculateBoxMonChecksum(boxMon) != boxMon->checksum)
    {
        boxMon->isBadEgg = TRUE;
        boxMon->isEgg = TRUE;
        GetSubstruct3(boxMon)->isEgg = TRUE;

        return TRUE;
    }

    return FALSE;
}

static ALWAYS_INLINE bool32 IsEggOrBadEgg(struct BoxPokemon *boxMon)
{
    return GetSubstruct3(boxMon)->isEgg || IsBadEgg(boxMon);
}

/* GameFreak called GetBoxMonData with either 2 or 3 arguments, for type
 * safety we have a GetBoxMonData macro (in include/pokemon.h) which
 * dispatches to either GetBoxMonData2 or GetBoxMonData3 based on the
 * number of arguments. */
u32 GetBoxMonData3(struct BoxPokemon *boxMon, s32 field, u8 *data)
{
    s32 i;
    u32 retVal = 0;

    // Any field greater than MON_DATA_ENCRYPT_SEPARATOR is encrypted and must be treated as such
    if (field > MON_DATA_ENCRYPT_SEPARATOR)
    {
        DecryptBoxMon(boxMon);

        switch (field)
        {
        case MON_DATA_NICKNAME:
        case MON_DATA_NICKNAME10:
        {
            if (IsBadEgg(boxMon))
            {
                for (retVal = 0;
                    retVal < POKEMON_NAME_LENGTH && gText_BadEgg[retVal] != EOS;
                    data[retVal] = gText_BadEgg[retVal], retVal++) {}

                data[retVal] = EOS;
            }
            else if (boxMon->isEgg)
            {
                StringCopy(data, gText_EggNickname);
                retVal = StringLength(data);
            }
            else if (boxMon->language == LANGUAGE_JAPANESE)
            {
                data[0] = EXT_CTRL_CODE_BEGIN;
                data[1] = EXT_CTRL_CODE_JPN;

                for (retVal = 2, i = 0;
                    i < 5 && boxMon->nickname[i] != EOS;
                    data[retVal] = boxMon->nickname[i], retVal++, i++) {}

                data[retVal++] = EXT_CTRL_CODE_BEGIN;
                data[retVal++] = EXT_CTRL_CODE_ENG;
                data[retVal] = EOS;
            }
            else
            {
                retVal = 0;
                while (retVal < min(sizeof(boxMon->nickname), POKEMON_NAME_LENGTH))
                {
                    data[retVal] = boxMon->nickname[retVal];
                    retVal++;
                }

                // Vanilla Pokémon have 0s in nickname11 and nickname12
                // so if both are 0 we assume that this is a vanilla
                // Pokémon and replace them with EOS. This means that
                // two CHAR_SPACE at the end of a nickname are trimmed.
                struct PokemonSubstruct0 *substruct0 = GetSubstruct0(boxMon);
                if (field != MON_DATA_NICKNAME10 && POKEMON_NAME_LENGTH >= 12)
                {
                    if (substruct0->nickname11 == 0 && substruct0->nickname12 == 0)
                    {
                        data[retVal++] = EOS;
                        data[retVal++] = EOS;
                    }
                    else
                    {
                        data[retVal++] = substruct0->nickname11;
                        data[retVal++] = substruct0->nickname12;
                    }
                }
                else if (field != MON_DATA_NICKNAME10 && POKEMON_NAME_LENGTH >= 11)
                {
                    if (substruct0->nickname11 == 0)
                    {
                        data[retVal++] = EOS;
                    }
                    else
                    {
                        data[retVal++] = substruct0->nickname11;
                    }
                }

                data[retVal] = EOS;
            }
            break;
        }
        case MON_DATA_SPECIES:
            retVal = IsBadEgg(boxMon) ? SPECIES_EGG : GetSubstruct0(boxMon)->species;
            break;
        case MON_DATA_HELD_ITEM:
            retVal = GetSubstruct0(boxMon)->heldItem;
            break;
        case MON_DATA_EXP:
            retVal = GetSubstruct0(boxMon)->experience;
            break;
        case MON_DATA_PP_BONUSES:
            retVal = GetSubstruct0(boxMon)->ppBonuses;
            break;
        case MON_DATA_FRIENDSHIP:
            retVal = GetSubstruct0(boxMon)->friendship;
            break;
        case MON_DATA_MOVE1:
            retVal = GetSubstruct1(boxMon)->move1;
            break;
        case MON_DATA_MOVE2:
            retVal = GetSubstruct1(boxMon)->move2;
            break;
        case MON_DATA_MOVE3:
            retVal = GetSubstruct1(boxMon)->move3;
            break;
        case MON_DATA_MOVE4:
            retVal = GetSubstruct1(boxMon)->move4;
            break;
        case MON_DATA_PP1:
            retVal = GetSubstruct1(boxMon)->pp1;
            break;
        case MON_DATA_PP2:
            retVal = GetSubstruct1(boxMon)->pp2;
            break;
        case MON_DATA_PP3:
            retVal = GetSubstruct1(boxMon)->pp3;
            break;
        case MON_DATA_PP4:
            retVal = GetSubstruct1(boxMon)->pp4;
            break;
        case MON_DATA_HP_EV:
            retVal = GetSubstruct2(boxMon)->hpEV;
            break;
        case MON_DATA_ATK_EV:
            retVal = GetSubstruct2(boxMon)->attackEV;
            break;
        case MON_DATA_DEF_EV:
            retVal = GetSubstruct2(boxMon)->defenseEV;
            break;
        case MON_DATA_SPEED_EV:
            retVal = GetSubstruct2(boxMon)->speedEV;
            break;
        case MON_DATA_SPATK_EV:
            retVal = GetSubstruct2(boxMon)->spAttackEV;
            break;
        case MON_DATA_SPDEF_EV:
            retVal = GetSubstruct2(boxMon)->spDefenseEV;
            break;
        case MON_DATA_COOL:
            retVal = GetSubstruct2(boxMon)->cool;
            break;
        case MON_DATA_BEAUTY:
            retVal = GetSubstruct2(boxMon)->beauty;
            break;
        case MON_DATA_CUTE:
            retVal = GetSubstruct2(boxMon)->cute;
            break;
        case MON_DATA_SMART:
            retVal = GetSubstruct2(boxMon)->smart;
            break;
        case MON_DATA_TOUGH:
            retVal = GetSubstruct2(boxMon)->tough;
            break;
        case MON_DATA_SHEEN:
            retVal = GetSubstruct2(boxMon)->sheen;
            break;
        case MON_DATA_POKERUS:
            retVal = GetSubstruct3(boxMon)->pokerus;
            break;
        case MON_DATA_POKERUS_STRAIN:
            retVal = ((GetSubstruct3(boxMon)->pokerus & 0xF0) >> 4);
            break;
        case MON_DATA_POKERUS_DAYS_LEFT:
            retVal = (GetSubstruct3(boxMon)->pokerus & 0x0F);
            break;
        case MON_DATA_MET_LOCATION:
            retVal = GetSubstruct3(boxMon)->metLocation;
            break;
        case MON_DATA_MET_LEVEL:
            retVal = GetSubstruct3(boxMon)->metLevel;
            break;
        case MON_DATA_MET_GAME:
            retVal = GetSubstruct3(boxMon)->metGame;
            break;
        case MON_DATA_POKEBALL:
            retVal = GetSubstruct0(boxMon)->pokeball;
            break;
        case MON_DATA_OT_GENDER:
            retVal = GetSubstruct3(boxMon)->otGender;
            break;
        case MON_DATA_HP_IV:
            retVal = GetSubstruct3(boxMon)->hpIV;
            break;
        case MON_DATA_ATK_IV:
            retVal = GetSubstruct3(boxMon)->attackIV;
            break;
        case MON_DATA_DEF_IV:
            retVal = GetSubstruct3(boxMon)->defenseIV;
            break;
        case MON_DATA_SPEED_IV:
            retVal = GetSubstruct3(boxMon)->speedIV;
            break;
        case MON_DATA_SPATK_IV:
            retVal = GetSubstruct3(boxMon)->spAttackIV;
            break;
        case MON_DATA_SPDEF_IV:
            retVal = GetSubstruct3(boxMon)->spDefenseIV;
            break;
        case MON_DATA_IS_EGG:
            retVal = IsEggOrBadEgg(boxMon);
            break;
        case MON_DATA_ABILITY_NUM:
            retVal = GetSubstruct3(boxMon)->abilityNum;
            break;
        case MON_DATA_COOL_RIBBON:
            retVal = GetSubstruct3(boxMon)->coolRibbon;
            break;
        case MON_DATA_BEAUTY_RIBBON:
            retVal = GetSubstruct3(boxMon)->beautyRibbon;
            break;
        case MON_DATA_CUTE_RIBBON:
            retVal = GetSubstruct3(boxMon)->cuteRibbon;
            break;
        case MON_DATA_SMART_RIBBON:
            retVal = GetSubstruct3(boxMon)->smartRibbon;
            break;
        case MON_DATA_TOUGH_RIBBON:
            retVal = GetSubstruct3(boxMon)->toughRibbon;
            break;
        case MON_DATA_CHAMPION_RIBBON:
            retVal = GetSubstruct3(boxMon)->championRibbon;
            break;
        case MON_DATA_WINNING_RIBBON:
            retVal = GetSubstruct3(boxMon)->winningRibbon;
            break;
        case MON_DATA_VICTORY_RIBBON:
            retVal = GetSubstruct3(boxMon)->victoryRibbon;
            break;
        case MON_DATA_ARTIST_RIBBON:
            retVal = GetSubstruct3(boxMon)->artistRibbon;
            break;
        case MON_DATA_EFFORT_RIBBON:
            retVal = GetSubstruct3(boxMon)->effortRibbon;
            break;
        case MON_DATA_MARINE_RIBBON:
            retVal = GetSubstruct3(boxMon)->marineRibbon;
            break;
        case MON_DATA_LAND_RIBBON:
            retVal = GetSubstruct3(boxMon)->landRibbon;
            break;
        case MON_DATA_SKY_RIBBON:
            retVal = GetSubstruct3(boxMon)->skyRibbon;
            break;
        case MON_DATA_COUNTRY_RIBBON:
            retVal = GetSubstruct3(boxMon)->countryRibbon;
            break;
        case MON_DATA_NATIONAL_RIBBON:
            retVal = GetSubstruct3(boxMon)->nationalRibbon;
            break;
        case MON_DATA_EARTH_RIBBON:
            retVal = GetSubstruct3(boxMon)->earthRibbon;
            break;
        case MON_DATA_WORLD_RIBBON:
            retVal = GetSubstruct3(boxMon)->worldRibbon;
            break;
        case MON_DATA_MODERN_FATEFUL_ENCOUNTER:
            retVal = GetSubstruct3(boxMon)->modernFatefulEncounter;
            break;
        case MON_DATA_SPECIES_OR_EGG:
            retVal = GetSubstruct0(boxMon)->species;
            if (retVal && IsEggOrBadEgg(boxMon))
                retVal = SPECIES_EGG;
            break;
        case MON_DATA_IVS:
        {
            struct PokemonSubstruct3 *substruct3 = GetSubstruct3(boxMon);
            retVal = substruct3->hpIV
                    | (substruct3->attackIV << 5)
                    | (substruct3->defenseIV << 10)
                    | (substruct3->speedIV << 15)
                    | (substruct3->spAttackIV << 20)
                    | (substruct3->spDefenseIV << 25);
            break;
        }
        case MON_DATA_KNOWN_MOVES:
            if (GetSubstruct0(boxMon)->species && !IsEggOrBadEgg(boxMon))
            {
                struct PokemonSubstruct1 *substruct1 = GetSubstruct1(boxMon);
                u16 *moves = (u16 *)data;
                s32 i = 0;

                while (moves[i] != MOVES_COUNT)
                {
                    enum Move move = moves[i];
                    if (substruct1->move1 == move
                        || substruct1->move2 == move
                        || substruct1->move3 == move
                        || substruct1->move4 == move)
                        retVal |= (1u << i);
                    i++;
                }
            }
            break;
        case MON_DATA_RIBBON_COUNT:
            if (GetSubstruct0(boxMon)->species && !IsEggOrBadEgg(boxMon))
            {
                struct PokemonSubstruct3 *substruct3 = GetSubstruct3(boxMon);
                retVal = 0;
                retVal += substruct3->coolRibbon;
                retVal += substruct3->beautyRibbon;
                retVal += substruct3->cuteRibbon;
                retVal += substruct3->smartRibbon;
                retVal += substruct3->toughRibbon;
                retVal += substruct3->championRibbon;
                retVal += substruct3->winningRibbon;
                retVal += substruct3->victoryRibbon;
                retVal += substruct3->artistRibbon;
                retVal += substruct3->effortRibbon;
                retVal += substruct3->marineRibbon;
                retVal += substruct3->landRibbon;
                retVal += substruct3->skyRibbon;
                retVal += substruct3->countryRibbon;
                retVal += substruct3->nationalRibbon;
                retVal += substruct3->earthRibbon;
                retVal += substruct3->worldRibbon;
            }
            break;
        case MON_DATA_RIBBONS:
            if (GetSubstruct0(boxMon)->species && !IsEggOrBadEgg(boxMon))
            {
                struct PokemonSubstruct3 *substruct3 = GetSubstruct3(boxMon);
                retVal = substruct3->championRibbon
                       | (substruct3->coolRibbon << 1)
                       | (substruct3->beautyRibbon << 4)
                       | (substruct3->cuteRibbon << 7)
                       | (substruct3->smartRibbon << 10)
                       | (substruct3->toughRibbon << 13)
                       | (substruct3->winningRibbon << 16)
                       | (substruct3->victoryRibbon << 17)
                       | (substruct3->artistRibbon << 18)
                       | (substruct3->effortRibbon << 19)
                       | (substruct3->marineRibbon << 20)
                       | (substruct3->landRibbon << 21)
                       | (substruct3->skyRibbon << 22)
                       | (substruct3->countryRibbon << 23)
                       | (substruct3->nationalRibbon << 24)
                       | (substruct3->earthRibbon << 25)
                       | (substruct3->worldRibbon << 26);
            }
            break;
        case MON_DATA_HYPER_TRAINED_HP:
            retVal = GetSubstruct1(boxMon)->hyperTrainedHP;
            break;
        case MON_DATA_HYPER_TRAINED_ATK:
            retVal = GetSubstruct1(boxMon)->hyperTrainedAttack;
            break;
        case MON_DATA_HYPER_TRAINED_DEF:
            retVal = GetSubstruct1(boxMon)->hyperTrainedDefense;
            break;
        case MON_DATA_HYPER_TRAINED_SPEED:
            retVal = GetSubstruct1(boxMon)->hyperTrainedSpeed;
            break;
        case MON_DATA_HYPER_TRAINED_SPATK:
            retVal = GetSubstruct1(boxMon)->hyperTrainedSpAttack;
            break;
        case MON_DATA_HYPER_TRAINED_SPDEF:
            retVal = GetSubstruct1(boxMon)->hyperTrainedSpDefense;
            break;
        case MON_DATA_IS_SHADOW:
            retVal = GetSubstruct3(boxMon)->isShadow;
            break;
        case MON_DATA_DYNAMAX_LEVEL:
            retVal = GetSubstruct3(boxMon)->dynamaxLevel;
            break;
        case MON_DATA_GIGANTAMAX_FACTOR:
            retVal = GetSubstruct3(boxMon)->gigantamaxFactor;
            break;
        case MON_DATA_EVOLUTION_TRACKER:
            {
                struct PokemonSubstruct1 *substruct1 = GetSubstruct1(boxMon);
                retVal = (union EvolutionTracker) {
                    .tracker1 = substruct1->evolutionTracker1,
                    .tracker2 = substruct1->evolutionTracker2,
                }.combinedValue;
            }
            break;
        default:
            break;
        }
    }
    else
    {
        switch (field)
        {
        case MON_DATA_STATUS:
            retVal = UncompressStatus(boxMon->compressedStatus);
            break;
        case MON_DATA_HP_LOST:
            retVal = boxMon->hpLost;
            break;
        case MON_DATA_PERSONALITY:
            retVal = boxMon->personality;
            break;
        case MON_DATA_OT_ID:
            retVal = boxMon->otId;
            break;
        case MON_DATA_LANGUAGE:
            retVal = boxMon->language;
            break;
        case MON_DATA_SANITY_IS_BAD_EGG:
            retVal = boxMon->isBadEgg;
            break;
        case MON_DATA_SANITY_HAS_SPECIES:
            retVal = boxMon->hasSpecies;
            break;
        case MON_DATA_SANITY_IS_EGG:
            retVal = boxMon->isEgg;
            break;
        case MON_DATA_OT_NAME:
        {
            retVal = 0;

            while (retVal < PLAYER_NAME_LENGTH)
            {
                data[retVal] = boxMon->otName[retVal];
                retVal++;
            }

            data[retVal] = EOS;
            break;
        }
        case MON_DATA_MARKINGS:
            retVal = boxMon->markings;
            break;
        case MON_DATA_CHECKSUM:
            retVal = boxMon->checksum;
            break;
        case MON_DATA_IS_SHINY:
        {
            u32 shinyValue = GET_SHINY_VALUE(boxMon->otId, boxMon->personality);
            retVal = (shinyValue < SHINY_ODDS) ^ boxMon->shinyModifier;
            break;
        }
        case MON_DATA_HIDDEN_NATURE:
        {
            u32 nature = GetNatureFromPersonality(boxMon->personality);
            retVal = nature ^ boxMon->hiddenNatureModifier;
            break;
        }
        case MON_DATA_DAYS_SINCE_FORM_CHANGE:
            retVal = boxMon->daysSinceFormChange;
            break;
        default:
            break;
        }
    }

    if (field > MON_DATA_ENCRYPT_SEPARATOR)
        EncryptBoxMon(boxMon);

    return retVal;
}

u32 GetBoxMonData2(struct BoxPokemon *boxMon, s32 field)
{
    return GetBoxMonData3(boxMon, field, NULL);
}

#define SET8(lhs) (lhs) = *data
#define SET16(lhs) (lhs) = data[0] + (data[1] << 8)
#define SET32(lhs) (lhs) = data[0] + (data[1] << 8) + (data[2] << 16) + (data[3] << 24)
//
// Prefer SET_BY_WIDTH for fields whose types might be extended (e.g.
// anything whose typedef is in gametypes.h).
//
#define SET_BY_WIDTH(lhs) \
    do { \
       if (sizeof(lhs) == 1) \
          SET8(lhs); \
       else if (sizeof(lhs) == 2) \
          SET16(lhs); \
       else if (sizeof(lhs) == 4) \
          SET32(lhs); \
   } while (0)

void SetMonData(struct Pokemon *mon, s32 field, const void *dataArg)
{
    const u8 *data = dataArg;

    switch (field)
    {
    case MON_DATA_STATUS:
        SET32(mon->status);
        SetBoxMonData(&mon->box, MON_DATA_STATUS, dataArg);
        break;
    case MON_DATA_LEVEL:
        SET8(mon->level);
        break;
    case MON_DATA_HP:
    {
        u32 hpLost;
        SET16(mon->hp);
        hpLost = mon->maxHP - mon->hp;
        SetBoxMonData(&mon->box, MON_DATA_HP_LOST, &hpLost);
        break;
    }
    case MON_DATA_HP_LOST:
    {
        u32 hpLost;
        SET16(hpLost);
        mon->hp = mon->maxHP - hpLost;
        SetBoxMonData(&mon->box, MON_DATA_HP_LOST, &hpLost);
        break;
    }
    case MON_DATA_MAX_HP:
        SET16(mon->maxHP);
        break;
    case MON_DATA_ATK:
        SET16(mon->attack);
        break;
    case MON_DATA_DEF:
        SET16(mon->defense);
        break;
    case MON_DATA_SPEED:
        SET16(mon->speed);
        break;
    case MON_DATA_SPATK:
        SET16(mon->spAttack);
        break;
    case MON_DATA_SPDEF:
        SET16(mon->spDefense);
        break;
    case MON_DATA_MAIL:
        SET8(mon->mail);
        break;
    case MON_DATA_SPECIES_OR_EGG:
        break;
    default:
        SetBoxMonData(&mon->box, field, data);
        break;
    }
}

void SetBoxMonData(struct BoxPokemon *boxMon, s32 field, const void *dataArg)
{
    const u8 *data = dataArg;
    if (field > MON_DATA_ENCRYPT_SEPARATOR)
    {
        if (CalculateBoxMonChecksumDecrypt(boxMon) != boxMon->checksum)
        {
            boxMon->isBadEgg = TRUE;
            boxMon->isEgg = TRUE;
            GetSubstruct3(boxMon)->isEgg = TRUE;
            EncryptBoxMon(boxMon);
            return;
        }

        switch (field)
        {
        case MON_DATA_NICKNAME:
        case MON_DATA_NICKNAME10:
        {
            s32 i;
            struct PokemonSubstruct0 *substruct0 = GetSubstruct0(boxMon);
            for (i = 0; i < min(sizeof(boxMon->nickname), POKEMON_NAME_LENGTH); i++)
                boxMon->nickname[i] = data[i];
            if (field != MON_DATA_NICKNAME10)
            {
                if (POKEMON_NAME_LENGTH >= 11)
                    substruct0->nickname11 = data[10];
                if (POKEMON_NAME_LENGTH >= 12)
                    substruct0->nickname12 = data[11];
            }
            else
            {
                substruct0->nickname11 = EOS;
                substruct0->nickname12 = EOS;
            }
            break;
        }
        case MON_DATA_SPECIES:
        {
            struct PokemonSubstruct0 *substruct0 = GetSubstruct0(boxMon);
            SET16(substruct0->species);
            if (substruct0->species)
                boxMon->hasSpecies = TRUE;
            else
                boxMon->hasSpecies = FALSE;
            break;
        }
        case MON_DATA_HELD_ITEM:
            SET16(GetSubstruct0(boxMon)->heldItem);
            break;
        case MON_DATA_EXP:
            SET32(GetSubstruct0(boxMon)->experience);
            break;
        case MON_DATA_PP_BONUSES:
            SET8(GetSubstruct0(boxMon)->ppBonuses);
            break;
        case MON_DATA_FRIENDSHIP:
            SET8(GetSubstruct0(boxMon)->friendship);
            break;
        case MON_DATA_MOVE1:
            SET16(GetSubstruct1(boxMon)->move1);
            break;
        case MON_DATA_MOVE2:
            SET16(GetSubstruct1(boxMon)->move2);
            break;
        case MON_DATA_MOVE3:
            SET16(GetSubstruct1(boxMon)->move3);
            break;
        case MON_DATA_MOVE4:
            SET16(GetSubstruct1(boxMon)->move4);
            break;
        case MON_DATA_PP1:
            SET8(GetSubstruct1(boxMon)->pp1);
            break;
        case MON_DATA_PP2:
            SET8(GetSubstruct1(boxMon)->pp2);
            break;
        case MON_DATA_PP3:
            SET8(GetSubstruct1(boxMon)->pp3);
            break;
        case MON_DATA_PP4:
            SET8(GetSubstruct1(boxMon)->pp4);
            break;
        case MON_DATA_HP_EV:
            SET8(GetSubstruct2(boxMon)->hpEV);
            break;
        case MON_DATA_ATK_EV:
            SET8(GetSubstruct2(boxMon)->attackEV);
            break;
        case MON_DATA_DEF_EV:
            SET8(GetSubstruct2(boxMon)->defenseEV);
            break;
        case MON_DATA_SPEED_EV:
            SET8(GetSubstruct2(boxMon)->speedEV);
            break;
        case MON_DATA_SPATK_EV:
            SET8(GetSubstruct2(boxMon)->spAttackEV);
            break;
        case MON_DATA_SPDEF_EV:
            SET8(GetSubstruct2(boxMon)->spDefenseEV);
            break;
        case MON_DATA_COOL:
            SET8(GetSubstruct2(boxMon)->cool);
            break;
        case MON_DATA_BEAUTY:
            SET8(GetSubstruct2(boxMon)->beauty);
            break;
        case MON_DATA_CUTE:
            SET8(GetSubstruct2(boxMon)->cute);
            break;
        case MON_DATA_SMART:
            SET8(GetSubstruct2(boxMon)->smart);
            break;
        case MON_DATA_TOUGH:
            SET8(GetSubstruct2(boxMon)->tough);
            break;
        case MON_DATA_SHEEN:
            SET8(GetSubstruct2(boxMon)->sheen);
            break;
        case MON_DATA_POKERUS:
            SET8(GetSubstruct3(boxMon)->pokerus);
            break;
        case MON_DATA_POKERUS_STRAIN:
            GetSubstruct3(boxMon)->pokerus = (*data << 4) | (GetSubstruct3(boxMon)->pokerus & 0x0F);
            break;
        case MON_DATA_POKERUS_DAYS_LEFT:
            GetSubstruct3(boxMon)->pokerus = (GetSubstruct3(boxMon)->pokerus & 0xF0) | *data;
            break;
        case MON_DATA_MET_LOCATION:
            SET8(GetSubstruct3(boxMon)->metLocation);
            break;
        case MON_DATA_MET_LEVEL:
            SET8(GetSubstruct3(boxMon)->metLevel);
            break;
        case MON_DATA_MET_GAME:
            SET8(GetSubstruct3(boxMon)->metGame);
            break;
        case MON_DATA_POKEBALL:
            SET8(GetSubstruct0(boxMon)->pokeball);
            break;
        case MON_DATA_OT_GENDER:
            SET8(GetSubstruct3(boxMon)->otGender);
            break;
        case MON_DATA_HP_IV:
            SET8(GetSubstruct3(boxMon)->hpIV);
            break;
        case MON_DATA_ATK_IV:
            SET8(GetSubstruct3(boxMon)->attackIV);
            break;
        case MON_DATA_DEF_IV:
            SET8(GetSubstruct3(boxMon)->defenseIV);
            break;
        case MON_DATA_SPEED_IV:
            SET8(GetSubstruct3(boxMon)->speedIV);
            break;
        case MON_DATA_SPATK_IV:
            SET8(GetSubstruct3(boxMon)->spAttackIV);
            break;
        case MON_DATA_SPDEF_IV:
            SET8(GetSubstruct3(boxMon)->spDefenseIV);
            break;
        case MON_DATA_IS_EGG:
            SET8(GetSubstruct3(boxMon)->isEgg);
            SET8(boxMon->isEgg);
            break;
        case MON_DATA_ABILITY_NUM:
            SET8(GetSubstruct3(boxMon)->abilityNum);
            break;
        case MON_DATA_COOL_RIBBON:
            SET8(GetSubstruct3(boxMon)->coolRibbon);
            break;
        case MON_DATA_BEAUTY_RIBBON:
            SET8(GetSubstruct3(boxMon)->beautyRibbon);
            break;
        case MON_DATA_CUTE_RIBBON:
            SET8(GetSubstruct3(boxMon)->cuteRibbon);
            break;
        case MON_DATA_SMART_RIBBON:
            SET8(GetSubstruct3(boxMon)->smartRibbon);
            break;
        case MON_DATA_TOUGH_RIBBON:
            SET8(GetSubstruct3(boxMon)->toughRibbon);
            break;
        case MON_DATA_CHAMPION_RIBBON:
            SET8(GetSubstruct3(boxMon)->championRibbon);
            break;
        case MON_DATA_WINNING_RIBBON:
            SET8(GetSubstruct3(boxMon)->winningRibbon);
            break;
        case MON_DATA_VICTORY_RIBBON:
            SET8(GetSubstruct3(boxMon)->victoryRibbon);
            break;
        case MON_DATA_ARTIST_RIBBON:
            SET8(GetSubstruct3(boxMon)->artistRibbon);
            break;
        case MON_DATA_EFFORT_RIBBON:
            SET8(GetSubstruct3(boxMon)->effortRibbon);
            break;
        case MON_DATA_MARINE_RIBBON:
            SET8(GetSubstruct3(boxMon)->marineRibbon);
            break;
        case MON_DATA_LAND_RIBBON:
            SET8(GetSubstruct3(boxMon)->landRibbon);
            break;
        case MON_DATA_SKY_RIBBON:
            SET8(GetSubstruct3(boxMon)->skyRibbon);
            break;
        case MON_DATA_COUNTRY_RIBBON:
            SET8(GetSubstruct3(boxMon)->countryRibbon);
            break;
        case MON_DATA_NATIONAL_RIBBON:
            SET8(GetSubstruct3(boxMon)->nationalRibbon);
            break;
        case MON_DATA_EARTH_RIBBON:
            SET8(GetSubstruct3(boxMon)->earthRibbon);
            break;
        case MON_DATA_WORLD_RIBBON:
            SET8(GetSubstruct3(boxMon)->worldRibbon);
            break;
        case MON_DATA_MODERN_FATEFUL_ENCOUNTER:
            SET8(GetSubstruct3(boxMon)->modernFatefulEncounter);
            break;
        case MON_DATA_IVS:
        {
            u32 ivs;
            struct PokemonSubstruct3 *substruct3 = GetSubstruct3(boxMon);
            SET32(ivs);
            substruct3->hpIV = ivs & MAX_IV_MASK;
            substruct3->attackIV = (ivs >> 5) & MAX_IV_MASK;
            substruct3->defenseIV = (ivs >> 10) & MAX_IV_MASK;
            substruct3->speedIV = (ivs >> 15) & MAX_IV_MASK;
            substruct3->spAttackIV = (ivs >> 20) & MAX_IV_MASK;
            substruct3->spDefenseIV = (ivs >> 25) & MAX_IV_MASK;
            break;
        }
        case MON_DATA_HYPER_TRAINED_HP:
            SET8(GetSubstruct1(boxMon)->hyperTrainedHP);
            break;
        case MON_DATA_HYPER_TRAINED_ATK:
            SET8(GetSubstruct1(boxMon)->hyperTrainedAttack);
            break;
        case MON_DATA_HYPER_TRAINED_DEF:
            SET8(GetSubstruct1(boxMon)->hyperTrainedDefense);
            break;
        case MON_DATA_HYPER_TRAINED_SPEED:
            SET8(GetSubstruct1(boxMon)->hyperTrainedSpeed);
            break;
        case MON_DATA_HYPER_TRAINED_SPATK:
            SET8(GetSubstruct1(boxMon)->hyperTrainedSpAttack);
            break;
        case MON_DATA_HYPER_TRAINED_SPDEF:
            SET8(GetSubstruct1(boxMon)->hyperTrainedSpDefense);
            break;
        case MON_DATA_IS_SHADOW:
            SET8(GetSubstruct3(boxMon)->isShadow);
            break;
        case MON_DATA_DYNAMAX_LEVEL:
            SET8(GetSubstruct3(boxMon)->dynamaxLevel);
            break;
        case MON_DATA_GIGANTAMAX_FACTOR:
            SET8(GetSubstruct3(boxMon)->gigantamaxFactor);
            break;
        case MON_DATA_EVOLUTION_TRACKER:
        {
            union EvolutionTracker evoTracker;
            struct PokemonSubstruct1 *substruct1 = GetSubstruct1(boxMon);
            SET32(evoTracker.combinedValue);
            substruct1->evolutionTracker1 = evoTracker.tracker1;
            substruct1->evolutionTracker2 = evoTracker.tracker2;
            break;
        }
        default:
            break;
        }
    }
    else
    {
        switch (field)
        {
        case MON_DATA_STATUS:
        {
            u32 status;
            SET32(status);
            boxMon->compressedStatus = CompressStatus(status);
            break;
        }
        case MON_DATA_HP_LOST:
            SET16(boxMon->hpLost);
            break;
        case MON_DATA_PERSONALITY:
            SET32(boxMon->personality);
            break;
        case MON_DATA_OT_ID:
            SET32(boxMon->otId);
            break;
        case MON_DATA_LANGUAGE:
            SET8(boxMon->language);
            break;
        case MON_DATA_SANITY_IS_BAD_EGG:
            SET8(boxMon->isBadEgg);
            break;
        case MON_DATA_SANITY_HAS_SPECIES:
            SET8(boxMon->hasSpecies);
            break;
        case MON_DATA_SANITY_IS_EGG:
            SET8(boxMon->isEgg);
            break;
        case MON_DATA_OT_NAME:
        {
            s32 i;
            for (i = 0; i < PLAYER_NAME_LENGTH; i++)
                boxMon->otName[i] = data[i];
            break;
        }
        case MON_DATA_MARKINGS:
            SET8(boxMon->markings);
            break;
        case MON_DATA_CHECKSUM:
            SET16(boxMon->checksum);
            break;
        case MON_DATA_IS_SHINY:
        {
            u32 shinyValue = GET_SHINY_VALUE(boxMon->otId, boxMon->personality);
            bool32 isShiny;
            SET8(isShiny);
            boxMon->shinyModifier = (shinyValue < SHINY_ODDS) ^ isShiny;
            break;
        }
        case MON_DATA_HIDDEN_NATURE:
        {
            u32 nature = GetNatureFromPersonality(boxMon->personality);
            u32 hiddenNature;
            SET8(hiddenNature);
            boxMon->hiddenNatureModifier = nature ^ hiddenNature;
            break;
        }
        case MON_DATA_DAYS_SINCE_FORM_CHANGE:
            SET8(boxMon->daysSinceFormChange);
            break;
        }
    }

    if (field > MON_DATA_ENCRYPT_SEPARATOR)
        boxMon->checksum = CalculateBoxMonChecksumReencrypt(boxMon);
}

// Emerald Champions: every Pokemon the player owns has perfect IVs, so no one
// grinds for them. Ivy in Fallarbor lowers Attack or Speed on request (Trick
// Room, special attackers) and changes Hidden Power; trainers keep their
// authored IVs.
static void MaxBoxMonIVs(struct BoxPokemon *boxMon)
{
    u8 iv = MAX_PER_STAT_IVS;

    if (GetBoxMonData(boxMon, MON_DATA_SPECIES) == SPECIES_NONE)
        return;
    for (u32 stat = 0; stat < NUM_STATS; stat++)
        SetBoxMonData(boxMon, MON_DATA_HP_IV + stat, &iv);
}

void MaxPlayerMonIVs(struct Pokemon *mon)
{
    if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_NONE)
        return;
    MaxBoxMonIVs(&mon->box);
    CalculateMonStats(mon);
}

// Saves from before the rule get one upgrade on load. New games set the flag
// at once, so a Speed or Attack IV the player chose at Ivy's is never undone.
void MaxPlayerIVsIfNeeded(void)
{
    if (FlagGet(FLAG_EC_PLAYER_IVS_MAXED))
        return;
    for (u32 i = 0; i < PARTY_SIZE; i++)
        MaxPlayerMonIVs(&gParties[B_TRAINER_PLAYER][i]);
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
    {
        for (u32 pos = 0; pos < IN_BOX_COUNT; pos++)
            MaxBoxMonIVs(&gPokemonStoragePtr->boxes[box][pos]);
    }
    FlagSet(FLAG_EC_PLAYER_IVS_MAXED);
}

bool32 ClampMonToPlayerLevelCap(struct Pokemon *mon)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    u32 cap;
    u32 experience;

    if (species == SPECIES_NONE || species >= NUM_SPECIES || GetMonData(mon, MON_DATA_IS_EGG))
        return FALSE;
    cap = GetPlayerLevelCapForSpecies(species);
    if (GetLevelFromMonExp(mon) <= cap)
        return FALSE;
    experience = gExperienceTables[gSpeciesInfo[species].growthRate][cap];
    SetMonData(mon, MON_DATA_EXP, &experience);
    CalculateMonStats(mon); // Preserves fainting and clamps HP to the new maximum.
    return TRUE;
}

bool32 ClampBoxMonToPlayerLevelCap(struct BoxPokemon *boxMon)
{
    struct Pokemon mon;
    enum Species species = GetBoxMonData(boxMon, MON_DATA_SPECIES);

    if (species == SPECIES_NONE || species >= NUM_SPECIES || GetBoxMonData(boxMon, MON_DATA_IS_EGG)
        || GetLevelFromBoxMonExp(boxMon) <= GetPlayerLevelCapForSpecies(species))
        return FALSE;
    BoxMonToMon(boxMon, &mon);
    ClampMonToPlayerLevelCap(&mon);
    *boxMon = mon.box;
    return TRUE;
}

enum RestrictedPartyClass GetRestrictedPartyClass(enum Species species)
{
    const struct SpeciesInfo *info;

    if (species == SPECIES_NONE || species == SPECIES_EGG || species >= NUM_SPECIES)
        return RESTRICTED_PARTY_NONE;
    info = &gSpeciesInfo[GET_BASE_SPECIES_ID(species)];
    if (info->isUltraBeast)
        return RESTRICTED_PARTY_ULTRA_BEAST;
    if (info->isRestrictedLegendary || info->isSubLegendary || info->isMythical)
        return RESTRICTED_PARTY_LEGENDARY;
    if (info->isParadox)
        return RESTRICTED_PARTY_PARADOX;
    return RESTRICTED_PARTY_NONE;
}

bool32 CanAddRestrictedMonToParty(enum Species species, s32 replacedSlot)
{
    enum RestrictedPartyClass kind = GetRestrictedPartyClass(species);

    if (kind == RESTRICTED_PARTY_NONE)
        return TRUE;
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
    {
        if ((s32)slot != replacedSlot
         && GetRestrictedPartyClass(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_SPECIES)) == kind)
            return FALSE;
    }
    return TRUE;
}

bool32 PlayerPartyWithinRestrictedLimit(void)
{
    u8 legends = 0, ultraBeasts = 0, paradoxes = 0;

    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
    {
        switch (GetRestrictedPartyClass(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_SPECIES)))
        {
        case RESTRICTED_PARTY_LEGENDARY: legends++; break;
        case RESTRICTED_PARTY_ULTRA_BEAST: ultraBeasts++; break;
        case RESTRICTED_PARTY_PARADOX: paradoxes++; break;
        default: break;
        }
    }
    return legends <= 1 && ultraBeasts <= 1 && paradoxes <= 1;
}

// The League door uses the same party rule as everywhere else: at most one
// Legendary-class, one Ultra Beast and one Paradox Pokemon.
bool32 PlayerPartyLeagueEligible(void)
{
    return PlayerPartyWithinRestrictedLimit();
}

static u8 GiveMonToPartyOrPC(struct Pokemon *mon)
{
    s32 i;

    // Whatever the player receives is the player's, never trainer-owned.
    SetMonTrainerOwned(mon, FALSE);
    ClampMonToPlayerLevelCap(mon);
    MaxPlayerMonIVs(mon);

    if (!CanAddRestrictedMonToParty(GetMonData(mon, MON_DATA_SPECIES), PARTY_SIZE))
        return CopyMonToPC(mon);

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES) == SPECIES_NONE)
            break;
    }

    if (i >= PARTY_SIZE)
        return CopyMonToPC(mon);

    memcpy(&gParties[B_TRAINER_PLAYER][i], mon, sizeof(*mon));
    gPartiesCount[B_TRAINER_PLAYER] = i + 1;
    EmeraldChampions_UnlockBattleItem(GetMonData(mon, MON_DATA_HELD_ITEM));
    return MON_GIVEN_TO_PARTY;
}

u8 GiveCapturedMonToPlayer(struct Pokemon *mon)
{
    SetMonData(mon, MON_DATA_OT_NAME, gSaveBlock2Ptr->playerName);
    SetMonData(mon, MON_DATA_OT_GENDER, &gSaveBlock2Ptr->playerGender);
    SetMonData(mon, MON_DATA_OT_ID, gSaveBlock2Ptr->playerTrainerId);
    return GiveMonToPartyOrPC(mon);
}

u8 CopyMonToPC(struct Pokemon *mon)
{
    s32 boxNo, boxPos;

    SetPCBoxToSendMon(VarGet(VAR_PC_BOX_TO_SEND_MON));

    boxNo = StorageGetCurrentBox();

    do
    {
        for (boxPos = 0; boxPos < IN_BOX_COUNT; boxPos++)
        {
            struct BoxPokemon *checkingMon = GetBoxedMonPtr(boxNo, boxPos);
            if (GetBoxMonData(checkingMon, MON_DATA_SPECIES) == SPECIES_NONE)
            {
                MonRestorePP(mon);
                memcpy(checkingMon, &mon->box, sizeof(mon->box));
                EmeraldChampions_UnlockBattleItem(GetMonData(mon, MON_DATA_HELD_ITEM));
                gSpecialVar_MonBoxId = boxNo;
                gSpecialVar_MonBoxPos = boxPos;
                if (GetPCBoxToSendMon() != boxNo)
                    FlagClear(FLAG_SHOWN_BOX_WAS_FULL_MESSAGE);
                VarSet(VAR_PC_BOX_TO_SEND_MON, boxNo);
                return MON_GIVEN_TO_PC;
            }
        }

        boxNo++;
        if (boxNo == TOTAL_BOXES_COUNT)
            boxNo = 0;
    } while (boxNo != StorageGetCurrentBox());

    return MON_CANT_GIVE;
}

static s32 GetUniquePartyLegendarySlot(void)
{
    s32 found = PARTY_SIZE;
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
    {
        if (GetRestrictedPartyClass(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_SPECIES)) != RESTRICTED_PARTY_LEGENDARY)
            continue;
        if (found != PARTY_SIZE)
            return -1;
        found = slot;
    }
    return found;
}

// A gift was already placed safely in the PC. Replacing the party Legendary
// swaps that exact box slot, so even a now-full PC cannot lose either Pokémon.
u16 GetBoxedLegendaryGiftSwapStatus(void)
{
    if (gPokemonStoragePtr == NULL || gSpecialVar_MonBoxId >= TOTAL_BOXES_COUNT
     || gSpecialVar_MonBoxPos >= IN_BOX_COUNT)
        return 0;

    struct BoxPokemon *gift = GetBoxedMonPtr(gSpecialVar_MonBoxId, gSpecialVar_MonBoxPos);
    if (GetRestrictedPartyClass(GetBoxMonData(gift, MON_DATA_SPECIES)) != RESTRICTED_PARTY_LEGENDARY)
        return 0;

    s32 slot = GetUniquePartyLegendarySlot();
    if (slot < 0 || slot >= PARTY_SIZE)
        return 0;

    GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_NICKNAME, gStringVar1);
    GetBoxMonData(gift, MON_DATA_NICKNAME, gStringVar2);
    return ItemIsMail(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HELD_ITEM)) ? 2 : 1;
}

u16 SwapBoxedLegendaryGiftWithParty(void)
{
    if (GetBoxedLegendaryGiftSwapStatus() != 1)
        return FALSE;

    s32 slot = GetUniquePartyLegendarySlot();
    struct BoxPokemon *giftBox = GetBoxedMonPtr(gSpecialVar_MonBoxId, gSpecialVar_MonBoxPos);
    struct Pokemon gift;
    struct Pokemon old = gParties[B_TRAINER_PLAYER][slot];
    BoxMonToMon(giftBox, &gift);
    MonRestorePP(&old);

    Script_RequestEffects(SCREFF_V1 | SCREFF_SAVE);
    *giftBox = old.box;
    gParties[B_TRAINER_PLAYER][slot] = gift;
    CalculatePlayerPartyCount();
    return TRUE;
}

u8 CalculatePartyCount(enum BattleTrainer trainer)
{
    u32 partyCount = 0;

    while (partyCount < PARTY_SIZE
        && GetMonData(&gParties[trainer][partyCount], MON_DATA_SPECIES) != SPECIES_NONE)
    {
        partyCount++;
    }

    return partyCount;
}

u8 CalculatePartyCountOfSide(enum BattlerId battler)
{
    return CalculatePartyCount(GetBattlerTrainer(battler)) + (BattleSideHasTwoTrainers(battler & BIT_SIDE) ? CalculatePartyCount(GetBattlerTrainer(GetPartnerBattler(battler))) : 0);
}

u8 CalculatePlayerPartyCount(void)
{
    gPartiesCount[B_TRAINER_PLAYER] = CalculatePartyCount(B_TRAINER_PLAYER);
    return gPartiesCount[B_TRAINER_PLAYER];
}

u8 CalculateEnemyPartyCount(void)
{
    gPartiesCount[B_TRAINER_OPPONENT_A] = CalculatePartyCount(B_TRAINER_OPPONENT_A);
    gPartiesCount[B_TRAINER_OPPONENT_B] = CalculatePartyCount(B_TRAINER_OPPONENT_B);
    return gPartiesCount[B_TRAINER_OPPONENT_A] + gPartiesCount[B_TRAINER_OPPONENT_B];
}

u8 GetMonsStateToDoubles(void)
{
    s32 aliveCount = 0;
    s32 i;
    CalculatePlayerPartyCount();

    if (OW_DOUBLE_APPROACH_WITH_ONE_MON)
        return PLAYER_HAS_TWO_USABLE_MONS;

    if (gPartiesCount[B_TRAINER_PLAYER] == 1)
        return gPartiesCount[B_TRAINER_PLAYER]; // PLAYER_HAS_ONE_MON

    for (i = 0; i < gPartiesCount[B_TRAINER_PLAYER]; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG) != SPECIES_EGG
         && GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HP) != 0
         && GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG) != SPECIES_NONE)
            aliveCount++;
    }

    return (aliveCount > 1) ? PLAYER_HAS_TWO_USABLE_MONS : PLAYER_HAS_ONE_USABLE_MON;
}

u8 GetMonsStateToDoubles_2(void)
{
    s32 aliveCount = 0;
    s32 i;

    if (OW_DOUBLE_APPROACH_WITH_ONE_MON
     || FollowerNPCIsBattlePartner())
        return PLAYER_HAS_TWO_USABLE_MONS;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        enum Species species = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG);
        if (species != SPECIES_EGG && species != SPECIES_NONE
         && GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HP) != 0)
            aliveCount++;
    }

    if (aliveCount == 1)
        return PLAYER_HAS_ONE_MON; // may have more than one, but only one is alive

    return (aliveCount > 1) ? PLAYER_HAS_TWO_USABLE_MONS : PLAYER_HAS_ONE_USABLE_MON;
}

// gSpeciesInfo's Ability for a slot, as trainer-owned Pokemon and the
// Pokedex use it. Pokemon-specific callers use GetMonAbility.
enum Ability GetAbilityBySpecies(enum Species species, u8 abilityNum)
{
    return GetAbilityBySpeciesForOwner(species, abilityNum, TRUE);
}

// Slots 0-2 always resolve through gSpeciesInfo exactly as before. Slots 3
// and up (ABILITY_SLOT_INCLEMENT + i) are the species' Inclement added
// Abilities for a Pokemon that is not trainer-owned; an empty one resolves
// like slot 0.
enum Ability GetAbilityBySpeciesForOwner(enum Species species, u8 abilityNum, bool32 trainerOwned)
{
    int i;

    if (abilityNum >= ABILITY_SLOT_INCLEMENT && !trainerOwned)
    {
        gLastUsedAbility = GetInclementAddedAbility(species, abilityNum);
        if (gLastUsedAbility != ABILITY_NONE)
            return gLastUsedAbility;
        abilityNum = 0;
    }

    if (abilityNum < NUM_ABILITY_SLOTS)
        gLastUsedAbility = GetSpeciesAbility(species, abilityNum);
    else
        gLastUsedAbility = ABILITY_NONE;

    if (abilityNum >= NUM_NORMAL_ABILITY_SLOTS) // if abilityNum is empty hidden ability, look for other hidden abilities
    {
        for (i = NUM_NORMAL_ABILITY_SLOTS; i < NUM_ABILITY_SLOTS && gLastUsedAbility == ABILITY_NONE; i++)
        {
            gLastUsedAbility = GetSpeciesAbility(species, i);
        }
    }

    for (i = 0; i < NUM_ABILITY_SLOTS && gLastUsedAbility == ABILITY_NONE; i++) // look for any non-empty ability
    {
        gLastUsedAbility = GetSpeciesAbility(species, i);
    }

    return gLastUsedAbility;
}

enum Ability GetMonAbility(struct Pokemon *mon)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    u8 abilityNum = GetMonData(mon, MON_DATA_ABILITY_NUM);
    return GetAbilityBySpeciesForOwner(species, abilityNum, IsMonTrainerOwned(mon));
}

void CreateSecretBaseEnemyParty(struct SecretBase *secretBaseRecord)
{
    s32 i, j;

    ZeroEnemyPartyMons();
    *gBattleResources->secretBase = *secretBaseRecord;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (gBattleResources->secretBase->party.species[i])
        {
            CreateMonWithIVs(&gParties[B_TRAINER_OPPONENT_A][i],
                gBattleResources->secretBase->party.species[i],
                gBattleResources->secretBase->party.levels[i],
                gBattleResources->secretBase->party.personality[i],
                OTID_STRUCT_RANDOM_NO_SHINY,
                MAX_PER_STAT_IVS);
            SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_HELD_ITEM, &gBattleResources->secretBase->party.heldItems[i]);

            for (j = 0; j < NUM_STATS; j++)
                SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_HP_EV + j, &gBattleResources->secretBase->party.EVs[i]);

            for (j = 0; j < MAX_MON_MOVES; j++)
            {
                SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_MOVE1 + j, &gBattleResources->secretBase->party.moves[i * MAX_MON_MOVES + j]);
                u32 pp = GetMoveMaxPP(gBattleResources->secretBase->party.moves[i * MAX_MON_MOVES + j]);
                SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_PP1 + j, &pp);
            }
        }
    }
}

enum TrainerPicID GetSecretBaseTrainerPicIndex(void)
{
    u8 facilityClass = sSecretBaseFacilityClasses[gBattleResources->secretBase->gender][gBattleResources->secretBase->trainerId[0] % NUM_SECRET_BASE_CLASSES];
    return gFacilityClassToPicIndex[facilityClass];
}

enum TrainerClassID GetSecretBaseTrainerClass(void)
{
    u8 facilityClass = sSecretBaseFacilityClasses[gBattleResources->secretBase->gender][gBattleResources->secretBase->trainerId[0] % NUM_SECRET_BASE_CLASSES];
    return gFacilityClassToTrainerClass[facilityClass];
}

bool8 IsPlayerPartyAndPokemonStorageFull(void)
{
    s32 i;

    for (i = 0; i < PARTY_SIZE; i++)
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES) == SPECIES_NONE)
            return FALSE;

    return IsPokemonStorageFull();
}

bool8 IsPokemonStorageFull(void)
{
    s32 i, j;

    for (i = 0; i < TOTAL_BOXES_COUNT; i++)
        for (j = 0; j < IN_BOX_COUNT; j++)
            if (GetBoxMonDataAt(i, j, MON_DATA_SPECIES) == SPECIES_NONE)
                return FALSE;

    return TRUE;
}

const u8 *GetSpeciesName(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].speciesName[0] == 0)
        return gSpeciesInfo[SPECIES_NONE].speciesName;
    return gSpeciesInfo[species].speciesName;
}

const u8 *GetSpeciesCategory(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].categoryName[0] == 0)
        return gSpeciesInfo[SPECIES_NONE].categoryName;
    return gSpeciesInfo[species].categoryName;
}

const u8 *GetSpeciesPokedexDescription(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].description == NULL)
        return gSpeciesInfo[SPECIES_NONE].description;
    return gSpeciesInfo[species].description;
}

u32 GetSpeciesHeight(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].height;
}

u32 GetSpeciesWeight(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].weight;
}

enum Type GetSpeciesType(enum Species species, u8 slot)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].types[slot];
}

enum Ability GetSpeciesAbility(enum Species species, u8 slot)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].abilities[slot];
}

// The Inclement Ability added in an ability slot (3 and up), or ABILITY_NONE.
enum Ability GetInclementAddedAbility(enum Species species, u32 slot)
{
    if (!IS_INCLEMENT_ABILITY_SLOT(slot))
        return ABILITY_NONE;
    return sInclementLayer[SanitizeSpeciesId(species)].addedAbilities[slot - ABILITY_SLOT_INCLEMENT];
}

// The Ability stored in a slot (0-2: gSpeciesInfo; 3: the Inclement extra,
// which trainer-owned Pokemon never have). ABILITY_NONE when empty.
enum Ability GetSpeciesAbilityForOwner(enum Species species, u8 slot, bool32 trainerOwned)
{
    if (slot >= ABILITY_SLOT_INCLEMENT)
        return trainerOwned ? ABILITY_NONE : GetInclementAddedAbility(species, slot);
    if (slot >= NUM_ABILITY_SLOTS)
        return ABILITY_NONE;
    return GetSpeciesAbility(species, slot);
}

bool32 FindSpeciesAbilitySlotForOwner(enum Species species, enum Ability ability, bool32 trainerOwned, u32 *slot)
{
    for (u32 i = 0; i < NUM_ABILITY_SLOTS; i++)
    {
        if (GetSpeciesAbility(species, i) == ability)
        {
            *slot = i;
            return TRUE;
        }
    }
    // Inclement slots only ever hold a real Ability, and never for a trainer.
    for (u32 i = ABILITY_SLOT_INCLEMENT; ability != ABILITY_NONE && i < NUM_OWNER_ABILITY_SLOTS; i++)
    {
        if (GetSpeciesAbilityForOwner(species, i, trainerOwned) == ability)
        {
            *slot = i;
            return TRUE;
        }
    }
    return FALSE;
}

// Normal (non-hidden) slots a Pokemon may hold, in cycle order: 0, then 1
// when it names a different Ability, then the Inclement slots.
static u32 GetNormalAbilitySlots(enum Species species, bool32 trainerOwned, u8 *slots)
{
    u32 count = 0;
    enum Ability first = GetSpeciesAbility(species, 0);

    slots[count++] = 0;
    if (GetSpeciesAbility(species, 1) != ABILITY_NONE && GetSpeciesAbility(species, 1) != first)
        slots[count++] = 1;
    for (u32 slot = ABILITY_SLOT_INCLEMENT; !trainerOwned && slot < NUM_OWNER_ABILITY_SLOTS; slot++)
    {
        if (GetInclementAddedAbility(species, slot) != ABILITY_NONE)
            slots[count++] = slot;
    }
    return count;
}

// Random normal slot for a new Pokemon, Inclement slots included. Without an
// Inclement slot this is the original personality bit (slot 1 only when the
// species has one).
u32 RollNormalAbilitySlot(enum Species species, u32 personality)
{
    u8 slots[NUM_OWNER_ABILITY_SLOTS];
    u32 count = 0;

    slots[count++] = 0;
    if (GetSpeciesAbility(species, 1) != ABILITY_NONE)
        slots[count++] = 1;
    for (u32 slot = ABILITY_SLOT_INCLEMENT; slot < NUM_OWNER_ABILITY_SLOTS; slot++)
    {
        if (GetInclementAddedAbility(species, slot) != ABILITY_NONE)
            slots[count++] = slot;
    }
    return slots[personality % count];
}

// Every distinct Ability a Pokemon may switch to (party menu Ability), in the
// order normal slots, Inclement slots, hidden slot. Returns the count.
u32 GetMonSelectableAbilitySlots(struct Pokemon *mon, u8 *slots)
{
    u8 order[NUM_OWNER_ABILITY_SLOTS];
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    bool32 trainerOwned = IsMonTrainerOwned(mon);
    enum Ability seen[NUM_OWNER_ABILITY_SLOTS] = {ABILITY_NONE};
    u32 count = 0, orderCount = 0;

    order[orderCount++] = 0;
    order[orderCount++] = 1;
    for (u32 slot = ABILITY_SLOT_INCLEMENT; slot < NUM_OWNER_ABILITY_SLOTS; slot++)
        order[orderCount++] = slot;
    order[orderCount++] = 2;
    for (u32 i = 0; i < orderCount; i++)
    {
        enum Ability ability = GetSpeciesAbilityForOwner(species, order[i], trainerOwned);
        bool32 duplicate = FALSE;

        if (ability == ABILITY_NONE)
            continue;
        for (u32 j = 0; j < count; j++)
            duplicate |= seen[j] == ability;
        if (duplicate)
            continue;
        seen[count] = ability;
        if (slots != NULL)
            slots[count] = order[i];
        count++;
    }
    return count;
}

// Ability Capsule: the next normal slot in the cycle (Inclement slots
// included). NUM_OWNER_ABILITY_SLOTS when it has no effect (a hidden
// Ability, or only one normal Ability).
u32 GetAbilityCapsuleTargetSlot(struct Pokemon *mon)
{
    u8 slots[NUM_OWNER_ABILITY_SLOTS];
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    u32 current = GetMonData(mon, MON_DATA_ABILITY_NUM);
    u32 count = GetNormalAbilitySlots(species, IsMonTrainerOwned(mon), slots);

    if (species == SPECIES_NONE || count < 2 || current == 2)
        return NUM_OWNER_ABILITY_SLOTS;
    for (u32 i = 0; i < count; i++)
    {
        if (slots[i] == current)
            return slots[(i + 1) % count];
    }
    return slots[0];
}

// Ability Patch: hidden slot to slot 0, any normal slot (Inclement slots
// included) to the hidden slot. NUM_OWNER_ABILITY_SLOTS when it has no effect.
u32 GetAbilityPatchTargetSlot(struct Pokemon *mon)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);

    if (species == SPECIES_NONE)
        return NUM_OWNER_ABILITY_SLOTS;
    if (GetMonData(mon, MON_DATA_ABILITY_NUM) == 2)
        return 0;
    if (GetSpeciesAbility(species, 2) == ABILITY_NONE)
        return NUM_OWNER_ABILITY_SLOTS;
    return 2;
}

u32 GetSpeciesBaseHP(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].baseHP;
}

u32 GetSpeciesBaseAttack(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].baseAttack;
}

u32 GetSpeciesBaseDefense(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].baseDefense;
}

u32 GetSpeciesBaseSpAttack(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].baseSpAttack;
}

u32 GetSpeciesBaseSpDefense(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].baseSpDefense;
}

u32 GetSpeciesBaseSpeed(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].baseSpeed;
}

u32 GetSpeciesBaseStat(enum Species species, u32 statIndex)
{
    switch (statIndex)
    {
    case STAT_HP:
        return GetSpeciesBaseHP(species);
    case STAT_ATK:
        return GetSpeciesBaseAttack(species);
    case STAT_DEF:
        return GetSpeciesBaseDefense(species);
    case STAT_SPEED:
        return GetSpeciesBaseSpeed(species);
    case STAT_SPATK:
        return GetSpeciesBaseSpAttack(species);
    case STAT_SPDEF:
        return GetSpeciesBaseSpDefense(species);
    }
    return 0;
}

// The base stat line of wild and player-owned Pokemon: gSpeciesInfo plus
// Inclement's buffs.
u32 GetInclementSpeciesBaseStat(enum Species species, u32 statIndex)
{
    species = SanitizeSpeciesId(species);
    if (statIndex < NUM_STATS && sInclementLayer[species].hasBaseStats)
        return sInclementLayer[species].baseStats[statIndex];
    return GetSpeciesBaseStat(species, statIndex);
}

u32 GetSpeciesBaseStatForOwner(enum Species species, u32 statIndex, bool32 trainerOwned)
{
    return trainerOwned ? GetSpeciesBaseStat(species, statIndex) : GetInclementSpeciesBaseStat(species, statIndex);
}

u32 GetSpeciesBaseStatTotal(enum Species species)
{
    u32 total = 0;

    for (u32 i = 0; i < NUM_STATS; i++)
        total += GetSpeciesBaseStat(species, i);

    return total;
}

const struct LevelUpMove *GetSpeciesLevelUpLearnset(enum Species species)
{
    const struct LevelUpMove *learnset = gSpeciesInfo[SanitizeSpeciesId(species)].levelUpLearnset;
    if (learnset == NULL)
        return gSpeciesInfo[SPECIES_NONE].levelUpLearnset;
    return learnset;
}

const u16 *GetSpeciesTeachableLearnset(enum Species species)
{
    const u16 *learnset = gSpeciesInfo[SanitizeSpeciesId(species)].teachableLearnset;
    if (learnset == NULL)
        return gSpeciesInfo[SPECIES_NONE].teachableLearnset;
    return learnset;
}

const u16 *GetSpeciesEggMoves(enum Species species)
{
    const u16 *learnset = gSpeciesInfo[SanitizeSpeciesId(species)].eggMoveLearnset;
    if (learnset == NULL)
        return gSpeciesInfo[SPECIES_NONE].eggMoveLearnset;
    return learnset;
}

//only used in test assumptions at the moment
bool32 SpeciesHasEggMove(enum Species species, enum Move move)
{
    const u16 *learnset = GetSpeciesEggMoves(species);
    for (u32 i = 0; learnset[i] != MOVE_UNAVAILABLE; i++)
    {
        if (learnset[i] == move)
            return TRUE;
    }
    return FALSE;
}

const struct Evolution *GetSpeciesEvolutions(enum Species species)
{
    const struct Evolution *evolutions = gSpeciesInfo[SanitizeSpeciesId(species)].evolutions;
    if (evolutions == NULL)
        return gSpeciesInfo[SPECIES_NONE].evolutions;
    return evolutions;
}

const u16 *GetSpeciesFormTable(enum Species species)
{
    const u16 *formTable = gSpeciesInfo[SanitizeSpeciesId(species)].formSpeciesIdTable;
    if (formTable == NULL)
        return gSpeciesInfo[SPECIES_NONE].formSpeciesIdTable;
    return formTable;
}

const struct FormChange *GetSpeciesFormChanges(enum Species species)
{
    const struct FormChange *formChanges = gSpeciesInfo[SanitizeSpeciesId(species)].formChangeTable;
    if (formChanges == NULL)
        return gSpeciesInfo[SPECIES_NONE].formChangeTable;
    return formChanges;
}

void RemoveMonPPBonus(struct Pokemon *mon, u8 moveIndex)
{
    RemoveBoxMonPPBonus(&mon->box, moveIndex);
}

void RemoveBoxMonPPBonus(struct BoxPokemon *mon, u8 moveIndex)
{
    u8 ppBonuses = GetBoxMonData(mon, MON_DATA_PP_BONUSES);
    ppBonuses &= gPPUpClearMask[moveIndex];
    SetBoxMonData(mon, MON_DATA_PP_BONUSES, &ppBonuses);
}

void PokemonToBattleMon(struct Pokemon *src, struct BattlePokemon *dst)
{
    s32 i;
    u8 nickname[POKEMON_NAME_BUFFER_SIZE];

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        dst->moves[i] = GetMonData(src, MON_DATA_MOVE1 + i);
        dst->pp[i] = GetMonData(src, MON_DATA_PP1 + i);
    }

    dst->species = GetMonData(src, MON_DATA_SPECIES);
    dst->item = GetMonData(src, MON_DATA_HELD_ITEM);
    dst->ppBonuses = GetMonData(src, MON_DATA_PP_BONUSES);
    dst->friendship = GetMonData(src, MON_DATA_FRIENDSHIP);
    dst->experience = GetMonData(src, MON_DATA_EXP);
    dst->hpIV = GetMonData(src, MON_DATA_HP_IV);
    dst->attackIV = GetMonData(src, MON_DATA_ATK_IV);
    dst->defenseIV = GetMonData(src, MON_DATA_DEF_IV);
    dst->speedIV = GetMonData(src, MON_DATA_SPEED_IV);
    dst->spAttackIV = GetMonData(src, MON_DATA_SPATK_IV);
    dst->spDefenseIV = GetMonData(src, MON_DATA_SPDEF_IV);
    dst->personality = GetMonData(src, MON_DATA_PERSONALITY);
    dst->status1 = GetMonData(src, MON_DATA_STATUS);
    dst->level = GetMonData(src, MON_DATA_LEVEL);
    dst->hp = GetMonData(src, MON_DATA_HP);
    dst->maxHP = GetMonData(src, MON_DATA_MAX_HP);
    dst->attack = GetMonData(src, MON_DATA_ATK);
    dst->defense = GetMonData(src, MON_DATA_DEF);
    dst->speed = GetMonData(src, MON_DATA_SPEED);
    dst->spAttack = GetMonData(src, MON_DATA_SPATK);
    dst->spDefense = GetMonData(src, MON_DATA_SPDEF);
    dst->abilityNum = GetMonData(src, MON_DATA_ABILITY_NUM);
    dst->otId = GetMonData(src, MON_DATA_OT_ID);
    dst->types[0] = GetSpeciesType(dst->species, 0);
    dst->types[1] = GetSpeciesType(dst->species, 1);
    dst->types[2] = TYPE_MYSTERY;
    dst->isShiny = IsMonShiny(src);
    dst->affectionHearts = GetMonAffectionHearts(src);
    dst->ability = GetAbilityBySpeciesForOwner(dst->species, dst->abilityNum, IsMonTrainerOwned(src));
    GetMonData(src, MON_DATA_NICKNAME, nickname);
    StringCopy_Nickname(dst->nickname, nickname);
    GetMonData(src, MON_DATA_OT_NAME, dst->otName);

    for (i = 0; i < NUM_BATTLE_STATS; i++)
        dst->statStages[i] = DEFAULT_STAT_STAGE;

    memset(&dst->volatiles, 0, sizeof(struct Volatiles));
}

bool8 ExecuteTableBasedItemEffect(struct Pokemon *mon, enum Item item, u8 partyIndex, u8 moveIndex)
{
    return PokemonUseItemEffects(mon, item, partyIndex, moveIndex, FALSE);
}

#define UPDATE_FRIENDSHIP_FROM_ITEM()                                                                   \
{                                                                                                       \
    if ((!retVal || friendshipOnly) && !ShouldSkipFriendshipChange() && friendshipChange == 0)      \
    {                                                                                                   \
        friendshipChange = itemEffect[itemEffectParam];                                                 \
        friendship = GetMonData(mon, MON_DATA_FRIENDSHIP);                                        \
        friendship += CalculateFriendshipBonuses(mon,friendshipChange,holdEffect);                      \
        if (friendship < 0)                                                                             \
            friendship = 0;                                                                             \
        if (friendship > MAX_FRIENDSHIP)                                                                \
            friendship = MAX_FRIENDSHIP;                                                                \
        SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);                                              \
        retVal = FALSE;                                                                                 \
    }                                                                                                   \
}

// EXP candies store an index for this table in their holdEffectParam.
const u32 sExpCandyExperienceTable[] = {
    [EXP_100 - 1] = 100,
    [EXP_800 - 1] = 800,
    [EXP_3000 - 1] = 3000,
    [EXP_10000 - 1] = 10000,
    [EXP_30000 - 1] = 30000,
};

// Returns TRUE if the item has no effect on the Pokémon, FALSE otherwise
bool8 PokemonUseItemEffects(struct Pokemon *mon, enum Item item, u8 partyIndex, u8 moveIndex, bool8 usedByAI)
{
    u32 dataUnsigned;
    s32 dataSigned, evCap;
    s32 friendship;
    s32 i;
    bool8 retVal = TRUE;
    const u8 *itemEffect;
    u8 itemEffectParam = ITEM_EFFECT_ARG_START;
    u32 temp1, temp2;
    s8 friendshipChange = 0;
    enum HoldEffect holdEffect;
    enum BattlerId battler = MAX_BATTLERS_COUNT;
    bool32 friendshipOnly = FALSE;
    enum Item heldItem;
    u8 effectFlags;
    s8 evChange;
    u16 evCount;
    u8 levelBefore;
    bool8 didLevelUp = FALSE;
    bool8 isLevelUpItem;

    // Determine the EV cap to use
    u32 maxAllowedEVs = !B_EV_ITEMS_CAP ? MAX_TOTAL_EVS : GetCurrentEVCap();

    // Get item hold effect
    heldItem = GetMonData(mon, MON_DATA_HELD_ITEM);
    holdEffect = GetItemHoldEffect(heldItem);

    // Skip using the item if it won't do anything
    if (GetItemEffect(item) == NULL && item != ITEM_ENIGMA_BERRY_E_READER)
        return TRUE;

    // Get item effect
    itemEffect = GetItemEffect(item);
    isLevelUpItem = (itemEffect[3] & ITEM3_LEVEL_UP) != 0;
    levelBefore = GetMonData(mon, MON_DATA_LEVEL, NULL);

    // Do item effect
    for (i = 0; i < ITEM_EFFECT_ARG_START; i++)
    {
        switch (i)
        {

        // Handle ITEM0 effects (infatuation, Dire Hit, X Attack). ITEM0_SACRED_ASH is handled in party_menu.c
        // Now handled in item battle scripts.
        case 0:
            break;

        // Handle ITEM1 effects (in-battle stat boosting effects)
        // Now handled in item battle scripts.
        case 1:
            break;
        // Formerly used by the item effects of the X Sp. Atk and the X Accuracy
        case 2:
            break;

        // Handle ITEM3 effects (Guard Spec, Rare Candy, cure status)
        case 3:
            // Rare Candy / EXP Candy
            if ((itemEffect[i] & ITEM3_LEVEL_UP)
             && GetMonData(mon, MON_DATA_LEVEL) != MAX_LEVEL)
            {
                u8 param = GetItemHoldEffectParam(item);
                dataUnsigned = 0;

                if (param == 0) // Rare Candy
                {
                    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
                    u32 level = GetMonData(mon, MON_DATA_LEVEL);
                    if (!B_RARE_CANDY_CAP || level < GetPlayerLevelCapForSpecies(species))
                        dataUnsigned = gExperienceTables[gSpeciesInfo[species].growthRate][level + 1];
                }
                else if (param - 1 < ARRAY_COUNT(sExpCandyExperienceTable)) // EXP Candies
                {
                    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
                    dataUnsigned = sExpCandyExperienceTable[param - 1] + GetMonData(mon, MON_DATA_EXP);

                    if (B_RARE_CANDY_CAP && B_EXP_CAP_TYPE == EXP_CAP_HARD)
                    {
                        u32 currentLevelCap = GetPlayerLevelCapForSpecies(species);
                        if (dataUnsigned > gExperienceTables[gSpeciesInfo[species].growthRate][currentLevelCap])
                            dataUnsigned = gExperienceTables[gSpeciesInfo[species].growthRate][currentLevelCap];
                    }
                    else if (dataUnsigned > gExperienceTables[gSpeciesInfo[species].growthRate][MAX_LEVEL])
                    {
                        dataUnsigned = gExperienceTables[gSpeciesInfo[species].growthRate][MAX_LEVEL];
                    }
                }

                if (dataUnsigned != 0) // Failsafe
                {
                    SetMonData(mon, MON_DATA_EXP, &dataUnsigned);
                    CalculateMonStats(mon);
                    if (GetMonData(mon, MON_DATA_LEVEL, NULL) > levelBefore)
                        didLevelUp = TRUE;
                    retVal = FALSE;
                }
            }

            // Cure status
            if ((itemEffect[i] & ITEM3_SLEEP) && HealStatusConditions(mon, STATUS1_SLEEP, battler) == 0)
                retVal = FALSE;
            if ((itemEffect[i] & ITEM3_POISON) && HealStatusConditions(mon, STATUS1_PSN_ANY | STATUS1_TOXIC_COUNTER, battler) == 0)
                retVal = FALSE;
            if ((itemEffect[i] & ITEM3_BURN) && HealStatusConditions(mon, STATUS1_BURN, battler) == 0)
                retVal = FALSE;
            if ((itemEffect[i] & ITEM3_FREEZE) && HealStatusConditions(mon, STATUS1_ICY_ANY, battler) == 0)
                retVal = FALSE;
            if ((itemEffect[i] & ITEM3_PARALYSIS) && HealStatusConditions(mon, STATUS1_PARALYSIS, battler) == 0)
                retVal = FALSE;
            break;

        // Handle ITEM4 effects (Change HP/Atk EVs, HP heal, PP heal, PP up, Revive, and evolution stones)
        case 4:
            effectFlags = itemEffect[i];

            // PP Up
            if (effectFlags & ITEM4_PP_UP)
            {
                u32 ppBonuses = GetMonData(mon, MON_DATA_PP_BONUSES);
                effectFlags &= ~ITEM4_PP_UP;
                dataUnsigned = (ppBonuses & gPPUpGetMask[moveIndex]) >> (moveIndex * 2);
                temp1 = GetMoveMaxPP(GetMonData(mon, MON_DATA_MOVE1 + moveIndex));
                if (dataUnsigned <= 2 && temp1 > 4)
                {
                    dataUnsigned = ppBonuses + gPPUpAddValues[moveIndex];
                    SetMonData(mon, MON_DATA_PP_BONUSES, &dataUnsigned);

                    dataUnsigned = GetMoveMaxPP(GetMonData(mon, MON_DATA_MOVE1 + moveIndex)) - temp1;
                    dataUnsigned = GetMonData(mon, MON_DATA_PP1 + moveIndex) + dataUnsigned;
                    SetMonData(mon, MON_DATA_PP1 + moveIndex, &dataUnsigned);
                    retVal = FALSE;
                }
            }
            temp1 = 0;

            // Loop through and try each of the remaining ITEM4 effects
            while (effectFlags != 0)
            {
                if (effectFlags & 1)
                {
                    switch (temp1)
                    {
                    case 0: // ITEM4_EV_HP
                    case 1: // ITEM4_EV_ATK
                        evCount = GetMonEVCount(mon);
                        temp2 = itemEffect[itemEffectParam];
                        dataSigned = GetMonData(mon, sGetMonDataEVConstants[temp1]);
                        evChange = temp2;

                        if (evChange > 0) // Increasing EV (HP or Atk)
                        {
                            // Check if the total EV limit is reached
                            if (evCount >= maxAllowedEVs)
                                return TRUE;

                            // Ensure the increase does not exceed the max EV per stat (252)
                            evCap = (itemEffect[10] & ITEM10_IS_VITAMIN) ? EV_ITEM_RAISE_LIMIT : MAX_PER_STAT_EVS;

                            // Check if the per-stat limit is reached
                            if (dataSigned >= evCap)
                                return TRUE;  // Prevents item use if the per-stat cap is already reached

                            if (dataSigned + evChange > evCap)
                                temp2 = evCap - dataSigned;
                            else
                                temp2 = evChange;

                            // Ensure the total EVs do not exceed the maximum allowed (510)
                            if (evCount + temp2 > maxAllowedEVs)
                                temp2 = maxAllowedEVs - evCount;

                            // Prevent item use if no EVs can be increased
                            if (temp2 == 0)
                                return TRUE;

                            // Apply the EV increase
                            dataSigned += temp2;
                        }
                        else if (evChange < 0) // Decreasing EV (HP or Atk)
                        {
                            if (dataSigned == 0)
                            {
                                // No EVs to lose, but make sure friendship updates anyway
                                friendshipOnly = TRUE;
                                itemEffectParam++;
                                break;
                            }
                            dataSigned += evChange;
                            if (I_BERRY_EV_JUMP == GEN_4 && dataSigned > 100)
                                dataSigned = 100;
                            if (dataSigned < 0)
                                dataSigned = 0;
                        }
                        else // Reset EV (HP or Atk)
                        {
                            if (dataSigned == 0)
                                break;

                            dataSigned = 0;
                        }

                        // Update EVs and stats
                        SetMonData(mon, sGetMonDataEVConstants[temp1], &dataSigned);
                        CalculateMonStats(mon);
                        itemEffectParam++;
                        retVal = FALSE;
                        break;

                    case 2: // ITEM4_HEAL_HP
                    {
                        u32 currentHP = GetMonData(mon, MON_DATA_HP);
                        u32 maxHP = GetMonData(mon, MON_DATA_MAX_HP);
                        if (isLevelUpItem && !didLevelUp && (effectFlags & (ITEM4_REVIVE >> 2)))
                        {
                            itemEffectParam++;
                            break;
                        }
                        // Check use validity.
                        if ((effectFlags & (ITEM4_REVIVE >> 2) && currentHP != 0)
                              || (!(effectFlags & (ITEM4_REVIVE >> 2)) && currentHP == 0))
                        {
                            itemEffectParam++;
                            break;
                        }

                        // Get amount of HP to restore
                        dataUnsigned = itemEffect[itemEffectParam++];
                        switch (dataUnsigned)
                        {
                        case ITEM6_HEAL_HP_FULL:
                            dataUnsigned = maxHP - currentHP;
                            break;
                        case ITEM6_HEAL_HP_HALF:
                            dataUnsigned = maxHP / 2;
                            if (dataUnsigned == 0)
                                dataUnsigned = 1;
                            break;
                        case ITEM6_HEAL_HP_LVL_UP:
                            dataUnsigned = gBattleScripting.levelUpHP;
                            break;
                        case ITEM6_HEAL_HP_QUARTER:
                            dataUnsigned = maxHP / 4;
                            if (dataUnsigned == 0)
                                dataUnsigned = 1;
                            break;
                        }

                        // Only restore HP if not at max health
                        if (maxHP != currentHP)
                        {
                            // Restore HP
                            dataUnsigned = currentHP + dataUnsigned;
                            if (dataUnsigned > maxHP)
                                dataUnsigned = maxHP;
                            SetMonData(mon, MON_DATA_HP, &dataUnsigned);
                            retVal = FALSE;
                        }
                        effectFlags &= ~(ITEM4_REVIVE >> 2);
                        break;
                    }
                    case 3: // ITEM4_HEAL_PP
                        if (!(effectFlags & (ITEM4_HEAL_PP_ONE >> 3)))
                        {
                            // Heal PP for all moves
                            for (temp2 = 0; (signed)(temp2) < (signed)(MAX_MON_MOVES); temp2++)
                            {
                                enum Move move;
                                u32 ppBonus;
                                dataUnsigned = GetMonData(mon, MON_DATA_PP1 + temp2);
                                move = GetMonData(mon, MON_DATA_MOVE1 + temp2);
                                ppBonus = GetMoveMaxPP(move);
                                if (dataUnsigned != ppBonus)
                                {
                                    dataUnsigned += itemEffect[itemEffectParam];
                                    if (dataUnsigned > ppBonus)
                                        dataUnsigned = ppBonus;
                                    SetMonData(mon, MON_DATA_PP1 + temp2, &dataUnsigned);
                                    retVal = FALSE;
                                }
                            }
                            itemEffectParam++;
                        }
                        else
                        {
                            // Heal PP for one move
                            enum Move move;
                            dataUnsigned = GetMonData(mon, MON_DATA_PP1 + moveIndex);
                            move = GetMonData(mon, MON_DATA_MOVE1 + moveIndex);
                            u32 ppBonus = GetMoveMaxPP(move);
                            if (dataUnsigned != ppBonus)
                            {
                                dataUnsigned += itemEffect[itemEffectParam++];
                                if (dataUnsigned > ppBonus)
                                    dataUnsigned = ppBonus;
                                SetMonData(mon, MON_DATA_PP1 + moveIndex, &dataUnsigned);
                                retVal = FALSE;
                            }
                        }
                        break;

                    // cases 4-6 are ITEM4_HEAL_PP_ONE, ITEM4_PP_UP, and ITEM4_REVIVE, which
                    // are already handled above by other cases or before the loop

                    case 7: // ITEM4_EVO_STONE
                        {
                            bool32 canStopEvo = TRUE;
                            enum Species targetSpecies = GetEvolutionTargetSpecies(mon, EVO_MODE_ITEM_USE, item, NULL, &canStopEvo, CHECK_EVO);

                            if (targetSpecies != SPECIES_NONE)
                            {
                                GetEvolutionTargetSpecies(mon, EVO_MODE_ITEM_USE, item, NULL, &canStopEvo, DO_EVO);
                                BeginEvolutionScene(mon, targetSpecies, canStopEvo, partyIndex);
                                return FALSE;
                            }
                        }
                        break;
                    }
                }
                temp1++;
                effectFlags >>= 1;
            }
            break;

        // Handle ITEM5 effects (Change Def/SpDef/SpAtk/Speed EVs, PP Max, and friendship changes)
        case 5:
            effectFlags = itemEffect[i];
            temp1 = 0;

            // Loop through and try each of the ITEM5 effects
            while (effectFlags != 0)
            {
                if (effectFlags & 1)
                {
                    switch (temp1)
                    {
                    case 0: // ITEM5_EV_DEF
                    case 1: // ITEM5_EV_SPEED
                    case 2: // ITEM5_EV_SPDEF
                    case 3: // ITEM5_EV_SPATK
                        evCount = GetMonEVCount(mon);
                        temp2 = itemEffect[itemEffectParam];
                        dataSigned = GetMonData(mon, sGetMonDataEVConstants[temp1 + 2]);
                        evChange = temp2;
                        if (evChange > 0) // Increasing EV
                        {
                            // Check if the total EV limit is reached
                            if (evCount >= maxAllowedEVs)
                                return TRUE;

                            // Ensure the increase does not exceed the max EV per stat (252)
                            evCap = (itemEffect[10] & ITEM10_IS_VITAMIN) ? EV_ITEM_RAISE_LIMIT : MAX_PER_STAT_EVS;

                            // Check if the per-stat limit is reached
                            if (dataSigned >= evCap)
                                return TRUE;  // Prevents item use if the per-stat cap is already reached

                            if (dataSigned + evChange > evCap)
                                temp2 = evCap - dataSigned;
                            else
                                temp2 = evChange;

                            // Ensure the total EVs do not exceed the maximum allowed (510)
                            if (evCount + temp2 > maxAllowedEVs)
                                temp2 = maxAllowedEVs - evCount;

                            // Prevent item use if no EVs can be increased
                            if (temp2 == 0)
                                return TRUE;

                            // Apply the EV increase
                            dataSigned += temp2;
                        }
                        else if (evChange < 0) // Decreasing EV
                        {
                            if (dataSigned == 0)
                            {
                                // No EVs to lose, but make sure friendship updates anyway
                                friendshipOnly = TRUE;
                                itemEffectParam++;
                                break;
                            }
                            dataSigned += evChange;
                            if (I_BERRY_EV_JUMP == GEN_4 && dataSigned > 100)
                                dataSigned = 100;
                            if (dataSigned < 0)
                                dataSigned = 0;
                        }
                        else // Reset EV
                        {
                            if (dataSigned == 0)
                                break;

                            dataSigned = 0;
                        }

                        // Update EVs and stats
                        SetMonData(mon, sGetMonDataEVConstants[temp1 + 2], &dataSigned);
                        CalculateMonStats(mon);
                        retVal = FALSE;
                        itemEffectParam++;
                        break;

                    case 4: // ITEM5_PP_MAX
                    {
                        u32 ppBonuses = GetMonData(mon, MON_DATA_PP_BONUSES);
                        dataUnsigned = (ppBonuses & gPPUpGetMask[moveIndex]) >> (moveIndex * 2);
                        temp2 = GetMoveMaxPP(GetMonData(mon, MON_DATA_MOVE1 + moveIndex));

                        // Check if 3 PP Ups have been applied already, and that the move has a total PP of at least 5 (excludes Sketch)
                        if (dataUnsigned < 3 && temp2 >= 5)
                        {
                            dataUnsigned = ppBonuses;
                            dataUnsigned &= gPPUpClearMask[moveIndex];
                            dataUnsigned += gPPUpAddValues[moveIndex] * 3; // Apply 3 PP Ups (max)

                            SetMonData(mon, MON_DATA_PP_BONUSES, &dataUnsigned);
                            dataUnsigned = GetMoveMaxPP(GetMonData(mon, MON_DATA_MOVE1 + moveIndex)) - temp2;
                            dataUnsigned = GetMonData(mon, MON_DATA_PP1 + moveIndex) + dataUnsigned;
                            SetMonData(mon, MON_DATA_PP1 + moveIndex, &dataUnsigned);
                            retVal = FALSE;
                        }
                        break;
                    }
                    case 5: // ITEM5_FRIENDSHIP_LOW
                        // Changes to friendship are given differently depending on
                        // how much friendship the Pokémon already has.
                        // In general, Pokémon with lower friendship receive more,
                        // and Pokémon with higher friendship receive less.
                        if (GetMonData(mon, MON_DATA_FRIENDSHIP) < 100)
                            UPDATE_FRIENDSHIP_FROM_ITEM();
                        itemEffectParam++;
                        break;

                    case 6: // ITEM5_FRIENDSHIP_MID
                        if (GetMonData(mon, MON_DATA_FRIENDSHIP) >= 100 && GetMonData(mon, MON_DATA_FRIENDSHIP) < 200)
                            UPDATE_FRIENDSHIP_FROM_ITEM();
                        itemEffectParam++;
                        break;

                    case 7: // ITEM5_FRIENDSHIP_HIGH
                        if (GetMonData(mon, MON_DATA_FRIENDSHIP) >= 200)
                            UPDATE_FRIENDSHIP_FROM_ITEM();
                        itemEffectParam++;
                        break;
                    }
                }
                temp1++;
                effectFlags >>= 1;
            }
            break;
        }
    }
    return retVal;
}

bool8 HealStatusConditions(struct Pokemon *mon, u32 healMask, enum BattlerId battler)
{
    u32 status = GetMonData(mon, MON_DATA_STATUS, 0);

    PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, battler, gBattlerPartyIndexes[battler]);

    if (status & healMask)
    {
        if (status & STATUS1_PARALYSIS)
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_PARALYSIS;
        else if (status & STATUS1_POISON || status & STATUS1_TOXIC_POISON)
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_POISON;
        else if (status & STATUS1_BURN)
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_BURN;
        else if (status & STATUS1_SLEEP)
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_SLEEP;
        else if (status & STATUS1_FREEZE)
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_FREEZE;
        else if (status & STATUS1_FROSTBITE)
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_FROSTBITE;
        status &= ~healMask;
        SetMonData(mon, MON_DATA_STATUS, &status);
        if (gMain.inBattle && battler != MAX_BATTLERS_COUNT)
        {
            gBattleMons[battler].status1 &= ~healMask;
            if ((healMask & STATUS1_SLEEP))
            {
                struct Pokemon *party = GetBattlerParty(battler);

                for (u32 i = 0; i < PARTY_SIZE; i++)
                {
                    if (&party[i] == mon)
                    {
                        TryDeactivateSleepClause(battler, i);
                        break;
                    }
                }
            }
        }
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

u8 GetItemEffectParamOffset(enum BattlerId battler, enum Item itemId, u8 effectByte, u8 effectBit)
{
    const u8 *itemEffect = itemId == ITEM_ENIGMA_BERRY_E_READER
        ? gEnigmaBerries[battler].itemEffect : GetItemEffect(itemId);
    u8 offset;
    int i;
    u8 j;
    u8 effectFlags;

    offset = ITEM_EFFECT_ARG_START;

    if (itemEffect == NULL)
        return 0;

    for (i = 0; i < ITEM_EFFECT_ARG_START; i++)
    {
        switch (i)
        {
        case 0:
        case 1:
        case 2:
        case 3:
            if (i == effectByte)
                return 0;
            break;
        case 4:
            effectFlags = itemEffect[4];
            if (effectFlags & ITEM4_PP_UP)
                effectFlags &= ~(ITEM4_PP_UP);
            j = 0;
            while (effectFlags)
            {
                if (effectFlags & 1)
                {
                    switch (j)
                    {
                    case 2: // ITEM4_HEAL_HP
                        if (effectFlags & (ITEM4_REVIVE >> 2))
                            effectFlags &= ~(ITEM4_REVIVE >> 2);
                        // fallthrough
                    case 0: // ITEM4_EV_HP
                        if (i == effectByte && (effectBit & 1))
                            return offset;
                        offset++;
                        break;
                    case 1: // ITEM4_EV_ATK
                        if (i == effectByte && (effectBit & 1))
                            return offset;
                        offset++;
                        break;
                    case 3: // ITEM4_HEAL_PP
                        if (i == effectByte && (effectBit & 1))
                            return offset;
                        offset++;
                        break;
                    case 7: // ITEM4_EVO_STONE
                        if (i == effectByte)
                            return 0;
                        break;
                    }
                }
                j++;
                effectFlags >>= 1;
                if (i == effectByte)
                    effectBit >>= 1;
            }
            break;
        case 5:
            effectFlags = itemEffect[5];
            j = 0;
            while (effectFlags)
            {
                if (effectFlags & 1)
                {
                    switch (j)
                    {
                    case 0: // ITEM5_EV_DEF
                    case 1: // ITEM5_EV_SPEED
                    case 2: // ITEM5_EV_SPDEF
                    case 3: // ITEM5_EV_SPATK
                    case 4: // ITEM5_PP_MAX
                    case 5: // ITEM5_FRIENDSHIP_LOW
                    case 6: // ITEM5_FRIENDSHIP_MID
                        if (i == effectByte && (effectBit & 1))
                            return offset;
                        offset++;
                        break;
                    case 7: // ITEM5_FRIENDSHIP_HIGH
                        if (i == effectByte)
                            return 0;
                        break;
                    }
                }
                j++;
                effectFlags >>= 1;
                if (i == effectByte)
                    effectBit >>= 1;
            }
            break;
        }
    }

    return offset;
}

u8 GetNature(struct Pokemon *mon)
{
    return GetMonData(mon, MON_DATA_PERSONALITY, 0) % NUM_NATURES;
}

u8 GetNatureFromPersonality(u32 personality)
{
    return personality % NUM_NATURES;
}

enum Species GetGMaxTargetSpecies(enum Species species)
{
    const struct FormChange *formChanges = GetSpeciesFormChanges(species);
    u32 i;
    for (i = 0; formChanges != NULL && formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        if (formChanges[i].method == FORM_CHANGE_BATTLE_GIGANTAMAX)
            return formChanges[i].targetSpecies;
    }
    return species;
}

bool32 DoesMonMeetAdditionalConditions(struct Pokemon *mon, const struct EvolutionParam *params, struct Pokemon *tradePartner, u32 partyId, bool32 *canStopEvo, enum EvoState evoState)
{
    u32 i, j;
    enum Item heldItem = GetMonData(mon, MON_DATA_HELD_ITEM);
    u32 gender = GetMonGender(mon);
    u32 friendship = GetMonData(mon, MON_DATA_FRIENDSHIP, 0);
    u32 attack = GetMonData(mon, MON_DATA_ATK, 0);
    u32 defense = GetMonData(mon, MON_DATA_DEF, 0);
    u32 personality = GetMonData(mon, MON_DATA_PERSONALITY, 0);
    u16 upperPersonality = personality >> 16;
    u32 weather = GetCurrentWeather();
    u32 nature = GetMonData(mon, MON_DATA_HIDDEN_NATURE);
    bool32 removeHoldItem = FALSE;
    enum Item removeBagItem = ITEM_NONE;
    u32 removeBagItemCount = 0;
    u32 evolutionTracker = GetMonData(mon, MON_DATA_EVOLUTION_TRACKER, 0);
    enum Species partnerSpecies;
    enum Item partnerHeldItem;
    enum HoldEffect partnerHoldEffect;

    if (tradePartner != NULL)
    {
        partnerSpecies = GetMonData(tradePartner, MON_DATA_SPECIES, 0);
        partnerHeldItem = GetMonData(tradePartner, MON_DATA_HELD_ITEM, 0);
        partnerHoldEffect = GetItemHoldEffect(partnerHeldItem);
    }
    else
    {
        partnerSpecies = SPECIES_NONE;
        partnerHeldItem = ITEM_NONE;
        partnerHoldEffect = HOLD_EFFECT_NONE;
    }

    // Check for additional conditions (only if the primary method passes). Skips if there's no additional conditions.
    for (i = 0; params != NULL && params[i].condition != CONDITIONS_END; i++)
    {
        enum EvolutionConditions condition = params[i].condition;
        bool32 currentCondition = FALSE;

        switch (condition)
        {
        // Gen 2
        case IF_GENDER:
            if (gender == params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_MIN_FRIENDSHIP:
            if (friendship >= params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_ATK_GT_DEF:
            if (attack > defense)
                currentCondition = TRUE;
            break;
        case IF_ATK_EQ_DEF:
            if (attack == defense)
                currentCondition = TRUE;
            break;
        case IF_ATK_LT_DEF:
            if (attack < defense)
                currentCondition = TRUE;
            break;
        case IF_TIME:
            if (GetTimeOfDay() == params[i].arg1)
                currentCondition = TRUE;

            break;
        case IF_NOT_TIME:
            if (GetTimeOfDay() != params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_HOLD_ITEM:
            if (heldItem == params[i].arg1)
            {
                currentCondition = TRUE;
                removeHoldItem = TRUE;
            }
            break;
        // Gen 3
        case IF_PID_UPPER_MODULO_10_GT:
            if ((upperPersonality % 10) > params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_PID_UPPER_MODULO_10_EQ:
            if ((upperPersonality % 10) == params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_PID_UPPER_MODULO_10_LT:
            if ((upperPersonality % 10) < params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_MIN_BEAUTY:
        {
            u32 beauty = GetMonData(mon, MON_DATA_BEAUTY, 0);
            if (beauty >= params[i].arg1)
                currentCondition = TRUE;
            break;
        }
        case IF_MIN_COOLNESS:
        {
            u32 coolness = GetMonData(mon, MON_DATA_COOL, 0);
            if (coolness >= params[i].arg1)
                currentCondition = TRUE;
            break;
        }
        case IF_MIN_SMARTNESS:
        // remember that even though it's called "Smart/Smartness" here,
        // from gen 6 and up it's known as "Clever/Cleverness."
        {
            u32 smartness = GetMonData(mon, MON_DATA_SMART, 0);
            if (smartness >= params[i].arg1)
                currentCondition = TRUE;
            break;
        }
        case IF_MIN_TOUGHNESS:
        {
            u32 toughness = GetMonData(mon, MON_DATA_TOUGH, 0);
            if (toughness >= params[i].arg1)
                currentCondition = TRUE;
            break;
        }
        case IF_MIN_CUTENESS:
        {
            u32 cuteness = GetMonData(mon, MON_DATA_CUTE, 0);
            if (cuteness >= params[i].arg1)
                currentCondition = TRUE;
            break;
        }
        // Gen 4
        case IF_SPECIES_IN_PARTY:
            for (j = 0; j < PARTY_SIZE; j++)
            {
                if (GetMonData(&gParties[B_TRAINER_PLAYER][j], MON_DATA_SPECIES) == params[i].arg1)
                {
                    currentCondition = TRUE;
                    break;
                }
            }
            break;
        case IF_IN_MAP:
            if (params[i].arg1 == ((gSaveBlock1Ptr->location.mapGroup) << 8 | gSaveBlock1Ptr->location.mapNum))
                currentCondition = TRUE;
            break;
        case IF_IN_MAPSEC:
            if (gMapHeader.regionMapSectionId == params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_KNOWS_MOVE:
            if (MonKnowsMove(mon, params[i].arg1))
                currentCondition = TRUE;
            break;
        // Gen 5
        case IF_TRADE_PARTNER_SPECIES:
            if (params[i].arg1 == partnerSpecies && partnerHoldEffect != HOLD_EFFECT_PREVENT_EVOLVE)
                currentCondition = TRUE;
            break;
        // Gen 6
        case IF_TYPE_IN_PARTY:
            for (j = 0; j < PARTY_SIZE; j++)
            {
                enum Species currSpecies = GetMonData(&gParties[B_TRAINER_PLAYER][j], MON_DATA_SPECIES);
                if (GetSpeciesType(currSpecies, 0) == params[i].arg1
                 || GetSpeciesType(currSpecies, 1) == params[i].arg1)
                {
                    currentCondition = TRUE;
                    break;
                }
            }
            break;
        case IF_WEATHER:
            if (params[i].arg1 == WEATHER_RAIN)
            {
                if (weather == WEATHER_RAIN || weather == WEATHER_RAIN_THUNDERSTORM || weather == WEATHER_DOWNPOUR)
                    currentCondition = TRUE;
            }
            else if (params[i].arg1 == WEATHER_FOG)
            {
                if (weather == WEATHER_FOG_DIAGONAL || weather == WEATHER_FOG_HORIZONTAL)
                    currentCondition = TRUE;
            }
            else if (weather == params[i].arg1)
            {
                currentCondition = TRUE;
            }
            break;
        case IF_KNOWS_MOVE_TYPE:
            for (j = 0; j < MAX_MON_MOVES; j++)
            {
                if (GetMoveType(GetMonData(mon, MON_DATA_MOVE1 + j)) == params[i].arg1)
                {
                    currentCondition = TRUE;
                    break;
                }
            }
            break;
        // Gen 8
        case IF_NATURE:
            if (nature == params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_AMPED_NATURE:
            switch (nature)
            {
            case NATURE_HARDY:
            case NATURE_BRAVE:
            case NATURE_ADAMANT:
            case NATURE_NAUGHTY:
            case NATURE_DOCILE:
            case NATURE_IMPISH:
            case NATURE_LAX:
            case NATURE_HASTY:
            case NATURE_JOLLY:
            case NATURE_NAIVE:
            case NATURE_RASH:
            case NATURE_SASSY:
            case NATURE_QUIRKY:
                currentCondition = TRUE;
                break;
            }
            break;
        case IF_LOW_KEY_NATURE:
            switch (nature)
            {
            case NATURE_LONELY:
            case NATURE_BOLD:
            case NATURE_RELAXED:
            case NATURE_TIMID:
            case NATURE_SERIOUS:
            case NATURE_MODEST:
            case NATURE_MILD:
            case NATURE_QUIET:
            case NATURE_BASHFUL:
            case NATURE_CALM:
            case NATURE_GENTLE:
            case NATURE_CAREFUL:
                currentCondition = TRUE;
                break;
            }
            break;
        case IF_RECOIL_DAMAGE_GE:
            if (evolutionTracker >= params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_CURRENT_DAMAGE_GE:
        {
            u32 currentHp = GetMonData(mon, MON_DATA_HP);
            if (currentHp != 0 && (GetMonData(mon, MON_DATA_MAX_HP) - currentHp >= params[i].arg1))
                currentCondition = TRUE;
            break;
        }
        case IF_CRITICAL_HITS_GE:
            if (partyId != PARTY_SIZE && gPartyCriticalHits[partyId] >= params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_USED_MOVE_X_TIMES:
            if (evolutionTracker >= params[i].arg2)
                currentCondition = TRUE;
            break;
        // Gen 9
        case IF_DEFEAT_X_WITH_ITEMS:
            if (evolutionTracker >= params[i].arg3)
                currentCondition = TRUE;
            break;
        case IF_PID_MODULO_100_GT:
            if ((personality % 100) > params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_PID_MODULO_100_EQ:
            if ((personality % 100) == params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_PID_MODULO_100_LT:
            if ((personality % 100) < params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_MIN_OVERWORLD_STEPS:
            if (mon == GetFirstLiveMon() && gFollowerSteps >= params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_BAG_ITEM_COUNT:
            if (CheckBagHasItem(params[i].arg1, params[i].arg2))
            {
                currentCondition = TRUE;
                removeBagItem = params[i].arg1;
                removeBagItemCount = params[i].arg2;
            }
            break;
        case IF_REGION:
            if (GetCurrentRegion() == params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_NOT_REGION:
            if (GetCurrentRegion() != params[i].arg1)
                currentCondition = TRUE;
            break;
        case CONDITIONS_END:
            break;
        }

        if (currentCondition == FALSE)
            return FALSE;
    }

    // Commit costs only after every condition passed, and only once. An
    // evolution that spends an item can't be cancelled, or the item is lost.
    if ((removeBagItem != ITEM_NONE || removeHoldItem) && canStopEvo != NULL)
        *canStopEvo = FALSE;
    if (evoState == DO_EVO)
    {
        if (removeHoldItem)
        {
            enum Item item = ITEM_NONE;
            SetMonData(mon, MON_DATA_HELD_ITEM, &item);
        }
        if (removeBagItem != ITEM_NONE)
            RemoveBagItem(removeBagItem, removeBagItemCount);
    }
    return TRUE;
}

bool32 RaiseMonToLevelerTarget(struct Pokemon *mon)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    u32 target = GetPlayerLevelCapForSpecies(species);
    u32 experience;

    if (species == SPECIES_NONE || GetMonData(mon, MON_DATA_IS_EGG)
     || GetMonData(mon, MON_DATA_LEVEL) >= target)
        return FALSE;
    experience = gExperienceTables[gSpeciesInfo[species].growthRate][target];
    SetMonData(mon, MON_DATA_EXP, &experience);
    CalculateMonStats(mon);
    return TRUE;
}

bool32 IsMonEligibleForLeveler(struct Pokemon *mon)
{
    return GetMonData(mon, MON_DATA_SPECIES) != SPECIES_NONE
        && !GetMonData(mon, MON_DATA_IS_EGG)
        && (GetMonData(mon, MON_DATA_LEVEL) < GetPlayerLevelCapForSpecies(GetMonData(mon, MON_DATA_SPECIES))
            || GetEvolutionTargetSpecies(mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, NULL, CHECK_EVO) != SPECIES_NONE);
}

enum Species GetEvolutionTargetSpecies(struct Pokemon *mon, enum EvolutionMode mode, enum Item evolutionItem, struct Pokemon *tradePartner, bool32 *canStopEvo, enum EvoState evoState)
{
    int i;
    enum Species targetSpecies = SPECIES_NONE;
    enum Species species = GetMonData(mon, MON_DATA_SPECIES, 0);
    enum Item heldItem = GetMonData(mon, MON_DATA_HELD_ITEM, 0);
    u32 level = GetMonData(mon, MON_DATA_LEVEL, 0);
    enum HoldEffect holdEffect;
    const struct Evolution *evolutions = GetSpeciesEvolutions(species);

    if (evolutions == NULL)
        return SPECIES_NONE;

    holdEffect = GetItemHoldEffect(heldItem);

    // Prevent evolution with Everstone, unless we're just viewing the party menu with an evolution item
    if (holdEffect == HOLD_EFFECT_PREVENT_EVOLVE
        && mode != EVO_MODE_ITEM_CHECK
        && (P_KADABRA_EVERSTONE < GEN_4 || species != SPECIES_KADABRA))
        return SPECIES_NONE;

    switch (mode)
    {
    case EVO_MODE_NORMAL:
    case EVO_MODE_BATTLE_READY:
        for (i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
        {
            bool32 conditionsMet = FALSE;
            if (SanitizeSpeciesId(evolutions[i].targetSpecies) == SPECIES_NONE)
                continue;

            // Check main primary evolution method
            switch (evolutions[i].method)
            {
            case EVO_LEVEL:
                if (mode != EVO_MODE_BATTLE_READY && evolutions[i].param <= level)
                    conditionsMet = TRUE;
                break;
            case EVO_LEVEL_BATTLE_ONLY:
                if (mode == EVO_MODE_BATTLE_READY && evolutions[i].param <= level)
                    conditionsMet = TRUE;
                break;
            }

            if (conditionsMet && DoesMonMeetAdditionalConditions(mon, evolutions[i].params, NULL, PARTY_SIZE, canStopEvo, evoState))
            {
                // All checks passed, so stop checking the rest of the evolutions.
                // This is different from vanilla where the loop continues.
                // If you have overlapping evolutions, put the ones you want to happen first on top of the list.
                targetSpecies = evolutions[i].targetSpecies;
                break;
            }
        }
        break;
    case EVO_MODE_TRADE:
        for (i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
        {
            bool32 conditionsMet = FALSE;
            if (SanitizeSpeciesId(evolutions[i].targetSpecies) == SPECIES_NONE)
                continue;

            switch (evolutions[i].method)
            {
            case EVO_TRADE:
                conditionsMet = TRUE;
                break;
            }

            if (conditionsMet && DoesMonMeetAdditionalConditions(mon, evolutions[i].params, tradePartner, PARTY_SIZE, canStopEvo, evoState))
            {
                // All checks passed, so stop checking the rest of the evolutions.
                // This is different from vanilla where the loop continues.
                // If you have overlapping evolutions, put the ones you want to happen first on top of the list.
                targetSpecies = evolutions[i].targetSpecies;
                break;
            }
        }
        break;
    case EVO_MODE_ITEM_USE:
    case EVO_MODE_ITEM_CHECK:
        for (i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
        {
            bool32 conditionsMet = FALSE;
            if (SanitizeSpeciesId(evolutions[i].targetSpecies) == SPECIES_NONE)
                continue;

            switch (evolutions[i].method)
            {
            case EVO_ITEM:
                if (evolutions[i].param == evolutionItem)
                    conditionsMet = TRUE;
                break;
            }

            if (conditionsMet && DoesMonMeetAdditionalConditions(mon, evolutions[i].params, NULL, PARTY_SIZE, canStopEvo, evoState))
            {
                // All checks passed, so stop checking the rest of the evolutions.
                // This is different from vanilla where the loop continues.
                // If you have overlapping evolutions, put the ones you want to happen first on top of the list.
                targetSpecies = evolutions[i].targetSpecies;
                if (canStopEvo != NULL)
                    *canStopEvo = FALSE;
                break;
            }
        }
        break;
    // Battle evolution without leveling; party slot is being passed into the evolutionItem arg.
    case EVO_MODE_BATTLE_SPECIAL:
        for (i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
        {
            bool32 conditionsMet = FALSE;
            if (SanitizeSpeciesId(evolutions[i].targetSpecies) == SPECIES_NONE)
                continue;

            switch (evolutions[i].method)
            {
            case EVO_BATTLE_END:
                conditionsMet = TRUE;
                break;
            }

            if (conditionsMet && DoesMonMeetAdditionalConditions(mon, evolutions[i].params, NULL, evolutionItem, canStopEvo, evoState))
            {
                // All checks passed, so stop checking the rest of the evolutions.
                // This is different from vanilla where the loop continues.
                // If you have overlapping evolutions, put the ones you want to happen first on top of the list.
                targetSpecies = evolutions[i].targetSpecies;
                break;
            }
        }
        break;
    // Overworld evolution without leveling; evolution method is being passed into the evolutionItem arg.
    case EVO_MODE_OVERWORLD_SPECIAL:
        for (i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
        {
            bool32 conditionsMet = FALSE;
            if (SanitizeSpeciesId(evolutions[i].targetSpecies) == SPECIES_NONE)
                continue;

            switch (evolutions[i].method)
            {
            case EVO_SPIN:
                if (gSpecialVar_0x8000 == evolutions[i].param)
                    conditionsMet = TRUE;
                break;
            }

            if (conditionsMet && DoesMonMeetAdditionalConditions(mon, evolutions[i].params, NULL, PARTY_SIZE, canStopEvo, evoState))
            {
                // All checks passed, so stop checking the rest of the evolutions.
                // This is different from vanilla where the loop continues.
                // If you have overlapping evolutions, put the ones you want to happen first on top of the list.
                targetSpecies = evolutions[i].targetSpecies;
                break;
            }
        }
        break;
    case EVO_MODE_SCRIPT_TRIGGER:
        for (i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
        {
            if (SanitizeSpeciesId(evolutions[i].targetSpecies) == SPECIES_NONE)
                continue;
            if (evolutions[i].method != EVO_SCRIPT_TRIGGER)
                continue;
            if (evolutions[i].param != evolutionItem)
                continue;
            if (DoesMonMeetAdditionalConditions(mon, evolutions[i].params, NULL, PARTY_SIZE, canStopEvo, evoState))
            {
                // All checks passed, so stop checking the rest of the evolutions.
                // This is different from vanilla where the loop continues.
                // If you have overlapping evolutions, put the ones you want to happen first on top of the list.
                targetSpecies = evolutions[i].targetSpecies;
                break;
            }
        }
        break;
    }

    // Pikachu, Meowth, Eevee and Duraludon cannot evolve if they have the
    // Gigantamax Factor. We assume that is because their evolutions
    // do not have a Gigantamax Form.
    if (GetMonData(mon, MON_DATA_GIGANTAMAX_FACTOR)
     && GetGMaxTargetSpecies(species) != species
     && GetGMaxTargetSpecies(targetSpecies) == targetSpecies)
    {
        return SPECIES_NONE;
    }

    return targetSpecies;
}

enum Species NationalPokedexNumToSpecies(enum NationalDexOrder nationalNum)
{
    enum Species species;

    if (!nationalNum)
        return SPECIES_NONE;

    species = 1;

    while (species < (NUM_SPECIES) && gSpeciesInfo[species].natDexNum != nationalNum)
        species++;

    if (species == NUM_SPECIES)
        return SPECIES_NONE;

    return GET_BASE_SPECIES_ID(species);
}

u32 NationalToRegionalOrder(enum NationalDexOrder nationalNum)
{
    if (IS_FRLG)
        return NationalToKantoOrder(nationalNum);
    return NationalToHoennOrder(nationalNum);
}

enum KantoDexOrder NationalToKantoOrder(enum NationalDexOrder nationalNum)
{
    if (nationalNum == NATIONAL_DEX_NONE)
        return 0;
    for (u32 i = 0; i < ARRAY_COUNT(sKantoToNationalOrder); i++)
        if (sKantoToNationalOrder[i] == nationalNum)
            return i + 1;
    return 0;
}

enum HoennDexOrder NationalToHoennOrder(enum NationalDexOrder nationalNum)
{
    if (nationalNum == NATIONAL_DEX_NONE)
        return 0;
    for (u32 i = 0; i < ARRAY_COUNT(sHoennToNationalOrder); i++)
        if (sHoennToNationalOrder[i] == nationalNum)
            return i + 1;
    return 0;
}

enum NationalDexOrder SpeciesToNationalPokedexNum(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (!species)
        return NATIONAL_DEX_NONE;

    return gSpeciesInfo[species].natDexNum;
}

u32 SpeciesToRegionalPokedexNum(enum Species species)
{
    if (IS_FRLG)
        return SpeciesToKantoPokedexNum(species);
    return SpeciesToHoennPokedexNum(species);
}

enum KantoDexOrder SpeciesToKantoPokedexNum(enum Species species)
{
    if (!species)
        return 0;
    return NationalToKantoOrder(gSpeciesInfo[species].natDexNum);
}

enum HoennDexOrder SpeciesToHoennPokedexNum(enum Species species)
{
    if (!species)
        return 0;
    return NationalToHoennOrder(gSpeciesInfo[species].natDexNum);
}

enum NationalDexOrder RegionalToNationalOrder(u32 regionalNum)
{
    if (IS_FRLG)
        return KantoToNationalOrder(regionalNum);
    return HoennToNationalOrder(regionalNum);
}

enum NationalDexOrder KantoToNationalOrder(enum KantoDexOrder kantoNum)
{
    if (!kantoNum || kantoNum >= KANTO_DEX_COUNT)
        return 0;

    return sKantoToNationalOrder[kantoNum - 1];
}

enum NationalDexOrder HoennToNationalOrder(enum HoennDexOrder hoennNum)
{
    if (!hoennNum || hoennNum >= HOENN_DEX_COUNT)
        return 0;

    return sHoennToNationalOrder[hoennNum - 1];
}

void EvolutionRenameMon(struct Pokemon *mon, enum Species oldSpecies, enum Species newSpecies)
{
    u8 language;
    GetMonData(mon, MON_DATA_NICKNAME, gStringVar1);
    language = GetMonData(mon, MON_DATA_LANGUAGE, &language);
    if (language == GAME_LANGUAGE && !StringCompare(GetSpeciesName(oldSpecies), gStringVar1))
        SetMonData(mon, MON_DATA_NICKNAME, GetSpeciesName(newSpecies));
}

// The below two functions determine which side of a multi battle the trainer battles on
// 0 is the left (top in  party menu), 1 is right (bottom in party menu)
u8 GetPlayerFlankId(void)
{
    u8 flankId = 0;
    switch (gLinkPlayers[GetMultiplayerId()].id)
    {
    case 0:
    case 1:
        flankId = 0;
        break;
    case 2:
    case 3:
        flankId = 1;
        break;
    }
    return flankId;
}

u16 GetLinkTrainerFlankId(u8 linkPlayerId)
{
    u16 flankId = 0;
    switch (gLinkPlayers[linkPlayerId].id)
    {
    case 0:
    case 1:
        flankId = 0;
        break;
    case 2:
    case 3:
        flankId = 1;
        break;
    }
    return flankId;
}

s32 GetBattlerMultiplayerId(u16 id)
{
    s32 multiplayerId;
    for (multiplayerId = 0; multiplayerId < MAX_LINK_PLAYERS; multiplayerId++)
        if (gLinkPlayers[multiplayerId].id == id)
            break;
    return multiplayerId;
}

u8 GetTrainerEncounterMusicId(u16 trainerOpponentId)
{
    if (CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE)
        return GetTrainerEncounterMusicIdInBattlePyramid(trainerOpponentId);
    else if (InTrainerHillChallenge())
        return GetTrainerEncounterMusicIdInTrainerHill(trainerOpponentId);
    else
        return GetTrainerStructFromId(trainerOpponentId)->encounterMusic;
}

u16 ModifyStatByNature(u8 nature, u16 stat, enum Stat statIndex)
{
    // Don't modify HP, Accuracy, or Evasion by nature
    if (statIndex <= STAT_HP || statIndex > NUM_NATURE_STATS || gNaturesInfo[nature].statUp == gNaturesInfo[nature].statDown)
        return stat;
    else if (statIndex == gNaturesInfo[nature].statUp)
        return stat * 110 / 100;
    else if (statIndex == gNaturesInfo[nature].statDown)
        return stat * 90 / 100;
    else
        return stat;
}

void AdjustFriendship(struct Pokemon *mon, u8 event)
{
    enum Species species;
    enum Item heldItem;
    enum HoldEffect holdEffect;
    s8 mod;

    if (ShouldSkipFriendshipChange())
        return;

    species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG, 0);
    heldItem = GetMonData(mon, MON_DATA_HELD_ITEM, 0);
    holdEffect = GetItemHoldEffect(heldItem);

    if (species && species != SPECIES_EGG)
    {
        u8 friendshipLevel = 0;
        s32 friendship = GetMonData(mon, MON_DATA_FRIENDSHIP, 0);

        if (friendship > 99)
            friendshipLevel++;
        if (friendship > 199)
            friendshipLevel++;

        if (event == FRIENDSHIP_EVENT_WALKING)
        {
            // 50% chance every 128 steps
            if (Random() & 1)
                return;
        }
        if (event == FRIENDSHIP_EVENT_LEAGUE_BATTLE)
        {
            // Only if it's a trainer battle with league progression significance
            if (!(gBattleTypeFlags & BATTLE_TYPE_TRAINER))
                return;

            if (IsSpecialTrainer(TRAINER_BATTLE_PARAM.opponentA))
                return;

            enum TrainerClassID opponentTrainerClass = GetTrainerClassFromId(TRAINER_BATTLE_PARAM.opponentA);
            if (!(opponentTrainerClass == TRAINER_CLASS_LEADER
                || opponentTrainerClass == TRAINER_CLASS_ELITE_FOUR
                || opponentTrainerClass == TRAINER_CLASS_CHAMPION))
                return;
        }

        mod = sFriendshipEventModifiers[event][friendshipLevel];
        friendship += CalculateFriendshipBonuses(mon,mod,holdEffect);

        if (friendship < 0)
            friendship = 0;
        if (friendship > MAX_FRIENDSHIP)
            friendship = MAX_FRIENDSHIP;

        SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);
    }
}

s32 CalculateFriendshipBonuses(struct Pokemon *mon, s32 modifier, enum HoldEffect itemHoldEffect)
{
    s32 bonus = 0;

    if ((modifier > 0) && (itemHoldEffect == HOLD_EFFECT_FRIENDSHIP_UP))
        bonus += 150 * modifier / 100;
    else
        bonus += modifier;

    if (modifier == 0)
        return bonus;

    if (GetMonData(mon, MON_DATA_POKEBALL) == BALL_LUXURY)
        bonus += ITEM_FRIENDSHIP_LUXURY_BONUS;

    if (GetMonData(mon, MON_DATA_MET_LOCATION) == GetCurrentRegionMapSectionId())
        bonus += ITEM_FRIENDSHIP_MAPSEC_BONUS;

    return bonus;
}

u16 GetMonEVCount(struct Pokemon *mon)
{
    int i;
    u16 count = 0;

    for (i = 0; i < NUM_STATS; i++)
        count += GetMonData(mon, MON_DATA_HP_EV + i, 0);

    return count;
}

bool8 TryIncrementMonLevel(struct Pokemon *mon)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES, 0);
    u8 nextLevel = GetMonData(mon, MON_DATA_LEVEL, 0) + 1;
    u32 expPoints = GetMonData(mon, MON_DATA_EXP, 0);
    if (expPoints > gExperienceTables[gSpeciesInfo[species].growthRate][MAX_LEVEL])
    {
        expPoints = gExperienceTables[gSpeciesInfo[species].growthRate][MAX_LEVEL];
        SetMonData(mon, MON_DATA_EXP, &expPoints);
    }
    if (nextLevel > GetPlayerLevelCapForSpecies(species) || expPoints < gExperienceTables[gSpeciesInfo[species].growthRate][nextLevel])
    {
        return FALSE;
    }
    else
    {
        SetMonData(mon, MON_DATA_LEVEL, &nextLevel);
        return TRUE;
    }
}

bool32 CanLearnTeachableMove(enum Species species, enum Move move)
{
    const u16 *teachableLearnset = GetSpeciesTeachableLearnset(species);
    if (species == SPECIES_EGG)
        return FALSE;
    for (u32 i = 0; teachableLearnset[i] != MOVE_UNAVAILABLE; i++)
    {
        if (teachableLearnset[i] == move)
            return TRUE;
    }
    return FALSE;
}

u16 SpeciesToPokedexNum(enum Species species)
{
    if (IsNationalPokedexEnabled())
    {
        return SpeciesToNationalPokedexNum(species);
    }
    else
    {
        // Species outside the regional dex map to 0, which would print No000.
        species = SpeciesToRegionalPokedexNum(species);
        if (species != 0 && species <= REGIONAL_DEX_COUNT)
            return species;
        return 0xFFFF;
    }
}

bool32 IsSpeciesInRegionalDex(enum Species species)
{
    if (IS_FRLG)
        return IsSpeciesInKantoDex(species);
    return IsSpeciesInHoennDex(species);
}

bool32 IsSpeciesInKantoDex(enum Species species)
{
    if (SpeciesToKantoPokedexNum(species) > KANTO_DEX_COUNT)
        return FALSE;
    else
        return TRUE;
}

bool32 IsSpeciesInHoennDex(enum Species species)
{
    if (SpeciesToHoennPokedexNum(species) > HOENN_DEX_COUNT)
        return FALSE;
    else
        return TRUE;
}

u16 GetBattleBGM(void)
{
    if (gBattleTypeFlags & BATTLE_TYPE_LEGENDARY)
    {
        switch (GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES))
        {
        case SPECIES_RAYQUAZA:
            return MUS_VS_RAYQUAZA;
        case SPECIES_KYOGRE:
        case SPECIES_GROUDON:
            return MUS_VS_KYOGRE_GROUDON;
        case SPECIES_REGIROCK:
        case SPECIES_REGICE:
        case SPECIES_REGISTEEL:
        case SPECIES_REGIGIGAS:
        case SPECIES_REGIELEKI:
        case SPECIES_REGIDRAGO:
            return MUS_VS_REGI;
        default:
            return MUS_RG_VS_LEGEND;
        }
    }
    else if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
    {
        return MUS_VS_TRAINER;
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
    {
        enum TrainerClassID trainerClass;

        if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
            trainerClass = GetFrontierOpponentClass(TRAINER_BATTLE_PARAM.opponentA);
        else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL)
            trainerClass = TRAINER_CLASS_EXPERT;
        else
            trainerClass = GetTrainerClassFromId(TRAINER_BATTLE_PARAM.opponentA);

        switch (trainerClass)
        {
        case TRAINER_CLASS_AQUA_LEADER:
        case TRAINER_CLASS_MAGMA_LEADER:
            return MUS_VS_AQUA_MAGMA_LEADER;
        case TRAINER_CLASS_TEAM_AQUA:
        case TRAINER_CLASS_TEAM_MAGMA:
        case TRAINER_CLASS_AQUA_ADMIN:
        case TRAINER_CLASS_MAGMA_ADMIN:
            return MUS_VS_AQUA_MAGMA;
        case TRAINER_CLASS_LEADER:
            return MUS_VS_GYM_LEADER;
        case TRAINER_CLASS_CHAMPION:
            return MUS_VS_CHAMPION;
        case TRAINER_CLASS_RIVAL:
            if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
                return MUS_VS_RIVAL;
            if (!StringCompare(GetTrainerNameFromId(TRAINER_BATTLE_PARAM.opponentA), gText_BattleWallyName))
                return MUS_VS_TRAINER;
            return MUS_VS_RIVAL;
        case TRAINER_CLASS_ELITE_FOUR:
            return MUS_VS_ELITE_FOUR;
        case TRAINER_CLASS_CHAMPION_FRLG:
            return MUS_RG_VS_CHAMPION;
        case TRAINER_CLASS_LEADER_FRLG:
        case TRAINER_CLASS_ELITE_FOUR_FRLG:
            return MUS_RG_VS_GYM_LEADER;
        case TRAINER_CLASS_SALON_MAIDEN:
        case TRAINER_CLASS_DOME_ACE:
        case TRAINER_CLASS_PALACE_MAVEN:
        case TRAINER_CLASS_ARENA_TYCOON:
        case TRAINER_CLASS_FACTORY_HEAD:
        case TRAINER_CLASS_PIKE_QUEEN:
        case TRAINER_CLASS_PYRAMID_KING:
            return MUS_VS_FRONTIER_BRAIN;
        default:
            if (GetCurrentRegion() == REGION_KANTO)
                return MUS_RG_VS_TRAINER;
            else
                return MUS_VS_TRAINER;
        }
    }
    else
    {
        if (GetCurrentRegion() == REGION_KANTO)
            return MUS_RG_VS_WILD;
        else
            return MUS_VS_WILD;
    }
}

void PlayBattleBGM(void)
{
    ResetMapMusic();
    m4aMPlayAllStop();
    PlayBGM(GetBattleBGM());
}

void PlayMapChosenOrBattleBGM(u16 songId)
{
    ResetMapMusic();
    m4aMPlayAllStop();
    if (songId)
        PlayNewMapMusic(songId);
    else
        PlayNewMapMusic(GetBattleBGM());
}

// Identical to PlayMapChosenOrBattleBGM, but uses a task instead
// Only used by Battle Dome
#define tSongId data[0]
void CreateTask_PlayMapChosenOrBattleBGM(u16 songId)
{
    u8 taskId;

    ResetMapMusic();
    m4aMPlayAllStop();

    taskId = CreateTask(Task_PlayMapChosenOrBattleBGM, 0);
    gTasks[taskId].tSongId = songId;
}

static void Task_PlayMapChosenOrBattleBGM(u8 taskId)
{
    if (gTasks[taskId].tSongId)
        PlayNewMapMusic(gTasks[taskId].tSongId);
    else
        PlayNewMapMusic(GetBattleBGM());
    DestroyTask(taskId);
}

#undef tSongId

const u16 *GetMonFrontSpritePal(struct Pokemon *mon)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    bool32 isShiny = GetMonData(mon, MON_DATA_IS_SHINY);
    u32 personality = GetMonData(mon, MON_DATA_PERSONALITY);
    bool32 isEgg = GetMonData(mon, MON_DATA_IS_EGG);
    return GetMonSpritePalFromSpeciesAndPersonalityIsEgg(species, isShiny, personality, isEgg);
}

const u16 *GetMonSpritePalFromSpeciesAndPersonality(enum Species species, bool32 isShiny, u32 personality)
{
    return GetMonSpritePalFromSpeciesIsEgg(species, isShiny, IsPersonalityFemale(species, personality), FALSE);
}

const u16 *GetMonSpritePalFromSpeciesAndPersonalityIsEgg(enum Species species, bool32 isShiny, u32 personality, bool32 isEgg)
{
    return GetMonSpritePalFromSpeciesIsEgg(species, isShiny, IsPersonalityFemale(species, personality), isEgg);
}

const u16 *GetMonSpritePalFromSpecies(enum Species species, bool32 isShiny, bool32 isFemale)
{
    return GetMonSpritePalFromSpeciesIsEgg(species, isShiny, isFemale, FALSE);
}

const u16 *GetMonSpritePalFromSpeciesIsEgg(enum Species species, bool32 isShiny, bool32 isFemale, bool32 isEgg)
{
    species = SanitizeSpeciesId(species);

    if (isEgg)
    {
        if (gSpeciesInfo[species].eggId != EGG_ID_NONE)
            return gEggDatas[gSpeciesInfo[species].eggId].eggPalette;
        else
            return gSpeciesInfo[SPECIES_EGG].palette;
    }
    else if (isShiny)
    {
    #if P_GENDER_DIFFERENCES
        if (gSpeciesInfo[species].shinyPaletteFemale != NULL && isFemale)
            return gSpeciesInfo[species].shinyPaletteFemale;
        else
    #endif
        if (gSpeciesInfo[species].shinyPalette != NULL)
            return gSpeciesInfo[species].shinyPalette;
        else
            return gSpeciesInfo[SPECIES_NONE].shinyPalette;
    }
    else
    {
    #if P_GENDER_DIFFERENCES
        if (gSpeciesInfo[species].paletteFemale != NULL && isFemale)
            return gSpeciesInfo[species].paletteFemale;
        else
    #endif
        if (gSpeciesInfo[species].palette != NULL)
            return gSpeciesInfo[species].palette;
        else
            return gSpeciesInfo[SPECIES_NONE].palette;
    }
}

#define OR_MOVE_IS_HM(_hm) || (move == MOVE_##_hm)

bool32 IsMoveHM(enum Move move)
{
    return FALSE FOREACH_HM(OR_MOVE_IS_HM);
}

#undef OR_MOVE_IS_HM

bool32 CannotForgetMove(enum Move move)
{
    if (P_CAN_FORGET_HIDDEN_MOVE)
        return FALSE;

    return IsMoveHM(move);
}

bool8 IsMonSpriteNotFlipped(enum Species species)
{
    return gSpeciesInfo[species].noFlip;
}

s8 GetMonFlavorRelation(struct Pokemon *mon, enum Flavor flavor)
{
    u8 nature = GetNature(mon);
    return gPokeblockFlavorCompatibilityTable[nature * FLAVOR_COUNT + flavor];
}

s8 GetFlavorRelationByPersonality(u32 personality, enum Flavor flavor)
{
    u8 nature = GetNatureFromPersonality(personality);
    return gPokeblockFlavorCompatibilityTable[nature * FLAVOR_COUNT + flavor];
}

bool8 IsTradedMon(struct Pokemon *mon)
{
    u8 otName[PLAYER_NAME_LENGTH + 1];
    u32 otId;
    GetMonData(mon, MON_DATA_OT_NAME, otName);
    otId = GetMonData(mon, MON_DATA_OT_ID, 0);
    return IsOtherTrainer(otId, otName);
}

bool8 IsOtherTrainer(u32 otId, u8 *otName)
{
    if (otId == READ_OTID_FROM_SAVE)
    {
        int i;
        for (i = 0; otName[i] != EOS; i++)
            if (otName[i] != gSaveBlock2Ptr->playerName[i])
                return TRUE;
        return FALSE;
    }

    return TRUE;
}

void MonRestorePP(struct Pokemon *mon)
{
    BoxMonRestorePP(&mon->box);
}

void BoxMonRestorePP(struct BoxPokemon *boxMon)
{
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
    {
        enum Move move = GetBoxMonData(boxMon, MON_DATA_MOVE1 + i);
        u8 pp = GetMoveMaxPP(move);
        SetBoxMonData(boxMon, MON_DATA_PP1 + i, &pp);
    }
}

void SetMonPreventsSwitchingString(void)
{
    gLastUsedAbility = gBattleStruct->abilityPreventingSwitchout;

    gBattleTextBuff1[0] = B_BUFF_PLACEHOLDER_BEGIN;
    gBattleTextBuff1[1] = B_BUFF_MON_NICK_WITH_PREFIX;
    gBattleTextBuff1[2] = gBattleStruct->battlerPreventingSwitchout;
    gBattleTextBuff1[4] = B_BUFF_EOS;

    if (IsOnPlayerSide(gBattleStruct->battlerPreventingSwitchout))
        gBattleTextBuff1[3] = GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[gBattleStruct->battlerPreventingSwitchout]);
    else
        gBattleTextBuff1[3] = gBattlerPartyIndexes[gBattleStruct->battlerPreventingSwitchout];

    PREPARE_MON_NICK_WITH_PREFIX_BUFFER(gBattleTextBuff2, gBattlerInMenuId, GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[gBattlerInMenuId]))

    BattleStringExpandPlaceholders(gText_PkmnsXPreventsSwitching, gStringVar4, sizeof(gStringVar4));
}

static s32 GetWildMonTableIdInAlteringCave(enum Species species)
{
    s32 i;
    for (i = 0; i < (s32) ARRAY_COUNT(sAlteringCaveWildMonHeldItems); i++)
        if (sAlteringCaveWildMonHeldItems[i].species == species)
            return i;
    return 0;
}

static inline bool32 CanFirstMonBoostHeldItemRarity(void)
{
    enum Ability ability;
    if (GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SANITY_IS_EGG))
        return FALSE;

    ability = GetMonAbility(&gParties[B_TRAINER_PLAYER][0]);
    if (ability == ABILITY_COMPOUND_EYES)
        return TRUE;
    else if ((OW_SUPER_LUCK >= GEN_8) && ability == ABILITY_SUPER_LUCK)
        return TRUE;
    return FALSE;
}

void SetWildMonHeldItem(void)
{
    // Emerald Champions: a wild Pokemon carries nothing. Held items are found,
    // bought or earned, so catching one is never a way to acquire one.
    if (!B_EC_WILD_HELD_ITEMS)
        return;

    u16 rnd;
    enum Species species;
    u16 count = (WILD_DOUBLE_BATTLE) ? 2 : 1;
    u16 i;
    bool32 itemHeldBoost = CanFirstMonBoostHeldItemRarity();
    u16 chanceNoItem = itemHeldBoost ? 20 : 45;
    u16 chanceNotRare = itemHeldBoost ? 80 : 95;

    for (i = 0; i < count; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_HELD_ITEM) != ITEM_NONE)
            continue; // prevent overwriting previously set item

        rnd = Random() % 100;
        species = GetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_SPECIES, 0);
        if (gMapHeader.mapLayoutId == LAYOUT_ALTERING_CAVE)
        {
            s32 alteringCaveId = GetWildMonTableIdInAlteringCave(species);
            if (alteringCaveId != 0)
            {
                // In active Altering Cave, use special item list
                if (rnd < chanceNotRare)
                    continue;
                SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_HELD_ITEM, &sAlteringCaveWildMonHeldItems[alteringCaveId].item);
            }
            else
            {
                // In inactive Altering Cave, use normal items
                if (rnd < chanceNoItem)
                    continue;
                if (rnd < chanceNotRare)
                    SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_HELD_ITEM, &gSpeciesInfo[species].itemCommon);
                else
                    SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_HELD_ITEM, &gSpeciesInfo[species].itemRare);
            }
        }
        else
        {
            if (gSpeciesInfo[species].itemCommon == gSpeciesInfo[species].itemRare && gSpeciesInfo[species].itemCommon != ITEM_NONE)
            {
                // Both held items are the same, 100% chance to hold item
                SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_HELD_ITEM, &gSpeciesInfo[species].itemCommon);
            }
            else
            {
                if (rnd < chanceNoItem)
                    continue;
                if (rnd < chanceNotRare)
                    SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_HELD_ITEM, &gSpeciesInfo[species].itemCommon);
                else
                    SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_HELD_ITEM, &gSpeciesInfo[species].itemRare);
            }
        }
    }
}

bool8 IsMonShiny(struct Pokemon *mon)
{
    return GetMonData(mon, MON_DATA_IS_SHINY);
}

const u8 *GetTrainerPartnerName(void)
{
    if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
    {
        GetFrontierTrainerName(gStringVar1, gPartnerTrainerId);
        return gStringVar1;
    }
    else
    {
        u8 id = GetMultiplayerId();
        return gLinkPlayers[GetBattlerMultiplayerId(gLinkPlayers[id].id ^ 2)].name;
    }
}

#define READ_PTR_FROM_TASK(taskId, dataId)                      \
    (void *)(                                                   \
    ((u16)(gTasks[taskId].data[dataId]) |                       \
    ((u16)(gTasks[taskId].data[dataId + 1]) << 16)))

#define STORE_PTR_IN_TASK(ptr, taskId, dataId)                 \
{                                                              \
    gTasks[taskId].data[dataId] = (u32)(ptr);                  \
    gTasks[taskId].data[dataId + 1] = (u32)(ptr) >> 16;        \
}

#define sAnimId    data[2]
#define sAnimDelay data[3]

static void Task_AnimateAfterDelay(u8 taskId)
{
    if (--gTasks[taskId].sAnimDelay == 0)
    {
        LaunchAnimationTaskForFrontSprite(READ_PTR_FROM_TASK(taskId, 0), gTasks[taskId].sAnimId);
        DestroyTask(taskId);
    }
}

static void Task_PokemonSummaryAnimateAfterDelay(u8 taskId)
{
    if (--gTasks[taskId].sAnimDelay == 0)
    {
        StartMonSummaryAnimation(READ_PTR_FROM_TASK(taskId, 0), gTasks[taskId].sAnimId);
        SummaryScreen_SetAnimDelayTaskId(TASK_NONE);
        DestroyTask(taskId);
    }
}

void BattleAnimateFrontSprite(struct Sprite *sprite, enum Species species, bool8 noCry, u8 panMode)
{
    if (gHitMarker & HITMARKER_NO_ANIMATIONS && !(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK)))
        DoMonFrontSpriteAnimation(sprite, species, noCry, panMode | SKIP_FRONT_ANIM);
    else
        DoMonFrontSpriteAnimation(sprite, species, noCry, panMode);
}

void DoMonFrontSpriteAnimation(struct Sprite *sprite, enum Species species, bool8 noCry, u8 panModeAnimFlag)
{
    s8 pan;
    switch (panModeAnimFlag & (u8)~SKIP_FRONT_ANIM) // Exclude anim flag to get pan mode
    {
    case 0:
        pan = -25;
        break;
    case 1:
        pan = 25;
        break;
    default:
        pan = 0;
        break;
    }
    if (panModeAnimFlag & SKIP_FRONT_ANIM || (gBattleTypeFlags & BATTLE_TYPE_GHOST))
    {
        // No animation, only check if cry needs to be played
        if (!noCry)
            PlayCry_Normal(species, pan);
        sprite->callback = SpriteCallbackDummy;
    }
    else
    {
        if (!noCry)
        {
            PlayCry_Normal(species, pan);
            if (HasTwoFramesAnimation(species))
                StartSpriteAnim(sprite, 1);
        }
        if (gSpeciesInfo[species].frontAnimDelay != 0)
        {
            // Animation has delay, start delay task
            u8 taskId = CreateTask(Task_AnimateAfterDelay, 0);
            STORE_PTR_IN_TASK(sprite, taskId, 0);
            gTasks[taskId].sAnimId = gSpeciesInfo[species].frontAnimId;
            gTasks[taskId].sAnimDelay = gSpeciesInfo[species].frontAnimDelay;
        }
        else
        {
            // No delay, start animation
            LaunchAnimationTaskForFrontSprite(sprite, gSpeciesInfo[species].frontAnimId);
        }
        sprite->callback = SpriteCallbackDummy_2;
    }
}

void PokemonSummaryDoMonAnimation(struct Sprite *sprite, enum Species species, bool8 oneFrame)
{
    if (!oneFrame && HasTwoFramesAnimation(species))
        StartSpriteAnim(sprite, 1);
    if (gSpeciesInfo[species].frontAnimDelay != 0)
    {
        // Animation has delay, start delay task
        u8 taskId = CreateTask(Task_PokemonSummaryAnimateAfterDelay, 0);
        STORE_PTR_IN_TASK(sprite, taskId, 0);
        gTasks[taskId].sAnimId = gSpeciesInfo[species].frontAnimId;
        gTasks[taskId].sAnimDelay = gSpeciesInfo[species].frontAnimDelay;
        SummaryScreen_SetAnimDelayTaskId(taskId);
        SetSpriteCB_MonAnimDummy(sprite);
    }
    else
    {
        // No delay, start animation
        StartMonSummaryAnimation(sprite, gSpeciesInfo[species].frontAnimId);
    }
}

void StopPokemonAnimationDelayTask(void)
{
    u8 delayTaskId = FindTaskIdByFunc(Task_PokemonSummaryAnimateAfterDelay);
    if (delayTaskId != TASK_NONE)
        DestroyTask(delayTaskId);
}

void BattleAnimateBackSprite(struct Sprite *sprite, enum Species species)
{
    if (gHitMarker & HITMARKER_NO_ANIMATIONS && !(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK)))
    {
        sprite->callback = SpriteCallbackDummy;
    }
    else
    {
        LaunchAnimationTaskForBackSprite(sprite, GetSpeciesBackAnimSet(species));
        sprite->callback = SpriteCallbackDummy_2;
    }
}

// Identical to GetOpposingLinkMultiBattlerId but for the player
// "rightSide" from that team's perspective, i.e. B_POSITION_*_RIGHT
u8 GetOpposingLinkMultiBattlerId(bool8 rightSide, u8 multiplayerId)
{
    s32 i;
    s32 battler = 0;
    switch (gLinkPlayers[multiplayerId].id)
    {
    case 0:
    case 2:
        battler = rightSide ? 3 : 1;
        break;
    case 1:
    case 3:
        battler = rightSide ? 2 : 0;
        break;
    }
    for (i = 0; i < MAX_LINK_PLAYERS; i++)
    {
        if (gLinkPlayers[i].id == (s16)battler)
            break;
    }
    return i;
}

enum TrainerPicID FacilityClassToPicIndex(u16 facilityClass)
{
    return gFacilityClassToPicIndex[facilityClass];
}

enum TrainerPicID PlayerGenderToFrontTrainerPicId(enum Gender playerGender)
{
    if (playerGender != MALE)
        return FacilityClassToPicIndex(IS_FRLG ? FACILITY_CLASS_LEAF : FACILITY_CLASS_MAY);
    else
        return FacilityClassToPicIndex(IS_FRLG ? FACILITY_CLASS_RED : FACILITY_CLASS_BRENDAN);
}

void HandleSetPokedexFlag(enum NationalDexOrder nationalNum, u8 caseId, u32 personality)
{
    u8 getFlagCaseId = (caseId == FLAG_SET_SEEN) ? FLAG_GET_SEEN : FLAG_GET_CAUGHT;
    if (!GetSetPokedexFlag(nationalNum, getFlagCaseId)) // don't set if it's already set
    {
        GetSetPokedexFlag(nationalNum, caseId);
        if (NationalPokedexNumToSpecies(nationalNum) == SPECIES_UNOWN)
            gSaveBlock2Ptr->pokedex.unownPersonality = personality;
        if (NationalPokedexNumToSpecies(nationalNum) == SPECIES_SPINDA)
            gSaveBlock2Ptr->pokedex.spindaPersonality = personality;
    }
}

void HandleSetPokedexFlagFromMon(struct Pokemon *mon, u32 caseId)
{
    u32 personality = GetMonData(mon, MON_DATA_PERSONALITY);
    enum NationalDexOrder nationalNum = SpeciesToNationalPokedexNum(GetMonData(mon, MON_DATA_SPECIES));

    HandleSetPokedexFlag(nationalNum, caseId, personality);
}

bool8 HasTwoFramesAnimation(enum Species species)
{
    return P_TWO_FRAME_FRONT_SPRITES
        && gSpeciesInfo[species].frontAnimFrames != sAnims_SingleFramePlaceHolder
        && species != SPECIES_UNOWN
        && !gTestRunnerHeadless;
}

bool8 ShouldSkipFriendshipChange(void)
{
    if (gMain.inBattle && gBattleTypeFlags & (BATTLE_TYPE_FRONTIER))
        return TRUE;
    if (!gMain.inBattle && (InBattlePike() || CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE))
        return TRUE;
    return FALSE;
}

// The below functions are for the 'MonSpritesGfxManager', a method of allocating
// space for Pokémon sprites. These are only used for the summary screen Pokémon
// sprites (unless gMonSpritesGfxPtr is in use), but were set up for more general use.
// Only the 'default' mode (MON_SPR_GFX_MODE_NORMAL) is used, which is set
// up to allocate 4 sprites using the battler sprite templates (gBattlerSpriteTemplates).
// MON_SPR_GFX_MODE_BATTLE is identical but never used.
// MON_SPR_GFX_MODE_FULL_PARTY is set up to allocate 7 sprites (party + trainer?)
// using a generic 64x64 template, and is also never used.

// Between the unnecessarily large sizes below, a mistake allocating the spritePointers
// field, and the fact that ultimately only 1 of the 4 sprite positions is used, this
// system wastes a good deal of memory.

#define ALLOC_FAIL_BUFFER (1 << 0)
#define ALLOC_FAIL_STRUCT (1 << 1)
#define GFX_MANAGER_ACTIVE 0xA3 // Arbitrary value

static void InitMonSpritesGfx_Battle(struct MonSpritesGfxManager *gfx)
{
    u16 i, j;
    for (i = 0; i < gfx->numSprites; i++)
    {
        gfx->templates[i] = gBattlerSpriteTemplates[i];
        for (j = 0; j < gfx->numFrames; j++)
            gfx->frameImages[i * gfx->numFrames + j].data = &gfx->spritePointers[i][j * MON_PIC_SIZE];

        gfx->templates[i].images = &gfx->frameImages[i * gfx->numFrames];
    }
}

static void InitMonSpritesGfx_FullParty(struct MonSpritesGfxManager *gfx)
{
    u16 i, j;
    for (i = 0; i < gfx->numSprites; i++)
    {
        gfx->templates[i] = sSpriteTemplate_64x64;
        for (j = 0; j < gfx->numFrames; j++)
            gfx->frameImages[i * gfx->numSprites + j].data = &gfx->spritePointers[i][j * MON_PIC_SIZE];

        gfx->templates[i].images = &gfx->frameImages[i * gfx->numSprites];
        gfx->templates[i].anims = gAnims_MonPic;
        gfx->templates[i].paletteTag = i;
    }
}

struct MonSpritesGfxManager *CreateMonSpritesGfxManager(u8 managerId, u8 mode)
{
    u8 i;
    u8 failureFlags;
    struct MonSpritesGfxManager *gfx;

    failureFlags = 0;
    managerId %= MON_SPR_GFX_MANAGERS_COUNT;
    gfx = AllocZeroed(sizeof(*gfx));
    if (gfx == NULL)
        return NULL;

    switch (mode)
    {
    case MON_SPR_GFX_MODE_FULL_PARTY:
        gfx->numSprites = PARTY_SIZE + 1;
        gfx->numFrames = MAX_MON_PIC_FRAMES;
        gfx->dataSize = 1;
        gfx->mode = MON_SPR_GFX_MODE_FULL_PARTY;
        break;
 // case MON_SPR_GFX_MODE_BATTLE:
    case MON_SPR_GFX_MODE_NORMAL:
    default:
        gfx->numSprites = MAX_BATTLERS_COUNT;
        gfx->numFrames = MAX_MON_PIC_FRAMES;
        gfx->dataSize = 1;
        gfx->mode = MON_SPR_GFX_MODE_NORMAL;
        break;
    }

    // Set up sprite / sprite pointer buffers
    gfx->spriteBuffer = AllocZeroed(gfx->dataSize * MON_PIC_SIZE * MAX_MON_PIC_FRAMES * gfx->numSprites);
    gfx->spritePointers = AllocZeroed(gfx->numSprites * 32); // ? Only * 4 is necessary, perhaps they were thinking bits.
    if (gfx->spriteBuffer == NULL || gfx->spritePointers == NULL)
    {
        failureFlags |= ALLOC_FAIL_BUFFER;
    }
    else
    {
        for (i = 0; i < gfx->numSprites; i++)
            gfx->spritePointers[i] = gfx->spriteBuffer + (gfx->dataSize * MON_PIC_SIZE * MAX_MON_PIC_FRAMES * i);
    }

    // Set up sprite structs
    gfx->templates = AllocZeroed(sizeof(struct SpriteTemplate) * gfx->numSprites);
    gfx->frameImages = AllocZeroed(sizeof(struct SpriteFrameImage) * gfx->numSprites * gfx->numFrames);
    if (gfx->templates == NULL || gfx->frameImages == NULL)
    {
        failureFlags |= ALLOC_FAIL_STRUCT;
    }
    else
    {
        for (i = 0; i < gfx->numFrames * gfx->numSprites; i++)
            gfx->frameImages[i].size = MON_PIC_SIZE;

        switch (gfx->mode)
        {
        case MON_SPR_GFX_MODE_FULL_PARTY:
            InitMonSpritesGfx_FullParty(gfx);
            break;
        case MON_SPR_GFX_MODE_NORMAL:
        case MON_SPR_GFX_MODE_BATTLE:
        default:
            InitMonSpritesGfx_Battle(gfx);
            break;
        }
    }

    // If either of the allocations failed free their respective members
    if (failureFlags & ALLOC_FAIL_STRUCT)
    {
        TRY_FREE_AND_SET_NULL(gfx->frameImages);
        TRY_FREE_AND_SET_NULL(gfx->templates);
    }
    if (failureFlags & ALLOC_FAIL_BUFFER)
    {
        TRY_FREE_AND_SET_NULL(gfx->spritePointers);
        TRY_FREE_AND_SET_NULL(gfx->spriteBuffer);
    }

    if (failureFlags)
    {
        // Clear, something failed to allocate
        memset(gfx, 0, sizeof(*gfx));
        Free(gfx);
    }
    else
    {
        gfx->active = GFX_MANAGER_ACTIVE;
        sMonSpritesGfxManagers[managerId] = gfx;
    }

    return sMonSpritesGfxManagers[managerId];
}

void DestroyMonSpritesGfxManager(u8 managerId)
{
    struct MonSpritesGfxManager *gfx;

    managerId %= MON_SPR_GFX_MANAGERS_COUNT;
    gfx = sMonSpritesGfxManagers[managerId];
    if (gfx == NULL)
        return;

    if (gfx->active != GFX_MANAGER_ACTIVE)
    {
        memset(gfx, 0, sizeof(*gfx));
    }
    else
    {
        TRY_FREE_AND_SET_NULL(gfx->frameImages);
        TRY_FREE_AND_SET_NULL(gfx->templates);
        TRY_FREE_AND_SET_NULL(gfx->spritePointers);
        TRY_FREE_AND_SET_NULL(gfx->spriteBuffer);
        memset(gfx, 0, sizeof(*gfx));
        Free(gfx);
    }
}

u8 *MonSpritesGfxManager_GetSpritePtr(u8 managerId, u8 spriteNum)
{
    struct MonSpritesGfxManager *gfx = sMonSpritesGfxManagers[managerId % MON_SPR_GFX_MANAGERS_COUNT];
    if (gfx->active != GFX_MANAGER_ACTIVE)
    {
        return NULL;
    }
    else
    {
        if (spriteNum >= gfx->numSprites)
            spriteNum = 0;

        return gfx->spritePointers[spriteNum];
    }
}

enum Species GetFormSpeciesId(enum Species speciesId, u8 formId)
{
    if (GetSpeciesFormTable(speciesId) != NULL)
        return GetSpeciesFormTable(speciesId)[formId];
    else
        return speciesId;
}

u8 GetFormIdFromFormSpeciesId(enum Species formSpeciesId)
{
    u8 targetFormId = 0;

    if (GetSpeciesFormTable(formSpeciesId) != NULL)
    {
        for (targetFormId = 0; GetSpeciesFormTable(formSpeciesId)[targetFormId] != FORM_SPECIES_END; targetFormId++)
        {
            if (formSpeciesId == GetSpeciesFormTable(formSpeciesId)[targetFormId])
                break;
        }
    }
    return targetFormId;
}

// Returns the current species if no form change is possible.
// changedMove is only used by FORM_CHANGE_MOVE. The final moveset determines
// whether that move was learned or forgotten.
static enum Species GetFormChangeTargetSpeciesBoxMonWithMove(struct BoxPokemon *boxMon, enum FormChanges method, enum Move changedMove)
{
    enum Species species = GetBoxMonData(boxMon, MON_DATA_SPECIES, NULL);
    const struct FormChange *formChanges = GetSpeciesFormChanges(species);

    if (formChanges == NULL)
        return species;

    struct FormChangeContext ctx =
    {
        .method = method,
        .currentSpecies = species,
        .heldItem = GetBoxMonData(boxMon, MON_DATA_HELD_ITEM),
        .ability = GetAbilityBySpeciesForOwner(species, GetBoxMonData(boxMon, MON_DATA_ABILITY_NUM), IsBoxMonTrainerOwned(boxMon)),
        .partyItemUsed = gSpecialVar_ItemId,
        .multichoiceSelection = gSpecialVar_Result,
        .status = GetBoxMonData(boxMon, MON_DATA_STATUS),
        .learnedMove = changedMove,
    };

    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        ctx.moves[i] = GetBoxMonData(boxMon, MON_DATA_MOVE1 + i);

    return GetFormChangeTargetSpecies_Internal(ctx);
}

// Returns the current species if no form change is possible
enum Species GetFormChangeTargetSpecies_Internal(struct FormChangeContext ctx)
{
    u32 i;
    enum Species targetSpecies = ctx.currentSpecies;
    const struct FormChange *formChanges = GetSpeciesFormChanges(ctx.currentSpecies);

    if (formChanges == NULL)
        return ctx.currentSpecies;

    for (i = 0; formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        if (!(ctx.method == formChanges[i].method && ctx.currentSpecies != formChanges[i].targetSpecies))
            continue;

        switch (ctx.method)
        {
        case FORM_CHANGE_ITEM_HOLD:
            if ((ctx.heldItem == formChanges[i].param1 || formChanges[i].param1 == ITEM_NONE)
                && (ctx.ability == formChanges[i].param2 || formChanges[i].param2 == ABILITY_NONE))
            {
                // This is to prevent reverting to base form when giving the item to the corresponding form.
                // Eg. Giving a Zap Plate to an Electric Arceus without an item (most likely to happen when using givemon)
                bool32 currentItemForm = FALSE;
                for (u32 j = 0; formChanges[j].method != FORM_CHANGE_TERMINATOR; j++)
                {
                    if (ctx.currentSpecies == formChanges[j].targetSpecies
                        && formChanges[j].param1 == ctx.heldItem
                        && formChanges[j].param1 != ITEM_NONE)
                    {
                        currentItemForm = TRUE;
                        break;
                    }
                }
                if (!currentItemForm)
                    targetSpecies = formChanges[i].targetSpecies;
            }
            break;
        case FORM_CHANGE_ITEM_USE:
            if (ctx.partyItemUsed == formChanges[i].param1)
            {
                bool32 pass = TRUE;
                switch (formChanges[i].param2)
                {
                case DAY:
                    if (GetTimeOfDay() == TIME_NIGHT)
                        pass = FALSE;
                    break;
                case NIGHT:
                    if (GetTimeOfDay() != TIME_NIGHT)
                        pass = FALSE;
                    break;
                }

                if (formChanges[i].param3 != STATUS1_NONE && ctx.status & formChanges[i].param3)
                    pass = FALSE;

                if (pass)
                    targetSpecies = formChanges[i].targetSpecies;
            }
            break;
        case FORM_CHANGE_ITEM_USE_MULTICHOICE:
            if (ctx.partyItemUsed == formChanges[i].param1
             && ctx.multichoiceSelection == formChanges[i].param2)
            {
                targetSpecies = formChanges[i].targetSpecies;
            }
            break;
        case FORM_CHANGE_MOVE:
        {
            bool32 knowsMove = FALSE;

            if (ctx.learnedMove != formChanges[i].param1)
                break;
            for (u32 j = 0; j < MAX_MON_MOVES; j++)
            {
                if (ctx.moves[j] == formChanges[i].param1)
                {
                    knowsMove = TRUE;
                    break;
                }
            }
            if ((formChanges[i].param2 == WHEN_LEARNED && knowsMove)
             || (formChanges[i].param2 == WHEN_FORGOTTEN && !knowsMove))
                targetSpecies = formChanges[i].targetSpecies;
            break;
        }
        case FORM_CHANGE_BEGIN_BATTLE:
        case FORM_CHANGE_END_BATTLE:
            if (ctx.heldItem == formChanges[i].param1 || formChanges[i].param1 == ITEM_NONE)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_END_BATTLE_ENVIRONMENT:
            if (gBattleEnvironment == formChanges[i].param1)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_WITHDRAW:
        case FORM_CHANGE_DEPOSIT:
        case FORM_CHANGE_FAINT:
        case FORM_CHANGE_DAYS_PASSED:
        case FORM_CHANGE_BEGIN_WILD_ENCOUNTER:
            targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_STATUS:
            if (ctx.status & formChanges[i].param1)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_TIME_OF_DAY:
            switch (formChanges[i].param1)
            {
            case DAY:
                if (GetTimeOfDay() != TIME_NIGHT)
                    targetSpecies = formChanges[i].targetSpecies;
                break;
            case NIGHT:
                if (GetTimeOfDay() == TIME_NIGHT)
                    targetSpecies = formChanges[i].targetSpecies;
                break;
            }
            break;
        case FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM:
        case FORM_CHANGE_BATTLE_PRIMAL_REVERSION:
        case FORM_CHANGE_BATTLE_ULTRA_BURST:
            if (ctx.heldItem == formChanges[i].param1)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_BATTLE_MEGA_EVOLUTION_MOVE:
            if (ctx.moves[0] == formChanges[i].param1
                || ctx.moves[1] == formChanges[i].param1
                || ctx.moves[2] == formChanges[i].param1
                || ctx.moves[3] == formChanges[i].param1)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_BATTLE_SWITCH_OUT:
            if (formChanges[i].param1 == ctx.ability || formChanges[i].param1 == ABILITY_NONE)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_BATTLE_HP_PERCENT_DURING_MOVE:
            if (ctx.ability == formChanges[i].param1
                && gCurrentMove == formChanges[i].param4)
            {
                // We multiply by 100 to make sure that integer division doesn't mess with the health check.
                u32 hpCheck = ctx.hp * 100 * 100 / ctx.maxHP;
                switch (formChanges[i].param2)
                {
                case HP_HIGHER_THAN:
                    if (hpCheck > formChanges[i].param3 * 100)
                        targetSpecies = formChanges[i].targetSpecies;
                    break;
                case HP_LOWER_EQ_THAN:
                    if (hpCheck <= formChanges[i].param3 * 100)
                        targetSpecies = formChanges[i].targetSpecies;
                    break;
                }
            }
            break;
        case FORM_CHANGE_BATTLE_HP_PERCENT_TURN_END:
        case FORM_CHANGE_BATTLE_HP_PERCENT_SEND_OUT:
            if (ctx.ability == formChanges[i].param1
                && ctx.level >= formChanges[i].param4)
            {
                // We multiply by 100 to make sure that integer division doesn't mess with the health check.
                u32 hpCheck = ctx.hp * 100 * 100 / ctx.maxHP;
                switch (formChanges[i].param2)
                {
                case HP_HIGHER_THAN:
                    if (hpCheck > formChanges[i].param3 * 100)
                        targetSpecies = formChanges[i].targetSpecies;
                    break;
                case HP_LOWER_EQ_THAN:
                    if (hpCheck <= formChanges[i].param3 * 100)
                        targetSpecies = formChanges[i].targetSpecies;
                    break;
                }
            }
            break;
        case FORM_CHANGE_BATTLE_GIGANTAMAX:
            if (ctx.gmaxFactor)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_BATTLE_WEATHER:
            // Check if there is a required ability and if the battler's ability does not match it
            // or is suppressed. If so, revert to the no weather form.
            if (formChanges[i].param2
                && ctx.ability != formChanges[i].param2
                && formChanges[i].param1 == B_WEATHER_NONE)
            {
                targetSpecies = formChanges[i].targetSpecies;
            }
            // We need to revert the weather form if the field is under Air Lock, too.
            else if (!HasWeatherEffect() && formChanges[i].param1 == B_WEATHER_NONE)
            {
                targetSpecies = formChanges[i].targetSpecies;
            }
            // Otherwise, just check for a match between the weather and the form change table.
            // Added a check for whether the weather is in effect to prevent end-of-turn soft locks with Cloud Nine / Air Lock
            else if (((gBattleWeather & formChanges[i].param1) && HasWeatherEffect())
                || (gBattleWeather == B_WEATHER_NONE && formChanges[i].param1 == B_WEATHER_NONE))
            {
                targetSpecies = formChanges[i].targetSpecies;
            }
            break;
        case FORM_CHANGE_BATTLE_HIT_BY_MOVE_CATEGORY:
            if (ctx.ability == formChanges[i].param1
                && formChanges[i].param2 == GetBattleMoveCategory(gCurrentMove))
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_BATTLE_SWITCH_IN:
        case FORM_CHANGE_BATTLE_TURN_END:
        case FORM_CHANGE_BATTLE_HIT_BY_CONFUSION_SELF_DMG:
        case FORM_CHANGE_BATTLE_BOND:
            if (formChanges[i].param1 == ctx.ability)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_BATTLE_BEFORE_MOVE:
        case FORM_CHANGE_BATTLE_AFTER_MOVE:
            if (formChanges[i].param1 == gCurrentMove
                && (formChanges[i].param2 == ABILITY_NONE || formChanges[i].param2 == ctx.ability))
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_BATTLE_BEFORE_MOVE_CATEGORY:
            if (formChanges[i].param1 == GetBattleMoveCategory(gCurrentMove)
                && (formChanges[i].param2 == ABILITY_NONE || formChanges[i].param2 == ctx.ability))
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_OVERWORLD_WEATHER:
        case FORM_CHANGE_TERMINATOR:
            break;
        }
    }

    return targetSpecies;
}


void TrySetDayLimitToFormChange(struct Pokemon *mon)
{
    u32 i;
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    const struct FormChange *formChanges = GetSpeciesFormChanges(species);

    for (i = 0; formChanges != NULL && formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        if (formChanges[i].method == FORM_CHANGE_DAYS_PASSED && species != formChanges[i].targetSpecies)
        {
            SetMonData(mon, MON_DATA_DAYS_SINCE_FORM_CHANGE, &formChanges[i].param1);
            break;
        }
    }
}

bool32 DoesSpeciesHaveFormChangeMethod(enum Species species, enum FormChanges method)
{
    u32 i;
    const struct FormChange *formChanges = GetSpeciesFormChanges(species);

    for (i = 0; formChanges != NULL && formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        if (method == formChanges[i].method && species != formChanges[i].targetSpecies)
            return TRUE;
    }

    return FALSE;
}

u16 MonTryLearningNewMoveEvolution(struct Pokemon *mon, bool8 firstMove)
{
    if (!P_LEVEL_UP_MOVE_LEARNING)
        return MOVE_NONE;

    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    u8 level = GetMonData(mon, MON_DATA_LEVEL);
    const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(species);

    // Since you can learn more than one move per level,
    // the game needs to know whether you decided to
    // learn it or keep the old set to avoid asking
    // you to learn the same move over and over again.
    if (firstMove)
    {
        sLearningMoveTableID = 0;
    }
    while (learnset[sLearningMoveTableID].move != LEVEL_UP_MOVE_END)
    {
        while ((learnset[sLearningMoveTableID].level == 0 || learnset[sLearningMoveTableID].level == level)
             && !(P_EVOLUTION_LEVEL_1_LEARN >= GEN_8 && learnset[sLearningMoveTableID].level == 1))
        {
            gMoveToLearn = learnset[sLearningMoveTableID].move;
            sLearningMoveTableID++;
            return GiveMoveToMon(mon, gMoveToLearn);
        }
        sLearningMoveTableID++;
    }
    return 0;
}

void TrySpecialOverworldEvo(void)
{
    u8 i;
    bool32 canStopEvo = FALSE;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        enum Species targetSpecies = GetEvolutionTargetSpecies(&gParties[B_TRAINER_PLAYER][i], EVO_MODE_OVERWORLD_SPECIAL, 0, NULL, &canStopEvo, CHECK_EVO);

        if (targetSpecies != SPECIES_NONE && !(gTriedEvolving & (1u << i)))
        {
            GetEvolutionTargetSpecies(&gParties[B_TRAINER_PLAYER][i], EVO_MODE_OVERWORLD_SPECIAL, 0, NULL, &canStopEvo, DO_EVO);
            gTriedEvolving |= 1u << i;

            if (gMain.callback2 == TrySpecialOverworldEvo) // This fixes small graphics glitches.
                EvolutionScene(&gParties[B_TRAINER_PLAYER][i], targetSpecies, canStopEvo, i);
            else
                BeginEvolutionScene(&gParties[B_TRAINER_PLAYER][i], targetSpecies, canStopEvo, i);

            gCB2_AfterEvolution = TrySpecialOverworldEvo;
            return;
        }
    }

    gTriedEvolving = 0;
    SetMainCallback2(CB2_ReturnToField);
}

bool32 SpeciesHasGenderDifferences(enum Species species)
{
#if P_GENDER_DIFFERENCES
    if (gSpeciesInfo[species].frontPicFemale != NULL
     || gSpeciesInfo[species].backPicFemale != NULL
     || gSpeciesInfo[species].paletteFemale != NULL
     || gSpeciesInfo[species].shinyPaletteFemale != NULL
     || gSpeciesInfo[species].iconSpriteFemale != NULL)
        return TRUE;
#endif

    return FALSE;
}

static struct PartyState *GetBattlerPartyStateByPokemon(struct Pokemon *partyMon, enum BattleTrainer trainer)
{
    struct Pokemon *party = GetTrainerParty(trainer);

    if (gBattleStruct == NULL)
        return NULL;

    for (int i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &party[i];
        if (partyMon == mon)
            return &gBattleStruct->partyState[trainer][i];
    }
    return NULL;
}

static bool32 TryFormChangeInternal(struct Pokemon *mon, enum FormChanges method, enum Move changedMove, enum BattleTrainer trainer)
{
    if (GetMonData(mon, MON_DATA_SPECIES_OR_EGG, 0) == SPECIES_NONE
     || GetMonData(mon, MON_DATA_SPECIES_OR_EGG, 0) == SPECIES_EGG)
        return FALSE;

    enum Species currentSpecies = GetMonData(mon, MON_DATA_SPECIES);
    enum Species targetSpecies = GetFormChangeTargetSpeciesBoxMonWithMove(&mon->box, method, changedMove);

    struct PartyState *battlePartyState = GetBattlerPartyStateByPokemon(mon, trainer);
    // If the battle ends, and there's not a specified species to change back to,
    // use the species at the start of the battle.
    if (targetSpecies == SPECIES_NONE
        && battlePartyState != NULL && battlePartyState->changedSpecies != SPECIES_NONE
        // This is added to prevent FORM_CHANGE_END_BATTLE_ENVIRONMENT from omitting move changes
        // at the end of the battle, as it was being counting as a successful form change.
        && (method == FORM_CHANGE_END_BATTLE || method == FORM_CHANGE_FAINT))
    {
        targetSpecies = battlePartyState->changedSpecies;
    }

    assertf(targetSpecies != SPECIES_NONE, "form change target returned NONE. cur:%d, method:%d", currentSpecies, method)
    {
        return FALSE;
    }

    if (targetSpecies != currentSpecies)
    {
        TryToSetBattleFormChangeMoves(mon, method);
        SetMonData(mon, MON_DATA_SPECIES, &targetSpecies);
        TrySetDayLimitToFormChange(mon);
        CalculateMonStats(mon);
        return TRUE;
    }

    return FALSE;
}

bool32 TryFormChange(struct Pokemon *mon, enum FormChanges method, enum BattleTrainer trainer)
{
    return TryFormChangeInternal(mon, method, MOVE_NONE, trainer);
}

bool32 TryFormChangeOnMove(struct Pokemon *mon, enum Move changedMove, enum BattleTrainer trainer)
{
    if (changedMove == MOVE_NONE)
        return FALSE;
    return TryFormChangeInternal(mon, FORM_CHANGE_MOVE, changedMove, trainer);
}

static bool32 TryBoxMonFormChangeInternal(struct BoxPokemon *boxMon, enum FormChanges method, enum Move changedMove)
{
    if (GetBoxMonData(boxMon, MON_DATA_SPECIES_OR_EGG, 0) == SPECIES_NONE
     || GetBoxMonData(boxMon, MON_DATA_SPECIES_OR_EGG, 0) == SPECIES_EGG)
        return FALSE;

    enum Species currentSpecies = GetBoxMonData(boxMon, MON_DATA_SPECIES, NULL);
    enum Species targetSpecies = GetFormChangeTargetSpeciesBoxMonWithMove(boxMon, method, changedMove);

    assertf(targetSpecies != SPECIES_NONE, "form change target returned NONE. cur:%d, method:%d", currentSpecies, method)
    {
        return FALSE;
    }

    if (targetSpecies != currentSpecies)
    {
        SetBoxMonData(boxMon, MON_DATA_SPECIES, &targetSpecies);
        return TRUE;
    }
    return FALSE;
}

bool32 TryBoxMonFormChange(struct BoxPokemon *boxMon, enum FormChanges method)
{
    return TryBoxMonFormChangeInternal(boxMon, method, MOVE_NONE);
}

bool32 TryBoxMonFormChangeOnMove(struct BoxPokemon *boxMon, enum Move changedMove)
{
    if (changedMove == MOVE_NONE)
        return FALSE;
    return TryBoxMonFormChangeInternal(boxMon, FORM_CHANGE_MOVE, changedMove);
}

enum Species SanitizeSpeciesId(enum Species species)
{
    assertf(species <= NUM_SPECIES, "invalid species: %d", species)
    {
        return SPECIES_NONE;
    }

    assertf(species == SPECIES_NONE || IsSpeciesEnabled(species), "disabled species: %d", species)
    {
        return SPECIES_NONE;
    }

    return species;
}

bool32 IsSpeciesEnabled(enum Species species)
{
    // This function should not use the GetSpeciesBaseHP function, as the included sanitation will result in an infinite loop
    return gSpeciesInfo[species].baseHP > 0 || species == SPECIES_EGG;
}

void TryToSetBattleFormChangeMoves(struct Pokemon *mon, enum FormChanges method)
{
    int i, j;
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    const struct FormChange *formChanges = GetSpeciesFormChanges(species);

    if (formChanges == NULL
        || (method != FORM_CHANGE_BEGIN_BATTLE && method != FORM_CHANGE_END_BATTLE))
        return;

    for (i = 0; formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        if (formChanges[i].method == method
            && formChanges[i].param2
            && formChanges[i].param3
            && formChanges[i].targetSpecies != species)
        {
            u16 originalMove = formChanges[i].param2;
            u16 newMove = formChanges[i].param3;

            for (j = 0; j < MAX_MON_MOVES; j++)
            {
                u16 currMove = GetMonData(mon, MON_DATA_MOVE1 + j);
                if (currMove == originalMove)
                    SetMonMoveSlot_KeepPP(mon, newMove, j);
            }
            break;
        }
    }
}

u32 GetMonFriendshipScore(struct Pokemon *pokemon)
{
    u32 friendshipScore = GetMonData(pokemon, MON_DATA_FRIENDSHIP);

    if (friendshipScore == MAX_FRIENDSHIP)
        return FRIENDSHIP_MAX;
    if (friendshipScore >= 200)
        return FRIENDSHIP_200_TO_254;
    if (friendshipScore >= 150)
        return FRIENDSHIP_150_TO_199;
    if (friendshipScore >= 100)
        return FRIENDSHIP_100_TO_149;
    if (friendshipScore >= 50)
        return FRIENDSHIP_50_TO_99;
    if (friendshipScore >= 1)
        return FRIENDSHIP_1_TO_49;

    return FRIENDSHIP_NONE;
}

u32 GetMonAffectionHearts(struct Pokemon *pokemon)
{
    u32 friendship = GetMonData(pokemon, MON_DATA_FRIENDSHIP);

    if (friendship == MAX_FRIENDSHIP)
        return AFFECTION_FIVE_HEARTS;
    if (friendship >= 220)
        return AFFECTION_FOUR_HEARTS;
    if (friendship >= 180)
        return AFFECTION_THREE_HEARTS;
    if (friendship >= 130)
        return AFFECTION_TWO_HEARTS;
    if (friendship >= 80)
        return AFFECTION_ONE_HEART;

    return AFFECTION_NO_HEARTS;
}

void UpdateMonPersonality(struct BoxPokemon *boxMon, u32 personality)
{
    struct PokemonSubstruct0 *old0, *new0;
    struct PokemonSubstruct1 *old1, *new1;
    struct PokemonSubstruct2 *old2, *new2;
    struct PokemonSubstruct3 *old3, *new3;
    struct BoxPokemon old;

    bool32 isShiny = GetBoxMonData(boxMon, MON_DATA_IS_SHINY);
    u32 hiddenNature = GetBoxMonData(boxMon, MON_DATA_HIDDEN_NATURE);

    old = *boxMon;
    old0 = &(GetSubstruct(&old, old.personality, SUBSTRUCT_TYPE_0)->type0);
    old1 = &(GetSubstruct(&old, old.personality, SUBSTRUCT_TYPE_1)->type1);
    old2 = &(GetSubstruct(&old, old.personality, SUBSTRUCT_TYPE_2)->type2);
    old3 = &(GetSubstruct(&old, old.personality, SUBSTRUCT_TYPE_3)->type3);

    new0 = &(GetSubstruct(boxMon, personality, SUBSTRUCT_TYPE_0)->type0);
    new1 = &(GetSubstruct(boxMon, personality, SUBSTRUCT_TYPE_1)->type1);
    new2 = &(GetSubstruct(boxMon, personality, SUBSTRUCT_TYPE_2)->type2);
    new3 = &(GetSubstruct(boxMon, personality, SUBSTRUCT_TYPE_3)->type3);

    DecryptBoxMon(&old);
    boxMon->personality = personality;
    *new0 = *old0;
    *new1 = *old1;
    *new2 = *old2;
    *new3 = *old3;
    boxMon->checksum = CalculateBoxMonChecksumReencrypt(boxMon);

    SetBoxMonData(boxMon, MON_DATA_IS_SHINY, &isShiny);
    SetBoxMonData(boxMon, MON_DATA_HIDDEN_NATURE, &hiddenNature);
}

void HealPokemon(struct Pokemon *mon)
{
    u32 data;

    data = GetMonData(mon, MON_DATA_MAX_HP);
    SetMonData(mon, MON_DATA_HP, &data);

    data = STATUS1_NONE;
    SetMonData(mon, MON_DATA_STATUS, &data);

    MonRestorePP(mon);
}

void HealBoxPokemon(struct BoxPokemon *boxMon)
{
    u32 data;

    data = 0;
    SetBoxMonData(boxMon, MON_DATA_HP_LOST, &data);

    data = STATUS1_NONE;
    SetBoxMonData(boxMon, MON_DATA_STATUS, &data);

    BoxMonRestorePP(boxMon);
}

enum PokemonCry GetCryIdBySpecies(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (P_CRIES_ENABLED == FALSE || gSpeciesInfo[species].cryId >= CRY_COUNT || gTestRunnerHeadless)
        return CRY_NONE;
    return gSpeciesInfo[species].cryId;
}

enum Species GetSpeciesPreEvolution(enum Species species)
{
    int i, j;

    for (i = SPECIES_BULBASAUR; i < NUM_SPECIES; i++)
    {
        if (!IsSpeciesEnabled(i))
            continue;

        const struct Evolution *evolutions = GetSpeciesEvolutions(i);
        if (evolutions == NULL)
            continue;

        for (j = 0; evolutions[j].method != EVOLUTIONS_END; j++)
        {
            if (IsSpeciesEnabled(evolutions[j].targetSpecies) && SanitizeSpeciesId(evolutions[j].targetSpecies) == species)
                return i;
        }
    }

    return SPECIES_NONE;
}

void UpdateDaysPassedSinceFormChange(u16 days)
{
    u32 i;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        enum Species currentSpecies = GetMonData(mon, MON_DATA_SPECIES);
        u8 daysSinceFormChange;

        if (currentSpecies == SPECIES_NONE)
            continue;

        daysSinceFormChange = GetMonData(mon, MON_DATA_DAYS_SINCE_FORM_CHANGE, 0);
        if (daysSinceFormChange == 0)
            continue;

        if (daysSinceFormChange > days)
            daysSinceFormChange -= days;
        else
            daysSinceFormChange = 0;

        SetMonData(mon, MON_DATA_DAYS_SINCE_FORM_CHANGE, &daysSinceFormChange);

        if (daysSinceFormChange == 0)
            TryFormChange(mon, FORM_CHANGE_DAYS_PASSED, B_TRAINER_PLAYER);
    }
}

enum Type CheckDynamicMoveType(struct Pokemon *mon, enum Move move, enum BattlerId battler, enum MonState state)
{
    enum Type moveType = GetDynamicMoveType(mon, move, battler, GetBattlerAbility(battler), GetBattlerHoldEffect(battler), state);
    if (moveType != TYPE_NONE)
        return moveType;
    return GetMoveType(move);
}

uq4_12_t GetDynamaxLevelHPMultiplier(u32 dynamaxLevel, bool32 inverseMultiplier)
{
    if (inverseMultiplier)
        return UQ_4_12(1.0/(1.5 + 0.05 * dynamaxLevel));
    return UQ_4_12(1.5 + 0.05 * dynamaxLevel);
}

bool32 IsSpeciesRegionalForm(enum Species species)
{
    return gSpeciesInfo[species].isAlolanForm
        || gSpeciesInfo[species].isGalarianForm
        || gSpeciesInfo[species].isHisuianForm
        || gSpeciesInfo[species].isPaldeanForm;
}

bool32 IsSpeciesRegionalFormFromRegion(enum Species species, enum Region region)
{
    switch (region)
    {
    case REGION_ALOLA:  return gSpeciesInfo[species].isAlolanForm;
    case REGION_GALAR:  return gSpeciesInfo[species].isGalarianForm;
    case REGION_HISUI:  return gSpeciesInfo[species].isHisuianForm;
    case REGION_PALDEA: return gSpeciesInfo[species].isPaldeanForm;
    default:            return FALSE;
    }
}

bool32 SpeciesHasRegionalForm(enum Species species)
{
    u32 formId;
    const u16 *formTable = GetSpeciesFormTable(species);
    for (formId = 0; formTable != NULL && formTable[formId] != FORM_SPECIES_END; formId++)
    {
        if (IsSpeciesRegionalForm(formTable[formId]))
            return TRUE;
    }
    return FALSE;
}

enum Species GetRegionalFormByRegion(enum Species species, enum Region region)
{
    u32 formId = 0;
    enum Species firstFoundSpecies = 0;
    const u16 *formTable = GetSpeciesFormTable(species);

    if (formTable != NULL)
    {
        for (formId = 0; formTable[formId] != FORM_SPECIES_END; formId++)
        {
            if (firstFoundSpecies == 0)
                firstFoundSpecies = formTable[formId];

            if (IsSpeciesRegionalFormFromRegion(formTable[formId], region))
                return formTable[formId];
        }
        if (firstFoundSpecies != 0)
            return firstFoundSpecies;
    }
    return species;
}

bool32 IsSpeciesForeignRegionalForm(enum Species species, enum Region currentRegion)
{
    for (enum Region i = 0; i < REGIONS_COUNT; i++)
    {
        if (currentRegion != i && IsSpeciesRegionalFormFromRegion(species, i))
            return TRUE;
        else if (currentRegion == i && SpeciesHasRegionalForm(species) && !IsSpeciesRegionalFormFromRegion(species, i))
            return TRUE;
    }
    return FALSE;
}

struct Pokemon *GetSavedPlayerPartyMon(u32 index)
{
    return &gSaveBlock1Ptr->playerParty[index];
}

u8 *GetSavedPlayerPartyCount(void)
{
    return &gSaveBlock1Ptr->playerPartyCount;
}

void SavePlayerPartyMon(u32 index, struct Pokemon *mon)
{
    gSaveBlock1Ptr->playerParty[index] = *mon;
}

bool32 IsSpeciesOfType(enum Species species, enum Type type)
{
    if (gSpeciesInfo[species].types[0] == type
     || gSpeciesInfo[species].types[1] == type)
        return TRUE;
    return FALSE;
}

struct BoxPokemon *GetSelectedBoxMonFromPcOrParty(void)
{
    struct BoxPokemon *boxmon;
    if (gSpecialVar_0x8004 == PC_MON_CHOSEN)
        boxmon = GetBoxedMonPtr(gSpecialVar_MonBoxId, gSpecialVar_MonBoxPos);
    else
        boxmon = &(gParties[B_TRAINER_PLAYER][gSpecialVar_0x8004].box);
    return boxmon;
}

u32 GiveScriptedMonToPlayer(struct Pokemon *mon, u8 slot)
{
    u32 sentToPc;
    ClampMonToPlayerLevelCap(mon);
    if (slot < PARTY_SIZE)
    {
        if (!CanAddRestrictedMonToParty(GetMonData(mon, MON_DATA_SPECIES), slot))
            sentToPc = CopyMonToPC(mon);
        else
        {
            memcpy(&gParties[B_TRAINER_PLAYER][slot], mon, sizeof(struct Pokemon));
            sentToPc = MON_GIVEN_TO_PARTY;
        }
    }
    else
    {
        sentToPc = GiveMonToPartyOrPC(mon);
    }
    if (sentToPc != MON_CANT_GIVE)
    {
        EmeraldChampions_UnlockBattleItem(GetMonData(mon, MON_DATA_HELD_ITEM));
        HandleSetPokedexFlagFromMon(mon, FLAG_SET_SEEN);
        HandleSetPokedexFlagFromMon(mon, FLAG_SET_CAUGHT);
    }
    CalculatePlayerPartyCount();
    return sentToPc;
}

void ChangePokemonNicknameWithCallback(void (*callback)(void))
{
    struct BoxPokemon *boxMon = GetSelectedBoxMonFromPcOrParty();
    GetBoxMonData(boxMon, MON_DATA_NICKNAME, gStringVar3);
    GetBoxMonData(boxMon, MON_DATA_NICKNAME, gStringVar2);
    DoNamingScreen(NAMING_SCREEN_NICKNAME, gStringVar2, GetBoxMonData(boxMon, MON_DATA_SPECIES), GetBoxMonGender(boxMon), GetBoxMonData(boxMon, MON_DATA_PERSONALITY), callback);
}

bool32 HasShedinjaHPHandling(enum Species species)
{
    if (species == SPECIES_SHEDINJA)
        return TRUE;
    if (P_BASE_HP_1_SHEDINJA_HANDLING && GetSpeciesBaseHP(species) == 1)
        return TRUE;
    return FALSE;
}

static u32 ResolveAbility(enum Species species, u32 abilityNum)
{
    // Scripted gifts may name an official slot or one of the species' Inclement slots.
    assertf((abilityNum < NUM_ABILITY_SLOTS && GetAbilityBySpecies(species, abilityNum) != ABILITY_NONE)
         || GetInclementAddedAbility(species, abilityNum) != ABILITY_NONE,
            "invalid ability num %d for species %d", abilityNum, species)
    {
        return 0;
    }
    return abilityNum;
}

static enum PokeBall ResolveBall(u32 ballTemplate)
{
    if (ballTemplate < POKEBALL_COUNT)
        return ballTemplate;
    if (ballTemplate == BALL_RANDOM)
        return GetRandomBall();

    errorf("Unknown ball value %d when creating pokemon", ballTemplate);
    return BALL_STRANGE;
}

void ResolveEVs(const u16 *evsTemplate, u8 *evs, bool32 ignoreTotalEvCheck)
{
    u32 evTotal = 0;
    for (u32 i = 0; i < NUM_STATS; i++)
    {
        if (evsTemplate[i] <= MAX_PER_STAT_EVS)
        {
            evs[i] = evsTemplate[i];
        }
        else
        {
            errorf("invalid ev value of %d above maximum of %d", evs[i], MAX_PER_STAT_EVS);
            evs[i] = 0;
        }
        evTotal += evs[i];
    }
    if (!OW_CHECK_FOR_TOTAL_EVS || ignoreTotalEvCheck)
        return;
    assertf(evTotal <= MAX_TOTAL_EVS, "invalid total ev value of %d above maximum of %d", evTotal, MAX_TOTAL_EVS)
    {
        for (u32 i = 0; i < NUM_STATS; i++)
        {
            evs[i] = 0;
        }
    }
}

static enum Species ResolveSpecies(u32 speciesTemplate)
{
    assertf (IsSpeciesEnabled(speciesTemplate), "unknown species %d when creating pokemon", speciesTemplate)
    {
        return SPECIES_PORYGON;
    }
    return speciesTemplate;
}

static u32 ResolveLevel(u32 levelTemplate)
{
    assertf (MIN_LEVEL <= levelTemplate &&levelTemplate <= MAX_LEVEL, "using invalid level %d when creating pokemon", levelTemplate)
    {
        return MIN_LEVEL;
    }
    return levelTemplate;
}

static u32 ResolvePersonality(enum Species species, u32 genderTemplate, u32 natureTemplate, enum GeneratedMonOrigin originTemplate)
{
    u32 gender;
    u32 nature;
    assertf(originTemplate != UNDEFINED_MON_ORIGIN, "origin was not explicitly set when creating pokemon")
    {
        // if the origin was not explictly set, never apply Synchronize or Cute Charm effects
        if (genderTemplate == MON_GENDER_MAY_CUTE_CHARM)
            genderTemplate = MON_GENDER_RANDOM;
        if (natureTemplate == NATURE_MAY_SYNCHRONIZE)
            natureTemplate = NATURE_RANDOM;
    }
    if (genderTemplate == MON_GENDER_MAY_CUTE_CHARM)
        gender = GetSynchronizedGender(originTemplate, genderTemplate);
    else
        gender = genderTemplate;
    if (natureTemplate == NATURE_MAY_SYNCHRONIZE)
        nature = GetSynchronizedNature(originTemplate, natureTemplate);
    else
        nature = natureTemplate;
    return GetMonPersonality(species, gender, nature, RANDOM_UNOWN_LETTER);
}

static bool32 ResolveShinyness(u32 isShinyTemplate)
{
    assertf(isShinyTemplate == TRUE || isShinyTemplate == FALSE, "using non boolean isShiny value %d when creating pokemon", isShinyTemplate)
    {
        return FALSE;
    }
    return isShinyTemplate;
}

static bool32 ResolveDynamaxLevel(u32 dynamaxLevelTemplate)
{
    assertf(dynamaxLevelTemplate <= MAX_DYNAMAX_LEVEL, "invalid dynamax level value %d when creating pokemon", dynamaxLevelTemplate)
    {
        return FALSE;
    }
    return dynamaxLevelTemplate;
}

static bool32 ResolveGmaxFactor(u32 gmaxFactorTemplate)
{
    assertf(gmaxFactorTemplate == TRUE || gmaxFactorTemplate == FALSE, "using non boolean gmaxFactor value %d when creating pokemon", gmaxFactorTemplate)
    {
        return FALSE;
    }
    return gmaxFactorTemplate;
}

static bool32 ResolveIsEgg(u32 isEggTemplate)
{
    assertf(isEggTemplate == TRUE || isEggTemplate == FALSE, "using non boolean isEgg value %d when creating pokemon", isEggTemplate)
    {
        return FALSE;
    }
    return isEggTemplate;
}

static enum Item ResolveHeldItem(u32 heldItemTemplate)
{
    assertf(heldItemTemplate < ITEMS_COUNT,"using invalid item %d when creating pokemon", heldItemTemplate)
    {
        return ITEM_NONE;
    }
    assertf(!ItemIsMail(heldItemTemplate) && !GetItemImportance(heldItemTemplate), "trying to give item %d that can't be held to newly created pokemon", heldItemTemplate)
    {
        return ITEM_NONE;
    }
    return heldItemTemplate;
}

void CreateMonFromTemplate(struct Pokemon *mon, const struct PokemonTemplate *monTemplate)
{
    enum Species species = ResolveSpecies(monTemplate->species);
    u8 level = ResolveLevel(monTemplate->level);
    u32 personality = ResolvePersonality(species, monTemplate->gender, monTemplate->nature, monTemplate->origin);
    CreateMon(mon, species, level, personality, OTID_STRUCT_PLAYER_ID);

    enum Item heldItem = ResolveHeldItem(monTemplate->heldItem);
    SetMonData(mon, MON_DATA_HELD_ITEM, &heldItem);

    if (monTemplate->doNotUseDefaultBall)
    {
        enum PokeBall ball = ResolveBall(monTemplate->ball);
        SetMonData(mon, MON_DATA_POKEBALL, &ball);
    }

    if (monTemplate->doNotUseDefaultShinyness && monTemplate->isShiny != SHINY_MODE_RANDOM)
    {
        bool32 isShiny = ResolveShinyness(monTemplate->isShiny);
        SetMonData(mon, MON_DATA_IS_SHINY, &isShiny);
    }

    if (monTemplate->doNotUseDefaultAbility)
    {
        u8 abilityNum = ResolveAbility(species, monTemplate->abilityNum);
        SetMonData(mon, MON_DATA_ABILITY_NUM, &abilityNum);
    }

    u8 evs[NUM_STATS];
    ResolveEVs(monTemplate->evs, evs, monTemplate->ignoreTotalEvCheck);
    for (u32 i = 0; i < NUM_STATS; i++)
    {
        SetMonData(mon, MON_DATA_HP_EV + i, &evs[i]);
    }

    // Unspecified IVs keep the universal constructed-Pokémon value; explicit ones are honored.
    for (u32 i = 0; i < NUM_STATS; i++)
    {
        if (monTemplate->ivs[i] <= MAX_PER_STAT_IVS)
        {
            u8 iv = monTemplate->ivs[i];
            SetMonData(mon, MON_DATA_HP_IV + i, &iv);
        }
    }

    enum Move moves[MAX_MON_MOVES];
    ResolveMoves(species, level, monTemplate->moves, moves);
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
    {
        SetMonMoveSlot(mon, moves[i], i);
    }

    u32 dmaxLevel = ResolveDynamaxLevel(monTemplate->dmaxLevel);
    SetMonData(mon, MON_DATA_DYNAMAX_LEVEL, &dmaxLevel);

    bool32 gmaxFactor = ResolveGmaxFactor(monTemplate->gmaxFactor);
    SetMonData(mon, MON_DATA_GIGANTAMAX_FACTOR, &gmaxFactor);

    bool32 isEgg = ResolveIsEgg(monTemplate->isEgg);
    SetMonData(mon, MON_DATA_IS_EGG, &isEgg);

    CalculateMonStats(mon);
    TryFormChange(mon, FORM_CHANGE_ITEM_HOLD, B_TRAINER_PLAYER);
}
