# New-game initialization audit — September 19, 2026

Reviewed new_game.c entrypoints/default options/trainer-ID handling and full
initialization sequence against save blocks, Pokedex reset, map flags, party/PC
clears, item pointers and scene preparation caller. Scoped native test exercises
the actual NewGameInitData function on dirty campaign state.

Confirmed direct-entry leak: harvestedBerries and gardenCelebiUnlocked are in
SaveBlock2, which the normal title flow clears before NewGameInitData but direct
scene preparation does not. Existing initializer already resets other Pokedex
progress explicitly. Added those two progress resets beside it, preserving
player identity and options. Native before.log retains 200 harvest credits;
after.log clears every credit, the garden unlock, finale completion and the
Regenerator, initializes zero party and 6000 money, and preserves name/frame choice.

Removed duplicated clock/Pokedex includes and redundant second player-party
clear. ZeroPlayerPartyMons at the beginning already clears the party count; no
intervening initializer adds party Pokemon. No save layout or AI changed.
Evidence: work/new-game-audit-20260919/. Remaining: broader boot/title flow and
all reset subsystem invariants; this is not full-game completion.
