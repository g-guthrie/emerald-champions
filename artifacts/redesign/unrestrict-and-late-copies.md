# Un-restrict Gholdengo and Ursaluna, tutor ban, late-game copies: implementation spec

Approved by the owner. Follows `artifacts/redesign/star-rule-ring-and-cap45-55.md`
and the fixes in d4f828a0f6 and bd6cc1fe91. Same working rules: a species
moving into a wild slot takes that slot's levels; a replaced trainer member
keeps its level offset; regenerate the manifest at the end and check every
"expect" line. Do not retune trainer levels.

## 1. Gholdengo and both Ursaluna forms are ordinary again

They are strong but not restricted-tier; their problem was arriving early.
Restricting them made them dead picks once box legends and 700-BST Megas
arrive. The restricted rule goes back to the official classes (Legendary,
Mythical, Ultra Beast, Paradox) plus Mega Evolution sharing that slot. Keep
everything Mega-related from the previous spec.

### 1a. Remove the exceptions
- `src/pokemon.c` `GetRestrictedPartyClass`: delete the block that returns a
  class for `SPECIES_GHOLDENGO`, `SPECIES_URSALUNA`, `SPECIES_URSALUNA_BLOODMOON`.
- `scripts/reference_pool.py` `restricted_class`: delete the same exception;
  restore the `player_rules` "restricted" text to the four official classes
  plus the Mega sentence.
- `scripts/tuning_pool_check.py`: the "more than one restricted Pokemon" message
  lists only the official classes.

### 1b. Remove the evolution block (now dead code)
With the exceptions gone, no ordinary species evolves into a restricted one
(checked over every `EVO_*` entry). Remove the guard and its plumbing:
`CanEvolveMonWithinRestrictedLimit` (`src/pokemon.c`, `include/pokemon.h`) and
every call site: the `evoState` ternaries in `GetEvolutionTargetSpecies` go
back to plain `evoState`; `PokemonUseItemEffects`; `src/evolution_scene.c`
(`CommitEvolution` guard, `EVOSTATE_RESTRICTED_WAIT`, the trade-evolution
branch); `src/party_menu.c` `ItemUseCB_EvolutionStone`;
`gText_RestrictedEvolutionBlocked` (`src/strings.c`, `include/strings.h`);
`test/restricted_evolution.c`.

### 1c. Remove Bloodmoon's one-time-encounter logic
Bloodmoon becomes an ordinary encounter that keeps spawning. Remove
`FLAG_EC_CAUGHT_URSALUNA_BLOODMOON` (free 0x2AB), `RecordOwnedBloodmoon`
(`src/legendary_signs.c`, `include/legendary_signs.h`, and its `callnative`
in `EventScript_PkmnCenterNurse_RestrictedRule`), the Bloodmoon branch in
`IsWildSlotSpeciesAcquirable`, and the Bloodmoon lines in
`HandleSetPokedexFlagFromMon` and `MarkLegendarySignCaughtBySpecies`.

### 1d. New homes (`src/data/wild_encounters.json`, land tables)

| Change | Slot (keeps its levels) | Rates |
|---|---|---|
| Ashen Woods slot 11: Bloodmoon Ursaluna -> Ursaring | L41-43 | restore the pre-6329e7f1d2 rates: slot 4 = 12, slot 11 = 4 |
| Safari Zone Southeast slot 10: Granbull -> Ursaluna (Bloodmoon) | L50 | unchanged (4%); it lives beside the Safari's Ursaring |
| Route 116 slot 10: Gimmighoul (Roaming) -> Rookidee | L8 | unchanged |
| Route 121 slot 10: Zoroark -> Gimmighoul (Roaming) | L45 | unchanged |
| Mirage Tower B1F slot 2: Gimmighoul (Chest) -> Sandile | L39 | unchanged |
| Mt Pyre 2F slot 9: Misdreavus -> Gimmighoul (Chest) | L51 | unchanged |
| Meteor Falls Steven's Cave: rates back to before b31dabd49b | - | slot 2 (Lunatone) 16, slot 4 (Gholdengo) 15 |

Gimmighoul keeps its level-45 evolution, so one caught at cap 55 evolves at
once. Plain Ursaluna (Peat Block, Lilycove) is unchanged.

### 1e. Text and rules
- `EmeraldChampions_Text_RestrictedRule` (`data/scripts/emerald_champions.inc`):
  list Legendary, Mythical, Ultra Beast or Paradox only; drop the
  Gholdengo/Ursaluna and Gimmighoul/Teddiursa/Ursaring pages.
- `AGENTS.md` (~lines 87-93): drop "Gholdengo or either Ursaluna form" and the
  pre-evolution sentence; keep the Mega and Wattson/Norman sentences.
