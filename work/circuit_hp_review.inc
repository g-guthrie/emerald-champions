#line 24000 "work/circuit_hp_review.inc"
// HISTORICAL: superseded by the universal cutoff+1 HP policy. The even-HP
// assertion below reproduces the old finding; it is not a current requirement.
// Temporary native diagnostic, included at the end of champions_circuit.c.
// Exercise actual opponent construction without adding a production test API.
#include "test/test.h"
#include "item.h"

void RunCircuitHpReview(void)
{
    static const u8 levels[] = {14, 96, 98, 100, 101, 150, 200, 255};
    u32 inspected = 0;
    bool32 sawLinoone = FALSE;
    bool32 sawSitrus = FALSE;

    VarSet(VAR_CHAMPIONS_CIRCUIT_ACTIVE, TRUE);
    for (u32 v = 0; v < ARRAY_COUNT(gShowdownCircuitVariants); v++)
    {
        const struct ShowdownCircuitVariant *variant = &gShowdownCircuitVariants[v];
        for (u32 t = variant->templateOffset; t < variant->templateOffset + variant->templateCount; t++)
        {
            const struct ShowdownCircuitTemplate *template = &gShowdownCircuitTemplates[t];
            struct CircuitGeneratedSet set = {0};
            bool32 drum = FALSE;
            for (u32 m = 0; m < MAX_MON_MOVES; m++)
                drum |= template->moves[m] == MOVE_BELLY_DRUM;
            if (!template->authored || !drum)
                continue;
            bool32 needsEven = template->item != ITEM_SITRUS_BERRY
                && (GetItemHoldEffect(template->item) == HOLD_EFFECT_RESTORE_HP
                 || (GetItemHoldEffect(template->item) == HOLD_EFFECT_CONFUSE_FLAVOR
                  && template->abilities[0] == ABILITY_GLUTTONY));
            if (!needsEven && template->item != ITEM_SITRUS_BERRY)
                continue;

            set.variantIndex = v;
            set.ability = template->abilities[0];
            set.item = template->item;
            set.nature = template->nature;
            memcpy(set.moves, template->moves, sizeof(set.moves));
            memcpy(set.evs, template->evs, sizeof(set.evs));
            sawLinoone |= variant->partySpecies == SPECIES_LINOONE;
            sawSitrus |= set.item == ITEM_SITRUS_BERRY;
            for (u32 l = 0; l < ARRAY_COUNT(levels); l++)
            {
                struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][0];
                u32 rawHp = CalculateSpeciesStat(variant->partySpecies, set.nature,
                    STAT_HP, levels[l], set.evs[STAT_HP]);
                ZeroEnemyPartyMons();
                CreateCircuitMon(mon, &set, levels[l]);
                u32 hp = GetMonData(mon, MON_DATA_MAX_HP);
                u32 evTotal = 0;
                Test_MgbaPrintf("CIRCUIT_HP species=%S template=%d level=%d item=%S rawHp=%d postHp=%d hpEV=%d",
                    GetSpeciesName(variant->partySpecies), t, levels[l],
                    GetItemName(set.item), rawHp, hp, GetMonData(mon, MON_DATA_HP_EV));
                EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), levels[l]);
                EXPECT_EQ(GetMonData(mon, MON_DATA_HP), hp);
                if (needsEven)
                    EXPECT_EQ(hp % 2, 0);
                else
                    EXPECT_EQ(hp, rawHp);
                for (u32 stat = 0; stat < NUM_STATS; stat++)
                {
                    u32 ev = GetMonData(mon, MON_DATA_HP_EV + stat);
                    if (stat != STAT_HP || !needsEven)
                        EXPECT_EQ(ev, set.evs[stat]);
                    evTotal += ev;
                }
                EXPECT_LE(evTotal, MAX_TOTAL_EVS);
                inspected++;
            }
        }
    }
    EXPECT(sawLinoone);
    EXPECT(sawSitrus);
    Test_MgbaPrintf("CIRCUIT_HP applications=%d", inspected);
    VarSet(VAR_CHAMPIONS_CIRCUIT_ACTIVE, FALSE);
}
