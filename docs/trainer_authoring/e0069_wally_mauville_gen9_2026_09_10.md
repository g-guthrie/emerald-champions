# E0069 Wally, Mauville — Gen 9 native review

## Encounter and accepted changes

`data/maps/MauvilleCity/scripts.inc:201` invokes
`trainerbattle_no_intro_double TRAINER_WALLY_MAUVILLE`. This is one fixed
four-member party, not a Birch-starter-dependent rival alias. Native Normal
construction at two badges/cap30 gives Swablu/Roselia29 and Kirlia/Magneton30,
with universal effective IV31. Preserve the Hoenn partners and Kirlia identity.

- **Swablu: Sitrus →Eviolite, Calm special bulk →Bold physical bulk,
  Protect →Helping Hand.** Retain Hyper Voice, Roost, Tailwind and Natural
  Cure. Eviolite plus defense investment makes the support bird substantially
  harder to remove physically while keeping similar effective special bulk.
  Roost supplies repeatable healing; losing Sitrus's immediate recovery matters.
- **Kirlia: Timid special Speed →Modest 140HP/252SpA/116Spe.** Retain Eviolite,
  Trace, Psychic, Dazzling Gleam, Calm Mind and Protect. Gain HP and damage while
  retaining enough Speed for a meaningful Tailwind threshold. Losing Tailwind
  now exposes a slower Kirlia; this is an explicit trade, not a free upgrade.
- Retain Sash/Natural Cure Roselia and Balloon/Sturdy Magneton with their
  existing offensive moves and EVs. Grass/Poison and Electric/Steel reserves
  complement Psychic/Fairy pressure, though both dislike Fire.
- Rewrite canonical plan/crack. Retain executable SETUP/TAILWIND preferences
  and Conservative AI. No shared-engine changes or new retained tests needed.
  No shared wild/tutor sets, levels, progression or species changed.

Coaching: establish useful Tailwind and use Calm Mind when the boost can pay
off; amplify attacks with Helping Hand instead of making Swablu stall; fall
back to direct damage, Roselia's sleep/Grass pressure and Magneton's coverage.
Not every safe-looking turn needs Calm Mind, and Trace is matchup-dependent.
Nearby trainers also use Magneton/Kirlia, but this fight keeps Wally's familiar
four families and centers the bird's support of his original Psychic partner.

## Stats and Speed review

| Pokemon | Nature; EVs HP/Atk/Def/SpA/SpD/Spe | Native HP/Atk/Def/SpA/SpD/Spe |
|---|---|---|
| Swablu, Eviolite | Bold;252/0/252/0/4/0 |92/33/73/37/57/42|
| Kirlia, Eviolite | Modest;140/0/0/252/0/116 |82/31/35/79/47/53|
| Roselia, Focus Sash | Timid;4/0/0/252/0/252 |77/43/40/90/60/75|
| Magneton, Air Balloon | Modest;4/0/0/252/0/252 |79/45/71/115/56/75|

Swablu previously had49Def/82SpDef without Eviolite. New effective defenses
are109/85; raw HP and Speed remain92/42. Kirlia previously had72HP/72SpA/69Spe;
now82/79/53. Native Speed queries confirm **106 under Tailwind against Xatu105**.
Kirlia still outruns reference Eevee47, Lombre44 and Gurdurr38 without wind,
but trails Pachirisu71 and Mienfoo79. These are level30 reference thresholds,
not guarantees across all opposing builds or difficulty offsets.

Swablu reaches84 under Tailwind, ahead of those slower references but not
Xatu/Haunter105. Both reserves reach150; their75/75 tie has no required ordered
ally-activation move. Helping Hand's priority permits support before Kirlia.
No IV exceptions or automatic berry-parity adjustment were introduced.

## Native verification

`work/wally-mauville-gen9-final-verified.log`: **8 groups/53 parameter cases
pass** using `work/wally_mauville_gen9_probe.inc`. Actual-party cases use native
construction and autonomous replacement decisions. Promoted Roselia/Magneton
controls reorder them into the lead and mark the original leads fainted;
those are isolated reserve controls, not claims of natural entry. Player
references have native level30 stats and checked preparation moves. NPC actions
are unforced; some Sleep Powder outcomes and poison rolls are favorable.

