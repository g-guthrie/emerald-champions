from pathlib import Path
import json,re,collections
B=Path(__file__).resolve().parents[2];S=B/'baseline/source';d=json.loads((B/'review/mega-rewards.json').read_text());rows=d['stones'];signs=json.loads((B/'inventory/legendary-signs.json').read_text());sp=json.loads((B/'inventory/species.json').read_text());itemtext=(S/'src/data/items.h').read_text()
def mon(n):return n.removeprefix('SPECIES_').replace('_',' ').title().replace('Mega','Mega')
def iname(it):
 m=re.search(r'\['+re.escape(it)+r'\]\s*=\s*\{(.*?)\n    \}',itemtext,re.S)
 if m:
  x=re.search(r'\.name\s*=\s*ITEM_NAME\("([^"]+)"\)',m[1])
  if x:return x[1]
 return it.removeprefix('ITEM_').replace('_',' ').title()
def rootdesc(a):
 k=a['kind']
 if k=='wild':return f"{a.get('map')} / {a.get('method')} / {a.get('percent')}% / authored levels {a.get('level')}"
 if k=='legendary':
  z=a['definition'];return a['id']+f"; {z.get('source',z.get('map'))}; badge={z.get('badges','provider')} flag={z.get('required_flag','provider')} species={z.get('required_species','provider')}"
 return k+': '+a.get('map',a.get('source',a.get('condition','see acquisition catalogue')))
def evtext(e):
 m=e['method'];par=e['parameter'];cond='; '.join(e['conditions'])
 if m=='EVO_LEVEL':req='level '+par+' or later' if par!='0' else 'level-up when conditions are met (PREP-02 also handles ready evolution at cap)'
 elif m=='EVO_ITEM':req='use '+par
 elif m=='EVO_TRADE':req='legacy trade method (a listed native item route can replace multiplayer trading)'
 else:req=m+' '+par
 return f"{mon(e['from'])} → {mon(e['target'])}: {req}"+(f'; {cond}' if cond else '')
