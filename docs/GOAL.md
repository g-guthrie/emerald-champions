# Current goal: solo source cleanup and title repair

The user replaced the campaign-playthrough approach on September19. Stop all
subagents. Review the largest game-responsible source files first, then proceed
file by file. Clean demonstrated code/integration problems while preserving
authored doubles battles and AI. Compare against the good pre-integration game;
do not tune behavior to make a synthetic test pass.

Repair the Champions title PNG and its integration. Measure source art, palette,
GBA tiles/sprites and motion. Match native pixelation, restore a complete outline
and inspect headless title frames. No gameplay campaign required; bounded native
captures only when a concrete question calls for them.

Review queue: work/code-cleanup-20260919/runtime-source-order.json. Record what
was actually examined and changed, plus remaining coverage, in a concise ledger.
Build and check proportionately. Preserve existing saves and delivered batch10.
Experimental AI batch11 is not approved for publication. Historical goal and
handoff are preserved in work/code-cleanup-20260919/*before.

Latest approved convenience rules: HMs require only their story license and badge;
no compatible-party or move-slot requirement. Preserve legendary puzzle conditions.
Leveler raises every non-egg party member to its current species cap. Standard
modern full/half-share XP below the cap; no custom flat/catch-up bonuses and no
XP award/message/animation at cap. Consumed berries still restore after battle.
