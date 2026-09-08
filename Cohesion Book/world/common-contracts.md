# Shared world contracts

These contracts are applied to the exact instances in the per-map ledger. Each map still carries its own coordinates, state, neighbors and local disposition. Reuse is explicit; no claim is made that static source inspection is a runtime traversal.

<a id="w-c-pickup"></a>

## W-C-PICKUP — Physical item rewards

**KEEP.** Common_EventScript_FindItem obtains item/quantity from the event template and delegates to the existing finditem transaction. Keep item, flag, coordinates and quantity as cataloged unless the acquisition volume explicitly changes them. A failed insertion must leave the reward available. Item-ball appearance and pickup identity must agree.

Source: [`data/scripts/item_ball_scripts.inc`](../baseline/source/data/scripts/item_ball_scripts.inc).

<a id="w-c-hidden"></a>

## W-C-HIDDEN — Hidden items

**KEEP.** Keep exact coordinate, elevation, item and persistent flag. Finding a hidden item commits only on insertion success; revisit preserves ownership and no duplicate reward. This optional search is not a substitute for an advertised acquisition route.

Source: [`data/scripts/obtain_item.inc`](../baseline/source/data/scripts/obtain_item.inc).

<a id="w-c-berry"></a>

## W-C-BERRY — Berry trees

**KEEP.** Retain tree ID, planting/harvest/watering state and local berry species. Track regrowth separately from one-time Mega exchanges. Existing Berry Master accepts the listed garden mixture; no new berry grind or free Mega archive.

Source: [`data/scripts/berry_tree.inc`](../baseline/source/data/scripts/berry_tree.inc).

<a id="w-c-field"></a>

## W-C-FIELD — Field obstacles

**KEEP.** Use existing HM-license plus badge plus non-Egg compatible-party capability model. A known move is preferred; the fallback does not occupy a battle move slot. Preserve obstacle positions and local puzzle state. Flash/tide/Regi puzzle-specific scripts retain their own explicit rules. W-FIELD-SURF fixes only the deletion capability mismatch.

Source: [`src/field_move.c`](../baseline/source/src/field_move.c).

<a id="w-c-center"></a>

## W-C-CENTER — Healing and first-visit tools

**REVISE.** Retain respawn routing, healing, Vial refill and missing-tool recovery. W-CENTER-FIRST-VISIT finishes the first handoff by offering the ordinary heal flow. Every one of the17 native Centers/League Center supplies the existing free specialist and vendor.

Source: [`data/scripts/pkmn_center_nurse.inc`](../baseline/source/data/scripts/pkmn_center_nurse.inc).

<a id="w-c-tutor"></a>

## W-C-TUTOR — Free preparation

**KEEP.** Retain legal move editing, complete battle sets, Nature, Stat Points, name/delete services, cancel paths and Mega-stone checks. Every service applies immediately without introducing a progression curriculum. W-FREE-SERVICE-COHESION removes live paid duplicates.

Source: [`data/scripts/emerald_champions.inc`](../baseline/source/data/scripts/emerald_champions.inc).

<a id="w-c-vendor"></a>

## W-C-VENDOR — Battle supplies and evolution archive

**KEEP.** Retain free held items and the Ring-gated free evolution archive. No universal Mega Stone archive. Selection cursor/back/cancel and item grants remain one shared implementation.

Source: [`data/scripts/emerald_champions.inc`](../baseline/source/data/scripts/emerald_champions.inc).

<a id="w-c-legend"></a>

## W-C-LEGEND — Legendary and static encounters

**KEEP.** Retain deliberate landmarks, optional Devon guidance, actual badge/story/species gates, capture/permanent reward state and re-entry retry. Ordinary wild and legendary capture battles remain singles; scripted INTRO-01 rescue is separately specified. WILD-ENGINE-01 prevents Sweet Scent from starting a stale battle after a rejected normal draw. W-SPIRITOMB-01 removes only that alternate encounter’s unexplained species gate.

Source: [`src/legendary_signs.c`](../baseline/source/src/legendary_signs.c).

<a id="w-c-trainer"></a>

## W-C-TRAINER — Authored trainer interaction

**REVISE.** Preserve object, script identity, line of sight, defeated/rematch flags, dialogue flow and story placement unless a named world change applies. Final parties, moves and tactical prose are owned by the individual battle volumes; all native trainer battles are doubles and standard teams have at least4. DIFF-01 supplies the Medium floor at live cap minus2.

Source: [`data/scripts/trainer_battle.inc`](../baseline/source/data/scripts/trainer_battle.inc).

<a id="w-c-flavor"></a>

## W-C-FLAVOR — Local NPC and environmental prose

**KEEP.** The actual referenced text is reproduced in the dialogue ledger. Preserve examined local personality, biome history, directions and harmless optional social interactions unless a listed text replacement repairs a concrete mismatch. Do not turn every resident into a battle lecturer.

