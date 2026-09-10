// Temporary catalogue review evidence, not a retained regression suite.
#line 9000 "work/preset_ev_review.inc"
TEST("EC preset EV review: offense axes preserve intended stats")
{
    static const enum Species species[] = {SPECIES_BEEDRILL, SPECIES_TANGELA, SPECIES_AVALUGG_HISUI};
    static const u8 choices[] = {1, 1, 2};
    static const enum Move identifyingMove[] = {MOVE_TAILWIND, MOVE_SLEEP_POWDER, MOVE_WIDE_GUARD};
    u32 gains[3] = {0};
    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct EmeraldChampionsBattleSet *preset = GetEmeraldChampionsRawBattleSet(species[i], choices[i]);
        EXPECT_EQ(preset->moves[0], identifyingMove[i]);
        struct EmeraldChampionsBattleSet previous = *preset;
        if (i == 0)
        {
            previous.nature = NATURE_JOLLY;
            previous.evs[1] = 252;
            previous.evs[3] = 0;
            EXPECT_EQ(preset->nature, NATURE_TIMID);
            EXPECT_EQ(preset->moves[1], MOVE_ENDEAVOR);
            EXPECT_EQ(preset->moves[2], MOVE_ELECTROWEB);
        }
        else
        {
            previous.evs[4] = 0;
            previous.evs[i == 1 ? 3 : 1] = 4;
            EXPECT_EQ(preset->evs[4], 4);
        }
        for (u32 level = 2; level <= 100; level++)
        {
            struct Pokemon before, after;
            CreateRandomMonWithIVs(&before, species[i], level, MAX_PER_STAT_IVS);
            after = before;
            ApplyEmeraldChampionsScriptedSet(&before, &previous);
            ApplyEmeraldChampionsScriptedSet(&after, preset);
            EXPECT_EQ(GetMonData(&before, MON_DATA_MAX_HP), GetMonData(&after, MON_DATA_MAX_HP));
            EXPECT_EQ(GetMonData(&before, MON_DATA_DEF), GetMonData(&after, MON_DATA_DEF));
            EXPECT_EQ(GetMonData(&before, MON_DATA_SPEED), GetMonData(&after, MON_DATA_SPEED));
            u32 improved = i == 0 ? MON_DATA_SPATK : MON_DATA_SPDEF;
            EXPECT(GetMonData(&after, improved) >= GetMonData(&before, improved));
            gains[i] += GetMonData(&after, improved) > GetMonData(&before, improved);
            if (i == 1)
                EXPECT_EQ(GetMonData(&before, MON_DATA_ATK), GetMonData(&after, MON_DATA_ATK));
            if (i == 2)
                EXPECT_EQ(GetMonData(&before, MON_DATA_SPATK), GetMonData(&after, MON_DATA_SPATK));
        }
        EXPECT(gains[i] > 0);
    }
    Test_MgbaPrintf("Preset offense EV review levels2-100 gains BeedrillSpA=%d TangelaSpDef=%d HisuiAvaluggSpDef=%d; HP Def Speed preserved", gains[0], gains[1], gains[2]);
}

TEST("EC preset EV review: slow Perish and Sash offense follow native stats")
{
    const struct EmeraldChampionsBattleSet *wiggly = NULL;
    const struct EmeraldChampionsBattleSet *applin = NULL;
    const struct EmeraldChampionsBattleSet *rolycoly = NULL;
    for (u32 i = 0; i < GetEmeraldChampionsRawBattleSetCount(SPECIES_WIGGLYTUFF); i++)
    {
        const struct EmeraldChampionsBattleSet *p = GetEmeraldChampionsRawBattleSet(SPECIES_WIGGLYTUFF, i);
        if (p->moves[0] == MOVE_FAKE_OUT && p->moves[1] == MOVE_PERISH_SONG)
            wiggly = p;
    }
    for (u32 i = 0; i < GetEmeraldChampionsRawBattleSetCountForFormat(SPECIES_APPLIN, EC_BATTLE_FORMAT_SINGLES); i++)
    {
        const struct EmeraldChampionsBattleSet *p = GetEmeraldChampionsRawBattleSetForFormat(SPECIES_APPLIN, i, EC_BATTLE_FORMAT_SINGLES);
        if (p->item == ITEM_FOCUS_SASH)
            applin = p;
    }
    for (u32 i = 0; i < GetEmeraldChampionsRawBattleSetCount(SPECIES_ROLYCOLY); i++)
    {
        const struct EmeraldChampionsBattleSet *p = GetEmeraldChampionsRawBattleSet(SPECIES_ROLYCOLY, i);
        if (p->item == ITEM_WEAKNESS_POLICY)
            rolycoly = p;
    }
    EXPECT(wiggly != NULL && applin != NULL && rolycoly != NULL);
    EXPECT_EQ(wiggly->evs[0], 4);
    EXPECT_EQ(wiggly->evs[5], 0);
    EXPECT_EQ(applin->nature, NATURE_RASH);
    EXPECT_EQ(rolycoly->moves[1], MOVE_ROCK_BLAST);
    struct EmeraldChampionsBattleSet oldWiggly = *wiggly, oldApplin = *applin;
    oldWiggly.evs[0] = 0;
    oldWiggly.evs[5] = 4;
    oldApplin.nature = NATURE_QUIET;
    u32 hpGains = 0, speedGains = 0;
    for (u32 level = 3; level <= 100; level++)
    {
        struct Pokemon before, after;
        CreateRandomMonWithIVs(&before, SPECIES_WIGGLYTUFF, level, MAX_PER_STAT_IVS);
        after = before;
        ApplyEmeraldChampionsScriptedSet(&before, &oldWiggly);
        ApplyEmeraldChampionsScriptedSet(&after, wiggly);
        EXPECT(GetMonData(&after, MON_DATA_MAX_HP) >= GetMonData(&before, MON_DATA_MAX_HP));
        EXPECT(GetMonData(&after, MON_DATA_SPEED) <= GetMonData(&before, MON_DATA_SPEED));
        hpGains += GetMonData(&after, MON_DATA_MAX_HP) > GetMonData(&before, MON_DATA_MAX_HP);
        for (u32 stat = 1; stat <= 4; stat++)
            EXPECT_EQ(GetMonData(&before, EC_STAT_VALUE_DATA(stat)), GetMonData(&after, EC_STAT_VALUE_DATA(stat)));
        CreateRandomMonWithIVs(&before, SPECIES_APPLIN, level, MAX_PER_STAT_IVS);
        after = before;
        ApplyEmeraldChampionsScriptedSet(&before, &oldApplin);
        ApplyEmeraldChampionsScriptedSet(&after, applin);
        EXPECT(GetMonData(&after, MON_DATA_SPEED) >= GetMonData(&before, MON_DATA_SPEED));
        EXPECT(GetMonData(&after, MON_DATA_SPDEF) <= GetMonData(&before, MON_DATA_SPDEF));
        speedGains += GetMonData(&after, MON_DATA_SPEED) > GetMonData(&before, MON_DATA_SPEED);
        for (u32 stat = 0; stat <= 3; stat++)
            EXPECT_EQ(GetMonData(&before, EC_STAT_VALUE_DATA(stat)), GetMonData(&after, EC_STAT_VALUE_DATA(stat)));
    }
    EXPECT(hpGains > 0 && speedGains > 0);
    Test_MgbaPrintf("Preset EV review levels3-100 WigglytuffHPgainLevels=%d ApplinSpeedGainLevels=%d; Applin deliberately trades SpDef", hpGains, speedGains);
}
