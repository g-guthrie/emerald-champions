# Continue the earned v4 campaign

This is the only current handoff. Read `AGENTS.md`, start/resume the unfinished
objective in `docs/GOAL.md`, and use the canonical Game Book in this repository.
The old account was paused for a machine/account transfer, not because the game
was finished. No model-specific account, private memory, local Desktop folder,
paid API, browser emulator, or additional agent is needed to continue.

## First actions on a new machine

1. Clone the default `main` branch with full history. Work in this repository;
   do not select an archived 7.0/7.1 branch as the current implementation.
2. Follow `docs/VERIFICATION.md` to install/build the native dependencies. Download
   the exact resume assets with `python3 scripts/playthrough/fetch_handoff.py`.
   The downloader verifies hashes and preserves any differing existing work.
3. Read `handoff/checkpoint.json`. Run the read-only checkpoint/party inspection,
   and view the final save/contact sheet. The supplied session already contains
   its immutable matching ROM, ELF, state, normal save and complete trace.
4. Continue native play from that session. The immediate plan was to catch a
   Sableye in Granite Cave B1F, prepare it through the Center, and retry Cristian.
   Inspect Cristian's current book card/team and relevant AI again before battle.
5. Continue Jocelyn, Brawly, Steven's Ring handoff, the manor and the rest of the
   campaign. Build missing/revised story scenes, repair systems/AI as encountered,
   update this book and its consumers, and inspect evidence along the way.

The normal battery save supports a clean Continue if a different mGBA version
cannot load the archived savestate. After ANY ROM rebuild, use normal Save and
clean Continue; never transplant a savestate into another ROM/ELF.

## Exact stopping point

- Session: `work/v4-c13-brenden-final-ai-play`, step **096**, after a confirmed
  native Save and export to `earned-machine-handoff-granite-b1f.sav`.
- Location: Granite Cave B1F `(4,21)`, facing down, free overworld control.
  The interrupted walking command completed before the handoff; its step094
  trace is present. No capture occurred. The exact current state is step096.
- **Medium**, player cap **20**, **33 unique earned trainer clears**. Roxanne,
  the Rusturf rescue, Devon delivery setup, passage to Dewford, the Granite Cave
  letter and the relevant prior trainer route are earned. Gym clears include
  Laura, Takao, Lilith and Brenden. **Cristian, Jocelyn and Brawly are not beaten.**
- Cristian has two recorded eight-turn losses on the current AI. No enemy team
  or AI change was made in response. Repel Spray is OFF for the Sableye search;
  Poké Vial has zero charges. All six current Pokemon are healthy after native
  loss recovery. Sableye is **not yet caught**.
- Full hashes, save/state names and the 33 actual trainer IDs are in
  `handoff/checkpoint.json`. Do not count unrelated flags after trainer ID855 as
  trainer victories, or use the per-session battle serial as total progress.
- An earlier normal save,
  `earned-brenden-original-levels-fixed-ai-clear.sav`, is preserved as a recovery
  checkpoint. It is not the latest primary location. Do not silently roll back
  the current run or discard the two Cristian attempts.

The resume ROM is `f97bdbd7b97aff049b366b2c5e8da478a561cbc7ba329d5210aba12592c3b43d`;
its ELF is `e9dfda8ce313e0557283e6743bc2da18f0a426974f81405185614e5ef8f65658`.
The archived `inputs.json` belongs to those artifacts. `resume-source.json` and
`resume-source.patch` preserve the gameplay source base and the two pending map
edits used by that build. The published handoff adds portable host tools, book
status corrections and documentation; its newly built production artifact has a
separate stamp. Do not restamp the old resume ROM against the new checkout.

## Current party and intended adaptation

Field order is restored automatically after native win/loss. Forced replacements
reorder the in-battle party, so inspect before selecting a slot. Never rearrange
back based on an assumed order while the battle is still active.