L=['# Complete native Mega Stone catalogue','Every one of the 99 current native stones was reviewed individually against its actual form table, source reward routes, receipts and world context. These counts bind this book to the snapshot; they are not a production test quota or a reason to block a future deliberate roster change.','**Global activation:** carry the Mega Ring in the Bag, use the exact listed base form and stone, and satisfy the current battle eligibility rules. The production Ring check is bypassed in TESTING builds, so such builds cannot prove Ring gating.','**Timing:** this catalogue records source story approaches and exact reward positions. It does not derive earliest access from trainer caps, broad map prefixes or the old timing script. Pickup-specific reachability remains explicit implementation evidence. A stone can be worth finding before its base is evolved. Direct captures can bypass an evolution chain; the level listed in an optional evolution is not itself a Mega restriction.','**Tests:** acceptance statements describe observable outcomes to validate where behavior changes or evidence is missing. They do not request 99 mirrored unit tests, frozen prose checks, or repeated unchanged suites. Audit existing checks/fixtures/artifacts first; preserve independent meaningful regressions.','## Index','| Stone | Exact native base(s) | Decision | World entitlements |','|---|---|---|---:|']
for it,r in rows.items():L.append(f'| [{iname(it)}](#{r["id"].lower()}) | '+', '.join(mon(x) for x in r['matching_base_forms'])+f' | {r["disposition"]} | {r["distinct_world_entitlements"]} |')
for it,r in rows.items():
 L+=['',f'<a id="{r["id"].lower()}"></a>',f'## {r["id"]} — {iname(it)}',f'**{r["disposition"]}.** {r["individual_assessment"]}',f'Item: `{it}`. {r["placement_decision"]}.', '**Native binding**','']
 for f in r['native_bindings']:
  L.append(f'- `{f["base"]}` → `{f["mega"]}` via [`{f["table"]}`](../baseline/source/{f["table_source"]}#L{f["table_line"]}). Base: [{f["base_source"]["file"]}:{f["base_source"]["line"]}](../baseline/source/{f["base_source"]["file"]}#L{f["base_source"]["line"]}); Mega: [{f["mega_source"]["file"]}:{f["mega_source"]["line"]}](../baseline/source/{f["mega_source"]["file"]}#L{f["mega_source"]["line"]}).')
 L += ['', '**Exact reward routes**']
 for rt in r['routes']:
  L+=['',f'**{rt["id"]} — {rt["kind"].replace("_"," ")}**']
  if rt['kind']=='conditional_original_starter':
   L += [f'Source: [Steven starter table](../baseline/source/src/field_specials.c#L555), original '+', '.join(mon(x) for x in rt['eligible_original_families'])+f'. Receipt: `{rt["receipt_variable"]}`.','Requires delivered letter, Knuckle Badge, Ring handoff and actual permanent delivery of the qualifying original starter. Legacy encoded-but-still-owed second choices create no stone entitlement yet.',rt['final_behavior']+' If a legacy save already hid Steven, INTRO-STONES wakes only his Granite Cave object after the actual second delivery when populated second-family slots remain owed.']
   continue
  L += [f'Map: [{rt["map"]}](../world/maps/{rt["map"]}.md); [map source](../baseline/source/{rt["map_source"]}).']
  if rt['kind']=='physical_pickup':
   e=rt['event'];g=rt['geometry'];a=', '.join(f"({x['x']},{x['y']},elevation{x['elevation']})" for x in g['adjacent_cells'] if x['collision']==0)
   L += [f'Event `{rt["event_id"]}`: ({e["x"]},{e["y"]}), elevation{e["elevation"]}, `{e["graphics_id"]}`, `{rt["source_root"]}`; quantity1. Fixed-layout walkable neighbors: {a}. This is local geometry, not a proven complete route.']
   obstacles=rt.get('field_obstacle_candidates',[])
   if obstacles:L += ['This map also contains field-obstacle instances at '+', '.join(f"({o['x']},{o['y']}) `{o['script']}`" for o in obstacles)+'. Check which, if any, the actual pickup approach crosses; do not require every obstacle merely because it shares the map.']
  elif rt['kind']=='script_grant':
   L += [f'Grant: [`{rt["source_root"]}`](../baseline/source/{rt["source"]}#L{rt["source_line"]}).']
   L += ['Entry roots (objects, triggers or map callbacks): '+ '; '.join(f"`{x['event_id']}` `{x['root']}` at {tuple(x['coordinates'])}" for x in rt['physical_roots'])+'.']
  else:L += ['Source: [`TradeEmeraldChampionsGardenBerries`](../baseline/source/src/mega_stone_rewards.c#L40), through Berry Master’s existing house NPC. Price:20 berries total, any mixture of the 11 listed garden species.']
  if rt.get('receipt_flags'):L+=['Receipt flags: '+', '.join(f'`{x["flag"]}` ({x.get("hex","unresolved")})' for x in rt['receipt_flags'])+'.']
  elif rt['kind']=='script_grant':L+=['Receipt: `VAR_TRICK_HOUSE_LEVEL`/`VAR_TRICK_HOUSE_PRIZE_PICKUP` stage transaction; final Alakazite receives the independent W-TRICK-REWARDS receipt.']
  aa=rt['map_access'];L += ['Normal source itinerary: **'+aa['stage']+'**. '+aa['note'],'Path prerequisites/conditions: '+'; '.join(aa['hard_dependencies'])+'.']
  if rt.get('prerequisites'):L += ['Grant-specific predicates: '+'; '.join(rt['prerequisites'])+'.']
  L += ['Final behavior: '+rt['final_behavior']]
 L += ['', '**Base acquisition and evolution prerequisites**']
 direct=r['base_family_acquisition_roots_snapshot']
 if direct:
  for name,arr in direct.items():
   # Human catalogue gives an explicit representative for each ancestor; complete roots are in JSON and the acquisition owner.
   shows=arr[:2];L += [f'- {mon(name)}: '+'; '.join(rootdesc(x) for x in shows)+(f'; {len(arr)-len(shows)} other snapshot root(s) in [machine catalogue](../review/mega-rewards.json).' if len(arr)>len(shows) else '.')]
 else:L += ['- This exact base is a form transition, not a direct catch. Follow the explicit FORM-08/FORM-12 route; a related family appearing somewhere does not satisfy this requirement.']
 for e in r['optional_evolution_edges']:
  src=e.get('source');L += ['- '+evtext(e)+(f' ([source](../baseline/source/{src["file"]}#L{src["line"]}))' if src else '')+'.']
 if not r['optional_evolution_edges']:L += ['- No ordinary level/item evolution is required for this exact base itself; verify its direct capture, gift, or specified form transition.']
 if it=='ITEM_FLOETTITE':L += ['- FORM-12: current Eternal Floette already appears in Verdanturf Meadow at7%; the ordinary five flower-color forms are not Mega bases.']
 if it=='ITEM_ZYGARDITE':L += ['- FORM-08/12: obtain ordinary Zygarde, use the Cube for a Power Construct form, and reach≤50% HP at turn end to become Complete. Complete is the Mega base; a Cube does not directly grant a persistent Mega form. Test this actual battle sequence.']
 if it=='ITEM_TATSUGIRINITE':L += ['- FORM-04 adds Droopy and Stretchy acquisition alongside Curly at the submarine. Their Order Up behavior differs; they are not merely cosmetic styles. Preserve the Dondozo relationship.']
 if it=='ITEM_MAGEARNITE':L += ['- FORM-05 permits the verified Original color conversion only after actual Magearna ownership. The prototype’s Diancie prerequisite remains separate from the eight-badge stone gift.']
 if 'item_description_change' in r:
  change=r['item_description_change'];L += ['', '**MEGA-FORM-LABELS — exact Bag description**','In `src/data/items.h`, replace only this item’s `.description` with:','```c','.description = COMPOUND_STRING(']
  for j,line in enumerate(change['final_lines']):L.append('    "'+line+('\\n"' if j<2 else '"),'))
  L+=['```']
 L += ['', '**Final acceptance**',f'- Walk the exact listed reward approaches under their real story/field states; a map-level first visit or a valid neighboring tile is insufficient.',f'- Check successful/full-storage/revisit behavior for each distinct entitlement, including shared first/retry branches and alternate-route ordering.',f'- Confirm exact-base Mega eligibility and reversion for the listed binding(s), using a relevant existing valid check or a focused runtime case when needed. Wrong-form and production no-Ring behavior must remain correct.', 'Dependencies: '+', '.join(r['dependencies'])+'. [Full form/acquisition authority](../chapters/04-acquisition-evolutions-forms.md).']
