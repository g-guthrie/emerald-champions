# Trainer frontier — 2026-09-10 23:01 UTC

This goal turn progressed: Kai authored, natively reviewed and reported.
Prior goal work completed Charlotte; the intervening Mega acknowledgement
was not implementation. Goal remains active.141/516 branches,110/468 records
complete through E0118 Charlotte and Kai. Next E0119 TRAINER_CLAUDE.
Read full goal/currentAGENTS. Single main thread, no subagents;4 CPU workers.
Do not rebuild/release/replace Downloads without request.

No live build. Shared session39208 exited0, ai-shared-through-kai-final.log
60/60. Scratch Kai include removed fromtest/battle/ai/emerald_champions_plans.c.
No engine/runner/new retained test edits. Preexisting runner14+/3− retained.
Canonical516/2339 and6923presetsmatch, scopedgitdiff--checkclean.
Another task ran a release build while Kai's diagnostics ran; it was terminal
before later compiled-input edits. This review did not touch its artifact.

Kai trainer713 actualRoute114scripts.inc203 independentCharlotte714at198.
Native threebadgesNormal all38, nominal offsets-2/-2/-1/-1 unchanged.
Final changes:
Grapploct Brave→Adamant, ExpertBelt→Sitrus, stillLimberPB.
Sealeo Protect→HelpingHand, stillEvioliteThickFatModestSB.
Chesnaught LeechSeed→IronDefense, stillHelmetBulletproofImpishWD.
Greninja LifeOrbProteanTimidSS unchanged.
Strategy NONE→SETUP, Conservative remains; canonicalplan/crackupdated.
Stats HP/Atk/Def/SpA/SpD/Spe:
Grap144/143/85/62/77/48 (oldSpe43,SpA69).
Sealeo152/55/70/106/69/50.
Gren114/79/67/119/70/146.
Ches150/98/146/65/74/65.
Sealeo50 stillbeforeGrap48; noTR, noMega. Species/order/EVs/IV31 unchanged.

Final work/kai-gen9-final-sitrus.log5groups51cases:
29core,15bulky,3Octolock,2abilities,2timing.
Complete48full,26iso,48afterOcto;initial44/22,maxcore45.
Final fullphysical4Grap144/Ches18, Sealeovoluntaryswitch1; baseline3Grap144/
Sealeo148, soactualphysicaldurability/tempo regression acknowledged.
Special3Grap144/Sealeo20. FOScald4at144/80, actualflinch1whileSealeobrines,
HHDrain2. Ground2at102/72 (IcyWinddamagenotprovenspeedpayoff).
TauntGiga3Grap144Sealeodead (baseline4alsodead).
UmbFoulPlay/PachiTbolt5Grap144/Gren103, Sealeofalls4; SitruslosesbeltKOtempo.
DusNightShade/PachiSuperFang5Grap77/Gren35;Sealeo38VOLUNTARYswitch3,
Gren57afterSuperFang thenDarkPulses4/5. NoOctoneededtheseoffenseboards.

IsoGren/Ches:
Physical2at24/106;special3at81/120 ID1BP2onlyonesetup;
FOScald3at71/100, Grenflinch1Hydro2Ice3 remainsWater afteronceProtean;
Ground2GrenfaintChes120;TauntGiga3at81/102 Uturn2noavailablebench;
Umb3GrenfaintChes120;Dus2at92/38 ID1BP2, onefasterthanbaseline.
DusNightShade neverlandsiniso becauseDarkPulseflinches;notlandedpressure.

Six-player defaultset revealedguardscenario:
PlayerMienProtect/XatuProtect1; enemyGrapProtect/SealeoIceBeam.
2actualMienHJKGrap + XatuPsychicSealeo: GrapOctoMien,SealeoIceKOsXatu,
player SEND_OUT right2Marshtomp. Def/SpD5/5, Octopp14.
Grap144−78=66 triggersSitrus+36→102 (threshold73).
3MienHJK→24, MarshtompEarthPowerSealeo; IceBeam+DrainfinishMien,
Grap36Sealeo39, SEND_OUTleft3Grovyle. NoOctorecast.
ExpertBeltcounterfactualGrapdies3, Mien25remains. Clearitemdependentpayoff.
Notfullsixvsfourwin. LimbernativepopupexecutedTWavePP19Grapstatus0.
BulkyYacheRoseradeSludgeBomb2 actualPP8 BulletproofpopupChes150,
ID1BodyPress2;Gren26Roserade61left2.

Fixture corrections, notenginebugs:
DefaultMienhasHJKnotDrain, defaultXatuPsychic/HeatWave notAirSlash.
Initialbadfifthmove requestsfixedagainstactualpresets.
Player replacementsneededafterXatu/MienKO; neverrelaxedturnassert.
OriginalScarfRoserSludgeBombhitSpikyShield, notBulletproofdefect;
bulkyYacheCalm252HP252SpD playerprovidedactualtwohitimmunityproof.
Onlyolder too-longprefix failures shortened toactualfinishedboards.
All final resultsreviewedbystate/PP; nextqueuedAIactionnotexecution.

Kai8/10 mainchatP50744 fileline(notordinal50742),UTC2026-09-10 23:01:10.583.
Verbatimbacklog: “Better Sealeo preservation and less costly reserve positioning
against physical pressure would improve Kai.”
README,individuale0118_kai_gen9_2026_09_10.md,inventory,backlog,batchnotice
synchronized. Don'trepeatcompletion. Noallcampaign/all50claims.

NextClaude currentcanonE0119RAIN/SETUP,Conservative:
HuntailAVWaterVeilJollyPS Waterfall/Crunch/IceFang/SuckerPunch offset0.
GorebyssSashSwiftSwimTimidSS RainDance/MuddyWater/IceBeam/Psychic0.
SeadraScopeLensSniperModestSS FocusEnergy/HydroPump/IceBeam/Protect-1.
RotomWashSitrusLevitateModestSS Tbolt/HydroPump/Wisp/Protect0.
Verifyactualsource/stats/nativeweatherexecution; do notassumeRainoptimalor
FocusEnergyexecutedfromrosterpresence. Preservecurrentdirtyeconomy/legendary/
Center/Mart/SweetScent/headlessedits andMega timing/positioningindividual.
