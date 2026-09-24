# Emerald Champions — standing rules

Source and native runtime behavior are the only ground truth. This repository
keeps no docs, ledgers, handoffs or progress notes: they drift. Do not create
them. Report findings in the conversation and put durable facts in code, tests
or data.

## Working in the tree

- Build with `make -j4 release`. One build at a time.
- Trainer authoring lives in `data/emerald_champions/emerald_champions_battle_teams.txt`;
  run `python3 scripts/emerald_champions_teams.py --write`, then `--check`.
- Commit by explicit path only; never `git add -A`, `commit -a`, reset, clean
  or wholesale restores. Preserve saves, archives and other agents' work.
- Never load a savestate into a different ROM; move a save between builds with
  a native in-game save and a clean boot.
- Codex owns overworld presentation (map.json objects, local ids).

## Design rules (owner decisions)

- Scope: one playthrough ending at the finale — League → Wally (authored team)
  → five S.S. Tidal cabin teams → Steven → Birth Island/Deoxys (defeat or
  capture) → Buffel. Every reward pays off before or inside it. The Battle
  Frontier and Champions Circuit come later; never gate on them.
- Inclement is the world/story/progression/economy baseline. Universal
  legal-move tutor, no TMs. HMs need their story license, the badge and a party
  member able to learn the move — no move slot. Flight Beacon may use a
  Fly-capable Pokémon from party or PC. Leveler raises the party to the current
  cap. Standard modern XP below the cap, none at it. Native ability switching.
- No Game Book, player guides, Center battle presets or Center stat editor. No
  new ground or hidden items: only replace original pickups or TM gifts.
- No Terastallization anywhere.
- Held items: consumed held Berries return after battle only once the
  Regenerator Key Item arrives (Norman, after Badge 5). Knock Off removes an
  item for the current battle; Thief and Covet transfer permanently.
- AI: authored doubles teams and the shared planner are preserved. Opponents may
  know loadouts but never read the player's committed move, target, switch or
  replacement. Tune difficulty with levels, not by weakening sets or strategy.
- Every trainer must be beatable by a stage-legal team.
- Wild legends: ordinary 1% slots (at most two per table), Ultra Beasts and
  Paradox 2-3%, ordinary slots never below 2%; gates in
  `src/data/pokemon/legendary_signs.h`. Legend-class spawns arrive at the
  current cap with competitive sets. One Legendary/Mythical, one Ultra Beast and
  one Paradox per party. Static legends are high stakes: a knockout loses one.
  No percentages in dialogue.
