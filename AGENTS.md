Current authorization (September 18): fix the full Inclement integration.
Keep the immediate free opening gift of Eviolite, Choice Band, Choice Specs,
Choice Scarf and Focus Sash; subsequent vendor copies are purchased. Preserve
other major battle-item discoveries unless a concrete conflict needs repair.
Proceed with repairs and native validation; the earlier discussion hold is lifted.

# Current user direction — September 18, 2026

Inclement Emerald is the default for story, world, progression and economy.
Preserve authored doubles teams and AI, the universal legal-move tutor, native
ability switcher, Flight Beacon and field-move convenience, reduced trainer
roster, previous-cap Leveler, and the requested EXP improvements. Battle items
are paid starting stock plus stock unlocked through successful acquisition.
Remove Center battle-set presets and Center stat editing; use Inclement's
existing EV/IV services. No Game Book or player guides. Do not add rematches.
Older campaign policies below apply only where consistent with this direction.
Current priority: core playability from a genuine new game, ordinary native input,
opening progression, Center services, and native Save followed by clean Continue.
Optional Contests, PokéNav extras and Frontier activities are last. One Astra
Medium implementation agent works under the main agent's read-only review;
no nested delegation. No commits, pushes, resets, or guide regeneration.
Read the current top of docs/CONTINUE.md. Preserve the delivered 105e9c9e test
ROM package when building later fixes.

Everything below is historical policy; apply only where consistent with the
current Inclement baseline and explicit retained features above.

## Goal and autonomy

- The user wants the game BUILT, PLAYED AND BEATEN. Continue the earned fresh-save
  Medium campaign, repairing demonstrated issues as you go. Do not stop at a
  chapter, a passing test suite, a design document or a playable title screen.
- When the user asks to continue/start goal work, create or resume the goal in
  `docs/GOAL.md` using the available goal tool. An account/machine change does not
  imply that the game goal is complete. Do not recreate an already active goal.
- Subagents (user direction, September 15): use Sonnet subagents for basic,
  well-scoped code implementation from a self-contained spec (files, constraints,
  verification command). The main agent reviews every subagent diff and runs the
  checks before it lands, and keeps design decisions, trainer teams
  and playtesting itself. No nested delegation.
- Preserve existing user/concurrent work. Use Git history for older source rather
  than introducing competing copies into the live tree. Commit and push supported
  progress within the user's authorized GitHub handoff/development scope.
- Favor deletion, reuse, explicit state and the smallest coherent change. Avoid
  new frameworks, abstraction layers and defensive systems without a real need.

## Ownership split (September 15, 2026)

Two sessions work this repository concurrently. One owns trainer teams and the
battle AI: `data/emerald_champions/emerald_champions_battle_teams.txt`, its
materialized outputs, `src/battle_ai*`, `src/battle_util*`, `test/battle/ai/*`.
The other owns story scripts, economy, world data, docs and the acceptance machinery (`scripts/*`, `docs/trainer-review-index.json`).
Do not edit the other side's files; commit by explicit path so a concurrent
working-tree change is never swept into your commit; re-materialize teams and
refresh the ladder index only after the team side's commits land. Never delete a
clone or worktree without `git status --ignored`; `work/` holds earned saves.

## Source ownership

- Inclement Emerald is the default design authority. Preserve the authored
  doubles teams and AI and the user's explicit quality-of-life exceptions.
- The Game Book and player guides were deleted at the user's request on
  September 18. Do not recreate them or require publication checks for builds.
- Edit exact trainers in `data/emerald_champions/emerald_champions_battle_teams.txt`
  and materialize with `scripts/emerald_champions_teams.py --write`. Never edit
  generated party projections independently.
- Report source agreement, native behavior, visual evidence and untested paths
  separately. Keep current work in `docs/CONTINUE.md` and commands in
  `docs/VERIFICATION.md`; these are development records, not player guides.

## Earned campaign, battles and difficulty

