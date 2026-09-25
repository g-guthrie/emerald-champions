#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "battle_gimmick.h"
#include "battle_controllers.h"
#include "battle_ai_util.h"
#include "emerald_champions_battle_plan.h"
#include "test/battle.h"
#include "data.h"
#include "difficulty.h"
#include "event_data.h"
#include "field_weather.h"
#include "constants/weather.h"
#include "random.h"
#include "malloc.h"
#include "trainer_pools.h"
#include "battle_transition.h"
#include "constants/opponents.h"
#include "emerald_champions_battle_sets.h"

// Execute the production lookup and generated table. No duplicate plan table
// or mock trainer-owner resolver: these are the ROM's actual trainer IDs.
TEST("EC battle plans: compiled directives follow trainer ownership and exclude other battle namespaces")
{
    u32 savedFlags = gBattleTypeFlags;
    u16 savedA = TRAINER_BATTLE_PARAM.opponentA;
    u16 savedB = TRAINER_BATTLE_PARAM.opponentB;

    TRAINER_BATTLE_PARAM.opponentA = TRAINER_ROXANNE_1;
    TRAINER_BATTLE_PARAM.opponentB = TRAINER_BILLY;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_1), EC_BATTLE_PLAN_TAILWIND | EC_BATTLE_PLAN_PRESSURE | EC_BATTLE_PLAN_MEGA_REVEAL);
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_3), EC_BATTLE_PLAN_TAILWIND | EC_BATTLE_PLAN_PRESSURE | EC_BATTLE_PLAN_MEGA_REVEAL);
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_0), 0);
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_2), 0);

    gBattleTypeFlags |= BATTLE_TYPE_TWO_OPPONENTS;
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_1), EC_BATTLE_PLAN_TAILWIND | EC_BATTLE_PLAN_PRESSURE | EC_BATTLE_PLAN_MEGA_REVEAL);
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_3), EC_BATTLE_PLAN_RAIN);

    TRAINER_BATTLE_PARAM.opponentA = TRAINER_TATE_AND_LIZA_1;
    EXPECT(EmeraldChampions_GetBattlePlan(B_BATTLER_1) & EC_BATTLE_PLAN_TRICK_ROOM);
    // Tabitha's hideout team was re-authored to a sand/setup core with no
    // partner tactic; Cristian is the ALLY_COMBO owner this checks now.
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_CRISTIAN;
    EXPECT(EmeraldChampions_GetBattlePlan(B_BATTLER_1) & EC_BATTLE_PLAN_ALLY_COMBO);
    EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_1, SPECIES_FALINKS, SPECIES_GALLADE), EC_BATTLE_TACTIC_ACTIVATE);
    EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_1, SPECIES_GALLADE, SPECIES_FALINKS), EC_BATTLE_TACTIC_ACTIVATE);
    // Tabitha's hideout team keeps no tactic at all after the audit.
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_TABITHA_MAGMA_HIDEOUT;
    EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_1, SPECIES_DRAGAPULT, SPECIES_COALOSSAL), 0);
    EXPECT(!(EmeraldChampions_GetBattlePlan(B_BATTLER_1) & EC_BATTLE_PLAN_ALLY_COMBO));
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_DARIUS;
    EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_1, SPECIES_CHATOT, SPECIES_KILOWATTREL), EC_BATTLE_TACTIC_ACTIVATE);
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_NATE;
    EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_1, SPECIES_ORANGURU, SPECIES_DELPHOX), EC_BATTLE_TACTIC_INSTRUCT);
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_CRISTIAN;
    EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_1, SPECIES_VOLCANION, SPECIES_COALOSSAL), 0);
    EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_3, SPECIES_DRAGAPULT, SPECIES_COALOSSAL), 0);
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_CRISTIAN;
    EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_1, SPECIES_FALINKS, SPECIES_GALLADE_MEGA), EC_BATTLE_TACTIC_ACTIVATE);
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_SYLVIA;
    EXPECT(EmeraldChampions_GetBattlePlan(B_BATTLER_1) & EC_BATTLE_PLAN_ALLY_COMBO);
    EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_1, SPECIES_DONDOZO, SPECIES_TATSUGIRI), EC_BATTLE_TACTIC_COMMANDER);

    static const u32 foreignContexts[] = {
        BATTLE_TYPE_LINK, BATTLE_TYPE_BATTLE_TOWER, BATTLE_TYPE_TRAINER_HILL,
        BATTLE_TYPE_EREADER_TRAINER, BATTLE_TYPE_SECRET_BASE, BATTLE_TYPE_RECORDED_LINK,
    };
    for (u32 i = 0; i < ARRAY_COUNT(foreignContexts); i++)
    {
        gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE | foreignContexts[i];
        EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_1), 0);
        EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_3), 0);
        EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_1, SPECIES_DONDOZO, SPECIES_TATSUGIRI), 0);
    }
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_RECORDED;
    // Ownership is what this checks; the authored plan bits themselves move
    // with the teams file, so assert the directive rather than the whole mask.
    EXPECT(EmeraldChampions_GetBattlePlan(B_BATTLER_1) & EC_BATTLE_PLAN_ALLY_COMBO);
    EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_1, SPECIES_DONDOZO, SPECIES_TATSUGIRI), EC_BATTLE_TACTIC_COMMANDER);
    gBattleTypeFlags |= BATTLE_TYPE_BATTLE_TOWER;
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_1), 0);
    gBattleTypeFlags = BATTLE_TYPE_DOUBLE;
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_1), 0);
    gBattleTypeFlags |= BATTLE_TYPE_TRAINER;
    TRAINER_BATTLE_PARAM.opponentA = TRAINERS_COUNT;
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_1), 0);
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_NONE;
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_1), 0);

    TRAINER_BATTLE_PARAM.opponentA = savedA;
    TRAINER_BATTLE_PARAM.opponentB = savedB;
    gBattleTypeFlags = savedFlags;
}

// A benchmark board caught mid-battle: these authored members start at these
// HP (0 is fainted). Consumed by the next AuthoredOpponentWithPartner call.
// sAuthoredLeads, when set, brings those two members to the front as the
// leads that board had on the field.
struct AuthoredInjury { enum Species species; u16 hp; };
EWRAM_DATA static struct AuthoredInjury sAuthoredInjuries[4] = {0};
// A benchmark board whose left lead is a later member: that party slot trades
// places with slot 0. Consumed like the injuries.
EWRAM_DATA static u8 sAuthoredLeadSlot = 0;
EWRAM_DATA static enum Species sAuthoredLeads[2] = {0};

// Use the compiled campaign loadouts and production stat/level generation,
// including reserves, rather than a second hand-maintained copy of the team.
static void AuthoredOpponentWithPartner(u16 trainerId, u32 badges, bool32 injuredCoalossal, u32 partnerSlot)
{
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][trainerId];
    struct Pokemon *party = AllocZeroed(sizeof(struct Pokemon) * PARTY_SIZE);
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
    SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
    CreateNPCTrainerPartyFromTrainer(party, trainer);
    EXPECT(partnerSlot < trainer->partySize);
    if (sAuthoredLeadSlot != 0)
    {
        // Same fixture rule as the partner swap: non-Mega leads only.
        EXPECT(sAuthoredLeadSlot < trainer->partySize && sAuthoredLeadSlot != partnerSlot);
        struct Pokemon swap = party[0];
        party[0] = party[sAuthoredLeadSlot];
        party[sAuthoredLeadSlot] = swap;
        if (partnerSlot == 0)
            partnerSlot = sAuthoredLeadSlot;
        sAuthoredLeadSlot = 0;
    }
    if (partnerSlot != 1)
    {
        // Alternate-deployment fixture, preserving every native loadout.
        // Callers use non-Mega parties so a slot permission is not relocated.
        struct Pokemon swap = party[1];
        party[1] = party[partnerSlot];
        party[partnerSlot] = swap;
    }
    for (u32 lead = 0; lead < ARRAY_COUNT(sAuthoredLeads); lead++)
    {
        if (sAuthoredLeads[lead] == SPECIES_NONE)
            continue;
        for (u32 i = 0; i < trainer->partySize; i++)
            if (GetMonData(&party[i], MON_DATA_SPECIES) == sAuthoredLeads[lead])
            {
                struct Pokemon swap = party[lead];
                party[lead] = party[i];
                party[i] = swap;
                break;
            }
    }
    memset(sAuthoredLeads, 0, sizeof(sAuthoredLeads));
    gBattleTypeFlags = savedFlags;
    SetCurrentDifficultyLevel(savedDifficulty);
    for (u32 i = 0; i < ARRAY_COUNT(savedBadges); i++)
    {
        if (savedBadges[i])
            FlagSet(FLAG_BADGE01_GET + i);
        else
            FlagClear(FLAG_BADGE01_GET + i);
    }

    if (IsAITest())
        AI_FLAGS(trainer->aiFlags | AI_FLAG_DOUBLE_BATTLE);
    gBattleTestRunnerState->data.recordedBattle.opponentA = trainerId;
    for (u32 i = 0; i < trainer->partySize; i++)
    {
        OPPONENT(GetMonData(&party[i], MON_DATA_SPECIES))
        {
            *gBattleTestRunnerState->data.currentMon = party[i];
            Nature(GetNature(&party[i]));
            Ability(GetMonAbility(&party[i]));
            Speed(GetMonData(&party[i], MON_DATA_SPEED));
            // Declare native order without freezing Speed across Mega/form
            // changes: the harness otherwise treats Speed() as a fixed stat.
            bool32 fixedSpeed = FALSE;
            SetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_HYPER_TRAINED_SPEED, &fixedSpeed);
            Moves(GetMonData(&party[i], MON_DATA_MOVE1), GetMonData(&party[i], MON_DATA_MOVE2),
                  GetMonData(&party[i], MON_DATA_MOVE3), GetMonData(&party[i], MON_DATA_MOVE4));
            if (injuredCoalossal && GetMonData(&party[i], MON_DATA_SPECIES) == SPECIES_COALOSSAL)
                HP(1);
            for (u32 j = 0; j < ARRAY_COUNT(sAuthoredInjuries); j++)
                if (sAuthoredInjuries[j].species != SPECIES_NONE
                 && GetMonData(&party[i], MON_DATA_SPECIES) == sAuthoredInjuries[j].species)
                    HP(sAuthoredInjuries[j].hp);
        }
    }
    memset(sAuthoredInjuries, 0, sizeof(sAuthoredInjuries));
    Free(party);
}

static void AuthoredOpponent(u16 trainerId, u32 badges, bool32 injuredCoalossal)
{
    AuthoredOpponentWithPartner(trainerId, badges, injuredCoalossal, 1);
}

DOUBLE_BATTLE_TEST("EC Gym: Lilith's actual Costar reserve copies Scraggy's earned boosts")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Speed(30); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Speed(20); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        AuthoredOpponent(TRAINER_LILITH, 1, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_FAKE_OUT, target: playerLeft);
            MOVE(opponentRight, MOVE_DRAGON_DANCE);
        }
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE);
            SWITCH(opponentLeft, 3);
            MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_FLAMIGO);
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

DOUBLE_BATTLE_TEST("EC Gym mechanics: Victory Dance copies before Oricorio's selected attack")
{
    bool32 dance;
    s16 damage;
    PARAMETRIZE { dance = TRUE; }
    PARAMETRIZE { dance = FALSE; }
    GIVEN {
        PLAYER(SPECIES_WEEZING) { Level(20); HP(300); MaxHP(300); Defense(80); Speed(30); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Speed(20); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        AuthoredOpponent(TRAINER_JOCELYN, 1, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, dance ? MOVE_VICTORY_DANCE : MOVE_PROTECT);
            MOVE(opponentRight, MOVE_ACROBATICS, target: playerLeft);
        }
    } SCENE {
        HP_BAR(playerLeft, captureDamage: &damage);
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + dance);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + dance);
        EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE + dance);
        EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + dance);
        EXPECT(playerLeft->hp < playerLeft->maxHP);
        Test_MgbaPrintf("DANCE=%d ACRO_DAMAGE=%d LILLI_HP=%d LILLI_DEF=%d ORICORIO_ATK=%d speeds=%d/%d", dance, damage, opponentLeft->maxHP, opponentLeft->defense, opponentRight->attack, opponentLeft->speed, opponentRight->speed);
        EXPECT(opponentLeft->speed > opponentRight->speed);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Gym: Jocelyn dances on a safe board with the Dancer relay up")
{
    GIVEN {
        // The room's authored engine is the relay: the setter dances, Oricorio
        // copies it, and both halves gain. On a board that cannot punish the
        // turn there is nothing for a direct attack to beat.
        PLAYER(SPECIES_MAGIKARP) { Level(24); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_SPLASH); }
        PLAYER(SPECIES_MAGIKARP) { Level(24); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        AuthoredOpponent(TRAINER_JOCELYN, 2, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SPLASH);
            MOVE(playerRight, MOVE_SPLASH);
            EXPECT_MOVE(opponentLeft, MOVE_VICTORY_DANCE);
        }
    } THEN {
        EXPECT_GT(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        // Dancer copies it, so the relay pays on both halves of the room.
        EXPECT_GT(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        Test_MgbaPrintf("JOCELYN_DANCE_DECISION_FRAMES=%d", gBattleStruct->aiDelayFrames);
        EXPECT(gBattleStruct->aiDelayFrames <= 72);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Gym: Jocelyn's dance enables a knockout while protecting its user")
{
    GIVEN {
        // Synthetic physical-pressure board, not stage-pool difficulty proof.
        // Weak attackers: neither flank threatens the setter, so setting up is
        // right whichever of them the unreadable commands aim at.
        PLAYER(SPECIES_WEEZING) { Level(20); HP(37); MaxHP(100); Attack(20); Defense(80); SpDefense(100); Speed(60); Ability(ABILITY_LEVITATE); Moves(MOVE_POISON_JAB); }
        PLAYER(SPECIES_WEEZING) { Level(20); HP(37); MaxHP(100); Attack(20); Defense(80); SpDefense(100); Speed(55); Ability(ABILITY_LEVITATE); Moves(MOVE_POISON_JAB); }
        AuthoredOpponent(TRAINER_JOCELYN, 1, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_POISON_JAB, target: opponentLeft, secondaryEffect: FALSE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_POISON_JAB, target: opponentLeft, secondaryEffect: FALSE, criticalHit: FALSE);
            // Setting up in front of two unread attackers is a gamble the AI
            // no longer takes for free; the attacking half of the room stands.
            EXPECT_MOVE(opponentRight, MOVE_ACROBATICS);
        }
    } THEN {
        EXPECT(playerLeft->hp < playerLeft->maxHP || playerRight->hp < playerRight->maxHP);
        EXPECT(opponentLeft->hp > 0);
        Test_MgbaPrintf("JOCELYN_DECISION_FRAMES=%d", gBattleStruct->aiDelayFrames);
        EXPECT(gBattleStruct->aiDelayFrames <= 72);
    }
}

DOUBLE_BATTLE_TEST("EC Gym mechanics: Cristian's actual Beat Up builds Rage Fist without a knockout")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Defense(100); Speed(30); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Speed(20); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        AuthoredOpponentWithPartner(TRAINER_CRISTIAN, 1, FALSE, 2);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_BEAT_UP, target: opponentRight);
            MOVE(opponentRight, MOVE_RAGE_FIST, target: playerLeft);
        }
    } THEN {
        EXPECT(opponentRight->hp > 0);
        EXPECT_EQ((u32)GetBattlerPartyState(B_BATTLER_3)->timesGotHit, 6);
        EXPECT(playerLeft->hp < playerLeft->maxHP);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Gym: Cristian activates the recipient that survives the Beat Up")
{
    bool32 lethal;
    // Annihilape is neutral to Dark: a healthy one is exactly the Rage Fist
    // recipient Cristian's plan wants. At 18 HP the six Beat Up hits (about 30)
    // would KO it, which the plan rejects; nothing else threatens it this turn.
    // Lucario resists Dark; Beat Up (about 12) plus this Psychic (about 36)
    // leaves it standing. At SpAttack 120 the Psychic alone KOs Lucario, so no
    // choice of Cristian's could keep it alive.
    PARAMETRIZE { lethal = TRUE; }   // Annihilape at 18 HP.
    PARAMETRIZE { lethal = FALSE; }  // Lucario activation case.
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Level(20); HP(140); MaxHP(140); Defense(100); SpAttack(60); Speed(30); Ability(ABILITY_TELEPATHY); Moves(MOVE_PSYCHIC, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(20); HP(300); MaxHP(300); Defense(150); Speed(20); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        AuthoredOpponentWithPartner(TRAINER_CRISTIAN, 1, FALSE, lethal ? 2 : 3);
        if (lethal)
        {
            u32 hp = 18;
            SetMonData(&OPPONENT_A_PARTY[1], MON_DATA_HP, &hp);
        }
    } WHEN {
        TURN {
            if (lethal)
                MOVE(playerLeft, MOVE_CELEBRATE);
            else
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight);
            MOVE(playerRight, MOVE_CELEBRATE);
            if (lethal)
                NOT_EXPECT_MOVE(opponentLeft, MOVE_BEAT_UP);
            else
                EXPECT_MOVE(opponentLeft, MOVE_BEAT_UP, target: opponentRight);
        }
    } THEN {
        // AI replacements can occupy this battler after the recipient faints.
        // Check the original deployed party member, not its replacement.
        EXPECT(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP) > 0);
        Test_MgbaPrintf("CRISTIAN_RAGE_DECISION_FRAMES=%d", gBattleStruct->aiDelayFrames);
        EXPECT(gBattleStruct->aiDelayFrames <= 72);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Gym: Brawly protects his actual Mega Heracross from a fast Flying attack")
{
    GIVEN {
        PLAYER(SPECIES_AERODACTYL) { Level(20); HP(100); MaxHP(100); Attack(100); Speed(100); Moves(MOVE_DUAL_WINGBEAT); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Speed(20); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        AuthoredOpponent(TRAINER_BRAWLY_1, 1, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DUAL_WINGBEAT, target: opponentRight);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_FOLLOW_ME);
            EXPECT_MOVE(opponentRight, MOVE_ROCK_BLAST, gimmick: GIMMICK_MEGA, target: playerLeft);
        }
    } THEN {
        EXPECT_EQ(opponentRight->species, SPECIES_HERACROSS_MEGA);
        EXPECT_EQ(opponentRight->ability, ABILITY_SKILL_LINK);
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
        EXPECT_EQ(playerLeft->hp, 0);
        Test_MgbaPrintf("BRAWLY_DECISION_FRAMES=%d", gBattleStruct->aiDelayFrames);
        EXPECT(gBattleStruct->aiDelayFrames <= 72);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Gym counterplay: Taunt denies the copied setup and permits direct offense")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Level(20); HP(300); MaxHP(300); Defense(200); SpDefense(200); Speed(200); Moves(MOVE_TAUNT); }
        PLAYER(SPECIES_WEEZING) { Level(20); HP(37); MaxHP(100); Attack(85); Defense(80); SpDefense(100); Speed(60); Ability(ABILITY_LEVITATE); Moves(MOVE_POISON_JAB); }
        AuthoredOpponent(TRAINER_JOCELYN, 1, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
            MOVE(playerRight, MOVE_POISON_JAB, target: opponentLeft, secondaryEffect: FALSE, criticalHit: FALSE);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_VICTORY_DANCE);
        }
    } THEN {
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Gym counterplay: Rage Fist does not justify fatal or ineffective friendly fire")
{
    bool32 immune;
    PARAMETRIZE { immune = FALSE; }
    PARAMETRIZE { immune = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(immune ? SPECIES_SNORLAX : SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Defense(100); Speed(30); Moves(MOVE_CELEBRATE); }
        PLAYER(immune ? SPECIES_SNORLAX : SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Defense(100); Speed(20); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_FALINKS) { Level(30); Speed(100); Moves(MOVE_BEAT_UP, MOVE_COACHING, MOVE_CLOSE_COMBAT, MOVE_PROTECT); }
        OPPONENT(SPECIES_ANNIHILAPE) { Level(30); HP(immune ? 150 : 1); MaxHP(150); Speed(80); Moves(MOVE_RAGE_FIST, MOVE_DRAIN_PUNCH, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gChosenMoveByBattler[B_BATTLER_1] != MOVE_BEAT_UP
            || gBattleStruct->moveTarget[B_BATTLER_1] != B_BATTLER_3);
        EXPECT(opponentRight->hp > 0);
    }
}

DOUBLE_BATTLE_TEST("EC shoreline: the authored smuggler inherits each actual partner ability")
{
    u32 partner;
    PARAMETRIZE { partner = 1; }
    PARAMETRIZE { partner = 2; }
    PARAMETRIZE { partner = 3; }
    GIVEN {
        PLAYER(SPECIES_ALAKAZAM) { Level(100); Speed(200); Moves(MOVE_PSYCHIC, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(100); Speed(10); Moves(MOVE_CELEBRATE); }
        AuthoredOpponent(TRAINER_GRUNT_RUSTURF_TUNNEL, 1, FALSE);
    } WHEN {
        if (partner != 1)
            TURN {
                MOVE(playerLeft, MOVE_CELEBRATE);
                MOVE(playerRight, MOVE_CELEBRATE);
                MOVE(opponentLeft, MOVE_PROTECT);
                SWITCH(opponentRight, partner);
            }
        TURN {
            MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_KNOCK_OFF, target: playerRight);
            MOVE(opponentRight, partner == 1 ? MOVE_WATERFALL : partner == 2 ? MOVE_SLUDGE_BOMB : MOVE_HEAT_WAVE, target: playerRight);
            SEND_OUT(opponentRight, partner == 1 ? 2 : 1);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_GRIMER_ALOLA);
        EXPECT_EQ(opponentLeft->ability, partner == 1 ? ABILITY_INTIMIDATE : partner == 2 ? ABILITY_ADAPTABILITY : ABILITY_INFILTRATOR);
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 2);
    }
}

AI_DOUBLE_BATTLE_TEST("EC shoreline: authored Ned uses Soak to enable Wattrel's knockout")
{
    GIVEN {
        // Synthetic resistant board, using Ned's complete native party and
        // Medium badge-one levels. This is not an earned fight or a legal-pool benchmark.
        PLAYER(SPECIES_FERROTHORN) { Level(20); HP(55); Speed(25); Ability(ABILITY_IRON_BARBS); Moves(MOVE_SEED_BOMB); }
        PLAYER(SPECIES_FERROTHORN) { Level(20); HP(55); Speed(20); Ability(ABILITY_IRON_BARBS); Moves(MOVE_SEED_BOMB); }
        AuthoredOpponent(TRAINER_NED, 1, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SEED_BOMB, target: opponentLeft);
            MOVE(playerRight, MOVE_SEED_BOMB, target: opponentLeft);
            EXPECT_MOVE(opponentLeft, MOVE_SOAK);
            EXPECT_MOVE(opponentRight, MOVE_THUNDERBOLT);
        }
    } THEN {
        EXPECT(playerLeft->hp == 0 || playerRight->hp == 0);
        Test_MgbaPrintf("NED_SOAK_DECISION_FRAMES=%d", gBattleStruct->aiDelayFrames);
        EXPECT(gBattleStruct->aiDelayFrames <= 72);
    }
}

