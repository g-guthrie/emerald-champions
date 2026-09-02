# Inclement choreography seam audit (2026-09-01)

## Scope

Every Hoenn map whose layout is byte-identical to Inclement Emerald v1.13 (all 463
shared layouts) runs pokeemerald-expansion scripts on Inclement geometry. This audit
looks for the seam where Inclement moved objects or rewrote choreography while our
scripts kept upstream paths.

## Method (static, no emulator)

1. Diff every `applymovement` body and literal-coordinate command against
   `inclement-game-source/data/maps/*/scripts.inc` after normalising `TRUE/FALSE`
   and Inclement's `.set LOCALID_*` aliases.
2. Simulate every scripted NPC path in both trees, tracking `setobjectxy`,
   `addobject`, and earlier movements, and report only sequences whose path does not
   exist in Inclement and that cross collision tiles or leave the map.
3. Compare each map's `local_id -> graphics_id` binding against `HEAD` and the
   pre-restoration merge commit to catch objects whose identity was rebound when
   Inclement's object list replaced ours.
4. Check every `OBJ_EVENT_GFX_VAR_*` object for an in-map graphics-variable setup.

## Findings and repairs

- **MeteorFalls_1F_1R (confirmed, fixed).** The restoration kept Inclement's NPC
  positions but vanilla paths, and rebound `LOCALID_METEOR_FALLS_ARCHIE` to an Aqua
  grunt sprite while the Archie sprite carried `LOCALID_METEOR_FALLS_AQUA_GRUNT_2`.
  Both Magma exits and Archie's approach walked through the ledge at (12..13,19).
  The map's `OnLoad` also never called `Common_EventScript_SetupRivalGfxId`, so the
  `OBJ_EVENT_GFX_VAR_0` rival inherited whatever the previous map left in the
  variable. Repairs: rebound the three Aqua local IDs to their sprites, named the
  rival object `LOCALID_METEOR_FALLS_RIVAL`, ported Inclement's paths (player walks
  down the (14,19) ramp, every exit climbs it before the row-18 corridor), moved the
  rival aside with the player exactly as Inclement does, and added the rival graphics
  setup to `OnLoad`. Every step was re-simulated on the layout with no collision.
- **Parity manifest.** Regenerating the manifest picked up gitignored `.4bpp` build
  outputs under `data/tilesets`; the generator now skips git-ignored files. Only the
  Meteor Falls entry changed in `docs/inclement_overworld_parity_manifest.json`.
- **Visual-contract gate.** The literal local-ID visual-call pin moved from 1841 to
  1844 for the rewritten scene.
- No other Hoenn map showed an ours-only path across collision once state tracking
  and Inclement's label aliases were handled. Remaining hits are vanilla off-map exits
  (boats, Battle Frontier corridors) that Inclement shares.
- `FallarborTown_MoveRelearnersHouse` rebound its NPC from `FAT_MAN` to `GIRL_3`; the
  dialogue is gender-neutral, so this is cosmetic only.

## Limits

Player-relative movements depend on trigger position and were not simulated. Camera
and field-effect anchors are covered by `verify_emerald_champions_visual_contracts.py`.
A rendered pass of the Meteor Falls scene still needs an emulator or a human.
