# Player avatar pass — September 20, 2026

Partial source review: state-mask replacement/preserved control flags, gender/
graphics tables, position/facing helpers, Surf eligibility, dismount task and
forced-movement entrypoints. No demonstrated movement-rule defect in this scope.
Reused PlayerGetDestCoords for the facing-position helper; updated Surf comment
to match badge plus authorization, not party move compatibility.

Three native groups pass: all25 pairwise walk/Mach/Acro/Surf/Dive mode changes
preserve dash/forced/control bits; male/female graphics round-trip across nine
presentations; four facing offsets leave actual player coordinates unchanged.
Evidence: work/avatar-audit-20260920/. These test state/helper behavior, not
actual obstacle traversal, surf animation, ledge collisions or battle return.

Remaining: native movement/collision/warp sequences, forced-tile tasks, follower
handoffs, and full return-from-battle/underwater graphical state. The sourcepass
is partial and does not certify the whole field_player_avatar.c file as perfect.
AI, gameplay rules and save layout unchanged.