Source: [`data/maps/`](../baseline/source/data/maps/).

<a id="w-c-story"></a>

## W-C-STORY — Stateful story interaction

**KEEP.** Preserve the map’s specific flags/vars, actor choreography and success handoffs reproduced by source references. Required battles do not commit defeated/reward/story-success state on loss. Every cited acceptance route must still be tested in the actual game; resolved script references alone are not traversal proof.

Source: [`data/maps/`](../baseline/source/data/maps/).

<a id="w-c-warp"></a>

## W-C-WARP — Warp and connection

**KEEP.** Keep exact source location, elevation, destination map and destination warp/offset. Refer to map-specific story/field gates and active layout variants. The structural gate resolved all1402 warps and152 connections; actual state-dependent traversal remains implementation evidence.

Source: [`data/maps/`](../baseline/source/data/maps/).

<a id="w-c-coord"></a>

## W-C-COORD — Coordinate trigger

**KEEP.** Keep exact location, state variable and required value. Shared entrance columns must run one scene, not repeated scenes; success advances only the intended state. Weather triggers keep local biome atmosphere. Every change to a triggering scene must include its sibling coordinates.

Source: [`data/maps/`](../baseline/source/data/maps/).

<a id="w-c-background"></a>

## W-C-BACKGROUND — Sign and background interaction

**KEEP.** Preserve exact position/facing direction and source root. Route roster appendices follow GUIDE-01 and must show only enabled actual methods. Proposed clue and rule-board replacements identify exact labels; inert guide prose is never counted as player-facing guidance.

Source: [`data/maps/`](../baseline/source/data/maps/).

<a id="w-c-map_callback"></a>

## W-C-MAP_CALLBACK — Map-load/frame/resume callbacks

**KEEP.** Keep callback type and entry label. Review map flags, dynamic layout, object visibility and handoff state together. For Circuit/Tent/Hill conversions use their explicit origin/restore contracts; do not let retained legacy callbacks launch unsupported modes.

Source: [`data/maps/`](../baseline/source/data/maps/).

<a id="w-c-passive"></a>

## W-C-PASSIVE — Passive actor or prop

**KEEP.** An object with no script may be a cutscene actor, staged Pokémon, temporary item-ball prop or visual resident. Keep graphics, ID, movement, flag and position. It is not an advertised interactive reward merely because it resembles one; scene owners control its lifetime.

Source: [`data/maps/`](../baseline/source/data/maps/).

<a id="w-c-link"></a>

## W-C-LINK — External battle and record support

**REVISE.** Preserve trading, record mixing, link minigame and imported data. LINK-01 owns the exact wired/wireless doubles-only entry and native recorded-owner challenge retirement. Retire unsupported native e-Reader battles. LINK-01 restricts wired/wireless battle entry to the exact Double/Multi menus and retires native recorded Secret Base owner challenges before save or daily-state mutation. Preserve imported profiles and decorations; never manufacture missing opponents or delete records.

Source: [`data/scripts/cable_club.inc`](../baseline/source/data/scripts/cable_club.inc).

<a id="w-c-facility"></a>

## W-C-FACILITY — Native competition interfaces

**REVISE.** FAC-01 supplies local six-Pokémon tent exhibitions; FAC-02 preserves Hill Time Attack with coherent generated doubles teams; Frontier desks use central Circuit. World changes reconcile guides, records, duplicate paid services and obsolete wagers. Legacy challenge rooms are not reactivated by this book.

Source: [`src/champions_circuit.c`](../baseline/source/src/champions_circuit.c).

<a id="w-c-contest"></a>

## W-C-CONTEST — Contests and social minigames

**KEEP.** Preserve optional noncombat contests, paintings, ribbons, Berry Blender, style changes and spectator interactions. These remain worthwhile Hoenn culture without becoming battle-preparation tolls. Their registered staging maps are not trainer-format exceptions.

Source: [`data/scripts/contest_hall.inc`](../baseline/source/data/scripts/contest_hall.inc).

<a id="w-c-daycare"></a>

## W-C-DAYCARE — Daycare and gifts

**KEEP.** Retain two-compatible-parent breeding, short waits, Togepi gift, explicit gift-egg exclusion and one qualifying hatched egg for Kangaskhanite. Adult means breeding-eligible, not necessarily fully evolved. Preserve direct early babies and free preparation.

Source: [`data/scripts/day_care.inc`](../baseline/source/data/scripts/day_care.inc).

<a id="w-c-travel"></a>

## W-C-TRAVEL — Transport

**KEEP.** Keep Briney’s letter/delivery itinerary, return options, Cable Car, tides, public ferry passes and explicit route directions. Test source and destination state after cancel, success and later revisit. Movement convenience must not bypass unearned HM/badge access.

Source: [`data/maps/`](../baseline/source/data/maps/).

## Shared-root instance index

