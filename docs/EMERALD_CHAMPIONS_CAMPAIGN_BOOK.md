# Emerald Champions: The Complete Campaign

## Executive Summary

- **The campaign has a clear and unusually coherent identity.** It remains Emerald's land-versus-sea story, but every major system now supports one thesis: catching and preparing a team is easy; reading and winning battles is hard. The regional starter choice, immediate National Pokédex, Poké Vial, Leveler, competitive set tutor, free held-item vendor, strict caps, live difficulty control, mostly doubles trainers, Mega progression, and Legendary Signs all reinforce that promise.

- **The playable story spine is structurally connected.** The current source passes the named map, warp, script-reference, story-item, badge, HM, Mega, League, reward-economy, legendary-availability, and dialogue-width invariants. Those static checks are not a playthrough. The checked surface contains 540 Hoenn maps, 4,086 physical events, 1,402 warps, 17,938 resolved references across 104,701 assembled script lines, 391 value-returning special-call contracts, 513 finite physical trainer groups, 561 battle branches, and 82 complete Legendary Sign definitions.

- **The story itself flows well from local discovery to regional catastrophe.** Champion's Signs begin as strange marks in Petalburg Woods and Granite Cave, become a Devon and Steven research problem, are connected to Magma and Aqua's machines at Slateport and Mt. Chimney, become a spiritual network at Mt. Pyre, and finally explain why Groudon, Kyogre, and Rayquaza react so violently. Rayquaza remains the campaign climax rather than being displaced by a new fan-made “true” villain.

- **The battle master now matches the playable campaign.** Its `campaign_order` follows live story prerequisites, places optional fights at their earliest natural opportunity, and closes the League as Sidney, Phoebe, Glacia, Drake, then Wallace before entering the postgame. The master is now safe to use as the chronological source for guide generation and future trainer work.

This book and the [generated evidence atlas](EMERALD_CHAMPIONS_CAMPAIGN_EVIDENCE.txt) have been refreshed against the current source and reordered [master battle design](emerald_champions_master_battle_design.txt). The atlas is the detailed loadout-and-dialogue appendix for this exact campaign snapshot.

## How to Read This Book

The main chapters describe what a player actually does, when paths open, what is mandatory, what is optional, what the story is saying, and why the transitions make sense. The companion evidence atlas supplies the molecular layer that would make the main narrative unreadable:

- every one of the 513 finite physical trainer groups;
- all 561 starter, rival, paired-trainer, and other battle branches;
- every materialized Pokémon, item, Ability, nature, Stat Point spread, and move;
- every trainer's live intro, defeat, and post-battle dialogue that is attached to the campaign script;
- the exact source file and label for that dialogue;
- all 82 Legendary Sign acquisition definitions.

The atlas deliberately excludes rematch-only dialogue from the primary path because ordinary and Gym rematches are disabled campaign content. Repeatable endgame play belongs to the Champions Circuit and Battle Frontier. Its encounter order now matches the reordered master.

## The Rules the Story Assumes

### Level and difficulty progression

Hard is the authored game. Medium leaves every team and AI decision intact but lowers every opposing trainer Pokémon by two levels. Easy lowers them by four. The option is live and can be changed between attempts.

| Campaign state | Player level cap |
| --- | ---: |
| Before the Stone Badge | 14 |
| After the Stone Badge | 20 |
| After the Knuckle Badge | 30 |
| After the Dynamo Badge | 40 |
| After the Heat Badge | 45 |
| After the Balance Badge | 55 |
| After the Feather Badge | 60 |
| After the Mind Badge | 70 |
| After the Rain Badge | 80 |
| After becoming Champion | 100 |

The cap is strict. Experience stops at it, Rare Candies stop at it, and the reusable Leveler brings the whole party to it. The game therefore never asks the player to solve a battle by grinding past its intended regime.

### Team preparation

The first Pokémon Center visit gives two permanent tools:

- The Poké Vial restores the full party outside battle. It begins with one charge, refills at every Center, and can be upgraded through optional exploration.
- The Leveler raises the whole party to the current cap and never runs out.

Every Center also exposes the game's competitive preparation loop. The executable corpus contains 5,311 named presets: 2,619 Doubles and 2,692 Singles, with at least two non-Mega roles per format for every direct species/form. The tutor applies four moves where the species can legally know four, nature, Ability, 66 Stat Points, perfect-IV backing data, and an ordinary held item. Mega-oriented sets explain the required stone but never create it. A free native editor then lets the player customize any 0-32-per-stat, 66-total spread. A separate Center vendor gives 111 ordinary competitive held items for free, while berries, Mega Stones, orbs, drives, masks, memories, plates, and other transformation or progression items remain scarce.

Ordinary table wild Pokémon arrive with one of their species' authored non-Mega Doubles sets. If a species has multiple Doubles presets, its wild orientations are distributed across those presets. Evolution replaces that prior-stage orientation with the evolved species' first non-Mega Doubles recommendation. All 616 species/forms parsed from encounter data resolve a preset; the current Hoenn campaign distribution exposes 592 unique species/forms through 146 encounter headers on 138 maps. Finite Legendary Signs and temporary battle forms remain on their explicit quest paths rather than entering ordinary randomization. This makes a newly caught ordinary Pokémon immediately legible and useful instead of an unfinished project.

### Exploration rewards

Poké Marts retain ordinary campaign progression and sell Rare Candies for $1,000 anywhere medicine is sold. Gyms award badges and one thematic Mega Stone instead of redundant TMs. World TMs and obsolete X-items have been converted into capture, recovery, PP, evolution, fossil, Mega, or story rewards. Berries remain a real cultivation economy.

The complete Mega Stone and evolution-item archives open at the Center vendor after the eighth badge, before the League. That fulfills the game's promise that every Mega can be used in the authored campaign while preserving discovery value during most of the journey.

# Part I — The First Team

## 1. Moving into Littleroot

The game opens with Emerald's native moving sequence. The player rides in the back of the moving truck, steps into Littleroot, and is welcomed by Mom. The Vigoroth movers are still finishing the house.

The first mandatory action is upstairs: set the wall clock. The stairs remain blocked as an introduction until the clock has been handled. Mom then comes upstairs, asks how the new room feels, and returns downstairs. At the television, the player and Mom narrowly miss an interview with Norman at the Petalburg Gym. Mom directs the player to introduce themselves to Professor Birch's family next door.

This scene is deliberately ordinary. The game does not front-load a new mythology before the player has a home, a parent, and a reason to walk outside.

### Optional before leaving town