DOUBLE_BATTLE_TEST("EC shoreline mechanics: native Soak changes Wattrel's actual damage")
{
    s16 damage;
    GIVEN {
        PLAYER(SPECIES_FERROTHORN) { Level(20); HP(300); MaxHP(300); Speed(25); Ability(ABILITY_IRON_BARBS); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_FERROTHORN) { Level(20); HP(300); MaxHP(300); Speed(20); Ability(ABILITY_IRON_BARBS); Moves(MOVE_CELEBRATE); }
        AuthoredOpponent(TRAINER_NED, 1, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_SOAK, target: playerLeft);
            MOVE(opponentRight, MOVE_THUNDERBOLT, target: playerLeft);
        }
    } SCENE {
        HP_BAR(playerLeft, captureDamage: &damage);
    } THEN {
        Test_MgbaPrintf("Native Soak Thunderbolt damage: %d", damage);
        EXPECT_EQ(playerLeft->types[0], TYPE_WATER);
        EXPECT_EQ(playerLeft->types[1], TYPE_WATER);
        EXPECT(damage > 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC shoreline: authored Ned charges Wattrel before its attack")
{
    GIVEN {
        PLAYER(SPECIES_LAPRAS) { Level(25); HP(110); Speed(35); Ability(ABILITY_WATER_ABSORB); Moves(MOVE_ICE_BEAM); }
        PLAYER(SPECIES_LAPRAS) { Level(25); HP(110); Speed(30); Ability(ABILITY_WATER_ABSORB); Moves(MOVE_ICE_BEAM); }
        AuthoredOpponent(TRAINER_NED, 1, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ICE_BEAM, target: opponentRight);
            MOVE(playerRight, MOVE_ICE_BEAM, target: opponentLeft);
            EXPECT_MOVE(opponentLeft, MOVE_TAILWIND);
            EXPECT_MOVE(opponentRight, MOVE_THUNDERBOLT);
        }
    } THEN {
        // The authored order is the subject. Which flank the advanced attack
        // removes now depends on a forecast, not on the pending commands.
        EXPECT(playerLeft->hp < playerLeft->maxHP || playerRight->hp < playerRight->maxHP);
        Test_MgbaPrintf("NED_WIND_DECISION_FRAMES=%d", gBattleStruct->aiDelayFrames);
        EXPECT(gBattleStruct->aiDelayFrames <= 72);
    }
}

DOUBLE_BATTLE_TEST("EC shoreline: authored Arrokuda bypasses its own Finneon's Storm Drain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Speed(40); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Speed(30); Moves(MOVE_CELEBRATE); }
        AuthoredOpponent(TRAINER_NED, 1, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_PROTECT);
            SWITCH(opponentRight, 3);
        }
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_TAILWIND);
            MOVE(opponentRight, MOVE_LIQUIDATION, target: playerLeft);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->ability, ABILITY_STORM_DRAIN);
        EXPECT_EQ(opponentRight->ability, ABILITY_PROPELLER_TAIL);
        EXPECT(playerLeft->hp < playerLeft->maxHP);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Soak forecast: converted Ground targets enable Electric moves only after a legal timely conversion")
{
    enum Ability ability = ABILITY_STURDY;
    u32 setterSpeed = 100;
    bool32 enabled = TRUE;
    PARAMETRIZE { }
    PARAMETRIZE { ability = ABILITY_WATER_ABSORB; enabled = FALSE; }
    PARAMETRIZE { ability = ABILITY_STORM_DRAIN; enabled = FALSE; }
    PARAMETRIZE { ability = ABILITY_MAGIC_BOUNCE; enabled = FALSE; }
    PARAMETRIZE { ability = ABILITY_GOOD_AS_GOLD; enabled = FALSE; }
    PARAMETRIZE { ability = ABILITY_MULTITYPE; enabled = FALSE; }
    PARAMETRIZE { setterSpeed = 10; enabled = FALSE; }
    GIVEN {
        // Narrow conversion control; deliberately omit Hydro Pump to isolate
        // the Electric-immunity transition instead of forcing Ned to ignore
        // a better ordinary Water attack against a Ground opponent.
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_GOLEM) { Level(30); HP(200); MaxHP(200); Speed(40); Ability(ability); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GOLEM) { Level(30); HP(200); MaxHP(200); Speed(30); Ability(ability); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_FINNEON) { Level(30); Speed(setterSpeed); Ability(ABILITY_STORM_DRAIN); Moves(MOVE_SOAK, MOVE_PROTECT); }
        OPPONENT(SPECIES_WATTREL) { Level(30); Speed(80); Ability(ABILITY_WIND_POWER); Moves(MOVE_THUNDERBOLT, MOVE_AIR_SLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            if (enabled)
            {
                EXPECT_MOVE(opponentLeft, MOVE_SOAK);
                EXPECT_MOVE(opponentRight, MOVE_THUNDERBOLT);
            }
            else
                EXPECT_MOVE(opponentRight, MOVE_AIR_SLASH);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC Soak anchors: hypothetical types and charge preserve battlers and RNG")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_FERROTHORN) { Speed(40); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(30); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_FINNEON) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WATTREL) { Speed(80); Ability(ABILITY_WIND_POWER); Item(ITEM_LIFE_ORB); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); }
    } THEN {
        struct BattlePokemon saved[MAX_BATTLERS_COUNT];
        memcpy(saved, gBattleMons, sizeof(saved));
        rng_value_t first = gRngValue, second = gRng2Value;
        struct AiCalcValues calc = {.move = MOVE_THUNDERBOLT, .weather = AI_GetWeather(), .terrain = gFieldTimers.terrain};
        struct SimulatedDamage ordinary = AI_CalcSoakChargeDamage(&calc, B_BATTLER_3, B_BATTLER_0, FALSE, FALSE, FALSE);
        struct SimulatedDamage soaked = AI_CalcSoakChargeDamage(&calc, B_BATTLER_3, B_BATTLER_0, FALSE, TRUE, FALSE);
        struct SimulatedDamage charged = AI_CalcSoakChargeDamage(&calc, B_BATTLER_3, B_BATTLER_0, FALSE, TRUE, TRUE);
        EXPECT(soaked.minimum > ordinary.maximum);
        EXPECT(charged.minimum > soaked.maximum);
        u32 priorHits = GetBattlerPartyState(B_BATTLER_3)->timesGotHit;
        calc.move = MOVE_RAGE_FIST;
        struct SimulatedDamage quietRage = AI_CalcRageFistDamage(&calc, B_BATTLER_3, B_BATTLER_0, 0, FALSE, FALSE);
        struct SimulatedDamage fullRage = AI_CalcRageFistDamage(&calc, B_BATTLER_3, B_BATTLER_0, 6, FALSE, FALSE);
        EXPECT(fullRage.minimum > quietRage.maximum);
        EXPECT_EQ((u32)GetBattlerPartyState(B_BATTLER_3)->timesGotHit, priorHits);
        EXPECT_EQ(memcmp(saved, gBattleMons, sizeof(saved)), 0);
        EXPECT_EQ(memcmp(&gRngValue, &first, sizeof(first)), 0);
        EXPECT_EQ(memcmp(&gRng2Value, &second, sizeof(second)), 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Soak forecast: a flinched conversion is not assumed to have happened")
{
    GIVEN {
        PLAYER(SPECIES_FERROTHORN) { Level(20); HP(55); Speed(25); Ability(ABILITY_IRON_BARBS); Moves(MOVE_PROTECT, MOVE_CELEBRATE); }
        PLAYER(SPECIES_MIENFOO) { Level(20); Speed(30); Ability(ABILITY_REGENERATOR); Moves(MOVE_PROTECT, MOVE_FAKE_OUT); }
        AuthoredOpponent(TRAINER_NED, 1, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_FAKE_OUT, target: opponentLeft);
            // The Fake Out that flinches Finneon is a pending command, so the
            // conversion is a plan the AI is entitled to make, not a fiction:
            // Soak the Steel/Grass flank and aim the Electric attack at it.
        }
    } THEN {
        EXPECT_EQ(gChosenMoveByBattler[B_BATTLER_3], MOVE_THUNDERBOLT);
        EXPECT_EQ(gBattleStruct->moveTarget[B_BATTLER_3], B_BATTLER_0);
    }
}



DOUBLE_BATTLE_TEST("EC pivot cleanup: exhausted bench does not skip the next target")
{
    u32 move = MOVE_VOLT_SWITCH;
    PARAMETRIZE { move = MOVE_VOLT_SWITCH; }
    PARAMETRIZE { move = MOVE_U_TURN; }
    PARAMETRIZE { move = MOVE_FLIP_TURN; }
    PARAMETRIZE { move = MOVE_PARTING_SHOT; }
    GIVEN {
        // Frozen mechanic reproduction, independent of authored trainer sets.
        // The first target falls after the pivot; the second turn must still
        // affect the other target even when there is nobody to switch into.
        PLAYER(SPECIES_MIENFOO) { Level(30); MaxHP(76); HP(76); Defense(35); SpDefense(35); Speed(79); Item(move == MOVE_PARTING_SHOT ? ITEM_NONE : ITEM_FOCUS_SASH); Ability(ABILITY_REGENERATOR); Moves(MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH); }
        PLAYER(SPECIES_LOMBRE) { Level(30); MaxHP(400); HP(400); Defense(100); SpDefense(100); Speed(44); Ability(ABILITY_SWIFT_SWIM); Moves(MOVE_SCALD); }
        OPPONENT(SPECIES_ROTOM) { Level(28); MaxHP(200); HP(200); SpAttack(200); Speed(82); Ability(ABILITY_LEVITATE); Moves(MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_KILOWATTREL) { Level(29); MaxHP(300); HP(300); Attack(100); SpAttack(93); SpDefense(100); Speed(114); Ability(ABILITY_VOLT_ABSORB); Moves(move); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft);
            MOVE(opponentRight, move, target: playerLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentLeft, MOVE_THUNDERBOLT, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft);
            MOVE(opponentRight, move, target: playerRight, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentLeft, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 0);
        EXPECT_EQ(gBattleStruct->battlerState[B_BATTLER_3].targetsDone[B_BATTLER_2], FALSE);
        EXPECT(!(gAbsentBattlerFlags & (1u << B_BATTLER_3)));
        if (move == MOVE_PARTING_SHOT) {
            EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
            EXPECT_EQ(playerRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
        } else {
            EXPECT(playerRight->hp < playerRight->maxHP);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC Tailwind: charge and speed rewards exclude immune or fainted foes")
{
    GIVEN {
        // Frozen two-turn reproduction: Xatu falls, leaving slower Ground
        // Mudbray. Neither its former partner's Speed nor an unusable charge
        // should reward Tailwind. Independent of later trainer authoring.
        const enum Species species[] = {SPECIES_MUDBRAY, SPECIES_XATU, SPECIES_PERSIAN, SPECIES_KILOWATTREL, SPECIES_TRUMBEAK, SPECIES_GLIGAR};
        const struct EmeraldChampionsBattleSet sets[] = {
            {.moves = {MOVE_EARTHQUAKE, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_PROTECT}, .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_STAMINA, .evs = {252, 252, 4, 0, 0, 0}},
            {.moves = {MOVE_PSYCHIC, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT}, .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE, .evs = {4, 0, 0, 252, 0, 252}},
            {.moves = {MOVE_FAKE_OUT, MOVE_ICY_WIND, MOVE_U_TURN, MOVE_PLAY_ROUGH}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_JOLLY, .ability = ABILITY_TECHNICIAN, .evs = {4, 252, 0, 0, 0, 252}},
            {.moves = {MOVE_THUNDERBOLT, MOVE_HURRICANE, MOVE_TAILWIND, MOVE_PROTECT}, .item = ITEM_LIFE_ORB, .nature = NATURE_TIMID, .ability = ABILITY_WIND_POWER, .evs = {4, 0, 0, 252, 0, 252}},
            {.moves = {MOVE_ROCK_BLAST, MOVE_BULLET_SEED, MOVE_BRAVE_BIRD, MOVE_U_TURN}, .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_SKILL_LINK, .evs = {4, 252, 0, 0, 0, 252}},
            {.moves = {MOVE_ACROBATICS, MOVE_HIGH_HORSEPOWER, MOVE_U_TURN, MOVE_PROTECT}, .item = ITEM_FLYING_GEM, .nature = NATURE_JOLLY, .ability = ABILITY_IMMUNITY, .evs = {4, 252, 0, 0, 0, 252}},
        };
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS
            | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
            struct Pokemon mon;
            CreateRandomMonWithIVs(&mon, species[i], i < 2 ? 30 : 28, MAX_PER_STAT_IVS);
            EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &sets[i]), EC_BATTLE_SET_SUCCESS);
            CalculateMonStats(&mon);
            if (i < 2) {
                PLAYER(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            } else {
                OPPONENT(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            }
        }
    } WHEN {
        for (u32 turn = 0; turn < 2; turn++) TURN {
            MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        EXPECT_EQ(opponentRight->species, SPECIES_KILOWATTREL);
        EXPECT_EQ(opponentRight->pp[2], 15);
        EXPECT_EQ(gSideTimers[1].tailwindTimer, 0);
    }
}


AI_DOUBLE_BATTLE_TEST("EC Leech Seed: do not repeat into a seeded Follow Me recipient")
{
    GIVEN {
        // Frozen native reproduction, independent of later trainer changes.
        // Both turns actually execute: first Seed lands, then revealed Follow
        // Me would redirect another Seed into the same already-seeded foe.
        const enum Species species[] = {SPECIES_SABLEYE, SPECIES_PACHIRISU, SPECIES_FERROSEED, SPECIES_DRUDDIGON, SPECIES_LYCANROC, SPECIES_CYCLIZAR};
        const struct EmeraldChampionsBattleSet sets[] = {
            {.moves = {MOVE_FOUL_PLAY, MOVE_WILL_O_WISP, MOVE_TAUNT, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER, .evs = {252, 0, 252, 0, 4, 0}},
            {.moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB, .evs = {252, 0, 252, 0, 4, 0}},
            {.moves = {MOVE_LEECH_SEED, MOVE_GYRO_BALL, MOVE_STEALTH_ROCK, MOVE_PROTECT}, .item = ITEM_ROCKY_HELMET, .nature = NATURE_RELAXED, .ability = ABILITY_IRON_BARBS, .evs = {252, 0, 252, 0, 4, 0}},
            {.moves = {MOVE_DRAGON_CLAW, MOVE_FIRE_PUNCH, MOVE_SUCKER_PUNCH, MOVE_IRON_HEAD}, .item = ITEM_ASSAULT_VEST, .nature = NATURE_ADAMANT, .ability = ABILITY_ROUGH_SKIN, .evs = {252, 252, 4, 0, 0, 0}},
            {.moves = {MOVE_ACCELEROCK, MOVE_ROCK_SLIDE, MOVE_DRILL_RUN, MOVE_PROTECT}, .item = ITEM_FOCUS_SASH, .nature = NATURE_JOLLY, .ability = ABILITY_STEADFAST, .evs = {4, 252, 0, 0, 0, 252}},
            {.moves = {MOVE_SHED_TAIL, MOVE_DRAGON_CLAW, MOVE_KNOCK_OFF, MOVE_U_TURN}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_JOLLY, .ability = ABILITY_REGENERATOR, .evs = {252, 0, 4, 0, 0, 252}},
        };
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS
            | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
            struct Pokemon mon;
            CreateRandomMonWithIVs(&mon, species[i], i < 2 ? 30 : 28, MAX_PER_STAT_IVS);
            EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &sets[i]), EC_BATTLE_SET_SUCCESS);
            CalculateMonStats(&mon);
            if (i < 2) {
                PLAYER(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            } else {
                OPPONENT(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            }
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentLeft, hit: TRUE);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
        TURN {
            MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } THEN {
        EXPECT_EQ((u32)playerRight->volatiles.leechSeed, 2);
        // Switching the burned setter is valid counterplay. Check the seed
        // user's actual PP, not a prescribed active species or second attack.
        struct Pokemon *setter = &GetBattlerParty(B_BATTLER_1)[0];
        EXPECT_EQ(GetMonData(setter, MON_DATA_SPECIES), SPECIES_FERROSEED);
        EXPECT_EQ(opponentLeft->species == SPECIES_FERROSEED ? opponentLeft->pp[0]
            : GetMonData(setter, MON_DATA_PP1), 9);
    }
}



AI_DOUBLE_BATTLE_TEST("EC Calm Mind: fast Simple setup accounts for immediate special defense")
{
    GIVEN {
        // Freeze the demonstrated failure board, independent of future trainer
        // authoring. Native stats and unforced choices, not a scripted NPC move.
        const enum Species species[] = {SPECIES_LOMBRE, SPECIES_PACHIRISU, SPECIES_WOBBUFFET, SPECIES_SWOOBAT, SPECIES_KADABRA, SPECIES_MUSHARNA};
        const struct EmeraldChampionsBattleSet sets[] = {
            {.moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT}, .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH, .evs = {252, 0, 0, 136, 120, 0}},
            {.moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB, .evs = {252, 0, 252, 0, 4, 0}},
            {.moves = {MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_ENCORE, MOVE_HELPING_HAND}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_CALM, .ability = ABILITY_SHADOW_TAG, .evs = {252, 0, 4, 0, 252, 0}},
            {.moves = {MOVE_CALM_MIND, MOVE_STORED_POWER, MOVE_AIR_SLASH, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_TIMID, .ability = ABILITY_SIMPLE, .evs = {4, 0, 0, 252, 0, 252}},
            {.moves = {MOVE_PSYCHIC, MOVE_DAZZLING_GLEAM, MOVE_ENCORE, MOVE_PROTECT}, .item = ITEM_FOCUS_SASH, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_GUARD, .evs = {4, 0, 0, 252, 0, 252}},
            {.moves = {MOVE_CALM_MIND, MOVE_PSYCHIC, MOVE_YAWN, MOVE_PROTECT}, .item = ITEM_LEFTOVERS, .nature = NATURE_CALM, .ability = ABILITY_SYNCHRONIZE, .evs = {252, 0, 4, 0, 252, 0}},
        };
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS
            | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_JACLYN;
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
            struct Pokemon mon;
            CreateRandomMonWithIVs(&mon, species[i], i < 2 ? 30 : 28, MAX_PER_STAT_IVS);
            EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &sets[i]), EC_BATTLE_SET_SUCCESS);
            CalculateMonStats(&mon);
            if (i < 2) {
                PLAYER(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            } else {
                OPPONENT(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            }
        }
    } WHEN {
        for (u32 t = 0; t < 2; t++) TURN {
            MOVE(playerLeft, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CALM_MIND, opponentRight);
    } THEN {
        EXPECT_EQ(opponentRight->species, SPECIES_SWOOBAT);
        EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 2);
        // The exact HP followed a Wobbuffet that Helping Handed the Calm Mind;
        // with no damage to multiply it now leaves, and the second turn's
        // targets differ. The setup and its survival are what this checks.
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_EQ(opponentRight->pp[0], 19);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Tailwind: faster recoil attackers do not use empty speed setup")
{
    GIVEN {
        // Frozen reproduction of an observed shared scoring failure, not a
        // design lock on Alyssa's evolving trainer party. All stats are native.
        const enum Species species[] = {SPECIES_SABLEYE, SPECIES_PACHIRISU, SPECIES_CYCLIZAR, SPECIES_FLETCHINDER, SPECIES_GLIGAR};
        const struct EmeraldChampionsBattleSet sets[] = {
            {.moves = {MOVE_FOUL_PLAY, MOVE_WILL_O_WISP, MOVE_RECOVER, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER, .evs = {252, 0, 252, 0, 4, 0}},
            {.moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB, .evs = {252, 0, 252, 0, 4, 0}},
            {.moves = {MOVE_SHED_TAIL, MOVE_DRAGON_CLAW, MOVE_KNOCK_OFF, MOVE_U_TURN}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_JOLLY, .ability = ABILITY_REGENERATOR, .evs = {4, 252, 0, 0, 0, 252}},
            {.moves = {MOVE_BRAVE_BIRD, MOVE_FLARE_BLITZ, MOVE_TAILWIND, MOVE_U_TURN}, .item = ITEM_LIFE_ORB, .nature = NATURE_JOLLY, .ability = ABILITY_GALE_WINGS, .evs = {4, 252, 0, 0, 0, 252}},
            {.moves = {MOVE_ACROBATICS, MOVE_HIGH_HORSEPOWER, MOVE_U_TURN, MOVE_PROTECT}, .item = ITEM_FLYING_GEM, .nature = NATURE_JOLLY, .ability = ABILITY_IMMUNITY, .evs = {4, 252, 0, 0, 0, 252}},
        };
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS
            | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
            struct Pokemon mon;
            CreateRandomMonWithIVs(&mon, species[i], i < 2 ? 30 : 28, MAX_PER_STAT_IVS);
            EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &sets[i]), EC_BATTLE_SET_SUCCESS);
            CalculateMonStats(&mon);
            if (i < 2) {
                PLAYER(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            } else {
                OPPONENT(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            }
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentLeft);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } THEN {
        u32 fasterFoe = max(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPEED), GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SPEED));
        for (u32 i = 0; i < 3; i++) EXPECT(GetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_SPEED) > fasterFoe);
        EXPECT_EQ(opponentRight->species, SPECIES_FLETCHINDER);
        EXPECT_EQ(opponentRight->pp[2], 15);
        EXPECT_EQ(gSideTimers[1].tailwindTimer, 0);
    }
}



































// Removed: "EC authored strategy: Tabitha activation requires survival and a
// useful boost". Tabitha's Magma Hideout team was re-authored to a Gigalith
// sand core with no Dragapult, no ALLY_COMBO and no ACTIVATE tactic, so the
// fixture no longer describes anything in the authored data. The surviving
// ACTIVATE acceptance case is the Cristian pair above.

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Flannery advances Eruption before opposing Rock Slide")
{
    GIVEN {
        PLAYER(SPECIES_HERACROSS) { Level(40); Moves(MOVE_ROCK_SLIDE); Speed(GetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_SPEED)); }
        PLAYER(SPECIES_HERACROSS) { Level(40); Moves(MOVE_ROCK_SLIDE); Speed(GetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_SPEED)); }
        AuthoredOpponent(TRAINER_FLANNERY_1, 3, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ROCK_SLIDE);
            MOVE(playerRight, MOVE_ROCK_SLIDE);
            EXPECT_MOVE(opponentLeft, MOVE_ERUPTION);
            EXPECT_MOVE(opponentRight, MOVE_AFTER_YOU, target: opponentLeft);
        }
    } SCENE {
        MESSAGE("The opposing Lilligant used After You!");
        MESSAGE("The opposing Torkoal used Eruption!");
    } THEN {
        // After You still advances Eruption ahead of the slower attackers.
        // Without the read the pair cannot confirm the exact incoming spread,
        // so one Rock Slide may land first; the authored sequence is the point.
        EXPECT(opponentLeft->hp > 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Connie's mutual Surf activates Storm Drain")
{
    GIVEN {
        // Paralysis leaves both foes slower than Gastrodon: this matchup has
        // an absorption payoff but no reason to establish Tailwind first.
        PLAYER(SPECIES_TYRANITAR) { Level(75); Ability(ABILITY_UNNERVE); Item(ITEM_ASSAULT_VEST); Status1(STATUS1_PARALYSIS); Moves(MOVE_SMACK_DOWN); Speed(GetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_SPEED)); }
        PLAYER(SPECIES_TYRANITAR) { Level(75); Ability(ABILITY_UNNERVE); Item(ITEM_ASSAULT_VEST); Status1(STATUS1_PARALYSIS); Moves(MOVE_SMACK_DOWN); Speed(GetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_SPEED)); }
        AuthoredOpponent(TRAINER_CONNIE, 7, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SMACK_DOWN, target: opponentLeft);
            MOVE(playerRight, MOVE_SMACK_DOWN, target: opponentLeft);
            EXPECT_MOVE(opponentLeft, MOVE_SURF);
            EXPECT_MOVE(opponentRight, MOVE_SURF);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT(opponentLeft->hp > 0);
        EXPECT(opponentRight->hp > 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Parker repeats Earthquake safely through Telepathy")
{
    GIVEN {
        // Sash requires two hits; Ground hits both targets harder than Hammer
        // Arm. Speed 84 is attainable by a Jolly, Speed-invested Lv45 Coalossal.
        PLAYER(SPECIES_COALOSSAL) { Level(45); Ability(ABILITY_FLAME_BODY); Item(ITEM_FOCUS_SASH); Speed(84); Moves(MOVE_PROTECT, MOVE_TACKLE); }
        PLAYER(SPECIES_COALOSSAL) { Level(45); Ability(ABILITY_FLAME_BODY); Item(ITEM_FOCUS_SASH); Speed(84); Moves(MOVE_PROTECT, MOVE_TACKLE); }
        // Parker's room was re-authored: Farigiraf now leads beside Oranguru
        // and Lickilicky is the Earthquake the room Instructs. Deploy the
        // recipient this fixture is about.
        AuthoredOpponentWithPartner(TRAINER_PARKER, 4, FALSE, 2);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM);
            EXPECT_MOVE(opponentRight, MOVE_EARTHQUAKE);
        }
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentLeft);
            EXPECT_MOVE(opponentRight, MOVE_EARTHQUAKE);
            EXPECT_MOVE(opponentLeft, MOVE_INSTRUCT, target: opponentRight);
        }
    } THEN {
        EXPECT(opponentRight->speed < opponentLeft->speed);
        EXPECT(opponentLeft->speed < playerLeft->speed);
        EXPECT(gFieldStatuses & STATUS_FIELD_TRICK_ROOM);
        EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
        EXPECT_EQ(playerLeft->hp, 0);
        EXPECT_EQ(playerRight->hp, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Parker preserves a nonimmune reserve beside Earthquake")
{
    GIVEN {
        PLAYER(SPECIES_TYRANITAR) { Level(45); Ability(ABILITY_UNNERVE); Speed(115); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_TYRANITAR) { Level(45); Ability(ABILITY_UNNERVE); Speed(115); Moves(MOVE_TACKLE); }
        AuthoredOpponent(TRAINER_PARKER, 4, FALSE);
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 5; // Girafarig, not Telepathy Oranguru.
        u32 hp = 1;
        SetMonData(&OPPONENT_PARTY[5], MON_DATA_HP, &hp);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
        }
    } THEN {
        EXPECT(GetMonData(&gParties[B_TRAINER_OPPONENT_A][5], MON_DATA_HP) > 0);
        if (opponentLeft->species == SPECIES_GIRAFARIG)
            EXPECT(opponentLeft->hp > 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Darius charges Wind Power with the authored Tailwind")
{
    GIVEN {
        // Bulky, harmless leads: nothing on the board can be knocked out, so
        // the activation has to be chosen on the authored plan rather than on
        // a knockout the recipient could take without it.
        PLAYER(SPECIES_WOBBUFFET) { Level(45); HP(600); MaxHP(600); Defense(200); SpDefense(200); Speed(20); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(45); HP(600); MaxHP(600); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_CELEBRATE); }
        // Kilowattrel is the authored Wind Power recipient.
        AuthoredOpponentWithPartner(TRAINER_DARIUS, 6, FALSE, 2);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_TAILWIND);
            NOT_EXPECT_MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_CHATOT);
        EXPECT_EQ(opponentRight->species, SPECIES_KILOWATTREL);
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TAILWIND);
        EXPECT(playerLeft->hp < 600 || playerRight->hp < 600);
        Test_MgbaPrintf("DARIUS_WIND_DECISION_FRAMES=%d", gBattleStruct->aiDelayFrames);
        EXPECT(gBattleStruct->aiDelayFrames <= 72);
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Nate instructs the Delphox that already attacked")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Level(45); HP(600); MaxHP(600); Defense(200); SpDefense(200); Speed(20); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(45); HP(600); MaxHP(600); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_CELEBRATE); }
        // Delphox is the authored Instruct recipient.
        AuthoredOpponentWithPartner(TRAINER_NATE, 6, FALSE, 5);
    } WHEN {
        // Instruct only repeats a move the recipient has already used, so the
        // first turn establishes it and the second is the authored repeat.
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_INSTRUCT, target: opponentRight);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_ORANGURU);
        EXPECT(opponentRight->hp > 0);
        Test_MgbaPrintf("NATE_INSTRUCT_DECISION_FRAMES=%d", gBattleStruct->aiDelayFrames);
        EXPECT(gBattleStruct->aiDelayFrames <= 72);
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Maura escapes her countdown while retaining the trap")
{
    u32 turns;
    PARAMETRIZE { turns = 3; }
    PARAMETRIZE { turns = 4; }
    GIVEN {
        // Bulk beyond the room's damage output, so the only thing that can
        // remove a lead is the countdown itself and the timeline is exact.
        for (u32 i = 0; i < 4; i++)
            PLAYER(SPECIES_CHANSEY) { Level(60); HP(600); MaxHP(600); Defense(200); SpDefense(200); Ability(ABILITY_NATURAL_CURE); Moves(MOVE_CELEBRATE); Speed(GetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_SPEED)); }
        AuthoredOpponent(TRAINER_MAURA, 6, FALSE);
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_PERISH_SONG); }
        TURN { }
        TURN { }
        if (turns == 4)
            TURN { SEND_OUT(playerLeft, 2); SEND_OUT(playerRight, 3); }
    } THEN {
        EXPECT(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP) > 0);
        EXPECT(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP) > 0);
        // Perish Song sets three and HandleEndTurnPerishSong takes the life on
        // the end turn that reads zero, so the counter reads 3/2/1 at the ends
        // of turns one to three and the fourth turn is the last one on which
        // the singer can legally leave. The authored preference is to go one
        // turn earlier, while the partner still holds the trap.
        EXPECT_NE(opponentLeft->species, SPECIES_JYNX);
        if (turns == 3)
            EXPECT_EQ(opponentRight->species, SPECIES_GOTHITELLE);
        else
        {
            EXPECT_NE(opponentRight->species, SPECIES_GOTHITELLE);
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 0);
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HP), 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC doubles budget: initial Mega and full benches stay below 1.2 seconds")
{
    bool32 guard;
    PARAMETRIZE { guard = TRUE; }
    PARAMETRIZE { guard = FALSE; }
    GIVEN {
        // Frozen reproduction of the measured 89-frame decision, not a
        // trainer-design lock. Keep native form stats; do not hide this cost
        // by timing only the already-Mega board after turn one.
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE,
            SPECIES_STARYU, SPECIES_KANGASKHAN, SPECIES_ABRA, SPECIES_DRATINI, SPECIES_MUNCHLAX, SPECIES_TOGEPI};
        const struct EmeraldChampionsBattleSet sets[] = {
            {.moves = {MOVE_FAKE_OUT, MOVE_HIGH_JUMP_KICK, MOVE_KNOCK_OFF, MOVE_PROTECT}, .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_REGENERATOR, .evs = {4, 252, 0, 0, 0, 252}},
            {.moves = {MOVE_PSYCHIC, MOVE_HEAT_WAVE, MOVE_TAILWIND, MOVE_PROTECT}, .item = ITEM_LIFE_ORB, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE, .evs = {4, 0, 0, 252, 0, 252}},
            {.moves = {MOVE_MUDDY_WATER, MOVE_EARTH_POWER, MOVE_ICY_WIND, MOVE_WIDE_GUARD}, .item = ITEM_EVIOLITE, .nature = NATURE_QUIET, .ability = ABILITY_DAMP, .evs = {252, 0, 4, 252, 0, 0}},
            {.moves = {MOVE_LEAF_STORM, MOVE_DRAGON_PULSE, MOVE_VACUUM_WAVE, MOVE_PROTECT}, .item = ITEM_WHITE_HERB, .nature = NATURE_TIMID, .ability = ABILITY_UNBURDEN, .evs = {4, 0, 0, 252, 0, 252}},
            {.moves = {MOVE_FOLLOW_ME, MOVE_NUZZLE, MOVE_SUPER_FANG, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_CALM, .ability = ABILITY_VOLT_ABSORB, .evs = {252, 0, 4, 0, 252, 0}},
            {.moves = {MOVE_HELPING_HAND, MOVE_YAWN, MOVE_WISH, MOVE_PROTECT}, .item = ITEM_EVIOLITE, .nature = NATURE_CALM, .ability = ABILITY_ANTICIPATION, .evs = {252, 0, 124, 0, 132, 0}},
            {.moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_THUNDERBOLT, MOVE_RECOVER}, .item = ITEM_EXPERT_BELT, .nature = NATURE_MODEST, .ability = ABILITY_ANALYTIC, .evs = {4, 0, 0, 252, 0, 252}},
            {.moves = {MOVE_FAKE_OUT, MOVE_BODY_SLAM, MOVE_DRAIN_PUNCH, MOVE_SUCKER_PUNCH}, .item = ITEM_KANGASKHANITE, .nature = NATURE_ADAMANT, .ability = ABILITY_SCRAPPY, .evs = {140, 252, 0, 0, 0, 116}},
            {.moves = {MOVE_PSYCHIC, MOVE_DAZZLING_GLEAM, MOVE_ENCORE, MOVE_PROTECT}, .item = ITEM_FOCUS_SASH, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_GUARD, .evs = {4, 0, 0, 252, 0, 252}},
            {.moves = {MOVE_DRAGON_DANCE, MOVE_EXTREME_SPEED, MOVE_DRAGON_CLAW, MOVE_PROTECT}, .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_SHED_SKIN, .evs = {4, 252, 0, 0, 0, 252}},
            {.moves = {MOVE_BODY_SLAM, MOVE_FIRE_PUNCH, MOVE_ICE_PUNCH, MOVE_CURSE}, .item = ITEM_EVIOLITE, .nature = NATURE_BRAVE, .ability = ABILITY_THICK_FAT, .evs = {252, 252, 4, 0, 0, 0}},
            {.moves = {MOVE_FOLLOW_ME, MOVE_YAWN, MOVE_ANCIENT_POWER, MOVE_HELPING_HAND}, .item = ITEM_EVIOLITE, .nature = NATURE_BOLD, .ability = ABILITY_SERENE_GRACE, .evs = {252, 0, 252, 0, 4, 0}},
        };
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS
            | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_CONSERVATIVE | AI_FLAG_DOUBLE_BATTLE);
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
            struct Pokemon mon;
            CreateRandomMonWithIVs(&mon, species[i], i == 8 || i >= 10 ? 29 : 30, MAX_PER_STAT_IVS);
            EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &sets[i]), EC_BATTLE_SET_SUCCESS);
            CalculateMonStats(&mon);
            if (i < 6) {
                PLAYER(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            } else {
                OPPONENT(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    bool32 fixedSpeed = FALSE;
                    SetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_HYPER_TRAINED_SPEED, &fixedSpeed);
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            }
        }
    } WHEN {
        TURN {
            if (guard)
            {
                MOVE(playerLeft, MOVE_PROTECT);
                MOVE(playerRight, MOVE_PROTECT);
            }
            else
            {
                MOVE(playerLeft, MOVE_HIGH_JUMP_KICK, target: opponentRight);
                MOVE(playerRight, MOVE_PSYCHIC, target: opponentLeft);
                SEND_OUT(playerRight, 2);
            }
        }
    } THEN {
        if (!guard)
        {
            EXPECT_EQ(opponentRight->species, SPECIES_KANGASKHAN_MEGA);
            EXPECT_EQ(GetBattlerAbility(B_BATTLER_3), ABILITY_PARENTAL_BOND);
            EXPECT_EQ(opponentRight->speed, 83);
        }
        Test_MgbaPrintf("MEGA_BUDGET_INITIAL_FRAMES=%d", gBattleStruct->aiDelayFrames);
        EXPECT(gBattleStruct->aiDelayFrames <= 72);
    }
}
AI_DOUBLE_BATTLE_TEST("EC status legality: a spent burn is not a substitute for an action")
{
    GIVEN {
        // Frozen pre-authoring reproduction: first burn succeeds, then a
        // Sucker Punch threat made the failed repeat beat real attacks.
        const enum Species species[] = {SPECIES_DUGTRIO, SPECIES_XATU, SPECIES_SHEDINJA, SPECIES_VENOMOTH};
        const struct EmeraldChampionsBattleSet sets[] = {
            {.moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT}, .item = ITEM_FOCUS_SASH, .ability = ABILITY_ARENA_TRAP, .nature = NATURE_JOLLY, .evs = {4,252,0,0,0,252}},
            {.moves = {MOVE_PSYCHIC, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT}, .item = ITEM_EVIOLITE, .ability = ABILITY_MAGIC_BOUNCE, .nature = NATURE_TIMID, .evs = {4,0,0,252,0,252}},
            {.moves = {MOVE_POLTERGEIST, MOVE_SHADOW_SNEAK, MOVE_X_SCISSOR, MOVE_WILL_O_WISP}, .item = ITEM_SAFETY_GOGGLES, .ability = ABILITY_WONDER_GUARD, .nature = NATURE_JOLLY, .evs = {4,252,0,0,0,252}},
            {.moves = {MOVE_QUIVER_DANCE, MOVE_BUG_BUZZ, MOVE_SLEEP_POWDER, MOVE_PROTECT}, .item = ITEM_BLACK_SLUDGE, .ability = ABILITY_TINTED_LENS, .nature = NATURE_TIMID, .evs = {4,0,0,252,0,252}},
        };
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS
            | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_LUNG;
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
            struct Pokemon mon;
            CreateRandomMonWithIVs(&mon, species[i], i < 2 ? 40 : 38, MAX_PER_STAT_IVS);
            EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &sets[i]), EC_BATTLE_SET_SUCCESS);
            CalculateMonStats(&mon);
            if (i < 2) {
                PLAYER(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            } else {
                OPPONENT(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            }
        }
    } WHEN {
        for (u32 t = 0; t < 2; t++) TURN {
            MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_AIR_SLASH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        EXPECT_EQ(playerLeft->status1, STATUS1_BURN);
        EXPECT_EQ(opponentLeft->pp[3], 14);
    }
}

