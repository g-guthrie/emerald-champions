#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "battle_ai_util.h"
#include "battle_util.h"
#include "test/battle.h"
#include "data.h"
#include "difficulty.h"
#include "event_data.h"
#include "malloc.h"
#include "constants/opponents.h"
#include "emerald_champions_battle_sets.h"

// Boards from the build-l benchmark runs (work/benchmarks-20260924/l1, l2),
// replayed on the campaign's own compiled teams with production stat
// generation, each board's in-battle state restored before the AI reads it.

extern void (*gTestAiTurnSetupHook)(void);
#if TESTING
extern u8 gTestPairBudgetPairs;
#endif

// A member's state on the board being replayed. noItem: the member's item was
// spent or removed before this board.
struct ForecastInjury { enum Species species; u16 hp; u32 status; bool32 noItem; };
EWRAM_DATA static struct ForecastInjury sForecastInjuries[6] = {0};
// A milestone past the badges that sets the fight's level cap. Consumed by the
// next authored build.
EWRAM_DATA static u16 sForecastMilestone = 0;

static void ForecastBuildAuthoredParty(u16 trainerId, u32 badges, struct Pokemon *party, u32 flags)
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
    bool8 savedMilestone = sForecastMilestone ? FlagGet(sForecastMilestone) : FALSE;
    if (sForecastMilestone)
        FlagSet(sForecastMilestone);
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
    if (sForecastMilestone && !savedMilestone)
        FlagClear(sForecastMilestone);
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
static void ForecastAuthoredOpponent(u16 trainerId, u32 badges)
{
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][trainerId];
    struct Pokemon *party = AllocZeroed(sizeof(struct Pokemon) * PARTY_SIZE);
    ForecastBuildAuthoredParty(trainerId, badges, party, BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE);
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
            for (u32 j = 0; j < ARRAY_COUNT(sForecastInjuries); j++)
                if (sForecastInjuries[j].species != SPECIES_NONE
                 && GetMonData(mon, MON_DATA_SPECIES) == sForecastInjuries[j].species)
                {
                    HP(sForecastInjuries[j].hp);
                    if (sForecastInjuries[j].status)
                        Status1(sForecastInjuries[j].status);
                    if (sForecastInjuries[j].noItem)
                        Item(ITEM_NONE);
                }
        }
    }
    memset(sForecastInjuries, 0, sizeof(sForecastInjuries));
    sForecastMilestone = 0;
    Free(party);
}

// A benchmark player member: the prepared set at the fight's cap, perfect IVs
// and the set's EVs, at the HP it had on the board (0 keeps it full). form,
// when given, is the form it had already taken (a Mega spent earlier).
static void ForecastPlayerForm(enum Species species, enum Species form, u32 level, u16 hp, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, level, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    if (form != SPECIES_NONE)
        SetMonData(&mon, MON_DATA_SPECIES, &form);
    CalculateMonStats(&mon);
    PLAYER(form != SPECIES_NONE ? form : species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
        if (form == SPECIES_NONE)
            Ability(GetMonAbility(&mon));
        Moves(set->moves[0], set->moves[1], set->moves[2], set->moves[3]);
        if (hp)
            HP(hp);
    }
}

static void ForecastPlayer(enum Species species, u32 level, u16 hp, const struct EmeraldChampionsBattleSet *set)
{
    ForecastPlayerForm(species, SPECIES_NONE, level, hp, set);
}

#define SET(m1, m2, m3, m4, _nature, _ability, _item, hp, atk, def, spa, spd, spe) \
    (&(const struct EmeraldChampionsBattleSet){ .moves = {m1, m2, m3, m4}, .nature = _nature, \
        .ability = _ability, .item = _item, .evs = {hp, atk, def, spa, spd, spe} })

// The engine sends out the first living members, in party order; tell the
// runner which party slots those leads came from.
static void ForecastLeads(u32 left, u32 right)
{
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = left;
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = right;
}

// A move this battler used before the board: on its record, and as its last
// move when it was the latest.
static void ForecastUsed(enum BattlerId battler, enum Move move, bool32 last)
{
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
        if (gBattleMons[battler].moves[slot] == move)
            gBattleHistory->usedMoves[battler][slot] = move;
    if (last)
        gLastMoves[battler] = gLastResultingMoves[battler] = gLastLandedMoves[battler] = move;
}

