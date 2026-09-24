# Current direction — September 19, 2026

Main owns a gate-by-gate source audit of the entire story, in progression order.
For each gate, trace prerequisites, map callbacks, every NPC dialogue branch in
that area, battle outcomes/retries, rewards, receipt flags, exits and the next
reachable destination. Refactor and debug files as concrete opportunities arise;
opening a file does not require cosmetic changes. Preserve existing work.

Latest user delegation rule: retain only one reusable Sol Low worker
(compatibility_audit); all other existing workers remain stopped. Do not spawn
additional workers. For each confirmed bug, give that worker a bounded scan of
related callers/data and analogous bugs elsewhere. It may fix only straightforward,
source-proven cases in explicitly assigned files; main handles anything complex,
ambiguous, behavioral or design-sensitive. No nested agents or worker builds.
Main reviews all worker edits and serializes builds. Keep reports concise.

Proceed route by route and person by person, including optional interactions:
NPC dialogue and state branches, battle/rematch promises, gifts and full-Bag
retries, item identity/quantity/duplication and reward value, map callbacks,
story/HM unlocks, re-entry and exits. Early Eviolite/Choice/Focus access changes
later reward incentives; useful extra copies are not automatically bugs. Retain
Inclement placement locations. Keep coverage and unresolved findings in
`docs/STORY_GATE_AUDIT.md`. Use targeted headless screenshots for concrete visual
or choreography doubts, and focused behavior checks for risky changes. Never
claim reading source proves every bug is gone.

Preserve the pre-integration authored doubles AI and trainer teams. Compare
questionable behavior with pre-integration main508775fad8 before changing it;
explain a proposed behavioral change to the user first. The experimental guard
forecast patch is parked under work/code-cleanup-20260919 and restored out of
active source. Batch11 contains that experiment and MUST NOT be delivered.

Inclement is the world/story/progression/economy baseline, with retained native
ability switching, universal legal-move tutor/no TMs, HMs (license + badge + capable party member, no moveslot),
Flight Beacon, current-cap Leveler and normal modern XP and the five immediate held-item
gifts with paid discovery stock. Preserve the native EV/IV services. No Game
Book, player guides, Center battle presets or Center stat editor. No new ground
or hidden item placements: only replace original Inclement pickups or TM gifts.

First explicit visual repair: fix the Champions title artwork. Measure the
indexed canvas, palette, sprite/tile layout, placement and animation; preserve
the native pixel style and verify the complete outline with headless captures.
Use targeted native screenshots only for concrete visual or behavioral doubts;
no campaign playthrough. Never represent source review as gameplay acceptance.

Preserve existing work, archives and saves. No reset/clean/wholesale restores.
Only remove our own reviewed experimental changes, preserving a patch first.
No simultaneous builds. Build and run focused checks proportionate to changes;
inspect failing tests for actual contract correctness rather than force green.
Keep a concise source-review ledger and current handoff. Do not update memory
files, publish, push or merge without applicable user authorization.

Latest approved convenience rules: HMs require their story license, badge, and a
party member able to learn the move; no move-slot requirement. Flight Beacon may
use a Fly-capable Pokémon from the party or PC. Preserve legendary puzzle conditions.
Leveler raises every non-egg party member to its current species cap. Standard
modern full/half-share XP below the cap; no custom flat/catch-up bonuses and no
XP award/message/animation at cap. Consumed held Berries stay spent until the
Regenerator Key Item is received from Norman after Badge5. Then they restore
after battle, never during it. Other held-item restoration remains unchanged.

Latest direction supersedes blanket rematch removal. Keep routine route/Gym
rematches and their UI clutter retired, but restore the exact authored post-League
Wally fight. Build a deliberate finale: League → Wally → five S.S. Tidal cabin
teams → Steven → Birth Island/Deoxys (defeat or capture) → Buffel final trial.
Preserve repeat Elite Four. User explicitly authorizes strengthening Buffel/boat
teams and native tactical playtests; do not rewrite Wally's authored team or
shared planner casually. Latest clarification: full moveset knowledge is acceptable;
preserve the shared authored information profile because all 368 battle entries
were tuned with it. OMNISCIENT/KNOW_OPPONENT_PARTY here provide loadout knowledge,
not committed-command access. Their temporary removal and the public-forecast
experiment have been reverted. Never let opponent scoring read the player's
selected move, target, decision to switch, or chosen replacement. Audit reachability
across the whole campaign before any AI change; a flag name alone is not evidence.
Never claim literal unbeatable/perfect gameplay from limited benchmarks.

