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
4. Read the exact stopping point below before using any published resume asset.
   The local earned continuation is ahead of the original transfer download.
   Never silently replace this continuation with that older checkpoint.
5. Continue Brawly, Steven's Ring handoff, the manor and the rest of the
   campaign. Build missing/revised story scenes, repair systems/AI as encountered,
   update this book and its consumers, and inspect evidence along the way.

The normal battery save supports a clean Continue if a different mGBA version
cannot load the archived savestate. After ANY ROM rebuild, use normal Save and
clean Continue; never transplant a savestate into another ROM/ELF.

## Exact stopping point

- Primary session: `work/v4-c13-quash-fixed-play`, step **435**. Brawly has five earned losses.
  Native recovery and Save completed at434; export `earned-brawly-five-losses.sav`.
  Petalburg Center `(8,4)`, facing up, healthy party and free control.
  Source now reduces only Brawly Hawlucha22→21; rebuild and clean Continue are next.
- **Medium**, cap **20**, **35 earned unique trainer clears**. Cristian: two
  eight-turn losses then a13-turn win. Jocelyn:13-turn loss, eight-turn loss,
  then seven-turn win. Brawly:10-,10-,13-,11-, and11-turn losses; still uncleared.
- Current immutable ROM includes the capture-menu repair and shared timely burn/
  Quash forecasts. Clean Continue from each previous normal save preserved ancestry.
  Native acquisition/preparation includes Carbink and Prankster Sableye. Sableye’s
  actual B1F habitat rate is13%; caught on the twentieth natural encounter.
- Party healthy. Repel ON, Old Rod registered. No manual healing consumables or
  X items in these fights. Naturally activated held items are allowed. The current
  Champions code restores battle-start held items, including berries, at settlement;
  they remain consumed within battle unless restored by a native move/ability.
- Every AI bug requires the book’s backward/bespoke/forward affected-team index,
  repairs for every plausible affected implementation and focused regressions.
  Source coverage does not mean every earlier battle was replayed.
- Published `v4-handoff-20260913` assets remain older than this continuation until
  repackaging/upload/download verification. Do not roll back to that old save.

Current ROM/ELF hashes and ancestry are in the session trace. Both repairs pass
focused native regressions; the normal release also builds. Full-party capture YES replacement/
Yes-No cleanup was exercised successfully on the earned Abra catch. Direct-PC NO is earned-verified with Azurill; full-storage failure remains pending. Never transplant
savestates between different ROMs.

## Current party and rematch plan

| Slot | Pokemon / native preparation | Moves | HP |
| --- | --- | --- | --- |
| 0 | Kadabra, Life Orb, Magic Guard, Timid | Psychic, Dazzling Gleam, Counter, Protect | 52 |
| 1 | Sableye, Sitrus Berry, Prankster, Careful | Taunt, Quash, Fake Out, Foul Play | 76 |
| 2 | Aerodactyl, Focus Sash, Unnerve, Jolly | Taunt, Tailwind, Dual Wingbeat, Protect | 68 |
| 3 | Shaymin, Life Orb, Natural Cure, Modest | Seed Flare, Earth Power, Giga Drain, Psychic | 76 |
| 4 | Pachirisu, Sitrus Berry, Volt Absorb, Bold | Follow Me, Nuzzle, Super Fang, Protect | 72 |
| 5 | Azumarill, Assault Vest, Huge Power, Adamant | Liquidation, Play Rough, Aqua Jet, Knock Off | 88 |

All are level20 and prepared through native services. Mienfoo, Carbink, Marshtomp and
Combusken are stored. Abra was caught on Granite Cave1F with one Great Ball,
natively replaced Combusken, evolved through the Leveler and received Life Orb.
Linking Cord is not available until Slateport. Sableye's tutor visit replaced
Will-O-Wisp: the move-selection inputs came before the summary screen opened.
Read-only replay confirmed slot0 was still selected; no wrong-slot write proven.
Check the actual move cursor and resulting moves after future teaching.

Azurill was caught on the first Route104 encounter with one Great Ball, sent directly
into PC, withdrawn in place of Marshtomp, bonded to255 and evolved twice with the
Leveler. The specialist applied the Assault Vest doubles preset natively. Save/export
followed. Attempt five then lost in11 turns; Azumarill's switch into Super Fang and
next-turn focus fire were costly. Hawlucha's Protect had value, its84 Speed exceeded
Aerodactyl82, and it survived Wingbeat at5HP before helping close the battle.

First modest balance adjustment: only E0040-B01 Hawlucha offset+2→+1 (22→21 here).
Expected unboosted Speed81 gives Aerodactyl a response before Unburden. Shared U0174,
Humberto's reuse, other Brawly levels and all builds/AI stay as authored. Rebuild the
normal and headless artifacts, then clean Continue from the actual435 battery save.
Rematch and verify actual stats/outcome; no tuned win claimed. Conkeldurr was the
initial candidate but fifth-attempt evidence favored Hawlucha's concrete threshold.

