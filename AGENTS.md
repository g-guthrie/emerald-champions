# Working on Emerald Champions v4

Read `docs/CONTINUE.md`, `docs/GOAL.md` and the relevant parts of the single
canonical `Game Blueprint/Emerald_Champions_Game_Book.txt` before working.
This repository root is the active native pokeemerald-expansion implementation.
The separate replacement-engine experiment is parked and is not the game to run.
Old 7.0/7.1 branches and archived reports are evidence, not competing instructions.

## Goal and autonomy

- The user wants the game BUILT, PLAYED AND BEATEN. Continue the earned fresh-save
  Medium campaign, repairing demonstrated issues as you go. Do not stop at a
  chapter, a passing test suite, a design document or a playable title screen.
- When the user asks to continue/start goal work, create or resume the goal in
  `docs/GOAL.md` using the available goal tool. An account/machine change does not
  imply that the game goal is complete. Do not recreate an already active goal.
- The one authorized fresh Astra High full-book review is complete; section 26
  contains its dispositions. The user stopped subagent work. Use no additional
  agents or agent follow-ups. The main agent owns implementation and playtesting.
- Preserve existing user/concurrent work. Use Git history for older source rather
  than introducing competing copies into the live tree. Commit and push supported
  progress within the user's authorized GitHub handoff/development scope.
- Favor deletion, reuse, explicit state and the smallest coherent change. Avoid
  new frameworks, abstraction layers and defensive systems without a real need.

## Book and source stay together

- The book owns intended design. Source implements the game. Reconcile a mismatch;
  do not silently treat either stale prose or accidental code as the final choice.
- Every gameplay change must update its book rule, exact party/build/tactical
  record, habitat, progression, reward or scene description in the same change.
  Update implemented/pending statements and evidence scope too. Do not leave the
  book describing a discarded experiment. A hypothesis is not an approved policy.
- For trainer changes, edit book sections 8/9/22 and relevant executable-intent
  rows, then run the book import and check in `docs/VERIFICATION.md`. Do not edit
  generated tables alone. Shared preparation presets are separately authored;
  do not overwrite bespoke trainer moves or EVs with player presets.
- Keep the full specification in this one book. `docs/CONTINUE.md` owns checkpoint
  and next-work state, `docs/VERIFICATION.md` owns commands, and `docs/GOAL.md` owns
  the goal. Do not accumulate new competing handoffs or historical task lists.

## Earned campaign, battles and difficulty

- Follow book play order: C01, first-arrival Oldale C03, rival/send-off C02, then
  onward. Implement and inspect each reached chapter and handoff. Later synthetic
  scene tests never count as earned traversal. Current progress is C15; use the checkpoint for the exact next encounter.
- Before calling a useful item unavailable, trace the actual bag, reachable
  harvests, gifts, shops, wild held items and native preparation sources. Obtain
  useful stage-legal berries through play; checking one vendor is insufficient.
  Use a suitable status-specific berry when it solves the same battle problem.
- Catch and prepare stage-available Pokemon through native play. Do not write
  party, progress, RNG, trainer-defeat flags or battle results into the earned
  run. Read-only diagnostics and the observation bridge are allowed. Synthetic
  fixtures remain separate and must be labeled as such.
- Before EVERY trainer battle, briefly inspect the exact team, intent/tactics and
  applicable shared AI code. Expect possible quality decline beyond roughly the
  first 130 historically authored encounters. Freely improve demonstrated team,
  moveset, strategy or AI weaknesses; synchronize the book and consumers.
- Every AI bug requires an affected-team index in the canonical book: earlier
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
  Zero or one player faint is a trigger to assess sustained pressure, matchup and
  useful enemy actions. Propose concrete team or level improvements when helpful;
  do not turn the faint count into an automatic quota. Fix AI defects before
  compensating with levels, and preserve thematic species and strong counterplay.
- Every Gym trainer should be an exceptional difficulty benchmark; leaders are
  a major step beyond. A comfortable win deserves scrutiny. Audit AI consequences
  before inflating levels; judge pressure/counterplay, not a quota of player faints.
- Preserve cool species, coherent themes, strong moves and correct AI. When a
  battle/gauntlet is unreasonable, prefer small individual level-offset reductions.
  Do not nerf competent AI or dismantle a strategy merely because it wins.
  After roughly five failed, meaningfully prepared attempts, review cumulative
  pressure and make coordinated, material level cuts where justified. Do not
  prolong a blocked encounter through endless one-level cuts or demand perfect
  play. An isolated spike may warrant one cut; RNG-only losses need not.
- Review opposing Protect throughout EVERY fight: actual payoff, lost opportunity,
  partner survival, consecutive risk and pacing. Fix demonstrated empty, harmful
  or repetitive guarding; an eventual loss/win never excuses poor decisions.
- Full team/state plus already-committed player moves/targets is approved at all
  difficulties. It does not include future RNG or uncommitted future choices.
  The complete opponent decision budget is 1.2 seconds including setup/both actors.
- Ordinary adjacent fights retain individual clears. Selected clearly announced
  endurance challenges may restart all legs. Preserve Lilith/Brenden's original
  overlap and individual wins; losing to Brenden does not reset Lilith.
- Continue on Medium. Difficulty is trainer levels only; preserve its formulas.
  New games currently initialize Hard regardless of the title preference; changing
  that behavior needs a design decision. The earned save is already Medium.

## Overworld and visual work

- Build a flowing new adventure with repeatable guide-free directions for EVERY
  required handoff and the required exploration integrated into the main story.
  Do not leave old Inclement routing that contradicts the book.
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
- Do not stall campaign progress on exhaustive furniture/menu/inspection checklists.
  Cover meaningful interactions, progression, services, rewards and demonstrated
  failure branches in proportion to risk.

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
  the maintained book. Upload success alone is not delivery; verify downloads.
