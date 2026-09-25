#include "global.h"
#include "battle.h"
#include "battle_gimmick.h"
#include "battle_setup.h"
#include "battle_ai_util.h"
#include "battle_util.h"
#include "emerald_champions_battle_plan.h"
#include "test/battle.h"
#include "data.h"
#include "difficulty.h"
#include "event_data.h"
#include "malloc.h"
#include "constants/opponents.h"
#include "emerald_champions_battle_sets.h"

// Locks, switch-ins, move and target choice replayed from the build-k
// benchmark boards: the campaign's own compiled teams, production stat
// generation, and each board's in-battle state restored before the AI reads
// it.

extern void (*gTestAiTurnSetupHook)(void);

// A member's state on the board being replayed. noItem: the member's item was
// spent or removed before this board.
struct LockInjury { enum Species species; u16 hp; u32 status; bool32 noItem; };
EWRAM_DATA static struct LockInjury sLockInjuries[6] = {0};
// A milestone past the badges that sets the fight's level cap. Consumed by the
// next authored build.
EWRAM_DATA static u16 sLockMilestone = 0;

static void LockBuildAuthoredParty(u16 trainerId, u32 badges, struct Pokemon *party, u32 flags)
{
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][trainerId];
    u32 savedFlags = gBattleTypeFlags;
    enum DifficultyLevel savedDifficulty = GetCurrentDifficultyLevel();
    bool8 savedBadges[8];
    for (u32 i = 0; i < ARRAY_COUNT(savedBadges); i++)
    {
        savedBadges[i] = FlagGet(FLAG_BADGE01_GET + i);
        if (i < badges)
            FlagSet(FLAG_BADGE01_GET + i);
        else
            FlagClear(FLAG_BADGE01_GET + i);
    }
    bool8 savedMilestone = sLockMilestone ? FlagGet(sLockMilestone) : FALSE;
    if (sLockMilestone)
        FlagSet(sLockMilestone);
    SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
    gBattleTypeFlags = flags;
    // Past the Hall of Fame a trainer's levels run over 100, which the game
    // keeps only for a battle party. Build in the spare opponent party, as
    // the benchmark's own battle did, and copy the members out.
    struct Pokemon *spare = gParties[B_TRAINER_OPPONENT_B];
    struct Pokemon *saved = AllocZeroed(sizeof(struct Pokemon) * PARTY_SIZE);
    memcpy(saved, spare, sizeof(struct Pokemon) * PARTY_SIZE);
    CreateNPCTrainerPartyFromTrainer(spare, trainer);
    memcpy(party, spare, sizeof(struct Pokemon) * PARTY_SIZE);
    memcpy(spare, saved, sizeof(struct Pokemon) * PARTY_SIZE);
    Free(saved);
    gBattleTypeFlags = savedFlags;
    if (sLockMilestone && !savedMilestone)
        FlagClear(sLockMilestone);
    SetCurrentDifficultyLevel(savedDifficulty);
    for (u32 i = 0; i < ARRAY_COUNT(savedBadges); i++)
    {
        if (savedBadges[i])
            FlagSet(FLAG_BADGE01_GET + i);
        else
            FlagClear(FLAG_BADGE01_GET + i);
    }
}

