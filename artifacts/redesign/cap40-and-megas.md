# Cap 40, spec corrections and Mega Stones: implementation spec

Approved by the owner. Follows `artifacts/redesign/cap20-30-changes.md`
(implemented in 237c389645). Same rules: a species moving into a slot takes
that slot's levels; a filler keeps the old slot's levels; regenerate the
manifest at the end and check every "expect" line below against it.

## 1. Corrections to the cap-20/30 spec (my errors)

Route 118's grass is across the river (Surf, cap 55) and Meteor Falls 1F 1R's
encounter area opens after Juan (cap 80), so five moves landed far too late.

Undo (in `src/data/wild_encounters.json`):
- Route 118 land: Sewaddle -> Linoone, Chingling -> Raticate, Buneary -> Liepard.
- Meteor Falls 1F 1R land: Riolu -> Druddigon, Woobat -> Ferroseed.

Redo:

| Species | New slot (levels) | Replaces | Expect first cap |
|---|---|---|---|
| Buneary | Verdanturf Meadow land 2 (42) | Kecleon (still on Route 119) | 30 (Lopunny 30) |
| Sewaddle | Route 117 honey 4 (26) | 2nd Shelmet | 30 (Leavanny 30) |
| Chingling | Route 117 honey 3 (26) | 2nd Audino | 30 (Chimecho 30) |
| Riolu | Jagged Pass land 11 (36) | 3rd Turtonator | 40 (Lucario 40) |
| Woobat | Fiery Path land 11 (34) | 2nd Heatmor | 40 (Swoobat 40) |

## 2. Bloodmoon Ursaluna uses the one restricted slot

Bloodmoon Ursaluna is officially none of Legendary, Mythical, Ultra Beast or
Paradox, so it stacks with a Legendary. Count it as Legendary-class:
- `src/pokemon.c` `GetRestrictedPartyClass`: before reading the base species'
  flags, `if (species == SPECIES_URSALUNA_BLOODMOON) return RESTRICTED_PARTY_LEGENDARY;`
  (check the raw species: its base species is plain Ursaluna, which stays
  unrestricted).
- `scripts/reference_pool.py` `restricted_class`: same exception on the raw
  species, before `self.base(...)`.
