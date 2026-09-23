# Flight Beacon and region map pass — September 19, 2026

Partial review: item-use lock/location/follower checks, party/PC rider selection,
Bag and registered-item callbacks, region-map destination classification, selection,
exit/cancel ownership and special home/League landing points. Full region-map
projection/zoom/icon rendering and every landing location remain to audit.

Corrected Beacon refusal text: unlock requires both Feather Badge and Fly
permission, while the old text told every rejected player to earn the badge.
The new message names both requirements without implying a compatible party is
needed. It does not change unlock behavior or inventory.

Native test passes both player-home variants and Ever Grande's locked/unlocked
League landmark and city tile. Source tracing confirms confirmed flight resolves
before map resource release; cancel uses the supplied Bag/field callback and
clears the rider override. The latter is source evidence only, not a fresh UI
replay. Southern Island remains a ferry route rather than a freely selectable
Fly city; no unintended unlock was added.

Evidence: work/fly-audit-20260919/tests.log and release build/gate logs. AI,
authored battles, field access and save layout unchanged.