- Inspect the PC and bedroom objects.
- Speak to both parents' neighbors and the moving Pokémon.
- Visit Birch's lab before the rescue; the aide explains that Birch prefers fieldwork to desk work.

Source anchors: `data/scripts/players_house.inc`, `data/maps/LittlerootTown/scripts.inc`, and both player-house maps.

## 2. The rival chooses the regional tradition with you

At the rival's house, the rival's parent identifies the upstairs bedroom. Interacting with the Poké Ball triggers the meeting with May or Brendan. This scene now contains the game's first major choice: Kanto, Johto, Hoenn, Sinnoh, Unova, Kalos, Alola, Galar, or Paldea.

The rival explains that Birch studies regional battling traditions and packed the selected trio in his field bag. Whichever starter the player ultimately chooses, the rival will train the same generation's type-advantaged counterpart. That prevents the expanded starter choice from breaking the rival relationship.

The rival then leaves for fieldwork. Littleroot's northern exit is now the obvious next step.

Source anchors: `Common_EventScript_ChooseStarterRegion` in `data/scripts/emerald_champions.inc` and the May/Brendan house scripts.

## 3. Birch's rescue and the first partner

On Route 101, a child warns that someone is being attacked. The player walks north, finds Birch being chased, and opens his field bag. The three balls correspond to the region selected in the rival's room.

The player chooses one starter and wins the scripted rescue battle. Back in the lab, Birch frames the victory as more than luck: the player trusted one tradition immediately and read the battle. He gives the starter permanently and suggests a nickname.

Birch then points toward Route 103. The rival is waiting with the informed type counter. The game explicitly says that losing is information: adapt, return, and make the rival adapt.

## 4. Oldale, the first Center, and Route 103

Route 101 leads to Oldale Town. The Poké Mart employee still provides the native introductory supply. The first Center visit is more important: the nurse gives the Poké Vial and Leveler and explains that both exist to eliminate team-preparation grind.

North of Oldale, the rival waits on Route 103. This first fight is a single battle because neither side has assembled a team yet. It branches across the selected starter family, but every version asks the same fair question: can the player's starter beat its known type counter using legal coverage, utility, and held-item access?

The rival fight is Encounter 0001 in the evidence atlas. It is required. The remaining Route 103 trainers are later or optional encounters, not part of this first trip.

After winning, the rival leads the player back to Birch.

## 5. The National Pokédex and the real start of the journey

Birch gives the Pokédex and immediately enables the National roster. This is not a postgame reward. It is the field book for a campaign that expects species from every generation. The rival gives five Poké Balls.

Birch's new explanation states the game's contract plainly:

- every route offers different tools;
- route signs identify the species present;
- newly caught partners do not need grinding before they matter;
- preparation is easy, but battles are not.

Mom gives the Running Shoes as the player leaves Littleroot. From this point forward, the adventure flag is set and the full capture loop is active.

# Part II — The Stone Examination

## 6. Route 102 and Petalburg

Route 102 is the first real team-building route. Calvin is the normally forced-sight fight. His Zorua, Jigglypuff, Smeargle, and Mew teach Illusion, support sequencing, redirection, and the need to preserve a real Mew answer. Rick, Allen, and Tiana are optional. Their live teams and all dialogue are Encounters 0002 through 0005 in the evidence atlas.

The route signs name the wild species but no longer display misleading percentage totals. Berry trees remain meaningful because berries are not part of the free held-item inventory.

In Petalburg, Norman recognizes the player but refuses a Gym challenge this early. Wally arrives and asks for help catching a Pokémon. Norman lends Wally a Zigzagoon and a Poké Ball; the player accompanies him to Route 102 for the native Ralts-catching tutorial. Wally returns delighted, and Norman points the player toward Rustboro and Roxanne.

This scene establishes the personal campaign arc without pretending Norman is a tutorial boss. He is the midpoint examination and will only battle after four badges.

## 7. Route 104 and Petalburg Woods

The south half of Route 104 introduces more optional trainers, Mr. Briney's cottage, berries, the flower shop, and the beach. Petalburg Woods then becomes the first story dungeon.

A Devon researcher is cornered by Team Aqua. The player must defeat the grunt, recover the researcher's papers, and learn that Aqua is also targeting something in Rustboro. This is the first required doubles-style story encounter after the opening.

The expanded woods add two deeper maps without replacing the native forest. The restored area teaches the Champion's Sign vocabulary through wardens and inscriptions. Visible signs can be inspected before they are eligible; they explain the missing badge or partner instead of silently failing.

The early woods contain three finite trainer encounters in the main battle atlas, including the required Aqua confrontation. The surrounding Route 104 section contains the remaining Stone-era optionals.

### Side content visible now

- Petalburg Woods 2 and 3 can be explored for new species, evolution apples, Mega discoveries, and the first sign inscriptions.
- Kartana is an ordinary battle-ready wild encounter in the deeper woods.
- Virizion's visible Sign eventually requires two badges and a Breloom-family partner.
- Celebi eventually requires four badges and a Blissey-family partner.
- Wo-Chien eventually requires four badges and a Trevenant-family partner.

## 8. Rustboro and Roxanne

Rustboro is the first city where preparation, research, and battle identity meet. The school explains core mechanics, the Cutter's house gives HM Cut, Devon anchors the story, and the Gym presents Rock as several doctrines rather than one defensive stereotype.

Three Gym students are optional. Roxanne is required. Her dialogue explicitly says that Rock can mean endurance, redirection, reverse speed, and pressure, and that her students showed the pieces before she asks for the whole solution.

Defeating Roxanne awards:

- the Stone Badge;
- the level-cap increase from 14 to 20;
- field authorization for Cut when the HM is owned;
- Aerodactylite;
- Old Amber for Devon's fossil program.

Roxanne's post-battle line warns that later Gyms will not repeat the same question. That is exactly the right thesis statement for the campaign.

# Part III — Devon, Dewford, and Mega Evolution

## 9. The Devon theft on Route 116

Leaving Rustboro east reveals that Team Aqua stole the Devon Goods and abducted Peeko. Route 116 contains seven finite battle groups. Rusturf Tunnel contains the required grunt encounter.

The player defeats the grunt, recovers the Devon Parts, and frees Peeko. Mr. Briney returns to his cottage and offers passage. The Devon employee takes the player to the company's third floor.

Mr. Stone now explains the widened plot:

- geometric marks are appearing in caves, forests, and ruins;
- strong teams make them react;
- Magma and Aqua are searching for them;
- Steven understands the stones;
- Captain Stern needs the recovered parts.

