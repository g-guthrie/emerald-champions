# E0053 Greta in Slateport — Gen 9 native review

`TRAINER_GRETA_SLATEPORT` is an optional, separately accepted doubles battle
available on the first Slateport visit. The map actor has no hide flag and her
script has no Museum/badge prerequisite. She awards Gengarite; this does not
grant the Mega Ring. Preserve the reward and two-usable-Pokémon guard. Review
uses first-access Normal, one badge/cap20, native levels20/19/19/21.

## Accepted authoring

Gastly: Sludge Bomb → Sludge Wave. The native move targets foes and ally.
Shedinja's Wonder Guard blocks the allied Poison hit, allowing actual spread
damage beside that partner without spending its Focus Sash. This is not a
blanket safe-spread rule: Heracross and Umbreon are not immune. Shadow Ball
remains available for suitable targets and nonimmune partner situations.

Keep Life Orb, Levitate, Timid and the original SpA/Speed investment. Keep
Heracross, Umbreon and Shedinja unchanged. No species, EV, item, ability, level,
shared-AI, stat/berry or wild/tutor changes. Updated plan/crack prose describes
native decisions, not a new executable instruction layer.

| Pokémon | EVs HP/Atk/Def/SpA/SpD/Spe | Native HP/Atk/Def/SpA/SpD/Spe |
|---|---|---|
| Heracross |4/252/0/0/0/252|68/73/41/24/49/62|
| Umbreon |252/0/4/0/252/0|82/31/52/33/79/35|
| Shedinja |4/252/0/0/0/252|1/62/27/19/22/38|
| Gastly |4/0/0/252/0/252|50/23/24/66/26/63|

Universal IV31 and exact508-EV spreads remain. Shedinja's HP is fixed at1;
its HP EVs do not grant survival. Heracross62 becomes93 with Choice Scarf,
above the ordinary Natu56/Pachirisu49/Mienfoo53 references. Gastly63 is fast
enough to attack before them; Shedinja38 uses priority when appropriate.
Umbreon35 is slower than those fast foes, but its special bulk and Snarl let
it survive to recover. Foul Play does not require investment in its own Attack.
Icy Wind remains optional, not a compulsory action simply because it is present.

Distribution: retain Greta's Heracross/Umbreon/Shedinja signature trio and
the sole authored Gastly. This Scarf Moxie Heracross differs from Brawly's
Choice Band and later Mega/Guts variants. The later Shedinja has Goggles;
Greta's Sash keeps direct-hit protection but not residual immunity. The
second Umbreon is physically defensive with Yawn; Greta retains demonstrated
special mitigation and Wish. No species-count or habitat changes.

Coaching card: use Scarf attacks to secure a knockout and exploit Moxie;
Umbreon mitigates special pressure or punishes physical attackers with Foul
Play. When injured and able to survive, Wish heals on the following turn
without requiring a redundant guard. Reserves combine Wonder Guard, priority
and Gastly's safe spread attack. Protect remains situational, and Sludge Wave
must be evaluated against the actual partner. Choice lock, fourfold Flying
weakness, status/residual damage and Gastly's frailty remain counterplay.

## Native evidence

`work/greta-slateport-gen9-final-counterplay.log`:7 groups/28 parameter cases
PASS. Actual compiled/native trainer generation supplies opponents. Player
references use level20, IV31, exact EVs and preparation-legal moves. NPC
choices are unforced. Promoted Shedinja/Gastly and explicit Heracross/Gastly
are constructed survivor pairings, not claimed natural replacement choices.

- Physical lead: Close Combat removes Eevee turn1 and Moxie gives+1 Attack,
  asserted. Continued Close Combat with Foul Play finishes Timburr turn2;
  both leads remain full HP. Umbreon's initial Protect is not repeated.
- Psychic/Electric lead: Knock Off removes Natu turn1 and activates Moxie;
  continued Knock Off plus Snarl finishes Pachirisu turn3. Heracross38HP/
  Umbreon82HP remain. Choice lock is respected rather than silently changing
  Heracross's attack after the first knockout.
- Fake Out/Taunt: Heracross flinches first and Umbreon guards, then Close
  Combat/Snarl, Close Combat/Foul Play resume offense. Moxie activates turn3;
  turn4 finishes both foes, Heracross4HP/Umbreon82HP. No failed Wish spam
  through Taunt and no consecutive guards.
