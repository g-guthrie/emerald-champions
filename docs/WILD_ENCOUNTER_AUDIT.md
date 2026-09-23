# Wild encounter audit — September 19, 2026

## Inclement Honey habitats restored — September 22, 2026

The September 21 caller finding below is superseded. Original Inclement gives
Honey separate six-slot encounter rosters, but the integration had replaced
Honey use with Sweet Scent and left 29 Honey tables unreachable. Restored Honey
selection, field script and Pokédex sources. Honey now works only while standing
on a land-encounter tile in a map with a Honey table. Invalid use explains the
failure and preserves the item. Route signs list Honey separately from Hidden
and explain that Sweet Scent changes only grass/Surf odds.

Honey had also been sorted into Medicine by its status-recovery sort tag,
despite being a field lure. Changed its sort tag to field-use, placing it in
Items with Repel and Lure. A scan of other field-use items found no second
status-recovery misclassification. The distribution verifier now includes
Honey in potential ordinary sources: 671 canonical species across active
Hoenn encounter tables, versus 633 before this path was wired. These counts
do not prove every map tile is reachable.

Focused native tests pass for six-slot selection, Pokédex membership, item
pocket and all 294 route-sign renderings (maximum 700 of 1000 bytes). A further
native layout test found at least one non-colliding land-encounter tile on each
of the 29 Honey maps. It does not prove that every tile is reachable from the
map entrance or survives dynamic on-load changes. A native
headless scene on ROM SHA-256 `703523a450624fbba79c03f4b40a881cdb5b38eb94faa0359fd9bf1577ab7589`
shows Route 101 Honey attracting Audino and reducing Bag quantity from 3 to 2;
the path-tile scene shows readable refusal text, no battle and quantity 3.
The Honey-scoped release ROM SHA-256 was
`69f1d58a81c97d62947c7077c3bab68fedef07a884d26391ee7bb19bc3356a76`;
it included a later S.S. Tidal script edit and passed release build/gates and
input-stamp check. The current release also includes a separate AI switch
legality fix; the Honey scene ran on the preceding headless build with the same
Honey implementation. Evidence is in
`work/honey-revival-20260922/`.

Open design issue: all 29 imported Honey rosters spawn at level 2–3, and many
early maps yield already-evolved Pokémon or common Audino. Honey first appears
in general shop stock after Badge 1, while the Center gives the Leveler on its
first visit. These Inclement XP-farm rosters are therefore active alongside a
system that removes their original leveling incentive. There are 38 canonical
species with Honey as their only *ordinary Hoenn table* source; this does not
mean all 38 are otherwise unobtainable, since evolution and gifts are separate.
Roster adaptation is awaiting a design choice; do not treat this restoration as
a completed encounter-balance pass.

## Hoenn land outlier and Mirage Island correction — September 22, 2026

Scoped all 114 active Hoenn land tables for level outliers, then compared the
flagged maps with pre-rebase authored and original Inclement rosters. Desert
Underpass had a level-19 Maschiff among level-38–45 residents. It was added
to preserve a Maschiff source during the rebase, but its level did not match
the late fossil tunnel. Raised that slot to level 40 without changing its 4%
chance or any other species. Other flagged high-level residents are retained;
the scan alone does not establish that every one is balanced.

Route 130's Mirage Island land table had an 18% Natu slot. Original Inclement
uses Wynaut in every slot, the area-map code hides Wynaut's secret island
habitat, and Natu already has an ordinary Route 102 source. Restored the first
slot to level-30 Wynaut. The structural verifier now requires Mirage Island's
land roster to remain Wynaut-only. This preserves the island's identity and
avoids advertising a misleading Route 130 Natu area-map source.

This is a two-map correction and outlier scan, not acceptance of all land
rosters, map access, or full-game encounter balance.
The 289-table verifier and its Natu-reinsertion counterexample pass. Release
build, release gates, input stamp and patch whitespace checks pass for ROM SHA-256
`7249bd05d8a97e94d2a0b03ec2d58a3ae78c4eaa085d5d40d3656847cbd1c15c`;
logs are in `work/wild-land-20260922/`. This is build/structural evidence, not
a native encounter traversal or a full-game release signoff.

## Former Circuit reward sources — September 21, 2026

Traced the12 retained Circuit rewards beyond ordinary encounter JSON. Celesteela
has a NATIVE_WILD_SIGN on Route120; Xurkitree has one in NewMauville_Inside.
ChooseRareWildLegendarySpecies consumes those definitions on land encounters,
subject to acquisition state, independently of ordinary wild tables.

The other10 have only unmapped Circuit/mastery definitions (mapId0xFFFF), no
ordinary-table entry and no mapped event grant: Calyrex, Glastrier, Necrozma,
Spectrier, Zacian, Zamazenta, Zarude, Koraidon, Miraidon and Eternatus. Circuit has
no current map entry point. Do not count these10 as obtainable. Arceus's separate
Devon gift checks eight badges/weather resolution, not Circuit completion.

Removed two Center lead rows directing players to unavailable Circuit rewards.
No species placements, acquisition flags or rates changed. Asked user whether
to plan postgame discoveries in existing Inclement areas or retain temporary
unavailability; replacement design remains undecided. Existing production Circuit
reward implementation is retained, not re-enabled by this cleanup.

## Unreachable rock tables removed — September 21, 2026