// A doubles opponent built from its authored team, in authored order, with
// the board's injuries applied.
static void LockAuthoredOpponent(u16 trainerId, u32 badges)
{
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][trainerId];
    struct Pokemon *party = AllocZeroed(sizeof(struct Pokemon) * PARTY_SIZE);
    LockBuildAuthoredParty(trainerId, badges, party, BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE);
    if (IsAITest())
        AI_FLAGS(trainer->aiFlags | AI_FLAG_DOUBLE_BATTLE);
    gBattleTestRunnerState->data.recordedBattle.opponentA = trainerId;
    for (u32 i = 0; i < trainer->partySize; i++)
    {
        struct Pokemon *mon = &party[i];
        OPPONENT(GetMonData(mon, MON_DATA_SPECIES))
        {
            *gBattleTestRunnerState->data.currentMon = *mon;
            Nature(GetNature(mon));
            Ability(GetMonAbility(mon));
            Speed(GetMonData(mon, MON_DATA_SPEED));
            bool32 fixedSpeed = FALSE;
            SetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_HYPER_TRAINED_SPEED, &fixedSpeed);
            Moves(GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                  GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
            for (u32 j = 0; j < ARRAY_COUNT(sLockInjuries); j++)
                if (sLockInjuries[j].species != SPECIES_NONE
                 && GetMonData(mon, MON_DATA_SPECIES) == sLockInjuries[j].species)
                {
                    HP(sLockInjuries[j].hp);
                    if (sLockInjuries[j].status)
                        Status1(sLockInjuries[j].status);
                    if (sLockInjuries[j].noItem)
                        Item(ITEM_NONE);
                }
        }
    }
    memset(sLockInjuries, 0, sizeof(sLockInjuries));
    sLockMilestone = 0;
    Free(party);
}

// A benchmark player member: the prepared set at the fight's cap, perfect IVs
// and the set's EVs, at the HP it had on the board (0 keeps it full).
static void LockPlayer(enum Species species, u32 level, u16 hp, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, level, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(set->moves[0], set->moves[1], set->moves[2], set->moves[3]);
        if (hp)
            HP(hp);
    }
}

#define SET(m1, m2, m3, m4, _nature, _ability, _item, hp, atk, def, spa, spd, spe) \
    (&(const struct EmeraldChampionsBattleSet){ .moves = {m1, m2, m3, m4}, .nature = _nature, \
        .ability = _ability, .item = _item, .evs = {hp, atk, def, spa, spd, spe} })

// The engine sends out the first living members, in party order; tell the
// runner which party slots those leads came from.
static void LockLeads(u32 left, u32 right)
{
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = left;
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = right;
}

// Both sides a turn into the battle: Fake Out spent, nobody on its first turn.
static void LockNotFirstTurn(void)
{
    for (enum BattlerId battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
        gBattleStruct->battlerState[battler].isFirstTurn = 0;
}

// A move this battler used before the board: on its record, and as its last
// move when it was the latest.
static void LockUsed(enum BattlerId battler, enum Move move, bool32 last)
{
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
        if (gBattleMons[battler].moves[slot] == move)
            gBattleHistory->usedMoves[battler][slot] = move;
    if (last)
        gLastMoves[battler] = gLastResultingMoves[battler] = gLastLandedMoves[battler] = move;
}

// k4/cyn1 turn 1 as it stood: Togekiss shielded last turn and Milotic
// flinched, Groudon has shown Heat Crash and Incineroar Fake Out.
static void CynthiaTogekissBoard(void)
{
    LockNotFirstTurn();
    gBattleMons[B_BATTLER_3].volatiles.consecutiveMoveUses = 1;
    LockUsed(B_BATTLER_3, MOVE_PROTECT, TRUE);
    LockUsed(B_BATTLER_0, MOVE_HEAT_CRASH, TRUE);
    LockUsed(B_BATTLER_2, MOVE_FAKE_OUT, TRUE);
}

// k4/cyn1 turn 1: Togekiss, at full HP and immune to Primal Groudon's visible
// Precipice Blades, left for Garchomp - which takes that spread hit neutrally
// - just as it landed, and Garchomp arrived at 42%. The body it relieved was
// full, so the entry cost only ever charged a super-effective hit worth more
// than a whole Togekiss; a hit the outgoing body was immune to was free.
AI_DOUBLE_BATTLE_TEST("EC switching: Cynthia's Togekiss does not hand Garchomp a Precipice Blades it is immune to")
{
    bool32 champion = FlagGet(FLAG_IS_CHAMPION);
    GIVEN {
        FlagSet(FLAG_IS_CHAMPION);
        LockPlayer(SPECIES_GROUDON, 100, 0, SET(MOVE_PRECIPICE_BLADES, MOVE_HEAT_CRASH, MOVE_STONE_EDGE, MOVE_PROTECT, NATURE_ADAMANT, ABILITY_DROUGHT, ITEM_RED_ORB, 4, 252, 0, 0, 0, 252));
        LockPlayer(SPECIES_INCINEROAR, 100, 0, SET(MOVE_FAKE_OUT, MOVE_THROAT_CHOP, MOVE_FLARE_BLITZ, MOVE_PARTING_SHOT, NATURE_ADAMANT, ABILITY_INTIMIDATE, ITEM_SITRUS_BERRY, 252, 252, 4, 0, 0, 0));
        LockPlayer(SPECIES_FLUTTER_MANE, 100, 0, SET(MOVE_MOONBLAST, MOVE_SHADOW_BALL, MOVE_DAZZLING_GLEAM, MOVE_PROTECT, NATURE_TIMID, ABILITY_PROTOSYNTHESIS, ITEM_BOOSTER_ENERGY, 4, 0, 0, 252, 0, 252));
        LockPlayer(SPECIES_KARTANA, 100, 0, SET(MOVE_LEAF_BLADE, MOVE_SACRED_SWORD, MOVE_SMART_STRIKE, MOVE_PROTECT, NATURE_JOLLY, ABILITY_BEAST_BOOST, ITEM_LIFE_ORB, 4, 252, 0, 0, 0, 252));
        sLockInjuries[0] = (struct LockInjury){SPECIES_MILOTIC, 381};
        LockAuthoredOpponent(TRAINER_CYNTHIA_1, 8);
        if (!champion)
            FlagClear(FLAG_IS_CHAMPION);
        gTestAiTurnSetupHook = CynthiaTogekissBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PRECIPICE_BLADES);
            MOVE(playerRight, MOVE_THROAT_CHOP, target: opponentLeft);
            EXPECT_MOVES(opponentRight, MOVE_AIR_SLASH, MOVE_FOLLOW_ME, MOVE_THUNDER_WAVE, MOVE_PROTECT);
        }
    }
}

