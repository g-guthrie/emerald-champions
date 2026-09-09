# E0012 — Winston, Route104

Status: **bounded executable-AI review complete, intention8/10**. The sections before "Final checkpoint"
are chronological observations, including rejected trials and now-repaired gaps.

## Actual compiled baseline

TRAINER_WINSTON_1, TRICK_ROOM|SETUP, full native party, Normal live cap14.
All four become level12, despite stored levels14/13/13/14. Native stats below
are HP/Atk/Def/SpA/SpD/Speed before held-item multipliers:

- Porygon73/23/27/67/26/16; Eviolite, Download, Quiet; Tri Attack/Trick Room/
  Thunderbolt/Protect.
- Honedge68/64/34/17/17/13; Life Orb, No Guard, Brave; Iron Head/Sacred Sword/
  Shadow Sneak/Swords Dance.
- Gimmighoul68/15/27/63/25/9; Focus Sash, Rattled, Quiet; Shadow Ball/Power Gem/
  Substitute/Protect.
- Wooloo67/55/23/18/19/18; Leftovers, Fluffy, Brave; Cotton Guard/Double-Edge/
  Thunder Wave/Protect.

All16 moves and4 abilities have native owners. Only Wooloo repeats later in
the compiled party census (Dawson); the other three are unique. No
distribution-driven replacement is justified by this census.

## Initial native observations

Twelve parameters in three temporary native groups pass:6 leads,4 reserves,
2 forced recoil controls. A PASS is execution, not tactical correctness.

Physical lead against bulky Eevee Bite into Honedge and Timburr Drain Punch
into Porygon: turn1 Trick Room/Swords Dance, own47/56HP; Download raises SpA.
Turn2 Tri Attack/Sacred Sword, own21/38, Eevee6/Timburr81, Room timer3.

Special lead against Lotad Giga Drain into Porygon and Charmander Flamethrower
into Honedge: turn1 Trick Room/Shadow Sneak trades Honedge, Porygon58 and
replacement Wooloo67, foes71/54. Download instead raises Attack on this board.
Turn2 Tri Attack/Double-Edge trades Wooloo, Porygon43 and replacement
Gimmighoul68, foes71/5. This loss sequence is not accepted as optimized.

Denial lead against Mienfoo Fake Out then Taunt into Porygon and Pachirisu
Thunderbolt into Honedge: turn1 Room is interrupted while Honedge uses Swords
Dance, own68/47. Turn2 Porygon attempts Room into Taunt; Honedge voluntarily
pivots to Wooloo53. Porygon remains68, foes72/77, no Room. The AI must not read
pending human Taunt to avoid it; evaluate useful follow-through rather than
forcing clairvoyance.

Reserves use the actual party with original leads fainted, no invented Room.
Physical Eevee Bite/Gimmighoul and Timburr Drain Punch/Wooloo: both Protect
once, then Shadow Ball/Cotton Guard; own34/49, foes75/56, WoolooDef+3 and
GimmighoulSpeed+1 from Rattled. Special Natu Psychic/Gimmighoul and Charmander
Flamethrower/Wooloo: Shadow Ball/Protect leaves own25/67, foes23/71; repeated
Protect on turn2 loses Wooloo, Gimmighoul25 survives. Native Fire counterplay
is legitimate, but repeated waiting and reserve selection still need review.

Forced native Double-Edge with actual Wooloo into bulky Pachirisu deals16 and
charges5 recoil. Full67HP ends66 after Leftovers; explicit13HP injury ends12.
The raw AI self-sacrifice helper instead estimates22 from user maximumHP.
This proves the formula is wrong, not that the restricted attack-comparison
helper alone determines Wooloo's full-party choice.

## Executable work identified at the initial checkpoint

- Cotton Guard gets a native setup opinion but no same-turn Defense change in
  the paired forecast. Isolate native survival and unrestricted decisions.
- Damage-based move recoil is absent from paired HP continuation, separately
  from the wrong max-HP raw helper. Preserve native rounding, actual dealt HP,
  Substitute, Disguise/Ice Face, Rock Head/Magic Guard and contact ordering.
- Verify useful Room follow-through/expiry, setup denial, reserve transitions,
  and timing before accepting item/stat/plan changes.
- New Substitute cost/protection and Rattled's speed gain are not represented
  as full paired continuation. Do not claim full support from native effects
  simply occurring after a selected attack.

