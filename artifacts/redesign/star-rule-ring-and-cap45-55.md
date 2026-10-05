# One restricted Pokemon, the Mega Ring at Wattson, caps 45-55: implementation spec

Approved by the owner. Follows `artifacts/redesign/cap40-and-megas.md`
(implemented in 6329e7f1d2). Same working rules: a species moving into a
slot takes that slot's levels; a replaced trainer member keeps its level
offset; only swap the contents of existing item balls (no new ground or
hidden items) and rename any flag whose name carries the old item;
regenerate the manifest at the end and check every "expect" line.

Do not retune trainer levels in this pass; serial Hard tuning will do that.

## 1. The Mega Ring comes from Wattson; Norman gives the starter stones

Progression: early gyms are ordinary doubles, Wattson demonstrates Mega
Evolution (his ace is Mega Manectric) and then hands it over, Norman expands
the options.

1. `data/maps/MauvilleCity_Gym/scripts.inc`: after Wattson's defeat, where
   he already gives Raichunite X (`MauvilleCity_Gym_EventScript_GiveVoltSwitch2`,
   ~line 110), also `giveitem ITEM_MEGA_RING` and `setflag
   FLAG_SYS_RECEIVED_KEYSTONE`. Check Bag space for both before giving
   either. His text: he showed you Mega Evolution, now it's yours; one Mega
   per battle; Mega Evolution uses your party's one restricted slot (section 2),
   so no Mega while a Legendary-class Pokemon is in your party.
2. `data/maps/PetalburgCity_Gym/scripts.inc`: Norman no longer gives the Ring.
   At four badges (`VAR_PETALBURG_GYM_STATE` 6) talking to him starts the
   battle directly. After his defeat, give the starter-pair stones with the
   existing chain (`BufferNormanMegaGiftKind`, `BufferNormanStarterMegaStone`,
   `MarkStarterMegaStoneReceived`, `src/mega_stone_rewards.c`), keeping the
   fallback stone and the later partner-stone pickups. `CanReceiveNormanMegaGift`
   checks Bag space for the stones only. Track "Norman's stones given" with
   the existing receipt marks or a new flag; `FLAG_SYS_RECEIVED_KEYSTONE`
   no longer means it. Rewrite his intro and post-battle lines accordingly.
3. Everything keyed to `FLAG_SYS_RECEIVED_KEYSTONE` now opens after Wattson:
   the Center clerk's Form Items shelf (`src/field_specials.c` ~712,
   `data/scripts/general_mart.inc`) and the Center guide
   (`src/center_guide.c` ~218, update its text). Check every other reader.
4. Tools: `scripts/reference_pool.py` (Ring source ~1111-1124 becomes
   Wattson; starter-pair stones require Norman's defeat; the
   `TRAINER_NORMAN_1` requirement ~1405 drops `FLAG_SYS_RECEIVED_KEYSTONE`;
   gate texts ~1741 and ~1795 say "Wattson's gift after three Gym wins"),
   `scripts/mega_register.py`, and anything else that names Norman as the
   Ring source.

Expect: the manifest's "Mega Ring" line is in the step after Wattson (cap 40).
Gift stones already held (Delphoxite, Emboarite, Aggronite, Gardevoirite,
Raichunite X, Galladite) are usable from cap 40. Starter-pair stones first
appear at cap 55. Petalburg's gym trainers are fought with the Ring.

## 2. One restricted Pokemon per party, and Megas count

The rule: a party may hold one restricted Pokemon, and Mega Evolution
counts as that one slot.

Restricted: Legendary, Mythical, Ultra Beast, Paradox (as today), plus
Gholdengo, Ursaluna and Bloodmoon Ursaluna. Gimmighoul (both forms),
Teddiursa and Ursaring stay ordinary.

