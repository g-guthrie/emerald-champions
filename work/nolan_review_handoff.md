# Trainer frontier — 2026-09-10 22:34 UTC

Previous goal turn: progress (Sophie). Current turn: progress (Nolan complete,
2 canonical set improvements, native main strategies, docs/provenance closed).
139/516 branches,109/468 records complete through E0117 Nolan. Next E0118
TRAINER_CHARLOTTE; neighboring TRAINER_KAI is separate. Neither reviewed yet.
Read full objective and current AGENTS before continuing. Single main thread;
bounded4CPU native workers allowed. No subagents/release/Downloads/commit/push.

No live build. session94806 exited0; work/ai-shared-through-nolan-final.log
60/60. Scratch Nolan include removed from test/battle/ai/emerald_champions_plans.c.
Temporary NOLAN_SCORE tracing removed from test/test_runner_battle.c; preserve
its preexisting14+/3- diff. No engine or permanent runner/test changes.
Canonical516/2339 and6923presets match, scopedgitdiff--check clean.

Nolan trainer342 Route114script178,threebadges allnative38nominal40.
Final Dondozo Leftovers Unaware Adamant252HP4Def212SpD40Spe; Speed43→47,
Commander86→94 beatingPachi93,SpD90→86. The44SpeedcandidatehadSpD85same47Spe
and was improved to40. Stats197/101/104/59/86/47.
Palafin MysticWater→Sitrus, sameJollyPS;Zero136/93/71/51/63/127,
Hero136/162/90/87/82/127. TatsuSashCommanderTimidSS111/48/62/131/88/113;
SlowbroSitrusRegeneratorBoldWD155/65/136/92/77/39 unchanged. All moves/order/
species/otherEVs/levels/ALLY_COMBO COMMANDER tactic/Conservative unchanged.
CurlyOrderUp addsAtk; Commander+2fivecombatstats,hiddenTatsunotsecondattacker.

work/nolan-gen9-final-verified.log3groups70cases:61core,6Haze,3timing.
Full core outcomesphysical3Dozo57/Tatsu111;special3at44/111;FOGiga5at6/111;
GrassElectric5Slow155/Tatsu111Dozodies4;Taunt4at66/111. NativeCommander94,
OrderUpAtk9/10+ anddamagingpayoff. Hazeactuallylands1andclearsboosts,
Dozodies3;PalentryFlip4→Slowbro/Tatsufinish6at113/111.

IsoPal/Slowwithnoreserve deliberatelycannotHero. Physicalfinish3at19/119;
special4at19/90. FOGigaloses5Lombre128;GrassElectricloses3Lombre135Pachi76;
TauntGigaunfinished5Pal60vsLombre135Slowdead. Sitrus/SlackOffactual.
PostDozostatehasPal/TatsuactiveSlowbenchDozofainted(notfakeHero,realsets):
physicalFlip1plusTatsuswitchSlow,PalHero reentry2afterTatsufaints,HeroWave3
finish115/101 (oldMysticHero died3). SpecialreduceddamageleavesXatulive1,
Paldead2,Slow/Tatsuwin4at155/36. FOHero66+Slow155win5;TauntTatsuHelpingHand2,
Heroreentry3win4at155/136. GrassElectric5unfinishedSlow52/Tatsu61vsLombre135,
IcyWind4thenDragon5;SlowsustainedSlackOffnotwinningpayoff. Regeneratorheal
notindependentlyverified. AllWateridentityretained; laterCommanderE0394Sylvia.

DecisiontimingcompleteCommander13,iso33,livebench53(~0.88s);initial11/30/50,
postHaze10. No unmeasuredallstate/ROMRAMclaim.

Diagnosticlesson: runnerSetAiActionToPass queuesimplicitrightAIpasses even
whileCommanderhidden. CheckChosenMove whenTatsureappears wrongly advances
lastActionTurn tofutureunexecutedselection:3TURNspecified4ran. Temporary
NolanClearCommanderPasses at endWHEN forCommanderstarts asserts entries are
onlyimplicitpass thenunsetsactionSet. Playerrecordsretainturnboundary;
explicitexpectations/nativegameuntouched. This is not enginefixor3→4relaxation.

Weakno-benchFlipTurnrepetitionintoGigaDrainLombre remains. CountUsablePartyMons
correctlyexcludesactivepartners=0,ShouldPivotdoesnotawardHero. Actual
AI_ScoreMoveAgainstTarget givesFlip95damage10vsIcePunch100damage26.
ThefirstfinalScoretracezeroswasnotactivepairscore,notbugproof. Exactpair
selectioncauseunresolved; no sharedbranches added. Don'tclaimfixed. Temporary
work/nolan-gen9-rescored.log has evidence; originalfinal.log onlycompilebitfield
EXPECT_EQerror (fixedEXPECTboolean in final-verified), notsuccessfulnative run.

Nolan8/10reportP50249(fileline,notordinal50247),UTC2026-09-10 22:34:02.286.
Verbatim: “Cleaner no-bench move choices and stronger Grass/Electric
counterplay would improve Nolan.” README,individuale0117_nolan_gen9_2026_09_10.md,
inventorytheme14,backlog,batchnotice,pipeline diagnosticnotes synchronized.
Alreadyreportedmainchat; don'tduplicate. Wholegoalall516/all50unfinished.

NextCharlottecanonicalroughly1400:ShadowTagGothoritaEvioliteQuietSB TR/Psychic/
FakeTears/Protect;KomalaLifeOrbComatoseBravePB SD/DoubleEdge/KnockOff/SuckerPunch;
BouffalantAVSoundproofBravePB HeadCharge/HighHorsepower/CloseCombat/WildCharge;
ArmarougeWeaknessPolicyFlashFireQuietSB ArmorCannon/Psychic/EnergyBall/Protect.
TR/SETUPplan,ai empty,classcasual. CurrentcrackprosewronglynamesretiredSealeo/
Grapploct; fixduringreviewagainstactualsource. Don'tmixKaiindependentteam.
Preserveunrelateddirtyeconomy/legendary/Center/Mart/SweetScent/headlesswork,
Mega placement/timingindividual,GlaciaWalrein,universalIV31/exactEVs/berryrule.
Latest user ROM remains Sep9hazardsDownloads,notnewauthoringfrontier.
