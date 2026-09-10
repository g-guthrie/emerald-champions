import copy,json,pathlib,re
root=pathlib.Path('.')
owned=set(json.loads((root/'work/wild-habitat-20260908/rating_scopes.json').read_text())['early'])
encounters=json.loads((root/'src/data/wild_encounters.json').read_text())['wild_encounter_groups'][0]['encounters']
base={}
for e in encounters:
    if e['map'] not in owned:continue
    methods={k:[m['species'].removeprefix('SPECIES_') for m in e[k]['mons']] for k in ('land_mons','water_mons','rock_smash_mons') if k in e}
    if 'fishing_mons' in e:
        fish=[m['species'].removeprefix('SPECIES_') for m in e['fishing_mons']['mons']]
        methods.update(old_rod=fish[:2],good_rod=fish[2:5],super_rod=fish[5:])
    base[e['map']]=methods
plan={}
def use(area,note,**methods):
    ident='MAP_'+area
    assert ident in owned
    changed={k:(v.split() if isinstance(v,str) else v) for k,v in methods.items()}
    assert all(k in base[ident] and v!=base[ident][k] for k,v in changed.items()),ident
    plan[ident]={'methods':changed,'note':note}
def repl(area,method,*pairs):
    a=base['MAP_'+area][method].copy()
    for old,new in pairs:
        assert a.count(old)==1,(area,old)
        a[a.index(old)]=new
    return a
def swaps(area,method,*pairs):
    a=base['MAP_'+area][method].copy()
    for first,second in pairs:
        i,j=a.index(first),a.index(second);a[i],a[j]=a[j],a[i]
    return a