### 2a. Species list
- `src/pokemon.c` `GetRestrictedPartyClass` (~3149): before reading the base
  species' flags, return a restricted class for the raw species
  `SPECIES_GHOLDENGO`, `SPECIES_URSALUNA` and `SPECIES_URSALUNA_BLOODMOON`.
  Add a new enum value (e.g. `RESTRICTED_PARTY_LISTED`) if a class needs a
  name, and update every `switch` on the enum (`src/legendary_signs.c` ~93,
  `src/emerald_champions_agent_prep.c`, `src/wild_encounter.c`, and any
  other caller).
- `scripts/reference_pool.py` `restricted_class`: same, on the raw species.

### 2b. Mega Evolution uses the slot (player only, checked in battle)
- In the player's Mega Evolution availability check (the Ring/stone check
  near `src/battle_util.c` ~8503): a player battler cannot Mega Evolve if
  any other Pokemon in the player's party is restricted. A restricted
  Pokemon may Mega Evolve itself (Zygardite, Zeraorite and other legend
  stones stay useful). The Mega trigger simply does not appear.
- Player side only: opponents and an AI partner in multi battles are not
  affected. Authored trainers keep their legends and Megas together.
- Explain it once: the first time the rule removes a Mega option, print one
  line ("Mega Evolution is unavailable: your party has a restricted
  Pokemon."). Use a free save flag for once-per-save; if none is free, show
  it at the start of each battle where it applies.
- No Bag or PC restriction on Mega Stones.

### 2c. Evolving into a restricted species
If a level-up, Leveler, post-battle or item evolution would turn a Pokemon
into a restricted species while a different restricted Pokemon is in the
party (Gimmighoul at 45, Ursaring with a Peat Block), cancel it with a
message ("{name} can't evolve while another restricted Pokemon is in your
party."). Block it where the evolution runs, not in
`GetEvolutionTargetSpecies`, so the evolution stays pending:
`IsMonEligibleForLeveler` already counts a pending evolution at the cap,
so after the player frees the slot the Leveler evolves it at the same
level. An evolution item is refused and not consumed.

### 2d. Text and teaching
- PC message (`src/pokemon_storage_system.c` ~1059, "Only 1 Legendary,
  Mythical,\nUltra Beast or Paradox total."): replace with "Only 1
  restricted Pokemon\nper party." (check the width).
- Teach the rule in the first-visit tool handoff
  (`data/scripts/pkmn_center_nurse.inc`, `EventScript_PkmnCenterNurse_CheckTools`,
  the first-visit explanation branch), before Kubfu is catchable: one
  restricted Pokemon per party (Legendary, Mythical, Ultra Beast, Paradox,
  Gholdengo, Ursaluna). Wattson adds the Mega part (section 1).
- Check text widths with `scripts/check_text_widths.py` (10 pre-existing
  overflows; add none).

### 2e. Tools and rules
- `scripts/tuning_pool_check.py` (and everything sharing its checker:
  `scripts/retune_recheck.py`, `scripts/pool_cards.py`): a benchmark team
  holds at most one restricted Pokemon; if it holds one, only that Pokemon
  may hold a usable Mega Stone; a team with no restricted Pokemon may have
  one Mega. `reference_pool.player_rules` text says the same.
- `src/emerald_champions_agent_prep.c`: same rule for prepared parties.
- `AGENTS.md` (~lines 87-88): replace "One Legendary, Mythical, Ultra Beast
  or Paradox per party in total" with the new rule and list.
- Tests: native (a party with Entei cannot Mega Evolve its Gardevoir; a
  lone Mega-capable legend can; Gimmighoul at 45 with Entei in the party
  stays Gimmighoul and evolves through the Leveler once Entei is boxed; a
  Peat Block on Ursaring is refused with Entei present) and Python
  (`restricted_class` for the three species; the team checker rejects
  legend + another member's Mega). Update `tests/test_cap40_redesign.py`:
  both Ursaluna forms are now restricted, Bloodmoon's first cap is 40.

## 3. Bloodmoon Ursaluna back to Ashen Woods

Undo 6329e7f1d2's move in `src/data/wild_encounters.json`:
- Ashen Woods land slot 11 (L41): Ursaring -> Ursaluna (Bloodmoon).
- Victory Road 1F land slot 11 (L74): Ursaluna (Bloodmoon) -> Lairon (its
  original species).

As a restricted species it must behave like the other Ashen Woods legend
slots (Chi-Yu, Okidogi, Buzzwole, Brute Bonnet): a 5% encounter that
stops once caught. Use whatever mechanism those slots use (move it to a
legend position in the table if position matters), and add a test that
catching it closes the encounter.

Expect: Bloodmoon first at cap 40, reported as restricted.

## 4. Pseudo-legendary Mega Stones a stage after their Pokemon

At cap 55 every pseudo-legendary evolves at once. Their 700-BST Megas and
Mega Kangaskhan must not land in the same stage.

| Stone | Current source | Change | Expect first cap |
|---|---|---|---|
| Garchompite Z | Route 111 ball (`Route111_EventScript_ItemTM63RockSlide`, 3,113) | swap into an existing ball in an area that opens at cap 70+ | 70+ |
| Tyranitarite | Scorched Slab B2F ball (`ScorchedSlab_EventScript_ItemTyranitarite`) | same | 70+ |
| Tyranitarite, Dragoninite, Baxcalibrite | Harvest Pouch berry requests (`sBerryStoneTrades`, `src/mega_stone_rewards.c`; menu in `data/scripts/emerald_champions.inc` ~902) | offered only from `FLAG_BADGE07_GET`; before that the request shows as not yet available | 70+ |
| Kangaskhanite | Safari Zone Northeast ball (`SafariZone_Northeast_EventScript_ItemKangaskhanite`) | swap into an existing ball in an area that opens at cap 60-65 | 60-65 |

The displaced item from each target ball goes into the old ball. Dragoninite's
ball (Seafloor Cavern) and Baxcalibrite's ball (Seaspray Cave, 46,18) are
already late; leave them. The pseudo-legendary species keep their natural
evolution levels.

## 5. Manifest gaps

The manifest (the authority for stage-legal teams) never reports these, while
`reference_pool` does: the Safari Zone (wild species and its Kangaskhanite
and Glimmoranite balls), Lilycove's Altarianite gift, Route 133
(Starminite, Medichamite), Pacifidlog's Diancite gift and the Aqua Hideout
Master Ball. Teach the manifest (`scripts/manifest_campaign.py` /
`scripts/manifest_world.py` and their source readers) to reach them, or
document why each is unreachable. Expect them in the regenerated manifest
at the steps where their areas open.

## 6. Trainer changes

Edit `data/emerald_champions/emerald_champions_battle_teams.txt`, keep each
replaced member's level offset (already filled in below), update the
trainer's `plan:`/`crack:` text to describe the new member, then run
`python3 scripts/emerald_champions_teams.py --write` and `--check` (all
PASS). All sets below are legal in the All Legal Moves table; if `--check`
still rejects one, pick the closest legal move. Battle numbers are from the
current order and may shift; use the trainer IDs.

### Cap 45
1. #118 Drew (`TRAINER_DREW`), third copy of Ethan's Cacturne moveset.
   Replace CACTURNE with
   `RAMPARDOS @LIFE_ORB SHEER_FORCE BRAVE 252/252/4/0/0/0 2 | ROCK_SLIDE, ZEN_HEADBUTT, FIRE_PUNCH, PROTECT | ivs=31/31/31/31/31/0 | friendship=255`
   (slow Sheer Force nuke for his Trick Room; no trainer uses Rampardos yet).
2. #122 Justin (`TRAINER_JUSTIN`), Bouffalant and Cacturne are exact copies
   of Ethan's (#104). Replace
   - BOUFFALANT with `KROOKODILE @LIFE_ORB INTIMIDATE ADAMANT 4/252/0/0/0/252 3 | HIGH_HORSEPOWER, CRUNCH, ROCK_SLIDE, PROTECT | ivs=31/31/31/31/31/31 | friendship=255`
   - CACTURNE with `GRIMMSNARL @LIGHT_CLAY PRANKSTER CAREFUL 252/0/4/0/252/0 4 | SPIRIT_BREAK, REFLECT, LIGHT_SCREEN, THUNDER_WAVE | ivs=31/31/31/31/31/31 | friendship=255`
     (Prankster screens behind Malamar's Contrary boosts: Trick House tricks).
3. #121 Dusty (`TRAINER_DUSTY_1`):
   - SIGILYPH is Courtney's (#95) set move for move. Replace with
     `SIGILYPH @LIFE_ORB TINTED_LENS TIMID 4/0/0/252/0/252 13 | AIR_SLASH, PSYCHIC, TAILWIND, PROTECT | ivs=31/31/31/31/31/31 | friendship=255`
   - Ting-Lu's Vessel of Ruin lowers the Special Attack of his own Sigilyph,
     Aurorus and Bastiodon. Replace TING_LU with
     `WO_CHIEN @LEFTOVERS TABLETS_OF_RUIN CALM 252/0/4/0/252/0 12 | RUINATION, POLLEN_PUFF, LEECH_SEED, PROTECT | ivs=31/31/31/31/31/31 | friendship=255`
     (same Ruin theme and Ruination; its aura lowers Attack, which his
     special team never uses). Remove the aura opportunity-cost caveat from
     his plan.

### Cap 55
4. #132 Yuji (`TRAINER_YUJI`): the rival's Lilycove team (#188) carries the
   same Hariyama. Keep the rival's. Replace Yuji's HARIYAMA with
   `HITMONCHAN @ASSAULT_VEST IRON_FIST ADAMANT 252/252/4/0/0/0 4 | FAKE_OUT, DRAIN_PUNCH, ICE_PUNCH, THUNDER_PUNCH | ivs=31/31/31/31/31/31 | friendship=255`
   (a Hitmon pair for the dojo; it keeps the Fake Out role).
5. #128 Alexia (`TRAINER_ALEXIA`, cap 45): the rival's Swellow (#188, grown
   from his Guts Taillow since battle #1) runs her set. Keep the rival's.
   Replace Alexia's SWELLOW with
   `RATICATE @FLAME_ORB GUTS JOLLY 4/252/0/0/0/252 7 | FACADE, SUCKER_PUNCH, U_TURN, PROTECT | ivs=31/31/31/31/31/31 | friendship=255`
   (still a self-burning berserker; Raticate is unused elsewhere).
6. #141 Garrison (`TRAINER_GARRISON`): Choice Band Relicanth is Foster's
   (#133) exact set. Replace RELICANTH with
   `TYRANTRUM @LIFE_ORB STRONG_JAW ADAMANT 4/252/0/0/0/252 5 | ROCK_SLIDE, CRUNCH, FIRE_FANG, PROTECT | ivs=31/31/31/31/31/31 | friendship=255`
   (keeps his fossil theme).
7. #163 Weather Institute grunt (`TRAINER_GRUNT_WEATHER_INST_5`): his Scizor
   is Wendy's (#176) Mega Scizor's moveset. Keep Wendy's ace. Replace the
   grunt's SCIZOR with
   `ESCAVALIER @SITRUS_BERRY OVERCOAT ADAMANT 252/252/4/0/0/0 2 | IRON_HEAD, MEGAHORN, SWORDS_DANCE, PROTECT | ivs=31/31/31/31/31/31 | friendship=255`
   (an armoured knight for "the armoury"; Toxicroak's Fake Out still buys
   the Swords Dance).
8. #164 Weather Institute grunt (`TRAINER_GRUNT_WEATHER_INST_2`): his Pelipper
   is Matt's (#197, #223) exact set. Keep Matt's. Replace the grunt's PELIPPER with
   `POLITOED @DAMP_ROCK DRIZZLE CALM 252/0/4/0/252/0 5 | SCALD, ENCORE, ICE_BEAM, PROTECT | ivs=31/31/31/31/31/31 | friendship=255`
   and set `strategy: RAIN,SETUP` (no Tailwind left). This set shares only
   Protect with Darrin's Politoed (#136).
9. #161 Martha (`TRAINER_MARTHA`): Flare Boost Gourgeist is Diana's (#102)
   exact set. Replace GOURGEIST with
   `POLTEAGEIST @WHITE_HERB WEAK_ARMOR MODEST 4/0/0/252/0/252 4 | SHELL_SMASH, SHADOW_BALL, GIGA_DRAIN, PROTECT | ivs=31/31/31/31/31/31 | friendship=255`
   (no trainer uses Polteageist yet).
10. #177 Braxton (`TRAINER_BRAXTON`): his Mega Salamence is the rival's
    (#188) exact set. Keep the rival's. Replace
    - SALAMENCE with `DRACOZOLT @LIFE_ORB VOLT_ABSORB ADAMANT 4/252/0/0/0/252 2 | BOLT_BEAK, DRAGON_CLAW, ROCK_SLIDE, PROTECT | ivs=31/31/31/31/31/31 | friendship=255`
      (Bolt Beak doubles when it moves first, which Braviary's Tailwind
      sets up)
    - BLASTOISE with `BLASTOISE @BLASTOISINITE TORRENT MODEST 4/0/0/252/0/252 1 | WATER_PULSE, AURA_SPHERE, DARK_PULSE, PROTECT | ivs=31/31/31/31/31/31 | friendship=255`
      (a real Mega Blastoise; Mega Launcher comes from the Mega, so the
      custom Mega Launcher permission on base Blastoise goes away).
    - Set `mega_slots: 5`.
11. #187 Cristin (`TRAINER_CRISTIN_1`): her Drought Ninetales is Miu & Yuki's
    (#154) exact set. Keep the twins'. Replace NINETALES with
    `VOLCARONA @HEAT_ROCK FLAME_BODY TIMID 4/0/0/252/0/252 2 | SUNNY_DAY, HEAT_WAVE, QUIVER_DANCE, PROTECT | ivs=31/31/31/31/31/31 | friendship=255`
    (the sun becomes a move rather than an ability; update the plan's
    turn-one sequencing).
12. #166 Shelly (`TRAINER_SHELLY_WEATHER_INSTITUTE`): her Alolan Ninetales is
    Miu & Yuki's (#154) exact set. Replace NINETALES_ALOLA with
    `ABOMASNOW @LIGHT_CLAY SNOW_WARNING MODEST 252/0/4/252/0/0 6 | BLIZZARD, AURORA_VEIL, GIGA_DRAIN, PROTECT | ivs=31/31/31/31/31/31 | friendship=255`
    (same snow and Aurora Veil job).

Kept on purpose: Parker's Bloodmoon Ursaluna (trainer previews are fine),
Wendy's Mega Scizor, Matt's Pelipper, Miu & Yuki's Ninetales pair and the
rival's team (it grows across his fights). Copies whose later member is at
cap 60+ (hideout grunts and others) wait for those caps' audits.

## 7. After implementing

1. `make -j4`, `make -j4 check` (new tests pass; only the 9 known expected
   failures), `python3 -m unittest discover -s tests` all green, including
   any failures left by 6329e7f1d2.
2. Regenerate the manifest (`python3 scripts/manifest_campaign.py --out
   work/progression-manifest`, then `python3 scripts/manifest_text.py
   work/progression-manifest/progression.pickle --out
   artifacts/progression-manifest/availability-manifest.txt`) and check every
   "expect" above.
3. Commit and push to `claude/eager-feynman-m216tp`.