The player receives the Letter and PokéNav. The Letter is retry-safe and can be archived to the PC if the key-item pocket is full. The PokéNav provides the regional map and contact flavor, not an ordinary rematch ladder.

The Devon researcher on 2F is an optional Legendary Sign oracle, not a required hub. The researcher can name the next eligible map and remotely tune a Sign when the player brings its partner to Devon. Conditional wild Signs also awaken locally: once the badge and story gates are met, walking the marked habitat with the depicted partner is enough to unlock that encounter. Visible Sign objects work the same way when inspected in place.

## 10. Briney's boat and Dewford

Briney sails from Route 104 to Dewford. The first Dewford trip opens Route 106, Granite Cave, the Gym, the Old Rod, Dewford Hall, Dewford Meadow, and the abandoned manor.

The campaign intentionally requires three separate accomplishments before Slateport passage:

1. deliver Mr. Stone's Letter to Steven;
2. defeat Brawly for the Knuckle Badge;
3. return to Steven and receive the Mega Ring.

Briney's dialogue names whichever requirement remains. That closes a common progression ambiguity: the player is never left wondering why Slateport is unavailable.

## 11. Granite Cave, Brawly, and the Mega Ring

Granite Cave provides HM Flash and the first conditional wild Sign: Cobalion can eventually appear on B2F when the player has at least one badge and brings a Lucario-family partner.

Steven's chamber is a two-stage scene. The player can deliver the Letter before Brawly, but Steven will not give the Mega Ring until the Knuckle Badge proves the partnership. The Letter scene introduces the markings as an ancient mineral network. The badge return turns that mystery into a mechanic.

Brawly's Gym contains five optional battle groups and the required leader battle. Defeating him awards:

- the Knuckle Badge;
- the cap increase from 20 to 30;
- field authorization for Flash when owned;
- Lucarionite.

Returning to Steven awards the Mega Ring. Steven explains that one matching stone allows one partner to Mega Evolve in battle and that the Ring and Champion's Signs answer to the same bond. He registers in the PokéNav and heads east.

Back in Rustboro, Mr. Stone recognizes the completed Mega rite and gives Pidgeotite. He explains that shops do not stock rare stones and that each one should mark discovery or victory. The script, save flag, labels, dialogue, and Match Call state all now name Pidgeotite consistently while preserving the existing save value.

### Dewford optional story

- Dewford Meadow's warden says Steven reopened the manor when the Signs began shining.
- Pheromosa is an ordinary wild encounter in the meadow.
- The manor contains Mega-history clues and later visible quests for Meloetta and Munkidori.
- The Old Rod and Granite Cave's deeper paths expand encounter access without changing the required route.

# Part IV — Aqua Declares Itself

## 12. Route 109 and Slateport

With the Letter delivered, Brawly defeated, and Mega Ring received, Briney sails to Route 109. The beach, Seashore House, market, Contest Hall, shipyard, and museum are all available. Route 109 and the Seashore House contain nine optional battle groups.

The Devon Parts must first be taken to Stern's Shipyard. Dock explains that Stern is at the Oceanic Museum. Team Aqua vacates the museum entrance, allowing the player upstairs.

## 13. The Oceanic Museum gauntlet

Captain Stern identifies the parts as pressure-mapping equipment and connects recent tidal shifts with shining marks across Hoenn. Team Aqua interrupts.

The player fights two grunts in sequence. Archie then enters and personally battles the player. His rewritten ideology is concrete: land creates borders; the sea connects and changes life. Stern's parts can trace the deep current waking the Signs, while Maxie wants to lock that power beneath more land.

Archie's first defeat does not make him incompetent. He recognizes that the player changed course faster, leaves the parts for now, and promises that Aqua will find the current without them. Stern takes the parts and begins a race toward the seafloor.

These are Encounters 0046 through 0048. The script heals the player after the complete museum sequence, so the gauntlet is hard without creating accidental attrition into Route 110.

# Part V — Mauville and the Dynamo Badge

## 14. Route 110, the rival, and Wattson

Route 110 offers the lower road, Cycling Road, Trick House, New Mauville clues, and many optional trainers. The rival battle is mandatory and still branches around the player's starter choice.

The rival gives the Dowsing Machine after the encounter. The route then leads to Mauville, where Wally asks to challenge the Gym. Defeating Wally establishes his ambition and opens the Gym entrance.

Wattson's Gym contains three student battles and the leader. Its dialogue presents Electric as speed control, redirection, field pressure, and positioning rather than a uniform fast-offense type.

Defeating Wattson awards:

- the Dynamo Badge;
- the cap increase from 30 to 40;
- field authorization for Rock Smash when the HM is owned;
- Manectite.

Rock Smash comes from the Mauville house. The badge and HM together open the northern Route 111 path and the Rusturf Tunnel reunion route. The bike shop and Game Corner are optional. The Game Corner's active prizes are Pokémon and decorations: Genesect and Poipole remain unique high-value prizes, while the native Starter Archive offers each unchosen regional starter once with a competitive non-Mega set for 500 Coins. The obsolete TM prize path is gone.

### Optional systems now available

- Route 117 and the Day Care;
- Verdanturf and the Rusturf Tunnel reunion;
- the first additional Trick House puzzle;
- New Mauville later, when Wattson authorizes it;
- Raikou as a conditional Route 110 catch and the visible Thundurus encounter on that route, both after three badges with a Manectric-family partner;
- Entei in Fiery Path with three badges and a Torkoal-family partner;
- the one-time Kubfu Sign on Route 112 with three badges and a Makuhita-family partner.

# Part VI — The Mountain Breaks Open

## 15. Route 111 to Fallarbor

Rock Smash opens Route 111's northern approach. The Winstrate family remains an optional sequential challenge, but its script no longer creates an unintended doubles lock. Completing the related Poké Vial side quest eventually upgrades the Vial from one charge to two.

The desert is visible but blocked without Go-Goggles. The required path bends through Route 112, Fiery Path, Route 113, Fallarbor, and Route 114.

These routes form the largest early-midgame trainer block. The full atlas records every optional camper, hiker, ninja, collector, pair, and story branch. Their difficulty varies deliberately; ordinary “breather” roles appear among the serious puzzles so the ascent does not become one unbroken boss rush.

## 16. Meteor Falls

At Meteor Falls, Team Magma steals Professor Cozmo's Meteorite. Emerald Champions adds a real two-trainer formation before Aqua interrupts: Courtney and a Magma grunt battle the player together.

