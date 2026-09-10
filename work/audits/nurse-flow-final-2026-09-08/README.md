# Final-build first-center optional healing — native evidence

Both Yes and No passed on September 8, 2026, using the final rebuilt headless ROM and its matching ELF, existing scenario 66, and `build/headless/emerald_champions_mgba_runner`. No previous save or old-ROM state was loaded. The earlier `nurse-flow-2026-09-08` evidence was preserved untouched.

## Exact artifacts

- `nurse-fixture.gba`: SHA-256 `e710cb6233294685b31529207a79ef6123ad02bc2e488124af808d57a620dc12`
- `nurse-fixture.elf`: SHA-256 `e9878c9a216204f03b7e499ddb3d85728481a1cfb63f94668ab3c620fc7640c8`
- Copied from the final `pokeemerald-repair-headless-20260908.gba/.elf` after the build owner confirmed completion. Both original and copied hashes match those supplied by the build owner.

These are the test-fixture artifacts, not the production distribution ROM. No game source, fixture, observer expectation or permanent test framework was changed during this check.

## Fresh setup and actual prompt

The runner started with `--save -`, wrote scenario 66 to `gEcHeadlessFixtureScenario` at frame 60, held Up at frame 300 for 20 frames, pressed A at frame 380, and pressed A every 40 frames from 420. The existing fixture creates prebadge Treecko at 1 HP with poison, a second Poochyena, no center tools, and the player at Oldale's nurse.

Addresses were resolved from the final ELF, not reused from the earlier ROM:

| Symbol / field | Address |
| --- | --- |
| Scenario | `0x0200228c` |
| Setup result | `0x02002208` |
| Yes-specific observed result | `0x0200223c` |
| Full observer mask (`gEcHeadlessCampaignQueryValue`) | `0x0200224c` |
| Battle / capture serial | `0x0200227c` / `0x02002224` |
| `sGlobalScriptContext` | `0x030016bc` |
| `gText_EmeraldChampionsHealAfterTools` | `0x0833eb4b` |
| Player party base | `0x020336d0` |
| First Pokémon status / HP / max HP | `0x02033720` / `0x02033726` / `0x02033728` |

The script-context data offset remains 100 bytes in the unchanged ARM structure; the party symbol is `0x960` bytes for 24 Pokémon, confirming 100-byte Pokémon records. Execution stopped on `--until 4:0x03001720:0xffffffff:0x0833eb4b` after **1,862 frames**, producing `heal-offer-start.ss`. Another 150 idle frames produced `heal-offer-menu.ss` and the visually inspected `heal-offer-menu.png`:

> Well, now that you're here, would
> you also like to heal your Pokémon?

The actual Yes/No menu was visible. At this point the mask was `0x11fff`, all four tools were present, and Treecko remained at 1 HP with status `0x8`. Both branches below loaded this same newly created, exact-ROM menu state.

## Results

| Observation | Yes | No |
| --- | --- | --- |
| Choice input | A at frame 30 | Down at 10, A at 30 |
| Additional input | A every 80 frames from 180 | A every 80 frames from 180 |
| Frames until expected completion mask | 422 | 182 |
| Setup result | 1 | 1 |
| Full mask | `0x3ffff` | `0x31fff` |
| Yes-specific observed result | 1 | 0, correctly |
| HP / max HP | 31 / 31 | 1 / 31 |
| Status | 0 | `0x8` (poison) |
| Used-Center counter equals 1 | Yes | No |
| All four tools and charged Vial | Present | Present |
| Field unlocked, script ended, outside battle | Yes | Yes |
| Battle / capture serial | 0 / 0 | 0 / 0 |

Both expected masks and independent HP/status/serial memory reads were asserted. The No mask clears exactly the three healing-specific bits (13–15), preserving the other 15 checks; the existing Yes-only boolean was not weakened to make No pass.

`yes.ss`, `no.ss`, `yes.png` and `no.png` preserve release-frame evidence. Another 30 idle frames in each branch produced visually inspected `yes-settled.png` and `no-settled.png`: the normal Center without a remaining text box, with the same completed masks. Their matching visual hash is expected because party health is not drawn in the overworld; independent memory reads distinguish the outcomes.

This verifies fresh first-time tool acquisition → optional healing prompt → both actual choices in these exact final headless artifacts. It does not certify every Center, partial delivery with a full Bag, a production-ROM walkthrough, or future rebuilt artifacts.