use('DEWFORD_MANOR_1F',"Sableye/Hoothoot swap to8%/5%. Glameow takes the6% repeated Misdreavus slot, preserving Misdreavus in Woods3. Jigglypuff takes the7% repeated Mime Jr. slot as a sleep-themed manor resident; Mime Jr. remains before Roxanne on unchanged Route116. Gastly, Sinistea and Galarian Slowpoke stay in their slots.",land_mons='GASTLY DROWZEE SOLOSIS LITWICK RATTATA SABLEYE SLOWPOKE_GALAR JIGGLYPUFF GLAMEOW SHUPPET HOOTHOOT SINISTEA')
use('DEWFORD_TOWN',"Keep Old Rod Mantyke unchanged; the Good Rod now leads with Qwilfish and follows with Chinchou/Staryu. Neighboring Route106 supplies Old Rod Remoraid.",good_rod='QWILFISH CHINCHOU STARYU')
use('GRANITE_CAVE_B1F',"Swap Sableye/Mawile into the13%/12% leading slots and move Zubat/Aron down to10%/11%. Retain Meditite and all other species; original first-floor access remains unchanged.",land_mons=swaps('GRANITE_CAVE_B1F','land_mons',('ZUBAT','SABLEYE'),('ARON','MAWILE')))
use('GRANITE_CAVE_B2F',"Tinkatink/Duraludon/Deino become the13%/12%/11% discoveries through swaps with Sableye/Mawile/Aron. Every original resident remains and no access is delayed.",land_mons=swaps('GRANITE_CAVE_B2F','land_mons',('SABLEYE','TINKATINK'),('MAWILE','DURALUDON'),('ARON','DEINO')))
use('GRANITE_CAVE_STEVENS_ROOM',"Swap Beldum into Aron's13% slot; Aron takes Beldum's8%. Metang, Lucario and unchanged Granite1F Beldum are retained.",land_mons=swaps('GRANITE_CAVE_STEVENS_ROOM','land_mons',('ARON','BELDUM')))
use('PETALBURG_CITY',"Replace marine pond inhabitants with native freshwater families. Old Rod Magikarp/Marill stays unchanged; later rods progress from small pond fish to evolved freshwater partners.",water_mons='MARILL LOTAD POLIWAG WOOPER CORPHISH',good_rod='BARBOACH CORPHISH POLIWAG',super_rod='AZUMARILL LUDICOLO POLITOED WHISCASH GASTRODON')
use('PETALBURG_WOODS',"Latest user correction: Heracross11% and Pineco8% enter first-visit grass via Kricketot/Tarountula slots; Ferroseed5%, Pichu8%, Shroomish13%, Slakoth12%, Caterpie, Buneary and Foongus remain. Kricketot/Tarountula move to orchard grass. Aipom/Cherubi move to Woods2 grass and Exeggcute to Woods3 grass. Rock Smash is supplemental: Pineco35%, Dwebble37%, Ferroseed18%, Heracross10%.",land_mons=repl('PETALBURG_WOODS','land_mons',('KRICKETOT','HERACROSS'),('TAROUNTULA','PINECO')),rock_smash_mons='PINECO DWEBBLE FERROSEED DWEBBLE HERACROSS')
use('PETALBURG_WOODS_2',"Parasect13% and Beedrill11% replace repeated Caterpie/Kakuna. Heracross is already first-visit grass11%, so the former Metapod10% slot instead receives Aipom. Paras8% becomes Cherubi. Keep Weedle, all three monkeys, Applin, Larvesta and Bounsweet. Caterpie stays in Woods1. Kakuna/Metapod/Paras lose direct slots but their families remain through Weedle/Caterpie and Parasect. Remove foliage Exeggcute from Rock Smash now that Woods3 has grass access.",land_mons='PARASECT WEEDLE BEEDRILL AIPOM VENIPEDE CHERUBI PANSAGE PANPOUR PANSEAR APPLIN LARVESTA BOUNSWEET',rock_smash_mons='VENONAT PINECO DWEBBLE DWEBBLE HERACROSS')
use('PETALBURG_WOODS_3',"Exeggcute13% replaces Oddish in actual foliage; Oddish receives orchard grass6%. Keep Bellsprout, Kartana, Croagunk, Murkrow, Goomy, Phantump, Misdreavus and Emolga early. The pond's later rod now develops freshwater residents instead of Lanturn/Octillery.",land_mons=repl('PETALBURG_WOODS_3','land_mons',('ODDISH','EXEGGCUTE')),super_rod='LUDICOLO POLITOED SEISMITOAD CRAWDAUNT FEEBAS')
use('ROUTE101',"Exact approved probability swaps: Mienfoo13% with Zigzagoon10%, Bonsly12% with Poochyena7%, Eevee8% with Sentret5%. All twelve species remain.",land_mons=swaps('ROUTE101','land_mons',('MIENFOO','ZIGZAGOON'),('BONSLY','POOCHYENA'),('EEVEE','SENTRET')))
use('ROUTE102',"Keep every improved land slot including Timburr11% and Pachirisu8%; upgrade the Super Rod Lombre slot to Ludicolo20%.",super_rod=repl('ROUTE102','super_rod',('LOMBRE','LUDICOLO')))
use('ROUTE103',"Replace Blitzle7%/Yamper6% with Corphish/Wingull. Both Electric species move to Route110 grass; Shellos, Growlithe, Shinx, Mareep, Electrike and every other resident remain.",land_mons=repl('ROUTE103','land_mons',('BLITZLE','CORPHISH'),('YAMPER','WINGULL')))
use('ROUTE104',"Roselia10% replaces Ledyba and Croagunk8% replaces Pidove. Preserve Azurill, Budew and Fletchling. Ledyba receives Route105 shore-foliage grass10%; Pidove receives Route106 shore grass10%, replacing repeated Wingull now available earlier on Route103.",land_mons=repl('ROUTE104','land_mons',('LEDYBA','ROSELIA'),('PIDOVE','CROAGUNK')))
use('ROUTE105',"Mantine replaces the leading Super Rod Luvdisc30%. Receive Ledyba10% from Route104 in repeated Wingull's palm-shore foliage slot, retaining Crabrawler, Inkay, Chatot and Cramorant; Wingull now has earlier Route103 grass and plentiful coastal Surf homes.",land_mons=repl('ROUTE105','land_mons',('WINGULL','LEDYBA')),super_rod=repl('ROUTE105','super_rod',('LUVDISC','MANTINE')))
use('ROUTE106',"Barbaracle replaces Super Rod Luvdisc30%; Shuckle replaces Rock Smash Woobat10%; Old Rod Remoraid40% supports Dewford Mantyke. Receive Pidove in repeated Wingull10% grass, retaining all Fighting options and earlier Route103 Wingull.",land_mons=repl('ROUTE106','land_mons',('WINGULL','PIDOVE')),old_rod='MAGIKARP REMORAID',rock_smash_mons=repl('ROUTE106','rock_smash_mons',('WOOBAT','SHUCKLE')),super_rod=repl('ROUTE106','super_rod',('LUVDISC','BARBARACLE')))
use('ROUTE107',"Floatzel replaces Surf Kingler18%. Arrokuda joins Old Rod40%; Good Rod follows Floatzel/Arrokuda/Barraskewda, and Barraskewda leads Super Rod30% with Floatzel as a smaller return. This deliberately develops the fast-swimmer population.",water_mons=repl('ROUTE107','water_mons',('KINGLER','FLOATZEL')),old_rod='MAGIKARP ARROKUDA',good_rod='FLOATZEL ARROKUDA BARRASKEWDA',super_rod='BARRASKEWDA QWILFISH WISHIWASHI CLAWITZER FLOATZEL')
use('ROUTE108',"Receive Lapras from ship B1F in the outside Surf Mantine18% slot; Mantine retains other coasts. Replace Super Rod Luvdisc20% with Jellicent. Keep Dhelmise limited at Surf10% and preserve other wreck-associated residents.",water_mons=repl('ROUTE108','water_mons',('MANTINE','LAPRAS')),super_rod=repl('ROUTE108','super_rod',('LUVDISC','JELLICENT')))
use('ROUTE109',"Upgrade Super Rod Shellder20% to Cloyster. Keep all beach-rock Sandygast/Pincurchin encounters and other methods unchanged.",super_rod=repl('ROUTE109','super_rod',('SHELLDER','CLOYSTER')))
use('ROUTE110',"Receive Blitzle13%/Yamper8% in repeated Electrike/Pachirisu slots; Electrike103 and Pachirisu102 remain earlier. Receive Patrat6% from123 and Watchog5% from120; Stunky goes to121 and Morpeko to115. Tadbulb moves from land5% to Old Rod60%, retaining its pre-Wattson timing; Chinchou stays Old Rod40%. Meowth receives freed land5% from117. Preserve Plusle/Minun, all pollution residents and Roaming Gimmighoul. Lanturn leads stronger fishing and Clawitzer replaces late Luvdisc.",land_mons='BLITZLE GULPIN PLUSLE MINUN VAROOM SHROODLE YAMPER TRUBBISH PATRAT MEOWTH WATCHOG GIMMIGHOUL_ROAMING',old_rod='TADBULB CHINCHOU',good_rod='CHINCHOU HORSEA WISHIWASHI',super_rod='LANTURN DRAGALGE SHARPEDO CLAWITZER FEEBAS')
use('ROUTE111',"Keep the desert land roster intact. Replace Super Rod Octillery30% with Whiscash and its old10% Whiscash slot with Gastrodon.",super_rod='WHISCASH GOLDUCK LOMBRE QUAGSIRE GASTRODON')
use('ROUTE111_RUINS_EXTERIOR',"Xatu becomes21% through the repeated Hawlucha8% slot; Farigiraf swaps into Helioptile12%, Sandy Shocks into Girafarig10%. Sigilyph replaces Skiploom7%; keep Jumpluff and both Rockruff forms. Hawlucha remains earlier on106 and unchanged112. Skiploom loses a direct slot but Hoppip remains104 and Jumpluff here.",land_mons='XATU FARIGIRAF ROCKRUFF ROCKRUFF_OWN_TEMPO SANDY_SHOCKS MEDITITE XATU SIGILYPH HELIOPTILE MINIOR JUMPLUFF GIRAFARIG')
use('ROUTE113',"Replace repeated Mienfoo7% with another Skarmory slot, giving Skarmory19%; preserve earliest Route101 Mienfoo and retain Spinda/Spoink and all other ash residents.",land_mons=repl('ROUTE113','land_mons',('MIENFOO','SKARMORY')))
use('ROUTE114',"Floatzel replaces Super Rod Seaking30%, retaining Dratini. Receive Komala from118 in repeated Nuzleaf11% cliffside tree grass; Seedot remains102 and Nuzleaf evolves from it, while the other established cliff residents stay.",land_mons=repl('ROUTE114','land_mons',('NUZLEAF','KOMALA')),super_rod=repl('ROUTE114','super_rod',('SEAKING','FLOATZEL')))
use('ROUTE115',"Lead with Tangela13%, Sawk12%, Pancham11% and evolved Swellow10%. Receive Furfrou10% and rare three-member Maushold8% from121, plus Morpeko5% from110, in grass by the coastal settlement. Keep Farfetchd, Snubbull, Fomantis, Cleffa and Minior. Jigglypuff moves to manor grass7%, Skwovet to orchard5%; Spritzee remains unchanged Dewford Meadow. Taillow remains101.",land_mons='TANGELA SAWK PANCHAM SWELLOW FURFROU MAUSHOLD_THREE FARFETCHD SNUBBULL FOMANTIS CLEFFA MORPEKO MINIOR')
use('ROUTE117',"Floette8%, Combee8% and Sunkern6% move from rocks into grass; Sunflora goes to orchard11%. Keep Minccino5% for timely Cinccino preparation, plus Tandemaus/Audino/Igglybuff/Petilil/Gossifleur/Ditto/Galarian Farfetchd/Wooloo and both Old Rod species. Meowth moves earlier to110 grass5%, Glameow to manor grass6%; Exeggcute gains Woods3 grass. Rock Smash uses Dwebble/Pineco/Ferroseed/Shuckle/Heracross, with all three requested forest tools already accessible in first Woods grass.",land_mons='TANDEMAUS AUDINO IGGLYBUFF PETILIL GOSSIFLEUR FLOETTE COMBEE DITTO SUNKERN MINCCINO FARFETCHD_GALAR WOOLOO',rock_smash_mons='DWEBBLE PINECO FERROSEED SHUCKLE HERACROSS')
use('ROUTE118',"Emphasize Passimian20%, Manectric12%, Zorua11% and Liepard13% through swaps and duplicate slots. Retain Duraludon5%, Galarian Zigzagoon and all other unique residents. Komala moves to Route114 tree grass11%; Carnivine to Route123 wet orchard grass5%.",land_mons='PASSIMIAN MANECTRIC ZORUA ZIGZAGOON_GALAR LINOONE LIEPARD DEDENNE PASSIMIAN LICKITUNG SQUAWKABILLY_GREEN DURALUDON LIEPARD')
use('ROUTE120',"Gourgeist replaces Watchog10%; Watchog moves to settlement-edge110. Receive Gloom11% from the orchard in repeated Tropius's slot; Tropius retains its earlier unchanged119 habitat. Preserve every Pumpkaboo size, Absol, Morelull, Shiftry and Spiritomb.",land_mons=repl('ROUTE120','land_mons',('WATCHOG','GOURGEIST'),('TROPIUS','GLOOM')))
use('ROUTE121',"Feature Skuntank13%, Mabosstiff12%, Zoroark10%, Mimikyu13% and Poltchageist11%. Receive Stunky10% from110. Elgyem is assigned externally to Origin side chamber3; Furfrou/Maushold Three move to115 grass. Retain Alolan Grimer, Pangoro, Toedscruel and Ekans.",land_mons='SKUNTANK MABOSSTIFF GRIMER_ALOLA ZOROARK STUNKY MIMIKYU PANGORO TOEDSCRUEL POLTCHAGEIST EKANS MIMIKYU POLTCHAGEIST')
use('ROUTE122',"Replace Super Rod Kingler20%/Shellder15% with Jellicent/Basculegion. Keep Surf Frillish and Super Rod Dhelmise10% around Mt. Pyre.",super_rod=repl('ROUTE122','super_rod',('KINGLER','JELLICENT'),('SHELLDER','BASCULEGION')))
use('ROUTE123',"Lead with Applin13%/Smoliv12% and Sunflora11% received from117. Keep Karrablast10%/Shelmet8%, promote Oranguru8%, and retain Flamigo7%. Receive Kricketot10%/Tarountula5%/Oddish6% from Woods, Skwovet5% from115 and Carnivine5% from118 as orchard/garden residents. Patrat goes to110 with Watchog; Gloom goes to120. Stantler/Kecleon retain other direct homes; Linoone remains118. Mightyena loses its only direct slot, but Poochyena remains101 and evolves normally.",land_mons='APPLIN SMOLIV SUNFLORA KRICKETOT KARRABLAST SHELMET ORANGURU FLAMIGO ODDISH TAROUNTULA SKWOVET CARNIVINE')
use('RUSTURF_TUNNEL',"Replace Geodude12% with another Whismur slot for25%. Keep Machop, Drilbur, Bagon, Larvitar and Chingling in their exact slots. Geodude remains unchanged Granite1F, Route106 and existing Rock Smash homes.",land_mons=repl('RUSTURF_TUNNEL','land_mons',('GEODUDE','WHISMUR')))
use('SEASPRAY_CAVE',"Replace Surf Wailord12% with Lanturn. The rods develop Chinchou/Relicanth and sheltered rock-pool shellfish instead of generic beach filler; retain all land Tynamo and both Stunfisk forms unchanged.",water_mons=repl('SEASPRAY_CAVE','water_mons',('WAILORD','LANTURN')),old_rod='MAGIKARP CHINCHOU',good_rod='CHINCHOU RELICANTH KRABBY',super_rod='LANTURN RELICANTH CLOYSTER CLAMPERL MAREANIE')