// k3/tl-2 turn 3 as it stood: Mega Gardevoir (evolved turn 1, shielded last
// turn), Mega Tyranitar's sand with two turns left, Incineroar at -1 Attack
// from the Intimidate Gardevoir Traced, its Sitrus spent.
static void TateLizaLeleBoard(void)
{
    gBattleStruct->battlerState[B_BATTLER_0].isFirstTurn = 0;
    gBattleStruct->battlerState[B_BATTLER_1].isFirstTurn = 0;
    gBattleStruct->battlerState[B_BATTLER_2].isFirstTurn = 0;
    gBattleWeather = B_WEATHER_SANDSTORM;
    gBattleStruct->weatherDuration = 2;
    SetActiveGimmick(B_BATTLER_1, GIMMICK_MEGA);
    SetGimmickAsActivated(B_BATTLER_1, GIMMICK_MEGA);
    TryBattleFormChange(B_BATTLER_1, FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM, GetBattlerAbility(B_BATTLER_1));
    gBattleMons[B_BATTLER_1].volatiles.overwrittenAbility = ABILITY_NONE;
    gBattleMons[B_BATTLER_1].hp = 206;
    gBattleMons[B_BATTLER_1].volatiles.consecutiveMoveUses = 1;
    LockUsed(B_BATTLER_1, MOVE_HYPER_VOICE, FALSE);
    LockUsed(B_BATTLER_1, MOVE_PROTECT, TRUE);
    gBattleMons[B_BATTLER_0].statStages[STAT_ATK] = DEFAULT_STAT_STAGE - 1;
    LockUsed(B_BATTLER_0, MOVE_FAKE_OUT, FALSE);
    LockUsed(B_BATTLER_0, MOVE_KNOCK_OFF, FALSE);
    LockUsed(B_BATTLER_0, MOVE_PARTING_SHOT, TRUE);
    LockUsed(B_BATTLER_2, MOVE_FLASH_CANNON, TRUE);
}