Completed the five outstanding interaction checks: DewfordTown, LilycoveCity,
SandstrewnRuins_2F, SandstrewnRuins_3F and SandstrewnRuins_B1F have no Cut/Smash
actors or custom RockSmashWildEncounter path. Original Inclement has the same
actor layouts; these were dormant imported tables, not missing placed rocks.
Removed those five rock_smash_mons fields so Pokedex MapHasSpecies and route
queries no longer report these nonexistent sources. No map objects changed.
Binacle/Wimpod/Dwebble remain on Route106's real breakable rocks;
Baltoy/Onix/Steelix remain on SandstrewnRuins' real breakable rocks, with
additional ordinary sources for most of them. All 633 ordinary-table species
remain represented. Structural table count is now 289 (formerly 294).
The remaining 22 rock tables have corresponding Cut, Smash or Route109 sand
interactions; this is an interaction-source check, not full geometric traversal.

## Cut habitat integration — September 21, 2026

Confirmed against original Inclement that Cut trees consume the map's
rock_smash_mons table. The integrated Cut script had lost this encounter branch,
making ten maps' tree habitats inaccessible through their intended interaction.
Restored the original one-in-three attempt, active-Repel suppression, encounter
result check and battle wait after tree removal. No random item drops restored;
license/badge checks and follower choreography remain unchanged. Maps with Cut
actors and these tables: PetalburgWoods, PetalburgWoods_2, PetalburgWoods_3,
Route103, Route116, Route117, Route118, Route120, Route121 and Route123.
The one-in-three roll is an attempt, followed by the existing encounter-rate
check; it is not a one-in-three guaranteed battle.

All 27 Hoenn rock tables compared with Inclement. Route109 uses custom sand
mounds, not the Rock Smash license; five other tables still need interaction
access review (DewfordTown, LilycoveCity, SandstrewnRuins_2F/_3F/_B1F).
No roster changes made. Distribution verifier and release gates pass; compiled
ROM rebuilt. Evidence: work/cut-habitats-20260921/release-{build,gates}.log.
This verifies source integration/build, not a rendered Cut-to-battle transition
or complete habitat reachability.

## Post-League expedition handoff — September 21, 2026

Traced Hall of Fame invitation and clear setup, Wally exit actor flag and
TRAINER_WALLY_VR_2, all five voyage trainer IDs, Steven stage restriction and
retryable ticket delivery, Birth Island outcomes and Buffel gate. Hall of Fame
setup reveals exit Wally and harbors; invitation is first-clear only. Stage derives
from earned trainer flags; prior earned ship wins count rather than requiring
reauthoring/replaying teams. Aurora receipt/ship-enable flags occur only after
successful giveitem; defeated Steven retries delivery until received. Capture or
defeat resolves Deoxys; fleeing does not. League clear copies legacy defeat into
the permanent finale flag before resetting the legendary for another attempt.

Eight current-source native finale groups pass, including ticket menu state
combinations, voyage transitions, puzzle counter bounds, Deoxys outcomes/re-entry,
and Buffel gating. Evidence: work/finale-access-20260921/{build,tests}.log.
No gameplay changes in this pass. Tests establish progression/state behavior,
not finale tactical difficulty, native visual choreography or full traversal.

## Waterfall/League admission verified — September 21, 2026

Sootopolis state5 conversations set separate met-Maxie/met-Archie flags; either
conversation order reaches their shared departure handler after both. Departure
sets FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE, enabling Wallace's license handoff.
HM07 aliases WATERFALL. Receipt is a flag, not a Bag item; repeat dialogue selects
the Gym branch. Text accurately distinguishes license receipt from Rain Badge8
use. FieldMove Waterfall selects Badge8 for Hoenn. League first admission checks
all eight flags; already-admitted/repeat-clear paths remain separate.

Native current-source run:42 groups pass from campaign_gates.c and
inclement_integration.c, including all256 League badge masks and all eight HM
badge/license combinations without compatible-party/move requirements. Existing
tests were inspected before running; they establish flag logic, not choreography
or every map's geometric reachability. No production changes in this pass.
Evidence: work/campaign-access-20260921/{build,tests}.log.

## Surf/Dive story handoffs traced — September 21, 2026

Surf: Petalburg Gym victory sets Badge5, concludes the Wally-dad choreography,
sets city state4, and warps outside. City's state4 frame callback walks to Wally's
house; house state4 frame callback sets FLAG_RECEIVED_HM03 (alias of SURF license)
and advances city state5. Repeat visits do not retrigger the frame scene. Grant
uses setflag, so Bag capacity cannot block it. Central access requires both Badge5
and license. Dialogue explicitly says press A at water with any team/no learned
Surf. This is script/control-flow evidence, not a movement screenshot.

Dive: Space Center victory sets Steven-house state1 and reveals Steven there.
House state1 frame callback sets HM08 (alias of DIVE license), removes the
Seafloor entrance guard via its actual object flag, and advances house state2.
Central access additionally requires Badge7. No item capacity dependency; repeat
entry does not regrant/replay. Dialogue says press A over dark water with Mind
Badge/no special Pokemon, and distinguishes places where surfacing is impossible.
No defect found in these handoffs; no gameplay files modified in this pass.

## Wild-double failure handling repaired — September 21, 2026

Resolved the previously recorded unchecked second-generation failure. Consolidated
land/Surf/Rock Smash battle startup into StartGeneratedWildBattle and shared
TryGenerateSecondWildMon. Failed second generation starts a single battle without
copying the first Pokemon into slot1. Successful generation retains existing
new-first/original-second order. TryGenerateWildMon failure exits occur before
CreateWildMon, so the saved first remains untouched on failure.

Native regression invokes production generation with a guaranteed Repel rejection
for each of the three areas; checks byte-identical original and empty second slot,
then successful generation and byte-identical original in slot1. All31 focused
native groups pass: work/wild-second-20260921/tests.log. Wild-double configuration
remains disabled; authored trainer doubles/AI untouched. This closes the earlier
compatibility defect, not an assertion that it affected the active campaign.

