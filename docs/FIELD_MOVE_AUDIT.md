# Field-move integration pass — September 19, 2026

Reviewed field_move.c's full unlock/party-user selection and metadata table,
inline accessors, party menu visibility, and Flash/Dig/Braille call paths.
Consolidated HM classification with the existing eight-license table instead of
maintaining a second list in the header. Preserved eight exact HM identities,
Emerald badge+license rules, FRLG badge-only behavior, optional species-based
non-HM moves and move visibility. Removed a stale comment about a deleted party
replacement helper. No access or puzzle requirements changed.

Four existing native groups pass: all eight HM badge/license/empty-party cases,
regional Flash behavior, badge-versus-license message selection, and tested Regi
conditions (Wailord/Relicanth order; Dig/Regirock/Registeel positions and completion
flags). Evidence: work/hm-audit-20260919/. These are engine checks, not full native
visual traversal of every obstacle. Regice path/choreography and all blocked-map
failure messages still need broader campaign coverage. AI unchanged.

September 23 rule update: all eight HM actions now require a compatible
non-Egg party member in addition to their badge and license; the move need not
occupy a moveslot. The Flight Beacon may use a compatible party or PC member.
The Sealed Chamber still requires a Pokémon that can actually use Dig.

## September 23 full HM-system audit

**Rule and source paths.** The eight entries in `src/field_move.c` require their
Emerald story license, matching Badge, and a non-Egg party species able to learn
the field move. The species need not know it. `checkfieldmove` supplies the party
slot to object scripts and field animations. Cut, Rock Smash, and Strength use
shared obstacle scripts; their registered map objects number 89 trees across 33
maps, 182 rocks across 34 maps, and 122 boulders across 27 maps, including donor
maps. These are source counts, not claims of 393 native traversals. Surf,
Waterfall, and Dive use the common terrain-input dispatcher. Flash applies on
cave entry only with an eligible party member. Fly is hidden from the party
menu; the Beacon can select a compatible party or PC species after the Fly
license and Feather Badge. Dig remains a learned-move requirement at the Sealed
Chamber. Teleport and Sweet Scent retain their explicit capability fallback.

| Move | License source | Badge | Native action evidence |
|---|---|---|---|
| Cut | Rustboro Cutter | Stone | Route 104 tree cleared with untaught Zigzagoon; incompatible Magikarp refused |
| Flash | Granite Cave hiker | Knuckle | Granite Cave bright/dark comparison; Ancient Tomb door opened |
| Rock Smash | Mauville Rock Smash Dude | Dynamo | Route 111 rock cleared; Desert Ruins door opened |
| Strength | Rusturf Tunnel rescue | Heat | Fiery Path Linoone activated Strength and pushed its boulder |
| Surf | Wally's family after Norman | Balance | Route 118 Marill entered water; incompatible Magikarp refused |
| Fly | Route 119 rival | Feather | boxed Wingull without Fly opened Beacon flight; B cancel returned to field and cleared rider override |
| Dive | Steven after Mossdeep events | Mind | Route 126 down and B surface both warped correctly; incompatible Magikarp refused |
| Waterfall | Wallace after weather crisis | Rain | Ever Grande Wailmer climbed from y68 to y59; incompatible Magikarp remained below |

Scene recipes, native recordings, contact sheets, and result JSON live in
`work/studio/hm-20260923/`. Each successful result was checked for
`outcome.passed`, and the contact sheets were inspected. The two Regi door
scenes validated their puzzle completion bits. The Dive destination tests
also cover the connectionless Abandoned Ship and the Sealed Chamber's special
surface tile and eight neighboring Route 134 returns. The Fly destination test
covers home identity and Ever Grande's League landmark.

**Corrections.** Active HM dialogue now describes the compatible-party rule;
Steven and the Center explain B to surface; the Rock Smash Dude no longer
promises item or Fossil drops that the current encounter script cannot make.
Cut now has a party-menu action for compatible Pokémon after its unlock,
restoring Inclement's optional grass clearing while trees still work by A.
Native Route 101 grass was cleared without a Cut moveslot; on Faraway Island
the same action triggered Mew's existing watched-feeling response.
The Strength visual fixture now uses Linoone, which can learn Strength, instead
of Zigzagoon, which cannot. The Fly map's allocation-failure branch used a
fatal allocator and could not recover; it now uses `AllocUnchecked`, returns to
the caller's Bag/field callback, and clears the selected rider. A focused native
test forces that allocation failure and passes.

**Boundary.** This audits the shared system and one native traversal for each
action type. It does not claim every registered obstacle was walked, every
gift conversation was replayed from earned progression, every Fly destination
was flown to, or every follower configuration was exercised. Regice's perimeter
walk is a separate puzzle, not an HM action. Those are campaign-play coverage
items, not an outstanding HM unlock discrepancy.