expected={'MAP_'+line.split('|',1)[0] for line in (root/'work/wild-habitat-20260908/personal_recommendations.txt').read_text().splitlines() if 'MAP_'+line.split('|',1)[0] in owned}
assert set(plan)==expected,(expected-set(plan),set(plan)-expected)
assert len(plan)==32
species=set(re.findall(r'\bSPECIES_([A-Z0-9_]+)\s*=',(root/'include/constants/species.h').read_text()))
lengths={'land_mons':12,'water_mons':5,'rock_smash_mons':5,'old_rod':2,'good_rod':3,'super_rod':5}
for ident,entry in plan.items():
    for method,mons in entry['methods'].items():
        assert method in base[ident]
        assert len(mons)==lengths[method],(ident,method,len(mons))
        assert all(mon in species and mon!='NONE' for mon in mons),(ident,method,set(mons)-species)
# Normalize prose spacing without changing exact species IDs or array data.
for entry in plan.values():
    entry['note']=re.sub(r'(?<=[a-zA-Z%])(?=\d)', ' ',entry['note'])
path=root/'work/wild-habitat-20260908/approved_early_plan.json'
path.write_text(json.dumps(plan,indent=2,ensure_ascii=False)+'\n')
print('Wrote',len(plan),'changed maps;',sum(len(p['methods']) for p in plan.values()),'changed methods')
print('Protected unchanged:',sorted(owned-set(plan)))
# Compare active-map species coverage, including updates to changed methods only.
active=json.loads((root/'work/wild-habitat-20260908/current-area-rosters.json').read_text())
before={s for e in active for mons in e['methods'].values() for s in mons}
after=set()
for e in active:
    for method,mons in e['methods'].items():
        after.update(plan.get(e['id'],{}).get('methods',{}).get(method,mons))
print('Direct losses pending global integration:',sorted(before-after))
print('New direct species:',sorted(after-before))