Source review finds no new Download, No Guard or Fluffy defect. Thunder Wave
forecast reorders speeds with Room considered. Fluffy halves contact damage
but doubles non-contact Fire: Fire Punch and Flamethrower are not equivalent.
Substitute consumes Gimmighoul's full-HP Sash condition, a trade to observe.

The four Winston work/*.inc includes are temporary diagnostics while this
encounter remains unfinished. No additional permanent regression, completion
increment, ROM, commit or publication at this checkpoint.

## Historical item and policy comparisons — bundle rejected

Tested actual compiled Honedge Life Orb→Focus Sash and Gimmighoul Focus
Sash→Eviolite, otherwise unchanged. All12 original parameters execute. Honedge
survives the special lead at1HP, but turn2 Porygon Protects and Honedge still
faints after attacking; Charmander38 remains, Porygon58 and Gimmighoul68.
Physical turn2 also shifts to Porygon Protect: own47/44, Eevee47/Timburr81.
Thus the item trial buys survival but gives up substantial immediate damage;
it is not declared better based on survival alone.

The Eviolite Gimmighoul reserve comparison is more promising. Physical turn1
Shadow Ball/Protect produces46/67HP and Timburr56, instead of both waiting;
turn2 Shadow Ball/Double-Edge gives24/40, foes47/30. Special reserve turn1
Shadow Ball/Protect gives38/67; turn2 Shadow Ball KOs Natu, Gimmighoul8 survives
while Wooloo still faints to Fire. This does not make the entire item bundle
optimal or establish a full endgame win.

Adding PRESSURE to that item trial only changes the sampled special lead
turn2 to Tri Attack instead of Protect: own43/Gimmighoul68, foes53/38. It does
not rescue Honedge or change the sampled physical/denial/reserve choices.
Both trial items and the extra plan flag are restored to their original
compiled values pending the remaining AI review. Master-to-party projection
verification again passes516 branches; this is not encounter completion.

## Native recoil repair and Cotton Guard controls

The raw tied-attack self-sacrifice comparison now derives ordinary single-hit
EFFECT_RECOIL from minimum cached hit damage, clipped to actual target HP or
Substitute HP. It respects native floor-percent/minimum1, effective first-hit
Disguise/Ice Face prevention, Rock Head/Magic Guard, and excludes unsupported
multi-hit/Parental Bond/spread/Max/Z aggregates. Half-max-HP recoil effects are
unchanged. No extra damage calculation or cache allocation is added.

An actual-native-cache query gives minimum16damage, hence5recoil:5HP is lethal
on hit and6HP is not. Restoring only the old22HP estimate makes the safe6HP
assertion fail; the negative mutation is removed. The initial query omitted
opposing reserves and therefore did not exercise the helper's pre-existing
reserve-only penalty gate. Adding a genuine third native player member fixes
that fixture precondition. The helper still does not penalize recoil when no
opposing reserves remain; that inherited heuristic is not an engine rule or
claimed repaired here. This narrow comparison is not paired recoil modeling.

Two forced native Cotton Guard controls use actual Wooloo50HP against native
slower Munchlax Body Slam. Double-Edge deals33, pays10 recoil, then takes24
uncapped Slam damage and ends20 after Leftovers. Cotton Guard instead takes9
uncapped damage and ends45. Native Speed18 versus10 establishes move order;
no artificial Room is supplied. During the item/pressure trial, a separate
unrestricted20HP reserve board chooses Gimmighoul Shadow Ball/Wooloo Protect
and finishes Wooloo24HP. With the original items/plan restored, both choose
Protect and Wooloo again finishes24HP. That
reasonable Protect choice is not evidence that Cotton Guard modeling works.
The production paired forecast still lacks the immediate Defense increase.

Current integrated checkpoint:15 native parameters in5 temporary groups pass
with original Winston items/plan and the raw recoil repair. The19 retained
shared paired-AI regression groups also pass after this repair. Other initial
lead/reserve observations remain unchanged. No Winston completion claim.

## Final checkpoint

Only Gimmighoul's Focus Sash changes to Eviolite. Native evolution eligibility
supports Eviolite; it improves repeated-hit survival and avoids the full-HP Sash
condition conflicting with Substitute. Losing guaranteed single-hit insurance
is a real tradeoff. Honedge retains Life Orb: the Sash bundle weakened sampled
physical pressure without curing the Fire matchup. TRICK_ROOM|SETUP, all species,
moves, abilities, points, natures and levels remain unchanged. Canonical teams,
master and compiled party agree; projection verification passes516 branches,
which is data consistency rather than516 reviewed battles.

Paired AI now forecasts Cotton Guard's timely Defense change using native
stat-change eligibility and Simple/Contrary-adjusted stages. It does not award
physical mitigation before the move executes, or against ordinary special
damage. This adds16 cache bytes without another damage-engine evaluation.
An unbonused, self-contained decision regression chooses Cotton Guard against
slower physical pressure and Double-Edge against faster physical or special
pressure. Disabling only the Cotton cache makes the slow-physical expectation
fail. The separate actual-trainer isolated fixture still selects Cotton Guard
in all three positions: its SETUP preference remains influential. Do not present
that fixture as proof of universally correct setup selection or full future-turn
reasoning. The faster fixture also differs in HP/Defense, not speed alone.

Paired ordinary single-hit damage-based recoil now uses scaled damage clipped
to target/Substitute HP, native rounding and effective first-hit Disguise/Ice
Face prevention, with Rock Head/Magic Guard respected. Contact, move recoil and
Life Orb costs share the landed-hit probability rather than inventing separate
miss/death branches. Intervening contact-triggered held healing is excluded from
this new recoil calculation; multi-hit, Parental Bond, spread, Max/Z aggregates
are not newly modeled. Existing approximations about virtual Substitute/ability
state changes and conditional damage remain. Newly inflicted same-turn Thunder
Wave does not yet discount a later Cotton Guard; pre-existing paralysis does.
This boundary was found in final subagent source review, not exercised by the
Winston scenarios. No extra search pass is added.
The raw tied-attack comparison repair is separately documented above.

Native restricted Wooloo decisions: at5 or6HP, Protect; at7HP, Double-Edge,
dealing16 and paying5 recoil, finishing6 after Leftovers. SixHP is vulnerable
to the predicted damage range, not guaranteed to faint on the sampled roll.
Disabling only paired move recoil makes the5HP Protect assertion fail. Both
negative mutations were restored. Two distinct shared regression groups are
retained (Cotton timing and recoil survival); no permanent Winston suite added.

Final current-team run:29 parameters in10 temporary groups pass. Full-menu
three-turn lead observations retain physical Trick Room/Swords Dance into
attacks: Eevee faints on turn3, own Porygon21/Honedge32, all four alive. Under
Fake Out/Taunt denial, turn3 instead attacks with Porygon and uses Wooloo Cotton
Guard; own44/39, all four alive, no Room. Against special Fire pressure, Honedge
and Wooloo still trade away before turn3 finishes Charmander; own Porygon28/
Gimmighoul68 versus Lotad53. That weakness and reserve choice are not solved by
the item change. These are sampled turns, not full-match wins.

Eviolite reserve follow-through without invented Room: physical turn1 Shadow
Ball/Protect; turn2 Shadow Ball/Thunder Wave into Eevee; turn3 Shadow Ball KOs
Timburr while Double-Edge hits Eevee. Own24/22, Eevee47, Timburr0. Rattled rises
to+2 Speed after native Bites. The sample includes paralysis; it is not a
guaranteed outcome. Special reserves attack Natu twice instead of repeatedly
waiting: Natu faints and Gimmighoul survives8HP, while Wooloo still falls to Fire.
New Substitute and future Rattled speed are not fully modeled continuations.

After a genuine initial native turn, explicit field-state queries select Room
when absent and attacks at active timers2 and1; no sampled self-cancellation.
These queries are not a natural full-duration Winston cycle. Native six-player
versus actual four-opponent timing is40 warm frames and46 cache-rebuild frames
(about0.77 seconds at60Hz). This sampled board is not a worst-case latency bound
or whole-game freeze certification.

All eight temporary Winston includes are removed from the retained test source;
scratch diagnostic files remain unbuilt in work/. All21 shared paired-AI groups
pass after removal; final diff whitespace check passes. No release ROM, commit, publication or full-game
completion is claimed at this checkpoint.

Intention rating:8/10. Slow mixed offense, Normal/Ghost defensive options and
Fluffy/Cotton Guard give this collector a distinct identity without repeating
the same regional species. Better than8 needs more selective reserve deployment
against Fire and tighter setup/remaining-Room-turn decisions; this is a design
judgment, not a measured win rate. Early Mienfoo denial/Fighting, Dark pressure
and Natu Fire coverage preserve accessible counterplay. Munchlax in isolated
mechanic controls is a Route116 reference, not a pre-Winston availability claim.
