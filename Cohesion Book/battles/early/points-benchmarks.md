# Early battle Stat Point benchmarks

**Source calculation, not runtime victory evidence.** These numbers use the configured snapshot species metadata and the exact current Champions stat formula. They make the point/nature levers concrete for the opening, Cristian and Roxanne. All source stats below exclude transient battle modifiers unless the row explicitly says otherwise.

## The actual formula and storage order

[src/pokemon.c:1425](../../baseline/source/src/pokemon.c:1425) adds Stat Points after level scaling and before nature. [HP calculation](../../baseline/source/src/pokemon.c:1452) follows the same fixed-point principle. [Nature rounding](../../baseline/source/src/pokemon.c:4807) uses integer multiplication/division.

```text
Non-HP before nature = floor((2 × baseStat + 31) × level / 100) + 5 + points
Nature raised stat    = floor(beforeNature × 110 / 100)
Nature lowered stat   = floor(beforeNature × 90 / 100)
HP                    = floor((2 × baseHP + 31) × level / 100) + level + 10 + HPpoints
```

The engine forces effective IV31 in the Champions branch. A stored Speed IV0 does not make a slow Room Pokémon slower in this build. There is no friendship stat bonus (`B_FRIENDSHIP_BOOST=FALSE`). HP does not receive nature changes, and Shedinja’s special HP rule remains1.

The builder budget is [66 total and32 per stat](../../baseline/source/include/constants/emerald_champions.h:12). Do not use252/510 EV assumptions: those legacy constants still exist but do not describe the Champions preparation contract. A point adds one before nature at level5 just as it does at level100.

The authored/display order is **HP/Atk/Def/SpA/SpD/Spe**. Native `STAT_*` storage order is **HP/Atk/Def/Spe/SpA/SpD**. Use [gEmeraldChampionsStatPointOrder](../../baseline/source/src/emerald_champions_battle_sets.c:548) or `EC_STAT_POINT_DATA`; never write the display vector directly into sequential engine fields.

## Cristian: repairing the actual Beat Up order

|Difficulty/scenario|Falinks level|Gallade level|Old Falinks Speed|Final Falinks Speed|Gallade Speed|Result|
|---|---:|---:|---:|---:|---:|---|
|Easy|16|17|33|71|69|Final Falinks acts first at equal priority|
|Normal/Medium|18|19|37|75|73|Final Falinks acts first at equal priority|
|Hard|20|21|41|80|77|Final Falinks acts first at equal priority|
|Normal/Medium revisit at live cap30|28|28|55|95|90|Final Falinks acts first at equal priority|

At Hard levels20/21: old Falinks Speed is `floor(181×20/100)+5 =41`; final Jolly/32-Speed Falinks is `floor((36+5+32)×110/100)=80`; Gallade is `floor(191×21/100)+5+32=77`. Normal/Medium levels18/19 become37→75 against73. The shared floor does not erase positive authored level offsets, and a later revisit can place both at the live floor.

The exact repair changes Falinks Adamant/PB `32/32/2/0/0/0` to Jolly/PS `2/32/0/0/0/32`. It loses30 HP points,2 Defense points and the Attack-raising nature, gaining32 Speed points plus a Speed nature. That is a deliberate activation trade, not free stats. Gallade and the other five party fields remain unchanged.

This proves unmodified equal-priority order only. Beat Up damage, Gallade survival, all Justified activations, redirection, paralysis, opposing priority and Trick Room must be tested in battle. The AI may use Coaching or an attack instead when the actual board makes Beat Up worse.

## Route103: Light Ball Pikachu and both player starters

|Opponent|Level|Raw HP/Atk/Def/SpA/SpD/Spe|Light Ball offensive values|
|---|---:|---|---|
|Normal/Medium Pikachu|12|36/18/18/52/20/68|Atk 36, SpA 104; Speed unchanged|
|Hard Pikachu|14|40/21/20/55/23/72|Atk 42, SpA 110; Speed unchanged|

Pikachu is Timid `2/0/0/32/0/32`, with Fake Out/Thunderbolt/Electroweb/Protect. Light Ball is a battle offensive modifier, not extra displayed points or Speed. The exact regional starter partner and both owned starters vary by the54-choice table; all54 raw-speed orders are included in [the JSON](points-benchmarks.json).

|Starter|Player Speed at rescue L5|Player Speed at cap14|Rival Speed at Medium L12|Rival Speed at Hard L14|
|---|---:|---:|---:|---:|
|BULBASAUR|47|58|56|58|
|CHARMANDER|49|64|61|64|
|SQUIRTLE|10|21|19|21|
|CHIKORITA|11|21|19|21|
|CYNDAQUIL|49|64|61|64|
|TOTODILE|42|53|51|53|
|TREECKO|13|28|62|66|
|TORCHIC|47|58|56|58|
|MUDKIP|10|20|50|52|
|TURTWIG|45|55|52|55|
|CHIMCHAR|12|26|23|26|
|PIPLUP|10|20|18|20|
|SNIVY|48|63|60|63|
|TEPIG|47|58|56|58|
|OSHAWOTT|47|58|56|58|
|CHESPIN|10|19|17|19|
|FENNEKIN|48|63|60|63|
|FROAKIE|49|67|62|67|
|ROWLET|46|58|55|58|
|LITTEN|49|66|62|66|
|POPPLIO|42|52|50|52|
|GROOKEY|13|27|24|27|
|SCORBUNNY|49|66|62|66|
|SOBBLE|49|66|62|66|
|SPRIGATITO|49|64|61|64|
|FUECOCO|10|19|17|19|
|QUAXLY|43|55|52|55|

