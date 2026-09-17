#ifndef GUARD_BATTLE_FACTORY_H
#define GUARD_BATTLE_FACTORY_H

// Emerald Champions: the Battle Factory facility is retired and
// BATTLE_TYPE_FACTORY is never set any more. src/battle_ai_main.c still calls
// this from its BATTLE_TYPE_FACTORY branch; see src/retired_frontier.c.
u64 GetAiScriptsInBattleFactory(void);

#endif // GUARD_BATTLE_FACTORY_H
