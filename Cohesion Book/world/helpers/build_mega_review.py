from pathlib import Path
import json,re,collections,struct,ast,operator
B=Path(__file__).resolve().parents[2];S=B/'baseline/source';V=B/'inventory'
species=json.loads((V/'species.json').read_text());forms=json.loads((V/'form-changes.json').read_text());reward_inv=json.loads((V/'mega-rewards.json').read_text());maps=json.loads((V/'maps.json').read_text());world=json.loads((B/'world/event-ledger.json').read_text())['maps'];acq=json.loads((V/'acquisition-roots.json').read_text());signs=json.loads((V/'legendary-signs.json').read_text())
notes={
'ABOMASITE':'The frozen Seaspray floor is an appropriate local discovery for the Snover family. Preserve its early discoverability; an evolution requirement is not a reason to remove anticipation or other early Ice options.',
'ABSOLITE':'The Jagged Pass find and Lucy’s Lavaridge reward are two deliberate finite alternatives: one exploration, one authored challenge. Preserve both with separate receipts and no repeat award from either.',
'ABSOLITE_Z':'Keep the Route114 alternate stone as a separate exploration choice for ordinary Absol; it selects the Z Mega and must not be mistaken for a regional Absol acquisition.',
'AERODACTYLITE':'Roxanne’s Old Amber/stone pairing gives the fossil a memorable personal connection. Preserve both rewards and their independent full-storage recovery; Mega use still waits for Ring and an actual Aerodactyl.',
'AGGRONITE':'The Meteor Falls mineral chamber suits Aron’s family and rewards a cave visit. Keep the pickup instead of attaching Aggron to an arbitrary power chapter.',
'ALAKAZITE':'Keep Granite Cave exploration and the authored final Trick Master gift as known alternatives. Repair final tent/stone debt independently under W-TRICK-REWARDS; the early cave stone remains useful to the Abra family.',
'ALTARIANITE':'Preserve the Route111 find plus one shared Winona/Lilycove-keeper entitlement. The keeper is a recovery/alternate route to the same leader gift, not an additional freely repeatable stone.',
'AMPHAROSITE':'The Mauville household’s beacon metaphor fits the city and an early Mareep investment. Keep the personal gift and its one-time flag.',
'AUDINITE':'Keep the ordinary Route117 flower NPC gift. It is intentionally separate from Daycare breeding and uses the existing relocated pickup receipt.',
'BANETTITE':'The abandoned Dewford manor gives this stone an appropriate haunted setting. Preserve the moved Mt.Pyre-named flag without resurrecting its former physical pickup.',
'BARBARACITE':'The Dewford tide-pool story gives Binacle’s evolution a believable local stone. Preserve that household gift without adding a capture/party requirement.',
'BAXCALIBRITE':'Keep one of the three exclusive mixed-garden-berry exchanges. Twenty eligible berries are an exploration/cultivation reward, not free preset currency.',
'BEEDRILLITE':'The deep old-growth forest is a good insect discovery. Keep its Cut-dependent geographical branch rather than making Beedrill wait for an arbitrary strength milestone.',
'BLASTOISINITE':'Keep Seaspray’s wet-cave pickup and the conditional original-starter gift. Both belong in the broad early team-building environment; INTRO-STONES updates only the two-starter receipts.',
'BLAZIKENITE':'The Lavaridge footwork story fits Blaziken. Keep the household stone and conditional starter gift; choosing another starter never removes the world source.',
'BUTTERFRENITE':'A familiar early forest insect deserves a convincing Mega option. Keep the Petalburg Woods stone and broad ordinary Butterfree acquisition; Ring remains the shared activation gate.',
'CAMERUPTITE':'Keep Fiery Path exploration and Flannery’s one-time reward as finite alternatives. The two GiveCameruptite labels are first/retry presentations of one leader entitlement.',
'CHANDELURITE':'The manor’s haunted lamps suit Chandelure. Preserve the stone and the separate Litwick/Lampent evolution requirements; do not move all ghost options back to Mt.Pyre.',
'CHARIZARDITE_X':'Keep the Route110 find and the conditional Charmander gift. The X form is distinct from Y, and both stable starter receipt slots must survive partial storage failure.',
'CHARIZARDITE_Y':'Keep the Route117 find and conditional Charmander gift. The Y option is independent of X; one successful receipt must never clear the other pending stone.',
'CHESNAUGHTITE':'Keep Route117’s meadow find and the Chespin starter alternative. Neither route requires creating a separate Mega archive or revising an already good starter set.',
'CHIMECHITE':'The Fortree household’s wind-chime story directly fits Chimecho. Keep the gift; early Chingling remains a separate usable opening choice rather than a breeding obligation.',
'CLEFABLITE':'Cozmo’s meteor household and Clefairy associations make a strong local gift. Preserve ordinary Clefable’s Moon Stone path and the current NPC receipt.',
'CRABOMINITE':'Keep the frozen Seaspray discovery. The acquisition volume owns Crabrawler’s practical Ice Stone evolution; the stone does not itself evolve Crabrawler.',
'DARKRANITE':'Victory Road provides a late exploration reward for Darkrai’s existing nightmare quest. Keep the specific Darkrai acquisition prerequisites; do not treat the stone as a Darkrai unlock.',
'DELPHOXITE':'Keep the Route110 find plus the original Fennekin gift. Broad starter access lets players use the stone without a required initial selection.',
'DIANCITE':'Keep the crystal-chamber pickup beside Diancie. W-WALLACE-ROOT restores the optional exhibition without a second Diancite handoff; the stone remains an exploration reward.',
'DRAGALGITE':'The long rainy Route119 and marine family connect naturally. Keep the exact pickup; Skrelp/Dragalge acquisition must use an actually unlocked fishing/Surf method.',
'DRAGONINITE':'Keep the exclusive twenty-berry exchange. Dratini-family availability and Dragonite’s evolution are separate from paying for the stone; do not sell this berry currency through free presets.',
'DRAMPANITE':'Keep the Route111 discovery as an unusual optional partner reward. Drampa is already a complete single-stage base, so stone usability depends on an actual Drampa and Ring rather than an evolution level.',
'EELEKTROSSITE':'Keep the Route110 electrical corridor discovery. Tynamo/Eelektrik/Thunder Stone and Ring availability must be considered together; the later free evolution archive already makes the item requirement practical.',
'EMBOARITE':'Keep the Route110 world find and original Tepig alternative. Preserve the complete native Emboar Mega binding and do not invent a Mega for the other Unova starters.',
'EXCADRITE':'The ash-country excavation theme fits Excadrill. Keep the Route113 find and its persistent identity.',
'FALINKSITE':'Keep Falinks’s Route113 discovery; a single-stage tactical specialist is a legitimate strong option without a conventional evolution payoff.',
'FERALIGITE':'Keep the Route110 find plus original Totodile gift. The current Totodile family can reach Feraligatr at its native evolution level; that timing is a valid feature of the chosen family.',
'FLOETTITE':'Keep Verdanturf Meadow’s stone beside its existing Eternal Floette habitat. Only Eternal Floette uses it; ordinary flower-color Floette cannot be silently treated as equivalent.',
'FLYGONITE':'Sandstrewn Ruins reinforces Flygon’s desert identity. Preserve the pickup and global Trapinch-family coverage; exploration and evolution are distinct requirements.',
'FROSLASSITE':'Keep the frozen-cave reward for female Snorunt’s Dawn Stone branch. Its early stone does not eliminate Glalie’s separate option.',
'GALLADITE':'The Verdanturf household’s protective-blades story fits Gallade and Wally’s wider family identity. Preserve the male Kirlia/Dawn Stone condition and free-item practicality.',
'GARCHOMPITE':'Keep the main Sandstrewn Ruins stone for the ordinary Mega. Its desert location provides an alternative to the separate Z-form stone on Route116.',
'GARCHOMPITE_Z':'Keep the Route116 eastern discovery for Garchomp’s Z Mega. The base remains ordinary Garchomp; there is no separate catchable Garchomp-Z prerequisite.',
'GARDEVOIRITE':'Keep Wally’s aunt’s partnership story and gift. The separate proposed Indeedee-F gift must never gate, duplicate or consume the Gardevoirite entitlement.',
'GENGARITE':'Keep Greta’s authored Slateport challenge and the Route120 find as two finite sources. Early strength is deliberate; do not move the stone solely because Gengar is powerful.',
'GLALITITE':'Keep Seaspray’s frozen-floor find and ordinary Glalie binding. Glacia’s restored Walrein identity does not require deleting Glalie/Froslass options from the world.',
'GLIMMORANITE':'Granite Cave’s crystalline minerals fit Glimmora. Preserve the source tile and the actual Glimmet evolution path rather than inferring immediate use from an unevolved family encounter.',
'GOLISOPITE':'Keep the Route109 coastal reward for Wimpod’s evolved form. The stone does not remove Wimpod’s native evolution requirement.',
'GOLURKITE':'The archaeological ruins provide a convincing home for Golurk’s stone. Preserve the main-floor exploration path and Golett-family acquisition.',
'GRENINJITE':'Keep Route117 and original Froakie alternatives. This stone binds ordinary Greninja, not the distinct Ash form; final form coverage must respect that distinction.',
'GYARADOSITE':'Keep the Dewford household gift and Juan’s leader gift as two explicitly finite routes. Magikarp is broadly available; the world gift keeps initial starter selection from restricting this option.',
'HAWLUCHANITE':'The Dewford meadow offers a distinctive athletic single-stage partner option. Keep the stone and its exact marker without a new species-showcase gate.',
'HEATRANITE':'Keep Heatran’s chamber stone separate from the Magma Stone reveal and capture. The visibility flag’s name is not a capture ledger; preserve actual permanent-capture state.',
'HERACRONITE':'Petalburg Woods is a strong habitat for a memorable fighting beetle. Keep the exploration reward without delaying it on power-tier grounds.',
'HOUNDOOMINITE':'Keep Fiery Path’s upper reward and Houndour-family access. Check the exact boulder/side-corridor approach during traversal instead of assigning the whole map a single acquisition cap.',
'KANGASKHANITE':'Keep the one qualifying Daycare-produced-and-hatched egg reward. Gift eggs do not count, both legacy receipt flags prevent duplicates, and Safari Kangaskhan remains the advertised base route.',
'KINGDRANITE':'The rainy river route suits the Horsea family. Preserve the Route119 find, actual rod/Surf access and practical Dragon Scale evolution.',
'KINGLERITE':'Keep the Dewford coastal household story and one-time gift for Krabby’s evolved form.',
'LAPRASITE':'The Lilycove family’s rescue-at-sea story gives this stone emotional value. Keep the household gift and broad legitimate Lapras acquisition.',
'LATIASITE':'Keep Southern Island’s distinct eon-stone discovery. The Champion ferry pass and actual island route are the acquisition gate; do not label it pre-League merely from an80-cap fallback.',
'LATIOSITE':'Keep the complementary Southern Island eon stone. Both eon families have native routes; the selected/unchosen encounter distinction must not strand either matching stone.',
'LOPUNNITE':'Keep the Petalburg Woods find and Norman’s personal reward as two finite sources. Buneary’s friendship evolution is supported by convenient preparation; no stat breeding required.',
'LUCARIONITE':'Keep Route116 exploration and Brawly’s one-time reward. Riolu’s early availability is deliberate; native evolution conditions and Ring delivery determine actual usability.',
'LUCARIONITE_Z':'Keep the distinct Route116 Z Mega stone. Its base is ordinary Lucario; preserve both Lucario Mega options without conflating their item receipts.',
'MACHAMPITE':'The Lavaridge bath-building story suits Machamp. Keep the gift and practical Machoke evolution item; no multiplayer trade requirement should be invented.',
'MAGEARNITE':'Keep the eight-badge Devon gift. Magearna’s actual Diancie-linked prototype acquisition and verified Original-color conversion belong to the form chapter; this one item supports both native Mega bindings.',
'MALAMARITE':'The name rater’s upside-down Inkay story is a well-matched local reward. Preserve the gift and the build’s actual Inkay evolution condition, not an assumed device-motion requirement.',
'MANECTITE':'Wattson’s leader reward is the single world entitlement. Preserve first/retry branches sharing one flag; no repeat reward on ordinary rematch.',
'MAWILITE':'Keep the Dewford meadow find for a distinctive single-stage Pokémon. Broad early team options are the intended design.',
'MEDICHAMITE':'Keep the northern Route115 exploration reward. Its actual shore/ledge approach needs a physical route check; Meditite’s family appearing earlier is not a reason to relocate the stone automatically.',
'MEGANIUMITE':'Keep the Route110 world source and Chikorita starter alternative. The stone supports Meganium specifically; later evolution readiness is a practical condition rather than a protected chapter.',
'MEOWSTICITE':'One Route116 stone supports male and female Meowstic through separate native form tables. Preserve the shared item and actual sex-specific base acquisition.',
'METAGROSSITE':'Keep Meteor Falls exploration and Tate/Liza’s one-time reward as finite alternatives. Beldum’s early availability and Steven’s later gift are independent ways to obtain the family.',
'MEWTWONITE_X':'Keep Leaf’s authored reward in Altering Cave with its pending-Bag recovery choreography. Preserve Mewtwo’s postgame Sign gate and one-time capture rather than making the stone grant Mewtwo.',
'MEWTWONITE_Y':'Keep the deeper Altering Cave Y stone as exploration alongside Leaf’s X reward. The distinct choices share Mewtwo as their base.',
'MILOTICITE':'The Fortree household gift complements Milotic’s beauty and the Feebas discovery. Preserve practical Prism Scale evolution; this item does not require contest grinding.',
'PIDGEOTITE':'Keep Mr. Stone’s post-Ring thanks. Repair the stale free-Mega-archive explanation under W-MEGA-GUIDE; preserve the actual Pidgeotite receipt.',
'PINSIRITE':'Keep Ashen Woods’s insect-side discovery and exact flagged pickup. Strong optional bugs are welcome before the League.',
'PYROARITE':'Keep Route115’s lower-shore stone for Litleo’s evolution. Verify the physical shore approach instead of assuming all of Route115 has the same gate.',
'RAICHUNITE_X':'Keep the Verdanturf friendship household gift and its Pichu story. The native X Mega belongs to ordinary Raichu, not Alolan Raichu.',
'RAICHUNITE_Y':'Keep Route115’s distinct Y-form exploration reward. Ordinary Raichu is the matching base; the two Raichu stones are not regional-form acquisition substitutes.',
'SABLENITE':'The abandoned manor is an excellent Sableye setting. Keep the stone and haunted exploration rather than restricting all ghost discoveries to later Mt.Pyre.',
'SALAMENCITE':'Keep Meteor Falls’s dragon-associated find. Bagon can remain an early long-term option; no exclusive late chapter is imposed on its family.',
'SCEPTILITE':'Keep the northern Route104 world source and original Treecko alternative. The forest-side geography supplies nostalgia without making the initial choice exclusive.',
'SCIZORITE':'Keep Route118’s eastern discovery and actual Surf access. Scyther’s Metal Coat route is practical through the existing evolution archive.',
'SCOLIPITE':'Keep the eastern Petalburg Woods insect stone, checking the local Cut branch rather than treating the entire woods as freely traversable.',
'SCOVILLAINITE':'Jagged Pass is an appropriate hot-slope discovery for Capsakid’s Fire Stone evolution. Preserve the exact pickup and usable evolution item access.',
'SCRAFTINITE':'Keep the rough Jagged Pass find and Scraggy-family route. Preserve the evolved-base requirement and source party cap semantics.',
'SHARPEDONITE':'Keep the Route109 coastal discovery. Water methods and rod access determine when Carvanha/Sharpedo are actually obtainable; the map’s early visit alone does not.',
'SKARMORITE':'Route112’s exposed rocky slopes fit Skarmory. Preserve the stone, single-stage base and exact lower/upper approach check.',
'SLOWBRONITE':'Keep the cold Seaspray side-floor reward for ordinary Slowbro. Galarian Slowbro is a different base and is not enabled by this stone.',
'STARAPTITE':'Keep the Route116 find and Trick House puzzle1 gift as deliberate finite alternatives. First and entrance-retry gift labels represent the same puzzle entitlement.',
'STARMINITE':'The Lilycove sea-and-stars gift fits Starmie. Preserve the practical Water Stone path and actual marine acquisition methods.',
'STEELIXITE':'The ruins exterior is a convincing mineral reward for Onix’s evolution. Preserve the stair/terrace approach and practical Metal Coat use.',
'SWAMPERTITE':'Keep the Route104 world source and original Mudkip gift. The family’s broad starter access keeps the stone useful regardless of the original pair.',
'TATSUGIRINITE':'Keep the submarine-area stone with Dondozo/Tatsugiri. FORM-04 adds the missing functional forms at that same habitat; this item supports all three distinct native Mega forms.',
'TYRANITARITE':'Keep the exclusive twenty mixed-garden-berry trade. Larvitar-family growth and direct catches remain global acquisition matters; neither creates free exchange currency.',
'VENUSAURITE':'The flower-shop gift gives the Bulbasaur family an appropriate local connection. Preserve it and the original-starter alternative with updated shared receipts.',
'VICTREEBELITE':'Keep Route115’s vegetated coastal discovery for Bellsprout’s Leaf Stone branch. It enriches a familiar region without another NPC toll.',
'ZERAORITE':'Keep Route120’s exploration reward while Zeraora remains a deliberate New Mauville discovery. Cross-region stone/base pairings can encourage useful revisits when clues stay accurate.',
'ZYGARDITE':'Keep the Victory Road find as anticipation of Zygarde’s postgame quest. Only Complete Forme is a native Mega base: Cube/Power Construct and the at-most-half-HP turn-end transformation are required, as specified in FORM-08/12.',
}
assert set('ITEM_'+k for k in notes)==set(reward_inv),(set(reward_inv)-set('ITEM_'+k for k in notes),set('ITEM_'+k for k in notes)-set(reward_inv))
# Source label index limited to the actual Hoenn map scripts; no game code is executed.
labels={};maptexts={}
for n in maps:
 p=S/'data/maps'/n/'scripts.inc'
 if not p.exists():continue
 text=p.read_text();maptexts[n]=text;lines=text.splitlines();marks=[]
 for i,line in enumerate(lines):
  mm=re.match(r'^([A-Za-z_]\w*)::?',line)
  if mm:marks.append((i,mm[1]))
 for j,(start,lab) in enumerate(marks):
  end=marks[j+1][0] if j+1<len(marks) else len(lines);labels[lab]={'map':n,'source':str(p.relative_to(S)),'line':start+1,'body':'\n'.join(lines[start+1:end])}
