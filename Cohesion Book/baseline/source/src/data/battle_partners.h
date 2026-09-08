//
// DO NOT MODIFY THIS FILE! It is auto-generated from src/data/battle_partners.party
//
// If you want to modify this file see expansion PR #7154
//

#line 1 "src/data/battle_partners.party"

#line 1
    [DIFFICULTY_NORMAL][PARTNER_NONE] =
    {
#line 3
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 4
        .trainerPic = TRAINER_PIC_BRENDAN,
#line 5
        .gender = TRAINER_GENDER_MALE,
#line 6
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
        .partySize = 0,
        .party = (const struct TrainerMon[])
        {
        },
    },
#line 8
    [DIFFICULTY_NORMAL][PARTNER_STEVEN] =
    {
#line 9
        .trainerName = _("STEVEN"),
#line 10
        .trainerClass = TRAINER_CLASS_RIVAL,
#line 11
        .trainerPic = TRAINER_PIC_STEVEN,
#line 12
        .gender = TRAINER_GENDER_MALE,
#line 13
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 14
        .aiFlags = AI_FLAG_SMART_TRAINER | AI_FLAG_PREDICTION | AI_FLAG_HP_AWARE | AI_FLAG_SMART_SWITCHING | AI_FLAG_PREDICT_SWITCH | AI_FLAG_PREDICT_INCOMING_MON | AI_FLAG_ASSUME_STAB | AI_FLAG_ASSUME_STATUS_MOVES,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
        .partySize = 3,
        .party = (const struct TrainerMon[])
        {
            {
#line 16
            .species = SPECIES_METAGROSS,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 16
            .heldItem = ITEM_ASSAULT_VEST,
#line 21
            .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
#line 20
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 17
            .ability = ABILITY_CLEAR_BODY,
#line 19
            .lvl = 70,
            .ball = POKEBALL_COUNT,
#line 18
            .nature = NATURE_ADAMANT,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            .moves = {
#line 22
                MOVE_BULLET_PUNCH,
                MOVE_HEAVY_SLAM,
                MOVE_PSYCHIC_FANGS,
                MOVE_STOMPING_TANTRUM,
            },
            },
            {
#line 27
            .species = SPECIES_SKARMORY,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 27
            .heldItem = ITEM_ROCKY_HELMET,
#line 32
            .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
#line 31
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 28
            .ability = ABILITY_STURDY,
#line 30
            .lvl = 70,
            .ball = POKEBALL_COUNT,
#line 29
            .nature = NATURE_IMPISH,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            .moves = {
#line 33
                MOVE_TAILWIND,
                MOVE_BODY_PRESS,
                MOVE_BRAVE_BIRD,
                MOVE_PROTECT,
            },
            },
            {
#line 38
            .species = SPECIES_AGGRON,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 38
            .heldItem = ITEM_AIR_BALLOON,
#line 43
            .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
#line 42
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 39
            .ability = ABILITY_ROCK_HEAD,
#line 41
            .lvl = 70,
            .ball = POKEBALL_COUNT,
#line 40
            .nature = NATURE_ADAMANT,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            .moves = {
#line 44
                MOVE_HEAD_SMASH,
                MOVE_HEAVY_SLAM,
                MOVE_HIGH_HORSEPOWER,
                MOVE_PROTECT,
            },
            },
        },
    },
