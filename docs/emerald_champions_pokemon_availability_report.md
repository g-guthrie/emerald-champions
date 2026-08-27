# Emerald Champions — Complete Pokémon Encounter and Acquisition Report

## What this document is

This is a source-derived technical report describing the current Pokémon availability model in **Emerald Champions**. It is deliberately structured for ingestion by another language model: it first explains the game’s design goals and progression systems, then enumerates every random encounter table, then inventories non-random acquisitions, and finally gives one acquisition classification for every runtime species/form ID.

The report distinguishes **within-method species probability** from an encounter method’s trigger rate. Grass, Surf, Rock Smash, each Rod, Honey, Berry-tree, roaming, and facility encounters are separate pools. Percentages are valid only inside their named method and are never meant to add together across methods.

## Technical summary

- Source state: commit `712da9a722a1f44833230af44c2d22a6d3804e49`; clean working tree at generation: **False**.
- Random encounter coverage: **139** populated main overworld maps from **147** configured entries, plus **576** distinct species/forms in catchable random pools.
- Loadout system: **1309** total competitive sets. Wild Pokémon roll the exact one/two/three-set tutor count at 100%, 50/50, or approximately one-third each.
- Item system: **65** ordinary competitive held items are free and unlimited. Berries and evolution/transformation progression items are not vendor stock.
- Species/form appendix: **1269** runtime IDs classified; **93** remain unresolved by the automated source scan and are explicitly listed rather than guessed.
- Probability validation: **PASS** — every emitted method pool sums independently to 100%.
- Battle context: **810** reachable definitions, including **565 doubles** and **289 singles**; **72** canonical encounters are currently source-closed bespoke redesigns.

## Acquisition classification summary

The classification totals separate exact direct sources and permanent evolution/breeding paths from non-collectible form endpoints and unresolved audit rows. The HTML report renders these same counts as a chart; the Markdown table preserves them for language-model ingestion.

| Acquisition classification | Runtime species/form IDs |
|---|---|
| Direct acquisition | 646 |
| Evolution from obtainable Pokémon | 280 |
| Form / battle transformation; not separately acquired | 126 |
| Alternate form from obtainable base | 104 |
| Unresolved by automated acquisition scan | 93 |
| Breeding / obtainable evolution family | 20 |

## Game identity and why the encounter distribution exists

Emerald Champions is an extremely hard, doubles-focused competitive Pokémon campaign built on Emerald/Inclement Emerald foundations.

The player selects from 21 Generation 1–7 starters. The first Route 103 rival uses the same-region type-counter starter at level 15 against the opening cap of 14.

The current guide resolves 810 reachable battle definitions: 565 doubles and 289 intentional singles. The first 72 canonical encounters have source-closed bespoke redesigns; the remaining campaign is still part of the continuing battle-authoring program.

The game front-loads competitive agency and moves the challenge into battle solving. Players can catch unusual and powerful species early, teach every legal move at Pokémon Centers, switch legal abilities natively, obtain free ordinary competitive held items, and immediately raise a party to the strict cap. The campaign is mostly doubles, ordinary trainers are intended to be serious threats, and bosses are built as bespoke competitive puzzles rather than stat-only checks.

Wild encounters follow the same philosophy. Eligible ordinary wild Pokémon do not appear with filler level-up moves: before battle, each one uniformly rolls one of its finalized competitive tutor sets and receives that set’s moves, nature, ability, and ordinary held item. The encountered Pokémon uses that loadout against the player and retains it if caught. Capture-hostile moves such as Explosion, Memento, Teleport, phazing, or Perish Song are not filtered.

## Progression and strict level caps

| Phase | Badges | Strict cap |
|---|---|---|
| Opening — before the Stone Badge | 0 | 14 |
| Stone Badge — Rustboro to Dewford | 1 | 20 |
| Knuckle Badge — Dewford to Slateport | 2 | 30 |
| Dynamo Badge — Mauville, ash country, and Mt. Chimney | 3 | 40 |
| Heat Badge — Petalburg return and eastern routes | 4 | 45 |
| Balance Badge — Fortree, Safari Zone, and Mt. Pyre | 5 | 55 |
| Feather Badge — Lilycove, ocean routes, and Mossdeep | 6 | 60 |
| Mind Badge — deep ocean, Sootopolis, and Cave of Origin | 7 | 70 |
| Rain Badge — Sky Pillar, Victory Road, and the League approach | 8 | 80 |
| Champion / postgame — open-world cleanup and Battle Frontier | 9 | 101 |

The phase labels in this report use the earliest trainer-guide evidence for a map when available and a documented route/location heuristic otherwise. They are navigation context, not a replacement for story-event flags.

## Random encounter methodology

- `chance_percent` is the conditional chance of that species after the named method is active. Duplicate slots are aggregated.
- `encounter_rate` is the table's raw encounter-rate field; it is not multiplied into `chance_percent` because step checks, terrain, abilities, Repel, and method invocation differ.
- The chapter shown for each row is the **earliest usable phase**, calculated as the later of location access and the method's actual field gate. Surf, Rock Smash, Good Rod, Super Rod, and Honey rows therefore no longer appear as opening access merely because their maps are early.
- Land weights are 13/13/10/10/10/10/5/5/8/8/4/4. Surf and Rock Smash are 60/30/5/5. Old Rod is 60/40; Good Rod 60/20/20; Super Rod 40/30/15/10/5; Honey 50/15/15/10/5/5.
- Route 119 under-bridge Feebas is a separate 100% special fishing override at levels 20–25.
- Battle Pyramid and Battle Pike tables are reported as facility-only random battles, not normal overworld acquisition promises.

## Complete random encounter flow

### Opening — before the Stone Badge (cap 14)

| Location | Method | Access requirement | Raw rate | Species | Within-method % | Levels |
|---|---|---|---|---|---|---|
| Route 101 | Land | No field move; available when the location itself is reachable | 20 | Wurmple | 20 | 2–3 |
| Route 101 | Land | No field move; available when the location itself is reachable | 20 | Poochyena | 13 | 2–2 |
| Route 101 | Land | No field move; available when the location itself is reachable | 20 | Zigzagoon | 13 | 2–2 |
| Route 101 | Land | No field move; available when the location itself is reachable | 20 | Sewaddle | 10 | 3–3 |
| Route 101 | Land | No field move; available when the location itself is reachable | 20 | Sprigatito | 10 | 3–3 |
| Route 101 | Land | No field move; available when the location itself is reachable | 20 | Bonsly | 9 | 3–3 |
| Route 101 | Land | No field move; available when the location itself is reachable | 20 | Lillipup | 9 | 3–3 |
| Route 101 | Land | No field move; available when the location itself is reachable | 20 | Dreepy | 8 | 2–2 |
| Route 101 | Land | No field move; available when the location itself is reachable | 20 | Larvesta | 8 | 2–2 |
| Route 102 | Land | No field move; available when the location itself is reachable | 20 | Bidoof | 20 | 4–4 |
| Route 102 | Land | No field move; available when the location itself is reachable | 20 | Lotad | 13 | 3–3 |
| Route 102 | Land | No field move; available when the location itself is reachable | 20 | Seedot | 13 | 3–3 |
| Route 102 | Land | No field move; available when the location itself is reachable | 20 | Gothita | 10 | 4–4 |
| Route 102 | Land | No field move; available when the location itself is reachable | 20 | Nacli | 10 | 3–3 |
| Route 102 | Land | No field move; available when the location itself is reachable | 20 | Ralts | 9 | 3–4 |
| Route 102 | Land | No field move; available when the location itself is reachable | 20 | Surskit | 9 | 3–3 |
| Route 102 | Land | No field move; available when the location itself is reachable | 20 | Hatenna | 8 | 4–4 |
| Route 102 | Land | No field move; available when the location itself is reachable | 20 | Indeedee | 8 | 4–4 |
| Route 102 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Corphish | 60 | 5–10 |
| Route 102 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Goldeen | 40 | 5–10 |
| Route 103 | Land | No field move; available when the location itself is reachable | 20 | Shinx | 20 | 3–4 |
| Route 103 | Land | No field move; available when the location itself is reachable | 20 | Shellos | 13 | 3–3 |
| Route 103 | Land | No field move; available when the location itself is reachable | 20 | Wingull | 13 | 2–2 |
| Route 103 | Land | No field move; available when the location itself is reachable | 20 | Fuecoco | 10 | 2–2 |
| Route 103 | Land | No field move; available when the location itself is reachable | 20 | Kricketot | 10 | 3–3 |
| Route 103 | Land | No field move; available when the location itself is reachable | 20 | Blitzle | 9 | 4–4 |
| Route 103 | Land | No field move; available when the location itself is reachable | 20 | Grubbin | 9 | 2–3 |
| Route 103 | Land | No field move; available when the location itself is reachable | 20 | Toxel | 8 | 3–3 |
| Route 103 | Land | No field move; available when the location itself is reachable | 20 | Yamper | 8 | 3–3 |
| Route 103 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 60 | 5–10 |
| Route 103 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Tentacool | 40 | 5–10 |
| Route 104 | Land | No field move; available when the location itself is reachable | 20 | Azurill | 13 | 5–7 |
| Route 104 | Land | No field move; available when the location itself is reachable | 20 | Taillow | 13 | 5–7 |
| Route 104 | Land | No field move; available when the location itself is reachable | 20 | Mareanie | 12 | 5–7 |
| Route 104 | Land | No field move; available when the location itself is reachable | 20 | Wimpod | 12 | 5–7 |
| Route 104 | Land | No field move; available when the location itself is reachable | 20 | Budew | 10 | 5–7 |
| Route 104 | Land | No field move; available when the location itself is reachable | 20 | Litleo | 10 | 5–7 |
| Route 104 | Land | No field move; available when the location itself is reachable | 20 | Pidove | 10 | 5–7 |
| Route 104 | Land | No field move; available when the location itself is reachable | 20 | Sentret | 10 | 5–7 |
| Route 104 | Land | No field move; available when the location itself is reachable | 20 | Bunnelby | 5 | 5–7 |
| Route 104 | Land | No field move; available when the location itself is reachable | 20 | Ledyba | 5 | 5–7 |
| Route 104 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Chewtle | 60 | 5–10 |
| Route 104 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Sobble | 40 | 5–10 |
| Petalburg Woods | Land | No field move; available when the location itself is reachable | 20 | Shroomish | 13 | 6–8 |
| Petalburg Woods | Land | No field move; available when the location itself is reachable | 20 | Slakoth | 13 | 6–8 |
| Petalburg Woods | Land | No field move; available when the location itself is reachable | 20 | Buneary | 10 | 6–8 |
| Petalburg Woods | Land | No field move; available when the location itself is reachable | 20 | Paras | 10 | 6–8 |
| Petalburg Woods | Land | No field move; available when the location itself is reachable | 20 | Phantump | 10 | 6–8 |
| Petalburg Woods | Land | No field move; available when the location itself is reachable | 20 | Pidgey | 10 | 6–8 |
| Petalburg Woods | Land | No field move; available when the location itself is reachable | 20 | Foongus | 8 | 6–8 |
| Petalburg Woods | Land | No field move; available when the location itself is reachable | 20 | Impidimp | 8 | 6–8 |
| Petalburg Woods | Land | No field move; available when the location itself is reachable | 20 | Pichu | 5 | 6–8 |
| Petalburg Woods | Land | No field move; available when the location itself is reachable | 20 | Scyther | 5 | 8–8 |
| Petalburg Woods | Land | No field move; available when the location itself is reachable | 20 | Cascoon | 4 | 7–8 |
| Petalburg Woods | Land | No field move; available when the location itself is reachable | 20 | Silcoon | 4 | 7–8 |
| Petalburg Woods 2 | Land | No field move; available when the location itself is reachable | 20 | Kakuna | 13 | 12–14 |
| Petalburg Woods 2 | Land | No field move; available when the location itself is reachable | 20 | Metapod | 13 | 12–14 |
| Petalburg Woods 2 | Land | No field move; available when the location itself is reachable | 20 | Panpour | 10 | 12–14 |
| Petalburg Woods 2 | Land | No field move; available when the location itself is reachable | 20 | Pansage | 10 | 12–14 |
| Petalburg Woods 2 | Land | No field move; available when the location itself is reachable | 20 | Pansear | 10 | 12–14 |
| Petalburg Woods 2 | Land | No field move; available when the location itself is reachable | 20 | Venipede | 10 | 12–14 |
| Petalburg Woods 2 | Land | No field move; available when the location itself is reachable | 20 | Bounsweet | 9 | 12–14 |
| Petalburg Woods 2 | Land | No field move; available when the location itself is reachable | 20 | Morelull | 9 | 12–14 |
| Petalburg Woods 2 | Land | No field move; available when the location itself is reachable | 20 | Applin | 8 | 12–14 |
| Petalburg Woods 2 | Land | No field move; available when the location itself is reachable | 20 | Blipbug | 8 | 12–14 |
| Petalburg Woods 3 | Land | No field move; available when the location itself is reachable | 20 | Dewpider | 17 | 13–15 |
| Petalburg Woods 3 | Land | No field move; available when the location itself is reachable | 20 | Emolga | 17 | 13–15 |
| Petalburg Woods 3 | Land | No field move; available when the location itself is reachable | 20 | Bellsprout | 13 | 13–15 |
| Petalburg Woods 3 | Land | No field move; available when the location itself is reachable | 20 | Oddish | 13 | 13–15 |
| Petalburg Woods 3 | Land | No field move; available when the location itself is reachable | 20 | Croagunk | 10 | 13–15 |
| Petalburg Woods 3 | Land | No field move; available when the location itself is reachable | 20 | Misdreavus | 10 | 13–15 |
| Petalburg Woods 3 | Land | No field move; available when the location itself is reachable | 20 | Murkrow | 10 | 13–15 |
| Petalburg Woods 3 | Land | No field move; available when the location itself is reachable | 20 | Yanma | 10 | 13–15 |
| Petalburg Woods 3 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Poliwag | 60 | 13–18 |
| Petalburg Woods 3 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Slowpoke | 40 | 13–18 |
| Rustboro City | Land | No field move; available when the location itself is reachable | 20 | Cottonee | 20 | 9–11 |
| Rustboro City | Land | No field move; available when the location itself is reachable | 20 | Eevee | 17 | 9–11 |
| Rustboro City | Land | No field move; available when the location itself is reachable | 20 | Glameow | 17 | 9–11 |
| Rustboro City | Land | No field move; available when the location itself is reachable | 20 | Nidoran♀ | 13 | 9–11 |
| Rustboro City | Land | No field move; available when the location itself is reachable | 20 | Nidoran♂ | 13 | 9–11 |
| Rustboro City | Land | No field move; available when the location itself is reachable | 20 | Gimmighoul | 10 | 9–11 |
| Rustboro City | Land | No field move; available when the location itself is reachable | 20 | Petilil | 10 | 9–11 |
| Berry tree encounter table 1: G Berry Stage Sprouted | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Sewaddle | 34 | 5–5 |
| Berry tree encounter table 1: G Berry Stage Sprouted | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Scatterbug | 20 | 5–5 |
| Berry tree encounter table 1: G Berry Stage Sprouted | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Wurmple | 20 | 5–5 |
| Berry tree encounter table 1: G Berry Stage Sprouted | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Caterpie | 13 | 5–5 |
| Berry tree encounter table 1: G Berry Stage Sprouted | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Weedle | 13 | 5–5 |
| Berry tree encounter table 2: G Berry Stage Taller | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Spewpa | 34 | 5–5 |
| Berry tree encounter table 2: G Berry Stage Taller | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Cascoon | 20 | 5–5 |
| Berry tree encounter table 2: G Berry Stage Taller | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Silcoon | 20 | 5–5 |
| Berry tree encounter table 2: G Berry Stage Taller | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Kakuna | 13 | 5–5 |
| Berry tree encounter table 2: G Berry Stage Taller | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Metapod | 13 | 5–5 |
| Berry tree encounter table 3: G Berry Stage Flowering | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Cherubi | 20 | 5–5 |
| Berry tree encounter table 3: G Berry Stage Flowering | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Cutiefly | 20 | 5–5 |
| Berry tree encounter table 3: G Berry Stage Flowering | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Illumise | 17 | 5–5 |
| Berry tree encounter table 3: G Berry Stage Flowering | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Volbeat | 17 | 5–5 |
| Berry tree encounter table 3: G Berry Stage Flowering | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Combee | 13 | 5–5 |
| Berry tree encounter table 3: G Berry Stage Flowering | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Flabebe | 13 | 5–5 |
| Berry tree encounter table 4: G Berry Stage Berries | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Munchlax | 24 | 5–5 |
| Berry tree encounter table 4: G Berry Stage Berries | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Burmy | 23 | 5–5 |
| Berry tree encounter table 4: G Berry Stage Berries | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Crabrawler | 23 | 5–5 |
| Berry tree encounter table 4: G Berry Stage Berries | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Aipom | 15 | 5–5 |
| Berry tree encounter table 4: G Berry Stage Berries | Berry Tree | Interact with an eligible Berry tree when its location is reachable | 10 | Pikipek | 15 | 5–5 |

### Stone Badge — Rustboro to Dewford (cap 20)

| Location | Method | Access requirement | Raw rate | Species | Within-method % | Levels |
|---|---|---|---|---|---|---|
| Route 101 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 50 | 2–2 |
| Route 101 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Dustox | 20 | 2–3 |
| Route 101 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Beautifly | 15 | 2–2 |
| Route 101 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Swadloon | 10 | 3–3 |
| Route 101 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Skwovet | 5 | 3–3 |
| Route 102 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 50 | 2–2 |
| Route 102 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Gothorita | 20 | 2–3 |
| Route 102 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Kirlia | 20 | 2–3 |
| Route 102 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Bibarel | 10 | 3–3 |
| Route 103 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 50 | 2–2 |
| Route 103 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Charjabug | 20 | 2–3 |
| Route 103 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Luxio | 20 | 2–3 |
| Route 103 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Kricketune | 10 | 3–3 |
| Route 104 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 50 | 2–2 |
| Route 104 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Yungoos | 30 | 2–3 |
| Route 104 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Patrat | 20 | 2–3 |
| Route 105 | Land | No field move; available when the location itself is reachable | 20 | Exeggutor | 34 | 3–5 |
| Route 105 | Land | No field move; available when the location itself is reachable | 20 | Crabrawler | 20 | 4–5 |
| Route 105 | Land | No field move; available when the location itself is reachable | 20 | Floatzel | 20 | 5–5 |
| Route 105 | Land | No field move; available when the location itself is reachable | 20 | Chatot | 13 | 4–4 |
| Route 105 | Land | No field move; available when the location itself is reachable | 20 | Malamar | 13 | 4–4 |
| Route 105 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Clauncher | 60 | 5–10 |
| Route 105 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Skrelp | 40 | 5–10 |
| Route 116 | Land | No field move; available when the location itself is reachable | 20 | Nincada | 13 | 8–10 |
| Route 116 | Land | No field move; available when the location itself is reachable | 20 | Skiddo | 13 | 8–10 |
| Route 116 | Land | No field move; available when the location itself is reachable | 20 | Joltik | 10 | 8–10 |
| Route 116 | Land | No field move; available when the location itself is reachable | 20 | Mareep | 10 | 8–10 |
| Route 116 | Land | No field move; available when the location itself is reachable | 20 | Riolu | 10 | 8–10 |
| Route 116 | Land | No field move; available when the location itself is reachable | 20 | Starly | 10 | 8–10 |
| Route 116 | Land | No field move; available when the location itself is reachable | 20 | Skitty | 9 | 8–10 |
| Route 116 | Land | No field move; available when the location itself is reachable | 20 | Dreepy | 8 | 8–10 |
| Route 116 | Land | No field move; available when the location itself is reachable | 20 | Rookidee | 8 | 8–10 |
| Route 116 | Land | No field move; available when the location itself is reachable | 20 | Houndour | 5 | 8–10 |
| Route 116 | Land | No field move; available when the location itself is reachable | 20 | Meowth Galarian | 4 | 8–10 |
| Route 116 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 50 | 2–2 |
| Route 116 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Pancham | 25 | 2–3 |
| Route 116 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Purrloin | 20 | 2–3 |
| Route 116 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Nickit | 5 | 3–3 |
| Petalburg Woods | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 50 | 2–2 |
| Petalburg Woods | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Pidgey | 25 | 2–3 |
| Petalburg Woods | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Pikachu | 25 | 2–3 |
| Petalburg Woods 2 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 50 | 2–2 |
| Petalburg Woods 2 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Caterpie | 25 | 2–3 |
| Petalburg Woods 2 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Weedle | 25 | 2–3 |
| Petalburg Woods 3 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 50 | 2–2 |
| Petalburg Woods 3 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Emolga | 45 | 2–3 |
| Petalburg Woods 3 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Goomy | 5 | 3–3 |
| Rusturf Tunnel | Land | No field move; available when the location itself is reachable | 10 | Noibat | 13 | 8–10 |
| Rusturf Tunnel | Land | No field move; available when the location itself is reachable | 10 | Whismur | 13 | 8–10 |
| Rusturf Tunnel | Land | No field move; available when the location itself is reachable | 10 | Chingling | 10 | 8–10 |
| Rusturf Tunnel | Land | No field move; available when the location itself is reachable | 10 | Drilbur | 10 | 8–10 |
| Rusturf Tunnel | Land | No field move; available when the location itself is reachable | 10 | Dunsparce | 10 | 8–10 |
| Rusturf Tunnel | Land | No field move; available when the location itself is reachable | 10 | Teddiursa | 10 | 8–10 |
| Rusturf Tunnel | Land | No field move; available when the location itself is reachable | 10 | Larvitar | 9 | 8–10 |
| Rusturf Tunnel | Land | No field move; available when the location itself is reachable | 10 | Roggenrola | 9 | 8–10 |
| Rusturf Tunnel | Land | No field move; available when the location itself is reachable | 10 | Bagon | 8 | 8–10 |
| Rusturf Tunnel | Land | No field move; available when the location itself is reachable | 10 | Larvesta | 8 | 8–10 |
| Granite Cave 1F | Land | No field move; available when the location itself is reachable | 10 | Axew | 17 | 12–14 |
| Granite Cave 1F | Land | No field move; available when the location itself is reachable | 10 | Geodude | 13 | 12–14 |
| Granite Cave 1F | Land | No field move; available when the location itself is reachable | 10 | Zubat | 13 | 12–14 |
| Granite Cave 1F | Land | No field move; available when the location itself is reachable | 10 | Abra | 10 | 12–14 |
| Granite Cave 1F | Land | No field move; available when the location itself is reachable | 10 | Aron | 10 | 12–14 |
| Granite Cave 1F | Land | No field move; available when the location itself is reachable | 10 | Glimmet | 10 | 12–14 |
| Granite Cave 1F | Land | No field move; available when the location itself is reachable | 10 | Makuhita | 10 | 12–14 |
| Granite Cave 1F | Land | No field move; available when the location itself is reachable | 10 | Timburr | 9 | 12–14 |
| Granite Cave 1F | Land | No field move; available when the location itself is reachable | 10 | Rolycoly | 8 | 12–14 |
| Granite Cave B1F | Land | No field move; available when the location itself is reachable | 10 | Aron | 20 | 13–15 |
| Granite Cave B1F | Land | No field move; available when the location itself is reachable | 10 | Bronzor | 17 | 13–15 |
| Granite Cave B1F | Land | No field move; available when the location itself is reachable | 10 | Cubone | 17 | 13–15 |
| Granite Cave B1F | Land | No field move; available when the location itself is reachable | 10 | Mawile | 13 | 13–15 |
| Granite Cave B1F | Land | No field move; available when the location itself is reachable | 10 | Sableye | 13 | 13–15 |
| Granite Cave B1F | Land | No field move; available when the location itself is reachable | 10 | Carbink | 10 | 13–15 |
| Granite Cave B1F | Land | No field move; available when the location itself is reachable | 10 | Onix | 10 | 13–15 |
| Granite Cave B2F | Land | No field move; available when the location itself is reachable | 10 | Aron | 20 | 13–15 |
| Granite Cave B2F | Land | No field move; available when the location itself is reachable | 10 | Bronzor | 17 | 13–15 |
| Granite Cave B2F | Land | No field move; available when the location itself is reachable | 10 | Cubone | 17 | 13–15 |
| Granite Cave B2F | Land | No field move; available when the location itself is reachable | 10 | Mawile | 13 | 13–15 |
| Granite Cave B2F | Land | No field move; available when the location itself is reachable | 10 | Sableye | 13 | 13–15 |
| Granite Cave B2F | Land | No field move; available when the location itself is reachable | 10 | Carbink | 10 | 13–15 |
| Granite Cave B2F | Land | No field move; available when the location itself is reachable | 10 | Onix | 10 | 13–15 |
| Route 106 | Land | No field move; available when the location itself is reachable | 20 | Exeggcute | 34 | 3–5 |
| Route 106 | Land | No field move; available when the location itself is reachable | 20 | Buizel | 20 | 5–5 |
| Route 106 | Land | No field move; available when the location itself is reachable | 20 | Crabrawler | 20 | 4–5 |
| Route 106 | Land | No field move; available when the location itself is reachable | 20 | Chatot | 13 | 4–4 |
| Route 106 | Land | No field move; available when the location itself is reachable | 20 | Inkay | 13 | 4–4 |
| Route 106 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Clauncher | 60 | 10–15 |
| Route 106 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Skrelp | 40 | 10–15 |
| Granite Cave Stevens Room | Land | No field move; available when the location itself is reachable | 10 | Makuhita | 20 | 13–15 |
| Granite Cave Stevens Room | Land | No field move; available when the location itself is reachable | 10 | Aron | 13 | 13–15 |
| Granite Cave Stevens Room | Land | No field move; available when the location itself is reachable | 10 | Zubat | 13 | 13–15 |
| Granite Cave Stevens Room | Land | No field move; available when the location itself is reachable | 10 | Abra | 10 | 13–15 |
| Granite Cave Stevens Room | Land | No field move; available when the location itself is reachable | 10 | Beldum | 10 | 13–15 |
| Granite Cave Stevens Room | Land | No field move; available when the location itself is reachable | 10 | Axew | 9 | 13–15 |
| Granite Cave Stevens Room | Land | No field move; available when the location itself is reachable | 10 | Timburr | 9 | 13–15 |
| Granite Cave Stevens Room | Land | No field move; available when the location itself is reachable | 10 | Dreepy | 8 | 13–15 |
| Granite Cave Stevens Room | Land | No field move; available when the location itself is reachable | 10 | Larvesta | 8 | 13–15 |
| Dewford Town | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Staryu | 60 | 10–15 |
| Dewford Town | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Shellder | 40 | 10–15 |
| Dewford Meadow | Land | No field move; available when the location itself is reachable | 10 | Butterfree | 29 | 41–43 |
| Dewford Meadow | Land | No field move; available when the location itself is reachable | 10 | Combee | 15 | 41–43 |
| Dewford Meadow | Land | No field move; available when the location itself is reachable | 10 | Cutiefly | 13 | 41–43 |
| Dewford Meadow | Land | No field move; available when the location itself is reachable | 10 | Oricorio | 13 | 41–43 |
| Dewford Meadow | Land | No field move; available when the location itself is reachable | 10 | Flabebe | 10 | 41–43 |
| Dewford Meadow | Land | No field move; available when the location itself is reachable | 10 | Flabebe Orange Flower | 10 | 41–43 |
| Dewford Meadow | Land | No field move; available when the location itself is reachable | 10 | Flabebe Yellow Flower | 10 | 41–43 |
| Dewford Meadow | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 50 | 2–2 |
| Dewford Meadow | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Spritzee | 25 | 2–3 |
| Dewford Meadow | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Swirlix | 25 | 2–3 |
| Dewford Manor 1F | Land | No field move; available when the location itself is reachable | 20 | Mime Jr. | 24 | 41–43 |
| Dewford Manor 1F | Land | No field move; available when the location itself is reachable | 20 | Litwick | 20 | 41–43 |
| Dewford Manor 1F | Land | No field move; available when the location itself is reachable | 20 | Hoothoot | 15 | 41–43 |
| Dewford Manor 1F | Land | No field move; available when the location itself is reachable | 20 | Rattata | 15 | 41–43 |
| Dewford Manor 1F | Land | No field move; available when the location itself is reachable | 20 | Gastly | 13 | 41–43 |
| Dewford Manor 1F | Land | No field move; available when the location itself is reachable | 20 | Solosis | 13 | 41–43 |
| Rustboro City | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 60 | 2–3 |
| Rustboro City | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Nidorina | 20 | 2–3 |
| Rustboro City | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Nidorino | 20 | 2–3 |

### Knuckle Badge — Dewford to Slateport (cap 30)

| Location | Method | Access requirement | Raw rate | Species | Within-method % | Levels |
|---|---|---|---|---|---|---|
| Route 110 | Land | No field move; available when the location itself is reachable | 20 | Electrike | 13 | 12–12 |
| Route 110 | Land | No field move; available when the location itself is reachable | 20 | Gulpin | 13 | 12–12 |
| Route 110 | Land | No field move; available when the location itself is reachable | 20 | Gimmighoul Roaming | 10 | 13–13 |
| Route 110 | Land | No field move; available when the location itself is reachable | 20 | Magnemite | 10 | 13–13 |
| Route 110 | Land | No field move; available when the location itself is reachable | 20 | Minun | 10 | 13–13 |
| Route 110 | Land | No field move; available when the location itself is reachable | 20 | Plusle | 10 | 12–12 |
| Route 110 | Land | No field move; available when the location itself is reachable | 20 | Heliolisk | 8 | 12–12 |
| Route 110 | Land | No field move; available when the location itself is reachable | 20 | Toxel | 8 | 12–12 |
| Route 110 | Land | No field move; available when the location itself is reachable | 20 | Pachirisu | 5 | 13–13 |
| Route 110 | Land | No field move; available when the location itself is reachable | 20 | Trubbish | 5 | 13–13 |
| Route 110 | Land | No field move; available when the location itself is reachable | 20 | Morpeko | 4 | 12–12 |
| Route 110 | Land | No field move; available when the location itself is reachable | 20 | Stunky | 4 | 13–13 |
| Route 110 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Wailmer | 60 | 5–10 |
| Route 110 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Chinchou | 40 | 5–10 |
| Route 110 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 50 | 2–2 |
| Route 110 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Doduo | 20 | 2–3 |
| Route 110 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Ekans | 20 | 2–3 |
| Route 110 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Zigzagoon Galarian | 10 | 3–3 |
| Route 109 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Corsola | 60 | 5–10 |
| Route 109 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Mareanie | 40 | 5–10 |
| Slateport City | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Horsea | 60 | 5–10 |
| Slateport City | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Frillish | 40 | 5–10 |

### Dynamo Badge — Mauville, ash country, and Mt. Chimney (cap 40)

| Location | Method | Access requirement | Raw rate | Species | Within-method % | Levels |
|---|---|---|---|---|---|---|
| Route 103 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Pineco | 60 | 10–15 |
| Route 103 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Hoothoot | 40 | 5–20 |
| Route 111 | Land | No field move; available when the location itself is reachable | 10 | Cacnea | 13 | 20–20 |
| Route 111 | Land | No field move; available when the location itself is reachable | 10 | Sandshrew | 13 | 20–20 |
| Route 111 | Land | No field move; available when the location itself is reachable | 10 | Baltoy | 10 | 19–19 |
| Route 111 | Land | No field move; available when the location itself is reachable | 10 | Gible | 10 | 19–19 |
| Route 111 | Land | No field move; available when the location itself is reachable | 10 | Great Tusk | 10 | 21–21 |
| Route 111 | Land | No field move; available when the location itself is reachable | 10 | Trapinch | 10 | 21–21 |
| Route 111 | Land | No field move; available when the location itself is reachable | 10 | Sandile | 8 | 21–21 |
| Route 111 | Land | No field move; available when the location itself is reachable | 10 | Silicobra | 8 | 20–20 |
| Route 111 | Land | No field move; available when the location itself is reachable | 10 | Hippopotas | 5 | 20–20 |
| Route 111 | Land | No field move; available when the location itself is reachable | 10 | Maractus | 5 | 19–19 |
| Route 111 | Land | No field move; available when the location itself is reachable | 10 | Stonjourner | 4 | 22–22 |
| Route 111 | Land | No field move; available when the location itself is reachable | 10 | Yamask Galarian | 4 | 22–22 |
| Route 111 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Geodude | 60 | 10–15 |
| Route 111 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Graveler | 30 | 5–10 |
| Route 111 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Shuckle | 10 | 15–20 |
| Route 111 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Barboach | 60 | 5–10 |
| Route 111 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Goldeen | 40 | 5–10 |
| Route 112 | Land | No field move; available when the location itself is reachable | 20 | Machoke | 13 | 15–15 |
| Route 112 | Land | No field move; available when the location itself is reachable | 20 | Numel | 13 | 15–15 |
| Route 112 | Land | No field move; available when the location itself is reachable | 20 | Ponyta | 10 | 14–14 |
| Route 112 | Land | No field move; available when the location itself is reachable | 20 | Sawk | 10 | 14–14 |
| Route 112 | Land | No field move; available when the location itself is reachable | 20 | Throh | 10 | 16–16 |
| Route 112 | Land | No field move; available when the location itself is reachable | 20 | Tyrogue | 10 | 14–14 |
| Route 112 | Land | No field move; available when the location itself is reachable | 20 | Rufflet | 9 | 16–16 |
| Route 112 | Land | No field move; available when the location itself is reachable | 20 | Vullaby | 9 | 16–16 |
| Route 112 | Land | No field move; available when the location itself is reachable | 20 | Hawlucha | 8 | 16–16 |
| Route 112 | Land | No field move; available when the location itself is reachable | 20 | Kubfu | 8 | 16–16 |
| Route 112 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 60 | 2–3 |
| Route 112 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Sawk | 15 | 2–2 |
| Route 112 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Throh | 15 | 2–2 |
| Route 112 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Hitmonchan | 5 | 3–3 |
| Route 112 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Hitmonlee | 5 | 3–3 |
| Route 113 | Land | No field move; available when the location itself is reachable | 20 | Mienfoo | 17 | 16–16 |
| Route 113 | Land | No field move; available when the location itself is reachable | 20 | Scraggy | 13 | 15–15 |
| Route 113 | Land | No field move; available when the location itself is reachable | 20 | Skarmory | 13 | 16–16 |
| Route 113 | Land | No field move; available when the location itself is reachable | 20 | Spinda | 13 | 15–15 |
| Route 113 | Land | No field move; available when the location itself is reachable | 20 | Bouffalant | 10 | 14–14 |
| Route 113 | Land | No field move; available when the location itself is reachable | 20 | Fletchinder | 10 | 14–14 |
| Route 113 | Land | No field move; available when the location itself is reachable | 20 | Klefki | 10 | 14–14 |
| Route 113 | Land | No field move; available when the location itself is reachable | 20 | Pawniard | 10 | 15–15 |
| Route 113 | Land | No field move; available when the location itself is reachable | 20 | Falinks | 4 | 16–16 |
| Route 113 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Spinda | 90 | 2–3 |
| Route 113 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Mienshao | 5 | 3–3 |
| Route 113 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Talonflame | 5 | 3–3 |
| Route 114 | Land | No field move; available when the location itself is reachable | 20 | Phanpy | 17 | 16–17 |
| Route 114 | Land | No field move; available when the location itself is reachable | 20 | Skorupi | 17 | 15–18 |
| Route 114 | Land | No field move; available when the location itself is reachable | 20 | Ducklett | 13 | 16–16 |
| Route 114 | Land | No field move; available when the location itself is reachable | 20 | Swablu | 13 | 16–16 |
| Route 114 | Land | No field move; available when the location itself is reachable | 20 | Lombre | 10 | 17–17 |
| Route 114 | Land | No field move; available when the location itself is reachable | 20 | Nuzleaf | 10 | 15–15 |
| Route 114 | Land | No field move; available when the location itself is reachable | 20 | Seviper | 10 | 16–16 |
| Route 114 | Land | No field move; available when the location itself is reachable | 20 | Zangoose | 10 | 15–15 |
| Route 114 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Geodude | 60 | 10–15 |
| Route 114 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Graveler | 30 | 5–10 |
| Route 114 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Shuckle | 10 | 15–20 |
| Route 114 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Wishiwashi | 60 | 5–10 |
| Route 114 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Barboach | 40 | 5–10 |
| Route 114 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 50 | 2–2 |
| Route 114 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Donphan | 25 | 2–3 |
| Route 114 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Quagsire | 25 | 2–3 |
| Route 116 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Spinarak | 60 | 10–15 |
| Route 116 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Fomantis | 40 | 5–20 |
| Route 117 | Land | No field move; available when the location itself is reachable | 20 | Deerling | 13 | 13–13 |
| Route 117 | Land | No field move; available when the location itself is reachable | 20 | Roselia | 13 | 13–13 |
| Route 117 | Land | No field move; available when the location itself is reachable | 20 | Farfetch'd | 10 | 13–13 |
| Route 117 | Land | No field move; available when the location itself is reachable | 20 | Illumise | 10 | 14–14 |
| Route 117 | Land | No field move; available when the location itself is reachable | 20 | Meowth | 10 | 13–13 |
| Route 117 | Land | No field move; available when the location itself is reachable | 20 | Volbeat | 10 | 14–14 |
| Route 117 | Land | No field move; available when the location itself is reachable | 20 | Grookey | 8 | 14–14 |
| Route 117 | Land | No field move; available when the location itself is reachable | 20 | Scorbunny | 8 | 14–14 |
| Route 117 | Land | No field move; available when the location itself is reachable | 20 | Farfetchd Galarian | 5 | 13–13 |
| Route 117 | Land | No field move; available when the location itself is reachable | 20 | Minccino | 5 | 13–13 |
| Route 117 | Land | No field move; available when the location itself is reachable | 20 | Gossifleur | 4 | 13–13 |
| Route 117 | Land | No field move; available when the location itself is reachable | 20 | Wooloo | 4 | 13–13 |
| Route 117 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Floette | 60 | 10–15 |
| Route 117 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Sunkern | 40 | 5–20 |
| Route 117 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Goldeen | 60 | 5–10 |
| Route 117 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Corphish | 40 | 5–10 |
| Route 117 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 60 | 2–3 |
| Route 117 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Karrablast | 20 | 2–3 |
| Route 117 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Shelmet | 20 | 2–3 |
| Petalburg Woods | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Aipom | 60 | 10–15 |
| Petalburg Woods | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Cherubi | 40 | 5–20 |
| Petalburg Woods 2 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Exeggcute | 60 | 10–15 |
| Petalburg Woods 2 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Venonat | 40 | 5–20 |
| Petalburg Woods 3 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Beedrill | 60 | 10–15 |
| Petalburg Woods 3 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Trevenant | 40 | 5–20 |
| Granite Cave B2F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Nosepass | 70 | 22–25 |
| Granite Cave B2F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Dwebble | 30 | 22–25 |
| Fiery Path | Land | No field move; available when the location itself is reachable | 10 | Diglett | 13 | 15–15 |
| Fiery Path | Land | No field move; available when the location itself is reachable | 10 | Slugma | 13 | 15–15 |
| Fiery Path | Land | No field move; available when the location itself is reachable | 10 | Grimer | 10 | 15–15 |
| Fiery Path | Land | No field move; available when the location itself is reachable | 10 | Koffing | 10 | 15–15 |
| Fiery Path | Land | No field move; available when the location itself is reachable | 10 | Magby | 10 | 15–15 |
| Fiery Path | Land | No field move; available when the location itself is reachable | 10 | Torkoal | 10 | 16–16 |
| Fiery Path | Land | No field move; available when the location itself is reachable | 10 | Durant | 9 | 14–16 |
| Fiery Path | Land | No field move; available when the location itself is reachable | 10 | Charmander | 8 | 16–16 |
| Fiery Path | Land | No field move; available when the location itself is reachable | 10 | Larvesta | 8 | 14–14 |
| Fiery Path | Land | No field move; available when the location itself is reachable | 10 | Heatmor | 5 | 16–16 |
| Fiery Path | Land | No field move; available when the location itself is reachable | 10 | Sizzlipede | 4 | 14–14 |
| Meteor Falls B1F 2R | Land | No field move; available when the location itself is reachable | 10 | Bagon | 43 | 30–40 |
| Meteor Falls B1F 2R | Land | No field move; available when the location itself is reachable | 10 | Golbat | 20 | 25–39 |
| Meteor Falls B1F 2R | Land | No field move; available when the location itself is reachable | 10 | Solrock | 13 | 33–33 |
| Meteor Falls B1F 2R | Land | No field move; available when the location itself is reachable | 10 | Lunatone | 10 | 37–37 |
| Meteor Falls B1F 2R | Land | No field move; available when the location itself is reachable | 10 | Roaring Moon | 10 | 35–35 |
| Meteor Falls B1F 2R | Land | No field move; available when the location itself is reachable | 10 | Salamence | 4 | 38–38 |
| Meteor Falls B1F 2R | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Barboach | 60 | 5–10 |
| Meteor Falls B1F 2R | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Dratini | 40 | 5–10 |
| Jagged Pass | Land | No field move; available when the location itself is reachable | 20 | Grumpig | 13 | 21–21 |
| Jagged Pass | Land | No field move; available when the location itself is reachable | 20 | Salandit | 13 | 21–21 |
| Jagged Pass | Land | No field move; available when the location itself is reachable | 20 | Fearow | 10 | 20–20 |
| Jagged Pass | Land | No field move; available when the location itself is reachable | 20 | Gligar | 10 | 20–20 |
| Jagged Pass | Land | No field move; available when the location itself is reachable | 20 | Mudbray | 10 | 20–20 |
| Jagged Pass | Land | No field move; available when the location itself is reachable | 20 | Primeape | 10 | 21–21 |
| Jagged Pass | Land | No field move; available when the location itself is reachable | 20 | Jangmo-o | 9 | 21–22 |
| Jagged Pass | Land | No field move; available when the location itself is reachable | 20 | Turtonator | 9 | 22–22 |
| Jagged Pass | Land | No field move; available when the location itself is reachable | 20 | Bagon | 8 | 22–22 |
| Jagged Pass | Land | No field move; available when the location itself is reachable | 20 | Deino | 8 | 22–22 |
| Jagged Pass | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 70 | 2–3 |
| Jagged Pass | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Hakamo-o | 25 | 2–3 |
| Jagged Pass | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Salazzle | 5 | 3–3 |
| Route 106 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 25 | Binacle | 60 | 25–30 |
| Route 106 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 25 | Wimpod | 30 | 20–25 |
| Route 106 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 25 | Dwebble | 10 | 30–35 |
| Route 109 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 255 | Sandygast | 65 | 14–16 |
| Route 109 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 255 | Pyukumuku | 30 | 14–16 |
| Route 109 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 255 | Pincurchin | 5 | 14–16 |
| New Mauville Inside | Land | No field move; available when the location itself is reachable | 10 | Magneton | 23 | 23–24 |
| New Mauville Inside | Land | No field move; available when the location itself is reachable | 10 | Electabuzz | 17 | 22–26 |
| New Mauville Inside | Land | No field move; available when the location itself is reachable | 10 | Togedemaru | 17 | 22–26 |
| New Mauville Inside | Land | No field move; available when the location itself is reachable | 10 | Klang | 13 | 24–24 |
| New Mauville Inside | Land | No field move; available when the location itself is reachable | 10 | Elekid | 10 | 25–25 |
| New Mauville Inside | Land | No field move; available when the location itself is reachable | 10 | Iron Hands | 10 | 23–23 |
| New Mauville Inside | Land | No field move; available when the location itself is reachable | 10 | Klink | 10 | 25–25 |
| New Mauville Entrance | Land | No field move; available when the location itself is reachable | 10 | Magneton | 23 | 23–24 |
| New Mauville Entrance | Land | No field move; available when the location itself is reachable | 10 | Electabuzz | 17 | 22–26 |
| New Mauville Entrance | Land | No field move; available when the location itself is reachable | 10 | Togedemaru | 17 | 22–26 |
| New Mauville Entrance | Land | No field move; available when the location itself is reachable | 10 | Klang | 13 | 24–24 |
| New Mauville Entrance | Land | No field move; available when the location itself is reachable | 10 | Electrode | 10 | 23–23 |
| New Mauville Entrance | Land | No field move; available when the location itself is reachable | 10 | Elekid | 10 | 25–25 |
| New Mauville Entrance | Land | No field move; available when the location itself is reachable | 10 | Klink | 10 | 25–25 |
| Meteor Falls 1F 1R | Land | No field move; available when the location itself is reachable | 10 | Minior | 20 | 15–18 |
| Meteor Falls 1F 1R | Land | No field move; available when the location itself is reachable | 10 | Druddigon | 17 | 14–20 |
| Meteor Falls 1F 1R | Land | No field move; available when the location itself is reachable | 10 | Ferroseed | 17 | 18–19 |
| Meteor Falls 1F 1R | Land | No field move; available when the location itself is reachable | 10 | Lunatone | 13 | 16–16 |
| Meteor Falls 1F 1R | Land | No field move; available when the location itself is reachable | 10 | Solrock | 13 | 17–17 |
| Meteor Falls 1F 1R | Land | No field move; available when the location itself is reachable | 10 | Clefairy | 10 | 14–14 |
| Meteor Falls 1F 1R | Land | No field move; available when the location itself is reachable | 10 | Deino | 10 | 16–16 |
| Meteor Falls 1F 1R | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Barboach | 60 | 5–10 |
| Meteor Falls 1F 1R | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Dratini | 40 | 5–10 |
| Meteor Falls 1F 2R | Land | No field move; available when the location itself is reachable | 10 | Golbat | 20 | 15–18 |
| Meteor Falls 1F 2R | Land | No field move; available when the location itself is reachable | 10 | Druddigon | 17 | 14–20 |
| Meteor Falls 1F 2R | Land | No field move; available when the location itself is reachable | 10 | Ferrothorn | 17 | 18–19 |
| Meteor Falls 1F 2R | Land | No field move; available when the location itself is reachable | 10 | Lunatone | 13 | 16–16 |
| Meteor Falls 1F 2R | Land | No field move; available when the location itself is reachable | 10 | Solrock | 13 | 17–17 |
| Meteor Falls 1F 2R | Land | No field move; available when the location itself is reachable | 10 | Clefairy | 10 | 14–14 |
| Meteor Falls 1F 2R | Land | No field move; available when the location itself is reachable | 10 | Zweilous | 10 | 16–16 |
| Meteor Falls 1F 2R | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Barboach | 60 | 5–10 |
| Meteor Falls 1F 2R | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Dratini | 40 | 5–10 |
| Meteor Falls B1F 1R | Land | No field move; available when the location itself is reachable | 10 | Druddigon | 17 | 14–20 |
| Meteor Falls B1F 1R | Land | No field move; available when the location itself is reachable | 10 | Ferrothorn | 17 | 18–19 |
| Meteor Falls B1F 1R | Land | No field move; available when the location itself is reachable | 10 | Lunatone | 13 | 16–16 |
| Meteor Falls B1F 1R | Land | No field move; available when the location itself is reachable | 10 | Solrock | 13 | 17–17 |
| Meteor Falls B1F 1R | Land | No field move; available when the location itself is reachable | 10 | Clefairy | 10 | 14–14 |
| Meteor Falls B1F 1R | Land | No field move; available when the location itself is reachable | 10 | Drampa | 10 | 18–18 |
| Meteor Falls B1F 1R | Land | No field move; available when the location itself is reachable | 10 | Golbat | 10 | 15–15 |
| Meteor Falls B1F 1R | Land | No field move; available when the location itself is reachable | 10 | Zweilous | 10 | 16–16 |
| Meteor Falls B1F 1R | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Barboach | 60 | 5–10 |
| Meteor Falls B1F 1R | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Dratini | 40 | 5–10 |
| Dewford Town | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 25 | Binacle | 60 | 25–30 |
| Dewford Town | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 25 | Wimpod | 30 | 20–25 |
| Dewford Town | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 25 | Dwebble | 10 | 30–35 |
| Route 111 Ruins Exterior | Land | No field move; available when the location itself is reachable | 10 | Rockruff | 20 | 20–20 |
| Route 111 Ruins Exterior | Land | No field move; available when the location itself is reachable | 10 | Meditite | 17 | 22–24 |
| Route 111 Ruins Exterior | Land | No field move; available when the location itself is reachable | 10 | Skiploom | 17 | 22–24 |
| Route 111 Ruins Exterior | Land | No field move; available when the location itself is reachable | 10 | Helioptile | 13 | 21–21 |
| Route 111 Ruins Exterior | Land | No field move; available when the location itself is reachable | 10 | Xatu | 13 | 21–21 |
| Route 111 Ruins Exterior | Land | No field move; available when the location itself is reachable | 10 | Girafarig | 10 | 20–20 |
| Route 111 Ruins Exterior | Land | No field move; available when the location itself is reachable | 10 | Hawlucha | 10 | 20–20 |
| Route 111 Ruins Exterior | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Jumpluff | 100 | 2–3 |

### Heat Badge — Petalburg return and eastern routes (cap 45)

| Location | Method | Access requirement | Raw rate | Species | Within-method % | Levels |
|---|---|---|---|---|---|---|
| Petalburg City | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Goldeen | 60 | 5–10 |
| Petalburg City | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Corphish | 40 | 5–10 |
| Ashen Woods | Land | No field move; available when the location itself is reachable | 10 | Noctowl | 24 | 41–43 |
| Ashen Woods | Land | No field move; available when the location itself is reachable | 10 | Growlithe | 13 | 41–43 |
| Ashen Woods | Land | No field move; available when the location itself is reachable | 10 | Salandit | 13 | 41–43 |
| Ashen Woods | Land | No field move; available when the location itself is reachable | 10 | Camerupt | 10 | 41–43 |
| Ashen Woods | Land | No field move; available when the location itself is reachable | 10 | Chi Yu | 10 | 41–43 |
| Ashen Woods | Land | No field move; available when the location itself is reachable | 10 | Heracross | 10 | 41–43 |
| Ashen Woods | Land | No field move; available when the location itself is reachable | 10 | Pinsir | 10 | 41–43 |
| Ashen Woods | Land | No field move; available when the location itself is reachable | 10 | Trumbeak | 10 | 41–43 |
| Ashen Woods | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 90 | 2–3 |
| Ashen Woods | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Salazzle | 5 | 3–3 |
| Ashen Woods | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Toucannon | 5 | 3–3 |

### Balance Badge — Fortree, Safari Zone, and Mt. Pyre (cap 55)

| Location | Method | Access requirement | Raw rate | Species | Within-method % | Levels |
|---|---|---|---|---|---|---|
| Route 102 | Surf | Balance Badge (5) and HM03 Surf | 4 | Surskit | 60 | 20–30 |
| Route 102 | Surf | Balance Badge (5) and HM03 Surf | 4 | Azumarill | 30 | 10–20 |
| Route 102 | Surf | Balance Badge (5) and HM03 Surf | 4 | Masquerain | 10 | 5–35 |
| Route 102 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Corphish | 60 | 10–30 |
| Route 102 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Crawdaunt | 20 | 10–30 |
| Route 102 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Seaking | 20 | 10–30 |
| Route 103 | Surf | Balance Badge (5) and HM03 Surf | 4 | Wingull | 60 | 5–35 |
| Route 103 | Surf | Balance Badge (5) and HM03 Surf | 4 | Gastrodon | 30 | 10–30 |
| Route 103 | Surf | Balance Badge (5) and HM03 Surf | 4 | Pelipper | 10 | 15–30 |
| Route 103 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Tentacool | 60 | 10–30 |
| Route 103 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Gyarados | 20 | 10–30 |
| Route 103 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Wailmer | 20 | 10–30 |
| Route 104 | Surf | Balance Badge (5) and HM03 Surf | 4 | Wingull | 90 | 10–30 |
| Route 104 | Surf | Balance Badge (5) and HM03 Surf | 4 | Pelipper | 10 | 25–30 |
| Route 104 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Luvdisc | 60 | 10–30 |
| Route 104 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Finizen | 20 | 10–30 |
| Route 104 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Magikarp | 20 | 10–30 |
| Route 105 | Surf | Balance Badge (5) and HM03 Surf | 4 | Tentacool | 60 | 5–35 |
| Route 105 | Surf | Balance Badge (5) and HM03 Surf | 4 | Wingull | 30 | 10–30 |
| Route 105 | Surf | Balance Badge (5) and HM03 Surf | 4 | Pelipper | 10 | 15–30 |
| Route 105 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Skrelp | 60 | 10–30 |
| Route 105 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Clauncher | 40 | 10–30 |
| Route 110 | Surf | Balance Badge (5) and HM03 Surf | 4 | Shellos | 60 | 5–35 |
| Route 110 | Surf | Balance Badge (5) and HM03 Surf | 4 | Gastrodon | 30 | 10–30 |
| Route 110 | Surf | Balance Badge (5) and HM03 Surf | 4 | Pelipper | 10 | 15–30 |
| Route 110 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Chinchou | 60 | 10–30 |
| Route 110 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Wailmer | 40 | 10–30 |
| Route 111 | Surf | Balance Badge (5) and HM03 Surf | 4 | Marill | 60 | 20–30 |
| Route 111 | Surf | Balance Badge (5) and HM03 Surf | 4 | Surskit | 30 | 10–20 |
| Route 111 | Surf | Balance Badge (5) and HM03 Surf | 4 | Azumarill | 5 | 5–10 |
| Route 111 | Surf | Balance Badge (5) and HM03 Surf | 4 | Masquerain | 5 | 30–35 |
| Route 111 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Goldeen | 60 | 10–30 |
| Route 111 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Basculin | 40 | 10–30 |
| Route 114 | Surf | Balance Badge (5) and HM03 Surf | 4 | Wooper | 60 | 20–30 |
| Route 114 | Surf | Balance Badge (5) and HM03 Surf | 4 | Quagsire | 30 | 10–20 |
| Route 114 | Surf | Balance Badge (5) and HM03 Surf | 4 | Masquerain | 10 | 5–35 |
| Route 114 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Barboach | 80 | 10–30 |
| Route 114 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Wishiwashi | 20 | 10–30 |
| Route 117 | Surf | Balance Badge (5) and HM03 Surf | 4 | Marill | 60 | 20–30 |
| Route 117 | Surf | Balance Badge (5) and HM03 Surf | 4 | Azumarill | 30 | 10–20 |
| Route 117 | Surf | Balance Badge (5) and HM03 Surf | 4 | Seaking | 10 | 5–35 |
| Route 117 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Corphish | 60 | 10–30 |
| Route 117 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Goldeen | 40 | 10–30 |
| Route 118 | Land | No field move; available when the location itself is reachable | 20 | Linoone | 13 | 24–24 |
| Route 118 | Land | No field move; available when the location itself is reachable | 20 | Manectric | 13 | 24–24 |
| Route 118 | Land | No field move; available when the location itself is reachable | 20 | Lickitung | 10 | 26–26 |
| Route 118 | Land | No field move; available when the location itself is reachable | 20 | Liepard | 10 | 26–26 |
| Route 118 | Land | No field move; available when the location itself is reachable | 20 | Passimian | 10 | 26–26 |
| Route 118 | Land | No field move; available when the location itself is reachable | 20 | Raticate | 10 | 26–26 |
| Route 118 | Land | No field move; available when the location itself is reachable | 20 | Carnivine | 9 | 25–25 |
| Route 118 | Land | No field move; available when the location itself is reachable | 20 | Dedenne | 9 | 25–27 |
| Route 118 | Land | No field move; available when the location itself is reachable | 20 | Type: Null | 8 | 26–26 |
| Route 118 | Land | No field move; available when the location itself is reachable | 20 | Zorua | 8 | 26–26 |
| Route 118 | Surf | Balance Badge (5) and HM03 Surf | 4 | Tentacool | 60 | 5–35 |
| Route 118 | Surf | Balance Badge (5) and HM03 Surf | 4 | Dondozo | 30 | 10–30 |
| Route 118 | Surf | Balance Badge (5) and HM03 Surf | 4 | Pelipper | 10 | 25–30 |
| Route 118 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Fearow | 60 | 10–15 |
| Route 118 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Venomoth | 40 | 5–20 |
| Route 118 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 60 | 5–10 |
| Route 118 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Carvanha | 40 | 5–10 |
| Route 118 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Tatsugiri | 60 | 10–30 |
| Route 118 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Tatsugiri Droopy | 20 | 10–30 |
| Route 118 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Tatsugiri Stretchy | 20 | 10–30 |
| Route 118 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 60 | 2–3 |
| Route 118 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Dedenne | 20 | 2–3 |
| Route 118 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Lickilicky | 20 | 2–3 |
| Petalburg Woods 3 | Surf | Balance Badge (5) and HM03 Surf | 4 | Poliwag | 60 | 10–30 |
| Petalburg Woods 3 | Surf | Balance Badge (5) and HM03 Surf | 4 | Slowpoke | 30 | 15–25 |
| Petalburg Woods 3 | Surf | Balance Badge (5) and HM03 Surf | 4 | Poliwhirl | 10 | 25–30 |
| Petalburg Woods 3 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Slowpoke | 60 | 10–30 |
| Petalburg Woods 3 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Poliwag | 20 | 10–30 |
| Petalburg Woods 3 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Poliwhirl | 20 | 10–30 |
| Mt Pyre 1F | Land | No field move; available when the location itself is reachable | 10 | Litwick | 20 | 26–29 |
| Mt Pyre 1F | Land | No field move; available when the location itself is reachable | 10 | Misdreavus | 17 | 23–29 |
| Mt Pyre 1F | Land | No field move; available when the location itself is reachable | 10 | Murkrow | 17 | 22–24 |
| Mt Pyre 1F | Land | No field move; available when the location itself is reachable | 10 | Duskull | 13 | 28–28 |
| Mt Pyre 1F | Land | No field move; available when the location itself is reachable | 10 | Shuppet | 13 | 27–27 |
| Mt Pyre 1F | Land | No field move; available when the location itself is reachable | 10 | Golbat | 10 | 25–25 |
| Mt Pyre 1F | Land | No field move; available when the location itself is reachable | 10 | Haunter | 10 | 24–24 |
| Safari Zone South | Land | No field move; available when the location itself is reachable | 25 | Meowth Alolan | 20 | 25–27 |
| Safari Zone South | Land | No field move; available when the location itself is reachable | 25 | Persian Alolan | 17 | 25–27 |
| Safari Zone South | Land | No field move; available when the location itself is reachable | 25 | Raichu Alolan | 17 | 27–29 |
| Safari Zone South | Land | No field move; available when the location itself is reachable | 25 | Pikachu | 13 | 27–27 |
| Safari Zone South | Land | No field move; available when the location itself is reachable | 25 | Wobbuffet | 13 | 25–25 |
| Safari Zone South | Land | No field move; available when the location itself is reachable | 25 | Girafarig | 10 | 25–25 |
| Safari Zone South | Land | No field move; available when the location itself is reachable | 25 | Smeargle | 10 | 25–25 |
| Abandoned Ship Rooms B1F | Surf | Balance Badge (5) and HM03 Surf | 4 | Frillish | 95 | 5–35 |
| Abandoned Ship Rooms B1F | Surf | Balance Badge (5) and HM03 Surf | 4 | Jellicent | 5 | 5–35 |
| Abandoned Ship Rooms B1F | Old Rod | Old Rod from Mom in Littleroot during the opening | 20 | Frillish | 60 | 5–10 |
| Abandoned Ship Rooms B1F | Old Rod | Old Rod from Mom in Littleroot during the opening | 20 | Skrelp | 40 | 5–10 |
| Abandoned Ship Rooms B1F | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 20 | Skrelp | 60 | 10–30 |
| Abandoned Ship Rooms B1F | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 20 | Dhelmise | 20 | 10–30 |
| Abandoned Ship Rooms B1F | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 20 | Frillish | 20 | 10–30 |
| Meteor Falls B1F 2R | Surf | Balance Badge (5) and HM03 Surf | 4 | Golbat | 60 | 5–35 |
| Meteor Falls B1F 2R | Surf | Balance Badge (5) and HM03 Surf | 4 | Dratini | 30 | 30–35 |
| Meteor Falls B1F 2R | Surf | Balance Badge (5) and HM03 Surf | 4 | Lunatone | 5 | 15–25 |
| Meteor Falls B1F 2R | Surf | Balance Badge (5) and HM03 Surf | 4 | Solrock | 5 | 25–35 |
| Meteor Falls B1F 2R | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Dratini | 60 | 10–30 |
| Meteor Falls B1F 2R | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Barboach | 20 | 10–30 |
| Meteor Falls B1F 2R | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Dragonair | 20 | 10–30 |
| Route 106 | Surf | Balance Badge (5) and HM03 Surf | 4 | Tentacool | 60 | 5–35 |
| Route 106 | Surf | Balance Badge (5) and HM03 Surf | 4 | Wingull | 30 | 10–30 |
| Route 106 | Surf | Balance Badge (5) and HM03 Surf | 4 | Pelipper | 10 | 15–30 |
| Route 106 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Skrelp | 60 | 10–15 |
| Route 106 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Clauncher | 40 | 10–30 |
| Route 107 | Surf | Balance Badge (5) and HM03 Surf | 4 | Frillish | 60 | 5–35 |
| Route 107 | Surf | Balance Badge (5) and HM03 Surf | 4 | Mantyke | 30 | 10–30 |
| Route 107 | Surf | Balance Badge (5) and HM03 Surf | 4 | Wingull | 10 | 15–30 |
| Route 107 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Remoraid | 60 | 5–10 |
| Route 107 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Finneon | 40 | 5–10 |
| Route 107 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Finneon | 60 | 10–30 |
| Route 107 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Remoraid | 40 | 10–30 |
| Route 108 | Surf | Balance Badge (5) and HM03 Surf | 4 | Frillish | 60 | 5–35 |
| Route 108 | Surf | Balance Badge (5) and HM03 Surf | 4 | Mantyke | 30 | 10–30 |
| Route 108 | Surf | Balance Badge (5) and HM03 Surf | 4 | Wingull | 10 | 15–30 |
| Route 108 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Remoraid | 60 | 5–10 |
| Route 108 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Finneon | 40 | 5–10 |
| Route 108 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Finneon | 60 | 10–30 |
| Route 108 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Remoraid | 40 | 10–30 |
| Route 109 | Surf | Balance Badge (5) and HM03 Surf | 4 | Frillish | 60 | 5–35 |
| Route 109 | Surf | Balance Badge (5) and HM03 Surf | 4 | Mantyke | 30 | 10–30 |
| Route 109 | Surf | Balance Badge (5) and HM03 Surf | 4 | Pyukumuku | 10 | 15–30 |
| Route 109 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Corsola | 60 | 10–30 |
| Route 109 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Clobbopus | 20 | 10–30 |
| Route 109 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Mareanie | 20 | 10–30 |
| Route 115 | Land | No field move; available when the location itself is reachable | 20 | Tangela | 22 | 24–25 |
| Route 115 | Land | No field move; available when the location itself is reachable | 20 | Jigglypuff | 13 | 23–23 |
| Route 115 | Land | No field move; available when the location itself is reachable | 20 | Swellow | 13 | 23–23 |
| Route 115 | Land | No field move; available when the location itself is reachable | 20 | Munchlax | 12 | 24–26 |
| Route 115 | Land | No field move; available when the location itself is reachable | 20 | Dodrio | 10 | 24–24 |
| Route 115 | Land | No field move; available when the location itself is reachable | 20 | Duraludon | 10 | 25–25 |
| Route 115 | Land | No field move; available when the location itself is reachable | 20 | Lurantis | 10 | 24–25 |
| Route 115 | Land | No field move; available when the location itself is reachable | 20 | Spritzee | 10 | 25–25 |
| Route 115 | Surf | Balance Badge (5) and HM03 Surf | 4 | Wishiwashi | 60 | 5–35 |
| Route 115 | Surf | Balance Badge (5) and HM03 Surf | 4 | Wingull | 30 | 10–30 |
| Route 115 | Surf | Balance Badge (5) and HM03 Surf | 4 | Tentacruel | 10 | 15–30 |
| Route 115 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Wishiwashi | 60 | 5–10 |
| Route 115 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Wailmer | 40 | 5–10 |
| Route 115 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Wailmer | 60 | 10–30 |
| Route 115 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Qwilfish | 20 | 10–30 |
| Route 115 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Wishiwashi | 20 | 10–30 |
| Route 115 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 90 | 2–3 |
| Route 115 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Aromatisse | 5 | 3–3 |
| Route 115 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Slurpuff | 5 | 3–3 |
| Route 119 | Land | No field move; available when the location itself is reachable | 15 | Goomy | 14 | 25–27 |
| Route 119 | Land | No field move; available when the location itself is reachable | 15 | Gumshoos | 13 | 25–25 |
| Route 119 | Land | No field move; available when the location itself is reachable | 15 | Tropius | 13 | 25–25 |
| Route 119 | Land | No field move; available when the location itself is reachable | 15 | Comfey | 10 | 26–26 |
| Route 119 | Land | No field move; available when the location itself is reachable | 15 | Cramorant | 10 | 24–27 |
| Route 119 | Land | No field move; available when the location itself is reachable | 15 | Oranguru | 10 | 25–25 |
| Route 119 | Land | No field move; available when the location itself is reachable | 15 | Raging Bolt | 10 | 27–27 |
| Route 119 | Land | No field move; available when the location itself is reachable | 15 | Dreepy | 8 | 25–25 |
| Route 119 | Land | No field move; available when the location itself is reachable | 15 | Larvesta | 8 | 26–26 |
| Route 119 | Land | No field move; available when the location itself is reachable | 15 | Amoonguss | 4 | 27–27 |
| Route 119 | Surf | Balance Badge (5) and HM03 Surf | 4 | Gastrodon East Sea | 60 | 5–35 |
| Route 119 | Surf | Balance Badge (5) and HM03 Surf | 4 | Floatzel | 30 | 10–30 |
| Route 119 | Surf | Balance Badge (5) and HM03 Surf | 4 | Pelipper | 10 | 25–30 |
| Route 119 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Carvanha | 60 | 5–10 |
| Route 119 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 40 | 5–10 |
| Route 119 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Carvanha | 60 | 10–30 |
| Route 119 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Magikarp | 20 | 10–30 |
| Route 119 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Sharpedo | 20 | 10–30 |
| Route 119 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 60 | 2–3 |
| Route 119 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Amoonguss | 20 | 2–3 |
| Route 119 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Sliggoo | 20 | 2–3 |
| Route 120 | Land | No field move; available when the location itself is reachable | 20 | Absol | 13 | 25–25 |
| Route 120 | Land | No field move; available when the location itself is reachable | 20 | Watchog | 13 | 25–25 |
| Route 120 | Land | No field move; available when the location itself is reachable | 20 | Ogerpon | 10 | 25–25 |
| Route 120 | Land | No field move; available when the location itself is reachable | 20 | Pumpkaboo Large | 10 | 27–27 |
| Route 120 | Land | No field move; available when the location itself is reachable | 20 | Pumpkaboo Small | 10 | 26–26 |
| Route 120 | Land | No field move; available when the location itself is reachable | 20 | Tropius | 10 | 25–25 |
| Route 120 | Land | No field move; available when the location itself is reachable | 20 | Venomoth | 10 | 27–27 |
| Route 120 | Land | No field move; available when the location itself is reachable | 20 | Honedge | 8 | 27–27 |
| Route 120 | Land | No field move; available when the location itself is reachable | 20 | Mimikyu | 8 | 25–25 |
| Route 120 | Land | No field move; available when the location itself is reachable | 20 | Pumpkaboo | 4 | 25–25 |
| Route 120 | Land | No field move; available when the location itself is reachable | 20 | Pumpkaboo Super | 4 | 25–25 |
| Route 120 | Surf | Balance Badge (5) and HM03 Surf | 4 | Azumarill | 60 | 20–30 |
| Route 120 | Surf | Balance Badge (5) and HM03 Surf | 4 | Stunfisk | 30 | 10–20 |
| Route 120 | Surf | Balance Badge (5) and HM03 Surf | 4 | Masquerain | 10 | 5–35 |
| Route 120 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Pinsir | 60 | 10–15 |
| Route 120 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Masquerain | 40 | 5–20 |
| Route 120 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Barboach | 60 | 5–10 |
| Route 120 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 40 | 5–10 |
| Route 120 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Barboach | 60 | 10–30 |
| Route 120 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Magikarp | 20 | 10–30 |
| Route 120 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Stunfisk | 20 | 10–30 |
| Route 120 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 90 | 2–3 |
| Route 120 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Tangrowth | 5 | 3–3 |
| Route 120 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Yanmega | 5 | 3–3 |
| Route 122 | Surf | Balance Badge (5) and HM03 Surf | 4 | Pelipper | 60 | 5–35 |
| Route 122 | Surf | Balance Badge (5) and HM03 Surf | 4 | Frillish | 30 | 10–30 |
| Route 122 | Surf | Balance Badge (5) and HM03 Surf | 4 | Alomomola | 10 | 15–30 |
| Route 122 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 60 | 5–10 |
| Route 122 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Wailmer | 40 | 5–10 |
| Route 122 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Wailmer | 60 | 10–30 |
| Route 122 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Finneon | 40 | 10–30 |
| Route 123 | Land | No field move; available when the location itself is reachable | 20 | Karrablast | 20 | 26–28 |
| Route 123 | Land | No field move; available when the location itself is reachable | 20 | Shelmet | 20 | 26–28 |
| Route 123 | Land | No field move; available when the location itself is reachable | 20 | Gloom | 17 | 25–28 |
| Route 123 | Land | No field move; available when the location itself is reachable | 20 | Stantler | 17 | 26–28 |
| Route 123 | Land | No field move; available when the location itself is reachable | 20 | Linoone | 13 | 26–26 |
| Route 123 | Land | No field move; available when the location itself is reachable | 20 | Mightyena | 13 | 26–26 |
| Route 123 | Surf | Balance Badge (5) and HM03 Surf | 4 | Azumarill | 60 | 5–35 |
| Route 123 | Surf | Balance Badge (5) and HM03 Surf | 4 | Surskit | 30 | 10–30 |
| Route 123 | Surf | Balance Badge (5) and HM03 Surf | 4 | Masquerain | 10 | 15–30 |
| Route 123 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Noctowl | 60 | 10–15 |
| Route 123 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Exeggcute | 40 | 5–20 |
| Route 123 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Goldeen | 60 | 5–10 |
| Route 123 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Corphish | 40 | 5–10 |
| Route 123 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Corphish | 60 | 10–30 |
| Route 123 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Goldeen | 40 | 10–30 |
| Route 123 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 90 | 2–3 |
| Route 123 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Accelgor | 5 | 3–3 |
| Route 123 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Escavalier | 5 | 3–3 |
| Mt Pyre Exterior | Land | No field move; available when the location itself is reachable | 10 | Vulpix | 23 | 27–28 |
| Mt Pyre Exterior | Land | No field move; available when the location itself is reachable | 10 | Drifblim | 20 | 29–29 |
| Mt Pyre Exterior | Land | No field move; available when the location itself is reachable | 10 | Bronzong | 17 | 25–28 |
| Mt Pyre Exterior | Land | No field move; available when the location itself is reachable | 10 | Growlithe | 13 | 27–29 |
| Mt Pyre Exterior | Land | No field move; available when the location itself is reachable | 10 | Medicham | 13 | 27–27 |
| Mt Pyre Exterior | Land | No field move; available when the location itself is reachable | 10 | Beheeyem | 10 | 27–27 |
| Mt Pyre Exterior | Land | No field move; available when the location itself is reachable | 10 | Corsola Galarian | 4 | 26–26 |
| Mt Pyre Exterior | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 70 | 2–3 |
| Mt Pyre Exterior | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Arcanine | 15 | 2–2 |
| Mt Pyre Exterior | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Ninetales | 15 | 2–2 |
| Abandoned Ship Hidden Floor Corridors | Surf | Balance Badge (5) and HM03 Surf | 4 | Frillish | 95 | 5–35 |
| Abandoned Ship Hidden Floor Corridors | Surf | Balance Badge (5) and HM03 Surf | 4 | Jellicent | 5 | 5–35 |
| Abandoned Ship Hidden Floor Corridors | Old Rod | Old Rod from Mom in Littleroot during the opening | 20 | Frillish | 60 | 5–10 |
| Abandoned Ship Hidden Floor Corridors | Old Rod | Old Rod from Mom in Littleroot during the opening | 20 | Skrelp | 40 | 5–10 |
| Abandoned Ship Hidden Floor Corridors | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 20 | Skrelp | 60 | 10–30 |
| Abandoned Ship Hidden Floor Corridors | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 20 | Dhelmise | 20 | 10–30 |
| Abandoned Ship Hidden Floor Corridors | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 20 | Frillish | 20 | 10–30 |
| Safari Zone Southwest | Land | No field move; available when the location itself is reachable | 25 | Palpitoad | 20 | 25–27 |
| Safari Zone Southwest | Land | No field move; available when the location itself is reachable | 25 | Arbok | 17 | 27–29 |
| Safari Zone Southwest | Land | No field move; available when the location itself is reachable | 25 | Sliggoo | 17 | 25–27 |
| Safari Zone Southwest | Land | No field move; available when the location itself is reachable | 25 | Karrablast | 13 | 25–25 |
| Safari Zone Southwest | Land | No field move; available when the location itself is reachable | 25 | Shelmet | 13 | 27–27 |
| Safari Zone Southwest | Land | No field move; available when the location itself is reachable | 25 | Seismitoad | 10 | 25–25 |
| Safari Zone Southwest | Land | No field move; available when the location itself is reachable | 25 | Toxicroak | 10 | 27–27 |
| Safari Zone Southwest | Surf | Balance Badge (5) and HM03 Surf | 9 | Stunfisk | 60 | 20–30 |
| Safari Zone Southwest | Surf | Balance Badge (5) and HM03 Surf | 9 | Sliggoo | 30 | 20–30 |
| Safari Zone Southwest | Surf | Balance Badge (5) and HM03 Surf | 9 | Seismitoad | 10 | 30–35 |
| Safari Zone Southwest | Old Rod | Old Rod from Mom in Littleroot during the opening | 35 | Magikarp | 60 | 5–10 |
| Safari Zone Southwest | Old Rod | Old Rod from Mom in Littleroot during the opening | 35 | Goldeen | 40 | 5–10 |
| Safari Zone Southwest | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 35 | Goldeen | 60 | 10–30 |
| Safari Zone Southwest | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 35 | Seaking | 40 | 10–30 |
| Safari Zone North | Land | No field move; available when the location itself is reachable | 25 | Donphan | 20 | 27–29 |
| Safari Zone North | Land | No field move; available when the location itself is reachable | 25 | Dugtrio Alolan | 17 | 27–29 |
| Safari Zone North | Land | No field move; available when the location itself is reachable | 25 | Gloom | 13 | 27–27 |
| Safari Zone North | Land | No field move; available when the location itself is reachable | 25 | Kangaskhan | 13 | 29–31 |
| Safari Zone North | Land | No field move; available when the location itself is reachable | 25 | Weepinbell | 13 | 27–27 |
| Safari Zone North | Land | No field move; available when the location itself is reachable | 25 | Heracross | 10 | 29–29 |
| Safari Zone North | Land | No field move; available when the location itself is reachable | 25 | Marowak Alolan | 10 | 29–29 |
| Safari Zone North | Land | No field move; available when the location itself is reachable | 25 | Cufant | 4 | 31–31 |
| Safari Zone North | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 25 | Graveler Alolan | 100 | 5–25 |
| Safari Zone Northwest | Land | No field move; available when the location itself is reachable | 25 | Rhyhorn | 20 | 29–29 |
| Safari Zone Northwest | Land | No field move; available when the location itself is reachable | 25 | Chansey | 17 | 27–29 |
| Safari Zone Northwest | Land | No field move; available when the location itself is reachable | 25 | Raticate Alolan | 17 | 29–31 |
| Safari Zone Northwest | Land | No field move; available when the location itself is reachable | 25 | Dodrio | 13 | 27–27 |
| Safari Zone Northwest | Land | No field move; available when the location itself is reachable | 25 | Xatu | 13 | 27–27 |
| Safari Zone Northwest | Land | No field move; available when the location itself is reachable | 25 | Pinsir | 10 | 29–29 |
| Safari Zone Northwest | Land | No field move; available when the location itself is reachable | 25 | Rhydon | 10 | 27–27 |
| Safari Zone Northwest | Surf | Balance Badge (5) and HM03 Surf | 9 | Grimer Alolan | 90 | 20–30 |
| Safari Zone Northwest | Surf | Balance Badge (5) and HM03 Surf | 9 | Muk Alolan | 10 | 30–35 |
| Safari Zone Northwest | Old Rod | Old Rod from Mom in Littleroot during the opening | 35 | Grimer Alolan | 60 | 5–10 |
| Safari Zone Northwest | Old Rod | Old Rod from Mom in Littleroot during the opening | 35 | Grimer | 40 | 5–10 |
| Safari Zone Northwest | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 35 | Grimer Alolan | 60 | 10–30 |
| Safari Zone Northwest | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 35 | Muk Alolan | 40 | 10–30 |
| Meteor Falls 1F 1R | Surf | Balance Badge (5) and HM03 Surf | 4 | Golbat | 60 | 5–35 |
| Meteor Falls 1F 1R | Surf | Balance Badge (5) and HM03 Surf | 4 | Dratini | 30 | 30–35 |
| Meteor Falls 1F 1R | Surf | Balance Badge (5) and HM03 Surf | 4 | Lunatone | 5 | 15–25 |
| Meteor Falls 1F 1R | Surf | Balance Badge (5) and HM03 Surf | 4 | Solrock | 5 | 25–35 |
| Meteor Falls 1F 1R | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Dratini | 60 | 10–30 |
| Meteor Falls 1F 1R | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Barboach | 20 | 10–30 |
| Meteor Falls 1F 1R | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Dragonair | 20 | 10–30 |
| Meteor Falls 1F 2R | Surf | Balance Badge (5) and HM03 Surf | 4 | Golbat | 60 | 5–35 |
| Meteor Falls 1F 2R | Surf | Balance Badge (5) and HM03 Surf | 4 | Dratini | 30 | 30–35 |
| Meteor Falls 1F 2R | Surf | Balance Badge (5) and HM03 Surf | 4 | Lunatone | 5 | 15–25 |
| Meteor Falls 1F 2R | Surf | Balance Badge (5) and HM03 Surf | 4 | Solrock | 5 | 25–35 |
| Meteor Falls 1F 2R | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Dratini | 60 | 10–30 |
| Meteor Falls 1F 2R | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Barboach | 20 | 10–30 |
| Meteor Falls 1F 2R | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Dragonair | 20 | 10–30 |
| Meteor Falls B1F 1R | Surf | Balance Badge (5) and HM03 Surf | 4 | Golbat | 60 | 5–35 |
| Meteor Falls B1F 1R | Surf | Balance Badge (5) and HM03 Surf | 4 | Dratini | 30 | 30–35 |
| Meteor Falls B1F 1R | Surf | Balance Badge (5) and HM03 Surf | 4 | Lunatone | 5 | 15–25 |
| Meteor Falls B1F 1R | Surf | Balance Badge (5) and HM03 Surf | 4 | Solrock | 5 | 25–35 |
| Meteor Falls B1F 1R | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Dratini | 60 | 10–30 |
| Meteor Falls B1F 1R | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Barboach | 20 | 10–30 |
| Meteor Falls B1F 1R | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Dragonair | 20 | 10–30 |
| Dewford Town | Surf | Balance Badge (5) and HM03 Surf | 4 | Tentacool | 60 | 5–35 |
| Dewford Town | Surf | Balance Badge (5) and HM03 Surf | 4 | Wingull | 30 | 10–30 |
| Dewford Town | Surf | Balance Badge (5) and HM03 Surf | 4 | Frillish | 5 | 25–30 |
| Dewford Town | Surf | Balance Badge (5) and HM03 Surf | 4 | Pelipper | 5 | 25–30 |
| Dewford Town | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Shellder | 60 | 10–15 |
| Dewford Town | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Finneon | 20 | 10–30 |
| Dewford Town | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Staryu | 20 | 10–30 |
| Slateport City | Surf | Balance Badge (5) and HM03 Surf | 4 | Tentacool | 60 | 5–35 |
| Slateport City | Surf | Balance Badge (5) and HM03 Surf | 4 | Wingull | 30 | 10–30 |
| Slateport City | Surf | Balance Badge (5) and HM03 Surf | 4 | Pelipper | 10 | 25–30 |
| Slateport City | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Horsea | 60 | 10–30 |
| Slateport City | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Alomomola | 20 | 10–30 |
| Slateport City | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Frillish | 20 | 10–30 |
| Petalburg City | Surf | Balance Badge (5) and HM03 Surf | 1 | Marill | 60 | 20–30 |
| Petalburg City | Surf | Balance Badge (5) and HM03 Surf | 1 | Azumarill | 30 | 10–20 |
| Petalburg City | Surf | Balance Badge (5) and HM03 Surf | 1 | Masquerain | 10 | 5–35 |
| Petalburg City | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Corphish | 60 | 10–30 |
| Petalburg City | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Goldeen | 40 | 10–30 |
| Safari Zone Southeast | Land | No field move; available when the location itself is reachable | 25 | Ambipom | 20 | 33–36 |
| Safari Zone Southeast | Land | No field move; available when the location itself is reachable | 25 | Ariados | 20 | 34–35 |
| Safari Zone Southeast | Land | No field move; available when the location itself is reachable | 25 | Granbull | 17 | 35–39 |
| Safari Zone Southeast | Land | No field move; available when the location itself is reachable | 25 | Stantler | 17 | 34–40 |
| Safari Zone Southeast | Land | No field move; available when the location itself is reachable | 25 | Flaaffy | 13 | 34–34 |
| Safari Zone Southeast | Land | No field move; available when the location itself is reachable | 25 | Gligar | 13 | 33–33 |
| Safari Zone Southeast | Surf | Balance Badge (5) and HM03 Surf | 9 | Wooper | 60 | 25–30 |
| Safari Zone Southeast | Surf | Balance Badge (5) and HM03 Surf | 9 | Quagsire | 40 | 25–35 |
| Safari Zone Southeast | Old Rod | Old Rod from Mom in Littleroot during the opening | 35 | Remoraid | 60 | 25–30 |
| Safari Zone Southeast | Old Rod | Old Rod from Mom in Littleroot during the opening | 35 | Dratini | 40 | 25–30 |
| Safari Zone Southeast | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 35 | Dratini | 60 | 25–30 |
| Safari Zone Southeast | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 35 | Dragonair | 40 | 25–35 |
| Safari Zone Northeast | Land | No field move; available when the location itself is reachable | 25 | Nidorina | 17 | 34–40 |
| Safari Zone Northeast | Land | No field move; available when the location itself is reachable | 25 | Nidorino | 17 | 35–39 |
| Safari Zone Northeast | Land | No field move; available when the location itself is reachable | 25 | Houndoom | 13 | 33–33 |
| Safari Zone Northeast | Land | No field move; available when the location itself is reachable | 25 | Pineco | 13 | 34–34 |
| Safari Zone Northeast | Land | No field move; available when the location itself is reachable | 25 | Forretress | 10 | 35–35 |
| Safari Zone Northeast | Land | No field move; available when the location itself is reachable | 25 | Ledian | 10 | 33–33 |
| Safari Zone Northeast | Land | No field move; available when the location itself is reachable | 25 | Miltank | 10 | 36–36 |
| Safari Zone Northeast | Land | No field move; available when the location itself is reachable | 25 | Tauros | 10 | 34–34 |
| Safari Zone Northeast | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 25 | Shuckle | 100 | 20–35 |
| Route 119 under the bridge | Any Rod under bridge | Balance Badge (5), Surf access to the Route 119 bridge, and any Rod | special | Feebas | 100 | 20–25 |

### Feather Badge — Lilycove, ocean routes, and Mossdeep (cap 60)

| Location | Method | Access requirement | Raw rate | Species | Within-method % | Levels |
|---|---|---|---|---|---|---|
| Route 102 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Crawdaunt | 70 | 20–45 |
| Route 102 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Seaking | 30 | 30–35 |
| Route 103 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Gyarados | 40 | 30–35 |
| Route 103 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Sharpedo | 30 | 25–45 |
| Route 103 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Wailmer | 30 | 30–35 |
| Route 104 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Luvdisc | 70 | 25–35 |
| Route 104 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Gyarados | 30 | 20–45 |
| Route 105 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Clawitzer | 55 | 20–30 |
| Route 105 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Dragalge | 45 | 30–45 |
| Route 110 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Chinchou | 40 | 25–30 |
| Route 110 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Wailmer | 30 | 30–35 |
| Route 110 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Lanturn | 15 | 20–25 |
| Route 110 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Wailord | 15 | 35–45 |
| Route 111 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Basculin | 70 | 25–35 |
| Route 111 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Seaking | 30 | 20–45 |
| Route 114 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Wishiwashi | 40 | 25–30 |
| Route 114 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Seaking | 30 | 30–35 |
| Route 114 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Whiscash | 30 | 20–45 |
| Route 117 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Seaking | 40 | 25–30 |
| Route 117 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Corphish | 30 | 30–35 |
| Route 117 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Crawdaunt | 30 | 20–45 |
| Route 118 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Basculin | 40 | 30–35 |
| Route 118 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Arrokuda | 30 | 30–35 |
| Route 118 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Gyarados | 15 | 35–45 |
| Route 118 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Sharpedo | 15 | 20–25 |
| Route 124 | Surf | Balance Badge (5) and HM03 Surf | 4 | Jellicent | 60 | 5–35 |
| Route 124 | Surf | Balance Badge (5) and HM03 Surf | 4 | Alomomola | 30 | 10–30 |
| Route 124 | Surf | Balance Badge (5) and HM03 Surf | 4 | Milotic | 10 | 15–30 |
| Route 124 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 60 | 5–10 |
| Route 124 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Finneon | 40 | 5–10 |
| Route 124 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Finneon | 60 | 10–30 |
| Route 124 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Alomomola | 20 | 10–30 |
| Route 124 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Frillish | 20 | 10–30 |
| Route 124 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Lumineon | 40 | 30–35 |
| Route 124 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Alomomola | 30 | 30–35 |
| Route 124 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Kingdra | 15 | 25–30 |
| Route 124 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Manaphy | 15 | 35–45 |
| Petalburg Woods 3 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Slowbro | 40 | 25–30 |
| Petalburg Woods 3 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Poliwhirl | 30 | 30–35 |
| Petalburg Woods 3 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Slowpoke | 15 | 20–25 |
| Petalburg Woods 3 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Poliwrath | 10 | 35–40 |
| Petalburg Woods 3 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Politoed | 5 | 40–45 |
| Abandoned Ship Rooms B1F | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 20 | Dragalge | 40 | 25–30 |
| Abandoned Ship Rooms B1F | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 20 | Dhelmise | 30 | 30–35 |
| Abandoned Ship Rooms B1F | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 20 | Jellicent | 30 | 20–35 |
| Meteor Falls B1F 2R | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Dratini | 40 | 25–30 |
| Meteor Falls B1F 2R | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Dragonair | 30 | 30–35 |
| Meteor Falls B1F 2R | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Whiscash | 25 | 20–40 |
| Meteor Falls B1F 2R | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Dragonite | 5 | 40–45 |
| Route 106 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Clawitzer | 55 | 20–30 |
| Route 106 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Dragalge | 45 | 30–45 |
| Route 107 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Lumineon | 40 | 25–30 |
| Route 107 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Gyarados | 30 | 20–45 |
| Route 107 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Octillery | 30 | 30–35 |
| Route 108 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Lumineon | 40 | 25–30 |
| Route 108 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Gyarados | 30 | 20–45 |
| Route 108 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Octillery | 30 | 30–35 |
| Route 109 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Bruxish | 55 | 25–45 |
| Route 109 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Corsola | 30 | 30–35 |
| Route 109 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Toxapex | 15 | 20–25 |
| Route 115 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Qwilfish | 40 | 25–30 |
| Route 115 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Wishiwashi | 30 | 30–35 |
| Route 115 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Wailmer | 15 | 20–25 |
| Route 115 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Wailord | 15 | 35–45 |
| Route 119 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Carvanha | 40 | 25–30 |
| Route 119 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Gyarados | 30 | 30–35 |
| Route 119 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Sharpedo | 25 | 20–40 |
| Route 119 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Milotic | 5 | 40–45 |
| Route 120 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Whiscash | 40 | 25–30 |
| Route 120 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Gyarados | 30 | 20–45 |
| Route 120 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Stunfisk | 30 | 30–35 |
| Route 121 | Land | No field move; available when the location itself is reachable | 20 | Duskull | 13 | 26–26 |
| Route 121 | Land | No field move; available when the location itself is reachable | 20 | Shuppet | 13 | 26–26 |
| Route 121 | Land | No field move; available when the location itself is reachable | 20 | Sinistea | 12 | 25–27 |
| Route 121 | Land | No field move; available when the location itself is reachable | 20 | Zoroark | 12 | 26–28 |
| Route 121 | Land | No field move; available when the location itself is reachable | 20 | Elgyem | 10 | 28–28 |
| Route 121 | Land | No field move; available when the location itself is reachable | 20 | Furfrou | 10 | 28–28 |
| Route 121 | Land | No field move; available when the location itself is reachable | 20 | Hypno | 10 | 26–26 |
| Route 121 | Land | No field move; available when the location itself is reachable | 20 | Pangoro | 10 | 26–26 |
| Route 121 | Land | No field move; available when the location itself is reachable | 20 | Arbok | 5 | 28–28 |
| Route 121 | Land | No field move; available when the location itself is reachable | 20 | Komala | 5 | 28–28 |
| Route 121 | Surf | Balance Badge (5) and HM03 Surf | 4 | Pelipper | 60 | 5–35 |
| Route 121 | Surf | Balance Badge (5) and HM03 Surf | 4 | Frillish | 30 | 10–30 |
| Route 121 | Surf | Balance Badge (5) and HM03 Surf | 4 | Alomomola | 10 | 15–30 |
| Route 121 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Arbok | 60 | 10–15 |
| Route 121 | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Heracross | 40 | 5–20 |
| Route 121 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 60 | 5–10 |
| Route 121 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Wailmer | 40 | 5–10 |
| Route 121 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Wailmer | 100 | 10–30 |
| Route 121 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Wailmer | 40 | 25–30 |
| Route 121 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Gyarados | 30 | 30–35 |
| Route 121 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Lumineon | 30 | 20–45 |
| Route 121 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 50 | 2–2 |
| Route 121 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Banette | 25 | 2–3 |
| Route 121 | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Dusclops | 25 | 2–3 |
| Route 122 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Wailmer | 40 | 25–30 |
| Route 122 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Gyarados | 30 | 30–35 |
| Route 122 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Lumineon | 30 | 20–45 |
| Route 123 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Corphish | 40 | 25–30 |
| Route 123 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Crawdaunt | 30 | 30–35 |
| Route 123 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Seaking | 30 | 20–45 |
| Mt Pyre 2F | Land | No field move; available when the location itself is reachable | 10 | Litwick | 20 | 26–29 |
| Mt Pyre 2F | Land | No field move; available when the location itself is reachable | 10 | Misdreavus | 17 | 23–29 |
| Mt Pyre 2F | Land | No field move; available when the location itself is reachable | 10 | Murkrow | 17 | 22–24 |
| Mt Pyre 2F | Land | No field move; available when the location itself is reachable | 10 | Duskull | 13 | 28–28 |
| Mt Pyre 2F | Land | No field move; available when the location itself is reachable | 10 | Shuppet | 13 | 27–27 |
| Mt Pyre 2F | Land | No field move; available when the location itself is reachable | 10 | Golbat | 10 | 25–25 |
| Mt Pyre 2F | Land | No field move; available when the location itself is reachable | 10 | Haunter | 10 | 24–24 |
| Mt Pyre 3F | Land | No field move; available when the location itself is reachable | 10 | Litwick | 20 | 26–29 |
| Mt Pyre 3F | Land | No field move; available when the location itself is reachable | 10 | Misdreavus | 17 | 23–29 |
| Mt Pyre 3F | Land | No field move; available when the location itself is reachable | 10 | Murkrow | 17 | 22–24 |
| Mt Pyre 3F | Land | No field move; available when the location itself is reachable | 10 | Duskull | 13 | 28–28 |
| Mt Pyre 3F | Land | No field move; available when the location itself is reachable | 10 | Shuppet | 13 | 27–27 |
| Mt Pyre 3F | Land | No field move; available when the location itself is reachable | 10 | Golbat | 10 | 25–25 |
| Mt Pyre 3F | Land | No field move; available when the location itself is reachable | 10 | Haunter | 10 | 24–24 |
| Mt Pyre 4F | Land | No field move; available when the location itself is reachable | 10 | Misdreavus | 17 | 23–29 |
| Mt Pyre 4F | Land | No field move; available when the location itself is reachable | 10 | Murkrow | 17 | 22–24 |
| Mt Pyre 4F | Land | No field move; available when the location itself is reachable | 10 | Banette | 13 | 27–27 |
| Mt Pyre 4F | Land | No field move; available when the location itself is reachable | 10 | Dusclops | 13 | 28–28 |
| Mt Pyre 4F | Land | No field move; available when the location itself is reachable | 10 | Golbat | 10 | 24–24 |
| Mt Pyre 4F | Land | No field move; available when the location itself is reachable | 10 | Haunter | 10 | 29–29 |
| Mt Pyre 4F | Land | No field move; available when the location itself is reachable | 10 | Lampent | 10 | 26–26 |
| Mt Pyre 4F | Land | No field move; available when the location itself is reachable | 10 | Mimikyu | 10 | 25–25 |
| Mt Pyre 5F | Land | No field move; available when the location itself is reachable | 10 | Misdreavus | 17 | 23–29 |
| Mt Pyre 5F | Land | No field move; available when the location itself is reachable | 10 | Murkrow | 17 | 22–24 |
| Mt Pyre 5F | Land | No field move; available when the location itself is reachable | 10 | Banette | 13 | 27–27 |
| Mt Pyre 5F | Land | No field move; available when the location itself is reachable | 10 | Dusclops | 13 | 28–28 |
| Mt Pyre 5F | Land | No field move; available when the location itself is reachable | 10 | Golbat | 10 | 24–24 |
| Mt Pyre 5F | Land | No field move; available when the location itself is reachable | 10 | Haunter | 10 | 29–29 |
| Mt Pyre 5F | Land | No field move; available when the location itself is reachable | 10 | Lampent | 10 | 26–26 |
| Mt Pyre 5F | Land | No field move; available when the location itself is reachable | 10 | Mimikyu | 10 | 25–25 |
| Mt Pyre 6F | Land | No field move; available when the location itself is reachable | 10 | Misdreavus | 17 | 23–29 |
| Mt Pyre 6F | Land | No field move; available when the location itself is reachable | 10 | Murkrow | 17 | 22–24 |
| Mt Pyre 6F | Land | No field move; available when the location itself is reachable | 10 | Banette | 13 | 27–27 |
| Mt Pyre 6F | Land | No field move; available when the location itself is reachable | 10 | Dusclops | 13 | 28–28 |
| Mt Pyre 6F | Land | No field move; available when the location itself is reachable | 10 | Golbat | 10 | 24–24 |
| Mt Pyre 6F | Land | No field move; available when the location itself is reachable | 10 | Haunter | 10 | 29–29 |
| Mt Pyre 6F | Land | No field move; available when the location itself is reachable | 10 | Lampent | 10 | 26–26 |
| Mt Pyre 6F | Land | No field move; available when the location itself is reachable | 10 | Mimikyu | 10 | 25–25 |
| Mt Pyre Summit | Land | No field move; available when the location itself is reachable | 10 | Drifblim | 20 | 26–27 |
| Mt Pyre Summit | Land | No field move; available when the location itself is reachable | 10 | Medicham | 13 | 29–29 |
| Mt Pyre Summit | Land | No field move; available when the location itself is reachable | 10 | Vulpix | 13 | 28–28 |
| Mt Pyre Summit | Land | No field move; available when the location itself is reachable | 10 | Chimecho | 12 | 28–30 |
| Mt Pyre Summit | Land | No field move; available when the location itself is reachable | 10 | Growlithe | 12 | 26–28 |
| Mt Pyre Summit | Land | No field move; available when the location itself is reachable | 10 | Beheeyem | 10 | 24–28 |
| Mt Pyre Summit | Land | No field move; available when the location itself is reachable | 10 | Bronzong | 10 | 25–25 |
| Mt Pyre Summit | Land | No field move; available when the location itself is reachable | 10 | Flutter Mane | 10 | 30–30 |
| Mt Pyre Summit | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 70 | 2–3 |
| Mt Pyre Summit | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Bronzong | 15 | 2–2 |
| Mt Pyre Summit | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Chimecho | 15 | 2–2 |
| Route 125 | Surf | Balance Badge (5) and HM03 Surf | 4 | Jellicent | 60 | 5–35 |
| Route 125 | Surf | Balance Badge (5) and HM03 Surf | 4 | Alomomola | 30 | 10–30 |
| Route 125 | Surf | Balance Badge (5) and HM03 Surf | 4 | Lapras | 10 | 15–30 |
| Route 125 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 60 | 5–10 |
| Route 125 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Finneon | 40 | 5–10 |
| Route 125 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Finneon | 60 | 10–30 |
| Route 125 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Alomomola | 20 | 10–30 |
| Route 125 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Frillish | 20 | 10–30 |
| Route 125 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Lumineon | 40 | 30–35 |
| Route 125 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Alomomola | 30 | 30–35 |
| Route 125 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Kingdra | 15 | 25–30 |
| Route 125 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Suicune | 15 | 35–45 |
| Route 126 | Surf | Balance Badge (5) and HM03 Surf | 4 | Jellicent | 60 | 5–35 |
| Route 126 | Surf | Balance Badge (5) and HM03 Surf | 4 | Alomomola | 30 | 10–30 |
| Route 126 | Surf | Balance Badge (5) and HM03 Surf | 4 | Primarina | 10 | 15–30 |
| Route 126 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 60 | 5–10 |
| Route 126 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Finneon | 40 | 5–10 |
| Route 126 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Finneon | 60 | 10–30 |
| Route 126 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Alomomola | 20 | 10–30 |
| Route 126 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Frillish | 20 | 10–30 |
| Route 126 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Lumineon | 40 | 30–35 |
| Route 126 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Alomomola | 30 | 30–35 |
| Route 126 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Kingdra | 15 | 25–30 |
| Route 126 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Tapu Fini | 15 | 35–45 |
| Route 127 | Surf | Balance Badge (5) and HM03 Surf | 4 | Jellicent | 60 | 5–35 |
| Route 127 | Surf | Balance Badge (5) and HM03 Surf | 4 | Alomomola | 30 | 10–30 |
| Route 127 | Surf | Balance Badge (5) and HM03 Surf | 4 | Golisopod | 10 | 15–30 |
| Route 127 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 60 | 5–10 |
| Route 127 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Finneon | 40 | 5–10 |
| Route 127 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Finneon | 60 | 10–30 |
| Route 127 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Alomomola | 20 | 10–30 |
| Route 127 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Frillish | 20 | 10–30 |
| Route 127 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Lumineon | 40 | 30–35 |
| Route 127 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Alomomola | 30 | 30–35 |
| Route 127 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Dhelmise | 15 | 25–30 |
| Route 127 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Keldeo | 15 | 35–45 |
| Route 128 | Surf | Balance Badge (5) and HM03 Surf | 4 | Jellicent | 60 | 5–35 |
| Route 128 | Surf | Balance Badge (5) and HM03 Surf | 4 | Alomomola | 30 | 10–30 |
| Route 128 | Surf | Balance Badge (5) and HM03 Surf | 4 | Kingdra | 10 | 15–30 |
| Route 128 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 60 | 5–10 |
| Route 128 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Luvdisc | 40 | 5–10 |
| Route 128 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Luvdisc | 60 | 10–30 |
| Route 128 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Corsola | 40 | 10–30 |
| Route 128 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Luvdisc | 40 | 30–35 |
| Route 128 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Corsola | 30 | 30–35 |
| Route 128 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Dragalge | 15 | 30–35 |
| Route 128 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Primarina | 15 | 35–45 |
| Abandoned Ship Hidden Floor Corridors | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 20 | Dragalge | 40 | 25–30 |
| Abandoned Ship Hidden Floor Corridors | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 20 | Dhelmise | 30 | 30–35 |
| Abandoned Ship Hidden Floor Corridors | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 20 | Jellicent | 30 | 20–35 |
| Safari Zone Southwest | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 35 | Seaking | 100 | 25–40 |
| Safari Zone Northwest | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 35 | Muk Alolan | 60 | 25–40 |
| Safari Zone Northwest | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 35 | Muk | 40 | 25–30 |
| Meteor Falls 1F 1R | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Dratini | 40 | 25–30 |
| Meteor Falls 1F 1R | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Dragonair | 30 | 30–35 |
| Meteor Falls 1F 1R | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Whiscash | 25 | 20–40 |
| Meteor Falls 1F 1R | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Dragonite | 5 | 40–45 |
| Meteor Falls 1F 2R | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Dratini | 40 | 25–30 |
| Meteor Falls 1F 2R | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Dragonair | 30 | 30–35 |
| Meteor Falls 1F 2R | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Whiscash | 25 | 20–40 |
| Meteor Falls 1F 2R | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Dragonite | 5 | 40–45 |
| Meteor Falls B1F 1R | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Dratini | 40 | 25–30 |
| Meteor Falls B1F 1R | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Dragonair | 30 | 30–35 |
| Meteor Falls B1F 1R | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Whiscash | 25 | 20–40 |
| Meteor Falls B1F 1R | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Dragonite | 5 | 40–45 |
| Shoal Cave Low Tide Stairs Room | Land | No field move; available when the location itself is reachable | 10 | Golbat | 20 | 28–28 |
| Shoal Cave Low Tide Stairs Room | Land | No field move; available when the location itself is reachable | 10 | Sealeo | 20 | 30–30 |
| Shoal Cave Low Tide Stairs Room | Land | No field move; available when the location itself is reachable | 10 | Delibird | 17 | 32–32 |
| Shoal Cave Low Tide Stairs Room | Land | No field move; available when the location itself is reachable | 10 | Dewgong | 17 | 32–32 |
| Shoal Cave Low Tide Stairs Room | Land | No field move; available when the location itself is reachable | 10 | Cubchoo | 13 | 26–26 |
| Shoal Cave Low Tide Stairs Room | Land | No field move; available when the location itself is reachable | 10 | Spheal | 13 | 26–26 |
| Shoal Cave Low Tide Lower Room | Land | No field move; available when the location itself is reachable | 10 | Golbat | 20 | 28–28 |
| Shoal Cave Low Tide Lower Room | Land | No field move; available when the location itself is reachable | 10 | Sealeo | 20 | 30–30 |
| Shoal Cave Low Tide Lower Room | Land | No field move; available when the location itself is reachable | 10 | Delibird | 17 | 32–32 |
| Shoal Cave Low Tide Lower Room | Land | No field move; available when the location itself is reachable | 10 | Dewgong | 17 | 32–32 |
| Shoal Cave Low Tide Lower Room | Land | No field move; available when the location itself is reachable | 10 | Cubchoo | 13 | 26–26 |
| Shoal Cave Low Tide Lower Room | Land | No field move; available when the location itself is reachable | 10 | Spheal | 13 | 26–26 |
| Shoal Cave Low Tide Inner Room | Land | No field move; available when the location itself is reachable | 10 | Golbat | 20 | 28–28 |
| Shoal Cave Low Tide Inner Room | Land | No field move; available when the location itself is reachable | 10 | Sealeo | 20 | 30–30 |
| Shoal Cave Low Tide Inner Room | Land | No field move; available when the location itself is reachable | 10 | Delibird | 17 | 32–32 |
| Shoal Cave Low Tide Inner Room | Land | No field move; available when the location itself is reachable | 10 | Dewgong | 17 | 32–32 |
| Shoal Cave Low Tide Inner Room | Land | No field move; available when the location itself is reachable | 10 | Cubchoo | 13 | 26–26 |
| Shoal Cave Low Tide Inner Room | Land | No field move; available when the location itself is reachable | 10 | Spheal | 13 | 26–26 |
| Shoal Cave Low Tide Inner Room | Surf | Balance Badge (5) and HM03 Surf | 4 | Spheal | 60 | 5–35 |
| Shoal Cave Low Tide Inner Room | Surf | Balance Badge (5) and HM03 Surf | 4 | Golbat | 30 | 5–35 |
| Shoal Cave Low Tide Inner Room | Surf | Balance Badge (5) and HM03 Surf | 4 | Lapras | 10 | 25–30 |
| Shoal Cave Low Tide Inner Room | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Magikarp | 60 | 5–10 |
| Shoal Cave Low Tide Inner Room | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Tentacool | 40 | 5–10 |
| Shoal Cave Low Tide Inner Room | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Tentacool | 60 | 10–30 |
| Shoal Cave Low Tide Inner Room | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Slowpoke | 40 | 10–30 |
| Shoal Cave Low Tide Inner Room | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Tentacruel | 40 | 25–30 |
| Shoal Cave Low Tide Inner Room | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Slowbro | 30 | 20–45 |
| Shoal Cave Low Tide Inner Room | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Slowpoke | 30 | 30–35 |
| Shoal Cave Low Tide Entrance Room | Land | No field move; available when the location itself is reachable | 10 | Golbat | 20 | 28–28 |
| Shoal Cave Low Tide Entrance Room | Land | No field move; available when the location itself is reachable | 10 | Sealeo | 20 | 30–30 |
| Shoal Cave Low Tide Entrance Room | Land | No field move; available when the location itself is reachable | 10 | Delibird | 17 | 32–32 |
| Shoal Cave Low Tide Entrance Room | Land | No field move; available when the location itself is reachable | 10 | Dewgong | 17 | 32–32 |
| Shoal Cave Low Tide Entrance Room | Land | No field move; available when the location itself is reachable | 10 | Cubchoo | 13 | 26–26 |
| Shoal Cave Low Tide Entrance Room | Land | No field move; available when the location itself is reachable | 10 | Spheal | 13 | 26–26 |
| Shoal Cave Low Tide Entrance Room | Surf | Balance Badge (5) and HM03 Surf | 4 | Spheal | 60 | 5–35 |
| Shoal Cave Low Tide Entrance Room | Surf | Balance Badge (5) and HM03 Surf | 4 | Golbat | 30 | 5–35 |
| Shoal Cave Low Tide Entrance Room | Surf | Balance Badge (5) and HM03 Surf | 4 | Lapras | 10 | 25–30 |
| Shoal Cave Low Tide Entrance Room | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Magikarp | 60 | 5–10 |
| Shoal Cave Low Tide Entrance Room | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Tentacool | 40 | 5–10 |
| Shoal Cave Low Tide Entrance Room | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Tentacool | 60 | 10–30 |
| Shoal Cave Low Tide Entrance Room | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Slowpoke | 40 | 10–30 |
| Shoal Cave Low Tide Entrance Room | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Tentacruel | 40 | 25–30 |
| Shoal Cave Low Tide Entrance Room | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Slowbro | 30 | 20–45 |
| Shoal Cave Low Tide Entrance Room | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Slowpoke | 30 | 30–35 |
| Lilycove City | Surf | Balance Badge (5) and HM03 Surf | 4 | Floatzel | 60 | 5–35 |
| Lilycove City | Surf | Balance Badge (5) and HM03 Surf | 4 | Pelipper | 30 | 10–30 |
| Lilycove City | Surf | Balance Badge (5) and HM03 Surf | 4 | Bruxish | 10 | 15–30 |
| Lilycove City | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 25 | Binacle | 60 | 25–30 |
| Lilycove City | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 25 | Wimpod | 30 | 20–25 |
| Lilycove City | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 25 | Dwebble | 10 | 30–35 |
| Lilycove City | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Magikarp | 60 | 5–10 |
| Lilycove City | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Finneon | 40 | 5–10 |
| Lilycove City | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Finneon | 60 | 10–30 |
| Lilycove City | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Bruxish | 20 | 10–30 |
| Lilycove City | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Staryu | 20 | 10–30 |
| Lilycove City | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Lumineon | 40 | 25–30 |
| Lilycove City | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Bruxish | 30 | 30–35 |
| Lilycove City | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Starmie | 30 | 25–45 |
| Dewford Town | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Staryu | 40 | 25–30 |
| Dewford Town | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Shellder | 30 | 30–35 |
| Dewford Town | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Lumineon | 15 | 20–25 |
| Dewford Town | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Starmie | 10 | 35–40 |
| Dewford Town | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Cloyster | 5 | 40–45 |
| Slateport City | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Seadra | 40 | 25–30 |
| Slateport City | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Alomomola | 30 | 30–35 |
| Slateport City | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Jellicent | 25 | 20–40 |
| Slateport City | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Kingdra | 5 | 40–45 |
| Mossdeep City | Surf | Balance Badge (5) and HM03 Surf | 4 | Pelipper | 60 | 5–35 |
| Mossdeep City | Surf | Balance Badge (5) and HM03 Surf | 4 | Floatzel | 30 | 10–30 |
| Mossdeep City | Surf | Balance Badge (5) and HM03 Surf | 4 | Sharpedo | 10 | 15–30 |
| Mossdeep City | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Magikarp | 60 | 5–10 |
| Mossdeep City | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Finneon | 40 | 5–10 |
| Mossdeep City | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Finneon | 60 | 10–30 |
| Mossdeep City | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Sharpedo | 20 | 10–30 |
| Mossdeep City | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Wailmer | 20 | 10–30 |
| Mossdeep City | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Sharpedo | 40 | 30–35 |
| Mossdeep City | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Lumineon | 30 | 25–45 |
| Mossdeep City | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Wailmer | 30 | 30–35 |
| Petalburg City | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Corphish | 60 | 20–45 |
| Petalburg City | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Crawdaunt | 40 | 25–30 |
| Shoal Cave Low Tide Ice Room | Land | No field move; available when the location itself is reachable | 10 | Beartic | 20 | 28–28 |
| Shoal Cave Low Tide Ice Room | Land | No field move; available when the location itself is reachable | 10 | Jynx | 13 | 32–32 |
| Shoal Cave Low Tide Ice Room | Land | No field move; available when the location itself is reachable | 10 | Sealeo | 13 | 26–26 |
| Shoal Cave Low Tide Ice Room | Land | No field move; available when the location itself is reachable | 10 | Snorunt | 13 | 26–26 |
| Shoal Cave Low Tide Ice Room | Land | No field move; available when the location itself is reachable | 10 | Chien Pao | 10 | 30–30 |
| Shoal Cave Low Tide Ice Room | Land | No field move; available when the location itself is reachable | 10 | Iron Bundle | 10 | 30–30 |
| Shoal Cave Low Tide Ice Room | Land | No field move; available when the location itself is reachable | 10 | Eiscue | 8 | 32–32 |
| Shoal Cave Low Tide Ice Room | Land | No field move; available when the location itself is reachable | 10 | Cryogonal | 5 | 32–32 |
| Shoal Cave Low Tide Ice Room | Land | No field move; available when the location itself is reachable | 10 | Mr Mime Galarian | 4 | 32–32 |
| Shoal Cave Low Tide Ice Room | Land | No field move; available when the location itself is reachable | 10 | Snom | 4 | 32–32 |
| Safari Zone Southeast | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 35 | Dratini | 65 | 25–35 |
| Safari Zone Southeast | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 35 | Dragonair | 30 | 25–30 |
| Safari Zone Southeast | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 35 | Dragonite | 5 | 35–40 |
| Magma Hideout 1F | Land | No field move; available when the location itself is reachable | 10 | Excadrill | 15 | 28–30 |
| Magma Hideout 1F | Land | No field move; available when the location itself is reachable | 10 | Boldore | 13 | 28–28 |
| Magma Hideout 1F | Land | No field move; available when the location itself is reachable | 10 | Graveler | 13 | 27–27 |
| Magma Hideout 1F | Land | No field move; available when the location itself is reachable | 10 | Magmortar | 12 | 31–33 |
| Magma Hideout 1F | Land | No field move; available when the location itself is reachable | 10 | Volcarona | 12 | 30–32 |
| Magma Hideout 1F | Land | No field move; available when the location itself is reachable | 10 | Gouging Fire | 10 | 29–29 |
| Magma Hideout 1F | Land | No field move; available when the location itself is reachable | 10 | Torkoal | 10 | 30–30 |
| Magma Hideout 1F | Land | No field move; available when the location itself is reachable | 10 | Weezing | 10 | 30–30 |
| Magma Hideout 1F | Land | No field move; available when the location itself is reachable | 10 | Turtonator | 5 | 30–30 |
| Magma Hideout 2F 1R | Land | No field move; available when the location itself is reachable | 10 | Excadrill | 15 | 28–30 |
| Magma Hideout 2F 1R | Land | No field move; available when the location itself is reachable | 10 | Turtonator | 15 | 29–30 |
| Magma Hideout 2F 1R | Land | No field move; available when the location itself is reachable | 10 | Boldore | 13 | 28–28 |
| Magma Hideout 2F 1R | Land | No field move; available when the location itself is reachable | 10 | Graveler | 13 | 27–27 |
| Magma Hideout 2F 1R | Land | No field move; available when the location itself is reachable | 10 | Magmortar | 12 | 31–33 |
| Magma Hideout 2F 1R | Land | No field move; available when the location itself is reachable | 10 | Volcarona | 12 | 30–32 |
| Magma Hideout 2F 1R | Land | No field move; available when the location itself is reachable | 10 | Torkoal | 10 | 30–30 |
| Magma Hideout 2F 1R | Land | No field move; available when the location itself is reachable | 10 | Weezing | 10 | 30–30 |
| Magma Hideout 2F 2R | Land | No field move; available when the location itself is reachable | 10 | Excadrill | 15 | 28–30 |
| Magma Hideout 2F 2R | Land | No field move; available when the location itself is reachable | 10 | Turtonator | 15 | 29–30 |
| Magma Hideout 2F 2R | Land | No field move; available when the location itself is reachable | 10 | Boldore | 13 | 28–28 |
| Magma Hideout 2F 2R | Land | No field move; available when the location itself is reachable | 10 | Graveler | 13 | 27–27 |
| Magma Hideout 2F 2R | Land | No field move; available when the location itself is reachable | 10 | Magmortar | 12 | 31–33 |
| Magma Hideout 2F 2R | Land | No field move; available when the location itself is reachable | 10 | Volcarona | 12 | 30–32 |
| Magma Hideout 2F 2R | Land | No field move; available when the location itself is reachable | 10 | Torkoal | 10 | 30–30 |
| Magma Hideout 2F 2R | Land | No field move; available when the location itself is reachable | 10 | Weezing | 10 | 30–30 |
| Magma Hideout 3F 1R | Land | No field move; available when the location itself is reachable | 10 | Excadrill | 15 | 28–30 |
| Magma Hideout 3F 1R | Land | No field move; available when the location itself is reachable | 10 | Turtonator | 15 | 29–30 |
| Magma Hideout 3F 1R | Land | No field move; available when the location itself is reachable | 10 | Boldore | 13 | 28–28 |
| Magma Hideout 3F 1R | Land | No field move; available when the location itself is reachable | 10 | Graveler | 13 | 27–27 |
| Magma Hideout 3F 1R | Land | No field move; available when the location itself is reachable | 10 | Magmortar | 12 | 31–33 |
| Magma Hideout 3F 1R | Land | No field move; available when the location itself is reachable | 10 | Volcarona | 12 | 30–32 |
| Magma Hideout 3F 1R | Land | No field move; available when the location itself is reachable | 10 | Torkoal | 10 | 30–30 |
| Magma Hideout 3F 1R | Land | No field move; available when the location itself is reachable | 10 | Weezing | 10 | 30–30 |
| Magma Hideout 3F 2R | Land | No field move; available when the location itself is reachable | 10 | Excadrill | 15 | 28–30 |
| Magma Hideout 3F 2R | Land | No field move; available when the location itself is reachable | 10 | Turtonator | 15 | 29–30 |
| Magma Hideout 3F 2R | Land | No field move; available when the location itself is reachable | 10 | Boldore | 13 | 28–28 |
| Magma Hideout 3F 2R | Land | No field move; available when the location itself is reachable | 10 | Graveler | 13 | 27–27 |
| Magma Hideout 3F 2R | Land | No field move; available when the location itself is reachable | 10 | Magmortar | 12 | 31–33 |
| Magma Hideout 3F 2R | Land | No field move; available when the location itself is reachable | 10 | Volcarona | 12 | 30–32 |
| Magma Hideout 3F 2R | Land | No field move; available when the location itself is reachable | 10 | Torkoal | 10 | 30–30 |
| Magma Hideout 3F 2R | Land | No field move; available when the location itself is reachable | 10 | Weezing | 10 | 30–30 |
| Magma Hideout 4F | Land | No field move; available when the location itself is reachable | 10 | Excadrill | 15 | 28–30 |
| Magma Hideout 4F | Land | No field move; available when the location itself is reachable | 10 | Turtonator | 15 | 29–30 |
| Magma Hideout 4F | Land | No field move; available when the location itself is reachable | 10 | Boldore | 13 | 28–28 |
| Magma Hideout 4F | Land | No field move; available when the location itself is reachable | 10 | Graveler | 13 | 27–27 |
| Magma Hideout 4F | Land | No field move; available when the location itself is reachable | 10 | Emboar | 12 | 31–33 |
| Magma Hideout 4F | Land | No field move; available when the location itself is reachable | 10 | Volcanion | 12 | 30–32 |
| Magma Hideout 4F | Land | No field move; available when the location itself is reachable | 10 | Torkoal | 10 | 30–30 |
| Magma Hideout 4F | Land | No field move; available when the location itself is reachable | 10 | Weezing | 10 | 30–30 |
| Magma Hideout 3F 3R | Land | No field move; available when the location itself is reachable | 10 | Excadrill | 15 | 28–30 |
| Magma Hideout 3F 3R | Land | No field move; available when the location itself is reachable | 10 | Turtonator | 15 | 29–30 |
| Magma Hideout 3F 3R | Land | No field move; available when the location itself is reachable | 10 | Boldore | 13 | 28–28 |
| Magma Hideout 3F 3R | Land | No field move; available when the location itself is reachable | 10 | Graveler | 13 | 27–27 |
| Magma Hideout 3F 3R | Land | No field move; available when the location itself is reachable | 10 | Magmortar | 12 | 31–33 |
| Magma Hideout 3F 3R | Land | No field move; available when the location itself is reachable | 10 | Volcarona | 12 | 30–32 |
| Magma Hideout 3F 3R | Land | No field move; available when the location itself is reachable | 10 | Torkoal | 10 | 30–30 |
| Magma Hideout 3F 3R | Land | No field move; available when the location itself is reachable | 10 | Weezing | 10 | 30–30 |
| Magma Hideout 2F 3R | Land | No field move; available when the location itself is reachable | 10 | Excadrill | 15 | 28–30 |
| Magma Hideout 2F 3R | Land | No field move; available when the location itself is reachable | 10 | Turtonator | 15 | 29–30 |
| Magma Hideout 2F 3R | Land | No field move; available when the location itself is reachable | 10 | Boldore | 13 | 28–28 |
| Magma Hideout 2F 3R | Land | No field move; available when the location itself is reachable | 10 | Graveler | 13 | 27–27 |
| Magma Hideout 2F 3R | Land | No field move; available when the location itself is reachable | 10 | Magmortar | 12 | 31–33 |
| Magma Hideout 2F 3R | Land | No field move; available when the location itself is reachable | 10 | Volcarona | 12 | 30–32 |
| Magma Hideout 2F 3R | Land | No field move; available when the location itself is reachable | 10 | Torkoal | 10 | 30–30 |
| Magma Hideout 2F 3R | Land | No field move; available when the location itself is reachable | 10 | Weezing | 10 | 30–30 |

### Mind Badge — deep ocean, Sootopolis, and Cave of Origin (cap 70)

| Location | Method | Access requirement | Raw rate | Species | Within-method % | Levels |
|---|---|---|---|---|---|---|
| Route 129 | Surf | Balance Badge (5) and HM03 Surf | 4 | Tentacruel | 60 | 5–35 |
| Route 129 | Surf | Balance Badge (5) and HM03 Surf | 4 | Alomomola | 30 | 10–30 |
| Route 129 | Surf | Balance Badge (5) and HM03 Surf | 4 | Wailord | 10 | 15–30 |
| Route 129 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 60 | 5–10 |
| Route 129 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Finneon | 40 | 5–10 |
| Route 129 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Finneon | 60 | 10–30 |
| Route 129 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Alomomola | 20 | 10–30 |
| Route 129 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Tentacool | 20 | 10–30 |
| Route 129 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Lumineon | 40 | 30–35 |
| Route 129 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Alomomola | 30 | 30–35 |
| Route 129 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Tentacruel | 30 | 25–45 |
| Route 130 | Land | No field move; available when the location itself is reachable | 20 | Wynaut | 100 | 5–50 |
| Route 130 | Surf | Balance Badge (5) and HM03 Surf | 4 | Tentacruel | 60 | 5–35 |
| Route 130 | Surf | Balance Badge (5) and HM03 Surf | 4 | Alomomola | 30 | 10–30 |
| Route 130 | Surf | Balance Badge (5) and HM03 Surf | 4 | Wailord | 10 | 15–30 |
| Route 130 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 60 | 5–10 |
| Route 130 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Finneon | 40 | 5–10 |
| Route 130 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Finneon | 60 | 10–30 |
| Route 130 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Alomomola | 20 | 10–30 |
| Route 130 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Tentacool | 20 | 10–30 |
| Route 130 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Lumineon | 40 | 30–35 |
| Route 130 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Alomomola | 30 | 30–35 |
| Route 130 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Tentacruel | 30 | 25–45 |
| Route 131 | Surf | Balance Badge (5) and HM03 Surf | 4 | Tentacruel | 60 | 5–35 |
| Route 131 | Surf | Balance Badge (5) and HM03 Surf | 4 | Alomomola | 30 | 10–30 |
| Route 131 | Surf | Balance Badge (5) and HM03 Surf | 4 | Wailord | 10 | 15–30 |
| Route 131 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 60 | 5–10 |
| Route 131 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Finneon | 40 | 5–10 |
| Route 131 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Finneon | 60 | 10–30 |
| Route 131 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Alomomola | 20 | 10–30 |
| Route 131 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Tentacool | 20 | 10–30 |
| Route 131 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Lumineon | 40 | 30–35 |
| Route 131 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Alomomola | 30 | 30–35 |
| Route 131 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Tentacruel | 30 | 25–45 |
| Route 132 | Surf | Balance Badge (5) and HM03 Surf | 4 | Jellicent | 60 | 5–35 |
| Route 132 | Surf | Balance Badge (5) and HM03 Surf | 4 | Pelipper | 30 | 10–30 |
| Route 132 | Surf | Balance Badge (5) and HM03 Surf | 4 | Wailord | 10 | 15–30 |
| Route 132 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 60 | 5–10 |
| Route 132 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Tentacool | 40 | 5–10 |
| Route 132 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Magikarp | 60 | 10–30 |
| Route 132 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Horsea | 40 | 10–30 |
| Route 132 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Horsea | 65 | 25–40 |
| Route 132 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Seadra | 30 | 30–35 |
| Route 132 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Kingdra | 5 | 40–45 |
| Route 133 | Surf | Balance Badge (5) and HM03 Surf | 4 | Jellicent | 60 | 5–35 |
| Route 133 | Surf | Balance Badge (5) and HM03 Surf | 4 | Pelipper | 30 | 10–30 |
| Route 133 | Surf | Balance Badge (5) and HM03 Surf | 4 | Wailord | 10 | 15–30 |
| Route 133 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 60 | 5–10 |
| Route 133 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Tentacool | 40 | 5–10 |
| Route 133 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Magikarp | 60 | 10–30 |
| Route 133 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Horsea | 40 | 10–30 |
| Route 133 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Horsea | 65 | 25–40 |
| Route 133 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Seadra | 30 | 30–35 |
| Route 133 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Kingdra | 5 | 40–45 |
| Route 134 | Surf | Balance Badge (5) and HM03 Surf | 4 | Jellicent | 60 | 5–35 |
| Route 134 | Surf | Balance Badge (5) and HM03 Surf | 4 | Pelipper | 30 | 10–30 |
| Route 134 | Surf | Balance Badge (5) and HM03 Surf | 4 | Wailord | 10 | 15–30 |
| Route 134 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 60 | 5–10 |
| Route 134 | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Tentacool | 40 | 5–10 |
| Route 134 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Magikarp | 60 | 10–30 |
| Route 134 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Horsea | 40 | 10–30 |
| Route 134 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Horsea | 65 | 25–40 |
| Route 134 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Seadra | 30 | 30–35 |
| Route 134 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Kingdra | 5 | 40–45 |
| Seafloor Cavern Room1 | Land | No field move; available when the location itself is reachable | 4 | Golbat | 35 | 29–35 |
| Seafloor Cavern Room1 | Land | No field move; available when the location itself is reachable | 4 | Dragalge | 13 | 30–30 |
| Seafloor Cavern Room1 | Land | No field move; available when the location itself is reachable | 4 | Golisopod | 13 | 31–31 |
| Seafloor Cavern Room1 | Land | No field move; available when the location itself is reachable | 4 | Crobat | 12 | 35–36 |
| Seafloor Cavern Room1 | Land | No field move; available when the location itself is reachable | 4 | Greninja | 12 | 33–34 |
| Seafloor Cavern Room1 | Land | No field move; available when the location itself is reachable | 4 | Dhelmise | 10 | 28–28 |
| Seafloor Cavern Room1 | Land | No field move; available when the location itself is reachable | 4 | Malamar | 5 | 34–34 |
| Seafloor Cavern Room2 | Land | No field move; available when the location itself is reachable | 4 | Golbat | 35 | 29–35 |
| Seafloor Cavern Room2 | Land | No field move; available when the location itself is reachable | 4 | Dragalge | 13 | 30–30 |
| Seafloor Cavern Room2 | Land | No field move; available when the location itself is reachable | 4 | Golisopod | 13 | 31–31 |
| Seafloor Cavern Room2 | Land | No field move; available when the location itself is reachable | 4 | Crobat | 12 | 35–36 |
| Seafloor Cavern Room2 | Land | No field move; available when the location itself is reachable | 4 | Greninja | 12 | 33–34 |
| Seafloor Cavern Room2 | Land | No field move; available when the location itself is reachable | 4 | Dhelmise | 10 | 28–28 |
| Seafloor Cavern Room2 | Land | No field move; available when the location itself is reachable | 4 | Malamar | 5 | 34–34 |
| Seafloor Cavern Room3 | Land | No field move; available when the location itself is reachable | 4 | Golbat | 35 | 29–35 |
| Seafloor Cavern Room3 | Land | No field move; available when the location itself is reachable | 4 | Dragalge | 13 | 30–30 |
| Seafloor Cavern Room3 | Land | No field move; available when the location itself is reachable | 4 | Golisopod | 13 | 31–31 |
| Seafloor Cavern Room3 | Land | No field move; available when the location itself is reachable | 4 | Crobat | 12 | 35–36 |
| Seafloor Cavern Room3 | Land | No field move; available when the location itself is reachable | 4 | Greninja | 12 | 33–34 |
| Seafloor Cavern Room3 | Land | No field move; available when the location itself is reachable | 4 | Dhelmise | 10 | 28–28 |
| Seafloor Cavern Room3 | Land | No field move; available when the location itself is reachable | 4 | Malamar | 5 | 34–34 |
| Seafloor Cavern Room4 | Land | No field move; available when the location itself is reachable | 4 | Golbat | 35 | 29–35 |
| Seafloor Cavern Room4 | Land | No field move; available when the location itself is reachable | 4 | Dragalge | 13 | 30–30 |
| Seafloor Cavern Room4 | Land | No field move; available when the location itself is reachable | 4 | Golisopod | 13 | 31–31 |
| Seafloor Cavern Room4 | Land | No field move; available when the location itself is reachable | 4 | Crobat | 12 | 35–36 |
| Seafloor Cavern Room4 | Land | No field move; available when the location itself is reachable | 4 | Greninja | 12 | 33–34 |
| Seafloor Cavern Room4 | Land | No field move; available when the location itself is reachable | 4 | Dhelmise | 10 | 28–28 |
| Seafloor Cavern Room4 | Land | No field move; available when the location itself is reachable | 4 | Malamar | 5 | 34–34 |
| Seafloor Cavern Room5 | Land | No field move; available when the location itself is reachable | 4 | Golbat | 35 | 29–35 |
| Seafloor Cavern Room5 | Land | No field move; available when the location itself is reachable | 4 | Dragalge | 13 | 30–30 |
| Seafloor Cavern Room5 | Land | No field move; available when the location itself is reachable | 4 | Golisopod | 13 | 31–31 |
| Seafloor Cavern Room5 | Land | No field move; available when the location itself is reachable | 4 | Crobat | 12 | 35–36 |
| Seafloor Cavern Room5 | Land | No field move; available when the location itself is reachable | 4 | Greninja | 12 | 33–34 |
| Seafloor Cavern Room5 | Land | No field move; available when the location itself is reachable | 4 | Dhelmise | 10 | 28–28 |
| Seafloor Cavern Room5 | Land | No field move; available when the location itself is reachable | 4 | Malamar | 5 | 34–34 |
| Seafloor Cavern Room6 | Land | No field move; available when the location itself is reachable | 4 | Golbat | 35 | 29–35 |
| Seafloor Cavern Room6 | Land | No field move; available when the location itself is reachable | 4 | Dragalge | 13 | 30–30 |
| Seafloor Cavern Room6 | Land | No field move; available when the location itself is reachable | 4 | Golisopod | 13 | 31–31 |
| Seafloor Cavern Room6 | Land | No field move; available when the location itself is reachable | 4 | Crobat | 12 | 35–36 |
| Seafloor Cavern Room6 | Land | No field move; available when the location itself is reachable | 4 | Greninja | 12 | 33–34 |
| Seafloor Cavern Room6 | Land | No field move; available when the location itself is reachable | 4 | Dhelmise | 10 | 28–28 |
| Seafloor Cavern Room6 | Land | No field move; available when the location itself is reachable | 4 | Malamar | 5 | 34–34 |
| Seafloor Cavern Room6 | Surf | Balance Badge (5) and HM03 Surf | 4 | Kingdra | 60 | 5–35 |
| Seafloor Cavern Room6 | Surf | Balance Badge (5) and HM03 Surf | 4 | Milotic | 30 | 5–35 |
| Seafloor Cavern Room6 | Surf | Balance Badge (5) and HM03 Surf | 4 | Lapras | 10 | 30–35 |
| Seafloor Cavern Room6 | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Magikarp | 60 | 5–10 |
| Seafloor Cavern Room6 | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Tentacool | 40 | 5–10 |
| Seafloor Cavern Room6 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Tentacool | 60 | 10–30 |
| Seafloor Cavern Room6 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Wailmer | 40 | 10–30 |
| Seafloor Cavern Room6 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Wailmer | 85 | 20–35 |
| Seafloor Cavern Room6 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Manaphy | 15 | 35–45 |
| Seafloor Cavern Room7 | Land | No field move; available when the location itself is reachable | 4 | Golbat | 35 | 29–35 |
| Seafloor Cavern Room7 | Land | No field move; available when the location itself is reachable | 4 | Dragalge | 13 | 30–30 |
| Seafloor Cavern Room7 | Land | No field move; available when the location itself is reachable | 4 | Golisopod | 13 | 31–31 |
| Seafloor Cavern Room7 | Land | No field move; available when the location itself is reachable | 4 | Crobat | 12 | 35–36 |
| Seafloor Cavern Room7 | Land | No field move; available when the location itself is reachable | 4 | Greninja | 12 | 33–34 |
| Seafloor Cavern Room7 | Land | No field move; available when the location itself is reachable | 4 | Dhelmise | 10 | 28–28 |
| Seafloor Cavern Room7 | Land | No field move; available when the location itself is reachable | 4 | Malamar | 5 | 34–34 |
| Seafloor Cavern Room7 | Surf | Balance Badge (5) and HM03 Surf | 4 | Kingdra | 60 | 5–35 |
| Seafloor Cavern Room7 | Surf | Balance Badge (5) and HM03 Surf | 4 | Milotic | 30 | 5–35 |
| Seafloor Cavern Room7 | Surf | Balance Badge (5) and HM03 Surf | 4 | Lapras | 10 | 30–35 |
| Seafloor Cavern Room7 | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Magikarp | 60 | 5–10 |
| Seafloor Cavern Room7 | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Tentacool | 40 | 5–10 |
| Seafloor Cavern Room7 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Tentacool | 60 | 10–30 |
| Seafloor Cavern Room7 | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Wailmer | 40 | 10–30 |
| Seafloor Cavern Room7 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Wailmer | 85 | 20–35 |
| Seafloor Cavern Room7 | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Manaphy | 15 | 35–45 |
| Seafloor Cavern Room8 | Land | No field move; available when the location itself is reachable | 4 | Golbat | 35 | 29–35 |
| Seafloor Cavern Room8 | Land | No field move; available when the location itself is reachable | 4 | Dragalge | 13 | 30–30 |
| Seafloor Cavern Room8 | Land | No field move; available when the location itself is reachable | 4 | Golisopod | 13 | 31–31 |
| Seafloor Cavern Room8 | Land | No field move; available when the location itself is reachable | 4 | Crobat | 12 | 35–36 |
| Seafloor Cavern Room8 | Land | No field move; available when the location itself is reachable | 4 | Greninja | 12 | 33–34 |
| Seafloor Cavern Room8 | Land | No field move; available when the location itself is reachable | 4 | Dhelmise | 10 | 28–28 |
| Seafloor Cavern Room8 | Land | No field move; available when the location itself is reachable | 4 | Malamar | 5 | 34–34 |
| Seafloor Cavern Entrance | Surf | Balance Badge (5) and HM03 Surf | 4 | Kingdra | 60 | 5–35 |
| Seafloor Cavern Entrance | Surf | Balance Badge (5) and HM03 Surf | 4 | Milotic | 30 | 5–35 |
| Seafloor Cavern Entrance | Surf | Balance Badge (5) and HM03 Surf | 4 | Lapras | 10 | 30–35 |
| Seafloor Cavern Entrance | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Magikarp | 60 | 5–10 |
| Seafloor Cavern Entrance | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Tentacool | 40 | 5–10 |
| Seafloor Cavern Entrance | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Tentacool | 60 | 10–30 |
| Seafloor Cavern Entrance | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Wailmer | 40 | 10–30 |
| Seafloor Cavern Entrance | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Wailmer | 85 | 20–35 |
| Seafloor Cavern Entrance | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Manaphy | 15 | 35–45 |
| Cave Of Origin Entrance | Land | No field move; available when the location itself is reachable | 4 | Noivern | 34 | 33–36 |
| Cave Of Origin Entrance | Land | No field move; available when the location itself is reachable | 4 | Boldore | 20 | 28–32 |
| Cave Of Origin Entrance | Land | No field move; available when the location itself is reachable | 4 | Carbink | 20 | 29–33 |
| Cave Of Origin Entrance | Land | No field move; available when the location itself is reachable | 4 | Mawile | 13 | 31–31 |
| Cave Of Origin Entrance | Land | No field move; available when the location itself is reachable | 4 | Sableye | 13 | 30–30 |
| Cave Of Origin 1F | Land | No field move; available when the location itself is reachable | 4 | Noivern | 34 | 33–36 |
| Cave Of Origin 1F | Land | No field move; available when the location itself is reachable | 4 | Carbink | 20 | 29–33 |
| Cave Of Origin 1F | Land | No field move; available when the location itself is reachable | 4 | Mawile | 13 | 31–31 |
| Cave Of Origin 1F | Land | No field move; available when the location itself is reachable | 4 | Sableye | 13 | 30–30 |
| Cave Of Origin 1F | Land | No field move; available when the location itself is reachable | 4 | Boldore | 10 | 32–32 |
| Cave Of Origin 1F | Land | No field move; available when the location itself is reachable | 4 | Walking Wake | 10 | 28–28 |
| Cave Of Origin Unused Ruby Sapphire Map1 | Land | No field move; available when the location itself is reachable | 4 | Carbink | 46 | 32–35 |
| Cave Of Origin Unused Ruby Sapphire Map1 | Land | No field move; available when the location itself is reachable | 4 | Boldore | 20 | 30–32 |
| Cave Of Origin Unused Ruby Sapphire Map1 | Land | No field move; available when the location itself is reachable | 4 | Golbat | 13 | 30–30 |
| Cave Of Origin Unused Ruby Sapphire Map1 | Land | No field move; available when the location itself is reachable | 4 | Sableye | 13 | 31–31 |
| Cave Of Origin Unused Ruby Sapphire Map1 | Land | No field move; available when the location itself is reachable | 4 | Gigalith | 8 | 33–36 |
| Cave Of Origin Unused Ruby Sapphire Map2 | Land | No field move; available when the location itself is reachable | 4 | Carbink | 46 | 32–35 |
| Cave Of Origin Unused Ruby Sapphire Map2 | Land | No field move; available when the location itself is reachable | 4 | Boldore | 20 | 30–32 |
| Cave Of Origin Unused Ruby Sapphire Map2 | Land | No field move; available when the location itself is reachable | 4 | Golbat | 13 | 30–30 |
| Cave Of Origin Unused Ruby Sapphire Map2 | Land | No field move; available when the location itself is reachable | 4 | Sableye | 13 | 31–31 |
| Cave Of Origin Unused Ruby Sapphire Map2 | Land | No field move; available when the location itself is reachable | 4 | Gigalith | 8 | 33–36 |
| Cave Of Origin Unused Ruby Sapphire Map3 | Land | No field move; available when the location itself is reachable | 4 | Carbink | 46 | 32–35 |
| Cave Of Origin Unused Ruby Sapphire Map3 | Land | No field move; available when the location itself is reachable | 4 | Boldore | 20 | 30–32 |
| Cave Of Origin Unused Ruby Sapphire Map3 | Land | No field move; available when the location itself is reachable | 4 | Golbat | 13 | 30–30 |
| Cave Of Origin Unused Ruby Sapphire Map3 | Land | No field move; available when the location itself is reachable | 4 | Sableye | 13 | 31–31 |
| Cave Of Origin Unused Ruby Sapphire Map3 | Land | No field move; available when the location itself is reachable | 4 | Gigalith | 8 | 33–36 |
| Cave Of Origin Diancies Room | Land | No field move; available when the location itself is reachable | 4 | Carbink | 100 | 30–36 |
| Sootopolis City | Surf | Balance Badge (5) and HM03 Surf | 1 | Magikarp | 100 | 5–35 |
| Sootopolis City | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Magikarp | 60 | 5–10 |
| Sootopolis City | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Tentacool | 40 | 5–10 |
| Sootopolis City | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Magikarp | 100 | 10–30 |
| Sootopolis City | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Magikarp | 70 | 30–35 |
| Sootopolis City | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Gyarados | 30 | 5–45 |

### Rain Badge — Sky Pillar, Victory Road, and the League approach (cap 80)

| Location | Method | Access requirement | Raw rate | Species | Within-method % | Levels |
|---|---|---|---|---|---|---|
| Victory Road 1F | Land | No field move; available when the location itself is reachable | 10 | Lairon | 13 | 40–40 |
| Victory Road 1F | Land | No field move; available when the location itself is reachable | 10 | Medicham | 13 | 40–40 |
| Victory Road 1F | Land | No field move; available when the location itself is reachable | 10 | Gabite | 10 | 38–38 |
| Victory Road 1F | Land | No field move; available when the location itself is reachable | 10 | Golbat | 10 | 40–40 |
| Victory Road 1F | Land | No field move; available when the location itself is reachable | 10 | Iron Valiant | 10 | 36–36 |
| Victory Road 1F | Land | No field move; available when the location itself is reachable | 10 | Noivern | 10 | 40–40 |
| Victory Road 1F | Land | No field move; available when the location itself is reachable | 10 | Pupitar | 10 | 36–36 |
| Victory Road 1F | Land | No field move; available when the location itself is reachable | 10 | Kommo-o | 8 | 36–36 |
| Victory Road 1F | Land | No field move; available when the location itself is reachable | 10 | Metagross | 8 | 36–36 |
| Victory Road 1F | Land | No field move; available when the location itself is reachable | 10 | Dragapult | 4 | 36–36 |
| Victory Road 1F | Land | No field move; available when the location itself is reachable | 10 | Volcarona | 4 | 36–36 |
| Victory Road B1F | Land | No field move; available when the location itself is reachable | 10 | Gurdurr | 15 | 40–42 |
| Victory Road B1F | Land | No field move; available when the location itself is reachable | 10 | Steelix | 15 | 40–42 |
| Victory Road B1F | Land | No field move; available when the location itself is reachable | 10 | Mawile | 13 | 40–40 |
| Victory Road B1F | Land | No field move; available when the location itself is reachable | 10 | Sableye | 13 | 40–40 |
| Victory Road B1F | Land | No field move; available when the location itself is reachable | 10 | Donphan | 10 | 38–38 |
| Victory Road B1F | Land | No field move; available when the location itself is reachable | 10 | Rhydon | 10 | 38–38 |
| Victory Road B1F | Land | No field move; available when the location itself is reachable | 10 | Aegislash | 8 | 42–42 |
| Victory Road B1F | Land | No field move; available when the location itself is reachable | 10 | Terrakion | 8 | 38–38 |
| Victory Road B1F | Land | No field move; available when the location itself is reachable | 10 | Kommo-o | 4 | 38–38 |
| Victory Road B1F | Land | No field move; available when the location itself is reachable | 10 | Metagross | 4 | 42–42 |
| Victory Road B1F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Graveler | 60 | 30–40 |
| Victory Road B1F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Shuckle | 30 | 30–40 |
| Victory Road B1F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Golem | 10 | 35–40 |
| Victory Road B2F | Land | No field move; available when the location itself is reachable | 10 | Carbink | 15 | 40–44 |
| Victory Road B2F | Land | No field move; available when the location itself is reachable | 10 | Zweilous | 15 | 40–44 |
| Victory Road B2F | Land | No field move; available when the location itself is reachable | 10 | Exploud | 13 | 40–40 |
| Victory Road B2F | Land | No field move; available when the location itself is reachable | 10 | Steelix | 13 | 40–40 |
| Victory Road B2F | Land | No field move; available when the location itself is reachable | 10 | Aggron | 10 | 42–42 |
| Victory Road B2F | Land | No field move; available when the location itself is reachable | 10 | Donphan | 10 | 42–42 |
| Victory Road B2F | Land | No field move; available when the location itself is reachable | 10 | Hydreigon | 8 | 42–42 |
| Victory Road B2F | Land | No field move; available when the location itself is reachable | 10 | Volcarona | 8 | 42–42 |
| Victory Road B2F | Land | No field move; available when the location itself is reachable | 10 | Aegislash | 4 | 44–44 |
| Victory Road B2F | Land | No field move; available when the location itself is reachable | 10 | Dragapult | 4 | 44–44 |
| Victory Road B2F | Surf | Balance Badge (5) and HM03 Surf | 4 | Dewgong | 60 | 30–35 |
| Victory Road B2F | Surf | Balance Badge (5) and HM03 Surf | 4 | Golbat | 30 | 25–30 |
| Victory Road B2F | Surf | Balance Badge (5) and HM03 Surf | 4 | Lapras | 10 | 35–40 |
| Victory Road B2F | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 60 | 5–10 |
| Victory Road B2F | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Barboach | 40 | 5–10 |
| Victory Road B2F | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Barboach | 60 | 10–30 |
| Victory Road B2F | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Basculin | 40 | 10–30 |
| Victory Road B2F | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Basculin | 70 | 25–35 |
| Victory Road B2F | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Whiscash | 30 | 30–45 |
| Ever Grande City | Surf | Balance Badge (5) and HM03 Surf | 4 | Pelipper | 60 | 5–35 |
| Ever Grande City | Surf | Balance Badge (5) and HM03 Surf | 4 | Tentacruel | 30 | 10–30 |
| Ever Grande City | Surf | Balance Badge (5) and HM03 Surf | 4 | Floatzel | 10 | 15–30 |
| Ever Grande City | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Luvdisc | 100 | 5–10 |
| Ever Grande City | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Luvdisc | 60 | 10–30 |
| Ever Grande City | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Corsola | 20 | 10–30 |
| Ever Grande City | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Mareanie | 20 | 10–30 |
| Ever Grande City | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Luvdisc | 40 | 30–35 |
| Ever Grande City | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Corsola | 30 | 30–35 |
| Ever Grande City | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Toxapex | 30 | 30–45 |
| Sky Pillar 1F | Land | No field move; available when the location itself is reachable | 10 | Claydol | 22 | 36–38 |
| Sky Pillar 1F | Land | No field move; available when the location itself is reachable | 10 | Golurk | 22 | 37–37 |
| Sky Pillar 1F | Land | No field move; available when the location itself is reachable | 10 | Banette | 15 | 35–38 |
| Sky Pillar 1F | Land | No field move; available when the location itself is reachable | 10 | Dusclops | 15 | 34–36 |
| Sky Pillar 1F | Land | No field move; available when the location itself is reachable | 10 | Mawile | 13 | 34–34 |
| Sky Pillar 1F | Land | No field move; available when the location itself is reachable | 10 | Sableye | 13 | 33–33 |
| Sky Pillar 3F | Land | No field move; available when the location itself is reachable | 10 | Claydol | 22 | 36–38 |
| Sky Pillar 3F | Land | No field move; available when the location itself is reachable | 10 | Golurk | 22 | 37–37 |
| Sky Pillar 3F | Land | No field move; available when the location itself is reachable | 10 | Banette | 15 | 35–38 |
| Sky Pillar 3F | Land | No field move; available when the location itself is reachable | 10 | Dusclops | 15 | 34–36 |
| Sky Pillar 3F | Land | No field move; available when the location itself is reachable | 10 | Mawile | 13 | 34–34 |
| Sky Pillar 3F | Land | No field move; available when the location itself is reachable | 10 | Sableye | 13 | 33–33 |
| Sky Pillar 5F | Land | No field move; available when the location itself is reachable | 10 | Claydol | 22 | 36–38 |
| Sky Pillar 5F | Land | No field move; available when the location itself is reachable | 10 | Golurk | 22 | 37–37 |
| Sky Pillar 5F | Land | No field move; available when the location itself is reachable | 10 | Mawile | 13 | 34–34 |
| Sky Pillar 5F | Land | No field move; available when the location itself is reachable | 10 | Sableye | 13 | 33–33 |
| Sky Pillar 5F | Land | No field move; available when the location itself is reachable | 10 | Altaria | 10 | 36–38 |
| Sky Pillar 5F | Land | No field move; available when the location itself is reachable | 10 | Banette | 10 | 35–35 |
| Sky Pillar 5F | Land | No field move; available when the location itself is reachable | 10 | Dusclops | 10 | 34–34 |

### Champion / postgame — open-world cleanup and Battle Frontier (cap 101)

| Location | Method | Access requirement | Raw rate | Species | Within-method % | Levels |
|---|---|---|---|---|---|---|
| Underwater Route 126 | Land | No field move; available when the location itself is reachable | 25 | Lumineon | 18 | 25–25 |
| Underwater Route 126 | Land | No field move; available when the location itself is reachable | 25 | Chinchou | 13 | 27–27 |
| Underwater Route 126 | Land | No field move; available when the location itself is reachable | 25 | Clamperl | 13 | 25–25 |
| Underwater Route 126 | Land | No field move; available when the location itself is reachable | 25 | Golisopod | 12 | 27–29 |
| Underwater Route 126 | Land | No field move; available when the location itself is reachable | 25 | Lanturn | 10 | 25–25 |
| Underwater Route 126 | Land | No field move; available when the location itself is reachable | 25 | Relicanth | 10 | 25–25 |
| Underwater Route 126 | Land | No field move; available when the location itself is reachable | 25 | Starmie | 10 | 27–27 |
| Underwater Route 126 | Land | No field move; available when the location itself is reachable | 25 | Gorebyss | 5 | 25–25 |
| Underwater Route 126 | Land | No field move; available when the location itself is reachable | 25 | Huntail | 5 | 27–27 |
| Underwater Route 126 | Land | No field move; available when the location itself is reachable | 25 | Kingdra | 4 | 27–27 |
| Pacifidlog Town | Surf | Balance Badge (5) and HM03 Surf | 4 | Tentacruel | 60 | 5–35 |
| Pacifidlog Town | Surf | Balance Badge (5) and HM03 Surf | 4 | Pelipper | 30 | 10–30 |
| Pacifidlog Town | Surf | Balance Badge (5) and HM03 Surf | 4 | Sharpedo | 10 | 15–30 |
| Pacifidlog Town | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Magikarp | 60 | 5–10 |
| Pacifidlog Town | Old Rod | Old Rod from Mom in Littleroot during the opening | 10 | Skrelp | 40 | 5–10 |
| Pacifidlog Town | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Skrelp | 60 | 10–30 |
| Pacifidlog Town | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 10 | Wailmer | 40 | 10–30 |
| Pacifidlog Town | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Dragalge | 40 | 30–35 |
| Pacifidlog Town | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Dhelmise | 30 | 25–45 |
| Pacifidlog Town | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 10 | Sharpedo | 30 | 30–35 |
| Underwater Route 124 | Land | No field move; available when the location itself is reachable | 25 | Lumineon | 18 | 25–25 |
| Underwater Route 124 | Land | No field move; available when the location itself is reachable | 25 | Chinchou | 13 | 27–27 |
| Underwater Route 124 | Land | No field move; available when the location itself is reachable | 25 | Clamperl | 13 | 25–25 |
| Underwater Route 124 | Land | No field move; available when the location itself is reachable | 25 | Golisopod | 12 | 27–29 |
| Underwater Route 124 | Land | No field move; available when the location itself is reachable | 25 | Lanturn | 10 | 25–25 |
| Underwater Route 124 | Land | No field move; available when the location itself is reachable | 25 | Relicanth | 10 | 25–25 |
| Underwater Route 124 | Land | No field move; available when the location itself is reachable | 25 | Starmie | 10 | 27–27 |
| Underwater Route 124 | Land | No field move; available when the location itself is reachable | 25 | Gorebyss | 5 | 25–25 |
| Underwater Route 124 | Land | No field move; available when the location itself is reachable | 25 | Huntail | 5 | 27–27 |
| Underwater Route 124 | Land | No field move; available when the location itself is reachable | 25 | Kingdra | 4 | 27–27 |
| Mirage Tower 1F | Land | No field move; available when the location itself is reachable | 10 | Bronzor | 20 | 20–20 |
| Mirage Tower 1F | Land | No field move; available when the location itself is reachable | 10 | Darumaka | 17 | 22–24 |
| Mirage Tower 1F | Land | No field move; available when the location itself is reachable | 10 | Yamask | 17 | 22–24 |
| Mirage Tower 1F | Land | No field move; available when the location itself is reachable | 10 | Sandshrew | 13 | 21–21 |
| Mirage Tower 1F | Land | No field move; available when the location itself is reachable | 10 | Trapinch | 13 | 21–21 |
| Mirage Tower 1F | Land | No field move; available when the location itself is reachable | 10 | Golett | 10 | 20–20 |
| Mirage Tower 1F | Land | No field move; available when the location itself is reachable | 10 | Sigilyph | 10 | 20–20 |
| Mirage Tower 2F | Land | No field move; available when the location itself is reachable | 10 | Bronzor | 20 | 20–20 |
| Mirage Tower 2F | Land | No field move; available when the location itself is reachable | 10 | Darumaka | 17 | 22–24 |
| Mirage Tower 2F | Land | No field move; available when the location itself is reachable | 10 | Yamask | 17 | 22–24 |
| Mirage Tower 2F | Land | No field move; available when the location itself is reachable | 10 | Sandshrew | 13 | 21–21 |
| Mirage Tower 2F | Land | No field move; available when the location itself is reachable | 10 | Trapinch | 13 | 21–21 |
| Mirage Tower 2F | Land | No field move; available when the location itself is reachable | 10 | Golett | 10 | 20–20 |
| Mirage Tower 2F | Land | No field move; available when the location itself is reachable | 10 | Sigilyph | 10 | 20–20 |
| Mirage Tower 3F | Land | No field move; available when the location itself is reachable | 10 | Bronzor | 20 | 20–20 |
| Mirage Tower 3F | Land | No field move; available when the location itself is reachable | 10 | Darumaka | 17 | 22–24 |
| Mirage Tower 3F | Land | No field move; available when the location itself is reachable | 10 | Yamask | 17 | 22–24 |
| Mirage Tower 3F | Land | No field move; available when the location itself is reachable | 10 | Sandshrew | 13 | 21–21 |
| Mirage Tower 3F | Land | No field move; available when the location itself is reachable | 10 | Trapinch | 13 | 21–21 |
| Mirage Tower 3F | Land | No field move; available when the location itself is reachable | 10 | Golett | 10 | 20–20 |
| Mirage Tower 3F | Land | No field move; available when the location itself is reachable | 10 | Sigilyph | 10 | 20–20 |
| Mirage Tower 4F | Land | No field move; available when the location itself is reachable | 10 | Bronzor | 20 | 20–20 |
| Mirage Tower 4F | Land | No field move; available when the location itself is reachable | 10 | Darumaka | 17 | 22–24 |
| Mirage Tower 4F | Land | No field move; available when the location itself is reachable | 10 | Yamask | 17 | 22–24 |
| Mirage Tower 4F | Land | No field move; available when the location itself is reachable | 10 | Sandshrew | 13 | 21–21 |
| Mirage Tower 4F | Land | No field move; available when the location itself is reachable | 10 | Trapinch | 13 | 21–21 |
| Mirage Tower 4F | Land | No field move; available when the location itself is reachable | 10 | Golett | 10 | 20–20 |
| Mirage Tower 4F | Land | No field move; available when the location itself is reachable | 10 | Sigilyph | 10 | 20–20 |
| Mirage Tower B1F | Land | No field move; available when the location itself is reachable | 10 | Bronzor | 20 | 20–20 |
| Mirage Tower B1F | Land | No field move; available when the location itself is reachable | 10 | Darumaka | 17 | 22–24 |
| Mirage Tower B1F | Land | No field move; available when the location itself is reachable | 10 | Yamask | 17 | 22–24 |
| Mirage Tower B1F | Land | No field move; available when the location itself is reachable | 10 | Sandshrew | 13 | 21–21 |
| Mirage Tower B1F | Land | No field move; available when the location itself is reachable | 10 | Trapinch | 13 | 21–21 |
| Mirage Tower B1F | Land | No field move; available when the location itself is reachable | 10 | Golett | 10 | 20–20 |
| Mirage Tower B1F | Land | No field move; available when the location itself is reachable | 10 | Sigilyph | 10 | 20–20 |
| Desert Underpass | Land | No field move; available when the location itself is reachable | 10 | Ditto | 66 | 35–42 |
| Desert Underpass | Land | No field move; available when the location itself is reachable | 10 | Ting Lu | 10 | 41–41 |
| Desert Underpass | Land | No field move; available when the location itself is reachable | 10 | Arctozolt | 8 | 43–43 |
| Desert Underpass | Land | No field move; available when the location itself is reachable | 10 | Dracozolt | 8 | 38–38 |
| Desert Underpass | Land | No field move; available when the location itself is reachable | 10 | Arctovish | 4 | 45–45 |
| Desert Underpass | Land | No field move; available when the location itself is reachable | 10 | Dracovish | 4 | 44–44 |
| Artisan Cave B1F | Land | No field move; available when the location itself is reachable | 10 | Smeargle | 100 | 40–50 |
| Seaspray Cave | Land | No field move; available when the location itself is reachable | 20 | Woobat | 22 | 10–12 |
| Seaspray Cave | Land | No field move; available when the location itself is reachable | 20 | Wooper | 20 | 10–12 |
| Seaspray Cave | Land | No field move; available when the location itself is reachable | 20 | Zubat | 20 | 10–12 |
| Seaspray Cave | Land | No field move; available when the location itself is reachable | 20 | Psyduck | 13 | 10–12 |
| Seaspray Cave | Land | No field move; available when the location itself is reachable | 20 | Tynamo | 13 | 10–12 |
| Seaspray Cave | Land | No field move; available when the location itself is reachable | 20 | Stunfisk | 12 | 10–12 |
| Seaspray Cave | Surf | Balance Badge (5) and HM03 Surf | 4 | Quagsire | 60 | 20–30 |
| Seaspray Cave | Surf | Balance Badge (5) and HM03 Surf | 4 | Eelektrik | 30 | 10–20 |
| Seaspray Cave | Surf | Balance Badge (5) and HM03 Surf | 4 | Golbat | 10 | 5–35 |
| Seaspray Cave | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Binacle | 60 | 10–15 |
| Seaspray Cave | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Dwebble | 40 | 5–20 |
| Seaspray Cave | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Wishiwashi | 60 | 5–10 |
| Seaspray Cave | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Krabby | 40 | 5–10 |
| Seaspray Cave | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Krabby | 60 | 10–30 |
| Seaspray Cave | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Wishiwashi | 40 | 10–30 |
| Seaspray Cave | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Wishiwashi | 60 | 20–45 |
| Seaspray Cave | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Kingler | 40 | 30–40 |
| Seaspray Cave B1F | Land | No field move; available when the location itself is reachable | 20 | Spheal | 17 | 10–12 |
| Seaspray Cave B1F | Land | No field move; available when the location itself is reachable | 20 | Vanillite | 17 | 10–12 |
| Seaspray Cave B1F | Land | No field move; available when the location itself is reachable | 20 | Seel | 13 | 10–12 |
| Seaspray Cave B1F | Land | No field move; available when the location itself is reachable | 20 | Swinub | 13 | 10–12 |
| Seaspray Cave B1F | Land | No field move; available when the location itself is reachable | 20 | Bergmite | 10 | 10–12 |
| Seaspray Cave B1F | Land | No field move; available when the location itself is reachable | 20 | Smoochum | 10 | 10–12 |
| Seaspray Cave B1F | Land | No field move; available when the location itself is reachable | 20 | Sneasel | 10 | 10–12 |
| Seaspray Cave B1F | Land | No field move; available when the location itself is reachable | 20 | Snorunt | 10 | 10–12 |
| Seaspray Cave B1F | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Cubchoo | 50 | 2–2 |
| Seaspray Cave B1F | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Snover | 40 | 2–3 |
| Seaspray Cave B1F | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Cryogonal | 5 | 3–3 |
| Seaspray Cave B1F | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Delibird | 5 | 3–3 |
| Ember Path | Land | No field move; available when the location itself is reachable | 10 | Larvesta | 24 | 41–43 |
| Ember Path | Land | No field move; available when the location itself is reachable | 10 | Magcargo | 23 | 41–43 |
| Ember Path | Land | No field move; available when the location itself is reachable | 10 | Boldore | 20 | 41–43 |
| Ember Path | Land | No field move; available when the location itself is reachable | 10 | Magmar | 13 | 41–43 |
| Ember Path | Land | No field move; available when the location itself is reachable | 10 | Golbat | 10 | 41–43 |
| Ember Path | Land | No field move; available when the location itself is reachable | 10 | Grumpig | 10 | 41–43 |
| Sandstrewn Ruins | Land | No field move; available when the location itself is reachable | 10 | Claydol | 17 | 22–24 |
| Sandstrewn Ruins | Land | No field move; available when the location itself is reachable | 10 | Gabite | 17 | 22–24 |
| Sandstrewn Ruins | Land | No field move; available when the location itself is reachable | 10 | Bronzong | 13 | 21–21 |
| Sandstrewn Ruins | Land | No field move; available when the location itself is reachable | 10 | Yamask | 13 | 21–21 |
| Sandstrewn Ruins | Land | No field move; available when the location itself is reachable | 10 | Darumaka | 10 | 20–20 |
| Sandstrewn Ruins | Land | No field move; available when the location itself is reachable | 10 | Golett | 10 | 20–20 |
| Sandstrewn Ruins | Land | No field move; available when the location itself is reachable | 10 | Honedge | 10 | 20–20 |
| Sandstrewn Ruins | Land | No field move; available when the location itself is reachable | 10 | Sigilyph | 10 | 20–20 |
| Sandstrewn Ruins | Surf | Balance Badge (5) and HM03 Surf | 4 | Relicanth | 100 | 5–35 |
| Sandstrewn Ruins | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Baltoy | 60 | 20–30 |
| Sandstrewn Ruins | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Onix | 30 | 10–20 |
| Sandstrewn Ruins | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Steelix | 10 | 5–35 |
| Sandstrewn Ruins | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Relicanth | 100 | 5–10 |
| Sandstrewn Ruins | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Relicanth | 100 | 10–30 |
| Sandstrewn Ruins | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Relicanth | 100 | 20–45 |
| Sandstrewn Ruins B1F | Land | No field move; available when the location itself is reachable | 10 | Claydol | 17 | 22–24 |
| Sandstrewn Ruins B1F | Land | No field move; available when the location itself is reachable | 10 | Gabite | 17 | 22–24 |
| Sandstrewn Ruins B1F | Land | No field move; available when the location itself is reachable | 10 | Bronzong | 13 | 21–21 |
| Sandstrewn Ruins B1F | Land | No field move; available when the location itself is reachable | 10 | Yamask | 13 | 21–21 |
| Sandstrewn Ruins B1F | Land | No field move; available when the location itself is reachable | 10 | Darumaka | 10 | 20–20 |
| Sandstrewn Ruins B1F | Land | No field move; available when the location itself is reachable | 10 | Golett | 10 | 20–20 |
| Sandstrewn Ruins B1F | Land | No field move; available when the location itself is reachable | 10 | Honedge | 10 | 20–20 |
| Sandstrewn Ruins B1F | Land | No field move; available when the location itself is reachable | 10 | Sigilyph | 10 | 20–20 |
| Sandstrewn Ruins B1F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Baltoy | 60 | 20–30 |
| Sandstrewn Ruins B1F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Onix | 30 | 10–20 |
| Sandstrewn Ruins B1F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Steelix | 10 | 5–35 |
| Sandstrewn Ruins 2F | Land | No field move; available when the location itself is reachable | 10 | Claydol | 17 | 22–24 |
| Sandstrewn Ruins 2F | Land | No field move; available when the location itself is reachable | 10 | Gabite | 17 | 22–24 |
| Sandstrewn Ruins 2F | Land | No field move; available when the location itself is reachable | 10 | Bronzong | 13 | 21–21 |
| Sandstrewn Ruins 2F | Land | No field move; available when the location itself is reachable | 10 | Yamask | 13 | 21–21 |
| Sandstrewn Ruins 2F | Land | No field move; available when the location itself is reachable | 10 | Darumaka | 10 | 20–20 |
| Sandstrewn Ruins 2F | Land | No field move; available when the location itself is reachable | 10 | Golett | 10 | 20–20 |
| Sandstrewn Ruins 2F | Land | No field move; available when the location itself is reachable | 10 | Honedge | 10 | 20–20 |
| Sandstrewn Ruins 2F | Land | No field move; available when the location itself is reachable | 10 | Sigilyph | 10 | 20–20 |
| Sandstrewn Ruins 2F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Baltoy | 60 | 20–30 |
| Sandstrewn Ruins 2F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Onix | 30 | 10–20 |
| Sandstrewn Ruins 2F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Steelix | 10 | 5–35 |
| Sandstrewn Ruins 3F | Land | No field move; available when the location itself is reachable | 10 | Claydol | 17 | 22–24 |
| Sandstrewn Ruins 3F | Land | No field move; available when the location itself is reachable | 10 | Gabite | 17 | 22–24 |
| Sandstrewn Ruins 3F | Land | No field move; available when the location itself is reachable | 10 | Bronzong | 13 | 21–21 |
| Sandstrewn Ruins 3F | Land | No field move; available when the location itself is reachable | 10 | Yamask | 13 | 21–21 |
| Sandstrewn Ruins 3F | Land | No field move; available when the location itself is reachable | 10 | Darumaka | 10 | 20–20 |
| Sandstrewn Ruins 3F | Land | No field move; available when the location itself is reachable | 10 | Golett | 10 | 20–20 |
| Sandstrewn Ruins 3F | Land | No field move; available when the location itself is reachable | 10 | Honedge | 10 | 20–20 |
| Sandstrewn Ruins 3F | Land | No field move; available when the location itself is reachable | 10 | Sigilyph | 10 | 20–20 |
| Sandstrewn Ruins 3F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Baltoy | 60 | 20–30 |
| Sandstrewn Ruins 3F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Onix | 30 | 10–20 |
| Sandstrewn Ruins 3F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Steelix | 10 | 5–35 |
| Verdanturf Meadow | Land | No field move; available when the location itself is reachable | 20 | Floette White Flower | 20 | 41–43 |
| Verdanturf Meadow | Land | No field move; available when the location itself is reachable | 20 | Espurr | 13 | 41–43 |
| Verdanturf Meadow | Land | No field move; available when the location itself is reachable | 20 | Munna | 13 | 41–43 |
| Verdanturf Meadow | Land | No field move; available when the location itself is reachable | 20 | Flabebe Blue Flower | 12 | 41–43 |
| Verdanturf Meadow | Land | No field move; available when the location itself is reachable | 20 | Flabebe White Flower | 12 | 41–43 |
| Verdanturf Meadow | Land | No field move; available when the location itself is reachable | 20 | Ribombee | 10 | 41–43 |
| Verdanturf Meadow | Land | No field move; available when the location itself is reachable | 20 | Stufful | 10 | 41–43 |
| Verdanturf Meadow | Land | No field move; available when the location itself is reachable | 20 | Vivillon Poke Ball | 10 | 41–43 |
| Verdanturf Meadow | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Audino | 50 | 2–2 |
| Verdanturf Meadow | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Ribombee | 45 | 2–3 |
| Verdanturf Meadow | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Milcery | 5 | 3–3 |
| Scorched Slab | Land | No field move; available when the location itself is reachable | 20 | Golbat | 88 | 2–4 |
| Scorched Slab | Land | No field move; available when the location itself is reachable | 20 | Crobat | 12 | 3–4 |
| Scorched Slab | Surf | Balance Badge (5) and HM03 Surf | 4 | Golbat | 95 | 5–35 |
| Scorched Slab | Surf | Balance Badge (5) and HM03 Surf | 4 | Crobat | 5 | 25–30 |
| Scorched Slab | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Goldeen | 60 | 5–10 |
| Scorched Slab | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Barboach | 40 | 5–10 |
| Scorched Slab | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Barboach | 60 | 10–30 |
| Scorched Slab | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Goldeen | 20 | 10–30 |
| Scorched Slab | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Whiscash | 20 | 10–30 |
| Scorched Slab | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Barboach | 40 | 30–35 |
| Scorched Slab | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Seaking | 30 | 30–35 |
| Scorched Slab | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Whiscash | 30 | 25–45 |
| Scorched Slab B1F | Land | No field move; available when the location itself is reachable | 20 | Zweilous | 24 | 2–4 |
| Scorched Slab B1F | Land | No field move; available when the location itself is reachable | 20 | Boldore | 20 | 2–3 |
| Scorched Slab B1F | Land | No field move; available when the location itself is reachable | 20 | Dugtrio | 20 | 3–4 |
| Scorched Slab B1F | Land | No field move; available when the location itself is reachable | 20 | Golbat | 18 | 2–3 |
| Scorched Slab B1F | Land | No field move; available when the location itself is reachable | 20 | Gurdurr | 18 | 3–4 |
| Scorched Slab B1F | Surf | Balance Badge (5) and HM03 Surf | 4 | Golbat | 95 | 5–35 |
| Scorched Slab B1F | Surf | Balance Badge (5) and HM03 Surf | 4 | Crobat | 5 | 25–30 |
| Scorched Slab B1F | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Goldeen | 60 | 5–10 |
| Scorched Slab B1F | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Barboach | 40 | 5–10 |
| Scorched Slab B1F | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Barboach | 60 | 10–30 |
| Scorched Slab B1F | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Goldeen | 20 | 10–30 |
| Scorched Slab B1F | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Whiscash | 20 | 10–30 |
| Scorched Slab B1F | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Barboach | 40 | 30–35 |
| Scorched Slab B1F | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Seaking | 30 | 30–35 |
| Scorched Slab B1F | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Whiscash | 30 | 25–45 |
| Scorched Slab B2F | Land | No field move; available when the location itself is reachable | 20 | Boldore | 20 | 2–3 |
| Scorched Slab B2F | Land | No field move; available when the location itself is reachable | 20 | Dugtrio | 20 | 3–4 |
| Scorched Slab B2F | Land | No field move; available when the location itself is reachable | 20 | Magmar | 17 | 2–3 |
| Scorched Slab B2F | Land | No field move; available when the location itself is reachable | 20 | Turtonator | 17 | 3–4 |
| Scorched Slab B2F | Land | No field move; available when the location itself is reachable | 20 | Golbat | 13 | 2–2 |
| Scorched Slab B2F | Land | No field move; available when the location itself is reachable | 20 | Gurdurr | 13 | 3–3 |
| Scorched Slab Heatrans Room | Land | No field move; available when the location itself is reachable | 20 | Boldore | 20 | 2–3 |
| Scorched Slab Heatrans Room | Land | No field move; available when the location itself is reachable | 20 | Dugtrio | 20 | 3–4 |
| Scorched Slab Heatrans Room | Land | No field move; available when the location itself is reachable | 20 | Magmar | 17 | 2–3 |
| Scorched Slab Heatrans Room | Land | No field move; available when the location itself is reachable | 20 | Turtonator | 17 | 3–4 |
| Scorched Slab Heatrans Room | Land | No field move; available when the location itself is reachable | 20 | Golbat | 13 | 2–2 |
| Scorched Slab Heatrans Room | Land | No field move; available when the location itself is reachable | 20 | Magcargo | 13 | 3–3 |
| Artisan Cave 1F | Land | No field move; available when the location itself is reachable | 10 | Smeargle | 100 | 40–50 |
| Altering Cave | Land | No field move; available when the location itself is reachable | 20 | Eelektross | 24 | 2–4 |
| Altering Cave | Land | No field move; available when the location itself is reachable | 20 | Rhydon | 13 | 2–2 |
| Altering Cave | Land | No field move; available when the location itself is reachable | 20 | Swoobat | 13 | 3–3 |
| Altering Cave | Land | No field move; available when the location itself is reachable | 20 | Druddigon | 10 | 3–3 |
| Altering Cave | Land | No field move; available when the location itself is reachable | 20 | Gigalith | 10 | 3–4 |
| Altering Cave | Land | No field move; available when the location itself is reachable | 20 | Lucario | 10 | 4–4 |
| Altering Cave | Land | No field move; available when the location itself is reachable | 20 | Parasect | 10 | 2–2 |
| Altering Cave | Land | No field move; available when the location itself is reachable | 20 | Wobbuffet | 10 | 3–3 |
| Altering Cave | Surf | Balance Badge (5) and HM03 Surf | 4 | Seaking | 60 | 5–35 |
| Altering Cave | Surf | Balance Badge (5) and HM03 Surf | 4 | Gastrodon | 30 | 10–30 |
| Altering Cave | Surf | Balance Badge (5) and HM03 Surf | 4 | Whiscash | 10 | 15–30 |
| Altering Cave | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Steelix | 60 | 10–15 |
| Altering Cave | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Shuckle | 30 | 5–10 |
| Altering Cave | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Crustle | 10 | 15–20 |
| Altering Cave | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 60 | 5–10 |
| Altering Cave | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Barboach | 40 | 5–10 |
| Altering Cave | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Gyarados | 60 | 10–30 |
| Altering Cave | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Whiscash | 40 | 10–30 |
| Altering Cave | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Gyarados | 40 | 30–35 |
| Altering Cave | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Seaking | 30 | 25–45 |
| Altering Cave | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Whiscash | 30 | 30–35 |
| Altering Cave | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Blissey | 100 | 2–3 |
| Altering Cave 1F | Land | No field move; available when the location itself is reachable | 20 | Dugtrio | 24 | 2–4 |
| Altering Cave 1F | Land | No field move; available when the location itself is reachable | 20 | Noivern | 13 | 2–2 |
| Altering Cave 1F | Land | No field move; available when the location itself is reachable | 20 | Swoobat | 13 | 3–3 |
| Altering Cave 1F | Land | No field move; available when the location itself is reachable | 20 | Druddigon | 10 | 3–3 |
| Altering Cave 1F | Land | No field move; available when the location itself is reachable | 20 | Gigalith | 10 | 3–4 |
| Altering Cave 1F | Land | No field move; available when the location itself is reachable | 20 | Lucario | 10 | 4–4 |
| Altering Cave 1F | Land | No field move; available when the location itself is reachable | 20 | Shiinotic | 10 | 2–2 |
| Altering Cave 1F | Land | No field move; available when the location itself is reachable | 20 | Wobbuffet | 10 | 3–3 |
| Altering Cave 1F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Steelix | 60 | 10–15 |
| Altering Cave 1F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Shuckle | 30 | 5–10 |
| Altering Cave 1F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Crustle | 10 | 15–20 |
| Altering Cave 1F | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Blissey | 100 | 2–3 |
| Altering Cave B1F | Land | No field move; available when the location itself is reachable | 20 | Eelektross | 24 | 2–4 |
| Altering Cave B1F | Land | No field move; available when the location itself is reachable | 20 | Rhydon | 13 | 2–2 |
| Altering Cave B1F | Land | No field move; available when the location itself is reachable | 20 | Swoobat | 13 | 3–3 |
| Altering Cave B1F | Land | No field move; available when the location itself is reachable | 20 | Druddigon | 10 | 3–3 |
| Altering Cave B1F | Land | No field move; available when the location itself is reachable | 20 | Gigalith | 10 | 3–4 |
| Altering Cave B1F | Land | No field move; available when the location itself is reachable | 20 | Lucario | 10 | 4–4 |
| Altering Cave B1F | Land | No field move; available when the location itself is reachable | 20 | Parasect | 10 | 2–2 |
| Altering Cave B1F | Land | No field move; available when the location itself is reachable | 20 | Wobbuffet | 10 | 3–3 |
| Altering Cave B1F | Surf | Balance Badge (5) and HM03 Surf | 4 | Seaking | 60 | 5–35 |
| Altering Cave B1F | Surf | Balance Badge (5) and HM03 Surf | 4 | Gastrodon | 30 | 10–30 |
| Altering Cave B1F | Surf | Balance Badge (5) and HM03 Surf | 4 | Whiscash | 10 | 15–30 |
| Altering Cave B1F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Steelix | 60 | 10–15 |
| Altering Cave B1F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Shuckle | 30 | 5–10 |
| Altering Cave B1F | Rock Smash | Dynamo Badge (3) and HM06 Rock Smash | 20 | Crustle | 10 | 15–20 |
| Altering Cave B1F | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Magikarp | 60 | 5–10 |
| Altering Cave B1F | Old Rod | Old Rod from Mom in Littleroot during the opening | 30 | Barboach | 40 | 5–10 |
| Altering Cave B1F | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Gyarados | 60 | 10–30 |
| Altering Cave B1F | Good Rod | Balance Badge route access and the Good Rod gift on Route 118 | 30 | Whiscash | 40 | 10–30 |
| Altering Cave B1F | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Gyarados | 40 | 30–35 |
| Altering Cave B1F | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Seaking | 30 | 25–45 |
| Altering Cave B1F | Super Rod | Feather Badge route access and the Super Rod gift in Mossdeep | 30 | Whiscash | 30 | 30–35 |
| Altering Cave B1F | Honey | Stone Badge (1); Honey enters ordinary medicine-Mart stock | 20 | Blissey | 100 | 2–3 |
| Altering Cave | Land | No field move; available when the location itself is reachable | 7 | Unown | 100 | 3–13 |
| Altering Cave | Land | No field move; available when the location itself is reachable | 7 | Pineco | 100 | 19–29 |
| Altering Cave | Land | No field move; available when the location itself is reachable | 7 | Houndour | 100 | 12–22 |
| Altering Cave | Land | No field move; available when the location itself is reachable | 7 | Teddiursa | 100 | 6–16 |
| Altering Cave | Land | No field move; available when the location itself is reachable | 7 | Aipom | 100 | 18–28 |
| Altering Cave | Land | No field move; available when the location itself is reachable | 7 | Shuckle | 100 | 18–28 |
| Altering Cave | Land | No field move; available when the location itself is reachable | 7 | Stantler | 100 | 18–28 |
| Altering Cave | Land | No field move; available when the location itself is reachable | 7 | Smeargle | 100 | 18–28 |
| Meteor Falls Stevens Cave | Land | No field move; available when the location itself is reachable | 10 | Crobat | 20 | 38–40 |
| Meteor Falls Stevens Cave | Land | No field move; available when the location itself is reachable | 10 | Metang | 20 | 33–35 |
| Meteor Falls Stevens Cave | Land | No field move; available when the location itself is reachable | 10 | Metagross | 14 | 33–40 |
| Meteor Falls Stevens Cave | Land | No field move; available when the location itself is reachable | 10 | Lunatone | 13 | 33–33 |
| Meteor Falls Stevens Cave | Land | No field move; available when the location itself is reachable | 10 | Solrock | 13 | 35–35 |
| Meteor Falls Stevens Cave | Land | No field move; available when the location itself is reachable | 10 | Druddigon | 10 | 35–39 |
| Meteor Falls Stevens Cave | Land | No field move; available when the location itself is reachable | 10 | Ferrothorn | 10 | 37–37 |

## Facility-only random battle tables

| Facility table | Method | Species | Source-table % | Source levels |
|---|---|---|---|---|
| Battle Pyramid Wild Mon Headers table 1 | Land | Bulbasaur | 46 | 5–5 |
| Battle Pyramid Wild Mon Headers table 1 | Land | Venusaur | 22 | 5–5 |
| Battle Pyramid Wild Mon Headers table 1 | Land | Ivysaur | 20 | 5–5 |
| Battle Pyramid Wild Mon Headers table 1 | Land | Charmander | 12 | 5–5 |
| Battle Pyramid Wild Mon Headers table 2 | Land | Ivysaur | 46 | 5–5 |
| Battle Pyramid Wild Mon Headers table 2 | Land | Charmander | 22 | 5–5 |
| Battle Pyramid Wild Mon Headers table 2 | Land | Venusaur | 20 | 5–5 |
| Battle Pyramid Wild Mon Headers table 2 | Land | Charmeleon | 12 | 5–5 |
| Battle Pyramid Wild Mon Headers table 3 | Land | Venusaur | 46 | 5–5 |
| Battle Pyramid Wild Mon Headers table 3 | Land | Charmeleon | 22 | 5–5 |
| Battle Pyramid Wild Mon Headers table 3 | Land | Charmander | 20 | 5–5 |
| Battle Pyramid Wild Mon Headers table 3 | Land | Charizard | 12 | 5–5 |
| Battle Pyramid Wild Mon Headers table 4 | Land | Charmander | 46 | 5–5 |
| Battle Pyramid Wild Mon Headers table 4 | Land | Charizard | 22 | 5–5 |
| Battle Pyramid Wild Mon Headers table 4 | Land | Charmeleon | 20 | 5–5 |
| Battle Pyramid Wild Mon Headers table 4 | Land | Squirtle | 12 | 5–5 |
| Battle Pyramid Wild Mon Headers table 5 | Land | Charmeleon | 46 | 5–5 |
| Battle Pyramid Wild Mon Headers table 5 | Land | Squirtle | 22 | 5–5 |
| Battle Pyramid Wild Mon Headers table 5 | Land | Charizard | 20 | 5–5 |
| Battle Pyramid Wild Mon Headers table 5 | Land | Wartortle | 12 | 5–5 |
| Battle Pyramid Wild Mon Headers table 6 | Land | Charizard | 36 | 5–5 |
| Battle Pyramid Wild Mon Headers table 6 | Land | Wartortle | 34 | 5–5 |
| Battle Pyramid Wild Mon Headers table 6 | Land | Squirtle | 20 | 5–5 |
| Battle Pyramid Wild Mon Headers table 6 | Land | Charmeleon | 10 | 5–5 |
| Battle Pyramid Wild Mon Headers table 7 | Land | Squirtle | 30 | 5–5 |
| Battle Pyramid Wild Mon Headers table 7 | Land | Wartortle | 26 | 5–5 |
| Battle Pyramid Wild Mon Headers table 7 | Land | Charmeleon | 24 | 5–5 |
| Battle Pyramid Wild Mon Headers table 7 | Land | Charizard | 20 | 5–5 |
| Battle Pike Wild Mon Headers table 1 | Land | Dusclops | 56 | 5–5 |
| Battle Pike Wild Mon Headers table 1 | Land | Milotic | 22 | 5–5 |
| Battle Pike Wild Mon Headers table 1 | Land | Seviper | 22 | 5–5 |
| Battle Pike Wild Mon Headers table 2 | Land | Electrode | 56 | 5–5 |
| Battle Pike Wild Mon Headers table 2 | Land | Milotic | 22 | 5–5 |
| Battle Pike Wild Mon Headers table 2 | Land | Seviper | 22 | 5–5 |
| Battle Pike Wild Mon Headers table 3 | Land | Breloom | 56 | 5–5 |
| Battle Pike Wild Mon Headers table 3 | Land | Milotic | 22 | 5–5 |
| Battle Pike Wild Mon Headers table 3 | Land | Seviper | 22 | 5–5 |
| Battle Pike Wild Mon Headers table 4 | Land | Wobbuffet | 56 | 5–5 |
| Battle Pike Wild Mon Headers table 4 | Land | Milotic | 22 | 5–5 |
| Battle Pike Wild Mon Headers table 4 | Land | Seviper | 22 | 5–5 |

Facility source levels may be transformed by the facility runtime. These rows document source-table composition, not a promise that the displayed level is the final battle level.

## Non-random and special acquisition systems

The following table covers source-resolved scripted/static battles, gifts, Eggs, starter choices, fossil restoration, mystery gifts, Game Corner prizes, NPC trades, and roamers. Dynamic event variables are expanded where their backing source table is explicit.

| Species | Acquisition type | Location/system | Requirement/details | Source |
|---|---|---|---|---|
| Cosmog | Authored gift/restoration | Littleroot Town Professor Birchs Lab | cosmog | data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc |
| Meltan | Authored gift/restoration | Mossdeep City Stevens House | meltan | data/maps/MossdeepCity_StevensHouse/scripts.inc |
| Togepi | Authored gift/restoration | Route 117 Pokemon Day Care | togepi | data/maps/Route117_PokemonDayCare/scripts.inc |
| Castform | Authored gift/restoration | Route 119 Weather Institute 2F | castform | data/maps/Route119_WeatherInstitute_2F/scripts.inc |
| Spiritomb | Authored static acquisition | Abandoned Ship Room B1F | spiritomb | data/maps/AbandonedShip_Room_B1F/scripts.inc |
| Mewtwo | Authored static acquisition | Altering Cave B1F | mewtwo | data/maps/AlteringCave_B1F/scripts.inc |
| Registeel | Authored static acquisition | Ancient Tomb | registeel | data/maps/AncientTomb/scripts.inc |
| Deoxys | Authored static acquisition | Birth Island Exterior | deoxys | data/maps/BirthIsland_Exterior/scripts.inc |
| Diancie | Authored static acquisition | Cave Of Origin Diancies Room | diancie | data/maps/CaveOfOrigin_DianciesRoom/scripts.inc |
| Regirock | Authored static acquisition | Desert Ruins | regirock | data/maps/DesertRuins/scripts.inc |
| Moltres | Authored static acquisition | Ember Path | moltres | data/maps/EmberPath/scripts.inc |
| Mew | Authored static acquisition | Faraway Island Interior | mew | data/maps/FarawayIsland_Interior/scripts.inc |
| Regice | Authored static acquisition | Island Cave | regice | data/maps/IslandCave/scripts.inc |
| Kyogre | Authored static acquisition | Marine Cave End | kyogre | data/maps/MarineCave_End/scripts.inc |
| Jirachi | Authored static acquisition | Meteor Falls Jirachis Room | jirachi | data/maps/MeteorFalls_JirachisRoom/scripts.inc |
| Lugia | Authored static acquisition | Navel Rock Bottom | lugia | data/maps/NavelRock_Bottom/scripts.inc |
| Ho-Oh | Authored static acquisition | Navel Rock Top | ho_oh | data/maps/NavelRock_Top/scripts.inc |
| Rotom | Authored static acquisition | New Mauville Inside | rotom | data/maps/NewMauville_Inside/scripts.inc |
| Zapdos | Authored static acquisition | New Mauville Inside | zapdos | data/maps/NewMauville_Inside/scripts.inc |
| Kecleon | Authored static acquisition | Route 120 | kecleon | data/scripts/kecleon.inc and data/maps/Route120/scripts.inc |
| Heatran | Authored static acquisition | Scorched Slab Heatrans Room | heatran | data/maps/ScorchedSlab_HeatransRoom/scripts.inc |
| Regigigas | Authored static acquisition | Sealed Chamber Inner Room | regigigas | data/maps/SealedChamber_InnerRoom/scripts.inc |
| Articuno | Authored static acquisition | Shoal Cave Low Tide Ice Room | articuno | data/maps/ShoalCave_LowTideIceRoom/scripts.inc |
| Rayquaza | Authored static acquisition | Sky Pillar Top | rayquaza | data/maps/SkyPillar_Top/scripts.inc |
| Latias | Authored static acquisition | Southern Island Interior | latias | data/maps/SouthernIsland_Interior/scripts.inc |
| Latios | Authored static acquisition | Southern Island Interior | latios | data/maps/SouthernIsland_Interior/scripts.inc |
| Groudon | Authored static acquisition | Terra Cave End | groudon | data/maps/TerraCave_End/scripts.inc |
| Aerodactyl | Fossil restoration | Rustboro Devon Corporation | Restore ITEM_OLD_AMBER | src/field_specials.c |
| Amaura | Fossil restoration | Rustboro Devon Corporation | Restore ITEM_SAIL_FOSSIL | src/field_specials.c |
| Anorith | Fossil restoration | Rustboro Devon Corporation | Restore ITEM_CLAW_FOSSIL | src/field_specials.c |
| Archen | Fossil restoration | Rustboro Devon Corporation | Restore ITEM_PLUME_FOSSIL | src/field_specials.c |
| Cranidos | Fossil restoration | Rustboro Devon Corporation | Restore ITEM_SKULL_FOSSIL | src/field_specials.c |
| Kabuto | Fossil restoration | Rustboro Devon Corporation | Restore ITEM_DOME_FOSSIL | src/field_specials.c |
| Lileep | Fossil restoration | Rustboro Devon Corporation | Restore ITEM_ROOT_FOSSIL | src/field_specials.c |
| Omanyte | Fossil restoration | Rustboro Devon Corporation | Restore ITEM_HELIX_FOSSIL | src/field_specials.c |
| Shieldon | Fossil restoration | Rustboro Devon Corporation | Restore ITEM_ARMOR_FOSSIL | src/field_specials.c |
| Tirtouga | Fossil restoration | Rustboro Devon Corporation | Restore ITEM_COVER_FOSSIL | src/field_specials.c |
| Tyrunt | Fossil restoration | Rustboro Devon Corporation | Restore ITEM_JAW_FOSSIL | src/field_specials.c |
| Pichu | Gift Egg | Global event system | Literal giveegg acquisition | data/scripts/mevent_pichu.inc |
| Wynaut | Gift Egg | Lavaridge Town | Literal giveegg acquisition | data/maps/LavaridgeTown/scripts.inc |
| Happiny | In-game trade | NPC trade | Trade for SPECIES_PYUKUMUKU; received at level 32 | src/data/trade.h |
| Karrablast | In-game trade | NPC trade | Trade for SPECIES_SHELMET; received at level 50 | src/data/trade.h |
| Pawniard | In-game trade | NPC trade | Trade for SPECIES_GOTHITA; received at level 15 | src/data/trade.h |
| Tropius | In-game trade | NPC trade | Trade for SPECIES_CROAGUNK; received at level 21 | src/data/trade.h |
| Chikorita | NPC or story gift | Littleroot Town Professor Birchs Lab | Literal givemon acquisition | data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc |
| Cosmog | NPC or story gift | Littleroot Town Professor Birchs Lab | Literal givemon acquisition | data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc |
| Cyndaquil | NPC or story gift | Littleroot Town Professor Birchs Lab | Literal givemon acquisition | data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc |
| Totodile | NPC or story gift | Littleroot Town Professor Birchs Lab | Literal givemon acquisition | data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc |
| Meltan | NPC or story gift | Mossdeep City Stevens House | Literal givemon acquisition | data/maps/MossdeepCity_StevensHouse/scripts.inc |
| Castform | NPC or story gift | Route 119 Weather Institute 2F | Literal givemon acquisition | data/maps/Route119_WeatherInstitute_2F/scripts.inc |
| Audino | NPC or story gift | Verdanturf Meadow | Literal givemon acquisition | data/maps/VerdanturfMeadow/scripts.inc |
| Greninja Battle Bond | Pokémon Center mystery gift | Pokémon Centers | Requires FLAG_BADGE04_GET; arrives holding ITEM_COMET_SHARD | src/field_specials.c |
| Magearna | Pokémon Center mystery gift | Pokémon Centers | Requires FLAG_SYS_GAME_CLEAR; arrives holding ITEM_MASTER_BALL | src/field_specials.c |
| Meloetta | Pokémon Center mystery gift | Pokémon Centers | Requires FLAG_BADGE05_GET; arrives holding ITEM_RELIC_STATUE | src/field_specials.c |
| Bulbasaur | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Charmander | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Chespin | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Chikorita | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Chimchar | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Cyndaquil | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Diglett Alolan | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Exeggutor Alolan | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Fennekin | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Froakie | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Geodude Alolan | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Grimer Alolan | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Litten | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Marowak Alolan | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Meowth Alolan | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Mudkip | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Munchlax | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Oshawott | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Piplup | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Popplio | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Porygon | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Raichu Alolan | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Rattata Alolan | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Rowlet | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Sandshrew Alolan | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Snivy | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Squirtle | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Tepig | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Torchic | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Totodile | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Treecko | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Turtwig | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Vulpix Alolan | Prize Pokémon | Mauville Game Corner | Coin-exchange Pokémon prize | data/maps/MauvilleCity_GameCorner/scripts.inc |
| Latias | Roaming encounter | Hoenn overworld after the television choice | Roamer has no fixed route percentage | src/roamer.c |
| Latios | Roaming encounter | Hoenn overworld after the television choice | Roamer has no fixed route percentage | src/roamer.c |
| Spiritomb | Scripted/static encounter | Abandoned Ship Room B1F | Literal setwildbattle acquisition | data/maps/AbandonedShip_Room_B1F/scripts.inc |
| Mewtwo | Scripted/static encounter | Altering Cave B1F | Literal setwildbattle acquisition | data/maps/AlteringCave_B1F/scripts.inc |
| Registeel | Scripted/static encounter | Ancient Tomb | Literal setwildbattle acquisition | data/maps/AncientTomb/scripts.inc |
| Electrode | Scripted/static encounter | Aqua Hideout B1F | Literal setwildbattle acquisition | data/maps/AquaHideout_B1F/scripts.inc |
| Sudowoodo | Scripted/static encounter | Battle Frontier Outside East | Literal setwildbattle acquisition | data/maps/BattleFrontier_OutsideEast/scripts.inc |
| Diancie | Scripted/static encounter | Cave Of Origin Diancies Room | Literal setwildbattle acquisition | data/maps/CaveOfOrigin_DianciesRoom/scripts.inc |
| Regirock | Scripted/static encounter | Desert Ruins | Literal setwildbattle acquisition | data/maps/DesertRuins/scripts.inc |
| Moltres | Scripted/static encounter | Ember Path | Literal setwildbattle acquisition | data/maps/EmberPath/scripts.inc |
| Kecleon | Scripted/static encounter | Global event system | Literal setwildbattle acquisition | data/scripts/kecleon.inc |
| Regice | Scripted/static encounter | Island Cave | Literal setwildbattle acquisition | data/maps/IslandCave/scripts.inc |
| Kyogre | Scripted/static encounter | Marine Cave End | Literal setwildbattle acquisition | data/maps/MarineCave_End/scripts.inc |
| Jirachi | Scripted/static encounter | Meteor Falls Jirachis Room | Literal setwildbattle acquisition | data/maps/MeteorFalls_JirachisRoom/scripts.inc |
| Electrode | Scripted/static encounter | New Mauville Inside | Literal setwildbattle acquisition | data/maps/NewMauville_Inside/scripts.inc |
| Rotom | Scripted/static encounter | New Mauville Inside | Literal setwildbattle acquisition | data/maps/NewMauville_Inside/scripts.inc |
| Zapdos | Scripted/static encounter | New Mauville Inside | Literal setwildbattle acquisition | data/maps/NewMauville_Inside/scripts.inc |
| Kecleon | Scripted/static encounter | Route 120 | Literal setwildbattle acquisition | data/maps/Route120/scripts.inc |
| Heatran | Scripted/static encounter | Scorched Slab Heatrans Room | Literal setwildbattle acquisition | data/maps/ScorchedSlab_HeatransRoom/scripts.inc |
| Regigigas | Scripted/static encounter | Sealed Chamber Inner Room | Literal setwildbattle acquisition | data/maps/SealedChamber_InnerRoom/scripts.inc |
| Articuno | Scripted/static encounter | Shoal Cave Low Tide Ice Room | Literal setwildbattle acquisition | data/maps/ShoalCave_LowTideIceRoom/scripts.inc |
| Rayquaza | Scripted/static encounter | Sky Pillar Top | Literal setwildbattle acquisition | data/maps/SkyPillar_Top/scripts.inc |
| Groudon | Scripted/static encounter | Terra Cave End | Literal setwildbattle acquisition | data/maps/TerraCave_End/scripts.inc |
| Bulbasaur | Starter choice | Littleroot opening | Selectable Kanto starter | src/starter_choose.c |
| Charmander | Starter choice | Littleroot opening | Selectable Kanto starter | src/starter_choose.c |
| Chespin | Starter choice | Littleroot opening | Selectable Kalos starter | src/starter_choose.c |
| Chikorita | Starter choice | Littleroot opening | Selectable Johto starter | src/starter_choose.c |
| Chimchar | Starter choice | Littleroot opening | Selectable Sinnoh starter | src/starter_choose.c |
| Cyndaquil | Starter choice | Littleroot opening | Selectable Johto starter | src/starter_choose.c |
| Fennekin | Starter choice | Littleroot opening | Selectable Kalos starter | src/starter_choose.c |
| Froakie | Starter choice | Littleroot opening | Selectable Kalos starter | src/starter_choose.c |
| Litten | Starter choice | Littleroot opening | Selectable Alola starter | src/starter_choose.c |
| Mudkip | Starter choice | Littleroot opening | Selectable Hoenn starter | src/starter_choose.c |
| Oshawott | Starter choice | Littleroot opening | Selectable Unova starter | src/starter_choose.c |
| Piplup | Starter choice | Littleroot opening | Selectable Sinnoh starter | src/starter_choose.c |
| Popplio | Starter choice | Littleroot opening | Selectable Alola starter | src/starter_choose.c |
| Rowlet | Starter choice | Littleroot opening | Selectable Alola starter | src/starter_choose.c |
| Snivy | Starter choice | Littleroot opening | Selectable Unova starter | src/starter_choose.c |
| Squirtle | Starter choice | Littleroot opening | Selectable Kanto starter | src/starter_choose.c |
| Tepig | Starter choice | Littleroot opening | Selectable Unova starter | src/starter_choose.c |
| Torchic | Starter choice | Littleroot opening | Selectable Hoenn starter | src/starter_choose.c |
| Totodile | Starter choice | Littleroot opening | Selectable Johto starter | src/starter_choose.c |
| Treecko | Starter choice | Littleroot opening | Selectable Hoenn starter | src/starter_choose.c |
| Turtwig | Starter choice | Littleroot opening | Selectable Sinnoh starter | src/starter_choose.c |

## Complete species/form acquisition appendix

This appendix covers every runtime species/form ID. `Direct acquisition` means the exact ID appears in a catchable random pool or source-resolved non-random event. `Evolution` means a permanent evolution chain from an obtainable parent. Battle-only and automatic forms are not separate collectibles. `Unresolved` is an audit flag, not a claim that the Pokémon is definitely unobtainable.

| # | Species/form | Runtime ID | Classification | Acquisition path | Runtime status |
|---|---|---|---|---|---|
| 1 | Bulbasaur | SPECIES_BULBASAUR | Direct acquisition | Starter choice: Littleroot opening (Selectable Kanto starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 2 | Ivysaur | SPECIES_IVYSAUR | Evolution from obtainable Pokémon | Bulbasaur → Ivysaur via Level (16) | party-selectable species/form |
| 3 | Venusaur | SPECIES_VENUSAUR | Evolution from obtainable Pokémon | Bulbasaur → Ivysaur via Level (16) ; Ivysaur → Venusaur via Level (32) | party-selectable species/form |
| 4 | Charmander | SPECIES_CHARMANDER | Direct acquisition | Random wild in 1 catchable method pool(s): Fiery Path / Land \| Starter choice: Littleroot opening (Selectable Kanto starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 5 | Charmeleon | SPECIES_CHARMELEON | Evolution from obtainable Pokémon | Charmander → Charmeleon via Level (16) | party-selectable species/form |
| 6 | Charizard | SPECIES_CHARIZARD | Evolution from obtainable Pokémon | Charmander → Charmeleon via Level (16) ; Charmeleon → Charizard via Level (36) | party-selectable species/form |
| 7 | Squirtle | SPECIES_SQUIRTLE | Direct acquisition | Starter choice: Littleroot opening (Selectable Kanto starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 8 | Wartortle | SPECIES_WARTORTLE | Evolution from obtainable Pokémon | Squirtle → Wartortle via Level (16) | party-selectable species/form |
| 9 | Blastoise | SPECIES_BLASTOISE | Evolution from obtainable Pokémon | Squirtle → Wartortle via Level (16) ; Wartortle → Blastoise via Level (36) | party-selectable species/form |
| 10 | Caterpie | SPECIES_CATERPIE | Direct acquisition | Random wild in 2 catchable method pool(s): Berry tree encounter table 1: G Berry Stage Sprouted / Berry Tree; Petalburg Woods 2 / Honey | party-selectable species/form |
| 11 | Metapod | SPECIES_METAPOD | Direct acquisition | Random wild in 2 catchable method pool(s): Berry tree encounter table 2: G Berry Stage Taller / Berry Tree; Petalburg Woods 2 / Land | party-selectable species/form |
| 12 | Butterfree | SPECIES_BUTTERFREE | Direct acquisition | Random wild in 1 catchable method pool(s): Dewford Meadow / Land | party-selectable species/form |
| 13 | Weedle | SPECIES_WEEDLE | Direct acquisition | Random wild in 2 catchable method pool(s): Berry tree encounter table 1: G Berry Stage Sprouted / Berry Tree; Petalburg Woods 2 / Honey | party-selectable species/form |
| 14 | Kakuna | SPECIES_KAKUNA | Direct acquisition | Random wild in 2 catchable method pool(s): Berry tree encounter table 2: G Berry Stage Taller / Berry Tree; Petalburg Woods 2 / Land | party-selectable species/form |
| 15 | Beedrill | SPECIES_BEEDRILL | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods 3 / Rock Smash | party-selectable species/form |
| 16 | Pidgey | SPECIES_PIDGEY | Direct acquisition | Random wild in 2 catchable method pool(s): Petalburg Woods / Honey; Petalburg Woods / Land | party-selectable species/form |
| 17 | Pidgeotto | SPECIES_PIDGEOTTO | Evolution from obtainable Pokémon | Pidgey → Pidgeotto via Level (18) | party-selectable species/form |
| 18 | Pidgeot | SPECIES_PIDGEOT | Evolution from obtainable Pokémon | Pidgey → Pidgeotto via Level (18) ; Pidgeotto → Pidgeot via Level (36) | party-selectable species/form |
| 19 | Rattata | SPECIES_RATTATA | Direct acquisition | Random wild in 1 catchable method pool(s): Dewford Manor 1F / Land | party-selectable species/form |
| 20 | Raticate | SPECIES_RATICATE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 118 / Land | party-selectable species/form |
| 21 | Spearow | SPECIES_SPEAROW | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Fearow; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 22 | Fearow | SPECIES_FEAROW | Direct acquisition | Random wild in 2 catchable method pool(s): Jagged Pass / Land; Route 118 / Rock Smash | party-selectable species/form |
| 23 | Ekans | SPECIES_EKANS | Direct acquisition | Random wild in 1 catchable method pool(s): Route 110 / Honey | party-selectable species/form |
| 24 | Arbok | SPECIES_ARBOK | Direct acquisition | Random wild in 3 catchable method pool(s): Route 121 / Land; Route 121 / Rock Smash; Safari Zone Southwest / Land | party-selectable species/form |
| 25 | Pikachu | SPECIES_PIKACHU | Direct acquisition | Random wild in 2 catchable method pool(s): Petalburg Woods / Honey; Safari Zone South / Land | party-selectable species/form |
| 26 | Raichu | SPECIES_RAICHU | Evolution from obtainable Pokémon | Pikachu → Raichu via Use/hold ITEM_THUNDER_STONE | party-selectable species/form |
| 27 | Sandshrew | SPECIES_SANDSHREW | Direct acquisition | Random wild in 6 catchable method pool(s): Mirage Tower 1F / Land; Mirage Tower 2F / Land; Mirage Tower 3F / Land; Mirage Tower 4F / Land; Mirage Tower B1F / Land; Route 111 / Land | party-selectable species/form |
| 28 | Sandslash | SPECIES_SANDSLASH | Evolution from obtainable Pokémon | Sandshrew → Sandslash via Level (22) | party-selectable species/form |
| 29 | Nidoran♀ | SPECIES_NIDORAN_F | Direct acquisition | Random wild in 1 catchable method pool(s): Rustboro City / Land | party-selectable species/form |
| 30 | Nidorina | SPECIES_NIDORINA | Direct acquisition | Random wild in 2 catchable method pool(s): Rustboro City / Honey; Safari Zone Northeast / Land | party-selectable species/form |
| 31 | Nidoqueen | SPECIES_NIDOQUEEN | Evolution from obtainable Pokémon | Nidorina → Nidoqueen via Use/hold ITEM_MOON_STONE | party-selectable species/form |
| 32 | Nidoran♂ | SPECIES_NIDORAN_M | Direct acquisition | Random wild in 1 catchable method pool(s): Rustboro City / Land | party-selectable species/form |
| 33 | Nidorino | SPECIES_NIDORINO | Direct acquisition | Random wild in 2 catchable method pool(s): Rustboro City / Honey; Safari Zone Northeast / Land | party-selectable species/form |
| 34 | Nidoking | SPECIES_NIDOKING | Evolution from obtainable Pokémon | Nidorino → Nidoking via Use/hold ITEM_MOON_STONE | party-selectable species/form |
| 35 | Clefairy | SPECIES_CLEFAIRY | Direct acquisition | Random wild in 3 catchable method pool(s): Meteor Falls 1F 1R / Land; Meteor Falls 1F 2R / Land; Meteor Falls B1F 1R / Land | party-selectable species/form |
| 36 | Clefable | SPECIES_CLEFABLE | Evolution from obtainable Pokémon | Clefairy → Clefable via Use/hold ITEM_MOON_STONE | party-selectable species/form |
| 37 | Vulpix | SPECIES_VULPIX | Direct acquisition | Random wild in 2 catchable method pool(s): Mt Pyre Exterior / Land; Mt Pyre Summit / Land | party-selectable species/form |
| 38 | Ninetales | SPECIES_NINETALES | Direct acquisition | Random wild in 1 catchable method pool(s): Mt Pyre Exterior / Honey | party-selectable species/form |
| 39 | Jigglypuff | SPECIES_JIGGLYPUFF | Direct acquisition | Random wild in 1 catchable method pool(s): Route 115 / Land | party-selectable species/form |
| 40 | Wigglytuff | SPECIES_WIGGLYTUFF | Evolution from obtainable Pokémon | Jigglypuff → Wigglytuff via Use/hold ITEM_MOON_STONE | party-selectable species/form |
| 41 | Zubat | SPECIES_ZUBAT | Direct acquisition | Random wild in 3 catchable method pool(s): Granite Cave 1F / Land; Granite Cave Stevens Room / Land; Seaspray Cave / Land | party-selectable species/form |
| 42 | Golbat | SPECIES_GOLBAT | Direct acquisition | Random wild in 40 catchable method pool(s): Cave Of Origin Unused Ruby Sapphire Map1 / Land; Cave Of Origin Unused Ruby Sapphire Map2 / Land; Cave Of Origin Unused Ruby Sapphire Map3 / Land; Ember Path / Land; Meteor Falls 1F 1R / Surf; Meteor Falls 1F 2R / Land; Meteor Falls 1F 2R / Surf; Meteor Falls B1F 1R / Land; Meteor Falls B1F 1R / Surf; Meteor Falls B1F 2R / Land; Meteor Falls B1F 2R / Surf; Mt Pyre 1F / Land; Mt Pyre 2F / Land; Mt Pyre 3F / Land; Mt Pyre 4F / Land; Mt Pyre 5F / Land; Mt Pyre 6F / Land; Scorched Slab / Land; Scorched Slab / Surf; Scorched Slab B1F / Land; Scorched Slab B1F / Surf; Scorched Slab B2F / Land; Scorched Slab Heatrans Room / Land; Seafloor Cavern Room1 / Land; Seafloor Cavern Room2 / Land; Seafloor Cavern Room3 / Land; Seafloor Cavern Room4 / Land; Seafloor Cavern Room5 / Land; Seafloor Cavern Room6 / Land; Seafloor Cavern Room7 / Land; Seafloor Cavern Room8 / Land; Seaspray Cave / Surf; Shoal Cave Low Tide Entrance Room / Land; Shoal Cave Low Tide Entrance Room / Surf; Shoal Cave Low Tide Inner Room / Land; Shoal Cave Low Tide Inner Room / Surf; Shoal Cave Low Tide Lower Room / Land; Shoal Cave Low Tide Stairs Room / Land; Victory Road 1F / Land; Victory Road B2F / Surf | party-selectable species/form |
| 43 | Oddish | SPECIES_ODDISH | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods 3 / Land | party-selectable species/form |
| 44 | Gloom | SPECIES_GLOOM | Direct acquisition | Random wild in 2 catchable method pool(s): Route 123 / Land; Safari Zone North / Land | party-selectable species/form |
| 45 | Vileplume | SPECIES_VILEPLUME | Evolution from obtainable Pokémon | Gloom → Vileplume via Use/hold ITEM_LEAF_STONE | party-selectable species/form |
| 46 | Paras | SPECIES_PARAS | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods / Land | party-selectable species/form |
| 47 | Parasect | SPECIES_PARASECT | Direct acquisition | Random wild in 2 catchable method pool(s): Altering Cave / Land; Altering Cave B1F / Land | party-selectable species/form |
| 48 | Venonat | SPECIES_VENONAT | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods 2 / Rock Smash | party-selectable species/form |
| 49 | Venomoth | SPECIES_VENOMOTH | Direct acquisition | Random wild in 2 catchable method pool(s): Route 118 / Rock Smash; Route 120 / Land | party-selectable species/form |
| 50 | Diglett | SPECIES_DIGLETT | Direct acquisition | Random wild in 1 catchable method pool(s): Fiery Path / Land | party-selectable species/form |
| 51 | Dugtrio | SPECIES_DUGTRIO | Direct acquisition | Random wild in 4 catchable method pool(s): Altering Cave 1F / Land; Scorched Slab B1F / Land; Scorched Slab B2F / Land; Scorched Slab Heatrans Room / Land | party-selectable species/form |
| 52 | Meowth | SPECIES_MEOWTH | Direct acquisition | Random wild in 1 catchable method pool(s): Route 117 / Land | party-selectable species/form |
| 53 | Persian | SPECIES_PERSIAN | Evolution from obtainable Pokémon | Meowth → Persian via Level (28) | party-selectable species/form |
| 54 | Psyduck | SPECIES_PSYDUCK | Direct acquisition | Random wild in 1 catchable method pool(s): Seaspray Cave / Land | party-selectable species/form |
| 55 | Golduck | SPECIES_GOLDUCK | Evolution from obtainable Pokémon | Psyduck → Golduck via Level (33) | party-selectable species/form |
| 56 | Mankey | SPECIES_MANKEY | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Annihilape; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 57 | Primeape | SPECIES_PRIMEAPE | Direct acquisition | Random wild in 1 catchable method pool(s): Jagged Pass / Land | party-selectable species/form |
| 58 | Growlithe | SPECIES_GROWLITHE | Direct acquisition | Random wild in 3 catchable method pool(s): Ashen Woods / Land; Mt Pyre Exterior / Land; Mt Pyre Summit / Land | party-selectable species/form |
| 59 | Arcanine | SPECIES_ARCANINE | Direct acquisition | Random wild in 1 catchable method pool(s): Mt Pyre Exterior / Honey | party-selectable species/form |
| 60 | Poliwag | SPECIES_POLIWAG | Direct acquisition | Random wild in 3 catchable method pool(s): Petalburg Woods 3 / Good Rod; Petalburg Woods 3 / Old Rod; Petalburg Woods 3 / Surf | party-selectable species/form |
| 61 | Poliwhirl | SPECIES_POLIWHIRL | Direct acquisition | Random wild in 3 catchable method pool(s): Petalburg Woods 3 / Good Rod; Petalburg Woods 3 / Super Rod; Petalburg Woods 3 / Surf | party-selectable species/form |
| 62 | Poliwrath | SPECIES_POLIWRATH | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods 3 / Super Rod | party-selectable species/form |
| 63 | Abra | SPECIES_ABRA | Direct acquisition | Random wild in 2 catchable method pool(s): Granite Cave 1F / Land; Granite Cave Stevens Room / Land | party-selectable species/form |
| 64 | Kadabra | SPECIES_KADABRA | Evolution from obtainable Pokémon | Abra → Kadabra via Level (16) | party-selectable species/form |
| 65 | Alakazam | SPECIES_ALAKAZAM | Evolution from obtainable Pokémon | Abra → Kadabra via Level (16) ; Kadabra → Alakazam via Friendship | party-selectable species/form |
| 66 | Machop | SPECIES_MACHOP | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Machamp; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 67 | Machoke | SPECIES_MACHOKE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 112 / Land | party-selectable species/form |
| 68 | Machamp | SPECIES_MACHAMP | Evolution from obtainable Pokémon | Machoke → Machamp via Friendship | party-selectable species/form |
| 69 | Bellsprout | SPECIES_BELLSPROUT | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods 3 / Land | party-selectable species/form |
| 70 | Weepinbell | SPECIES_WEEPINBELL | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone North / Land | party-selectable species/form |
| 71 | Victreebel | SPECIES_VICTREEBEL | Evolution from obtainable Pokémon | Weepinbell → Victreebel via Use/hold ITEM_LEAF_STONE | party-selectable species/form |
| 72 | Tentacool | SPECIES_TENTACOOL | Direct acquisition | Random wild in 24 catchable method pool(s): Dewford Town / Surf; Route 103 / Good Rod; Route 103 / Old Rod; Route 105 / Surf; Route 106 / Surf; Route 118 / Surf; Route 129 / Good Rod; Route 130 / Good Rod; Route 131 / Good Rod; Route 132 / Old Rod; Route 133 / Old Rod; Route 134 / Old Rod; Seafloor Cavern Entrance / Good Rod; Seafloor Cavern Entrance / Old Rod; Seafloor Cavern Room6 / Good Rod; Seafloor Cavern Room6 / Old Rod; Seafloor Cavern Room7 / Good Rod; Seafloor Cavern Room7 / Old Rod; Shoal Cave Low Tide Entrance Room / Good Rod; Shoal Cave Low Tide Entrance Room / Old Rod; Shoal Cave Low Tide Inner Room / Good Rod; Shoal Cave Low Tide Inner Room / Old Rod; Slateport City / Surf; Sootopolis City / Old Rod | party-selectable species/form |
| 73 | Tentacruel | SPECIES_TENTACRUEL | Direct acquisition | Random wild in 11 catchable method pool(s): Ever Grande City / Surf; Pacifidlog Town / Surf; Route 115 / Surf; Route 129 / Super Rod; Route 129 / Surf; Route 130 / Super Rod; Route 130 / Surf; Route 131 / Super Rod; Route 131 / Surf; Shoal Cave Low Tide Entrance Room / Super Rod; Shoal Cave Low Tide Inner Room / Super Rod | party-selectable species/form |
| 74 | Geodude | SPECIES_GEODUDE | Direct acquisition | Random wild in 3 catchable method pool(s): Granite Cave 1F / Land; Route 111 / Rock Smash; Route 114 / Rock Smash | party-selectable species/form |
| 75 | Graveler | SPECIES_GRAVELER | Direct acquisition | Random wild in 11 catchable method pool(s): Magma Hideout 1F / Land; Magma Hideout 2F 1R / Land; Magma Hideout 2F 2R / Land; Magma Hideout 2F 3R / Land; Magma Hideout 3F 1R / Land; Magma Hideout 3F 2R / Land; Magma Hideout 3F 3R / Land; Magma Hideout 4F / Land; Route 111 / Rock Smash; Route 114 / Rock Smash; Victory Road B1F / Rock Smash | party-selectable species/form |
| 76 | Golem | SPECIES_GOLEM | Direct acquisition | Random wild in 1 catchable method pool(s): Victory Road B1F / Rock Smash | party-selectable species/form |
| 77 | Ponyta | SPECIES_PONYTA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 112 / Land | party-selectable species/form |
| 78 | Rapidash | SPECIES_RAPIDASH | Evolution from obtainable Pokémon | Ponyta → Rapidash via Level (40) | party-selectable species/form |
| 79 | Slowpoke | SPECIES_SLOWPOKE | Direct acquisition | Random wild in 8 catchable method pool(s): Petalburg Woods 3 / Good Rod; Petalburg Woods 3 / Old Rod; Petalburg Woods 3 / Super Rod; Petalburg Woods 3 / Surf; Shoal Cave Low Tide Entrance Room / Good Rod; Shoal Cave Low Tide Entrance Room / Super Rod; Shoal Cave Low Tide Inner Room / Good Rod; Shoal Cave Low Tide Inner Room / Super Rod | party-selectable species/form |
| 80 | Slowbro | SPECIES_SLOWBRO | Direct acquisition | Random wild in 3 catchable method pool(s): Petalburg Woods 3 / Super Rod; Shoal Cave Low Tide Entrance Room / Super Rod; Shoal Cave Low Tide Inner Room / Super Rod | party-selectable species/form |
| 81 | Magnemite | SPECIES_MAGNEMITE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 110 / Land | party-selectable species/form |
| 82 | Magneton | SPECIES_MAGNETON | Direct acquisition | Random wild in 2 catchable method pool(s): New Mauville Entrance / Land; New Mauville Inside / Land | party-selectable species/form |
| 83 | Farfetch'd | SPECIES_FARFETCHD | Direct acquisition | Random wild in 1 catchable method pool(s): Route 117 / Land | party-selectable species/form |
| 84 | Doduo | SPECIES_DODUO | Direct acquisition | Random wild in 1 catchable method pool(s): Route 110 / Honey | party-selectable species/form |
| 85 | Dodrio | SPECIES_DODRIO | Direct acquisition | Random wild in 2 catchable method pool(s): Route 115 / Land; Safari Zone Northwest / Land | party-selectable species/form |
| 86 | Seel | SPECIES_SEEL | Direct acquisition | Random wild in 1 catchable method pool(s): Seaspray Cave B1F / Land | party-selectable species/form |
| 87 | Dewgong | SPECIES_DEWGONG | Direct acquisition | Random wild in 5 catchable method pool(s): Shoal Cave Low Tide Entrance Room / Land; Shoal Cave Low Tide Inner Room / Land; Shoal Cave Low Tide Lower Room / Land; Shoal Cave Low Tide Stairs Room / Land; Victory Road B2F / Surf | party-selectable species/form |
| 88 | Grimer | SPECIES_GRIMER | Direct acquisition | Random wild in 2 catchable method pool(s): Fiery Path / Land; Safari Zone Northwest / Old Rod | party-selectable species/form |
| 89 | Muk | SPECIES_MUK | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone Northwest / Super Rod | party-selectable species/form |
| 90 | Shellder | SPECIES_SHELLDER | Direct acquisition | Random wild in 3 catchable method pool(s): Dewford Town / Good Rod; Dewford Town / Old Rod; Dewford Town / Super Rod | party-selectable species/form |
| 91 | Cloyster | SPECIES_CLOYSTER | Direct acquisition | Random wild in 1 catchable method pool(s): Dewford Town / Super Rod | party-selectable species/form |
| 92 | Gastly | SPECIES_GASTLY | Direct acquisition | Random wild in 1 catchable method pool(s): Dewford Manor 1F / Land | party-selectable species/form |
| 93 | Haunter | SPECIES_HAUNTER | Direct acquisition | Random wild in 6 catchable method pool(s): Mt Pyre 1F / Land; Mt Pyre 2F / Land; Mt Pyre 3F / Land; Mt Pyre 4F / Land; Mt Pyre 5F / Land; Mt Pyre 6F / Land | party-selectable species/form |
| 94 | Gengar | SPECIES_GENGAR | Evolution from obtainable Pokémon | Haunter → Gengar via Friendship | party-selectable species/form |
| 95 | Onix | SPECIES_ONIX | Direct acquisition | Random wild in 6 catchable method pool(s): Granite Cave B1F / Land; Granite Cave B2F / Land; Sandstrewn Ruins / Rock Smash; Sandstrewn Ruins 2F / Rock Smash; Sandstrewn Ruins 3F / Rock Smash; Sandstrewn Ruins B1F / Rock Smash | party-selectable species/form |
| 96 | Drowzee | SPECIES_DROWZEE | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Hypno; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 97 | Hypno | SPECIES_HYPNO | Direct acquisition | Random wild in 1 catchable method pool(s): Route 121 / Land | party-selectable species/form |
| 98 | Krabby | SPECIES_KRABBY | Direct acquisition | Random wild in 2 catchable method pool(s): Seaspray Cave / Good Rod; Seaspray Cave / Old Rod | party-selectable species/form |
| 99 | Kingler | SPECIES_KINGLER | Direct acquisition | Random wild in 1 catchable method pool(s): Seaspray Cave / Super Rod | party-selectable species/form |
| 100 | Voltorb | SPECIES_VOLTORB | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Electrode; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 101 | Electrode | SPECIES_ELECTRODE | Direct acquisition | Random wild in 1 catchable method pool(s): New Mauville Entrance / Land \| Scripted/static encounter: Aqua Hideout B1F (Literal setwildbattle acquisition) \| Scripted/static encounter: New Mauville Inside (Literal setwildbattle acquisition) | party-selectable species/form |
| 102 | Exeggcute | SPECIES_EXEGGCUTE | Direct acquisition | Random wild in 3 catchable method pool(s): Petalburg Woods 2 / Rock Smash; Route 106 / Land; Route 123 / Rock Smash | party-selectable species/form |
| 103 | Exeggutor | SPECIES_EXEGGUTOR | Direct acquisition | Random wild in 1 catchable method pool(s): Route 105 / Land | party-selectable species/form |
| 104 | Cubone | SPECIES_CUBONE | Direct acquisition | Random wild in 2 catchable method pool(s): Granite Cave B1F / Land; Granite Cave B2F / Land | party-selectable species/form |
| 105 | Marowak | SPECIES_MAROWAK | Evolution from obtainable Pokémon | Cubone → Marowak via Level (28) | party-selectable species/form |
| 106 | Hitmonlee | SPECIES_HITMONLEE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 112 / Honey | party-selectable species/form |
| 107 | Hitmonchan | SPECIES_HITMONCHAN | Direct acquisition | Random wild in 1 catchable method pool(s): Route 112 / Honey | party-selectable species/form |
| 108 | Lickitung | SPECIES_LICKITUNG | Direct acquisition | Random wild in 1 catchable method pool(s): Route 118 / Land | party-selectable species/form |
| 109 | Koffing | SPECIES_KOFFING | Direct acquisition | Random wild in 1 catchable method pool(s): Fiery Path / Land | party-selectable species/form |
| 110 | Weezing | SPECIES_WEEZING | Direct acquisition | Random wild in 8 catchable method pool(s): Magma Hideout 1F / Land; Magma Hideout 2F 1R / Land; Magma Hideout 2F 2R / Land; Magma Hideout 2F 3R / Land; Magma Hideout 3F 1R / Land; Magma Hideout 3F 2R / Land; Magma Hideout 3F 3R / Land; Magma Hideout 4F / Land | party-selectable species/form |
| 111 | Rhyhorn | SPECIES_RHYHORN | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone Northwest / Land | party-selectable species/form |
| 112 | Rhydon | SPECIES_RHYDON | Direct acquisition | Random wild in 4 catchable method pool(s): Altering Cave / Land; Altering Cave B1F / Land; Safari Zone Northwest / Land; Victory Road B1F / Land | party-selectable species/form |
| 113 | Chansey | SPECIES_CHANSEY | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone Northwest / Land | party-selectable species/form |
| 114 | Tangela | SPECIES_TANGELA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 115 / Land | party-selectable species/form |
| 115 | Kangaskhan | SPECIES_KANGASKHAN | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone North / Land | party-selectable species/form |
| 116 | Horsea | SPECIES_HORSEA | Direct acquisition | Random wild in 8 catchable method pool(s): Route 132 / Good Rod; Route 132 / Super Rod; Route 133 / Good Rod; Route 133 / Super Rod; Route 134 / Good Rod; Route 134 / Super Rod; Slateport City / Good Rod; Slateport City / Old Rod | party-selectable species/form |
| 117 | Seadra | SPECIES_SEADRA | Direct acquisition | Random wild in 4 catchable method pool(s): Route 132 / Super Rod; Route 133 / Super Rod; Route 134 / Super Rod; Slateport City / Super Rod | party-selectable species/form |
| 118 | Goldeen | SPECIES_GOLDEEN | Direct acquisition | Random wild in 15 catchable method pool(s): Petalburg City / Good Rod; Petalburg City / Old Rod; Route 102 / Old Rod; Route 111 / Good Rod; Route 111 / Old Rod; Route 117 / Good Rod; Route 117 / Old Rod; Route 123 / Good Rod; Route 123 / Old Rod; Safari Zone Southwest / Good Rod; Safari Zone Southwest / Old Rod; Scorched Slab / Good Rod; Scorched Slab / Old Rod; Scorched Slab B1F / Good Rod; Scorched Slab B1F / Old Rod | party-selectable species/form |
| 119 | Seaking | SPECIES_SEAKING | Direct acquisition | Random wild in 15 catchable method pool(s): Altering Cave / Super Rod; Altering Cave / Surf; Altering Cave B1F / Super Rod; Altering Cave B1F / Surf; Route 102 / Good Rod; Route 102 / Super Rod; Route 111 / Super Rod; Route 114 / Super Rod; Route 117 / Super Rod; Route 117 / Surf; Route 123 / Super Rod; Safari Zone Southwest / Good Rod; Safari Zone Southwest / Super Rod; Scorched Slab / Super Rod; Scorched Slab B1F / Super Rod | party-selectable species/form |
| 120 | Staryu | SPECIES_STARYU | Direct acquisition | Random wild in 4 catchable method pool(s): Dewford Town / Good Rod; Dewford Town / Old Rod; Dewford Town / Super Rod; Lilycove City / Good Rod | party-selectable species/form |
| 121 | Starmie | SPECIES_STARMIE | Direct acquisition | Random wild in 4 catchable method pool(s): Dewford Town / Super Rod; Lilycove City / Super Rod; Underwater Route 124 / Land; Underwater Route 126 / Land | party-selectable species/form |
| 122 | Mr. Mime | SPECIES_MR_MIME | Evolution from obtainable Pokémon | Mime Jr. → Mr. Mime via Level while knowing MOVE_MIMIC | party-selectable species/form |
| 123 | Scyther | SPECIES_SCYTHER | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods / Land | party-selectable species/form |
| 124 | Jynx | SPECIES_JYNX | Direct acquisition | Random wild in 1 catchable method pool(s): Shoal Cave Low Tide Ice Room / Land | party-selectable species/form |
| 125 | Electabuzz | SPECIES_ELECTABUZZ | Direct acquisition | Random wild in 2 catchable method pool(s): New Mauville Entrance / Land; New Mauville Inside / Land | party-selectable species/form |
| 126 | Magmar | SPECIES_MAGMAR | Direct acquisition | Random wild in 3 catchable method pool(s): Ember Path / Land; Scorched Slab B2F / Land; Scorched Slab Heatrans Room / Land | party-selectable species/form |
| 127 | Pinsir | SPECIES_PINSIR | Direct acquisition | Random wild in 3 catchable method pool(s): Ashen Woods / Land; Route 120 / Rock Smash; Safari Zone Northwest / Land | party-selectable species/form |
| 128 | Tauros | SPECIES_TAUROS | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone Northeast / Land | party-selectable species/form |
| 129 | Magikarp | SPECIES_MAGIKARP | Direct acquisition | Random wild in 39 catchable method pool(s): Altering Cave / Old Rod; Altering Cave B1F / Old Rod; Lilycove City / Old Rod; Mossdeep City / Old Rod; Pacifidlog Town / Old Rod; Route 103 / Old Rod; Route 104 / Good Rod; Route 118 / Old Rod; Route 119 / Good Rod; Route 119 / Old Rod; Route 120 / Good Rod; Route 120 / Old Rod; Route 121 / Old Rod; Route 122 / Old Rod; Route 124 / Old Rod; Route 125 / Old Rod; Route 126 / Old Rod; Route 127 / Old Rod; Route 128 / Old Rod; Route 129 / Old Rod; Route 130 / Old Rod; Route 131 / Old Rod; Route 132 / Good Rod; Route 132 / Old Rod; Route 133 / Good Rod; Route 133 / Old Rod; Route 134 / Good Rod; Route 134 / Old Rod; Safari Zone Southwest / Old Rod; Seafloor Cavern Entrance / Old Rod; Seafloor Cavern Room6 / Old Rod; Seafloor Cavern Room7 / Old Rod; Shoal Cave Low Tide Entrance Room / Old Rod; Shoal Cave Low Tide Inner Room / Old Rod; Sootopolis City / Good Rod; Sootopolis City / Old Rod; Sootopolis City / Super Rod; Sootopolis City / Surf; Victory Road B2F / Old Rod | party-selectable species/form |
| 130 | Gyarados | SPECIES_GYARADOS | Direct acquisition | Random wild in 15 catchable method pool(s): Altering Cave / Good Rod; Altering Cave / Super Rod; Altering Cave B1F / Good Rod; Altering Cave B1F / Super Rod; Route 103 / Good Rod; Route 103 / Super Rod; Route 104 / Super Rod; Route 107 / Super Rod; Route 108 / Super Rod; Route 118 / Super Rod; Route 119 / Super Rod; Route 120 / Super Rod; Route 121 / Super Rod; Route 122 / Super Rod; Sootopolis City / Super Rod | party-selectable species/form |
| 131 | Lapras | SPECIES_LAPRAS | Direct acquisition | Random wild in 7 catchable method pool(s): Route 125 / Surf; Seafloor Cavern Entrance / Surf; Seafloor Cavern Room6 / Surf; Seafloor Cavern Room7 / Surf; Shoal Cave Low Tide Entrance Room / Surf; Shoal Cave Low Tide Inner Room / Surf; Victory Road B2F / Surf | party-selectable species/form |
| 132 | Ditto | SPECIES_DITTO | Direct acquisition | Random wild in 1 catchable method pool(s): Desert Underpass / Land | party-selectable species/form |
| 133 | Eevee | SPECIES_EEVEE | Direct acquisition | Random wild in 1 catchable method pool(s): Rustboro City / Land | party-selectable species/form |
| 134 | Vaporeon | SPECIES_VAPOREON | Evolution from obtainable Pokémon | Eevee → Vaporeon via Use/hold ITEM_WATER_STONE | party-selectable species/form |
| 135 | Jolteon | SPECIES_JOLTEON | Evolution from obtainable Pokémon | Eevee → Jolteon via Use/hold ITEM_THUNDER_STONE | party-selectable species/form |
| 136 | Flareon | SPECIES_FLAREON | Evolution from obtainable Pokémon | Eevee → Flareon via Use/hold ITEM_FIRE_STONE | party-selectable species/form |
| 137 | Porygon | SPECIES_PORYGON | Direct acquisition | Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 138 | Omanyte | SPECIES_OMANYTE | Direct acquisition | Fossil restoration: Rustboro Devon Corporation (Restore ITEM_HELIX_FOSSIL) | party-selectable species/form |
| 139 | Omastar | SPECIES_OMASTAR | Evolution from obtainable Pokémon | Omanyte → Omastar via Level (40) | party-selectable species/form |
| 140 | Kabuto | SPECIES_KABUTO | Direct acquisition | Fossil restoration: Rustboro Devon Corporation (Restore ITEM_DOME_FOSSIL) | party-selectable species/form |
| 141 | Kabutops | SPECIES_KABUTOPS | Evolution from obtainable Pokémon | Kabuto → Kabutops via Level (40) | party-selectable species/form |
| 142 | Aerodactyl | SPECIES_AERODACTYL | Direct acquisition | Fossil restoration: Rustboro Devon Corporation (Restore ITEM_OLD_AMBER) | party-selectable species/form |
| 143 | Snorlax | SPECIES_SNORLAX | Evolution from obtainable Pokémon | Munchlax → Snorlax via Friendship | party-selectable species/form |
| 144 | Articuno | SPECIES_ARTICUNO | Direct acquisition | Scripted/static encounter: Shoal Cave Low Tide Ice Room (Literal setwildbattle acquisition) \| Authored static acquisition: Shoal Cave Low Tide Ice Room (articuno) | party-selectable species/form |
| 145 | Zapdos | SPECIES_ZAPDOS | Direct acquisition | Scripted/static encounter: New Mauville Inside (Literal setwildbattle acquisition) \| Authored static acquisition: New Mauville Inside (zapdos) | party-selectable species/form |
| 146 | Moltres | SPECIES_MOLTRES | Direct acquisition | Scripted/static encounter: Ember Path (Literal setwildbattle acquisition) \| Authored static acquisition: Ember Path (moltres) | party-selectable species/form |
| 147 | Dratini | SPECIES_DRATINI | Direct acquisition | Random wild in 19 catchable method pool(s): Meteor Falls 1F 1R / Good Rod; Meteor Falls 1F 1R / Old Rod; Meteor Falls 1F 1R / Super Rod; Meteor Falls 1F 1R / Surf; Meteor Falls 1F 2R / Good Rod; Meteor Falls 1F 2R / Old Rod; Meteor Falls 1F 2R / Super Rod; Meteor Falls 1F 2R / Surf; Meteor Falls B1F 1R / Good Rod; Meteor Falls B1F 1R / Old Rod; Meteor Falls B1F 1R / Super Rod; Meteor Falls B1F 1R / Surf; Meteor Falls B1F 2R / Good Rod; Meteor Falls B1F 2R / Old Rod; Meteor Falls B1F 2R / Super Rod; Meteor Falls B1F 2R / Surf; Safari Zone Southeast / Good Rod; Safari Zone Southeast / Old Rod; Safari Zone Southeast / Super Rod | party-selectable species/form |
| 148 | Dragonair | SPECIES_DRAGONAIR | Direct acquisition | Random wild in 10 catchable method pool(s): Meteor Falls 1F 1R / Good Rod; Meteor Falls 1F 1R / Super Rod; Meteor Falls 1F 2R / Good Rod; Meteor Falls 1F 2R / Super Rod; Meteor Falls B1F 1R / Good Rod; Meteor Falls B1F 1R / Super Rod; Meteor Falls B1F 2R / Good Rod; Meteor Falls B1F 2R / Super Rod; Safari Zone Southeast / Good Rod; Safari Zone Southeast / Super Rod | party-selectable species/form |
| 149 | Dragonite | SPECIES_DRAGONITE | Direct acquisition | Random wild in 5 catchable method pool(s): Meteor Falls 1F 1R / Super Rod; Meteor Falls 1F 2R / Super Rod; Meteor Falls B1F 1R / Super Rod; Meteor Falls B1F 2R / Super Rod; Safari Zone Southeast / Super Rod | party-selectable species/form |
| 150 | Mewtwo | SPECIES_MEWTWO | Direct acquisition | Scripted/static encounter: Altering Cave B1F (Literal setwildbattle acquisition) \| Authored static acquisition: Altering Cave B1F (mewtwo) | party-selectable species/form |
| 151 | Mew | SPECIES_MEW | Direct acquisition | Authored static acquisition: Faraway Island Interior (mew) | party-selectable species/form |
| 152 | Chikorita | SPECIES_CHIKORITA | Direct acquisition | NPC or story gift: Littleroot Town Professor Birchs Lab (Literal givemon acquisition) \| Starter choice: Littleroot opening (Selectable Johto starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 153 | Bayleef | SPECIES_BAYLEEF | Evolution from obtainable Pokémon | Chikorita → Bayleef via Level (16) | party-selectable species/form |
| 154 | Meganium | SPECIES_MEGANIUM | Evolution from obtainable Pokémon | Chikorita → Bayleef via Level (16) ; Bayleef → Meganium via Level (32) | party-selectable species/form |
| 155 | Cyndaquil | SPECIES_CYNDAQUIL | Direct acquisition | NPC or story gift: Littleroot Town Professor Birchs Lab (Literal givemon acquisition) \| Starter choice: Littleroot opening (Selectable Johto starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 156 | Quilava | SPECIES_QUILAVA | Evolution from obtainable Pokémon | Cyndaquil → Quilava via Level (14) | party-selectable species/form |
| 157 | Typhlosion | SPECIES_TYPHLOSION | Evolution from obtainable Pokémon | Cyndaquil → Quilava via Level (14) ; Quilava → Typhlosion via Level (36) | party-selectable species/form |
| 158 | Totodile | SPECIES_TOTODILE | Direct acquisition | NPC or story gift: Littleroot Town Professor Birchs Lab (Literal givemon acquisition) \| Starter choice: Littleroot opening (Selectable Johto starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 159 | Croconaw | SPECIES_CROCONAW | Evolution from obtainable Pokémon | Totodile → Croconaw via Level (16) | party-selectable species/form |
| 160 | Feraligatr | SPECIES_FERALIGATR | Evolution from obtainable Pokémon | Totodile → Croconaw via Level (16) ; Croconaw → Feraligatr via Level (30) | party-selectable species/form |
| 161 | Sentret | SPECIES_SENTRET | Direct acquisition | Random wild in 1 catchable method pool(s): Route 104 / Land | party-selectable species/form |
| 162 | Furret | SPECIES_FURRET | Evolution from obtainable Pokémon | Sentret → Furret via Level (15) | party-selectable species/form |
| 163 | Hoothoot | SPECIES_HOOTHOOT | Direct acquisition | Random wild in 2 catchable method pool(s): Dewford Manor 1F / Land; Route 103 / Rock Smash | party-selectable species/form |
| 164 | Noctowl | SPECIES_NOCTOWL | Direct acquisition | Random wild in 2 catchable method pool(s): Ashen Woods / Land; Route 123 / Rock Smash | party-selectable species/form |
| 165 | Ledyba | SPECIES_LEDYBA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 104 / Land | party-selectable species/form |
| 166 | Ledian | SPECIES_LEDIAN | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone Northeast / Land | party-selectable species/form |
| 167 | Spinarak | SPECIES_SPINARAK | Direct acquisition | Random wild in 1 catchable method pool(s): Route 116 / Rock Smash | party-selectable species/form |
| 168 | Ariados | SPECIES_ARIADOS | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone Southeast / Land | party-selectable species/form |
| 169 | Crobat | SPECIES_CROBAT | Direct acquisition | Random wild in 12 catchable method pool(s): Meteor Falls Stevens Cave / Land; Scorched Slab / Land; Scorched Slab / Surf; Scorched Slab B1F / Surf; Seafloor Cavern Room1 / Land; Seafloor Cavern Room2 / Land; Seafloor Cavern Room3 / Land; Seafloor Cavern Room4 / Land; Seafloor Cavern Room5 / Land; Seafloor Cavern Room6 / Land; Seafloor Cavern Room7 / Land; Seafloor Cavern Room8 / Land | party-selectable species/form |
| 170 | Chinchou | SPECIES_CHINCHOU | Direct acquisition | Random wild in 5 catchable method pool(s): Route 110 / Good Rod; Route 110 / Old Rod; Route 110 / Super Rod; Underwater Route 124 / Land; Underwater Route 126 / Land | party-selectable species/form |
| 171 | Lanturn | SPECIES_LANTURN | Direct acquisition | Random wild in 3 catchable method pool(s): Route 110 / Super Rod; Underwater Route 124 / Land; Underwater Route 126 / Land | party-selectable species/form |
| 172 | Pichu | SPECIES_PICHU | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods / Land \| Gift Egg: Global event system (Literal giveegg acquisition) | party-selectable species/form |
| 173 | Cleffa | SPECIES_CLEFFA | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Clefable; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 174 | Igglybuff | SPECIES_IGGLYBUFF | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Jigglypuff; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 175 | Togepi | SPECIES_TOGEPI | Direct acquisition | Authored gift/restoration: Route 117 Pokemon Day Care (togepi) | party-selectable species/form |
| 176 | Togetic | SPECIES_TOGETIC | Evolution from obtainable Pokémon | Togepi → Togetic via Friendship | party-selectable species/form |
| 177 | Natu | SPECIES_NATU | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Xatu; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 178 | Xatu | SPECIES_XATU | Direct acquisition | Random wild in 2 catchable method pool(s): Route 111 Ruins Exterior / Land; Safari Zone Northwest / Land | party-selectable species/form |
| 179 | Mareep | SPECIES_MAREEP | Direct acquisition | Random wild in 1 catchable method pool(s): Route 116 / Land | party-selectable species/form |
| 180 | Flaaffy | SPECIES_FLAAFFY | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone Southeast / Land | party-selectable species/form |
| 181 | Ampharos | SPECIES_AMPHAROS | Evolution from obtainable Pokémon | Flaaffy → Ampharos via Level (30) | party-selectable species/form |
| 182 | Bellossom | SPECIES_BELLOSSOM | Evolution from obtainable Pokémon | Gloom → Bellossom via Use/hold ITEM_SUN_STONE | party-selectable species/form |
| 183 | Marill | SPECIES_MARILL | Direct acquisition | Random wild in 3 catchable method pool(s): Petalburg City / Surf; Route 111 / Surf; Route 117 / Surf | party-selectable species/form |
| 184 | Azumarill | SPECIES_AZUMARILL | Direct acquisition | Random wild in 6 catchable method pool(s): Petalburg City / Surf; Route 102 / Surf; Route 111 / Surf; Route 117 / Surf; Route 120 / Surf; Route 123 / Surf | party-selectable species/form |
| 185 | Sudowoodo | SPECIES_SUDOWOODO | Direct acquisition | Scripted/static encounter: Battle Frontier Outside East (Literal setwildbattle acquisition) | party-selectable species/form |
| 186 | Politoed | SPECIES_POLITOED | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods 3 / Super Rod | party-selectable species/form |
| 187 | Hoppip | SPECIES_HOPPIP | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Jumpluff; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 188 | Skiploom | SPECIES_SKIPLOOM | Direct acquisition | Random wild in 1 catchable method pool(s): Route 111 Ruins Exterior / Land | party-selectable species/form |
| 189 | Jumpluff | SPECIES_JUMPLUFF | Direct acquisition | Random wild in 1 catchable method pool(s): Route 111 Ruins Exterior / Honey | party-selectable species/form |
| 190 | Aipom | SPECIES_AIPOM | Direct acquisition | Random wild in 3 catchable method pool(s): Altering Cave / Land; Berry tree encounter table 4: G Berry Stage Berries / Berry Tree; Petalburg Woods / Rock Smash | party-selectable species/form |
| 191 | Sunkern | SPECIES_SUNKERN | Direct acquisition | Random wild in 1 catchable method pool(s): Route 117 / Rock Smash | party-selectable species/form |
| 192 | Sunflora | SPECIES_SUNFLORA | Evolution from obtainable Pokémon | Sunkern → Sunflora via Use/hold ITEM_SUN_STONE | party-selectable species/form |
| 193 | Yanma | SPECIES_YANMA | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods 3 / Land | party-selectable species/form |
| 194 | Wooper | SPECIES_WOOPER | Direct acquisition | Random wild in 3 catchable method pool(s): Route 114 / Surf; Safari Zone Southeast / Surf; Seaspray Cave / Land | party-selectable species/form |
| 195 | Quagsire | SPECIES_QUAGSIRE | Direct acquisition | Random wild in 4 catchable method pool(s): Route 114 / Honey; Route 114 / Surf; Safari Zone Southeast / Surf; Seaspray Cave / Surf | party-selectable species/form |
| 196 | Espeon | SPECIES_ESPEON | Evolution from obtainable Pokémon | Eevee → Espeon via Use/hold ITEM_SUN_STONE | party-selectable species/form |
| 197 | Umbreon | SPECIES_UMBREON | Evolution from obtainable Pokémon | Eevee → Umbreon via Use/hold ITEM_MOON_STONE | party-selectable species/form |
| 198 | Murkrow | SPECIES_MURKROW | Direct acquisition | Random wild in 7 catchable method pool(s): Mt Pyre 1F / Land; Mt Pyre 2F / Land; Mt Pyre 3F / Land; Mt Pyre 4F / Land; Mt Pyre 5F / Land; Mt Pyre 6F / Land; Petalburg Woods 3 / Land | party-selectable species/form |
| 199 | Slowking | SPECIES_SLOWKING | Evolution from obtainable Pokémon | Slowpoke → Slowking via Trade Item (ITEM_KINGS_ROCK) | party-selectable species/form |
| 200 | Misdreavus | SPECIES_MISDREAVUS | Direct acquisition | Random wild in 7 catchable method pool(s): Mt Pyre 1F / Land; Mt Pyre 2F / Land; Mt Pyre 3F / Land; Mt Pyre 4F / Land; Mt Pyre 5F / Land; Mt Pyre 6F / Land; Petalburg Woods 3 / Land | party-selectable species/form |
| 201 | Unown | SPECIES_UNOWN | Direct acquisition | Random wild in 1 catchable method pool(s): Altering Cave / Land | party-selectable species/form |
| 202 | Wobbuffet | SPECIES_WOBBUFFET | Direct acquisition | Random wild in 4 catchable method pool(s): Altering Cave / Land; Altering Cave 1F / Land; Altering Cave B1F / Land; Safari Zone South / Land | party-selectable species/form |
| 203 | Girafarig | SPECIES_GIRAFARIG | Direct acquisition | Random wild in 2 catchable method pool(s): Route 111 Ruins Exterior / Land; Safari Zone South / Land | party-selectable species/form |
| 204 | Pineco | SPECIES_PINECO | Direct acquisition | Random wild in 3 catchable method pool(s): Altering Cave / Land; Route 103 / Rock Smash; Safari Zone Northeast / Land | party-selectable species/form |
| 205 | Forretress | SPECIES_FORRETRESS | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone Northeast / Land | party-selectable species/form |
| 206 | Dunsparce | SPECIES_DUNSPARCE | Direct acquisition | Random wild in 1 catchable method pool(s): Rusturf Tunnel / Land | party-selectable species/form |
| 207 | Gligar | SPECIES_GLIGAR | Direct acquisition | Random wild in 2 catchable method pool(s): Jagged Pass / Land; Safari Zone Southeast / Land | party-selectable species/form |
| 208 | Steelix | SPECIES_STEELIX | Direct acquisition | Random wild in 9 catchable method pool(s): Altering Cave / Rock Smash; Altering Cave 1F / Rock Smash; Altering Cave B1F / Rock Smash; Sandstrewn Ruins / Rock Smash; Sandstrewn Ruins 2F / Rock Smash; Sandstrewn Ruins 3F / Rock Smash; Sandstrewn Ruins B1F / Rock Smash; Victory Road B1F / Land; Victory Road B2F / Land | party-selectable species/form |
| 209 | Snubbull | SPECIES_SNUBBULL | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Granbull; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 210 | Granbull | SPECIES_GRANBULL | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone Southeast / Land | party-selectable species/form |
| 211 | Qwilfish | SPECIES_QWILFISH | Direct acquisition | Random wild in 2 catchable method pool(s): Route 115 / Good Rod; Route 115 / Super Rod | party-selectable species/form |
| 212 | Scizor | SPECIES_SCIZOR | Evolution from obtainable Pokémon | Scyther → Scizor via Trade Item (ITEM_METAL_COAT) | party-selectable species/form |
| 213 | Shuckle | SPECIES_SHUCKLE | Direct acquisition | Random wild in 8 catchable method pool(s): Altering Cave / Land; Altering Cave / Rock Smash; Altering Cave 1F / Rock Smash; Altering Cave B1F / Rock Smash; Route 111 / Rock Smash; Route 114 / Rock Smash; Safari Zone Northeast / Rock Smash; Victory Road B1F / Rock Smash | party-selectable species/form |
| 214 | Heracross | SPECIES_HERACROSS | Direct acquisition | Random wild in 3 catchable method pool(s): Ashen Woods / Land; Route 121 / Rock Smash; Safari Zone North / Land | party-selectable species/form |
| 215 | Sneasel | SPECIES_SNEASEL | Direct acquisition | Random wild in 1 catchable method pool(s): Seaspray Cave B1F / Land | party-selectable species/form |
| 216 | Teddiursa | SPECIES_TEDDIURSA | Direct acquisition | Random wild in 2 catchable method pool(s): Altering Cave / Land; Rusturf Tunnel / Land | party-selectable species/form |
| 217 | Ursaring | SPECIES_URSARING | Evolution from obtainable Pokémon | Teddiursa → Ursaring via Level (30) | party-selectable species/form |
| 218 | Slugma | SPECIES_SLUGMA | Direct acquisition | Random wild in 1 catchable method pool(s): Fiery Path / Land | party-selectable species/form |
| 219 | Magcargo | SPECIES_MAGCARGO | Direct acquisition | Random wild in 2 catchable method pool(s): Ember Path / Land; Scorched Slab Heatrans Room / Land | party-selectable species/form |
| 220 | Swinub | SPECIES_SWINUB | Direct acquisition | Random wild in 1 catchable method pool(s): Seaspray Cave B1F / Land | party-selectable species/form |
| 221 | Piloswine | SPECIES_PILOSWINE | Evolution from obtainable Pokémon | Swinub → Piloswine via Level (33) | party-selectable species/form |
| 222 | Corsola | SPECIES_CORSOLA | Direct acquisition | Random wild in 7 catchable method pool(s): Ever Grande City / Good Rod; Ever Grande City / Super Rod; Route 109 / Good Rod; Route 109 / Old Rod; Route 109 / Super Rod; Route 128 / Good Rod; Route 128 / Super Rod | party-selectable species/form |
| 223 | Remoraid | SPECIES_REMORAID | Direct acquisition | Random wild in 5 catchable method pool(s): Route 107 / Good Rod; Route 107 / Old Rod; Route 108 / Good Rod; Route 108 / Old Rod; Safari Zone Southeast / Old Rod | party-selectable species/form |
| 224 | Octillery | SPECIES_OCTILLERY | Direct acquisition | Random wild in 2 catchable method pool(s): Route 107 / Super Rod; Route 108 / Super Rod | party-selectable species/form |
| 225 | Delibird | SPECIES_DELIBIRD | Direct acquisition | Random wild in 5 catchable method pool(s): Seaspray Cave B1F / Honey; Shoal Cave Low Tide Entrance Room / Land; Shoal Cave Low Tide Inner Room / Land; Shoal Cave Low Tide Lower Room / Land; Shoal Cave Low Tide Stairs Room / Land | party-selectable species/form |
| 226 | Mantine | SPECIES_MANTINE | Evolution from obtainable Pokémon | Mantyke → Mantine via Specific Mon In Party (SPECIES_REMORAID) | party-selectable species/form |
| 227 | Skarmory | SPECIES_SKARMORY | Direct acquisition | Random wild in 1 catchable method pool(s): Route 113 / Land | party-selectable species/form |
| 228 | Houndour | SPECIES_HOUNDOUR | Direct acquisition | Random wild in 2 catchable method pool(s): Altering Cave / Land; Route 116 / Land | party-selectable species/form |
| 229 | Houndoom | SPECIES_HOUNDOOM | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone Northeast / Land | party-selectable species/form |
| 230 | Kingdra | SPECIES_KINGDRA | Direct acquisition | Random wild in 13 catchable method pool(s): Route 124 / Super Rod; Route 125 / Super Rod; Route 126 / Super Rod; Route 128 / Surf; Route 132 / Super Rod; Route 133 / Super Rod; Route 134 / Super Rod; Seafloor Cavern Entrance / Surf; Seafloor Cavern Room6 / Surf; Seafloor Cavern Room7 / Surf; Slateport City / Super Rod; Underwater Route 124 / Land; Underwater Route 126 / Land | party-selectable species/form |
| 231 | Phanpy | SPECIES_PHANPY | Direct acquisition | Random wild in 1 catchable method pool(s): Route 114 / Land | party-selectable species/form |
| 232 | Donphan | SPECIES_DONPHAN | Direct acquisition | Random wild in 4 catchable method pool(s): Route 114 / Honey; Safari Zone North / Land; Victory Road B1F / Land; Victory Road B2F / Land | party-selectable species/form |
| 233 | Porygon2 | SPECIES_PORYGON2 | Evolution from obtainable Pokémon | Porygon → Porygon2 via Trade Item (ITEM_UPGRADE) | party-selectable species/form |
| 234 | Stantler | SPECIES_STANTLER | Direct acquisition | Random wild in 3 catchable method pool(s): Altering Cave / Land; Route 123 / Land; Safari Zone Southeast / Land | party-selectable species/form |
| 235 | Smeargle | SPECIES_SMEARGLE | Direct acquisition | Random wild in 4 catchable method pool(s): Altering Cave / Land; Artisan Cave 1F / Land; Artisan Cave B1F / Land; Safari Zone South / Land | party-selectable species/form |
| 236 | Tyrogue | SPECIES_TYROGUE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 112 / Land | party-selectable species/form |
| 237 | Hitmontop | SPECIES_HITMONTOP | Evolution from obtainable Pokémon | Tyrogue → Hitmontop via Level Atk Eq Def (20) | party-selectable species/form |
| 238 | Smoochum | SPECIES_SMOOCHUM | Direct acquisition | Random wild in 1 catchable method pool(s): Seaspray Cave B1F / Land | party-selectable species/form |
| 239 | Elekid | SPECIES_ELEKID | Direct acquisition | Random wild in 2 catchable method pool(s): New Mauville Entrance / Land; New Mauville Inside / Land | party-selectable species/form |
| 240 | Magby | SPECIES_MAGBY | Direct acquisition | Random wild in 1 catchable method pool(s): Fiery Path / Land | party-selectable species/form |
| 241 | Miltank | SPECIES_MILTANK | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone Northeast / Land | party-selectable species/form |
| 242 | Blissey | SPECIES_BLISSEY | Direct acquisition | Random wild in 3 catchable method pool(s): Altering Cave / Honey; Altering Cave 1F / Honey; Altering Cave B1F / Honey | party-selectable species/form |
| 243 | Raikou | SPECIES_RAIKOU | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 244 | Entei | SPECIES_ENTEI | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 245 | Suicune | SPECIES_SUICUNE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 125 / Super Rod | party-selectable species/form |
| 246 | Larvitar | SPECIES_LARVITAR | Direct acquisition | Random wild in 1 catchable method pool(s): Rusturf Tunnel / Land | party-selectable species/form |
| 247 | Pupitar | SPECIES_PUPITAR | Direct acquisition | Random wild in 1 catchable method pool(s): Victory Road 1F / Land | party-selectable species/form |
| 248 | Tyranitar | SPECIES_TYRANITAR | Evolution from obtainable Pokémon | Pupitar → Tyranitar via Level (55) | party-selectable species/form |
| 249 | Lugia | SPECIES_LUGIA | Direct acquisition | Authored static acquisition: Navel Rock Bottom (lugia) | party-selectable species/form |
| 250 | Ho-Oh | SPECIES_HO_OH | Direct acquisition | Authored static acquisition: Navel Rock Top (ho_oh) | party-selectable species/form |
| 251 | Celebi | SPECIES_CELEBI | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 252 | Treecko | SPECIES_TREECKO | Direct acquisition | Starter choice: Littleroot opening (Selectable Hoenn starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 253 | Grovyle | SPECIES_GROVYLE | Evolution from obtainable Pokémon | Treecko → Grovyle via Level (16) | party-selectable species/form |
| 254 | Sceptile | SPECIES_SCEPTILE | Evolution from obtainable Pokémon | Treecko → Grovyle via Level (16) ; Grovyle → Sceptile via Level (36) | party-selectable species/form |
| 255 | Torchic | SPECIES_TORCHIC | Direct acquisition | Starter choice: Littleroot opening (Selectable Hoenn starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 256 | Combusken | SPECIES_COMBUSKEN | Evolution from obtainable Pokémon | Torchic → Combusken via Level (16) | party-selectable species/form |
| 257 | Blaziken | SPECIES_BLAZIKEN | Evolution from obtainable Pokémon | Torchic → Combusken via Level (16) ; Combusken → Blaziken via Level (36) | party-selectable species/form |
| 258 | Mudkip | SPECIES_MUDKIP | Direct acquisition | Starter choice: Littleroot opening (Selectable Hoenn starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 259 | Marshtomp | SPECIES_MARSHTOMP | Evolution from obtainable Pokémon | Mudkip → Marshtomp via Level (16) | party-selectable species/form |
| 260 | Swampert | SPECIES_SWAMPERT | Evolution from obtainable Pokémon | Mudkip → Marshtomp via Level (16) ; Marshtomp → Swampert via Level (36) | party-selectable species/form |
| 261 | Poochyena | SPECIES_POOCHYENA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 101 / Land | party-selectable species/form |
| 262 | Mightyena | SPECIES_MIGHTYENA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 123 / Land | party-selectable species/form |
| 263 | Zigzagoon | SPECIES_ZIGZAGOON | Direct acquisition | Random wild in 1 catchable method pool(s): Route 101 / Land | party-selectable species/form |
| 264 | Linoone | SPECIES_LINOONE | Direct acquisition | Random wild in 2 catchable method pool(s): Route 118 / Land; Route 123 / Land | party-selectable species/form |
| 265 | Wurmple | SPECIES_WURMPLE | Direct acquisition | Random wild in 2 catchable method pool(s): Berry tree encounter table 1: G Berry Stage Sprouted / Berry Tree; Route 101 / Land | party-selectable species/form |
| 266 | Silcoon | SPECIES_SILCOON | Direct acquisition | Random wild in 2 catchable method pool(s): Berry tree encounter table 2: G Berry Stage Taller / Berry Tree; Petalburg Woods / Land | party-selectable species/form |
| 267 | Beautifly | SPECIES_BEAUTIFLY | Direct acquisition | Random wild in 1 catchable method pool(s): Route 101 / Honey | party-selectable species/form |
| 268 | Cascoon | SPECIES_CASCOON | Direct acquisition | Random wild in 2 catchable method pool(s): Berry tree encounter table 2: G Berry Stage Taller / Berry Tree; Petalburg Woods / Land | party-selectable species/form |
| 269 | Dustox | SPECIES_DUSTOX | Direct acquisition | Random wild in 1 catchable method pool(s): Route 101 / Honey | party-selectable species/form |
| 270 | Lotad | SPECIES_LOTAD | Direct acquisition | Random wild in 1 catchable method pool(s): Route 102 / Land | party-selectable species/form |
| 271 | Lombre | SPECIES_LOMBRE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 114 / Land | party-selectable species/form |
| 272 | Ludicolo | SPECIES_LUDICOLO | Evolution from obtainable Pokémon | Lombre → Ludicolo via Use/hold ITEM_WATER_STONE | party-selectable species/form |
| 273 | Seedot | SPECIES_SEEDOT | Direct acquisition | Random wild in 1 catchable method pool(s): Route 102 / Land | party-selectable species/form |
| 274 | Nuzleaf | SPECIES_NUZLEAF | Direct acquisition | Random wild in 1 catchable method pool(s): Route 114 / Land | party-selectable species/form |
| 275 | Shiftry | SPECIES_SHIFTRY | Evolution from obtainable Pokémon | Nuzleaf → Shiftry via Use/hold ITEM_LEAF_STONE | party-selectable species/form |
| 276 | Taillow | SPECIES_TAILLOW | Direct acquisition | Random wild in 1 catchable method pool(s): Route 104 / Land | party-selectable species/form |
| 277 | Swellow | SPECIES_SWELLOW | Direct acquisition | Random wild in 1 catchable method pool(s): Route 115 / Land | party-selectable species/form |
| 278 | Wingull | SPECIES_WINGULL | Direct acquisition | Random wild in 10 catchable method pool(s): Dewford Town / Surf; Route 103 / Land; Route 103 / Surf; Route 104 / Surf; Route 105 / Surf; Route 106 / Surf; Route 107 / Surf; Route 108 / Surf; Route 115 / Surf; Slateport City / Surf | party-selectable species/form |
| 279 | Pelipper | SPECIES_PELIPPER | Direct acquisition | Random wild in 18 catchable method pool(s): Dewford Town / Surf; Ever Grande City / Surf; Lilycove City / Surf; Mossdeep City / Surf; Pacifidlog Town / Surf; Route 103 / Surf; Route 104 / Surf; Route 105 / Surf; Route 106 / Surf; Route 110 / Surf; Route 118 / Surf; Route 119 / Surf; Route 121 / Surf; Route 122 / Surf; Route 132 / Surf; Route 133 / Surf; Route 134 / Surf; Slateport City / Surf | party-selectable species/form |
| 280 | Ralts | SPECIES_RALTS | Direct acquisition | Random wild in 1 catchable method pool(s): Route 102 / Land | party-selectable species/form |
| 281 | Kirlia | SPECIES_KIRLIA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 102 / Honey | party-selectable species/form |
| 282 | Gardevoir | SPECIES_GARDEVOIR | Evolution from obtainable Pokémon | Kirlia → Gardevoir via Level (30) | party-selectable species/form |
| 283 | Surskit | SPECIES_SURSKIT | Direct acquisition | Random wild in 4 catchable method pool(s): Route 102 / Land; Route 102 / Surf; Route 111 / Surf; Route 123 / Surf | party-selectable species/form |
| 284 | Masquerain | SPECIES_MASQUERAIN | Direct acquisition | Random wild in 7 catchable method pool(s): Petalburg City / Surf; Route 102 / Surf; Route 111 / Surf; Route 114 / Surf; Route 120 / Rock Smash; Route 120 / Surf; Route 123 / Surf | party-selectable species/form |
| 285 | Shroomish | SPECIES_SHROOMISH | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods / Land | party-selectable species/form |
| 286 | Breloom | SPECIES_BRELOOM | Evolution from obtainable Pokémon | Shroomish → Breloom via Level (23) | party-selectable species/form |
| 287 | Slakoth | SPECIES_SLAKOTH | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods / Land | party-selectable species/form |
| 288 | Vigoroth | SPECIES_VIGOROTH | Evolution from obtainable Pokémon | Slakoth → Vigoroth via Level (18) | party-selectable species/form |
| 289 | Slaking | SPECIES_SLAKING | Evolution from obtainable Pokémon | Slakoth → Vigoroth via Level (18) ; Vigoroth → Slaking via Level (36) | party-selectable species/form |
| 290 | Nincada | SPECIES_NINCADA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 116 / Land | party-selectable species/form |
| 291 | Ninjask | SPECIES_NINJASK | Evolution from obtainable Pokémon | Nincada → Ninjask via Level Ninjask (20) | party-selectable species/form |
| 292 | Shedinja | SPECIES_SHEDINJA | Evolution from obtainable Pokémon | Nincada → Shedinja via Level Shedinja (20) | party-selectable species/form |
| 293 | Whismur | SPECIES_WHISMUR | Direct acquisition | Random wild in 1 catchable method pool(s): Rusturf Tunnel / Land | party-selectable species/form |
| 294 | Loudred | SPECIES_LOUDRED | Evolution from obtainable Pokémon | Whismur → Loudred via Level (20) | party-selectable species/form |
| 295 | Exploud | SPECIES_EXPLOUD | Direct acquisition | Random wild in 1 catchable method pool(s): Victory Road B2F / Land | party-selectable species/form |
| 296 | Makuhita | SPECIES_MAKUHITA | Direct acquisition | Random wild in 2 catchable method pool(s): Granite Cave 1F / Land; Granite Cave Stevens Room / Land | party-selectable species/form |
| 297 | Hariyama | SPECIES_HARIYAMA | Evolution from obtainable Pokémon | Makuhita → Hariyama via Level (24) | party-selectable species/form |
| 298 | Azurill | SPECIES_AZURILL | Direct acquisition | Random wild in 1 catchable method pool(s): Route 104 / Land | party-selectable species/form |
| 299 | Nosepass | SPECIES_NOSEPASS | Direct acquisition | Random wild in 1 catchable method pool(s): Granite Cave B2F / Rock Smash | party-selectable species/form |
| 300 | Skitty | SPECIES_SKITTY | Direct acquisition | Random wild in 1 catchable method pool(s): Route 116 / Land | party-selectable species/form |
| 301 | Delcatty | SPECIES_DELCATTY | Evolution from obtainable Pokémon | Skitty → Delcatty via Use/hold ITEM_MOON_STONE | party-selectable species/form |
| 302 | Sableye | SPECIES_SABLEYE | Direct acquisition | Random wild in 11 catchable method pool(s): Cave Of Origin 1F / Land; Cave Of Origin Entrance / Land; Cave Of Origin Unused Ruby Sapphire Map1 / Land; Cave Of Origin Unused Ruby Sapphire Map2 / Land; Cave Of Origin Unused Ruby Sapphire Map3 / Land; Granite Cave B1F / Land; Granite Cave B2F / Land; Sky Pillar 1F / Land; Sky Pillar 3F / Land; Sky Pillar 5F / Land; Victory Road B1F / Land | party-selectable species/form |
| 303 | Mawile | SPECIES_MAWILE | Direct acquisition | Random wild in 8 catchable method pool(s): Cave Of Origin 1F / Land; Cave Of Origin Entrance / Land; Granite Cave B1F / Land; Granite Cave B2F / Land; Sky Pillar 1F / Land; Sky Pillar 3F / Land; Sky Pillar 5F / Land; Victory Road B1F / Land | party-selectable species/form |
| 304 | Aron | SPECIES_ARON | Direct acquisition | Random wild in 4 catchable method pool(s): Granite Cave 1F / Land; Granite Cave B1F / Land; Granite Cave B2F / Land; Granite Cave Stevens Room / Land | party-selectable species/form |
| 305 | Lairon | SPECIES_LAIRON | Direct acquisition | Random wild in 1 catchable method pool(s): Victory Road 1F / Land | party-selectable species/form |
| 306 | Aggron | SPECIES_AGGRON | Direct acquisition | Random wild in 1 catchable method pool(s): Victory Road B2F / Land | party-selectable species/form |
| 307 | Meditite | SPECIES_MEDITITE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 111 Ruins Exterior / Land | party-selectable species/form |
| 308 | Medicham | SPECIES_MEDICHAM | Direct acquisition | Random wild in 3 catchable method pool(s): Mt Pyre Exterior / Land; Mt Pyre Summit / Land; Victory Road 1F / Land | party-selectable species/form |
| 309 | Electrike | SPECIES_ELECTRIKE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 110 / Land | party-selectable species/form |
| 310 | Manectric | SPECIES_MANECTRIC | Direct acquisition | Random wild in 1 catchable method pool(s): Route 118 / Land | party-selectable species/form |
| 311 | Plusle | SPECIES_PLUSLE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 110 / Land | party-selectable species/form |
| 312 | Minun | SPECIES_MINUN | Direct acquisition | Random wild in 1 catchable method pool(s): Route 110 / Land | party-selectable species/form |
| 313 | Volbeat | SPECIES_VOLBEAT | Direct acquisition | Random wild in 2 catchable method pool(s): Berry tree encounter table 3: G Berry Stage Flowering / Berry Tree; Route 117 / Land | party-selectable species/form |
| 314 | Illumise | SPECIES_ILLUMISE | Direct acquisition | Random wild in 2 catchable method pool(s): Berry tree encounter table 3: G Berry Stage Flowering / Berry Tree; Route 117 / Land | party-selectable species/form |
| 315 | Roselia | SPECIES_ROSELIA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 117 / Land | party-selectable species/form |
| 316 | Gulpin | SPECIES_GULPIN | Direct acquisition | Random wild in 1 catchable method pool(s): Route 110 / Land | party-selectable species/form |
| 317 | Swalot | SPECIES_SWALOT | Evolution from obtainable Pokémon | Gulpin → Swalot via Level (26) | party-selectable species/form |
| 318 | Carvanha | SPECIES_CARVANHA | Direct acquisition | Random wild in 4 catchable method pool(s): Route 118 / Old Rod; Route 119 / Good Rod; Route 119 / Old Rod; Route 119 / Super Rod | party-selectable species/form |
| 319 | Sharpedo | SPECIES_SHARPEDO | Direct acquisition | Random wild in 9 catchable method pool(s): Mossdeep City / Good Rod; Mossdeep City / Super Rod; Mossdeep City / Surf; Pacifidlog Town / Super Rod; Pacifidlog Town / Surf; Route 103 / Super Rod; Route 118 / Super Rod; Route 119 / Good Rod; Route 119 / Super Rod | party-selectable species/form |
| 320 | Wailmer | SPECIES_WAILMER | Direct acquisition | Random wild in 23 catchable method pool(s): Mossdeep City / Good Rod; Mossdeep City / Super Rod; Pacifidlog Town / Good Rod; Route 103 / Good Rod; Route 103 / Super Rod; Route 110 / Good Rod; Route 110 / Old Rod; Route 110 / Super Rod; Route 115 / Good Rod; Route 115 / Old Rod; Route 115 / Super Rod; Route 121 / Good Rod; Route 121 / Old Rod; Route 121 / Super Rod; Route 122 / Good Rod; Route 122 / Old Rod; Route 122 / Super Rod; Seafloor Cavern Entrance / Good Rod; Seafloor Cavern Entrance / Super Rod; Seafloor Cavern Room6 / Good Rod; Seafloor Cavern Room6 / Super Rod; Seafloor Cavern Room7 / Good Rod; Seafloor Cavern Room7 / Super Rod | party-selectable species/form |
| 321 | Wailord | SPECIES_WAILORD | Direct acquisition | Random wild in 8 catchable method pool(s): Route 110 / Super Rod; Route 115 / Super Rod; Route 129 / Surf; Route 130 / Surf; Route 131 / Surf; Route 132 / Surf; Route 133 / Surf; Route 134 / Surf | party-selectable species/form |
| 322 | Numel | SPECIES_NUMEL | Direct acquisition | Random wild in 1 catchable method pool(s): Route 112 / Land | party-selectable species/form |
| 323 | Camerupt | SPECIES_CAMERUPT | Direct acquisition | Random wild in 1 catchable method pool(s): Ashen Woods / Land | party-selectable species/form |
| 324 | Torkoal | SPECIES_TORKOAL | Direct acquisition | Random wild in 9 catchable method pool(s): Fiery Path / Land; Magma Hideout 1F / Land; Magma Hideout 2F 1R / Land; Magma Hideout 2F 2R / Land; Magma Hideout 2F 3R / Land; Magma Hideout 3F 1R / Land; Magma Hideout 3F 2R / Land; Magma Hideout 3F 3R / Land; Magma Hideout 4F / Land | party-selectable species/form |
| 325 | Spoink | SPECIES_SPOINK | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Grumpig; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 326 | Grumpig | SPECIES_GRUMPIG | Direct acquisition | Random wild in 2 catchable method pool(s): Ember Path / Land; Jagged Pass / Land | party-selectable species/form |
| 327 | Spinda | SPECIES_SPINDA | Direct acquisition | Random wild in 2 catchable method pool(s): Route 113 / Honey; Route 113 / Land | party-selectable species/form |
| 328 | Trapinch | SPECIES_TRAPINCH | Direct acquisition | Random wild in 6 catchable method pool(s): Mirage Tower 1F / Land; Mirage Tower 2F / Land; Mirage Tower 3F / Land; Mirage Tower 4F / Land; Mirage Tower B1F / Land; Route 111 / Land | party-selectable species/form |
| 329 | Vibrava | SPECIES_VIBRAVA | Evolution from obtainable Pokémon | Trapinch → Vibrava via Level (35) | party-selectable species/form |
| 330 | Flygon | SPECIES_FLYGON | Evolution from obtainable Pokémon | Trapinch → Vibrava via Level (35) ; Vibrava → Flygon via Level (45) | party-selectable species/form |
| 331 | Cacnea | SPECIES_CACNEA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 111 / Land | party-selectable species/form |
| 332 | Cacturne | SPECIES_CACTURNE | Evolution from obtainable Pokémon | Cacnea → Cacturne via Level (32) | party-selectable species/form |
| 333 | Swablu | SPECIES_SWABLU | Direct acquisition | Random wild in 1 catchable method pool(s): Route 114 / Land | party-selectable species/form |
| 334 | Altaria | SPECIES_ALTARIA | Direct acquisition | Random wild in 1 catchable method pool(s): Sky Pillar 5F / Land | party-selectable species/form |
| 335 | Zangoose | SPECIES_ZANGOOSE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 114 / Land | party-selectable species/form |
| 336 | Seviper | SPECIES_SEVIPER | Direct acquisition | Random wild in 1 catchable method pool(s): Route 114 / Land | party-selectable species/form |
| 337 | Lunatone | SPECIES_LUNATONE | Direct acquisition | Random wild in 9 catchable method pool(s): Meteor Falls 1F 1R / Land; Meteor Falls 1F 1R / Surf; Meteor Falls 1F 2R / Land; Meteor Falls 1F 2R / Surf; Meteor Falls B1F 1R / Land; Meteor Falls B1F 1R / Surf; Meteor Falls B1F 2R / Land; Meteor Falls B1F 2R / Surf; Meteor Falls Stevens Cave / Land | party-selectable species/form |
| 338 | Solrock | SPECIES_SOLROCK | Direct acquisition | Random wild in 9 catchable method pool(s): Meteor Falls 1F 1R / Land; Meteor Falls 1F 1R / Surf; Meteor Falls 1F 2R / Land; Meteor Falls 1F 2R / Surf; Meteor Falls B1F 1R / Land; Meteor Falls B1F 1R / Surf; Meteor Falls B1F 2R / Land; Meteor Falls B1F 2R / Surf; Meteor Falls Stevens Cave / Land | party-selectable species/form |
| 339 | Barboach | SPECIES_BARBOACH | Direct acquisition | Random wild in 23 catchable method pool(s): Altering Cave / Old Rod; Altering Cave B1F / Old Rod; Meteor Falls 1F 1R / Good Rod; Meteor Falls 1F 1R / Old Rod; Meteor Falls 1F 2R / Good Rod; Meteor Falls 1F 2R / Old Rod; Meteor Falls B1F 1R / Good Rod; Meteor Falls B1F 1R / Old Rod; Meteor Falls B1F 2R / Good Rod; Meteor Falls B1F 2R / Old Rod; Route 111 / Old Rod; Route 114 / Good Rod; Route 114 / Old Rod; Route 120 / Good Rod; Route 120 / Old Rod; Scorched Slab / Good Rod; Scorched Slab / Old Rod; Scorched Slab / Super Rod; Scorched Slab B1F / Good Rod; Scorched Slab B1F / Old Rod; Scorched Slab B1F / Super Rod; Victory Road B2F / Good Rod; Victory Road B2F / Old Rod | party-selectable species/form |
| 340 | Whiscash | SPECIES_WHISCASH | Direct acquisition | Random wild in 17 catchable method pool(s): Altering Cave / Good Rod; Altering Cave / Super Rod; Altering Cave / Surf; Altering Cave B1F / Good Rod; Altering Cave B1F / Super Rod; Altering Cave B1F / Surf; Meteor Falls 1F 1R / Super Rod; Meteor Falls 1F 2R / Super Rod; Meteor Falls B1F 1R / Super Rod; Meteor Falls B1F 2R / Super Rod; Route 114 / Super Rod; Route 120 / Super Rod; Scorched Slab / Good Rod; Scorched Slab / Super Rod; Scorched Slab B1F / Good Rod; Scorched Slab B1F / Super Rod; Victory Road B2F / Super Rod | party-selectable species/form |
| 341 | Corphish | SPECIES_CORPHISH | Direct acquisition | Random wild in 11 catchable method pool(s): Petalburg City / Good Rod; Petalburg City / Old Rod; Petalburg City / Super Rod; Route 102 / Good Rod; Route 102 / Old Rod; Route 117 / Good Rod; Route 117 / Old Rod; Route 117 / Super Rod; Route 123 / Good Rod; Route 123 / Old Rod; Route 123 / Super Rod | party-selectable species/form |
| 342 | Crawdaunt | SPECIES_CRAWDAUNT | Direct acquisition | Random wild in 5 catchable method pool(s): Petalburg City / Super Rod; Route 102 / Good Rod; Route 102 / Super Rod; Route 117 / Super Rod; Route 123 / Super Rod | party-selectable species/form |
| 343 | Baltoy | SPECIES_BALTOY | Direct acquisition | Random wild in 5 catchable method pool(s): Route 111 / Land; Sandstrewn Ruins / Rock Smash; Sandstrewn Ruins 2F / Rock Smash; Sandstrewn Ruins 3F / Rock Smash; Sandstrewn Ruins B1F / Rock Smash | party-selectable species/form |
| 344 | Claydol | SPECIES_CLAYDOL | Direct acquisition | Random wild in 7 catchable method pool(s): Sandstrewn Ruins / Land; Sandstrewn Ruins 2F / Land; Sandstrewn Ruins 3F / Land; Sandstrewn Ruins B1F / Land; Sky Pillar 1F / Land; Sky Pillar 3F / Land; Sky Pillar 5F / Land | party-selectable species/form |
| 345 | Lileep | SPECIES_LILEEP | Direct acquisition | Fossil restoration: Rustboro Devon Corporation (Restore ITEM_ROOT_FOSSIL) | party-selectable species/form |
| 346 | Cradily | SPECIES_CRADILY | Evolution from obtainable Pokémon | Lileep → Cradily via Level (40) | party-selectable species/form |
| 347 | Anorith | SPECIES_ANORITH | Direct acquisition | Fossil restoration: Rustboro Devon Corporation (Restore ITEM_CLAW_FOSSIL) | party-selectable species/form |
| 348 | Armaldo | SPECIES_ARMALDO | Evolution from obtainable Pokémon | Anorith → Armaldo via Level (40) | party-selectable species/form |
| 349 | Feebas | SPECIES_FEEBAS | Direct acquisition | Random wild in 1 catchable method pool(s): Route 119 under the bridge / Any Rod under bridge | party-selectable species/form |
| 350 | Milotic | SPECIES_MILOTIC | Direct acquisition | Random wild in 5 catchable method pool(s): Route 119 / Super Rod; Route 124 / Surf; Seafloor Cavern Entrance / Surf; Seafloor Cavern Room6 / Surf; Seafloor Cavern Room7 / Surf | party-selectable species/form |
| 351 | Castform | SPECIES_CASTFORM | Direct acquisition | NPC or story gift: Route 119 Weather Institute 2F (Literal givemon acquisition) \| Authored gift/restoration: Route 119 Weather Institute 2F (castform) | party-selectable species/form |
| 352 | Kecleon | SPECIES_KECLEON | Direct acquisition | Scripted/static encounter: Route 120 (Literal setwildbattle acquisition) \| Scripted/static encounter: Global event system (Literal setwildbattle acquisition) \| Authored static acquisition: Route 120 (kecleon) | party-selectable species/form |
| 353 | Shuppet | SPECIES_SHUPPET | Direct acquisition | Random wild in 4 catchable method pool(s): Mt Pyre 1F / Land; Mt Pyre 2F / Land; Mt Pyre 3F / Land; Route 121 / Land | party-selectable species/form |
| 354 | Banette | SPECIES_BANETTE | Direct acquisition | Random wild in 7 catchable method pool(s): Mt Pyre 4F / Land; Mt Pyre 5F / Land; Mt Pyre 6F / Land; Route 121 / Honey; Sky Pillar 1F / Land; Sky Pillar 3F / Land; Sky Pillar 5F / Land | party-selectable species/form |
| 355 | Duskull | SPECIES_DUSKULL | Direct acquisition | Random wild in 4 catchable method pool(s): Mt Pyre 1F / Land; Mt Pyre 2F / Land; Mt Pyre 3F / Land; Route 121 / Land | party-selectable species/form |
| 356 | Dusclops | SPECIES_DUSCLOPS | Direct acquisition | Random wild in 7 catchable method pool(s): Mt Pyre 4F / Land; Mt Pyre 5F / Land; Mt Pyre 6F / Land; Route 121 / Honey; Sky Pillar 1F / Land; Sky Pillar 3F / Land; Sky Pillar 5F / Land | party-selectable species/form |
| 357 | Tropius | SPECIES_TROPIUS | Direct acquisition | Random wild in 2 catchable method pool(s): Route 119 / Land; Route 120 / Land \| In-game trade: NPC trade (Trade for SPECIES_CROAGUNK; received at level 21) | party-selectable species/form |
| 358 | Chimecho | SPECIES_CHIMECHO | Direct acquisition | Random wild in 2 catchable method pool(s): Mt Pyre Summit / Honey; Mt Pyre Summit / Land | party-selectable species/form |
| 359 | Absol | SPECIES_ABSOL | Direct acquisition | Random wild in 1 catchable method pool(s): Route 120 / Land | party-selectable species/form |
| 360 | Wynaut | SPECIES_WYNAUT | Direct acquisition | Random wild in 1 catchable method pool(s): Route 130 / Land \| Gift Egg: Lavaridge Town (Literal giveegg acquisition) | party-selectable species/form |
| 361 | Snorunt | SPECIES_SNORUNT | Direct acquisition | Random wild in 2 catchable method pool(s): Seaspray Cave B1F / Land; Shoal Cave Low Tide Ice Room / Land | party-selectable species/form |
| 362 | Glalie | SPECIES_GLALIE | Evolution from obtainable Pokémon | Snorunt → Glalie via Level (30) | party-selectable species/form |
| 363 | Spheal | SPECIES_SPHEAL | Direct acquisition | Random wild in 7 catchable method pool(s): Seaspray Cave B1F / Land; Shoal Cave Low Tide Entrance Room / Land; Shoal Cave Low Tide Entrance Room / Surf; Shoal Cave Low Tide Inner Room / Land; Shoal Cave Low Tide Inner Room / Surf; Shoal Cave Low Tide Lower Room / Land; Shoal Cave Low Tide Stairs Room / Land | party-selectable species/form |
| 364 | Sealeo | SPECIES_SEALEO | Direct acquisition | Random wild in 5 catchable method pool(s): Shoal Cave Low Tide Entrance Room / Land; Shoal Cave Low Tide Ice Room / Land; Shoal Cave Low Tide Inner Room / Land; Shoal Cave Low Tide Lower Room / Land; Shoal Cave Low Tide Stairs Room / Land | party-selectable species/form |
| 365 | Walrein | SPECIES_WALREIN | Evolution from obtainable Pokémon | Sealeo → Walrein via Level (44) | party-selectable species/form |
| 366 | Clamperl | SPECIES_CLAMPERL | Direct acquisition | Random wild in 2 catchable method pool(s): Underwater Route 124 / Land; Underwater Route 126 / Land | party-selectable species/form |
| 367 | Huntail | SPECIES_HUNTAIL | Direct acquisition | Random wild in 2 catchable method pool(s): Underwater Route 124 / Land; Underwater Route 126 / Land | party-selectable species/form |
| 368 | Gorebyss | SPECIES_GOREBYSS | Direct acquisition | Random wild in 2 catchable method pool(s): Underwater Route 124 / Land; Underwater Route 126 / Land | party-selectable species/form |
| 369 | Relicanth | SPECIES_RELICANTH | Direct acquisition | Random wild in 6 catchable method pool(s): Sandstrewn Ruins / Good Rod; Sandstrewn Ruins / Old Rod; Sandstrewn Ruins / Super Rod; Sandstrewn Ruins / Surf; Underwater Route 124 / Land; Underwater Route 126 / Land | party-selectable species/form |
| 370 | Luvdisc | SPECIES_LUVDISC | Direct acquisition | Random wild in 8 catchable method pool(s): Ever Grande City / Good Rod; Ever Grande City / Old Rod; Ever Grande City / Super Rod; Route 104 / Good Rod; Route 104 / Super Rod; Route 128 / Good Rod; Route 128 / Old Rod; Route 128 / Super Rod | party-selectable species/form |
| 371 | Bagon | SPECIES_BAGON | Direct acquisition | Random wild in 3 catchable method pool(s): Jagged Pass / Land; Meteor Falls B1F 2R / Land; Rusturf Tunnel / Land | party-selectable species/form |
| 372 | Shelgon | SPECIES_SHELGON | Evolution from obtainable Pokémon | Bagon → Shelgon via Level (30) | party-selectable species/form |
| 373 | Salamence | SPECIES_SALAMENCE | Direct acquisition | Random wild in 1 catchable method pool(s): Meteor Falls B1F 2R / Land | party-selectable species/form |
| 374 | Beldum | SPECIES_BELDUM | Direct acquisition | Random wild in 1 catchable method pool(s): Granite Cave Stevens Room / Land | party-selectable species/form |
| 375 | Metang | SPECIES_METANG | Direct acquisition | Random wild in 1 catchable method pool(s): Meteor Falls Stevens Cave / Land | party-selectable species/form |
| 376 | Metagross | SPECIES_METAGROSS | Direct acquisition | Random wild in 3 catchable method pool(s): Meteor Falls Stevens Cave / Land; Victory Road 1F / Land; Victory Road B1F / Land | party-selectable species/form |
| 377 | Regirock | SPECIES_REGIROCK | Direct acquisition | Scripted/static encounter: Desert Ruins (Literal setwildbattle acquisition) \| Authored static acquisition: Desert Ruins (regirock) | party-selectable species/form |
| 378 | Regice | SPECIES_REGICE | Direct acquisition | Scripted/static encounter: Island Cave (Literal setwildbattle acquisition) \| Authored static acquisition: Island Cave (regice) | party-selectable species/form |
| 379 | Registeel | SPECIES_REGISTEEL | Direct acquisition | Scripted/static encounter: Ancient Tomb (Literal setwildbattle acquisition) \| Authored static acquisition: Ancient Tomb (registeel) | party-selectable species/form |
| 380 | Latias | SPECIES_LATIAS | Direct acquisition | Authored static acquisition: Southern Island Interior (latias) \| Roaming encounter: Hoenn overworld after the television choice (Roamer has no fixed route percentage) | party-selectable species/form |
| 381 | Latios | SPECIES_LATIOS | Direct acquisition | Authored static acquisition: Southern Island Interior (latios) \| Roaming encounter: Hoenn overworld after the television choice (Roamer has no fixed route percentage) | party-selectable species/form |
| 382 | Kyogre | SPECIES_KYOGRE | Direct acquisition | Scripted/static encounter: Marine Cave End (Literal setwildbattle acquisition) \| Authored static acquisition: Marine Cave End (kyogre) | party-selectable species/form |
| 383 | Groudon | SPECIES_GROUDON | Direct acquisition | Scripted/static encounter: Terra Cave End (Literal setwildbattle acquisition) \| Authored static acquisition: Terra Cave End (groudon) | party-selectable species/form |
| 384 | Rayquaza | SPECIES_RAYQUAZA | Direct acquisition | Scripted/static encounter: Sky Pillar Top (Literal setwildbattle acquisition) \| Authored static acquisition: Sky Pillar Top (rayquaza) | party-selectable species/form |
| 385 | Jirachi | SPECIES_JIRACHI | Direct acquisition | Scripted/static encounter: Meteor Falls Jirachis Room (Literal setwildbattle acquisition) \| Authored static acquisition: Meteor Falls Jirachis Room (jirachi) | party-selectable species/form |
| 386 | Deoxys | SPECIES_DEOXYS | Direct acquisition | Authored static acquisition: Birth Island Exterior (deoxys) | party-selectable species/form |
| 387 | Turtwig | SPECIES_TURTWIG | Direct acquisition | Starter choice: Littleroot opening (Selectable Sinnoh starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 388 | Grotle | SPECIES_GROTLE | Evolution from obtainable Pokémon | Turtwig → Grotle via Level (16) | party-selectable species/form |
| 389 | Torterra | SPECIES_TORTERRA | Evolution from obtainable Pokémon | Turtwig → Grotle via Level (16) ; Grotle → Torterra via Level (32) | party-selectable species/form |
| 390 | Chimchar | SPECIES_CHIMCHAR | Direct acquisition | Starter choice: Littleroot opening (Selectable Sinnoh starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 391 | Monferno | SPECIES_MONFERNO | Evolution from obtainable Pokémon | Chimchar → Monferno via Level (14) | party-selectable species/form |
| 392 | Infernape | SPECIES_INFERNAPE | Evolution from obtainable Pokémon | Chimchar → Monferno via Level (14) ; Monferno → Infernape via Level (36) | party-selectable species/form |
| 393 | Piplup | SPECIES_PIPLUP | Direct acquisition | Starter choice: Littleroot opening (Selectable Sinnoh starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 394 | Prinplup | SPECIES_PRINPLUP | Evolution from obtainable Pokémon | Piplup → Prinplup via Level (16) | party-selectable species/form |
| 395 | Empoleon | SPECIES_EMPOLEON | Evolution from obtainable Pokémon | Piplup → Prinplup via Level (16) ; Prinplup → Empoleon via Level (36) | party-selectable species/form |
| 396 | Starly | SPECIES_STARLY | Direct acquisition | Random wild in 1 catchable method pool(s): Route 116 / Land | party-selectable species/form |
| 397 | Staravia | SPECIES_STARAVIA | Evolution from obtainable Pokémon | Starly → Staravia via Level (14) | party-selectable species/form |
| 398 | Staraptor | SPECIES_STARAPTOR | Evolution from obtainable Pokémon | Starly → Staravia via Level (14) ; Staravia → Staraptor via Level (34) | party-selectable species/form |
| 399 | Bidoof | SPECIES_BIDOOF | Direct acquisition | Random wild in 1 catchable method pool(s): Route 102 / Land | party-selectable species/form |
| 400 | Bibarel | SPECIES_BIBAREL | Direct acquisition | Random wild in 1 catchable method pool(s): Route 102 / Honey | party-selectable species/form |
| 401 | Kricketot | SPECIES_KRICKETOT | Direct acquisition | Random wild in 1 catchable method pool(s): Route 103 / Land | party-selectable species/form |
| 402 | Kricketune | SPECIES_KRICKETUNE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 103 / Honey | party-selectable species/form |
| 403 | Shinx | SPECIES_SHINX | Direct acquisition | Random wild in 1 catchable method pool(s): Route 103 / Land | party-selectable species/form |
| 404 | Luxio | SPECIES_LUXIO | Direct acquisition | Random wild in 1 catchable method pool(s): Route 103 / Honey | party-selectable species/form |
| 405 | Luxray | SPECIES_LUXRAY | Evolution from obtainable Pokémon | Luxio → Luxray via Level (30) | party-selectable species/form |
| 406 | Budew | SPECIES_BUDEW | Direct acquisition | Random wild in 1 catchable method pool(s): Route 104 / Land | party-selectable species/form |
| 407 | Roserade | SPECIES_ROSERADE | Evolution from obtainable Pokémon | Roselia → Roserade via Use/hold ITEM_SHINY_STONE | party-selectable species/form |
| 408 | Cranidos | SPECIES_CRANIDOS | Direct acquisition | Fossil restoration: Rustboro Devon Corporation (Restore ITEM_SKULL_FOSSIL) | party-selectable species/form |
| 409 | Rampardos | SPECIES_RAMPARDOS | Evolution from obtainable Pokémon | Cranidos → Rampardos via Level (30) | party-selectable species/form |
| 410 | Shieldon | SPECIES_SHIELDON | Direct acquisition | Fossil restoration: Rustboro Devon Corporation (Restore ITEM_ARMOR_FOSSIL) | party-selectable species/form |
| 411 | Bastiodon | SPECIES_BASTIODON | Evolution from obtainable Pokémon | Shieldon → Bastiodon via Level (30) | party-selectable species/form |
| 412 | Burmy | SPECIES_BURMY | Direct acquisition | Random wild in 1 catchable method pool(s): Berry tree encounter table 4: G Berry Stage Berries / Berry Tree | party-selectable species/form |
| 413 | Wormadam | SPECIES_WORMADAM | Evolution from obtainable Pokémon | Burmy → Wormadam via Level Female (20) | party-selectable species/form |
| 414 | Mothim | SPECIES_MOTHIM | Evolution from obtainable Pokémon | Burmy → Mothim via Level Male (20) | party-selectable species/form |
| 415 | Combee | SPECIES_COMBEE | Direct acquisition | Random wild in 2 catchable method pool(s): Berry tree encounter table 3: G Berry Stage Flowering / Berry Tree; Dewford Meadow / Land | party-selectable species/form |
| 416 | Vespiquen | SPECIES_VESPIQUEN | Evolution from obtainable Pokémon | Combee → Vespiquen via Level Female (21) | party-selectable species/form |
| 417 | Pachirisu | SPECIES_PACHIRISU | Direct acquisition | Random wild in 1 catchable method pool(s): Route 110 / Land | party-selectable species/form |
| 418 | Buizel | SPECIES_BUIZEL | Direct acquisition | Random wild in 1 catchable method pool(s): Route 106 / Land | party-selectable species/form |
| 419 | Floatzel | SPECIES_FLOATZEL | Direct acquisition | Random wild in 5 catchable method pool(s): Ever Grande City / Surf; Lilycove City / Surf; Mossdeep City / Surf; Route 105 / Land; Route 119 / Surf | party-selectable species/form |
| 420 | Cherubi | SPECIES_CHERUBI | Direct acquisition | Random wild in 2 catchable method pool(s): Berry tree encounter table 3: G Berry Stage Flowering / Berry Tree; Petalburg Woods / Rock Smash | party-selectable species/form |
| 421 | Cherrim | SPECIES_CHERRIM | Evolution from obtainable Pokémon | Cherubi → Cherrim via Level (25) | party-selectable species/form |
| 422 | Shellos | SPECIES_SHELLOS | Direct acquisition | Random wild in 2 catchable method pool(s): Route 103 / Land; Route 110 / Surf | party-selectable species/form |
| 423 | Gastrodon | SPECIES_GASTRODON | Direct acquisition | Random wild in 4 catchable method pool(s): Altering Cave / Surf; Altering Cave B1F / Surf; Route 103 / Surf; Route 110 / Surf | party-selectable species/form |
| 424 | Ambipom | SPECIES_AMBIPOM | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone Southeast / Land | party-selectable species/form |
| 425 | Drifloon | SPECIES_DRIFLOON | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Drifblim; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 426 | Drifblim | SPECIES_DRIFBLIM | Direct acquisition | Random wild in 2 catchable method pool(s): Mt Pyre Exterior / Land; Mt Pyre Summit / Land | party-selectable species/form |
| 427 | Buneary | SPECIES_BUNEARY | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods / Land | party-selectable species/form |
| 428 | Lopunny | SPECIES_LOPUNNY | Evolution from obtainable Pokémon | Buneary → Lopunny via Friendship | party-selectable species/form |
| 429 | Mismagius | SPECIES_MISMAGIUS | Evolution from obtainable Pokémon | Misdreavus → Mismagius via Use/hold ITEM_DUSK_STONE | party-selectable species/form |
| 430 | Honchkrow | SPECIES_HONCHKROW | Evolution from obtainable Pokémon | Murkrow → Honchkrow via Use/hold ITEM_DUSK_STONE | party-selectable species/form |
| 431 | Glameow | SPECIES_GLAMEOW | Direct acquisition | Random wild in 1 catchable method pool(s): Rustboro City / Land | party-selectable species/form |
| 432 | Purugly | SPECIES_PURUGLY | Evolution from obtainable Pokémon | Glameow → Purugly via Level (38) | party-selectable species/form |
| 433 | Chingling | SPECIES_CHINGLING | Direct acquisition | Random wild in 1 catchable method pool(s): Rusturf Tunnel / Land | party-selectable species/form |
| 434 | Stunky | SPECIES_STUNKY | Direct acquisition | Random wild in 1 catchable method pool(s): Route 110 / Land | party-selectable species/form |
| 435 | Skuntank | SPECIES_SKUNTANK | Evolution from obtainable Pokémon | Stunky → Skuntank via Level (34) | party-selectable species/form |
| 436 | Bronzor | SPECIES_BRONZOR | Direct acquisition | Random wild in 7 catchable method pool(s): Granite Cave B1F / Land; Granite Cave B2F / Land; Mirage Tower 1F / Land; Mirage Tower 2F / Land; Mirage Tower 3F / Land; Mirage Tower 4F / Land; Mirage Tower B1F / Land | party-selectable species/form |
| 437 | Bronzong | SPECIES_BRONZONG | Direct acquisition | Random wild in 7 catchable method pool(s): Mt Pyre Exterior / Land; Mt Pyre Summit / Honey; Mt Pyre Summit / Land; Sandstrewn Ruins / Land; Sandstrewn Ruins 2F / Land; Sandstrewn Ruins 3F / Land; Sandstrewn Ruins B1F / Land | party-selectable species/form |
| 438 | Bonsly | SPECIES_BONSLY | Direct acquisition | Random wild in 1 catchable method pool(s): Route 101 / Land | party-selectable species/form |
| 439 | Mime Jr. | SPECIES_MIME_JR | Direct acquisition | Random wild in 1 catchable method pool(s): Dewford Manor 1F / Land | party-selectable species/form |
| 440 | Happiny | SPECIES_HAPPINY | Direct acquisition | In-game trade: NPC trade (Trade for SPECIES_PYUKUMUKU; received at level 32) | party-selectable species/form |
| 441 | Chatot | SPECIES_CHATOT | Direct acquisition | Random wild in 2 catchable method pool(s): Route 105 / Land; Route 106 / Land | party-selectable species/form |
| 442 | Spiritomb | SPECIES_SPIRITOMB | Direct acquisition | Scripted/static encounter: Abandoned Ship Room B1F (Literal setwildbattle acquisition) \| Authored static acquisition: Abandoned Ship Room B1F (spiritomb) | party-selectable species/form |
| 443 | Gible | SPECIES_GIBLE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 111 / Land | party-selectable species/form |
| 444 | Gabite | SPECIES_GABITE | Direct acquisition | Random wild in 5 catchable method pool(s): Sandstrewn Ruins / Land; Sandstrewn Ruins 2F / Land; Sandstrewn Ruins 3F / Land; Sandstrewn Ruins B1F / Land; Victory Road 1F / Land | party-selectable species/form |
| 445 | Garchomp | SPECIES_GARCHOMP | Evolution from obtainable Pokémon | Gabite → Garchomp via Level (48) | party-selectable species/form |
| 446 | Munchlax | SPECIES_MUNCHLAX | Direct acquisition | Random wild in 2 catchable method pool(s): Berry tree encounter table 4: G Berry Stage Berries / Berry Tree; Route 115 / Land \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 447 | Riolu | SPECIES_RIOLU | Direct acquisition | Random wild in 1 catchable method pool(s): Route 116 / Land | party-selectable species/form |
| 448 | Lucario | SPECIES_LUCARIO | Direct acquisition | Random wild in 3 catchable method pool(s): Altering Cave / Land; Altering Cave 1F / Land; Altering Cave B1F / Land | party-selectable species/form |
| 449 | Hippopotas | SPECIES_HIPPOPOTAS | Direct acquisition | Random wild in 1 catchable method pool(s): Route 111 / Land | party-selectable species/form |
| 450 | Hippowdon | SPECIES_HIPPOWDON | Evolution from obtainable Pokémon | Hippopotas → Hippowdon via Level (34) | party-selectable species/form |
| 451 | Skorupi | SPECIES_SKORUPI | Direct acquisition | Random wild in 1 catchable method pool(s): Route 114 / Land | party-selectable species/form |
| 452 | Drapion | SPECIES_DRAPION | Evolution from obtainable Pokémon | Skorupi → Drapion via Level (40) | party-selectable species/form |
| 453 | Croagunk | SPECIES_CROAGUNK | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods 3 / Land | party-selectable species/form |
| 454 | Toxicroak | SPECIES_TOXICROAK | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone Southwest / Land | party-selectable species/form |
| 455 | Carnivine | SPECIES_CARNIVINE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 118 / Land | party-selectable species/form |
| 456 | Finneon | SPECIES_FINNEON | Direct acquisition | Random wild in 24 catchable method pool(s): Dewford Town / Good Rod; Lilycove City / Good Rod; Lilycove City / Old Rod; Mossdeep City / Good Rod; Mossdeep City / Old Rod; Route 107 / Good Rod; Route 107 / Old Rod; Route 108 / Good Rod; Route 108 / Old Rod; Route 122 / Good Rod; Route 124 / Good Rod; Route 124 / Old Rod; Route 125 / Good Rod; Route 125 / Old Rod; Route 126 / Good Rod; Route 126 / Old Rod; Route 127 / Good Rod; Route 127 / Old Rod; Route 129 / Good Rod; Route 129 / Old Rod; Route 130 / Good Rod; Route 130 / Old Rod; Route 131 / Good Rod; Route 131 / Old Rod | party-selectable species/form |
| 457 | Lumineon | SPECIES_LUMINEON | Direct acquisition | Random wild in 16 catchable method pool(s): Dewford Town / Super Rod; Lilycove City / Super Rod; Mossdeep City / Super Rod; Route 107 / Super Rod; Route 108 / Super Rod; Route 121 / Super Rod; Route 122 / Super Rod; Route 124 / Super Rod; Route 125 / Super Rod; Route 126 / Super Rod; Route 127 / Super Rod; Route 129 / Super Rod; Route 130 / Super Rod; Route 131 / Super Rod; Underwater Route 124 / Land; Underwater Route 126 / Land | party-selectable species/form |
| 458 | Mantyke | SPECIES_MANTYKE | Direct acquisition | Random wild in 3 catchable method pool(s): Route 107 / Surf; Route 108 / Surf; Route 109 / Surf | party-selectable species/form |
| 459 | Snover | SPECIES_SNOVER | Direct acquisition | Random wild in 1 catchable method pool(s): Seaspray Cave B1F / Honey | party-selectable species/form |
| 460 | Abomasnow | SPECIES_ABOMASNOW | Evolution from obtainable Pokémon | Snover → Abomasnow via Level (40) | party-selectable species/form |
| 461 | Weavile | SPECIES_WEAVILE | Evolution from obtainable Pokémon | Sneasel → Weavile via Use/hold ITEM_RAZOR_CLAW | party-selectable species/form |
| 462 | Magnezone | SPECIES_MAGNEZONE | Evolution from obtainable Pokémon | Magneton → Magnezone via Specific Mapsec (MAPSEC_NEW_MAUVILLE) | party-selectable species/form |
| 463 | Lickilicky | SPECIES_LICKILICKY | Direct acquisition | Random wild in 1 catchable method pool(s): Route 118 / Honey | party-selectable species/form |
| 464 | Rhyperior | SPECIES_RHYPERIOR | Evolution from obtainable Pokémon | Rhydon → Rhyperior via Trade Item (ITEM_PROTECTOR) | party-selectable species/form |
| 465 | Tangrowth | SPECIES_TANGROWTH | Direct acquisition | Random wild in 1 catchable method pool(s): Route 120 / Honey | party-selectable species/form |
| 466 | Electivire | SPECIES_ELECTIVIRE | Evolution from obtainable Pokémon | Electabuzz → Electivire via Trade Item (ITEM_ELECTIRIZER) | party-selectable species/form |
| 467 | Magmortar | SPECIES_MAGMORTAR | Direct acquisition | Random wild in 7 catchable method pool(s): Magma Hideout 1F / Land; Magma Hideout 2F 1R / Land; Magma Hideout 2F 2R / Land; Magma Hideout 2F 3R / Land; Magma Hideout 3F 1R / Land; Magma Hideout 3F 2R / Land; Magma Hideout 3F 3R / Land | party-selectable species/form |
| 468 | Togekiss | SPECIES_TOGEKISS | Evolution from obtainable Pokémon | Togepi → Togetic via Friendship ; Togetic → Togekiss via Use/hold ITEM_SHINY_STONE | party-selectable species/form |
| 469 | Yanmega | SPECIES_YANMEGA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 120 / Honey | party-selectable species/form |
| 470 | Leafeon | SPECIES_LEAFEON | Evolution from obtainable Pokémon | Eevee → Leafeon via Use/hold ITEM_LEAF_STONE | party-selectable species/form |
| 471 | Glaceon | SPECIES_GLACEON | Evolution from obtainable Pokémon | Eevee → Glaceon via Use/hold ITEM_ICE_STONE | party-selectable species/form |
| 472 | Gliscor | SPECIES_GLISCOR | Evolution from obtainable Pokémon | Gligar → Gliscor via Use/hold ITEM_RAZOR_FANG | party-selectable species/form |
| 473 | Mamoswine | SPECIES_MAMOSWINE | Evolution from obtainable Pokémon | Swinub → Piloswine via Level (33) ; Piloswine → Mamoswine via Level while knowing MOVE_ANCIENT_POWER | party-selectable species/form |
| 474 | Porygon-Z | SPECIES_PORYGON_Z | Evolution from obtainable Pokémon | Porygon → Porygon2 via Trade Item (ITEM_UPGRADE) ; Porygon2 → Porygon-Z via Trade Item (ITEM_DUBIOUS_DISC) | party-selectable species/form |
| 475 | Gallade | SPECIES_GALLADE | Evolution from obtainable Pokémon | Kirlia → Gallade via Use/hold ITEM_DAWN_STONE | party-selectable species/form |
| 476 | Probopass | SPECIES_PROBOPASS | Evolution from obtainable Pokémon | Nosepass → Probopass via Use/hold ITEM_THUNDER_STONE | party-selectable species/form |
| 477 | Dusknoir | SPECIES_DUSKNOIR | Evolution from obtainable Pokémon | Dusclops → Dusknoir via Trade Item (ITEM_REAPER_CLOTH) | party-selectable species/form |
| 478 | Froslass | SPECIES_FROSLASS | Evolution from obtainable Pokémon | Snorunt → Froslass via Use/hold ITEM_DAWN_STONE | party-selectable species/form |
| 479 | Rotom | SPECIES_ROTOM | Direct acquisition | Scripted/static encounter: New Mauville Inside (Literal setwildbattle acquisition) \| Authored static acquisition: New Mauville Inside (rotom) | party-selectable species/form |
| 480 | Uxie | SPECIES_UXIE | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 481 | Mesprit | SPECIES_MESPRIT | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 482 | Azelf | SPECIES_AZELF | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 483 | Dialga | SPECIES_DIALGA | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 484 | Palkia | SPECIES_PALKIA | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 485 | Heatran | SPECIES_HEATRAN | Direct acquisition | Scripted/static encounter: Scorched Slab Heatrans Room (Literal setwildbattle acquisition) \| Authored static acquisition: Scorched Slab Heatrans Room (heatran) | party-selectable species/form |
| 486 | Regigigas | SPECIES_REGIGIGAS | Direct acquisition | Scripted/static encounter: Sealed Chamber Inner Room (Literal setwildbattle acquisition) \| Authored static acquisition: Sealed Chamber Inner Room (regigigas) | party-selectable species/form |
| 487 | Giratina | SPECIES_GIRATINA | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 488 | Cresselia | SPECIES_CRESSELIA | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 489 | Phione | SPECIES_PHIONE | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 490 | Manaphy | SPECIES_MANAPHY | Direct acquisition | Random wild in 4 catchable method pool(s): Route 124 / Super Rod; Seafloor Cavern Entrance / Super Rod; Seafloor Cavern Room6 / Super Rod; Seafloor Cavern Room7 / Super Rod | party-selectable species/form |
| 491 | Darkrai | SPECIES_DARKRAI | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 492 | Shaymin | SPECIES_SHAYMIN | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 493 | Arceus | SPECIES_ARCEUS | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 494 | Victini | SPECIES_VICTINI | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 495 | Snivy | SPECIES_SNIVY | Direct acquisition | Starter choice: Littleroot opening (Selectable Unova starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 496 | Servine | SPECIES_SERVINE | Evolution from obtainable Pokémon | Snivy → Servine via Level (16) | party-selectable species/form |
| 497 | Serperior | SPECIES_SERPERIOR | Evolution from obtainable Pokémon | Snivy → Servine via Level (16) ; Servine → Serperior via Level (36) | party-selectable species/form |
| 498 | Tepig | SPECIES_TEPIG | Direct acquisition | Starter choice: Littleroot opening (Selectable Unova starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 499 | Pignite | SPECIES_PIGNITE | Evolution from obtainable Pokémon | Tepig → Pignite via Level (16) | party-selectable species/form |
| 500 | Emboar | SPECIES_EMBOAR | Direct acquisition | Random wild in 1 catchable method pool(s): Magma Hideout 4F / Land | party-selectable species/form |
| 501 | Oshawott | SPECIES_OSHAWOTT | Direct acquisition | Starter choice: Littleroot opening (Selectable Unova starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 502 | Dewott | SPECIES_DEWOTT | Evolution from obtainable Pokémon | Oshawott → Dewott via Level (16) | party-selectable species/form |
| 503 | Samurott | SPECIES_SAMUROTT | Evolution from obtainable Pokémon | Oshawott → Dewott via Level (16) ; Dewott → Samurott via Level (36) | party-selectable species/form |
| 504 | Patrat | SPECIES_PATRAT | Direct acquisition | Random wild in 1 catchable method pool(s): Route 104 / Honey | party-selectable species/form |
| 505 | Watchog | SPECIES_WATCHOG | Direct acquisition | Random wild in 1 catchable method pool(s): Route 120 / Land | party-selectable species/form |
| 506 | Lillipup | SPECIES_LILLIPUP | Direct acquisition | Random wild in 1 catchable method pool(s): Route 101 / Land | party-selectable species/form |
| 507 | Herdier | SPECIES_HERDIER | Evolution from obtainable Pokémon | Lillipup → Herdier via Level (16) | party-selectable species/form |
| 508 | Stoutland | SPECIES_STOUTLAND | Evolution from obtainable Pokémon | Lillipup → Herdier via Level (16) ; Herdier → Stoutland via Level (32) | party-selectable species/form |
| 509 | Purrloin | SPECIES_PURRLOIN | Direct acquisition | Random wild in 1 catchable method pool(s): Route 116 / Honey | party-selectable species/form |
| 510 | Liepard | SPECIES_LIEPARD | Direct acquisition | Random wild in 1 catchable method pool(s): Route 118 / Land | party-selectable species/form |
| 511 | Pansage | SPECIES_PANSAGE | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods 2 / Land | party-selectable species/form |
| 512 | Simisage | SPECIES_SIMISAGE | Evolution from obtainable Pokémon | Pansage → Simisage via Use/hold ITEM_LEAF_STONE | party-selectable species/form |
| 513 | Pansear | SPECIES_PANSEAR | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods 2 / Land | party-selectable species/form |
| 514 | Simisear | SPECIES_SIMISEAR | Evolution from obtainable Pokémon | Pansear → Simisear via Use/hold ITEM_FIRE_STONE | party-selectable species/form |
| 515 | Panpour | SPECIES_PANPOUR | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods 2 / Land | party-selectable species/form |
| 516 | Simipour | SPECIES_SIMIPOUR | Evolution from obtainable Pokémon | Panpour → Simipour via Use/hold ITEM_WATER_STONE | party-selectable species/form |
| 517 | Munna | SPECIES_MUNNA | Direct acquisition | Random wild in 1 catchable method pool(s): Verdanturf Meadow / Land | party-selectable species/form |
| 518 | Musharna | SPECIES_MUSHARNA | Evolution from obtainable Pokémon | Munna → Musharna via Use/hold ITEM_MOON_STONE | party-selectable species/form |
| 519 | Pidove | SPECIES_PIDOVE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 104 / Land | party-selectable species/form |
| 520 | Tranquill | SPECIES_TRANQUILL | Evolution from obtainable Pokémon | Pidove → Tranquill via Level (21) | party-selectable species/form |
| 521 | Unfezant | SPECIES_UNFEZANT | Evolution from obtainable Pokémon | Pidove → Tranquill via Level (21) ; Tranquill → Unfezant via Level Male (32) | party-selectable species/form |
| 522 | Blitzle | SPECIES_BLITZLE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 103 / Land | party-selectable species/form |
| 523 | Zebstrika | SPECIES_ZEBSTRIKA | Evolution from obtainable Pokémon | Blitzle → Zebstrika via Level (27) | party-selectable species/form |
| 524 | Roggenrola | SPECIES_ROGGENROLA | Direct acquisition | Random wild in 1 catchable method pool(s): Rusturf Tunnel / Land | party-selectable species/form |
| 525 | Boldore | SPECIES_BOLDORE | Direct acquisition | Random wild in 17 catchable method pool(s): Cave Of Origin 1F / Land; Cave Of Origin Entrance / Land; Cave Of Origin Unused Ruby Sapphire Map1 / Land; Cave Of Origin Unused Ruby Sapphire Map2 / Land; Cave Of Origin Unused Ruby Sapphire Map3 / Land; Ember Path / Land; Magma Hideout 1F / Land; Magma Hideout 2F 1R / Land; Magma Hideout 2F 2R / Land; Magma Hideout 2F 3R / Land; Magma Hideout 3F 1R / Land; Magma Hideout 3F 2R / Land; Magma Hideout 3F 3R / Land; Magma Hideout 4F / Land; Scorched Slab B1F / Land; Scorched Slab B2F / Land; Scorched Slab Heatrans Room / Land | party-selectable species/form |
| 526 | Gigalith | SPECIES_GIGALITH | Direct acquisition | Random wild in 6 catchable method pool(s): Altering Cave / Land; Altering Cave 1F / Land; Altering Cave B1F / Land; Cave Of Origin Unused Ruby Sapphire Map1 / Land; Cave Of Origin Unused Ruby Sapphire Map2 / Land; Cave Of Origin Unused Ruby Sapphire Map3 / Land | party-selectable species/form |
| 527 | Woobat | SPECIES_WOOBAT | Direct acquisition | Random wild in 1 catchable method pool(s): Seaspray Cave / Land | party-selectable species/form |
| 528 | Swoobat | SPECIES_SWOOBAT | Direct acquisition | Random wild in 3 catchable method pool(s): Altering Cave / Land; Altering Cave 1F / Land; Altering Cave B1F / Land | party-selectable species/form |
| 529 | Drilbur | SPECIES_DRILBUR | Direct acquisition | Random wild in 1 catchable method pool(s): Rusturf Tunnel / Land | party-selectable species/form |
| 530 | Excadrill | SPECIES_EXCADRILL | Direct acquisition | Random wild in 8 catchable method pool(s): Magma Hideout 1F / Land; Magma Hideout 2F 1R / Land; Magma Hideout 2F 2R / Land; Magma Hideout 2F 3R / Land; Magma Hideout 3F 1R / Land; Magma Hideout 3F 2R / Land; Magma Hideout 3F 3R / Land; Magma Hideout 4F / Land | party-selectable species/form |
| 531 | Audino | SPECIES_AUDINO | Direct acquisition | Random wild in 25 catchable method pool(s): Ashen Woods / Honey; Dewford Meadow / Honey; Jagged Pass / Honey; Mt Pyre Exterior / Honey; Mt Pyre Summit / Honey; Petalburg Woods / Honey; Petalburg Woods 2 / Honey; Petalburg Woods 3 / Honey; Route 101 / Honey; Route 102 / Honey; Route 103 / Honey; Route 104 / Honey; Route 110 / Honey; Route 112 / Honey; Route 114 / Honey; Route 115 / Honey; Route 116 / Honey; Route 117 / Honey; Route 118 / Honey; Route 119 / Honey; Route 120 / Honey; Route 121 / Honey; Route 123 / Honey; Rustboro City / Honey; Verdanturf Meadow / Honey \| NPC or story gift: Verdanturf Meadow (Literal givemon acquisition) | party-selectable species/form |
| 532 | Timburr | SPECIES_TIMBURR | Direct acquisition | Random wild in 2 catchable method pool(s): Granite Cave 1F / Land; Granite Cave Stevens Room / Land | party-selectable species/form |
| 533 | Gurdurr | SPECIES_GURDURR | Direct acquisition | Random wild in 3 catchable method pool(s): Scorched Slab B1F / Land; Scorched Slab B2F / Land; Victory Road B1F / Land | party-selectable species/form |
| 534 | Conkeldurr | SPECIES_CONKELDURR | Evolution from obtainable Pokémon | Gurdurr → Conkeldurr via Friendship | party-selectable species/form |
| 535 | Tympole | SPECIES_TYMPOLE | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Palpitoad; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 536 | Palpitoad | SPECIES_PALPITOAD | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone Southwest / Land | party-selectable species/form |
| 537 | Seismitoad | SPECIES_SEISMITOAD | Direct acquisition | Random wild in 2 catchable method pool(s): Safari Zone Southwest / Land; Safari Zone Southwest / Surf | party-selectable species/form |
| 538 | Throh | SPECIES_THROH | Direct acquisition | Random wild in 2 catchable method pool(s): Route 112 / Honey; Route 112 / Land | party-selectable species/form |
| 539 | Sawk | SPECIES_SAWK | Direct acquisition | Random wild in 2 catchable method pool(s): Route 112 / Honey; Route 112 / Land | party-selectable species/form |
| 540 | Sewaddle | SPECIES_SEWADDLE | Direct acquisition | Random wild in 2 catchable method pool(s): Berry tree encounter table 1: G Berry Stage Sprouted / Berry Tree; Route 101 / Land | party-selectable species/form |
| 541 | Swadloon | SPECIES_SWADLOON | Direct acquisition | Random wild in 1 catchable method pool(s): Route 101 / Honey | party-selectable species/form |
| 542 | Leavanny | SPECIES_LEAVANNY | Evolution from obtainable Pokémon | Swadloon → Leavanny via Friendship | party-selectable species/form |
| 543 | Venipede | SPECIES_VENIPEDE | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods 2 / Land | party-selectable species/form |
| 544 | Whirlipede | SPECIES_WHIRLIPEDE | Evolution from obtainable Pokémon | Venipede → Whirlipede via Level (22) | party-selectable species/form |
| 545 | Scolipede | SPECIES_SCOLIPEDE | Evolution from obtainable Pokémon | Venipede → Whirlipede via Level (22) ; Whirlipede → Scolipede via Level (30) | party-selectable species/form |
| 546 | Cottonee | SPECIES_COTTONEE | Direct acquisition | Random wild in 1 catchable method pool(s): Rustboro City / Land | party-selectable species/form |
| 547 | Whimsicott | SPECIES_WHIMSICOTT | Evolution from obtainable Pokémon | Cottonee → Whimsicott via Use/hold ITEM_SUN_STONE | party-selectable species/form |
| 548 | Petilil | SPECIES_PETILIL | Direct acquisition | Random wild in 1 catchable method pool(s): Rustboro City / Land | party-selectable species/form |
| 549 | Lilligant | SPECIES_LILLIGANT | Evolution from obtainable Pokémon | Petilil → Lilligant via Use/hold ITEM_SUN_STONE | party-selectable species/form |
| 550 | Basculin | SPECIES_BASCULIN | Direct acquisition | Random wild in 5 catchable method pool(s): Route 111 / Good Rod; Route 111 / Super Rod; Route 118 / Super Rod; Victory Road B2F / Good Rod; Victory Road B2F / Super Rod | party-selectable species/form |
| 551 | Sandile | SPECIES_SANDILE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 111 / Land | party-selectable species/form |
| 552 | Krokorok | SPECIES_KROKOROK | Evolution from obtainable Pokémon | Sandile → Krokorok via Level (29) | party-selectable species/form |
| 553 | Krookodile | SPECIES_KROOKODILE | Evolution from obtainable Pokémon | Sandile → Krokorok via Level (29) ; Krokorok → Krookodile via Level (40) | party-selectable species/form |
| 554 | Darumaka | SPECIES_DARUMAKA | Direct acquisition | Random wild in 9 catchable method pool(s): Mirage Tower 1F / Land; Mirage Tower 2F / Land; Mirage Tower 3F / Land; Mirage Tower 4F / Land; Mirage Tower B1F / Land; Sandstrewn Ruins / Land; Sandstrewn Ruins 2F / Land; Sandstrewn Ruins 3F / Land; Sandstrewn Ruins B1F / Land | party-selectable species/form |
| 555 | Darmanitan | SPECIES_DARMANITAN | Evolution from obtainable Pokémon | Darumaka → Darmanitan via Level (35) | party-selectable species/form |
| 556 | Maractus | SPECIES_MARACTUS | Direct acquisition | Random wild in 1 catchable method pool(s): Route 111 / Land | party-selectable species/form |
| 557 | Dwebble | SPECIES_DWEBBLE | Direct acquisition | Random wild in 5 catchable method pool(s): Dewford Town / Rock Smash; Granite Cave B2F / Rock Smash; Lilycove City / Rock Smash; Route 106 / Rock Smash; Seaspray Cave / Rock Smash | party-selectable species/form |
| 558 | Crustle | SPECIES_CRUSTLE | Direct acquisition | Random wild in 3 catchable method pool(s): Altering Cave / Rock Smash; Altering Cave 1F / Rock Smash; Altering Cave B1F / Rock Smash | party-selectable species/form |
| 559 | Scraggy | SPECIES_SCRAGGY | Direct acquisition | Random wild in 1 catchable method pool(s): Route 113 / Land | party-selectable species/form |
| 560 | Scrafty | SPECIES_SCRAFTY | Evolution from obtainable Pokémon | Scraggy → Scrafty via Level (39) | party-selectable species/form |
| 561 | Sigilyph | SPECIES_SIGILYPH | Direct acquisition | Random wild in 9 catchable method pool(s): Mirage Tower 1F / Land; Mirage Tower 2F / Land; Mirage Tower 3F / Land; Mirage Tower 4F / Land; Mirage Tower B1F / Land; Sandstrewn Ruins / Land; Sandstrewn Ruins 2F / Land; Sandstrewn Ruins 3F / Land; Sandstrewn Ruins B1F / Land | party-selectable species/form |
| 562 | Yamask | SPECIES_YAMASK | Direct acquisition | Random wild in 9 catchable method pool(s): Mirage Tower 1F / Land; Mirage Tower 2F / Land; Mirage Tower 3F / Land; Mirage Tower 4F / Land; Mirage Tower B1F / Land; Sandstrewn Ruins / Land; Sandstrewn Ruins 2F / Land; Sandstrewn Ruins 3F / Land; Sandstrewn Ruins B1F / Land | party-selectable species/form |
| 563 | Cofagrigus | SPECIES_COFAGRIGUS | Evolution from obtainable Pokémon | Yamask → Cofagrigus via Level (34) | party-selectable species/form |
| 564 | Tirtouga | SPECIES_TIRTOUGA | Direct acquisition | Fossil restoration: Rustboro Devon Corporation (Restore ITEM_COVER_FOSSIL) | party-selectable species/form |
| 565 | Carracosta | SPECIES_CARRACOSTA | Evolution from obtainable Pokémon | Tirtouga → Carracosta via Level (37) | party-selectable species/form |
| 566 | Archen | SPECIES_ARCHEN | Direct acquisition | Fossil restoration: Rustboro Devon Corporation (Restore ITEM_PLUME_FOSSIL) | party-selectable species/form |
| 567 | Archeops | SPECIES_ARCHEOPS | Evolution from obtainable Pokémon | Archen → Archeops via Level (37) | party-selectable species/form |
| 568 | Trubbish | SPECIES_TRUBBISH | Direct acquisition | Random wild in 1 catchable method pool(s): Route 110 / Land | party-selectable species/form |
| 569 | Garbodor | SPECIES_GARBODOR | Evolution from obtainable Pokémon | Trubbish → Garbodor via Level (36) | party-selectable species/form |
| 570 | Zorua | SPECIES_ZORUA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 118 / Land | party-selectable species/form |
| 571 | Zoroark | SPECIES_ZOROARK | Direct acquisition | Random wild in 1 catchable method pool(s): Route 121 / Land | party-selectable species/form |
| 572 | Minccino | SPECIES_MINCCINO | Direct acquisition | Random wild in 1 catchable method pool(s): Route 117 / Land | party-selectable species/form |
| 573 | Cinccino | SPECIES_CINCCINO | Evolution from obtainable Pokémon | Minccino → Cinccino via Use/hold ITEM_SHINY_STONE | party-selectable species/form |
| 574 | Gothita | SPECIES_GOTHITA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 102 / Land | party-selectable species/form |
| 575 | Gothorita | SPECIES_GOTHORITA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 102 / Honey | party-selectable species/form |
| 576 | Gothitelle | SPECIES_GOTHITELLE | Evolution from obtainable Pokémon | Gothorita → Gothitelle via Level (41) | party-selectable species/form |
| 577 | Solosis | SPECIES_SOLOSIS | Direct acquisition | Random wild in 1 catchable method pool(s): Dewford Manor 1F / Land | party-selectable species/form |
| 578 | Duosion | SPECIES_DUOSION | Evolution from obtainable Pokémon | Solosis → Duosion via Level (32) | party-selectable species/form |
| 579 | Reuniclus | SPECIES_REUNICLUS | Evolution from obtainable Pokémon | Solosis → Duosion via Level (32) ; Duosion → Reuniclus via Level (41) | party-selectable species/form |
| 580 | Ducklett | SPECIES_DUCKLETT | Direct acquisition | Random wild in 1 catchable method pool(s): Route 114 / Land | party-selectable species/form |
| 581 | Swanna | SPECIES_SWANNA | Evolution from obtainable Pokémon | Ducklett → Swanna via Level (35) | party-selectable species/form |
| 582 | Vanillite | SPECIES_VANILLITE | Direct acquisition | Random wild in 1 catchable method pool(s): Seaspray Cave B1F / Land | party-selectable species/form |
| 583 | Vanillish | SPECIES_VANILLISH | Evolution from obtainable Pokémon | Vanillite → Vanillish via Level (25) | party-selectable species/form |
| 584 | Vanilluxe | SPECIES_VANILLUXE | Evolution from obtainable Pokémon | Vanillite → Vanillish via Level (25) ; Vanillish → Vanilluxe via Level (37) | party-selectable species/form |
| 585 | Deerling | SPECIES_DEERLING | Direct acquisition | Random wild in 1 catchable method pool(s): Route 117 / Land | party-selectable species/form |
| 586 | Sawsbuck | SPECIES_SAWSBUCK | Evolution from obtainable Pokémon | Deerling → Sawsbuck via Level (34) | party-selectable species/form |
| 587 | Emolga | SPECIES_EMOLGA | Direct acquisition | Random wild in 2 catchable method pool(s): Petalburg Woods 3 / Honey; Petalburg Woods 3 / Land | party-selectable species/form |
| 588 | Karrablast | SPECIES_KARRABLAST | Direct acquisition | Random wild in 3 catchable method pool(s): Route 117 / Honey; Route 123 / Land; Safari Zone Southwest / Land \| In-game trade: NPC trade (Trade for SPECIES_SHELMET; received at level 50) | party-selectable species/form |
| 589 | Escavalier | SPECIES_ESCAVALIER | Direct acquisition | Random wild in 1 catchable method pool(s): Route 123 / Honey | party-selectable species/form |
| 590 | Foongus | SPECIES_FOONGUS | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods / Land | party-selectable species/form |
| 591 | Amoonguss | SPECIES_AMOONGUSS | Direct acquisition | Random wild in 2 catchable method pool(s): Route 119 / Honey; Route 119 / Land | party-selectable species/form |
| 592 | Frillish | SPECIES_FRILLISH | Direct acquisition | Random wild in 18 catchable method pool(s): Abandoned Ship Hidden Floor Corridors / Good Rod; Abandoned Ship Hidden Floor Corridors / Old Rod; Abandoned Ship Hidden Floor Corridors / Surf; Abandoned Ship Rooms B1F / Good Rod; Abandoned Ship Rooms B1F / Old Rod; Abandoned Ship Rooms B1F / Surf; Dewford Town / Surf; Route 107 / Surf; Route 108 / Surf; Route 109 / Surf; Route 121 / Surf; Route 122 / Surf; Route 124 / Good Rod; Route 125 / Good Rod; Route 126 / Good Rod; Route 127 / Good Rod; Slateport City / Good Rod; Slateport City / Old Rod | party-selectable species/form |
| 593 | Jellicent | SPECIES_JELLICENT | Direct acquisition | Random wild in 13 catchable method pool(s): Abandoned Ship Hidden Floor Corridors / Super Rod; Abandoned Ship Hidden Floor Corridors / Surf; Abandoned Ship Rooms B1F / Super Rod; Abandoned Ship Rooms B1F / Surf; Route 124 / Surf; Route 125 / Surf; Route 126 / Surf; Route 127 / Surf; Route 128 / Surf; Route 132 / Surf; Route 133 / Surf; Route 134 / Surf; Slateport City / Super Rod | party-selectable species/form |
| 594 | Alomomola | SPECIES_ALOMOMOLA | Direct acquisition | Random wild in 26 catchable method pool(s): Route 121 / Surf; Route 122 / Surf; Route 124 / Good Rod; Route 124 / Super Rod; Route 124 / Surf; Route 125 / Good Rod; Route 125 / Super Rod; Route 125 / Surf; Route 126 / Good Rod; Route 126 / Super Rod; Route 126 / Surf; Route 127 / Good Rod; Route 127 / Super Rod; Route 127 / Surf; Route 128 / Surf; Route 129 / Good Rod; Route 129 / Super Rod; Route 129 / Surf; Route 130 / Good Rod; Route 130 / Super Rod; Route 130 / Surf; Route 131 / Good Rod; Route 131 / Super Rod; Route 131 / Surf; Slateport City / Good Rod; Slateport City / Super Rod | party-selectable species/form |
| 595 | Joltik | SPECIES_JOLTIK | Direct acquisition | Random wild in 1 catchable method pool(s): Route 116 / Land | party-selectable species/form |
| 596 | Galvantula | SPECIES_GALVANTULA | Evolution from obtainable Pokémon | Joltik → Galvantula via Level (36) | party-selectable species/form |
| 597 | Ferroseed | SPECIES_FERROSEED | Direct acquisition | Random wild in 1 catchable method pool(s): Meteor Falls 1F 1R / Land | party-selectable species/form |
| 598 | Ferrothorn | SPECIES_FERROTHORN | Direct acquisition | Random wild in 3 catchable method pool(s): Meteor Falls 1F 2R / Land; Meteor Falls B1F 1R / Land; Meteor Falls Stevens Cave / Land | party-selectable species/form |
| 599 | Klink | SPECIES_KLINK | Direct acquisition | Random wild in 2 catchable method pool(s): New Mauville Entrance / Land; New Mauville Inside / Land | party-selectable species/form |
| 600 | Klang | SPECIES_KLANG | Direct acquisition | Random wild in 2 catchable method pool(s): New Mauville Entrance / Land; New Mauville Inside / Land | party-selectable species/form |
| 601 | Klinklang | SPECIES_KLINKLANG | Evolution from obtainable Pokémon | Klang → Klinklang via Level (39) | party-selectable species/form |
| 602 | Tynamo | SPECIES_TYNAMO | Direct acquisition | Random wild in 1 catchable method pool(s): Seaspray Cave / Land | party-selectable species/form |
| 603 | Eelektrik | SPECIES_EELEKTRIK | Direct acquisition | Random wild in 1 catchable method pool(s): Seaspray Cave / Surf | party-selectable species/form |
| 604 | Eelektross | SPECIES_EELEKTROSS | Direct acquisition | Random wild in 2 catchable method pool(s): Altering Cave / Land; Altering Cave B1F / Land | party-selectable species/form |
| 605 | Elgyem | SPECIES_ELGYEM | Direct acquisition | Random wild in 1 catchable method pool(s): Route 121 / Land | party-selectable species/form |
| 606 | Beheeyem | SPECIES_BEHEEYEM | Direct acquisition | Random wild in 2 catchable method pool(s): Mt Pyre Exterior / Land; Mt Pyre Summit / Land | party-selectable species/form |
| 607 | Litwick | SPECIES_LITWICK | Direct acquisition | Random wild in 4 catchable method pool(s): Dewford Manor 1F / Land; Mt Pyre 1F / Land; Mt Pyre 2F / Land; Mt Pyre 3F / Land | party-selectable species/form |
| 608 | Lampent | SPECIES_LAMPENT | Direct acquisition | Random wild in 3 catchable method pool(s): Mt Pyre 4F / Land; Mt Pyre 5F / Land; Mt Pyre 6F / Land | party-selectable species/form |
| 609 | Chandelure | SPECIES_CHANDELURE | Evolution from obtainable Pokémon | Lampent → Chandelure via Use/hold ITEM_DUSK_STONE | party-selectable species/form |
| 610 | Axew | SPECIES_AXEW | Direct acquisition | Random wild in 2 catchable method pool(s): Granite Cave 1F / Land; Granite Cave Stevens Room / Land | party-selectable species/form |
| 611 | Fraxure | SPECIES_FRAXURE | Evolution from obtainable Pokémon | Axew → Fraxure via Level (38) | party-selectable species/form |
| 612 | Haxorus | SPECIES_HAXORUS | Evolution from obtainable Pokémon | Axew → Fraxure via Level (38) ; Fraxure → Haxorus via Level (48) | party-selectable species/form |
| 613 | Cubchoo | SPECIES_CUBCHOO | Direct acquisition | Random wild in 5 catchable method pool(s): Seaspray Cave B1F / Honey; Shoal Cave Low Tide Entrance Room / Land; Shoal Cave Low Tide Inner Room / Land; Shoal Cave Low Tide Lower Room / Land; Shoal Cave Low Tide Stairs Room / Land | party-selectable species/form |
| 614 | Beartic | SPECIES_BEARTIC | Direct acquisition | Random wild in 1 catchable method pool(s): Shoal Cave Low Tide Ice Room / Land | party-selectable species/form |
| 615 | Cryogonal | SPECIES_CRYOGONAL | Direct acquisition | Random wild in 2 catchable method pool(s): Seaspray Cave B1F / Honey; Shoal Cave Low Tide Ice Room / Land | party-selectable species/form |
| 616 | Shelmet | SPECIES_SHELMET | Direct acquisition | Random wild in 3 catchable method pool(s): Route 117 / Honey; Route 123 / Land; Safari Zone Southwest / Land | party-selectable species/form |
| 617 | Accelgor | SPECIES_ACCELGOR | Direct acquisition | Random wild in 1 catchable method pool(s): Route 123 / Honey | party-selectable species/form |
| 618 | Stunfisk | SPECIES_STUNFISK | Direct acquisition | Random wild in 5 catchable method pool(s): Route 120 / Good Rod; Route 120 / Super Rod; Route 120 / Surf; Safari Zone Southwest / Surf; Seaspray Cave / Land | party-selectable species/form |
| 619 | Mienfoo | SPECIES_MIENFOO | Direct acquisition | Random wild in 1 catchable method pool(s): Route 113 / Land | party-selectable species/form |
| 620 | Mienshao | SPECIES_MIENSHAO | Direct acquisition | Random wild in 1 catchable method pool(s): Route 113 / Honey | party-selectable species/form |
| 621 | Druddigon | SPECIES_DRUDDIGON | Direct acquisition | Random wild in 7 catchable method pool(s): Altering Cave / Land; Altering Cave 1F / Land; Altering Cave B1F / Land; Meteor Falls 1F 1R / Land; Meteor Falls 1F 2R / Land; Meteor Falls B1F 1R / Land; Meteor Falls Stevens Cave / Land | party-selectable species/form |
| 622 | Golett | SPECIES_GOLETT | Direct acquisition | Random wild in 9 catchable method pool(s): Mirage Tower 1F / Land; Mirage Tower 2F / Land; Mirage Tower 3F / Land; Mirage Tower 4F / Land; Mirage Tower B1F / Land; Sandstrewn Ruins / Land; Sandstrewn Ruins 2F / Land; Sandstrewn Ruins 3F / Land; Sandstrewn Ruins B1F / Land | party-selectable species/form |
| 623 | Golurk | SPECIES_GOLURK | Direct acquisition | Random wild in 3 catchable method pool(s): Sky Pillar 1F / Land; Sky Pillar 3F / Land; Sky Pillar 5F / Land | party-selectable species/form |
| 624 | Pawniard | SPECIES_PAWNIARD | Direct acquisition | Random wild in 1 catchable method pool(s): Route 113 / Land \| In-game trade: NPC trade (Trade for SPECIES_GOTHITA; received at level 15) | party-selectable species/form |
| 625 | Bisharp | SPECIES_BISHARP | Evolution from obtainable Pokémon | Pawniard → Bisharp via Level (42) | party-selectable species/form |
| 626 | Bouffalant | SPECIES_BOUFFALANT | Direct acquisition | Random wild in 1 catchable method pool(s): Route 113 / Land | party-selectable species/form |
| 627 | Rufflet | SPECIES_RUFFLET | Direct acquisition | Random wild in 1 catchable method pool(s): Route 112 / Land | party-selectable species/form |
| 628 | Braviary | SPECIES_BRAVIARY | Evolution from obtainable Pokémon | Rufflet → Braviary via Level (34) | party-selectable species/form |
| 629 | Vullaby | SPECIES_VULLABY | Direct acquisition | Random wild in 1 catchable method pool(s): Route 112 / Land | party-selectable species/form |
| 630 | Mandibuzz | SPECIES_MANDIBUZZ | Evolution from obtainable Pokémon | Vullaby → Mandibuzz via Level (34) | party-selectable species/form |
| 631 | Heatmor | SPECIES_HEATMOR | Direct acquisition | Random wild in 1 catchable method pool(s): Fiery Path / Land | party-selectable species/form |
| 632 | Durant | SPECIES_DURANT | Direct acquisition | Random wild in 1 catchable method pool(s): Fiery Path / Land | party-selectable species/form |
| 633 | Deino | SPECIES_DEINO | Direct acquisition | Random wild in 2 catchable method pool(s): Jagged Pass / Land; Meteor Falls 1F 1R / Land | party-selectable species/form |
| 634 | Zweilous | SPECIES_ZWEILOUS | Direct acquisition | Random wild in 4 catchable method pool(s): Meteor Falls 1F 2R / Land; Meteor Falls B1F 1R / Land; Scorched Slab B1F / Land; Victory Road B2F / Land | party-selectable species/form |
| 635 | Hydreigon | SPECIES_HYDREIGON | Direct acquisition | Random wild in 1 catchable method pool(s): Victory Road B2F / Land | party-selectable species/form |
| 636 | Larvesta | SPECIES_LARVESTA | Direct acquisition | Random wild in 6 catchable method pool(s): Ember Path / Land; Fiery Path / Land; Granite Cave Stevens Room / Land; Route 101 / Land; Route 119 / Land; Rusturf Tunnel / Land | party-selectable species/form |
| 637 | Volcarona | SPECIES_VOLCARONA | Direct acquisition | Random wild in 9 catchable method pool(s): Magma Hideout 1F / Land; Magma Hideout 2F 1R / Land; Magma Hideout 2F 2R / Land; Magma Hideout 2F 3R / Land; Magma Hideout 3F 1R / Land; Magma Hideout 3F 2R / Land; Magma Hideout 3F 3R / Land; Victory Road 1F / Land; Victory Road B2F / Land | party-selectable species/form |
| 638 | Cobalion | SPECIES_COBALION | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 639 | Terrakion | SPECIES_TERRAKION | Direct acquisition | Random wild in 1 catchable method pool(s): Victory Road B1F / Land | party-selectable species/form |
| 640 | Virizion | SPECIES_VIRIZION | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 641 | Tornadus | SPECIES_TORNADUS | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 642 | Thundurus | SPECIES_THUNDURUS | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 643 | Reshiram | SPECIES_RESHIRAM | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 644 | Zekrom | SPECIES_ZEKROM | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 645 | Landorus | SPECIES_LANDORUS | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 646 | Kyurem | SPECIES_KYUREM | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 647 | Keldeo | SPECIES_KELDEO | Direct acquisition | Random wild in 1 catchable method pool(s): Route 127 / Super Rod | party-selectable species/form |
| 648 | Meloetta | SPECIES_MELOETTA | Direct acquisition | Pokémon Center mystery gift: Pokémon Centers (Requires FLAG_BADGE05_GET; arrives holding ITEM_RELIC_STATUE) | party-selectable species/form |
| 649 | Genesect | SPECIES_GENESECT | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 650 | Chespin | SPECIES_CHESPIN | Direct acquisition | Starter choice: Littleroot opening (Selectable Kalos starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 651 | Quilladin | SPECIES_QUILLADIN | Evolution from obtainable Pokémon | Chespin → Quilladin via Level (16) | party-selectable species/form |
| 652 | Chesnaught | SPECIES_CHESNAUGHT | Evolution from obtainable Pokémon | Chespin → Quilladin via Level (16) ; Quilladin → Chesnaught via Level (36) | party-selectable species/form |
| 653 | Fennekin | SPECIES_FENNEKIN | Direct acquisition | Starter choice: Littleroot opening (Selectable Kalos starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 654 | Braixen | SPECIES_BRAIXEN | Evolution from obtainable Pokémon | Fennekin → Braixen via Level (16) | party-selectable species/form |
| 655 | Delphox | SPECIES_DELPHOX | Evolution from obtainable Pokémon | Fennekin → Braixen via Level (16) ; Braixen → Delphox via Level (36) | party-selectable species/form |
| 656 | Froakie | SPECIES_FROAKIE | Direct acquisition | Starter choice: Littleroot opening (Selectable Kalos starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 657 | Frogadier | SPECIES_FROGADIER | Evolution from obtainable Pokémon | Froakie → Frogadier via Level (16) | party-selectable species/form |
| 658 | Greninja | SPECIES_GRENINJA | Direct acquisition | Random wild in 8 catchable method pool(s): Seafloor Cavern Room1 / Land; Seafloor Cavern Room2 / Land; Seafloor Cavern Room3 / Land; Seafloor Cavern Room4 / Land; Seafloor Cavern Room5 / Land; Seafloor Cavern Room6 / Land; Seafloor Cavern Room7 / Land; Seafloor Cavern Room8 / Land | party-selectable species/form |
| 659 | Bunnelby | SPECIES_BUNNELBY | Direct acquisition | Random wild in 1 catchable method pool(s): Route 104 / Land | party-selectable species/form |
| 660 | Diggersby | SPECIES_DIGGERSBY | Evolution from obtainable Pokémon | Bunnelby → Diggersby via Level (20) | party-selectable species/form |
| 661 | Fletchling | SPECIES_FLETCHLING | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Fletchinder; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 662 | Fletchinder | SPECIES_FLETCHINDER | Direct acquisition | Random wild in 1 catchable method pool(s): Route 113 / Land | party-selectable species/form |
| 663 | Talonflame | SPECIES_TALONFLAME | Direct acquisition | Random wild in 1 catchable method pool(s): Route 113 / Honey | party-selectable species/form |
| 664 | Scatterbug | SPECIES_SCATTERBUG | Direct acquisition | Random wild in 1 catchable method pool(s): Berry tree encounter table 1: G Berry Stage Sprouted / Berry Tree | party-selectable species/form |
| 665 | Spewpa | SPECIES_SPEWPA | Direct acquisition | Random wild in 1 catchable method pool(s): Berry tree encounter table 2: G Berry Stage Taller / Berry Tree | party-selectable species/form |
| 666 | Vivillon | SPECIES_VIVILLON | Evolution from obtainable Pokémon | Spewpa → Vivillon via Level (12) | party-selectable species/form |
| 667 | Litleo | SPECIES_LITLEO | Direct acquisition | Random wild in 1 catchable method pool(s): Route 104 / Land | party-selectable species/form |
| 668 | Pyroar | SPECIES_PYROAR | Evolution from obtainable Pokémon | Litleo → Pyroar via Level Male (35) | party-selectable species/form |
| 669 | Flabebe | SPECIES_FLABEBE | Direct acquisition | Random wild in 2 catchable method pool(s): Berry tree encounter table 3: G Berry Stage Flowering / Berry Tree; Dewford Meadow / Land | party-selectable species/form |
| 670 | Floette | SPECIES_FLOETTE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 117 / Rock Smash | party-selectable species/form |
| 671 | Florges | SPECIES_FLORGES | Evolution from obtainable Pokémon | Floette → Florges via Use/hold ITEM_SHINY_STONE | party-selectable species/form |
| 672 | Skiddo | SPECIES_SKIDDO | Direct acquisition | Random wild in 1 catchable method pool(s): Route 116 / Land | party-selectable species/form |
| 673 | Gogoat | SPECIES_GOGOAT | Evolution from obtainable Pokémon | Skiddo → Gogoat via Level (32) | party-selectable species/form |
| 674 | Pancham | SPECIES_PANCHAM | Direct acquisition | Random wild in 1 catchable method pool(s): Route 116 / Honey | party-selectable species/form |
| 675 | Pangoro | SPECIES_PANGORO | Direct acquisition | Random wild in 1 catchable method pool(s): Route 121 / Land | party-selectable species/form |
| 676 | Furfrou | SPECIES_FURFROU | Direct acquisition | Random wild in 1 catchable method pool(s): Route 121 / Land | party-selectable species/form |
| 677 | Espurr | SPECIES_ESPURR | Direct acquisition | Random wild in 1 catchable method pool(s): Verdanturf Meadow / Land | party-selectable species/form |
| 678 | Meowstic | SPECIES_MEOWSTIC | Evolution from obtainable Pokémon | Espurr → Meowstic via Level Male (25) | party-selectable species/form |
| 679 | Honedge | SPECIES_HONEDGE | Direct acquisition | Random wild in 5 catchable method pool(s): Route 120 / Land; Sandstrewn Ruins / Land; Sandstrewn Ruins 2F / Land; Sandstrewn Ruins 3F / Land; Sandstrewn Ruins B1F / Land | party-selectable species/form |
| 680 | Doublade | SPECIES_DOUBLADE | Evolution from obtainable Pokémon | Honedge → Doublade via Level (35) | party-selectable species/form |
| 681 | Aegislash | SPECIES_AEGISLASH | Direct acquisition | Random wild in 2 catchable method pool(s): Victory Road B1F / Land; Victory Road B2F / Land | party-selectable species/form |
| 682 | Spritzee | SPECIES_SPRITZEE | Direct acquisition | Random wild in 2 catchable method pool(s): Dewford Meadow / Honey; Route 115 / Land | party-selectable species/form |
| 683 | Aromatisse | SPECIES_AROMATISSE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 115 / Honey | party-selectable species/form |
| 684 | Swirlix | SPECIES_SWIRLIX | Direct acquisition | Random wild in 1 catchable method pool(s): Dewford Meadow / Honey | party-selectable species/form |
| 685 | Slurpuff | SPECIES_SLURPUFF | Direct acquisition | Random wild in 1 catchable method pool(s): Route 115 / Honey | party-selectable species/form |
| 686 | Inkay | SPECIES_INKAY | Direct acquisition | Random wild in 1 catchable method pool(s): Route 106 / Land | party-selectable species/form |
| 687 | Malamar | SPECIES_MALAMAR | Direct acquisition | Random wild in 9 catchable method pool(s): Route 105 / Land; Seafloor Cavern Room1 / Land; Seafloor Cavern Room2 / Land; Seafloor Cavern Room3 / Land; Seafloor Cavern Room4 / Land; Seafloor Cavern Room5 / Land; Seafloor Cavern Room6 / Land; Seafloor Cavern Room7 / Land; Seafloor Cavern Room8 / Land | party-selectable species/form |
| 688 | Binacle | SPECIES_BINACLE | Direct acquisition | Random wild in 4 catchable method pool(s): Dewford Town / Rock Smash; Lilycove City / Rock Smash; Route 106 / Rock Smash; Seaspray Cave / Rock Smash | party-selectable species/form |
| 689 | Barbaracle | SPECIES_BARBARACLE | Evolution from obtainable Pokémon | Binacle → Barbaracle via Level (39) | party-selectable species/form |
| 690 | Skrelp | SPECIES_SKRELP | Direct acquisition | Random wild in 10 catchable method pool(s): Abandoned Ship Hidden Floor Corridors / Good Rod; Abandoned Ship Hidden Floor Corridors / Old Rod; Abandoned Ship Rooms B1F / Good Rod; Abandoned Ship Rooms B1F / Old Rod; Pacifidlog Town / Good Rod; Pacifidlog Town / Old Rod; Route 105 / Good Rod; Route 105 / Old Rod; Route 106 / Good Rod; Route 106 / Old Rod | party-selectable species/form |
| 691 | Dragalge | SPECIES_DRAGALGE | Direct acquisition | Random wild in 14 catchable method pool(s): Abandoned Ship Hidden Floor Corridors / Super Rod; Abandoned Ship Rooms B1F / Super Rod; Pacifidlog Town / Super Rod; Route 105 / Super Rod; Route 106 / Super Rod; Route 128 / Super Rod; Seafloor Cavern Room1 / Land; Seafloor Cavern Room2 / Land; Seafloor Cavern Room3 / Land; Seafloor Cavern Room4 / Land; Seafloor Cavern Room5 / Land; Seafloor Cavern Room6 / Land; Seafloor Cavern Room7 / Land; Seafloor Cavern Room8 / Land | party-selectable species/form |
| 692 | Clauncher | SPECIES_CLAUNCHER | Direct acquisition | Random wild in 4 catchable method pool(s): Route 105 / Good Rod; Route 105 / Old Rod; Route 106 / Good Rod; Route 106 / Old Rod | party-selectable species/form |
| 693 | Clawitzer | SPECIES_CLAWITZER | Direct acquisition | Random wild in 2 catchable method pool(s): Route 105 / Super Rod; Route 106 / Super Rod | party-selectable species/form |
| 694 | Helioptile | SPECIES_HELIOPTILE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 111 Ruins Exterior / Land | party-selectable species/form |
| 695 | Heliolisk | SPECIES_HELIOLISK | Direct acquisition | Random wild in 1 catchable method pool(s): Route 110 / Land | party-selectable species/form |
| 696 | Tyrunt | SPECIES_TYRUNT | Direct acquisition | Fossil restoration: Rustboro Devon Corporation (Restore ITEM_JAW_FOSSIL) | party-selectable species/form |
| 697 | Tyrantrum | SPECIES_TYRANTRUM | Evolution from obtainable Pokémon | Tyrunt → Tyrantrum via Level Day (39) | party-selectable species/form |
| 698 | Amaura | SPECIES_AMAURA | Direct acquisition | Fossil restoration: Rustboro Devon Corporation (Restore ITEM_SAIL_FOSSIL) | party-selectable species/form |
| 699 | Aurorus | SPECIES_AURORUS | Evolution from obtainable Pokémon | Amaura → Aurorus via Level Night (39) | party-selectable species/form |
| 700 | Sylveon | SPECIES_SYLVEON | Evolution from obtainable Pokémon | Eevee → Sylveon via Level while knowing a Fairy move | party-selectable species/form |
| 701 | Hawlucha | SPECIES_HAWLUCHA | Direct acquisition | Random wild in 2 catchable method pool(s): Route 111 Ruins Exterior / Land; Route 112 / Land | party-selectable species/form |
| 702 | Dedenne | SPECIES_DEDENNE | Direct acquisition | Random wild in 2 catchable method pool(s): Route 118 / Honey; Route 118 / Land | party-selectable species/form |
| 703 | Carbink | SPECIES_CARBINK | Direct acquisition | Random wild in 9 catchable method pool(s): Cave Of Origin 1F / Land; Cave Of Origin Diancies Room / Land; Cave Of Origin Entrance / Land; Cave Of Origin Unused Ruby Sapphire Map1 / Land; Cave Of Origin Unused Ruby Sapphire Map2 / Land; Cave Of Origin Unused Ruby Sapphire Map3 / Land; Granite Cave B1F / Land; Granite Cave B2F / Land; Victory Road B2F / Land | party-selectable species/form |
| 704 | Goomy | SPECIES_GOOMY | Direct acquisition | Random wild in 2 catchable method pool(s): Petalburg Woods 3 / Honey; Route 119 / Land | party-selectable species/form |
| 705 | Sliggoo | SPECIES_SLIGGOO | Direct acquisition | Random wild in 3 catchable method pool(s): Route 119 / Honey; Safari Zone Southwest / Land; Safari Zone Southwest / Surf | party-selectable species/form |
| 706 | Goodra | SPECIES_GOODRA | Evolution from obtainable Pokémon | Sliggoo → Goodra via Level Rain (50) | party-selectable species/form |
| 707 | Klefki | SPECIES_KLEFKI | Direct acquisition | Random wild in 1 catchable method pool(s): Route 113 / Land | party-selectable species/form |
| 708 | Phantump | SPECIES_PHANTUMP | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods / Land | party-selectable species/form |
| 709 | Trevenant | SPECIES_TREVENANT | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods 3 / Rock Smash | party-selectable species/form |
| 710 | Pumpkaboo | SPECIES_PUMPKABOO | Direct acquisition | Random wild in 1 catchable method pool(s): Route 120 / Land | party-selectable species/form |
| 711 | Gourgeist | SPECIES_GOURGEIST | Evolution from obtainable Pokémon | Pumpkaboo → Gourgeist via Use/hold ITEM_DUSK_STONE | party-selectable species/form |
| 712 | Bergmite | SPECIES_BERGMITE | Direct acquisition | Random wild in 1 catchable method pool(s): Seaspray Cave B1F / Land | party-selectable species/form |
| 713 | Avalugg | SPECIES_AVALUGG | Evolution from obtainable Pokémon | Bergmite → Avalugg via Level (37) | party-selectable species/form |
| 714 | Noibat | SPECIES_NOIBAT | Direct acquisition | Random wild in 1 catchable method pool(s): Rusturf Tunnel / Land | party-selectable species/form |
| 715 | Noivern | SPECIES_NOIVERN | Direct acquisition | Random wild in 4 catchable method pool(s): Altering Cave 1F / Land; Cave Of Origin 1F / Land; Cave Of Origin Entrance / Land; Victory Road 1F / Land | party-selectable species/form |
| 716 | Xerneas | SPECIES_XERNEAS | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 717 | Yveltal | SPECIES_YVELTAL | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 718 | Zygarde | SPECIES_ZYGARDE | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 719 | Diancie | SPECIES_DIANCIE | Direct acquisition | Scripted/static encounter: Cave Of Origin Diancies Room (Literal setwildbattle acquisition) \| Authored static acquisition: Cave Of Origin Diancies Room (diancie) | party-selectable species/form |
| 720 | Hoopa | SPECIES_HOOPA | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 721 | Volcanion | SPECIES_VOLCANION | Direct acquisition | Random wild in 1 catchable method pool(s): Magma Hideout 4F / Land | party-selectable species/form |
| 722 | Rowlet | SPECIES_ROWLET | Direct acquisition | Starter choice: Littleroot opening (Selectable Alola starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 723 | Dartrix | SPECIES_DARTRIX | Evolution from obtainable Pokémon | Rowlet → Dartrix via Level (16) | party-selectable species/form |
| 724 | Decidueye | SPECIES_DECIDUEYE | Evolution from obtainable Pokémon | Rowlet → Dartrix via Level (16) ; Dartrix → Decidueye via Level (34) | party-selectable species/form |
| 725 | Litten | SPECIES_LITTEN | Direct acquisition | Starter choice: Littleroot opening (Selectable Alola starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 726 | Torracat | SPECIES_TORRACAT | Evolution from obtainable Pokémon | Litten → Torracat via Level (16) | party-selectable species/form |
| 727 | Incineroar | SPECIES_INCINEROAR | Evolution from obtainable Pokémon | Litten → Torracat via Level (16) ; Torracat → Incineroar via Level (34) | party-selectable species/form |
| 728 | Popplio | SPECIES_POPPLIO | Direct acquisition | Starter choice: Littleroot opening (Selectable Alola starter) \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 729 | Brionne | SPECIES_BRIONNE | Evolution from obtainable Pokémon | Popplio → Brionne via Level (16) | party-selectable species/form |
| 730 | Primarina | SPECIES_PRIMARINA | Direct acquisition | Random wild in 2 catchable method pool(s): Route 126 / Surf; Route 128 / Super Rod | party-selectable species/form |
| 731 | Pikipek | SPECIES_PIKIPEK | Direct acquisition | Random wild in 1 catchable method pool(s): Berry tree encounter table 4: G Berry Stage Berries / Berry Tree | party-selectable species/form |
| 732 | Trumbeak | SPECIES_TRUMBEAK | Direct acquisition | Random wild in 1 catchable method pool(s): Ashen Woods / Land | party-selectable species/form |
| 733 | Toucannon | SPECIES_TOUCANNON | Direct acquisition | Random wild in 1 catchable method pool(s): Ashen Woods / Honey | party-selectable species/form |
| 734 | Yungoos | SPECIES_YUNGOOS | Direct acquisition | Random wild in 1 catchable method pool(s): Route 104 / Honey | party-selectable species/form |
| 735 | Gumshoos | SPECIES_GUMSHOOS | Direct acquisition | Random wild in 1 catchable method pool(s): Route 119 / Land | party-selectable species/form |
| 736 | Grubbin | SPECIES_GRUBBIN | Direct acquisition | Random wild in 1 catchable method pool(s): Route 103 / Land | party-selectable species/form |
| 737 | Charjabug | SPECIES_CHARJABUG | Direct acquisition | Random wild in 1 catchable method pool(s): Route 103 / Honey | party-selectable species/form |
| 738 | Vikavolt | SPECIES_VIKAVOLT | Evolution from obtainable Pokémon | Charjabug → Vikavolt via Specific Mapsec (MAPSEC_NEW_MAUVILLE) | party-selectable species/form |
| 739 | Crabrawler | SPECIES_CRABRAWLER | Direct acquisition | Random wild in 3 catchable method pool(s): Berry tree encounter table 4: G Berry Stage Berries / Berry Tree; Route 105 / Land; Route 106 / Land | party-selectable species/form |
| 740 | Crabominable | SPECIES_CRABOMINABLE | Evolution from obtainable Pokémon | Crabrawler → Crabominable via Use/hold ITEM_ICE_STONE | party-selectable species/form |
| 741 | Oricorio | SPECIES_ORICORIO | Direct acquisition | Random wild in 1 catchable method pool(s): Dewford Meadow / Land | party-selectable species/form |
| 742 | Cutiefly | SPECIES_CUTIEFLY | Direct acquisition | Random wild in 2 catchable method pool(s): Berry tree encounter table 3: G Berry Stage Flowering / Berry Tree; Dewford Meadow / Land | party-selectable species/form |
| 743 | Ribombee | SPECIES_RIBOMBEE | Direct acquisition | Random wild in 2 catchable method pool(s): Verdanturf Meadow / Honey; Verdanturf Meadow / Land | party-selectable species/form |
| 744 | Rockruff | SPECIES_ROCKRUFF | Direct acquisition | Random wild in 1 catchable method pool(s): Route 111 Ruins Exterior / Land | party-selectable species/form |
| 745 | Lycanroc | SPECIES_LYCANROC | Evolution from obtainable Pokémon | Rockruff → Lycanroc via Level Day (25) | party-selectable species/form |
| 746 | Wishiwashi | SPECIES_WISHIWASHI | Direct acquisition | Random wild in 10 catchable method pool(s): Route 114 / Good Rod; Route 114 / Old Rod; Route 114 / Super Rod; Route 115 / Good Rod; Route 115 / Old Rod; Route 115 / Super Rod; Route 115 / Surf; Seaspray Cave / Good Rod; Seaspray Cave / Old Rod; Seaspray Cave / Super Rod | party-selectable species/form |
| 747 | Mareanie | SPECIES_MAREANIE | Direct acquisition | Random wild in 4 catchable method pool(s): Ever Grande City / Good Rod; Route 104 / Land; Route 109 / Good Rod; Route 109 / Old Rod | party-selectable species/form |
| 748 | Toxapex | SPECIES_TOXAPEX | Direct acquisition | Random wild in 2 catchable method pool(s): Ever Grande City / Super Rod; Route 109 / Super Rod | party-selectable species/form |
| 749 | Mudbray | SPECIES_MUDBRAY | Direct acquisition | Random wild in 1 catchable method pool(s): Jagged Pass / Land | party-selectable species/form |
| 750 | Mudsdale | SPECIES_MUDSDALE | Evolution from obtainable Pokémon | Mudbray → Mudsdale via Level (30) | party-selectable species/form |
| 751 | Dewpider | SPECIES_DEWPIDER | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods 3 / Land | party-selectable species/form |
| 752 | Araquanid | SPECIES_ARAQUANID | Evolution from obtainable Pokémon | Dewpider → Araquanid via Level (22) | party-selectable species/form |
| 753 | Fomantis | SPECIES_FOMANTIS | Direct acquisition | Random wild in 1 catchable method pool(s): Route 116 / Rock Smash | party-selectable species/form |
| 754 | Lurantis | SPECIES_LURANTIS | Direct acquisition | Random wild in 1 catchable method pool(s): Route 115 / Land | party-selectable species/form |
| 755 | Morelull | SPECIES_MORELULL | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods 2 / Land | party-selectable species/form |
| 756 | Shiinotic | SPECIES_SHIINOTIC | Direct acquisition | Random wild in 1 catchable method pool(s): Altering Cave 1F / Land | party-selectable species/form |
| 757 | Salandit | SPECIES_SALANDIT | Direct acquisition | Random wild in 2 catchable method pool(s): Ashen Woods / Land; Jagged Pass / Land | party-selectable species/form |
| 758 | Salazzle | SPECIES_SALAZZLE | Direct acquisition | Random wild in 2 catchable method pool(s): Ashen Woods / Honey; Jagged Pass / Honey | party-selectable species/form |
| 759 | Stufful | SPECIES_STUFFUL | Direct acquisition | Random wild in 1 catchable method pool(s): Verdanturf Meadow / Land | party-selectable species/form |
| 760 | Bewear | SPECIES_BEWEAR | Evolution from obtainable Pokémon | Stufful → Bewear via Level (27) | party-selectable species/form |
| 761 | Bounsweet | SPECIES_BOUNSWEET | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods 2 / Land | party-selectable species/form |
| 762 | Steenee | SPECIES_STEENEE | Evolution from obtainable Pokémon | Bounsweet → Steenee via Level (18) | party-selectable species/form |
| 763 | Tsareena | SPECIES_TSAREENA | Evolution from obtainable Pokémon | Bounsweet → Steenee via Level (18) ; Steenee → Tsareena via Level while knowing MOVE_STOMP | party-selectable species/form |
| 764 | Comfey | SPECIES_COMFEY | Direct acquisition | Random wild in 1 catchable method pool(s): Route 119 / Land | party-selectable species/form |
| 765 | Oranguru | SPECIES_ORANGURU | Direct acquisition | Random wild in 1 catchable method pool(s): Route 119 / Land | party-selectable species/form |
| 766 | Passimian | SPECIES_PASSIMIAN | Direct acquisition | Random wild in 1 catchable method pool(s): Route 118 / Land | party-selectable species/form |
| 767 | Wimpod | SPECIES_WIMPOD | Direct acquisition | Random wild in 4 catchable method pool(s): Dewford Town / Rock Smash; Lilycove City / Rock Smash; Route 104 / Land; Route 106 / Rock Smash | party-selectable species/form |
| 768 | Golisopod | SPECIES_GOLISOPOD | Direct acquisition | Random wild in 11 catchable method pool(s): Route 127 / Surf; Seafloor Cavern Room1 / Land; Seafloor Cavern Room2 / Land; Seafloor Cavern Room3 / Land; Seafloor Cavern Room4 / Land; Seafloor Cavern Room5 / Land; Seafloor Cavern Room6 / Land; Seafloor Cavern Room7 / Land; Seafloor Cavern Room8 / Land; Underwater Route 124 / Land; Underwater Route 126 / Land | party-selectable species/form |
| 769 | Sandygast | SPECIES_SANDYGAST | Direct acquisition | Random wild in 1 catchable method pool(s): Route 109 / Rock Smash | party-selectable species/form |
| 770 | Palossand | SPECIES_PALOSSAND | Evolution from obtainable Pokémon | Sandygast → Palossand via Level (42) | party-selectable species/form |
| 771 | Pyukumuku | SPECIES_PYUKUMUKU | Direct acquisition | Random wild in 2 catchable method pool(s): Route 109 / Rock Smash; Route 109 / Surf | party-selectable species/form |
| 772 | Type: Null | SPECIES_TYPE_NULL | Direct acquisition | Random wild in 1 catchable method pool(s): Route 118 / Land | party-selectable species/form |
| 773 | Silvally | SPECIES_SILVALLY | Evolution from obtainable Pokémon | Type: Null → Silvally via Friendship | party-selectable species/form |
| 774 | Minior | SPECIES_MINIOR | Direct acquisition | Random wild in 1 catchable method pool(s): Meteor Falls 1F 1R / Land | party-selectable species/form |
| 775 | Komala | SPECIES_KOMALA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 121 / Land | party-selectable species/form |
| 776 | Turtonator | SPECIES_TURTONATOR | Direct acquisition | Random wild in 11 catchable method pool(s): Jagged Pass / Land; Magma Hideout 1F / Land; Magma Hideout 2F 1R / Land; Magma Hideout 2F 2R / Land; Magma Hideout 2F 3R / Land; Magma Hideout 3F 1R / Land; Magma Hideout 3F 2R / Land; Magma Hideout 3F 3R / Land; Magma Hideout 4F / Land; Scorched Slab B2F / Land; Scorched Slab Heatrans Room / Land | party-selectable species/form |
| 777 | Togedemaru | SPECIES_TOGEDEMARU | Direct acquisition | Random wild in 2 catchable method pool(s): New Mauville Entrance / Land; New Mauville Inside / Land | party-selectable species/form |
| 778 | Mimikyu | SPECIES_MIMIKYU | Direct acquisition | Random wild in 4 catchable method pool(s): Mt Pyre 4F / Land; Mt Pyre 5F / Land; Mt Pyre 6F / Land; Route 120 / Land | party-selectable species/form |
| 779 | Bruxish | SPECIES_BRUXISH | Direct acquisition | Random wild in 4 catchable method pool(s): Lilycove City / Good Rod; Lilycove City / Super Rod; Lilycove City / Surf; Route 109 / Super Rod | party-selectable species/form |
| 780 | Drampa | SPECIES_DRAMPA | Direct acquisition | Random wild in 1 catchable method pool(s): Meteor Falls B1F 1R / Land | party-selectable species/form |
| 781 | Dhelmise | SPECIES_DHELMISE | Direct acquisition | Random wild in 14 catchable method pool(s): Abandoned Ship Hidden Floor Corridors / Good Rod; Abandoned Ship Hidden Floor Corridors / Super Rod; Abandoned Ship Rooms B1F / Good Rod; Abandoned Ship Rooms B1F / Super Rod; Pacifidlog Town / Super Rod; Route 127 / Super Rod; Seafloor Cavern Room1 / Land; Seafloor Cavern Room2 / Land; Seafloor Cavern Room3 / Land; Seafloor Cavern Room4 / Land; Seafloor Cavern Room5 / Land; Seafloor Cavern Room6 / Land; Seafloor Cavern Room7 / Land; Seafloor Cavern Room8 / Land | party-selectable species/form |
| 782 | Jangmo-o | SPECIES_JANGMO_O | Direct acquisition | Random wild in 1 catchable method pool(s): Jagged Pass / Land | party-selectable species/form |
| 783 | Hakamo-o | SPECIES_HAKAMO_O | Direct acquisition | Random wild in 1 catchable method pool(s): Jagged Pass / Honey | party-selectable species/form |
| 784 | Kommo-o | SPECIES_KOMMO_O | Direct acquisition | Random wild in 2 catchable method pool(s): Victory Road 1F / Land; Victory Road B1F / Land | party-selectable species/form |
| 785 | Tapu Koko | SPECIES_TAPU_KOKO | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 786 | Tapu Lele | SPECIES_TAPU_LELE | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 787 | Tapu Bulu | SPECIES_TAPU_BULU | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 788 | Tapu Fini | SPECIES_TAPU_FINI | Direct acquisition | Random wild in 1 catchable method pool(s): Route 126 / Super Rod | party-selectable species/form |
| 789 | Cosmog | SPECIES_COSMOG | Direct acquisition | NPC or story gift: Littleroot Town Professor Birchs Lab (Literal givemon acquisition) \| Authored gift/restoration: Littleroot Town Professor Birchs Lab (cosmog) | party-selectable species/form |
| 790 | Cosmoem | SPECIES_COSMOEM | Evolution from obtainable Pokémon | Cosmog → Cosmoem via Level (43) | party-selectable species/form |
| 791 | Solgaleo | SPECIES_SOLGALEO | Evolution from obtainable Pokémon | Cosmog → Cosmoem via Level (43) ; Cosmoem → Solgaleo via Level Day (53) | party-selectable species/form |
| 792 | Lunala | SPECIES_LUNALA | Evolution from obtainable Pokémon | Cosmog → Cosmoem via Level (43) ; Cosmoem → Lunala via Level Night (53) | party-selectable species/form |
| 793 | Nihilego | SPECIES_NIHILEGO | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 794 | Buzzwole | SPECIES_BUZZWOLE | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 795 | Pheromosa | SPECIES_PHEROMOSA | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 796 | Xurkitree | SPECIES_XURKITREE | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 797 | Celesteela | SPECIES_CELESTEELA | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 798 | Kartana | SPECIES_KARTANA | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 799 | Guzzlord | SPECIES_GUZZLORD | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 800 | Necrozma | SPECIES_NECROZMA | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 801 | Magearna | SPECIES_MAGEARNA | Direct acquisition | Pokémon Center mystery gift: Pokémon Centers (Requires FLAG_SYS_GAME_CLEAR; arrives holding ITEM_MASTER_BALL) | party-selectable species/form |
| 802 | Marshadow | SPECIES_MARSHADOW | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 803 | Poipole | SPECIES_POIPOLE | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 804 | Naganadel | SPECIES_NAGANADEL | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 805 | Stakataka | SPECIES_STAKATAKA | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 806 | Blacephalon | SPECIES_BLACEPHALON | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 807 | Zeraora | SPECIES_ZERAORA | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 808 | Meltan | SPECIES_MELTAN | Direct acquisition | NPC or story gift: Mossdeep City Stevens House (Literal givemon acquisition) \| Authored gift/restoration: Mossdeep City Stevens House (meltan) | party-selectable species/form |
| 809 | Melmetal | SPECIES_MELMETAL | Evolution from obtainable Pokémon | Meltan → Melmetal via Use/hold ITEM_METAL_COAT | party-selectable species/form |
| 810 | Grookey | SPECIES_GROOKEY | Direct acquisition | Random wild in 1 catchable method pool(s): Route 117 / Land | party-selectable species/form |
| 811 | Thwackey | SPECIES_THWACKEY | Evolution from obtainable Pokémon | Grookey → Thwackey via Level (16) | party-selectable species/form |
| 812 | Rillaboom | SPECIES_RILLABOOM | Evolution from obtainable Pokémon | Grookey → Thwackey via Level (16) ; Thwackey → Rillaboom via Level (35) | party-selectable species/form |
| 813 | Scorbunny | SPECIES_SCORBUNNY | Direct acquisition | Random wild in 1 catchable method pool(s): Route 117 / Land | party-selectable species/form |
| 814 | Raboot | SPECIES_RABOOT | Evolution from obtainable Pokémon | Scorbunny → Raboot via Level (16) | party-selectable species/form |
| 815 | Cinderace | SPECIES_CINDERACE | Evolution from obtainable Pokémon | Scorbunny → Raboot via Level (16) ; Raboot → Cinderace via Level (35) | party-selectable species/form |
| 816 | Sobble | SPECIES_SOBBLE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 104 / Old Rod | party-selectable species/form |
| 817 | Drizzile | SPECIES_DRIZZILE | Evolution from obtainable Pokémon | Sobble → Drizzile via Level (16) | party-selectable species/form |
| 818 | Inteleon | SPECIES_INTELEON | Evolution from obtainable Pokémon | Sobble → Drizzile via Level (16) ; Drizzile → Inteleon via Level (35) | party-selectable species/form |
| 819 | Skwovet | SPECIES_SKWOVET | Direct acquisition | Random wild in 1 catchable method pool(s): Route 101 / Honey | party-selectable species/form |
| 820 | Greedent | SPECIES_GREEDENT | Evolution from obtainable Pokémon | Skwovet → Greedent via Level (24) | party-selectable species/form |
| 821 | Rookidee | SPECIES_ROOKIDEE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 116 / Land | party-selectable species/form |
| 822 | Corvisquire | SPECIES_CORVISQUIRE | Evolution from obtainable Pokémon | Rookidee → Corvisquire via Level (18) | party-selectable species/form |
| 823 | Corviknight | SPECIES_CORVIKNIGHT | Evolution from obtainable Pokémon | Rookidee → Corvisquire via Level (18) ; Corvisquire → Corviknight via Level (38) | party-selectable species/form |
| 824 | Blipbug | SPECIES_BLIPBUG | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods 2 / Land | party-selectable species/form |
| 825 | Dottler | SPECIES_DOTTLER | Evolution from obtainable Pokémon | Blipbug → Dottler via Level (10) | party-selectable species/form |
| 826 | Orbeetle | SPECIES_ORBEETLE | Evolution from obtainable Pokémon | Blipbug → Dottler via Level (10) ; Dottler → Orbeetle via Level (30) | party-selectable species/form |
| 827 | Nickit | SPECIES_NICKIT | Direct acquisition | Random wild in 1 catchable method pool(s): Route 116 / Honey | party-selectable species/form |
| 828 | Thievul | SPECIES_THIEVUL | Evolution from obtainable Pokémon | Nickit → Thievul via Level (18) | party-selectable species/form |
| 829 | Gossifleur | SPECIES_GOSSIFLEUR | Direct acquisition | Random wild in 1 catchable method pool(s): Route 117 / Land | party-selectable species/form |
| 830 | Eldegoss | SPECIES_ELDEGOSS | Evolution from obtainable Pokémon | Gossifleur → Eldegoss via Level (20) | party-selectable species/form |
| 831 | Wooloo | SPECIES_WOOLOO | Direct acquisition | Random wild in 1 catchable method pool(s): Route 117 / Land | party-selectable species/form |
| 832 | Dubwool | SPECIES_DUBWOOL | Evolution from obtainable Pokémon | Wooloo → Dubwool via Level (24) | party-selectable species/form |
| 833 | Chewtle | SPECIES_CHEWTLE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 104 / Old Rod | party-selectable species/form |
| 834 | Drednaw | SPECIES_DREDNAW | Evolution from obtainable Pokémon | Chewtle → Drednaw via Level (22) | party-selectable species/form |
| 835 | Yamper | SPECIES_YAMPER | Direct acquisition | Random wild in 1 catchable method pool(s): Route 103 / Land | party-selectable species/form |
| 836 | Boltund | SPECIES_BOLTUND | Evolution from obtainable Pokémon | Yamper → Boltund via Level (25) | party-selectable species/form |
| 837 | Rolycoly | SPECIES_ROLYCOLY | Direct acquisition | Random wild in 1 catchable method pool(s): Granite Cave 1F / Land | party-selectable species/form |
| 838 | Carkol | SPECIES_CARKOL | Evolution from obtainable Pokémon | Rolycoly → Carkol via Level (18) | party-selectable species/form |
| 839 | Coalossal | SPECIES_COALOSSAL | Evolution from obtainable Pokémon | Rolycoly → Carkol via Level (18) ; Carkol → Coalossal via Level (34) | party-selectable species/form |
| 840 | Applin | SPECIES_APPLIN | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods 2 / Land | party-selectable species/form |
| 841 | Flapple | SPECIES_FLAPPLE | Evolution from obtainable Pokémon | Applin → Flapple via Use/hold ITEM_NONE | party-selectable species/form |
| 842 | Appletun | SPECIES_APPLETUN | Evolution from obtainable Pokémon | Applin → Appletun via Use/hold ITEM_NONE | party-selectable species/form |
| 843 | Silicobra | SPECIES_SILICOBRA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 111 / Land | party-selectable species/form |
| 844 | Sandaconda | SPECIES_SANDACONDA | Evolution from obtainable Pokémon | Silicobra → Sandaconda via Level (36) | party-selectable species/form |
| 845 | Cramorant | SPECIES_CRAMORANT | Direct acquisition | Random wild in 1 catchable method pool(s): Route 119 / Land | party-selectable species/form |
| 846 | Arrokuda | SPECIES_ARROKUDA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 118 / Super Rod | party-selectable species/form |
| 847 | Barraskewda | SPECIES_BARRASKEWDA | Evolution from obtainable Pokémon | Arrokuda → Barraskewda via Level (26) | party-selectable species/form |
| 848 | Toxel | SPECIES_TOXEL | Direct acquisition | Random wild in 2 catchable method pool(s): Route 103 / Land; Route 110 / Land | party-selectable species/form |
| 849 | Toxtricity | SPECIES_TOXTRICITY | Evolution from obtainable Pokémon | Toxel → Toxtricity via Level (30) | party-selectable species/form |
| 850 | Sizzlipede | SPECIES_SIZZLIPEDE | Direct acquisition | Random wild in 1 catchable method pool(s): Fiery Path / Land | party-selectable species/form |
| 851 | Centiskorch | SPECIES_CENTISKORCH | Evolution from obtainable Pokémon | Sizzlipede → Centiskorch via Level (28) | party-selectable species/form |
| 852 | Clobbopus | SPECIES_CLOBBOPUS | Direct acquisition | Random wild in 1 catchable method pool(s): Route 109 / Good Rod | party-selectable species/form |
| 853 | Grapploct | SPECIES_GRAPPLOCT | Evolution from obtainable Pokémon | Clobbopus → Grapploct via Level while knowing MOVE_TAUNT | party-selectable species/form |
| 854 | Sinistea | SPECIES_SINISTEA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 121 / Land | party-selectable species/form |
| 855 | Polteageist | SPECIES_POLTEAGEIST | Evolution from obtainable Pokémon | Sinistea → Polteageist via Use/hold ITEM_NONE | party-selectable species/form |
| 856 | Hatenna | SPECIES_HATENNA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 102 / Land | party-selectable species/form |
| 857 | Hattrem | SPECIES_HATTREM | Evolution from obtainable Pokémon | Hatenna → Hattrem via Level (32) | party-selectable species/form |
| 858 | Hatterene | SPECIES_HATTERENE | Evolution from obtainable Pokémon | Hatenna → Hattrem via Level (32) ; Hattrem → Hatterene via Level (42) | party-selectable species/form |
| 859 | Impidimp | SPECIES_IMPIDIMP | Direct acquisition | Random wild in 1 catchable method pool(s): Petalburg Woods / Land | party-selectable species/form |
| 860 | Morgrem | SPECIES_MORGREM | Evolution from obtainable Pokémon | Impidimp → Morgrem via Level (32) | party-selectable species/form |
| 861 | Grimmsnarl | SPECIES_GRIMMSNARL | Evolution from obtainable Pokémon | Impidimp → Morgrem via Level (32) ; Morgrem → Grimmsnarl via Level (42) | party-selectable species/form |
| 862 | Obstagoon | SPECIES_OBSTAGOON | Evolution from obtainable Pokémon | Zigzagoon Galarian → Linoone Galarian via Level (20) ; Linoone Galarian → Obstagoon via Level Night (35) | party-selectable species/form |
| 863 | Perrserker | SPECIES_PERRSERKER | Evolution from obtainable Pokémon | Meowth Galarian → Perrserker via Level (28) | party-selectable species/form |
| 864 | Cursola | SPECIES_CURSOLA | Evolution from obtainable Pokémon | Corsola Galarian → Cursola via Level (38) | party-selectable species/form |
| 865 | Sirfetch'd | SPECIES_SIRFETCHD | Evolution from obtainable Pokémon | Farfetchd Galarian → Sirfetch'd via Level (0) | party-selectable species/form |
| 866 | Mr Rime | SPECIES_MR_RIME | Evolution from obtainable Pokémon | Mr Mime Galarian → Mr Rime via Level (42) | party-selectable species/form |
| 867 | Runerigus | SPECIES_RUNERIGUS | Evolution from obtainable Pokémon | Yamask Galarian → Runerigus via Level (0) | party-selectable species/form |
| 868 | Milcery | SPECIES_MILCERY | Direct acquisition | Random wild in 1 catchable method pool(s): Verdanturf Meadow / Honey | party-selectable species/form |
| 869 | Alcremie | SPECIES_ALCREMIE | Evolution from obtainable Pokémon | Milcery → Alcremie via Use/hold 0 | party-selectable species/form |
| 870 | Falinks | SPECIES_FALINKS | Direct acquisition | Random wild in 1 catchable method pool(s): Route 113 / Land | party-selectable species/form |
| 871 | Pincurchin | SPECIES_PINCURCHIN | Direct acquisition | Random wild in 1 catchable method pool(s): Route 109 / Rock Smash | party-selectable species/form |
| 872 | Snom | SPECIES_SNOM | Direct acquisition | Random wild in 1 catchable method pool(s): Shoal Cave Low Tide Ice Room / Land | party-selectable species/form |
| 873 | Frosmoth | SPECIES_FROSMOTH | Evolution from obtainable Pokémon | Snom → Frosmoth via Friendship Night | party-selectable species/form |
| 874 | Stonjourner | SPECIES_STONJOURNER | Direct acquisition | Random wild in 1 catchable method pool(s): Route 111 / Land | party-selectable species/form |
| 875 | Eiscue | SPECIES_EISCUE | Direct acquisition | Random wild in 1 catchable method pool(s): Shoal Cave Low Tide Ice Room / Land | party-selectable species/form |
| 876 | Indeedee | SPECIES_INDEEDEE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 102 / Land | party-selectable species/form |
| 877 | Morpeko | SPECIES_MORPEKO | Direct acquisition | Random wild in 1 catchable method pool(s): Route 110 / Land | party-selectable species/form |
| 878 | Cufant | SPECIES_CUFANT | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone North / Land | party-selectable species/form |
| 879 | Copperajah | SPECIES_COPPERAJAH | Evolution from obtainable Pokémon | Cufant → Copperajah via Level (34) | party-selectable species/form |
| 880 | Dracozolt | SPECIES_DRACOZOLT | Direct acquisition | Random wild in 1 catchable method pool(s): Desert Underpass / Land | party-selectable species/form |
| 881 | Arctozolt | SPECIES_ARCTOZOLT | Direct acquisition | Random wild in 1 catchable method pool(s): Desert Underpass / Land | party-selectable species/form |
| 882 | Dracovish | SPECIES_DRACOVISH | Direct acquisition | Random wild in 1 catchable method pool(s): Desert Underpass / Land | party-selectable species/form |
| 883 | Arctovish | SPECIES_ARCTOVISH | Direct acquisition | Random wild in 1 catchable method pool(s): Desert Underpass / Land | party-selectable species/form |
| 884 | Duraludon | SPECIES_DURALUDON | Direct acquisition | Random wild in 1 catchable method pool(s): Route 115 / Land | party-selectable species/form |
| 885 | Dreepy | SPECIES_DREEPY | Direct acquisition | Random wild in 4 catchable method pool(s): Granite Cave Stevens Room / Land; Route 101 / Land; Route 116 / Land; Route 119 / Land | party-selectable species/form |
| 886 | Drakloak | SPECIES_DRAKLOAK | Evolution from obtainable Pokémon | Dreepy → Drakloak via Level (50) | party-selectable species/form |
| 887 | Dragapult | SPECIES_DRAGAPULT | Direct acquisition | Random wild in 2 catchable method pool(s): Victory Road 1F / Land; Victory Road B2F / Land | party-selectable species/form |
| 888 | Zacian | SPECIES_ZACIAN | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 889 | Zamazenta | SPECIES_ZAMAZENTA | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 890 | Eternatus | SPECIES_ETERNATUS | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 891 | Kubfu | SPECIES_KUBFU | Direct acquisition | Random wild in 1 catchable method pool(s): Route 112 / Land | party-selectable species/form |
| 892 | Urshifu | SPECIES_URSHIFU | Evolution from obtainable Pokémon | Kubfu → Urshifu via Level (0) | party-selectable species/form |
| 893 | Zarude | SPECIES_ZARUDE | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 894 | Regieleki | SPECIES_REGIELEKI | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 895 | Regidrago | SPECIES_REGIDRAGO | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 896 | Glastrier | SPECIES_GLASTRIER | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 897 | Spectrier | SPECIES_SPECTRIER | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 898 | Calyrex | SPECIES_CALYREX | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 899 | Venusaur Mega | SPECIES_VENUSAUR_MEGA | Form / battle transformation; not separately acquired | Derived from Venusaur through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 900 | Charizard Mega X | SPECIES_CHARIZARD_MEGA_X | Form / battle transformation; not separately acquired | Derived from Charizard through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 901 | Charizard Mega Y | SPECIES_CHARIZARD_MEGA_Y | Form / battle transformation; not separately acquired | Derived from Charizard through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 902 | Blastoise Mega | SPECIES_BLASTOISE_MEGA | Form / battle transformation; not separately acquired | Derived from Blastoise through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 903 | Beedrill Mega | SPECIES_BEEDRILL_MEGA | Form / battle transformation; not separately acquired | Derived from Beedrill through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 904 | Pidgeot Mega | SPECIES_PIDGEOT_MEGA | Form / battle transformation; not separately acquired | Derived from Pidgeot through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 905 | Alakazam Mega | SPECIES_ALAKAZAM_MEGA | Form / battle transformation; not separately acquired | Derived from Alakazam through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 906 | Slowbro Mega | SPECIES_SLOWBRO_MEGA | Form / battle transformation; not separately acquired | Derived from Slowbro through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 907 | Gengar Mega | SPECIES_GENGAR_MEGA | Form / battle transformation; not separately acquired | Derived from Gengar through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 908 | Kangaskhan Mega | SPECIES_KANGASKHAN_MEGA | Form / battle transformation; not separately acquired | Derived from Kangaskhan through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 909 | Pinsir Mega | SPECIES_PINSIR_MEGA | Form / battle transformation; not separately acquired | Derived from Pinsir through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 910 | Gyarados Mega | SPECIES_GYARADOS_MEGA | Form / battle transformation; not separately acquired | Derived from Gyarados through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 911 | Aerodactyl Mega | SPECIES_AERODACTYL_MEGA | Form / battle transformation; not separately acquired | Derived from Aerodactyl through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 912 | Mewtwo Mega X | SPECIES_MEWTWO_MEGA_X | Form / battle transformation; not separately acquired | Derived from Mewtwo through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 913 | Mewtwo Mega Y | SPECIES_MEWTWO_MEGA_Y | Form / battle transformation; not separately acquired | Derived from Mewtwo through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 914 | Ampharos Mega | SPECIES_AMPHAROS_MEGA | Form / battle transformation; not separately acquired | Derived from Ampharos through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 915 | Steelix Mega | SPECIES_STEELIX_MEGA | Form / battle transformation; not separately acquired | Derived from Steelix through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 916 | Scizor Mega | SPECIES_SCIZOR_MEGA | Form / battle transformation; not separately acquired | Derived from Scizor through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 917 | Heracross Mega | SPECIES_HERACROSS_MEGA | Form / battle transformation; not separately acquired | Derived from Heracross through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 918 | Houndoom Mega | SPECIES_HOUNDOOM_MEGA | Form / battle transformation; not separately acquired | Derived from Houndoom through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 919 | Tyranitar Mega | SPECIES_TYRANITAR_MEGA | Form / battle transformation; not separately acquired | Derived from Tyranitar through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 920 | Sceptile Mega | SPECIES_SCEPTILE_MEGA | Form / battle transformation; not separately acquired | Derived from Sceptile through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 921 | Blaziken Mega | SPECIES_BLAZIKEN_MEGA | Form / battle transformation; not separately acquired | Derived from Blaziken through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 922 | Swampert Mega | SPECIES_SWAMPERT_MEGA | Form / battle transformation; not separately acquired | Derived from Swampert through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 923 | Gardevoir Mega | SPECIES_GARDEVOIR_MEGA | Form / battle transformation; not separately acquired | Derived from Gardevoir through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 924 | Sableye Mega | SPECIES_SABLEYE_MEGA | Form / battle transformation; not separately acquired | Derived from Sableye through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 925 | Mawile Mega | SPECIES_MAWILE_MEGA | Form / battle transformation; not separately acquired | Derived from Mawile through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 926 | Aggron Mega | SPECIES_AGGRON_MEGA | Form / battle transformation; not separately acquired | Derived from Aggron through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 927 | Medicham Mega | SPECIES_MEDICHAM_MEGA | Form / battle transformation; not separately acquired | Derived from Medicham through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 928 | Manectric Mega | SPECIES_MANECTRIC_MEGA | Form / battle transformation; not separately acquired | Derived from Manectric through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 929 | Sharpedo Mega | SPECIES_SHARPEDO_MEGA | Form / battle transformation; not separately acquired | Derived from Sharpedo through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 930 | Camerupt Mega | SPECIES_CAMERUPT_MEGA | Form / battle transformation; not separately acquired | Derived from Camerupt through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 931 | Altaria Mega | SPECIES_ALTARIA_MEGA | Form / battle transformation; not separately acquired | Derived from Altaria through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 932 | Banette Mega | SPECIES_BANETTE_MEGA | Form / battle transformation; not separately acquired | Derived from Banette through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 933 | Absol Mega | SPECIES_ABSOL_MEGA | Form / battle transformation; not separately acquired | Derived from Absol through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 934 | Glalie Mega | SPECIES_GLALIE_MEGA | Form / battle transformation; not separately acquired | Derived from Glalie through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 935 | Salamence Mega | SPECIES_SALAMENCE_MEGA | Form / battle transformation; not separately acquired | Derived from Salamence through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 936 | Metagross Mega | SPECIES_METAGROSS_MEGA | Form / battle transformation; not separately acquired | Derived from Metagross through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 937 | Latias Mega | SPECIES_LATIAS_MEGA | Form / battle transformation; not separately acquired | Derived from Latias through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 938 | Latios Mega | SPECIES_LATIOS_MEGA | Form / battle transformation; not separately acquired | Derived from Latios through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 939 | Lopunny Mega | SPECIES_LOPUNNY_MEGA | Form / battle transformation; not separately acquired | Derived from Lopunny through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 940 | Garchomp Mega | SPECIES_GARCHOMP_MEGA | Form / battle transformation; not separately acquired | Derived from Garchomp through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 941 | Lucario Mega | SPECIES_LUCARIO_MEGA | Form / battle transformation; not separately acquired | Derived from Lucario through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 942 | Abomasnow Mega | SPECIES_ABOMASNOW_MEGA | Form / battle transformation; not separately acquired | Derived from Abomasnow through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 943 | Gallade Mega | SPECIES_GALLADE_MEGA | Form / battle transformation; not separately acquired | Derived from Gallade through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 944 | Audino Mega | SPECIES_AUDINO_MEGA | Form / battle transformation; not separately acquired | Derived from Audino through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 945 | Diancie Mega | SPECIES_DIANCIE_MEGA | Form / battle transformation; not separately acquired | Derived from Diancie through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 946 | Rayquaza Mega | SPECIES_RAYQUAZA_MEGA | Form / battle transformation; not separately acquired | Derived from Rayquaza through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 947 | Kyogre Primal | SPECIES_KYOGRE_PRIMAL | Form / battle transformation; not separately acquired | Derived from Kyogre through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 948 | Groudon Primal | SPECIES_GROUDON_PRIMAL | Form / battle transformation; not separately acquired | Derived from Groudon through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 949 | Rattata Alolan | SPECIES_RATTATA_ALOLAN | Direct acquisition | Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 950 | Raticate Alolan | SPECIES_RATICATE_ALOLAN | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone Northwest / Land | party-selectable species/form |
| 951 | Raichu Alolan | SPECIES_RAICHU_ALOLAN | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone South / Land \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 952 | Sandshrew Alolan | SPECIES_SANDSHREW_ALOLAN | Direct acquisition | Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 953 | Sandslash Alolan | SPECIES_SANDSLASH_ALOLAN | Evolution from obtainable Pokémon | Sandshrew Alolan → Sandslash Alolan via Use/hold ITEM_ICE_STONE | party-selectable species/form |
| 954 | Vulpix Alolan | SPECIES_VULPIX_ALOLAN | Direct acquisition | Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 955 | Ninetales Alolan | SPECIES_NINETALES_ALOLAN | Evolution from obtainable Pokémon | Vulpix Alolan → Ninetales Alolan via Use/hold ITEM_ICE_STONE | party-selectable species/form |
| 956 | Diglett Alolan | SPECIES_DIGLETT_ALOLAN | Direct acquisition | Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 957 | Dugtrio Alolan | SPECIES_DUGTRIO_ALOLAN | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone North / Land | party-selectable species/form |
| 958 | Meowth Alolan | SPECIES_MEOWTH_ALOLAN | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone South / Land \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 959 | Persian Alolan | SPECIES_PERSIAN_ALOLAN | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone South / Land | party-selectable species/form |
| 960 | Geodude Alolan | SPECIES_GEODUDE_ALOLAN | Direct acquisition | Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 961 | Graveler Alolan | SPECIES_GRAVELER_ALOLAN | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone North / Rock Smash | party-selectable species/form |
| 962 | Golem Alolan | SPECIES_GOLEM_ALOLAN | Evolution from obtainable Pokémon | Graveler Alolan → Golem Alolan via Friendship | party-selectable species/form |
| 963 | Grimer Alolan | SPECIES_GRIMER_ALOLAN | Direct acquisition | Random wild in 3 catchable method pool(s): Safari Zone Northwest / Good Rod; Safari Zone Northwest / Old Rod; Safari Zone Northwest / Surf \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 964 | Muk Alolan | SPECIES_MUK_ALOLAN | Direct acquisition | Random wild in 3 catchable method pool(s): Safari Zone Northwest / Good Rod; Safari Zone Northwest / Super Rod; Safari Zone Northwest / Surf | party-selectable species/form |
| 965 | Exeggutor Alolan | SPECIES_EXEGGUTOR_ALOLAN | Direct acquisition | Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 966 | Marowak Alolan | SPECIES_MAROWAK_ALOLAN | Direct acquisition | Random wild in 1 catchable method pool(s): Safari Zone North / Land \| Prize Pokémon: Mauville Game Corner (Coin-exchange Pokémon prize) | party-selectable species/form |
| 967 | Meowth Galarian | SPECIES_MEOWTH_GALARIAN | Direct acquisition | Random wild in 1 catchable method pool(s): Route 116 / Land | party-selectable species/form |
| 968 | Ponyta Galarian | SPECIES_PONYTA_GALARIAN | Alternate form from obtainable base | Obtain Ponyta, then use the applicable form-change mechanic. | party-selectable species/form |
| 969 | Rapidash Galarian | SPECIES_RAPIDASH_GALARIAN | Alternate form from obtainable base | Obtain Rapidash, then use the applicable form-change mechanic. | party-selectable species/form |
| 970 | Slowpoke Galarian | SPECIES_SLOWPOKE_GALARIAN | Alternate form from obtainable base | Obtain Slowpoke, then use the applicable form-change mechanic. | party-selectable species/form |
| 971 | Slowbro Galarian | SPECIES_SLOWBRO_GALARIAN | Alternate form from obtainable base | Obtain Slowbro, then use the applicable form-change mechanic. | party-selectable species/form |
| 972 | Farfetchd Galarian | SPECIES_FARFETCHD_GALARIAN | Direct acquisition | Random wild in 1 catchable method pool(s): Route 117 / Land | party-selectable species/form |
| 973 | Weezing Galarian | SPECIES_WEEZING_GALARIAN | Alternate form from obtainable base | Obtain Weezing, then use the applicable form-change mechanic. | party-selectable species/form |
| 974 | Mr Mime Galarian | SPECIES_MR_MIME_GALARIAN | Direct acquisition | Random wild in 1 catchable method pool(s): Shoal Cave Low Tide Ice Room / Land | party-selectable species/form |
| 975 | Articuno Galarian | SPECIES_ARTICUNO_GALARIAN | Alternate form from obtainable base | Obtain Articuno, then use the applicable form-change mechanic. | party-selectable species/form |
| 976 | Zapdos Galarian | SPECIES_ZAPDOS_GALARIAN | Alternate form from obtainable base | Obtain Zapdos, then use the applicable form-change mechanic. | party-selectable species/form |
| 977 | Moltres Galarian | SPECIES_MOLTRES_GALARIAN | Alternate form from obtainable base | Obtain Moltres, then use the applicable form-change mechanic. | party-selectable species/form |
| 978 | Slowking Galarian | SPECIES_SLOWKING_GALARIAN | Alternate form from obtainable base | Obtain Slowking, then use the applicable form-change mechanic. | party-selectable species/form |
| 979 | Corsola Galarian | SPECIES_CORSOLA_GALARIAN | Direct acquisition | Random wild in 1 catchable method pool(s): Mt Pyre Exterior / Land | party-selectable species/form |
| 980 | Zigzagoon Galarian | SPECIES_ZIGZAGOON_GALARIAN | Direct acquisition | Random wild in 1 catchable method pool(s): Route 110 / Honey | party-selectable species/form |
| 981 | Linoone Galarian | SPECIES_LINOONE_GALARIAN | Evolution from obtainable Pokémon | Zigzagoon Galarian → Linoone Galarian via Level (20) | party-selectable species/form |
| 982 | Darumaka Galarian | SPECIES_DARUMAKA_GALARIAN | Alternate form from obtainable base | Obtain Darumaka, then use the applicable form-change mechanic. | party-selectable species/form |
| 983 | Darmanitan Galarian | SPECIES_DARMANITAN_GALARIAN | Alternate form from obtainable base | Obtain Darmanitan, then use the applicable form-change mechanic. | party-selectable species/form |
| 984 | Yamask Galarian | SPECIES_YAMASK_GALARIAN | Direct acquisition | Random wild in 1 catchable method pool(s): Route 111 / Land | party-selectable species/form |
| 985 | Stunfisk Galarian | SPECIES_STUNFISK_GALARIAN | Alternate form from obtainable base | Obtain Stunfisk, then use the applicable form-change mechanic. | party-selectable species/form |
| 986 | Pikachu Cosplay | SPECIES_PIKACHU_COSPLAY | Alternate form from obtainable base | Obtain Pikachu, then use the applicable form-change mechanic. | party-selectable species/form |
| 987 | Pikachu Rock Star | SPECIES_PIKACHU_ROCK_STAR | Alternate form from obtainable base | Obtain Pikachu, then use the applicable form-change mechanic. | party-selectable species/form |
| 988 | Pikachu Belle | SPECIES_PIKACHU_BELLE | Alternate form from obtainable base | Obtain Pikachu, then use the applicable form-change mechanic. | party-selectable species/form |
| 989 | Pikachu Pop Star | SPECIES_PIKACHU_POP_STAR | Alternate form from obtainable base | Obtain Pikachu, then use the applicable form-change mechanic. | party-selectable species/form |
| 990 | Pikachu Ph D | SPECIES_PIKACHU_PH_D | Alternate form from obtainable base | Obtain Pikachu, then use the applicable form-change mechanic. | party-selectable species/form |
| 991 | Pikachu Libre | SPECIES_PIKACHU_LIBRE | Alternate form from obtainable base | Obtain Pikachu, then use the applicable form-change mechanic. | party-selectable species/form |
| 992 | Pikachu Original Cap | SPECIES_PIKACHU_ORIGINAL_CAP | Alternate form from obtainable base | Obtain Pikachu, then use the applicable form-change mechanic. | party-selectable species/form |
| 993 | Pikachu Hoenn Cap | SPECIES_PIKACHU_HOENN_CAP | Alternate form from obtainable base | Obtain Pikachu, then use the applicable form-change mechanic. | party-selectable species/form |
| 994 | Pikachu Sinnoh Cap | SPECIES_PIKACHU_SINNOH_CAP | Alternate form from obtainable base | Obtain Pikachu, then use the applicable form-change mechanic. | party-selectable species/form |
| 995 | Pikachu Unova Cap | SPECIES_PIKACHU_UNOVA_CAP | Alternate form from obtainable base | Obtain Pikachu, then use the applicable form-change mechanic. | party-selectable species/form |
| 996 | Pikachu Kalos Cap | SPECIES_PIKACHU_KALOS_CAP | Alternate form from obtainable base | Obtain Pikachu, then use the applicable form-change mechanic. | party-selectable species/form |
| 997 | Pikachu Alola Cap | SPECIES_PIKACHU_ALOLA_CAP | Alternate form from obtainable base | Obtain Pikachu, then use the applicable form-change mechanic. | party-selectable species/form |
| 998 | Pikachu Partner Cap | SPECIES_PIKACHU_PARTNER_CAP | Alternate form from obtainable base | Obtain Pikachu, then use the applicable form-change mechanic. | party-selectable species/form |
| 999 | Pikachu World Cap | SPECIES_PIKACHU_WORLD_CAP | Alternate form from obtainable base | Obtain Pikachu, then use the applicable form-change mechanic. | party-selectable species/form |
| 1000 | Pichu Spiky Eared | SPECIES_PICHU_SPIKY_EARED | Alternate form from obtainable base | Obtain Pichu, then use the applicable form-change mechanic. | party-selectable species/form |
| 1001 | Unown B | SPECIES_UNOWN_B | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1002 | Unown C | SPECIES_UNOWN_C | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1003 | Unown D | SPECIES_UNOWN_D | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1004 | Unown E | SPECIES_UNOWN_E | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1005 | Unown F | SPECIES_UNOWN_F | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1006 | Unown G | SPECIES_UNOWN_G | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1007 | Unown H | SPECIES_UNOWN_H | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1008 | Unown I | SPECIES_UNOWN_I | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1009 | Unown J | SPECIES_UNOWN_J | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1010 | Unown K | SPECIES_UNOWN_K | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1011 | Unown L | SPECIES_UNOWN_L | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1012 | Unown M | SPECIES_UNOWN_M | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1013 | Unown N | SPECIES_UNOWN_N | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1014 | Unown O | SPECIES_UNOWN_O | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1015 | Unown P | SPECIES_UNOWN_P | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1016 | Unown Q | SPECIES_UNOWN_Q | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1017 | Unown R | SPECIES_UNOWN_R | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1018 | Unown S | SPECIES_UNOWN_S | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1019 | Unown T | SPECIES_UNOWN_T | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1020 | Unown U | SPECIES_UNOWN_U | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1021 | Unown V | SPECIES_UNOWN_V | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1022 | Unown W | SPECIES_UNOWN_W | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1023 | Unown X | SPECIES_UNOWN_X | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1024 | Unown Y | SPECIES_UNOWN_Y | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1025 | Unown Z | SPECIES_UNOWN_Z | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1026 | Unown Emark | SPECIES_UNOWN_EMARK | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1027 | Unown Qmark | SPECIES_UNOWN_QMARK | Form / battle transformation; not separately acquired | Derived from Unown through its form, personality, held-item, ability, or battle mechanic. | unown-personality-graphic-slot |
| 1028 | Castform Sunny | SPECIES_CASTFORM_SUNNY | Form / battle transformation; not separately acquired | Derived from Castform through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1029 | Castform Rainy | SPECIES_CASTFORM_RAINY | Form / battle transformation; not separately acquired | Derived from Castform through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1030 | Castform Snowy | SPECIES_CASTFORM_SNOWY | Form / battle transformation; not separately acquired | Derived from Castform through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1031 | Deoxys Attack | SPECIES_DEOXYS_ATTACK | Alternate form from obtainable base | Obtain Deoxys, then use the applicable form-change mechanic. | party-selectable species/form |
| 1032 | Deoxys Defense | SPECIES_DEOXYS_DEFENSE | Alternate form from obtainable base | Obtain Deoxys, then use the applicable form-change mechanic. | party-selectable species/form |
| 1033 | Deoxys Speed | SPECIES_DEOXYS_SPEED | Alternate form from obtainable base | Obtain Deoxys, then use the applicable form-change mechanic. | party-selectable species/form |
| 1034 | Burmy Sandy Cloak | SPECIES_BURMY_SANDY_CLOAK | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Burmy; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 1035 | Burmy Trash Cloak | SPECIES_BURMY_TRASH_CLOAK | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Burmy; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 1036 | Wormadam Sandy Cloak | SPECIES_WORMADAM_SANDY_CLOAK | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Burmy; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 1037 | Wormadam Trash Cloak | SPECIES_WORMADAM_TRASH_CLOAK | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Burmy; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 1038 | Cherrim Sunshine | SPECIES_CHERRIM_SUNSHINE | Form / battle transformation; not separately acquired | Derived from Cherrim through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1039 | Shellos East Sea | SPECIES_SHELLOS_EAST_SEA | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Gastrodon East Sea; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 1040 | Gastrodon East Sea | SPECIES_GASTRODON_EAST_SEA | Direct acquisition | Random wild in 1 catchable method pool(s): Route 119 / Surf | party-selectable species/form |
| 1041 | Rotom Heat | SPECIES_ROTOM_HEAT | Alternate form from obtainable base | Obtain Rotom, then use the applicable form-change mechanic. | party-selectable species/form |
| 1042 | Rotom Wash | SPECIES_ROTOM_WASH | Alternate form from obtainable base | Obtain Rotom, then use the applicable form-change mechanic. | party-selectable species/form |
| 1043 | Rotom Frost | SPECIES_ROTOM_FROST | Alternate form from obtainable base | Obtain Rotom, then use the applicable form-change mechanic. | party-selectable species/form |
| 1044 | Rotom Fan | SPECIES_ROTOM_FAN | Alternate form from obtainable base | Obtain Rotom, then use the applicable form-change mechanic. | party-selectable species/form |
| 1045 | Rotom Mow | SPECIES_ROTOM_MOW | Alternate form from obtainable base | Obtain Rotom, then use the applicable form-change mechanic. | party-selectable species/form |
| 1046 | Giratina Origin | SPECIES_GIRATINA_ORIGIN | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1047 | Shaymin Sky | SPECIES_SHAYMIN_SKY | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1048 | Arceus Fighting | SPECIES_ARCEUS_FIGHTING | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1049 | Arceus Flying | SPECIES_ARCEUS_FLYING | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1050 | Arceus Poison | SPECIES_ARCEUS_POISON | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1051 | Arceus Ground | SPECIES_ARCEUS_GROUND | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1052 | Arceus Rock | SPECIES_ARCEUS_ROCK | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1053 | Arceus Bug | SPECIES_ARCEUS_BUG | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1054 | Arceus Ghost | SPECIES_ARCEUS_GHOST | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1055 | Arceus Steel | SPECIES_ARCEUS_STEEL | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1056 | Arceus Fire | SPECIES_ARCEUS_FIRE | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1057 | Arceus Water | SPECIES_ARCEUS_WATER | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1058 | Arceus Grass | SPECIES_ARCEUS_GRASS | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1059 | Arceus Electric | SPECIES_ARCEUS_ELECTRIC | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1060 | Arceus Psychic | SPECIES_ARCEUS_PSYCHIC | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1061 | Arceus Ice | SPECIES_ARCEUS_ICE | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1062 | Arceus Dragon | SPECIES_ARCEUS_DRAGON | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1063 | Arceus Dark | SPECIES_ARCEUS_DARK | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1064 | Arceus Fairy | SPECIES_ARCEUS_FAIRY | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1065 | Basculin Blue Striped | SPECIES_BASCULIN_BLUE_STRIPED | Alternate form from obtainable base | Obtain Basculin, then use the applicable form-change mechanic. | party-selectable species/form |
| 1066 | Darmanitan Zen Mode | SPECIES_DARMANITAN_ZEN_MODE | Form / battle transformation; not separately acquired | Derived from Darmanitan through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1067 | Darmanitan Zen Mode Galarian | SPECIES_DARMANITAN_ZEN_MODE_GALARIAN | Form / battle transformation; not separately acquired | Derived from Darmanitan Zen Mode through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1068 | Deerling Summer | SPECIES_DEERLING_SUMMER | Alternate form from obtainable base | Obtain Deerling, then use the applicable form-change mechanic. | party-selectable species/form |
| 1069 | Deerling Autumn | SPECIES_DEERLING_AUTUMN | Alternate form from obtainable base | Obtain Deerling, then use the applicable form-change mechanic. | party-selectable species/form |
| 1070 | Deerling Winter | SPECIES_DEERLING_WINTER | Alternate form from obtainable base | Obtain Deerling, then use the applicable form-change mechanic. | party-selectable species/form |
| 1071 | Sawsbuck Summer | SPECIES_SAWSBUCK_SUMMER | Alternate form from obtainable base | Obtain Sawsbuck, then use the applicable form-change mechanic. | party-selectable species/form |
| 1072 | Sawsbuck Autumn | SPECIES_SAWSBUCK_AUTUMN | Alternate form from obtainable base | Obtain Sawsbuck, then use the applicable form-change mechanic. | party-selectable species/form |
| 1073 | Sawsbuck Winter | SPECIES_SAWSBUCK_WINTER | Alternate form from obtainable base | Obtain Sawsbuck, then use the applicable form-change mechanic. | party-selectable species/form |
| 1074 | Tornadus Therian | SPECIES_TORNADUS_THERIAN | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1075 | Thundurus Therian | SPECIES_THUNDURUS_THERIAN | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1076 | Landorus Therian | SPECIES_LANDORUS_THERIAN | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1077 | Kyurem White | SPECIES_KYUREM_WHITE | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1078 | Kyurem Black | SPECIES_KYUREM_BLACK | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1079 | Keldeo Resolute | SPECIES_KELDEO_RESOLUTE | Alternate form from obtainable base | Obtain Keldeo, then use the applicable form-change mechanic. | party-selectable species/form |
| 1080 | Meloetta Pirouette | SPECIES_MELOETTA_PIROUETTE | Form / battle transformation; not separately acquired | Derived from Meloetta through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1081 | Genesect Douse Drive | SPECIES_GENESECT_DOUSE_DRIVE | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1082 | Genesect Shock Drive | SPECIES_GENESECT_SHOCK_DRIVE | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1083 | Genesect Burn Drive | SPECIES_GENESECT_BURN_DRIVE | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1084 | Genesect Chill Drive | SPECIES_GENESECT_CHILL_DRIVE | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1085 | Greninja Battle Bond | SPECIES_GRENINJA_BATTLE_BOND | Direct acquisition | Pokémon Center mystery gift: Pokémon Centers (Requires FLAG_BADGE04_GET; arrives holding ITEM_COMET_SHARD) | party-selectable species/form |
| 1086 | Greninja Ash | SPECIES_GRENINJA_ASH | Form / battle transformation; not separately acquired | Derived from Greninja through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1087 | Vivillon Polar | SPECIES_VIVILLON_POLAR | Alternate form from obtainable base | Obtain Vivillon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1088 | Vivillon Tundra | SPECIES_VIVILLON_TUNDRA | Alternate form from obtainable base | Obtain Vivillon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1089 | Vivillon Continental | SPECIES_VIVILLON_CONTINENTAL | Alternate form from obtainable base | Obtain Vivillon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1090 | Vivillon Garden | SPECIES_VIVILLON_GARDEN | Alternate form from obtainable base | Obtain Vivillon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1091 | Vivillon Elegant | SPECIES_VIVILLON_ELEGANT | Alternate form from obtainable base | Obtain Vivillon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1092 | Vivillon Meadow | SPECIES_VIVILLON_MEADOW | Alternate form from obtainable base | Obtain Vivillon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1093 | Vivillon Modern | SPECIES_VIVILLON_MODERN | Alternate form from obtainable base | Obtain Vivillon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1094 | Vivillon Marine | SPECIES_VIVILLON_MARINE | Alternate form from obtainable base | Obtain Vivillon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1095 | Vivillon Archipelago | SPECIES_VIVILLON_ARCHIPELAGO | Alternate form from obtainable base | Obtain Vivillon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1096 | Vivillon High Plains | SPECIES_VIVILLON_HIGH_PLAINS | Alternate form from obtainable base | Obtain Vivillon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1097 | Vivillon Sandstorm | SPECIES_VIVILLON_SANDSTORM | Alternate form from obtainable base | Obtain Vivillon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1098 | Vivillon River | SPECIES_VIVILLON_RIVER | Alternate form from obtainable base | Obtain Vivillon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1099 | Vivillon Monsoon | SPECIES_VIVILLON_MONSOON | Alternate form from obtainable base | Obtain Vivillon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1100 | Vivillon Savanna | SPECIES_VIVILLON_SAVANNA | Alternate form from obtainable base | Obtain Vivillon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1101 | Vivillon Sun | SPECIES_VIVILLON_SUN | Alternate form from obtainable base | Obtain Vivillon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1102 | Vivillon Ocean | SPECIES_VIVILLON_OCEAN | Alternate form from obtainable base | Obtain Vivillon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1103 | Vivillon Jungle | SPECIES_VIVILLON_JUNGLE | Alternate form from obtainable base | Obtain Vivillon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1104 | Vivillon Fancy | SPECIES_VIVILLON_FANCY | Alternate form from obtainable base | Obtain Vivillon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1105 | Vivillon Poke Ball | SPECIES_VIVILLON_POKE_BALL | Direct acquisition | Random wild in 1 catchable method pool(s): Verdanturf Meadow / Land | party-selectable species/form |
| 1106 | Flabebe Yellow Flower | SPECIES_FLABEBE_YELLOW_FLOWER | Direct acquisition | Random wild in 1 catchable method pool(s): Dewford Meadow / Land | party-selectable species/form |
| 1107 | Flabebe Orange Flower | SPECIES_FLABEBE_ORANGE_FLOWER | Direct acquisition | Random wild in 1 catchable method pool(s): Dewford Meadow / Land | party-selectable species/form |
| 1108 | Flabebe Blue Flower | SPECIES_FLABEBE_BLUE_FLOWER | Direct acquisition | Random wild in 1 catchable method pool(s): Verdanturf Meadow / Land | party-selectable species/form |
| 1109 | Flabebe White Flower | SPECIES_FLABEBE_WHITE_FLOWER | Direct acquisition | Random wild in 1 catchable method pool(s): Verdanturf Meadow / Land | party-selectable species/form |
| 1110 | Floette Yellow Flower | SPECIES_FLOETTE_YELLOW_FLOWER | Evolution from obtainable Pokémon | Flabebe Yellow Flower → Floette Yellow Flower via Level (19) | party-selectable species/form |
| 1111 | Floette Orange Flower | SPECIES_FLOETTE_ORANGE_FLOWER | Evolution from obtainable Pokémon | Flabebe Orange Flower → Floette Orange Flower via Level (19) | party-selectable species/form |
| 1112 | Floette Blue Flower | SPECIES_FLOETTE_BLUE_FLOWER | Evolution from obtainable Pokémon | Flabebe Blue Flower → Floette Blue Flower via Level (19) | party-selectable species/form |
| 1113 | Floette White Flower | SPECIES_FLOETTE_WHITE_FLOWER | Direct acquisition | Random wild in 1 catchable method pool(s): Verdanturf Meadow / Land | party-selectable species/form |
| 1114 | Floette Eternal Flower | SPECIES_FLOETTE_ETERNAL_FLOWER | Alternate form from obtainable base | Obtain Floette, then use the applicable form-change mechanic. | party-selectable species/form |
| 1115 | Florges Yellow Flower | SPECIES_FLORGES_YELLOW_FLOWER | Evolution from obtainable Pokémon | Flabebe Yellow Flower → Floette Yellow Flower via Level (19) ; Floette Yellow Flower → Florges Yellow Flower via Use/hold ITEM_SHINY_STONE | party-selectable species/form |
| 1116 | Florges Orange Flower | SPECIES_FLORGES_ORANGE_FLOWER | Evolution from obtainable Pokémon | Flabebe Orange Flower → Floette Orange Flower via Level (19) ; Floette Orange Flower → Florges Orange Flower via Use/hold ITEM_SHINY_STONE | party-selectable species/form |
| 1117 | Florges Blue Flower | SPECIES_FLORGES_BLUE_FLOWER | Evolution from obtainable Pokémon | Flabebe Blue Flower → Floette Blue Flower via Level (19) ; Floette Blue Flower → Florges Blue Flower via Use/hold ITEM_SHINY_STONE | party-selectable species/form |
| 1118 | Florges White Flower | SPECIES_FLORGES_WHITE_FLOWER | Evolution from obtainable Pokémon | Floette White Flower → Florges White Flower via Use/hold ITEM_SHINY_STONE | party-selectable species/form |
| 1119 | Furfrou Heart Trim | SPECIES_FURFROU_HEART_TRIM | Alternate form from obtainable base | Obtain Furfrou, then use the applicable form-change mechanic. | party-selectable species/form |
| 1120 | Furfrou Star Trim | SPECIES_FURFROU_STAR_TRIM | Alternate form from obtainable base | Obtain Furfrou, then use the applicable form-change mechanic. | party-selectable species/form |
| 1121 | Furfrou Diamond Trim | SPECIES_FURFROU_DIAMOND_TRIM | Alternate form from obtainable base | Obtain Furfrou, then use the applicable form-change mechanic. | party-selectable species/form |
| 1122 | Furfrou Debutante Trim | SPECIES_FURFROU_DEBUTANTE_TRIM | Alternate form from obtainable base | Obtain Furfrou, then use the applicable form-change mechanic. | party-selectable species/form |
| 1123 | Furfrou Matron Trim | SPECIES_FURFROU_MATRON_TRIM | Alternate form from obtainable base | Obtain Furfrou, then use the applicable form-change mechanic. | party-selectable species/form |
| 1124 | Furfrou Dandy Trim | SPECIES_FURFROU_DANDY_TRIM | Alternate form from obtainable base | Obtain Furfrou, then use the applicable form-change mechanic. | party-selectable species/form |
| 1125 | Furfrou La Reine Trim | SPECIES_FURFROU_LA_REINE_TRIM | Alternate form from obtainable base | Obtain Furfrou, then use the applicable form-change mechanic. | party-selectable species/form |
| 1126 | Furfrou Kabuki Trim | SPECIES_FURFROU_KABUKI_TRIM | Alternate form from obtainable base | Obtain Furfrou, then use the applicable form-change mechanic. | party-selectable species/form |
| 1127 | Furfrou Pharaoh Trim | SPECIES_FURFROU_PHARAOH_TRIM | Alternate form from obtainable base | Obtain Furfrou, then use the applicable form-change mechanic. | party-selectable species/form |
| 1128 | Meowstic Female | SPECIES_MEOWSTIC_FEMALE | Evolution from obtainable Pokémon | Espurr → Meowstic Female via Level Female (25) | party-selectable species/form |
| 1129 | Aegislash Blade | SPECIES_AEGISLASH_BLADE | Form / battle transformation; not separately acquired | Derived from Aegislash through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1130 | Pumpkaboo Small | SPECIES_PUMPKABOO_SMALL | Direct acquisition | Random wild in 1 catchable method pool(s): Route 120 / Land | party-selectable species/form |
| 1131 | Pumpkaboo Large | SPECIES_PUMPKABOO_LARGE | Direct acquisition | Random wild in 1 catchable method pool(s): Route 120 / Land | party-selectable species/form |
| 1132 | Pumpkaboo Super | SPECIES_PUMPKABOO_SUPER | Direct acquisition | Random wild in 1 catchable method pool(s): Route 120 / Land | party-selectable species/form |
| 1133 | Gourgeist Small | SPECIES_GOURGEIST_SMALL | Evolution from obtainable Pokémon | Pumpkaboo Small → Gourgeist Small via Use/hold ITEM_DUSK_STONE | party-selectable species/form |
| 1134 | Gourgeist Large | SPECIES_GOURGEIST_LARGE | Evolution from obtainable Pokémon | Pumpkaboo Large → Gourgeist Large via Use/hold ITEM_DUSK_STONE | party-selectable species/form |
| 1135 | Gourgeist Super | SPECIES_GOURGEIST_SUPER | Evolution from obtainable Pokémon | Pumpkaboo Super → Gourgeist Super via Use/hold ITEM_DUSK_STONE | party-selectable species/form |
| 1136 | Xerneas Active | SPECIES_XERNEAS_ACTIVE | Form / battle transformation; not separately acquired | Derived from Xerneas through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1137 | Zygarde 10 | SPECIES_ZYGARDE_10 | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1138 | Zygarde 10 Power Construct | SPECIES_ZYGARDE_10_POWER_CONSTRUCT | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1139 | Zygarde 50 Power Construct | SPECIES_ZYGARDE_50_POWER_CONSTRUCT | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1140 | Zygarde Complete | SPECIES_ZYGARDE_COMPLETE | Form / battle transformation; not separately acquired | Derived from Zygarde through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1141 | Hoopa Unbound | SPECIES_HOOPA_UNBOUND | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1142 | Oricorio Pom Pom | SPECIES_ORICORIO_POM_POM | Alternate form from obtainable base | Obtain Oricorio, then use the applicable form-change mechanic. | party-selectable species/form |
| 1143 | Oricorio Pau | SPECIES_ORICORIO_PAU | Alternate form from obtainable base | Obtain Oricorio, then use the applicable form-change mechanic. | party-selectable species/form |
| 1144 | Oricorio Sensu | SPECIES_ORICORIO_SENSU | Alternate form from obtainable base | Obtain Oricorio, then use the applicable form-change mechanic. | party-selectable species/form |
| 1145 | Rockruff Own Tempo | SPECIES_ROCKRUFF_OWN_TEMPO | Breeding / obtainable evolution family | The permanent evolution family is obtainable through Lycanroc; breed or traverse the applicable branch to obtain this stage. | party-selectable species/form |
| 1146 | Lycanroc Midnight | SPECIES_LYCANROC_MIDNIGHT | Evolution from obtainable Pokémon | Rockruff → Lycanroc Midnight via Level Night (25) | party-selectable species/form |
| 1147 | Lycanroc Dusk | SPECIES_LYCANROC_DUSK | Evolution from obtainable Pokémon | Rockruff → Lycanroc Dusk via Level Dusk (25) | party-selectable species/form |
| 1148 | Wishiwashi School | SPECIES_WISHIWASHI_SCHOOL | Form / battle transformation; not separately acquired | Derived from Wishiwashi through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1149 | Silvally Fighting | SPECIES_SILVALLY_FIGHTING | Alternate form from obtainable base | Obtain Silvally, then use the applicable form-change mechanic. | party-selectable species/form |
| 1150 | Silvally Flying | SPECIES_SILVALLY_FLYING | Alternate form from obtainable base | Obtain Silvally, then use the applicable form-change mechanic. | party-selectable species/form |
| 1151 | Silvally Poison | SPECIES_SILVALLY_POISON | Alternate form from obtainable base | Obtain Silvally, then use the applicable form-change mechanic. | party-selectable species/form |
| 1152 | Silvally Ground | SPECIES_SILVALLY_GROUND | Alternate form from obtainable base | Obtain Silvally, then use the applicable form-change mechanic. | party-selectable species/form |
| 1153 | Silvally Rock | SPECIES_SILVALLY_ROCK | Alternate form from obtainable base | Obtain Silvally, then use the applicable form-change mechanic. | party-selectable species/form |
| 1154 | Silvally Bug | SPECIES_SILVALLY_BUG | Alternate form from obtainable base | Obtain Silvally, then use the applicable form-change mechanic. | party-selectable species/form |
| 1155 | Silvally Ghost | SPECIES_SILVALLY_GHOST | Alternate form from obtainable base | Obtain Silvally, then use the applicable form-change mechanic. | party-selectable species/form |
| 1156 | Silvally Steel | SPECIES_SILVALLY_STEEL | Alternate form from obtainable base | Obtain Silvally, then use the applicable form-change mechanic. | party-selectable species/form |
| 1157 | Silvally Fire | SPECIES_SILVALLY_FIRE | Alternate form from obtainable base | Obtain Silvally, then use the applicable form-change mechanic. | party-selectable species/form |
| 1158 | Silvally Water | SPECIES_SILVALLY_WATER | Alternate form from obtainable base | Obtain Silvally, then use the applicable form-change mechanic. | party-selectable species/form |
| 1159 | Silvally Grass | SPECIES_SILVALLY_GRASS | Alternate form from obtainable base | Obtain Silvally, then use the applicable form-change mechanic. | party-selectable species/form |
| 1160 | Silvally Electric | SPECIES_SILVALLY_ELECTRIC | Alternate form from obtainable base | Obtain Silvally, then use the applicable form-change mechanic. | party-selectable species/form |
| 1161 | Silvally Psychic | SPECIES_SILVALLY_PSYCHIC | Alternate form from obtainable base | Obtain Silvally, then use the applicable form-change mechanic. | party-selectable species/form |
| 1162 | Silvally Ice | SPECIES_SILVALLY_ICE | Alternate form from obtainable base | Obtain Silvally, then use the applicable form-change mechanic. | party-selectable species/form |
| 1163 | Silvally Dragon | SPECIES_SILVALLY_DRAGON | Alternate form from obtainable base | Obtain Silvally, then use the applicable form-change mechanic. | party-selectable species/form |
| 1164 | Silvally Dark | SPECIES_SILVALLY_DARK | Alternate form from obtainable base | Obtain Silvally, then use the applicable form-change mechanic. | party-selectable species/form |
| 1165 | Silvally Fairy | SPECIES_SILVALLY_FAIRY | Alternate form from obtainable base | Obtain Silvally, then use the applicable form-change mechanic. | party-selectable species/form |
| 1166 | Minior Meteor Orange | SPECIES_MINIOR_METEOR_ORANGE | Alternate form from obtainable base | Obtain Minior, then use the applicable form-change mechanic. | party-selectable species/form |
| 1167 | Minior Meteor Yellow | SPECIES_MINIOR_METEOR_YELLOW | Alternate form from obtainable base | Obtain Minior, then use the applicable form-change mechanic. | party-selectable species/form |
| 1168 | Minior Meteor Green | SPECIES_MINIOR_METEOR_GREEN | Alternate form from obtainable base | Obtain Minior, then use the applicable form-change mechanic. | party-selectable species/form |
| 1169 | Minior Meteor Blue | SPECIES_MINIOR_METEOR_BLUE | Alternate form from obtainable base | Obtain Minior, then use the applicable form-change mechanic. | party-selectable species/form |
| 1170 | Minior Meteor Indigo | SPECIES_MINIOR_METEOR_INDIGO | Alternate form from obtainable base | Obtain Minior, then use the applicable form-change mechanic. | party-selectable species/form |
| 1171 | Minior Meteor Violet | SPECIES_MINIOR_METEOR_VIOLET | Alternate form from obtainable base | Obtain Minior, then use the applicable form-change mechanic. | party-selectable species/form |
| 1172 | Minior Core Red | SPECIES_MINIOR_CORE_RED | Form / battle transformation; not separately acquired | Derived from Minior through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1173 | Minior Core Orange | SPECIES_MINIOR_CORE_ORANGE | Form / battle transformation; not separately acquired | Derived from Minior through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1174 | Minior Core Yellow | SPECIES_MINIOR_CORE_YELLOW | Form / battle transformation; not separately acquired | Derived from Minior through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1175 | Minior Core Green | SPECIES_MINIOR_CORE_GREEN | Form / battle transformation; not separately acquired | Derived from Minior through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1176 | Minior Core Blue | SPECIES_MINIOR_CORE_BLUE | Form / battle transformation; not separately acquired | Derived from Minior through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1177 | Minior Core Indigo | SPECIES_MINIOR_CORE_INDIGO | Form / battle transformation; not separately acquired | Derived from Minior through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1178 | Minior Core Violet | SPECIES_MINIOR_CORE_VIOLET | Form / battle transformation; not separately acquired | Derived from Minior through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1179 | Mimikyu Busted | SPECIES_MIMIKYU_BUSTED | Form / battle transformation; not separately acquired | Derived from Mimikyu through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1180 | Necrozma Dusk Mane | SPECIES_NECROZMA_DUSK_MANE | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1181 | Necrozma Dawn Wings | SPECIES_NECROZMA_DAWN_WINGS | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1182 | Necrozma Ultra | SPECIES_NECROZMA_ULTRA | Form / battle transformation; not separately acquired | Derived from Necrozma through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1183 | Magearna Original Color | SPECIES_MAGEARNA_ORIGINAL_COLOR | Alternate form from obtainable base | Obtain Magearna, then use the applicable form-change mechanic. | party-selectable species/form |
| 1184 | Cramorant Gulping | SPECIES_CRAMORANT_GULPING | Form / battle transformation; not separately acquired | Derived from Cramorant through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1185 | Cramorant Gorging | SPECIES_CRAMORANT_GORGING | Form / battle transformation; not separately acquired | Derived from Cramorant through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1186 | Toxtricity Low Key | SPECIES_TOXTRICITY_LOW_KEY | Evolution from obtainable Pokémon | Toxel → Toxtricity Low Key via Level (30) | party-selectable species/form |
| 1187 | Sinistea Antique | SPECIES_SINISTEA_ANTIQUE | Alternate form from obtainable base | Obtain Sinistea, then use the applicable form-change mechanic. | party-selectable species/form |
| 1188 | Polteageist Antique | SPECIES_POLTEAGEIST_ANTIQUE | Alternate form from obtainable base | Obtain Polteageist, then use the applicable form-change mechanic. | party-selectable species/form |
| 1189 | Alcremie Ruby Cream | SPECIES_ALCREMIE_RUBY_CREAM | Evolution from obtainable Pokémon | Milcery → Alcremie Ruby Cream via Use/hold ITEM_NONE | party-selectable species/form |
| 1190 | Alcremie Matcha Cream | SPECIES_ALCREMIE_MATCHA_CREAM | Evolution from obtainable Pokémon | Milcery → Alcremie Matcha Cream via Use/hold ITEM_NONE | party-selectable species/form |
| 1191 | Alcremie Mint Cream | SPECIES_ALCREMIE_MINT_CREAM | Evolution from obtainable Pokémon | Milcery → Alcremie Mint Cream via Use/hold ITEM_NONE | party-selectable species/form |
| 1192 | Alcremie Lemon Cream | SPECIES_ALCREMIE_LEMON_CREAM | Evolution from obtainable Pokémon | Milcery → Alcremie Lemon Cream via Use/hold ITEM_NONE | party-selectable species/form |
| 1193 | Alcremie Salted Cream | SPECIES_ALCREMIE_SALTED_CREAM | Evolution from obtainable Pokémon | Milcery → Alcremie Salted Cream via Use/hold ITEM_NONE | party-selectable species/form |
| 1194 | Alcremie Ruby Swirl | SPECIES_ALCREMIE_RUBY_SWIRL | Evolution from obtainable Pokémon | Milcery → Alcremie Ruby Swirl via Use/hold ITEM_NONE | party-selectable species/form |
| 1195 | Alcremie Caramel Swirl | SPECIES_ALCREMIE_CARAMEL_SWIRL | Evolution from obtainable Pokémon | Milcery → Alcremie Caramel Swirl via Use/hold ITEM_NONE | party-selectable species/form |
| 1196 | Alcremie Rainbow Swirl | SPECIES_ALCREMIE_RAINBOW_SWIRL | Evolution from obtainable Pokémon | Milcery → Alcremie Rainbow Swirl via Use/hold ITEM_NONE | party-selectable species/form |
| 1197 | Eiscue Noice Face | SPECIES_EISCUE_NOICE_FACE | Form / battle transformation; not separately acquired | Derived from Eiscue through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1198 | Indeedee Female | SPECIES_INDEEDEE_FEMALE | Alternate form from obtainable base | Obtain Indeedee, then use the applicable form-change mechanic. | party-selectable species/form |
| 1199 | Morpeko Hangry | SPECIES_MORPEKO_HANGRY | Form / battle transformation; not separately acquired | Derived from Morpeko through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1200 | Zacian Crowned Sword | SPECIES_ZACIAN_CROWNED_SWORD | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1201 | Zamazenta Crowned Shield | SPECIES_ZAMAZENTA_CROWNED_SHIELD | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1202 | Eternatus Eternamax | SPECIES_ETERNATUS_ETERNAMAX | Form / battle transformation; not separately acquired | Derived from Eternatus through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1203 | Urshifu Rapid Strike Style | SPECIES_URSHIFU_RAPID_STRIKE_STYLE | Evolution from obtainable Pokémon | Kubfu → Urshifu Rapid Strike Style via Level (0) | party-selectable species/form |
| 1204 | Zarude Dada | SPECIES_ZARUDE_DADA | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1205 | Calyrex Ice Rider | SPECIES_CALYREX_ICE_RIDER | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1206 | Calyrex Shadow Rider | SPECIES_CALYREX_SHADOW_RIDER | Unresolved by automated acquisition scan | No direct random, scripted, gift, trade, fossil, or permanent-evolution path was resolved from the audited sources. | party-selectable species/form |
| 1207 | Milotic Mega | SPECIES_MILOTIC_MEGA | Form / battle transformation; not separately acquired | Derived from Milotic through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1208 | Butterfree Mega | SPECIES_BUTTERFREE_MEGA | Form / battle transformation; not separately acquired | Derived from Butterfree through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1209 | Machamp Mega | SPECIES_MACHAMP_MEGA | Form / battle transformation; not separately acquired | Derived from Machamp through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1210 | Kingler Mega | SPECIES_KINGLER_MEGA | Form / battle transformation; not separately acquired | Derived from Kingler through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1211 | Lapras Mega | SPECIES_LAPRAS_MEGA | Form / battle transformation; not separately acquired | Derived from Lapras through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1212 | Flygon Mega | SPECIES_FLYGON_MEGA | Form / battle transformation; not separately acquired | Derived from Flygon through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1213 | Kingdra Mega | SPECIES_KINGDRA_MEGA | Form / battle transformation; not separately acquired | Derived from Kingdra through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1214 | Meganium Mega | SPECIES_MEGANIUM_MEGA | Form / battle transformation; not separately acquired | Derived from Meganium through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1215 | Feraligatr Mega | SPECIES_FERALIGATR_MEGA | Form / battle transformation; not separately acquired | Derived from Feraligatr through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1216 | Emboar Mega | SPECIES_EMBOAR_MEGA | Form / battle transformation; not separately acquired | Derived from Emboar through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1217 | Raichu Mega X | SPECIES_RAICHU_MEGA_X | Form / battle transformation; not separately acquired | Derived from Raichu through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1218 | Raichu Mega Y | SPECIES_RAICHU_MEGA_Y | Form / battle transformation; not separately acquired | Derived from Raichu through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1219 | Dragonite Mega | SPECIES_DRAGONITE_MEGA | Form / battle transformation; not separately acquired | Derived from Dragonite through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1220 | Excadrill Mega | SPECIES_EXCADRILL_MEGA | Form / battle transformation; not separately acquired | Derived from Excadrill through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1221 | Malamar Mega | SPECIES_MALAMAR_MEGA | Form / battle transformation; not separately acquired | Derived from Malamar through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1222 | Chandelure Mega | SPECIES_CHANDELURE_MEGA | Form / battle transformation; not separately acquired | Derived from Chandelure through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1223 | Hawlucha Mega | SPECIES_HAWLUCHA_MEGA | Form / battle transformation; not separately acquired | Derived from Hawlucha through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1224 | Greninja Mega | SPECIES_GRENINJA_MEGA | Form / battle transformation; not separately acquired | Derived from Greninja through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1226 | Sprigatito | SPECIES_SPRIGATITO | Direct acquisition | Random wild in 1 catchable method pool(s): Route 101 / Land | party-selectable species/form |
| 1227 | Floragato | SPECIES_FLORAGATO | Evolution from obtainable Pokémon | Sprigatito → Floragato via Level (16) | party-selectable species/form |
| 1228 | Meowscarada | SPECIES_MEOWSCARADA | Evolution from obtainable Pokémon | Sprigatito → Floragato via Level (16) ; Floragato → Meowscarada via Level (36) | party-selectable species/form |
| 1229 | Fuecoco | SPECIES_FUECOCO | Direct acquisition | Random wild in 1 catchable method pool(s): Route 103 / Land | party-selectable species/form |
| 1230 | Crocalor | SPECIES_CROCALOR | Evolution from obtainable Pokémon | Fuecoco → Crocalor via Level (16) | party-selectable species/form |
| 1231 | Skeledirge | SPECIES_SKELEDIRGE | Evolution from obtainable Pokémon | Fuecoco → Crocalor via Level (16) ; Crocalor → Skeledirge via Level (36) | party-selectable species/form |
| 1232 | Nacli | SPECIES_NACLI | Direct acquisition | Random wild in 1 catchable method pool(s): Route 102 / Land | party-selectable species/form |
| 1233 | Naclstack | SPECIES_NACLSTACK | Evolution from obtainable Pokémon | Nacli → Naclstack via Level (24) | party-selectable species/form |
| 1234 | Garganacl | SPECIES_GARGANACL | Evolution from obtainable Pokémon | Nacli → Naclstack via Level (24) ; Naclstack → Garganacl via Level (38) | party-selectable species/form |
| 1235 | Finizen | SPECIES_FINIZEN | Direct acquisition | Random wild in 1 catchable method pool(s): Route 104 / Good Rod | party-selectable species/form |
| 1236 | Palafin | SPECIES_PALAFIN | Evolution from obtainable Pokémon | Finizen → Palafin via Level (38) | party-selectable species/form |
| 1237 | Palafin Hero | SPECIES_PALAFIN_HERO | Form / battle transformation; not separately acquired | Derived from Palafin through its form, personality, held-item, ability, or battle mechanic. | automatic-or-battle-only-form |
| 1238 | Dondozo | SPECIES_DONDOZO | Direct acquisition | Random wild in 1 catchable method pool(s): Route 118 / Surf | party-selectable species/form |
| 1239 | Tatsugiri | SPECIES_TATSUGIRI | Direct acquisition | Random wild in 1 catchable method pool(s): Route 118 / Good Rod | party-selectable species/form |
| 1240 | Tatsugiri Droopy | SPECIES_TATSUGIRI_DROOPY | Direct acquisition | Random wild in 1 catchable method pool(s): Route 118 / Good Rod | party-selectable species/form |
| 1241 | Tatsugiri Stretchy | SPECIES_TATSUGIRI_STRETCHY | Direct acquisition | Random wild in 1 catchable method pool(s): Route 118 / Good Rod | party-selectable species/form |
| 1242 | Annihilape | SPECIES_ANNIHILAPE | Evolution from obtainable Pokémon | Primeape → Annihilape via Level while knowing MOVE_RAGE_FIST | party-selectable species/form |
| 1243 | Farigiraf | SPECIES_FARIGIRAF | Evolution from obtainable Pokémon | Girafarig → Farigiraf via Level while knowing MOVE_TWIN_BEAM | party-selectable species/form |
| 1244 | Kingambit | SPECIES_KINGAMBIT | Evolution from obtainable Pokémon | Pawniard → Bisharp via Level (42) ; Bisharp → Kingambit via Use/hold ITEM_LEADERS_CREST | party-selectable species/form |
| 1245 | Great Tusk | SPECIES_GREAT_TUSK | Direct acquisition | Random wild in 1 catchable method pool(s): Route 111 / Land | party-selectable species/form |
| 1246 | Flutter Mane | SPECIES_FLUTTER_MANE | Direct acquisition | Random wild in 1 catchable method pool(s): Mt Pyre Summit / Land | party-selectable species/form |
| 1247 | Iron Bundle | SPECIES_IRON_BUNDLE | Direct acquisition | Random wild in 1 catchable method pool(s): Shoal Cave Low Tide Ice Room / Land | party-selectable species/form |
| 1248 | Iron Hands | SPECIES_IRON_HANDS | Direct acquisition | Random wild in 1 catchable method pool(s): New Mauville Inside / Land | party-selectable species/form |
| 1249 | Glimmet | SPECIES_GLIMMET | Direct acquisition | Random wild in 1 catchable method pool(s): Granite Cave 1F / Land | party-selectable species/form |
| 1250 | Glimmora | SPECIES_GLIMMORA | Evolution from obtainable Pokémon | Glimmet → Glimmora via Level (35) | party-selectable species/form |
| 1251 | Gimmighoul | SPECIES_GIMMIGHOUL | Direct acquisition | Random wild in 1 catchable method pool(s): Rustboro City / Land | party-selectable species/form |
| 1252 | Gimmighoul Roaming | SPECIES_GIMMIGHOUL_ROAMING | Direct acquisition | Random wild in 1 catchable method pool(s): Route 110 / Land | party-selectable species/form |
| 1253 | Gholdengo | SPECIES_GHOLDENGO | Evolution from obtainable Pokémon | Gimmighoul → Gholdengo via Use/hold ITEM_GIMMIGHOUL_COIN | party-selectable species/form |
| 1254 | Chien Pao | SPECIES_CHIEN_PAO | Direct acquisition | Random wild in 1 catchable method pool(s): Shoal Cave Low Tide Ice Room / Land | party-selectable species/form |
| 1255 | Ting Lu | SPECIES_TING_LU | Direct acquisition | Random wild in 1 catchable method pool(s): Desert Underpass / Land | party-selectable species/form |
| 1256 | Chi Yu | SPECIES_CHI_YU | Direct acquisition | Random wild in 1 catchable method pool(s): Ashen Woods / Land | party-selectable species/form |
| 1257 | Roaring Moon | SPECIES_ROARING_MOON | Direct acquisition | Random wild in 1 catchable method pool(s): Meteor Falls B1F 2R / Land | party-selectable species/form |
| 1258 | Iron Valiant | SPECIES_IRON_VALIANT | Direct acquisition | Random wild in 1 catchable method pool(s): Victory Road 1F / Land | party-selectable species/form |
| 1259 | Walking Wake | SPECIES_WALKING_WAKE | Direct acquisition | Random wild in 1 catchable method pool(s): Cave Of Origin 1F / Land | party-selectable species/form |
| 1260 | Ogerpon | SPECIES_OGERPON | Direct acquisition | Random wild in 1 catchable method pool(s): Route 120 / Land | party-selectable species/form |
| 1261 | Ogerpon Wellspring | SPECIES_OGERPON_WELLSPRING | Alternate form from obtainable base | Obtain Ogerpon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1262 | Ogerpon Hearthflame | SPECIES_OGERPON_HEARTHFLAME | Alternate form from obtainable base | Obtain Ogerpon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1263 | Ogerpon Cornerstone | SPECIES_OGERPON_CORNERSTONE | Alternate form from obtainable base | Obtain Ogerpon, then use the applicable form-change mechanic. | party-selectable species/form |
| 1264 | Archaludon | SPECIES_ARCHALUDON | Evolution from obtainable Pokémon | Duraludon → Archaludon via Use/hold ITEM_METAL_ALLOY | party-selectable species/form |
| 1265 | Gouging Fire | SPECIES_GOUGING_FIRE | Direct acquisition | Random wild in 1 catchable method pool(s): Magma Hideout 1F / Land | party-selectable species/form |
| 1266 | Raging Bolt | SPECIES_RAGING_BOLT | Direct acquisition | Random wild in 1 catchable method pool(s): Route 119 / Land | party-selectable species/form |
| 1267 | Tatsugiri Mega | SPECIES_TATSUGIRI_MEGA | Form / battle transformation; not separately acquired | Derived from Tatsugiri through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1268 | Tatsugiri Droopy Mega | SPECIES_TATSUGIRI_DROOPY_MEGA | Form / battle transformation; not separately acquired | Derived from Tatsugiri Droopy through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1269 | Tatsugiri Stretchy Mega | SPECIES_TATSUGIRI_STRETCHY_MEGA | Form / battle transformation; not separately acquired | Derived from Tatsugiri Stretchy through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |
| 1270 | Glimmora Mega | SPECIES_GLIMMORA_MEGA | Form / battle transformation; not separately acquired | Derived from Glimmora through its form, personality, held-item, ability, or battle mechanic. | battle-transformation-endpoint |

## Limitations and robustness checks

- Every method pool was recomputed from slot weights and validated to sum independently to 100%.
- Effective encounter odds can change through Repel, terrain, lead abilities, outbreaks, scripted overrides, and facility scaling. The base conditional tables remain the canonical distribution reported here.
- Static/gift extraction combines explicit script commands with the checked-in bespoke acquisition ledger, starter tables, fossils, mystery gifts, Game Corner prizes, trades, and roamers. Variable-driven or future event systems may still require manual annotation.
- Alternate and battle-only forms are often not separate acquisitions. The appendix separates them from permanent obtainable species rather than pretending every graphics/form ID is independently catchable.
- Ordinary held-item rewards are replaced as part of the cohesion pass now that competitive items are free. Berries, evolution catalysts, and protected transformation/form items remain meaningful progression.
- The portable report intentionally uses exact tables rather than a summary chart: this artifact is optimized for complete lookup and language-model ingestion, and a chart would hide the map/method/species detail that is the point of the report.

## Recommended next refinement

Use this report to evaluate chapter-by-chapter roster quality, redundancy, missing competitive roles, overconcentration of premium species, unresolved acquisition flags, and whether direct access occurs at the intended cap. Any proposed change should update the source encounter table or acquisition script first and then regenerate this document.

## Further questions for the next model

1. Are all unresolved acquisition rows true gaps, or are some reachable through variable-driven scripts not captured by the scanner?
2. Does each chapter expose enough distinct competitive roles—not merely enough species—to support the battle difficulty at that cap?
3. Which now-redundant ordinary held-item rewards should become Pokémon, protected transformation items, services, invitations, or story access?
4. Are any species technically available but placed so late that their intended competitive identity has little campaign value?
