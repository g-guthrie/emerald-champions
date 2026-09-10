#line 28000 "work/berry_policy_controls.inc"
// Temporary exception checks for the universal HP threshold change.
SINGLE_BATTLE_TEST("Berry policy review: Ripen and Unnerve still exclude Berry Juice")
{
    enum Item item = ITEM_SITRUS_BERRY;
    bool32 unnerve = FALSE;
    static const enum Item items[] = {ITEM_SITRUS_BERRY, ITEM_ORAN_BERRY, ITEM_FIGY_BERRY, ITEM_BERRY_JUICE};
    for (u32 i = 0; i < ARRAY_COUNT(items); i++)
    {
        PARAMETRIZE { item = items[i]; unnerve = FALSE; }
        PARAMETRIZE { item = items[i]; unnerve = TRUE; }
    }
    u32 hp = item == ITEM_FIGY_BERRY ? 26 : 51;
    bool32 consumes = !unnerve || item == ITEM_BERRY_JUICE;
    u32 healing = item == ITEM_FIGY_BERRY ? 66 : item == ITEM_SITRUS_BERRY ? 50 : 20;
    GIVEN {
        PLAYER(SPECIES_FLAPPLE) { Ability(ABILITY_RIPEN); Nature(NATURE_HARDY); MaxHP(100); HP(hp); Item(item); }
        OPPONENT(SPECIES_CORVIKNIGHT) { Ability(unnerve ? ABILITY_UNNERVE : ABILITY_PRESSURE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, consumes ? min(100, hp + healing) : hp);
        EXPECT_EQ(player->item, consumes ? ITEM_NONE : item);
    }
}

SINGLE_BATTLE_TEST("Berry policy review: Stuff Cheeks still forces consumption at full HP through Unnerve")
{
    GIVEN {
        PLAYER(SPECIES_GREEDENT) { Ability(ABILITY_CHEEK_POUCH); MaxHP(100); HP(100); Item(ITEM_SALAC_BERRY); }
        OPPONENT(SPECIES_CORVIKNIGHT) { Ability(ABILITY_UNNERVE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STUFF_CHEEKS); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 100);
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 2);
    }
}
