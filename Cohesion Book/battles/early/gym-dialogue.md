# Early gym dialogue — exact tactical repairs

These label-level replacements reconcile the five early gyms with their final teams. They are specifications, not edits to native scripts. World proposal `W-PETALBURG-ROOMS` owns door text, guide directions and neighbor directions; the tactical intro replacements below use its agreed names. All unlisted source labels remain under the world interaction inventory.

Victory text must remain true after any legal winning method, including a clean sweep, status plan, direct coverage or memorized Retry solution. Battle dialogue can preserve personality without inventing observations or absent moves.

## RustboroCity_Gym_Text_JoshIntro

**REPAIR** — `TRAINER_JOSH` · [Snapshot source](../../baseline/source/data/maps/RustboroCity_Gym/scripts.inc:323)

Sturdy is conditional on full HP and can be bypassed; the current guarantee is false.

Current native text:

```text
	.string "STURDY means BONSLY and SHIELDON\n"
	.string "always live to swing back.\p"
	.string "Chip them first, or don't swing!$"
```

Exact proposed native text:

```text
	.string "BONSLY and SHIELDON can survive a\n"
	.string "knockout blow at full health.\p"
	.string "We make that extra action count!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## RustboroCity_Gym_Text_JoshPostBattle

**REPAIR** — `TRAINER_JOSH` · [Snapshot source](../../baseline/source/data/maps/RustboroCity_Gym/scripts.inc:331)

Roxanne now has Sturdy Nosepass, but even then Sturdy is not an unconditional extra turn.

Current native text:

```text
	.string "STURDY guarantees NOSEPASS one more\n"
	.string "turn. Plan for the turn after it.$"
```

Exact proposed native text:

```text
	.string "STURDY can preserve NOSEPASS at\n"
	.string "full health. Chip damage or an\n"
	.string "Ability that bypasses it changes\n"
	.string "the plan.$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## RustboroCity_Gym_Text_MarcIntro

**REPAIR** — `TRAINER_MARC` · [Snapshot source](../../baseline/source/data/maps/RustboroCity_Gym/scripts.inc:347)

Weather replacement after Veil is set does not remove the established screen.

Current native text:

```text
	.string "AMAURA calls the snow, and the veil\n"
	.string "it raises blunts everything.\p"
	.string "Break the snow, or break through!$"
```

Exact proposed native text:

```text
	.string "AMAURA calls snow and raises\n"
	.string "AURORA VEIL.\p"
	.string "Stop the screen or find a way\n"
	.string "through!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## RustboroCity_Gym_Text_GuideIntroduction

**REPAIR** — `TRAINER_ROXANNE_1` · [Snapshot source](../../baseline/source/data/maps/RustboroCity_Gym/scripts.inc:279)

Preserve teacher character while describing every team as a serious doubles contest, not staged beginner instruction.

Current native text:

```text
	.string "Yo! Welcome to your first GYM exam.\p"
	.string "ROXANNE battles with a full doubles team.\n"
	.string "Her students teach the pieces: field\l"
	.string "control, redirection, and reverse speed.\p"
	.string "Read each pair before attacking, and\n"
	.string "bring answers instead of one favorite.\p"
	.string "Take these FRESH WATERS for the road.\n"
	.string "Come see me after you earn the BADGE!$"
```

Exact proposed native text:

```text
	.string "Yo! Welcome to RUSTBORO GYM.\p"
	.string "Every trainer here brings a\n"
	.string "complete doubles plan. ROXANNE\n"
	.string "brings the full force of her Rock\n"
	.string "team.\p"
	.string "Prepare your partners, study the\n"
	.string "board, and make your own opening.\p"
	.string "Take these FRESH WATERS. Come see\n"
	.string "me after you earn the BADGE!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## RustboroCity_Gym_Text_GymGuideAdvice

**REPAIR** — `TRAINER_ROXANNE_1` · [Snapshot source](../../baseline/source/data/maps/RustboroCity_Gym/scripts.inc:289)

Replace broad type-only guidance with truthful optional advice supporting several solutions.

Current native text:

