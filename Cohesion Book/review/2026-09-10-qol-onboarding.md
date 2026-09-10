# Early quality-of-life onboarding — September 10, 2026

## Scope and authority

Reviewed executable C, current configuration, and connected map scripts to redistribute the opening's explanations through Rustboro, with reinforcement in Dewford. This is an onboarding coverage pass, not certification of every NPC or every battle mechanic in the entire game. Existing legendary/weather introductions remain intact. No mechanics, rewards, event flags, or trainer teams were changed in this pass.

“Available before Roxanne” does not mean every player is forced to read it. Birch, Norman, and the first Rustboro Gym welcome introduce essential preparation and point to the Center and school. Optional NPCs and the notebook hold detailed, repeatable lessons. No new tutorial menu or forced school visit was added.

## Coverage ledger

| Mechanic | Executable owner | Introduction / reinforcement |
|---|---|---|
| No battle experience; reusable whole-party Leveler; badge caps | `include/config/battle.h`, `src/party_menu.c`, `src/caps.c` | Birch before Route 103; first Center tools; Oldale house; Rustboro Gym; Dewford Center reminds players of cap 20 |
| Leveler also checks eligible evolutions, including at cap | `src/party_menu.c` | Oldale house; Rustboro outdoor boy; Bonding service |
| Battle Sets prepare moves, Nature, Ability, EVs and item; overwrite current build; preview/confirm; editable afterward | `src/emerald_champions_battle_sets.c`, `data/scripts/emerald_champions.inc` | Birch points to service; Oldale PC gentleman explains; service itself explains overwrite and separate leveling |
| Free legal move preparation, rather than level-up move prompts | `src/move_relearner.c`, `include/config/pokemon.h` | Oldale house; existing Tutor service |
| EV allocation: 252/stat, 510 total, free editing | `src/pokemon.c`, `src/field_specials.c`, `data/scripts/emerald_champions.inc` | Norman points to school; teacher gives definition and a 252/252/4 example; Tutor shows limits. Two EVs can remain unused in that example |
| Effective IVs fixed at 31; no breeding for better stats | `src/pokemon.c` | Rustboro Center Nature NPC; IVs are not a progression currency |
| Free Nature and available Ability changes | `src/field_specials.c`, `src/party_menu.c` | Rustboro Center Nature NPC names Tutor and party-menu access |
| Friendship adjustment and evolution conditions | `src/field_specials.c`, Bonding script | School student points to Tutor → Other Services → Bonding; existing service explains Leveler |
| Restored held items, including consumed berries; Bag consumables remain spent | `src/battle_util.c:TryRestoreHeldItems`, `src/battle_main.c` | School berry student distinguishes within-battle consumption from restoration afterward |
| RUN → restart current trainer battle; declining offers forfeit; declining again resumes | `src/battle_main.c`, `data/battle_scripts_1.s` | Birch introduces restart before rival; Norman explains choices and loss consequence. Applies to ordinary campaign trainer battles, not every special/link/facility mode |
| Start-menu Reload restores disk save, losing subsequent progress | `src/reload_save.c`, `src/start_menu.c` | Oldale saving girl; Norman distinguishes battle restart from Reload. Reload requires a valid save |
| R throws displayed Ball; hold R + directions cycles on action menu; release then tap to throw | `src/battle_controller_player.c`, `src/battle_interface.c` | Route 104 catching NPC; school notebook warns R in catchable wild battles throws rather than opening details |
| R opens move details in trainer battles | `src/battle_controller_player.c` | Mandatory Rustboro Gym welcome and school notebook |
| L foe-types panel; disabled when L acts as A | `src/battle_controller_player.c` | Gym welcome; notebook gives Options exception |
| Effectiveness symbols: hollow circle 1×, dotted circle 2×, star 4×, triangle ½×, inverted triangle ¼×, cross 0× | `MoveSelectionDisplayMoveEffectiveness` in `src/battle_controller_player.c` | Gym introduces strongest symbols; school notebook shows actual encoded glyphs and all meanings |
| Doubles effectiveness preview uses better living-foe matchup; selected target matters; status moves lack icons | `CheckTargetTypeEffectiveness`, move-target controller | School notebook explicitly explains limitations, avoiding a claim that the preview is an exact damage estimate |
| Difficulty changes opponent levels, not campaign plans: Easy −4, Medium −2, Hard full | `src/difficulty.c`, `src/option_menu.c` | School student; offsets apply after authored-level/live-cap floor, not directly to the player's level |
| Poké Vial one dose, full party outside-battle recovery, Center refill | `src/item_use.c`, nurse scripts | Existing first Center explanation; Oldale Center boy reinforces and teaches registered Key Item SELECT access |
| Repel Spray reusable 500 steps; fishing/Rock Smash/Sweet Scent still work | `src/item_use.c`, encounter code | Existing first Center tool explanation |
| Sweet Scent reverses ordinary species rarity; distinct legendary boost | `src/wild_encounter.c`, legendary encounter owner | Existing first Center introduction and route signs; precise encounter rules remain in those systems |
| Signs list encounters through fishing then legendaries; requirements and Caught; captures remove legendary availability | `src/legendary_signs.c`, roster/sign code | Existing mandatory Birch sendoff and Devon Woods explanation; route signs themselves |
| Legendary quests and weather; local progress-aware Center guide near PC | legendary encounter owner, guide scripts | Existing mandatory Birch/Devon dialogue, Center guides, Steven |
| Free battle held-item vendor; ordinary Mart consumables separate | Center vendor scripts | Existing nurse introduction and actual service menus; school berry dialogue clarifies consumption |
| HM field use requires HM, badge, eligible party member but no battle move slot | field-move code | Existing Cutter and school Scott; corrected nurse's abbreviated explanation; Dewford Center reinforces even fainted partner eligibility |
| Flight Beacon calls flier after Fly badge | item-use code | Existing tool introduction; corrected follow-up keeps Beacon distinct from other HM skills |
| Capturing with full party can swap partner into party | battle capture / party code | Existing Petalburg outdoor storage NPC |
| No walking poison damage | overworld poison code | Existing Petalburg Mart and school blackboard |
| Mega Ring + matching held stone; FIGHT then START; one partner per battle | battle gimmick code | Existing Steven acquisition dialogue after Brawly; this remains taught when usable rather than front-loading its full lesson before Roxanne |
| Evolution-item archive and legendary form tools unlock with Mega Ring; Mega Stones found separately | Center vendor and transformation code | Existing Steven archive dialogue and Battle Set service |

