# Current direction — September 19, 2026

Work solo on `main` after completing the user-authorized integration merge. The user stopped the subagents
and campaign playthrough. Do not resume them. Review the largest relevant game
source files first and continue file by file. Prefer removing obsolete code,
reusing established paths and fixing demonstrated integration defects. Avoid
cosmetic churn, new frameworks, speculative refactors and gameplay redesign.

Preserve the pre-integration authored doubles AI and trainer teams. Compare
questionable behavior with pre-integration main508775fad8 before changing it;
explain a proposed behavioral change to the user first. The experimental guard
forecast patch is parked under work/code-cleanup-20260919 and restored out of
active source. Batch11 contains that experiment and MUST NOT be delivered.

Inclement is the world/story/progression/economy baseline, with retained native
ability switching, universal legal-move tutor/no TMs, unlock-only HMs (license + badge, any party),
Flight Beacon, current-cap Leveler and normal modern XP and the five immediate held-item
gifts with paid discovery stock. Preserve the native EV/IV services. No Game
Book, player guides, Center battle presets or Center stat editor. No new ground
or hidden item placements: only replace original Inclement pickups or TM gifts.

First explicit visual repair: fix the Champions title artwork. Measure the
indexed canvas, palette, sprite/tile layout, placement and animation; preserve
the native pixel style and verify the complete outline with headless captures.
Use targeted native screenshots only for concrete visual or behavioral doubts;
no campaign playthrough. Never represent source review as gameplay acceptance.

Preserve existing work, archives and saves. No reset/clean/wholesale restores.
Only remove our own reviewed experimental changes, preserving a patch first.
No simultaneous builds. Build and run focused checks proportionate to changes;
inspect failing tests for actual contract correctness rather than force green.
Keep a concise source-review ledger and current handoff. Do not update memory
files, publish, push or merge without applicable user authorization.

Latest approved convenience rules: HMs require only their story license and badge;
no compatible-party or move-slot requirement. Preserve legendary puzzle conditions.
Leveler raises every non-egg party member to its current species cap. Standard
modern full/half-share XP below the cap; no custom flat/catch-up bonuses and no
XP award/message/animation at cap. Consumed berries still restore after battle.
