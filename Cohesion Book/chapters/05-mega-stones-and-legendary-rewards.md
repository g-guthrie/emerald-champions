# 05 — Mega Stones and legendary rewards

The current world reward architecture is worth preserving. The book keeps every existing Mega Stone position, the meaningful NPC gifts, the three Berry Master exchanges and the Ring’s acquisition. The repairs concern exact receipt behavior, accurate information and actual form availability—not a new policy rationing strong Pokémon by chapter.

## The complete native roster

The frozen source contains **99 Mega Stone items with 103 item-triggered native Mega bindings**. The extra bindings come from male/female Meowstic, ordinary/Original Magearna and the three functional Tatsugiri forms. Rayquaza supplies the additional stone-free Mega binding, bringing the compiled `.isMegaEvolution` total to 104. This is a snapshot inventory, not a permanent game gate or a claim about an external competitive service’s current roster.

[The complete per-stone catalogue](../appendices/mega-stone-catalog.md) individually specifies all 99 items, native base/Mega forms, source locations, flags, NPC roots, world context, evolution/acquisition dependencies, final disposition and acceptance. [Its JSON companion](../review/mega-rewards.json) keeps all raw source routes and complete ancestry/root data.

The current routes reconcile to 73 physical sparkles,41 literal scripted grant sites and 3 berry exchanges. First/retry grant sites are not separate prizes: after grouping their real receipts, these are109 distinct finite world entitlements. Ten stones have two deliberate world sources; the other89 have one. The conditional original-starter gifts are additional pathways, fulfilled by the same inventory/held ownership and receipt rules described below.

No physical Mega receipt aliases another physical or hidden-item reward numerically in this snapshot, and no different Mega Stones share a numeric receipt accidentally. All 73 sparkles have a matching fixed-layout elevation and at least one passable adjacent tile. Those useful structural results **do not establish each stone’s complete earliest approach**: routes contain ledges, currents, doors, boulders, Cut branches and dynamic layouts that must be followed in the actual story state.

## Ring and stone eligibility

Keep Steven in Granite Cave as the Ring owner. Deliver Mr.Stone’s letter and earn Brawly’s Knuckle Badge; successful Ring delivery then opens the existing evolution-item archive. The Ring enables Mega use but never dispenses the entire Mega Stone collection. Mr.Stone’s old free-Mega-archive claim is corrected under W-MEGA-GUIDE.

Production `CanMegaEvolve` checks **Mega Ring possession in the Bag**, the exact native form table, held stone, existing gimmick usage and other battle eligibility conditions. A related Pokémon family is insufficient. Importantly, the Ring check is explicitly bypassed under `TESTING`; a test-build Mega battle cannot prove that production gate. Verify it through an appropriate production path when changing it or when no valid evidence exists.

Rayquaza uses its native Dragon Ascent Mega condition rather than a stone, while still following production Ring/battle eligibility. Primal Orbs, fusion tools, masks, memories and similar form items belong to the form/relic systems. Do not count them as missing Mega Stones or create a second archive for them.

## Two original starters, one existing receipt ledger

Apply **INTRO-STONES** exactly as specified in [the opening chapter](01-opening-and-starters.md) and `review/opening.json`. The immutable first starter uses receipt slots0/1; the actually delivered second starter uses slots2/3 in the existing16-bit `VAR_STEVEN_STARTER_STONE_DELIVERY` at 0x40E0. Skip `ITEM_NONE` slots. Four slots are reserved; with the present same-region pair rule and stone table, at most three are populated at once (Charizard X/Y plus the other chosen Kanto starter).

The current table recognizes 12 starter families and 13 stones: Bulbasaur; Charmander X/Y; Squirtle; Chikorita; Totodile; Treecko; Torchic; Mudkip; Tepig; Chespin; Fennekin; Froakie. Do not invent Mega Stones for starter families without a native binding. All original choices remain supported and powerful team options regardless of whether their family has a Mega.

Before granting a slot, retain the existing check for an owned stone in Bag, item PC, party held items or PC Pokémon held items. An already owned stone fulfills that slot. Otherwise deliver to Bag, then item PC; persist only a successful receipt. Pending slots survive re-entry. Completed bits survive later disposal, so revisiting Steven does not regenerate a discarded gift.

Legacy saves require one additional visibility reconciliation already owned by INTRO-STONES: **an encoded but still-owed second Pokémon creates no second-family stone entitlement**. Only after actual second-Pokémon delivery, if letter/badge/Ring handoff is complete and a populated second slot is still pending, clear Steven’s Granite Cave hide flag. He resumes the pending stone handoff without repeating the letter, Ring or first-family gifts. No pending slot may be lost by making him depart; a no-Mega, already fulfilled or not-yet-delivered second family does not wake him.

## Preserve deliberate finite alternatives

The two-world-source stones are Absolite, Alakazite, Altarianite, Cameruptite, Gengarite, Gyaradosite, Lopunnite, Lucarionite, Metagrossite and Staraptite. These are explicit authored alternatives, with at most one award from each distinct receipt. Keep that finite generosity. Do not create a99-item global ownership ledger simply to prevent a player collecting a second legitimately authored copy.