// k3/tl-2 turn 3: Tate & Liza's Specs Tapu Lele arrived into Incineroar and
// Heatran and locked itself into Moonblast - neutral on Incineroar, a quarter
// on Heatran - with Focus Blast, super effective on both, in the set. It then
// sat in that lock for two more turns.
AI_DOUBLE_BATTLE_TEST("EC locks: Tate & Liza's Specs Tapu Lele locks into the move the board fears")
{
    GIVEN {
        LockPlayer(SPECIES_INCINEROAR, 65, 176, SET(MOVE_FAKE_OUT, MOVE_FLARE_BLITZ, MOVE_KNOCK_OFF, MOVE_PARTING_SHOT, NATURE_CAREFUL, ABILITY_INTIMIDATE, ITEM_NONE, 252, 0, 4, 0, 252, 0));
        LockPlayer(SPECIES_HEATRAN, 65, 0, SET(MOVE_HEAT_WAVE, MOVE_EARTH_POWER, MOVE_FLASH_CANNON, MOVE_PROTECT, NATURE_MODEST, ABILITY_FLASH_FIRE, ITEM_LEFTOVERS, 252, 0, 4, 252, 0, 0));
        LockPlayer(SPECIES_CELESTEELA, 65, 0, SET(MOVE_FLASH_CANNON, MOVE_AIR_SLASH, MOVE_FLAMETHROWER, MOVE_PROTECT, NATURE_MODEST, ABILITY_BEAST_BOOST, ITEM_LEFTOVERS, 252, 0, 4, 252, 0, 0));
        LockPlayer(SPECIES_METAGROSS, 65, 0, SET(MOVE_METEOR_MASH, MOVE_KNOCK_OFF, MOVE_BULLET_PUNCH, MOVE_PROTECT, NATURE_ADAMANT, ABILITY_CLEAR_BODY, ITEM_LIFE_ORB, 252, 252, 4, 0, 0, 0));
        LockPlayer(SPECIES_FLUTTER_MANE, 65, 167, SET(MOVE_MOONBLAST, MOVE_SHADOW_BALL, MOVE_DAZZLING_GLEAM, MOVE_PROTECT, NATURE_TIMID, ABILITY_PROTOSYNTHESIS, ITEM_CHOICE_SPECS, 4, 0, 0, 252, 0, 252));
        sLockInjuries[0] = (struct LockInjury){SPECIES_SOLROCK, 0};
        sLockInjuries[1] = (struct LockInjury){SPECIES_LUNATONE, 0};
        sLockMilestone = FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT;
        LockLeads(2, 3);
        LockAuthoredOpponent(TRAINER_TATE_AND_LIZA_1, 6);
        gTestAiTurnSetupHook = TateLizaLeleBoard;
    } WHEN {
        TURN {
            SWITCH(playerLeft, 2);
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentRight, MOVE_FOCUS_BLAST);
        }
    }
}

// k2/win-1 turn 3 as it stood: Tapu Koko's Electric Terrain with two turns
// left, Talonflame held in the Band Flare Blitz that removed Koko, Enamorus
// a turn past its Earth Power into Metagross.
static void WinonaBoard(void)
{
    gBattleStruct->battlerState[B_BATTLER_1].isFirstTurn = 0;
    gBattleStruct->battlerState[B_BATTLER_3].isFirstTurn = 0;
    gBattleStruct->battlerState[B_BATTLER_0].isFirstTurn = 0;
    gFieldTimers.terrain = B_TERRAIN_ELECTRIC;
    gFieldTimers.terrainTimer = 2;
    gBattleStruct->choicedMove[B_BATTLER_3] = MOVE_FLARE_BLITZ;
    LockUsed(B_BATTLER_3, MOVE_FLARE_BLITZ, TRUE);
    LockUsed(B_BATTLER_1, MOVE_EARTH_POWER, TRUE);
}