```text
	.string "Yo, how's it going?\p"
	.string "Listen, my friend!\n"
	.string "Would you like to become the CHAMPION?\p"
	.string "I'm no TRAINER, not me, but I can\n"
	.string "sure give you winning advice.\p"
	.string "That's settled, then! We'll aim for the\n"
	.string "POKéMON CHAMPIONSHIP together!\p"
	.string "It's your job as a TRAINER to collect\n"
	.string "GYM BADGES, am I right?\p"
	.string "But GYM LEADERS aren't pushovers!\p"
	.string "And that's where I come in!\n"
	.string "I'm here to provide expert advice!\p"
	.string "ROXANNE, the GYM LEADER, is a user\n"
	.string "of ROCK-type POKéMON.\p"
	.string "The ROCK type is very durable, but\n"
	.string "it can't stand WATER-type and\l"
	.string "GRASS-type moves.\p"
	.string "Come see me afterwards, if you beat\n"
	.string "the GYM LEADER.\p"
	.string "Well, go for it!$"
```

Exact proposed native text:

```text
	.string "ROXANNE uses Rock Pokémon with\n"
	.string "different jobs.\p"
	.string "CARBINK can reverse move order\n"
	.string "with TRICK ROOM. NOSEPASS can\n"
	.string "guard its partner from spread\n"
	.string "attacks.\p"
	.string "Water and Grass are useful, but\n"
	.string "target choice and timing still\n"
	.string "matter. Build the answer that\n"
	.string "suits your team!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## RustboroCity_Gym_Text_RoxanneIntro

**REPAIR** — `TRAINER_ROXANNE_1` · [Snapshot source](../../baseline/source/data/maps/RustboroCity_Gym/scripts.inc:359)

The final team has Wide Guard rather than an attack-redirecting ability; keep the teacher identity and strong challenge.

Current native text:

```text
	.string "I am ROXANNE, RUSTBORO's GYM LEADER.\p"
	.string "I teach here, but each challenger gives\n"
	.string "me something new to study.\p"
	.string "Rock can endure, redirect an attack, or\n"
	.string "reverse the order of a battle. My\l"
	.string "students have shown you the pieces.\p"
	.string "Now, let us put them together.$"
```

Exact proposed native text:

```text
	.string "I am ROXANNE, RUSTBORO's GYM\n"
	.string "LEADER.\p"
	.string "Each challenger gives me something\n"
	.string "new to study.\p"
	.string "My Pokémon can endure a blow,\n"
	.string "guard a partner, or reverse the\n"
	.string "order of battle.\p"
	.string "Show me how your team answers\n"
	.string "mine.$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## DewfordTown_Gym_Text_GymGuideAdvice

**REPAIR** — `TRAINER_BRAWLY_1` · [Snapshot source](../../baseline/source/data/maps/DewfordTown_Gym/scripts.inc:287)

Do not rule out all Normal support choices; explain actual redirection and priority while preserving the lighting cue.

Current native text:

```text
	.string "Hey, how's it going, CHAMPION-\n"
	.string "bound {PLAYER}?\p"
	.string "DEWFORD's GYM LEADER BRAWLY commands\n"
	.string "FIGHTING-type POKéMON.\p"
	.string "Going against him with NORMAL-type\n"
	.string "POKéMON is asking for trouble.\p"
	.string "FIGHTING POKéMON will lay a serious\n"
	.string "smack down on NORMAL POKéMON,\l"
	.string "so be careful!\p"
	.string "What's worse, the GYM is as dark as\n"
	.string "the ocean floor.\p"
	.string "But it will get brighter after defeating\n"
	.string "the TRAINERS in your way.\p"
	.string "Hey, okay, go for it!$"
```

Exact proposed native text:

```text
	.string "BRAWLY brings a powerful Fighting\n"
	.string "team, but his partners do more\n"
	.string "than attack.\p"
	.string "PACHIRISU draws single-target\n"
	.string "moves aside. His fighters use\n"
	.string "speed, priority and support to\n"
	.string "create openings.\p"
	.string "Normal Pokémon risk Fighting\n"
	.string "damage, but a useful support role\n"
	.string "can still earn a place on your\n"
	.string "team.\p"
	.string "The GYM grows brighter as you\n"
	.string "defeat its trainers. Make every\n"
	.string "action count!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## DewfordTown_Gym_Text_TakaoIntro

**REPAIR** — `TRAINER_TAKAO` · [Snapshot source](../../baseline/source/data/maps/DewfordTown_Gym/scripts.inc:307)

Shadow Tag/Octolock have escape exceptions; remove an absolute no-exit claim.

Current native text:

```text
	.string "GRAPPLOCT locks you down. WOBBUFFET\n"
	.string "makes sure you cannot leave at all.\p"
	.string "Win here, or do not win!$"