Distinguish alternatives from retries. Winona’s first/repeat gift functions and Lilycove’s Altarianite keeper share one `FLAG_RECEIVED_WINONA_ALTARIANITE` entitlement; the separate Route111 pickup has its own flag. The other leader first/retry functions likewise share their leader receipt. Trick House entrance retrieval is the same stage prize as the rear-room grant.

**W-TRICK-REWARDS** fixes the real exceptions. Puzzle 4’s retry must give King’s Rock, matching the immediate reward. The final chosen tent and Alakazite need independent successful receipts, with a one-time initialization marker for conservative old-state handling; claim only the still-owed part at the entrance. Keep Granite Cave’s Alakazite pickup as its deliberate alternate. The three receipt/initialization flags are reserved at 0x2B2–0x2B4 in the [central state allocation](../appendices/state-allocation.md); initialize them only in the agreed version 4 migration. Do not force players to replay puzzle 8 or grant extra tents to compensate for a failed stone insertion.

Diancie’s chamber keeps its physical Diancite. The restored authored Wallace exhibition is optional and independent under W-WALLACE-ROOT; its old unbound duplicate item handoff is not reactivated. Gardevoirite remains an unconditional one-time world gift from Wally’s aunt; the new optional Indeedee-F gift must not become its prerequisite or consume its receipt.

## Preserve berry exchanges as local exploration rewards

The Berry Master exchanges exactly 20 total garden berries for one Baxcalibrite, Dragoninite or Tyranitarite, once per stone. Eligible currency is Razz, Bluk, Nanab, Wepear, Pinap, Pomeg, Kelpsy, Qualot, Hondew, Grepa and Tamato, in any mixture. Keep the visible count, price, eligibility list and affirmative confirmation.

The existing source validates the selected stone, its one-time flag, enough berries and successful stone insertion before removing currency. Preserve that ordering. Invalid selection, refusal, already claimed, insufficient berries and full Bag do not debit or claim the reward. Keep the real garden sources and planting; free vendors and competitive presets must not produce this currency as a back door around the exchange.

This is a modest regional reward. It must not become a farming requirement for ordinary battle preparation, and the book adds neither a free Mega archive nor a new economic subsystem.

## Exact forms deserve exact information

Six item descriptions are repaired under **MEGA-FORM-LABELS**, with exact three-line replacements in their catalogue entries: Floettite names Eternal Floette; Zygardite names Complete Zygarde; Raichunite X/Y name Kantonian Raichu and the corresponding Mega; Slowbronite and Greninjite name their ordinary base forms. Preserve their item IDs, prices, icons and bindings.

Follow [chapter 04’s form authority](04-acquisition-evolutions-forms.md):

- **FORM-12:** Eternal Floette is already a real7% Verdanturf Meadow encounter; ordinary flower-color Floette cannot use Floettite. Zygardite needs Complete Forme.
- **FORM-08/12:** obtain Zygarde, use the Cube for Power Construct, and reach≤50% HP at turn end to become Complete. Validate the actual subsequent Mega opportunity rather than pretending the Cube grants persistent Complete/Mega directly.
- **FORM-04:** Droopy and Stretchy Tatsugiri join Curly at the submarine habitat. Their Order Up behavior differs; these are functional forms, not merely cosmetic recolors. Tatsugirinite supports all three.
- **FORM-05:** the verified Original Magearna color can be selected only after actual Magearna ownership. Magearnite supports both native colors. The prototype’s Diancie prerequisite is separate from the eight-badge stone gift.
- Meowsticite supports both sexes through distinct form tables. Raichunite does not activate Alolan Raichu; Slowbronite does not activate Galarian Slowbro; Greninjite’s base is ordinary Greninja.

Stone discovery can precede evolution or a later legendary. That anticipation is desirable when the player understands what is needed. It is not a reason to restrict the broad early roster, weaken the opening battles or assign each Pokémon a protected chapter.

## Legendary acquisition and relic rewards

The baseline has 82 Sign entries:51 deliberate landmarks,7 visible field entries,7 ordinary-wild entries and 17 other-provider entries. Those17 comprise 12 Circuit rewards,2 mastery rewards,2 Game Corner rewards and 1 breeding route. An `OTHER_SIGN`’s zero badge field is not evidence of opening availability; its real provider owns the gate.

Devon remains an optional guide, including after a discovery is completed. The actual field/badge/story/species conditions remain authoritative. Failed encounters can be retried after leaving and returning; captures and earned reward entitlements remain permanent. W-SIGN-OPTIONAL and W-SIGN-LOCAL remove stale mandatory research and retired partner silhouettes from live world text. Do not restore the old compulsory researcher visit or permanent failure rule.

FORM-06 appends the three distinct Galarian bird Signs at IDs82–84, leaving the original 82 identities and existing native one-off indexes stable. Exact-form acquisition is resolved before any permitted family fallback. Its detailed locations, conditions and capture identity are owned by chapter 04; this reward chapter does not duplicate that definition.