## Honey/Rock Smash caller audit — September 21, 2026

Honey is sold and used, but ItemUseOnFieldCB_Honey -> Task_UseHoneyOnField ->
StartSweetScentFieldEffect -> SweetScentWildEncounter chooses ordinary land/Surf.
No native encounter path selects honeyMonsInfo; its only C reference is the
route-sign info accessor.29 imported Honey tables are dormant authoring data,
not29 playable habitats. Verifier now explicitly labels their structural-only
coverage; ordinary-source reports already exclude them. Do not add their species
to obtainable-source claims or silently activate a new distribution mechanic.

RockSmashWildEncounter selects the map rockSmashMonsInfo and applies encounter
rate, then ordinary generation/Repel/Keen Eye. Spray prevents only step encounters;
standard Repel level checks still apply to Rock Smash. No behavior changed.

Potential compatibility defect: land/Surf/Rock Smash double branches ignore
second TryGenerateWildMon failure, which can leave duplicate first-mon state.
Not reachable through current wild-double configuration: chance0, force flag0,
followers disabled and partner-wild flag0. Keep distinct from authored trainer
doubles, which are unaffected. Repair with a targeted forced-path test before
future wild-double enablement; not claimed fixed in this pass.

## Habitat access evidence tooling — September 21, 2026

Corrected mega_stone_timing.py's stale compatible-party requirement: Surf,
Dive and Rock Smash require their player license and badges5/7/3 respectively,
not a learned move or compatible party species. Verified against current
HasBadgeForFieldMove and FieldMove_GetUserSlot. Rod gift references still resolve
to Littleroot,114 and Mossdeep House3.

Executing the report exposed a KeyError on shared-event Contest Hall maps.
Follow shared_events_map with cycle rejection and reuse loaded map definitions;
do not skip inherited objects. Complete report now runs. Evidence:
work/wild-access-20260921/mega-source-evidence.txt. Candidate evidence remains
explicitly NOT physical access or usable Mega timing. No game/ROM changes.

## Cave/late roster comparison and alias odds — September 21, 2026

Compared Inclement/current resident sets across Granite/Meteor/Pyre/Seafloor/
Shoal/Sky Pillar/Victory Road, plus Altering/Artisan caves, Ashen Woods, Origin,
Desert Underpass, Dewford Manor/Meadow, Ember, Magma Hideout, Mirage Tower,
Safari, Sandstrewn, Scorched Slab, Seaspray and Verdanturf Meadow. Inclement
regional identities largely remain with newer additions. No roster edits in this
pass. This is species-set inspection, not a timing/evolution readiness signoff.

Found validator alias aggregation mismatch: Dewford Meadow Flabebe/Flabebe Red,
Pyre4F Mimikyu/Disguised, and120 Pumpkaboo/Average are identical native species
but were counted separately for probability policy. Resolve aliases before
summing odds, matching native species identity. In-memory counterexample proves
two2% aliases pass as one4% resident while two distinct2% residents fail absent
an explicit rarity exception. All294 table checks and diff whitespace checks pass.
No engine, encounter data or ROM bytes changed in this verifier-only correction.
Release input stamp must be refreshed with the next coherent release build.

## Midgame land identities — September 21, 2026

Compared current/Inclement/pre-rebase land residents on111,112,Fiery Path,113,
114,115,Jagged Pass,117,118,119,120,121,123 and New Mauville Inside. Retained these
14 rosters: desert, volcanic, ashfall, forest and power-station identities largely
survive, with later-generation additions. This is resident-level comparison, not
proof of every area's timing or evolution accessibility. Authored paradox entries
are not blindly copied back into ordinary rosters; special acquisition remains
separate. Spiritomb has multiple cave/ruin sources and is not globally rare.

Route116 Dreepy is now a deliberate2% early discovery (formerly4%); the released
2 percentage points return to common Skiddo. All species/levels retained. Its
Route119 slot remains5%, a more plentiful later habitat. Native local-land selector
coverage added alongside existing custom Surf/fishing checks.

## Early land comparison — September 21, 2026

Compared current, Inclement, and pre-rebase land residents for101–104, Woods,
116, Rusturf,106, Granite1F and110. Restored authored Mienfoo access on101 in
one duplicate Bonsly slot (4%); Bonsly retains9%, Lillipup8%, every existing101
resident remains. Route113 Mienfoo remains available. No other land roster edits
in this batch. Retained Inclement early Zorua, Phantump, Riolu, Noibat and Axew.
Ferroseed's authored Woods access versus its current Meteor Falls habitat still
needs a deliberate choice; do not count this limited comparison as land completion.
Independent opening Mienfoo verifier contract added.

## Deliberate rarity and tile-exclusive Feebas — September 20, 2026

Latest approved policy supersedes universal 4%: keep convenient defaults, with
explicit rare residents at2%; reserve1% for exceptional choices, not arbitrary
tail slots. The user specifically chose vanilla-style Feebas: only the seeded
Route119 fishing spots, 50% per successful fishing encounter there, any rod.
No ordinary1% Feebas and no tile-mechanic removal. Dewford trend/clock behavior
unchanged; daily ranking changes can change the leading seed, not unconditional
daily rerandomization. Removed the inaccurate route-sign "under bridge" claim.
The Pokedex already has an explicit Route119 Feebas habitat entry.

