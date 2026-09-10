# E0011 — James, Petalburg Woods

Historical flat-stat review. Current completion and evidence:
[final Gen9 practical pass](e0011_gen9_2026_09_09.md).

Status: **COMPLETE — native scenario review and main-chat report.**
TRAINER_JAMES_1 (621). Intention rating: **8.5/10**, not a measured win rate.
Actual compiled party and native stat generation are the authority. This
supersedes the earlier source-only E0011 review and unfinished checkpoints.

## Accepted team and executable plan

RAIN plus PRESSURE. Preserve rain's immediate partner payoff, but allow an
injured setter to pivot while its partner attacks. This is an executable plan
selection, not a forced Rain Dance script. All species, moves, abilities,
natures and level settings are retained from the restart baseline.

Normal difficulty at live cap14 produces level12 for every member. Native
HP/Atk/Def/SpA/SpD/Speed, before held-item defense multipliers:

| Member | Item / ability | Native stats | Purpose |
| --- | --- | --- | --- |
| Surskit | Focus Sash / Swift Swim | 37/13/16/52/21/61 | Rain, Hydro Pump, Struggle Bug, Icy Wind |
| Dewpider | Eviolite / Water Bubble | 66/55/23/16/26/15 | Immediate Liquidation, Lunge, Leech Life, Protect |
| Venipede | Eviolite / Speed Boost | 52/39/22/13/26/44 | Protect, Poison Jab, Skitter Smack, conditional Toxic Spikes |
| Kricketune | Life Orb / Technician | 46/61/20/18/20/61 | Bug Bite, Aerial Ace, Taunt, Protect |

Dewpider changes Sitrus to Eviolite. Native defensive survival now makes attacking
alongside Rain Dance worthwhile on the sampled physical and special openings,
instead of waiting behind Protect. Venipede changes from the physical
Attack/Speed preset to20/20/0/0/8/18 Stat Points. It gains18HP and8SpD while
retaining66Speed after one Speed Boost, above the sampled optimized Mienfoo64.
The trade is lower immediate Attack and unboosted Speed; this is not a universal
upgrade against every foe. Kricketune's native Orb payment is4HP.

Surskit reaches122Speed in rain. Swift Swim does not speed Dewpider. Rain
does not improve Hydro Pump accuracy. Water Bubble's Water boost, Fire reduction
and burn immunity are native mechanics, not invented plan effects.

## Demonstrated AI defects repaired

- Lunge's Attack drop and Skitter Smack's Special Attack drop now contribute
  to later partner survival and attacks. The existing64-byte per-move drop
  matrix handles the appropriate offensive stat; there is no second matrix.
  Native eligibility, turn order, Substitute, Covert Cloak, retaliation,
  White Herb and move-specific Sheer Force guards remain in the calculation.
- Life Orb now forecasts one eligible payment after the attack, not one per
  target. A native affected-target flag occupies existing damage-cache padding:
  the record remains12bytes. Hits into Substitute/Disguise can charge Orb even
  without occupant HP damage; a blocked attack cannot invent that payment.
  Magic Guard, effective item state and move-specific Sheer Force are respected.
- Bug Bite/Pluck can value stolen Sitrus healing before contact damage and Orb.
  These outcomes are one hit event, avoiding independently averaged healing and
  lethal recoil. Knock Off can remove a later actor's Orb liability.
- The former Helmet-only contact cache also handles native Iron Barbs/Rough
  Skin damage. Native contact eligibility is reused without recording imagined
  battle-history observations. Stat/status effects precede modeled contact
  death, so self-fainting does not erase an earlier Lunge drop.

The changes reuse bounded evaluation passes and existing cache storage. They
do not inspect the human's pending commands or prescribe a hoped-for move.

## Actual-party decisions and follow-through

The final run executes39 parameters in14 temporary native groups. A diagnostic
PASS means execution; the observations below distinguish tactical payoff from
mere execution. Opposing comparison sets are prepared level14 player Pokémon.

Physical opening, Eevee Double-Edge into Dewpider and Timburr Ice Punch into
Surskit: turn1 Rain Dance/Liquidation KOs Eevee, ownHP26/12. Turn2 Hydro Pump/
Liquidation leaves Timburr11 and ownHP15/12. Turn3 Icy Wind/Liquidation finishes
the sampled two-player team, all four James members alive. Baseline Sitrus
Dewpider instead protected on both sampled turns and fainted on turn2.

Special opening, Natu Psychic into Dewpider and Pachirisu Thunderbolt into
Surskit: turn1 Rain Dance/Liquidation KOs Natu, ownHP5/38. Turn2 Surskit pivots
to Venipede43HP while Liquidation leaves Pachirisu43. Turn3 Protect/Liquidation
finishes that two-player team, preserving Surskit5HP on the bench and all four
James members. In the candidate comparison without PRESSURE, Surskit instead
fainted for extra chip. This is a bounded preservation benefit, not proof that
every switch is optimal.