```

Exact proposed native text:

```text
	.string "GRAPPLOCT can hold a target in\n"
	.string "place. WOBBUFFET makes switching\n"
	.string "difficult too.\p"
	.string "Let us see how you handle the\n"
	.string "pressure!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## DewfordTown_Gym_Text_TakaoDefeat

**REPAIR** — `TRAINER_TAKAO` · [Snapshot source](../../baseline/source/data/maps/DewfordTown_Gym/scripts.inc:312)

The victory may not have involved a critical window.

Current native text:

```text
	.string "You broke my critical window!$"
```

Exact proposed native text:

```text
	.string "You won a tough battle!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## DewfordTown_Gym_Text_TakaoPostBattle

**REPAIR** — `TRAINER_TAKAO` · [Snapshot source](../../baseline/source/data/maps/DewfordTown_Gym/scripts.inc:315)

No rare Dragon exists on this team.

Current native text:

```text
	.string "Preserve FAIRY or ICE pressure for the\n"
	.string "rare Dragon waiting behind my fighters.$"
```

Exact proposed native text:

```text
	.string "HITMONLEE's WHITE HERB can clear\n"
	.string "its stat drops and activate\n"
	.string "UNBURDEN.\p"
	.string "Speed can change in the middle of\n"
	.string "a turn. Watch the whole board.$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## DewfordTown_Gym_Text_JocelynIntro

**REPAIR** — `TRAINER_JOCELYN` · [Snapshot source](../../baseline/source/data/maps/DewfordTown_Gym/scripts.inc:319)

Priority tiers and opposing speed control mean Tailwind does not necessarily precede every opposing move.

Current native text:

```text
	.string "TAILWIND goes up before you move,\n"
	.string "and everything behind it runs you down.\p"
	.string "Four turns. Survive them!$"
```

Exact proposed native text:

```text
	.string "TAILWIND can change the race in an\n"
	.string "instant.\p"
	.string "Four turns of pressure. Can your\n"
	.string "team handle them?$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## DewfordTown_Gym_Text_JocelynDefeat

**REPAIR** — `TRAINER_JOCELYN` · [Snapshot source](../../baseline/source/data/maps/DewfordTown_Gym/scripts.inc:324)

Avoid claiming a specific observed pressure-denial solution.

Current native text:

```text
	.string "You never fed the pressure I needed!$"
```

Exact proposed native text:

```text
	.string "You took this battle!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## DewfordTown_Gym_Text_JocelynPostBattle

**REPAIR** — `TRAINER_JOCELYN` · [Snapshot source](../../baseline/source/data/maps/DewfordTown_Gym/scripts.inc:327)

No Natu or Magic Bounce exists on Jocelyn's current team.

Current native text:

```text
	.string "Attack around MAGIC BOUNCE instead of\n"
	.string "handing NATU your best status move.$"
```

Exact proposed native text:

```text
	.string "TAILWIND lasts four turns.\n"
	.string "BRELOOM's POISON HEAL can keep it\n"
	.string "going while the field changes.\p"
	.string "Plan for the partners still in\n"
	.string "reserve.$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## DewfordTown_Gym_Text_LauraDefeat

**REPAIR** — `TRAINER_LAURA` · [Snapshot source](../../baseline/source/data/maps/DewfordTown_Gym/scripts.inc:336)

Winning does not imply both Fake Outs were denied.

Current native text:

```text
	.string "You denied both of my free turns!$"
```

Exact proposed native text:

```text
	.string "That was a hard-fought win!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## DewfordTown_Gym_Text_LilithDefeat

**REPAIR** — `TRAINER_LILITH` · [Snapshot source](../../baseline/source/data/maps/DewfordTown_Gym/scripts.inc:348)