- `scripts/manifest_text.py` legend line (and the tracked manifest after
  regeneration): "[R] = Restricted: Legendary, Mythical, Ultra Beast or
  Paradox; one total per party." followed by the existing Mega line.
- Tests: `tests/test_cap40_redesign.py` and any native/Python test that
  expects the three species restricted or Bloodmoon at cap 40 / 5% / closing
  after capture. New expectations below.

Expect: Gholdengo, Ursaluna and Bloodmoon Ursaluna carry no [R];
Gimmighoul (both forms) and Gholdengo first appear at cap 55; Bloodmoon first
at cap 55 (Safari Zone Southeast); Ursaring stays at Ashen Woods (cap 40).

## 2. The tutor stops teaching one-hit-KO and evasion moves

No Guard Machop (cap 14), Machoke, Machamp and Golurk can learn Fissure,
which then always hits: a guaranteed KO on any grounded, non-Sturdy foe that
is not a higher level. Tuning already ignores these moves
(`UNTUNED_MOVES` in `scripts/tuning_pool_check.py`), so the player should not
be able to get them.

Banned for the player: `MOVE_SHEER_COLD`, `MOVE_FISSURE`, `MOVE_HORN_DRILL`,
`MOVE_GUILLOTINE`, `MOVE_DOUBLE_TEAM`, `MOVE_MINIMIZE`.
- All Legal Moves tutor: `GetEmeraldChampionsPreparationMovesToLearn`
  (`src/emerald_champions_battle_sets.c`) never offers them (same place the
  evolution-move gate filters).
- Level-up and evolution move learning for the player's Pokemon skips them.
- Pokemon the player receives (wild catches, gifts, eggs, trades) never carry
  them: skip them when the initial/egg moveset is generated.
- `src/emerald_champions_agent_prep.c` rejects them in prepared parties;
  `scripts/tuning_pool_check.py` reports them as illegal, not just untuned.
- Trainer teams are unaffected.
- Tests: native (a No Guard Machop is not offered Fissure; a level-up that
  would teach Double Team skips it; a wild Pokemon's generated moveset has
  none of the six) and Python (the checker rejects a party with Fissure).

## 3. Gina & Mia have nothing that touches Shedinja

