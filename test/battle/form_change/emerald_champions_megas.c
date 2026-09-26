#include "global.h"
#include "test/battle.h"
#include "battle_util.h"
#include "battle_controllers.h"
#include "battle_gimmick.h"
#include "battle_setup.h"
#include "data.h"
#include "battle_ai_util.h"
#include "emerald_champions_battle_plan.h"
#include "constants/opponents.h"

DOUBLE_BATTLE_TEST("Emerald Champions v4 Mega permissions constrain native eligibility and AI form forecasts")
{
    enum Species species;
    enum Item item;
    u32 slot;
    bool32 allowed;
    PARAMETRIZE { species = SPECIES_METAGROSS; item = ITEM_METAGROSSITE; allowed = TRUE; }
    PARAMETRIZE { species = SPECIES_RAYQUAZA; item = ITEM_FOCUS_SASH; allowed = FALSE; }
    GIVEN {
        PLAYER(SPECIES_MAGIKARP) { Moves(MOVE_SPLASH); }
        PLAYER(SPECIES_MAGIKARP) { Moves(MOVE_SPLASH); }
        OPPONENT(species) { Item(item); Moves(MOVE_FALSE_SWIPE, MOVE_DRAGON_ASCENT); }
        OPPONENT(SPECIES_MAGIKARP) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SPLASH); MOVE(playerRight, MOVE_SPLASH); MOVE(opponentLeft, MOVE_FALSE_SWIPE, target: playerLeft); MOVE(opponentRight, MOVE_SPLASH); }
    } THEN {
        TrainerBattleParameter savedParams = gTrainerBattleParameter;
        u32 savedFlags = gBattleTypeFlags;
        u32 savedSlot = gBattlerPartyIndexes[B_BATTLER_1];
        // Ordinary DOUBLE_BATTLE_TEST fixtures replay link controllers.
        // Explicitly enter the campaign namespace for this policy check.
        gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_IS_MASTER;
        TRAINER_BATTLE_PARAM.opponentA = TRAINER_STEVEN;
        const struct TrainerMon *party = GetTrainerPartyFromId(TRAINER_STEVEN);
        for (slot = 0; slot < GetTrainerPartySizeFromId(TRAINER_STEVEN); slot++)
            if (party[slot].species == species)
                break;
        EXPECT(slot < GetTrainerPartySizeFromId(TRAINER_STEVEN));
        gBattlerPartyIndexes[B_BATTLER_1] = slot;
        EXPECT_EQ(GetBattlerTrainer(B_BATTLER_1), B_TRAINER_OPPONENT_A);
        EXPECT_EQ(EmeraldChampions_IsMegaAllowed(B_BATTLER_1), allowed);
        EXPECT_EQ(CanMegaEvolve(B_BATTLER_1), allowed);
        if (!allowed)
        {
            EXPECT(!AI_ApplyMegaForm(B_BATTLER_1));
            EXPECT_EQ(opponentLeft->species, SPECIES_RAYQUAZA);
        }
        // An unauthored opponent retains ordinary native item/move eligibility.
        TRAINER_BATTLE_PARAM.opponentA = TRAINER_NONE;
        EXPECT(CanMegaEvolve(B_BATTLER_1));
        gBattlerPartyIndexes[B_BATTLER_1] = savedSlot;
        gBattleTypeFlags = savedFlags;
        gTrainerBattleParameter = savedParams;
    }
}