- The creative goal is an exceptionally demanding, elegant competitive doubles
  odyssey: every retained fight deserves attention, regional identity matters,
  and the campaign meaningfully showcases its Megas, beloved standouts and major
  competitive engines. Design the desired battle first; verify its AI and
  counterplay; then use individual level offsets as the primary difficulty lever.
  Low BST or a zero-faint win alone does not justify replacing a species. A themed
  baby team may need levels substantially above the player cap; an exceptional
  Mega may need a negative offset. The initial role ranges are not ceilings.
  Change composition for a better concept or demonstrated strategic defect,
  keeping authoring and generated source synchronized. Do not substitute stronger species
  for this design review or count a mechanic's mere presence as a showcase.
  Route aces, Gyms and villain bosses should demand serious optimization; League
  encounters are the culmination. Model failure is not an expert-human ceiling.
  Global level escalation is a later campaign-wide calibration pass, not a
  reason to inflate all levels before regional concepts and AI are validated.
- Assign an individual level to every opponent based on its actual build,
  partners and stage-available player counterteams; revisit earlier reviews too.
  Include legal detours, free preparation, evolution access and reachable legends.
  The user directs informed initial level choices now and tuning during later
  playtesting. Do not stall the five-team discussions for exhaustive simulations;
  keep chosen seeds distinct from demonstrated difficulty calibration.
- Historical earned-run order: C01, first-arrival Oldale C03, rival/send-off C02, then
  onward. Implement and inspect each reached chapter and handoff. Later synthetic
  scene tests never count as earned traversal. The fresh run's progress is in
  handoff/checkpoint.json; use it for the exact next encounter.
- Before calling a useful item unavailable, trace the actual bag, reachable
  harvests, gifts, shops, wild held items and native preparation sources. Obtain
  useful stage-legal berries through play; checking one vendor is insufficient.
  Use a suitable status-specific berry when it solves the same battle problem.
- The user now explicitly authorizes save editing to create stage-available
  Pokemon and optimized legal builds in the party/PC. Use the existing headless
  agent-preparation API and preserve a pre-edit save. Record habitat, evolution,
  quest prerequisites and equipment availability for each chosen build. Label
  these as assisted acquisitions, never native capture acceptance. Map tables
  alone do not establish availability: trace actual entrances, story/quest flags,
  badges, HMs, bikes, traversal obstacles, rods and evolution providers. Keep
  unresolved access out of the benchmark until verified. Continue
  actual native traversal and battles; do not write trainer defeats, battle
  results, story completion or future RNG. Native capture tests remain separate
  subsystem evidence and must not hold up trainer benchmarking.
- After every team revision, re-audit its executable strategy flags, bespoke
  tactics and relevant shared AI. Update obsolete plans with the loadout, and
  verify new partnerships before claiming they execute; reuse shared fixes where
  the same defect can affect other teams.
- Before EVERY trainer battle, briefly inspect the exact team, intent/tactics and
  applicable shared AI code. Expect possible quality decline beyond roughly the
  first 130 historically authored encounters. Freely improve demonstrated team,
  moveset, strategy or AI weaknesses; synchronize authoring and consumers.
- Every AI bug requires an affected-team index in the development evidence: earlier
  cleared encounters, bespoke tactics/implementations, and later authored teams.
  Trace every plausible shared path, repair every affected implementation, and
  run focused behavioral regressions for relevant mechanics and exceptions. Carry
  those regressions into future builds. A local fix is not complete while another
  identified path retains the defect. Separate verified, potential and unaffected
  cases; a source audit is not retrospective earned battle acceptance.
  Do not replay every affected trainer after a shared fix. Repair all affected
  owners, index the teams and use focused native tests where needed; broad
  retrospective battle replays belong to a later audit.
- Do not use Potions, Revives, X items or other manual healing consumables in
  this playthrough. Use native Center/story recovery; held effects and moves
  remain ordinary battle mechanics. Naturally activated held berries, White Herb,
  Weakness Policy and similar equipment are allowed. The Champions settlement
  restores battle-start held items (including berries) after the battle; they
  remain consumed during that battle unless a native move/ability restores them.
  Never add manual item healing to solve a loss.
