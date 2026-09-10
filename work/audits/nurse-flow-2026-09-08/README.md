# First-center optional healing — native runtime evidence

Tested September 8, 2026 using the existing FIRST_CENTER_ACQUISITION fixture (66), the freshly built matching headless ROM/ELF and the native libmGBA runner. No old save or old-ROM state was loaded. Both branches passed.

## Exact artifacts

- ROM: `nurse-fixture.gba`, SHA-256 `34d8db78fa14901f50266dc14a66766485c0c519e0ec65a970ce9643be48da58`
- ELF: `nurse-fixture.elf`, SHA-256 `ad5ed3a4c1d27b0668d1b0b162a8b3fe0f80137bed62b3d11967462727ec61ff`
- Original build names: `pokeemerald-repair-headless-20260908.gba/.elf`.

These local copies preserve the exact interim artifacts if the main build is subsequently replaced. They are test-only fixture builds, not the final production release. The test exercises actual nurse script bytecode with timed ordinary keys; fixture code only supplies initial state and observes results. Its battle-automation capability was not exercised: both battle and capture serials stayed zero.

## Setup and checkpoint

The existing fixture starts a fresh prebadge state with Treecko at 1 HP and poisoned, a second Poochyena, no center tools, and the player in Oldale's center. The runner writes scenario 66 at boot frame 60, turns upward at frame 300, talks at frame 380 and presses A every 40 frames from 420 onward.

Rather than guess dialogue timing, execution stops when `sGlobalScriptContext.data[0]` equals the ELF address of `gText_EmeraldChampionsHealAfterTools`. The ARM compiler independently confirmed the script data offset as 100 bytes. In this ELF: context `0x030016bc`, text `0x0833d497`; setup stops after 1,862 frames. `heal-offer-start.ss` is this exact-ROM state.

Another 150 frames with no keys lets the text finish and display the actual Yes/No menu. `heal-offer-menu.ss` is the common branch point. The inspected `heal-offer-menu.png` reads:

> Well, now that you're here, would
> you also like to heal your Pokémon?

All four tools have already been delivered. The generic welcome/rest dialogue was not inserted between delivery and this offer.

## Branch results

| Observation | Yes | No |
| --- | --- | --- |
| Initial input from menu checkpoint | A at frame 30 | Down at 10, A at 30 |
| Further dialogue input | A every 80 frames from 180 | A every 80 frames from 180 |
| Frames until completed observer state | 422 | 182 |
| Setup sentinel | 1 | 1 |
| Observer bitmask | `0x3ffff` | `0x31fff` |
| First Pokémon HP / maximum | 31 / 31 | 1 / 31 |
| Status | 0 | `0x8` (poison) |
| Used-Pokémon-Center counter equals 1 | Yes | No |
| All four tools and charged Vial | Present | Present |
| Field controls released / script ended | Yes | Yes |
| Overworld callback, outside battle | Yes | Yes |

The existing fixture's boolean `ObservedResult` is designed specifically for the healed branch, so it is 1 for Yes and 0 for No. No is validated using the full expected bitmask and independent HP/status memory reads, not by changing that observer to pass. Bits 13–15 are precisely heal count, full HP and cleared status; all other 15 checks remain set in the No branch.

Direct memory reads used player party base `0x020336d0`; the ELF party-array size confirms 100-byte Pokémon. Status is at `0x02033720`, HP at `0x02033726`, maximum HP at `0x02033728`. Observer mask is `0x0200224c`.

`yes.ss` / `no.ss` and `yes.png` / `no.png` capture the release frame. Another 30 no-input frames produced `yes-settled.png` / `no-settled.png`, confirming the dialogue disappears and the same completed observer masks remain. End-of-script rendering is one frame behind the observer, so the release-frame screenshots still show the farewell text; the settled screenshots show the normal center.

## Scope

This proves first-time tools → explicit optional healing → actual Yes and No behavior in these exact compiled artifacts, including retaining damage/status after No. It does not certify every Pokémon Center, partial gift delivery after a full Bag, or a later rebuilt ROM. No new fixture, baseline change or permanent test framework was introduced.