Optional per-table encounter_rates compile to cumulative bounds; default tables
retain existing method weights. Native selectors, Sweet Scent, local-species
helpers, Match Call and DexNav all receive the owning table. Fishing now uses the
same bounded selector instead of a separate probability ladder. Lure RNG order
is preserved. Lead ability and Sweet Scent/Lure discovery boosts remain deliberate.

Initial2% exceptions: Lapras Surf108, Dratini Surf111, and Milotic Super Rod in
Route119, Meteor Falls1F2R/B1F2R, Sootopolis and Victory RoadB2F. Lapras and Dratini
retain more plentiful principal habitats elsewhere; rarity here is geographical.
Feebas removed from ordinary active tables (including underwater126), with habitat-
appropriate replacements. Ordinary species count633 excludes Feebas; it remains
available through its original special fishing mechanic. No1% ordinary resident
is currently designated after the user's explicit Feebas decision.

Verification:29 native groups pass, including512-seed checks of all rods and
local Surf overrides with/without Lure, exact custom-weight Sweet Scent reversal,
and existing legendary, cap and habitat checks. Structural verifier uses local
weights, rejects unapproved sub4% residents, requires explicit rarity odds and
rejects any ordinary Feebas. Evidence: work/wild-rarity-20260920.

## Water reconciliation complete — September 20, 2026

All 58 Surf/rod areas now compared with both pre-rebase authored habitats
(13061e9ba2^) and original Inclement (cf41a95b68a39ca74fefeb934c460f6f47eb0b3b).
This final batch covers the remaining 35 eastern-sea and interior areas, including
all four Meteor Falls water floors, both Shoal water rooms, three Seafloor pools,
three Safari pools, both shipwreck pools, Woods3, Sandstrewn, Scorched Slab,
Seaspray, Altering Cave B1F and Victory Road B2F. Descriptions now explain the
actual chosen habitats, without inherited slot-count or outdated land promises.

Preserved distinctive Inclement identities: Alolan sludge Safari northwest,
Relicanth-dominated Sandstrewn pool, Eelektrik in Seaspray, dragon fishing in
Meteor Falls, the Poli/Slowpoke woodland pond, cold Shoal and Victory Road pools,
and Magikarp/Gyarados-dominated Sootopolis. Restored authored Finizen offshore,
Seel on134, shellfish in Seafloor, Sealeo/Avalugg in Shoal, and Golisopod in the
wreck. No new maps, pickups, access flags or battle/AI changes.

Final source checks: 294 tables valid; 634 canonical ordinary Hoenn species.
No last ordinary species source lost in this batch. Every species in the old
authored water sheet has at least one current ordinary Hoenn table source (not
necessarily the same method). Each map's Old Rod habitat plus selected Surf
signature residents now has a verifier contract independent of mirror equality.
Four injected interior-habitat regressions rejected; generator is idempotent.

Native run: 27 groups pass, including compiled-table identities in six distinct
habitats, Lure/RNG selection, caps, and legendary/Sweet Scent behavior. Updated
Sweet Scent fixtures to approved 40/30/20/10 water and 4%-minimum land weights;
preserved explicit duplicate-species, equal-rarity tie and legendary-slot checks.
Production Sweet Scent logic unchanged. Evidence: work/wild-eastern-20260920.

Remaining distribution scope: full land/Rock Smash/Honey roster design, method-
and map-access proof across story states, and special/legendary/Ultra Beast
acquisition coherence. This completes water roster reconciliation, not full
wild-distribution balance acceptance or campaign traversal.

## Midgame habitat reconciliation — September 20, 2026

Same three-way comparison: live tables, 13061e9ba2^ authored sheet, and Inclement
cf41a95b68a39ca74fefeb934c460f6f47eb0b3b. Reviewed 14 more water areas:
105, 107, 108, 111, 114, 115, 117, 118, 119, 120, 121, 123, 122, Lilycove.
Total now 23/58; 35 water areas and the full land design pass remain.

Preserved Inclement coastal Clauncher/Skrelp on105, Finneon/Remoraid on108,
Barboach/Goldeen on111, Wishiwashi/Barboach on114, Wishiwashi/Wailmer on115,
Tentacool/Carvanha on118, eastern Gastrodon/Floatzel on119, Stunfisk on120,
coastal Wailmer/Alomomola on121, and Finneon/Staryu/Bruxish at Lilycove.
Restored authored fast swimmers107, wreck Lapras/Dhelmise108, Paldean Wooper/
Tympole117, freshwater variety in the inland rivers, and Ghost-water identity122.
Old Rod levels match Inclement in these areas; other method levels unchanged.
No ordinary Hoenn species lost its last table source; eastern Gastrodon restored.
Potential canonical ordinary species count rises from633 to634.

Timing basis remains actual method access: opening Old Rod, Good Rod from114,
Surf after Norman, Super Rod at Mossdeep. Merely riding Briney's boat past105/
107/108 does not grant fishing or Surf access to those maps. No gates changed.
Expanded independent Old Rod/signature Surf contracts protect these decisions.
Evidence: work/wild-midgame-20260920. This is a source/design reconciliation,
not a full traversal or balance-playtest claim.

## Opening habitat reconciliation — September 20, 2026

Approved direction: compare authored pre-rebase habitats against Inclement for
EVERY area; keep the better fit for current story access, not a wholesale revert.
Baseline inspected: ../inclement-game-source at cf41a95b68a39ca74fefeb934c460f6f47eb0b3b;
authored reference: 13061e9ba2^.