The player columns use the exact initial-grant sets; they are editable through the preparation system. The rival columns use the explicit regional Route103 set, including the preserved different Hoenn authored builds. These columns do not assume that the player remains at level5 before the first trainer: the Leveler and preparation services are available beforehand.

A useful example is bulky initial Treecko. Its Berry Juice/Unburden set has Speed28 at level14, rising to56 after Unburden—still below the Medium rival Pikachu’s68. Having a speed-doubling ability is not itself proof of moving first.

|Treecko nature/points|HP|Attack|Raw Speed|After Unburden|Interpretation|
|---|---:|---:|---:|---:|---|
|ADAMANT 32/32/2/0/0/0|71|58|28|56|Initial default; slower than Medium Pikachu|
|ADAMANT 25/32/2/0/0/7|64|58|35|70|Reallocate7HP points to Speed; beats Medium68|
|JOLLY 28/32/2/0/0/4|67|53|35|70|Jolly plus4Speed points; beats Medium68, gives up Adamant Attack|
|ADAMANT 23/32/2/0/0/9|62|58|37|74|Reallocate9HP points to Speed; beats Hard72|

The retained player Torchic initial set has Speed58 at cap14; one Speed Boost reaches87 before other modifiers. Treecko’s alternative investments illustrate choice, not a compulsory solution. Ties are not guaranteed order, and Tailwind, paralysis, stat stages or terrain can change the comparison.

## Roxanne: sleep timing, Carbink and iconic Nosepass

|Mode / Pokémon|Level|HP/Atk/Def/SpA/SpD/Spe|
|---|---:|---|
|Normal/Medium / carbink|14|74/23/53/23/91/20|
|Normal/Medium / relicanth|15|91/81/50/23/29/23|
|Normal/Medium / nosepass|16|72/24/55/24/77/17|
|Normal/Medium / old_onix|16|74/61/63/19/24/28|
|Hard / carbink|16|78/25/59/25/97/22|
|Hard / relicanth|17|98/86/56/25/32/25|
|Hard / nosepass|18|76/26/61/26/81/18|
|Hard / old_onix|18|78/63/70/21/26/31|

The proposed Nosepass keeps Onix’s Eviolite, Sturdy and +4offset but changes its role to special bulk, special Rock/Ground attacks and Wide Guard. Its lower raw offensive damage is an explicit cost for partner protection and the requested signature restoration. Eviolite’s in-battle defense multiplier is not included in the raw table. Compare whole-pair outcomes rather than judging that replacement from one Attack number.

Roxanne’s Carbink has no Overcoat or Safety Goggles. Mental Herb handles eligible mental disruption, not sleep immunity. **Spore’s priority0 already precedes Trick Room’s−7** if the user gets to act; raising Speed is not required merely to precede Carbink choosing Trick Room. Speed can matter against Relicanth’s ordinary-priority attack that might hit the sleeper first. Powder legality, existing status, immunity and damage survival remain separate questions.

|Level14 Shroomish nature/points|HP|Defense|Speed|Purpose|
|---|---:|---:|---:|---|
|BOLD 32/0/32/0/2/0|77|63|19|Bulky baseline;19Speed|
|BOLD 27/0/32/0/2/5|72|63|24|Bold +5Speed points beats Medium Relicanth23|
|TIMID 29/0/32/0/2/3|74|58|24|Timid +3Speed points beats Medium23, losing Bold defense bonus|
|BOLD 25/0/32/0/2/7|70|63|26|Bold +7Speed points beats Hard Relicanth25|
|TIMID 27/0/32/0/2/5|72|58|26|Timid +5Speed points beats Hard25, losing Bold defense bonus|

Each example still totals66points and respects32perstat. The player pays for Speed through fewer HP points, a different nature, or both. None of these rows proves Shroomish survives Head Smash, wins the battle, or is the best sleep option. Those are damage and paired-action questions to test with the actual available roster.

## Priority is a different lever from Speed

|Move|Configured priority|Implication|
|---|---:|---|
|Helping Hand|+5|Can support an ally before most attacks regardless of its raw Speed.|
|Protect|+4|Normally precedes Fake Out and attacks, subject to its other rules.|
|Fake Out / Wide Guard|+3|Their relative order can depend on Speed; Fake Out still requires its entry turn.|
|Extreme Speed|+2|Outruns ordinary moves by priority, not merely a high Speed stat.|
|Quick Attack|+1|Lower priority than Extreme Speed.|
|Spore / Electroweb|0|Ordinary-priority order follows current Speed/field rules.|
|Trick Room|−7|Usually acts after ordinary attacks; once active it reverses Speed order within priority brackets.|

Move priorities are read from `src/data/moves_info.h`; Fake Out/Extreme Speed use the configured modern conditional values. Priority blockers and Psychic Terrain can invalidate moves against applicable targets; a larger Speed number does not bypass them.

## Acceptance boundary

The appendix independently calculates source-defined stats, checks66/32budgets and confirms the quoted arithmetic and all54matrix rows. It does not execute `CalculateMonStats`, compute full damage ranges, simulate enemy AI or prove a winning strategy. Implementation should add focused engine comparisons for these exact vectors and levels, including storedIV0versus31, display-to-engine order, nature rounding, and the post-floor party stats. Runtime scenarios must then verify action order, activation costs, item effects and alternate player solutions.
