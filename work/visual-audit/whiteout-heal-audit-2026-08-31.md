# Emerald Champions Whiteout Healing Placement Audit

## Scope

This audit reproduces every live Hoenn path into the screen-fixed Pokémon Center healing effect: ordinary restored Centers, whiteout recovery in both restored Center layouts, the Pokémon League lobby, and Trainer Hill. It also statically checks every nurse caller and the separate player-house recovery path.

## Step record

| Step | State | Health |
|---:|---|:---:|
| 1 | Pre-fix standard Center whiteout at `(7,4)` | FAIL — Poké Balls render 16 px left of the incubator |
| 2 | Fixed standard Center whiteout at `(8,4)` | PASS — Poké Balls land inside the incubator |
| 3 | Fixed Lavaridge-layout whiteout | PASS — alternate restored Center layout remains aligned |
| 4 | Pokémon League lobby whiteout | PASS — its explicit `(3,4)` camera anchor matches its nurse and machine |
| 5 | Trainer Hill ordinary nurse interaction | PASS — the non-Center map's native machine remains aligned |

![Healing entry-path audit](rendered/healing-entry-paths-2026-08-31/healing-entry-path-audit.png)

## Root cause

- `FldEff_PokecenterHeal` renders the balls at fixed screen coordinate `(93,36)` and the monitor at `(124,24)`.
- The normal nurse interaction is camera-aligned with the restored Center's nurse and incubator at map x-coordinate `8`.
- Modern whiteout recovery still uses expansion's default Center respawn x-coordinate `7`.
- That one-tile difference shifts the restored background 16 px to the right while the fixed-screen healing sprites do not move.
- All 16 Hoenn Pokémon Center whiteout destinations used the default x-coordinate `7`, while every corresponding nurse is at x-coordinate `8`.

## Production repair

- Added `respawn_x: 8` to all 16 Hoenn Center entries in `src/data/heal_locations.json` and regenerated `src/data/heal_locations.h`.
- Preserved unrelated FRLG defaults and the League lobby's reviewed explicit `(3,4)` position.
- Added real whiteout, Lavaridge, League, and Trainer Hill headless screenshots to the required native-UI regression set.
- Added a static invariant requiring every live Hoenn nurse entry path to identify an actual nurse and every whiteout camera x-coordinate to match that map's nurse/machine anchor.

## Broader code audit

- Checked all 18 live Hoenn calls into `Common_EventScript_PkmnCenterNurse`; every caller sets `VAR_0x800B` to an actual nurse on that map.
- Checked both calls into `EventScript_PkmnCenterNurse_TakeAndHealPkmn`; the only paths are normal nurse service and whiteout recovery.
- Verified whiteout setup assigns both `VAR_LAST_TALKED` and `VAR_0x800B` to the same healer.
- Verified the player-house whiteout deliberately uses the separate non-incubator healing path.
- Checked other fixed-screen Hoenn map effects. Hall of Fame recording has one dedicated map/camera contract; NPC Fly Out is a sprite-driven transition rather than a map-art alignment contract. No additional live Hoenn mismatch was found.

## Evidence limits

The screenshots prove the live geometry of every Hoenn healing layout class. Static checks cover caller identity and source topology; they cannot prove subjective animation timing, which still benefits from human playtesting.