Reviewed water/rod tables: Route 102, Petalburg, Route 103, Route 104, Dewford,
Route 106, Route 109, Slateport, Route 110 (9 of 58 water areas).
Inclement Corphish/Goldeen retained on Route 102, Horsea/Frillish at Slateport,
and Clauncher on Route 106. Authored freshwater Petalburg, coastal Wiglett/Skrelp,
Dewford Mantyke plus Route 106 Remoraid, and Route 110 Tadbulb/Chinchou restored.
Surf keeps four deliberate residents per area. Updated descriptions remove stale
land-slot/percentage claims. Old Rod levels restored from Inclement for these
areas; other method levels and the runtime current-cap ceiling remain unchanged.

Global ordinary land weights: 18,18,10,10,9,9,5,5,4,4,4,4. Surf: 40,30,20,10.
Rock Smash and rod weights unchanged; special/legendary overlays unchanged.
The 4% validator floor is restored. Independent early-fishing and coastal Wiglett
contracts reject the observed Petalburg/Route106/Route104 regressions even if the
executable JSON and mirror sheet agree. Three injected regression fixtures rejected.
Structural check: 294 tables, 633 potential ordinary canonical species. Native
focused run: 27 groups passed, including 512-seed slot/RNG parity for each
land/Surf/Rock Smash method with and without Lure, plus wild cap/lifecycle cases.
Evidence: work/wild-reconciliation-20260920/{build,tests}.log.

Remaining 49 water areas still need the same Inclement/authored/current comparison;
land species rosters have NOT received a full design review. This is not a
completed distribution or reachability/balance certification.

Partial source pass: ordinary land/water slot selection, Sweet Scent species
weighting, Lure slot reversal, level selection, fishing/Feebas creation paths,
ability selection and Repel callers. Structural scan covers 7,001 authored slots
across main/Pyramid/Pike groups: all min/max levels are within 1..100 and ordered.
This does not certify accessibility, rarity, species availability or game balance.

Consolidated duplicated land/water probability ladders into one bounded selector
and shared the existing threshold arrays with Sweet Scent. Wildlife-report helper
entrypoints remain for callers, but no longer carry separate probability ladders.
The change removes 62 net lines and preserves RNG draw count/order, including
Lure reversal. Native test compares 512 seeds per land/water/Lure combination
against the authored weights, checking both selected slots and resulting RNG
state (2,048 cases). Passed; source tables/species/rates were not modified.

Evidence: work/wild-audit-20260919/tests.log, early-route-tables.json, and release
build/gate logs. Early-route export covers 101–104 and Petalburg Woods methods,
including later-access fishing/water tables; do not mistake table presence for
availability at first arrival. Remaining: gate-linked distribution review across
all areas, legendary/Ultra Beast acquisition, time-of-day fallback and static
encounters, battle interactions, and native capture/encounter edge behavior.

## Guaranteed Shoal sighting

Confirmed and repaired selection-time consumption: the chooser cleared the
charted-sighting flag before Repel/lead-ability rejection. It now only selects;
TryGenerateWildMon records whether the guarantee was chosen and spends it after
CreateWildMon. The sole production caller is covered; ordinary successful encounters
in the room do not consume the guarantee. Native before/after evidence checks a
Repel-rejected attempt followed by an actual guaranteed resident creation.
Logs: work/shoal-sighting-20260919/.

Ultra Beast source inventory finds no ordinary-table slots and ten sign-defined
acquisition sources; Naganadel must be checked through Poipole's evolution path.
See ultra-beast-sources.json in the same work directory. This is not proof that
the locations, gates, rates, teams or progression timing are all correct; that
map-linked review remains required. No species/rate/AI changes were made here.

Ultra Beast native source/rate/capture/evolution checks are now recorded in ULTRA_BEAST_ACCESS_AUDIT.md. Both native groups pass; earliest-access and balance review remain incomplete.

## Shared creation cap and outbreak identity — September 20

Ordinary slot generation and ordinary fishing capped levels, but CreateWildMon
itself did not. Feebas/outbreak/direct callers could bypass that contract; DexNav
bonus calculation also only capped at100. Native regressions reproduced level50
at campaign cap14 and an outbreak passing Repel based on raw80 rather than its
intended capped14. CreateWildMon now clamps before stats/natural moves are made.
Removed the redundant regular-fishing clamp; ordinary encounter's pre-Repel cap
remains necessary. Outbreak computes its capped level before Repel and creation.
DexNav preview/moves use the same ceiling while preserving missing-species sentinel.
Removed three duplicate includes in wild_encounter.c.

Scope qualification: DexNav and overworld wild spawning are currently disabled.
Those edits are compatibility work, not evidence of active player-facing DexNav
bugs. Current post-Champion roaming Lati@s are40 at cap100 and need no adjustment.
Static outbreak sources are ordinary species; no wild tables, species placements,
rates or story gates changed. Exact earliest-access distribution remains unreviewed.

Central clamping exposed an overworld outbreak seam: collision asserted raw saved
level equals displayed level, then rebuilt the generated mon, rerolling its
identity. Preserve the existing visible mon and apply only the outbreak moves
through shared ApplyMassOutbreakMoves. Its stored level may be lower than the raw
outbreak level because of the cap at spawn; species and upper-level consistency
remain checked. This disabled-feature dispatcher was source-traced, not replayed.

Seven native groups pass: creation cap across prebadge/Badge1/Champion and low
levels, capped outbreak Repel with move retention, identity-preserving move
application (personality/OT/species/level/HP/shiny/RNG), prior slot-odds group,
Shoal sighting and two Ultra Beast groups. Baseline failures and final logs are
work/wild-audit-20260919/level-cap-{baseline,final}-*.log. Strict release evidence:
level-cap-release-build.log and level-cap-release-gates.log in that directory.
No full-world distribution or visual acceptance claim is made.