| Slot | Pokemon / native preparation | Moves | HP |
| --- | --- | --- | --- |
| 0 | Mienfoo, Eviolite, Inner Focus, Adamant | Fake Out, Helping Hand, Drain Punch, Brick Break | 54 |
| 1 | Aerodactyl, Focus Sash, Unnerve, Jolly | Taunt, Tailwind, Dual Wingbeat, Protect | 68 |
| 2 | Marshtomp, Eviolite, Damp, Quiet | Muddy Water, Earth Power, Icy Wind, Wide Guard | 76 |
| 3 | Combusken, Life Orb, Speed Boost, Timid | Heat Wave, Flamethrower, Feint, Protect | 60 |
| 4 | Shaymin, Choice Specs, Natural Cure, Modest | Seed Flare, Earth Power, Giga Drain, Psychic | 76 |
| 5 | Pachirisu, Sitrus Berry, Volt Absorb, Bold | Follow Me, Nuzzle, Super Fang, Protect | 72 |

All are level20 and were obtained/prepared through the game. The archived party
snapshots preserve the exact native bytes; prior prep traces preserve the menu
operations. Pachirisu uses 252 HP/252 Defense/4 Sp. Defense, and Marshtomp uses
252 HP/252 Sp. Attack/4 Defense in the underlying stat representation. The native
UI's Champions Stat Points are a separate presentation; do not invent EV changes.

Sableye is the leading 20% ordinary land slot in Granite Cave B1F, level13–15.
The planned adaptation is Prankster burn and useful immunity/utility, obtained
and prepared normally. Choose its final set through actual available services;
no set or capture has been granted. Do not use Icy Wind casually into Defiant
Falinks, and do not paralyze Guts Throh when revisiting a historical fixture.
Choice Specs locks the first selected move even if Protect blocks it; switching
or the appropriate native item-loss consequence is needed to change the lock.

For navigation, consult `map.json`, `scripts.inc` and `map_path.py`. The latter is
only static collision/elevation guidance: it can suggest an invalid wall end
point, does not model every dynamic object, and does not prove native movement.
The B1F return ladder is at the current entry area. Native movement and screenshots
remain authoritative. Flight Beacon and Repel Spray are ordinary key items.

## Most recent battle findings

**Brenden, E0036-B01 / TRAINER_BRENDEN 572.** Preserve original levels: Sawk,
Riolu, Throh, Machop and Stufful at cap20; Monferno at21. The temporary Throh19
experiment was reverted. Two shared bugs were repaired: fainted-target fallback
and future Coaching value on a doomed recipient. On the final implementation,
Brenden won one 14-turn attempt, then lost a 12-turn rematch with four player
faints; only Shaymin28 and Marshtomp76 survived. All twelve winning-turn decisions
were measured at 7–51 native VBlank frames, max approximately0.85 seconds including
setup. This is sampled evidence, not a global worst-case certificate.

**Cristian, E0038-B01 / TRAINER_CRISTIAN 574.** Current intent SETUP/ALLY_COMBO,
Double Ace; Falinks Beat Up activates Justified Gallade/Lucario when profitable.
Lead Falinks74HP/Sash/Defiant/Jolly, speed58, and Gallade65HP/Life Orb/Justified/
Adamant, speed54. Reserves: AV Iron Fist Hitmonchan, Expert Belt Justified Lucario,
Choice Band No Guard Machamp, and level21 Life Orb Vital Spirit Annihilape.
Check the actual book/team for all exact moves/EVs; these notes are observations.
The Beat Up count checks active status/HP and party participation, ally damage
must be survivable, and boosts affect a recipient's later physical attack.

