#line 59000 "work/stat_point_scaling_probe.inc"

// One native stat regression, not a battle or trainer-design assertion.
// Literal expectations cover low-level rounding, level-50 point semantics,
// level-100 scaling, Nature, and effective perfect IVs despite stored IV zero.
TEST("EC Stat Points: level scaling, Nature, perfect effective IVs and HP invariants")
{
    static const u8 levels[] = {5, 12, 50, 100};
    static const u8 points[] = {0, 1, 2, 32};
    static const u16 attack[][4] = {
        {9, 9, 9, 13},
        {16, 17, 17, 25},
        {55, 56, 57, 90},
        {105, 106, 108, 174},
    };
    static const u16 hp[][4] = {
        {20, 20, 20, 23},
        {34, 34, 35, 42},
        {113, 114, 115, 145},
        {217, 218, 220, 280},
    };
    struct Pokemon mon;
    u8 zero = 0;
    u8 maxPoints = 32;
    u8 nature = NATURE_ADAMANT;
    u16 faintedHp = 0;

    for (u32 level = 0; level < ARRAY_COUNT(levels); level++)
    {
        for (u32 investment = 0; investment < ARRAY_COUNT(points); investment++)
        {
            CreateRandomMonWithIVs(&mon, SPECIES_ZIGZAGOON, levels[level], 0);
            for (u32 stat = 0; stat < NUM_STATS; stat++)
                SetMonData(&mon, MON_DATA_HP_EV + stat, &zero);
            SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);
            SetMonData(&mon, MON_DATA_HP_EV, &points[investment]);
            SetMonData(&mon, MON_DATA_ATK_EV, &points[investment]);
            CalculateMonStats(&mon);

            EXPECT_EQ(GetMonData(&mon, MON_DATA_HP_IV), 0);
            EXPECT_EQ(GetMonData(&mon, MON_DATA_ATK_IV), 0);
            EXPECT_EQ(GetMonData(&mon, MON_DATA_ATK), attack[level][investment]);
            EXPECT_EQ(GetMonData(&mon, MON_DATA_MAX_HP), hp[level][investment]);
            EXPECT_EQ(GetMonData(&mon, MON_DATA_HP), hp[level][investment]);
        }
    }

    // Recalculation must not revive a fainted Pokemon when its maximum rises.
    SetMonData(&mon, MON_DATA_HP_EV, &zero);
    CalculateMonStats(&mon);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MAX_HP), 217);
    SetMonData(&mon, MON_DATA_HP, &faintedHp);
    SetMonData(&mon, MON_DATA_HP_EV, &maxPoints);
    CalculateMonStats(&mon);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MAX_HP), 280);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HP), 0);

    CreateRandomMonWithIVs(&mon, SPECIES_SHEDINJA, 100, 0);
    for (u32 stat = 0; stat < NUM_STATS; stat++)
        SetMonData(&mon, MON_DATA_HP_EV + stat, &zero);
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);
    SetMonData(&mon, MON_DATA_HP_EV, &maxPoints);
    CalculateMonStats(&mon);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MAX_HP), 1);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HP), 1);
}
