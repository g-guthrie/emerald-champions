# Current handoff — solo source cleanup

The user explicitly authorized merging all current integration/fix work into main
and continuing the file-by-file audit there. Complete and verify that merge/push
before beginning new edits. This supersedes all earlier no-merge status notes.

## Current design

Inclement Emerald is the world/story/progression baseline. Keep the authored
Champions doubles teams and AI, universal free legal-move tutor/no TMs, native
ability switcher and Inclement EV/IV services. Keep early Eviolite, Choice items
and Focus Sash, paid discovery stock, and postbattle held-item/berry restoration.
HMs need only their story unlock and badge, with no party compatibility or move
slot requirement. Preserve location checks and legendary puzzle conditions.
Leveler raises the party to each current species cap. XP uses normal modern
full/half shares without custom flat/catch-up bonuses; capped recipients receive
no XP award, message or animation. No new overworld item placements: only replace
original Inclement items or TM gifts. No Game Book or player-facing guides.

## Workflow and completed passes

Work solo. Do not resume subagents or the old campaign-playthrough goal. Review
largest game-responsible source files first, including their relationships to
NPC dialogue, rewards, flags and story progression. Use bounded native evidence
when a concrete question requires it; builds and scoped tests are not whole-game
acceptance. See AGENTS.md and docs/GOAL.md.

- Title banner regenerated using vanilla references and inspected in the game.
- HM/Leveler/XP changes built and verified with focused engine checks.
- event_object_movement.c: see docs/OBJECT_MOVEMENT_AUDIT.md.
- battle_script_commands.c: see docs/BATTLE_SCRIPT_AUDIT.md.
- Next file: src/battle_util.c. Full source/coherence review remains unfinished.

Authored AI/team files are unchanged against pre-integration508775fad8. The rejected
guard-forecast experiment was removed from active source and parked locally;
its experimental batch11 ROM must not be delivered. All current source is intended
for the authorized merge; old experimental/rewrite branches are not part of it.

## Evidence and artifacts

Latest audited normal build: work/battle-script-audit-20260919/validated-build.
Earlier convenience ROM remains on Desktop; it predates the two file audits.
Prior ROMs and earned saves remain preserved. Never cross-load raw savestates
between builds. Local work/ folders contain detailed before/after evidence.
Merge validation and remote-verification receipts: work/main-merge-20260919.