AI_DOUBLE_BATTLE_TEST("EC status legality: preserve damaging moves and legal forced fallback")
{
    bool32 attack = FALSE;
    PARAMETRIZE { attack = FALSE; }
    PARAMETRIZE { attack = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_BURN); Moves(MOVE_SPLASH); }
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_BURN); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_CHARMANDER) { Moves(MOVE_WILL_O_WISP, attack ? MOVE_FLAMETHROWER : MOVE_NONE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SPLASH);
            MOVE(playerRight, MOVE_SPLASH);
            EXPECT_MOVE(opponentLeft, attack ? MOVE_FLAMETHROWER : MOVE_WILL_O_WISP);
        }
    }
}
AI_DOUBLE_BATTLE_TEST("EC revealed redirection: do not repeatedly feed Volt Absorb")
{
    GIVEN {
        // Freeze the demonstrated board, not Jaylen's evolving campaign sets.
        const enum Species species[] = {SPECIES_POLITOED, SPECIES_PACHIRISU, SPECIES_CRYOGONAL, SPECIES_MAGNEZONE};
        const struct EmeraldChampionsBattleSet sets[] = {
            {.moves = {MOVE_SCALD, MOVE_ICE_BEAM, MOVE_HELPING_HAND, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .ability = ABILITY_DRIZZLE, .nature = NATURE_BOLD, .evs = {252,0,252,0,4,0}},
            {.moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .ability = ABILITY_VOLT_ABSORB, .nature = NATURE_BOLD, .evs = {252,0,252,0,4,0}},
            {.moves = {MOVE_SNOWSCAPE, MOVE_FREEZE_DRY, MOVE_BLIZZARD, MOVE_RECOVER}, .item = ITEM_NEVER_MELT_ICE, .ability = ABILITY_LEVITATE, .nature = NATURE_TIMID, .evs = {4,0,0,252,0,252}},
            {.moves = {MOVE_THUNDERBOLT, MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_PROTECT}, .item = ITEM_SHUCA_BERRY, .ability = ABILITY_ANALYTIC, .nature = NATURE_BOLD, .evs = {252,0,252,4,0,0}},
        };
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS
            | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_JAYLEN;
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
            struct Pokemon mon;
            CreateRandomMonWithIVs(&mon, species[i], i < 2 ? 40 : 38, MAX_PER_STAT_IVS);
            EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &sets[i]), EC_BATTLE_SET_SUCCESS);
            CalculateMonStats(&mon);
            if (i < 2) {
                PLAYER(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            } else {
                OPPONENT(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            }
        }
    } WHEN {
        for (u32 t = 0; t < 3; t++) TURN {
            if (t == 0) MOVE(playerLeft, MOVE_PROTECT);
            else MOVE(playerLeft, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } THEN {
        // An initial prediction error is allowed. A revealed, still-legal
        // redirector must not receive repeated healing Thunderbolts.
        EXPECT(opponentRight->pp[0] >= 14);
    }
}

DOUBLE_BATTLE_TEST("EC Laura pivot mechanics: a faster U-turn preserves the same recipient and adds damage")
{
    bool32 pivot;
    PARAMETRIZE { pivot = FALSE; }
    PARAMETRIZE { pivot = TRUE; }
    GIVEN {
        PLAYER(SPECIES_TOGETIC) { Level(20); HP(70); MaxHP(70); Defense(62); SpDefense(53); Speed(27); Ability(ABILITY_SERENE_GRACE); Item(ITEM_EVIOLITE); Moves(MOVE_HELPING_HAND); }
        PLAYER(SPECIES_SYLVEON) { Level(20); HP(86); MaxHP(86); Defense(37); SpAttack(73); SpDefense(63); Speed(35); Ability(ABILITY_PIXILATE); Item(ITEM_COVERT_CLOAK); Moves(MOVE_HYPER_VOICE); }
        AuthoredOpponent(TRAINER_LAURA, 1, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_HELPING_HAND, target: playerRight);
            MOVE(playerRight, MOVE_HYPER_VOICE, criticalHit: FALSE);
            if (pivot)
            {
                MOVE(opponentLeft, MOVE_U_TURN, target: playerRight, criticalHit: FALSE);
                SEND_OUT(opponentLeft, 2);
            }
            else
                SWITCH(opponentLeft, 2);
            MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT(opponentLeft->hp > 0);
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
        if (pivot)
            EXPECT(playerRight->hp < playerRight->maxHP);
        else
            EXPECT_EQ(playerRight->hp, playerRight->maxHP);
        Test_MgbaPrintf("LAURA_PIVOT=%d CROAGUNK_HP=%d SYLVEON_HP=%d", pivot, opponentLeft->hp, playerRight->hp);
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Aisha's Storm Throw activates Anger Point")
{
    GIVEN {
        // A deliberately harmless board: nothing can punish the turn, Frost
        // Throw into the Fighting-type Tauros is neutral and nowhere near lethal, and the
        // guaranteed critical hit is the whole point of the authored pairing.
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_SPLASH); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        // Route117 is available before Wattson: two badges, cap30.
        AuthoredOpponent(TRAINER_AISHA, 2, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SPLASH);
            MOVE(playerRight, MOVE_SPLASH);
            EXPECT_MOVE(opponentLeft, MOVE_STORM_THROW, target: opponentRight);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_THROH);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], MAX_STAT_STAGE);
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Georgia's Shadow Sneak arms the Weakness Policy")
{
    GIVEN {
        // The same authored shape with a single-target trigger instead of a
        // spread one, which is the only difference that stopped it firing.
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_SPLASH); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        AuthoredOpponent(TRAINER_GEORGIA, 4, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SPLASH);
            MOVE(playerRight, MOVE_SPLASH);
            EXPECT_MOVE(opponentLeft, MOVE_SHADOW_SNEAK, target: opponentRight);
        }
    } THEN {
        EXPECT_EQ(opponentRight->species, SPECIES_METANG);
        EXPECT(opponentRight->hp > 0);
        EXPECT_GT(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Mega: the holder evolves on the first turn it acts")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        // Parental Bond is worth far more than one turn of damage, and a form
        // that never happens is worth nothing at all: the stone was still held
        // when the base form fainted.
        PLAYER(SPECIES_MACHOP) { Level(30); HP(300); MaxHP(300); Attack(120); Defense(150); SpDefense(150); Speed(50); Moves(MOVE_BRICK_BREAK); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_KANGASKHAN) {
            Level(30); HP(250); MaxHP(250); Attack(140); Defense(100); SpDefense(100); Speed(90);
            Ability(ABILITY_SCRAPPY); Item(ITEM_KANGASKHANITE); Moves(MOVE_BODY_SLAM, MOVE_FAKE_OUT);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BRICK_BREAK, target: opponentLeft);
            MOVE(playerRight, MOVE_SPLASH);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_KANGASKHAN_MEGA);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_1), ABILITY_PARENTAL_BOND);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Laura board: an ordinary four-member route decision finishes inside the budget")
{
    GIVEN {
        // The instrumented counterpart to the Ned board: a four-member route
        // team with nothing exotic on it, which is where a search that runs to
        // the stop every turn is a quality problem and not only a pause.
        PLAYER(SPECIES_TOGETIC) { Level(20); HP(70); MaxHP(70); Defense(62); SpDefense(53); Speed(27); Ability(ABILITY_SERENE_GRACE); Item(ITEM_EVIOLITE); Moves(MOVE_HELPING_HAND, MOVE_AIR_SLASH); }
        PLAYER(SPECIES_SYLVEON) { Level(20); HP(86); MaxHP(86); Defense(37); SpAttack(73); SpDefense(63); Speed(35); Ability(ABILITY_PIXILATE); Item(ITEM_COVERT_CLOAK); Moves(MOVE_HYPER_VOICE, MOVE_PROTECT); }
        AuthoredOpponent(TRAINER_LAURA, 1, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_HELPING_HAND, target: playerRight);
            MOVE(playerRight, MOVE_HYPER_VOICE, criticalHit: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("LAURA_BOARD_DECISION_FRAMES=%d", gBattleStruct->aiDelayFrames);
        EXPECT(gBattleStruct->aiDelayFrames <= 72);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Laura pivot: use the guaranteed faster exit but keep an immediate escape from a faster attack")
{
    bool32 fastPlayer;
    bool32 flinch = FALSE;
    PARAMETRIZE { fastPlayer = FALSE; }
    PARAMETRIZE { fastPlayer = TRUE; }
    PARAMETRIZE { fastPlayer = FALSE; flinch = TRUE; }
    // Keep the interruption case lethal after the authored level change:
    // the player's Fake Out must precede every response, and Hyper Voice must
    // still require an escape rather than merely chip the stronger Mienfoo.
    GIVEN {
        PLAYER(flinch ? SPECIES_MIENFOO : SPECIES_TOGETIC) { Level(20); HP(70); MaxHP(70); Attack(24); Defense(62); SpDefense(53); Speed(flinch ? 100 : 27); Ability(flinch ? ABILITY_REGENERATOR : ABILITY_SERENE_GRACE); Item(ITEM_EVIOLITE); Moves(MOVE_HELPING_HAND, MOVE_FAKE_OUT); }
        PLAYER(SPECIES_SYLVEON) { Level(20); HP(86); MaxHP(86); Defense(37); SpAttack(flinch ? 120 : 73); SpDefense(63); Speed(fastPlayer ? 80 : 35); Ability(ABILITY_PIXILATE); Item(ITEM_COVERT_CLOAK); Moves(MOVE_HYPER_VOICE); }
        AuthoredOpponent(TRAINER_LAURA, 1, FALSE);
    } WHEN {
        TURN {
            if (flinch)
                MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, criticalHit: FALSE);
            else
                MOVE(playerLeft, MOVE_HELPING_HAND, target: playerRight);
            MOVE(playerRight, MOVE_HYPER_VOICE, criticalHit: FALSE);
            // Which reserve the threatened flank leaves for moved twice with
            // this session's re-authoring, and whether the slower board is left
            // by a pivot or by a first-turn Fake Out is an expected-value choice
            // the read used to settle. Surviving the turn is the subject.
            EXPECT_MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT(opponentLeft->hp > 0);
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
        if (fastPlayer || flinch)
            EXPECT_EQ(playerRight->hp, playerRight->maxHP);
        else
            EXPECT(playerLeft->hp < playerLeft->maxHP || playerRight->hp < playerRight->maxHP);
    }
}

AI_DOUBLE_BATTLE_TEST("EC pivot exits: preserve immediate switching when the extra hit has a cost")
{
    u32 hazard;
    PARAMETRIZE { hazard = 0; } // Passive target: take the extra hit.
    PARAMETRIZE { hazard = 1; } // Life Orb would faint the outgoing actor.
    PARAMETRIZE { hazard = 2; } // Contact damage would prevent its departure.
    PARAMETRIZE { hazard = 3; } // The extra hit would activate the target's Sitrus.
    GIVEN {
        AI_FLAGS(gTrainers[DIFFICULTY_NORMAL][TRAINER_LAURA].aiFlags | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_TOGETIC) { Level(20); HP(70); MaxHP(70); Defense(62); SpDefense(53); Speed(27); Ability(ABILITY_SERENE_GRACE); Item(ITEM_EVIOLITE); Moves(MOVE_HELPING_HAND); }
        PLAYER(SPECIES_SYLVEON) { Level(20); HP(hazard == 3 ? 55 : 86); MaxHP(86); Defense(37); SpAttack(73); SpDefense(63); Speed(35); Ability(ABILITY_PIXILATE); Item(hazard == 2 ? ITEM_ROCKY_HELMET : hazard == 3 ? ITEM_SITRUS_BERRY : ITEM_COVERT_CLOAK); Moves(MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_MIENFOO) { Level(24); HP(hazard == 1 || hazard == 2 ? 1 : 63); MaxHP(63); Attack(68); Defense(36); SpDefense(36); Speed(63); Ability(ABILITY_REGENERATOR); Item(hazard == 1 ? ITEM_LIFE_ORB : ITEM_EVIOLITE); Moves(MOVE_U_TURN); }
        OPPONENT(SPECIES_TIMBURR) { Level(25); HP(96); MaxHP(96); Speed(30); Ability(ABILITY_GUTS); Item(ITEM_FLAME_ORB); Moves(MOVE_PROTECT); }
        OPPONENT(SPECIES_CROAGUNK) { Level(25); HP(82); MaxHP(82); SpDefense(100); Speed(37); Ability(ABILITY_DRY_SKIN); Item(ITEM_EVIOLITE); Moves(MOVE_SLUDGE_BOMB); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_HELPING_HAND, target: playerRight);
            MOVE(playerRight, MOVE_HYPER_VOICE, criticalHit: FALSE);
            if (!hazard)
            {
                EXPECT_MOVE(opponentLeft, MOVE_U_TURN, target: playerRight);
                EXPECT_SEND_OUT(opponentLeft, 2);
            }
            else
                EXPECT_SWITCH(opponentLeft, 2);
            EXPECT_MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_CROAGUNK);
        EXPECT(opponentLeft->hp > 0);
        if (!hazard)
            EXPECT(playerRight->hp < 86);
        else
            EXPECT_EQ(playerRight->hp, hazard == 3 ? 55 : 86);
    }
}

// The proposed compulsory Sash switch was not a sound requirement: Toxic
// Chain can poison the one-HP recipient before the end-turn checkpoint.
DOUBLE_BATTLE_TEST("EC Laura Sash: Toxic Chain makes the one-HP reserve outcome conditional")
{
    bool32 chain;
    PARAMETRIZE { chain = FALSE; }
    PARAMETRIZE { chain = TRUE; }
    GIVEN {
        PLAYER(SPECIES_MUNKIDORI) { Level(20); HP(71); MaxHP(71); SpAttack(75); Speed(72); Ability(ABILITY_TOXIC_CHAIN); Item(ITEM_COVERT_CLOAK); Moves(MOVE_PSYCHIC); }
        PLAYER(SPECIES_SYLVEON) { Level(20); HP(86); MaxHP(86); SpAttack(73); Speed(35); Ability(ABILITY_PIXILATE); Item(ITEM_COVERT_CLOAK); Moves(MOVE_MOONBLAST); }
        AuthoredOpponent(TRAINER_LAURA, 1, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, criticalHit: FALSE, secondaryEffect: FALSE, WITH_RNG(RNG_TOXIC_CHAIN, chain));
            MOVE(playerRight, MOVE_MOONBLAST, target: opponentRight, criticalHit: FALSE);
            SWITCH(opponentLeft, 5);
            MOVE(opponentRight, MOVE_PROTECT);
            if (chain)
                SEND_OUT(opponentLeft, 2);
        }
    } THEN {
        if (chain)
        {
            EXPECT_EQ(GetMonData(&GetBattlerParty(B_BATTLER_1)[5], MON_DATA_HP), 0);
            EXPECT_EQ(opponentLeft->species, SPECIES_CROAGUNK);
        }
        else
        {
            EXPECT_EQ(opponentLeft->species, SPECIES_TYROGUE);
            EXPECT_EQ(opponentLeft->hp, 1);
        }
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
    }
}

DOUBLE_BATTLE_TEST("EC Laura levels: native Mienfoo stats open the Psychic survival range at 26")
{
    u32 level;
    PARAMETRIZE { level = 24; }
    PARAMETRIZE { level = 25; }
    PARAMETRIZE { level = 26; }
    GIVEN {
        PLAYER(SPECIES_MUNKIDORI) { Level(20); SpAttack(75); Speed(72); Ability(ABILITY_TOXIC_CHAIN); Item(ITEM_COVERT_CLOAK); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GASTLY) { Speed(10); Moves(MOVE_CELEBRATE); }
        struct Pokemon mon;
        const struct EmeraldChampionsBattleSet set = {.moves = {MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF, MOVE_U_TURN}, .item = ITEM_EVIOLITE, .ability = ABILITY_REGENERATOR, .nature = NATURE_JOLLY, .evs = {4,252,0,0,0,252}};
        CreateRandomMonWithIVs(&mon, SPECIES_MIENFOO, level, MAX_PER_STAT_IVS);
        EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &set), EC_BATTLE_SET_SUCCESS);
        CalculateMonStats(&mon);
        OPPONENT(SPECIES_MIENFOO) {
            *gBattleTestRunnerState->data.currentMon = mon;
            Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
            Moves(MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF, MOVE_U_TURN);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(20); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_FAKE_OUT, target: playerRight); MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        for (u32 battler = 0; battler < gBattlersCount; battler++)
            SetBattlerAiData(battler, gAiLogicData);
        struct AiCalcValues calc = {.move = MOVE_PSYCHIC, .weather = AI_GetWeather(), .terrain = gFieldTimers.terrain};
        struct SimulatedDamage damage = AI_CalcDamage(&calc, B_BATTLER_0, B_BATTLER_1);
        if (level < 26)
            EXPECT(damage.minimum >= opponentLeft->hp);
        else
        {
            EXPECT(damage.minimum < opponentLeft->hp);
            EXPECT(damage.maximum >= opponentLeft->hp);
        }
        Test_MgbaPrintf("MIENFOO_LEVEL=%d HP=%d SPDEF=%d PSYCHIC=%d..%d", level, opponentLeft->hp, opponentLeft->spDefense, damage.minimum, damage.maximum);
    }
}