- Focus Sash Natu with Air Slash: Rock Slide/Snarl remove Natu turn1 without
  a Heracross Moxie boost; continued spread attacks leave Pachirisu18HP at
  turn3, both leads full. This observed line defeats the attempted Flying
  answer, not proof of immunity to Flying or a completed win. The original
  Eviolite Natu reference instead follows the Knock Off line above. Native
  secondary-effect outcomes are not guarantees.
- Injured Umbreon: starts32/82HP against special pressure. It uses Wish,
  ends turn1 at28HP with the pending counter1, then uses Snarl while the
  actual Wish animation/healing resolves turn2 to65HP. Turn3 finishes both
  foes with Heracross68HP/Umbreon65HP. Wish PP remains9, asserted; it does
  not recast an already pending Wish or insist on Protect when attacking
  and surviving is sufficient. This demonstrates self-recovery, not Wish passing.
- Physical reserves: X-Scissor/Protect first, then X-Scissor/Sludge Wave
  removes Eevee and damages Timburr to44HP; old Sludge Bomb left Timburr78HP
  at that endpoint. Shadow Sneak/Shadow Ball finishes turn3 at Shedinja1HP/
  Gastly40HP. Actual Sludge Wave animation, Shedinja1HP, intact Sash and
  absence of status are asserted. Ordinary player Normal/Fighting commands
  cannot damage this Ghost pair; that reference is not optimal player play.
- Special reserves: X-Scissor/Shadow Ball removes Natu turn1, then priority
  plus special attacks finish Pachirisu turn3 at Shedinja1HP/Gastly35HP.
  Final Icy Wind participates in the knockout, not a claimed necessary
  speed advantage or repeated speed-setup loop.
- Ghost-coverage reference: Shadow Sneak followed by Sludge Wave removes
  the opposing Sash Gastly before it lands its attempted Shadow Ball, while
  the spread attack also pressures Pachirisu. Priority/Shadow Ball finish
  turn2 at1HP/13HP. Old
  Sludge Bomb's line lost Gastly and left Pachirisu14HP at turn2.
- Burn counterplay: Prankster Will-O-Wisp bypasses Wonder Guard; Shedinja
  faints turn1 with Sash still held. Gastly survives turn2 at15HP but is also
  burned, both foes alive. Residual status remains a real answer, not blocked
  by Sash or Wonder Guard. This does not establish good status-aware switching.
- Explicit nonimmune Heracross/Gastly: Gastly uses Protect then Shadow Ball,
  not Sludge Wave into its partner. Both foes fall turn2, Heracross50HP/
  Gastly45HP. This is one safety control, not proof of every mixed-pair decision.

No natural Shedinja entry or deliberate Wish passing was demonstrated. The
main Scarf/Moxie, Snarl/Wish and Wonder Guard/spread motifs do execute. Status
protection and deeper reserve positioning remain improvements under bounded
review, not reasons to add speculative shared branches.

Evidence: `work/greta-slateport-gen9-baseline.log`,
`work/greta-slateport-gen9-spread-support.log`,
`work/greta-slateport-gen9-final.log` and final counterplay log. The initial
scratch addition needed a C forward declaration; an overlong four-turn Wish
fixture was shortened after the battle ended on turn3. Neither was a game
defect. Temporary `work/greta_slateport_gen9_probe.inc` is excluded from the
retained build; earlier Museum helpers are reused, not their outcome evidence.

Complete cold six-player-party decision:33 GBA frames (~0.55s),6 setup+27
both-actor frames; warm28 excludes setup. Unchanged from baseline, under1.2s
for this sample, not campaign-wide worst-case certification. Canonical checks
match2339 trainer slots/516 branches and6923 shared sets. Clean shared
regression outcome is recorded in the ledger. No new search, allocations,
permanent trainer tests, release build or replacement of the Downloads ROM.

## Rating

8.5/10, intention-based. More deliberate Wish passing and safer status-aware
reserve positioning would improve Greta. She has a fast knockout engine,
demonstrated sustain and a distinctive safe-spread reserve partnership, with
meaningful Choice-lock and status counterplay still intact.