`TRAINER_GINA_AND_MIA_1` (#13): no member can damage Wonder Guard Shedinja.
Replace Illumise's `BUG_BUZZ` with `AIR_SLASH` (legal):
`ILLUMISE @LEFTOVERS PRANKSTER MODEST 4/0/0/252/0/252 4 | DAZZLING_GLEAM, AIR_SLASH, ENCORE, HELPING_HAND`.
Mention Air Slash in her `plan:`/`crack:`.

## 4. Copied trainer sets, caps 65 to post-game

A copy is the same species with the same ability and all four moves as a
member of a different trainer. Change only the member named below; keep the
other trainer's. Edit `data/emerald_champions/emerald_champions_battle_teams.txt`,
then `python3 scripts/emerald_champions_teams.py --write` and `--check`.

Rules for every replacement:
- Replace the species, not one move. The only exceptions are marked "new set"
  below, where the species is the trainer's identity.
- The new species must not appear on any other trainer within 60 encounter
  numbers (the `E0xxx` ids) either side, and not already on the same team.
- Keep the member's role (physical, special or support), item category and
  level offset, and fit the trainer's class, theme and plan.
- Never create a new copy: re-run the copy check after each change.
- Update the trainer's `plan:`/`crack:` text for the new member.

Battle numbers are approximate; use the trainer IDs.

| Trainer (change this one) | Member | Copies (keep that one) |
|---|---|---|
| `TRAINER_ALEXA` #230 | MAWILE | Spenser #169 (Frontier Brain signature) |
| `TRAINER_ALEXA` #230 | FERALIGATR | Shelly #166 (her Mega ace) |
| `TRAINER_ALEXA` #230 | SCIZOR | Wendy #176 (her Mega ace) |
| `TRAINER_GRUNT_AQUA_HIDEOUT_2` #214 | SHARPEDO | Archie #269 |
| `TRAINER_GRUNT_AQUA_HIDEOUT_2` #214 | CRAWDAUNT | Shelly #265 |
| `TRAINER_GRUNT_AQUA_HIDEOUT_2` #214 | DRAGALGE | Shelly #265 |
| `TRAINER_GRUNT_AQUA_HIDEOUT_5` #215 | BARRASKEWDA | Darrin #136 |
| `TRAINER_GRUNT_AQUA_HIDEOUT_7` #216 | CLOYSTER | Jenna #174 |
| `TRAINER_GRUNT_AQUA_HIDEOUT_6` #221 | TOXICROAK | Weather Institute grunt #163 |
| `TRAINER_NIKKI` #226 | LAPRAS | Spenser #169 |
| `TRAINER_SUSIE` #236 | MANTINE | Aqua grunt #213 |
| `TRAINER_KARA` #237 | WHISCASH | Myles #184 |
| `TRAINER_HANNAH` #244 | MALAMAR | Matt #223 |
| `TRAINER_SOPHIA` #254 | MUDSDALE | Magma Hideout grunt #210 |
| `TRAINER_SEBASTIAN` #255 | SCIZOR | Wendy #176 |
| `TRAINER_BRENDA` #262 | OCTILLERY | Connie #270 (Juan's gym) |
| `TRAINER_GRUNT_SEAFLOOR_CAVERN_1` #263 | OBSTAGOON | Space Center grunt #257 |
| `TRAINER_GRUNT_SEAFLOOR_CAVERN_1` #263 | SHARPEDO | Archie #269 |
| `TRAINER_GRUNT_SEAFLOOR_CAVERN_2` #264 | SCRAFTY | Magma Hideout grunt #203 |
| `TRAINER_GRUNT_SEAFLOOR_CAVERN_5` #266 | GYARADOS | Aqua grunt #222 |
| `TRAINER_ARCHIE` #269 | PELIPPER (new set) | Matt #197/#223; Drizzle is Aqua's identity, so keep the species and give Archie a distinct set |
| `TRAINER_MATT` #223 | GRIMMSNARL | Sidney #284 (Elite Four) |
| `TRAINER_GRUNT_MAGMA_HIDEOUT_15` #209 | INCINEROAR | Sidney #284 (Elite Four) |
| `TRAINER_ALEXIS` #274 | INDEEDEE_F | Wally #300 |
| `TRAINER_ALBERT` #277 | GLIMMORA | Seafloor grunt #264 |
| `TRAINER_QUINCY` #278 | BUTTERFREE | Katelyn #229 |
| `TRAINER_HOPE` #282 | VAPOREON | Auron #225 |
| `TRAINER_EDGAR` #283 | VENOMOTH | Magma Hideout grunt #204 (room design) |
| `TRAINER_MICHELLE` #287 | MEGANIUM | Ruben #231 |
| `TRAINER_MITCHELL` #288 | MAROWAK_ALOLA | Magma Hideout grunt #211 (room design) |
| `TRAINER_FELIX` #294 | DURALUDON | Space Center grunt #259 |
| `TRAINER_KEIRA` #302 | SANDSLASH | Caroline #292 |
| `TRAINER_KEIRA` #302 | RHYPERIOR | Albert #277 |
| `TRAINER_KEIRA` #302 | ROTOM_WASH | Space Center grunt #253 |
| `TRAINER_LEROY` #303 | MAWILE | Spenser #169 / Alexa #230 |
| `TRAINER_LEAF_ALTERING_CAVE` #305 | KANGASKHAN (new set) | Athena #228; Leaf's Kanto team keeps the species |
| `TRAINER_LEAF_ALTERING_CAVE` #305 | NINETALES (new set) | Vito #290 |
| `TRAINER_LEAF_ALTERING_CAVE` #305 | GENGAR (new set) | Phoebe #285 (Elite Four) |
| `TRAINER_LEA_AND_JED` #313 | URSALUNA | Albert #277 |
| `TRAINER_NAOMI` #314 | SALAZZLE | Hope #282 |
| `TRAINER_BUFFEL` #317 | TOGEKISS | Cynthia #306 |

"New set" means a different ability or at least two different moves, plus a
different role or item, so the two no longer read as the same Pokemon.

Add `scripts/trainer_set_copies.py`: report every pair of members on
different trainers (ignoring rival starter variants and the same trainer's
numbered rematches) with the same species, ability and all four moves whose
encounter ids are within 60 of each other. Add a Python test that it reports
none from the Aqua Hideout (`TRAINER_GRUNT_AQUA_HIDEOUT_1`) onward.

## 5. After implementing

1. `make -j4`, `make -j4 check` (only the 9 known expected failures),
   `python3 -m unittest discover -s tests`, `python3 scripts/check_text_widths.py`
   (no new overflows), `python3 scripts/emerald_champions_teams.py --check`.
2. Regenerate the manifest (`scripts/manifest_campaign.py`, then
   `scripts/manifest_text.py` into
   `artifacts/progression-manifest/availability-manifest.txt`) and check the
   section 1 expectations.
3. Commit and push to `claude/eager-feynman-m216tp`.
