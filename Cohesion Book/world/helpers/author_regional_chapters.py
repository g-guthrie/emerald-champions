from pathlib import Path
import json
W=Path(__file__).resolve().parents[1];members=json.loads((W/'chapter-membership.json').read_text())
texts={
'01-opening':r'''# The opening and the western woodland

The final opening is already a serious doubles game. INTRO-01 gives the player two chosen starters for the scripted Poochyena/Zigzagoon rescue, retains both partners, and provides the collection/preparation access needed before the four-Pokémon rival. The world’s contribution is a legible scene and a usable home region. It does not withhold tactical capabilities for later instruction.

The moving truck, mirrored houses, mother’s welcome, Birch’s fieldwork and Wally’s capture story retain Emerald’s personal scale. Ordinary villagers can joke, worry about their children, or discuss their Pokémon. They do not all need to explain competitive strategy. Preserve the family and settlement scenes where they already work. The opening/starter chapter owns every new bag-selection and Lab state transition; W-INTRO-GEOMETRY supplies the visible second opponent and safe field-return positions.

Oldale is the immediate preparation hub. Its Center already has the free specialist and held-item vendor, alongside healing and the Leveler/Vial/Repel Spray/Flight Beacon handoff. The first nurse visit should finish by offering ordinary healing in the same conversation. The nearby house’s lead-order advice becomes explicitly doubles advice. Keep the footprint joke as flavor, not a reason to prevent necessary preparation. The provisional revised opening must be traversed with both player genders, each starter-selection route, cancellation, a lost rescue, and a lost rival. Do not let a source-only successful path conceal a broken loss return.

Petalburg’s lake, father’s Gym and Wally’s family remain central. Wally’s demonstration is a story capture, not a trainer competition, and ordinary wild captures remain singles. Preserve the temporary party save/restore around his demonstration. A memory of a gentle family scene does not imply easy battles: the surrounding authored trainer encounters use the same tactical standard as the rest of the game.

Route104 and Petalburg Woods connect personal adventure to the wider conflict. Briney and Peeko, the flower shop, Shroomish-seeking Devon worker and Aqua ambush already provide distinct reasons to explore and care about this area. Keep those interactions and the wide roster. The exact wildlife revision must retain familiar forest anchors while preserving early babies, unevolved specialists and exotic choices that support interesting teams. W-SIGN-LOCAL changes the live deep-forest sign to truthful badge/landmark guidance; the unbound Breloom/Blissey ranger scripts are historical source, not live NPCs to restore automatically.

Rustboro is the first major integration test: the hard Gym, free preparation, Old Amber, evolution-item gifts, Devon, and eastbound tunnel rescue all need to agree. Keep the explicit goods-theft route: follow Route116 east, recover the parcel in Rusturf, and bring it back to the waiting worker. The reward and escort must not advance on an unsuccessful item handoff. The source already includes careful parcel/PC fallback paths; preserve their intent rather than making story access depend on a disposable Great Ball fitting in the Bag.

Rusturf remains a sound-sensitive habitat and a future shortcut, with Whismur nostalgia supplied by the acquisition volume. Peeko’s rescue and the lovers’ reunion remain separate states. The Strength HM reward is useful exploration, but its explanation must say that a compatible party member can use the licensed field technique without occupying a battle move slot. Neighboring Route116’s Dusk Stone seeker already recognizes the specific discovery and lets the player keep the stone; preserve that thoughtful interaction.

Seaspray remains a rewarding side discovery on the lower Route115 shore. Its early cold habitat is not automatically a mistake. Its encounters, form tools and Mega Stones should complement Shoal’s later discoveries under the global catalogue. The book preserves its two-floor routes and distinct wet/frozen presentation. It does not attach the unused explorer merely to repeat an obsolete Kingdra clue.

**Implementation sequence.** Apply INTRO-01 and the safe actor geometry together; verify Oldale preparation; apply W-OPENING-TIPS and the precise field/evolution clues; implement the individual early battles and acquisition changes; traverse all exits into the Woods, Rustboro, Route116, Rusturf and Seaspray. Keep the existing map IDs and local object identities. The complete per-map pages below specify every retained NPC, sign, trigger, warp and callback.
''',
'02-dewford-and-slateport':r'''# Dewford, Granite Cave and the port

The Dewford–Slateport loop should feel like a connected coastal journey with excellent team options throughout. The existing loop already has a clear purpose: deliver Stone’s letter to Steven, discover that Brawly is visiting Slateport, sail with Briney, find Brawly in the museum queue, then return for his hard doubles Gym and Steven’s Mega Ring. Preserve this structure and its explicit directions. It is a story and exploration loop, not a lesson prerequisite for strategic play.

Briney’s dialogue is state-sensitive. Before the letter is delivered, he points to Granite Cave; afterward he offers Slateport. The dock, boat, home and Peeko flags must move together. Slateport’s north guard explains that the Knuckle Badge permits passage and points the player back to the right boat. Test returning to every departure point before/after the letter, Brawly’s museum conversation and the museum battle sequence. A reachable destination is insufficient if its boat or return conversation disappears at the wrong time.

The Old Rod is a meaningful acquisition method during this visit. Keep the fishing NPC’s repeat advice and actual rod grant. Granite Cave offers a broader ecosystem than vanilla while retaining a convincing stone/cave identity. Its darkness, Flash HM and later field license must be checked through the actual active layout/light state. Steven’s letter, post-Brawly Ring, starter Mega reward and full-storage retry paths stay intact. His farewell currently contains an obsolete mandatory-Devon/Lucario-family instruction; W-SIGN-OPTIONAL repairs the text without altering the Ring or Sign gates.

Dewford Meadow and the manor expand the island’s identity. The meadow’s strong and unusual Pokémon are deliberate player ammunition, not automatically reserved for later. The haunted manor and its exploration rewards provide a different kind of discovery beside a small seaside town. The town woman already points toward it, and the manor sign describes the place. Most additional warden/historian scripts are not bound to objects; do not silently add a crowd of new explanatory NPCs to these quiet spaces. Use the actual present residents and landmarks.

Slateport’s market, Fan Club, shipyard and Oceanic Museum should all reward attention. Keep the prepared trade/gift Pokémon, Furfrou styling, friendship interactions and the sea exhibits. Evolution gifts can remain useful alternates even when the later Ring archive also supplies the item. Their personal context is part of the reward. Preserve the free battle-item scholar’s accurate referral, and let mundane market stalls remain commerce and local flavor rather than a second mandatory battle-preparation economy.

The shipyard/museum story is already cohesive: Dock identifies Stern’s location; Stern explains the pressure instruments; Aqua seeks the sea network; the player faces the authored escalating museum confrontations; the escape opens the next route and keeps Brawly’s badge handoff clear. Keep the staged actors, safe entry positions, party restoration and success flags. The same source flags govern the later submarine theft; those scenes must not appear early or disappear before their final handoff.

The Slateport Tent becomes FAC-01’s local six-Pokémon doubles exhibition. Keep the lobby’s personality and Prism Scale gift. Replace rental/swap instructions and the claim that players need no team. The exhibition starts and ends here, with the current cap and DIFF-01 opponent levels. Three wins earn a local prize from the existing pool, separate from Circuit rewards. This gives the player an immediate repeatable competition outlet without shipping them to the postgame Frontier.

The seashore café and coastal routes remain lively: hard authored trainers, retired encounters now serving as short comic conversations, sandcastle children, Zigzagoon’s cleanup and optional refreshments. Preserve these changes rather than reintroducing fights simply because an old rematch script still exists. At Surf access the same coast and the Abandoned Ship become useful revisits, with acquisition timing recorded by method rather than by map name alone.
''',
'03-mauville':r'''# Mauville, the meadows and New Mauville

Mauville is a crossroads for experimentation. The game already supplies free sets, legal moves, Nature and Stat Points, alongside the bike shop, Game Corner starter access, nearby evolution gifts and Daycare. The cohesion pass preserves that generous foundation. Nothing in this region is a staged introduction to Fake Out, speed control or other expert tools; the authored battles already use them seriously.

Wally’s team and challenge remain authored content to inspect in the battle volume. World changes focus on truthful presentation. His defeat text currently states that his wind never appeared, regardless of what happened. Replace it with an acknowledgment of the actual loss. Likewise, Route110’s rivals must not claim that levels never matter or that outleveling is impossible when difficulty intentionally uses level differences. They can recognize a strong opposing team without pretending to have logged a particular solution.

The Route110 and Route111 guards preserve ordered story access. Their diagrams, visible blockers and explanations must agree: Knuckle permits the route into Mauville; Dynamo permits the northern route beyond the city. They should tell players the required badge and the useful destination. The books’ difficulty rule DIFF-01 applies to every battle encountered here, including Gym trainers; no old underleveling assumption is preserved by a world chapter.

Rydel’s shop and both cycling-road entrances retain their existing bike checks, free swap and riding instructions. Technical riding is a distinct exploration activity. Keep both ride styles available through the current shop and do not impose new exclusive catches behind one irrevocable bicycle choice. Test both directions, a bicycle stored in the item PC, cancellation and leaving cycling mode on foot. The Cycling Road score is recreational, not a gate to serious Pokémon preparation.

Verdanturf’s clean air, Wanda and Wally, lovers’ tunnel story and southern meadow give the western branch an identity. Preserve the direct Audinite gift on Route117. The Daycare’s current contract is unusually clear: two compatible parents produce an egg, one qualifying hatch earns Kangaskhanite, and the Togepi gift does not qualify. Babies are already available early by other methods. Keep the short waits and free stat/move preparation; do not reinstate a breeding-stat or egg-only collection grind.

The Game Corner remains a source of prepared Pokémon with affirmative purchase confirmation and no debit/claim when its prepared set cannot be supplied. The acquisition volume owns exact stock and price decisions. The world requirement is that the displayed species, actual set, destination party/PC and claim flags agree. Preserve the coin case handoff and its no-room behavior. A player who discovers a desired starter here should understand the exchange without having to win a random jackpot.

New Mauville is a major return destination after Surf and Wattson’s key. Preserve its technological identity, switch puzzle, Voltorb decoys, Rotom generator sequence, appliance catalogue and deliberate Sign landmark. Current source already evolves Meltan by leveling in New Mauville with the generator either on or off; that is a KEEP, not an unresolved gap from earlier discussion. The generator’s after-capture dialogue must remove mandatory Devon research and former Manectric/Dialga requirements, while retaining the real Registeel association for Regieleki.

Mauville and Verdanturf Tent visits become FAC-01’s local exhibitions; the existing town maps do not become portals to the Frontier. A loss, withdrawal or full prize Bag must leave the player in the correct lobby with their real party restored. Counterexamples should be tested deliberately: an almost-empty party, an Egg, a damaged party, repeated exit, and a saved pending reward.
''',
'04-volcano-and-desert':r'''# The volcano, ash country and desert

The existing mountain chapter has a strong geographic and narrative spine. From Mauville the northern trail reaches Route112, Fiery Path, ash-covered Route113, Fallarbor and Meteor Falls. The rival and Cozmo explain the stolen meteorite and cable-car pursuit. At Mt. Chimney the competing factions and the machine make their motives visible. After the hard summit battle, Jagged Pass descends toward Flannery and Lavaridge. Preserve that authored order and its direct travel instructions.

The world should keep its contrasting habitats: mineral caves, wooded volcanic slopes, fine ash, hot springs and arid ruins. The acquisition volume distributes broad useful options within them. Do not replace these distinctions with arbitrary identical encounter palettes, and do not restrict all strong Pokémon to preserve a conventional power curve. The ash can plausibly host a surprising fighter, the furnace can host an exotic Fire threat, and the desert can be rich in fossils, provided each distribution remains purposeful and readable.

Route111’s Blob quest connects several maps rather than merely dispensing a better healing item. The current source tracks the nurse, Route112 escape, Jagged Pass handoff and Ashen Woods chase across map loads. A replacement Heal Ball remains available. The return reward already guards against reducing an existing Vial capacity, and the final Route133 nurse appears only at the appropriate two-charge stage. Keep those safeguards. Verify the chase’s state and actor positions at every interruption point; source variables alone do not establish that its sprites or paths render properly.

Fallarbor already gives current Kingambit, Ursaluna and Hisuian Sliggoo/Goodra guidance. Those are valuable signs of ongoing source work. Preserve them and verify against the acquisition chapter’s actual final definitions; do not repeat superseded claims that those paths are missing. Lanette’s fourteen-box/420-Pokémon note is accurate for this snapshot. Comprehensive availability means sequential acquisition and use; this book does not silently expand the save layout or promise a simultaneous living collection of every form.

Lavaridge retains its hot-spring atmosphere, egg gift, herb shop and free preparation. The Gym’s choreography and final team are examined independently in the battle volume. The world keeps the post-Flannery route home, Go-Goggles handoff and existing boulder access to Ember Path. Norman’s return is about the story and a different demanding encounter, not permission to begin expert play.

The desert is a substantial optional archaeology space. Mirage Tower’s disappearance must not strand Sandstrewn Ruins. Its basement connection and the Desert Underpass/Fossil Maniac route are parts of one access contract. Preserve fossil ownership, one-at-a-time Devon restoration, both fossil choices and the surviving alternate entrance. Test the ruins before and after the tower collapses, including pending fossil storage and the stair loop through the upper exterior. The Runerigus stone on Route111 is deliberately explained and remains available after the tower disappears.

Ashen Woods and Ember Path provide personal exploration alongside the main conflict. Keep the caretaker’s account of recovery, the ash/light pockets, the prepared optional battles, landmarks and visible Moltres. Scorched Slab is examined in the rainforest volume as a later branch of the same volcanic landscape. Its Magma Stone reveal is legitimate initial visibility state: the misleadingly named Heatran flag is not itself a captured ledger. Do not “repair” it by removing the intended discovery.

Fallarbor’s Tent adopts FAC-01 instead of the live old Arena singles format. The Glass Workshop and other crafting/trade side activities retain their local identities and existing earned-item retry paths. Their optional rewards may overlap free battle supplies; they should never be presented as the required way to prepare a competitive team.
''',
'05-rainforest':r'''# Rainforest, Fortree and Mt. Pyre

After Norman, the explicit Surf itinerary leads through Mauville’s eastern river onto Route118, then north through the rain and the Weather Institute. This is already a coherent transition in landscape and story. Preserve the free road nurse, beds, prepared gift Pokémon, rival encounter, scope demonstration, Fortree’s blocked Gym path and the Feather Badge’s eastward handoff. Difficulty remains expert throughout; the region adds different battle contexts and species rather than unlocking serious strategy.

The Weather Institute connects local weather to the factions’ ancient network. Keep that motive and its tangible Castform/Reveal Glass reward. Its current first and repeat explanations still send the player to Devon before every Sign. W-SIGN-OPTIONAL removes that false obligation. The roadside Castform silhouette is also obsolete for Tornadus; the revised sign points to the real badge condition and local landmark. The gift can retain its research-personality value without acting as a hidden key to unrelated encounters.

Fortree’s treetop geometry, rotating Gym doors and invisible Kecleon obstacle are iconic. Preserve them. Steven’s bridge encounter and explicit return-to-Winona directions prevent wandering through an unclear gate. A rejected battle prompt, a failed Scope handoff or a captured/defeated Kecleon must leave a coherent return path. The Scope interaction does not require occupying a battle moveslot and should remain a useful exploration instrument elsewhere.

The wooded, rainy and highland routes should reward broad team building. Familiar residents and modern additions can coexist; weather strategies and unusual support Pokémon are available here without being exclusive to this chapter. Local gifts, regional evolution locations and Sign marks need to agree with their actual source definitions. Remove Gardevoir/Xatu/Tauros silhouettes when they falsely imply requirements that the current acquisition code no longer checks.

Mt. Pyre has two identities that should both remain: a resting place for beloved Pokémon, and the setting for a major faction crime. Preserve the quiet memorial NPCs, the ghost-tower habitat, Duskull’s association, holes/stairs and the climb to the summit. The restored Hisuian Typhlosion/Braviary clue belongs alongside this identity. It is useful information without turning grief scenes into competitive lectures.

The summit’s old couple, Aqua’s Orb theft, Magma Emblem handoff and later Orb return are a single stateful storyline. Keep their actors and movement. The exact immediate destination—Jagged Pass’s strange boulder—should remain explicit. The new Sign stories must not instruct players to gather removed ordinary-family prerequisites. Current Darkrai/Cresselia, Pecharunt and Regi-related conditions remain deliberate and are specified by the acquisition owner; do not flatten every legendary into a universal badge-only pickup.

Scorched Slab’s deeper floors preserve a distinct optional cave route with boulder passages, a visible Heatran and a later legendary landmark. Most old warden scripts are unbound and remain inert. The cave need not contain an explanatory NPC on every floor. Its navigability, escape route and interaction conditions matter more than adding prose for its own sake.

Route123’s Berry Master remains an excellent bridge between habitats and Mega rewards. Keep the three one-time mixed-berry exchanges, visible price/count, list of eligible berries and commit-on-success transaction. The player’s ordinary battle berries and free held-item access remain separate. Do not turn a modest regional exchange into a compulsory farming curriculum.
''',
'06-sea-and-crisis':r'''# The eastern sea and Hoenn’s crisis

The eastern story should retain the current chain of cause and consequence: Mt. Pyre’s theft sends the player back to Magma’s Jagged Pass hideout; Groudon’s awakening sends them to warn Stern in Slateport; Aqua steals the submarine; Lilycove’s hideout opens the sea; Mossdeep’s Gym and Space Center lead to Dive; the submarine’s resting place reveals the cavern route; the seafloor confrontation triggers the crisis in Sootopolis. The source handoffs already explain much of this well. Preserve the flags, actors and specific travel directions together.

Magma and Aqua can field powerful, intricate teams at every appearance. Their story remains about conflicting aims and overconfidence, not about gradually learning doubles. World scripts must treat defeat and Retry correctly: a lost battle must not remove an admin, advance the hideout, grant a reward or launch the next irreversible cutscene. The Space Center’s three consecutive formations and partnered Steven battle deserve dedicated loss/retry/party-backup checks. The battle volumes own exact team quality; the world volume owns returning the player to a coherent place and state.

Lilycove remains a city worth visiting beyond its battle and blockade. Retain its art museum, contest culture, sea-facing homes, harbor passes, department-store convenience and people with their own lives. The rival’s parting dialogue should not assume a particular first-battle result or claim the lead was solved in one turn. A victory can acknowledge the team without narrating moves the game did not record.

The sea needs differentiated destinations. The acquisition chapter preserves New Mauville’s technology, Mt. Pyre’s ghosts, ancient Relicanth associations and the Dondozo/Tatsugiri submarine pairing. World geometry and clues should make those destinations discoverable: the stolen submarine’s hull confirms the route; its nearby landmark is deliberate; Mossdeep’s diver talks about the actual Mind Badge condition rather than an obsolete Relicanth prerequisite. Keep the broad route/fishing/Dive options without presenting inactive methods as available.

Shoal Cave’s tides are a real layout change, not two ordinary additional maps. The LowTide headers select high-tide layouts; the separately registered HighTide headers have no normal entrance. Preserve the active layout assets and test both tide phases through the real Route125 entrance. The Ice-room access, salt/shell collection, current regional-evolution explanation, visible Articuno and other Sign encounters require the correct phase and real route. Do not infer availability from a registered map name alone.

The Abandoned Ship is a rich optional revisit with keys, hidden rooms, Scanner and a distinct underwater connection. Preserve its layered exploration and item-on-success transactions. Its Spiritomb bin should offer a clear eerie clue and require the Odd Keystone alone; the hidden Lickitung/Slugma requirement has no useful world explanation. A failed or declined capture keeps the Keystone. This alternate encounter does not replace ordinary Spiritomb availability.

Sootopolis’s crisis retains its skyline, crater, residents and choreography. The Origin cave’s former Ruby/Sapphire floors are now live connections toward Diancie, despite their names. The quiet B1F also has its own deliberate later Terapagos discovery. Preserve the story route from Wallace to Sky Pillar, the first ascent, Rayquaza’s departure, the return scene and Waterfall/Juan handoff. The reopened cave remains accessible after the crisis instead of becoming a dead story prop.

W-WALLACE-ROOT restores the already authored optional rain exhibition in Diancie’s chamber. Wallace stands off the main aisle, offers a choice, waits until after the League, and uses the reviewed E0510 team. Diancie and Diancite remain independent. No new prize is needed merely to justify a great optional battle; avoid duplicating the stone’s pickup entitlement.

The smaller sea stories remain valuable: Pacifidlog’s stilt village, fast currents, Regi rumors, the final Vial rescue, Wingull’s mail and Briney’s later captaincy. Their optional nature supports exploration. The Regi chambers, Sealed Chamber and dynamic marine/terra caves must retain their actual state prerequisites and return behavior. Finite legendary captures remain permanent after success; failure returns according to the approved re-entry system. None of that is certified by static warp resolution alone.
''',
'07-league':r'''# Victory Road and the League

The League is the culmination of a game that has been hard from the start. It earns its higher position through the quality of its Pokémon, full parties, level calibration and exceptionally coherent play. The battle volume reviews the existing teams individually, restores signature Pokémon such as Glacia’s Walrein with a substantive role, and specifies any demonstrated AI or loadout repair. This world volume preserves the map and story framework around those teams.

Victory Road retains its layered cave traversal, field obstacles, optional discoveries, experienced trainers and Wally. A late catch should be usable without a new grind. Free preparation and the Leveler make the region a place to assemble or adjust a final team; the acquisition owner determines its roster without requiring players to replace favorite partners.

Ever Grande’s approach and the League gate preserve Waterfall and all-eight-badge requirements. The lower Center and League Center both have free services. Inspect each map independently: a correct nurse script is not proof that its object, counter access or respawn point is usable. The source currently provides the expected vendors/tutors and registered warps; an implemented traversal must confirm each real route.

Once inside, the halls and room state advance through Sidney, Phoebe, Glacia, Drake and Wallace. Keep the current order, certified opponent flags, barrier changes, party state and separate Retry/Reload behavior. Ordinary recovery between rooms and the prohibition on Bag items during trainer battles are distinct rules. Test defeat and Retry in each room rather than relying on a traversal harness that automatically resolves combat.

The final speeches should celebrate the player’s achievement without insisting that they constantly rebuilt all six Pokémon or won through one prescribed strategy. The world contains room for both experimentation and attachment. Where the battle reviewer changes a signature or decisive interaction, its related introductory/defeat text must match the final roster. The same principle governs every earlier Gym.

The Hall of Fame is a save-state milestone. Preserve the player’s final party, Champion registration, credits/return flow and postgame unlocks together. Success in this chapter requires an actual built-game chain through the final battle and save/reload, with artifacts bound to the implemented source. The book and static references cannot establish it in advance.
''',
'08-frontier':r'''# Frontier competition and exceptional revisits

The existing Frontier already routes its desks into the CHAMPIONS CIRCUIT. The world around those desks has not fully caught up: rule boards still advertise singles and three-Pokémon teams; Factory visitors promise rentals; Palace explanations describe autonomous move choice; Pike/Pyramid signs promise maze challenges; Reception says the other facilities preserve classic formats. This is a concrete cohesion failure. Repair the live presentation to describe the actual current competition.

Keep the Frontier’s recognizable buildings, outdoor characters and history. Every current desk registers the player for the central Tower arena. All native trainer matches are doubles with full prepared teams. The central competition chapter owns opponent generation, difficulty, party snapshots, restoration, exit and reward counters. W-CIRCUIT-RULES supplies one truthful rules contract; W-FRONTIER-GUIDES and W-FRONTIER-RESIDENTS replace the specific incompatible claims. Old mode scripts and records do not become active merely because they remain in source.

Current record boards must show actual Circuit current, best recorded and lifetime counters through a read-only view of the same saved state. FAC-03 adds a best-recorded counter: migration uses only an observed current run, never lifetime totals as a guessed historical best; an unknown prior best is shown as --. Tent exhibitions and Hill Time Attack do not advance those counters. Retain old imported/historical record data without mislabeling it as current results. A record read may not reset a streak, award a prize or change the active battle mode.

The Frontier should reward serious play without charging unnecessary preparation tolls. The two veteran move tutors keep their personality and now use the existing free specialist; the evolution counter uses the free Ring archive. Decoration and field-supply BP exchanges remain distinct services. The gambler’s old challenges depend on retired modes: close new betting, honor a recorded win, refund an unattempted stake, and preserve any payment that cannot fit under the BP cap. Do not invent a second competition economy to rescue a redundant system.

Scott’s welcome, recognition of the player and initial BP gift remain. His comments should describe current Circuit opportunities and architectural history. Already earned classic symbols and their unclaimed gifts remain valid historical entitlements; the book does not silently mint new symbols from unrelated Circuit counters. Any explicit new cosmetic milestone must be specified in the central reward chapter before it is advertised.

The Frontier’s wild corner, Sudowoodo and Artisan Cave remain optional local discoveries. Habitat recommendations belong to the acquisition volume: Smeargle’s identity can become more distinctive without deleting useful alternatives irresponsibly. Pure combat ambition does not require removing the outdoor world or the quieter delight of finding a Pokémon under a boulder.

Champion ferry passes open Southern Island, Navel Rock, Birth Island and Faraway Island. Each island retains a distinct interaction and visual payoff: the eon shrine, long ascent/descent, Deoxys’s moving triangle, and Mew’s hide-and-seek. Preserve the actual capture and retry conditions, exit sailor and permanent ownership. The tickets, party/PC-full state and revisit routes are separate things to test. A destination remaining in a travel menu is not proof that its encounter is available or its return boat works.

Altering Cave’s expanded route and Leaf/Mewtwo encounter likewise retain deliberate late exploration. Leaf’s already authored gift retry returns actors to their approach coordinates when the Bag is full; preserve that protection. Most old descriptive researchers in its source are unbound. Restore Wallace deliberately under its own proposal, but do not generalize that choice into blanket resurrection of every dormant NPC or fight.
''',
'09-side-activities':r'''# Side activities and shared spaces

Side activities should make Hoenn feel inhabited and give players more reasons to use its Pokémon. They do not all need to become combat systems. Preserve contests, art, Berry Blender, decorations, mail and recreational records as optional activities. Their rewards must not become a required toll for ordinary competitive preparation.

The three Battle Tents receive a coherent current role under FAC-01: six-Pokémon local doubles exhibitions using the shared competitive generator, current cap and DIFF-01. They start and finish in the originating lobby. The player commands both partners. Three victories award one prize from the existing local reward pool; party and held items are restored and Circuit entitlements remain untouched. Keep pending old tent prizes safe before entering the new mode. Their old corridor/battle-room pipelines are retained as inert support, not a second live ruleset.

Trainer Hill receives FAC-02 rather than removal. Preserve its Time Attack geography, floor navigation, timer, roof, prize storage and return route. Each floor uses one coherent six-Pokémon generated opposing side. If the existing two-NPC presentation remains, split that one team with coordinated leads rather than generating two unrelated halves. The timer must pause and resume consistently with the actual battle flow. Mark a floor complete only after a legitimate win; loss, draw, forfeit and withdrawal must clean up correctly without manufacturing victory. Existing earned prize state remains safe.

The Trick House retains all eight spatial puzzles, evolving entrance hiding places, personality and revisit gates. W-TRICK-QUIZ supplies all fifteen replacement questions because the old bank depends on obsolete species, prices and NPC counts. Each question tests an actual configured doubles rule with explicit choices and a correct index. This is a single optional quiz room, not a campaign curriculum. The other rooms keep their Cut, switches, rocks, rotating gates and slippery floors. W-TRICK-REWARDS fixes the real full-Bag mismatches and separates the final tent and Alakazite entitlements.

Safari remains a recognizably different collection destination. Preserve the entrance’s storage check, fee, Pokéblock-case condition, Catching Charm gift, encounter system, rest house and expansion paths unless the acquisition chapter explicitly changes a requirement. Its native capture battles remain singles. The player needs accurate knowledge of which methods and regions contain desired Pokémon, not a false dormant Hidden roster. Test exiting by choice, exhaustion and resource depletion with captured Pokémon safely stored.

Secret Bases are player-owned decoration and record-mixing spaces, not twenty-four additional handcrafted regions. The ledger accounts for every template object, entrance and callback under the shared contract. Preserve ownership, layout IDs, decorations and imported records. LINK-01 retires the native recorded-owner battle interaction before any save prompt, daily battled-owner write or battle preparation. Preserve the owner, imported profile and decorations; direct visitors to the Tents or Circuit for a native battle. Do not fabricate modern teams from imported records.

Contest Hall layouts similarly stage one shared activity. Keep NPC appeal flavor, paintings and ribbons with the underlying contest semantics. The canonical battle move editor must not charge for moves simply because they can also be used in a contest. The hall’s current path from reception through presentation to museum painting is one complete interaction chain, with party identity and exit state preserved.

The acceptance matrix for these activities follows distinct failure modes: cancel/invalid participant; full Bag or storage before a reward; loss and voluntary exit; reload during a supported session; and repeat interactions after success. Repeated unchanged test suites and historical event-count floors do not prove this behavior. Use direct observable checks for the new interfaces and maintain genuine save/party integrity protections.
''',
'10-support':r'''# Registered support, external connections and inert content

An exhaustive book must account for registered content without pretending every registered map is a normal playable destination. This volume distinguishes live geography, dynamic layout assets, player-owned templates, external connection rooms, recovery/staging maps and historical unused headers. The distinction is based on current references and behavior, not merely on names.

The three Cave of Origin floors whose names contain UnusedRubySapphire are live links in the expanded cave. Keep them in the adventure ledger. The old Ruby Aqua Hideout maps, prototype Route104/shop, unused contest headers and Lilycove unused Mart have no normal native role; preserve IDs and avoid reattaching them just to increase the map count. Shoal’s two high-tide headers are inert as destinations while their layout assets are used by the active low-tide map headers. That reuse is a specific verified source contract.

Pokémon Center second floors, Union Room, trade rooms, record corners and multiplayer minigame support retain their data and social functions. All trainer competition offered by the final native UI follows the doubles policy. LINK-01’s exact external-entry rules must explicitly reject or retire an unsupported mode; external records are not permission to silently introduce singles or a too-small native trainer team. Do not delete user-owned imported data to make a format check pass.

The Sootopolis e-Reader visitor currently offers a three-on-three legacy battle. W-EREADER-NATIVE-RETIRE preserves the house and harmless conversation while removing that native challenge path. The analogous Mossdeep locked-room support remains harmless. Recovery from an already saved legacy session must restore the actual backed-up party and exit safely; invalid backup data is an integrity problem to report, never an invitation to overwrite the player’s team.

Legacy Frontier interior rooms and Pyramid templates remain in the inventory because their map IDs and controller data still exist. The current desks enter central Circuit; FAC-01 and FAC-02 define the separate live Tent/Hill behavior. The book does not call the old Palace, Arena, Factory or Pyramid content a completed current attraction. An implementation-time negative traversal check must establish that players cannot bypass the new native entry contract through an old door, callback or stale continuation state.

The unrooted-entrypoint appendix is also deliberately conservative. It records candidate top-level script roots with no independent native reference found. Many are acknowledged old rematches, old facility attendants, variant movement leftovers or unbound expansion exposition. Keep them inert unless an explicit proposal calls for deletion or restoration. Wallace’s exhibition is the one deliberate restoration here, tied to its still-authored team and a concrete off-aisle placement. Broad regex reachability is not sufficient authority to delete compiled script code.

The final proof boundary remains explicit: the source gate checked all540 Hoenn registered maps, all4,196 physical NPC/trigger/sign events,1,402 warps,18,350 script references and368 value-returning special contracts. This established structural consistency of the snapshot. It did not play every story state, render every scene, execute every menu or win every battle. Those checks are specified beside the proposed changes and in the implementation sequence; they are still required after the book is implemented.
'''
}
for key,body in texts.items():
 body=body.strip()+'\n\n## Complete regional map register\n\nEvery registered map assigned to this neighborhood has a separate complete event page. Later revisits and shared-system dependencies are recorded there; chapter placement does not impose a power or availability tier.\n\n'+ '\n'.join(f'- [{n}](maps/{n}.md)' for n in members.get(key,[]))+'\n\n[World index](README.md) · [Exact proposed changes](changes.md) · [Shared contracts](common-contracts.md)\n'
 (W/(key+'.md')).write_text(body)