## Route roster bounds and local-species query contract

Native roster pass checks every compiled encounter header with uncaught and
caught legend bits:294 checks, longest generated text633bytes in gStringVar4's
1000-byte buffer. Native legend counts fit the per-method array, every generated
string terminates within the buffer, and generation preserves RNG. This covers
the compiled147 header entries, not every donor map in the JSON source tree or
all Altering Cave variant states. No buffer repair was needed for current data;
it is not a complete rendered-dialogue or progression-availability audit.

GetLocalWildMon now accepts NULL for its optional habitat output using a local
fallback. The two follower-transform callers are explicitNULL; enabled ambient
cries still pass their real output variable. The unconditional NULL write was a
source defect, but follower copying is disabled and no active crash was observed.
Native parity checks compare nullable/nonnullable queries across land, mixed,
water and missing-header maps for512seeds, including identical RNG state.

DoesCurrentMapHaveFishingMons rejects HEADER_NONE before asking for a timed
encounter table and returns table availability directly. The former order was
safe only while time-based tables were disabled; that remains the configuration.
Missing-map query behavior is covered by the native parity group.

Nine focused native groups pass together, preserving prior cap, outbreak identity,
slot odds, Shoal and Ultra Beast checks. Evidence: work/wild-audit-20260919/
roster-baseline-tests.log, roster-final-tests.log, roster-release-build.log and
roster-release-gates.log. No species, encounter rate, placement, unlock or AI
changes were made in this pass. Campaign-access distribution remains unfinished.

## Encounter selector consolidation

Land, water and Rock Smash now share one probability selector plus one Lure
reversal helper. Rock Smash retains its own generated cumulative thresholds even
though its current weights equal water's, so the authored tables remain separate.
The optional Lure draw still follows the ordinary slot draw, and reversal derives
from the selected table's size rather than duplicated11/3 constants. Match Call
wrappers retain ordinary odds without a Lure draw. Fishing remains unchanged;
its current two/three/five rod groups agree with the roster slices and data.

Removed the unreferenced ChooseWildMonIndex_Honey function after repository-wide
reference search and tracing ItemUseOutOfBattle_Honey -> Task_UseHoneyOnField ->
StartSweetScentFieldEffect. No public declaration or caller used that selector;
this does not remove the Honey item or change its actual Sweet Scent behavior.
Net49 source lines removed in this pass, with no new files/API surface.

Expanded native odds parity to three methods × Lure on/off ×512 seeds=3072cases.
It passed before and after refactoring, including exact resulting RNG state.
All nine wild groups pass together (lure-final-tests.log). Source/rate/placement
and story data unchanged. Evidence under work/wild-audit-20260919/:
lure-baseline-tests.log, lure-final-tests.log, lure-release-build.log and
lure-release-gates.log. Species accessibility/balance review remains unfinished.

## Distribution evidence scope and obsolete verifier removal

Retired scripts/verify_wild_open_sources.py after checking release/workflow/script
callers (none). It counted donor-region and unused Honey references as open
sources, used hardcoded sealed-map prefixes instead of current progression
analysis, and its zero-open-source reporting was unreachable by construction.
Its690-source PASS was not evidence of campaign availability. Archived a local
copy under work/wild-audit-20260919/retired-verify-wild-open-sources.py and removed
its obsolete inventory entry. Required release gates remain intact.

The existing verify_wild_distribution.py now accepts --source-report PATH.
It reports potential ordinary land/water/rock/fishing references on Hoenn maps,
canonicalizes species aliases, and explicitly excludes claims about map access,
method unlocks, gifts/evolution and rare/scripted overlays. Current147Hoenn header
entries reference633canonical species (642raw symbols) across122land/58water/
27rock/58fishing tables. Zero-rate tables would be retained for separate deliberate-
encounter review; none occur in this current ordinary set. This report is a
successful-run snapshot; callers must respect validator failure status rather
than treating an older output file as a fresh result.

The full structural validator still checks294Hoenn data tables, including unused
Honey data; its output now says data tables rather than claiming all are active
methods. All existing species/level/probability/route-sheet checks remain. Missing
species validation previously appended an error then crashed in rate aggregation;
invalid-mon tables now skip that dependent aggregation and fail with diagnostics.

Independent report equality and three in-memory negative cases (missing species,
invalid level, weights totaling99) pass; no repository data was changed to run
them. Evidence: ordinary-source-inventory.json, ordinary-source-report.json and
source-validator-checks.log in work/wild-audit-20260919/. Strict release evidence:
source-report-release-build.log and source-report-release-gates.log. No native
replay needed for tooling-only changes. Actual progression/distribution audit is
still required; no species absence/obtainability conclusion follows from counts.

## Scripted creation consolidation and stored-name initialization

CreateScriptedWildMon and CreateScriptedDoubleWildMon now share one private
initializer for synchronized personality, random IVs, initial moves and held
items. Each public entry still clears both opposing parties once; double creation
retains first/second ordering. Three duplicated initialization blocks become one
(net23 lines removed from script_pokemon_util.c); public APIs and callers remain.

A native byte-level parity check exposed uninitialized nickname storage in
CreateBoxMon: StringCopy stops at EOS, but the mon-data setter consumes the full
fixed-width nickname. Short names therefore copied stack bytes past EOS into the
stored Pokemon. Initialize that temporary buffer to EOS before copying. Equivalent
confirmed local buffers were fixed in both Battle Tower constructors, egg hatch,
both Pyramid configurations, record-mixed partner OT names and debug PC filler.
This does not change visible names, save layouts, teams, encounter rates or AI.