Latest held-item clarification: consumed held Berries return after battle with
the tool; Knock Off removes items only for the current battle, while Thief and
Covet transfer them permanently in ordinary trainer battles. Keep Cud Chew, Recycle,
Harvest and all normal in-battle effects functional without the tool. A genuinely
recovered original Berry is no longer lost. Preserve non-Berry restoration policy.
No cross-task coordination messages: user explicitly requested they stop.

# Pokémon distribution — September 23, 2026 (ground truth is the source)

The "Legendary Sign" encounter engine is retired. Every wild Legendary, Mythical
and Ultra Beast is an ordinary slot in `src/data/wild_encounters.json`; the only
engine-side data is the slim gate table in `src/data/pokemon/legendary_signs.h`
(species, unlock flag, required caught family, minimum badges, kind). A gated or
already-caught slot rerolls to the next slot. Any Legendary-class or Ultra Beast
wild slot or scripted battle spawns at the current level cap with a competitive
set (authored set from `legendary_authored_sets.h` when one exists). Roaming
Latias/Latios follow the cap. Rarity ladder, enforced by `test/wild_slot_odds.c`
and `scripts/verify_wild_distribution.py`: legends exactly 1%, at most two per
table; Ultra Beasts and Paradox 2-3%; ordinary slots never below 2%; every
method totals 100 via per-table `encounter_rates`. Sweet Scent multiplies live
legend/UB slots by five. Party rule everywhere, including the League door: one
Legendary/Mythical, one Ultra Beast, one Paradox. Regigigas needs the three
Regis caught in the Pokédex, not carried. Static legendaries are high stakes like
the vanilla games (owner rule, September 23): a knockout loses one for good, fleeing
keeps it, capture resolves it; save before them. Norman gives the Mega Stones that fit
the player's starters (Swampertite if none apply) and every other starter stone has one
dynamic home. Cut trees roll one shared table (Skwovet, Pineco, Aipom, Burmy, Applin,
Phantump) and those six live nowhere else. The Center guide is the built-in guide:
legend leads, the next story destination and the local side quests, requirements stated
once in prose. Route sign rosters page with A and close with B. Arceus is the Devon researcher's gift
after the Hall of Fame. Groudon/Kyogre weather reports open after the Sootopolis
crisis. The Champions Circuit gives no Pokémon; its former rewards live in the
campaign. Game Corner Pokémon prizes arrive at the cap; starters are one-time.
Water and rod tables use narrow level bands tied to when Surf/each rod opens;
land, Rock Smash and honey bands sit just under the cap of the gate that opens
the map (table maximum = cap minus 4), so no dungeon is trivially under-levelled.
Do not reintroduce percentage text in dialogue; the route sign rosters and
Center leads describe location and requirements only. Tune odds and timing in
the JSON and gate table, never in prose.

Weather anomalies (September 23, 2026). Twenty-two legends are "visitors": they
are wild only while a weather anomaly is live on their home route. Anomalies
exist from the Weather Institute rescue (FLAG_HIDE_ROUTE_119_TEAM_AQUA) until
Rayquaza calms the sky (FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE): four live at a
time, each lasting 1,500 player steps, refilled from unlocked uncaught visitors
with a one-cycle cooldown and never two on one map. A live anomaly overrides the
map's sky weather at runtime (rain, thunderstorm, downpour or horizontal fog
only; routes and caves, never cities; terrain weather such as the desert
sandstorm and Route 113's volcanic ash always wins, so no visitor lives there) and gives its visitor a flat 20% of encounters
there. Outside its anomaly a visitor's slot is inert; after the window closes
uncaught visitors become ordinary 1% residents. The visitor table lives beside
the gate table in src/legendary_signs.c; state is in save vars. The Institute
2F scientist lists live anomalies (BufferWeatherAnomalyReport); route signs add
one line while a storm is on. Codex owns overworld presentation: this system
adds no objects and edits no map.json.