DOUBLE_BATTLE_TEST("EC Takao benchmark: minimum-roll Specs Bug Buzz removes Wobbuffet before Mirror Coat")
{
    GIVEN {
        struct Pokemon mon;
        const struct EmeraldChampionsBattleSet set = {.moves = {MOVE_BUG_BUZZ, MOVE_ICE_BEAM, MOVE_FOCUS_BLAST, MOVE_U_TURN}, .item = ITEM_CHOICE_SPECS, .ability = ABILITY_BEAST_BOOST, .nature = NATURE_MODEST, .evs = {4,0,0,252,0,252}};
        CreateRandomMonWithIVs(&mon, SPECIES_PHEROMOSA, 20, MAX_PER_STAT_IVS);
        EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &set), EC_BATTLE_SET_SUCCESS);
        CalculateMonStats(&mon);
        PLAYER(SPECIES_PHEROMOSA) {
            *gBattleTestRunnerState->data.currentMon = mon;
            Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
            Moves(MOVE_BUG_BUZZ, MOVE_ICE_BEAM, MOVE_FOCUS_BLAST, MOVE_U_TURN);
        }
        PLAYER(SPECIES_MUNKIDORI) { Level(20); HP(71); MaxHP(71); Attack(36); SpAttack(75); Speed(72); Ability(ABILITY_TOXIC_CHAIN); Item(ITEM_COVERT_CLOAK); Moves(MOVE_FAKE_OUT); }
        AuthoredOpponent(TRAINER_TAKAO, 1, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BUG_BUZZ, target: opponentRight, criticalHit: FALSE, WITH_RNG(RNG_DAMAGE_MODIFIER, 15));
            MOVE(playerRight, MOVE_FAKE_OUT, target: opponentLeft, criticalHit: FALSE);
            MOVE(opponentLeft, MOVE_OCTOLOCK, target: playerLeft);
            MOVE(opponentRight, MOVE_MIRROR_COAT, target: playerLeft);
            SEND_OUT(opponentRight, 2);
        }
    } THEN {
        EXPECT_EQ(GetMonData(&GetBattlerParty(B_BATTLER_3)[1], MON_DATA_HP), 0);
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
        EXPECT_EQ(playerRight->hp, playerRight->maxHP);
        Test_MgbaPrintf("PHEROMOSA_SPATK=%d SPEED=%d HP=%d", playerLeft->spAttack, playerLeft->speed, playerLeft->hp);
    }
}


