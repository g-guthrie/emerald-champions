#ifndef GUARD_CONSTANTS_BATTLE_FACTORY_H
#define GUARD_CONSTANTS_BATTLE_FACTORY_H

// Emerald Champions: the Battle Factory is retired. Only this move-effect
// classification survives, because struct MoveEffectInfo still carries a
// battleFactoryStyle field in src/data/battle_move_effects.h.
enum FactoryStyle {
    FACTORY_STYLE_NONE,
    FACTORY_STYLE_PREPARATION,
    FACTORY_STYLE_SLOW_STEADY,
    FACTORY_STYLE_ENDURANCE,
    FACTORY_STYLE_HIGH_RISK,
    FACTORY_STYLE_WEAKENING,
    FACTORY_STYLE_UNPREDICTABLE,
    FACTORY_STYLE_WEATHER,
    FACTORY_NUM_STYLES
};

#endif // GUARD_CONSTANTS_BATTLE_FACTORY_H