(W/'README.md').write_text('''# World cohesion volume

This volume specifies the final world around the existing authored battles. It preserves the current game where it is coherent, names exact repairs where it is not, and leaves the live game and frozen baseline untouched.

The final charter is expert doubles from the beginning, broad convenient preparation, no native trainer singles, standard opposing parties of at least four, and DIFF-01's Medium opponent floor at the live player cap minus two. Ordinary wild and legendary capture encounters remain singles. The specifically requested Birch rescue is a scripted doubles exception with two chosen starters, owned by INTRO-01.

## Read the experience

1. [Opening and western woodland](01-opening.md)
2. [Dewford and Slateport](02-dewford-and-slateport.md)
3. [Mauville and the meadows](03-mauville.md)
4. [Volcano, ash and desert](04-volcano-and-desert.md)
5. [Rainforest and Mt. Pyre](05-rainforest.md)
6. [Eastern sea and story crisis](06-sea-and-crisis.md)
7. [Victory Road and League](07-league.md)
8. [Frontier and exceptional revisits](08-frontier.md)
9. [Side activities](09-side-activities.md)
10. [Support and inert content](10-support.md)

## Review and implement

- [Exact world changes](changes.md): complete dialogue replacements, interaction behavior, placement, prerequisites and acceptance.
- [Every registered map](map-index.md): 540 individual map pages, each accounting for all of its physical events and callbacks.
- [Shared contracts](common-contracts.md): reusable behavior contracts with the exact shared-root instance inventory.
- [Machine event ledger](event-ledger.json): all6,322 map event/callback records, keyed by map and event ID.
- [Dialogue ledger](dialogue-ledger.json): current resolved text labels and their event/proposal references.
- [Unrooted entrypoints](unrooted-entrypoints.json):132 candidate inactive roots, explicitly distinguished from currently accessible NPCs.
- [Proposed edits manifest](proposed-edits.json), [coverage evidence](coverage.json), and [opening geometry](opening-geometry.json).

## What the evidence means

The source snapshot contains540 registered Hoenn maps,3,052 objects,421 coordinate events,723 background events,1,402 warps,152 connections and572 map callbacks. The map scripts total86,041 lines; with shared assembled campaign scripts the structure gate examined106,569 lines. It passed map/label/reference geometry and value-returning-special contracts.

The review used per-map dialogue and state digests, complete event records, common engine contracts, and focused source traces for every proposed repair. Shared trees, berry plots, nurses and support rooms receive the same examined contract rather than invented per-instance mechanics. Map-specific coordinates, flags, neighbors and final dependencies remain explicit.

This is not an instruction-by-instruction runtime execution of every script. No ROM was built, no gameplay state was changed, and no fresh-save traversal or battle win is claimed. Full implementations still need the stated field, menu, transaction, scene and combat evidence. Catalogue coverage and an exact specification are useful preparation for that work; they are not a substitute for it.

Some prior suspected defects were rejected after checking current consumers: the Vial upgrade already prevents downgrades; Heatran's misleadingly named flag controls physical presence; Meltan, Ursaluna, Kingambit, Runerigus and several regional forms already received source fixes. The book preserves those corrections.
''')
print('Regional chapters',len(texts))