Avoid asserting a specific complete tactical read.

Current native text:

```text
	.string "You read every discipline correctly!$"
```

Exact proposed native text:

```text
	.string "You brought a winning team!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## DewfordTown_Gym_Text_BrendenDefeat

**REPAIR** — `TRAINER_BRENDEN` · [Snapshot source](../../baseline/source/data/maps/DewfordTown_Gym/scripts.inc:360)

Avoid asserting the player changed answers.

Current native text:

```text
	.string "You changed answers with the damage!$"
```

Exact proposed native text:

```text
	.string "We gave it everything!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## DewfordTown_Gym_Text_CristianDefeat

**REPAIR** — `TRAINER_CRISTIAN` · [Snapshot source](../../baseline/source/data/maps/DewfordTown_Gym/scripts.inc:372)

No form change is required or necessarily observed.

Current native text:

```text
	.string "You changed form without losing focus!$"
```

Exact proposed native text:

```text
	.string "Our formation could not hold!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## DewfordTown_Gym_Text_CristianPostBattle

**REPAIR** — `TRAINER_CRISTIAN` · [Snapshot source](../../baseline/source/data/maps/DewfordTown_Gym/scripts.inc:375)

Riolu/Leek/Throh belong to other teams; reference the actual repaired Beat Up lead.

Current native text:

```text
	.string "Stop RIOLU's stance, respect the LEEK,\n"
	.string "then contain THROH's committed power.$"
```

Exact proposed native text:

```text
	.string "FALINKS can power up JUSTIFIED\n"
	.string "before its partner attacks.\p"
	.string "GALLADE and LUCARIO can both use\n"
	.string "that opening. Always check who is\n"
	.string "active.$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## MauvilleCity_Gym_Text_KirkPostBattle

**REPAIR** — `TRAINER_KIRK` · [Snapshot source](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc:296)

The team is a terrain/ability puzzle, not a sound amplifier roster.

Current native text:

```text
	.string "Sound pressure is still special damage.\n"
	.string "SNARL and LIGHT SCREEN can quiet it.$"
```

Exact proposed native text:

```text
	.string "ELECTRIC TERRAIN protects grounded\n"
	.string "Pokémon from sleep.\p"
	.string "A Flying Pokémon or an AIR BALLOON\n"
	.string "changes which benefits apply.$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## MauvilleCity_Gym_Text_ShawnIntro

**REPAIR** — `TRAINER_SHAWN` · [Snapshot source](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc:300)

Several actual team members are grounded.

Current native text:

```text
	.string "You saved your EARTHQUAKE for me.\n"
	.string "Nothing here stands on the ground.\p"
	.string "Try again, from the air!$"
```

Exact proposed native text:

```text
	.string "Some of my Pokémon float above\n"
	.string "Ground attacks. Others use the\n"
	.string "terrain beneath them.\p"
	.string "Can you tell which answer fits\n"
	.string "each pair?$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## MauvilleCity_Gym_Text_ShawnPostBattle

**REPAIR** — `TRAINER_SHAWN` · [Snapshot source](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc:308)

There is no Helping Hand/Electroweb lead in this team.

Current native text:

```text
	.string "Stop one lead before HELPING HAND and\n"
	.string "ELECTROWEB turn every attack dangerous.$"
```

Exact proposed native text:

```text
	.string "BELLIBOLT can set ELECTRIC TERRAIN\n"
	.string "for RAICHU's SURGE SURFER.\p"
	.string "MEGA AMPHAROS brings a different\n"
	.string "pace and a Dragon typing.$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## MauvilleCity_Gym_Text_BenPostBattle

**REPAIR** — `TRAINER_BEN` · [Snapshot source](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc:320)

No Oranguru or Trick Room exists.

Current native text:

```text
	.string "Taunt ORANGURU or wait out TRICK ROOM.\n"
	.string "Never race a slow battery on its clock.$"
```

Exact proposed native text:

```text
	.string "CHARJABUG powers special attacks\n"
	.string "with BATTERY. DISCHARGE can even\n"
	.string "heal LANTURN.\p"
	.string "That same move is not safe beside\n"
	.string "every partner.$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## MauvilleCity_Gym_Text_AngeloIntro

