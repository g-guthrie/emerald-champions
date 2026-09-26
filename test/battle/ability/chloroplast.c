#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Chloroplast fires Solar Beam and Solar Blade without a charge turn")
{
    enum Move move;
    enum Ability ability;

    PARAMETRIZE { move = MOVE_SOLAR_BEAM; ability = ABILITY_CHLOROPLAST; }
    PARAMETRIZE { move = MOVE_SOLAR_BEAM; ability = ABILITY_CHLOROPHYLL; }
    PARAMETRIZE { move = MOVE_SOLAR_BLADE; ability = ABILITY_CHLOROPLAST; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SOLAR_BEAM) == EFFECT_SOLAR_BEAM);
        ASSUME(GetMoveEffect(MOVE_SOLAR_BLADE) == EFFECT_SOLAR_BEAM);
        PLAYER(SPECIES_EXEGGUTOR) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
        if (ability != ABILITY_CHLOROPLAST)
            TURN { SKIP_TURN(player); }
    } SCENE {
        if (ability == ABILITY_CHLOROPLAST) {
            // The charge message still plays, but the move fires on the same turn.
            MESSAGE("Exeggutor absorbed light!");
            ABILITY_POPUP(player, ABILITY_CHLOROPLAST);
            ANIMATION(ANIM_TYPE_MOVE, move, player);
            HP_BAR(opponent);
            MESSAGE("The opposing Wobbuffet used Celebrate!");
        } else {
            MESSAGE("Exeggutor absorbed light!");
            MESSAGE("The opposing Wobbuffet used Celebrate!");
            ANIMATION(ANIM_TYPE_MOVE, move, player);
            HP_BAR(opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Chloroplast keeps Solar Beam at full power in rain", s16 damage)
{
    enum Ability ability;

    PARAMETRIZE { ability = ABILITY_CHLOROPHYLL; }
    PARAMETRIZE { ability = ABILITY_CHLOROPLAST; }

    GIVEN {
        PLAYER(SPECIES_EXEGGUTOR) { Ability(ability); Item(ITEM_POWER_HERB); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_RAIN_DANCE); MOVE(player, MOVE_SOLAR_BEAM); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAIN_DANCE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SOLAR_BEAM, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Chloroplast makes Growth raise Attack and Sp. Atk by two stages")
{
    enum Ability ability;

    PARAMETRIZE { ability = ABILITY_CHLOROPHYLL; }
    PARAMETRIZE { ability = ABILITY_CHLOROPLAST; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_GROWTH) == EFFECT_GROWTH);
        PLAYER(SPECIES_EXEGGUTOR) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_GROWTH); }
    } THEN {
        u32 stages = (ability == ABILITY_CHLOROPLAST) ? 2 : 1;
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + stages);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + stages);
    }
}

SINGLE_BATTLE_TEST("Chloroplast makes Synthesis heal as in sunlight, even in rain")
{
    enum Ability ability;
    bool32 rain;

    PARAMETRIZE { ability = ABILITY_CHLOROPHYLL; rain = FALSE; }
    PARAMETRIZE { ability = ABILITY_CHLOROPLAST; rain = FALSE; }
    PARAMETRIZE { ability = ABILITY_CHLOROPHYLL; rain = TRUE; }
    PARAMETRIZE { ability = ABILITY_CHLOROPLAST; rain = TRUE; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SYNTHESIS) == EFFECT_SYNTHESIS);
        PLAYER(SPECIES_EXEGGUTOR) { Ability(ability); HP(1); MaxHP(300); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, rain ? MOVE_RAIN_DANCE : MOVE_CELEBRATE); MOVE(player, MOVE_SYNTHESIS); }
    } THEN {
        if (ability == ABILITY_CHLOROPLAST)
            EXPECT_EQ(player->hp, 1 + 200);
        else if (rain)
            EXPECT_EQ(player->hp, 1 + 75);
        else
            EXPECT_EQ(player->hp, 1 + 150);
    }
}

SINGLE_BATTLE_TEST("Chloroplast leaves Fire damage and Weather Ball to the real weather", s16 damage)
{
    enum Ability ability;
    enum Move move;

    PARAMETRIZE { ability = ABILITY_CHLOROPHYLL; move = MOVE_FLAMETHROWER; }
    PARAMETRIZE { ability = ABILITY_CHLOROPLAST; move = MOVE_FLAMETHROWER; }
    PARAMETRIZE { ability = ABILITY_CHLOROPHYLL; move = MOVE_WEATHER_BALL; }
    PARAMETRIZE { ability = ABILITY_CHLOROPLAST; move = MOVE_WEATHER_BALL; }

    GIVEN {
        PLAYER(SPECIES_EXEGGUTOR) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

// Both moves knock out. Not charging outranks accuracy in the AI's tie-break,
// so Leaf Storm wins only while Solar Beam still needs its charge turn.
AI_SINGLE_BATTLE_TEST("AI treats a Chloroplast Solar Beam as a one-turn attack")
{
    enum Ability ability;

    PARAMETRIZE { ability = ABILITY_CHLOROPHYLL; }
    PARAMETRIZE { ability = ABILITY_CHLOROPLAST; }

    GIVEN {
        ASSUME(GetMoveAccuracy(MOVE_SOLAR_BEAM) > GetMoveAccuracy(MOVE_LEAF_STORM));
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_EXEGGUTOR) { Ability(ability); Moves(MOVE_SOLAR_BEAM, MOVE_LEAF_STORM); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, ability == ABILITY_CHLOROPLAST ? MOVE_SOLAR_BEAM : MOVE_LEAF_STORM); SEND_OUT(player, 1); }
    }
}
