# AI test debt (2026-09-02)

Generated from a full run of `test/battle/ai/*.c` against `pokeemerald-test-all-ai.elf`.
Every identity here is pinned in `scripts/run_emerald_champions_runtime_gates.py` as `allowed_failing`;
a new failure or a debt item that starts passing both break the gate.

| File | Total | Failing identities | Likely cause |
|---|---|---|---|
| `ai.c` | 84 | AI prefers moves which deal more damage instead of moves which are super-effective but deal less damage 1/2; AI uses a guaranteed KO move instead of the move with the highest expected damage 1/2; First Impression is not chosen if it's blocked by certain abilities; First Impression is preferred on the first turn of the species if it's the best dmg move; Move scoring comparison properly awards bonus point to best OHKO move | stat/move-data drift (Champions IV=31, Stat Points, First Impression 100 BP); Water Spout vs Thunder tie-break and Slash-crit preference need a look |
| `ai_calc_best_move_score.c` | 17 | AI will not further increase Attack / Sp. Atk stat if it knows it faints to target: AI faster 2/2; AI will not further increase Attack / Sp. Atk stat if it knows it faints to target: AI slower 2/2; AI will not waste a turn setting up if it knows target can faint it 2/2 | Chip Away vs Swift on Kangaskhan — suspicious, investigate damage estimate under Champions formula |
| `ai_check_viability.c` | 31 | AI sees increased base power of Grav Apple | Grav Apple ASSUME pinned to 80 BP (Champions 90) |
| `ai_choice.c` | 11 | Choiced Pokémon won't switch out if they can still affect one opposing Pokémon in doubles (reversed) 1/2 (1/?) | Sucker Punch priority-bracket prediction (PASSES_RANDOMLY) |
| `ai_combo_attack.c` | 4 | Combo Attack: Fusion moves are only incentivised when partners are adjacent in turn order 2/2 | Fusion move adjacency incentive |
| `ai_flag_predict_move.c` | 3 | AI won't use Sucker Punch if it expects a move of the same priority bracket and the opponent is faster (1/?) | predict-move should still attack (PASSES_RANDOMLY) |
| `ai_flag_risky.c` | 5 | AI_FLAG_RISKY: Mid-battle switches prioritize offensive options 1/2 | switch-in ranking under Champions stats |
| `ai_smart_tera.c` | 4 | AI_FLAG_SMART_TERA: AI might tera if it gets saved from a ko (2/2); AI_FLAG_SMART_TERA: AI will tera if it enables a ko | Terastallization is not enabled in this game |
| `ai_switching.c` | 144 | AI_FLAG_SMART_SWITCHING: AI will not switch out if Pokemon would faint to hazards unless party member can clear them 1/2; AI_SMART_MON_CHOICES: AI sees its own terrain setting ability when considering switchin candidates; AI_SMART_MON_CHOICES: AI sees its own weather setting ability when considering switchin candidates 2/2; Retaliate sees damage correctly for post ko switch in | own weather/terrain ability in switch-in scoring, hazard-aware switching, Retaliate damage — investigate |
| `ai_thinking_time.c` | 6 | AI thinking time doesn't explode (Steven multi); AI thinking time doesn't explode (doubles, smart) | 1 frame over the doubles-smart / Steven-multi ceilings (29/28, 22/21) after the difficulty-flag pass |
| `check_bad_move.c` | 15 | AI avoids toxic when it can not poison target 1/4 | Toxic scored 103 vs Immunity target (expected -10) — investigate |

## Investigation notes

- **Toxic vs Immunity (check_bad_move.c):** verified by reading that `IsAiBattlerAware()` honors the legacy
  `AI_FLAG_OMNISCIENT`, `AI_CanPoison()` calls `CanBePoisoned()`, and `CanSetNonVolatileStatus()` blocks on
  `ABILITY_IMMUNITY`. The code path looks correct, so the failure needs an instrumented run
  (`Test_MgbaPrintf` on the Toxic score in `AI_CheckBadMove`) rather than more reading.
- **Chip Away vs Swift, switch-in weather ability, Retaliate:** same — no static explanation found; the
  Champions stat formula (IV=31, `min(2*ev, 63)`) changes every damage threshold these tests were tuned on, so the
  first step is printing the AI's damage estimates for both moves in the failing scenario.
- **Thinking time:** 1 frame over on doubles-smart (29/28) and Steven multi (22/21). Measured, not a crash.

- **Update:** "AI avoids toxic when it can not poison target 1/4" (Snorlax / Immunity) *passed* on a later run of
  the same binary. Under `AI_FLAG_OMNISCIENT` the ability must be known deterministically, so a flip means the
  AI sometimes takes the random-guess path in `AI_DecideKnownAbilityForTurn` (or `gAiLogicData->abilities`
  is stale for that battler on that turn). That is the lead for the instrumented run.