Archie arrives with his crew, drives Magma toward Mt. Chimney, and recognizes the player from Slateport. At this stage he still presents Aqua as the necessary counterweight to Magma. Cozmo remains behind and points toward the volcano.

This scene is strong because the rival teams are not interchangeable. Magma uses formation and engineered stability; Aqua uses movement and disruption. Both are still wrong about their ability to control ancient power.

## 17. Mt. Chimney and Maxie

The cable car becomes accessible after the Meteor Falls theft. At Mt. Chimney, Magma and Aqua are already fighting. The player breaks through the Magma defenders and confronts Maxie at the machine.

Maxie believes the Meteorite can create a stable, permanent field by expanding land. The battle demonstrates that preference for controlled formations. Defeating him stops the immediate activation, but the machine has already disturbed the mountain's fault network.

Archie thanks the player. Cozmo returns to Fallarbor. Jagged Pass opens as the descent to Lavaridge.

## 18. Flannery and the Heat Badge

Jagged Pass contains five optional encounters before Lavaridge. The hot-spring town provides the Gym, herb shop, egg gift, and restored-area clues.

Flannery's Gym contains seven battle groups. The guide explains how harsh sun, Solar Beam, Trick Room, and Water counterplay interact. Flannery is the complete heat examination rather than merely a high-powered Fire team.

Defeating Flannery awards:

- the Heat Badge;
- the cap increase from 40 to 45;
- field authorization for Strength when the HM is owned;
- Cameruptite.

The rival gives the Go-Goggles outside, opening Route 111's desert. Strength opens deeper optional routes and Ember Path.

# Part VII — Recovery and the Balance Badge

## 19. Ashen Woods, Ember Path, and Verdanturf Meadow

The volcanic disturbance makes the restored areas narratively relevant instead of arbitrary bonus maps.

Ashen Woods is a regrowth chapter. Four authored trainers teach distinct battle schools rather than repeating one weather core. A caretaker frames Rayquaza as Hoenn's balance rather than its conqueror. The optional Chansey “Blob” chase uses a Heal Ball and ultimately advances the Poké Vial upgrade quest.

Ember Path contains a native visible Moltres sanctuary. Moltres remains after a failed capture attempt and only completes on capture. The path also contains one of the restored Mega Stone rewards and ordinary wild Blacephalon.

Verdanturf Meadow provides a quiet tonal contrast. Shaymin becomes available after four badges with a Roserade-family partner. Later, Enamorus requires Shaymin and six badges, while Fezandipiti requires Altaria and six badges.

## 20. Desert exploration

Go-Goggles open the Route 111 desert. The fossil choice remains, but Sandstrewn Ruins expand the region into a longer survival archive. The restored ruins preserve multiple fossils, evolution items, Stakataka as an ordinary wild encounter, and the Landorus-to-Zygarde Sign chain.

Landorus becomes visible after five badges with Castform. Zygarde waits until the player is Champion and returns with Landorus. The route no longer depends on a collapsible Mirage Tower state that could permanently invalidate completion.

## 21. Norman and the midpoint

After four badges, Petalburg Gym opens. Seven room specialists are optional or chosen along the way; Norman is required. The room structure still lets the player choose which tactical lessons to face before the leader.

Norman is the personal midpoint. His story asks whether the player has become strong; his battle asks whether that strength can adapt. Defeating him awards:

- the Balance Badge;
- the cap increase from 45 to 55;
- field authorization for Surf when the HM is owned;
- Lopunnite.

Wally's father immediately gives HM Surf at Wally's house. Returning home after the badge also lets Mom give the Amulet Coin.

# Part VIII — The Water Routes and Fortree

## 22. What Surf opens

Surf is the campaign's largest optional expansion point. It opens:

- Routes 105, 106, 107, 108, and 109;
- the Abandoned Ship;
- Route 115's western and upper sections;
- New Mauville access when Wattson's request is active;
- sea routes around Slateport, Dewford, and Petalburg;
- more Trick House puzzles by badge milestone;
- Scorched Slab and Seaspray Cave approaches;
- many berries, evolution items, rare species, and Mega pickups.

The battle atlas deliberately places a block of Surf backtracking encounters here. They are optional and should feel like exploration, not mandatory fatigue.

## 23. Route 118 and the Weather Institute

The mandatory eastward route crosses Route 118. Steven meets the player, asks whether a Trainer should raise only favorites or many different species, and leaves the answer open. The conversation now reads naturally beside the game's wide-roster philosophy without turning Steven into a tutorial lecturer. Route 119 then becomes a long rain-soaked test with eighteen finite trainer groups on the route and six more inside the Institute.

Team Aqua occupies the Weather Institute to steal weather data and Castform. The player fights through the building and defeats Shelly. She explains that the Institute's records chart both weather and Champion's Signs and that Aqua wins by changing the board before the opponent understands it.

After Shelly's defeat, a grunt reports Magma moving toward Mt. Pyre. The Institute gives the player Castform and explicitly points toward the restored Route 111 ruins. Castform is the partner key for visible Landorus at the Route 111 ruins and visible Tornadus on Route 119; both become solvable after Balance Badge, while Thundurus has already become visible on Route 110 after Dynamo Badge with a Manectric-family partner.

## 24. The Route 119 rival and Fly

Immediately north of the Institute, the rival challenges the player in a required doubles battle. The rival's team still follows the starter branch. Winning awards HM Fly in a retry-safe path that can use the PC if the Bag is full.

Fly cannot be used in the field until Winona's Feather Badge. This makes the rival reward useful immediately after the next major examination without bypassing it.

## 25. Fortree, the Devon Scope, and Winona

An invisible Kecleon blocks the Fortree Gym. On Route 120, Steven demonstrates the Devon Scope, battles the Kecleon, and gives the device. Other Kecleon objects can then be revealed or caught.

Fortree Gym contains six student encounters and Winona. Its identity is speed, sky position, and asymmetric targeting rather than six copies of Tailwind.

Defeating Winona awards:

- the Feather Badge;
- the cap increase from 55 to 60;
- field authorization for Fly when the HM is owned;
- Altarianite.

The badge also unlocks a large sixth-badge Sign wave: Articuno, Azelf, Regidrago, Tapu Bulu, Tapu Lele, Enamorus, and Fezandipiti, provided their partner requirements are met.

# Part IX — Mt. Pyre and the Two Hideouts

## 26. Route 120, Route 121, and Lilycove

The player continues through Route 120 and Route 121. Route 123 is an optional southern branch with berries, trainers, and Sign possibilities. Lilycove provides the Department Store, Contest Hall, motel, harbor, and the final major rival encounter.