## Writing and implementation

Changed 21 text blocks across 13 source files. Preserved dialogue entry points, rewards, movement, and story state. Oldale and Rustboro no longer teach gaining experience from battles or faster leveling for traded Pokémon. The school has distinct voices: teacher explains EVs, one student experiments with difficulty, another discovers Bonding, and the berry student's notes distinguish held items from Bag items.

The mandatory Birch preparation message is five pages, Norman's sendoff seven, and the Gym welcome six. Each mixes practical next steps with local context. The eight-page notebook is an optional reference, not an interruption on the route. Existing weather/legendary conversations were not expanded again.

## Verification and limits

- Temporary source layout check: 21 edited blocks, 224 displayed lines; widest line 182 pixels against the 216-pixel textbox limit. Player/rival names are expanded to seven wide glyphs. Extended effectiveness symbols use a conservative eight-pixel cell. Checked explicit page/scroll breaks.
- Release build and artifact checks passed. ROM: 27,914,880 bytes; EWRAM: 232,282 bytes; IWRAM: 28,316 bytes. Input stamp matches 28,980 build inputs and both artifacts. See `work/qol-onboarding-20260910/build.log` and `verification.log`.
- Before snapshots, changed-label inventory and layout measurements are in that same directory. These identify this pass independently of unrelated working-tree changes.
- Follow-up native verification: rebuilt the headless ROM from the revised source. First Center acquisition passed its existing observed-result contract. Norman/Wally opening ran through the native catching tutorial and revised sendoff, finishing with Petalburg Gym state 2, controls unlocked, and script disabled. Evidence: `work/qol-onboarding-20260910/native-center/` and `native-norman.log`. This is scoped scene evidence, not a fresh-save playthrough or verification of every revised optional conversation.
- This ledger inventories player-facing preparation/navigation conveniences and the requested battle UI. It does not claim every imported generation rule, item interaction, evolution method, or later-game NPC has been exhaustively audited. Later chapters still need the ongoing source/dialogue review, with specialist explanations placed at actual unlocks.