**REPAIR** — `TRAINER_ANGELO` · [Snapshot source](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc:336)

Compound Eyes Thunder is 91 percent under ordinary accuracy conditions, not guaranteed.

Current native text:

```text
	.string "JOLTIK's THUNDER does not miss, and\n"
	.string "TOXTRICITY only gets louder.\p"
	.string "Accuracy is a luxury. I have it!$"
```

Exact proposed native text:

```text
	.string "JOLTIK makes THUNDER more\n"
	.string "reliable, and TOXTRICITY turns\n"
	.string "sound into power.\p"
	.string "We are ready to take our chances!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## MauvilleCity_Gym_Text_AngeloPostBattle

**REPAIR** — `TRAINER_ANGELO` · [Snapshot source](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc:344)

No Vikavolt or Sticky Web exists in Angelo's team.

Current native text:

```text
	.string "Clear the STICKY WEB before my\n"
	.string "VIKAVOLT outspeeds your whole team.$"
```

Exact proposed native text:

```text
	.string "WATTREL can set TAILWIND, and\n"
	.string "JOLTIK can slow opponents with\n"
	.string "ELECTROWEB.\p"
	.string "Those moves change speed in\n"
	.string "different ways.$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## MauvilleCity_Gym_Text_WattsonIntro

**REPAIR** — `TRAINER_WATTSON_1` · [Snapshot source](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc:348)

No Trick Room/reverse-order move exists in Wattson's actual final team.

Current native text:

```text
	.string "Wahahahah! I'm WATTSON!\p"
	.string "Electric teams can race forward or close\n"
	.string "the circuit and reverse move order.\p"
	.string "My opening is fixed. My reserves are not.\n"
	.string "Read the board each time it changes!$"
```

Exact proposed native text:

```text
	.string "Wahahahah! I am WATTSON!\p"
	.string "ELECTRODE sends the current.\n"
	.string "ELECTIVIRE turns it into speed.\p"
	.string "I know how my partners work\n"
	.string "together. Show me what yours can\n"
	.string "do!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## MauvilleCity_Gym_Text_WattsonDefeat

**REPAIR** — `TRAINER_WATTSON_1` · [Snapshot source](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc:355)

Memorization is a legitimate part of Retry and this text should not deny that winning method.

Current native text:

```text
	.string "Wahahah! You solved the circuit instead\n"
	.string "of memorizing it! Take the DYNAMO BADGE!$"
```

Exact proposed native text:

```text
	.string "Wahahah! What a battle!\p"
	.string "You have earned the DYNAMO BADGE!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## LavaridgeTown_Gym_1F_Text_GymGuideAdvice

**REPAIR** — `TRAINER_FLANNERY_1` · [Snapshot source](../../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc:184)

Flannery has neither Trick Room nor Solar Beam in this roster; explain After You and sun accurately.

Current native text:

```text
	.string "Hey, how's it going, CHAMPION-\n"
	.string "bound {PLAYER}?\p"
	.string "LAVARIDGE's GYM LEADER FLANNERY\n"
	.string "uses FIRE-type POKéMON.\p"
	.string "Her passion for POKéMON burns stronger\n"
	.string "and hotter than a volcano.\p"
	.string "Her sun weakens WATER and speeds SOLAR\n"
	.string "BEAM, while TRICK ROOM reverses tempo.\p"
	.string "Protect your WATER answer and disrupt\n"
	.string "the field before attacking.$"
```

Exact proposed native text:

```text
	.string "FLANNERY's sun strengthens Fire\n"
	.string "attacks and weakens Water attacks.\p"
	.string "LILLIGANT can use AFTER YOU to let\n"
	.string "slow TORKOAL attack sooner.\p"
	.string "Weather control, disruption and\n"
	.string "careful damage all offer different\n"
	.string "ways to challenge that\n"
	.string "partnership.$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## LavaridgeTown_Gym_1F_Text_FlanneryIntro

**REPAIR** — `TRAINER_FLANNERY_1` · [Snapshot source](../../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc:311)

The claimed two ways of making the slowest member act first refers to removed Room; preserve timing personality.

Current native text:

```text
	.string "I'm FLANNERY, LAVARIDGE's GYM LEADER!\p"
	.string "I used to think Fire meant attacking\n"
	.string "harder. MT. CHIMNEY taught me timing.\p"
	.string "My slowest POKéMON can move first in two\n"
	.string "different ways. Cool the engine early!$"