The rival's Lilycove battle is required for the Department Store scene but does not block the entire regional story. The Aqua grunts outside the northeastern cave make the next problem visible.

## 27. Mt. Pyre

The player takes the Route 122 pier to Mt. Pyre, climbs the interior floors, crosses the exterior, and reaches the summit. The interior and summit contain sixteen finite trainer groups, including Aqua and Magma story battles.

The summit wardens explain that the Red and Blue Orbs once stabilized a larger network and that the Champion's Signs are part of the same seal. Aqua steals the Red Orb; Magma already possesses or seeks the matching Blue Orb. The player receives the remaining orb and the Magma Emblem, which opens the Jagged Pass hideout.

Mt. Pyre also hosts the game's linked nightmare, dream, and time quest:

1. after the orb story, the player can awaken Darkrai through the summit storyteller's requirement;
2. catching Darkrai reveals Cresselia in a deeper dream location;
3. after becoming Champion and completing the chain, Dialga's time Sign awakens.

The chain is finite, repeat-safe on failed captures, and required for later Reshiram and Zekrom Signs.

## 28. Magma Hideout

The Magma Emblem opens the hidden Jagged Pass entrance. The hideout is a full campaign dungeon with nineteen finite battle groups across its floors. Tabitha and Courtney guard the final approach. Maxie confronts the player beside Groudon.

Maxie's rewritten arc lands correctly here. He treats every prior room as an attempt to make one approach predictable. The player proves that his controlled formations are not control over the world. Maxie still uses the Blue Orb, Groudon awakens, and the ancient Pokémon escapes.

Maxie finally recognizes that the plan did not adapt. He leaves to pursue Groudon because Archie still holds the matching disaster. Ordinary wild Volcanion in the deepest hideout gives the restored map a rare campaign reward without replacing Groudon's later native weather encounter.

## 29. Slateport submarine theft and Aqua Hideout

Returning to Slateport triggers Captain Stern's announcement about the undersea cavern. Archie interrupts the interview and steals the submarine. This is the clean connective tissue between the museum race and the final seafloor plot.

The player returns to Lilycove and enters Aqua Hideout. The dungeon contains nine finite battle groups. Matt is the major guard. Defeating him does not recover the submarine in time; Aqua departs toward the seafloor, but the route east from Lilycove opens.

This order is now identical in play and in the battle master: Mt. Pyre is Encounters 0305–0320, Magma Hideout is 0321–0339, Aqua Hideout is 0340–0348 after the Slateport theft, and Mossdeep Gym follows at 0388–0400 after the eastern sea routes.

# Part X — Mossdeep and the Deep Current

## 30. Routes 124–128 and the Mind Badge

With Aqua Hideout cleared, the player can Surf east through Routes 124, 125, 126, 127, and 128. Shoal Cave, the Treasure Hunter, Sootopolis's underwater entrance, and many late-game Sign prerequisites become reachable.

Mossdeep Gym contains twelve student encounters and Tate and Liza. The Gym is natively suited to the doubles-first identity: positioning, shared turns, room control, and partner coordination are the point rather than an imposed conversion.

Defeating Tate and Liza awards:

- the Mind Badge;
- the cap increase from 60 to 70;
- field authorization for Dive when the HM is owned;
- Metagrossite.

## 31. The Space Center invasion

After the Mind Badge, Team Magma invades Mossdeep Space Center to steal rocket fuel. The player fights through the first floor and a three-grunt 2F gauntlet, then joins Steven in a multi battle against Maxie and Courtney. Tabitha remains an additional optional defender.

The rewritten dialogue connects celestial disturbances, Jirachi, Meteor Falls, and the Sign network. Steven is not a passive exposition device; he is the player's battle partner in the climax.

After the invasion, Steven invites the player to his house and gives HM Dive. The script is state-aware and does not pretend the reward comes directly from the Gym.

The complete Space Center invasion is now grouped at Encounters 0401–0409, directly after Mossdeep Gym. That range includes the optional first-floor defenders, required stair guard, three-Grunt attrition sequence, optional Tabitha branch, and the required Steven partnership against Maxie and Courtney.

## 32. Optional ocean and sealed-chamber content

Dive opens the underwater routes, Seafloor Cavern, the Sealed Chamber, and additional item pockets. The Regi quest remains native in structure: interpret the braille, open the three ruins, and catch Regirock, Regice, and Registeel. Knockouts do not permanently consume the encounters.

With seven badges and Regirock in the party, the giant Regigigas object in the Sealed Chamber can awaken. Its 64×64 overworld art is a deliberate spectacle and remains an optional one-off.

Routes 129–134 are optional ocean exploration before or after the crisis. Route 133 contains the final Poké Vial upgrade, raising it to three charges.

# Part XI — Kyogre, Groudon, and Rayquaza

## 33. Seafloor Cavern

The stolen submarine marks the Seafloor Cavern entrance. The player navigates Strength, current, and rock puzzles and fights Aqua defenders. The underwater approach is deliberately one of the campaign's richest special habitats: Relicanth, Dhelmise, Dragalge, Kingdra, Golisopod, Basculegion, Dondozo, Veluza, Wishiwashi, rare Iron Bundle and Nihilego, and a one-percent Feebas trophy all live there. Manaphy is a finite conditional Sign rather than repeatable grass-table loot. Shelly guards a required chamber. Archie waits beside Kyogre in Room 9.

Archie's final team is the culmination of Aqua's changing-board identity. After losing, he still activates the Red Orb. Kyogre awakens and leaves. Reports of destructive rain force Archie to understand that freedom without limits can erase the people it was supposed to connect.

Maxie arrives and admits that his certainty woke Groudon. Their argument resolves into shared responsibility: neither understood the Orbs, and the Sign network is amplifying both legends. The player is warped outside as Sootopolis enters the crisis state.

## 34. Sootopolis and Cave of Origin

Groudon and Kyogre battle in the center of Sootopolis. Houses and the Gym are locked while the city is in crisis. Steven guides the player to Wallace at Cave of Origin.

Cave of Origin preserves its native story role and adds the crystal chamber as a late-story clue. Diancie and, later, Terapagos use that restored space without replacing the immediate objective.

Wallace asks where the third ancient Pokémon might be. The correct answer is Sky Pillar. He leaves to open the route.

## 35. Sky Pillar and Rayquaza

The player reaches Pacifidlog, Surfs to Route 131, meets Wallace at Sky Pillar, and climbs to Rayquaza. Awakening Rayquaza triggers the return to Sootopolis.