# Resolve simple native flag constants, preserving any unresolved expression explicitly.
defs={}
for pp in [S/'include/constants/flags.h',S/'include/constants/vars.h']:
 for line in pp.read_text().splitlines():
  m=re.match(r'^#define\s+(\w+)\s+(.+?)\s*(?://.*)?$',line)
  if m:defs[m[1]]=m[2].split('//')[0].strip()
ops={ast.Add:operator.add,ast.Sub:operator.sub,ast.Mult:operator.mul,ast.LShift:operator.lshift,ast.RShift:operator.rshift,ast.BitOr:operator.or_,ast.BitAnd:operator.and_,ast.BitXor:operator.xor}
def value(x,stack=()):
 if x in stack:raise ValueError(x)
 if x in defs:return calc(ast.parse(defs[x],mode='eval').body,stack+(x,))
 return int(x,0)
def calc(n,stack):
 if isinstance(n,ast.Constant) and isinstance(n.value,int):return n.value
 if isinstance(n,ast.Name):return value(n.id,stack)
 if isinstance(n,ast.BinOp) and type(n.op) in ops:return ops[type(n.op)](calc(n.left,stack),calc(n.right,stack))
 if isinstance(n,ast.UnaryOp) and isinstance(n.op,ast.USub):return-calc(n.operand,stack)
 raise ValueError(ast.dump(n))