| Root | Source | Instances | Contract |
|---|---|---:|---|
| `BattleColosseum_2P_EventScript_Attendant` | [data/scripts/cable_club.inc:772](../baseline/source/data/scripts/cable_club.inc#L772) | 1 | W-C-LINK |
| `BattleFrontier_BattleDomeLobby_OnWarp` | [data/maps/BattleFrontier_BattleDomeLobby/scripts.inc:11](../baseline/source/data/maps/BattleFrontier_BattleDomeLobby/scripts.inc#L11) | 1 | W-C-STORY |
| `BattleFrontier_BattlePikeRoomNormal_EventScript_Exit` | [data/scripts/battle_pike.inc:153](../baseline/source/data/scripts/battle_pike.inc#L153) | 1 | W-C-FACILITY |
| `BattleFrontier_BattlePikeRoomNormal_EventScript_NoTurningBack` | [data/scripts/battle_pike.inc:144](../baseline/source/data/scripts/battle_pike.inc#L144) | 3 | W-C-FACILITY |
| `BattleFrontier_BattlePikeRoomNormal_EventScript_SetEnteredRoom` | [data/scripts/battle_pike.inc:139](../baseline/source/data/scripts/battle_pike.inc#L139) | 3 | W-C-FACILITY |
| `BattleFrontier_BattlePikeRoomWildMons_EventScript_Exit` | [data/scripts/battle_pike.inc:191](../baseline/source/data/scripts/battle_pike.inc#L191) | 1 | W-C-FACILITY |
| `BattleFrontier_BattlePikeRoomWildMons_EventScript_NoTurningBack` | [data/scripts/battle_pike.inc:213](../baseline/source/data/scripts/battle_pike.inc#L213) | 3 | W-C-FACILITY |
| `BattleFrontier_BattlePikeRoomWildMons_EventScript_SetEnteredRoom` | [data/scripts/battle_pike.inc:208](../baseline/source/data/scripts/battle_pike.inc#L208) | 3 | W-C-FACILITY |
| `BattleFrontier_BattlePikeRoom_OnResume` | [data/scripts/battle_pike.inc:228](../baseline/source/data/scripts/battle_pike.inc#L228) | 2 | W-C-FACILITY |
| `BattleFrontier_BattlePikeRoom_OnTransition` | [data/scripts/battle_pike.inc:5](../baseline/source/data/scripts/battle_pike.inc#L5) | 1 | W-C-FACILITY |
| `BattleFrontier_BattlePikeRoom_OnWarp` | [data/scripts/battle_pike.inc:44](../baseline/source/data/scripts/battle_pike.inc#L44) | 1 | W-C-FACILITY |
| `BattleFrontier_BattlePikeThreePathRoom_EventScript_CenterRoomWarp` | [data/scripts/battle_pike.inc:77](../baseline/source/data/scripts/battle_pike.inc#L77) | 1 | W-C-FACILITY |
| `BattleFrontier_BattlePikeThreePathRoom_EventScript_LeftRoomWarp` | [data/scripts/battle_pike.inc:72](../baseline/source/data/scripts/battle_pike.inc#L72) | 1 | W-C-FACILITY |
| `BattleFrontier_BattlePikeThreePathRoom_EventScript_NoTurningBack` | [data/scripts/battle_pike.inc:130](../baseline/source/data/scripts/battle_pike.inc#L130) | 3 | W-C-FACILITY |
| `BattleFrontier_BattlePikeThreePathRoom_EventScript_RightRoomWarp` | [data/scripts/battle_pike.inc:82](../baseline/source/data/scripts/battle_pike.inc#L82) | 1 | W-C-FACILITY |
| `BattleFrontier_BattlePikeThreePathRoom_EventScript_SetEnteredRoom` | [data/scripts/battle_pike.inc:125](../baseline/source/data/scripts/battle_pike.inc#L125) | 3 | W-C-FACILITY |
| `BattleFrontier_BattleTowerLobby_EventScript_Apprentice` | [data/scripts/apprentice.inc:1](../baseline/source/data/scripts/apprentice.inc#L1) | 1 | W-C-LINK |
| `BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit` | [data/maps/BattleFrontier_BattleTowerLobby/scripts.inc:445](../baseline/source/data/maps/BattleFrontier_BattleTowerLobby/scripts.inc#L445) | 9 | W-C-STORY |
| `BattleFrontier_BattleTowerLobby_EventScript_Reporter` | [data/scripts/interview.inc:236](../baseline/source/data/scripts/interview.inc#L236) | 1 | W-C-FLAVOR |
| `BattleFrontier_OutsideWest_EventScript_Man3` | [data/maps/BattleFrontier_BattleDomeLobby/scripts.inc:388](../baseline/source/data/maps/BattleFrontier_BattleDomeLobby/scripts.inc#L388) | 1 | W-C-STORY |
| `BattlePyramid_FindItemBall` | [data/maps/BattleFrontier_BattlePyramidFloor/scripts.inc:115](../baseline/source/data/maps/BattleFrontier_BattlePyramidFloor/scripts.inc#L115) | 48 | W-C-STORY |
| `BattlePyramid_TrainerBattle` | [data/maps/BattleFrontier_BattlePyramidFloor/scripts.inc:106](../baseline/source/data/maps/BattleFrontier_BattlePyramidFloor/scripts.inc#L106) | 64 | W-C-STORY |
| `BerryBlender_EventScript_BerryBlender1` | [data/scripts/berry_blender.inc:237](../baseline/source/data/scripts/berry_blender.inc#L237) | 1 | W-C-CONTEST |
| `BerryBlender_EventScript_BerryBlender2` | [data/scripts/berry_blender.inc:316](../baseline/source/data/scripts/berry_blender.inc#L316) | 1 | W-C-CONTEST |
| `BerryBlender_EventScript_BerryBlender3` | [data/scripts/berry_blender.inc:374](../baseline/source/data/scripts/berry_blender.inc#L374) | 1 | W-C-CONTEST |
| `BerryBlender_EventScript_BerryBlenderLink` | [data/scripts/berry_blender.inc:537](../baseline/source/data/scripts/berry_blender.inc#L537) | 1 | W-C-CONTEST |
| `BerryBlender_EventScript_Blender1ExpertM` | [data/scripts/berry_blender.inc:491](../baseline/source/data/scripts/berry_blender.inc#L491) | 1 | W-C-CONTEST |
| `BerryBlender_EventScript_Blender2Man` | [data/scripts/berry_blender.inc:479](../baseline/source/data/scripts/berry_blender.inc#L479) | 1 | W-C-CONTEST |
| `BerryBlender_EventScript_Blender2Twin` | [data/scripts/berry_blender.inc:487](../baseline/source/data/scripts/berry_blender.inc#L487) | 1 | W-C-CONTEST |
| `BerryBlender_EventScript_Blender3PokefanF` | [data/scripts/berry_blender.inc:483](../baseline/source/data/scripts/berry_blender.inc#L483) | 1 | W-C-CONTEST |
| `BerryTreeScript` | [data/scripts/berry_tree.inc:1](../baseline/source/data/scripts/berry_tree.inc#L1) | 88 | W-C-BERRY |
| `CableClub_EventScript_MysteryGiftMan` | [data/scripts/cable_club.inc:21](../baseline/source/data/scripts/cable_club.inc#L21) | 17 | W-C-LINK |
| `CableClub_OnFrame` | [data/scripts/cable_club.inc:103](../baseline/source/data/scripts/cable_club.inc#L103) | 17 | W-C-LINK |
| `CableClub_OnLoad` | [data/scripts/cable_club.inc:68](../baseline/source/data/scripts/cable_club.inc#L68) | 18 | W-C-LINK |
| `CableClub_OnResume` | [data/scripts/cable_club.inc:1226](../baseline/source/data/scripts/cable_club.inc#L1226) | 17 | W-C-LINK |
| `CableClub_OnTransition` | [data/scripts/cable_club.inc:1](../baseline/source/data/scripts/cable_club.inc#L1) | 17 | W-C-LINK |
| `CableClub_OnWarp` | [data/scripts/cable_club.inc:51](../baseline/source/data/scripts/cable_club.inc#L51) | 17 | W-C-LINK |
| `CaveHole_CheckFallDownHole` | [data/scripts/cave_hole.inc:1](../baseline/source/data/scripts/cave_hole.inc#L1) | 6 | W-C-FIELD |
| `CaveHole_FixCrackedGround` | [data/scripts/cave_hole.inc:5](../baseline/source/data/scripts/cave_hole.inc#L5) | 4 | W-C-FIELD |
| `Common_EventScript_DirectCornerAttendant` | [data/event_scripts.s:1688](../baseline/source/data/event_scripts.s#L1688) | 17 | W-C-STORY |
| `Common_EventScript_EmeraldChampionsBattleVendor` | [data/scripts/emerald_champions.inc:384](../baseline/source/data/scripts/emerald_champions.inc#L384) | 18 | W-C-TUTOR |
| `Common_EventScript_EmeraldChampionsMoveTutor` | [data/scripts/emerald_champions.inc:1](../baseline/source/data/scripts/emerald_champions.inc#L1) | 17 | W-C-TUTOR |
| `Common_EventScript_FindItem` | [data/scripts/item_ball_scripts.inc:1](../baseline/source/data/scripts/item_ball_scripts.inc#L1) | 236 | W-C-PICKUP |
| `Common_EventScript_LegendaryLandmark` | [data/event_scripts.s:1575](../baseline/source/data/event_scripts.s#L1575) | 34 | W-C-STORY |
| `Common_EventScript_ShowPokemartSign` | [data/event_scripts.s:1173](../baseline/source/data/event_scripts.s#L1173) | 24 | W-C-STORY |
| `Common_EventScript_ShowPokemonCenterSign` | [data/event_scripts.s:1177](../baseline/source/data/event_scripts.s#L1177) | 32 | W-C-STORY |
| `Common_EventScript_UnionRoomAttendant` | [data/event_scripts.s:1672](../baseline/source/data/event_scripts.s#L1672) | 17 | W-C-STORY |
| `Common_EventScript_WirelessClubAttendant` | [data/event_scripts.s:1680](../baseline/source/data/event_scripts.s#L1680) | 17 | W-C-STORY |
| `EventScript_BattleColosseum_2P_PlayerSpot0` | [data/scripts/cable_club.inc:656](../baseline/source/data/scripts/cable_club.inc#L656) | 1 | W-C-LINK |
| `EventScript_BattleColosseum_2P_PlayerSpot1` | [data/scripts/cable_club.inc:661](../baseline/source/data/scripts/cable_club.inc#L661) | 1 | W-C-LINK |
| `EventScript_BattleColosseum_4P_PlayerSpot0` | [data/scripts/cable_club.inc:666](../baseline/source/data/scripts/cable_club.inc#L666) | 1 | W-C-LINK |
| `EventScript_BattleColosseum_4P_PlayerSpot1` | [data/scripts/cable_club.inc:674](../baseline/source/data/scripts/cable_club.inc#L674) | 1 | W-C-LINK |
| `EventScript_BattleColosseum_4P_PlayerSpot2` | [data/scripts/cable_club.inc:682](../baseline/source/data/scripts/cable_club.inc#L682) | 1 | W-C-LINK |
| `EventScript_BattleColosseum_4P_PlayerSpot3` | [data/scripts/cable_club.inc:690](../baseline/source/data/scripts/cable_club.inc#L690) | 1 | W-C-LINK |
| `EventScript_CutTree` | [data/scripts/field_move_scripts.inc:2](../baseline/source/data/scripts/field_move_scripts.inc#L2) | 40 | W-C-FIELD |
| `EventScript_RecordCenter_Spot0` | [data/scripts/cable_club.inc:723](../baseline/source/data/scripts/cable_club.inc#L723) | 1 | W-C-LINK |
| `EventScript_RecordCenter_Spot1` | [data/scripts/cable_club.inc:729](../baseline/source/data/scripts/cable_club.inc#L729) | 1 | W-C-LINK |
| `EventScript_RecordCenter_Spot2` | [data/scripts/cable_club.inc:735](../baseline/source/data/scripts/cable_club.inc#L735) | 1 | W-C-LINK |
| `EventScript_RecordCenter_Spot3` | [data/scripts/cable_club.inc:741](../baseline/source/data/scripts/cable_club.inc#L741) | 1 | W-C-LINK |
| `EventScript_RockSmash` | [data/scripts/field_move_scripts.inc:65](../baseline/source/data/scripts/field_move_scripts.inc#L65) | 86 | W-C-FIELD |
| `EventScript_StrengthBoulder` | [data/scripts/field_move_scripts.inc:226](../baseline/source/data/scripts/field_move_scripts.inc#L226) | 68 | W-C-FIELD |
| `EventScript_TradeCenter_Chair0` | [data/scripts/cable_club.inc:701](../baseline/source/data/scripts/cable_club.inc#L701) | 1 | W-C-LINK |
| `EventScript_TradeCenter_Chair1` | [data/scripts/cable_club.inc:706](../baseline/source/data/scripts/cable_club.inc#L706) | 1 | W-C-LINK |
| `EverGrandeCity_SidneysRoom_OnWarp` | [data/maps/EverGrandeCity_SidneysRoom/scripts.inc:26](../baseline/source/data/maps/EverGrandeCity_SidneysRoom/scripts.inc#L26) | 1 | W-C-STORY |
| `FortreeCity_House2_EventScript_SleepTalkTutor` | [data/scripts/move_tutors.inc:83](../baseline/source/data/scripts/move_tutors.inc#L83) | 1 | W-C-TUTOR |
| `GabbyAndTy_EventScript_GabbyBattle1` | [data/scripts/gabby_and_ty.inc:148](../baseline/source/data/scripts/gabby_and_ty.inc#L148) | 1 | W-C-TRAINER |
| `GabbyAndTy_EventScript_GabbyBattle2` | [data/scripts/gabby_and_ty.inc:160](../baseline/source/data/scripts/gabby_and_ty.inc#L160) | 1 | W-C-TRAINER |
| `GabbyAndTy_EventScript_GabbyBattle3` | [data/scripts/gabby_and_ty.inc:172](../baseline/source/data/scripts/gabby_and_ty.inc#L172) | 1 | W-C-TRAINER |
| `GabbyAndTy_EventScript_GabbyBattle4` | [data/scripts/gabby_and_ty.inc:184](../baseline/source/data/scripts/gabby_and_ty.inc#L184) | 1 | W-C-TRAINER |
| `GabbyAndTy_EventScript_GabbyBattle5` | [data/scripts/gabby_and_ty.inc:196](../baseline/source/data/scripts/gabby_and_ty.inc#L196) | 1 | W-C-TRAINER |
| `GabbyAndTy_EventScript_GabbyBattle6` | [data/scripts/gabby_and_ty.inc:208](../baseline/source/data/scripts/gabby_and_ty.inc#L208) | 3 | W-C-TRAINER |
| `GabbyAndTy_EventScript_TyBattle1` | [data/scripts/gabby_and_ty.inc:154](../baseline/source/data/scripts/gabby_and_ty.inc#L154) | 1 | W-C-TRAINER |
| `GabbyAndTy_EventScript_TyBattle2` | [data/scripts/gabby_and_ty.inc:166](../baseline/source/data/scripts/gabby_and_ty.inc#L166) | 1 | W-C-TRAINER |
| `GabbyAndTy_EventScript_TyBattle3` | [data/scripts/gabby_and_ty.inc:178](../baseline/source/data/scripts/gabby_and_ty.inc#L178) | 1 | W-C-TRAINER |
| `GabbyAndTy_EventScript_TyBattle4` | [data/scripts/gabby_and_ty.inc:190](../baseline/source/data/scripts/gabby_and_ty.inc#L190) | 1 | W-C-TRAINER |
| `GabbyAndTy_EventScript_TyBattle5` | [data/scripts/gabby_and_ty.inc:202](../baseline/source/data/scripts/gabby_and_ty.inc#L202) | 1 | W-C-TRAINER |
| `GabbyAndTy_EventScript_TyBattle6` | [data/scripts/gabby_and_ty.inc:214](../baseline/source/data/scripts/gabby_and_ty.inc#L214) | 3 | W-C-TRAINER |
| `LavaridgeTown_Gym_B1F_EventScript_Eli` | [data/maps/LavaridgeTown_Gym_1F/scripts.inc:144](../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc#L144) | 1 | W-C-STORY |
| `LavaridgeTown_Gym_B1F_EventScript_Jace` | [data/maps/LavaridgeTown_Gym_1F/scripts.inc:134](../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc#L134) | 1 | W-C-STORY |
| `LavaridgeTown_Gym_B1F_EventScript_Jeff` | [data/maps/LavaridgeTown_Gym_1F/scripts.inc:139](../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc#L139) | 1 | W-C-STORY |
| `LavaridgeTown_Gym_B1F_EventScript_Keegan` | [data/maps/LavaridgeTown_Gym_1F/scripts.inc:119](../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc#L119) | 1 | W-C-STORY |
| `LilycoveCity_ContestLobby_EventScript_Reporter` | [data/scripts/interview.inc:163](../baseline/source/data/scripts/interview.inc#L163) | 1 | W-C-FLAVOR |
| `LilycoveCity_DepartmentStoreRooftop_EventScript_SubstituteTutor` | [data/scripts/move_tutors.inc:99](../baseline/source/data/scripts/move_tutors.inc#L99) | 1 | W-C-TUTOR |
| `LilycoveCity_LilycoveMuseum_EventScript_BirdSculpture` | [data/maps/LilycoveCity_LilycoveMuseum_1F/scripts.inc:111](../baseline/source/data/maps/LilycoveCity_LilycoveMuseum_1F/scripts.inc#L111) | 2 | W-C-STORY |
| `LilycoveCity_PokemonCenter_1F_EventScript_ContestLadyMon` | [data/scripts/lilycove_lady.inc:462](../baseline/source/data/scripts/lilycove_lady.inc#L462) | 1 | W-C-FLAVOR |
| `LilycoveCity_PokemonCenter_1F_EventScript_LilycoveLady` | [data/scripts/lilycove_lady.inc:1](../baseline/source/data/scripts/lilycove_lady.inc#L1) | 1 | W-C-FLAVOR |
| `LittlerootTown_BrendansHouse_2F_EventScript_WallClock` | [data/scripts/players_house.inc:43](../baseline/source/data/scripts/players_house.inc#L43) | 1 | W-C-STORY |
| `LittlerootTown_MaysHouse_2F_EventScript_WallClock` | [data/scripts/players_house.inc:49](../baseline/source/data/scripts/players_house.inc#L49) | 1 | W-C-STORY |
| `MauvilleCity_EventScript_RolloutTutor` | [data/scripts/move_tutors.inc:18](../baseline/source/data/scripts/move_tutors.inc#L18) | 1 | W-C-TUTOR |
| `MauvilleCity_PokemonCenter_1F_EventScript_MauvilleOldMan` | [data/scripts/mauville_man.inc:1](../baseline/source/data/scripts/mauville_man.inc#L1) | 1 | W-C-FLAVOR |
| `MossdeepCity_EventScript_DynamicPunchTutor` | [data/scripts/move_tutors.inc:115](../baseline/source/data/scripts/move_tutors.inc#L115) | 1 | W-C-TUTOR |
| `MossdeepCity_GameCorner_1F_EventScript_DodrioBerryPickingRecords` | [data/scripts/cable_club.inc:1385](../baseline/source/data/scripts/cable_club.inc#L1385) | 1 | W-C-LINK |
| `MossdeepCity_GameCorner_1F_EventScript_PokemonJumpRecords` | [data/scripts/cable_club.inc:1379](../baseline/source/data/scripts/cable_club.inc#L1379) | 1 | W-C-LINK |
| `MtPyre_4F_EventScript_Tasha` | [data/maps/MtPyre_5F/scripts.inc:6](../baseline/source/data/maps/MtPyre_5F/scripts.inc#L6) | 1 | W-C-STORY |
| `MtPyre_5F_EventScript_Atsushi` | [data/maps/MtPyre_4F/scripts.inc:6](../baseline/source/data/maps/MtPyre_4F/scripts.inc#L6) | 1 | W-C-STORY |
| `PacifidlogTown_PokemonCenter_1F_EventScript_ExplosionTutor` | [data/scripts/move_tutors.inc:147](../baseline/source/data/scripts/move_tutors.inc#L147) | 1 | W-C-TUTOR |
| `PlayersHouse_1F_EventScript_Mom` | [data/scripts/players_house.inc:299](../baseline/source/data/scripts/players_house.inc#L299) | 2 | W-C-STORY |
| `PlayersHouse_1F_EventScript_Vigoroth1` | [data/scripts/players_house.inc:368](../baseline/source/data/scripts/players_house.inc#L368) | 2 | W-C-STORY |
| `PlayersHouse_1F_EventScript_Vigoroth2` | [data/scripts/players_house.inc:378](../baseline/source/data/scripts/players_house.inc#L378) | 2 | W-C-STORY |
| `PlayersHouse_2F_EventScript_GameCube` | [data/maps/LittlerootTown_BrendansHouse_2F/scripts.inc:276](../baseline/source/data/maps/LittlerootTown_BrendansHouse_2F/scripts.inc#L276) | 1 | W-C-STORY |
| `PlayersHouse_2F_EventScript_Notebook` | [data/maps/LittlerootTown_BrendansHouse_2F/scripts.inc:272](../baseline/source/data/maps/LittlerootTown_BrendansHouse_2F/scripts.inc#L272) | 1 | W-C-STORY |
| `ProfBirch_EventScript_RatePokedexOrRegister` | [data/scripts/prof_birch.inc:35](../baseline/source/data/scripts/prof_birch.inc#L35) | 2 | W-C-STORY |
| `ProfileMan_EventScript_Man` | [data/scripts/profile_man.inc:1](../baseline/source/data/scripts/profile_man.inc#L1) | 1 | W-C-FLAVOR |
| `RecordCorner_EventScript_Attendant` | [data/scripts/cable_club.inc:786](../baseline/source/data/scripts/cable_club.inc#L786) | 1 | W-C-LINK |
| `RivalsHouse_1F_EventScript_RivalMom` | [data/maps/LittlerootTown_MaysHouse_1F/scripts.inc:112](../baseline/source/data/maps/LittlerootTown_MaysHouse_1F/scripts.inc#L112) | 1 | W-C-STORY |
| `RivalsHouse_1F_EventScript_RivalSibling` | [data/maps/LittlerootTown_MaysHouse_1F/scripts.inc:138](../baseline/source/data/maps/LittlerootTown_MaysHouse_1F/scripts.inc#L138) | 1 | W-C-STORY |
| `RivalsHouse_2F_EventScript_Rival` | [data/maps/LittlerootTown_MaysHouse_2F/scripts.inc:244](../baseline/source/data/maps/LittlerootTown_MaysHouse_2F/scripts.inc#L244) | 1 | W-C-STORY |
| `Roulette_EventScript_Table1` | [data/scripts/roulette.inc:1](../baseline/source/data/scripts/roulette.inc#L1) | 6 | W-C-CONTEST |
| `Roulette_EventScript_Table2` | [data/scripts/roulette.inc:11](../baseline/source/data/scripts/roulette.inc#L11) | 6 | W-C-CONTEST |
| `Route111_EventScript_SecretPowerMan` | [data/scripts/secret_power_tm.inc:20](../baseline/source/data/scripts/secret_power_tm.inc#L20) | 1 | W-C-FIELD |
| `Route117_EventScript_DaycareMan` | [data/scripts/day_care.inc:1](../baseline/source/data/scripts/day_care.inc#L1) | 1 | W-C-DAYCARE |
| `Route117_PokemonDayCare_EventScript_DaycareWoman` | [data/scripts/day_care.inc:71](../baseline/source/data/scripts/day_care.inc#L71) | 1 | W-C-DAYCARE |
| `Route119_EventScript_Kecleon1` | [data/scripts/kecleon.inc:36](../baseline/source/data/scripts/kecleon.inc#L36) | 1 | W-C-LEGEND |
| `Route119_EventScript_Kecleon2` | [data/scripts/kecleon.inc:43](../baseline/source/data/scripts/kecleon.inc#L43) | 1 | W-C-LEGEND |
| `Route120_EventScript_Kecleon1` | [data/scripts/kecleon.inc:1](../baseline/source/data/scripts/kecleon.inc#L1) | 1 | W-C-LEGEND |
| `Route120_EventScript_Kecleon2` | [data/scripts/kecleon.inc:8](../baseline/source/data/scripts/kecleon.inc#L8) | 1 | W-C-LEGEND |
| `Route120_EventScript_Kecleon3` | [data/scripts/kecleon.inc:15](../baseline/source/data/scripts/kecleon.inc#L15) | 1 | W-C-LEGEND |
| `Route120_EventScript_Kecleon4` | [data/scripts/kecleon.inc:22](../baseline/source/data/scripts/kecleon.inc#L22) | 1 | W-C-LEGEND |
| `Route120_EventScript_Kecleon5` | [data/scripts/kecleon.inc:29](../baseline/source/data/scripts/kecleon.inc#L29) | 1 | W-C-LEGEND |
| `SafariZone_Northeast_EventScript_Boy` | [data/maps/SafariZone_South/scripts.inc:140](../baseline/source/data/maps/SafariZone_South/scripts.inc#L140) | 1 | W-C-STORY |
| `SafariZone_Northeast_EventScript_Girl` | [data/maps/SafariZone_South/scripts.inc:148](../baseline/source/data/maps/SafariZone_South/scripts.inc#L148) | 1 | W-C-STORY |
| `SafariZone_Northeast_EventScript_Woman` | [data/maps/SafariZone_South/scripts.inc:144](../baseline/source/data/maps/SafariZone_South/scripts.inc#L144) | 1 | W-C-STORY |
| `SafariZone_Southeast_EventScript_ExpansionZoneAttendant` | [data/maps/SafariZone_South/scripts.inc:120](../baseline/source/data/maps/SafariZone_South/scripts.inc#L120) | 1 | W-C-STORY |
| `SafariZone_Southeast_EventScript_FatMan` | [data/maps/SafariZone_South/scripts.inc:132](../baseline/source/data/maps/SafariZone_South/scripts.inc#L132) | 1 | W-C-STORY |
| `SafariZone_Southeast_EventScript_LittleGirl` | [data/maps/SafariZone_South/scripts.inc:128](../baseline/source/data/maps/SafariZone_South/scripts.inc#L128) | 1 | W-C-STORY |
| `SafariZone_Southeast_EventScript_RichBoy` | [data/maps/SafariZone_South/scripts.inc:136](../baseline/source/data/maps/SafariZone_South/scripts.inc#L136) | 1 | W-C-STORY |
| `SecretBase_EventScript_RecordMixTrainer` | [data/scripts/secret_base.inc:284](../baseline/source/data/scripts/secret_base.inc#L284) | 24 | W-C-LINK |
| `SlateportCity_OceanicMuseum_1F_EventScript_Reporter` | [data/scripts/interview.inc:46](../baseline/source/data/scripts/interview.inc#L46) | 1 | W-C-FLAVOR |
| `SlateportCity_PokemonFanClub_EventScript_Reporter` | [data/scripts/interview.inc:94](../baseline/source/data/scripts/interview.inc#L94) | 1 | W-C-FLAVOR |
| `SlateportCity_PokemonFanClub_EventScript_SwaggerTutor` | [data/scripts/move_tutors.inc:1](../baseline/source/data/scripts/move_tutors.inc#L1) | 1 | W-C-TUTOR |
| `TradeCenter_EventScript_Attendant` | [data/scripts/cable_club.inc:779](../baseline/source/data/scripts/cable_club.inc#L779) | 1 | W-C-LINK |
| `TrainerHill_OnFrame` | [data/scripts/trainer_hill.inc:20](../baseline/source/data/scripts/trainer_hill.inc#L20) | 5 | W-C-FACILITY |
| `TrainerHill_OnResume` | [data/scripts/trainer_hill.inc:1](../baseline/source/data/scripts/trainer_hill.inc#L1) | 5 | W-C-FACILITY |
| `TrainerHill_OnWarp` | [data/scripts/trainer_hill.inc:10](../baseline/source/data/scripts/trainer_hill.inc#L10) | 2 | W-C-FACILITY |
| `VerdanturfTown_PokemonCenter_1F_EventScript_FuryCutterTutor` | [data/scripts/move_tutors.inc:35](../baseline/source/data/scripts/move_tutors.inc#L35) | 1 | W-C-TUTOR |