```

Exact proposed native text:

```text
	.string "I am FLANNERY, LAVARIDGE's GYM\n"
	.string "LEADER!\p"
	.string "MT. CHIMNEY taught me that power\n"
	.string "needs timing.\p"
	.string "LILLIGANT and TORKOAL are ready to\n"
	.string "turn up the heat. Show us your\n"
	.string "best!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## LavaridgeTown_Gym_1F_Text_FlanneryDefeat

**REPAIR** — `TRAINER_FLANNERY_1` · [Snapshot source](../../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc:318)

No Balloon exists and no winning sequence is mandatory; preserve reward handoff.

Current native text:

```text
	.string "You weakened the eruptions, broke the\n"
	.string "Balloon, and changed pace with my heat.\p"
	.string "No type-chart shortcut. Take the\n"
	.string "HEAT BADGE!$"
```

Exact proposed native text:

```text
	.string "That was a blazing battle!\p"
	.string "You beat the team I put my heart\n"
	.string "into. Take the HEAT BADGE!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## PetalburgCity_Gym_Text_RandallIntro

**REPAIR** — `TRAINER_RANDALL` · [Snapshot source](../../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc:1343)

Use reconciled room name and truthful multihit wording.

Current native text:

```text
	.string "The ability to attack before the\n"
	.string "opponent…\p"
	.string "Just that alone puts me at a great\n"
	.string "advantage, don't you agree?\p"
	.string "Five hits a turn. No SASH lives!$"
```

Exact proposed native text:

```text
	.string "This is the MULTIHIT ROOM.\p"
	.string "One attack can strike again and\n"
	.string "again. A FOCUS SASH may not be\n"
	.string "enough!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## PetalburgCity_Gym_Text_ParkerIntro

**REPAIR** — `TRAINER_PARKER` · [Snapshot source](../../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc:1367)

Use reconciled room name and describe reverse speed without promising universal order.

Current native text:

```text
	.string "This is the CONFUSION ROOM.\p"
	.string "Let me see how well bonded you are\n"
	.string "with your POKéMON!\p"
	.string "TRICK ROOM. My slowest strike first!$"
```

Exact proposed native text:

```text
	.string "Welcome to the TRICK ROOM.\p"
	.string "The slower Pokémon can move first\n"
	.string "here. ORANGURU can also make its\n"
	.string "partner repeat an attack.\p"
	.string "Let us see how you position your\n"
	.string "team.$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## PetalburgCity_Gym_Text_ParkerDefeat

**REPAIR** — `TRAINER_PARKER` · [Snapshot source](../../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc:1373)

Current team has no confusion move to support the inherited claim.

Current native text:

```text
	.string "I couldn't confuse your team enough…\n"
	.string "You share a strong bond together.$"
```

Exact proposed native text:

```text
	.string "Our timing was not enough. Good\n"
	.string "battle!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## PetalburgCity_Gym_Text_GeorgeIntro

**REPAIR** — `TRAINER_GEORGE` · [Snapshot source](../../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc:1385)

Use reconciled name and recognize offense as well as healing.

Current native text:

```text
	.string "Just when you think you're going to\n"
	.string "win, your opponent restores HP…\p"
	.string "Can you just imagine how awful that\n"
	.string "must feel?\p"
	.string "I'll show you exactly how awful it\n"
	.string "feels!$"
```

Exact proposed native text:

```text
	.string "This is the ENDURANCE ROOM.\p"
	.string "Healing buys time. DUBWOOL can\n"
	.string "turn that time into BODY PRESS\n"
	.string "power.\p"
	.string "Can your team finish the job?$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## PetalburgCity_Gym_Text_GeorgeDefeat

**REPAIR** — `TRAINER_GEORGE` · [Snapshot source](../../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc:1393)

A win may come through status, PP, disruption or direct damage.

Current native text:

```text
	.string "I couldn't restore HP enough…\n"
	.string "What ATTACK power…$"