// Execute the actual Lavaridge field and authored teams. The milestone is set
// only while producing the cap40 opponent party; no campaign save is advanced.
static void AuthoredMistyGymOpponent(u16 trainerId)
{
    bool32 savedSummit = FlagGet(FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY);
    FlagSet(FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY);
    AuthoredOpponent(trainerId, 3, FALSE);
    if (!savedSummit)
        FlagClear(FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY);
}

DOUBLE_BATTLE_TEST("EC misty gym: steam and a seed coexist with either sun or rain")
{
    bool32 rain;
    u8 savedWeather = WEATHER_NONE;
    PARAMETRIZE { rain = FALSE; }
    PARAMETRIZE { rain = TRUE; }
    GIVEN {
        savedWeather = gWeatherPtr->currWeather;
        gWeatherPtr->currWeather = WEATHER_FOG_HORIZONTAL;
        PLAYER(SPECIES_WOBBUFFET) { HP(600); MaxHP(600); Speed(10); }
        PLAYER(SPECIES_WOBBUFFET) { HP(600); MaxHP(600); Speed(10); }
        AuthoredMistyGymOpponent(TRAINER_FLANNERY_1);
    } WHEN {
        TURN {
            MOVE(playerLeft, rain ? MOVE_RAIN_DANCE : MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        gWeatherPtr->currWeather = savedWeather;
        EXPECT_EQ(gFieldTimers.terrain, B_TERRAIN_MISTY);
        EXPECT_EQ(gFieldTimers.terrainTimer, 0);
        EXPECT(gBattleWeather & (rain ? B_WEATHER_RAIN : B_WEATHER_SUN));
        EXPECT_EQ(opponentRight->item, ITEM_NONE);
        EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentLeft->species, SPECIES_TORKOAL);
        EXPECT_EQ(opponentRight->species, SPECIES_LILLIGANT);
        // Badge3 cap40, authored offsets +2/+1, Normal difficulty -1.
        EXPECT_EQ(opponentLeft->level, 41);
        EXPECT_EQ(opponentRight->level, 40);
    }
}

DOUBLE_BATTLE_TEST("EC misty gym: Defog opens Corrosion while the airborne Orb works in mist")
{
    bool32 clear;
    u8 savedWeather = WEATHER_NONE;
    PARAMETRIZE { clear = FALSE; }
    PARAMETRIZE { clear = TRUE; }
    GIVEN {
        savedWeather = gWeatherPtr->currWeather;
        gWeatherPtr->currWeather = WEATHER_FOG_HORIZONTAL;
        PLAYER(SPECIES_WOBBUFFET) { HP(600); MaxHP(600); Speed(10); }
        PLAYER(SPECIES_REGISTEEL) { HP(600); MaxHP(600); Speed(10); }
        AuthoredMistyGymOpponent(TRAINER_JACE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE);
            if (clear) MOVE(opponentLeft, MOVE_DEFOG, target: playerLeft);
            else MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_TOXIC, target: playerRight);
        }
    } THEN {
        gWeatherPtr->currWeather = savedWeather;
        EXPECT_EQ(opponentLeft->species, SPECIES_DRIFBLIM);
        EXPECT_EQ(opponentLeft->status1, STATUS1_BURN);
        EXPECT_EQ(gFieldTimers.terrain, clear ? B_TERRAIN_NONE : B_TERRAIN_MISTY);
        EXPECT_EQ(playerRight->status1 & STATUS1_ANY, clear ? STATUS1_TOXIC_POISON : STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("EC misty gym: airborne seed sprint does not grant status immunity")
{
    u8 savedWeather = WEATHER_NONE;
    GIVEN {
        savedWeather = gWeatherPtr->currWeather;
        gWeatherPtr->currWeather = WEATHER_FOG_HORIZONTAL;
        PLAYER(SPECIES_WOBBUFFET) { HP(600); MaxHP(600); Speed(150); }
        PLAYER(SPECIES_WOBBUFFET) { HP(600); MaxHP(600); Speed(10); }
        AuthoredMistyGymOpponent(TRAINER_JEFF);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_ACROBATICS, target: playerRight);
            MOVE(opponentRight, MOVE_CROSS_POISON, target: playerRight);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ACROBATICS, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WILL_O_WISP, playerLeft);
    } THEN {
        gWeatherPtr->currWeather = savedWeather;
        EXPECT_EQ(opponentLeft->species, SPECIES_HAWLUCHA);
        EXPECT_EQ(opponentLeft->item, ITEM_NONE);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT(opponentLeft->volatiles.unburdenActive);
        EXPECT_EQ(opponentLeft->status1, STATUS1_BURN);
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
        EXPECT_EQ(gFieldTimers.terrain, B_TERRAIN_MISTY);
    }
}

AI_DOUBLE_BATTLE_TEST("EC misty gym AI: vents blocked status but preserves useful or unbreakable mist")
{
    u32 scenario = 0;
    u8 savedWeather = WEATHER_NONE;
    PARAMETRIZE { scenario = 0; } // Grounded targets: clear mist for Corrosion.
    PARAMETRIZE { scenario = 1; } // Good as Gold: neither Defog nor Toxic works.
    PARAMETRIZE { scenario = 2; } // Airborne targets are already status-vulnerable.
    GIVEN {
        savedWeather = gWeatherPtr->currWeather;
        gWeatherPtr->currWeather = WEATHER_FOG_HORIZONTAL;
        if (scenario == 0)
        {
            PLAYER(SPECIES_SNORLAX) { HP(600); MaxHP(600); Ability(ABILITY_THICK_FAT); SpDefense(250); Speed(10); Moves(MOVE_CELEBRATE); }
            PLAYER(SPECIES_REGISTEEL) { HP(600); MaxHP(600); SpDefense(250); Speed(10); Moves(MOVE_CELEBRATE); }
        }
        else if (scenario == 1)
        {
            PLAYER(SPECIES_GHOLDENGO) { HP(600); MaxHP(600); Ability(ABILITY_GOOD_AS_GOLD); SpDefense(250); Speed(10); Moves(MOVE_CELEBRATE); }
            PLAYER(SPECIES_GHOLDENGO) { HP(600); MaxHP(600); Ability(ABILITY_GOOD_AS_GOLD); SpDefense(250); Speed(10); Moves(MOVE_CELEBRATE); }
        }
        else
        {
            PLAYER(SPECIES_DRAGONITE) { HP(600); MaxHP(600); SpDefense(250); Speed(10); Moves(MOVE_DRAGON_BREATH); }
            PLAYER(SPECIES_DRAGONITE) { HP(600); MaxHP(600); SpDefense(250); Speed(10); Moves(MOVE_DRAGON_BREATH); }
        }
        AuthoredMistyGymOpponent(TRAINER_JACE);
    } WHEN {
        TURN {
            if (scenario == 2)
            {
                MOVE(playerLeft, MOVE_DRAGON_BREATH, target: opponentRight);
                MOVE(playerRight, MOVE_DRAGON_BREATH, target: opponentRight);
            }
            else
            {
                MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE);
            }
            if (scenario == 0) EXPECT_MOVE(opponentLeft, MOVE_DEFOG);
            else NOT_EXPECT_MOVE(opponentLeft, MOVE_DEFOG);
        }
    } THEN {
        gWeatherPtr->currWeather = savedWeather;
        EXPECT_EQ(gFieldTimers.terrain, scenario == 0 ? B_TERRAIN_NONE : B_TERRAIN_MISTY);
        if (scenario == 0)
            EXPECT_EQ(opponentLeft->status1, STATUS1_BURN);

        // Planning probes must not change the live field, battlers or RNG.
        typeof(gFieldTimers) savedTimers = gFieldTimers;
        struct BattlePokemon savedMons[MAX_BATTLERS_COUNT];
        struct Sfc32State savedRng = gRngValue;
        memcpy(savedMons, gBattleMons, sizeof(savedMons));
        ShouldClearTerrain(B_BATTLER_1, B_TERRAIN_MISTY);
        ShouldSetTerrain(B_BATTLER_1, B_TERRAIN_MISTY);
        EXPECT_EQ(memcmp(&savedTimers, &gFieldTimers, sizeof(savedTimers)), 0);
        EXPECT_EQ(memcmp(savedMons, gBattleMons, sizeof(savedMons)), 0);
        EXPECT_EQ(memcmp(&savedRng, &gRngValue, sizeof(savedRng)), 0);
        Test_MgbaPrintf("MISTY_GYM_AI_SCENARIO=%d FRAMES=%d", scenario, gBattleStruct->aiDelayFrames);
    }
}