// k2/win-1 turn 3: a 19 HP Metagross beside a full Lanturn. Talonflame's
// Band Flare Blitz and Enamorus's Earth Power both went at Metagross; either
// one removes it, and Earth Power hits Lanturn super effectively too.
AI_DOUBLE_BATTLE_TEST("EC KO allocation: Winona does not send both attackers at a 19 HP Metagross")
{
    GIVEN {
        LockPlayer(SPECIES_METAGROSS, 55, 19, SET(MOVE_METEOR_MASH, MOVE_ZEN_HEADBUTT, MOVE_BULLET_PUNCH, MOVE_PROTECT, NATURE_JOLLY, ABILITY_CLEAR_BODY, ITEM_LIFE_ORB, 4, 252, 0, 0, 0, 252));
        LockPlayer(SPECIES_LANTURN, 55, 0, SET(MOVE_SCALD, MOVE_THUNDERBOLT, MOVE_ICE_BEAM, MOVE_PROTECT, NATURE_MODEST, ABILITY_VOLT_ABSORB, ITEM_SITRUS_BERRY, 252, 0, 4, 252, 0, 0));
        LockPlayer(SPECIES_GARDEVOIR, 55, 0, SET(MOVE_HYPER_VOICE, MOVE_PSYSHOCK, MOVE_MOONBLAST, MOVE_PROTECT, NATURE_TIMID, ABILITY_TRACE, ITEM_GARDEVOIRITE, 4, 0, 0, 252, 0, 252));
        sLockInjuries[0] = (struct LockInjury){SPECIES_ZAPDOS, 0};
        sLockInjuries[1] = (struct LockInjury){SPECIES_ALTARIA, 0};
        sLockInjuries[2] = (struct LockInjury){SPECIES_ENAMORUS, 150};
        sLockInjuries[3] = (struct LockInjury){SPECIES_TALONFLAME, 132};
        LockLeads(2, 3);
        LockAuthoredOpponent(TRAINER_WINONA_1, 5);
        gTestAiTurnSetupHook = WinonaBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        // Both shields held, so neither hit was retargeted.
        EXPECT(gBattleStruct->battlerState[B_BATTLER_1].lastMoveTarget
            != gBattleStruct->battlerState[B_BATTLER_3].lastMoveTarget);
    }
}

// k1/jef-1 turn 0 as it stood: Jeff's authored Misty Terrain, which never
// ends.
static void JeffOpeningBoard(void)
{
    gFieldTimers.terrain = B_TERRAIN_MISTY;
    gFieldTimers.terrainTimer = 0;
}

// k1/jef-1 turn 0: Jeff's Gale Wings Talonflame, faster than both foes,
// opened with Flare Blitz into Glimmora - which resists it - while Brave Bird
// hit Incineroar neutrally. Brave Bird carried the penalty for a priority move
// on a user already faster than its target, meant for a weaker move taken only
// for its place in the order; Gale Wings gives a full Brave Bird that place
// for nothing.
AI_DOUBLE_BATTLE_TEST("EC move choice: Jeff's Talonflame is not charged for the priority Gale Wings lends Brave Bird")
{
    GIVEN {
        LockPlayer(SPECIES_INCINEROAR, 40, 0, SET(MOVE_FAKE_OUT, MOVE_FLARE_BLITZ, MOVE_KNOCK_OFF, MOVE_PARTING_SHOT, NATURE_ADAMANT, ABILITY_INTIMIDATE, ITEM_SITRUS_BERRY, 252, 252, 4, 0, 0, 0));
        LockPlayer(SPECIES_GLIMMORA, 40, 0, SET(MOVE_SLUDGE_BOMB, MOVE_POWER_GEM, MOVE_EARTH_POWER, MOVE_PROTECT, NATURE_MODEST, ABILITY_TOXIC_DEBRIS, ITEM_LIFE_ORB, 252, 0, 4, 252, 0, 0));
        LockPlayer(SPECIES_TALONFLAME, 40, 0, SET(MOVE_BRAVE_BIRD, MOVE_FLARE_BLITZ, MOVE_TAILWIND, MOVE_PROTECT, NATURE_JOLLY, ABILITY_GALE_WINGS, ITEM_SITRUS_BERRY, 4, 252, 0, 0, 0, 252));
        // Lavaridge, before Flannery: three badges, cap40.
        LockAuthoredOpponent(TRAINER_JEFF, 3);
        gTestAiTurnSetupHook = JeffOpeningBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft);
            MOVE(playerRight, MOVE_PROTECT);
            NOT_EXPECT_MOVE(opponentRight, MOVE_FLARE_BLITZ);
        }
    }
}