First loss: T1 both Protect denied Fake Out/flying pressure; T2 Helping Hand plus
Dual Wingbeat removed Gallade, but Falinks damaged Aerodactyl. The AI then used
Mach Punch plus Close Combat to KO Mienfoo before Drain Punch, survived Psychic
with Falinks's Sash, focused Shaymin, switched Falinks out of Feint into Machamp,
and used High Horsepower to KO Follow Me Pachirisu. Lucario/Machamp then focused
Marshtomp. Second loss: Falinks's consecutive Protect succeeded and bought the
Mienfoo KO; a later Dual Wingbeat missed. After Falinks fell, Hitmonchan priority
and Gallade Close Combat removed both attackers. Heat Wave/Follow Me removed
Gallade at the cost of Pachirisu; Lucario/Hitmonchan finished the last pair.

These attempts show real pressure and purposeful defense/switching, not a reason
to weaken the trainer. The complete exact input traces, per-turn memory reads,
reviews and contact sheets are archived. No full Cristian acceptance or win is
claimed, and the Justified engine has not yet been observed executing its whole
positive line in this earned matchup.

## Implementation already integrated

The Git history contains the complete native base plus the v4 implementation;
there is no missing uncommitted source tree to transplant. Useful old refactor
work was checkpointed at `81dfb738c6`; the current source continues from it.
The replacement engine and other parked checkouts are not execution dependencies.

| Area | Current work and evidence boundary |
| --- | --- |
| Book and roster | One complete 50-chapter v4 book; single Astra High review integrated; 342 battles/324 groups/369 party definitions. Exact catalogue import/check passes. Full playthrough quality is unfinished. |
| Core systems | Legendary cap consumers, source-owned preparation/evolution access, fixed-IV/stat rules, finite/duplicate/pending rewards, economy and scripted recovery have implemented changes and scoped checks. Preserve current contracts; do not restore old proposals. |
| Native AI | Committed-command knowledge and continuation; move-availability fix; blocked-attack opinion fix; first/repeated lone-survivor Protect rules; retaliation forecast; fainted-target fallback; recipient-survival Coaching value. Focused native failures/fixes are archived. |
| Early story | C01/C03/C02 opening and directions, Woods/Devon theft, Rustboro/Cut guidance, Roxanne reward/handoff, Rusturf rescue, Devon two deliveries, Briney's passage, Dewford directions and first letter journey were exercised in the earned run and repaired where needed. |
| C13 current | Individual Lilith/Brenden clears are preserved. Brawly/Steven text removes obsolete Slateport/Flash directions and names the Ring/manor route. First-letter/repeat text was inspected; post-Brawly Ring/manor handoff still needs earned execution. |
| Later scenes | Significant source preparation exists, including C48 observation interactions and selected restaged world/reward scenes. Synthetic evidence is available, but none makes later chapters earned or visually user-approved. Continue integrating/polishing them as reached. |

Notable source commits: `600463d17b` committed-command AI; `b650bbe1c4` blocked
attacks; `108551a35e` native move legality; `94554ad5f7` empty lone Protect;
`99a77527f7` first-letter Flash access; `e62e64aff1` retaliation; `c2bf321969`
fainted-target fallback; `ea87ae2c99` Coaching recipient value/restored Throh20.
`8b65e7f748` contains C48 observation and original-pickup composition changes.
Use commit diffs and current consumers, not a stale summary, when editing.

## Unfinished work and constraints that must survive the handoff

- Finish C13 and continue every chapter through Champion/postgame obligations in
  the book. Audit every retained trainer before fighting, review both wins/losses,
  tune supported weaknesses, acquire sensible counterteams, and keep the book
  synchronized. Do not substitute autowin traversal or a battle-lab grant for
  earned campaign progress.
- Continue the required exploration/story interactions with guide-free repeated
  directions, meaningful new/restaged choreography, native screenshots and actual
  user visual review. Avoid arbitrary decorative actors/sparkles. The rejected
  Diancie coordinate (7,9) remains rejected.
- Complete remaining mechanics/AI acceptance and relevant variant/randomized/
  alternate-archetype coverage. The 33 earned wins are not 33 fully closed
  acceptance matrices. Earlier shared fixes require dependency-based rechecks;
  prior scores do not automatically certify a newly changed engine.