Rayquaza descends, ends the weather war, and restores the relation between Groudon and Kyogre. The new story explicitly treats this as mediation and board restoration, not conquest. Maxie and Archie accept that their perfect answers broke the region.

Wallace gives HM Waterfall after the resolution and opens the Sootopolis Gym. Rayquaza remains available as a capture encounter later; the crisis scene does not silently award it.

## 36. Juan and the Rain Badge

Sootopolis Gym contains ten optional floor trainers and Juan. The Gym is the final badge examination and uses Water as a broad competitive vocabulary rather than a single rain core.

Defeating Juan awards:

- the Rain Badge;
- the cap increase from 70 to 80;
- field authorization for Waterfall;
- Gyaradosite;
- access to the complete Mega Stone and evolution archives at Center vendors.

The full campaign toolkit is now available before Victory Road. No Mega is reserved solely for a postgame the player may never reach.

# Part XII — Victory Road and the League

## 37. Ever Grande and Victory Road

Waterfall opens Ever Grande. Victory Road contains seventeen optional trainer encounters plus the required Wally battle. Wally is no longer the timid child from Petalburg; his final campaign team is a serious proof that the region's preparation tools are available to rivals as well as the player.

The Pokémon League lobby provides healing, a medicine-and-capture Mart, and the final chance to prepare. The reusable Leveler remains the sole leveling shortcut. The door guard explicitly checks all eight badge flags before opening the League, so its implementation now matches its dialogue without relying on the normal story path to imply the other seven.

## 38. The Elite Four attrition arc

The League is Sidney, Phoebe, Glacia, Drake, then Wallace. All five battles are doubles. The cap is 80.

The native attrition rules are preserved:

- the entry door locks behind the player;
- the party is not automatically healed between rooms;
- the Bag can be used manually between battles;
- the Bag cannot be used during the competitive battles;
- the next door opens only after victory.

If fewer than two usable Pokémon remain, the next opponent offers a native retirement choice. The player can stay, use a Revive from the Bag, and speak again, or end the run and return to the lobby. This prevents a one-survivor save trap without making the League automatically heal.

The battle master and room scripts agree on the final sequence: Victory Road occupies Encounters 0476–0492, followed by Sidney 0493, Phoebe 0494, Glacia 0495, Drake 0496, and Wallace 0497. Postgame battles begin at 0498; Wallace's Cave of Origin exhibition is correctly reserved for 0510.

## 39. Wallace and the Hall of Fame

Wallace is the final authored regional examination. His dialogue summarizes the campaign: every time he changes the question, the player must find another answer. Winning means the player solved the region, not merely overleveled it.

The rival and Birch enter after the battle. Wallace leads the player into the Hall of Fame. The game sets the Champion state, raises the cap to 100, resets the League for replays, opens the S.S. Tidal, expands the Safari Zone, exposes Steven's Beldum gift, and schedules the home and Pokédex-sync scenes.

# Part XIII — The Postgame

## 40. Home, the Lati report, and the S.S. Ticket

The player wakes at home. Norman appears and gives the S.S. Ticket. The television reports a roaming Eon Pokémon, and the player's color answer selects Latias or Latios. The roaming system begins.

Birch's lab scene is now a research sync rather than a National Dex unlock, because the National Dex was active from the beginning. Completing the Hoenn roster no longer repeats the initial Johto starter choice. It awards the Legendary Form Research Kit: DNA Splicers, Zygarde Cube, N-Solarizer, N-Lunarizer, and Reins of Unity. Each tool is checked and delivered independently, so a full Key Items pocket pauses rather than loses or duplicates the reward; legacy saves at the old starter-reward state reopen the new finite kit when any tool is missing.

Scott calls about the Battle Frontier. The S.S. Tidal can sail between Slateport, Lilycove, and the Frontier.

## 41. Event islands and native legendary roots

The postgame harbor can provide and enable all four island passes:

- Eon Ticket for Southern Island;
- Mystic Ticket for Navel Rock;
- Aurora Ticket for Birth Island;
- Old Sea Map for Faraway Island.

Southern Island, Ho-Oh, Lugia, Deoxys, and Mew all complete only on capture. A knockout does not permanently consume the encounter. The same capture-only rule applies to the Regis, Groudon, Kyogre, and Rayquaza.

The Weather Institute begins the native abnormal-weather hunt for Terra Cave and Marine Cave. Groudon and Kyogre can be caught there. Steven's hidden Meteor Falls chamber, the Diancie chamber, and Altering Cave open their full postgame roles.

## 42. The Legendary Sign mastery path

The Sign system contains 82 acquisition definitions and covers all 101 legendary-class families through visible quests, conditional wild encounters, ordinary wild roots, native encounters, breeding, the Game Corner, the Champions Circuit, or mastery rewards.

The most important postgame chains are:

- Darkrai → Cresselia → Dialga;
- Dialga plus the appropriate map keys → Reshiram and Zekrom;
- Jirachi → Cosmog;
- Diancie → Magearna and Terapagos;
- Okidogi → Pecharunt;
- Landorus → Zygarde;
- Ditto → Mewtwo;
- Unown → Hoopa;
- all 81 other finite Sign definitions, including Circuit and Game Corner milestones → Arceus.

Every required partner accepts the evolution family, so a player is not rejected for bringing an evolved form. No quest depends on Mirage Tower remaining uncollapsed or on a Safari capture. Conditional wild Signs awaken at their own marked habitats when the qualifying partner is present; visible Signs are solved at their objects. Restored-area wardens and inscriptions provide local hints, while the Devon researcher remains an optional remote clue, tuning, and mastery service.

The complete, exact 82-entry table is in the evidence atlas.

## 43. Cynthia, Leaf, Steven, and other superbosses

The postgame includes one-time authored exhibitions rather than an ordinary rematch treadmill:

- Cynthia in Mossdeep uses her dedicated overworld and trainer portrait and a full six-Pokémon doubles team.
- Leaf waits in Altering Cave as a distortion-area superboss.
- Steven remains a Meteor Falls examination.
- Wallace has a Cave of Origin legendary doubles exhibition.
- S.S. Tidal trainers provide a finite postgame travel gauntlet.

These battles are separate from the endlessly generated Frontier loop and therefore can carry stronger character-specific dialogue and rewards.

## 44. The Champions Circuit and Battle Frontier

The Frontier is the true endless endgame. Its architecture, ferry access, shops, and social spaces remain, but every challenge attendant now enters the Champions Circuit. The old Gen 3 facility generators are retired rather than competing with the game's doubles identity.

