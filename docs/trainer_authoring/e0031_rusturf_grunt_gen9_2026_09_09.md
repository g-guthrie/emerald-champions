# E0031 Rusturf Tunnel Grunt — final Gen 9 review

Actual dispatch: `trainerbattle_no_intro_double TRAINER_GRUNT_RUSTURF_TUNNEL`
at `data/maps/RusturfTunnel/scripts.inc:305`. Roxanne's defeat grants Badge1
and triggers the Rustboro stolen-goods scene that reveals this Grunt. Review
therefore uses one badge, player level20 and four actual trainer records—not
the preceding gym's level14 fixtures or a six-member invented party.

Mightyena/Qwilfish/Skrelp/Zubat occur9/7/3/2 times in current base parties.
Keep the recognizable Aqua identity; double Intimidate/Howl and a special
Tailwind reserve distinguish this encounter from Jerry's Choice Specs Skrelp.

## Changes and coaching

- Skrelp: Modest252HP/252SpA/4Def → **252HP/168SpA/88Speed**. Keep Eviolite,
  Adaptability, Sludge Bomb, Muddy Water, Acid Spray and Protect. At native
  level18, HP remains62, SpA47→42 and Speed21→25; Tailwind42→50 now passes
  reference Pachi49. It still does not pass Mienfoo53 under Tailwind. The
  discarded120HP/252SpA/136Speed candidate had56HP/27Speed and died a turn
  earlier under physical pressure; preserve bulk instead of buying every
  possible Speed crossing. No shared preset or player EVs were changed.
- Zubat: Focus Sash → **Eviolite**. Preserve Inner Focus, Jolly4HP/252Atk/
  252Speed, Brave Bird, Crunch, Tailwind and Protect. Brave Bird recoil quickly
  disables a Sash; repeated-hit bulk is more useful here. Loss of Sash's
  guaranteed full-health survival against a sufficiently strong hit is real.
- Preserve Life Orb/Jolly fast Mightyena and Black Sludge/Adamant bulky
  Qwilfish, all their moves, levels and EVs. Faster Howl supports the physical
  partner immediately; Thunder Wave remains conditional utility, not mandatory.
- Preserve executable SETUP/TAILWIND and the common expert AI. No shared-engine
  changes, added search, storage or recursion; universal IV31/berry rules remain.

Establish: double Intimidate buys a useful Howl turn; otherwise attack directly.
Exploit: both physical attackers use the boost, rather than repeating Howl.
Fallback: special Skrelp and Zubat's Tailwind/physical coverage respond to a
broken lead. Tailwind is valuable for the slower partner, even when Zubat is
already fast. Poison can justify a single waiting turn followed by attacks.
Special attacks, Clear Amulet/Intimidate immunity, focused Ground pressure and
breaking the reserve support pair remain separate answers.

Native Normal HP/Atk/Def/SpA/SpD/Speed before items:
MightyenaLv19:61/57/37/29/33/53; QwilfishLv19:71/63/54/27/31/43;
SkrelpLv18:62/28/32/42/32/25; ZubatLv19:50/39/24/19/26/47.
Mightyena53 acts before Qwilfish43; Howl's native boosts affect both. Tailwind
Zubat94/Qwilfish86 can pass Mienfoo53. No old flat-point Speed assumptions.

## Native evidence and limits

Baseline `work/rusturf-gen9-baseline.log`:15 cases. Faster/lower-HP candidate
`work/rusturf-gen9-tailwind-splits.log` also exposed a fixture requesting turn4
after a turn3 loss; the runner correctly rejected it. Bulky-speed comparison
`work/rusturf-gen9-bulky-speed.log`; counter/follow-through
`work/rusturf-gen9-counter-followthrough.log`.

Final `work/rusturf-gen9-final.log`:4 groups/25 cases:14 lead/reserve,
6 disruption,4 Clear Amulet,1 complete timing. Trainer factory uses actual
one-badge stats/compiled party. Player references are native level20 current
sets with checked preparation access. Reserve snapshots faint the two leads
and promote the actual final pair; normal lead runs retain natural switches.

- Physical lead: Howl/Waterfall reaches+1Attack on both partners. Subsequent
  Play Rough/Waterfall removes Eevee then Timburr by turn3, own49/71HP.
- Special lead: Howl/Poison Jab removes Lotad; Crunch/Waterfall follow-through
  finishes Pachi on turn3, own7/71HP. Assertions verify one boost, not repeated
  setup, and the native two-player finishes. These are not six-player win rates.
- Clear Amulet control: ordinary Eevee/Timburr end turn1 at−2Attack each;
  Amulet Timburr retains neutral Attack. Against that unweakened threat,
  Mightyena uses Play Rough immediately rather than Howl. Amulet sacrifices
  Eviolite, and Timburr falls on turn1 in that line; resistance to Intimidate
  is not claimed to be an automatic winning counter. Assertions verify native
  stat-drop prevention and the observed refusal of Howl.
- Fake Out/phazing into Taunt brings Zubat in for Mightyena. Turn2 Tailwind
  raises Zubat/Qwilfish to94/86 and Waterfall leaves Dreepy1HP; Mienfoo remains
  full66HP. This is recovery of initiative, not a completed win. Baseline
  attacked Dreepy down immediately instead: the revision has a tempo tradeoff.
- Fire/Grass lead: direct Crunch/Poison Jab removes Lotad, then finishes
  Electrike on turn2, own27/71HP. No unnecessary Howl under special pressure.
- Physical reserves: Muddy Water/Protect, then Muddy Water/Tailwind leaves
  Skrelp2HP and Zubat43HP; native Speed50/94 and timer3 are asserted. Next
  Sludge Bomb/Brave Bird attacks rather than refreshing Tailwind; Skrelp
  subsequently falls. By turn4 Eevee is removed, Timburr7HP and Zubat14HP
  remain. Lower-HP candidate lost both reserves by turn3; final remains weak
  to sustained physical focus, rather than being tuned to beat every board.
- Special reserves: Sludge Bomb/Brave Bird removes Lotad and poisons Pachi.
  Turn2 double Protect lets poison lower Pachi55→46HP; turn3 attacks leave1HP;
  turn4 Sludge Bomb/Brave Bird finishes, own8/28HP. This is one productive
  residual wait followed by offense, not repeated empty Protect certification.
- Fire/Grass reserves: Sludge Bomb/Brave Bird removes Lotad, then Tailwind/
  Sludge Bomb finishes Electrike on turn2, own54/29HP. A terminal snapshot's
  raw speed cache does not by itself prove an additional future turn occurred.
- Complete cold setup7 + both actor wrappers49 =56GBA frames (~0.94s), warm49;
  baseline57 complete. Actual four-member trainer and six current player
  presets with switch/form consideration. One sampled board, not a worst-case
  guarantee or measurement of every later trainer state.

Native chance effects and deterministic reference commands are observations,
not a competitive win-rate estimate. Player references include deliberately
retained unevolved Eviolite options; this does not exhaust newly available
post-badge team building. Scratch includes are removed; no permanent per-trainer
tests added. Frozen1848 ROM and Downloads copy are unchanged.

Clean `work/ai-shared-through-rusturf.log` passes46/46 retained shared groups.
Trainer and6923 preset materializations match; scoped whitespace checks pass.
This does not certify the remaining campaign or all possible Protect decisions.

Intention rating: **8.5/10**. Stronger reserve protection against concentrated
physical attacks and more deliberate Intimidate recycling would improve this
Grunt. After main-chat completion:37/516 branches,29/468 encounter records;
E0032 Rustboro rival variants are next.