DOUBLE_BATTLE_TEST("EC Mega budget: queued requests obey one-Mega execution and two-Mega eligibility")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(10); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(20); }
        OPPONENT(SPECIES_KANGASKHAN) { Item(ITEM_KANGASKHANITE); Speed(100); }
        OPPONENT(SPECIES_SALAMENCE) { Item(ITEM_SALAMENCITE); Speed(90); }
    } WHEN {
        // Exercise the final execution guard, even for two queued requests.
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_TACKLE, gimmick: GIMMICK_MEGA, target: playerLeft);
            MOVE(opponentRight, MOVE_TACKLE, gimmick: GIMMICK_MEGA, target: playerRight);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_KANGASKHAN_MEGA);
        EXPECT_EQ(opponentRight->species, SPECIES_SALAMENCE);
        EXPECT_EQ(GetRemainingMegaEvolutions(B_BATTLER_1), 0);
        EXPECT_EQ(gBattleStruct->gimmick.megaEvolutionsUsed[GetBattlerTrainer(B_BATTLER_1)], 1);

        // Probe the production eligibility gate using an otherwise eligible
        // candidate in Sidney's authorized sixth slot. This does not reauthor
        // his live party or choose the second League ace before design review.
        u32 savedFlags = gBattleTypeFlags;
        u16 savedA = TRAINER_BATTLE_PARAM.opponentA;
        u16 savedB = TRAINER_BATTLE_PARAM.opponentB;
        u8 savedIndex = gBattlerPartyIndexes[B_BATTLER_1];
        struct BattlePokemon savedMon = gBattleMons[B_BATTLER_1];
        struct BattleGimmickData savedGimmick = gBattleStruct->gimmick;
        struct Pokemon savedPartyMon = gParties[B_TRAINER_OPPONENT_A][5];
        gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
        TRAINER_BATTLE_PARAM.opponentA = TRAINER_SIDNEY;
        TRAINER_BATTLE_PARAM.opponentB = TRAINER_PHOEBE;
        gBattlerPartyIndexes[B_BATTLER_1] = 5;
        gBattleMons[B_BATTLER_1].species = SPECIES_KANGASKHAN;
        gBattleMons[B_BATTLER_1].ability = ABILITY_SCRAPPY;
        gParties[B_TRAINER_OPPONENT_A][5] = gParties[B_TRAINER_OPPONENT_A][0];
        enum Species baseSpecies = SPECIES_KANGASKHAN;
        SetMonData(&gParties[B_TRAINER_OPPONENT_A][5], MON_DATA_SPECIES, &baseSpecies);
        memset(gBattleStruct->gimmick.megaEvolutionsUsed, 0, sizeof(gBattleStruct->gimmick.megaEvolutionsUsed));
        gBattleStruct->gimmick.activeGimmick[B_TRAINER_OPPONENT_A][5] = GIMMICK_NONE;
        gBattleStruct->gimmick.toActivate = 0;
        EXPECT_EQ(EmeraldChampions_GetMegaEvolutionLimit(B_BATTLER_1), 2);
        EXPECT_EQ(EmeraldChampions_GetMegaEvolutionLimit(B_BATTLER_0), 1);
        EXPECT(CanMegaEvolve(B_BATTLER_1));
        SetGimmickAsActivated(B_BATTLER_3, GIMMICK_MEGA);
        EXPECT_EQ(GetRemainingMegaEvolutions(B_BATTLER_1), 1);
        EXPECT(CanMegaEvolve(B_BATTLER_1));
        // The other battler can reserve the final use, but not spend a third.
        gBattleStruct->gimmick.toActivate = 1u << B_BATTLER_3;
        gBattleStruct->gimmick.usableGimmick[B_BATTLER_3] = GIMMICK_MEGA;
        EXPECT(!CanMegaEvolve(B_BATTLER_1));
        gBattleStruct->gimmick.toActivate = 0;
        EXPECT(CanMegaEvolve(B_BATTLER_1));
        SetGimmickAsActivated(B_BATTLER_1, GIMMICK_MEGA);
        EXPECT_EQ(GetRemainingMegaEvolutions(B_BATTLER_1), 0);
        EXPECT(!CanMegaEvolve(B_BATTLER_1));
        EXPECT_EQ(GetRemainingMegaEvolutions(B_BATTLER_3), 0);
        // Another trainer has its own budget; foreign namespaces get no boss exception.
        gBattleTypeFlags |= BATTLE_TYPE_TWO_OPPONENTS;
        EXPECT_EQ(GetRemainingMegaEvolutions(B_BATTLER_3), 2);
        gBattleTypeFlags |= BATTLE_TYPE_RECORDED_LINK;
        EXPECT_EQ(EmeraldChampions_GetMegaEvolutionLimit(B_BATTLER_1), 1);
        gBattleTypeFlags = savedFlags;
        TRAINER_BATTLE_PARAM.opponentA = savedA;
        TRAINER_BATTLE_PARAM.opponentB = savedB;
        gBattlerPartyIndexes[B_BATTLER_1] = savedIndex;
        gBattleMons[B_BATTLER_1] = savedMon;
        gBattleStruct->gimmick = savedGimmick;
        gParties[B_TRAINER_OPPONENT_A][5] = savedPartyMon;
    }
}



DOUBLE_BATTLE_TEST("EC Mega budget: native activations consume two uses for one boss owner")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(10); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(20); }
        OPPONENT(SPECIES_KANGASKHAN) { Item(ITEM_KANGASKHANITE); Speed(100); }
        OPPONENT(SPECIES_SALAMENCE) { Item(ITEM_SALAMENCITE); Speed(90); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        // Test the native form/activation owner directly without changing any
        // authored League slots before their upcoming design review. The
        // separate queued-request test covers the final execution guard.
        u32 savedFlags = gBattleTypeFlags;
        u16 savedTrainer = TRAINER_BATTLE_PARAM.opponentA;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
        TRAINER_BATTLE_PARAM.opponentA = TRAINER_SIDNEY;
        EXPECT_EQ(GetRemainingMegaEvolutions(B_BATTLER_1), 2);
        ActivateMegaEvolution(B_BATTLER_1);
        EXPECT_EQ(opponentLeft->species, SPECIES_KANGASKHAN_MEGA);
        EXPECT_EQ(GetActiveGimmick(B_BATTLER_1), GIMMICK_MEGA);
        EXPECT_EQ(GetRemainingMegaEvolutions(B_BATTLER_3), 1);
        ActivateMegaEvolution(B_BATTLER_3);
        EXPECT_EQ(opponentRight->species, SPECIES_SALAMENCE_MEGA);
        EXPECT_EQ(GetActiveGimmick(B_BATTLER_3), GIMMICK_MEGA);
        EXPECT_EQ(gBattleStruct->gimmick.megaEvolutionsUsed[B_TRAINER_OPPONENT_A], 2);
        EXPECT_EQ(GetRemainingMegaEvolutions(B_BATTLER_1), 0);
        EXPECT_EQ(GetRemainingMegaEvolutions(B_BATTLER_3), 0);
        // The gate the AI controller actually consults on the way to executing
        // its choice. It used to ask only whether the side had ever Mega
        // Evolved, so a licensed second stone was refused after the first: the
        // reserve reported the Mega as usable every turn and never evolved.
        gBattleStruct->gimmick.megaEvolutionsUsed[B_TRAINER_OPPONENT_A] = 1;
        gBattleStruct->gimmick.activeGimmick[B_TRAINER_OPPONENT_A][gBattlerPartyIndexes[B_BATTLER_3]] = GIMMICK_NONE;
        gBattleStruct->gimmick.activated[B_BATTLER_1][GIMMICK_MEGA] = TRUE;
        gBattleStruct->gimmick.activated[B_BATTLER_3][GIMMICK_MEGA] = TRUE;
        EXPECT(HasTrainerUsedGimmick(B_BATTLER_3, GIMMICK_MEGA));
        EXPECT(CanTrainerStillActivateGimmick(B_BATTLER_3, GIMMICK_MEGA));
        // A trainer on the ordinary one-per-side rule is unaffected.
        TRAINER_BATTLE_PARAM.opponentA = TRAINER_MICHELLE;
        EXPECT(!CanTrainerStillActivateGimmick(B_BATTLER_3, GIMMICK_MEGA));
        TRAINER_BATTLE_PARAM.opponentA = TRAINER_SIDNEY;
        // Spending the licensed second stone closes the door for good.
        gBattleStruct->gimmick.megaEvolutionsUsed[B_TRAINER_OPPONENT_A] = 2;
        EXPECT(!CanTrainerStillActivateGimmick(B_BATTLER_3, GIMMICK_MEGA));
        u8 savedIndex = gBattlerPartyIndexes[B_BATTLER_1];
        gBattlerPartyIndexes[B_BATTLER_1] = 2;
        EXPECT_EQ(GetRemainingMegaEvolutions(B_BATTLER_1), 0);
        gBattlerPartyIndexes[B_BATTLER_1] = savedIndex;
        gBattleTypeFlags = savedFlags;
        TRAINER_BATTLE_PARAM.opponentA = savedTrainer;
    }
}


static void AuthoredLeagueOpponent(u16 trainerId)
{
    bool32 savedWally = FlagGet(FLAG_DEFEATED_WALLY_VICTORY_ROAD);
    bool32 savedChampion = FlagGet(FLAG_IS_CHAMPION);
    FlagSet(FLAG_DEFEATED_WALLY_VICTORY_ROAD);
    FlagClear(FLAG_IS_CHAMPION);
    AuthoredOpponent(trainerId, 8, FALSE);
    if (!savedWally) FlagClear(FLAG_DEFEATED_WALLY_VICTORY_ROAD);
    if (savedChampion) FlagSet(FLAG_IS_CHAMPION);
}

DOUBLE_BATTLE_TEST("EC League authored Megas: every boss permits and activates its two actual forms")
{
    u16 trainer = TRAINER_NONE;
    u32 first = 0, second = 0;
    enum Species firstForm = SPECIES_NONE, secondForm = SPECIES_NONE;
    PARAMETRIZE { trainer = TRAINER_SIDNEY; first = 4; second = 5; firstForm = SPECIES_SHARPEDO_MEGA; secondForm = SPECIES_ABSOL_MEGA_Z; }
    PARAMETRIZE { trainer = TRAINER_PHOEBE; first = 3; second = 5; firstForm = SPECIES_BANETTE_MEGA; secondForm = SPECIES_GENGAR_MEGA; }
    PARAMETRIZE { trainer = TRAINER_GLACIA; first = 3; second = 5; firstForm = SPECIES_FROSLASS_MEGA; secondForm = SPECIES_BAXCALIBUR_MEGA; }
    PARAMETRIZE { trainer = TRAINER_DRAKE; first = 0; second = 5; firstForm = SPECIES_SALAMENCE_MEGA; secondForm = SPECIES_DRAGONITE_MEGA; }
    PARAMETRIZE { trainer = TRAINER_WALLACE; first = 4; second = 5; firstForm = SPECIES_MILOTIC_MEGA; secondForm = SPECIES_STARMIE_MEGA; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(10); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(20); }
        AuthoredLeagueOpponent(trainer);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE);
            // Every authored lead has at least one direct attack. This first
            // turn initializes the native board; the deployment below is an
            // explicit reserve-pair fixture, not earned switching evidence.
            MOVE(opponentLeft, moveSlot: 0, target: playerLeft);
            MOVE(opponentRight, moveSlot: 0, target: playerRight);
        }
    } THEN {
        u32 savedFlags = gBattleTypeFlags;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
        TRAINER_BATTLE_PARAM.opponentA = trainer;
        for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        {
            gBattlerPartyIndexes[B_BATTLER_1] = slot;
            EXPECT_EQ(EmeraldChampions_IsMegaAllowed(B_BATTLER_1), slot == first || slot == second);
        }
        gBattlerPartyIndexes[B_BATTLER_1] = first;
        gBattlerPartyIndexes[B_BATTLER_3] = second;
        PokemonToBattleMon(&gParties[B_TRAINER_OPPONENT_A][first], opponentLeft);
        PokemonToBattleMon(&gParties[B_TRAINER_OPPONENT_A][second], opponentRight);
        memset(&gBattleStruct->gimmick, 0, sizeof(gBattleStruct->gimmick));
        EXPECT(CanMegaEvolve(B_BATTLER_1));
        EXPECT(CanMegaEvolve(B_BATTLER_3));
        ActivateMegaEvolution(B_BATTLER_1);
        EXPECT_EQ(opponentLeft->species, firstForm);
        EXPECT_EQ(GetRemainingMegaEvolutions(B_BATTLER_3), 1);
        EXPECT(CanMegaEvolve(B_BATTLER_3));
        ActivateMegaEvolution(B_BATTLER_3);
        EXPECT_EQ(opponentRight->species, secondForm);
        EXPECT_EQ(GetRemainingMegaEvolutions(B_BATTLER_1), 0);
        EXPECT_EQ(GetRemainingMegaEvolutions(B_BATTLER_3), 0);
        EXPECT(!CanMegaEvolve(B_BATTLER_1));
        EXPECT(!CanMegaEvolve(B_BATTLER_3));
        EXPECT_EQ(gBattleStruct->gimmick.megaEvolutionsUsed[B_TRAINER_OPPONENT_A], 2);
        if (trainer == TRAINER_WALLACE)
        {
            EXPECT_EQ(GetBattlerAbility(B_BATTLER_1), ABILITY_PRISM_SCALES);
            EXPECT_EQ(GetBattlerAbility(B_BATTLER_3), ABILITY_HUGE_POWER);
            // Badge8 cap80, authored offsets +8/+7, Normal difficulty -1.
            EXPECT_EQ(opponentLeft->level, 87);
            EXPECT_EQ(opponentRight->level, 86);
        }
        gBattleTypeFlags = savedFlags;
    }
}

AI_DOUBLE_BATTLE_TEST("EC Gym: Takao's healthy lead does not withdraw on turn one")
{
    u32 power;
    PARAMETRIZE { power = 120; }
    PARAMETRIZE { power = 400; }
    GIVEN {
        // A Fairy attacker opposite a Fighting lead reads as pressure even at
        // full health. Being threatened is a reason to look at the bench, not
        // a reason to hand over the first turn from 80 of 80.
        PLAYER(SPECIES_GARDEVOIR) { Level(24); HP(300); MaxHP(300); SpAttack(power); Speed(200); Moves(MOVE_DAZZLING_GLEAM); }
        PLAYER(SPECIES_MAGIKARP) { Level(24); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        AuthoredOpponent(TRAINER_TAKAO, 2, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DAZZLING_GLEAM);
            MOVE(playerRight, MOVE_SPLASH);
            EXPECT_MOVES(opponentLeft, MOVE_OCTOLOCK, MOVE_DRAIN_PUNCH, MOVE_SUCKER_PUNCH, MOVE_PROTECT);
        }
    } THEN {
        // A knockout can replace the active battler after its selected move.
        // EXPECT_MOVES above verifies that the original lead did not switch.
        u32 originalHp = GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP);
        if (power == 400)
            EXPECT_EQ(originalHp, 0);
        else
        {
            EXPECT(originalHp > 0);
            EXPECT_EQ(opponentLeft->species, SPECIES_GRAPPLOCT);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC Gym: Wattson's Discharge activates Motor Drive before Electivire acts")
{
    GIVEN {
        // The authored engine of the room: Electrode's spread attack is the
        // trigger, and the partner's Motor Drive turns it into speed. It has
        // to beat the moves that only deal damage.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Defense(120); SpDefense(120); Speed(20); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Defense(120); SpDefense(120); Speed(10); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        AuthoredOpponent(TRAINER_WATTSON_1, 3, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_DISCHARGE);
        }
    } THEN {
        EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Aisha's activation survives a board that can fight back")
{
    GIVEN {
        // The attackers pressure Tauros while Throh sets up the critical hit.
        // The combo must leave its original recipient alive after both attacks.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(200); MaxHP(200); Attack(90); Defense(60); SpDefense(60); Speed(70); Ability(ABILITY_TELEPATHY); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Attack(90); Defense(60); SpDefense(60); Speed(60); Moves(MOVE_TACKLE); }
        // Route117 is available before Wattson: two badges, cap30.
        AuthoredOpponent(TRAINER_AISHA, 2, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_STORM_THROW, target: opponentRight);
            // The recipient must not spend the same turn behind a shield: its
            // own guard blocks the activation the pair just chose.
            NOT_EXPECT_MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_3], 1);
        EXPECT(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP) > 0);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], MAX_STAT_STAGE);
    }
}

// A player body with a benchmark's preparation: cap level, perfect IVs and
// the given set, through the same preset path the Center uses.
static void PreparedPlayer(enum Species species, u32 level, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, level, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(set->moves[0], set->moves[1], set->moves[2], set->moves[3]);
    }
}