The Circuit generates doubles teams inside the ROM from a pinned Pokémon Showdown Champions corpus: 311 species or form variants and 444 role templates. It enforces Species Clause by National Dex family, at most one Mega, type and weakness limits, weather and field compatibility, role-driven moves, and Ability filtering. It retains Showdown Random Doubles item behavior rather than imposing a separate Item Clause.

The player's party is temporarily normalized to level 80 and restored afterward. Opponent scaling advances one slot by one level after each win, then rolls the whole team upward. Twelve finite legendary rewards finish before the win-40 Eternatus mastery reward. Circuit state is separate from the native Battle Tower records.

This is the correct final expression of the campaign thesis: after the authored regional examination ends, the game generates new competitive questions instead of recycling ordinary route rematches.

# Side-Quest Availability by Milestone

This is the practical “what just opened?” layer. It lists the most important changes, while the evidence atlas records every exact Sign.

## Before the first badge

- National Pokédex, Poké Vial, Leveler, battle-set tutor, and free ordinary held items.
- Deep Petalburg Woods exploration and ordinary Kartana.
- Dewford Meadow and ordinary Pheromosa once Briney reaches Dewford.
- Visible Signs can be inspected, but most explain that the story or badge requirement is incomplete.

## After the Stone Badge

- Cut authorization.
- Cap 20.
- Route 116 and Rusturf story resolution.
- Cobalion can be awakened in Granite Cave with a Lucario-family partner.

## After the Knuckle Badge

- Flash authorization.
- Cap 30.
- Mega Ring from Steven.
- Virizion can be awakened in Petalburg Woods with a Breloom-family partner.
- Slateport passage after the Letter and Ring requirements are complete.

## After the Dynamo Badge

- Rock Smash authorization.
- Cap 40.
- Northern Route 111, Fiery Path, and the Fallarbor circuit.
- Entei and Raikou conditional encounters.
- Ordinary Kubfu on Route 112.

## After the Heat Badge

- Strength authorization and Go-Goggles.
- Cap 45.
- Desert, Sandstrewn Ruins, deeper volcanic routes, Ashen Woods, Ember Path, and Verdanturf Meadow content.
- Celebi, Shaymin, Meloetta, Munkidori, Okidogi, and Wo-Chien become possible with their partner families.
- Moltres is available in Ember Path.

## After the Balance Badge

- Surf authorization.
- Cap 55.
- The full Surf backtracking network, Abandoned Ship, New Mauville, Scorched Slab, Seaspray Cave, and many late Hoenn routes.
- Landorus, Mesprit, Tapu Koko, Tornadus, Thundurus, Uxie, Zeraora, Meltan, Zapdos, and Regieleki gates begin opening.

## After the Feather Badge

- Fly authorization.
- Cap 60.
- Articuno, Azelf, Regidrago, Tapu Bulu, Tapu Lele, Enamorus, and Fezandipiti gates.
- Faster return access makes the distributed Sign network substantially easier to pursue.

## After the Mind Badge

- Dive authorization after the Space Center story.
- Cap 70.
- Sealed Chamber, underwater routes, Seafloor Cavern, and Regigigas with Regirock.

## After the Rain Badge

- Waterfall authorization.
- Cap 80.
- Victory Road and the League.
- Complete Mega Stone and evolution-item archives at Center vendors.
- Magearna can be prepared once Diancie is available; Victini's Victory Road condition is active.

## After becoming Champion

- Cap 100.
- S.S. Tidal, Battle Frontier, Champions Circuit, Safari expansion, event-island tickets, roaming Lati, Beldum, Terra/Marine Cave, hidden chambers, Altering Cave, the postgame Sign chains, and Arceus mastery.

# Cohesion Audit

## What is already excellent

### One design thesis reaches every system

The story never says “build many answers” while the economy forces one permanent team. The Leveler, wild presets, tutor presets, free items, strict caps, and route signs make experimentation practical. The campaign's mechanics and dialogue are saying the same thing.

### The new mythology supports Emerald instead of replacing it

Champion's Signs explain why the disturbance spreads across caves, forests, weather, and ancient Pokémon. Steven, Devon, the Weather Institute, Mt. Pyre, Wallace, Magma, and Aqua each understand a different part of the same phenomenon. Rayquaza still resolves the central catastrophe.

### Progression rewards still matter

Mega Stones, evolution items, fossils, berries, rare Balls, PP resources, Poké Vial upgrades, and legendary access retain scarcity. Free held items do not make every pickup meaningless. The complete pre-League archive is a deliberate final unlock, not an accidental early leak.

### Failure paths are unusually well protected

Critical items do not advance the story when delivery fails. Most can retry or use the PC where the native contract permits. Static legendary knockouts do not permanently consume the encounter. The League has a retirement route. Fossil delivery removes the fossil only after a valid selection and preserves old save states.

### Presentation is source-gated

All critical story strings and every literal Hoenn dialogue line are measured against the native font. The current audit checked 10,846 targeted story lines and 47,623 literal Hoenn lines without overflow.

## Resolved audit findings

### Campaign chronology is now one source of truth

The battle master follows live script prerequisites, places optional encounters at their earliest natural opportunity, and uses legal evolution-phase teams at those earlier caps. All six Gabby and Ty milestones appear at their route phases, and the League and postgame no longer interleave. The critical late-game ranges are:

- Mt. Pyre 0305–0320;
- Magma Hideout 0321–0339;
- Aqua Hideout 0340–0348;
- Mossdeep Gym 0388–0400;
- Space Center 0401–0409;
- Seafloor Cavern 0452–0458;
- Juan 0459–0469;
- Victory Road 0476–0492;
- Sidney through Wallace 0493–0497;
- finite postgame encounters 0498–0513.

### The League contract is explicit

The entrance now checks `FLAG_BADGE01_GET` through `FLAG_BADGE08_GET` individually before setting the Elite Four entry state. Dialogue and code both require all eight badges.

### Winona closes the midgame sequence break

Steven remains on Route 120's one-tile bridge after giving the Devon Scope and opens the eastern road only after the Feather Badge. Tate and Liza and Juan independently reject an out-of-order challenge. This preserves the intended 55 → 60 → 70 → 80 cap ladder instead of allowing the rest of Hoenn to execute while badge six remains missing.

The master also corrects 41 local optional-route encounters to the cap at which their exact tiles first become reachable: Route 115/116 at 14, Route 117 and Mauville-side Route 111/118 at 30, and post-Rock-Smash Route 111/Mt. Chimney at 40. Their teams were rewritten into legal evolution phases where required; Blazikenite and Chandelurite were preserved as later showcases.

