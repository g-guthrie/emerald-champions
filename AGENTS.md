# Inclement Emerald 2 — standing rules

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
- UI changes require native screenshots of affected states and menu navigation:
  opening, selection, page changes, cancellation and returned player control.
- Studio instructions live in `tools/studio/skill/SKILL.md`; prefer this
  checked-in source over installed copies.

## Design rules (owner decisions)

- Scope: one playthrough ending at the finale — League → Wally (authored team)
  → five S.S. Tidal cabin teams → Steven → Birth Island/Deoxys (defeat or
  capture) → Buffel. Every reward pays off before or inside it. The Battle
  Frontier and Champions Circuit come later; never gate on them.
- Inclement is the world/story/progression/economy baseline. Universal
  legal-move tutor, no TMs. HMs need their story license, the badge and a party
  member able to learn the move — no move slot. Flight Beacon may use a
  Fly-capable Pokémon from party or PC. Leveler raises the party to the current
  cap. Battles grant no experience; leveling and evolving never change moves.
  Native ability switching.
- Inclement's species stat buffs and additional Ability slots apply to
  player, wild, trainer and partner Pokémon alike. Existing authored teams
  keep their chosen Abilities until deliberately retuned in playtesting. On every difficulty, authored Casual and Regular teams lose one
  additional level (minimum 1); other categories retain their levels. Base
  difficulty offsets live in `GetTrainerLevelReduction`; any
  text quoting them derives the numbers from it. Text is always Instant; there
  is no text speed option.
- No Game Book, player guides or Center battle presets. No
  new ground or hidden items: only replace original pickups or TM gifts.
- No Terastallization anywhere.
- Held items: Center nurses and the visiting Oldale Mart nurse share the
  initial tool handoff, including the Regenerator and Leveler. With the
  Regenerator, a held Berry its own holder eats in battle returns after
  battle. A Berry swapped away, eaten directly by a foe, burned or corroded
  is gone. Knock Off removes an item, Berries included, for the current battle
  only. Thief and Covet theft is temporary in Trainer battles: both sides'
  stolen items return to their original owners afterward, including a stolen
  Berry later eaten or destroyed. Wild theft remains permanent. Items acquired
  through theft or Pickup never unlock vendor stock.
- EVs: every Pokémon joining the player (catch, gift, starter, trade, hatch)
  arrives with 252 HP / 52 Atk, Def, SpA, SpD / 50 Spe; owned Pokémon keep
  their chosen spread. Every Center move tutor plans a whole spread and
  applies it at once for a flat ¥500 (free when unchanged, nothing on
  cancel). Evie in Fallarbor uses the same editor at ¥2 per added EV, with free
  reductions. Both editors show real stat previews and direct row adjustment.
- Steam Engine also makes Water never super effective on its holder.
- AI: authored doubles teams and the shared planner are preserved. Opponents may
  know loadouts but never read the player's committed move, target, switch or
  replacement. Tune difficulty with levels first. When levels alone cannot
  make a fight fair on every difficulty, tuning may also change a trainer's
  team, sets or items, keeping the fight's authored doubles plan and never
  weakening the AI's reasoning.
- Every trainer must be beatable by a stage-legal team: one the player can
  hold before that battle per the progression manifest
  (`artifacts/progression-manifest/`, checked by `scripts/tuning_pool_check.py
  --trainer`), not merely anything inside its level-cap window. One-hit KO
  moves and evasion boosts are never tuned for.
- Wild encounters: ordinary table slots are at least 4%; Legendary/Mythical,
  Ultra Beast and Paradox slots are 5%. Feebas keeps its native tile rule.
  Active storm visitors have a 25% encounter roll, reducing the normal table's
  share. After they settle, their ordinary resident slots are 5%.
  Caught legend-class species stop spawning. Gates live in
  `src/data/pokemon/legendary_signs.h`; legend-class spawns arrive at the
  current cap with authored sets. One Legendary, Mythical, Ultra Beast or
  Paradox per party in total; ordinary and pseudo-legendary Pokemon have no
  category limit. Static legends are high stakes: a knockout loses one.
  No wild-table percentages in dialogue. The rival demonstrates DexNav after
  Birch's gift; Birch offers the detailed hunting lesson on request.
- DexNav and the PokéNav atlas share the native wild roster. Show every
  currently available species with its icon, without seen/caught discovery
  gating. Hide encounters until their actual unlock conditions are met.
  Resident legend searching is allowed. Active storm guests stay visible but
  require ordinary wild encounters; they cannot be searched or registered.
  Mirage Tower species may be missed.

- DexNav chains are shiny/Pokérus hunting only. Above zero, search odds are
  chain percent shiny and half-chain percent Pokérus (cap 100); independent
  rolls. Zero uses base odds. Searches expire after 15 seconds, with a visible countdown
  that turns red in the last five seconds. No level/egg-move/IV rewards.
  Search HUD shows icon/name, direction, remaining seconds, Hold A, and Chain
  only above zero.
- Pokérus adds five percentage points to a beneficial nature modifier
  (+10% to +15%);
  ordinary recovery retains the -10% drawback. It grants no EV bonus.
  Direct infections have two total transmissions, one adjacent recipient per
  successful postbattle spread roll; recipients cannot spread. No daily decay.
  The benefit follows future nature changes and survives recovery and boxing.
  Summary stats use gold for the Pokérus-enhanced nature stat, retaining the
  native active/recovered markers; no spread-count or boost-percentage text.
  After 25 consecutive steps in Lavaridge's hot-spring water, any party
  Pokémon that has had Pokérus can receive treatment from the spring NPC.
  Soaking preserves stats and transmission budgets; treatment is chosen per
  Pokémon after an actual-stat preview. Special recovery gives the favored
  nature stat +15% and the other nature stat +5%, both gold; its recovered
  mark is red. Neutral natures
  gain no bonus. Leaving the water resets soak progress. Special recovery ends
  spreading and follows later nature changes, boxing and saving.