Final native tests:64 seeded cases across four species/items and absent/Abra leads
compare complete mon bytes and final RNG state for two singles versus one double;
assert levels/items/names, EOS padding and clearing of stale opponent slots. Both
Battle Tower creation entrypoints preserve ACE and initialize its remaining name
field. All15 combined wild/legendary pipeline groups pass. An early fixture layout
passed once after moving assertions, then failed again after the refactor; neither
was accepted as proof until the uninitialized name tail was fixed. Final tests use
memcpy snapshots and explicit tail assertions. Related hatch/Pyramid/partner/debug
paths were source-reviewed and built, not individually played.
Evidence: work/ability-audit-20260920/scripted-wild-*. Full distribution/access and
other core files remain incomplete. ScriptSetMonMoveSlot's legacy fallback for an
empty party and slot validation remains a candidate for a subsequent caller audit.

Strict release build/gates pass: source/artifact stamp bd59dc716124.

## Verdanturf Meadow distribution pass

Inspected map connection, objects, ordinary encounter table, rare overlay and
Audino gift source. The existing TM21 pickup is Skarmorite at its original object
location; two existing hidden Nectar pickups remain. The meadow connects north
to Verdanturf with no scripted warp or coordinate gate; collision-level access
was not independently replayed. Land slots are Cottonee/Espurr19each, Munna/
Vivillon10each, Stufful/EternalFloette9each, Hatenna/BlueFlabebe5each, WhiteFlabebe/
Indeedee4each, Milcery/GalarPonyta3each before rare/lead-ability selection effects.
Raw41..43 levels pass through the already-tested live-cap clamp before creation.
Hidden/Honey tables are not counted as enabled acquisition routes: DexNav is off
and Honey uses Sweet Scent instead of the unused Honey table.

Native exhaustive percentile tests confirm Enamorus and Fezandipiti3percent each
with no badge requirement; capture removes only the caught species and preserves
the other3percent. SweetScent uses25percent shared across eligible residents,
then falls back to ordinary selection; after both captures no rare overlay remains.
Water/RockSmash calls do not leak these land residents. All16 combined wild and
legendary pipeline test groups pass: work/ability-audit-20260920/
meadow-distribution-build.log and meadow-distribution-tests.log.

Audino gift code source-traced: levels use strongest non-Egg party member then
GiveScriptedMonToPlayer clamps to current species cap; LuckyEgg remains attached;
party/PC naming and no-capacity retry branches exist, and receipt flag is reached
only after delivery. No gift runtime or visual replay in this pass. No production
encounter/gift changes were justified; this adds scoped native distribution
coverage, not whole-game balance/reachability or all-species availability proof.

## Route113 soot and Marshadow integration repair

The active glass workshop was restored to spending VAR_ASH_GATHER_COUNT, while
ash steps only incremented VAR_EC_SOOT_PROGRESS. Its shop could never earn new
spendable ash. Ash collection now updates both counters independently, only with
a Soot Sack, saturating each at9999 and preserving the lifetime receipt high bit.
Spending at the existing flute/decor shop affects only the original balance.

The active glassmaker had no discovery caller despite the guide promising
Marshadow at250 lifetime soot. Reused TryUnlockLocalLegendaryDiscovery, adding
Marshadow's glass-workshop map and threshold, and wired it into that NPC. A new
one-time message directs the player to Route113 ash grass. No LinkingCord, Mega
Stone or other new item reward was added; the dormant old milestone helper was
not activated. Its obsolete item tiers remain a separate cleanup candidate.
Also check Soot Sack receipt before setting workshop state1, preserving retry on
failed Bag insertion (a robustness case; ordinary early-game Key Items capacity
is ample). Existing paid-item pending states10..16 are unchanged.

Native tests cover no Sack,250 collected, spending/recollection, both saturation
limits/high-bit preservation, wrong-map and249-soot rejection,250 unlock without
spending, repeat conversation/capture removal, and the actual Route113 rare
selector at all100 percentile rolls (Marshadow1percent after discovery). These
exercise the production collection helper through a TESTING-only wrapper, not
map walking/ash sprite replacement. Source confirms the per-step callback invokes
it only on a newly entered ash tile. Full workshop UI/visual traversal and all
species availability remain outside this scoped pass.
Evidence work/ability-audit-20260920/soot-integration-*.

All18 combined native encounter groups and strict release gates pass;
release stamp 54aeec4340da.

## Outbreak TV index bounds repair

PrepareTvShowForRandomOutbreak used RandomUniform(0, ARRAY_COUNT(table)) despite
inclusive upper-bound semantics, selecting index5 past a five-entry authored table.
Corrected to count-1. Native real-RNG baseline reproduced outbreakIndex>5; corrected
512draw regression covers every valid outbreak and checks species/index consistency.
Function-test RNG_NONE defaults were masking the selection: the final test copies
the runner locally, disables its forced uniform callback only around the production
TV call, and restores it before assertions. Earlier seen-mask failures came from
the harness forcing0 and are not the out-of-bounds reproduction.

StartStaticMassOutbreak now rejects invalid indices before any read/write, protecting
saved TV records, and absorbs its single-use by-value writer to remove that extra
function/copy. Native invalid indices5/255/65535 leave all active outbreak fields
unchanged; expiry still works. All10 wild-slot groups pass, including prior ordinary
slot/RNG parity, capped Repel, scripted generation and outbreak moves/identity tests.
Read-only analogous scan found no other production count-as-inclusive-bound defect.
Evidence: work/ability-audit-20260920/outbreak-index-*. Existing table species, moves,
locations, probabilities and durations unchanged. This does not prove all campaign
species availability, reachability or distribution balance. Full game audit remains
unfinished, including saved-data validation beyond this index boundary.

