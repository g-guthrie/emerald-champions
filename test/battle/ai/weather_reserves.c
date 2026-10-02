#include "global.h"
#include "test/battle.h"
#include "battle_ai_field_statuses.h"
#include "battle_util.h"

// Each opponent owns a different party. The second owner's switch to slot 1
// must not hide the first owner's Swift Swim reserve in its own slot 1.
MULTI_BATTLE_TEST("EC weather reserves: another owner's active slot does not hide a reserve")
{
    GIVEN {
        PLAYER(SPECIES_CLEFAIRY) { Ability(ABILITY_MAGIC_GUARD); Moves(MOVE_CELEBRATE); }
        PARTNER(SPECIES_CLEFAIRY) { Ability(ABILITY_MAGIC_GUARD); Moves(MOVE_CELEBRATE); }
        OPPONENT_A(SPECIES_SABLEYE) { Ability(ABILITY_KEEN_EYE); Moves(MOVE_CELEBRATE); }
        OPPONENT_A(SPECIES_LUDICOLO) { Ability(ABILITY_SWIFT_SWIM); Moves(MOVE_CELEBRATE); }
        OPPONENT_B(SPECIES_CHANSEY) { Ability(ABILITY_NATURAL_CURE); Moves(MOVE_CELEBRATE); }
        OPPONENT_B(SPECIES_CHANSEY) { Ability(ABILITY_NATURAL_CURE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(opponentRight, 1); }
    } THEN {
        enum BattlerId actor = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        enum BattlerId partner = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
        EXPECT_NE(GetBattlerParty(actor), GetBattlerParty(partner));
        EXPECT_EQ(gBattlerPartyIndexes[actor], 0);
        EXPECT_EQ(gBattlerPartyIndexes[partner], 1);
        EXPECT(AI_ReserveBenefitsFromWeather(actor, B_WEATHER_RAIN));
        EXPECT(!AI_ReserveBenefitsFromWeather(actor, B_WEATHER_SUN));
        // Keep the existing strategy scope: the other owner does not count
        // this Ludicolo as one of its own reserves.
        EXPECT(!AI_ReserveBenefitsFromWeather(partner, B_WEATHER_RAIN));
    }
}

// In ordinary doubles both active slots really belong to the same party.
// A weather beneficiary already on the field must not count as a reserve.
DOUBLE_BATTLE_TEST("EC weather reserves: a shared party's active partner stays excluded")
{
    GIVEN {
        PLAYER(SPECIES_CLEFAIRY) { Ability(ABILITY_MAGIC_GUARD); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_CLEFAIRY) { Ability(ABILITY_MAGIC_GUARD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SABLEYE) { Ability(ABILITY_KEEN_EYE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_LUDICOLO) { Ability(ABILITY_SWIFT_SWIM); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {}
    } THEN {
        enum BattlerId actor = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        enum BattlerId partner = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
        EXPECT_EQ(GetBattlerParty(actor), GetBattlerParty(partner));
        EXPECT_EQ(gBattlerPartyIndexes[actor], 0);
        EXPECT_EQ(gBattlerPartyIndexes[partner], 1);
        EXPECT(!AI_ReserveBenefitsFromWeather(actor, B_WEATHER_RAIN));
        EXPECT(!AI_ReserveBenefitsFromWeather(partner, B_WEATHER_RAIN));
    }
}
