# Center guides and Mart counters — September 10

All 16 city/town Centers (including Ever Grande and the Frontier) have the same
local guide position at (14, 4), near the PC. Petalburg reuses its profile man;
the other Centers receive a matching guide. The existing move tutor and battle
supply vendor remain available. Guide conversations use direct text pages, with
no choices or legendary menus.

The 47 local leads cover all 15 quest/story/capture-gated wild discoveries,
nearby visible encounters, regional rewards, ordinary wild examples, and the
original legendary scenes. Lilycove's island travel advice is grouped into one
passage with a count of the six associated species already caught. Center
assignments and wording live in `src/data/pokemon/center_legendary_leads.h`.
Shared runtime progression/discovery predicates drive gated status text. Guides
never unlock a quest. Caught entries acknowledge completion; native wild entries
explain that additional captures remain possible. The old Meteor Falls reference
to Devon's removed menu now points to the Fallarbor Center guide.

Seven duplicate Mart clerks were removed: Petalburg, Slateport, Mauville,
Verdanturf, Fallarbor, Lavaridge, and Fortree. Stock and shop progression scripts
were not changed. The shared Mart layout's counter is one row shorter, leaving
open floor where the extra workstation stood. The remaining clerk's floor and
counter interaction tile are preserved. Existing department-store specialists
are unaffected.

Verification records in `work/center-guides-marts/`:

- `guide-test.log`: native production test checks the locked, unlocked and
  caught Meloetta dialogue, confirms that talking does not unlock the quest,
  checks Oldale's native sighting, then visits all 16 city lead lists to check
  termination, nonempty dialogue, buffer length and actual font line widths.
- `placement-check.txt`: guide positions, collision/elevation, tutor retention,
  duplicate-clerk removal, coverage of all 15 gated wild discoveries, and Mart
  NPC floor/counter integrity.
- `mart-layout.png`: inspected tile/palette reconstruction of the edited layout.
  This is layout inspection, not an emulator screenshot with actors.
- `build.log`, `verification.log`, `boot.log`: final release build, required
  checks, and emulator boot check.

These checks do not constitute a full campaign playthrough or native visual
interaction with every Center guide and Mart clerk.
