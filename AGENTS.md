# Current integration direction — September 19, 2026

Work on `inclement-merge` in this checkout. Inclement Emerald is the default
world/story/progression/economy reference. Keep the authored doubles teams and
AI, universal legal-move tutor without TMs, native ability switcher, Flight
Beacon and capability-based field moves, previous-cap Leveler and EXP changes.
Give Eviolite, Choice Band, Choice Specs, Choice Scarf and Focus Sash immediately;
subsequent vendor copies are paid, with stock unlocked by successful acquisition.
Use Inclement EV/IV services. No Center presets/stat editor or player guides.

Latest user direction: perform a full native integration/debug pass. Keep the
components together and validate in stages instead of deleting/rebuilding them.
Birch's rescue must be doubles with the requested Mightyena overworld staging.
Fix dialogue, choreography, HM dependencies, useless retired-trainer NPCs,
hot-spring rendering, and wild/Ultra Beast acquisition. Optional activities last.

Record battle outcomes and concrete strategy/AI defects during integration. Do
not apply historical faint-count quotas as automatic level or team changes;
preserve the authored campaign while proving traversal and system coherence.

The main agent coordinates TWO Astra Low implementation agents (no nested
agents). Campaign agent owns `data/maps/**` and campaign-scene evidence. Systems
agent owns engine C, headers, `data/scripts/**`, relevant graphics/configuration,
and wild/legendary data. Send requested cross-owner changes to the owner; do not
edit their files. Authored trainer/AI files remain protected unless the main
agent explicitly approves a demonstrated defect repair. Main owns these working
directions, Git coordination, review, builds and delivery. No simultaneous builds.

For each confirmed bug, main dispatches or extends a read-only Sol Low audit for
other instances of the same cause. Group related instances into one sweep; report
confirmed defects separately from hypotheses and route fixes to the two owners.
Check actual reachability and engine contracts before promoting an audit finding.

Preserve the three starting dirty files recorded in
`work/integration-20260919/starting-state.json`. No resets, cleans, wholesale
historical restores, or guide generation. Do not commit/push/merge without main
coordination. Preserve old ROM/save evidence and bind every new test to its own
ROM/ELF/stamp. Native fresh-save progression, synthetic fixtures, static gates
and actual battle quality are separate evidence. Never mark skipped play as done.

User's explicit item-placement constraint: always follow Inclement's overworld.
Do not add any new ground or hidden item locations. Added Mega Stones may replace
existing Inclement pickups (using native sparkle graphics) or existing TM gifts.
Preserve original coordinates, access, map layout and receipt/actor identity.
Do not invent extra sparkle spots, move rewards to new locations, or duplicate
stone sources. Current source already gives every added stone one source.

Design coherence is part of acceptance: trace each retained QoL change through
progression, incentives, world rewards/shops/quests, availability and dialogue.
A technically valid Rare Candy replacement is not automatically a worthwhile
reward under the Leveler/XP system. Keep discretionary economy/distribution
redesign pending review of actual stage purpose and concrete player tradeoffs.
See the latest explicit coherence criteria in docs/GOAL.md.

## Execution and protected work

- Keep the full goal active until the game is built, played and beaten with its
  integration requirements verified. A build, chapter or green suite is not completion.
  Use the existing active goal; do not create duplicates.
- Preserve authored trainer teams and AI unless main approves a demonstrated
  defect repair. Canonical trainer source is
  data/emerald_champions/emerald_champions_battle_teams.txt; materialize through
  scripts/emerald_champions_teams.py, never edit generated parties independently.
- Before a trainer, briefly inspect its actual team and strategy. Record outcomes,
  useful/detrimental Protect use and concrete AI defects. No automated faint quota,
  arbitrary level inflation or wholesale team redesign during integration.
- Opponent AI must not read the player's pending commands. Preserve expected-value
  decisions and its existing 1.2-second whole-turn decision budget; do not add
  guard quotas/cooldowns. A shared AI defect needs an affected-team index and
  focused regression coverage, including earlier and later affected encounters.
- Ordinary adjacent wins persist after later losses. Only explicitly announced
  endurance gauntlets may reset all legs. Preserve native Retry/Reload semantics.
- Continue the earned save on Medium. Difficulty changes trainer levels only;
  existing saves keep their chosen setting. Do not reintroduce Terastallization,
  Tera items/moves/types/forms, or replace Terapagos's ordinary Terra/Awakened
  ability form with Tera Shift.

## Native acceptance

- Use native traversal and real battle choices/outcomes. Never write victory,
  trainer-defeat or story-completion flags to claim earned progress. Synthetic
  prerequisites are allowed for labeled, bounded subsystem/scene tests.
- Stage-legal assisted party preparation is authorized with a preserved pre-edit
  save and evidence of habitat, traversal, evolution, item and service access.
  Keep it separate from native capture acceptance; unavailable future resources
  or fabricated outcomes are not allowed.
- Do not use manual Potions, Revives, X items or similar battle healing in the
  earned benchmark. Native recovery, moves and held effects remain valid. Field
  medicine remains available to the actual player. Preserve postbattle held-item
  restoration while consumption still matters within each battle.
- Exercise functional NPCs as reached: shops, gifts, trades, training, quests and
  rewards. Assisted preparation does not substitute for service acceptance.
  Record unvisited branches as pending; source reachability is not a native visit.
- Material scene changes need native execution and inspected unmodified pixels:
  choreography, facing, collision, camera, messages, exits, outcomes and retry.
  Keep original frames, labeled contact sheets, hashes and earned/synthetic scope.
  Never fabricate gameplay evidence or imply the user approved unseen images.
- Existing progression directions must truthfully identify reachable next steps
  and prerequisites. Keep dialogue concise and natural; do not revive old guide
  quests or assume a removed historical habitat/provider still exists.

## Verification and delivery

- Audit failing gates and fixtures before editing production code. Confirm the
  requirement, inputs and artifact; never weaken a valid check to manufacture green.
- Use focused behavioral regressions for real defects. Avoid mirrored checks,
  redundant suites and repeated unchanged tests without a concrete new reason.
- Only main runs shared builds, during an agreed source freeze. Bind ROM, ELF and
  input stamp to the exact built tree; stamping cannot make stale objects fresh.
- Use actual in-game Save and clean Continue between ROMs. Never load a savestate
  into a different ROM. Preserve earned saves, temporary-party restoration,
  full-storage delivery retries and original before/after evidence.
- Distinguish static checks, fixtures, actual battles, traversal, visuals and full
  campaign acceptance. Do not infer whole-game success from a passing test count.
- Deliver a verified normal ROM (no fixture controls) and preserve prior builds.
  Keep current implementation/evidence/Git state in docs/CONTINUE.md, not player
  guides. State remaining work honestly; do not merge main prematurely.

Superseded September15 policies (including early Steven Mega unlock and automatic
battle retuning) were removed from this active file. Their historical text remains
in Git history and work/integration-20260919/historical-AGENTS-before-consolidation.md.