SINGLE_BATTLE_TEST("Every Emerald Champions Mega transforms with its native requirement")
{
    enum Species base, mega;
    enum Item item;
    enum Move requiredMove;
    PARAMETRIZE { base = SPECIES_VENUSAUR; mega = SPECIES_VENUSAUR_MEGA; item = ITEM_VENUSAURITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_CHARIZARD; mega = SPECIES_CHARIZARD_MEGA_X; item = ITEM_CHARIZARDITE_X; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_CHARIZARD; mega = SPECIES_CHARIZARD_MEGA_Y; item = ITEM_CHARIZARDITE_Y; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_BLASTOISE; mega = SPECIES_BLASTOISE_MEGA; item = ITEM_BLASTOISINITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_BUTTERFREE; mega = SPECIES_BUTTERFREE_MEGA; item = ITEM_BUTTERFRENITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_BEEDRILL; mega = SPECIES_BEEDRILL_MEGA; item = ITEM_BEEDRILLITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_PIDGEOT; mega = SPECIES_PIDGEOT_MEGA; item = ITEM_PIDGEOTITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_RAICHU; mega = SPECIES_RAICHU_MEGA_X; item = ITEM_RAICHUNITE_X; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_RAICHU; mega = SPECIES_RAICHU_MEGA_Y; item = ITEM_RAICHUNITE_Y; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_CLEFABLE; mega = SPECIES_CLEFABLE_MEGA; item = ITEM_CLEFABLITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_ALAKAZAM; mega = SPECIES_ALAKAZAM_MEGA; item = ITEM_ALAKAZITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_MACHAMP; mega = SPECIES_MACHAMP_MEGA; item = ITEM_MACHAMPITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_VICTREEBEL; mega = SPECIES_VICTREEBEL_MEGA; item = ITEM_VICTREEBELITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_SLOWBRO; mega = SPECIES_SLOWBRO_MEGA; item = ITEM_SLOWBRONITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_GENGAR; mega = SPECIES_GENGAR_MEGA; item = ITEM_GENGARITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_STEELIX; mega = SPECIES_STEELIX_MEGA; item = ITEM_STEELIXITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_KINGLER; mega = SPECIES_KINGLER_MEGA; item = ITEM_KINGLERITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_KANGASKHAN; mega = SPECIES_KANGASKHAN_MEGA; item = ITEM_KANGASKHANITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_STARMIE; mega = SPECIES_STARMIE_MEGA; item = ITEM_STARMINITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_SCIZOR; mega = SPECIES_SCIZOR_MEGA; item = ITEM_SCIZORITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_PINSIR; mega = SPECIES_PINSIR_MEGA; item = ITEM_PINSIRITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_GYARADOS; mega = SPECIES_GYARADOS_MEGA; item = ITEM_GYARADOSITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_LAPRAS; mega = SPECIES_LAPRAS_MEGA; item = ITEM_LAPRASITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_AERODACTYL; mega = SPECIES_AERODACTYL_MEGA; item = ITEM_AERODACTYLITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_DRAGONITE; mega = SPECIES_DRAGONITE_MEGA; item = ITEM_DRAGONINITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_MEWTWO; mega = SPECIES_MEWTWO_MEGA_X; item = ITEM_MEWTWONITE_X; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_MEWTWO; mega = SPECIES_MEWTWO_MEGA_Y; item = ITEM_MEWTWONITE_Y; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_MEGANIUM; mega = SPECIES_MEGANIUM_MEGA; item = ITEM_MEGANIUMITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_FERALIGATR; mega = SPECIES_FERALIGATR_MEGA; item = ITEM_FERALIGITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_AMPHAROS; mega = SPECIES_AMPHAROS_MEGA; item = ITEM_AMPHAROSITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_HERACROSS; mega = SPECIES_HERACROSS_MEGA; item = ITEM_HERACRONITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_SKARMORY; mega = SPECIES_SKARMORY_MEGA; item = ITEM_SKARMORITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_HOUNDOOM; mega = SPECIES_HOUNDOOM_MEGA; item = ITEM_HOUNDOOMINITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_TYRANITAR; mega = SPECIES_TYRANITAR_MEGA; item = ITEM_TYRANITARITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_SCEPTILE; mega = SPECIES_SCEPTILE_MEGA; item = ITEM_SCEPTILITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_BLAZIKEN; mega = SPECIES_BLAZIKEN_MEGA; item = ITEM_BLAZIKENITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_SWAMPERT; mega = SPECIES_SWAMPERT_MEGA; item = ITEM_SWAMPERTITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_GARDEVOIR; mega = SPECIES_GARDEVOIR_MEGA; item = ITEM_GARDEVOIRITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_GALLADE; mega = SPECIES_GALLADE_MEGA; item = ITEM_GALLADITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_SABLEYE; mega = SPECIES_SABLEYE_MEGA; item = ITEM_SABLENITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_MAWILE; mega = SPECIES_MAWILE_MEGA; item = ITEM_MAWILITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_AGGRON; mega = SPECIES_AGGRON_MEGA; item = ITEM_AGGRONITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_MEDICHAM; mega = SPECIES_MEDICHAM_MEGA; item = ITEM_MEDICHAMITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_MANECTRIC; mega = SPECIES_MANECTRIC_MEGA; item = ITEM_MANECTITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_SHARPEDO; mega = SPECIES_SHARPEDO_MEGA; item = ITEM_SHARPEDONITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_CAMERUPT; mega = SPECIES_CAMERUPT_MEGA; item = ITEM_CAMERUPTITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_ALTARIA; mega = SPECIES_ALTARIA_MEGA; item = ITEM_ALTARIANITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_BANETTE; mega = SPECIES_BANETTE_MEGA; item = ITEM_BANETTITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_CHIMECHO; mega = SPECIES_CHIMECHO_MEGA; item = ITEM_CHIMECHITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_ABSOL; mega = SPECIES_ABSOL_MEGA; item = ITEM_ABSOLITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_ABSOL; mega = SPECIES_ABSOL_MEGA_Z; item = ITEM_ABSOLITE_Z; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_GLALIE; mega = SPECIES_GLALIE_MEGA; item = ITEM_GLALITITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_FROSLASS; mega = SPECIES_FROSLASS_MEGA; item = ITEM_FROSLASSITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_SALAMENCE; mega = SPECIES_SALAMENCE_MEGA; item = ITEM_SALAMENCITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_METAGROSS; mega = SPECIES_METAGROSS_MEGA; item = ITEM_METAGROSSITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_LATIAS; mega = SPECIES_LATIAS_MEGA; item = ITEM_LATIASITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_LATIOS; mega = SPECIES_LATIOS_MEGA; item = ITEM_LATIOSITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_RAYQUAZA; mega = SPECIES_RAYQUAZA_MEGA; item = ITEM_NONE; requiredMove = MOVE_DRAGON_ASCENT; }
    PARAMETRIZE { base = SPECIES_STARAPTOR; mega = SPECIES_STARAPTOR_MEGA; item = ITEM_STARAPTITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_LOPUNNY; mega = SPECIES_LOPUNNY_MEGA; item = ITEM_LOPUNNITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_GARCHOMP; mega = SPECIES_GARCHOMP_MEGA; item = ITEM_GARCHOMPITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_GARCHOMP; mega = SPECIES_GARCHOMP_MEGA_Z; item = ITEM_GARCHOMPITE_Z; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_LUCARIO; mega = SPECIES_LUCARIO_MEGA; item = ITEM_LUCARIONITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_LUCARIO; mega = SPECIES_LUCARIO_MEGA_Z; item = ITEM_LUCARIONITE_Z; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_ABOMASNOW; mega = SPECIES_ABOMASNOW_MEGA; item = ITEM_ABOMASITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_HEATRAN; mega = SPECIES_HEATRAN_MEGA; item = ITEM_HEATRANITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_DARKRAI; mega = SPECIES_DARKRAI_MEGA; item = ITEM_DARKRANITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_EMBOAR; mega = SPECIES_EMBOAR_MEGA; item = ITEM_EMBOARITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_EXCADRILL; mega = SPECIES_EXCADRILL_MEGA; item = ITEM_EXCADRITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_AUDINO; mega = SPECIES_AUDINO_MEGA; item = ITEM_AUDINITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_SCOLIPEDE; mega = SPECIES_SCOLIPEDE_MEGA; item = ITEM_SCOLIPITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_SCRAFTY; mega = SPECIES_SCRAFTY_MEGA; item = ITEM_SCRAFTINITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_EELEKTROSS; mega = SPECIES_EELEKTROSS_MEGA; item = ITEM_EELEKTROSSITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_CHANDELURE; mega = SPECIES_CHANDELURE_MEGA; item = ITEM_CHANDELURITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_GOLURK; mega = SPECIES_GOLURK_MEGA; item = ITEM_GOLURKITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_CHESNAUGHT; mega = SPECIES_CHESNAUGHT_MEGA; item = ITEM_CHESNAUGHTITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_DELPHOX; mega = SPECIES_DELPHOX_MEGA; item = ITEM_DELPHOXITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_GRENINJA; mega = SPECIES_GRENINJA_MEGA; item = ITEM_GRENINJITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_PYROAR; mega = SPECIES_PYROAR_MEGA; item = ITEM_PYROARITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_FLOETTE_ETERNAL; mega = SPECIES_FLOETTE_MEGA; item = ITEM_FLOETTITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_MEOWSTIC_M; mega = SPECIES_MEOWSTIC_M_MEGA; item = ITEM_MEOWSTICITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_MEOWSTIC_F; mega = SPECIES_MEOWSTIC_F_MEGA; item = ITEM_MEOWSTICITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_MALAMAR; mega = SPECIES_MALAMAR_MEGA; item = ITEM_MALAMARITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_BARBARACLE; mega = SPECIES_BARBARACLE_MEGA; item = ITEM_BARBARACITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_DRAGALGE; mega = SPECIES_DRAGALGE_MEGA; item = ITEM_DRAGALGITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_HAWLUCHA; mega = SPECIES_HAWLUCHA_MEGA; item = ITEM_HAWLUCHANITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_ZYGARDE_COMPLETE; mega = SPECIES_ZYGARDE_MEGA; item = ITEM_ZYGARDITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_DIANCIE; mega = SPECIES_DIANCIE_MEGA; item = ITEM_DIANCITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_CRABOMINABLE; mega = SPECIES_CRABOMINABLE_MEGA; item = ITEM_CRABOMINITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_GOLISOPOD; mega = SPECIES_GOLISOPOD_MEGA; item = ITEM_GOLISOPITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_DRAMPA; mega = SPECIES_DRAMPA_MEGA; item = ITEM_DRAMPANITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_MAGEARNA; mega = SPECIES_MAGEARNA_MEGA; item = ITEM_MAGEARNITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_MAGEARNA_ORIGINAL; mega = SPECIES_MAGEARNA_ORIGINAL_MEGA; item = ITEM_MAGEARNITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_ZERAORA; mega = SPECIES_ZERAORA_MEGA; item = ITEM_ZERAORITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_FALINKS; mega = SPECIES_FALINKS_MEGA; item = ITEM_FALINKSITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_SCOVILLAIN; mega = SPECIES_SCOVILLAIN_MEGA; item = ITEM_SCOVILLAINITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_GLIMMORA; mega = SPECIES_GLIMMORA_MEGA; item = ITEM_GLIMMORANITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_TATSUGIRI_CURLY; mega = SPECIES_TATSUGIRI_CURLY_MEGA; item = ITEM_TATSUGIRINITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_TATSUGIRI_DROOPY; mega = SPECIES_TATSUGIRI_DROOPY_MEGA; item = ITEM_TATSUGIRINITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_TATSUGIRI_STRETCHY; mega = SPECIES_TATSUGIRI_STRETCHY_MEGA; item = ITEM_TATSUGIRINITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_BAXCALIBUR; mega = SPECIES_BAXCALIBUR_MEGA; item = ITEM_BAXCALIBRITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_FLYGON; mega = SPECIES_FLYGON_MEGA; item = ITEM_FLYGONITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_MILOTIC; mega = SPECIES_MILOTIC_MEGA; item = ITEM_MILOTICITE; requiredMove = MOVE_SPLASH; }
    PARAMETRIZE { base = SPECIES_KINGDRA; mega = SPECIES_KINGDRA_MEGA; item = ITEM_KINGDRANITE; requiredMove = MOVE_SPLASH; }
    GIVEN {
        PLAYER(base) { Item(item); Moves(MOVE_CELEBRATE, requiredMove); }
        // Forecast cannot be copied by Mega Alakazam's Trace.
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_FORECAST); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, player);
    } THEN {
        EXPECT_EQ(player->species, mega);
        // The player's Mega reads the Inclement layer (Mega Sceptile: Chloroplast).
        EXPECT_EQ(player->ability, GetSpeciesAbilityForOwner(mega, 0, FALSE));
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), base);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), item);
    }
}

SINGLE_BATTLE_TEST("Mega eligibility after switching respects the held stone and trainer usage")
{
    bool32 usedMega, correctStone;
    PARAMETRIZE { usedMega = FALSE; correctStone = FALSE; }
    PARAMETRIZE { usedMega = FALSE; correctStone = TRUE; }
    PARAMETRIZE { usedMega = TRUE; correctStone = FALSE; }
    PARAMETRIZE { usedMega = TRUE; correctStone = TRUE; }
    GIVEN {
        PLAYER(SPECIES_VENUSAUR) { Item(ITEM_VENUSAURITE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_CHARIZARD) { Item(correctStone ? ITEM_CHARIZARDITE_X : ITEM_VENUSAURITE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MAGIKARP) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, gimmick: usedMega ? GIMMICK_MEGA : GIMMICK_NONE); }
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_CHARIZARD);
        EXPECT_EQ(GetActiveGimmick(B_BATTLER_0), GIMMICK_NONE);
        EXPECT_EQ(HasTrainerUsedGimmick(B_BATTLER_0, GIMMICK_MEGA), usedMega);
        EXPECT_EQ(CanMegaEvolve(B_BATTLER_0), !usedMega && correctStone);
    }
}