Fake Out opening, Mienfoo into Surskit and Pachirisu Thunderbolt into Dewpider:
rain is interrupted, but Liquidation deals useful damage. Turn2 Surskit pivots
and Dewpider KOs Mienfoo. Turn3 Skitter Smack/Liquidation leaves Venipede47,
Dewpider16, Pachirisu46, with no rain. The plan has useful actions after rain
denial; it does not guarantee a win or establish rain through Fake Out.

Actual reserves are also sampled with both leads already fainted and no
invented rain. Against physical Eevee/Timburr they Protect once, then Poison
Jab/Bug Bite trades Kricketune for damage; Venipede survives at41HP versus23
with its original spread. Turn3 Poison Jab finishes Eevee, Venipede30 and
Timburr81 remain. Against prepared Natu/Charmander, both reserves still lose
on turn2 after attacking. That no-rain counter remains real; no move was forced
or team redesigned just to turn every sampled loss into a win.

Two unrestricted hazard boards compare zero versus two healthy opposing bench
members. Neither sets Toxic Spikes in its sampled first turn. Retaining the
move as optional does not establish useful future poison or switch payoff.

## Mechanic controls and necessary regressions

Forced native controls isolate mechanics; they are not unrestricted AI wins.

- Actual Liquidation deals40 dry versus60 in rain in the sampled uncapped hits.
- Actual Lunge deals19; later Seed Bomb deals18 with the drop versus25 with
  Covert Cloak preventing it on the final Eviolite Dewpider.
- Landed actual Skitter Smack deals15; later Psychic deals19 versus28 in the
  missed-drop control. Native accuracy is preserved.
- Kricketune Orb controls finish46→42,5→1,4→0; its own Protect and target
  Protect preserve4HP. Prior-turn Substitute and Disguise hits still cause
  the4HP recoil KO. Disguise is a later-species boundary, not early availability.
- At4HP, stolen Sitrus heals11 before Orb4, leaving11; the no-berry control
  faints. At5HP into native Ferroseed, Iron Barbs5 causes a KO before Orb.
  Stealing its Sitrus first instead produces5+11−5−4=7HP.

Only two new permanent shared regression groups are retained:
six offensive-drop parameters and five Orb/berry/contact parameters. The latter
uses explicitly restricted menus and explicit stats; it is not presented as
James's full-party AI. It distinguishes Protect at fatal ordinary/contact
thresholds from attacking at a survivable threshold or after stolen healing.

Negative probes independently removing Lunge, Skitter, ordinary Orb, stolen
healing and Iron Barbs forecasting reproduce the corresponding wrong choices.
All negative mutations are restored. One new contact fixture initially demanded
Dewpider Liquidation even when its resisted move made Protect reasonable; that
unrelated assertion was removed, preserving the Kricketune decision and native
HP/item assertions. The corrected five-parameter group passes.

The final shared run passes19 groups, including existing Trick Room, Follow Me,
weather, Perish, activation and state-restoration regressions. This does not
mark any later encounter individually reviewed. All12 temporary James includes
are removed from the permanent test build; scratch diagnostics remain available
under work/ without becoming a maintained per-battle suite.

## Timing, integration and limits

The final six-native-player timing board selects both actions in43 warm frames
and50 cache-rebuild frames, approximately0.83seconds for the latter at60Hz.
This is one measured board, not a campaign latency ceiling or a no-freeze claim.
No extra outer8KiB snapshot is used by that timing diagnostic.

The accepted E0011 team is synchronized to the master and compiled party;
its generated plan is RAIN|PRESSURE. Full master-to-party verification passes
for516 branches, but that is projection consistency, not516 completed reviews.
The completed restart prefix after James is16branches/11encounters.

Dewpider repeats Rick in a distinct physical-rain role. Surskit and Venipede
first appear in the reviewed prefix. Kricketune later repeats, including the
same four moves; keeping its regional insect identity does not warrant copying
this rain plan elsewhere. No species redistribution is made here.

Early counterplay includes Natu/Pachirisu/Mienfoo/Ferroseed, Route103 Storm Drain
Shellos and Route104 Dry Skin Croagunk. Rain denial, redirection, Water immunity
and focused physical pressure matter; Fire is not a blanket answer to the
Water Bubble/rain lead.

Why8.5 rather than10: the rain opening now has a concrete partner payoff and a
tested preservation pivot, but the late reserve phase is less distinctive and
less coordinated without rain. Better conditional switching and demonstrated
hazard/endgame payoff could improve it without replacing every insect with
another Water attacker. No full six-versus-four match series or playthrough
is claimed. Multihit/spread/drain-plus-Orb correlations, other stolen berry
benefits and Aftermath are not newly certified by this work. Ripen/Cheek Pouch
branches follow native source but are not individually exercised here.

No release ROM, commit or publication at this checkpoint.