Strict release build/gates pass, stampb1ce40c9c9f8.

## Roamer lifecycle and movement cleanup

Read roamer.c and traced active post-League TV initialization, encounter creation,
Repel handling and battle_main return/capture retirement. No new production defect
was established. Current roamers retain their authored level40 (post-League cap
permits it), identity and moves. Native tests cover Latias/Latios, single-slot
capacity, IV/personality/shiny preservation, half-HP and paralysis/frostbite through
reencounter, 64other-set moves each, defeated-roamer full-HP/status-clear retry and
explicit deactivation. Battle-end capture-vs-defeat dispatch is source-traced,
not a replayed capture battle.

Simplified the redundant early-return branch in other-set movement, history zeroing
and location predicate. Random draws, rejection condition and route table unchanged.
All11 focused wild groups pass. Structural check confirms all20roaming sets have
>=3distinct maps, more than one starting map, every destination has a starting row,
and each route has a land/water encounter table. This checks documented loop
preconditions; it does not prove map collision reachability or full campaign balance.
Evidence: work/ability-audit-20260920/roamer-lifecycle-* and roamer-route-table.log.
Full core/campaign/battle/wild objective remains unfinished.

Strict release build/gates pass, stampebd75cbbc2cc.

## AlteringCave habitat-display integration repair

Strict release build/gates pass, stampfda653d679f7.

Native baseline reproduced Noivern missing from the discovered AlteringCave area.
MapHasSpecies used the shared MAPSEC_ALTERING_CAVE label to filter nine original
rotating tables, then incorrectly discarded restored1F/B1F as variants10/11.
Filter now checks the exact original map group/number; new floors retain their
independent ordinary tables. Encounter species/levels/odds are unchanged.

Native full habitat-search tests (TESTING-only allocated screen-state wrapper,
no rendering) cover Noivern1F/BasculegionB1F under all9variant choices plus invalid
choice fallback; original-map Zoroark still appears only for set0/fallback. Hidden
landmark behavior remains. Initial missing-landmark hypothesis was disproved:
Route103 enters the original cave, whose transition already sets the flag before
newfloors (and B1F can branch directly from that original cave). No redundant map
hook added. Other expansion landmark metadata has no current visibility consumers.

Removed optional broken fishing-length branch: always scan the10fishing slots,
not12land slots. Synthetic neighboring Mew/Celebi entries are excluded while valid
Magikarp is found; empty table remains absent. Did not independently establish
whether the old BUGFIX-off branch was active in the shipped build. All13wild groups
pass, including prior odds/caps/outbreak/roamer checks. Evidence under
work/ability-audit-20260920/habitat-cave-*. Full habitat rendering/time-of-day/rare
resident overlays and all-species campaign availability remain unverified.
Full core/campaign/battle/wild objective remains unfinished.


## Rare wild habitat integration

Strict release build/gates pass, stampb8741a8e095f.

Time-of-day audit confirms shipped OW_TIME_OF_DAY_ENCOUNTERS isFALSE and both Dex
and live encounters select default tables; no active fallback mismatch established.
Native baseline instead proved unlockedMarshadow selectable onRoute113 but absent
from the Dex area search. Added only RARE_WILD/NATIVE_WILD sign locations when the
same CanAcquireLegendarySignSpecies predicate used by encounter selection allows
them, with current-region filtering and existing landmark/moving-area rules.
Consolidated regular/special/expansion map routing into AddMapToAreaScreen; original
Feebas routing retained. Fixed/gift/breeding/quest-battle sources remain excluded.
No encounter odds, species, levels, discovery requirements or capture rules changed.

All25 combined wild/sign native groups pass. New checks compare Marshadow runtime
selection with area presence before unlock/after unlock/after capture, reject its
Hoenn habitat while viewing Kanto, and verify Enamorus/Fezandipiti meadow glows
remove only the captured resident. Test wrapper queries actual area construction
without rendering. Related source audit verifies65rare/native definitions across
36maps each has the correct nonzero default habitat table: water on125/126/127,
land elsewhere, including land-routed UnderwaterSeafloor. This is structural/runtime
routing evidence, not proof of physical reachability or every discovery quest.
Evidence work/ability-audit-20260920/rare-habitat-* includes failing baseline.
Full habitat rendering and broader core/campaign/battle/wild objective unfinished.


## All rare/native resident selector coverage

Strict release build/gates pass, stamp018f5ce99f86.

Added native parameterized coverage for every RARE_WILD/NATIVE_WILD definition
(currently65residents): resolve its actual map header, require a nonzero runtime
habitat table, satisfy its declared progression/discovery requirements, enumerate
all100normal percentile rolls and verify authored3percent native/1percent quest
share, reject the other habitat, then mark captured and verify all100rolls exclude
that resident. 13000before/after selector rolls plus65wrong-habitat checks. This
proves configured selector availability once eligible, not physical map reachability
or that every discovery NPC can establish those requirements in a full playthrough.

The initial single-batch fixture reached the harness timeout; split into one
parameter per resident without dropping any checks. Parameterized baseline passed.
Selector now rejects non-wild source classes before expensive family/discovery
queries; those read-only results were previously ignored. Pool ordering, RNG draws,
probabilities and acquisition rules unchanged. All26combined wild/sign groups pass
after this cleanup, including existing ordinary-slot, habitat and lifecycle checks.
Evidence work/ability-audit-20260920/resident-distribution-*.
Full core/campaign/battle/wild objective remains unfinished.