- Review every trainer for insufficient difficulty, including ordinary route fights.
  A first-attempt win with fewer than three new player faints triggers review of
  a level increase, normally +1 to the lowest-level opponent, and a native retest
  if changed. Aim for at least three faints against a well-prepared competitive
  party; explain justified exceptions. This is a judgment threshold, not an
  automatic quota. Do not count pre-existing faints from an adjacent fight. Fix
  AI defects before compensating with levels; preserve themes and counterplay.
- Before every trainer, appraise its authored design, team composition, regional
  theme, distinctive plan and AI. Then audit the complete reachable Pokemon pool
  and construct the strongest practical legal team for that encounter, including
  available legendaries and Ultra Beasts. Assisted preparation replaces capture/
  shopping grind. A weak party or player mistake is not a difficulty benchmark.
  Record acquisition, build, role, matchup and outcomes to assess existing
  legendary level normalization. Propose any additional cap-relative penalties
  with evidence; do not implement new legendary penalties without approval.
- The most important creative check after building or auditing EACH team is
  "Could this team be cooler?" Ask it again after your own redesign, not only
  when reviewing someone else's existing team.
  Answer with a concrete judgment about species, partnerships, theme, memorable
  interactions and counterplay. Recommend a specific improvement when warranted;
  do not equate cooler with simply rarer Pokemon or higher stats. If the concept
  earns retention, explain why. Approved revisions update authoring, code and AI plan.
- Every Gym trainer and Gym leader must field six Pokemon. Build each Gym as a
  creative doubles showcase with unusual/rare species, meaningful partnerships
  and distinct room strategies. Occasional type-appropriate legendaries are
  authorized when they fit; balance their individual levels against the reachable
  player pool. Six slots or rare names alone do not earn design acceptance.
  Every Gym trainer should be an exceptional difficulty benchmark; leaders are
  a major step beyond. A comfortable win deserves scrutiny. Audit AI consequences
  before inflating levels; judge pressure/counterplay, not a quota of player faints.
- Preserve cool species, coherent themes, strong moves and correct AI. When a
  battle/gauntlet is unreasonable, prefer small individual level-offset reductions.
  Do not nerf competent AI or dismantle a strategy merely because it wins.
  After three failed optimized attempts, review preparation, mistakes and
  cumulative pressure; consider reducing an opponent by one or two levels where
  justified. This starts the review, not an automatic nerf. Do not
  prolong a blocked encounter through endless one-level cuts or demand perfect
  play. An isolated spike may warrant one cut; RNG-only losses need not.
- Review opposing Protect throughout EVERY fight: actual payoff, lost opportunity,
  partner survival, consecutive risk and pacing. Fix demonstrated empty, harmful
  or repetitive guarding; an eventual loss/win never excuses poor decisions.
- The opponent never reads the player's pending commands (user decision,
  September 15). It decides at turn start from full team/state, the trainer's
  knowledge flags, revealed sets, last used moves and native locks, and it
  scores Protect, Wide/Quick Guard and Fake Out on expected value under a mixed
  forecast of what the player might do. Never reintroduce committed-action
  reads, Protect quotas or cooldowns; tune the expected-value constants
  (`src/battle_ai_pair.c`, `PAIR_GUARD_*`, `PAIR_FORECAST_*`) against play.
  The complete opponent decision budget is 1.2 seconds including setup/both actors.
- Ordinary adjacent fights retain individual clears. Selected clearly announced
  endurance challenges may restart all legs. Preserve Lilith/Brenden's original
  overlap and individual wins; losing to Brenden does not reset Lilith.
- Terastallization does not exist in this game (user direction, September 16). There
  is no Tera gimmick, Tera Orb, Tera Shard, Tera Blast/Tera Starstorm, Tera type or
  Tera form, and the engine code for all of it is deleted rather than gated. Terapagos
  stays as an ordinary legendary: it carries the custom ability Terra, which changes it
  into SPECIES_TERAPAGOS_AWAKENED on switch-in through the plain ability form-change
  table - no orb, no charge, no Stellar type and no player-side counterpart. Do not
  reintroduce any of it, and do not "restore" Terra to Tera Shift.
