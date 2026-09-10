# Native wild legendaries — September 10

The approved change converts 39 former rare discoveries with no requirement
beyond badges into repeatable wild residents. Both their badge count and badge
flag are removed. Their habitat remains unchanged, their ordinary encounter
rate is 3% each, and their levels/preparation follow the normal wild creation
path. Capturing them records ownership without removing future encounters.

Sweet Scent has one 25% total legendary roll, shared evenly among all eligible
native residents and quest discoveries in the current habitat. It does not
bypass quest requirements. The 15 remaining rare-wild quest/story/capture
entries keep their existing gates, one-time capture behavior and 1% standard
encounter rates. Visible scenes, existing ordinary Ultra Beast tables and
special rewards are preserved.

All separate legendary selection menus are removed, including Devon's guide.
Area signs show the ordinary species roster, then native legendaries after the
rod sections. Remaining local wild requirements are automatic text pages with
no selection prompt. Twenty habitats without roster signs now have ordinary
species signs, reusing unoccupied former landmark locations. Quest NPCs speak
and resolve their quests directly. Related dialogue reflects the new rates.

Birch's first post-rival return acknowledges the win, gives 10 Great Balls once,
and directs the player through Route 102 to their father in Petalburg. If the
Bag is full, delivery uses the item PC; if both are full, the event remains
pending instead of losing the gift.

Verification records are in `work/native-wild-update/`:

- `native-test.log`: native production tests exhaust the 100 standard/Sweet
  Scent chance outcomes on Route 110 before and after quest eligibility,
  enumerate the Sweet Scent species choices, confirm repeat captures, and
  check all 39 species appear in their habitat's sign buffer.
- `quest-test.log`: native Meloetta/Landorus quest eligibility and persistence.
- `sign-placement-check.txt`: matching tile elevations, no warp/coordinate
  event overlap, and local floor connectivity around each added sign. This
  source check does not establish visual appearance or full traversal.
- `build.log` and `verification.log`: release build and required artifact gates.

This is focused verification, not a complete campaign playthrough or visual
playtest of every sign and the Birch scene.
