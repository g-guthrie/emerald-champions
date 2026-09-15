# Goal: fix the game — one coherent, calibrated Emerald Champions

Set September 15, 2026. Supersedes the previous goal text; that history lives in git.

## The game

Emerald Champions gives the player Pokémon Champions-style freedom to build any
legal team, then runs them through the hardest, most interesting competitive
doubles campaign ever built: every fight authored like a top VGC team, every
Mega represented on a trainer, six-member Gym rooms, and no grind between fights.

Everything that is not a battle does one of three jobs: deliver species and
Megas to the player, tell the story between fights, or get the player to the
next fight without friction. Anything that does none of those is cut.

## Protected (never reduce)

The 341-encounter roster, doubles everywhere, four-to-six member parties,
six-member Gym personnel, all 99 Mega Stones on trainers, two-Mega League
bosses, shared AI depth, levels-only difficulty with the existing formulas,
free build editing at the Leveler.

## Locked decisions

- Mandatory legendary scenes: Moltres, Landorus, the Southern Island Lati, Mew,
  Heatran, Diancie, Jirachi. Deoxys, the three Regis and Regigigas are optional
  with rewards. All legendary scenes stay singles, with authored sets.
- Fifteen cap milestones: one per badge, one per act climax.
- Story order tightened between Groudon and Kyogre; Birth Island and the Regis
  move to the post-Juan origins act.
- Sandstrewn collapse stays; unique progression content moves out of the sealed area.
- Money buys evolution items, Balls and cosmetics only. Stipends scaled to that.
  Poipole and Genesect become Circuit BP rewards. No real-time or counter gates
  on Mega Stones or legendaries. Rematches pay zero money.
- Mom gives the Old Rod with the Running Shoes.
- All NPC dialogue is unified in voice and rewritten against the new spine.
- The book and guide are rewritten from the new build, not patched.
- Quality of life from Run and Bun: key items register to L, R and Select with a
  visual indicator in the Bag.

## Phases

See `docs/CONSOLIDATION_PLAN.md` while it exists; it folds into
`docs/CONTINUE.md` when Phase 0 completes.

0. One baseline, one truth: single clone, generated numbers, consistency gate,
   known contradictions closed, book on a diet.
1. Campaign: acts, caps, gates, legendary sets, missables, difficulty labels.
2. Battles: acceptance ladder per encounter, reference parties per cap window,
   Mega register, major-battle-first order, resumed earned run.
3. Economy: income, sinks, stipend rescale, BP species, no timers, duplicates.
4. Docs steady state.

## Process

Sonnet subagents implement well-scoped code tasks from a self-contained spec.
The main agent reviews every diff, runs the checks and owns design decisions,
book prose and anything touching trainer teams. Book and source change together.
Executable source is ground truth when they disagree.

## Done means

The whole campaign is playable on the new spine with every encounter at least
at ladder level L2 and the earned run at L3 through the Champion, the book
describes exactly the shipped game, the release gate passes, and a working ROM
is delivered with organized evidence and honest remaining limits.