(B/'appendices/mega-stone-catalog.md').write_text('\n\n'.join(L[:6])+'\n\n'+'\n'.join(L[6:])+'\n')
# Native legendary overview is exact source order and identity; detailed form acquisition remains chapter 04.
native=[('GROUDON','TerraCave_End','Abnormal-weather postgame cave; actual event location and arrival state'),('KYOGRE','MarineCave_End','Abnormal-weather postgame cave reached through its current Dive approach'),('RAYQUAZA','SkyPillar_Top','After the story awakening and Sootopolis resolution; actual return/bike-floor approach'),('REGIROCK','DesertRuins','Sealed Chamber opened plus this ruin’s local puzzle'),('REGICE','IslandCave','Sealed Chamber opened plus this cave’s local puzzle'),('REGISTEEL','AncientTomb','Sealed Chamber opened plus this tomb’s local puzzle'),('LATIAS','SouthernIsland_Interior','Selected native roaming/shrine identity and the complementary island route; verify actual choice state'),('LATIOS','SouthernIsland_Interior','Selected native roaming/shrine identity and the complementary island route; verify actual choice state'),('LUGIA','NavelRock_Bottom','Champion-issued Mystic Ticket route and the long descent'),('HO_OH','NavelRock_Top','Champion-issued Mystic Ticket route and the upper ascent'),('MEW','FarawayIsland_Interior','Champion-issued Old Sea Map route and hide-and-seek'),('DEOXYS','BirthIsland_Exterior','Champion-issued Aurora Ticket route and triangle puzzle'),('JIRACHI','MeteorFalls_JirachisRoom','Actual deep Meteor Falls stair/Waterfall route to the chamber'),('DIANCIE','CaveOfOrigin_DianciesRoom','Eight-badge ladder and expanded Origin cave route'),('HEATRAN','ScorchedSlab_HeatransRoom','Actual deep cave path and Magma Stone initial reveal'),('MOLTRES','EmberPath','Heat Badge/Strength branch through Ember Path')]
chapter=r'''# 05 — Mega Stones and legendary rewards

The current world reward architecture is worth preserving. The book keeps every existing Mega Stone position, the meaningful NPC gifts, the three Berry Master exchanges and the Ring’s acquisition. The repairs concern exact receipt behavior, accurate information and actual form availability—not a new policy rationing strong Pokémon by chapter.

## The complete native roster

The frozen source contains **99 Mega Stone items with 103 item-triggered native Mega bindings**. The extra bindings come from male/female Meowstic, ordinary/Original Magearna and the three functional Tatsugiri forms. Rayquaza supplies the additional stone-free Mega binding, bringing the compiled `.isMegaEvolution` total to 104. This is a snapshot inventory, not a permanent game gate or a claim about an external competitive service’s current roster.

[The complete per-stone catalogue](../appendices/mega-stone-catalog.md) individually specifies all 99 items, native base/Mega forms, source locations, flags, NPC roots, world context, evolution/acquisition dependencies, final disposition and acceptance. [Its JSON companion](../review/mega-rewards.json) keeps all raw source routes and complete ancestry/root data.

The current routes reconcile to 73 physical sparkles,41 literal scripted grant sites and3 berry exchanges. First/retry grant sites are not separate prizes: after grouping their real receipts, these are109 distinct finite world entitlements. Ten stones have two deliberate world sources; the other89 have one. The conditional original-starter gifts are additional pathways, fulfilled by the same inventory/held ownership and receipt rules described below.

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

**W-TRICK-REWARDS** fixes the real exceptions. Puzzle 4’s retry must give King’s Rock, matching the immediate reward. The final chosen tent and Alakazite need independent successful receipts, with a one-time initialization marker for conservative old-state handling; claim only the still-owed part at the entrance. Keep Granite Cave’s Alakazite pickup as its deliberate alternate. The three receipt/initialization flags are reserved at 0x2B2–0x2B4 in the [central state allocation](../appendices/state-allocation.md); initialize them only in the agreed version4 migration. Do not force players to replay puzzle 8 or grant extra tents to compensate for a failed stone insertion.

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

The baseline has 82 Sign entries:51 deliberate landmarks,7 visible field entries,7 ordinary-wild entries and17 other-provider entries. Those17 comprise 12 Circuit rewards,2 mastery rewards,2 Game Corner rewards and1 breeding route. An `OTHER_SIGN`’s zero badge field is not evidence of opening availability; its real provider owns the gate.

Devon remains an optional guide, including after a discovery is completed. The actual field/badge/story/species conditions remain authoritative. Failed encounters can be retried after leaving and returning; captures and earned reward entitlements remain permanent. W-SIGN-OPTIONAL and W-SIGN-LOCAL remove stale mandatory research and retired partner silhouettes from live world text. Do not restore the old compulsory researcher visit or permanent failure rule.

FORM-06 appends the three distinct Galarian bird Signs at IDs82–84, leaving the original 82 identities and existing native one-off indexes stable. Exact-form acquisition is resolved before any permitted family fallback. Its detailed locations, conditions and capture identity are owned by chapter 04; this reward chapter does not duplicate that definition.

The 16 native one-off species below remain separate from the Sign array. Their reserved encounter indexes are96–111 in source order. The apparent `CAUGHT`/`DEFEATED` names of physical hide flags do not independently prove capture—Heatran’s reveal is a useful example. Preserve actual caught ownership and each event’s current retry behavior.
'''
chapter+='\n\n| Native species | Current principal scene | Actual access context |\n|---|---|---|\n'+'\n'.join(f'| {mon("SPECIES_"+name)} | [{mapn}](../world/maps/{mapn}.md) | {gate} |' for name,mapn,gate in native)
chapter+=r'''

Six capture reward groups grant 24 relic items: Groudon→Red Orb; Kyogre→Blue Orb; Zacian→Rusted Sword; Zamazenta→Rusted Shield; Ogerpon→three masks; Arceus→seventeen type plates. Keep the existing earned-group and pending-item bits. An actual acquisition whose insertion fails creates debt; later nurse/service retries settle it. Migration must not fabricate a pending reward merely from old Pokédex ownership or recreate a relic discarded after a fulfilled grant. Fusion tools and Zygarde Cube follow FORM-08 and the actual Birch research reward path.

## Implementation and verification

First reconcile the exact source and final acquisition/form definitions. Preserve all current stone placements while applying the named receipt, clue and description repairs. Extend the original-starter helper through INTRO-STONES’s single receipt variable; do not implement another parallel gift ledger. Apply the independent Trick House receipts together with entrance recovery and central flag allocation.

Then verify behavior in proportion to the changes. Useful distinct cases are: ordinary pickup with persistent ownership; a leader first/retry gift; the shared Winona/keeper entitlement; conditional two-starter delivery with partial storage; final tent/stone split; each materially different berry transaction outcome; the special exact-form bindings; and a production Ring check. Inspect unchanged data bindings once and reuse valid existing evidence rather than creating99 identical tests.

Field access checks must start from a legitimate story entry and reach the exact tile or NPC, including any real Cut, Strength, Surf, Dive, Waterfall, bike, tide, ledge or scripted gate. New Mauville requires the actual Surf/Key route; Dive access requires Mind Badge/Steven’s handoff; Southern Island uses the Champion ferry pass; desert approaches require their actual Goggles/underpass route. A fallback cap attached to a whole map is not acceptable proof. Record any unresolved earliest-access question explicitly instead of inventing a value.

Audit every existing gate’s contract, fixture, configuration and artifact freshness before interpreting a failure. Update or delete obsolete prose/count expectations that constrain deliberate design. Keep meaningful item/receipt, save and battle failure protections. This chapter’s99 count and quoted text are review scope and implementation instructions, not new production invariants. No ROM was built or gameplay changed in producing this book; traversal and battle acceptance remains implementation-time evidence.
'''
(B/'chapters/05-mega-stones-and-legendary-rewards.md').write_text(chapter.strip()+'\n');d['native_one_off_overview']=[{'species':'SPECIES_'+n,'map':m,'context':c,'reserved_index':96+i} for i,(n,m,c) in enumerate(native)];d['test_policy']='Counts/prose bind this snapshot book only. Do not add99mirrored tests or production count/prose gates; reuse valid evidence, audit failures, and verify distinct changed behaviors.';d['central_proposals']=['MEGA-RING KEEP','INTRO-STONES sharedfour-slots/actualsecond-delivery/Stevenwake','MEGA-FORM-LABELS sixdescriptionrepairs','W-TRICK-REWARDS','FORM-04','FORM-05','FORM-06','FORM-08','FORM-12'];(B/'review/mega-rewards.json').write_text(json.dumps(d,ensure_ascii=False,indent=2)+'\n')
print('Rendered99stonecatalog andMega/legendarychapter; current sources preserved.')
