# Retry, Reload and temporary parties — September 8, 2026

The focused source audit found and fixed one concrete control-flow defect: the opening rescue's Retry/forfeit eligibility was unreachable. Other inspected recovery paths below did not establish another bug. This is not a full save-corruption or emulator certification.

## Fixed: first-battle exclusion hid the rescue's Retry flow

`CanPlayerForfeitNormalTrainerBattle` in `src/battle_main.c` ended with an explicit allowance for `IsEmeraldChampionsBirchRescueBattle()`, but an earlier `BATTLE_TYPE_RECORDED_INVALID` rejection included `BATTLE_TYPE_FIRST_BATTLE`. The rescue always carries that flag. Thus its explicit allowance could never run.

The fix removes only `BATTLE_TYPE_FIRST_BATTLE` from the exclusion mask when the game identifies the active Emerald Champions rescue. It retains the global configuration switch and all other exclusions. An unrelated first battle remains excluded, as do link, capture tutorial, Safari, roaming, legendary, recorded, Trainer Hill, Secret Base and Frontier formats.

The source already supplies the rest of the rescue recovery path: the two-starter party is snapshotted before battle initialization; accepted Retry skips post-battle evolution, restores the party, recreates the rescue opponents and reinitializes battle; leaving an unsuccessful rescue restores the starter party before returning to its field script. This patch makes the intended action-selection branch reachable; it does not add another recovery implementation.

Verification: a temporary host C executable compiled the extracted production eligibility function and production rescue-identification function against the project's actual battle constants. It passed 25 cases with `-Wall -Wextra -Werror`: active rescue, non-rescue first battle, FRLG first battle, normal trainer doubles, ordinary wild doubles, and ten excluded formats each combined with rescue and with trainer battle flags. Environment lookups were minimal host stubs. This verifies eligibility, not controller timing, an on-screen confirmation or a completed retry in a ROM. No permanent test or gate framework was added.

## Reviewed source paths

| Path | Inspected behavior | Evidence limit |
| --- | --- | --- |
| Normal trainer Retry | Snapshots player and applicable partner parties. Confirmed Retry tears down the battle, restores parties, rebuilds opponents through the same trainer constructor, resets outcome and enters battle initialization. | No repeated-retry memory or controller stress test in this audit. |
| Retry vs Bag encryption | Retry copies independently encrypted Pokémon, not encrypted Bag slots. Battle initialization can rotate the save's encryption key. The existing comment correctly explains why copying old Bag slots would be unsafe. | No measured item-quantity corruption reproduction. |
| Post-battle evolution | `FreeResetData_ReturnToOvOrDoEvolutions` explicitly excludes a pending Retry. | Source inspection, not every evolution scenario. |
| Reload | Start-menu confirmation invokes `ReloadLastSave`; reload clears/reinitializes EWRAM, loads the disk save and continues from it. It does not call battle Retry. | No flash fault injection. |
| Reload cancellation | No/B returns `SAVE_CANCELED`, clears the dialog and rebuilds the start menu. Reload is exposed only for OK or recovered-valid save status. | No native UI run in this audit. |
| Circuit normalization | Entry saves the original party before setting competitive levels. Retirement and generation failure call `ChampionsCircuitEnd`. A loss calls it inside the result handler before the script returns to the lobby. | No end-to-end Circuit session played. |
| Tent normalization | Entry saves the original party. Declining continuation, completion, loss and opponent-generation failure each call `ChampionsTentEnd` before release. | No three-battle native run. |
| Opponent-generation failure | Competition generation has a finite 64-attempt outer loop and returns failure to scripts; the scripts restore parties rather than start a battle with a failed team. | This does not prove all inner generator code is bounded or fast. |
| Space Center party selection | Selection saves the party and cancellation loads it before returning to the readiness prompt. | Multi-battle outcome and party-order restoration still need native scenario coverage. |
| Follower wild battles | Generic follower preparation/restoration exists, but partner wild battles are disabled by the current `FNPC_FLAG_PARTNER_WILD_BATTLES` configuration. | Dormant follower code was not treated as an active campaign defect. |

No compatibility migration, recovery menu redesign, save-layout change or change to the distinction between Retry and Reload was introduced.

The next useful runtime evidence is an ordinary-key rescue Retry, cancellation back to action selection, a normal trainer Retry after damage/item consumption, and a Circuit/Tent exit with a deliberately distinctive original party. A passing build alone does not establish these outcomes.