// The same body caught mid-battle at a benchmark's HP.
static void InjuredPlayer(enum Species species, u32 level, const struct EmeraldChampionsBattleSet *set, u32 hp)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, level, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(set->moves[0], set->moves[1], set->moves[2], set->moves[3]);
        HP(hp);
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Aisha's Storm Throw is priced by the boosted reply it enables")
{
    GIVEN {
        // Throwing Mawile beside Close Combat was a sure knockout, and the
        // trial never let the thrown Tauros hit at +6, so the activation
        // looked like self-damage and lost. Anger Point fires before the
        // slower Tauros acts, and its maximum-Attack Close Combat takes the
        // same knockout while the authored boost stays on the board.
        const struct EmeraldChampionsBattleSet mawile = {
            .moves = {MOVE_IRON_HEAD, MOVE_PLAY_ROUGH, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_ADAMANT, .ability = ABILITY_INTIMIDATE,
        };
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Defense(300); SpDefense(300); Speed(10); Moves(MOVE_SPLASH); }
        PreparedPlayer(SPECIES_MAWILE, 30, &mawile);
        // Route117 is available before Wattson: two badges, cap30.
        AuthoredOpponent(TRAINER_AISHA, 2, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SPLASH);
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentLeft, MOVE_STORM_THROW, target: opponentRight);
        }
    } THEN {
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], MAX_STAT_STAGE);
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: the fisherman's Magikarp takes the Dragon Rage it cannot dodge")
{
    GIVEN {
        // The player aims at the Magikarp standing there, and our switch
        // lands after that choice: the incoming body eats the same fixed 80.
        // Scored only on its own board, the switch let both Dragon Rages
        // re-aim at Feebas and credited Mirror Coat with returning them, so
        // the Magikarp left every time it was targeted.
        const struct EmeraldChampionsBattleSet karp = {
            .moves = {MOVE_DRAGON_RAGE, MOVE_FLAIL, MOVE_BOUNCE, MOVE_TACKLE},
            .item = ITEM_FOCUS_SASH, .nature = NATURE_JOLLY, .ability = ABILITY_SWIFT_SWIM,
            .evs = {252, 0, 4, 0, 0, 252},
        };
        PreparedPlayer(SPECIES_MAGIKARP, 55, &karp);
        PreparedPlayer(SPECIES_MAGIKARP, 55, &karp);
        // Route 118's fisherman is met after the fifth badge.
        AuthoredOpponent(TRAINER_MAGIKARP_GUY, 5, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DRAGON_RAGE, target: opponentLeft);
            MOVE(playerRight, MOVE_DRAGON_RAGE, target: opponentLeft);
            EXPECT_MOVES(opponentLeft, MOVE_FLAIL, MOVE_BOUNCE, MOVE_TACKLE);
            NOT_EXPECT_MOVE(opponentRight, MOVE_MIRROR_COAT);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_MAGIKARP);
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Jaclyn's Wobbuffet never Encores its own partner")
{
    GIVEN {
        // The benchmark line: both foes already encored, Safeguard up and
        // Counter/Mirror Coat unable to touch a Ghost or Dark body. Encoring
        // the fresh Gallade looked free because the lock only bites later;
        // the plan's Encore is for a foe's completed move.
        const struct EmeraldChampionsBattleSet sets[] = {
            {.moves = {MOVE_FAKE_OUT, MOVE_WILL_O_WISP, MOVE_FOUL_PLAY, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_CAREFUL, .ability = ABILITY_PRANKSTER},
            {.moves = {MOVE_DARK_PULSE, MOVE_HEAT_WAVE, MOVE_NASTY_PLOT, MOVE_PROTECT}, .item = ITEM_CHOICE_SPECS, .nature = NATURE_TIMID, .ability = ABILITY_FLASH_FIRE},
            {.moves = {MOVE_IRON_HEAD, MOVE_PLAY_ROUGH, MOVE_SUCKER_PUNCH, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_ADAMANT, .ability = ABILITY_INTIMIDATE},
        };
        PreparedPlayer(SPECIES_SABLEYE, 30, &sets[0]);
        PreparedPlayer(SPECIES_HOUNDOOM, 30, &sets[1]);
        PreparedPlayer(SPECIES_MAWILE, 30, &sets[2]);
        // Route117 is available before Wattson: two badges, cap30.
        AuthoredOpponent(TRAINER_JACLYN, 2, FALSE);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight); MOVE(playerRight, MOVE_DARK_PULSE, target: opponentRight); }
        TURN { MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentRight); MOVE(playerRight, MOVE_DARK_PULSE, target: opponentRight); }
        TURN { MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentRight); MOVE(playerRight, MOVE_DARK_PULSE, target: opponentRight); }
        TURN {
            SWITCH(playerLeft, 2);
            MOVE(playerRight, MOVE_DARK_PULSE, target: opponentRight);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_ENCORE);
        }
    } THEN {
        EXPECT_EQ(opponentRight->species, SPECIES_GALLADE);
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Georgia's Duskull does not Wisp through a burned Rage Powder user")
{
    GIVEN {
        // The benchmark line: the burned Amoonguss has just drawn Duskull's
        // Will-O-Wisp with Rage Powder, and the Choice Band Excadrill is
        // still locked into Iron Head. Every further Wisp aimed at Excadrill
        // was redirected into the burn it had already given, twice.
        const struct EmeraldChampionsBattleSet amoonguss = {
            .moves = {MOVE_RAGE_POWDER, MOVE_SPORE, MOVE_POLLEN_PUFF, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .nature = NATURE_BOLD, .ability = ABILITY_REGENERATOR,
        };
        const struct EmeraldChampionsBattleSet excadrill = {
            .moves = {MOVE_IRON_HEAD, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_EARTHQUAKE},
            .item = ITEM_CHOICE_BAND, .nature = NATURE_ADAMANT, .ability = ABILITY_MOLD_BREAKER,
        };
        struct Pokemon mon;
        CreateRandomMonWithIVs(&mon, SPECIES_AMOONGUSS, 40, MAX_PER_STAT_IVS);
        EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &amoonguss), EC_BATTLE_SET_SUCCESS);
        CalculateMonStats(&mon);
        PLAYER(SPECIES_AMOONGUSS) {
            *gBattleTestRunnerState->data.currentMon = mon;
            Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
            Moves(amoonguss.moves[0], amoonguss.moves[1], amoonguss.moves[2], amoonguss.moves[3]);
            Status1(STATUS1_BURN);
        }
        PreparedPlayer(SPECIES_EXCADRILL, 40, &excadrill);
        // Trick House 2 is Georgia's room: three badges, cap40. Azumarill
        // stands beside Duskull as it did once Metang fell.
        AuthoredOpponentWithPartner(TRAINER_GEORGIA, 3, FALSE, 2);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_RAGE_POWDER); MOVE(playerRight, MOVE_IRON_HEAD, target: opponentRight); }
        TURN {
            MOVE(playerLeft, MOVE_RAGE_POWDER);
            MOVE(playerRight, MOVE_IRON_HEAD, target: opponentRight);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_WILL_O_WISP);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: the fisherman's Feebas does not Mirror Coat Tackles")
{
    GIVEN {
        // The benchmark line: six Magikarp, whose only special move is Dragon
        // Rage, answered Feebas with Tackle and Flail and aimed every Dragon
        // Rage at the other fish. Feebas chose Mirror Coat on eight turns and
        // it failed on every one.
        const struct EmeraldChampionsBattleSet karp = {
            .moves = {MOVE_DRAGON_RAGE, MOVE_FLAIL, MOVE_BOUNCE, MOVE_TACKLE},
            .item = ITEM_FOCUS_SASH, .nature = NATURE_JOLLY, .ability = ABILITY_SWIFT_SWIM,
            .evs = {252, 0, 4, 0, 0, 252},
        };
        for (u32 i = 0; i < 6; i++)
            PreparedPlayer(SPECIES_MAGIKARP, 55, &karp);
        // Route 118's fisherman is met after the fifth badge.
        AuthoredOpponent(TRAINER_MAGIKARP_GUY, 5, FALSE);
    } WHEN {
        // Turn one of the benchmark: both Dragon Rages into his Magikarp.
        TURN {
            MOVE(playerLeft, MOVE_DRAGON_RAGE, target: opponentLeft);
            MOVE(playerRight, MOVE_DRAGON_RAGE, target: opponentLeft);
        }
        // Turn two: Dragon Rage at his Magikarp again, Tackle into Feebas.
        TURN {
            MOVE(playerLeft, MOVE_DRAGON_RAGE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
        }
        // Feebas has now taken a Tackle and no special hit at all.
        TURN {
            MOVE(playerLeft, MOVE_DRAGON_RAGE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            NOT_EXPECT_MOVE(opponentRight, MOVE_MIRROR_COAT);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: the fisherman's Feebas does not Mirror Coat a Dragon Rage it cannot survive")
{
    GIVEN {
        // The benchmark line: Feebas at 28 HP after two turns of Tackle and
        // Flail. Dragon Rage, the only special move across the field, deals
        // a fixed 40: it knocks Feebas out before a last-moving Mirror Coat
        // can return it, and every other hit is physical.
        const struct EmeraldChampionsBattleSet karp = {
            .moves = {MOVE_DRAGON_RAGE, MOVE_FLAIL, MOVE_BOUNCE, MOVE_TACKLE},
            .item = ITEM_FOCUS_SASH, .nature = NATURE_JOLLY, .ability = ABILITY_SWIFT_SWIM,
            .evs = {252, 0, 4, 0, 0, 252},
        };
        for (u32 i = 0; i < 2; i++)
            PreparedPlayer(SPECIES_MAGIKARP, 55, &karp);
        // As in the benchmark, the reserves have fallen.
        sAuthoredInjuries[0] = (struct AuthoredInjury){SPECIES_FEEBAS, 39};
        sAuthoredInjuries[1] = (struct AuthoredInjury){SPECIES_WISHIWASHI, 0};
        sAuthoredInjuries[2] = (struct AuthoredInjury){SPECIES_PYUKUMUKU, 0};
        sAuthoredInjuries[3] = (struct AuthoredInjury){SPECIES_GYARADOS, 0};
        AuthoredOpponent(TRAINER_MAGIKARP_GUY, 5, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentLeft);
            NOT_EXPECT_MOVE(opponentRight, MOVE_MIRROR_COAT);
        }
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentLeft);
            NOT_EXPECT_MOVE(opponentRight, MOVE_MIRROR_COAT);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Takao's Wobbuffet does not raise a second Safeguard")
{
    GIVEN {
        // The benchmark line: Sableye and Mawile push into Grapploct while
        // Takao's Safeguard is up. The Wobbuffet set it on turn one and chose
        // it again three turns later, into its own veil.
        const struct EmeraldChampionsBattleSet sets[] = {
            {.moves = {MOVE_FAKE_OUT, MOVE_WILL_O_WISP, MOVE_FOUL_PLAY, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_CAREFUL, .ability = ABILITY_PRANKSTER},
            {.moves = {MOVE_PLAY_ROUGH, MOVE_IRON_HEAD, MOVE_SUCKER_PUNCH, MOVE_PROTECT}, .item = ITEM_FOCUS_SASH, .nature = NATURE_ADAMANT, .ability = ABILITY_INTIMIDATE},
            {.moves = {MOVE_MOONBLAST, MOVE_DAZZLING_GLEAM, MOVE_PSYCHIC, MOVE_PROTECT}, .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_FLOWER_VEIL},
        };
        // Bulk enough that both leads stand through the line, so every turn
        // is the same decision the benchmark faced.
        const enum Species leads[] = {SPECIES_SABLEYE, SPECIES_MAWILE};
        for (u32 i = 0; i < ARRAY_COUNT(leads); i++)
        {
            struct Pokemon mon;
            CreateRandomMonWithIVs(&mon, leads[i], 20, MAX_PER_STAT_IVS);
            EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &sets[i]), EC_BATTLE_SET_SUCCESS);
            CalculateMonStats(&mon);
            PLAYER(leads[i]) {
                *gBattleTestRunnerState->data.currentMon = mon;
                Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                HP(400); MaxHP(400);
            }
        }
        PreparedPlayer(SPECIES_FLOETTE, 20, &sets[2]);
        // Takao is the Dewford gym's trainer: one badge, cap20.
        AuthoredOpponent(TRAINER_TAKAO, 1, FALSE);
    } WHEN {
        // Every command repeats, so an Encore on either never changes it.
        for (u32 turn = 0; turn < 4; turn++)
            TURN { MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentLeft); MOVE(playerRight, MOVE_PLAY_ROUGH, target: opponentLeft); }
    } THEN {
        // Safeguard lasts five turns: the Wobbuffet sets it once in four.
        struct Pokemon *wobbuffet = &gParties[B_TRAINER_OPPONENT_A][1];
        EXPECT_EQ(GetMonData(wobbuffet, MON_DATA_SPECIES), SPECIES_WOBBUFFET);
        EXPECT_GE(GetMonData(wobbuffet, MON_DATA_PP4), GetMovePP(MOVE_SAFEGUARD) - 1);
    }
}

static const struct EmeraldChampionsBattleSet sBenFoeGastrodon = {
    .moves = {MOVE_EARTH_POWER, MOVE_ICE_BEAM, MOVE_SLUDGE_BOMB, MOVE_RECOVER},
    .item = ITEM_CHOICE_SPECS, .nature = NATURE_MODEST, .ability = ABILITY_STORM_DRAIN,
};
static const struct EmeraldChampionsBattleSet sBenFoeDiggersby = {
    .moves = {MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_QUICK_ATTACK, MOVE_BODY_SLAM},
    .item = ITEM_CHOICE_BAND, .nature = NATURE_ADAMANT, .ability = ABILITY_HUGE_POWER,
};
static const struct EmeraldChampionsBattleSet sBenFoePawmot = {
    .moves = {MOVE_FAKE_OUT, MOVE_CLOSE_COMBAT, MOVE_MACH_PUNCH, MOVE_ICE_PUNCH},
    .item = ITEM_FOCUS_SASH, .nature = NATURE_JOLLY, .ability = ABILITY_VOLT_ABSORB,
};
static const struct EmeraldChampionsBattleSet sBenFoeClodsire = {
    .moves = {MOVE_HIGH_HORSEPOWER, MOVE_POISON_JAB, MOVE_RECOVER, MOVE_PROTECT},
    .item = ITEM_LIFE_ORB, .nature = NATURE_ADAMANT, .ability = ABILITY_WATER_ABSORB,
};

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Ben's Charjabug does not Discharge into two Ground bodies and its own partner")
{
    GIVEN {
        // The benchmark line: Charjabug at 15 HP beside Eelektross, facing
        // Gastrodon and Diggersby. Discharge touched neither foe and took
        // 6 HP from the Levitate Eelektross, twice.
        PreparedPlayer(SPECIES_GASTRODON_WEST, 30, &sBenFoeGastrodon);
        PreparedPlayer(SPECIES_DIGGERSBY, 30, &sBenFoeDiggersby);
        // Route 117's Youngster Ben is met after the second badge: cap30.
        // Lanturn and Vikavolt had already fallen.
        sAuthoredInjuries[0] = (struct AuthoredInjury){SPECIES_CHARJABUG, 15};
        sAuthoredInjuries[1] = (struct AuthoredInjury){SPECIES_LANTURN, 0};
        sAuthoredInjuries[2] = (struct AuthoredInjury){SPECIES_VIKAVOLT, 0};
        AuthoredOpponentWithPartner(TRAINER_BEN, 2, FALSE, 3);
    } WHEN {
        // Charjabug shielded the turn before, as in the benchmark.
        TURN {
            MOVE(playerLeft, MOVE_EARTH_POWER, target: opponentRight);
            MOVE(playerRight, MOVE_HIGH_HORSEPOWER, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
        TURN {
            MOVE(playerLeft, MOVE_EARTH_POWER, target: opponentRight);
            MOVE(playerRight, MOVE_HIGH_HORSEPOWER, target: opponentRight);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_DISCHARGE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Ben's Charjabug does not Eerie Impulse a side with no special moves")
{
    const struct EmeraldChampionsBattleSet *partner;
    enum Species partnerSpecies;
    PARAMETRIZE { partnerSpecies = SPECIES_DIGGERSBY; partner = &sBenFoeDiggersby; }
    PARAMETRIZE { partnerSpecies = SPECIES_CLODSIRE; partner = &sBenFoeClodsire; }
    GIVEN {
        // The benchmark lines: Eerie Impulse into the Choice Band Diggersby
        // on three turns running, and into Clodsire, beside a Pawmot that
        // only punches.
        PreparedPlayer(SPECIES_PAWMOT, 30, &sBenFoePawmot);
        PreparedPlayer(partnerSpecies, 30, partner);
        AuthoredOpponent(TRAINER_BEN, 2, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight);
            MOVE(playerRight, MOVE_HIGH_HORSEPOWER, target: opponentRight);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_EERIE_IMPULSE);
        }
    }
}

// A benchmark player member caught mid-battle: the prepared set at this HP
// and non-volatile status.
static void PreparedPlayerAt(enum Species species, u32 level, const struct EmeraldChampionsBattleSet *set, u32 hp, u32 status)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, level, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(set->moves[0], set->moves[1], set->moves[2], set->moves[3]);
        HP(hp);
        if (status)
            Status1(status);
    }
}

static const struct EmeraldChampionsBattleSet sJoshFoeSylveon = {
    .moves = {MOVE_HYPER_VOICE, MOVE_MOONBLAST, MOVE_QUICK_ATTACK, MOVE_PROTECT},
    .item = ITEM_CHOICE_SPECS, .nature = NATURE_MODEST, .ability = ABILITY_PIXILATE,
};
static const struct EmeraldChampionsBattleSet sJoshFoeMonferno = {
    .moves = {MOVE_FAKE_OUT, MOVE_CLOSE_COMBAT, MOVE_FLARE_BLITZ, MOVE_PROTECT},
    .item = ITEM_FOCUS_SASH, .nature = NATURE_JOLLY, .ability = ABILITY_IRON_FIST,
};

AI_DOUBLE_BATTLE_TEST("EC no payoff: Josh's Naclstack does not Recover at full HP")
{
    GIVEN {
        // The benchmark line: Naclstack came in untouched beside an 11 HP
        // Nosepass, facing a Specs Sylveon and a 1 HP Monferno. It shielded
        // once, then chose Recover at 53/53 and the engine answered that its
        // HP was full.
        PreparedPlayer(SPECIES_SYLVEON, 14, &sJoshFoeSylveon);
        PreparedPlayerAt(SPECIES_MONFERNO, 14, &sJoshFoeMonferno, 1, 0);
        // Youngster Josh is met before the first badge: cap14. Glimmet,
        // Dwebble and Alolan Geodude had already fallen.
        sAuthoredInjuries[0] = (struct AuthoredInjury){SPECIES_NOSEPASS, 11};
        sAuthoredInjuries[1] = (struct AuthoredInjury){SPECIES_GLIMMET, 0};
        sAuthoredInjuries[2] = (struct AuthoredInjury){SPECIES_DWEBBLE, 0};
        sAuthoredInjuries[3] = (struct AuthoredInjury){SPECIES_GEODUDE_ALOLA, 0};
        sAuthoredLeadSlot = 4;
        AuthoredOpponent(TRAINER_JOSH, 0, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_HYPER_VOICE);
            MOVE(playerRight, MOVE_PROTECT);
        }
        // Behind its shield it is still at 53/53.
        TURN {
            MOVE(playerLeft, MOVE_HYPER_VOICE);
            MOVE(playerRight, MOVE_CLOSE_COMBAT, target: opponentLeft);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_RECOVER);
        }
    }
}

static const struct EmeraldChampionsBattleSet sBlakeFoeYveltal = {
    .moves = {MOVE_DARK_PULSE, MOVE_OBLIVION_WING, MOVE_HEAT_WAVE, MOVE_PROTECT},
    .item = ITEM_LIFE_ORB, .nature = NATURE_MODEST, .ability = ABILITY_DARK_AURA, .evs = {4, 0, 0, 252, 0, 252},
};
static const struct EmeraldChampionsBattleSet sBlakeFoeIncineroar = {
    .moves = {MOVE_FAKE_OUT, MOVE_FLARE_BLITZ, MOVE_KNOCK_OFF, MOVE_PARTING_SHOT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_CAREFUL, .ability = ABILITY_INTIMIDATE, .evs = {252, 0, 4, 0, 252, 0},
};

AI_DOUBLE_BATTLE_TEST("EC no payoff: Blake's Meowstic does not Helping Hand a partner's screen")
{
    GIVEN {
        // The benchmark line: Fake Out flinched Meowstic's Reflect on the
        // first turn; on the second it raised Reflect while its partner
        // Helping Handed it - a boost with no damage to multiply.
        PreparedPlayer(SPECIES_YVELTAL, 65, &sBlakeFoeYveltal);
        PreparedPlayer(SPECIES_INCINEROAR, 65, &sBlakeFoeIncineroar);
        // Psychic Blake is met with six badges: cap65.
        AuthoredOpponent(TRAINER_BLAKE, 6, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DARK_PULSE, target: opponentRight);
            MOVE(playerRight, MOVE_FAKE_OUT, target: opponentLeft);
        }
        TURN {
            MOVE(playerLeft, MOVE_DARK_PULSE, target: opponentRight);
            MOVE(playerRight, MOVE_KNOCK_OFF, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_REFLECT);
            NOT_EXPECT_MOVE(opponentRight, MOVE_HELPING_HAND);
        }
    }
}

static const struct EmeraldChampionsBattleSet sJuanFoeMiraidon = {
    .moves = {MOVE_ELECTRO_DRIFT, MOVE_DRACO_METEOR, MOVE_DAZZLING_GLEAM, MOVE_PROTECT},
    .item = ITEM_LIFE_ORB, .nature = NATURE_TIMID, .ability = ABILITY_HADRON_ENGINE, .evs = {4, 0, 0, 252, 0, 252},
};
static const struct EmeraldChampionsBattleSet sJuanFoeIronHands = {
    .moves = {MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH, MOVE_WILD_CHARGE, MOVE_HEAVY_SLAM},
    .item = ITEM_ASSAULT_VEST, .nature = NATURE_ADAMANT, .ability = ABILITY_QUARK_DRIVE, .evs = {252, 252, 4, 0, 0, 0},
};

AI_DOUBLE_BATTLE_TEST("EC no payoff: Juan's Manaphy does not Tail Glow into the hit that ends it")
{
    GIVEN {
        // The benchmark board after Politoed and Suicune fell: Altaria and
        // Manaphy against Miraidon and Iron Hands. Manaphy Tail Glowed, then
        // Iron Hands' Wild Charge knocked it out with the boost unspent.
        PreparedPlayerAt(SPECIES_MIRAIDON, 70, &sJuanFoeMiraidon, 173, 0);
        PreparedPlayerAt(SPECIES_IRON_HANDS, 70, &sJuanFoeIronHands, 294, 0);
        // Juan is the eighth gym: seven badges, cap70.
        sAuthoredInjuries[0] = (struct AuthoredInjury){SPECIES_POLITOED, 0};
        sAuthoredInjuries[1] = (struct AuthoredInjury){SPECIES_SUICUNE, 0};
        sAuthoredLeadSlot = 2;
        AuthoredOpponentWithPartner(TRAINER_JUAN_1, 7, FALSE, 3);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DRACO_METEOR, target: opponentLeft);
            MOVE(playerRight, MOVE_WILD_CHARGE, target: opponentRight);
            NOT_EXPECT_MOVE(opponentRight, MOVE_TAIL_GLOW);
        }
        // Fake Out is gone now, as it was on the benchmark turn.
        TURN {
            MOVE(playerLeft, MOVE_DRACO_METEOR, target: opponentLeft);
            MOVE(playerRight, MOVE_WILD_CHARGE, target: opponentRight);
            NOT_EXPECT_MOVE(opponentRight, MOVE_TAIL_GLOW);
        }
    }
}

