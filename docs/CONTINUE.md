# Current development checkpoint — September 14, 2026

Read `AGENTS.md`, `docs/GOAL.md`, and the relevant sections of the single canonical
`Game Blueprint/Emerald_Champions_Game_Book.txt`. This file owns current work and
resume state; the book owns design and historical acceptance evidence;
`docs/VERIFICATION.md` owns commands. Do not start another full-book review or
spawn agents. The native engine in this repository is the active project.

## Active work: story gating and permanent tower consequence

The user approved the story-gate/mandatory-encounter/ticket/finale changes in
both Book and native game. C26 basement research/report now precedes fossil
collection; either fossil permanently collapses the tower. All three external
approaches are sealed afterward: Route111, the elevated ruins and Desert
Underpass. Zygarde and Stakataka are intentionally missable. The required
archaeologist and final fossil YES/NO dialogue name both and the permanent loss.

The major story transitions now check their preceding accomplishments. Space
Center, Deoxys, Scanner and all three Regi challenges precede the submarine
climax; the acute Sootopolis/Sky Pillar response stays uninterrupted. C45 requires
all three puzzles AND win-or-catch guardian resolutions. C48 requires both
observations and Diancie/Jirachi resolutions, then Steven's Mossdeep conclusion.
Steven directly awards cap94/20000; Devon is an optional epilogue with any
unclaimed Pidgeotite preserved. The League checks C48 and eight badges.

Earned travel survives physical-ticket storage failure. Any Center nurse retries
pending documents without withholding healing or duplicating delivered papers.
Native Studio recordings in VERIFICATION cover the key blocks, collapse choices,
entrance closures, forced battle outcomes, missing/repeat finale and ticket Bag
boundaries. These are synthetic scenes; forced wins/catches/losses are not combat
or earned campaign evidence. The earlier permanent-entrance and optional C48
battle decisions are superseded. User visual approval and full route traversal
remain separate acceptance work.

The earlier section17 choreography implementation remains in place. Earned C15,
team reviews and export delivery stay paused; no additional legendary placement
was authorized. The live Studio sandbox remains independent of the earned save.

## Paused work: collaborative team reviews

The user paused the earned playthrough and export delivery to review teams in
small waves. Reviews now reach retained battle28, the end of Rustboro Gym.
Approved revisions through that point are implemented; Joey16 remains on hold
because of overlap with Calvin’s Belly Drum concept. Do not interpret reviewed
as difficulty-tested. Individual levels are informed Medium-baseline seeds.

The nine-commit handoff was restored and published at bd0a36f. Resume the team
discussion when the user asks; consult the book’s retained ordering rather
than confusing encounter numbers, E groups and party variants.

| Rustboro encounter | Approved design |
| --- | --- |
| 25 Josh | Glimmet / Nosepass / Dwebble / Alolan Geodude / Naclstack / Onix; mineral chemistry, Shell Smash and Galvanize |
| 26 Tommy | Cradily / Tirtouga / Anorith / Shuckle / Corsola / Binacle; fossil reef, Surf–Storm Drain and Power Split |
| 27 Marc | Amaura / Stonjourner / Tyrunt / Klawf / Archen / Cranidos; snow support, Power Spot and prehistoric attackers |
| 28 Roxanne | Mega Aerodactyl / Carbink / Relicanth / Tyrantrum / Omastar / Bombirdier; Tailwind, screens and fast fossil offense |

Roxanne has **no Trick Room**. Aerodactyl leads and is her sole Mega slot;
Carbink supplies screens/Rock Tomb, Relicanth and Omastar invest in Speed,
Tyrantrum attacks immediately with Assault Vest, and Bombirdier backs up Tailwind.
The scoped MEGA_REVEAL directive requires existing native Mega eligibility.
Her postbadge speech introduces Mega Evolution before the Old Amber reward.
Coalossal remains a volcano showcase, Minior remains at Meteor Falls, and
Regirock awaits a later important desert/ruins review. No later team was changed
merely to reserve those species.

Every individual audit explicitly asks “Could this team be cooler?” Explain the
species, partnerships, biome, counterplay and AI. All Gym trainers/leaders need
six Pokémon. Design first, then tune individual levels. Opponents use signed16
level offsets and native u8 battle levels (1–255), not a level100 balance ceiling.
New games initialize Medium; difficulty changes trainer levels, not AI quality.

## Book, economy and verification

- Current census: **343 encounters / 325 E groups / 370 party variants**.
  Exact roster check: book variants = nonempty native parties = Hoenn script IDs.
  All57 Gym trainers and8 leaders have six. Retired IDs retain empty metadata;
  146 obsolete loadouts remain excluded after Gina/Mia’s restoration.
- Opening economy through battle16 is reconciled in the book. Oldale gives3
  Heal Balls; the Woods researcher/nurse gives3 Dusk Balls; the visible southern
  Route115 pickup gives3 Net Balls. Earlier finite rewards retain pending
  receipts. Native reward/full-storage/retry checks are scoped in the book.
- Roxanne gives Old Amber and Devon revival directions. Steven gives Aerodactylite
  with the bracelet after Letter/Brawly, including Bag/PC fallback and retry.
  Couple future stones to obtainable families; selected reviewed Mega trainers
  may give their showcased stone. No bulk later gift assignment is approved.
