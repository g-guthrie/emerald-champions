# Opening dialogue review — September 10, 2026

## Boundary and evidence

Chapter 1: new-game introduction through winning the first Route 103 rival battle and returning to Birch. Source review covers every NPC accessible in this chapter, mandatory scenes, repeat conversations, gender variants, and the shared services those NPCs invoke. Mom's immediate Running Shoes send-off was also checked as the next transition.

The executable map scripts, C consumers, map object/coordinate events, flags, and text were the authority. This is a source audit with targeted native emulator checks, not a fresh-save recording of every conversation or a completed whole-game audit.

## NPC and scene checklist

| Place | People and interactions reviewed | Result |
|---|---|---|
| New-game introduction | Birch welcome, Pokémon explanation, main speech, gender/name confirmation, departure | Shortened the mechanics-heavy main speech; kept the two-partner premise, changing habitats, challenge and exploration. Center staff explain tools later. |
| Moving truck | All three boxes; arrival transition | Reflowed moving-company box text. |
| Littleroot exterior | Mom arrival; twin before/during/after rescue; PC/science man; Birch-fieldwork boy; town, house and lab signs | Clearer arrival and fieldwork directions. Running Shoes instructions shortened; removed the assumption that the party still contains only two Pokémon. |
| Player's house, both layouts | Mom arrival/upstairs clock scene/downstairs TV, repeat talk and healing; two Vigoroth; clock, PC email, notebook, books, GameCube, wall map and TV | TV report reflowed. Existing setup and home-rest sequence retained. Post-League Dad visit excluded from this chapter. |
| Rival's house, both layouts | Rival's mother and sibling; first rival meeting from each approach; region choice and confirmation; repeat packing dialogue; bedroom objects | Mother's repeat now acknowledges meeting the rival instead of saying they failed to notice the visit. Region selection remains nine choices. |
| Route 101 | Rescue scene: Birch, bag and two wild actors; youngster healing advice; boy's grass/catching advice; route sign | Birch now says to return to the lab together, matching the immediate transition. Catching advice tightened. Later roaming Birch is not active yet. |
| Birch's lab before rival | Aide before rescue and repeat; Birch rescue follow-up, both permanent partners, naming prompts, early Pokédex and supplies, rival directions; lab objects | Clear Route 103 destination through Oldale. Birch and aide repeat those directions while the battle is pending. No second Pokédex award at the later debrief. |
| Oldale exterior | Saving girl; Mart employee approach routes, introduction, Potion/supplies and repeat; footprint blocker; returning rival approach routes and direct interaction; signs | Guide no longer advertises unavailable opening Poké Balls. Saving and footprint lines tightened. Rival continues to direct the player back to Birch. |
| Oldale House 1 | Woman's doubles/usable-party explanation | Consistent with current party behavior. |
| Oldale House 2 | Woman explaining Leveler; man explaining team choices | Consistent with current free preparation tools. |
| Oldale Mart | Single clerk and opening/post-send-off inventory; stock woman; Potion boy | Stock dialogue points to actual opening supplies; Potion explanation distinguishes fainted Pokémon and directs the player to the Center. |
| Oldale Center 1F | Nurse first tools/normal healing/recovery prompts; PC gentleman; free-service boy; wireless girl; held-item vendor; move specialist; local legendary guide | First tools explanation now describes Sweet Scent favoring rarer ordinary species. Pre-send-off Shaymin hint first directs the player back to Birch, then west to Route 102 when it opens. |
| Center shared services | Vendor category/back/cancel; move learning and deletion; renaming; EV and Nature editing; Battle Set selection/preview/confirmation/errors; research/appearance branches and gates | Reviewed referenced scripts and C string builders, including nickname, move, Nature and item substitutions. Later unlocks remain gated. |
| Oldale Center 2F | Upstairs introduction, Union Room attendant, wireless information attendant, Direct Corner attendant; information, unavailable-adapter and cancel responses | Source reviewed. Mystery Gift delivery requires external setup and is absent from the fresh opening. Successful multiplayer sessions were not exercised. |
| Route 103 west bank | May/Brendan first battle introduction, win/return prompts and repeats; tired/Potion-advice boy; route sign | Story routes the player back through Oldale to Birch. East-bank trainers, swimmers, trees, items and berries are inaccessible in this chapter and deferred. |
| Birch's lab after rival | Automatic walk-up, debrief, 10 Great Balls to Bag/PC, storage-full response, retry, rival repeat and onward directions | Fixed a real automatic-scene loop. Pending gift uses lab state 6; state 4 starts the initial scene and state 5 marks completion. Speaking to Birch retries after making space. Successful completion opens the adventure exactly once. |

## Changes and story flow

The opening now introduces the journey before explaining its tools. The rescue leads immediately to Birch's lab; Birch gives both partners and the Pokédex before the rival battle, then directs the player north through Oldale to Route 103. Center staff supply and explain the preparation tools. Winning leads back to Birch for congratulations, 10 Great Balls and directions toward Route 102/Petalburg. Repeat dialogue follows those same milestones.

The storage bug mattered because the lab's automatic frame script was keyed to state 4. Leaving that state unchanged after both inventories rejected the gift restarted the approach scene. State 6 leaves the room usable and preserves the pending reward. No save migration is needed for this fresh-save revision.

## Text layout checks

The temporary inventory contains 466 text blocks / 1,452 nonempty lines across the opening maps and a broader shared-text superset. It includes unused and later-stage strings; those totals are not counts of opening conversations.

Checked explicit newline, scroll and page controls against the standard 27-tile (216-pixel) dialogue window and FONT_NORMAL widths. No measured line exceeded 216 pixels, and no extracted segment used multiple unscrolled newlines. A conservative 200-pixel warning threshold also identified lines for manual review; exceeding that margin alone is not a box overrun. Player/rival names were bounded at seven wide characters, nicknames at twelve, moves at sixteen, items at twenty, and Nature names separately. Remaining generic substitutions in the shared superset were inspected at their consumers where relevant to this chapter; the scanner is not a universal proof for every dynamic string in the game.

C-built Center previews use explicit page/scroll separators. Route-sign and local-guide formatting were reviewed alongside their current C code. Visual spot checks covered Birch's introduction, the seven-character-name debrief, completed Center visit and return to normal lab control. This does not claim every dialogue page was individually rendered.

## Runtime verification

- Native mGBA: both player genders, three Great Ball storage conditions each. Bag and PC fallback complete at lab state 5; both-full pauses at state 6 with controls and script execution unlocked.
- Native mGBA: making PC space, reloading the lab and manually speaking to Birch completes both full-storage cases. Final Great Ball count is exactly 10 in all six cases, including another interaction after completion.
- Existing first-Center-acquisition scenario: runtime state verified after the tools explanation and healing.
- Existing native `Center local guide reflects quest progress without unlocking discoveries` test: PASS.
- Release build and artifact/input verification: see `work/dialogue-opening-20260910/verification.log`.

Diagnostic scripts, inventories, before-edit snapshots, logs and screenshots are in `work/dialogue-opening-20260910/`. The focused Birch storage fixture extends the existing headless story-handoff scenario; the temporary text scanner is not a new permanent test framework.

## Next chapter

Continue with Route 102, every Petalburg NPC/interior, Norman and Wally's mandatory scenes, and the exits into Route 104. Recheck callbacks when returning to the opening towns at later milestones. Route 103's east bank belongs with its later accessible route segment. Unused legacy Birch single-starter/deferred-Pokédex text and postgame household scenes are not evidence about the current opening flow.