### Reward names and Birch's completion prize are coherent

Devon's Pidgeotite path now uses `FLAG_RECEIVED_PIDGEOTITE_FROM_DEVON` and `RustboroCity_DevonCorp_3F_EventScript_GivePidgeotite`; the stale EXP Share terminology and duplicate employee label are gone. Birch's obsolete Johto starter repeat has been replaced by the five-tool Legendary Form Research Kit, with retry-safe partial delivery and old-save migration.

### Legendary Sign discovery is local-first

Returning to Devon is optional. Conditional wild Signs unlock in their marked habitat when badge, story, and partner requirements are met; visible Signs unlock through their local objects. Devon remains useful as a remote tuner, next-clue oracle, and Arceus mastery endpoint, but no longer imposes repeated Rustboro travel.

### Rematches are dormant, not a campaign promise

Ordinary and Gym rematches remain intentionally disabled. Some legacy rematch battle blocks and dialogue are still compiled, but they have no physical campaign entry and sit behind the false runtime rematch guard. All reachable registration prompts and Match Call texts are promise-free. Removing the dormant blocks would be optional source cleanup, not a player-facing progression fix.

### Upstream runtime hardening

The inherited Expansion layer received a focused safety pass. Direct interaction with buried Trainers can no longer dispatch past its movement table. Secret Sword now drives Keldeo's Ordinary/Resolute form correctly through party moves, PC moves, the native tutor/deleter, authored presets, and wild presets. Unsafe battle recording and replay are disabled—and old Frontier Pass records are hidden—until upstream provides a proven serializer/playback repair. PC held-item text now reserves its terminator and selects a fitting native font for every item name.

Battle regression coverage protects coordinated Pledge selection and ally-only Decorate targeting. Commander state now survives and clears coherently across status damage, sleep and Yawn progression, Dondozo replacement, fainting, and forced-switch effects. Forecast and Flower Gift faint transitions no longer emit phantom form messages, and simultaneous voluntary switches follow modern Speed order. The curated manifest now includes copied-move targeting, Imposter AI, Sleep Clause re-entry, and Billy's Imposter lead as ordinary zero-debt regressions; this covers those named paths without claiming every Transform interaction is exhaustively proven.

Four parity questions remain deliberately deferred rather than patched speculatively:

- exact simultaneous faint-replacement order is not established well enough to encode as Champions truth;
- full per-target spread-move end-effect interleaving would require a risky architectural rewrite of the move-end pipeline;
- broader mid-turn AI cache invalidation beyond the two named switch-in reset regressions has not been confirmed with a reproducible failure;
- missing Gen 8/9 battle-animation assets and their presentation still require an asset pass and manual in-emulator playtesting.

## Remaining empirical concerns

### Trainer density and fatigue need human calibration

The finite campaign contains 513 physical encounter groups and 561 branches: 87 single, 460 double, and 14 multi, so 84.49% are doubles-like. Those parties contain 2,147 authored Pokémon across 754 species/forms and showcase all 92 Mega Stones plus all 57 targeted legendary species. Ordinary target difficulty averages 7.57: 143 fights are 6.x, 267 are 7.x, and 64 are 8.x. That is intentionally demanding, but long clusters remain: Route 119 plus the Weather Institute contains 24 encounters, Mt. Pyre contains 16, and Magma Hideout contains 19. Static variety checks prove these are not copies of one puzzle; only actual pacing data can prove that a completionist still wants the next battle.

### Static correctness is not playthrough proof

The structural gates prove that references, warps, item contracts, badge gates, dialogue widths, legendary chains, and authored trainer data are internally coherent. Every master battle still records `difficulty_observed: UNPLAYED`. A complete fresh-save human run is required to prove moment-to-moment clarity, tonal continuity, real difficulty, save/retry behavior, optional-clue discoverability, and absence of hardware-only presentation defects.

### The complete Mega archive is a deliberate catch-up mechanism

Opening every Mega Stone and evolution item after Juan guarantees the full toolkit before Victory Road, which directly serves the game's campaign-first philosophy. It also makes any missed world stone non-scarce at the final badge. This is not a contradiction to “fix”; playtesting should confirm that the archive feels like a welcome final safety net rather than an exploration reward arriving too late or erasing discoveries too abruptly.

# Recommended Next Steps

1. Start a clean save and use this book as the playtest checklist: record arrival state, available tools, mandatory event, optional branches, battle result, confusing dialogue, and save/retry behavior at each chapter.
2. Log attempts, team changes, and perceived effort for every trainer cluster; tune only after enough observations distinguish one hard battle from cumulative fatigue.
3. Verify that a player discovers the local Legendary Sign rule from wardens, inscriptions, and Devon's optional explanation without relying on an external guide.
4. Record the emotional and practical effect of the post-Juan Mega archive: which stones were found naturally, which become useful only through catch-up, and whether the timing improves the League.
5. Keep the generated evidence atlas in every release; CI now proves that its deterministic campaign-input fingerprint and rendered contents match the source tree.
6. Remove dormant rematch blocks only if smaller source surface is worth the compatibility and maintenance churn; they are not blocking the playthrough.

# Further Questions

- Should the four event-island passes all be handed out automatically postgame, or should some be attached to authored exploration milestones?
- Does the complete Mega archive after Juan land as the intended pre-League catch-up reward in practice?
- Should the old Battle Tents remain independent side facilities when the Champions Circuit already provides the strongest repeatable competitive identity?
- Do players understand that conditional wild Signs awaken locally, with Devon serving as an optional shortcut rather than a required return trip?
- After a full playthrough, which ordinary trainer blocks need to become true breathers rather than simply receiving lower level offsets?

# Caveats and Evidence Boundary

This book records source wiring and the contracts exercised by the named static gates; it is not itself evidence that a consolidated release run passed. Use fresh verifier, build, boot, and runtime logs for the exact candidate under test. It does not claim that every optional event will be seen in one playthrough, that all dialogue has been experienced on hardware, or that the authored difficulty targets match human outcomes.

The generated [campaign evidence atlas](EMERALD_CHAMPIONS_CAMPAIGN_EVIDENCE.txt) and reordered [master battle design](emerald_champions_master_battle_design.txt) now share the same encounter order and source snapshot. The existing [story-mode audit](EMERALD_CHAMPIONS_STORY_MODE_AUDIT.md), [story bible](emerald_champions_migration/STORY_BIBLE.md), and source verifiers remain supporting evidence rather than substitutes for playtesting.