- Continue on Medium. Difficulty is trainer levels only; preserve its formulas.
  New games initialize Medium, as explicitly directed by the user. Existing
  saves retain their selected difficulty. The earned save is already Medium.

## Overworld and visual work

- Mega Stone rewards begin only after Steven grants the Mega Ring/bracelet.
  Preserve later access and original discovery locations; no unusable early
  Mega gifts. Steven gives Aerodactylite with the bracelet; Roxanne gives Old Amber
  and Devon revival guidance. Couple later stone access to matching family/evolution
  access; a few reviewed Mega trainers may gift their showcased stone after victory.
  Badges, other rewards and story progression remain independent.

- Build a flowing new adventure with repeatable guide-free directions for EVERY
  required handoff and the required exploration integrated into the main story.
  Preserve Inclement routing unless an explicit user exception changes it.
- Main-path guidance must be unusually explicit: never assume a ROM-hack guide
  exists. Every gate names the next action, exact destination/person/entrance,
  and the nearest currently reachable source of each required resource. For a
  required move, name a catchable compatible species and its habitat. Make the
  nearby source naturally useful: manor Jigglypuff already know Sing. Give clear
  in-world destinations and goals, not menu/button walkthroughs or counter
  coordinates. Keep dialogue concise and natural. Repeat this guidance
  on blocked and return visits. Check the route and resource access in native
  play; apply any discovered guidance defect to analogous gates elsewhere.
  Optional trades, rewards and discoveries may reward exploration; mandatory
  progress must never depend on guessing a hidden resource or consulting a guide.
- New/materially changed scenes require native headless execution, meaningful
  screenshots, contact-sheet inspection and refinement: choreography, facing,
  entrances/exits, waits, camera, labels, collision, outcome and re-entry behavior.
  Correct collision or plausible code alone is not visual acceptance.
- Group unmodified captured pixels in labeled contact sheets by default. Preserve
  originals, hashes, trace references and earned/synthetic provenance. Never use
  generated artwork to recreate gameplay evidence. Inspect a single frame/crop
  only for a concrete detail. Present scene evidence for the user's visual review;
  do not claim the user has approved images they have not accepted.
- Prefer repurposing obsolete ORIGINAL Emerald/Inclement pickups. Presence in this
  checkout does not prove original placement. Respect the rejected Diancie (7,9)
  sparkle; do not replace it with another invented coordinate. No arbitrary sparkles.
- Physically visit and exercise EVERY functional NPC: shops, gifts, trades,
  missions, preparation, rewards and other non-dialogue interactions. Review
  their actual native screens in contact sheets. Assisted team creation does
  not replace this economy/service acceptance. At each chapter reconcile all
  active dialogue and systems with the intended Inclement behavior; record unvisited branches as
  pending. Decorative furniture does not substitute for functional coverage.

## Verification and delivery

- Audit failed checks first: requirement, fixture/input, config, source and artifact
  identity. Delete stale checks; never weaken a valid assertion just to pass.
- Test observable behavior with focused native regressions for real defects. Avoid
  mirrored/redundant tests and repeated unchanged suites. Preserve baseline failures
  and fixed-case evidence when they establish the repair.
- Build/stamp the exact tree. A fresh input stamp cannot repair stale objects.
  Keep ROM, ELF and input stamp together. Resume changed ROMs through an actual
  in-game Save and clean Continue, never a cross-ROM savestate.
- Distinguish generator/static checks, native fixtures, actual complete battles,
  campaign traversal, visual review, timing samples and full-game acceptance.
  Do not turn a count of passing tests or trainer flags into broader proof.
- Preserve fresh-run save integrity, full-storage/bag pending rewards, temporary
  party restoration and native Retry versus Reload. Field medicine stays available;
  free preparation must not become a resale exploit. No legacy save migrations.
- Deliver a source-bound normal ROM and working downloads, organized evidence and
  the source and development evidence. Upload success alone is not delivery; verify downloads.
