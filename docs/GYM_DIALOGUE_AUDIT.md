# Gym dialogue audit — September 8, 2026

## Subsequent approved team upgrades

The dialogue-only passes below describe the initial audit. The user then approved
targeted strategy upgrades: Maura now leads Perish Song Jynx and Shadow Tag
Gothitelle, and Parker leads Earthquake Lickilicky with Telepathy/Instruct
Oranguru. Their introductions and advice were updated alongside those loadouts.
Maura explains the countdown and Ghost/Soundproof/pivot counterplay. Parker
explains repeated Earthquake, Telepathy and Wide Guard while acknowledging his
single-target alternatives. Thus the earlier statements that dialogue repairs
did not change teams are not a claim of final roster parity with 5.1.

## First five gyms: bounded source repair

Compared attached trainer calls in Rustboro, Dewford, Mauville, both Lavaridge gym floors and Petalburg with current authored teams and their materialized `src/data/trainers.h` entries. This covers **34 first-battle trainer IDs and 204 Pokémon**: 4 Rustboro, 7 Dewford, 6 Mauville, 9 Lavaridge and 8 Petalburg trainers. Species, held items, abilities and four moves agree between authoring and native data for every inspected member. No team or mechanic was changed.

Most current species-specific explanations already match the teams. Corrections are local, retaining the original introductions and voices where possible:

| Gym | Demonstrated problem and correction |
| --- | --- |
| Rustboro | Josh's postbattle Sturdy explanation named Nosepass, absent from his team. It now names his Sturdy Bonsly. His defeat line no longer claims the player beat a particular chip-damage plan. |
| Dewford | Lilith's “weaken me” boast implied stat-drop punishment, but her team has neither Defiant nor Competitive. It now describes creating openings. Brenden names No Guard and Mold Breaker instead of the unqualified claim that Machop never misses; Protect and other non-accuracy defenses are not promised away. |
| Mauville | Ben and Vivian no longer claim the player reversed their current or bypassed every immunity. Their defeat lines acknowledge the loss without pretending to track the winning method. |
| Lavaridge | Cole's contact-burn warning is conditional. Axle names Armarouge's Weakness Policy rather than claiming every hard hit powers him up. Jace identifies Toxic, avoiding an implication that Poison damage itself bypasses Steel. Jeff's unconditional first-move claim now describes Talonflame's full-health Flying priority. Eli now describes his actual Levitate Claydol and Heatproof Bronzong instead of a nonexistent aim-dodging plan; his defeat line no longer claims he never controlled the battle. Gerald's inaccurate magma-temperature and guaranteed-survival lecture is replaced with his actual Chlorophyll/sun partnership. |
| Petalburg | Norman acknowledges losing with his carefully raised team rather than claiming the player read every mechanic. His existing 215-pixel badge sentence is split into two lines. |

Generic congratulations and figurative defeat reactions were retained. Ordinary dialogue does not inspect the player's battle decisions; it should explain an opponent's available plan, not certify that a particular counter occurred. The existing guide explanations for Roxanne's Trick Room/Wide Guard, Brawly's redirection, Wattson's Electric partnership, Flannery's sun/After You, and the named Petalburg room strategies were checked against the current teams, not historical design prose.

### Verification and limits

- An in-memory source check followed object/map-script references and found all 34 trainer calls in the local reference graph; local definitions remain unique. No labels, trainer IDs, branches or commands changed.
- Compared all 204 native species/item/ability/move tuples with their authored branches. Omitted native held items correctly count as `ITEM_NONE`.
- Measured all **44 visible lines in the 15 edited text blocks** using `gFontNormalLatinGlyphWidths` from `src/fonts.c` and `charmap.txt`, via the existing `scripts/audit/textwidth.py` reader. Maximum width is **201 pixels**, below the 208-pixel limit. No new test framework or permanent regression gate was added.
- `git diff --check` passes.

This is source attachment, roster and font-metric evidence, **not** a fresh ROM walkthrough, rendered-scene inspection, a complete rematch roster audit or proof that every AI strategy executes. Runtime strategy and build verification remain separate. Fortree, Mossdeep and later gyms are outside this section's scope.