def flaginfo(flag):
 try:v=value(flag);return {'flag':flag,'numeric':v,'hex':hex(v)}
 except:return {'flag':flag,'numeric':None,'expression':defs.get(flag)}
# Correct story bounds. These are not claims that every tile in a map is reachable at that moment.
def access(n):
 if n.startswith(('AlteringCave','SouthernIsland')):return {'stage':'Champion/postgame','hard_dependencies':['FLAG_SYS_GAME_CLEAR','For Southern Island: native Eon Ticket/registered ferry route from Lilycove'],'note':'For Altering Cave follow its actual postgame entrance and Leaf/Mewtwo side route; for island follow the actual harbor pass. No80-cap fallback claim.'}
 if n.startswith('Underwater_SeafloorCavern'):return {'stage':'After Mind Badge and Space Center/Dive handoff','hard_dependencies':['FLAG_BADGE07_GET','FLAG_RECEIVED_HM_DIVE','Surf license+Balance Badge','compatible Dive/Surf party users','Steven Space Center success handoff'],'note':'Dive is a70-cap-era field access in the current campaign, not a60-cap map fallback. Inspect submarine approach and pickup elevation.'}
 if n.startswith('VictoryRoad'):return {'stage':'After all eight badges and Waterfall access','hard_dependencies':['FLAG_BADGE08_GET','FLAG_RECEIVED_HM_WATERFALL','Waterfall-capable party member','Surf to Ever Grande'],'note':'The stone can precede its postgame legendary base; this is intentional anticipation.'}
 if n.startswith('CaveOfOrigin_DianciesRoom'):return {'stage':'Expanded Origin cave after eight-badge ladder gate','hard_dependencies':['Sootopolis story access','FLAG_BADGE08_GET clears Carbink ladder gate','CaveOfOrigin former R/S floor chain'],'note':'Wallace exhibition additionally needs game clear; Diancie/Diancite remain independent.'}
 if n.startswith('ScorchedSlab'):return {'stage':'Fortree/Route120 access plus actual cave descent','hard_dependencies':['Surf license+Balance Badge','Route120 cave entry','Strength license+Heat Badge for boulder descent as encountered','Magma Stone needed for Heatran reveal, not automatically for the stone pickup'],'note':'Confirm lower-floor geometry and separate Magma Stone ownership from the free chamber stone.'}
 if n.startswith(('SandstrewnRuins','Route111_RuinsExterior')):return {'stage':'Desert archaeology after Go-Goggles, with alternate post-collapse underpass','hard_dependencies':['FLAG_RECEIVED_GO_GOGGLES','Mirage Tower basement or current Desert Underpass entrance','Actual branching stairs/field obstacles'],'note':'Do not infer a30-cap desert opening. Tower collapse must not delete the alternate route to the ruins.'}
 if n.startswith('AshenWoods'):return {'stage':'After Heat Badge/Strength access through Ember Path','hard_dependencies':['FLAG_BADGE04_GET','FLAG_RECEIVED_HM_STRENGTH','compatible Strength user','JaggedPass→EmberPath→AshenWoods'],'note':'Retain local weather and trainer paths; verify actual final-tile approach.'}
 if n.startswith('NewMauville'):return {'stage':'After Norman/Surf and Wattson’s Basement Key','hard_dependencies':['FLAG_BADGE05_GET','FLAG_RECEIVED_HM_SURF','Basement Key','compatible Surf user'],'note':'The map does not open at40 merely because an audit fallback says so. Follow generator/door state separately.'}
 if n.startswith('RustboroCity_DevonCorp'):return {'stage':'Devon access after Rusturf parcel return; gift-specific gates below','hard_dependencies':['FLAG_RETURNED_DEVON_GOODS'],'note':'Mr.Stone’s Pidgeotite needs delivered letter+Mega Ring; Magearnite additionally needs eight badges. These are separate from access to the building.'}
 if n.startswith('RustboroCity_Gym'):return {'stage':'Roxanne victory','hard_dependencies':['FLAG_BADGE01_GET','leader defeated/reward receipt path'],'note':'Old Amber and Aerodactylite may be obtained before the Ring; keep both pending rewards safe.'}
 if n.startswith('DewfordTown_Gym'):return {'stage':'Brawly victory after museum-queue return','hard_dependencies':['Brawly returned from Slateport','FLAG_BADGE02_GET'],'note':'Stone may be granted just before Steven’s Ring; no arbitrary extra delay.'}
 if n.startswith('MauvilleCity_Gym'):return {'stage':'Wattson victory','hard_dependencies':['Knuckle gate to Route110/Mauville','FLAG_BADGE03_GET'],'note':'First and retry scripts share one leader receipt.'}
 if n.startswith('LavaridgeTown_Gym'):return {'stage':'Flannery victory','hard_dependencies':['Meteor Falls/Mt.Chimney story','FLAG_BADGE04_GET'],'note':'First and retry scripts share one leader receipt.'}
 if n.startswith('PetalburgCity_Gym'):return {'stage':'Norman victory after four badges','hard_dependencies':['four prior badges','FLAG_BADGE05_GET'],'note':'Preserve personal leader reward and Surf handoff independently.'}
 if n.startswith('FortreeCity_Gym'):return {'stage':'Winona victory','hard_dependencies':['Surf east/Weather Institute','Devon Scope Gym path','FLAG_BADGE06_GET'],'note':'Shared gift receipt also serves Lilycove keeper; field Route111 source has its own receipt.'}
 if n.startswith('MossdeepCity_Gym'):return {'stage':'Tate/Liza victory','hard_dependencies':['east sea open after Aqua hideout','FLAG_BADGE06_GET gate','FLAG_BADGE07_GET on win'],'note':'Space Center story follows; stone reward does not itself supply Dive.'}
 if n.startswith('SootopolisCity_Gym'):return {'stage':'Juan victory after sky/crater crisis','hard_dependencies':['Rayquaza story resolution','Mind Badge','FLAG_BADGE08_GET on win'],'note':'Keep Waterfall/League handoff and leader reward.'}
 if n.startswith(('LilycoveCity','MossdeepCity')):return {'stage':'Eastern region after Feather/Aqua-hideout story as applicable','hard_dependencies':['FLAG_BADGE06_GET to pass Route120 east gate','For Mossdeep: clear Aqua hideout Wailmer sea blockade'],'note':'Indoor gifts use their own flags and conditions; a city visit is not proof a field pickup was reachable.'}
 if n.startswith('Route123'):return {'stage':'Eastern fields/Berry Master after Surf and actual one-way route approach','hard_dependencies':['Surf license+Balance Badge','route approach from118/121','20 eligible mixed berries per exclusive trade'],'note':'A fee is a real prerequisite; no free preset or vendor may supply the garden currency.'}
 if n.startswith(('Fortree','Route118','Route119','Route120')):return {'stage':'Surf/rainforest/Fortree chapter','hard_dependencies':['FLAG_BADGE05_GET','FLAG_RECEIVED_HM_SURF','compatible Surf user','Weather Institute/Scope/Feather gates where the actual path crosses them'],'note':'Route118’s west bank exists earlier; its eastern pickup requires the river crossing. Route120 halves have different gates.'}
 if n.startswith(('Route117','Verdanturf','Mauville','Route110')):
  if n.startswith('Route110_TrickHouse'):return {'stage':'Specific Trick House puzzle clearance','hard_dependencies':['puzzle1: Route110 access and Cut; puzzle8: game clear','current stage/scroll completion','pending prize receipt'],'note':'The stage, not the map’s generic cap, determines the gift. Entrance retry is the same entitlement.'}
  return {'stage':'Mauville/meadows after Knuckle gate','hard_dependencies':['FLAG_BADGE02_GET to Route110/Mauville','actual Cut/Surf/bike subpath if applicable'],'note':'Route110 water islands and bike-road tiles require separate physical checks. Direct meadow gifts do not inherit a later evolution’s level.'}
 if n.startswith(('Route111','Route112','Route113','Route114','FieryPath','MeteorFalls','JaggedPass','Fallarbor','Lavaridge')):return {'stage':'Northern mountain route after Dynamo; subareas vary','hard_dependencies':['FLAG_BADGE03_GET for northern Route111 gate','Meteor Falls/Mt.Chimney progression for Jagged Pass/Lavaridge','Go-Goggles for desert','Strength/Surf/Waterfall/bike only where actual route requires them'],'note':'Do not assign one earliest cap to an entire long route or multi-floor cave. Exact pickup approach remains a required traversal case.'}
 if n.startswith(('Dewford','GraniteCave','Slateport','Route109')):return {'stage':'First coastal journey after Stone Badge/Devon letter','hard_dependencies':['Roxanne→Rusturf parcel→Mr.Stone letter','Briney sea itinerary','For Slateport: deliver Steven letter','Actual rod/Surf/bike/field subpath if applicable'],'note':'Granite darkness affects visibility; Flash license/badge is not automatically a hard physical gate. Keep early coastal discoveries.'}
 if n.startswith('PetalburgWoods_'):return {'stage':'Deep woods via eastern Cut paths','hard_dependencies':['FLAG_BADGE01_GET','FLAG_RECEIVED_HM_CUT','compatible Cut user','deep-forest warp route'],'note':'The parent Woods is available earlier. Inspect each actual approach/loop to the stone.'}
 return {'stage':'Western opening geography; exact subpath may need field access','hard_dependencies':['ordinary opening story access','Cut/Surf/bike gate only where physical approach requires it'],'note':'Routes104/115/116 span separate pockets. A map-level first visit does not establish exact-tile access.'}