// A battler held by its Choice item to the move it used last.
static void ForecastChoiceLock(enum BattlerId battler, enum Move move)
{
    gBattleStruct->choicedMove[battler] = move;
    ForecastUsed(battler, move, TRUE);
}

static void ForecastNotFirstTurn(enum BattlerId battler)
{
    gBattleStruct->battlerState[battler].isFirstTurn = 0;
}

// ---------------------------------------------------------------------------
// Stomping Tantrum after a flinch (engine, not AI).
// ---------------------------------------------------------------------------

// l2/juan2 turn 4: Juan's Scarf Landorus-Therian was flinched by Iron Hands'
// Fake Out, and its Stomping Tantrum the next turn removed a full Iron Hands.
// That is the official rule: Stomping Tantrum doubles when the user's last
// move failed or it was kept from moving - flinch, full paralysis, sleep -
// the turn before (Showdown's runMove treats a false BeforeMove as a failed
// move; the Smogon USUM research thread confirms flinching counts). A move
// stopped by Protect does not count.
SINGLE_BATTLE_TEST("EC engine: Stomping Tantrum doubles after the user flinched, not after Protect blocked it", s16 damage)
{
    u32 before;
    PARAMETRIZE { before = 0; } // The user moved normally the turn before.
    PARAMETRIZE { before = 1; } // It flinched.
    PARAMETRIZE { before = 2; } // Its Stomping Tantrum was blocked by Protect.
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_STOMPING_TANTRUM) == EFFECT_STOMPING_TANTRUM);
        PLAYER(SPECIES_LANDORUS_THERIAN) { Speed(100); Moves(MOVE_STOMPING_TANTRUM, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_IRON_HANDS) { Speed(50); HP(999); MaxHP(999); Moves(MOVE_FAKE_OUT, MOVE_PROTECT, MOVE_CELEBRATE); }
    } WHEN {
        if (before == 0)
            TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        else if (before == 1)
            TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_FAKE_OUT); }
        else
            TURN { MOVE(player, MOVE_STOMPING_TANTRUM); MOVE(opponent, MOVE_PROTECT); }
        TURN { MOVE(player, MOVE_STOMPING_TANTRUM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (before == 1)
            MESSAGE("Landorus flinched and couldn't move!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STOMPING_TANTRUM, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

// ---------------------------------------------------------------------------
// Choice locks held at a dropped stage.
// ---------------------------------------------------------------------------

#define JUAN_AMOONGUSS SET(MOVE_SPORE, MOVE_RAGE_POWDER, MOVE_POLLEN_PUFF, MOVE_PROTECT, NATURE_RELAXED, ABILITY_REGENERATOR, ITEM_ROCKY_HELMET, 252, 0, 252, 0, 4, 0)
#define JUAN_TYRANITAR SET(MOVE_ROCK_SLIDE, MOVE_CRUNCH, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT, NATURE_ADAMANT, ABILITY_SAND_STREAM, ITEM_TYRANITARITE, 252, 252, 4, 0, 0, 0)

// l2/juan1 turn 6 as it stood: Kingdra is held to the Draco Meteor it has
// fired twice, at -4 Sp. Atk; Manaphy sleeps two more turns at +3 after its
// Tail Glow; Rillaboom's Grassy Terrain has two turns left. Mega Tyranitar has
// just replaced Iron Hands.
static void JuanKingdraBoard(void)
{
    ForecastNotFirstTurn(B_BATTLER_0);
    ForecastNotFirstTurn(B_BATTLER_3);
    ForecastNotFirstTurn(B_BATTLER_1);
    gFieldTimers.terrain = B_TERRAIN_GRASSY;
    gFieldTimers.terrainTimer = 2;
    gBattleMons[B_BATTLER_3].statStages[STAT_SPATK] = DEFAULT_STAT_STAGE - 4;
    ForecastChoiceLock(B_BATTLER_3, MOVE_DRACO_METEOR);
    gBattleMons[B_BATTLER_1].statStages[STAT_SPATK] = DEFAULT_STAT_STAGE + 3;
    ForecastUsed(B_BATTLER_1, MOVE_SCALD, TRUE);
    ForecastUsed(B_BATTLER_1, MOVE_TAIL_GLOW, FALSE);
    ForecastUsed(B_BATTLER_0, MOVE_SPORE, TRUE);
}

// l2/juan1 turns 5-7: Juan's Specs Kingdra fired Draco Meteor at 0, -2, -4 and
// -6 while a full Mega-capable Gyarados sat behind it. At -4 the lock is a
// third of its power into two foes that take it neutrally.
AI_DOUBLE_BATTLE_TEST("EC locks: Juan's Kingdra leaves a -4 Draco Meteor lock for Gyarados")
{
    GIVEN {
        ForecastPlayer(SPECIES_AMOONGUSS, 70, 186, JUAN_AMOONGUSS);
        ForecastPlayerForm(SPECIES_TYRANITAR, SPECIES_TYRANITAR_MEGA, 70, 187, JUAN_TYRANITAR);
        sForecastInjuries[0] = (struct ForecastInjury){SPECIES_POLITOED, 0};
        sForecastInjuries[1] = (struct ForecastInjury){SPECIES_LANDORUS_THERIAN, 0};
        sForecastInjuries[2] = (struct ForecastInjury){SPECIES_ALTARIA, 0};
        sForecastInjuries[3] = (struct ForecastInjury){SPECIES_MANAPHY, 283, STATUS1_SLEEP_TURN(2)};
        sForecastInjuries[4] = (struct ForecastInjury){SPECIES_KINGDRA, 146};
        sForecastMilestone = FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT;
        // Manaphy stands on the left here; the benchmark had Kingdra there.
        ForecastLeads(3, 4);
        ForecastAuthoredOpponent(TRAINER_JUAN_1, 7);
        gTestAiTurnSetupHook = JuanKingdraBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_NE(gBattleMons[B_BATTLER_3].species, SPECIES_KINGDRA);
    }
}

// ---------------------------------------------------------------------------
// Charge turns and setup with nothing to cash it.
// ---------------------------------------------------------------------------

#define MAXIE_PELIPPER SET(MOVE_HURRICANE, MOVE_WEATHER_BALL, MOVE_WIDE_GUARD, MOVE_PROTECT, NATURE_MODEST, ABILITY_DRIZZLE, ITEM_DAMP_ROCK, 252, 0, 4, 252, 0, 0)
#define MAXIE_PRIMARINA SET(MOVE_HYPER_VOICE, MOVE_MOONBLAST, MOVE_ICE_BEAM, MOVE_PROTECT, NATURE_MODEST, ABILITY_LIQUID_VOICE, ITEM_MYSTIC_WATER, 252, 0, 4, 252, 0, 0)
#define MAXIE_KARTANA SET(MOVE_LEAF_BLADE, MOVE_SACRED_SWORD, MOVE_SMART_STRIKE, MOVE_PROTECT, NATURE_JOLLY, ABILITY_BEAST_BOOST, ITEM_LIFE_ORB, 4, 252, 0, 0, 0, 252)
#define MAXIE_INCINEROAR SET(MOVE_FAKE_OUT, MOVE_FLARE_BLITZ, MOVE_KNOCK_OFF, MOVE_PARTING_SHOT, NATURE_ADAMANT, ABILITY_INTIMIDATE, ITEM_SITRUS_BERRY, 252, 252, 4, 0, 0, 0)
#define MAXIE_LANDORUS SET(MOVE_EARTH_POWER, MOVE_SLUDGE_BOMB, MOVE_PSYCHIC, MOVE_PROTECT, NATURE_TIMID, ABILITY_SHEER_FORCE, ITEM_LIFE_ORB, 4, 0, 0, 252, 0, 252)

// l1/max-1 turn 5 as it stood: Pelipper has just replaced Whimsicott into its
// Damp Rock rain; Trick Room has ended; Runerigus spent its Mental Herb.
static void MaxieTorkoalBoard(void)
{
    ForecastNotFirstTurn(B_BATTLER_0);
    ForecastNotFirstTurn(B_BATTLER_1);
    ForecastNotFirstTurn(B_BATTLER_3);
    gBattleWeather = B_WEATHER_RAIN_NORMAL;
    gBattleStruct->weatherDuration = 8;
    ForecastUsed(B_BATTLER_1, MOVE_HEAT_WAVE, TRUE);
    ForecastUsed(B_BATTLER_3, MOVE_BODY_PRESS, TRUE);
    ForecastUsed(B_BATTLER_0, MOVE_HYPER_VOICE, FALSE);
}

// l1/max-1 turn 5: Maxie's Torkoal chose Solar Beam into Pelipper's rain,
// where it spends the turn charging and hits at half power the next, with
// Heat Wave and Earth Power in the set.
AI_DOUBLE_BATTLE_TEST("EC charge: Maxie's Torkoal does not Solar Beam in the rain")
{
    GIVEN {
        ForecastPlayer(SPECIES_PRIMARINA, 60, 171, MAXIE_PRIMARINA);
        ForecastPlayer(SPECIES_PELIPPER, 60, 6, MAXIE_PELIPPER);
        ForecastPlayer(SPECIES_LANDORUS, 60, 22, MAXIE_LANDORUS);
        ForecastPlayer(SPECIES_KARTANA, 60, 0, MAXIE_KARTANA);
        ForecastPlayer(SPECIES_INCINEROAR, 60, 0, MAXIE_INCINEROAR);
        sForecastInjuries[0] = (struct ForecastInjury){SPECIES_RUNERIGUS, 82, 0, TRUE};
        // The redesign leads Spiritomb; Runerigus (now slot 5) stood beside
        // Torkoal on the benchmark board.
        ForecastLeads(0, 4);
        ForecastAuthoredOpponent(TRAINER_MAXIE_MAGMA_HIDEOUT, 6);
        gTestAiTurnSetupHook = MaxieTorkoalBoard;
        // The benchmark decision ran into its clock (61 frames against a
        // 60-frame share): the board that keeps both bodies was the one it
        // settled on, and its switch boards were never reached.
        gTestPairBudgetPairs = 30;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_SOLAR_BEAM);
        }
    } THEN {
        gTestPairBudgetPairs = 0;
    }
}

#define WINONA_LANTURN SET(MOVE_SCALD, MOVE_THUNDERBOLT, MOVE_ICE_BEAM, MOVE_PROTECT, NATURE_MODEST, ABILITY_VOLT_ABSORB, ITEM_SITRUS_BERRY, 252, 0, 4, 252, 0, 0)

// l1/win-1 turn 7 as it stood: the player's last Pokemon, a full Lanturn,
// against Noivern held to its Specs Flamethrower and a Skarmory that has just
// replaced Enamorus, its stone in hand.
static void WinonaSkarmoryBoard(void)
{
    ForecastNotFirstTurn(B_BATTLER_0);
    ForecastNotFirstTurn(B_BATTLER_1);
    ForecastChoiceLock(B_BATTLER_1, MOVE_FLAMETHROWER);
    ForecastUsed(B_BATTLER_0, MOVE_THUNDERBOLT, TRUE);
}

// l1/win-1 turn 7: Winona's Skarmory Mega Evolved and used Iron Defense
// against a lone Lanturn whose Scald, Thunderbolt and Ice Beam are all
// special, and whose Thunderbolt hits it super effectively.
AI_DOUBLE_BATTLE_TEST("EC setup: Winona's Skarmory does not Iron Defense against a lone special attacker")
{
    GIVEN {
        ForecastPlayer(SPECIES_LANTURN, 55, 0, WINONA_LANTURN);
        sForecastInjuries[0] = (struct ForecastInjury){SPECIES_ZAPDOS, 0};
        sForecastInjuries[1] = (struct ForecastInjury){SPECIES_ALTARIA, 0};
        sForecastInjuries[2] = (struct ForecastInjury){SPECIES_ENAMORUS, 0};
        sForecastInjuries[3] = (struct ForecastInjury){SPECIES_TALONFLAME, 0};
        ForecastLeads(4, 5);
        ForecastAuthoredOpponent(TRAINER_WINONA_1, 5);
        gTestAiTurnSetupHook = WinonaSkarmoryBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            NOT_EXPECT_MOVE(opponentRight, MOVE_IRON_DEFENSE);
        }
    }
}