## Fortree and Mossdeep

The later gyms contained substantially stale strategy descriptions. Corrections
were checked against the current canonical teams, not the book's descriptions:

- Jared's Noctowl has no sleep move and Swoobat has Psychic, not Stored Power.
  Flint has no Tailwind user. Ashley's Pelipper, not Swanna, sets rain; changing
  weather does not disable Mega Pidgeot's No Guard.
- Humberto has neither Minior nor Shell Smash. His advice now describes the
  Gliscor/Corviknight defenses. Darius has no redirection move; his dialogue now
  describes the actual Tornadus/Braviary speed-control partnership.
- Preston's Magic Guard does not prevent every price or effect. Virgil leads
  Bronzong/Musharna, not Medicham; Protect does not prevent Trick Room.
  Blake's Girafarig has no Thunder Wave, and Hannah has no screens or Azelf.
- Samantha has Magic Bounce Espeon, not Xatu. Maura has no Ally Switch.
  Sylvia has no Torkoal/Oranguru: her Dondozo/Tatsugiri pair is a reserve phase,
  and the fish cannot be targeted while Commander is active.
- Kathleen has no Swoobat/Stored Power. Macey's Delphox has Blaze, not Magician,
  and her team has no sleep move. Nicholas has neither Trick Room nor Follow Me.
  Tate and Liza's finishing Mega is Gardevoir, not an ice rider.
- Static defeat lines no longer claim specific unobserved counters. Buffel's
  motel defeat line received the same narrow correction.

All added lines in these three map scripts measure at most 208 pixels with the
native normal-font metrics. The static map/script reference check passes after
the edits. No Pokémon, moves, abilities, trainer IDs, branches or rewards were
changed. This does not establish that their AI executes every described plan;
authored battle runtime checks are reported separately.

## Sootopolis and the League

Checked both Sootopolis gym floors and all five League rooms: **16 attached first-battle trainer IDs and 96 Pokémon**. Their native species, items, abilities and moves match the current authored branches. The ten basement trainers' generic dialogue did not warrant a rewrite.

- Juan has rain, Tailwind and setup, but no trapping or Trick Room. His introduction now reflects that team, and his defeat does not claim the player denied a particular final rain turn.
- Sidney's actual Tailwind replaces the suggestion of a reverse-speed mode. His defeat acknowledges losing rather than seeing every trick successfully countered.
- Phoebe has Marshadow's boost theft and several burn moves, but no Wide Guard, Shedinja or shadow rider. Her warning now names Spectrier and its knockout-dependent power. Her Glacia advice no longer demands one universal weather counter.
- Glacia has Baxcalibur, not a slow Ice horse. Her dialogue distinguishes changing weather from removing Aurora Veil: the existing screen can outlast snow. Her defeat no longer invents a specific weather/screen sequence. Her advice about Drake no longer promises two speed modes.
- Drake has no Dialga, Trick Room or setup moves. Salamence supplies Tailwind, Koraidon supplies sun for Reshiram, and Garchomp holds his Mega Stone. His introduction and defeat no longer describe the retired two-clock plan or present lead Salamence as the final Mega.
- Wallace no longer claims the player crossed the League without healing: healing moves, abilities and held items are distinct from prohibited Bag item use. His introduction avoids guarantees that every tool was available without effort. His defeat does not pretend to track every answer, and his victory speech does not require the player to have lost or rebuilt teams. Both rivals' advice now warns about remaining partners instead of claiming an automatic pace reset every turn.

Only **16 text blocks** changed across six scripts; no game commands, labels, teams or mechanics changed. All local definitions remain unique, the source reference graph reaches all 16 trainer IDs and all edited text labels, and **76 visible lines** measure at most **207 pixels** using the actual normal-font glyph widths and charmap. `git diff --check` passes. This is static/native-data and font-metric evidence, not rendered gameplay or a complete rematch audit. Game-source edits stopped after this pass for the integrated build.