```

Exact proposed native text:

```text
	.string "We could not outlast your team!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## PetalburgCity_Gym_Text_BerkeIntro

**REPAIR** — `TRAINER_BERKE` · [Snapshot source](../../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc:1410)

No Explosion exists in the actual roster.

Current native text:

```text
	.string "This is the BURST ROOM. SCRAPPY sound\n"
	.string "crosses Ghosts. One visible EXPLOSION\l"
	.string "can erase a careless board. Position!$"
```

Exact proposed native text:

```text
	.string "This is the BURST ROOM.\p"
	.string "EXPLOUD's voice can hit Ghosts.\n"
	.string "MEGA GLALIE turns Normal attacks\n"
	.string "into Ice.\p"
	.string "Prepare for both sides of the\n"
	.string "board!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## PetalburgCity_Gym_Text_BerkeDefeat

**REPAIR** — `TRAINER_BERKE` · [Snapshot source](../../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc:1415)

Avoid asserting a specific observed timing solution.

Current native text:

```text
	.string "You made every burst hit the wrong turn!$"
```

Exact proposed native text:

```text
	.string "Your team took the battle!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## PetalburgCity_Gym_Text_BerkePostBattle

**REPAIR** — `TRAINER_BERKE` · [Snapshot source](../../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc:1418)

No Trick Room or Explosion exists; Mega Glalie is a lead rather than final ace.

Current native text:

```text
	.string "Track TRICK ROOM, guard the telegraphed\n"
	.string "EXPLOSION, and keep an answer for the\l"
	.string "refrigerated final impact.$"
```

Exact proposed native text:

```text
	.string "A strong pair can attack through\n"
	.string "different defenses.\p"
	.string "The last reserves still matter:\n"
	.string "STOUTLAND and KANGASKHAN keep the\n"
	.string "pressure on.$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## PetalburgCity_Gym_Text_MaryIntro

**REPAIR** — `TRAINER_MARY` · [Snapshot source](../../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc:1428)

Use reconciled room name; Hustle repair expressly trades accuracy for pressure.

Current native text:

```text
	.string "Giggle…\n"
	.string "This is the ACCURACY ROOM.\p"
	.string "It's pretty nasty when every attack\n"
	.string "lands without fail.\p"
	.string "Hurt me halfway, and BERSERK answers!$"
```

Exact proposed native text:

```text
	.string "This is the RETALIATION ROOM.\p"
	.string "DRAMPA can gain power after a hard\n"
	.string "hit. RUFFLET's HUSTLE trades\n"
	.string "accuracy for strength.\p"
	.string "Every risk needs a purpose!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## PetalburgCity_Gym_Text_AlexiaIntro

**REPAIR** — `TRAINER_ALEXIA` · [Snapshot source](../../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc:1452)

Use reconciled room name and describe orb strategy accurately.

Current native text:

```text
	.string "The higher the DEFENSE, the more\n"
	.string "reckless I can be in attack.\p"
	.string "My own poison is the whole point!$"
```

Exact proposed native text:

```text
	.string "This is the STATUS ROOM.\p"
	.string "ZANGOOSE and URSARING make their\n"
	.string "own status a weapon.\p"
	.string "Bring a plan for the whole team!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

## PetalburgCity_Gym_Text_JodyIntro

**REPAIR** — `TRAINER_JODY` · [Snapshot source](../../baseline/source/data/maps/PetalburgCity_Gym/scripts.inc:1473)

Use reconciled room name without claiming that optimized power teams lack strategy.

Current native text:

```text
	.string "Our GYM LEADER told us to go all out\n"
	.string "and beat you.\p"
	.string "Even if you happen to be the\n"
	.string "LEADER's kid!\p"
	.string "No tricks in here. Only power!$"
```

Exact proposed native text:

```text
	.string "This is the POWER ROOM.\p"
	.string "Our LEADER told us to give you\n"
	.string "everything we have, even if you\n"
	.string "are his kid.\p"
	.string "That is exactly what we will do!$"
```

Implement only this label and retain its callers/flags/rewards. Render every page with the actual font and tokens; verify no overflow, correct badge/state transitions, and identical dialogue after materially different valid winning strategies.