- Keep ordinary overlapping trainers' individual victories. Full gauntlet reset
  is reserved for deliberately selected and clearly announced endurance events.
  Do not reset Lilith after a Brenden loss.
- The live legendary rule is only legendary/mythical species above600 configured
  BST: floor(chapterCap×600/BST), including at cap100. Species at600, such as this
  Shaymin, retain the full cap. A later idea to penalize600-BST legends and remove
  caps after Champion was **discussion only**, not approved or implemented.
- New-game title difficulty currently initializes Hard; this run was changed
  natively to Medium. A change to initialization is a design decision, not an
  implied cleanup. Do not change teams/AI quality with the difficulty setting.
- Validate important bag/storage failure, reward retry, loss/Retry/reload,
  temporary-party restoration and return/re-entry cases. Avoid exhaustive
  incidental furniture/menu coverage that stops the actual campaign.
- Keep native fixtures scoped. A baseline fixture that doesn't fail may be wrong:
  the Coaching reproducer needed actual Brenden identity to include his authored
  setup preference. Plain DOUBLE fixtures can carry recorded/link flags that
  bypass campaign caps; set campaign flags intentionally for cap tests.
- Final delivery still needs an integrated normal release, actual download checks,
  maintained book, organized honest evidence and explicit remaining limits. The
  handoff's development ROM is not a completion claim.

## Evidence layout and how to use it

`handoff/assets.json` indexes downloadable assets and their hashes. The default
fetch gets the complete current session and recent battle contact sheets. Fetch
`--evidence` when investigating earlier work; it restores the relevant original
`work/` paths from content-addressed archives without changing captured bytes.
The archive index distinguishes gameplay sessions, synthetic scene fixtures,
focused tests, earlier experiments and historical 7.0 references. Old experiments
are preserved for diagnosis, not reinstated as current decisions.

- Current session: `work/v4-c13-brenden-final-ai-play/trace.json`, per-turn
  `*-battle-read.json`, `decision-timing.json`, normal saves, before states,
  screenshot originals and two detailed Brenden plus two Cristian review records.
- Earned ancestry: the `battery_parent` and bridge fields in each session trace
  name the previous normal save and hash. Earlier current-run directories include
  `v4-opening-corrected-play`, `v4-c04-play`, `v4-c05-play`, `v4-c06-fixed-play`,
  `v4-c07-cadence-play`, `v4-c07-roxanne-play`, `v4-c08-selection-fixed-play`,
  `v4-c08-rusturf-play`, `v4-c09-devon-play`, `v4-c10-dewford-play`,
  `v4-c12-granite-play` and the C13 repair/rematch directories. Follow actual
  trace ancestry rather than sorting names into an assumed canonical chain.
- `work/contact-sheets/`: grouped captured pixels, composition inputs and sidecars
  containing original hashes/placements. Captured does not mean inspected; use
  the explicit reviewed-sheet records and continue visual checks for new work.
- `work/v4-scenes`, `work/v4-origins-integrated`, `work/book-scenes`,
  `work/book-economy-scenes`: synthetic setup and real native scripts, with
  separate outcomes. Read each trace's scope; these are not earned trainer wins.
- `work/v4-*-native.log` and matching immutable test ELFs/stamps preserve focused
  regression evidence. `work/7.0-first-100-ai-source.json` and related extracted
  records preserve old authored observations; book sections21–24 carry the useful
  intent. They are historical evidence, not current instructions or proof.
- `tests/campaign/playthrough.json` is an input-recipe catalogue, not the live
  checkpoint. Some later recipes reflect older flow and remain unvalidated until
  reached. Three early service/inspection recipes were preserved from the existing
  working tree; do not restart the current campaign to execute every recipe.

Large artifacts are release assets rather than committed build products. The
source checkout has one active book and one handoff; old duplicate playtest
READMEs/build stamps were removed from the live branch. Git history and labeled
archive assets preserve earlier evidence. GitHub is now the portable handoff and
download surface; the previous account's private website is not needed to resume.
