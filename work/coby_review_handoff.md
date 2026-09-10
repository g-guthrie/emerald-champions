# Trainer frontier — 2026-09-10 22:04 UTC

Previous goal turn progressed: Dillon complete. This turn progressed: Coby
complete, exactEV tweak, native main strategies, clean shared60/60, docs,
abilityinventory and exactchatprovenance updated. Current137/516 branches,
107/468 encounter records plus partialE0116(Cobyonly). Sophie separate next.

No live build: sharedsession36783 exited0, work/ai-shared-through-coby-final.log
60/60. Scratch Coby include removed from test/battle/ai/emerald_champions_plans.c.
Temporary EC_TEMP_MESSAGE line in test/test_runner_battle.c removed; preserve
that file's preexisting14/3diff,notours. No sharedAI/retainedtest/releaseROM/
Downloads/commit/push/subagent changes. Canonical516/2339,6923presets match.

Coby TRAINER_COBY709 Route113script122,allnative38 at3badgescap40. Sophie
script117 separateID,notjointbattle. CobychangesonlyCorvisquireImpish
252HP252Def4SpD→252HP244Def4SpD8Speed. Def90→89,Speed75→76,TW150→152
passesplayerDug151; Rockcaseend48/62 instead16/34, ToucSitrus preserved.
Allspecies/order/moves/itemsotherEVs unchanged. TAILWINDplan retained.
Stats HP/Atk/Def/SpA/SpD/Speed:Corvi135/67/89/44/58/76;Touc120/144/73/65/
73/86;Honch136/135/56/86/56/103;Kilo113/62/62/120/62/148. Eviolite,Sitrus,
ScopeLens,Magnet respectively. NoMega. NativeBeakBlastpriority−3,nottailwindfix.

work/coby-gen9-final.log3groups38cases:26core,10reservecases,2timing.
Complete60full/21isolated,initial60/19 (~1.00s,max72).
Fullcore outcomes:physical3Corvi70/Kilo113,Toucfalls2;special4Corvi76/
Honch136,Toucfalls3;FO3at51/69;Rock2at48/62;Taunt2at86/120.
Isolatedphysical2at48/113;special4at136/77;FO2at67/55;Rock2at60/45;
Taunt2at92/113. TWactualPP14 andspeeds152/172full,206Honchasserted;
ordinarymultihit/BraveBirdattacksaftersetup. EndingTWtimer3after2turnsmeans
endbattlebeforedecrement,notrecast (PPstays14).

Important diagnostic: initialUmbreonSnarlandDusclopsNightShadeneveracted!
KilowattrelThunderboltparalyzesthem, native runnerdefaultssecondaryTRUE and
fullparalysislastweightedoutcomeeveryturn. SourceRandomWeightedArrayDefaultValue
test/test_runner.c1129,paralyzecancelerbattle_move_resolution.c460.
Trace work/coby-reserve-message-trace.logconfirmed. NotCompetitiveenginebug,
notfailedspreadhitorfreeze. ChangedonlyUmbreonfixtureLeftovers→Lum. FinalSnarl1
actuallyspendsPP14, givesKilo net+1SpA(stage7),boostedTbolt2/3execute.
Umbreparalyzed2nofurtherSnarl. Kilodies3,Pachipersists,Honchwins5at34.
DusclopsremainsEviolite/paralyzed; HonchProtect1,NightSlash2actualcritmessage,
Dusdies2,win5Honch136/Kilo41. NotdeliveredNightShadepressureclaim.
ScopeLens+SuperLuck+NightSlashstage3nativeguaranteedcritabsentblockers;
othermovesnotguaranteed. ScenecheckscriticalwithoutforcingAI/RNG.

Temporary reusablework/ec_review_common.inc nowalso prints REVIEW_PLAYER:
species,status,last executedmove,PP. Useitgoingforwardtointerpretproposed
pressurecorrectly. Pipelineworkersdocrecordslesson. Oldcontrolledtracesarenot
winrates/exhaustivepressureevidence;donotreflexivelyreopenallcampaign.

Coby8.5/10mainchatP49610UTC2026-09-10 22:04:18.120. Verbatimbacklog:
“Better Rock/Electric resilience and cleaner preservation of a boosted
Kilowattrel would improve Coby.” README,individualdoc
e0116_coby_gen9_2026_09_10.md,inventorytheme44,backlogandbatchnoticeclosed.

NextSophiecanonical1378,generatedparty25927,Route113script117. NONEstrategy.
ForretressRedCardSturdyBraveWD Spikes/VoltSwitch/GyroBall/BodyPress;
DelphoxLifeOrbBlazeTimidSS HeatWave/Psychic/DazzlingGleam/Protect;
SawsMuscleBandSereneGraceJollyPS HornLeech/DoubleEdge/Protect/HelpingHand;
ParasectSashDrySkinCareful252HP4Def252SpD Spore/WideGuard/XScissor/Protect.
No currentnativeSophie work. CheckactualsturdyRedCard/pivot/hazards/support,
dryskinFirecounter,EVs,distribution,latency. Fourmons cap40expected38verify.
AfterSophieE0116fullyclosed;nextE0117readactualcanonbeforeassumingID.

ReadfullgoalobjectiveandcurrentAGENTSoncontinuation. Singlemainthread,
boundedCPUworkers,perbattlechatthenexactrolloutlineUTC(notordinal).
PreserveMegasneednotlead/immediatetransform,GlaciaWalrein,universalIV31,
exactEVs/berryconvenience,unrelatedeconomy/legendary/Center/Mart changes.