- **Actual physical Eevee/Gurdurr:** Tailwind/Protect1; Helping Hand and
  Psychic/Gleam pressure2–3; Swablu Roosts4, then supported Psychic finishes5
  with Swablu34/Kirlia58 and both reserves intact. No consecutive empty guards.
- **Actual special Xatu/Pachirisu:** Trace copies Volt Absorb. Tailwind1 lets
  Kirlia act before Xatu on following turns; direct attacks finish4 at12/82.
  Queries assert native Speed106 versus105, copied ability and a single
  Tailwind cast. This favorable Trace result is not guaranteed in all battles.
- **Supported setup, Lombre/Pachirisu:** Tailwind/Calm Mind1, then Helping
  Hand/boosted Gleam2 and Psychic3–4. Finish4 at92/44. Assertions verify exactly
  one Calm Mind, +1SpA/+1SpDef, attack PP and Helping Hand PP. Trace/Volt Absorb
  protects Kirlia from Thunderbolt, while Scald still damages it. No repeated
  setup after establishing the boost.
- **Fake Out/Scald:** Swablu flinches1; Kirlia's spread pressure and later
  Psychic remove Mienfoo3. Kirlia Calm Minds4 and delivers boosted Psychic5,
  but falls to Scald; Roselia enters and finishes6 at54/77. Preserving boosted
  Kirlia is still an opportunity, not claimed solved by the HP investment.
- **Taunt denial:** faster Mienfoo denies the opening Tailwind. Swablu uses
  Hyper Voice2–3 while taunted; Kirlia sets Calm Mind3 and attacks4–5. Swablu
  sets Tailwind4 after Taunt ends, not while still blocked. Finish5 at60/16
  with both reserves intact. Early assertions confirm no active Tailwind.
- **Burn/Follow Me:** Swablu is burned but remains a special/support actor.
  Gleam bypasses redirection, Helping Hand amplifies the attacks and the team
  finishes4 at77/82. Natural Cure is present but no cure-on-switch is claimed
  from this sample; Swablu stays in.
- **Ground Mudbray/Xatu:** Tailwind/Protect1, then spread pressure and Helping
  Hand remove Xatu before it can finish Swablu3. Earthquake still KOs Kirlia.
  Roselia enters naturally and Giga Drain finishes4 at12/77, Magneton intact.
  The queued Roost on4 does not execute because Roselia ends the battle first.
- **Fire/Ghost Ponyta/Haunter:** Tailwind/Protect1 blocks Shadow Ball; Kirlia
  then outspeeds and KOs Haunter with Psychic2. Helping Hand/Psychic finish
  Ponyta3 at26/82 with both reserves intact. Trace copies Levitate here, not
  Volt Absorb. Swablu's Hyper Voice does not damage the Ghost.
- **Promoted reserves:** physical reference finishes6 at31/7 through useful
  sleep, special attacks and Giga Drain recovery. Both opponents are slept
  on separate turns; sleep duration/accuracy help this result. Special
  reference finishes3 at77/67 with Roselia attacking after sleep support;
  Fake Out/Scald finishes4 at52/45; burn/redirection finishes4 at30/79.
  Early Thunderbolt into Follow Me/Volt Absorb remains imperfect before
  Magneton changes to Flash Cannon. The isolated physical finish is narrow,
  not proof of reliable survival against every Fighting attacker.

Baseline and support/Kirlia-bulk candidate logs are preserved in `work/`.
One extended fixture requested seven turns for a fight that finished in six;
only the horizon was corrected. No gameplay was changed to satisfy that error.
The scratch includes are removed after review. Item/ability presence is not
proof that every Sash, Sturdy, Balloon or Natural Cure interaction executed.

## Timing and closure

Complete native six-player-party decision: **45 frames (~0.75s)**,6 setup+39
both actors; warm pair39. Baseline44, Swablu-only trial46. Includes gimmick
assignment, move-category/data preparation, ordering and both actors. Well
below72 frames/1.2s, but not a campaign-wide or every-board worst-case claim.

Canonical trainer/preset checks pass:516 branches/2339 slots and6923 shared
sets. `work/ai-shared-through-wally-mauville.log`: **55/55 retained native
groups pass**, scratch excluded. No new engine code, retained scenarios or
ROM/RAM structures. No ROM built/replaced in Downloads.

**Intention rating:8.5/10.** Better preservation of a boosted Kirlia and more
reliable sleep-independent reserve pressure would improve Wally.
