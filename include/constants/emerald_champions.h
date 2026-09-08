#ifndef GUARD_CONSTANTS_EMERALD_CHAMPIONS_H
#define GUARD_CONSTANTS_EMERALD_CHAMPIONS_H

#define EC_BATTLE_SET_FAILED                 0
#define EC_BATTLE_SET_SUCCESS                1
#define EC_BATTLE_SET_SPECIAL_ITEM_EQUIPPED  2
#define EC_BATTLE_SET_MEGA                   3
#define EC_BATTLE_SET_MEGA_STONE_HELD        4

// Each Stat Point adds one stat before Nature at every level. The 66-point
// budget lets two stats max with two points to spare.
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

// Permanent two-starter opening transaction.
#define EC_OPENING_UNSELECTED                 0
#define EC_OPENING_PAIR_GRANTED               1
#define EC_OPENING_RESCUE_WON                 2
#define EC_OPENING_PRE_RIVAL_READY            3
#define EC_OPENING_COMPLETE                  4

#define EMERALD_CHAMPIONS_MEGA_ONLY          TRUE

#define EC_MEGA_BERRY_TRADE_COST             20
#define EC_MEGA_BERRY_TRADE_SUCCESS           0
#define EC_MEGA_BERRY_TRADE_ALREADY_DONE      1
#define EC_MEGA_BERRY_TRADE_NOT_ENOUGH        2
#define EC_MEGA_BERRY_TRADE_BAG_FULL          3
#define EC_MEGA_BERRY_TRADE_INVALID           4

#endif // GUARD_CONSTANTS_EMERALD_CHAMPIONS_H