Next: Brawly, then Steven’s Ring and manor/meadow handoffs. His exact T038/
U0170–0175 team was inspected: Pachirisu/Choice Band Heracross opening; Guts
Conkeldurr, AV Hariyama, White Herb Unburden Hawlucha and special Cobalion with
Coaching/Quick Guard. Shared redirection, Coaching recipient survival, burn
exceptions and priority protection code reviewed. Preserve the builds and apply only the documented Brawly Hawlucha level cut.
Navigation: Center exit `(8,8)`; Gym door `(8,17)`; arrival `(5,27)`; Brawly `(4,3)`.

Capture YES replacement is earned-verified with Abra; direct-PC NO with Azurill.
Full-storage failure remains pending. Earlier nine Old Rod casts produced no bite.
No odds/RNG were changed; do not stall on incidental fishing.
Field specialist Yes/No overlay was also observed and is not yet fixed/accepted.

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
Choice Band No Guard Machamp, and level21 Leftovers Vital Spirit Annihilape.
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
reviews and contact sheets are archived. Attempt three was an earned 13-turn
win with only Sableye (48/76 HP) remaining. Five party members fainted. Ordinary
burn, Quash, Foul Play, Feint and redirection made the difference. A newly inflicted
burn was missing from the shared immediate physical-damage forecast; baseline
failure and seven fixed native cases are preserved. This victory does not certify
that AI defect or the whole Justified engine. No enemy team/level nerf was made.

**Jocelyn, E0039-B01 / TRAINER_JOCELYN 425.** Attempt one lost in 13 turns;
Volbeat was the last opponent at 36/74 HP. Dual Wingbeat missed on turns3 and8.
Her opening Protect denied Wingbeat and earned Speed Boost. Breloom's Protect
on turn5 blocked a knockout and activated Toxic Orb; turn6 switched to Crabominable
instead of gambling on consecutive Protect. Tailwind was set on turns2,6,10,
after each previous instance expired. Sirfetch'd/Whimsicott focused Sableye and
Combusken; Breloom removed Aerodactyl and Marshtomp. Turn9 Encore on 1-HP Pachirisu
had no observed payoff because Breloom killed it before its action. Quash on turn2
exposed a shared forecast omission: native Combusken was moved behind Wingbeat,
but the scorer had no Quash turn-order effect. Preserve the baseline fixture and
the repaired shared path passed four native controls before rematching. Max sampled decision was 55 native frames,
including setup (13 decisions); this is not a universal timing certificate.

Attempt two lost in eight turns; maximum measured decision56 frames. Attempt
three won in seven turns with four healthy survivors, after matching Tailwind,
removing Combusken with Feint/Wingbeat and Crabominable with Psychic. Volbeat’s
Thunder Wave missed on turn4; Helping Hand/First Impression killed Shaymin on
turn5. Breloom’s Protect had defensive value. Wingbeat missed on turn6 before
turn7 victory. Maximum of seven full decisions:61 frames. No team/level nerf.

**Brawly, E0040 / TRAINER_BRAWLY 266.** Five native losses:10,10,13,11,11 turns;
maximum measured full decision budgets45,51,49,61,45 native frames respectively.
Exact T038/U0170–0175 and relevant shared AI were checked before every attempt.
All five losses used original enemy levels. Hawlucha21 is now authored; rematch pending. Attempt3 removed Cobalion/Hariyama/Heracross; timely burn
let Aerodactyl survive Hawlucha at2HP. Hawlucha's Protect blocked Foul Play while
Conkeldurr removed Sableye's berry. Attempt4's Taunt disabled early Follow Me;
Quash/Psychic removed Cobalion on turn3. Kadabra's Gleam punished Hawlucha's
Close Combat drops on turn8. Pachirisu fell on turn9; Heracross/Conkeldurr won.
Full observations are in the canonical book, trace and timing reports. These are
competitive enemy choices and player preparation/sequencing problems; no new
AI defect has been demonstrated by these attempts.

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
  alternate-archetype coverage. The 35 earned wins are not35 fully closed
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

- Current session: `work/v4-c13-quash-fixed-play/trace.json`; predecessor sessions
  `v4-c13-burn-fixed-play`,
  `v4-c13-capture-repair-play`, `v4-c13-native-linux-play`, and the extended
  `v4-c13-brenden-final-ai-play`. Each preserves native inputs/screens/before states.
  Timing reports: `work/cristian-third-attempt-timing.json` and
  `work/jocelyn-first-attempt-timing.json`, `work/jocelyn-second-attempt-timing.json`,
  and `work/jocelyn-third-attempt-timing.json`. Baseline/fixed burn and Quash fixtures
  are under `work/burn-forecast-baseline`, `work/quash-forecast-baseline` and logs.
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