- Any player-facing text that names the rule ("one Legendary, Mythical,
  Ultra Beast or Paradox") must still read correctly; add Bloodmoon Ursaluna
  where the rule is listed (including the AGENTS.md rule line).
- Tests: native (a party with Entei refuses Bloodmoon Ursaluna; plain Ursaluna
  is still allowed) and Python (`restricted_class` returns "legendary").
- It stays at Ashen Woods (cap 40).

## 3. Cap-40 trainer changes

Edit `data/emerald_champions/emerald_champions_battle_teams.txt`, keep each
replaced member's level offset, update that trainer's `plan:`/`crack:` text so
it describes the new member, then run
`python3 scripts/emerald_champions_teams.py --write` and `--check` (all PASS).
Use legal moves only; if `--check` rejects a move, pick the closest legal one.

Duplicate sets (re-roll the later copy):
1. #103 Julio (`TRAINER_JULIO`): replace Choice Band Dodrio (exact copy of
   Gabby & Ty's) with `LOKIX @FOCUS_SASH TINTED_LENS ADAMANT 4/252/0/0/0/252 | FIRST_IMPRESSION, SUCKER_PUNCH, LEECH_LIFE, PROTECT`.
2. #95 Courtney (`TRAINER_COURTNEY_METEOR_FALLS`): replace Claydol (exact
   copy of Brice's) with `BEHEEYEM @LIFE_ORB ANALYTIC MODEST 252/0/4/252/0/0 | PSYCHIC, THUNDERBOLT, SHADOW_BALL, PROTECT`.
3. #113 Jeff (`TRAINER_JEFF`): replace Scarf Darmanitan (exact copy of
   Jaylen's) with `ARCANINE @SITRUS_BERRY INTIMIDATE ADAMANT 4/252/0/0/0/252 | FLARE_BLITZ, EXTREME_SPEED, WILD_CHARGE, PROTECT`.
4. #112 Eli (`TRAINER_ELI`): replace Mega Camerupt (Maxie's signature at #100)
   with `SKELEDIRGE @LEFTOVERS UNAWARE QUIET 252/0/4/252/0/0 | TORCH_SONG, SHADOW_BALL, EARTH_POWER, PROTECT`
   (slow, fits his Trick Room). Set Eli's `mega_slots: NONE`.

Cyndy #96 (`TRAINER_CYNDY_1`), four unevolved Pokemon at cap 40: keep the
Prankster Riolu signature and Pure Power Meditite; evolve the other two:
- Crabrawler -> `CRABOMINABLE @SITRUS_BERRY IRON_FIST ADAMANT 252/252/0/0/4/0 | ICE_HAMMER, DRAIN_PUNCH, THUNDER_PUNCH, PROTECT`
- Tinkatink -> `TINKATON @LEFTOVERS MOLD_BREAKER ADAMANT 252/252/0/0/4/0 | GIGATON_HAMMER, PLAY_ROUGH, KNOCK_OFF, PROTECT`
If later Cyndy rematch branches (`TRAINER_CYNDY_2`+) still field the old
base forms at higher caps, leave them for the tuning pass.

## 4. Mega Stones: no inert field pickups before the Mega Ring

The Mega Ring arrives with Norman's gift (cap 45). About 37 stones were
handed out earlier and did nothing until then.

Keep as pre-Ring promises (gifts, unchanged): Delphoxite (Roxanne),
Emboarite (Brawly), Raichunite X (Wattson), Chandelurite (Flannery),
Gardevoirite (Wanda), Galladite (Cozmo), Aggronite (Route 116 NPC). Norman's
starter-pair stones stay with the Ring.

### 4a. Replace every pre-Ring field-pickup stone with a useful item

Only swap the contents of existing item balls (AGENTS.md: no new ground or
hidden items). Rename any flag whose name carries the old item, as was done
for the Seaspray Cave balls. Script labels below are the current pickups.

| Current stone | Pickup script | New item |
|---|---|---|
| Abomasite | `Seaspray_Cave_B1F_ItemAbomasite` | Rocky Helmet |
| Absolite Z | `Seaspray_Cave_B1F_ItemFreezeDry` | Damp Rock |
| Golurkite | `Seaspray_Cave_Stealth_Rock` | Smooth Rock |
| Audinite | `Route104_EventScript_ItemAudinite` | Shell Bell |
| Chimechite | `Route116_EventScript_TM77StruggleBug` | Wide Lens |
| Beedrillite | `PetalburgWoods_3_Beedrillite` | Black Sludge |
| Scolipite | `PetalburgWoods_2_Item_TM80Venoshock` | Toxic Orb |
| Alakazite | `Slateport_City_ItemAlakazite` | Light Clay |
| Banettite | `DewfordManor_EventScript_Banettite` | Air Balloon |
| Butterfrenite | `DewfordMeadow_EventScript_Butterfrenite` | Heat Rock |
| Meganiumite | `GraniteCave_B1F_EventScript_ItemTM65ShadowClaw` | Protective Pads |
| Steelixite | `GraniteCave_B2F_EventScript_ItemSteelixite` | Throat Spray |
| Sablenite | `RusturfTunnel_EventScript_ItemSablenite` | Eject Pack |
| Manectite | `Route110_EventScript_ItemManectite` | Assault Vest |
| Skarmorite | `VerdanturfMeadow_EventScript_TM21` | Lum Berry |
| Tatsugirinite | `Seaspray_Cave_Water_Pulse` | Covert Cloak |
| Drampanite | `Route114_EventScript_ItemTM54Psyshock` | Weakness Policy |
| Raichunite Y | `MeteorFalls_1F_1R_EventScript_ItemTM59DragonPulse` | Booster Energy |
| Chesnaughtite | `Ember_Path_ItemSmack_Down` | Clear Amulet |
| Charizardite Y | `Ember_Path_ItemCharizarditeY` | Electric Seed |
| Crabominite | `Ashen_Woods_ItemU_Turn` | Grassy Seed |
| Pinsirite | `Ashen_Woods_ItemPinsirite` | Mirror Herb |
| Falinksite | `JaggedPass_EventScript_ItemTM69RockPolish` | Adrenaline Orb |
| Houndoominite | `Route112_EventScript_ItemHoundoominite` | Safety Goggles |
| Charizardite X | `FieryPath_EventScript_ItemCharizarditeX` | Misty Seed |
| Scovillainite | `FieryPath_EventScript_ItemTM06` | Psychic Seed |
| Zygardite | `Route111_EventScript_ItemTM37` | Room Service |
| Darkranite | `Sandstrewn_Ruins_ItemLeechLife` | Ability Shield |
| Garchompite | `Sandstrewn_Ruins_ItemGarchompite` | Utility Umbrella |
| Aerodactylite | `Mirage_Tower_4F_EventScript_Aerodactylite` | Power Herb |

If a listed new item is already given at that same spot's cap by another
source, pick a different item from the unused rows' spirit (useful, not yet
available at that cap) and note it in the commit.

### 4b. Re-place those 30 stones after the Ring, strongest last

Put each stone into an existing item-ball pickup in an area that first opens
at the stated cap or later. Prefer balls that currently hold common
consumables or items available elsewhere; keep loose thematic fits (fire
stones in volcanic or late fire areas, Garchompite in a desert or cave).

- Wave 1, caps 45-55 (about 20): Audinite, Banettite, Butterfrenite,
  Beedrillite, Scolipite, Chimechite, Meganiumite, Abomasite, Golurkite,
  Sablenite, Steelixite, Falinksite, Crabominite, Chesnaughtite,
  Tatsugirinite, Drampanite, Pinsirite, Scovillainite, Skarmorite,
  Houndoominite, Absolite Z, Raichunite Y
- Wave 2, caps 60-65: Alakazite, Manectite, Aerodactylite, Charizardite X
- Wave 3, cap 70+: Charizardite Y, Garchompite, Zygardite, Darkranite

Expect in the regenerated manifest: no "New Mega Stones" lines from field
pickups before cap 45; only the seven gift stones above appear before the
Ring line.

## 5. After implementing

1. `make -j4`, `make -j4 check` (2003 pass + new tests, 9 expected
   failures), `python3 -m unittest discover -s tests`.
2. Regenerate the manifest (`scripts/manifest_campaign.py`, then
   `scripts/manifest_text.py` into
   `artifacts/progression-manifest/availability-manifest.txt`) and check
   every "expect" above. Also confirm Bloodmoon Ursaluna is reported as
   restricted.
3. Commit and push to `claude/eager-feynman-m216tp`.
