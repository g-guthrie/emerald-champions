# Verified playthrough - evidence log

Each row: what the real game showed or reported. Recipes: artifacts/playthrough/recipes/.
Recordings (screens, inputs, end.sav): work/studio/scenes/<leg>/ (local; not committed).
ROM: pokeemerald-headless.gba built from 2a29e751 + the chapter-100 fixture (66b8b5d1).

| Leg | Where | Game readback / screenshot | Source cross-check |
|---|---|---|---|
| pt00 | New game | Truck, VAR_LITTLEROOT_TOWN_STATE 0 | CB2_NewGame (overworld.c:1858) |
| pt00c | Player 2F | Clock set: VAR_LITTLEROOT_INTRO_STATE 5 -> 6 | |
| pt00f | May's 2F | Littleroot state 1, rival state 3, VAR_STARTER_GEN 3 (Hoenn, default cursor) | players_house.inc:4 |
| pt00h | Route 101 | Pair chosen on screen: Mudkip + Treecko; forced win; lab, VAR_BIRCH_LAB_STATE 3 | |
| pt01 | Oldale | Arrived; Bag: Old Rod 0, Poke Ball 0 | Old Rod needs Littleroot state 3 (lab Pokedex) |
| pt02 | Oldale | Mart employee at (13,14): Bag Poke Ball 5, FLAG_RECEIVED_POTION_OLDALE set | OldaleTown/scripts.inc:27, :79 |
| pt03 | Oldale Mart | Tour: Poke Vial 1, Leveler 1, Regenerator 1, Repel Spray 1, Flight Beacon 1 | |
| pt03 (A-tap run) | Oldale Mart | Shelf screenshot: Mental Herb 100, Red Card 1500, Cell Battery 1000 | poke_mart.inc:19 |
| pt04 | Route 103 | May refuses until the party is at the cap ("Use your Leveler") | Route103/scripts.inc:44 IsPlayerPartyBelowLevelCap |
| pt04 | Route 103 | Wild Blitzle met in grass (auto-captured by the forced-win fixture: synthetic) | wild table Route 103 |
| pt05 | Route 103 | Leveler: party to Lv14 = current cap (screen). May battle ran (forced win); VAR_BIRCH_LAB_STATE 4, May hidden, money +140 | caps.c pre-badge cap 14 |
| pt06 | Lab | Pokedex flag set; Littleroot state 3 -> Mom at (10,2): Bag Old Rod 1, FLAG_RECEIVED_OLD_ROD | LittlerootTown/scripts.inc:955 |
| pt07 | Oldale Center | Bag Cherish Ball 30 (May in lab, label says "PokeBalls"); vendor kit: Choice Band, Choice Specs, Choice Scarf, Focus Sash, Eviolite, Leftovers (1 each) | ProfessorBirchsLab/scripts.inc:408; field_specials.c:606 |
