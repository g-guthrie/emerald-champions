#ifndef GUARD_CONSTANTS_EMERALD_CHAMPIONS_H
#define GUARD_CONSTANTS_EMERALD_CHAMPIONS_H

#define EC_BATTLE_SET_FAILED                 0
#define EC_BATTLE_SET_SUCCESS                1
#define EC_BATTLE_SET_SPECIAL_ITEM_EQUIPPED  2
#define EC_BATTLE_SET_MEGA                   3
#define EC_BATTLE_SET_MEGA_STONE_HELD        4

// Stat Points: the Champions budget. 32 points is a 252 EV spread; 66 total
// lets two stats max with two points to spare.
#define EC_STAT_POINTS_PER_STAT              32
#define EC_STAT_POINT_BUDGET                 66

#define EC_BATTLE_FORMAT_DOUBLES             0
#define EC_BATTLE_FORMAT_SINGLES             1
#define EC_BATTLE_FORMAT_COUNT               2

#define EC_BATTLE_ITEM_CATEGORY_OFFENSE 0
#define EC_BATTLE_ITEM_CATEGORY_DEFENSE 1
#define EC_BATTLE_ITEM_CATEGORY_FIELD 2
#define EC_BATTLE_ITEM_CATEGORY_TYPE 3
#define EC_BATTLE_ITEM_CATEGORY_GEM 4
#define EC_BATTLE_ITEM_CATEGORY_SPECIES 5
#define EC_BATTLE_ITEM_CATEGORY_COUNT 6

#define EMERALD_CHAMPIONS_MEGA_ONLY          TRUE

#endif // GUARD_CONSTANTS_EMERALD_CHAMPIONS_H
