# Exact world revision specification

The following are proposed changes to implement later. Nothing here has been applied to the game or snapshot. Text blocks include native .string serialization; new flags use the [central state reservations](../appendices/state-allocation.md), and new events require local ID allocation plus the stated runtime acceptance.

This list is intentionally selective. The complete map/event ledger explicitly preserves the remaining authored world. Global INTRO-01, DIFF-01, GUIDE-01, FAC-01 and FAC-02 are owned by the lead-editor chapters.

<a id="w-sign-optional"></a>

## W-SIGN-OPTIONAL — Remove live mandatory-Devon instructions

**REPAIR.** The acquisition definitions explicitly make Devon optional, but several live story handoffs still require a visit before every Sign. These are active interactions, unlike the unbound expansion-guide scripts cataloged separately.

### `Route110_Text_MayExplainItemfinder`

Source: [data/maps/Route110/scripts.inc:854](../baseline/source/data/maps/Route110/scripts.inc#L854)

Current: MAY: That's a DOWSING MACHINE. Hidden /  items make it sing. //  Have DEVON's researcher translate /  each SIGN before seeking its partner. //  I'm heading north to MAUVILLE! /  WATTSON's GYM is there. See you ahead!

Final text:

```asm
Route110_Text_MayExplainItemfinder:
	.string "MAY: That's a DOWSING MACHINE. It\n"
	.string "reacts to hidden items.\p"
	.string "DEVON's researcher has useful\n"
	.string "leads if you want help finding a\l"
	.string "SIGN.\p"
	.string "I'm heading north to MAUVILLE!\n"
	.string "WATTSON's GYM is there. See you!$"
```

### `GraniteCave_StevensRoom_Text_IveGotToHurryAlong`

Source: [data/maps/GraniteCave_StevensRoom/scripts.inc:231](../baseline/source/data/maps/GraniteCave_StevensRoom/scripts.inc#L231)

Current: I'm heading east. MAGMA and AQUA are /  disturbing more than land and sea. //  A mark in the lower cave resembles /  LUCARIO. Take a member of that family /  to DEVON's researcher on the 2nd floor. //  Each SIGN needs its own translation /  before you return to seek its Pokémon.

Final text:

```asm
GraniteCave_StevensRoom_Text_IveGotToHurryAlong:
	.string "I'm heading east. MAGMA and AQUA\n"
	.string "are disturbing more than land and\l"
	.string "sea.\p"
	.string "Inspect the marked stone in the\n"
	.string "lower cave. COBALION may answer\l"
	.string "once you hold the STONE BADGE.\p"
	.string "DEVON's researcher keeps optional\n"
	.string "clues. You can explore without\l"
	.string "him.$"
```

### `NewMauville_Inside_Text_RotomCaught`

Source: [data/maps/NewMauville_Inside/scripts.inc:365](../baseline/source/data/maps/NewMauville_Inside/scripts.inc#L365)

Current: The generator finally falls silent. //  The ROTOM CATALOG nearby can change /  ROTOM between all five appliances. //  Faint conduits sketch REGISTEEL, /  MANECTRIC, and DIALGA. //  Bring them to the dream researcher /  at DEVON CORP. 2F in RUSTBORO. //  Research each Sign before returning /  to find its challenger here.

Final text:

```asm
NewMauville_Inside_Text_RotomCaught:
	.string "The generator finally falls\n"
	.string "silent.\p"
	.string "The ROTOM CATALOG nearby changes\n"
	.string "ROTOM into its five appliances.\p"
	.string "Inspect the plant's marked stone\n"
	.string "for other unusual discoveries.\p"
	.string "REGIELEKI responds to REGISTEEL.\n"
	.string "DEVON can explain that clue and\l"
	.string "the conditions for the other\l"
	.string "SIGNS.$"
```

### `Route119_WeatherInstitute_2F_Text_PokemonChangesWithWeather`

Source: [data/maps/Route119_WeatherInstitute_2F/scripts.inc:309](../baseline/source/data/maps/Route119_WeatherInstitute_2F/scripts.inc#L309)

Current: CASTFORM changes with the weather. /  Let it show you what a storm can do. //  Before each SIGN's challenge, visit /  DEVON's researchers in RUSTBORO.

Final text:

```asm
Route119_WeatherInstitute_2F_Text_PokemonChangesWithWeather:
	.string "CASTFORM changes with the weather.\n"
	.string "Let it show you what a storm can\l"
	.string "do.\p"
	.string "Investigate SIGNS as you explore.\n"
	.string "DEVON's researcher offers clues\l"
	.string "whenever you want some guidance.$"
```

### `Route119_WeatherInstitute_2F_Text_ChangingWeatherRidiculous`

Source: [data/maps/Route119_WeatherInstitute_2F/scripts.inc:326](../baseline/source/data/maps/Route119_WeatherInstitute_2F/scripts.inc#L326)

Current: Our instruments still trace that old /  network beneath HOENN. //  Report to DEVON's lab in RUSTBORO /  before each SIGN you want to challenge. //  The road north leads to FORTREE. /  You're welcome to rest here on the way.

Final text:

```asm
Route119_WeatherInstitute_2F_Text_ChangingWeatherRidiculous:
	.string "Our instruments still trace that\n"
	.string "old network beneath HOENN.\p"
	.string "DEVON's lab has optional leads for\n"
	.string "the SIGNS you want to investigate.\p"
	.string "The road north leads to FORTREE.\n"
	.string "You're welcome to rest here.$"
```

Dependencies: Acquisition Sign definitions; GUIDE-01

Acceptance: Read both rival variants and every listed first/repeat branch. Reach and activate a qualified landmark without ever speaking to Devon researcher; then read its completed clue afterward.

<a id="w-sign-local"></a>

## W-SIGN-LOCAL — Replace obsolete partner silhouettes with truthful local leads

**REPAIR.** Current Sign definitions removed most species prerequisites. Physical signs and live residents still name former required families. Keep clues exciting while making each specified badge or story condition match the snapshot acquisition owner.

### `Route110_Text_MauvilleCitySign`

Source: [data/maps/Route110/scripts.inc:1023](../baseline/source/data/maps/Route110/scripts.inc#L1023)

Current: ROUTE 110 /  {UP_ARROW} MAUVILLE CITY //  A scorched note says MANECTRIC may /  call a storm Champion after MAUVILLE.

Final text:

```asm
Route110_Text_MauvilleCitySign:
	.string "ROUTE 110 {UP_ARROW} MAUVILLE CITY\p"
	.string "A storm is carved into the stone.\n"
	.string "Return with the DYNAMO BADGE and\l"
	.string "inspect the nearby landmark.$"
```

### `Route112_Text_MtChimneySign`

Source: [data/maps/Route112/scripts.inc:175](../baseline/source/data/maps/Route112/scripts.inc#L175)

Current: MT. CHIMNEY //  “For LAVARIDGE TOWN or the summit, /  please take the CABLE CAR.” //  A newer carving points into FIERY PATH: /  a flame, a mane, and TORKOAL's shell.

Final text:

```asm
Route112_Text_MtChimneySign:
	.string "MT. CHIMNEY\p"
	.string "For LAVARIDGE TOWN or the summit,\n"
	.string "please take the CABLE CAR.\p"
	.string "A newer carving points to FIERY\n"
	.string "PATH. A fiery guardian may answer\l"
	.string "a holder of the DYNAMO BADGE.$"
```

### `Route119_Text_WeatherInstitute`

Source: [data/maps/Route119/scripts.inc:611](../baseline/source/data/maps/Route119/scripts.inc#L611)

Current: WEATHER INSTITUTE //  FIELD NOTE: CASTFORM may wake the /  storm Champion near FORTREE.

Final text:

```asm
Route119_Text_WeatherInstitute:
	.string "WEATHER INSTITUTE\p"
	.string "FIELD NOTE: Inspect this route's\n"
	.string "marked stone with the BALANCE\l"
	.string "BADGE. A storm may answer.$"
```

### `Route120_Text_RouteSignFortree`

Source: [data/maps/Route120/scripts.inc:469](../baseline/source/data/maps/Route120/scripts.inc#L469)

Current: ROUTE 120 /  {LEFT_ARROW} FORTREE CITY //  Two weathered marks show GARDEVOIR /  beside water and XATU beside grass.

Final text:

```asm
Route120_Text_RouteSignFortree:
	.string "ROUTE 120 {LEFT_ARROW} FORTREE\n"
	.string "CITY\p"
	.string "A lakeside SIGN answers the\n"
	.string "BALANCE BADGE. Other marks await a\l"
	.string "Champion who returns here.$"
```

### `Route123_Text_RouteSignMtPyre`

Source: [data/maps/Route123/scripts.inc:190](../baseline/source/data/maps/Route123/scripts.inc#L190)

Current: {UP_ARROW} MT. PYRE /  “Forbidden to the faint of heart.” //  A horned ward below the words resembles /  TAUROS.

Final text:

```asm
Route123_Text_RouteSignMtPyre:
	.string "{UP_ARROW} MT. PYRE Forbidden to\n"
	.string "the faint of heart.\p"
	.string "A guardian's mark stands among\n"
	.string "these fields. Return with the\l"
	.string "FEATHER BADGE and inspect it.$"
```

### `PetalburgWoods_Text_TrainerTipsPP`

Source: [data/maps/PetalburgWoods/scripts.inc:468](../baseline/source/data/maps/PetalburgWoods/scripts.inc#L468)

Current: TRAINER TIPS //  CUT reveals the eastern paths into the /  old-growth forest. //  Rangers report two CHAMPION'S SIGNS: /  one seeks BRELOOM, one seeks BLISSEY.

Final text:

```asm
PetalburgWoods_Text_TrainerTipsPP:
	.string "FOREST FIELD NOTES\p"
	.string "CUT reveals paths into the\n"
	.string "old-growth forest.\p"
	.string "One SIGN answers the KNUCKLE\n"
	.string "BADGE. Deeper marks answer the\l"
	.string "HEAT BADGE. Inspect them yourself;\l"
	.string "DEVON offers clues if you want.$"
```

### `MeteorFalls_1F_2R_Text_NicolasPostBattle`

Source: [data/maps/MeteorFalls_1F_2R/scripts.inc:83](../baseline/source/data/maps/MeteorFalls_1F_2R/scripts.inc#L83)

Current: The rear wall rings near REGIROCK. /  A deeper mark resembles ALAKAZAM. //  Bring those families through the Falls; /  the cave may answer without DEVON.

Final text:

```asm
MeteorFalls_1F_2R_Text_NicolasPostBattle:
	.string "A mark here answers REGIROCK once\n"
	.string "you hold the FEATHER BADGE.\p"
	.string "Other SIGNS lie deeper in the\n"
	.string "Falls. Inspect them as you\l"
	.string "explore; DEVON keeps their\l"
	.string "conditions on file.$"
```

### `Route114_FossilManiacsTunnel_Text_NotSafeThatWay`

Source: [data/maps/Route114_FossilManiacsTunnel/scripts.inc:101](../baseline/source/data/maps/Route114_FossilManiacsTunnel/scripts.inc#L101)

Current: Oh… /  It's not safe that way… //  I was digging away, you see… /  When the whole wall collapsed… //  I think there's a giant cavern /  underneath now… //  But I've left it alone because I don't /  think there are any FOSSILS there… //  One new wall mark does resemble /  LANDORUS. That is not a fossil.

Final text:

```asm
Route114_FossilManiacsTunnel_Text_NotSafeThatWay:
	.string "Oh... It's not safe that way...\p"
	.string "I was digging when the wall fell\n"
	.string "into an enormous cavern!\p"
	.string "It connects to SANDSTREWN RUINS.\n"
	.string "Look for ancient fossils and a\l"
	.string "marked stone beneath the desert.$"
```

### `MtPyre_Summit_Text_GroudonKyogreTale`

Source: [data/maps/MtPyre_Summit/scripts.inc:724](../baseline/source/data/maps/MtPyre_Summit/scripts.inc#L724)

Current: Long ago, GROUDON raised the land and /  KYOGRE carved the sea. Their clash grew /  until neither could stop itself. //  Ancient Trainers formed a network of /  SIGNS with trusted POKéMON as its keys. //  The RED and BLUE ORBS joined that net, /  but RAYQUAZA supplied its balance. //  The ORBS calmed the rivals, the SIGNS /  fell dark, and the sky guardian departed. //  The exterior still bears lesser marks: /  one sketches GARDEVOIR, one ABSOL.

Final text:

```asm
MtPyre_Summit_Text_GroudonKyogreTale:
	.string "Long ago, GROUDON raised the land\n"
	.string "and KYOGRE carved the sea. Their\l"
	.string "clash grew until neither could\l"
	.string "stop.\p"
	.string "Ancient Trainers formed a network\n"
	.string "of SIGNS with trusted POKéMON.\p"
	.string "The RED and BLUE ORBS joined it,\n"
	.string "but RAYQUAZA supplied its balance.\p"
	.string "The mountain still bears other\n"
	.string "SIGNS. Inspect them yourself;\l"
	.string "their conditions differ.$"
```

### `MtPyre_Summit_Text_HoennTrioTale`

Source: [data/maps/MtPyre_Summit/scripts.inc:752](../baseline/source/data/maps/MtPyre_Summit/scripts.inc#L752)

Current: Land and sea became absolute, and HOENN /  began to break between them. //  A green POKéMON descended from the sky, /  but it answered a mortal Trainer's call. //  RAYQUAZA restored the board. The Trainer /  restored the reason to keep playing. //  Each awakened SIGN waits to see what /  kind of Champion that Trainer becomes. //  On the sixth floor, a broken-world mark /  now sketches DUSKNOIR.

Final text:

```asm
MtPyre_Summit_Text_HoennTrioTale:
	.string "Land and sea became absolute, and\n"
	.string "HOENN began to break.\p"
	.string "A green POKéMON descended from the\n"
	.string "sky and answered your call.\p"
	.string "RAYQUAZA restored the balance. You\n"
	.string "gave it a reason to answer.\p"
	.string "The sixth floor holds another\n"
	.string "SIGN. Return there after earning\l"
	.string "the title of Champion.$"
```

### `MossdeepCity_Text_LifeNeedsSeaToLive`

Source: [data/maps/MossdeepCity/scripts.inc:364](../baseline/source/data/maps/MossdeepCity/scripts.inc#L364)

Current: All life needs the sea to live, even /  though it makes its home on the land. //  Old divers tell of a RELICANTH circling /  the SEAFLOOR CAVERN approach. //  When it does, a princely light stirs /  beneath the trench.

Final text:

```asm
MossdeepCity_Text_LifeNeedsSeaToLive:
	.string "All life needs the sea, even when\n"
	.string "it makes its home on the land.\p"
	.string "Divers found a princely SIGN near\n"
	.string "the SEAFLOOR CAVERN approach.\p"
	.string "Inspect that underwater landmark\n"
	.string "with the MIND BADGE. The stolen\l"
	.string "submarine points the way.$"
```

Dependencies: src/data/pokemon/legendary_signs.h; GUIDE-01

Acceptance: Inspect the exact listed landmarks at just-below and just-at their conditions. Confirm none of the removed ordinary-family prerequisites is checked. Preserve current Cresselia/Darkrai/Regi/quest prerequisites that remain intentional.

<a id="w-mega-guide"></a>

## W-MEGA-GUIDE — Make Mr. Stone describe the actual Mega economy

**REPAIR.** Mr. Stone promises a free Mega archive that no longer exists. The free archive contains evolution items; Mega Stones are world rewards.

### `RustboroCity_DevonCorp_3F_Text_ExplainPidgeotite`

Source: [data/maps/RustboroCity_DevonCorp_3F/scripts.inc:292](../baseline/source/data/maps/RustboroCity_DevonCorp_3F/scripts.inc#L292)

Current: Shops never sell MEGA STONES. Once you /  hold a RING, every POKéMON CENTER /  VENDOR lends the whole archive free. //  The ones in HOENN's gyms, ruins and /  caves are the originals. Find them.

Final text:

```asm
RustboroCity_DevonCorp_3F_Text_ExplainPidgeotite:
	.string "PIDGEOTITE lets a PIDGEOT Mega\n"
	.string "Evolve when it holds the stone and\l"
	.string "you possess a MEGA RING.\p"
	.string "Find Mega Stones in HOENN's caves,\n"
	.string "gyms and side paths, or through\l"
	.string "the people you meet.\p"
	.string "CENTER vendors supply held items\n"
	.string "and ordinary evolution items free\l"
	.string "of charge.$"
```

### `RustboroCity_DevonCorp_3F_Text_NotFamiliarWithTrends`

Source: [data/maps/RustboroCity_DevonCorp_3F/scripts.inc:299](../baseline/source/data/maps/RustboroCity_DevonCorp_3F/scripts.inc#L299)

Current: The old records call these marks a /  network of CHAMPION'S SIGNS. //  They do not reward strength alone. Each /  one asks for the right partner or deed.

Final text:

```asm
RustboroCity_DevonCorp_3F_Text_NotFamiliarWithTrends:
	.string "The old records call these marks a\n"
	.string "network of CHAMPION'S SIGNS.\p"
	.string "Each has its own conditions. Some\n"
	.string "answer a BADGE or a deed; a few\l"
	.string "recognize another POKéMON.\p"
	.string "Our researcher can explain them.\n"
	.string "His guidance is entirely optional.$"
```

Dependencies: World Mega reward catalogue

Acceptance: After obtaining Ring, read Mr. Stone repeat text and open a Center vendor: no Mega archive is offered; original world Mega routes remain unchanged.

<a id="w-outcome-prose"></a>

## W-OUTCOME-PROSE — Acknowledge wins without inventing their turn sequence

**REPAIR.** Scripted after-battle lines cannot know whether Tailwind occurred, a lead was read in one turn, or the player rebuilt a team. Level differences remain an intentional difficulty lever.

### `Route104_Text_MayIntro`

Source: [data/maps/Route104/scripts.inc:1098](../baseline/source/data/maps/Route104/scripts.inc#L1098)

Current: MAY: {PLAYER}{KUN}! Two badges' worth of /  catching between us now. Show me!

Final text:

```asm
Route104_Text_MayIntro:
	.string "MAY: {PLAYER}{KUN}! We've both\n"
	.string "found new partners since ROUTE\l"
	.string "103.\p"
	.string "Show me how your team works!$"
```

### `Route110_Text_MayTakeThis`

Source: [data/maps/Route110/scripts.inc:849](../baseline/source/data/maps/Route110/scripts.inc#L849)

Current: MAY: You didn't outlevel me. Nobody can, /  here. You built the better answer. //  Take this. It finds what routes hide.

Final text:

```asm
Route110_Text_MayTakeThis:
	.string "MAY: Your partners found an answer\n"
	.string "to mine. That was a good battle!\p"
	.string "Take this. It finds what routes\n"
	.string "hide.$"
```

### `Route110_Text_BrendanTakeThis`

Source: [data/maps/Route110/scripts.inc:887](../baseline/source/data/maps/Route110/scripts.inc#L887)

Current: BRENDAN: Levels don't decide anything in /  HOENN. Plans do. Yours was better. //  Here. Take this.

Final text:

```asm
Route110_Text_BrendanTakeThis:
	.string "BRENDAN: A strong team, well\n"
	.string "played. You earned that win.\p"
	.string "Here. Take this.$"
```

### `Route119_Text_MayPresentForYou`

Source: [data/maps/Route119/scripts.inc:500](../baseline/source/data/maps/Route119/scripts.inc#L500)

Current: MAY: I worried the BADGE road would /  harden you into one team. It didn't. //  Here. I have a present for you.

Final text:

```asm
Route119_Text_MayPresentForYou:
	.string "MAY: Your partners came through\n"
	.string "that battle together.\p"
	.string "Here. I have a present for you.$"
```

### `LilycoveCity_Text_MayDefeat`

Source: [data/maps/LilycoveCity/scripts.inc:472](../baseline/source/data/maps/LilycoveCity/scripts.inc#L472)

Current: … … … … … … … … //  I remember ROUTE 103. You didn't win /  on power then either. You read faster. //  You're still reading faster.

Final text:

```asm
LilycoveCity_Text_MayDefeat:
	.string "MAY: I remember our first battle\n"
	.string "on ROUTE 103.\p"
	.string "You keep finding ways through my\n"
	.string "teams. I'll have to try again!$"
```

### `LilycoveCity_Text_BrendanDefeat`

Source: [data/maps/LilycoveCity/scripts.inc:523](../baseline/source/data/maps/LilycoveCity/scripts.inc#L523)

Current: Humph… You read my lead in one turn. //  That stings. I had a head start on you /  as a TRAINER…

Final text:

```asm
LilycoveCity_Text_BrendanDefeat:
	.string "BRENDAN: You found a way through\n"
	.string "my team. That stings.\p"
	.string "I had a head start as a TRAINER...\n"
	.string "I'll have to make the next one\l"
	.string "count.$"
```

### `MauvilleCity_Text_WallyDefeat`

Source: [data/maps/MauvilleCity/scripts.inc:510](../baseline/source/data/maps/MauvilleCity/scripts.inc#L510)

Current: WALLY: … … … … … … … //  My wind never came. You took the lead /  before it mattered. //  I lost…

Final text:

```asm
MauvilleCity_Text_WallyDefeat:
	.string "WALLY: ...\p"
	.string "I had a plan for every partner.\n"
	.string "You still found your way through.\p"
	.string "I lost...$"
```

Dependencies: INTRO-01; DIFF-01; individual battle dialogue volume

Acceptance: Read each branch after different valid wins, including letting the opponent establish its field and preserving a favorite team. No line asserts an unobserved move or exact turn.

<a id="w-opening-tips"></a>

## W-OPENING-TIPS — Align opening field advice with the final doubles opening

**REPAIR.** Opening services and roster are changed by INTRO-01. Existing lead-order advice describes a single lead and can mislead players building their first doubles pair.

### `OldaleTown_House1_Text_LeftPokemonGoesOutFirst`

Source: [data/maps/OldaleTown_House1/scripts.inc:8](../baseline/source/data/maps/OldaleTown_House1/scripts.inc#L8)

Current: When a POKéMON battle starts, the one /  at the left of the list goes out first. //  So, when you get more POKéMON in your /  party, try switching around the order /  of your POKéMON. //  It could give you an advantage.

Final text:

```asm
OldaleTown_House1_Text_LeftPokemonGoesOutFirst:
	.string "In a DOUBLE BATTLE, your first two\n"
	.string "Pokémon able to battle form your\l"
	.string "opening pair.\p"
	.string "Change their party order before\n"
	.string "you challenge a Trainer. Those two\l"
	.string "partners set the opening.$"
```

### `Route104_Text_TrainerTipsDoubleBattles`

Source: [data/maps/Route104/scripts.inc:1057](../baseline/source/data/maps/Route104/scripts.inc#L1057)

Current: TRAINER TIPS //  In the HOENN region there are pairs /  of TRAINERS who challenge others /  for 2-on-2 POKéMON battles called /  DOUBLE BATTLES. //  In a DOUBLE BATTLE, the TRAINER must /  send out two POKéMON, the one at the /  left of the list and the top one. /  Watch how POKéMON are lined up.

Final text:

```asm
Route104_Text_TrainerTipsDoubleBattles:
	.string "TRAINER TIPS\p"
	.string "Trainer battles in HOENN are\n"
	.string "DOUBLE BATTLES. Prepare your first\l"
	.string "two partners together.\p"
	.string "A good opening pair gives each\n"
	.string "POKéMON room to do its job.$"
```

### `Route109_Text_TrainerTipsSign`

Source: [data/maps/Route109/scripts.inc:583](../baseline/source/data/maps/Route109/scripts.inc#L583)

Current: TRAINER TIPS //  POKéMON at the same level may not /  always have identical stats. //  POKéMON raised by TRAINERS are said /  to grow stronger than wild POKéMON.

Final text:

```asm
Route109_Text_TrainerTipsSign:
	.string "TRAINER TIPS\p"
	.string "Every partner has competitive\n"
	.string "potential. Battles do not award\l"
	.string "EVs to grind.\p"
	.string "Use a CENTER specialist to set\n"
	.string "Nature and STAT POINTS. Compare\l"
	.string "the resulting stats before battle.$"
```

### `Route104_Text_RouteSignPetalburg`

Source: [data/maps/Route104/scripts.inc:1046](../baseline/source/data/maps/Route104/scripts.inc#L1046)

Current: ROUTE 1O4 /  {RIGHT_ARROW} PETALBURG CITY

Final text:

```asm
Route104_Text_RouteSignPetalburg:
	.string "ROUTE 104 {RIGHT_ARROW} PETALBURG\n"
	.string "CITY$"
```

### `Route104_Text_RouteSignRustboro`

Source: [data/maps/Route104/scripts.inc:1050](../baseline/source/data/maps/Route104/scripts.inc#L1050)

Current: ROUTE 1O4 /  {UP_ARROW} RUSTBORO CITY

Final text:

```asm
Route104_Text_RouteSignRustboro:
	.string "ROUTE 104 {UP_ARROW} RUSTBORO CITY$"
```

Dependencies: INTRO-01; DIFF-01; GUIDE-01

Acceptance: Check party lead selection with a fainted first slot and two healthy partners. Display both route signs with dynamic roster appendix and verify no obsolete Hidden roster appears.

<a id="w-field-clues"></a>

## W-FIELD-CLUES — Explain the existing HM license and compatible-party model

**REPAIR.** The source already requires a badge, the received HM license, and a party member capable of learning the technique. Several NPCs still imply that the battle moveset must contain the HM.

### `RustboroCity_CuttersHouse_Text_ExplainCut`

Source: [data/maps/RustboroCity_CuttersHouse/scripts.inc:37](../baseline/source/data/maps/RustboroCity_CuttersHouse/scripts.inc#L37)

Current: That HM registers CUT as a field /  technique for your team. //  Once you earn the STONE BADGE, any /  POKéMON that knows CUT can chop thin /  trees outside battle.

Final text:

```asm
RustboroCity_CuttersHouse_Text_ExplainCut:
	.string "That HM registers CUT as a field\n"
	.string "technique for your team.\p"
	.string "With the STONE BADGE, a party\n"
	.string "member able to learn CUT can chop\l"
	.string "thin trees.\p"
	.string "It needn't keep CUT among its four\n"
	.string "battle moves.$"
```

### `RusturfTunnel_Text_ExplainStrength`

Source: [data/maps/RusturfTunnel/scripts.inc:516](../baseline/source/data/maps/RusturfTunnel/scripts.inc#L516)

Current: That HM contains STRENGTH. //  If a muscular POKéMON were to learn /  that, it would be able to move even /  large boulders.

Final text:

```asm
RusturfTunnel_Text_ExplainStrength:
	.string "That HM registers STRENGTH.\p"
	.string "With the HEAT BADGE, a party\n"
	.string "member able to learn STRENGTH can\l"
	.string "move large boulders.\p"
	.string "It needn't keep the move in its\n"
	.string "battle set.$"
```

### `Route118_Text_CanCrossRiversWithSurf`

Source: [data/maps/Route118/scripts.inc:308](../baseline/source/data/maps/Route118/scripts.inc#L308)

Current: Even if there isn't a boat, you can /  cross rivers and the sea if you have /  a POKéMON that knows SURF. //  POKéMON can be counted on to do so /  much!

Final text:

```asm
Route118_Text_CanCrossRiversWithSurf:
	.string "With SURF registered and the\n"
	.string "BALANCE BADGE, a compatible party\l"
	.string "member can cross water.\p"
	.string "It doesn't need SURF in its battle\n"
	.string "moveset. Partners can help in more\l"
	.string "ways than one!$"
```

### `RustboroCity_Flat2_2F_Text_PokemonHoldFloatStone`

Source: [data/maps/RustboroCity_Flat2_2F/scripts.inc:75](../baseline/source/data/maps/RustboroCity_Flat2_2F/scripts.inc#L75)

Current: A METAL COAT lets some POKéMON evolve /  when traded. DEVON refines them.

Final text:

```asm
RustboroCity_Flat2_2F_Text_PokemonHoldFloatStone:
	.string "DEVON refines METAL COATS.\p"
	.string "Use one from the BAG to evolve\n"
	.string "ONIX or SCYTHER. No trade with\l"
	.string "another player is required.$"
```

### `BattleFrontier_Lounge6_Text_PromiseIllBeGoodToIt`

Source: [data/maps/BattleFrontier_Lounge6/scripts.inc:38](../baseline/source/data/maps/BattleFrontier_Lounge6/scripts.inc#L38)

Current: Oh, it's adorable! /  Thank you! /  I promise I'll be good to it! //  {STR_VAR_2} comes with a complete /  battle set. Give it a memory disc /  and it will become SILVALLY.

Final text:

```asm
BattleFrontier_Lounge6_Text_PromiseIllBeGoodToIt:
	.string "Oh, it's adorable! Thank you! I\n"
	.string "promise I'll be good to it.\p"
	.string "TYPE: NULL already has a battle\n"
	.string "set. Raise its friendship, then\l"
	.string "level it up to get SILVALLY.\p"
	.string "A memory held by SILVALLY changes\n"
	.string "its type.$"
```

### `RustboroCity_PokemonSchool_Text_ScottMetAlreadyCut`

Source: [data/maps/RustboroCity_PokemonSchool/scripts.inc:293](../baseline/source/data/maps/RustboroCity_PokemonSchool/scripts.inc#L293)

Current: Hello? Didn't we meet before? /  I think back in PETALBURG CITY. //  Let me introduce myself. /  My name's SCOTT. //  I've been traveling everywhere in /  search of outstanding TRAINERS. //  More specifically, I'm looking for /  POKéMON battle experts. //  So, what brings you to this SCHOOL? /  Are you a TRAINER, too? //  The first thing you should do is to /  have a POKéMON learn the move CUT. //  If I remember correctly, someone in /  this town has CUT.

Final text:

```asm
RustboroCity_PokemonSchool_Text_ScottMetAlreadyCut:
	.string "SCOTT: We met in PETALBURG, didn't\n"
	.string "we? I'm looking for exceptional\l"
	.string "Trainers.\p"
	.string "The CUTTER nearby has the CUT HM.\n"
	.string "Collect it and the STONE BADGE.\p"
	.string "Then a compatible party member can\n"
	.string "clear thin trees without keeping\l"
	.string "CUT in its battle moves.$"
```

### `SlateportCity_Harbor_Text_TeamAquaLeftNeedDive`

Source: [data/maps/SlateportCity_Harbor/scripts.inc:473](../baseline/source/data/maps/SlateportCity_Harbor/scripts.inc#L473)

Current: CAPT. STERN: Oh, {PLAYER}{KUN}… //  Okay… So TEAM AQUA left before you /  could stop them… //  Oh, no, don't blame yourself. /  You're not responsible for this. //  Trying to catch a submarine… /  It's impossible for most people. //  You would need a POKéMON that knows /  how to DIVE… //  Perhaps if you went out to /  MOSSDEEP CITY… //  A lot of divers live out there, so /  someone might teach you…

Final text:

```asm
SlateportCity_Harbor_Text_TeamAquaLeftNeedDive:
	.string "CAPT. STERN: AQUA left before you\n"
	.string "could stop them... Don't blame\l"
	.string "yourself.\p"
	.string "To follow that submarine, you'll\n"
	.string "need DIVE registered, the MIND\l"
	.string "BADGE, and a partner able to use\l"
	.string "the technique.\p"
	.string "Travel to MOSSDEEP. Its divers and\n"
	.string "the SPACE CENTER may be able to\l"
	.string "help.$"
```

### `SlateportCity_Harbor_Text_NeedDiveToCatchSub`

Source: [data/maps/SlateportCity_Harbor/scripts.inc:488](../baseline/source/data/maps/SlateportCity_Harbor/scripts.inc#L488)

Current: CAPT. STERN: Trying to catch a /  submarine… It's impossible. //  You would need a POKéMON that knows /  how to DIVE… //  Perhaps if you went out to /  MOSSDEEP CITY… //  A lot of divers live out there, so /  someone might teach you…

Final text:

```asm
SlateportCity_Harbor_Text_NeedDiveToCatchSub:
	.string "CAPT. STERN: To follow the\n"
	.string "submarine, you'll need DIVE\l"
	.string "registered, the MIND BADGE, and a\l"
	.string "compatible partner.\p"
	.string "Visit MOSSDEEP's SPACE CENTER and\n"
	.string "find STEVEN. He may be able to\l"
	.string "help.$"
```

Dependencies: Evolution/acquisition catalogue; PREP-02; src/field_move.c; src/data/pokemon/species_info/gen_7_families.h

Acceptance: Perform Cut/Strength/Surf with compatible species whose four moves omit the HM; test missing badge, missing license, and no compatible species independently. Level Type: Null at friendship threshold; memory changes Silvally rather than evolving Type: Null.

<a id="w-field-surf"></a>

## W-FIELD-SURF — Use actual traversal capability when deleting Surf

**REPAIR.** IsLastMonThatKnowsSurf in src/party_menu.c checks only learned moves and can block reclaiming a move slot despite the compatible-species fallback. Preserve the softlock safeguard with the same field capability model.

### `IsLastMonThatKnowsSurf`

Source: [src/party_menu.c:8604](../baseline/source/src/party_menu.c#L8604)

Current: Rejects the last known Surf when no storage Pokémon knows Surf and P_CAN_FORGET_HIDDEN_MOVE is false.

Final behavior:

For Hoenn, when the selected move is Surf and the HM+badge license is unlocked, evaluate the party as if that slot no longer knew Surf: any other non-Egg member that knows Surf or any non-Egg member (including the selected member) whose species can learn Surf supplies a safe field user. If one exists, return FALSE. Otherwise retain the existing last-Surf safeguard. PC selection remains freely deletable as before. Do not mutate moves while checking, toggle global P_CAN_FORGET_HIDDEN_MOVE, or bypass HM/badge gates.

### `EmeraldChampions_EventScript_OpenMoveDeleter`

Source: [data/scripts/emerald_champions.inc:79](../baseline/source/data/scripts/emerald_champions.inc#L79)

Current: msgbox EmeraldChampions_Text_SpecialistForgetMove, MSGBOX_YESNO; goto_if_eq VAR_RESULT, NO, EmeraldChampions_EventScript_OtherServices

Final behavior:

KEEP this menu and its call to IsLastMonThatKnowsSurf; it receives corrected capability semantics.

### `LilycoveCity_MoveDeletersHouse_EventScript_MoveDeleter`

Source: [data/maps/LilycoveCity_MoveDeletersHouse/scripts.inc:4](../baseline/source/data/maps/LilycoveCity_MoveDeletersHouse/scripts.inc#L4)

Current: lockall; applymovement LOCALID_MOVE_DELETER, Common_Movement_FacePlayer; waitmovement 0; msgbox LilycoveCity_MoveDeletersHouse_Text_ICanMakeMonForgetMove, MSGBOX_YESNO; switch VAR_RESULT; case YES, LilycoveCity_MoveDeletersHouse_EventScript_ChooseMonAndMoveToForget; case NO, LilycoveCity_MoveDeletersHouse_EventScript_ComeAgain; releaseall; end

Final behavior:

KEEP this native specialist and its call to IsLastMonThatKnowsSurf; it receives corrected capability semantics.

Dependencies: src/field_move.c:SpeciesCanLearnFieldMove and IsFieldMoveUnlocked

Acceptance: A Surf-compatible Pokémon can delete its only learned Surf and still use Surf in the field. A species that only acquired Surf through an exceptional move-copy route retains protection if deleting it removes the last usable carrier. Test both specialist interfaces and cancellation; no source/game changes executed in book phase.

<a id="w-center-first-visit"></a>

## W-CENTER-FIRST-VISIT — Complete the first nurse visit with healing

**REPAIR.** The first nurse visit gives four useful tools and returns after explanation without the ordinary healing branch. A first visit to a nurse should offer the same immediate recovery as subsequent visits.

### `Common_EventScript_PkmnCenterNurse`

Source: [data/scripts/pkmn_center_nurse.inc:1](../baseline/source/data/scripts/pkmn_center_nurse.inc#L1)

Current: lock; faceplayer; checkitem ITEM_POKE_VIAL, 1; goto_if_eq VAR_RESULT, TRUE, EventScript_PkmnCenterNurse_HasVial; msgbox gText_EmeraldChampionsFirstCenterVisit, MSGBOX_DEFAULT; giveitem ITEM_POKE_VIAL, 1; goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull; setvar VAR_POKE_VIAL_MAX_CHARGES, 1; setvar VAR_POKE_VIAL_CHARGES, 1; msgbox gText_EmeraldChampionsGiveLeveler, MSGBOX_DEFAULT; giveitem ITEM_LEVELER, 1; goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull; msgbox gText_EmeraldChampionsGiveRepelSpray, MSGBOX_DEFAULT; giveitem ITEM_REPEL_SPRAY, 1; goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull; msgbox gText_EmeraldChampionsGiveFlightBeacon, MSGBOX_DEFAULT; giveitem ITEM_FLIGHT_BEACON, 1; goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull; message gText_EmeraldChampionsExplainTools; return

Final behavior:

Preserve all current item delivery and Bag-full retry branches. After the final ExplainTools message on first successful tool delivery, wait for its text and acknowledgment, then enter EventScript_PkmnCenterNurse_Main instead of returning. The ordinary Yes/No heal flow, vial refill, and pending legendary relic delivery execute exactly once if accepted. Do not repeat tool grants or suppress the first-visit explanation.

Dependencies: Shared nurse contract; INTRO-01

Acceptance: Enter any first Center with injured party. Finish tool handoff and accept healing in the same conversation; party and vial refill. Decline healing leaves items safely owned. Interrupt after each Bag-full delivery and return without duplication.

<a id="w-spiritomb-01"></a>

## W-SPIRITOMB-01 — Give the ship shrine a legible, thematic requirement

**REPAIR.** The active trash-can encounter silently requires Lickitung or Slugma as well as Odd Keystone. No map dialogue explains those species; ordinary Spiritomb routes already exist, so this is an optional thematic alternate.

### `AbandonedShip_Room_B1F_EventScript_Spiritomb`

Source: [data/maps/AbandonedShip_Room_B1F/scripts.inc:4](../baseline/source/data/maps/AbandonedShip_Room_B1F/scripts.inc#L4)

Current: lock; checkspecies SPECIES_LICKITUNG; goto_if_eq VAR_RESULT, TRUE, AbandonedShip_Room_B1F_EventScript_CheckOddKeystone; checkspecies SPECIES_SLUGMA; goto_if_eq VAR_RESULT, TRUE, AbandonedShip_Room_B1F_EventScript_CheckOddKeystone

Final behavior:

Remove both checkspecies branches. Lock and check ITEM_ODD_KEYSTONE directly. Without it, show the new eerie receptacle clue below, release and end. With it, retain existing placement Yes/No, species construction, cry, encounter and outcome handling. Consume exactly one Keystone only after B_OUTCOME_CAUGHT. Losing, fleeing, defeating Spiritomb or declining keeps the Keystone and allows another attempt. Preserve ordinary wild Spiritomb routes.

### `AbandonedShip_Room_B1F_Text_KeystoneMissing`

Source: [data/maps/AbandonedShip_Room_B1F/scripts.inc](../baseline/source/data/maps/AbandonedShip_Room_B1F/scripts.inc) — new entry

Final text:

```asm
AbandonedShip_Room_B1F_Text_KeystoneMissing:
	.string "Cold air seeps from the old bin.\p"
	.string "A cracked socket in its base looks\n"
	.string "made for an ODD KEYSTONE.$"
```

### `AbandonedShip_Room_B1F_Text_PutKeystoneInTrash`

Source: [data/maps/AbandonedShip_Room_B1F/scripts.inc:40](../baseline/source/data/maps/AbandonedShip_Room_B1F/scripts.inc#L40)

Current: Something compels you to place the /  ODD KEYSTONE in this trash can. //  Place the ODD KEYSTONE?

Final text:

```asm
AbandonedShip_Room_B1F_Text_PutKeystoneInTrash:
	.string "The ODD KEYSTONE fits the cracked\n"
	.string "socket in this old bin.\p"
	.string "A whisper rises from the hull. Set\n"
	.string "the stone in place?$"
```

Dependencies: Ordinary Spiritomb and Odd Keystone acquisition entries; wild/static battles remain singles

Acceptance: Interact without either former required species. Missing Keystone gives clue; declined/fled/defeated battle retains it; successful catch removes one. No unrelated item, flag, party member or ordinary encounter route is changed.

<a id="w-status-reference"></a>

## W-STATUS-REFERENCE — Make optional status explanations accurate for doubles

**REPAIR.** These optional field references should describe the configured game without absolute claims that would mislead an expert player. They remain available from the opening, never a staged skill unlock.

### `Route110_Text_TrainerTipsPrlzSleep`

Source: [data/maps/Route110/scripts.inc:1029](../baseline/source/data/maps/Route110/scripts.inc#L1029)

Current: TRAINER TIPS //  The foe can be made helpless by /  paralyzing it or causing it to sleep. //  It is an important technique for /  POKéMON battles.

Final text:

```asm
Route110_Text_TrainerTipsPrlzSleep:
	.string "TRAINER TIPS\p"
	.string "Paralysis usually lowers Speed and\n"
	.string "can interrupt a move.\p"
	.string "Sleep stops most moves until the\n"
	.string "Pokémon wakes. Abilities and\l"
	.string "terrain can change which plans\l"
	.string "work.$"
```

### `RustboroCity_PokemonSchool_Text_ExplainSleep`

Source: [data/maps/RustboroCity_PokemonSchool/scripts.inc:224](../baseline/source/data/maps/RustboroCity_PokemonSchool/scripts.inc#L224)

Current: If a POKéMON falls asleep, it will be /  unable to attack. //  A POKéMON may wake up on its own, /  but if a battle ends while it is /  sleeping, it will stay asleep. //  Wake it up using an AWAKENING.

Final text:

```asm
RustboroCity_PokemonSchool_Text_ExplainSleep:
	.string "Sleep prevents most moves until\n"
	.string "the Pokémon wakes.\p"
	.string "SLEEP TALK and SNORE are\n"
	.string "exceptions. A sleeping Pokémon\l"
	.string "stays asleep after battle;\l"
	.string "AWAKENING cures it.$"
```

### `RustboroCity_PokemonSchool_Text_ExplainBurn`

Source: [data/maps/RustboroCity_PokemonSchool/scripts.inc:232](../baseline/source/data/maps/RustboroCity_PokemonSchool/scripts.inc#L232)

Current: A burn reduces ATTACK power, and it /  steadily reduces the victim's HP. //  A burn lingers after battle. /  Cure a burn using a BURN HEAL.

Final text:

```asm
RustboroCity_PokemonSchool_Text_ExplainBurn:
	.string "A burn normally lowers damage from\n"
	.string "physical attacks and causes HP\l"
	.string "loss.\p"
	.string "Some Abilities change that result.\n"
	.string "GUTS attackers can benefit from\l"
	.string "burn!\p"
	.string "BURN HEAL removes a burn outside a\n"
	.string "trainer battle.$"
```

### `RustboroCity_PokemonSchool_Text_ExplainFreeze`

Source: [data/maps/RustboroCity_PokemonSchool/scripts.inc:238](../baseline/source/data/maps/RustboroCity_PokemonSchool/scripts.inc#L238)

Current: If a POKéMON is frozen, it becomes /  completely helpless. //  It will remain frozen after battle. /  Thaw it out using an ICE HEAL.

Final text:

```asm
RustboroCity_PokemonSchool_Text_ExplainFreeze:
	.string "A frozen Pokémon usually cannot\n"
	.string "move. It may thaw, and certain\l"
	.string "moves can help it thaw as well.\p"
	.string "Freezing remains after battle. An\n"
	.string "ICE HEAL removes it.$"
```

Dependencies: Configured status mechanics and individual battle/AI volume

Acceptance: Verify each stated interaction against actual configured production mechanics, including Sleep Talk/Snore and Guts. Render every paragraph without clipping. No new mechanics or difficulty easing.

<a id="w-petalburg-rooms"></a>

## W-PETALBURG-ROOMS — Make Norman’s room names describe the inspected teams

**REVISE.** The preserved teams no longer match the old Accuracy/Confusion/Defense names. Berke already has a Burst door but neighbors still direct players to One-hit KO. Names are coordinated with the early battle reviewer; maps, routes, identifiers, unlocking rules and geometry stay intact.

### `PetalburgCity_Gym_Text_EnterSpeedRoom`

Source: [data/maps/PetalburgCity_Gym/scripts.inc:1500](../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc#L1500)

Current: “SPEED ROOM,” the sign says. //  Do you want to go through?

Final text:

```asm
PetalburgCity_Gym_Text_EnterSpeedRoom:
	.string "\"MULTIHIT ROOM\", the sign says.\p"
	.string "Do you want to go through?$"
```

### `PetalburgCity_Gym_Text_EnterAccuracyRoom`

Source: [data/maps/PetalburgCity_Gym/scripts.inc:1509](../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc#L1509)

Current: “ACCURACY ROOM,” the sign says. //  Do you want to go through?

Final text:

```asm
PetalburgCity_Gym_Text_EnterAccuracyRoom:
	.string "\"RETALIATION ROOM\", the sign says.\p"
	.string "Do you want to go through?$"
```

### `PetalburgCity_Gym_Text_EnterConfusionRoom`

Source: [data/maps/PetalburgCity_Gym/scripts.inc:1513](../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc#L1513)

Current: “CONFUSION ROOM,” the sign says. //  Do you want to go through?

Final text:

```asm
PetalburgCity_Gym_Text_EnterConfusionRoom:
	.string "\"TRICK ROOM\", the sign says.\p"
	.string "Do you want to go through?$"
```

### `PetalburgCity_Gym_Text_EnterDefenseRoom`

Source: [data/maps/PetalburgCity_Gym/scripts.inc:1517](../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc#L1517)

Current: “DEFENSE ROOM,” the sign says. //  Do you want to go through?

Final text:

```asm
PetalburgCity_Gym_Text_EnterDefenseRoom:
	.string "\"STATUS ROOM\", the sign says.\p"
	.string "Do you want to go through?$"
```

### `PetalburgCity_Gym_Text_EnterRecoveryRoom`

Source: [data/maps/PetalburgCity_Gym/scripts.inc:1521](../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc#L1521)

Current: “RECOVERY ROOM,” the sign says. //  Do you want to go through?

Final text:

```asm
PetalburgCity_Gym_Text_EnterRecoveryRoom:
	.string "\"ENDURANCE ROOM\", the sign says.\p"
	.string "Do you want to go through?$"
```

### `PetalburgCity_Gym_Text_EnterStrengthRoom`

Source: [data/maps/PetalburgCity_Gym/scripts.inc:1525](../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc#L1525)

Current: “STRENGTH ROOM,” the sign says. //  Do you want to go through?

Final text:

```asm
PetalburgCity_Gym_Text_EnterStrengthRoom:
	.string "\"POWER ROOM\", the sign says.\p"
	.string "Do you want to go through?$"
```

### `PetalburgCity_Gym_Text_EnterOHKORoom`

Source: [data/maps/PetalburgCity_Gym/scripts.inc:1529](../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc#L1529)

Current: “BURST ROOM,” the sign says. //  Do you want to go through?

Final text:

```asm
PetalburgCity_Gym_Text_EnterOHKORoom:
	.string "\"BURST ROOM\", the sign says.\p"
	.string "Do you want to go through?$"
```

### `PetalburgCity_Gym_Text_GymGuideAdvice`

Source: [data/maps/PetalburgCity_Gym/scripts.inc:1319](../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc#L1319)

Current: Hey, how's it going, CHAMPION- /  bound {PLAYER}? //  The doors in this GYM open when you /  beat the awaiting TRAINERS. //  Whoops! The doors in this room are /  already open, so don't attack me! //  The TRAINERS of PETALBURG GYM /  use all kinds of items. //  The door at the left leads to /  the SPEED ROOM. //  The door at the right leads to /  the ACCURACY ROOM. //  The room's name will be on /  the door, so choose carefully. //  Once you've chosen the door… /  Well, hey, go for it!

Final text:

```asm
PetalburgCity_Gym_Text_GymGuideAdvice:
	.string "Welcome, CHAMPION-bound {PLAYER}!\p"
	.string "Defeat the Trainers in a room to\n"
	.string "open its onward doors.\p"
	.string "The left door leads to the\n"
	.string "MULTIHIT ROOM. The right leads to\l"
	.string "the RETALIATION ROOM.\p"
	.string "Each room's sign names its plan.\n"
	.string "Choose your route and prepare your\l"
	.string "partners together.$"
```

### `PetalburgCity_Gym_Text_MaryPostBattle`

Source: [data/maps/PetalburgCity_Gym/scripts.inc:1438](../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc#L1438)

Current: There are some even stronger /  TRAINERS waiting for you. //  The left is the DEFENSE ROOM, and /  the right is the RECOVERY ROOM. //  Your POKéMON's ATTACK power will be /  on trial either way.

Final text:

```asm
PetalburgCity_Gym_Text_MaryPostBattle:
	.string "More formidable teams await.\p"
	.string "The left door leads to the STATUS\n"
	.string "ROOM; the right leads to the\l"
	.string "ENDURANCE ROOM.$"
```

### `PetalburgCity_Gym_Text_RandallPostBattle`

Source: [data/maps/PetalburgCity_Gym/scripts.inc:1353](../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc#L1353)

Current: Go on to the next room where a new /  challenge awaits you. //  At the left is the CONFUSION ROOM. //  The right door leads to the DEFENSE /  ROOM.

Final text:

```asm
PetalburgCity_Gym_Text_RandallPostBattle:
	.string "Another challenge waits ahead.\p"
	.string "The left door leads to the TRICK\n"
	.string "ROOM; the right leads to the\l"
	.string "STATUS ROOM.$"
```

### `PetalburgCity_Gym_Text_ParkerPostBattle`

Source: [data/maps/PetalburgCity_Gym/scripts.inc:1377](../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc#L1377)

Current: The next room is the STRENGTH ROOM. /  Can you withstand brute force?

Final text:

```asm
PetalburgCity_Gym_Text_ParkerPostBattle:
	.string "The next door leads to the POWER\n"
	.string "ROOM.\p"
	.string "Keep your team ready for it!$"
```

### `PetalburgCity_Gym_Text_AlexiaPostBattle`

Source: [data/maps/PetalburgCity_Gym/scripts.inc:1460](../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc#L1460)

Current: I think you've taught me a valuable /  lesson here. //  Now, go on! The left door goes to /  the STRENGTH ROOM. //  The right door opens to /  the ONE-HIT KO ROOM. //  Both of them have TRAINERS who are /  skilled at offense.

Final text:

```asm
PetalburgCity_Gym_Text_AlexiaPostBattle:
	.string "You've given me something to think\n"
	.string "about.\p"
	.string "The left door leads to the POWER\n"
	.string "ROOM; the right leads to the BURST\l"
	.string "ROOM.$"
```

### `PetalburgCity_Gym_Text_GeorgePostBattle`

Source: [data/maps/PetalburgCity_Gym/scripts.inc:1397](../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc#L1397)

Current: I should have expected no less from /  our LEADER's kid. //  No, wait! A TRAINER's abilities are /  earned only through effort. //  I lost because my own efforts /  weren't enough. //  Go on! The ONE-HIT KO ROOM is next.

Final text:

```asm
PetalburgCity_Gym_Text_GeorgePostBattle:
	.string "That was well played.\p"
	.string "The next door leads to the BURST\n"
	.string "ROOM. Be ready for its\l"
	.string "concentrated pressure.$"
```

Dependencies: Early battle volume: Randall MULTIHIT, Mary RETALIATION, Parker TRICK, Alexia STATUS, George ENDURANCE, Jody POWER, Berke BURST; intro paragraphs owned there

Acceptance: Walk both branching routes and compare each door name to its destination and neighbor directions. Defeating a room still opens exactly its existing onward doors. No encounter is added or removed.

<a id="w-wallace-root"></a>

## W-WALLACE-ROOT — Restore the authored Wallace exhibition as an optional chamber encounter

**REPAIR.** The complete E0510 team/script remains authored but has no object/coordinate/background entry. Restore this one named authored exhibition deliberately; do not revive every retired script.

### `new LOCALID_EC_WALLACE_EXHIBITION`

Source: [data/maps/CaveOfOrigin_DianciesRoom/map.json](../baseline/source/data/maps/CaveOfOrigin_DianciesRoom/map.json) — new entry

Current: Only Diancie at (9,9) and Diancite at (7,9) are present.

Final behavior:

Wallace is visible whenever this chamber is reachable. Before game clear he declines. After game clear, interaction offers the existing one-time exhibition. He never auto-approaches or blocks Diancie, its stone, or the north exit. Preserve current map-local IDs and append the new ID.

Final object:

```json
{
  "local_id": "LOCALID_EC_WALLACE_EXHIBITION",
  "graphics_id": "OBJ_EVENT_GFX_WALLACE",
  "x": 11,
  "y": 10,
  "elevation": 3,
  "movement_type": "MOVEMENT_TYPE_FACE_LEFT",
  "movement_range_x": 0,
  "movement_range_y": 0,
  "trainer_type": "TRAINER_TYPE_NONE",
  "trainer_sight_or_berry_tree_id": "0",
  "script": "CaveOfOrigin_DianciesRoom_EventScript_WallaceExhibition",
  "flag": "0"
}
```

### `CaveOfOrigin_DianciesRoom_EventScript_WallaceExhibition`

Source: [data/maps/CaveOfOrigin_DianciesRoom/scripts.inc:4](../baseline/source/data/maps/CaveOfOrigin_DianciesRoom/scripts.inc#L4)

Current: lock; faceplayer; goto_if_unset FLAG_SYS_GAME_CLEAR, CaveOfOrigin_DianciesRoom_EventScript_WallaceNotReady; goto_if_defeated TRAINER_WALLACE_DOUBLES_LEGENDS, CaveOfOrigin_DianciesRoom_EventScript_WallaceReward; trainerbattle_double TRAINER_WALLACE_DOUBLES_LEGENDS, CaveOfOrigin_DianciesRoom_Text_WallaceIntro, CaveOfOrigin_DianciesRoom_Text_WallaceDefeat, EmeraldChampions_Text_NeedTwoPokemon

Final behavior:

lock
faceplayer
goto_if_unset FLAG_SYS_GAME_CLEAR, CaveOfOrigin_DianciesRoom_EventScript_WallaceNotReady
goto_if_defeated TRAINER_WALLACE_DOUBLES_LEGENDS, CaveOfOrigin_DianciesRoom_EventScript_WallaceAfter
msgbox CaveOfOrigin_DianciesRoom_Text_WallaceOffer, MSGBOX_YESNO
goto_if_eq VAR_RESULT, NO, CaveOfOrigin_DianciesRoom_EventScript_WallaceDecline
call EventScript_CheckStoryDoubleBattleParty
trainerbattle_double TRAINER_WALLACE_DOUBLES_LEGENDS, CaveOfOrigin_DianciesRoom_Text_WallaceIntro, CaveOfOrigin_DianciesRoom_Text_WallaceDefeat, EmeraldChampions_Text_NeedTwoPokemon
goto CaveOfOrigin_DianciesRoom_EventScript_WallaceAfter

### `CaveOfOrigin_DianciesRoom_EventScript_WallaceReward`

Source: [data/maps/CaveOfOrigin_DianciesRoom/scripts.inc:11](../baseline/source/data/maps/CaveOfOrigin_DianciesRoom/scripts.inc#L11)

Current: checkitem ITEM_DIANCITE, 1; goto_if_eq VAR_RESULT, TRUE, CaveOfOrigin_DianciesRoom_EventScript_WallaceAfter; giveitem ITEM_DIANCITE; goto_if_eq VAR_RESULT, FALSE, CaveOfOrigin_DianciesRoom_EventScript_WallaceBagFull

Final behavior:

Retire the duplicate Diancite handoff path. After a legitimate win, go directly to WallaceAfter; retain the trainer defeated flag. The chamber pickup remains the Diancite acquisition owner.

### `CaveOfOrigin_DianciesRoom_Text_WallaceAfter`

Source: [data/maps/CaveOfOrigin_DianciesRoom/scripts.inc:41](../baseline/source/data/maps/CaveOfOrigin_DianciesRoom/scripts.inc#L41)

Current: Take this DIANCITE. Let the jewel of /  this cave shine as brightly as you.

Final text:

```asm
CaveOfOrigin_DianciesRoom_Text_WallaceAfter:
	.string "The chamber has heard two\n"
	.string "Champions answer one another.\p"
	.string "The DIANCITE beside its guardian\n"
	.string "belongs to your exploration. Our\l"
	.string "battle was its own reward.$"
```

### `CaveOfOrigin_DianciesRoom_Text_WallaceOffer`

Source: [data/maps/CaveOfOrigin_DianciesRoom/scripts.inc](../baseline/source/data/maps/CaveOfOrigin_DianciesRoom/scripts.inc) — new entry

Final text:

```asm
CaveOfOrigin_DianciesRoom_Text_WallaceOffer:
	.string "Would you face my unrestricted\n"
	.string "rain exhibition here?\p"
	.string "The chamber's guardian and its\n"
	.string "stone will wait while we battle.$"
```

### `CaveOfOrigin_DianciesRoom_EventScript_WallaceDecline`

Source: [data/maps/CaveOfOrigin_DianciesRoom/scripts.inc](../baseline/source/data/maps/CaveOfOrigin_DianciesRoom/scripts.inc) — new entry

Final behavior:

msgbox CaveOfOrigin_DianciesRoom_Text_WallaceDecline, MSGBOX_DEFAULT; release; end

### `CaveOfOrigin_DianciesRoom_Text_WallaceDecline`

Source: [data/maps/CaveOfOrigin_DianciesRoom/scripts.inc](../baseline/source/data/maps/CaveOfOrigin_DianciesRoom/scripts.inc) — new entry

Final text:

```asm
CaveOfOrigin_DianciesRoom_Text_WallaceDecline:
	.string "Whenever your team is ready. I\n"
	.string "will be here.$"
```

Dependencies: E0510 final team and defeat prose owned by late battle volume; Diancite world pickup; no native trainer singles

Acceptance: Map.bin tile (11,10) is passable elevation3 in snapshot; render and walk around Wallace from each reachable side. Test pre-clear, decline, insufficient usable party, loss/Retry, first win, and revisit. Diancie remains independently interactable and Diancite is not reissued by battle.

<a id="w-trick-rewards"></a>

## W-TRICK-REWARDS — Make all Trick House reward retries preserve their earned contents

**REPAIR.** Puzzle4 changes its prize on the full-Bag route. Final puzzle8 collapses tent and Alakazite failures into one flag, while the entrance retries only the tent. Preserve both authored final rewards and support each independently.

### `Route110_TrickHouseEntrance_EventScript_GivePuzzle4Reward`

Source: [data/maps/Route110_TrickHouseEntrance/scripts.inc:362](../baseline/source/data/maps/Route110_TrickHouseEntrance/scripts.inc#L362)

Current: giveitem ITEM_SMOKE_BALL; goto_if_eq VAR_RESULT, TRUE, Route110_TrickHouseEntrance_EventScript_GotReward; call_if_eq VAR_RESULT, FALSE, Common_EventScript_BagIsFull; msgbox Route110_TrickHouseEntrance_Text_DidYouNotComeToClaimReward, MSGBOX_DEFAULT; releaseall; end

Final behavior:

Change only giveitem ITEM_SMOKE_BALL to giveitem ITEM_KINGS_ROCK. Preserve result branches and the remaining stage rewards.

### `Route110_TrickHouseEnd_EventScript_CompletedPuzzle8`

Source: [data/maps/Route110_TrickHouseEnd/scripts.inc:142](../baseline/source/data/maps/Route110_TrickHouseEnd/scripts.inc#L142)

Current: msgbox Route110_TrickHouseEnd_Text_AllNightPolishingFloors, MSGBOX_DEFAULT; closemessage; call_if_eq VAR_FACING, DIR_SOUTH, Route110_TrickHouseEnd_EventScript_TrickMasterFaceAwaySouth; call_if_eq VAR_FACING, DIR_NORTH, Route110_TrickHouseEnd_EventScript_TrickMasterFaceAwayNorth; call_if_eq VAR_FACING, DIR_WEST, Route110_TrickHouseEnd_EventScript_TrickMasterFaceAwayWest; call_if_eq VAR_FACING, DIR_EAST, Route110_TrickHouseEnd_EventScript_TrickMasterFaceAwayEast; delay 30; msgbox Route110_TrickHouseEnd_Text_FountainOfIdeasRunDry, MSGBOX_DEFAULT; closemessage; applymovement LOCALID_TRICK_MASTER_END, Common_Movement_FacePlayer; waitmovement 0; delay 30; msgbox Route110_TrickHouseEnd_Text_DefeatedMePreferWhichTent, MSGBOX_DEFAULT; setvar VAR_TRICK_HOUSE_PRIZE_PICKUP, 0; call Route110_TrickHouseEnd_EventScript_ChooseTent; call_if_eq VAR_RESULT, FALSE, Route110_TrickHouseEnd_EventScript_NoRoomForTent; msgbox Route110_TrickHouseEnd_Text_FinalPrizeMegaStone, MSGBOX_DEFAULT; giveitem ITEM_ALAKAZITE; call_if_eq VAR_RESULT, FALSE, Route110_TrickHouseEnd_EventScript_BagFull; msgbox Route110_TrickHouseEnd_Text_LeavingOnJourney, MSGBOX_DEFAULT; call Route110_TrickHouseEnd_EventScript_TrickMasterExit; special ResetTrickHouseNuggetFlag; release; end

Final behavior:

Keep final story, chosen RED/BLUE TENT, Alakazite, departure and level advancement. Add separate receipt flags FLAG_EC_TRICK_FINAL_TENT_RECEIVED and FLAG_EC_TRICK_FINAL_ALAKAZITE_RECEIVED. A successful givedecoration sets only tent receipt; a successful giveitem sets only Alakazite receipt. Failure of either leaves that receipt false. At level8 both are owed until individually received; VAR_TRICK_HOUSE_PRIZE_PICKUP becomes1 whenever either receipt is false,0 only when both true. Completing one must not clear the other. Share this claim operation with the entrance courier; do not replay puzzle8 to claim a pending part.

### `Route110_TrickHouseEntrance_EventScript_MechadollReward`

Source: [data/maps/Route110_TrickHouseEntrance/scripts.inc:402](../baseline/source/data/maps/Route110_TrickHouseEntrance/scripts.inc#L402)

Current: applymovement LOCALID_TRICK_MASTER, Common_Movement_FacePlayer; waitmovement 0; msgbox Route110_TrickHouseEntrance_Text_MechadollWhichTent, MSGBOX_DEFAULT; call Route110_TrickHouseEntrance_EventScript_ChooseTent; goto_if_eq VAR_RESULT, TRUE, Route110_TrickHouseEntrance_EventScript_ReceivedTent; call_if_eq VAR_RESULT, FALSE, Common_EventScript_NoRoomForDecor; msgbox Route110_TrickHouseEntrance_Text_PCFullAgain, MSGBOX_DEFAULT; releaseall; end

Final behavior:

Offer the tent choice only if its receipt is false, and Alakazite only if its receipt is false. After each insertion, mark only the successful part received. Keep the mechadoll visible whenever anything remains pending. When both receipts are true, clear PRIZE_PICKUP, retain LEVEL=8, and use the existing departure effect exactly once. Do not issue another tent just because Alakazite failed.

### `Route110_TrickHouseEntrance_OnTransition`

Source: [data/maps/Route110_TrickHouseEntrance/scripts.inc:16](../baseline/source/data/maps/Route110_TrickHouseEntrance/scripts.inc#L16)

Current: setflag FLAG_LANDMARK_TRICK_HOUSE; goto_if_eq VAR_TRICK_HOUSE_ENTER_FROM_CORRIDOR, 1, Route110_TrickHouseEntrance_EventScript_EnterFromCorridor; goto_if_eq VAR_TRICK_HOUSE_PRIZE_PICKUP, 1, Route110_TrickHouseEntrance_EventScript_SetReadyToGiveReward; goto_if_eq VAR_TRICK_HOUSE_FOUND_TRICK_MASTER, 1, Route110_TrickHouseEntrance_EventScript_MoveTrickMasterToDoor; call_if_eq VAR_TRICK_HOUSE_ENTRANCE_STATE, 5, Route110_TrickHouseEntrance_EventScript_CheckReadyForNextPuzzle; call_if_eq VAR_TRICK_HOUSE_ENTRANCE_STATE, 3, Route110_TrickHouseEntrance_EventScript_CheckReadyForNextPuzzle; call_if_eq VAR_TRICK_HOUSE_ENTRANCE_STATE, 0, Route110_TrickHouseEntrance_EventScript_CheckReadyForNextPuzzle; switch VAR_TRICK_HOUSE_ENTRANCE_STATE; case 0, Route110_TrickHouseEntrance_EventScript_ReadyBeingWatchedTrigger; case 1, Route110_TrickHouseEntrance_EventScript_SetNotBeingWatched1; case 3, Route110_TrickHouseEntrance_EventScript_SetNotBeingWatched2; case 4, Route110_TrickHouseEntrance_EventScript_SetNotBeingWatched3; end

Final behavior:

Before existing final-prize visibility selection, run one-time receipt initialization guarded by new FLAG_EC_TRICK_FINAL_STATE_INITIALIZED. If LEVEL<8, initialize marker only. For an old completed LEVEL>=8 and PRIZE_PICKUP=0, mark both receipts received. For old LEVEL>=8 and PRIZE_PICKUP=1, mark a tent received only if RED/BLUE TENT ownership is recorded (including placed decorations); mark Alakazite received if already in Bag, item PC or on any party/PC Pokémon. Otherwise preserve that part as a one-time claim. Then set the initialization marker and derive final PRIZE_PICKUP from the two receipts. Never reset puzzle progress, catches, other Mega flags or owned decorations. The old one-bit state cannot prove which reward was lost after disposal; record that migration limitation openly.

### `new symbolic Trick House receipt flags`

Source: [include/constants/flags.h](../baseline/source/include/constants/flags.h) — new entry

Final behavior:

Use the central reservations in appendices/state-allocation.md and review/state-allocations.json: FLAG_EC_TRICK_FINAL_TENT_RECEIVED=0x2B2, FLAG_EC_TRICK_FINAL_ALAKAZITE_RECEIVED=0x2B3, FLAG_EC_TRICK_FINAL_STATE_INITIALIZED=0x2B4. They replace declaration-only unused flags. The single version4 migration initializes them once, then reconstructs legacy final-prize state; later loads never clear the initializer or receipts. Recheck live-source collisions before applying these reserved addresses.

### `Route110_TrickHouseEntrance_Text_FinalRewardsPending`

Source: [data/maps/Route110_TrickHouseEntrance/scripts.inc](../baseline/source/data/maps/Route110_TrickHouseEntrance/scripts.inc) — new entry

Final text:

```asm
Route110_TrickHouseEntrance_Text_FinalRewardsPending:
	.string "YOUR FINAL REWARDS ARE RECORDED.\p"
	.string "THE TENT AND ALAKAZITE ARE\n"
	.string "SEPARATE GIFTS. MAKE ROOM FOR ANY\l"
	.string "PART STILL WAITING HERE.$"
```

Dependencies: Mega reward catalogue; persistent flag allocation; existing Alakazite Granite Cave alternate preserved

Acceptance: For stages1–7 compare immediate and pending item IDs; puzzle4 always King’s Rock. For stage8 exercise all four tent/stone insertion success combinations, reload/revisit, claim one part at a time, and verify no duplication. Test old completed saves with no pending state, tent-only ownership, stone-only ownership, both, neither; preserve unrelated inventory and puzzle flags.

<a id="w-trick-quiz"></a>

## W-TRICK-QUIZ — Reauthor all fifteen quiz questions against actual battle rules

**REPAIR.** The current random quiz bank asks obsolete wild-table, prior-team, price and NPC-count questions. Retain five dolls, three randomly chosen questions per doll, movement/incorrect-answer reset and reward topology, with a complete stable doubles question bank. This optional puzzle is not a campaign training curriculum.

### `Route110_TrickHousePuzzle5_Text_Mechadoll1Quiz1`

Source: [data/maps/Route110_TrickHousePuzzle5/scripts.inc:912](../baseline/source/data/maps/Route110_TrickHousePuzzle5/scripts.inc#L912)

Current: MECHADOLL 1 QUIZ. //  One of these POKéMON is not found /  on ROUTE 110. Which one is it?

Final text:

```asm
Route110_TrickHousePuzzle5_Text_Mechadoll1Quiz1:
	.string "MECHADOLL 1 QUIZ.\p"
	.string "Which move guards your side\n"
	.string "against opposing priority moves?$"
```

Choices in `MultichoiceList_Mechadoll1_Q1` (`src/data/script_menu.h`), zero-based: 0: PROTECT, 1: WIDE GUARD, 2: QUICK GUARD. Correct index: **2**. Update `Route110_TrickHousePuzzle5_EventScript_Mechadoll1Quiz1` to that case. Evidence: `src/battle_util.c:5915`.

### `Route110_TrickHousePuzzle5_Text_Mechadoll1Quiz2`

Source: [data/maps/Route110_TrickHousePuzzle5/scripts.inc:917](../baseline/source/data/maps/Route110_TrickHousePuzzle5/scripts.inc#L917)

Current: MECHADOLL 1 QUIZ. //  One of these POKéMON is not of the /  WATER type. Which one is it?

Final text:

```asm
Route110_TrickHousePuzzle5_Text_Mechadoll1Quiz2:
	.string "MECHADOLL 1 QUIZ.\p"
	.string "Which type normally ignores RAGE\n"
	.string "POWDER when choosing a target?$"
```

Choices in `MultichoiceList_Mechadoll1_Q2` (`src/data/script_menu.h`), zero-based: 0: GRASS, 1: NORMAL, 2: ROCK. Correct index: **0**. Update `Route110_TrickHousePuzzle5_EventScript_Mechadoll1Quiz2` to that case. Evidence: `src/battle_util.c; powder/redirection target checks`.

### `Route110_TrickHousePuzzle5_Text_Mechadoll1Quiz3`

Source: [data/maps/Route110_TrickHousePuzzle5/scripts.inc:922](../baseline/source/data/maps/Route110_TrickHousePuzzle5/scripts.inc#L922)

Current: MECHADOLL 1 QUIZ. //  One of these POKéMON does not use /  LEECH LIFE. Which one is it?

Final text:

```asm
Route110_TrickHousePuzzle5_Text_Mechadoll1Quiz3:
	.string "MECHADOLL 1 QUIZ.\p"
	.string "Which move normally hits both\n"
	.string "opponents instead of being\l"
	.string "redirected by FOLLOW ME?$"
```

Choices in `MultichoiceList_Mechadoll1_Q3` (`src/data/script_menu.h`), zero-based: 0: ROCK SLIDE, 1: THUNDERBOLT, 2: CLOSE COMBAT. Correct index: **0**. Update `Route110_TrickHousePuzzle5_EventScript_Mechadoll1Quiz3` to that case. Evidence: `src/data/moves_info.h; MOVE_ROCK_SLIDE target flags; redirection in src/battle_util.c`.

### `Route110_TrickHousePuzzle5_Text_Mechadoll2Quiz1`

Source: [data/maps/Route110_TrickHousePuzzle5/scripts.inc:948](../baseline/source/data/maps/Route110_TrickHousePuzzle5/scripts.inc#L948)

Current: MECHADOLL 2 QUIZ. //  Which of these POKéMON did WALLY /  borrow from your father?

Final text:

```asm
Route110_TrickHousePuzzle5_Text_Mechadoll2Quiz1:
	.string "MECHADOLL 2 QUIZ.\p"
	.string "TRICK ROOM reverses Speed order\n"
	.string "within which boundary?$"
```

Choices in `MultichoiceList_Mechadoll2_Q1` (`src/data/script_menu.h`), zero-based: 0: The whole turn, 1: Each priority bracket, 2: Only one side. Correct index: **1**. Update `Route110_TrickHousePuzzle5_EventScript_Mechadoll2Quiz1` to that case. Evidence: `src/battle_main.c:GetWhoStrikesFirst / Trick Room comparisons`.

### `Route110_TrickHousePuzzle5_Text_Mechadoll2Quiz2`

Source: [data/maps/Route110_TrickHousePuzzle5/scripts.inc:953](../baseline/source/data/maps/Route110_TrickHousePuzzle5/scripts.inc#L953)

Current: MECHADOLL 2 QUIZ. //  Which of these POKéMON was chasing /  PROF. BIRCH?

Final text:

```asm
Route110_TrickHousePuzzle5_Text_Mechadoll2Quiz2:
	.string "MECHADOLL 2 QUIZ.\p"
	.string "When can a Pokémon normally use\n"
	.string "FAKE OUT successfully?$"
```

Choices in `MultichoiceList_Mechadoll2_Q2` (`src/data/script_menu.h`), zero-based: 0: Only at full HP, 1: After every Protect, 2: First turn after entry. Correct index: **2**. Update `Route110_TrickHousePuzzle5_EventScript_Mechadoll2Quiz2` to that case. Evidence: `data/battle_scripts_1.s:BattleScript_EffectFirstTurnOnly; first-turn counter`.

### `Route110_TrickHousePuzzle5_Text_Mechadoll2Quiz3`

Source: [data/maps/Route110_TrickHousePuzzle5/scripts.inc:958](../baseline/source/data/maps/Route110_TrickHousePuzzle5/scripts.inc#L958)

Current: MECHADOLL 2 QUIZ. //  Which of these POKéMON did TEAM AQUA /  use in PETALBURG FOREST?

Final text:

```asm
Route110_TrickHousePuzzle5_Text_Mechadoll2Quiz3:
	.string "MECHADOLL 2 QUIZ.\p"
	.string "TAUNT prevents the target from\n"
	.string "choosing which moves?$"
```

Choices in `MultichoiceList_Mechadoll2_Q3` (`src/data/script_menu.h`), zero-based: 0: Non-damaging moves, 1: All special attacks, 2: All priority moves. Correct index: **0**. Update `Route110_TrickHousePuzzle5_EventScript_Mechadoll2Quiz3` to that case. Evidence: `src/battle_util.c:CheckMoveLimitations; tauntTimer`.

### `Route110_TrickHousePuzzle5_Text_Mechadoll3Quiz1`

Source: [data/maps/Route110_TrickHousePuzzle5/scripts.inc:968](../baseline/source/data/maps/Route110_TrickHousePuzzle5/scripts.inc#L968)

Current: MECHADOLL 3 QUIZ. //  Which costs more? /  Three HARBOR MAILS or one BURN HEAL?

Final text:

```asm
Route110_TrickHousePuzzle5_Text_Mechadoll3Quiz1:
	.string "MECHADOLL 3 QUIZ.\p"
	.string "Which status reduces physical\n"
	.string "damage for most attackers?$"
```

Choices in `MultichoiceList_Mechadoll3_Q1` (`src/data/script_menu.h`), zero-based: 0: BURN, 1: POISON, 2: SLEEP. Correct index: **0**. Update `Route110_TrickHousePuzzle5_EventScript_Mechadoll3Quiz1` to that case. Evidence: `src/battle_util.c physical damage modifier and Guts exception`.

### `Route110_TrickHousePuzzle5_Text_Mechadoll3Quiz2`

Source: [data/maps/Route110_TrickHousePuzzle5/scripts.inc:973](../baseline/source/data/maps/Route110_TrickHousePuzzle5/scripts.inc#L973)

Current: MECHADOLL 3 QUIZ. //  Sell one GREAT BALL and buy /  one POTION. How much money remains?

Final text:

```asm
Route110_TrickHousePuzzle5_Text_Mechadoll3Quiz2:
	.string "MECHADOLL 3 QUIZ.\p"
	.string "Which move boosts an ally's attack\n"
	.string "damage for the current turn?$"
```

Choices in `MultichoiceList_Mechadoll3_Q2` (`src/data/script_menu.h`), zero-based: 0: REFLECT, 1: SAFEGUARD, 2: HELPING HAND. Correct index: **2**. Update `Route110_TrickHousePuzzle5_EventScript_Mechadoll3Quiz2` to that case. Evidence: `src/data/moves_info.h:MOVE_HELPING_HAND; damage modifier`.

### `Route110_TrickHousePuzzle5_Text_Mechadoll3Quiz3`

Source: [data/maps/Route110_TrickHousePuzzle5/scripts.inc:978](../baseline/source/data/maps/Route110_TrickHousePuzzle5/scripts.inc#L978)

Current: MECHADOLL 3 QUIZ. //  Do one REPEL and SODA POP cost /  more than one SUPER POTION?

Final text:

```asm
Route110_TrickHousePuzzle5_Text_Mechadoll3Quiz3:
	.string "MECHADOLL 3 QUIZ.\p"
	.string "Which Pokémon can receive\n"
	.string "EVIOLITE's defensive benefit?$"
```

Choices in `MultichoiceList_Mechadoll3_Q3` (`src/data/script_menu.h`), zero-based: 0: One with no evolution, 1: One that can still evolve, 2: Any below level 50. Correct index: **1**. Update `Route110_TrickHousePuzzle5_EventScript_Mechadoll3Quiz3` to that case. Evidence: `src/battle_util.c:7291; CanSpeciesEvolve`.

### `Route110_TrickHousePuzzle5_Text_Mechadoll4Quiz1`

Source: [data/maps/Route110_TrickHousePuzzle5/scripts.inc:988](../baseline/source/data/maps/Route110_TrickHousePuzzle5/scripts.inc#L988)

Current: MECHADOLL 4 QUIZ. //  In SEASHORE HOUSE, were there more men /  or women?

Final text:

```asm
Route110_TrickHousePuzzle5_Text_Mechadoll4Quiz1:
	.string "MECHADOLL 4 QUIZ.\p"
	.string "Which held item can remove TAUNT\n"
	.string "from its holder?$"
```

Choices in `MultichoiceList_Mechadoll4_Q1` (`src/data/script_menu.h`), zero-based: 0: MENTAL HERB, 1: POWER HERB, 2: WHITE HERB. Correct index: **0**. Update `Route110_TrickHousePuzzle5_EventScript_Mechadoll4Quiz1` to that case. Evidence: `src/battle_script_commands.c:902; mental-herb status handling`.

### `Route110_TrickHousePuzzle5_Text_Mechadoll4Quiz2`

Source: [data/maps/Route110_TrickHousePuzzle5/scripts.inc:993](../baseline/source/data/maps/Route110_TrickHousePuzzle5/scripts.inc#L993)

Current: MECHADOLL 4 QUIZ. //  In LAVARIDGE TOWN, were there more /  elderly men or elderly women?

Final text:

```asm
Route110_TrickHousePuzzle5_Text_Mechadoll4Quiz2:
	.string "MECHADOLL 4 QUIZ.\p"
	.string "Which Ability redirects\n"
	.string "single-target WATER moves?$"
```

Choices in `MultichoiceList_Mechadoll4_Q2` (`src/data/script_menu.h`), zero-based: 0: STORM DRAIN, 1: WATER ABSORB, 2: RAIN DISH. Correct index: **0**. Update `Route110_TrickHousePuzzle5_EventScript_Mechadoll4Quiz2` to that case. Evidence: `src/battle_util.c:2358 and redirection handling`.

### `Route110_TrickHousePuzzle5_Text_Mechadoll4Quiz3`

Source: [data/maps/Route110_TrickHousePuzzle5/scripts.inc:998](../baseline/source/data/maps/Route110_TrickHousePuzzle5/scripts.inc#L998)

Current: MECHADOLL 4 QUIZ. //  In the TRAINER'S SCHOOL, how many /  girl students were there?

Final text:

```asm
Route110_TrickHousePuzzle5_Text_Mechadoll4Quiz3:
	.string "MECHADOLL 4 QUIZ.\p"
	.string "Which opposing target does PSYCHIC\n"
	.string "TERRAIN protect from FAKE OUT?$"
```

Choices in `MultichoiceList_Mechadoll4_Q3` (`src/data/script_menu.h`), zero-based: 0: An airborne target, 1: A grounded target, 2: Every target. Correct index: **1**. Update `Route110_TrickHousePuzzle5_EventScript_Mechadoll4Quiz3` to that case. Evidence: `src/battle_util.c:IsBattlerGrounded; Psychic Terrain priority protection`.

### `Route110_TrickHousePuzzle5_Text_Mechadoll5Quiz1`

Source: [data/maps/Route110_TrickHousePuzzle5/scripts.inc:1009](../baseline/source/data/maps/Route110_TrickHousePuzzle5/scripts.inc#L1009)

Current: MECHADOLL 5 QUIZ. //  In SLATEPORT's POKéMON FAN CLUB, /  how many POKéMON were there?

Final text:

```asm
Route110_TrickHousePuzzle5_Text_Mechadoll5Quiz1:
	.string "MECHADOLL 5 QUIZ.\p"
	.string "You switch out while TRICK ROOM is\n"
	.string "active. What happens to the room's\l"
	.string "timer?$"
```

Choices in `MultichoiceList_Mechadoll5_Q1` (`src/data/script_menu.h`), zero-based: 0: It ends immediately, 1: It keeps running, 2: It starts over. Correct index: **1**. Update `Route110_TrickHousePuzzle5_EventScript_Mechadoll5Quiz1` to that case. Evidence: `src/battle_main.c and field timer update; no switch reset`.

### `Route110_TrickHousePuzzle5_Text_Mechadoll5Quiz2`

Source: [data/maps/Route110_TrickHousePuzzle5/scripts.inc:1014](../baseline/source/data/maps/Route110_TrickHousePuzzle5/scripts.inc#L1014)

Current: MECHADOLL 5 QUIZ. //  In FORTREE CITY, how many /  tree houses were there?

Final text:

```asm
Route110_TrickHousePuzzle5_Text_Mechadoll5Quiz2:
	.string "MECHADOLL 5 QUIZ.\p"
	.string "Which move guards your side\n"
	.string "against most damaging moves that\l"
	.string "hit multiple targets?$"
```

Choices in `MultichoiceList_Mechadoll5_Q2` (`src/data/script_menu.h`), zero-based: 0: SAFEGUARD, 1: QUICK GUARD, 2: WIDE GUARD. Correct index: **2**. Update `Route110_TrickHousePuzzle5_EventScript_Mechadoll5Quiz2` to that case. Evidence: `src/battle_util.c:5896`.

### `Route110_TrickHousePuzzle5_Text_Mechadoll5Quiz3`

Source: [data/maps/Route110_TrickHousePuzzle5/scripts.inc:1019](../baseline/source/data/maps/Route110_TrickHousePuzzle5/scripts.inc#L1019)

Current: MECHADOLL 5 QUIZ. //  On the CYCLING ROAD, how many /  TRIATHLETES were there?

Final text:

```asm
Route110_TrickHousePuzzle5_Text_Mechadoll5Quiz3:
	.string "MECHADOLL 5 QUIZ.\p"
	.string "PRANKSTER-boosted TAUNT normally\n"
	.string "fails against an opposing Pokémon\l"
	.string "of which type?$"
```

Choices in `MultichoiceList_Mechadoll5_Q3` (`src/data/script_menu.h`), zero-based: 0: DARK, 1: DRAGON, 2: PSYCHIC. Correct index: **0**. Update `Route110_TrickHousePuzzle5_EventScript_Mechadoll5Quiz3` to that case. Evidence: `src/battle_util.c:9458; B_PRANKSTER_DARK_TYPES`.

Dependencies: Configured battle mechanics; current move/ability/item behavior; W-TRICK-REWARDS

Acceptance: Update all15 menu arrays and matching correct-answer cases together. Exercise one correct and each incorrect choice per question through the script path, with mechanical claims checked against production behavior. Keep any future mechanic change tied to its affected question instead of a historical prose lock.

<a id="w-tent-slateportcity"></a>

## W-TENT-SLATEPORTCITY — Present Slateport as a local doubles exhibition

**REVISE.** FAC-01 supersedes native legacy Factory/Palace/Arena battle rules. Preserve the location, local residents and unrelated gift, and make the actual competition format explicit.

### `SlateportCity_BattleTentLobby_EventScript_Attendant`

Source: [data/maps/SlateportCity_BattleTentLobby/scripts.inc:90](../baseline/source/data/maps/SlateportCity_BattleTentLobby/scripts.inc#L90)

Current: lock; faceplayer; slateporttent_getprize; goto_if_ne VAR_RESULT, ITEM_NONE, SlateportCity_BattleTentLobby_EventScript_GivePrize; special SavePlayerParty; msgbox SlateportCity_BattleTentLobby_Text_WelcomeToBattleTent, MSGBOX_DEFAULT

Final behavior:

Replace the legacy challenge entry with FAC-01 local exhibition entry. First settle any existing pending slateportTentPrize using its existing commit-on-success contract. Offer rules and participation; call the shared local exhibition API only after eligibility and affirmative choice. It performs battle in this lobby and returns here. Three exhibition victories earn exactly one prize from the existing local pool; no Circuit counters, legendary entitlements or legacy rental/Palace/Arena mode advances.

### `SlateportCity_BattleTentLobby_EventScript_RulesBoard`

Source: [data/maps/SlateportCity_BattleTentLobby/scripts.inc:226](../baseline/source/data/maps/SlateportCity_BattleTentLobby/scripts.inc#L226)

Current: lockall; msgbox BattleFrontier_BattleFactoryLobby_Text_RulesAreListed, MSGBOX_DEFAULT; goto SlateportCity_BattleTentLobby_EventScript_ReadRulesBoard; end

Final behavior:

Display the new ExhibitionRules text below as a sign and end. Remove the old scrolling headings for rentals, swaps, autonomous Nature choice, Arena judging, level30 and three-Pokémon teams.

### `SlateportCity_BattleTentLobby_Text_ExhibitionWelcome`

Source: [data/maps/SlateportCity_BattleTentLobby/scripts.inc](../baseline/source/data/maps/SlateportCity_BattleTentLobby/scripts.inc) — new entry

Final text:

```asm
SlateportCity_BattleTentLobby_Text_ExhibitionWelcome:
	.string "The port brings together teams\n"
	.string "from every shore.\p"
	.string "This is a six-Pokémon doubles\n"
	.string "exhibition. You command every move\l"
	.string "and switch.\p"
	.string "Your team uses the current cap.\n"
	.string "Difficulty sets opponent levels.\l"
	.string "Enter with six healthy partners?$"
```

### `SlateportCity_BattleTentLobby_Text_ExhibitionRules`

Source: [data/maps/SlateportCity_BattleTentLobby/scripts.inc](../baseline/source/data/maps/SlateportCity_BattleTentLobby/scripts.inc) — new entry

Final text:

```asm
SlateportCity_BattleTentLobby_Text_ExhibitionRules:
	.string "LOCAL DOUBLES EXHIBITIONS\p"
	.string "Bring six healthy Pokémon, with no\n"
	.string "Eggs. Prepare freely before\l"
	.string "entering.\p"
	.string "Three victories earn one local\n"
	.string "prize. Your party is restored\l"
	.string "after each exhibition.\p"
	.string "These results are separate from\n"
	.string "the CHAMPIONS CIRCUIT. You return\l"
	.string "to this lobby afterward.$"
```

Dependencies: FAC-01; DIFF-01; existing src/battle_tent.c local prize pool

Acceptance: Enter with5/6 and withEgg/faintedmember; decline cleanly; win three times, lose, withdraw, reload and fillBagbeforeprize. Preserve held items/moves/levels/HP and pendingprize transaction; never warp to Frontier. Confirm no change to Circuit lifetime/best/rewardstate.

<a id="w-tent-verdanturftown"></a>

## W-TENT-VERDANTURFTOWN — Present Verdanturf as a local doubles exhibition

**REVISE.** FAC-01 supersedes native legacy Factory/Palace/Arena battle rules. Preserve the location, local residents and unrelated gift, and make the actual competition format explicit.

### `VerdanturfTown_BattleTentLobby_EventScript_Attendant`

Source: [data/maps/VerdanturfTown_BattleTentLobby/scripts.inc:104](../baseline/source/data/maps/VerdanturfTown_BattleTentLobby/scripts.inc#L104)

Current: lock; faceplayer; verdanturftent_getprize; goto_if_ne VAR_RESULT, ITEM_NONE, VerdanturfTown_BattleTentLobby_EventScript_PrizeWaiting; special SavePlayerParty; msgbox VerdanturfTown_BattleTentLobby_Text_WelcomeToBattleTent, MSGBOX_DEFAULT

Final behavior:

Replace the legacy challenge entry with FAC-01 local exhibition entry. First settle any existing pending verdanturfTentPrize using its existing commit-on-success contract. Offer rules and participation; call the shared local exhibition API only after eligibility and affirmative choice. It performs battle in this lobby and returns here. Three exhibition victories earn exactly one prize from the existing local pool; no Circuit counters, legendary entitlements or legacy rental/Palace/Arena mode advances.

### `VerdanturfTown_BattleTentLobby_EventScript_RulesBoard`

Source: [data/maps/VerdanturfTown_BattleTentLobby/scripts.inc:270](../baseline/source/data/maps/VerdanturfTown_BattleTentLobby/scripts.inc#L270)

Current: lockall; msgbox VerdanturfTown_BattleTentLobby_Text_RulesAreListed, MSGBOX_DEFAULT; goto VerdanturfTown_BattleTentLobby_EventScript_ReadRulesBoard; end

Final behavior:

Display the new ExhibitionRules text below as a sign and end. Remove the old scrolling headings for rentals, swaps, autonomous Nature choice, Arena judging, level30 and three-Pokémon teams.

### `VerdanturfTown_BattleTentLobby_Text_ExhibitionWelcome`

Source: [data/maps/VerdanturfTown_BattleTentLobby/scripts.inc](../baseline/source/data/maps/VerdanturfTown_BattleTentLobby/scripts.inc) — new entry

Final text:

```asm
VerdanturfTown_BattleTentLobby_Text_ExhibitionWelcome:
	.string "Bring a team that supports each\n"
	.string "partner through a hard match.\p"
	.string "This is a six-Pokémon doubles\n"
	.string "exhibition. You command every move\l"
	.string "and switch.\p"
	.string "Your team uses the current cap.\n"
	.string "Difficulty sets opponent levels.\l"
	.string "Enter with six healthy partners?$"
```

### `VerdanturfTown_BattleTentLobby_Text_ExhibitionRules`

Source: [data/maps/VerdanturfTown_BattleTentLobby/scripts.inc](../baseline/source/data/maps/VerdanturfTown_BattleTentLobby/scripts.inc) — new entry

Final text:

```asm
VerdanturfTown_BattleTentLobby_Text_ExhibitionRules:
	.string "LOCAL DOUBLES EXHIBITIONS\p"
	.string "Bring six healthy Pokémon, with no\n"
	.string "Eggs. Prepare freely before\l"
	.string "entering.\p"
	.string "Three victories earn one local\n"
	.string "prize. Your party is restored\l"
	.string "after each exhibition.\p"
	.string "These results are separate from\n"
	.string "the CHAMPIONS CIRCUIT. You return\l"
	.string "to this lobby afterward.$"
```

Dependencies: FAC-01; DIFF-01; existing src/battle_tent.c local prize pool

Acceptance: Enter with5/6 and withEgg/faintedmember; decline cleanly; win three times, lose, withdraw, reload and fillBagbeforeprize. Preserve held items/moves/levels/HP and pendingprize transaction; never warp to Frontier. Confirm no change to Circuit lifetime/best/rewardstate.

<a id="w-tent-fallarbortown"></a>

## W-TENT-FALLARBORTOWN — Present Fallarbor as a local doubles exhibition

**REVISE.** FAC-01 supersedes native legacy Factory/Palace/Arena battle rules. Preserve the location, local residents and unrelated gift, and make the actual competition format explicit.

### `FallarborTown_BattleTentLobby_EventScript_Attendant`

Source: [data/maps/FallarborTown_BattleTentLobby/scripts.inc:103](../baseline/source/data/maps/FallarborTown_BattleTentLobby/scripts.inc#L103)

Current: lock; faceplayer; fallarbortent_getprize; goto_if_ne VAR_RESULT, ITEM_NONE, FallarborTown_BattleTentLobby_EventScript_PrizeWaiting; special SavePlayerParty; msgbox FallarborTown_BattleTentLobby_Text_WelcomeToBattleTent, MSGBOX_DEFAULT

Final behavior:

Replace the legacy challenge entry with FAC-01 local exhibition entry. First settle any existing pending fallarborTentPrize using its existing commit-on-success contract. Offer rules and participation; call the shared local exhibition API only after eligibility and affirmative choice. It performs battle in this lobby and returns here. Three exhibition victories earn exactly one prize from the existing local pool; no Circuit counters, legendary entitlements or legacy rental/Palace/Arena mode advances.

### `FallarborTown_BattleTentLobby_EventScript_RulesBoard`

Source: [data/maps/FallarborTown_BattleTentLobby/scripts.inc:253](../baseline/source/data/maps/FallarborTown_BattleTentLobby/scripts.inc#L253)

Current: lockall; msgbox BattleFrontier_BattleArenaLobby_Text_RulesAreListed, MSGBOX_DEFAULT; goto FallarborTown_BattleTentLobby_EventScript_ReadRulesBoard; end

Final behavior:

Display the new ExhibitionRules text below as a sign and end. Remove the old scrolling headings for rentals, swaps, autonomous Nature choice, Arena judging, level30 and three-Pokémon teams.

### `FallarborTown_BattleTentLobby_Text_ExhibitionWelcome`

Source: [data/maps/FallarborTown_BattleTentLobby/scripts.inc](../baseline/source/data/maps/FallarborTown_BattleTentLobby/scripts.inc) — new entry

Final text:

```asm
FallarborTown_BattleTentLobby_Text_ExhibitionWelcome:
	.string "Our challengers bring ideas from\n"
	.string "the mountains and far beyond.\p"
	.string "This is a six-Pokémon doubles\n"
	.string "exhibition. You command every move\l"
	.string "and switch.\p"
	.string "Your team uses the current cap.\n"
	.string "Difficulty sets opponent levels.\l"
	.string "Enter with six healthy partners?$"
```

### `FallarborTown_BattleTentLobby_Text_ExhibitionRules`

Source: [data/maps/FallarborTown_BattleTentLobby/scripts.inc](../baseline/source/data/maps/FallarborTown_BattleTentLobby/scripts.inc) — new entry

Final text:

```asm
FallarborTown_BattleTentLobby_Text_ExhibitionRules:
	.string "LOCAL DOUBLES EXHIBITIONS\p"
	.string "Bring six healthy Pokémon, with no\n"
	.string "Eggs. Prepare freely before\l"
	.string "entering.\p"
	.string "Three victories earn one local\n"
	.string "prize. Your party is restored\l"
	.string "after each exhibition.\p"
	.string "These results are separate from\n"
	.string "the CHAMPIONS CIRCUIT. You return\l"
	.string "to this lobby afterward.$"
```

Dependencies: FAC-01; DIFF-01; existing src/battle_tent.c local prize pool

Acceptance: Enter with5/6 and withEgg/faintedmember; decline cleanly; win three times, lose, withdraw, reload and fillBagbeforeprize. Preserve held items/moves/levels/HP and pendingprize transaction; never warp to Frontier. Confirm no change to Circuit lifetime/best/rewardstate.

<a id="w-tent-flavor"></a>

## W-TENT-FLAVOR — Remove obsolete rental and autonomous-move claims from live tent residents

**REVISE.** Native tent mechanics change under FAC-01. Their NPCs need to talk about the actual exhibitions rather than an unoffered ruleset.

### `SlateportCity_BattleTentLobby_Text_CouldntFindMonForMe`

Source: [data/maps/SlateportCity_BattleTentLobby/scripts.inc:275](../baseline/source/data/maps/SlateportCity_BattleTentLobby/scripts.inc#L275)

Current: So, like, I couldn't find myself any /  POKéMON that were, like, for me. //  So, I figured, like, hey, I should file /  a complaint to the guy there? //  And he wouldn't hear me out, like, hey! /  So, like, total bummer, man! //  Hey, like, you! Zip it, you know? /  Just, you know, take this!

Final text:

```asm
SlateportCity_BattleTentLobby_Text_CouldntFindMonForMe:
	.string "I couldn't settle on a team. Too\n"
	.string "many interesting choices!\p"
	.string "Then I found this PRISM SCALE.\n"
	.string "Maybe you can put it to use.$"
```

### `SlateportCity_BattleTentLobby_Text_BattleEvenWithoutToughMons`

Source: [data/maps/SlateportCity_BattleTentLobby/scripts.inc:295](../baseline/source/data/maps/SlateportCity_BattleTentLobby/scripts.inc#L295)

Current: You can battle all you want here even /  if you don't have any tough POKéMON.

Final text:

```asm
SlateportCity_BattleTentLobby_Text_BattleEvenWithoutToughMons:
	.string "The CENTER can prepare any legal\n"
	.string "set for free. Bring your own six\l"
	.string "and test them here.$"
```

### `SlateportCity_BattleTentLobby_Text_NiceIfMoreSelection`

Source: [data/maps/SlateportCity_BattleTentLobby/scripts.inc:299](../baseline/source/data/maps/SlateportCity_BattleTentLobby/scripts.inc#L299)

Current: Wouldn't it be nice if they had more of /  a selection?

Final text:

```asm
SlateportCity_BattleTentLobby_Text_NiceIfMoreSelection:
	.string "A new opponent every match! I keep\n"
	.string "leaving with another idea for my\l"
	.string "team.$"
```

### `VerdanturfTown_BattleTentLobby_Text_MonsReluctantToUseDislikedMoves`

Source: [data/maps/VerdanturfTown_BattleTentLobby/scripts.inc:326](../baseline/source/data/maps/VerdanturfTown_BattleTentLobby/scripts.inc#L326)

Current: If it doesn't like a certain move, /  a POKéMON will be reluctant to use it. //  It doesn't matter how strong it is, /  either. //  For example, a POKéMON with a GENTLE /  nature probably won't enjoy hurting /  its opponents. //  If it can't seem to live up to its /  potential, it's probably failing at /  using a disliked move against its will.

Final text:

```asm
VerdanturfTown_BattleTentLobby_Text_MonsReluctantToUseDislikedMoves:
	.string "Natures change stats, not which\n"
	.string "moves your Pokémon will obey.\p"
	.string "You command both partners here. A\n"
	.string "quiet Pokémon can have a very\l"
	.string "aggressive battle set.$"
```

### `FallarborTown_BattleTentLobby_Text_FallarborTentMyFavorite`

Source: [data/maps/FallarborTown_BattleTentLobby/scripts.inc:309](../baseline/source/data/maps/FallarborTown_BattleTentLobby/scripts.inc#L309)

Current: You know how BATTLE TENTS offer /  different events in different towns? //  My favorite is definitely the BATTLE /  TENT in FALLARBOR TOWN. //  I think it's fantastic how TRAINERS /  try to win with all their faith in /  their POKéMON.

Final text:

```asm
FallarborTown_BattleTentLobby_Text_FallarborTentMyFavorite:
	.string "The tents share the same serious\n"
	.string "doubles competition.\p"
	.string "FALLARBOR is my favorite place to\n"
	.string "visit between exhibitions.$"
```

Dependencies: FAC-01

Acceptance: Talk to all three lobbies’ residents after entering an exhibition; no resident promises rental selection, autonomous commands, three-turn judging or a different native format.

<a id="w-circuit-rules"></a>

## W-CIRCUIT-RULES — Give every active Frontier rules board one truthful contract

**REPAIR.** All current desks enter Circuit, but all seven legacy rule boards remain live and advertise incompatible old formats. Replace those interaction roots; keep their historical underlying labels inert for archival source compatibility.

### `BattleFrontier_BattleTowerLobby_EventScript_RulesBoard`

Source: [data/maps/BattleFrontier_BattleTowerLobby/scripts.inc:1017](../baseline/source/data/maps/BattleFrontier_BattleTowerLobby/scripts.inc#L1017)

Current: lockall; msgbox BattleFrontier_BattleTowerLobby_Text_RulesAreListed, MSGBOX_DEFAULT; goto BattleFrontier_BattleTowerLobby_EventScript_ReadRulesBoard; end

Final behavior:

Replace the old mode-selection help with: msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRules, MSGBOX_SIGN; end. This is information only and never starts a battle.

### `BattleFrontier_BattleDomeLobby_EventScript_RulesBoard`

Source: [data/maps/BattleFrontier_BattleDomeLobby/scripts.inc:402](../baseline/source/data/maps/BattleFrontier_BattleDomeLobby/scripts.inc#L402)

Current: lockall; msgbox BattleFrontier_BattleDomeLobby_Text_RulesAreListed, MSGBOX_DEFAULT; goto BattleFrontier_BattleDomeLobby_EventScript_ReadRulesBoard; end

Final behavior:

Replace the old mode-selection help with: msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRules, MSGBOX_SIGN; end. This is information only and never starts a battle.

### `BattleFrontier_BattlePalaceLobby_EventScript_RulesBoard`

Source: [data/maps/BattleFrontier_BattlePalaceLobby/scripts.inc:331](../baseline/source/data/maps/BattleFrontier_BattlePalaceLobby/scripts.inc#L331)

Current: lockall; msgbox BattleFrontier_BattlePalaceLobby_Text_RulesAreListed, MSGBOX_DEFAULT; goto BattleFrontier_BattlePalaceLobby_EventScript_ReadRulesBoard; end

Final behavior:

Replace the old mode-selection help with: msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRules, MSGBOX_SIGN; end. This is information only and never starts a battle.

### `BattleFrontier_BattlePyramidLobby_EventScript_RulesBoard`

Source: [data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc:486](../baseline/source/data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc#L486)

Current: lockall; msgbox BattleFrontier_BattlePyramidLobby_Text_RulesAreListed, MSGBOX_DEFAULT; goto BattleFrontier_BattlePyramidLobby_EventScript_ReadRulesBoard; end

Final behavior:

Replace the old mode-selection help with: msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRules, MSGBOX_SIGN; end. This is information only and never starts a battle.

### `BattleFrontier_BattleArenaLobby_EventScript_RulesBoard`

Source: [data/maps/BattleFrontier_BattleArenaLobby/scripts.inc:316](../baseline/source/data/maps/BattleFrontier_BattleArenaLobby/scripts.inc#L316)

Current: lockall; msgbox BattleFrontier_BattleArenaLobby_Text_RulesAreListed, MSGBOX_DEFAULT; goto BattleFrontier_BattleArenaLobby_EventScript_ReadRulesBoard; end

Final behavior:

Replace the old mode-selection help with: msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRules, MSGBOX_SIGN; end. This is information only and never starts a battle.

### `BattleFrontier_BattleFactoryLobby_EventScript_RulesBoard`

Source: [data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc:276](../baseline/source/data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc#L276)

Current: lockall; msgbox BattleFrontier_BattleFactoryLobby_Text_RulesAreListed, MSGBOX_DEFAULT; goto BattleFrontier_BattleFactoryLobby_EventScript_ReadRulesBoard; end

Final behavior:

Replace the old mode-selection help with: msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRules, MSGBOX_SIGN; end. This is information only and never starts a battle.

### `BattleFrontier_BattlePikeLobby_EventScript_RulesBoard`

Source: [data/maps/BattleFrontier_BattlePikeLobby/scripts.inc:231](../baseline/source/data/maps/BattleFrontier_BattlePikeLobby/scripts.inc#L231)

Current: lockall; msgbox BattleFrontier_BattlePikeLobby_Text_RulesAreListed, MSGBOX_DEFAULT; goto BattleFrontier_BattlePikeLobby_EventScript_ReadRulesBoard; end

Final behavior:

Replace the old mode-selection help with: msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRules, MSGBOX_SIGN; end. This is information only and never starts a battle.

### `BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRules`

Source: [data/maps/BattleFrontier_BattleTowerLobby/scripts.inc](../baseline/source/data/maps/BattleFrontier_BattleTowerLobby/scripts.inc) — new entry

Final text:

```asm
BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRules:
	.string "CHAMPIONS CIRCUIT\p"
	.string "Bring six healthy Pokémon and no\n"
	.string "Eggs. Every match is doubles; you\l"
	.string "control your own team.\p"
	.string "Your team battles at Lv. 100.\n"
	.string "Opponent levels begin from your\l"
	.string "difficulty and rise with wins.\p"
	.string "After each battle, your party and\n"
	.string "held items are restored. You can\l"
	.string "leave between matches.\p"
	.string "Every Frontier desk registers you\n"
	.string "for the central Tower arena.\l"
	.string "Milestone rewards remain recorded.$"
```

### `BattleFrontier_BattleTowerLobby_Text_CanLeaveUntilLossOrSevenWins`

Source: [data/maps/BattleFrontier_BattleTowerLobby/scripts.inc:1163](../baseline/source/data/maps/BattleFrontier_BattleTowerLobby/scripts.inc#L1163)

Current: Once you've entered the BATTLE TOWER, /  you can't leave until you either lose /  or you beat seven TRAINERS in a row. //  You'd best be certain that you're up /  to the challenge.

Final text:

```asm
BattleFrontier_BattleTowerLobby_Text_CanLeaveUntilLossOrSevenWins:
	.string "The CIRCUIT restores your team\n"
	.string "after each match.\p"
	.string "You may continue or leave then. A\n"
	.string "strong run deserves another\l"
	.string "well-prepared decision.$"
```

### `BattleFrontier_BattleTowerLobby_Text_CircuitWelcome`

Source: [data/maps/BattleFrontier_BattleTowerLobby/scripts.inc:506](../baseline/source/data/maps/BattleFrontier_BattleTowerLobby/scripts.inc#L506)

Current: This is the FRONTIER's premier challenge: /  the CHAMPIONS CIRCUIT. //  Every match is a full-team Double Battle /  assembled from competitive roles. //  Your team becomes Lv. 100. Opponents /  gain one total level after each win. //  They can climb as high as Lv. 255. /  Every desk enters this Circuit.

Final text:

```asm
BattleFrontier_BattleTowerLobby_Text_CircuitWelcome:
	.string "Welcome to the CHAMPIONS CIRCUIT!\p"
	.string "Every match is a full-team Double\n"
	.string "Battle with a competitive\l"
	.string "opponent.\p"
	.string "Your team battles at Lv. 100.\n"
	.string "Opponents grow stronger as your\l"
	.string "run continues, up to Lv. 255.\p"
	.string "Every Frontier desk registers you\n"
	.string "for the central Tower arena.$"
```

Dependencies: Shared Circuit API; DIFF-01; all native trainer battles doubles

Acceptance: Read every active lobby rules board and every desk. None offers singles, three-Pokémon teams, rentals, autonomous player move choice, Arena judging or seven-win forced lock-in. Confirm actual central-room route and between-match withdrawal.

<a id="w-frontier-guides"></a>

## W-FRONTIER-GUIDES — Reconcile the reception tour and Scott with the current competition

**REPAIR.** Reception still claims the other buildings preserve classic challenges, while their actual desks were replaced. Keep architecture and character history; describe current services exactly.

### `BattleFrontier_ReceptionGate_Text_BattleTowerInfo`

Source: [data/maps/BattleFrontier_ReceptionGate/scripts.inc:351](../baseline/source/data/maps/BattleFrontier_ReceptionGate/scripts.inc#L351)

Current: It is the gigantic tower considered /  to be the BATTLE FRONTIER's symbol. //  There are four kinds of BATTLE ROOMS /  in the tower for SINGLE, DOUBLE, MULTI, /  and LINK MULTI BATTLES. //  The far desk hosts the /  CHAMPIONS CIRCUIT, the FRONTIER's /  premier full-team test.

Final text:

```asm
BattleFrontier_ReceptionGate_Text_BattleTowerInfo:
	.string "The BATTLE TOWER is the great\n"
	.string "tower.\p"
	.string "Its reception desk enters the\n"
	.string "CHAMPIONS CIRCUIT at the central\l"
	.string "Tower arena. All matches are\l"
	.string "doubles.$"
```

### `BattleFrontier_ReceptionGate_Text_BattleDomeInfo`

Source: [data/maps/BattleFrontier_ReceptionGate/scripts.inc:361](../baseline/source/data/maps/BattleFrontier_ReceptionGate/scripts.inc#L361)

Current: The BATTLE DOME is the large building /  shaped like a huge egg. //  Events named Battle Tourneys are held /  in this facility. //  The Battle Tourneys are offered in /  two courses--for SINGLE and DOUBLE /  BATTLES.

Final text:

```asm
BattleFrontier_ReceptionGate_Text_BattleDomeInfo:
	.string "The BATTLE DOME is the oval\n"
	.string "building.\p"
	.string "Its reception desk enters the\n"
	.string "CHAMPIONS CIRCUIT at the central\l"
	.string "Tower arena. All matches are\l"
	.string "doubles.$"
```

### `BattleFrontier_ReceptionGate_Text_BattlePalaceInfo`

Source: [data/maps/BattleFrontier_ReceptionGate/scripts.inc:370](../baseline/source/data/maps/BattleFrontier_ReceptionGate/scripts.inc#L370)

Current: The BATTLE PALACE is the red building /  on the right of the BATTLE FRONTIER. //  There are two kinds of BATTLE HALLS /  for SINGLE and DOUBLE BATTLES.

Final text:

```asm
BattleFrontier_ReceptionGate_Text_BattlePalaceInfo:
	.string "The BATTLE PALACE is the red\n"
	.string "building to the east.\p"
	.string "Its reception desk enters the\n"
	.string "CHAMPIONS CIRCUIT at the central\l"
	.string "Tower arena. All matches are\l"
	.string "doubles.$"
```

### `BattleFrontier_ReceptionGate_Text_BattleArenaInfo`

Source: [data/maps/BattleFrontier_ReceptionGate/scripts.inc:376](../baseline/source/data/maps/BattleFrontier_ReceptionGate/scripts.inc#L376)

Current: The BATTLE ARENA is the dojo-like /  building at the center-right of /  the BATTLE FRONTIER. //  An event called the Set KO Tourney /  takes place at the BATTLE ARENA.

Final text:

```asm
BattleFrontier_ReceptionGate_Text_BattleArenaInfo:
	.string "The BATTLE ARENA is the central\n"
	.string "dojo.\p"
	.string "Its reception desk enters the\n"
	.string "CHAMPIONS CIRCUIT at the central\l"
	.string "Tower arena. All matches are\l"
	.string "doubles.$"
```

### `BattleFrontier_ReceptionGate_Text_BattleFactoryInfo`

Source: [data/maps/BattleFrontier_ReceptionGate/scripts.inc:383](../baseline/source/data/maps/BattleFrontier_ReceptionGate/scripts.inc#L383)

Current: The BATTLE FACTORY is the large /  building that is the closest to us. //  An event called the Battle Swap /  is conducted there. //  The Battle Swap event is offered in /  two courses for SINGLE and DOUBLE /  BATTLES.

Final text:

```asm
BattleFrontier_ReceptionGate_Text_BattleFactoryInfo:
	.string "The BATTLE FACTORY is the building\n"
	.string "nearest this gate.\p"
	.string "Its reception desk enters the\n"
	.string "CHAMPIONS CIRCUIT at the central\l"
	.string "Tower arena. All matches are\l"
	.string "doubles.$"
```

### `BattleFrontier_ReceptionGate_Text_BattlePikeInfo`

Source: [data/maps/BattleFrontier_ReceptionGate/scripts.inc:392](../baseline/source/data/maps/BattleFrontier_ReceptionGate/scripts.inc#L392)

Current: The BATTLE PIKE is the building shaped /  like a POKéMON at the center-left of /  the BATTLE FRONTIER. //  An event called the Battle Choice /  is conducted there.

Final text:

```asm
BattleFrontier_ReceptionGate_Text_BattlePikeInfo:
	.string "The BATTLE PIKE is the\n"
	.string "SEVIPER-shaped building.\p"
	.string "Its reception desk enters the\n"
	.string "CHAMPIONS CIRCUIT at the central\l"
	.string "Tower arena. All matches are\l"
	.string "doubles.$"
```

### `BattleFrontier_ReceptionGate_Text_BattlePyramidInfo`

Source: [data/maps/BattleFrontier_ReceptionGate/scripts.inc:399](../baseline/source/data/maps/BattleFrontier_ReceptionGate/scripts.inc#L399)

Current: The BATTLE PYRAMID is the enormous /  pyramid. //  An event called the Battle Quest /  is conducted there.

Final text:

```asm
BattleFrontier_ReceptionGate_Text_BattlePyramidInfo:
	.string "The BATTLE PYRAMID is the enormous\n"
	.string "pyramid.\p"
	.string "Its reception desk enters the\n"
	.string "CHAMPIONS CIRCUIT at the central\l"
	.string "Tower arena. All matches are\l"
	.string "doubles.$"
```

### `BattleFrontier_ReceptionGate_EventScript_RulesGuide`

Source: [data/maps/BattleFrontier_ReceptionGate/scripts.inc:206](../baseline/source/data/maps/BattleFrontier_ReceptionGate/scripts.inc#L206)

Current: lock; faceplayer; msgbox BattleFrontier_ReceptionGate_Text_YourGuideToRules, MSGBOX_DEFAULT; goto BattleFrontier_ReceptionGate_EventScript_ChooseRuleToLearnAbout; end

Final behavior:

Show BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRules with MSGBOX_NPC and end; retire old Level50/Open/duplicate-item/rental-entry menu from native interaction.

### `BattleFrontier_ReceptionGate_Text_ScottGreatToSeeYouHere`

Source: [data/maps/BattleFrontier_ReceptionGate/scripts.inc:331](../baseline/source/data/maps/BattleFrontier_ReceptionGate/scripts.inc#L331)

Current: SCOTT: It's great to see you here, /  it really is! //  Start with the CHAMPIONS CIRCUIT at /  the far desk inside the BATTLE TOWER. //  It assembles a new full-team Double /  Battle every round. The other facilities /  preserve HOENN's classic challenges. Naturally, I hope you'll also experience /  the pure essence of battling. //  I also have my quarters here, so feel /  free to visit if you have time.

Final text:

```asm
BattleFrontier_ReceptionGate_Text_ScottGreatToSeeYouHere:
	.string "SCOTT: It's great to see you here!\p"
	.string "Every desk enters the CHAMPIONS\n"
	.string "CIRCUIT. Six partners, coordinated\l"
	.string "opponents, and no easy matches.\p"
	.string "The buildings preserve the\n"
	.string "Frontier's history. The Tower\l"
	.string "holds our central arena.\p"
	.string "Visit my house when you have time.\n"
	.string "I'd like to hear about your teams.$"
```

### `BattleFrontier_ReceptionGate_Text_RankingHallInfo`

Source: [data/maps/BattleFrontier_ReceptionGate/scripts.inc:405](../baseline/source/data/maps/BattleFrontier_ReceptionGate/scripts.inc#L405)

Current: The RANKING HALL is located near /  the BATTLE TOWER. //  There, you may see the most fantastic /  records left by the TRAINERS that /  took on the many challenges of /  the BATTLE FRONTIER.

Final text:

```asm
BattleFrontier_ReceptionGate_Text_RankingHallInfo:
	.string "The RANKING HALL is near the\n"
	.string "BATTLE TOWER.\p"
	.string "Its current displays show your\n"
	.string "CHAMPIONS CIRCUIT run, best\l"
	.string "recorded run, and total wins.$"
```

### `BattleFrontier_ReceptionGate_EventScript_FrontierPassGuide`

Source: [data/maps/BattleFrontier_ReceptionGate/scripts.inc:257](../baseline/source/data/maps/BattleFrontier_ReceptionGate/scripts.inc#L257)

Current: lock; faceplayer; msgbox BattleFrontier_ReceptionGate_Text_YourGuideToFrontierPass, MSGBOX_DEFAULT; goto BattleFrontier_ReceptionGate_EventScript_ChooseFrontierPassInfoToLearnAbout; end

Final behavior:

Keep Pass acquisition/ownership. Replace the old symbol/recording submenu with an accurate current Pass explanation: it retains the Trainer Card, Battle Points and historical symbol records. Current Circuit records are displayed at current record boards; do not claim old symbols are earned from Circuit unless another explicit chapter authorizes it.

### `BattleFrontier_ScottsHouse_Text_HaveYouMetFrontierBrain`

Source: [data/maps/BattleFrontier_ScottsHouse/scripts.inc:266](../baseline/source/data/maps/BattleFrontier_ScottsHouse/scripts.inc#L266)

Current: SCOTT: Have you met any of /  the FRONTIER BRAINS? //  Better yet, have you obtained any /  symbols from them? //  I handpicked the FRONTIER BRAINS /  as the toughest TRAINERS anywhere. //  But I'm sure that seeing how tough /  you are will startle even them!

Final text:

```asm
BattleFrontier_ScottsHouse_Text_HaveYouMetFrontierBrain:
	.string "SCOTT: The FRONTIER BRAINS gave\n"
	.string "these buildings their character.\p"
	.string "Today every desk enters the\n"
	.string "CHAMPIONS CIRCUIT. I want to see\l"
	.string "what your team can do there!$"
```

Dependencies: W-CIRCUIT-RULES; Circuit record display; historic earned-symbol rewards remain stored

Acceptance: Read each reception menu choice, initial Scott scene and later Scott random comments. They all describe the same active Circuit. Historical symbol flags and already earned Scott gifts remain retrievable without creating new claims.

<a id="w-frontier-residents"></a>

## W-FRONTIER-RESIDENTS — Keep local personality while removing unavailable-service promises

**REPAIR.** The outdoor residents and active lobby NPCs still describe rentals, maze goals, autonomous move choice and classic-symbol ambitions that the live Circuit does not supply. These complete replacements preserve individual attitudes and the buildings’ history.

### `BattleFrontier_OutsideWest_Text_SureWeCanChallengeWithNoMons`

Source: [data/maps/BattleFrontier_OutsideWest/scripts.inc:374](../baseline/source/data/maps/BattleFrontier_OutsideWest/scripts.inc#L374)

Current: Hey, bro… //  Are you sure we can make challenges /  even if we don't have any POKéMON?

Final text:

```asm
BattleFrontier_OutsideWest_Text_SureWeCanChallengeWithNoMons:
	.string "Hey, bro... We need six Pokémon\n"
	.string "for the CIRCUIT.\p"
	.string "Let's visit the CENTER and finish\n"
	.string "our team first.$"
```

### `BattleFrontier_OutsideWest_Text_BigGuySaidIllLendYouMons`

Source: [data/maps/BattleFrontier_OutsideWest/scripts.inc:379](../baseline/source/data/maps/BattleFrontier_OutsideWest/scripts.inc#L379)

Current: Uh… /  I'm sure it'll be okay. //  I think… //  But remember that big scary guy? /  He said, “I'll lend you POKéMON!”

Final text:

```asm
BattleFrontier_OutsideWest_Text_BigGuySaidIllLendYouMons:
	.string "That big fellow showed me how to\n"
	.string "prepare a set at the CENTER.\p"
	.string "He said the team still needs to be\n"
	.string "ours. Fair enough!$"
```

### `BattleFrontier_OutsideWest_Text_WhosRaisingThoseRentalMons`

Source: [data/maps/BattleFrontier_OutsideWest/scripts.inc:386](../baseline/source/data/maps/BattleFrontier_OutsideWest/scripts.inc#L386)

Current: That's the BATTLE FACTORY. /  You can rent strong POKéMON there. //  But it makes me wonder. /  Who's raising those rental POKéMON?

Final text:

```asm
BattleFrontier_OutsideWest_Text_WhosRaisingThoseRentalMons:
	.string "That's the BATTLE FACTORY. Its\n"
	.string "desk also enters the CIRCUIT.\p"
	.string "Who prepares all those opposing\n"
	.string "teams? I'd like to compare notes!$"
```

### `BattleFrontier_OutsideWest_Text_KeepBattlingUntilIGetSymbol`

Source: [data/maps/BattleFrontier_OutsideWest/scripts.inc:422](../baseline/source/data/maps/BattleFrontier_OutsideWest/scripts.inc#L422)

Current: Today, I'm going to keep battling, no /  matter what, until I get a Symbol.

Final text:

```asm
BattleFrontier_OutsideWest_Text_KeepBattlingUntilIGetSymbol:
	.string "I'm going to keep battling until I\n"
	.string "beat my best Circuit streak!$"
```

### `BattleFrontier_OutsideWest_Text_WonIllTakePikeChallenge`

Source: [data/maps/BattleFrontier_OutsideWest/scripts.inc:404](../baseline/source/data/maps/BattleFrontier_OutsideWest/scripts.inc#L404)

Current: Yay! I won! /  I will take the BATTLE PIKE challenge!

Final text:

```asm
BattleFrontier_OutsideWest_Text_WonIllTakePikeChallenge:
	.string "I won! I'll register at the PIKE\n"
	.string "and start another Circuit run!$"
```

### `BattleFrontier_OutsideWest_Text_LostIllPutOffPikeChallenge`

Source: [data/maps/BattleFrontier_OutsideWest/scripts.inc:408](../baseline/source/data/maps/BattleFrontier_OutsideWest/scripts.inc#L408)

Current: Oh, no… /  I lost. //  I guess I'm not very lucky today. /  I'll put off my BATTLE PIKE challenge /  until tomorrow.

Final text:

```asm
BattleFrontier_OutsideWest_Text_LostIllPutOffPikeChallenge:
	.string "I lost. Maybe that's my cue to\n"
	.string "review my team before the CIRCUIT.$"
```

### `BattleFrontier_OutsideWest_Text_BattlePikeSign`

Source: [data/maps/BattleFrontier_OutsideWest/scripts.inc:308](../baseline/source/data/maps/BattleFrontier_OutsideWest/scripts.inc#L308)

Current: This is the BATTLE PIKE! /  Choose one of three paths!

Final text:

```asm
BattleFrontier_OutsideWest_Text_BattlePikeSign:
	.string "BATTLE PIKE\p"
	.string "CHAMPIONS CIRCUIT RECEPTION\n"
	.string "Central arena: BATTLE TOWER$"
```

### `BattleFrontier_OutsideEast_Text_PyramidTooHarsh`

Source: [data/maps/BattleFrontier_OutsideEast/scripts.inc:273](../baseline/source/data/maps/BattleFrontier_OutsideEast/scripts.inc#L273)

Current: The BATTLE PYRAMID's too harsh! /  I just can't make it to the top! //  Since I'm out of options, maybe I can /  climb the outside…

Final text:

```asm
BattleFrontier_OutsideEast_Text_PyramidTooHarsh:
	.string "The CIRCUIT is too harsh! I keep\n"
	.string "losing at the same point.\p"
	.string "Maybe I should change my plan\n"
	.string "instead of climbing this pyramid.$"
```

### `BattleFrontier_OutsideEast_Text_ThriveInDarkness`

Source: [data/maps/BattleFrontier_OutsideEast/scripts.inc:279](../baseline/source/data/maps/BattleFrontier_OutsideEast/scripts.inc#L279)

Current: I thrive in darkness… /  Yes… What is worthy of me? /  None other than the BATTLE PYRAMID… //  What say you to wandering in darkness /  and in utter and total desperation?

Final text:

```asm
BattleFrontier_OutsideEast_Text_ThriveInDarkness:
	.string "I thrive in darkness...\p"
	.string "A quiet pyramid, a difficult team\n"
	.string "to build, and nobody interrupting\l"
	.string "my thoughts...$"
```

### `BattleFrontier_OutsideEast_Text_PeopleCallMeBusybody`

Source: [data/maps/BattleFrontier_OutsideEast/scripts.inc:309](../baseline/source/data/maps/BattleFrontier_OutsideEast/scripts.inc#L309)

Current: People call me a busybody, /  but I can't help it. //  Your hat's on crooked! /  Oh, no, trash on the ground! /  Oops, it's almost dinnertime! //  I don't know if I can stand to just /  watch at the BATTLE PALACE…

Final text:

```asm
BattleFrontier_OutsideEast_Text_PeopleCallMeBusybody:
	.string "People call me a busybody. I keep\n"
	.string "spotting something to fix!\p"
	.string "That habit helps in doubles. There\n"
	.string "are two sides of the board to\l"
	.string "watch at once.$"
```

### `BattleFrontier_OutsideEast_Text_PowerOfOurLoveWillOvercome`

Source: [data/maps/BattleFrontier_OutsideEast/scripts.inc:399](../baseline/source/data/maps/BattleFrontier_OutsideEast/scripts.inc#L399)

Current: Ooh, darling, you are so wonderful! //  Ooh, I just can't wait anymore! //  Let's go to a MULTI BATTLE ROOM /  right this instant! //  If we get together in the BATTLE SALON, /  the power of our love will overcome /  everyone we meet. //  Why, before us, darling, everything /  will topple like dominoes!

Final text:

```asm
BattleFrontier_OutsideEast_Text_PowerOfOurLoveWillOvercome:
	.string "Let's sit down and build a team\n"
	.string "together, darling!\p"
	.string "Six partners that support one\n"
	.string "another. Then we can each see how\l"
	.string "far our plan goes!$"
```

### `BattleFrontier_OutsideEast_Text_BattlePyramidSign`

Source: [data/maps/BattleFrontier_OutsideEast/scripts.inc:247](../baseline/source/data/maps/BattleFrontier_OutsideEast/scripts.inc#L247)

Current: This is the BATTLE PYRAMID! /  Advance through the Battle Quest!

Final text:

```asm
BattleFrontier_OutsideEast_Text_BattlePyramidSign:
	.string "BATTLE PYRAMID\p"
	.string "CHAMPIONS CIRCUIT RECEPTION\n"
	.string "Central arena: BATTLE TOWER$"
```

### `BattleFrontier_BattleDomeLobby_Text_NeedToCheckOpponentCarefully`

Source: [data/maps/BattleFrontier_BattleDomeLobby/scripts.inc:635](../baseline/source/data/maps/BattleFrontier_BattleDomeLobby/scripts.inc#L635)

Current: I would've won if I'd kept this POKéMON /  held in reserve. //  You need to check your opponent's /  POKéMON carefully before choosing /  your battling POKéMON.

Final text:

```asm
BattleFrontier_BattleDomeLobby_Text_NeedToCheckOpponentCarefully:
	.string "I committed my best answer too\n"
	.string "early and lost it.\p"
	.string "A reserve is valuable only if you\n"
	.string "preserve the chance to use it.$"
```

### `BattleFrontier_BattleDomeLobby_Text_WinnersGainReputation`

Source: [data/maps/BattleFrontier_BattleDomeLobby/scripts.inc:621](../baseline/source/data/maps/BattleFrontier_BattleDomeLobby/scripts.inc#L621)

Current: When a TRAINER chains tournament /  wins at the BATTLE DOME, he or she /  gains a reputation as a star. //  Tough TRAINERS are drawn by that /  reputation to the BATTLE DOME. //  A true superstar is a TRAINER who /  can keep winning tournaments.

Final text:

```asm
BattleFrontier_BattleDomeLobby_Text_WinnersGainReputation:
	.string "The DOME built its reputation on\n"
	.string "great tournament battles.\p"
	.string "Now its desk enters the CIRCUIT.\n"
	.string "Keep winning, and people remember\l"
	.string "your team!$"
```

### `BattleFrontier_BattleDomeLobby_Text_TrashedInFirstRound`

Source: [data/maps/BattleFrontier_BattleDomeLobby/scripts.inc:630](../baseline/source/data/maps/BattleFrontier_BattleDomeLobby/scripts.inc#L630)

Current: I ran into one of the tournament /  favorites in the very first round. //  Of course I got trashed…

Final text:

```asm
BattleFrontier_BattleDomeLobby_Text_TrashedInFirstRound:
	.string "My very first Circuit opponent\n"
	.string "broke through my opening.\p"
	.string "I need a stronger answer from the\n"
	.string "first turn.$"
```

### `BattleFrontier_BattleDomeLobby_Text_LastWinnerWasTough`

Source: [data/maps/BattleFrontier_BattleDomeLobby/scripts.inc:599](../baseline/source/data/maps/BattleFrontier_BattleDomeLobby/scripts.inc#L599)

Current: Did you see it? /  The last Battle Tournament? //  The winner, {STR_VAR_1}, was seriously /  tough. //  You should check out the results /  on the monitor beside the PC.

Final text:

```asm
BattleFrontier_BattleDomeLobby_Text_LastWinnerWasTough:
	.string "These monitors preserve the DOME's\n"
	.string "old tournaments.\p"
	.string "Today's Circuit records are marked\n"
	.string "separately. A different challenge,\l"
	.string "the same ambition!$"
```

### `BattleFrontier_BattlePalaceLobby_Text_WhatNatureFavorsChippingAway`

Source: [data/maps/BattleFrontier_BattlePalaceLobby/scripts.inc:543](../baseline/source/data/maps/BattleFrontier_BattlePalaceLobby/scripts.inc#L543)

Current: I wonder what sort of nature a POKéMON /  would have if it favored enfeebling its /  opponents and chipping away slowly. //  I'd be surprised if it was a LAX nature. //  But, nah, that can't be right.

Final text:

```asm
BattleFrontier_BattlePalaceLobby_Text_WhatNatureFavorsChippingAway:
	.string "A careful Nature and Stat Point\n"
	.string "spread can help a partner survive\l"
	.string "the turn that matters.\p"
	.string "Then I choose what it does with\n"
	.string "that extra turn!$"
```

### `BattleFrontier_BattlePalaceLobby_Text_NatureAndMovesKeyHere`

Source: [data/maps/BattleFrontier_BattlePalaceLobby/scripts.inc:522](../baseline/source/data/maps/BattleFrontier_BattlePalaceLobby/scripts.inc#L522)

Current: Hmm… //  It appears that the nature of POKéMON /  and the moves that they have been /  taught are the keys to battle here. //  To be more precise, it's how well /  the moves match the nature of /  the POKéMON. //  If your POKéMON is in trouble and /  unable to live up to its potential, /  you may need to examine how well /  its moves match its nature.

Final text:

```asm
BattleFrontier_BattlePalaceLobby_Text_NatureAndMovesKeyHere:
	.string "The old Palace tested how Pokémon\n"
	.string "acted on their own.\p"
	.string "In the CIRCUIT, you command every\n"
	.string "move. Natures change stats, not\l"
	.string "obedience.$"
```

### `BattleFrontier_BattleFactoryLobby_Text_NeedKnowledgeOfMonsMoves`

Source: [data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc:471](../baseline/source/data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc#L471)

Current: Hi! /  You, there! //  Are you thinking that the events here /  are easy since you don't need to have /  a raised team of POKéMON? //  I wouldn't be too sure about winning /  that easily. //  If you don't have thorough knowledge /  about POKéMON and their moves, /  it will be tough to keep winning.

Final text:

```asm
BattleFrontier_BattleFactoryLobby_Text_NeedKnowledgeOfMonsMoves:
	.string "Don't judge an opposing Pokémon by\n"
	.string "its reputation alone.\p"
	.string "Look at the board and what its\n"
	.string "partner makes possible. That's how\l"
	.string "the CIRCUIT keeps surprising me.$"
```

### `BattleFrontier_BattleFactoryLobby_Text_SwappedForWeakMon`

Source: [data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc:483](../baseline/source/data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc#L483)

Current: I swapped for a weak POKéMON… /  I thought it was a good kind to have… //  They wiped the floor with us…

Final text:

```asm
BattleFrontier_BattleFactoryLobby_Text_SwappedForWeakMon:
	.string "I changed one member of my team\n"
	.string "and lost the support another\l"
	.string "partner depended on.\p"
	.string "The new Pokémon wasn't weak. My\n"
	.string "combination was incomplete.$"
```

### `BattleFrontier_BattleFactoryLobby_Text_CantFigureOutStaffHints`

Source: [data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc:495](../baseline/source/data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc#L495)

Current: You know how the staff here give you /  a few hints about your next opponent? //  Well, I'm a full-grown man, but I have /  trouble figuring out their hints.

Final text:

```asm
BattleFrontier_BattleFactoryLobby_Text_CantFigureOutStaffHints:
	.string "A name tells you which Pokémon\n"
	.string "you're facing. It doesn't tell you\l"
	.string "every choice it can make.\p"
	.string "I keep that in mind when the\n"
	.string "CIRCUIT introduces an opponent.$"
```

### `BattleFrontier_BattlePikeLobby_Text_OneRoomAwayFromGoal`

Source: [data/maps/BattleFrontier_BattlePikeLobby/scripts.inc:391](../baseline/source/data/maps/BattleFrontier_BattlePikeLobby/scripts.inc#L391)

Current: Arrgh! I blew my chance! /  I was one room away from the goal! //  In this place, you'd better watch out /  for poison, freezing, and so on.

Final text:

```asm
BattleFrontier_BattlePikeLobby_Text_OneRoomAwayFromGoal:
	.string "One more victory would have beaten\n"
	.string "my Circuit record.\p"
	.string "A status move stopped me. I left\n"
	.string "that answer out of my team.$"
```

### `BattleFrontier_BattlePikeLobby_Text_NeverHadToBattleTrainer`

Source: [data/maps/BattleFrontier_BattlePikeLobby/scripts.inc:397](../baseline/source/data/maps/BattleFrontier_BattlePikeLobby/scripts.inc#L397)

Current: I've completed the challenge 10 times /  now, but I've never had to battle /  a TRAINER once.

Final text:

```asm
BattleFrontier_BattlePikeLobby_Text_NeverHadToBattleTrainer:
	.string "I used to get lucky in the PIKE.\p"
	.string "The CIRCUIT makes you face a\n"
	.string "complete opposing team every\l"
	.string "match. I like that.$"
```

### `BattleFrontier_BattlePyramidLobby_Text_TrainersNoticeRunning`

Source: [data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc:841](../baseline/source/data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc#L841)

Current: Did you know? //  If you run fast, TRAINERS may notice /  and come after you for a battle. //  So, if you want to avoid TRAINERS, /  don't catch their eyes, but sneak /  cautiously and quietly past them.

Final text:

```asm
BattleFrontier_BattlePyramidLobby_Text_TrainersNoticeRunning:
	.string "The old PYRAMID was a maze. Now I\n"
	.string "study the CIRCUIT here.\p"
	.string "Its opponents can punish a\n"
	.string "careless switch just as quickly as\l"
	.string "a careless attack.$"
```

### `BattleFrontier_BattlePyramidLobby_Text_LostLotOfItems`

Source: [data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc:849](../baseline/source/data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc#L849)

Current: Awaaaaaaarrrrgh! //  I had a whole lot of items, but I lost /  them all when I lost! //  Awaaaaaaarrrrgh!

Final text:

```asm
BattleFrontier_BattlePyramidLobby_Text_LostLotOfItems:
	.string "I used to worry about losing items\n"
	.string "in the old PYRAMID.\p"
	.string "The CIRCUIT restores my team and\n"
	.string "held items after each match. I can\l"
	.string "focus on the battle.$"
```

### `BattleFrontier_PokemonCenter_1F_Text_GoingThroughEveryChallenge`

Source: [data/maps/BattleFrontier_PokemonCenter_1F/scripts.inc:53](../baseline/source/data/maps/BattleFrontier_PokemonCenter_1F/scripts.inc#L53)

Current: Giggle… I'm going to go through every /  challenge with just this baby!

Final text:

```asm
BattleFrontier_PokemonCenter_1F_Text_GoingThroughEveryChallenge:
	.string "I'm bringing SKITTY on my next\n"
	.string "Circuit team!\p"
	.string "Five partners will help this\n"
	.string "little one find its chance.$"
```

### `BattleFrontier_Mart_Text_FacilitiesDontAllowItems`

Source: [data/maps/BattleFrontier_Mart/scripts.inc:59](../baseline/source/data/maps/BattleFrontier_Mart/scripts.inc#L59)

Current: A lot of the BATTLE FRONTIER's /  facilities don't allow the use of items /  during battles. //  That rule makes things tougher than /  they already are!

Final text:

```asm
BattleFrontier_Mart_Text_FacilitiesDontAllowItems:
	.string "Trainer battles here don't allow\n"
	.string "items from the BAG.\p"
	.string "Prepare held items before entering\n"
	.string "the CHAMPIONS CIRCUIT.$"
```

Dependencies: W-CIRCUIT-RULES; preserved legacy maps/records

Acceptance: Read each changed NPC/sign from its real map position. All current-service claims match the Circuit entry, restoration and withdrawal behavior. No new battle or reward is introduced.

<a id="w-free-service-cohesion"></a>

## W-FREE-SERVICE-COHESION — Consolidate paid duplicate preparation into the existing free services

**REPAIR.** The native Frontier tutors still debit BP for moves available free elsewhere, and an evolution counter sells the free Ring archive. Keep the NPCs and their personality, with one owner for legal preparation.

### `BattleFrontier_Lounge7_EventScript_LeftMoveTutor`

Source: [data/maps/BattleFrontier_Lounge7/scripts.inc:5](../baseline/source/data/maps/BattleFrontier_Lounge7/scripts.inc#L5)

Current: lock; faceplayer; setvar VAR_TEMP_C, SCROLL_MULTI_BF_MOVE_TUTOR_1; goto_if_set FLAG_MET_FRONTIER_BEAUTY_MOVE_TUTOR, BattleFrontier_Lounge7_EventScript_AlreadyMetLeftTutor; msgbox BattleFrontier_Lounge7_Text_LeftTutorIntro, MSGBOX_DEFAULT; setflag FLAG_MET_FRONTIER_BEAUTY_MOVE_TUTOR; goto BattleFrontier_Lounge7_EventScript_ChooseLeftTutorMove; end

Final behavior:

Keep a brief left veteran greeting (new text below), close it, then goto Common_EventScript_EmeraldChampionsMoveTutor. Remove this live path to paid BP tutor selection and debit. Existing free legality/selection/cancel behavior is the owner; no second learnset table.

### `BattleFrontier_Lounge7_Text_LeftTutorFree`

Source: [data/maps/BattleFrontier_Lounge7/scripts.inc](../baseline/source/data/maps/BattleFrontier_Lounge7/scripts.inc) — new entry

Final text:

```asm
BattleFrontier_Lounge7_Text_LeftTutorFree:
	.string "Buhahaha! I still know a thing or\n"
	.string "two about beautiful moves.\p"
	.string "The lessons are free now. Let us\n"
	.string "prepare your team.$"
```

### `BattleFrontier_Lounge7_EventScript_RightMoveTutor`

Source: [data/maps/BattleFrontier_Lounge7/scripts.inc:127](../baseline/source/data/maps/BattleFrontier_Lounge7/scripts.inc#L127)

Current: lock; faceplayer; setvar VAR_TEMP_C, SCROLL_MULTI_BF_MOVE_TUTOR_2; goto_if_set FLAG_MET_FRONTIER_SWIMMER_MOVE_TUTOR, BattleFrontier_Lounge7_EventScript_AlreadyMetRightTutor; msgbox BattleFrontier_Lounge7_Text_RightTutorIntro, MSGBOX_DEFAULT; setflag FLAG_MET_FRONTIER_SWIMMER_MOVE_TUTOR; goto BattleFrontier_Lounge7_EventScript_ChooseRightTutorMove; end

Final behavior:

Keep a brief right veteran greeting (new text below), close it, then goto Common_EventScript_EmeraldChampionsMoveTutor. Remove this live path to paid BP tutor selection and debit. Existing free legality/selection/cancel behavior is the owner; no second learnset table.

### `BattleFrontier_Lounge7_Text_RightTutorFree`

Source: [data/maps/BattleFrontier_Lounge7/scripts.inc](../baseline/source/data/maps/BattleFrontier_Lounge7/scripts.inc) — new entry

Final text:

```asm
BattleFrontier_Lounge7_Text_RightTutorFree:
	.string "Ihihihi! Experience makes every\n"
	.string "move worth another look.\p"
	.string "The lessons are free now. Let us\n"
	.string "prepare your team.$"
```

### `BattleFrontier_ExchangeServiceCorner_EventScript_EvolutionClerk`

Source: [data/maps/BattleFrontier_ExchangeServiceCorner/scripts.inc:255](../baseline/source/data/maps/BattleFrontier_ExchangeServiceCorner/scripts.inc#L255)

Current: lock; faceplayer; setvar VAR_TEMP_2, EXCHANGE_CORNER_EVOLUTION_CLERK; call BattleFrontier_ExchangeServiceCorner_EventScript_ClerkWelcome; goto BattleFrontier_ExchangeServiceCorner_EventScript_ChooseEvolutionItem; end

Final behavior:

Explain that the evolution archive is free with the Mega Ring, then goto Common_EventScript_EmeraldChampionsBattleVendor. Do not take Battle Points for ordinary evolution items. Keep decoration and field-supply BP counters unchanged.

### `BattleFrontier_ExchangeServiceCorner_Text_ItemsWillGetMonTougher`

Source: [data/maps/BattleFrontier_ExchangeServiceCorner/scripts.inc:399](../baseline/source/data/maps/BattleFrontier_ExchangeServiceCorner/scripts.inc#L399)

Current: The items here aren't ordinary held /  battle gear. //  They're rare supplies and evolution /  tools worth saving Battle Points for!

Final text:

```asm
BattleFrontier_ExchangeServiceCorner_Text_ItemsWillGetMonTougher:
	.string "I save Battle Points for supplies\n"
	.string "and decorations.\p"
	.string "Ordinary evolution items are free\n"
	.string "from CENTER vendors once you have\l"
	.string "the MEGA RING.$"
```

Dependencies: Shared Center specialist/vendor; unchanged decoration and field-supply economics

Acceptance: Talk to both veteran tutors and the evolution clerk. Perform a legal edit, cancel a selection, and request an unavailable move; Battle Points never change. Ordinary item/decor purchases retain their current debit-on-success checks.

<a id="w-gambler-retire"></a>

## W-GAMBLER-RETIRE — Retire bets on unavailable formats without losing recorded stakes

**REPAIR.** The gambler’s challenge selection and settlement are indexed to classic facility modes; Circuit does not complete those records. Retire new stakes and settle any existing obligation before ordinary flavor.

### `BattleFrontier_Lounge3_EventScript_Gambler`

Source: [data/maps/BattleFrontier_Lounge3/scripts.inc:8](../baseline/source/data/maps/BattleFrontier_Lounge3/scripts.inc#L8)

Current: lock; faceplayer; goto_if_set FLAG_MET_BATTLE_FRONTIER_GAMBLER, BattleFrontier_Lounge3_EventScript_AlreadyMetGambler; call BattleFrontier_Lounge3_EventScript_CountSilverSymbols; goto_if_le VAR_0x8004, 2, BattleFrontier_Lounge3_EventScript_NotEnoughSilverSymbols; setflag FLAG_MET_BATTLE_FRONTIER_GAMBLER; msgbox BattleFrontier_Lounge3_Text_YouLookToughExplainGambling, MSGBOX_DEFAULT; goto BattleFrontier_Lounge3_EventScript_AskToEnterChallenge; end

Final behavior:

Before all old introduction/gate branches, inspect VAR_FRONTIER_GAMBLER_STATE and AMOUNT_BET. WAITING/no placed wager: show new observer text and end, never debit BP. PLACED_BET: refund5/10/15 according to the existing validated amount enum. WON: pay the recorded10/20/30 owed by the old win. LOST: preserve the recorded result, clear only this resolved wager state, and show outcome text. For a refund/payment that would exceed MAX_BATTLE_FRONTIER_POINTS, leave the complete obligation pending and ask player to make room by spending BP; never clamp away the balance. Once the full exact amount is credited, set state WAITING and clear the obsolete selected wager; repeated interaction cannot pay again. Invalid historical amount/state is not authority to invent funds; show neutral unavailable-record text and preserve data for repair review.

### `BattleFrontier_Lounge3_Text_CurrentObserver`

Source: [data/maps/BattleFrontier_Lounge3/scripts.inc](../baseline/source/data/maps/BattleFrontier_Lounge3/scripts.inc) — new entry

Final text:

```asm
BattleFrontier_Lounge3_Text_CurrentObserver:
	.string "I used to wager on the old\n"
	.string "Frontier events.\p"
	.string "Now I watch the CIRCUIT and argue\n"
	.string "about teams. It's cheaper...\l"
	.string "usually.$"
```

### `BattleFrontier_Lounge3_Text_SettlementPending`

Source: [data/maps/BattleFrontier_Lounge3/scripts.inc](../baseline/source/data/maps/BattleFrontier_Lounge3/scripts.inc) — new entry

Final text:

```asm
BattleFrontier_Lounge3_Text_SettlementPending:
	.string "Your old wager is still recorded.\p"
	.string "Spend a few Battle Points, then\n"
	.string "return. I will settle the full\l"
	.string "amount without losing any of it.$"
```

Dependencies: Existing VAR_FRONTIER_GAMBLER_STATE/AMOUNT_BET; MAX_BATTLE_FRONTIER_POINTS=9999

Acceptance: Test no wager, each placed stake, each recorded win, a recorded loss, cap-insufficient credit space and repeated visit/reload. Credits match the original obligation exactly; no new bets or Circuit milestones created.

<a id="w-intro-geometry"></a>

## W-INTRO-GEOMETRY — Show both rescue opponents without breaking the opening choreography

**REVISE.** INTRO-01 introduces a scripted two-starter rescue against Poochyena and Zigzagoon. The new field actor must match that battle while preserving the existing approach tracks and access to Birch’s bag.

### `new LOCALID_ROUTE101_POOCHYENA`

Source: [data/maps/Route101/map.json](../baseline/source/data/maps/Route101/map.json) — new entry

Current: The snapshot rescue has Birch(9,13 template), Zigzagoon(10,13 template), bag(7,14), two player triggers(10/11,19); the scene repositions its moving actors before the chase.

Final behavior:

Append the local object ID; reuse Birch’s rescue visibility lifecycle and do not add a second permanent hide flag.

Final object:

```json
{
  "local_id": "LOCALID_ROUTE101_POOCHYENA",
  "graphics_id": "OBJ_EVENT_GFX_POOCHYENA",
  "x": 6,
  "y": 14,
  "elevation": 3,
  "movement_type": "MOVEMENT_TYPE_FACE_UP",
  "movement_range_x": 0,
  "movement_range_y": 0,
  "trainer_type": "TRAINER_TYPE_NONE",
  "trainer_sight_or_berry_tree_id": "0",
  "script": "0",
  "flag": "FLAG_HIDE_ROUTE_101_BIRCH_ZIGZAGOON_BATTLE"
}
```

### `Route101_EventScript_StartBirchRescue`

Source: [data/maps/Route101/scripts.inc:19](../baseline/source/data/maps/Route101/scripts.inc#L19)

Current: lockall; playbgm MUS_HELP, TRUE; msgbox Route101_Text_HelpMe, MSGBOX_DEFAULT; closemessage; setobjectxy LOCALID_ROUTE101_BIRCH, 0, 15; setobjectxy LOCALID_ROUTE101_ZIGZAGOON, 0, 16; applymovement LOCALID_PLAYER, Route101_Movement_EnterScene; applymovement LOCALID_ROUTE101_BIRCH, Route101_Movement_BirchRunAway1; applymovement LOCALID_ROUTE101_ZIGZAGOON, Route101_Movement_ZigzagoonChase1; waitmovement 0; applymovement LOCALID_ROUTE101_ZIGZAGOON, Route101_Movement_ZigzagoonChaseInCircles; applymovement LOCALID_ROUTE101_BIRCH, Route101_Movement_BirchRunInCircles; waitmovement 0; applymovement LOCALID_ROUTE101_BIRCH, Common_Movement_WalkInPlaceFasterRight; waitmovement 0; applymovement LOCALID_ROUTE101_ZIGZAGOON, Route101_Movement_ZigzagoonFaceBirch; applymovement LOCALID_ROUTE101_BIRCH, Route101_Movement_BirchFaceZigzagoon; waitmovement 0; msgbox Route101_Text_PleaseHelp, MSGBOX_DEFAULT; closemessage; setvar VAR_ROUTE101_STATE, 2; releaseall; end

Final behavior:

Keep existing player/Birch/Zigzagoon movement and synchronization. After their circle chase and final facing movements have completed, move LOCALID_ROUTE101_POOCHYENA one ordinary step up from(6,14) to(6,13), then face left; wait for its movement. At rest Birch is(4,13), Zigzagoon(5,13), Poochyena(6,13). The bag remains(7,14). INTRO-01 supplies the two-ball help line and battle selection.

### `Route101_Movement_PoochyenaCloseIn`

Source: [data/maps/Route101/scripts.inc](../baseline/source/data/maps/Route101/scripts.inc) — new entry

Final behavior:

walk_up
face_left
step_end

### `Route101_EventScript_BirchsBag`

Source: [data/maps/Route101/scripts.inc:218](../baseline/source/data/maps/Route101/scripts.inc#L218)

Current: lock; faceplayer; setflag FLAG_SYS_POKEMON_GET; setflag FLAG_RESCUED_BIRCH; fadescreen FADE_TO_BLACK; removeobject LOCALID_ROUTE101_ZIGZAGOON; setobjectxy LOCALID_PLAYER, 6, 13; applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft; waitmovement 0; special ChooseStarter; applymovement LOCALID_ROUTE101_BIRCH, Route101_Movement_BirchApproachPlayer; waitmovement 0; msgbox Route101_Text_YouSavedMe, MSGBOX_DEFAULT; special HealPlayerParty; setflag FLAG_HIDE_ROUTE_101_BIRCH_ZIGZAGOON_BATTLE; clearflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_BIRCH; setflag FLAG_HIDE_ROUTE_101_BIRCH_STARTERS_BAG; setvar VAR_BIRCH_LAB_STATE, 2; setvar VAR_ROUTE101_STATE, 3; clearflag FLAG_HIDE_MAP_NAME_POPUP; checkplayergender; call_if_eq VAR_RESULT, MALE, Route101_EventScript_HideMayInBedroom; call_if_eq VAR_RESULT, FEMALE, Route101_EventScript_HideBrendanInBedroom; warp MAP_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB, 6, 5; waitstate; release; end

Final behavior:

Apply INTRO-01 selection/battle state. Remove both LOCALID_ROUTE101_ZIGZAGOON and LOCALID_ROUTE101_POOCHYENA during the post-win fade before moving the player to(6,13). Hide the rescue actor group only after the agreed legitimate rescue success. Cancellation and loss retain both field foes and a reachable bag. On a loss returning to the field, relocate the player while faded/locked to(7,15), face up, before restoring foes at(5,13)/(6,13) and Birch at(4,13); retain VAR_ROUTE101_STATE=2. Only then fade in and release. The standby tile is passable elevation3, directly south of the bag and contains no trigger or other object.

Dependencies: INTRO-01 authored by early battle volume; ordinary wild/static captures remain singles

Acceptance: Source geometry: both newactor cells are passable collision0/elevation3; the initial cell is off the chase track; the final cell is used only after the chase completes; bag is adjacent. Runtime: enter from each trigger(10,19)/(11,19); observe whole chase, approach bag from south/east, cancel starterchoice, lose/retry, win, and revisit. Confirm no actor overlap, blocked bag, duplicated Pokémon, or premature flags.

<a id="w-frontier-ancillary"></a>

## W-FRONTIER-ANCILLARY — Align remaining active NPC services with Circuit

**REPAIR.** Some retained lobby NPCs and record surfaces still offer rules or forecasts for unavailable facility modes. Keep their historical context while removing actionable misinformation.

### `BattleFrontier_BattlePyramidLobby_EventScript_HintGiver`

Source: [data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc:209](../baseline/source/data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc#L209)

Current: lockall; applymovement LOCALID_PYRAMID_LOBBY_HINT_GIVER, Common_Movement_FacePlayer; waitmovement 0; msgbox BattleFrontier_BattlePyramidLobby_Text_TellYouWhatMisfortunesAwait, MSGBOX_DEFAULT; call BattleFrontier_BattlePyramidLobby_EventScript_GiveHint; msgbox BattleFrontier_BattlePyramidLobby_Text_BelieveMyFortunesOrNot, MSGBOX_DEFAULT; releaseall; end

Final behavior:

Replace dynamic GetBattlePyramidHint forecast with a single current observer message below. Do not pretend the retired random-maze floor forecast describes the next Circuit opponent.

### `BattleFrontier_BattlePyramidLobby_Text_CircuitObserver`

Source: [data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc](../baseline/source/data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc) — new entry

Final text:

```asm
BattleFrontier_BattlePyramidLobby_Text_CircuitObserver:
	.string "I once read the PYRAMID's omens.\p"
	.string "The CIRCUIT asks a different\n"
	.string "question: how will your team\l"
	.string "answer the board before it?\p"
	.string "A prediction helps. A sound\n"
	.string "fallback helps even more.$"
```

### `BattleFrontier_BattleArenaLobby_Text_OrderOfMonsImportant`

Source: [data/maps/BattleFrontier_BattleArenaLobby/scripts.inc:507](../baseline/source/data/maps/BattleFrontier_BattleArenaLobby/scripts.inc#L507)

Current: In the BATTLE ARENA, the order of /  POKéMON is totally important. //  For example, if your first POKéMON /  has certain type disadvantages, /  try making your second POKéMON one /  with moves that are super effective /  against the first one. //  I think that will be a good way of /  making an effective team.

Final text:

```asm
BattleFrontier_BattleArenaLobby_Text_OrderOfMonsImportant:
	.string "Your first two Pokémon begin the\n"
	.string "Circuit battle together.\p"
	.string "Make the opening pair support one\n"
	.string "another, and keep useful answers\l"
	.string "in reserve.$"
```

### `BattleFrontier_BattleArenaLobby_Text_LandingHitsWorked`

Source: [data/maps/BattleFrontier_BattleArenaLobby/scripts.inc:496](../baseline/source/data/maps/BattleFrontier_BattleArenaLobby/scripts.inc#L496)

Current: I won in judging! //  Landing hits consistently on /  the opponent's POKéMON worked!

Final text:

```asm
BattleFrontier_BattleArenaLobby_Text_LandingHitsWorked:
	.string "I kept finding safe chances to\n"
	.string "land my attacks.\p"
	.string "That won my last Circuit match!$"
```

### `BattleFrontier_BattleArenaLobby_Text_MatchWasDeclaredDraw`

Source: [data/maps/BattleFrontier_BattleArenaLobby/scripts.inc:501](../baseline/source/data/maps/BattleFrontier_BattleArenaLobby/scripts.inc#L501)

Current: Our match was declared a draw. //  When we ran out of time, both my /  POKéMON and the opponent's had about /  the same amount of HP left.

Final text:

```asm
BattleFrontier_BattleArenaLobby_Text_MatchWasDeclaredDraw:
	.string "The old ARENA judged a match after\n"
	.string "three turns.\p"
	.string "The CIRCUIT lets a longer plan\n"
	.string "play out. You still have to\l"
	.string "survive those first turns.$"
```

### `BattleFrontier_BattleArenaLobby_Text_BadIdeaToNotAttack`

Source: [data/maps/BattleFrontier_BattleArenaLobby/scripts.inc:491](../baseline/source/data/maps/BattleFrontier_BattleArenaLobby/scripts.inc#L491)

Current: I lost on the REFEREE's decision… //  I don't think it was a good idea to only /  use defensive moves and not attack…

Final text:

```asm
BattleFrontier_BattleArenaLobby_Text_BadIdeaToNotAttack:
	.string "I protected every turn and never\n"
	.string "built a winning position.\p"
	.string "Next time, my defensive choices\n"
	.string "need to create an opportunity.$"
```

### `BattleFrontier_OutsideWest_Text_ChooseFishingOverBattling`

Source: [data/maps/BattleFrontier_OutsideWest/scripts.inc:415](../baseline/source/data/maps/BattleFrontier_OutsideWest/scripts.inc#L415)

Current: I believe I'm the only person here who, /  for some unknown reason, would choose /  fishing over battling. //  Huh? You can't catch anything here? /  That's disappointing…

Final text:

```asm
BattleFrontier_OutsideWest_Text_ChooseFishingOverBattling:
	.string "I came to fish instead of battle.\p"
	.string "I'll try the water and see what\n"
	.string "bites. A good partner can be its\l"
	.string "own reward.$"
```

### `BattleFrontier_Lounge2_EventScript_FrontierManiac`

Source: [data/maps/BattleFrontier_Lounge2/scripts.inc:10](../baseline/source/data/maps/BattleFrontier_Lounge2/scripts.inc#L10)

Current: lock; faceplayer; goto_if_set FLAG_MET_BATTLE_FRONTIER_MANIAC, BattleFrontier_Lounge2_EventScript_AlreadyMetManiac; setflag FLAG_MET_BATTLE_FRONTIER_MANIAC; msgbox BattleFrontier_Lounge2_Text_FrontierManiacIntro, MSGBOX_DEFAULT; goto BattleFrontier_Lounge2_EventScript_GiveAdvice; end

Final behavior:

Keep greeting and personality, replace old ShowFrontierManiacMessage selection with current Circuit observer text below. Do not recommend an unoffered singles/rental/Palace mode.

### `BattleFrontier_Lounge2_Text_CurrentCircuitNews`

Source: [data/maps/BattleFrontier_Lounge2/scripts.inc](../baseline/source/data/maps/BattleFrontier_Lounge2/scripts.inc) — new entry

Final text:

```asm
BattleFrontier_Lounge2_Text_CurrentCircuitNews:
	.string "Here's the latest: every desk\n"
	.string "enters the CHAMPIONS CIRCUIT!\p"
	.string "Different buildings, one central\n"
	.string "arena, and plenty of different\l"
	.string "teams to argue about.$"
```

Dependencies: W-CIRCUIT-RULES; preserved architecture and archived records

Acceptance: Talk to every listed resident and invoke HintGiver repeatedly; no retired-mode forecast or challenge is advertised. Fishing statement remains neutral regardless of exact acquisition-table placement.

<a id="w-ereader-native-retire"></a>

## W-EREADER-NATIVE-RETIRE — Keep legacy houses while retiring unsupported native trainer battles

**REPAIR.** The live Sootopolis visitor path chooses three Pokémon and starts SPECIAL_BATTLE_EREADER. The Mossdeep RS Mystery Event door exposes a similar legacy route. No native trainer singles is the explicit final rule.

### `SootopolisCity_MysteryEventsHouse_1F_EventScript_OldMan`

Source: [data/maps/SootopolisCity_MysteryEventsHouse_1F/scripts.inc:68](../baseline/source/data/maps/SootopolisCity_MysteryEventsHouse_1F/scripts.inc#L68)

Current: lock; faceplayer; frontier_checkvisittrainer; goto_if_eq VAR_RESULT, 1, SootopolisCity_MysteryEventsHouse_1F_EventScript_InvalidVisitingTrainer; goto_if_eq VAR_TEMP_1, 1, SootopolisCity_MysteryEventsHouse_1F_EventScript_TrainerVisiting; msgbox SootopolisCity_MysteryEventsHouse_1F_Text_OnlyAmusementWatchingBattles, MSGBOX_DEFAULT; release; end

Final behavior:

Replace active visitor challenge dispatch with a harmless conversation pointing to the League/Frontier. Do not call SavePlayerParty, ChooseHalfPartyForBattle, ReducePlayerPartyToSelectedMons or warp to the battle basement. Preserve stored e-Reader team data.

### `SootopolisCity_MysteryEventsHouse_1F_OnTransition`

Source: [data/maps/SootopolisCity_MysteryEventsHouse_1F/scripts.inc:6](../baseline/source/data/maps/SootopolisCity_MysteryEventsHouse_1F/scripts.inc#L6)

Current: frontier_checkvisittrainer; call_if_eq VAR_RESULT, 0, SootopolisCity_MysteryEventsHouse_1F_EventScript_SetTrainerVisitingLayout; call_if_ne VAR_SOOTOPOLIS_MYSTERY_EVENTS_STATE, 0, SootopolisCity_MysteryEventsHouse_1F_EventScript_MoveOldManToDoor; end

Final behavior:

Keep the normal house layout and ordinary old-man placement. Do not open the basement based on imported visitor validity. Preserve IDs and data. An already saved legacy in-progress basement session must restore its saved player party and return safely to the 1F entrance without starting another battle; do not alter its recorded historical result.

### `SootopolisCity_MysteryEventsHouse_B1F_OnFrame`

Source: [data/maps/SootopolisCity_MysteryEventsHouse_B1F/scripts.inc:10](../baseline/source/data/maps/SootopolisCity_MysteryEventsHouse_B1F/scripts.inc#L10)

Current: map_script_2 VAR_TEMP_1, 0, SootopolisCity_MysteryEventsHouse_B1F_EventScript_BattleVisitingTrainer; .2byte 0

Final behavior:

Replace native battle-start entry with recovery-only handling when reached from a pre-revision in-progress save: restore original saved party if its backup is valid, reset only the retired session state, and return to1F(3,1). Never construct an e-Reader opponent or fabricate a battle outcome.

### `SootopolisCity_MysteryEventsHouse_1F_Text_CurrentBattles`

Source: [data/maps/SootopolisCity_MysteryEventsHouse_1F/scripts.inc](../baseline/source/data/maps/SootopolisCity_MysteryEventsHouse_1F/scripts.inc) — new entry

Final text:

```asm
SootopolisCity_MysteryEventsHouse_1F_Text_CurrentBattles:
	.string "I still enjoy a fine Pokémon\n"
	.string "battle. These old visitor records\l"
	.string "bring back good memories.\p"
	.string "For a new challenge, seek the\n"
	.string "League or the Frontier's CHAMPIONS\l"
	.string "CIRCUIT.$"
```

### `RS_MysteryEventsHouse_EventScript_Door`

Source: [data/maps/MossdeepCity_GameCorner_1F/scripts.inc:37](../baseline/source/data/maps/MossdeepCity_GameCorner_1F/scripts.inc#L37)

Current: msgbox RS_MysteryEventsHouse_Text_DoorIsLocked, MSGBOX_SIGN; end

Final behavior:

Retain the locked-door flavor; do not expose an imported trainer challenge through this native home. Preserve externally stored data and the harmless minigame record displays in Mossdeep.

Dependencies: No native trainer singles; shared save/party-restoration integrity; external link-format decisions owned centrally

Acceptance: New game and imported-record save: neither house starts a trainer battle. A valid existing legacy in-progress save restores the original party and exits; invalid backup is surfaced as an integrity failure, never overwritten. Records/minigame viewing and harmless residents remain usable.

<a id="w-circuit-records"></a>

## W-CIRCUIT-RECORDS — Show actual Circuit progress on current record boards

**REPAIR.** Existing monitors read retired mode counters. A current standard competition needs records tied to the same state that governs its results and entitlements, without a second scoreboard state.

### `BattleFrontier_BattleTowerLobby_EventScript_ShowSinglesResults`

Source: [data/maps/BattleFrontier_BattleTowerLobby/scripts.inc:631](../baseline/source/data/maps/BattleFrontier_BattleTowerLobby/scripts.inc#L631)

Current: lockall; frontier_results FRONTIER_FACILITY_TOWER, FRONTIER_MODE_SINGLES; waitbuttonpress; special RemoveRecordsWindow; releaseall; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_BattleTowerLobby_EventScript_ShowDoublesResults`

Source: [data/maps/BattleFrontier_BattleTowerLobby/scripts.inc:639](../baseline/source/data/maps/BattleFrontier_BattleTowerLobby/scripts.inc#L639)

Current: lockall; frontier_results FRONTIER_FACILITY_TOWER, FRONTIER_MODE_DOUBLES; waitbuttonpress; special RemoveRecordsWindow; releaseall; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_BattleTowerLobby_EventScript_ShowMultisResults`

Source: [data/maps/BattleFrontier_BattleTowerLobby/scripts.inc:647](../baseline/source/data/maps/BattleFrontier_BattleTowerLobby/scripts.inc#L647)

Current: lockall; frontier_results FRONTIER_FACILITY_TOWER, FRONTIER_MODE_MULTIS; waitbuttonpress; special RemoveRecordsWindow; releaseall; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_BattleTowerLobby_EventScript_ShowLinkMultisResults`

Source: [data/maps/BattleFrontier_BattleTowerLobby/scripts.inc:655](../baseline/source/data/maps/BattleFrontier_BattleTowerLobby/scripts.inc#L655)

Current: lockall; frontier_results FRONTIER_FACILITY_TOWER, FRONTIER_MODE_LINK_MULTIS; waitbuttonpress; special RemoveRecordsWindow; releaseall; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_BattleDomeLobby_EventScript_ShowSinglesResults`

Source: [data/maps/BattleFrontier_BattleDomeLobby/scripts.inc:327](../baseline/source/data/maps/BattleFrontier_BattleDomeLobby/scripts.inc#L327)

Current: lockall; frontier_results FRONTIER_FACILITY_DOME, FRONTIER_MODE_SINGLES; waitbuttonpress; special RemoveRecordsWindow; releaseall; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_BattleDomeLobby_EventScript_ShowPrevTourneyTree`

Source: [data/maps/BattleFrontier_BattleDomeLobby/scripts.inc:343](../baseline/source/data/maps/BattleFrontier_BattleDomeLobby/scripts.inc#L343)

Current: dome_get DOME_DATA_PREV_TOURNEY_TYPE; call_if_eq VAR_RESULT, 0, BattleFrontier_BattleDomeLobby_EventScript_PrevTourneyResultsSinglesLv50; call_if_eq VAR_RESULT, 1, BattleFrontier_BattleDomeLobby_EventScript_PrevTourneyResultsDoublesLv50; call_if_eq VAR_RESULT, 2, BattleFrontier_BattleDomeLobby_EventScript_PrevTourneyResultsSinglesLvOpen; call_if_eq VAR_RESULT, 3, BattleFrontier_BattleDomeLobby_EventScript_PrevTourneyResultsDoublesLvOpen; fadescreen FADE_TO_BLACK; dome_showprevtourneytree; waitstate; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_BattleDomeLobby_EventScript_ShowDoublesResults`

Source: [data/maps/BattleFrontier_BattleDomeLobby/scripts.inc:335](../baseline/source/data/maps/BattleFrontier_BattleDomeLobby/scripts.inc#L335)

Current: lockall; frontier_results FRONTIER_FACILITY_DOME, FRONTIER_MODE_DOUBLES; waitbuttonpress; special RemoveRecordsWindow; releaseall; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_BattlePalaceLobby_EventScript_ShowSinglesResults`

Source: [data/maps/BattleFrontier_BattlePalaceLobby/scripts.inc:299](../baseline/source/data/maps/BattleFrontier_BattlePalaceLobby/scripts.inc#L299)

Current: lockall; frontier_results FRONTIER_FACILITY_PALACE, FRONTIER_MODE_SINGLES; waitbuttonpress; special RemoveRecordsWindow; releaseall; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_BattlePalaceLobby_EventScript_ShowDoublesResults`

Source: [data/maps/BattleFrontier_BattlePalaceLobby/scripts.inc:307](../baseline/source/data/maps/BattleFrontier_BattlePalaceLobby/scripts.inc#L307)

Current: lockall; frontier_results FRONTIER_FACILITY_PALACE, FRONTIER_MODE_DOUBLES; waitbuttonpress; special RemoveRecordsWindow; releaseall; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_BattlePyramidLobby_EventScript_ShowResults`

Source: [data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc:364](../baseline/source/data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc#L364)

Current: lockall; frontier_results FRONTIER_FACILITY_PYRAMID; waitbuttonpress; special RemoveRecordsWindow; releaseall; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_BattleArenaLobby_EventScript_ShowResults`

Source: [data/maps/BattleFrontier_BattleArenaLobby/scripts.inc:292](../baseline/source/data/maps/BattleFrontier_BattleArenaLobby/scripts.inc#L292)

Current: lockall; frontier_results FRONTIER_FACILITY_ARENA; waitbuttonpress; special RemoveRecordsWindow; releaseall; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_BattleFactoryLobby_EventScript_ShowSinglesResults`

Source: [data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc:242](../baseline/source/data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc#L242)

Current: lockall; frontier_results FRONTIER_FACILITY_FACTORY, FRONTIER_MODE_SINGLES; waitbuttonpress; special RemoveRecordsWindow; releaseall; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_BattleFactoryLobby_EventScript_ShowDoublesResults`

Source: [data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc:250](../baseline/source/data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc#L250)

Current: lockall; frontier_results FRONTIER_FACILITY_FACTORY, FRONTIER_MODE_DOUBLES; waitbuttonpress; special RemoveRecordsWindow; releaseall; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_BattlePikeLobby_EventScript_ShowResults`

Source: [data/maps/BattleFrontier_BattlePikeLobby/scripts.inc:197](../baseline/source/data/maps/BattleFrontier_BattlePikeLobby/scripts.inc#L197)

Current: lockall; frontier_results FRONTIER_FACILITY_PIKE; waitbuttonpress; special RemoveRecordsWindow; releaseall; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_RankingHall_EventScript_TowerSinglesRecords`

Source: [data/maps/BattleFrontier_RankingHall/scripts.inc:4](../baseline/source/data/maps/BattleFrontier_RankingHall/scripts.inc#L4)

Current: lockall; setvar VAR_0x8005, RANKING_HALL_TOWER_SINGLES; goto BattleFrontier_RankingHall_EventScript_ShowRecords; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_RankingHall_EventScript_TowerDoublesRecords`

Source: [data/maps/BattleFrontier_RankingHall/scripts.inc:10](../baseline/source/data/maps/BattleFrontier_RankingHall/scripts.inc#L10)

Current: lockall; setvar VAR_0x8005, RANKING_HALL_TOWER_DOUBLES; goto BattleFrontier_RankingHall_EventScript_ShowRecords; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_RankingHall_EventScript_TowerMultisRecords`

Source: [data/maps/BattleFrontier_RankingHall/scripts.inc:16](../baseline/source/data/maps/BattleFrontier_RankingHall/scripts.inc#L16)

Current: lockall; setvar VAR_0x8005, RANKING_HALL_TOWER_MULTIS; goto BattleFrontier_RankingHall_EventScript_ShowRecords; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_RankingHall_EventScript_TowerLinkRecords`

Source: [data/maps/BattleFrontier_RankingHall/scripts.inc:22](../baseline/source/data/maps/BattleFrontier_RankingHall/scripts.inc#L22)

Current: lockall; setvar VAR_0x8005, RANKING_HALL_TOWER_LINK; goto BattleFrontier_RankingHall_EventScript_ShowRecords; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_RankingHall_EventScript_ArenaRecords`

Source: [data/maps/BattleFrontier_RankingHall/scripts.inc:28](../baseline/source/data/maps/BattleFrontier_RankingHall/scripts.inc#L28)

Current: lockall; setvar VAR_0x8005, RANKING_HALL_ARENA; goto BattleFrontier_RankingHall_EventScript_ShowRecords; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_RankingHall_EventScript_PalaceRecords`

Source: [data/maps/BattleFrontier_RankingHall/scripts.inc:34](../baseline/source/data/maps/BattleFrontier_RankingHall/scripts.inc#L34)

Current: lockall; setvar VAR_0x8005, RANKING_HALL_PALACE; goto BattleFrontier_RankingHall_EventScript_ShowRecords; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_RankingHall_EventScript_FactoryRecords`

Source: [data/maps/BattleFrontier_RankingHall/scripts.inc:40](../baseline/source/data/maps/BattleFrontier_RankingHall/scripts.inc#L40)

Current: lockall; setvar VAR_0x8005, RANKING_HALL_FACTORY; goto BattleFrontier_RankingHall_EventScript_ShowRecords; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_RankingHall_EventScript_DomeRecords`

Source: [data/maps/BattleFrontier_RankingHall/scripts.inc:46](../baseline/source/data/maps/BattleFrontier_RankingHall/scripts.inc#L46)

Current: lockall; setvar VAR_0x8005, RANKING_HALL_DOME; goto BattleFrontier_RankingHall_EventScript_ShowRecords; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_RankingHall_EventScript_PikeRecords`

Source: [data/maps/BattleFrontier_RankingHall/scripts.inc:52](../baseline/source/data/maps/BattleFrontier_RankingHall/scripts.inc#L52)

Current: lockall; setvar VAR_0x8005, RANKING_HALL_PIKE; goto BattleFrontier_RankingHall_EventScript_ShowRecords; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_RankingHall_EventScript_PyramidRecords`

Source: [data/maps/BattleFrontier_RankingHall/scripts.inc:58](../baseline/source/data/maps/BattleFrontier_RankingHall/scripts.inc#L58)

Current: lockall; setvar VAR_0x8005, RANKING_HALL_PYRAMID; goto BattleFrontier_RankingHall_EventScript_ShowRecords; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_RankingHall_EventScript_DomePikeFactoryRecordsSign`

Source: [data/maps/BattleFrontier_RankingHall/scripts.inc:77](../baseline/source/data/maps/BattleFrontier_RankingHall/scripts.inc#L77)

Current: msgbox BattleFrontier_RankingHall_Text_DomePikeFactoryRecords, MSGBOX_SIGN; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_RankingHall_EventScript_PalaceArenaPyramidRecordsSIgn`

Source: [data/maps/BattleFrontier_RankingHall/scripts.inc:81](../baseline/source/data/maps/BattleFrontier_RankingHall/scripts.inc#L81)

Current: msgbox BattleFrontier_RankingHall_Text_PalaceArenaPyramidRecords, MSGBOX_SIGN; end

Final behavior:

Replace body with: special ChampionsCircuitBufferRecord; msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN; end. FAC-03 binds STR_VAR_1 to current VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, STR_VAR_2 to Best recorded VAR_EC_CIRCUIT_BEST_WINS (display -- when no observed record), STR_VAR_3 to lifetime VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS. Five decimal digits must fit. Viewing never mutates state; retained old facility record data does not become a live challenge.

### `BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord`

Source: [data/maps/BattleFrontier_BattleTowerLobby/scripts.inc](../baseline/source/data/maps/BattleFrontier_BattleTowerLobby/scripts.inc) — new entry

Final text:

```asm
BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord:
	.string "CHAMPIONS CIRCUIT\p"
	.string "Current run: {STR_VAR_1}\p"
	.string "Best recorded: {STR_VAR_2}\p"
	.string "Total wins: {STR_VAR_3}$"
```

### `BattleFrontier_RankingHall_Text_ExplainRankingHall`

Source: [data/maps/BattleFrontier_RankingHall/scripts.inc:103](../baseline/source/data/maps/BattleFrontier_RankingHall/scripts.inc#L103)

Current: This is the RANKING HALL. //  This is where we recognize the immortal /  TRAINERS who left great records in /  BATTLE FRONTIER events.

Final text:

```asm
BattleFrontier_RankingHall_Text_ExplainRankingHall:
	.string "This hall celebrates the\n"
	.string "Frontier's competitive history.\p"
	.string "Today's boards show CHAMPIONS\n"
	.string "CIRCUIT results: the current run,\l"
	.string "best recorded run, and total wins.$"
```

### `BattleFrontier_RankingHall_Text_DomePikeFactoryRecords`

Source: [data/maps/BattleFrontier_RankingHall/scripts.inc:109](../baseline/source/data/maps/BattleFrontier_RankingHall/scripts.inc#L109)

Current: BATTLE DOME, BATTLE PIKE, /  and BATTLE FACTORY Records

Final text:

```asm
BattleFrontier_RankingHall_Text_DomePikeFactoryRecords:
	.string "CHAMPIONS CIRCUIT RECORDS Current\n"
	.string "run, best recorded, total wins$"
```

### `BattleFrontier_RankingHall_Text_PalaceArenaPyramidRecords`

Source: [data/maps/BattleFrontier_RankingHall/scripts.inc:113](../baseline/source/data/maps/BattleFrontier_RankingHall/scripts.inc#L113)

Current: BATTLE PALACE, BATTLE ARENA, /  and BATTLE PYRAMID Records

Final text:

```asm
BattleFrontier_RankingHall_Text_PalaceArenaPyramidRecords:
	.string "CHAMPIONS CIRCUIT RECORDS Current\n"
	.string "run, best recorded, total wins$"
```

Dependencies: FAC-03: CURRENT_WINS0x40DB, TOTAL_WINS0x40DC, proposed BEST_WINS0x40E3; initialize best only from observed currentrun, never infer historical best from lifetime. Legacy record data preserved

Acceptance: After Circuit wins, withdrawal, loss and reload, each current board matches the same authoritative counters. Tent/Hill wins do not change these values. Viewing any record cannot claim rewards, alter streaks or start a battle.