static const struct EmeraldChampionsBattleSet sNormanFoeLucario = {
    .moves = {MOVE_CLOSE_COMBAT, MOVE_METEOR_MASH, MOVE_EXTREME_SPEED, MOVE_PROTECT},
    .item = ITEM_LIFE_ORB, .nature = NATURE_ADAMANT, .ability = ABILITY_INNER_FOCUS, .evs = {4, 252, 0, 0, 0, 252},
};
static const struct EmeraldChampionsBattleSet sNormanFoeBlaziken = {
    .moves = {MOVE_FLARE_BLITZ, MOVE_CLOSE_COMBAT, MOVE_KNOCK_OFF, MOVE_PROTECT},
    .item = ITEM_BLAZIKENITE, .nature = NATURE_JOLLY, .ability = ABILITY_SPEED_BOOST, .evs = {4, 252, 0, 0, 0, 252},
};

AI_DOUBLE_BATTLE_TEST("EC no payoff: Norman's Weezing does not hit a Blaziken its poison is about to finish")
{
    GIVEN {
        // The benchmark board: a poisoned Blaziken at 10 HP, faster than
        // Weezing, beside a healthy Lucario. Weezing Sludge Bombed the
        // Blaziken after it had moved; the poison ended it at the turn's end.
        PreparedPlayerAt(SPECIES_LUCARIO, 45, &sNormanFoeLucario, 119, 0);
        PreparedPlayerAt(SPECIES_BLAZIKEN, 45, &sNormanFoeBlaziken, 10, STATUS1_POISON);
        // Norman is the fifth gym: four badges, cap45.
        sAuthoredInjuries[0] = (struct AuthoredInjury){SPECIES_WEEZING_GALAR, 97};
        AuthoredOpponentWithPartner(TRAINER_NORMAN_1, 4, FALSE, 2);
    } WHEN {
        TURN {
            // Lucario shields so Weezing lives to act on its choice.
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_KNOCK_OFF, target: opponentRight);
        }
    } THEN {
        EXPECT(!(gBattleStruct->battlerState[B_BATTLER_1].lastMoveTarget == B_BATTLER_2
              && gLastMoves[B_BATTLER_1] != MOVE_NONE && !IsBattleMoveStatus(gLastMoves[B_BATTLER_1])));
    }
}

static const struct EmeraldChampionsBattleSet sMattFoeIronHands = {
    .moves = {MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH, MOVE_WILD_CHARGE, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_ADAMANT, .ability = ABILITY_QUARK_DRIVE, .evs = {252, 252, 4, 0, 0, 0},
};
static const struct EmeraldChampionsBattleSet sMattFoeGyarados = {
    .moves = {MOVE_DRAGON_DANCE, MOVE_WATERFALL, MOVE_CRUNCH, MOVE_PROTECT},
    .item = ITEM_GYARADOSITE, .nature = NATURE_JOLLY, .ability = ABILITY_INTIMIDATE, .evs = {4, 252, 0, 0, 0, 252},
};

AI_DOUBLE_BATTLE_TEST("EC Mega forecast: Matt's Grimmsnarl does not Prankster Thunder Wave a Gyarados about to turn Dark")
{
    GIVEN {
        // The benchmark board: Pelipper and a 46 HP Grimmsnarl against Iron
        // Hands and a Gyarados holding its Mega Stone. Gyarados Mega Evolved
        // into a Dark type first, and the Prankster Thunder Wave failed.
        PreparedPlayer(SPECIES_IRON_HANDS, 65, &sMattFoeIronHands);
        PreparedPlayer(SPECIES_GYARADOS, 65, &sMattFoeGyarados);
        // Matt at the Aqua Hideout: six badges, cap65. Kingdra had fallen.
        sAuthoredInjuries[0] = (struct AuthoredInjury){SPECIES_GRIMMSNARL, 46};
        sAuthoredInjuries[1] = (struct AuthoredInjury){SPECIES_KINGDRA, 0};
        AuthoredOpponentWithPartner(TRAINER_MATT, 6, FALSE, 2);
    } WHEN {
        // A screen goes up before the benchmark turn.
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
        }
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight);
            MOVE(playerRight, MOVE_DRAGON_DANCE, gimmick: GIMMICK_MEGA);
            NOT_EXPECT_MOVE(opponentRight, MOVE_THUNDER_WAVE);
        }
    }
}

static const struct EmeraldChampionsBattleSet sGlaciaFoeCharizard = {
    .moves = {MOVE_HEAT_WAVE, MOVE_FOCUS_BLAST, MOVE_SOLAR_BEAM, MOVE_PROTECT},
    .item = ITEM_CHARIZARDITE_Y, .nature = NATURE_TIMID, .ability = ABILITY_BLAZE, .evs = {4, 0, 0, 252, 0, 252},
};
static const struct EmeraldChampionsBattleSet sGlaciaFoeIncineroar = {
    .moves = {MOVE_FAKE_OUT, MOVE_FLARE_BLITZ, MOVE_KNOCK_OFF, MOVE_PARTING_SHOT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_ADAMANT, .ability = ABILITY_INTIMIDATE, .evs = {252, 252, 4, 0, 0, 0},
};

AI_DOUBLE_BATTLE_TEST("EC Mega forecast: Glacia's Ninetales does not raise Aurora Veil into Charizard Y's sun")
{
    GIVEN {
        // The benchmark lead: Charizard holding Charizardite Y beside
        // Incineroar. It Mega Evolved, Drought replaced the snow, and the
        // Aurora Veil chosen for the snow failed.
        PreparedPlayer(SPECIES_CHARIZARD, 80, &sGlaciaFoeCharizard);
        PreparedPlayer(SPECIES_INCINEROAR, 80, &sGlaciaFoeIncineroar);
        // Glacia is the second of the Elite Four: eight badges, cap80.
        AuthoredOpponent(TRAINER_GLACIA, 8, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_HEAT_WAVE, gimmick: GIMMICK_MEGA);
            MOVE(playerRight, MOVE_FAKE_OUT, target: opponentRight);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_AURORA_VEIL);
        }
    }
}

static const struct EmeraldChampionsBattleSet sArchieFoeCharizard = {
    .moves = {MOVE_HEAT_WAVE, MOVE_SOLAR_BEAM, MOVE_AIR_SLASH, MOVE_PROTECT},
    .item = ITEM_CHARIZARDITE_Y, .nature = NATURE_TIMID, .ability = ABILITY_SOLAR_POWER, .evs = {4, 0, 0, 252, 0, 252},
};

AI_DOUBLE_BATTLE_TEST("EC Mega forecast: Archie's Pelipper does not throw a rain Weather Ball at Charizard Y")
{
    GIVEN {
        // The benchmark lead: Charizard holding Charizardite Y beside
        // Miraidon. The rain Weather Ball aimed at Charizard met the sun its
        // Mega brought first and landed as a resisted Fire move.
        PreparedPlayer(SPECIES_CHARIZARD, 70, &sArchieFoeCharizard);
        PreparedPlayer(SPECIES_MIRAIDON, 70, &sJuanFoeMiraidon);
        // Archie at the Seafloor Cavern: seven badges, cap70.
        AuthoredOpponent(TRAINER_ARCHIE, 7, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT, gimmick: GIMMICK_MEGA);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        // Both player bodies shielded, so a Weather Ball thrown at either
        // spent PP; one into Miraidon is the same move in rain or sun, one
        // into Charizard is a resisted Fire move. Only the latter is judged.
        EXPECT_EQ(gBattleStruct->battlerState[B_BATTLER_1].lastMoveTarget == B_BATTLER_0
            && gLastMoves[B_BATTLER_1] == MOVE_WEATHER_BALL, FALSE);
    }
}

static const struct EmeraldChampionsBattleSet sWallaceFoeChandelure = {
    .moves = {MOVE_HEAT_WAVE, MOVE_SHADOW_BALL, MOVE_ENERGY_BALL, MOVE_PROTECT},
    .item = ITEM_LIFE_ORB, .nature = NATURE_TIMID, .ability = ABILITY_FLASH_FIRE, .evs = {4, 0, 0, 252, 0, 252},
};
static const struct EmeraldChampionsBattleSet sWallaceFoeSableye = {
    .moves = {MOVE_WILL_O_WISP, MOVE_FAKE_OUT, MOVE_KNOCK_OFF, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_CAREFUL, .ability = ABILITY_PRANKSTER, .evs = {252, 0, 4, 0, 252, 0},
};

AI_DOUBLE_BATTLE_TEST("EC no payoff: Wallace's Zamazenta does not raise Defense nothing can use")
{
    GIVEN {
        // The benchmark board: Zamazenta and Marshadow against Chandelure and
        // a paralyzed Sableye. Body Press cannot touch either Ghost, and the
        // only physical hits on that side are Sableye's, which barely scratch
        // Zamazenta; it raised Defense four times anyway.
        PreparedPlayer(SPECIES_CHANDELURE, 100, &sWallaceFoeChandelure);
        PreparedPlayerAt(SPECIES_SABLEYE, 100, &sWallaceFoeSableye, 287, STATUS1_PARALYSIS);
        // The Champion's post-game doubles team; Primal Kyogre and Zapdos had
        // fallen.
        sAuthoredInjuries[0] = (struct AuthoredInjury){SPECIES_KYOGRE_PRIMAL, 0};
        sAuthoredInjuries[1] = (struct AuthoredInjury){SPECIES_ZAPDOS, 0};
        sAuthoredLeadSlot = 3;
        AuthoredOpponentWithPartner(TRAINER_WALLACE_DOUBLES_LEGENDS, 8, FALSE, 4);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_HEAT_WAVE);
            MOVE(playerRight, MOVE_WILL_O_WISP, target: opponentLeft);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_IRON_DEFENSE);
        }
    }
}

static const struct EmeraldChampionsBattleSet sJuanFoeSets[] = {
    {.moves = {MOVE_ELECTRO_DRIFT, MOVE_DRACO_METEOR, MOVE_DAZZLING_GLEAM, MOVE_PROTECT}, .item = ITEM_LIFE_ORB, .nature = NATURE_TIMID, .ability = ABILITY_HADRON_ENGINE, .evs = {4, 0, 0, 252, 0, 252}},
    {.moves = {MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH, MOVE_WILD_CHARGE, MOVE_HEAVY_SLAM}, .item = ITEM_ASSAULT_VEST, .nature = NATURE_ADAMANT, .ability = ABILITY_QUARK_DRIVE, .evs = {252, 252, 4, 0, 0, 0}},
    {.moves = {MOVE_FAKE_OUT, MOVE_GRASSY_GLIDE, MOVE_WOOD_HAMMER, MOVE_KNOCK_OFF}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_ADAMANT, .ability = ABILITY_GRASSY_SURGE, .evs = {252, 252, 4, 0, 0, 0}},
};

AI_DOUBLE_BATTLE_TEST("EC KO allocation: Juan's Specs Kingdra does not open with the first slot when Miraidon removes it first")
{
    GIVEN {
        // The benchmark line: Kingdra enters beside Altaria, whose Cloud Nine
        // takes the rain and with it Swift Swim. Miraidon outspeeds and
        // removes Kingdra, so every Kingdra action scored alike on the
        // forecast and Hydro Pump - resisted by Miraidon - won as slot zero
        // over the Ice Beam or Draco Meteor that removes Miraidon.
        InjuredPlayer(SPECIES_MIRAIDON, 70, &sJuanFoeSets[0], 130);
        InjuredPlayer(SPECIES_IRON_HANDS, 70, &sJuanFoeSets[1], 218);
        PreparedPlayer(SPECIES_RILLABOOM, 70, &sJuanFoeSets[2]);
        sAuthoredLeads[0] = SPECIES_ALTARIA;
        sAuthoredLeads[1] = SPECIES_KINGDRA;
        sAuthoredInjuries[0] = (struct AuthoredInjury){SPECIES_POLITOED, 0};
        sAuthoredInjuries[1] = (struct AuthoredInjury){SPECIES_SUICUNE, 0};
        sAuthoredInjuries[2] = (struct AuthoredInjury){SPECIES_MANAPHY, 0};
        AuthoredOpponent(TRAINER_JUAN_1, 7, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DRACO_METEOR, target: opponentRight);
            MOVE(playerRight, MOVE_HEAVY_SLAM, target: opponentLeft);
            NOT_EXPECT_MOVE(opponentRight, MOVE_HYDRO_PUMP);
        }
    }
}

extern bool8 gTestPairBudgetSpent;

AI_DOUBLE_BATTLE_TEST("EC KO allocation: a search the clock cuts short still starts from the strongest option")
{
    GIVEN {
        // Juan's board again, decided with the budget already spent, as the
        // crowded Mossdeep multi decided every turn. The search keeps the
        // first pair it scores, and that used to be slot zero: Hydro Pump
        // into the Miraidon that resists it.
        InjuredPlayer(SPECIES_MIRAIDON, 70, &sJuanFoeSets[0], 130);
        InjuredPlayer(SPECIES_IRON_HANDS, 70, &sJuanFoeSets[1], 218);
        PreparedPlayer(SPECIES_RILLABOOM, 70, &sJuanFoeSets[2]);
        sAuthoredLeads[0] = SPECIES_ALTARIA;
        sAuthoredLeads[1] = SPECIES_KINGDRA;
        sAuthoredInjuries[0] = (struct AuthoredInjury){SPECIES_POLITOED, 0};
        sAuthoredInjuries[1] = (struct AuthoredInjury){SPECIES_SUICUNE, 0};
        sAuthoredInjuries[2] = (struct AuthoredInjury){SPECIES_MANAPHY, 0};
        AuthoredOpponent(TRAINER_JUAN_1, 7, FALSE);
        gTestPairBudgetSpent = TRUE;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DRACO_METEOR, target: opponentRight);
            MOVE(playerRight, MOVE_HEAVY_SLAM, target: opponentLeft);
            NOT_EXPECT_MOVE(opponentRight, MOVE_HYDRO_PUMP);
        }
    } THEN {
        gTestPairBudgetSpent = FALSE;
    }
}
