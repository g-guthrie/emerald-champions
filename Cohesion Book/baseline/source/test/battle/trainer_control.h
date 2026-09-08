//
// DO NOT MODIFY THIS FILE! It is auto-generated from test/battle/trainer_control.party
//
// If you want to modify this file see expansion PR #7154
//

#line 1 "test/battle/trainer_control.party"

#line 1
    [DIFFICULTY_NORMAL][TRAINER_NONE] =
    {
#line 3
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 4
        .trainerPic = TRAINER_PIC_HIKER,
#line 5
        .gender = TRAINER_GENDER_MALE,
#line 6
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 7
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
        .partySize = 0,
        .party = (const struct TrainerMon[])
        {
        },
    },
#line 9
    [DIFFICULTY_NORMAL][1] =
    {
#line 10
        .trainerName = _("RED"),
#line 11
        .trainerClass = TRAINER_CLASS_RIVAL,
#line 12
        .trainerPic = TRAINER_PIC_RED,
#line 13
        .gender = TRAINER_GENDER_MALE,
#line 14
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 15
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
        .partySize = 1,
        .party = (const struct TrainerMon[])
        {
            {
#line 17
            .species = SPECIES_CHARMANDER,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 19
            .iv = TRAINER_PARTY_IVS(0, 0, 0, 0, 0, 0),
#line 18
            .lvl = 5,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
#line 21
    [DIFFICULTY_NORMAL][2] =
    {
#line 22
        .trainerName = _("LEAF"),
#line 23
        .trainerClass = TRAINER_CLASS_RIVAL,
#line 24
        .trainerPic = TRAINER_PIC_LEAF,
#line 25
        .gender = TRAINER_GENDER_FEMALE,
#line 26
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 27
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
        .partySize = 1,
        .party = (const struct TrainerMon[])
        {
            {
#line 29
            .species = SPECIES_BULBASAUR,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 31
            .iv = TRAINER_PARTY_IVS(0, 0, 0, 0, 0, 0),
#line 30
            .lvl = 5,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
#line 34
    [DIFFICULTY_NORMAL][3] =
    {
#line 35
        .trainerName = _("Test1"),
#line 36
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 37
        .trainerPic = TRAINER_PIC_RED,
#line 38
        .gender = TRAINER_GENDER_MALE,
#line 39
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 40
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
        .partySize = 3,
        .party = (const struct TrainerMon[])
        {
            {
#line 42
            .nickname = COMPOUND_STRING("Bubbles"),
#line 42
            .species = SPECIES_WOBBUFFET,
#line 42
            .gender = TRAINER_MON_FEMALE,
#line 42
            .heldItem = ITEM_ASSAULT_VEST,
#line 47
            .ev = TRAINER_PARTY_EVS(252, 0, 0, 252, 4, 0),
#line 46
            .iv = TRAINER_PARTY_IVS(25, 26, 27, 28, 29, 30),
#line 45
            .ability = ABILITY_TELEPATHY,
#line 44
            .lvl = 67,
#line 50
            .ball = BALL_MASTER,
#line 48
            .friendship = 42,
#line 43
            .nature = NATURE_HASTY,
#line 49
            .isShiny = TRUE,
#line 51
            .dynamaxLevel = 5,
            .shouldUseDynamax = TRUE,
            .moves = {
#line 52
                MOVE_AIR_SLASH,
                MOVE_BARRIER,
                MOVE_SOLAR_BEAM,
                MOVE_EXPLOSION,
            },
            },
            {
#line 57
            .species = SPECIES_WOBBUFFET,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 61
            .iv = TRAINER_PARTY_IVS(0, 0, 0, 0, 0, 0),
#line 59
            .ability = ABILITY_SHADOW_TAG,
#line 58
            .lvl = 5,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
#line 60
            .dynamaxLevel = 10,
            .shouldUseDynamax = TRUE,
            },
            {
#line 63
            .species = SPECIES_WYNAUT,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 66
            .iv = TRAINER_PARTY_IVS(0, 0, 0, 0, 0, 0),
#line 64
            .lvl = 5,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 65
            .teraType = TYPE_WATER,
            },
        },
    },
#line 68
#line 75
    [DIFFICULTY_NORMAL][4] =
    {
#line 69
        .trainerName = _("Test2"),
#line 70
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 71
        .trainerPic = TRAINER_PIC_RED,
#line 72
        .gender = TRAINER_GENDER_MALE,
#line 73
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 74
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
        .partySize = 1,
        .party = (const struct TrainerMon[])
        {
            {
#line 77
            .species = SPECIES_MEWTWO,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 79
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 78
            .lvl = 5,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
#line 80
#line 87
    [DIFFICULTY_NORMAL][5] =
    {
#line 81
        .trainerName = _("Test2"),
#line 82
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 83
        .trainerPic = TRAINER_PIC_RED,
#line 84
        .gender = TRAINER_GENDER_MALE,
#line 85
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 86
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
        .partySize = 1,
        .party = (const struct TrainerMon[])
        {
            {
#line 89
            .species = SPECIES_MEWTWO,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 91
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 90
            .lvl = 50,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
#line 92
#line 99
    [DIFFICULTY_EASY][5] =
    {
#line 93
        .trainerName = _("Test2"),
#line 94
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 95
        .trainerPic = TRAINER_PIC_RED,
#line 96
        .gender = TRAINER_GENDER_MALE,
#line 97
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 98
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
        .partySize = 1,
        .party = (const struct TrainerMon[])
        {
            {
#line 101
            .species = SPECIES_METAPOD,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 103
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 102
            .lvl = 1,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
#line 104
#line 111
    [DIFFICULTY_HARD][5] =
    {
#line 105
        .trainerName = _("Test2"),
#line 106
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 107
        .trainerPic = TRAINER_PIC_RED,
#line 108
        .gender = TRAINER_GENDER_MALE,
#line 109
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 110
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
        .partySize = 1,
        .party = (const struct TrainerMon[])
        {
            {
#line 113
            .species = SPECIES_ARCEUS,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 115
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 114
            .lvl = 99,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
#line 116
    [DIFFICULTY_NORMAL][6] =
    {
#line 117
        .trainerName = _("Test3"),
#line 118
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 119
        .trainerPic = TRAINER_PIC_RED,
#line 120
        .gender = TRAINER_GENDER_MALE,
#line 121
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 122
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
#line 123
        .partySize = 1,
        .poolSize = 4,
        .party = (const struct TrainerMon[])
        {
            {
#line 125
            .species = SPECIES_WYNAUT,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 126
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 126
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
            {
#line 127
            .species = SPECIES_WOBBUFFET,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 128
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 128
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
            {
#line 129
            .species = SPECIES_EEVEE,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 130
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 130
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
            {
#line 131
            .species = SPECIES_MEW,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 132
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 132
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
#line 133
    [DIFFICULTY_NORMAL][7] =
    {
#line 134
        .trainerName = _("Test4"),
#line 135
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 136
        .trainerPic = TRAINER_PIC_RED,
#line 137
        .gender = TRAINER_GENDER_MALE,
#line 138
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 139
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
#line 140
        .partySize = 3,
        .poolSize = 6,
        .party = (const struct TrainerMon[])
        {
            {
#line 142
            .species = SPECIES_WYNAUT,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 143
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 143
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
            {
#line 144
            .species = SPECIES_WOBBUFFET,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 146
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 146
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 145
            .tags = MON_POOL_TAG_LEAD,
            },
            {
#line 147
            .species = SPECIES_EEVEE,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 149
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 149
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 148
            .tags = MON_POOL_TAG_ACE,
            },
            {
#line 150
            .species = SPECIES_MEW,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 151
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 151
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
            {
#line 152
            .species = SPECIES_ODDISH,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 154
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 154
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 153
            .tags = MON_POOL_TAG_ACE,
            },
            {
#line 155
            .species = SPECIES_ARON,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 157
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 157
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 156
            .tags = MON_POOL_TAG_LEAD,
            },
        },
    },
#line 158
    [DIFFICULTY_NORMAL][8] =
    {
#line 159
        .trainerName = _("Test5"),
#line 160
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 161
        .trainerPic = TRAINER_PIC_RED,
#line 162
        .gender = TRAINER_GENDER_MALE,
#line 163
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 164
        .battleType = TRAINER_BATTLE_TYPE_DOUBLES,
#line 166
        .poolRuleIndex = POOL_RULESET_WEATHER_DOUBLES,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
#line 165
        .partySize = 3,
        .poolSize = 10,
        .party = (const struct TrainerMon[])
        {
            {
#line 168
            .species = SPECIES_WYNAUT,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 170
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 170
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 169
            .tags = MON_POOL_TAG_LEAD,
            },
            {
#line 171
            .species = SPECIES_WOBBUFFET,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 173
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 173
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 172
            .tags = MON_POOL_TAG_LEAD,
            },
            {
#line 174
            .species = SPECIES_VULPIX,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 176
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 176
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 175
            .tags = MON_POOL_TAG_LEAD | MON_POOL_TAG_WEATHER_SETTER,
            },
            {
#line 177
            .species = SPECIES_BULBASAUR,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 179
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 179
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 178
            .tags = MON_POOL_TAG_LEAD | MON_POOL_TAG_WEATHER_ABUSER,
            },
            {
#line 180
            .species = SPECIES_TORKOAL,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 182
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 182
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 181
            .tags = MON_POOL_TAG_LEAD | MON_POOL_TAG_WEATHER_SETTER,
            },
            {
#line 183
            .species = SPECIES_CHERRIM,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 185
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 185
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 184
            .tags = MON_POOL_TAG_LEAD | MON_POOL_TAG_WEATHER_ABUSER,
            },
            {
#line 186
            .species = SPECIES_MEW,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 188
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 188
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 187
            .tags = MON_POOL_TAG_LEAD,
            },
            {
#line 189
            .species = SPECIES_ARON,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 191
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 191
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 190
            .tags = MON_POOL_TAG_LEAD,
            },
            {
#line 192
            .species = SPECIES_ODDISH,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 193
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 193
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
            {
#line 194
            .species = SPECIES_EEVEE,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 195
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 195
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
#line 196
    [DIFFICULTY_NORMAL][9] =
    {
#line 197
        .trainerName = _("Test6"),
#line 198
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 199
        .trainerPic = TRAINER_PIC_RED,
#line 200
        .gender = TRAINER_GENDER_MALE,
#line 201
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 202
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
#line 204
        .poolRuleIndex = POOL_RULESET_BASIC,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
#line 203
        .partySize = 2,
        .poolSize = 3,
        .party = (const struct TrainerMon[])
        {
            {
#line 206
            .species = SPECIES_WYNAUT,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 208
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 208
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 207
            .tags = MON_POOL_TAG_LEAD,
            },
            {
#line 209
            .species = SPECIES_WOBBUFFET,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 211
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 211
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 210
            .tags = MON_POOL_TAG_LEAD,
            },
            {
#line 212
            .species = SPECIES_EEVEE,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 214
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 214
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 213
            .tags = MON_POOL_TAG_LEAD,
            },
        },
    },
#line 215
    [DIFFICULTY_NORMAL][10] =
    {
#line 216
        .trainerName = _("Test1"),
#line 217
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 218
        .trainerPic = TRAINER_PIC_RED,
#line 219
        .gender = TRAINER_GENDER_MALE,
#line 220
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 221
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
#line 223
        .poolRuleIndex = POOL_RULESET_BASIC,
#line 224
        .poolPruneIndex = POOL_PRUNE_TEST,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
#line 222
        .partySize = 2,
        .poolSize = 3,
        .party = (const struct TrainerMon[])
        {
            {
#line 226
            .species = SPECIES_WYNAUT,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 227
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 227
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
            {
#line 228
            .species = SPECIES_WOBBUFFET,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 230
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 230
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 229
            .tags = MON_POOL_TAG_LEAD,
            },
            {
#line 231
            .species = SPECIES_EEVEE,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 232
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 232
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
#line 233
    [DIFFICULTY_NORMAL][11] =
    {
#line 234
        .trainerName = _("Test1"),
#line 235
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 236
        .trainerPic = TRAINER_PIC_RED,
#line 237
        .gender = TRAINER_GENDER_MALE,
#line 238
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 239
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
#line 241
        .poolRuleIndex = POOL_RULESET_BASIC,
#line 242
        .poolPickIndex = POOL_PICK_LOWEST,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
#line 240
        .partySize = 2,
        .poolSize = 3,
        .party = (const struct TrainerMon[])
        {
            {
#line 244
            .species = SPECIES_WYNAUT,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 246
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 246
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 245
            .tags = MON_POOL_TAG_ACE,
            },
            {
#line 247
            .species = SPECIES_WOBBUFFET,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 248
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 248
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
            {
#line 249
            .species = SPECIES_EEVEE,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 251
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 251
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 250
            .tags = MON_POOL_TAG_LEAD,
            },
        },
    },
#line 252
    [DIFFICULTY_NORMAL][12] =
    {
#line 253
        .trainerName = _("Test9"),
#line 254
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 255
        .trainerPic = TRAINER_PIC_RED,
#line 256
        .gender = TRAINER_GENDER_MALE,
#line 257
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 258
        .battleType = TRAINER_BATTLE_TYPE_DOUBLES,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
#line 259
        .partySize = 2,
        .poolSize = 2,
        .party = (const struct TrainerMon[])
        {
            {
#line 261
            .species = SPECIES_WYNAUT,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 262
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 262
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
            {
#line 263
            .species = SPECIES_WOBBUFFET,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 264
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 264
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
#line 265
    [DIFFICULTY_NORMAL][13] =
    {
#line 266
        .trainerName = _("Test10"),
#line 267
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 268
        .trainerPic = TRAINER_PIC_RED,
#line 269
        .gender = TRAINER_GENDER_MALE,
#line 270
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 271
        .battleType = TRAINER_BATTLE_TYPE_DOUBLES,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
#line 272
        .partySize = 2,
        .poolSize = 2,
        .party = (const struct TrainerMon[])
        {
            {
#line 274
            .species = SPECIES_WYNAUT,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 275
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 275
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
            {
#line 276
            .species = SPECIES_WOBBUFFET,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 277
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 277
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
#line 278
    [DIFFICULTY_NORMAL][14] =
    {
#line 279
        .trainerName = _("Test11"),
#line 280
        .trainerClass = TRAINER_CLASS_BLACK_BELT,
#line 281
        .trainerPic = TRAINER_PIC_RED,
#line 282
        .gender = TRAINER_GENDER_MALE,
#line 283
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 284
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
#line 285
        .partySize = 6,
        .poolSize = 6,
        .party = (const struct TrainerMon[])
        {
            {
#line 287
            .species = SPECIES_WYNAUT,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 288
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 288
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
            {
#line 289
            .species = SPECIES_WOBBUFFET,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 290
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 290
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
            {
#line 291
            .species = SPECIES_EEVEE,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 292
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 292
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
            {
#line 293
            .species = SPECIES_MEW,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 294
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 294
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
            {
#line 295
            .species = SPECIES_ODDISH,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 296
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 296
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
            {
#line 297
            .species = SPECIES_ARON,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 298
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 298
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
#line 299
    [DIFFICULTY_NORMAL][15] =
    {
#line 300
        .trainerName = _("TestTera"),
#line 301
        .trainerClass = TRAINER_CLASS_BLACK_BELT,
#line 302
        .trainerPic = TRAINER_PIC_RED,
#line 303
        .gender = TRAINER_GENDER_MALE,
#line 304
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
#line 306
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
#line 305
        .aiFlags = AI_FLAG_SMART_TERA,
#line 0
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
#line 307
        .partySize = 1,
        .poolSize = 1,
        .party = (const struct TrainerMon[])
        {
            {
#line 309
            .species = SPECIES_WYNAUT,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 310
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 310
            .lvl = 100,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