- All129 NPC bindings in29 opening maps were inventoried/source-reviewed.
  Source alignment is not a claim that every interaction was exercised. The
  specialist overlay did not reproduce in the recorded native checks; do not
  invent a fix. Optional untested storage/service branches remain open.
- Source import/ability/roster checks pass. Normal production build and release
  gates pass: `work/rustboro-final-release.log` and matching `-gates.log`.
- Focused native Mega reveal (with/without stone) and compiled-plan tests pass:
  `work/rustboro-focused-tests.log`. Earlier primary-support, weather-survival,
  trainer-level and economy regressions are documented in VERIFICATION/book.
- `work/roxanne-mega-native-v2` shows native Mega Evolution before the player
  bracelet, then a weak fixture-party loss. `work/roxanne-rewards-v2` uses a
  separate automatic victory to check the real badge/speech/reward scripts:
  Old Amber1, Ring0, Aerodactylite0 and returned controls. Neither is an earned
  clear or a difficulty benchmark. Screens: `work/contact-sheets/rustboro-mega-reveal-native.png`.
- The trainer exporter is available at `scripts/export_trainer_catalogue.py`;
  delivery remains paused. Regenerate before delivering any old TXT snapshot.

## Paused earned playthrough: C15 Dwayne experiment

**Do not advance this battle while the user has paused playthrough.**
`handoff/checkpoint.json` owns machine-readable state and hashes. Current session:
`work/v4-c15-dwayne-revised-play`, step024 `t5-board`, turn-five command selection,
Medium, cap24, inside Route109 Seashore House. No turn-five commands submitted.

Original Dwayne was beaten in four turns with zero player faints in
`work/v4-c15-assisted-play`, step054. That is the 41st distinct evidenced win;
the retest began from the prebattle save with40 clears. His evolved
Magmar/Jynx/Electabuzz/Monferno roster is an **experiment**, not an accepted final
concept. The user then clarified that themed low-stat teams should be tuned
through levels, not evolved merely to increase stats.

| Current party | HP | Battle position |
| --- | --- | --- |
| Mega Aerodactyl | 0 | Fainted |
| Munkidori | 40/83 | Reserve |
| Alakazam | 0 | Fainted; critical Thunder Punch |
| Azumarill | 67/104 | Active |
| Kartana | 70/70 | Active |
| Togekiss | 97/97 | Reserve |

Opponents: Electabuzz25/72 and Monferno54/72. All player members are level24.
Preparation/access manifests: `handoff/benchmarks/c15_dwayne.json` and
`c15_available_pool.json`. The user authorized stage-legal assisted acquisitions;
these are not native capture acceptance. Access checks must include paths,
story flags, licenses, bike requirements and evolution/equipment providers.

Preserved battery saves:
- `work/v4-c15-assisted-play/pre-dwayne.sav`
- `work/v4-c15-assisted-play/dwayne-original-won.sav`
- `work/v4-c15-berry-reconciled-play/pre-assisted-preparation.sav`

The immutable midbattle session uses an older ROM. Resume it only with its own
ROM/ELF; to test current code, use the actual prebattle battery save in a clean
boot of the new build. Never transplant a savestate across ROMs or write earned
wins/story progress. The original-win save is a separate completed branch.

Brawly was won on attempt7; Steven’s Ring handoff, local Jigglypuff capture,
manor song and sailing are earned. Hailey, Edmond, Lola and Chandler are cleared.
Their historical battles/party snapshots live in the book and immutable traces,
not as competing current instructions here. Ordinary overlapping trainers keep
individual wins; do not introduce full gauntlet resets.

Slateport Mart Shiny Stone/Linking Cord/Great Ball purchases, duplicate Cord
refusal and stock-end/Cancel were exercised. Full-bag/insufficient-money branches
remain pending. The Seashore owner introduction was exercised; its three-trainer
reward remains pending. When playthrough resumes, settle Dwayne’s design/retest,
then Johanna, Simon, Mel/Paul and the museum in book order.

## Portable resume and remaining work

Clone `main`, install dependencies from VERIFICATION, inspect the checkpoint,
and fetch assets using `scripts/playthrough/fetch_handoff.py`. Check asset dates
and hashes: old `v4-handoff-20260913` downloads predate the C15 continuation and
must never silently replace a later local save. The source checkout is authoritative
for current code; each saved session remains bound to its own immutable artifacts.

Continue the whole campaign when the user resumes play. Audit every trainer’s
team/AI and each reached area’s dialogue, gates, economy, services and rewards.
Use strong stage-legal preparation, native combat and no manual healing items.
Review insufficient pressure and repeated optimized losses; faint counts are
signals, not quotas. Revisit affected shared AI owners through focused regression
checks rather than automatically replaying every previously won trainer.

The full game is not played, balanced or visually accepted. Remaining work includes
later team reviews, level calibration, story/service/return-path coverage and
full campaign completion. Keep the legendary rule unchanged unless explicitly
approved: only legendary/mythical species above600 BST receive the existing
floor(chapterCap×600/BST) cap. Six hundred BST Shaymin keeps the full cap.