# Native item↔form binding: use tables and their ordinary owners, not display-name guessing.
bindings=collections.defaultdict(list)
for table,d in forms.items():
 for entry in d['entries']:
  m=re.search(r'FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM,\s*(SPECIES_\w+),\s*(ITEM_\w+)',entry)
  if not m:continue
  owners=[n for n,v in species.items() if v['change_table']==table and 'isMegaEvolution' not in v['flags'] and 'isGigantamax' not in v['flags']]
  for base in owners:bindings[m[2]].append({'base':base,'mega':m[1],'table':table,'table_source':'src/data/pokemon/form_change_tables.h','table_line':d['line'],'base_source':species[base]['source'],'mega_source':species[m[1]]['source']})
assert set(bindings)==set(reward_inv)
# Evolution ancestry is a set of exact optional routes; direct catches may bypass a level evolution.
parents=collections.defaultdict(list)
for n,v in species.items():
 for ev in v['evolutions']:parents[ev['target']].append({'from':n,**ev,'source':v['source']})
def ancestry(base):
 seen=set();todo=[base];edges=[]
 while todo:
  q=todo.pop()
  if q in seen:continue
  seen.add(q)
  for e in parents[q]:edges.append(e);todo.append(e['from'])
 return sorted(seen),edges
# Geometry: tile and adjacent approach candidates only, never a false stateful reachability proof.
layouts={r['id']:r for r in json.loads((S/'data/layouts/layouts.json').read_text())['layouts']}
def geom(n,e):
 l=layouts[maps[n]['layout']];data=(S/l['blockdata_filepath']).read_bytes();a=struct.unpack('<'+'H'*(len(data)//2),data);x,y=e['x'],e['y'];word=a[y*l['width']+x]
 adj=[]
 for dx,dy in [(0,-1),(-1,0),(1,0),(0,1)]:
  xx,yy=x+dx,y+dy
  if 0<=xx<l['width'] and 0<=yy<l['height']:
   z=a[yy*l['width']+xx];adj.append({'x':xx,'y':yy,'collision':(z>>10)&3,'elevation':z>>12})
 return {'layout':l['id'],'blockdata':l['blockdata_filepath'],'tile_collision':(word>>10)&3,'tile_elevation':word>>12,'adjacent_cells':adj,'scope':'Fixed snapshot layout only; dynamic metatiles, objects, ledges, currents, elevation transitions and stateful access are not solved.'}
# Readable source grant sites with physical roots and persistent receipts.
rows={};allroutes=[];startertext=(S/'src/field_specials.c').read_text();starterblock=re.search(r'sStarterStones\[\].*?=\s*\{(.*?)\n    \};',startertext,re.S)[1];starterstones=collections.defaultdict(list)
for a,b,c in re.findall(r'\{(SPECIES_\w+),\s*(ITEM_\w+),\s*(ITEM_\w+)\}',starterblock):
 for st in [b,c]:
  if st!='ITEM_NONE':starterstones[st].append(a)
for seq,item in enumerate(sorted(reward_inv),1):
 rs=[]
 for ix,desc in enumerate(reward_inv[item],1):
  n=desc.split(':',1)[0];r={'id':f'MEGA-{seq:03d}-R{ix:02d}','map':n,'map_source':f'data/maps/{n}/map.json','inventory_description':desc,'map_access':access(n),'disposition':'KEEP'}
  if 'sparkle at' in desc:
   coords=tuple(map(int,re.search(r'\((\d+), (\d+)\)',desc).groups()));matches=[(i+1,e) for i,e in enumerate(maps[n]['object_events']) if e.get('trainer_sight_or_berry_tree_id')==item and (e['x'],e['y'])==coords];assert len(matches)==1
   oi,e=matches[0];r.update({'kind':'physical_pickup','event_id':f'{n}:object_events:{oi:03d}','event':e,'receipt_flags':[flaginfo(e['flag'])],'source_root':'Common_EventScript_FindItem','geometry':geom(n,e),'entitlement':'pickup:'+e['flag'],'final_behavior':'Keep this exact sparkle/position/item/quantity1. Existing finditem transaction removes the object and records its persistent flag only after successful insertion. Full storage leaves it claimable; return/reload never duplicates it.'})
  elif 'berry trade' in desc:
   fl='FLAG_EC_BERRY_TRADE_'+item.removeprefix('ITEM_');r.update({'kind':'berry_trade','source':'src/mega_stone_rewards.c','source_root':'TradeEmeraldChampionsGardenBerries','npc_root':'Route123_BerryMastersHouse_EventScript_BerryMaster','receipt_flags':[flaginfo(fl)],'entitlement':'trade:'+fl,'price':20,'eligible_berries':['RAZZ','BLUK','NANAB','WEPEAR','PINAP','POMEG','KELPSY','QUALOT','HONDEW','GREPA','TAMATO'],'final_behavior':'Keep one exchange per stone for20 total listed garden berries in any mixture. Validate stone ID, unclaimed flag, berry total and item insertion before debit. Grant exactly1 stone, remove exactly20 across existing stacks, set flag once. Invalid/declined/duplicate/insufficient/full-Bag paths do not debit or mark success.'})
  else:
   lab=re.search(r'\((\w+)\)$',desc)[1];node=labels[lab];body=node['body'];flags=re.findall(r'\bsetflag\s+(FLAG_\w+)',body);ctx=[]
   for eid,ev in world[n]['events'].items():
    if lab==ev.get('root') or lab in ev.get('local_labels',[]):ctx.append({'event_id':eid,'root':ev.get('root'),'kind':ev['kind'],'coordinates':[ev['data'].get('x'),ev['data'].get('y')],'local_id':ev['data'].get('local_id')})
   ent='flags:'+','.join(sorted(flags)) if flags else ('trick:1' if 'Puzzle1' in lab or 'CompletedPuzzle1' in lab else 'trick:8' if 'CompletedPuzzle8' in lab else 'script:'+lab)
   r.update({'kind':'script_grant','source':node['source'],'source_line':node['line'],'source_root':lab,'source_body':body,'physical_roots':ctx,'receipt_flags':[flaginfo(f) for f in flags],'entitlement':ent,'final_behavior':'Preserve the exact one-time grant and its successful-insertion receipt. The source root(s) below supply access and parent-story predicates. First/retry presentations sharing a receipt are one entitlement; rematches cannot grant it again.'})
   if item=='ITEM_ALAKAZITE':r['disposition']='REPAIR';r['final_behavior']='Apply W-TRICK-REWARDS: final tent and Alakazite use independent receipts; retry the missing part from the entrance. Preserve the Granite Cave alternative.';r['entitlement']='trick:8:alakazite'
   if item=='ITEM_STARAPTITE':r['entitlement']='trick:1' if 'TrickHouse' in n else ent
   if item=='ITEM_PIDGEOTITE':r['disposition']='REPAIR';r['final_behavior']+=' W-MEGA-GUIDE replaces false free-Mega-archive dialogue only.'
   if item=='ITEM_KANGASKHANITE':r['prerequisites']=['FLAG_EC_HATCHED_DAYCARE_EGG','Neither legacy Kangaskhanite receipt flag set'];r['final_behavior']+=' Keep the two compatible deposited parents→produced egg→hatched egg requirement. Gift eggs/Togepi do not count. Both legacy receipts are fulfilled together only on successful grant.'
   if item=='ITEM_MAGEARNITE':r['prerequisites']=['FLAG_BADGE08_GET'];r['final_behavior']+=' The stone gift itself does not require Diancie; the separate Magearna prototype does.'
   if item=='ITEM_MEWTWONITE_X':r['prerequisites']=['Leaf trainer defeated','Mewtwo Sign independently requires game clear'];r['final_behavior']+=' Keep Leaf present and reset actor approach positions when item storage fails; do not repeat the battle to collect a pending prize.'
   if item=='ITEM_ALTARIANITE' and n=='LilycoveCity':r['prerequisites']=['SPECIES_ALTARIA in party','FLAG_RECEIVED_WINONA_ALTARIANITE unset','affirmative offer'];r['final_behavior']+=' This shares Winona’s receipt; it can satisfy a pending leader gift but never adds a third entitlement.'
  r['map_access']['earliest_exact_pickup_access']='Not certified by this source-only review. The listed route is the normal story approach; actual earliest tile/NPC access requires its specified field traversal.'
  if r['kind']=='physical_pickup':
   r['map_entry_ports']=maps[n].get('warp_events') or []
   r['map_connections']=maps[n].get('connections') or []
   r['field_obstacle_candidates']=[{'x':o['x'],'y':o['y'],'script':o.get('script'),'flag':o.get('flag')} for o in maps[n]['object_events'] if o.get('script') in ['EventScript_CutTree','EventScript_RockSmash','EventScript_StrengthBoulder']]
  r['acceptance']=[f'Approach the actual {n} reward from its legitimate source entrance and current story state; test field/obstacle prerequisites at the listed tile or NPC, not only arrival on the map.',f'Claim {item}, decline if offered, force applicable insertion failure, revisit/reload and verify only this entitlement changes. For alternate routes, claim in each order and check the explicit receipt grouping.']
  rs.append(r);allroutes.append(r)
 bases=sorted(set(x['base'] for x in bindings[item]));family=[];evos=[];direct={}
 for base in bases:
  ancestors,edges=ancestry(base);family+=ancestors;evos+=edges
  for sp in ancestors:
   rr=[v for v in acq.get(sp,[]) if not(v.get('kind')=='wild' and v.get('method')=='hidden_mons')]
   if rr:direct[sp]=rr
 deps=['MEGA-RING','World/acquisition final map tables; actual licensed methods']
 if item in starterstones:deps+=['INTRO-STONES'];rs.append({'id':f'MEGA-{seq:03d}-STARTER','kind':'conditional_original_starter','source':'src/field_specials.c','source_line':555,'source_root':'GetEmeraldChampionsStarterMegaStone / GiveEmeraldChampionsStarterMegaStones','eligible_original_families':starterstones[item],'receipt_variable':'VAR_STEVEN_STARTER_STONE_DELIVERY','slot_contract':'INTRO-STONES: first starter slots0/1, second slots2/3; one stable bit per slot, ITEM_NONE skipped','prerequisites':['FLAG_DELIVERED_STEVEN_LETTER','FLAG_BADGE02_GET','MEGA_RING obtained','matching immutable original choice'], 'disposition':'REPAIR','final_behavior':'Apply INTRO-STONES’s existing four-slot receipt extension; preserve inventory/PC/held ownership acknowledgment and permanent delivered bits. Never create a second receipt ledger or reissue after disposal. Keep Steven until every populated slot is fulfilled.','acceptance':['Choose each qualifying original pair, including Charmander and a non-Mega family; test every partial Bag/PC insertion and return before Steven disappears.']})
 if item=='ITEM_ALAKAZITE':deps+=['W-TRICK-REWARDS']
 if item=='ITEM_PIDGEOTITE':deps+=['W-MEGA-GUIDE']
 if item=='ITEM_DIANCITE':deps+=['W-WALLACE-ROOT']
 if item=='ITEM_GARDEVOIRITE':deps+=['FORM-03: independent optional NPC gift']
 if item=='ITEM_MAGEARNITE':deps+=['FORM-05','FORM-12']
 if item=='ITEM_FLOETTITE':deps+=['FORM-12']
 if item=='ITEM_TATSUGIRINITE':deps+=['FORM-04','FORM-12']
 if item=='ITEM_ZYGARDITE':deps+=['FORM-08','FORM-12']
 status='REPAIR' if any(r['disposition']=='REPAIR' for r in rs) or item in ['ITEM_TATSUGIRINITE','ITEM_MAGEARNITE'] else 'KEEP'
 rows[item]={'id':f'MEGA-{seq:03d}','stone':item,'disposition':status,'placement_decision':'KEEP every existing world position unless an explicit referenced world/acquisition proposal changes it','individual_assessment':notes[item.removeprefix('ITEM_')],'native_bindings':bindings[item],'matching_base_forms':bases,'optional_evolution_edges':evos,'base_family_acquisition_roots_snapshot':direct,'activation_requirements':['Carry the MEGA_RING in the Bag (production CanMegaEvolve checks Bag possession)','Use an exact listed native base form','Have it hold this exact stone','Choose the Mega action under the current battle rules; other battle eligibility conditions remain authoritative'],'routes':rs,'distinct_world_entitlements':len(set(r['entitlement'] for r in rs if 'entitlement' in r)),'dependencies':deps,'final_specification':'Preserve the native stone/form bindings and all finite world entitlements explicitly listed. Apply only named receipt, clue or exact-form acquisition repairs; no free universal Mega archive, no new power-tier timing gate.','acceptance':[f'Obtain each exact base in {bases} through a real final acquisition path; evolving is optional if a direct base capture exists.',f'Enter a real doubles battle with Ring and {item}, activate each listed Mega form, and verify the intended form/ability plus faint/end-battle reversion.', 'Try the wrong regional/form base and no-Ring case; do not allow a generic family name to bypass the native binding. Source mapping is not runtime proof.']}
# Exact item-description improvements do not change native bindings.
form_labels={
 'ITEM_FLOETTITE':['Enables Eternal','Floette to Mega','Evolve in battle.'],
 'ITEM_ZYGARDITE':['Enables Complete','Zygarde to Mega','Evolve in battle.'],
 'ITEM_RAICHUNITE_X':['Enables Kantonian','Raichu to become','Mega Raichu X.'],
 'ITEM_RAICHUNITE_Y':['Enables Kantonian','Raichu to become','Mega Raichu Y.'],
 'ITEM_SLOWBRONITE':['Enables ordinary','Slowbro to Mega','Evolve in battle.'],
 'ITEM_GRENINJITE':['Enables ordinary','Greninja to Mega','Evolve in battle.'],
}
for it,lines in form_labels.items():
 rows[it]['disposition']='REPAIR'; rows[it]['dependencies'].append('MEGA-FORM-LABELS');rows[it]['item_description_change']={'id':'MEGA-FORM-LABELS','source':'src/data/items.h','anchor':it+'.description','final_lines':lines,'final_behavior':'Replace only this3-line COMPOUND_STRING description. Preserve itemID, price, hold effect, pocket/sort, icon, Mega binding and actualbattle rules.','acceptance':'Render in the real Bag window; confirm complete text fits and exactnamedbaseform agrees with its form-change table.'}

# Numeric flag collisions involving physical stone rewards.
physical_flags=collections.defaultdict(list)
for n,m in maps.items():
 for kind in ['object_events','bg_events']:
  for oi,e in enumerate(m.get(kind) or []):
   if e.get('script')!='Common_EventScript_FindItem' and e.get('type')!='hidden_item':continue
   fl=e.get('flag');it=e.get('item') or e.get('trainer_sight_or_berry_tree_id')
   if fl in [None,0,'0']:continue
   v=flaginfo(fl)
   if v['numeric'] is not None:physical_flags[v['numeric']].append({'map':n,'kind':kind,'index':oi+1,'item':it,'flag':fl})
collisions=[x for x in physical_flags.values() if len(x)>1 and any(v['item'] in reward_inv for v in x)]
counts={'native_stones':len(rows),'item_triggered_mega_bindings':sum(len(r['native_bindings']) for r in rows.values()),'native_mega_flagged_species':sum('isMegaEvolution' in v['flags'] for v in species.values()),'physical_pickups':sum(r['kind']=='physical_pickup' for r in allroutes),'literal_giveitem_sites':sum(r['kind']=='script_grant' for r in allroutes),'berry_trade_rows':sum(r['kind']=='berry_trade' for r in allroutes),'raw_world_routes':len(allroutes),'distinct_world_entitlements':sum(r['distinct_world_entitlements'] for r in rows.values()),'conditional_starter_items':len(starterstones),'distinct_starter_families':len(set(x for a in starterstones.values() for x in a)),'numeric_physical_flag_collisions':len(collisions),'form_description_repairs':len(form_labels),'dispositions':dict(collections.Counter(r['disposition'] for r in rows.values()))}
result={'scope':'All99 native Mega Stone items, all currentphysical/NPC/berry routes, conditional original-starter grant, and legendary reward overview. Separate documentation only.','source':'baseline/source','counts':counts,'stones':rows,'physical_flag_collisions':collisions,'legendary_source_counts':dict(collections.Counter(x['kind'] for x in signs)),'legendary_provider_counts':dict(collections.Counter(x.get('source','field') for x in signs)),'evidence_boundary':'Each stone’s source binding, routes, receipt identity, ecological context and exact-base prerequisites reviewed; local tile neighbors checked where physical. No claim of stateful full traversal or executed Mega activation. Wrong generic-cap timing fallbacks are intentionally excluded.'}
(B/'review/mega-rewards.json').write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n');print(json.dumps(counts,indent=2));print('collisions',json.dumps(collisions));print('routes missing physicalroots',[(it,r['source_root']) for it,d in rows.items() for r in d['routes'] if r['kind']=='script_grant' and not r['physical_roots']])