The 16 native one-off species below remain separate from the Sign array. Their reserved encounter indexes are96–111 in source order. The apparent `CAUGHT`/`DEFEATED` names of physical hide flags do not independently prove capture—Heatran’s reveal is a useful example. Preserve actual caught ownership and each event’s current retry behavior.


| Native species | Current principal scene | Actual access context |
|---|---|---|
| Groudon | [TerraCave_End](../world/maps/TerraCave_End.md) | Abnormal-weather postgame cave; actual event location and arrival state |
| Kyogre | [MarineCave_End](../world/maps/MarineCave_End.md) | Abnormal-weather postgame cave reached through its current Dive approach |
| Rayquaza | [SkyPillar_Top](../world/maps/SkyPillar_Top.md) | After the story awakening and Sootopolis resolution; actual return/bike-floor approach |
| Regirock | [DesertRuins](../world/maps/DesertRuins.md) | Sealed Chamber opened plus this ruin’s local puzzle |
| Regice | [IslandCave](../world/maps/IslandCave.md) | Sealed Chamber opened plus this cave’s local puzzle |
| Registeel | [AncientTomb](../world/maps/AncientTomb.md) | Sealed Chamber opened plus this tomb’s local puzzle |
| Latias | [SouthernIsland_Interior](../world/maps/SouthernIsland_Interior.md) | Selected native roaming/shrine identity and the complementary island route; verify actual choice state |
| Latios | [SouthernIsland_Interior](../world/maps/SouthernIsland_Interior.md) | Selected native roaming/shrine identity and the complementary island route; verify actual choice state |
| Lugia | [NavelRock_Bottom](../world/maps/NavelRock_Bottom.md) | Champion-issued Mystic Ticket route and the long descent |
| Ho Oh | [NavelRock_Top](../world/maps/NavelRock_Top.md) | Champion-issued Mystic Ticket route and the upper ascent |
| Mew | [FarawayIsland_Interior](../world/maps/FarawayIsland_Interior.md) | Champion-issued Old Sea Map route and hide-and-seek |
| Deoxys | [BirthIsland_Exterior](../world/maps/BirthIsland_Exterior.md) | Champion-issued Aurora Ticket route and triangle puzzle |
| Jirachi | [MeteorFalls_JirachisRoom](../world/maps/MeteorFalls_JirachisRoom.md) | Actual deep Meteor Falls stair/Waterfall route to the chamber |
| Diancie | [CaveOfOrigin_DianciesRoom](../world/maps/CaveOfOrigin_DianciesRoom.md) | Eight-badge ladder and expanded Origin cave route |
| Heatran | [ScorchedSlab_HeatransRoom](../world/maps/ScorchedSlab_HeatransRoom.md) | Actual deep cave path and Magma Stone initial reveal |
| Moltres | [EmberPath](../world/maps/EmberPath.md) | Heat Badge/Strength branch through Ember Path |

Six capture reward groups grant 24 relic items: Groudon→Red Orb; Kyogre→Blue Orb; Zacian→Rusted Sword; Zamazenta→Rusted Shield; Ogerpon→three masks; Arceus→seventeen type plates. Keep the existing earned-group and pending-item bits. An actual acquisition whose insertion fails creates debt; later nurse/service retries settle it. Migration must not fabricate a pending reward merely from old Pokédex ownership or recreate a relic discarded after a fulfilled grant. Fusion tools and Zygarde Cube follow FORM-08 and the actual Birch research reward path.

## Implementation and verification

First reconcile the exact source and final acquisition/form definitions. Preserve all current stone placements while applying the named receipt, clue and description repairs. Extend the original-starter helper through INTRO-STONES’s single receipt variable; do not implement another parallel gift ledger. Apply the independent Trick House receipts together with entrance recovery and central flag allocation.

Then verify behavior in proportion to the changes. Useful distinct cases are: ordinary pickup with persistent ownership; a leader first/retry gift; the shared Winona/keeper entitlement; conditional two-starter delivery with partial storage; final tent/stone split; each materially different berry transaction outcome; the special exact-form bindings; and a production Ring check. Inspect unchanged data bindings once and reuse valid existing evidence rather than creating99 identical tests.

Field access checks must start from a legitimate story entry and reach the exact tile or NPC, including any real Cut, Strength, Surf, Dive, Waterfall, bike, tide, ledge or scripted gate. New Mauville requires the actual Surf/Key route; Dive access requires Mind Badge/Steven’s handoff; Southern Island uses the Champion ferry pass; desert approaches require their actual Goggles/underpass route. A fallback cap attached to a whole map is not acceptable proof. Record any unresolved earliest-access question explicitly instead of inventing a value.

Audit every existing gate’s contract, fixture, configuration and artifact freshness before interpreting a failure. Update or delete obsolete prose/count expectations that constrain deliberate design. Keep meaningful item/receipt, save and battle failure protections. This chapter’s99 count and quoted text are review scope and implementation instructions, not new production invariants. No ROM was built or gameplay changed in producing this book; traversal and battle acceptance remains implementation-time evidence.
