# Legendary Signs and Champions Circuit checkpoint

## Legendary acquisition layer

Emerald Champions tracks 53 additional legendary, mythical, and Ultra Beast
acquisitions without adding a quest-log UI. Existing Hoenn NPCs and locations
carry the discovery layer:

- The Devon dream researcher reveals conditional wild Signs once the player
  has the required badge/story state and brings the paired species.
- The Mt. Pyre storyteller owns the Darkrai to Cresselia to Dialga visible
  encounter chain.
- Manaphy and Ditto produce Phione through the native Day Care.
- Genesect and Poipole replace the now-redundant TM reward counter at Mauville
  Game Corner and are one-time prizes.
- Seventeen endgame species are Champions Circuit milestones; Eternatus is the
  mastery reward.
- Capturing every conditional and visible Sign unlocks Arceus.

Hoopa is on permanent Route 111 rather than collapsible Mirage Tower. Tapu
Bulu and Xerneas were moved out of the Safari Zone, so no mastery acquisition
is forced through Safari flee, ball, or step rules. Visible encounters use the
modern expansion's native follower graphics for Darkrai, Cresselia, and
Dialga. Every reward level is clamped to `MAX_LEVEL`.

## Live Showdown Champions Circuit

The Circuit does not choose from preseeded teams. Every match is generated in
the ROM when the player requests it.

Pinned source:

- Pokemon Showdown commit:
  `bb179fbf8449e3c31632bd56f671ffb4404fa6e7`
- Champions doubles data SHA-256:
  `851114e68805aafbecbed0aaee7994164d199d890deed78bafc2beaf3c2221d8`
- Source pool: 311 species/form variants and 444 role templates.

The C port preserves the defining Showdown allocator behavior: uniform
base-species sampling, species clause, Mega-form preference before the team's
single Mega slot is spent, two-per-type limit, weakness and double-weakness
limits, Freeze-Dry pressure, conflicting weather exclusions, screen/web and
Lightning Rod compatibility, team-detail tracking, role-enforced moves,
Ability culling, and Champions doubles item rules. GBA-specific adaptations
are the Circuit's level escalation, Emerald battle presentation and AI, and
the campaign's Mega-only selectable-gimmick contract.

Player teams are temporarily normalized to level 80. Opponents begin at level
80; after each victory one additional slot rises by one level, then the whole
team advances. HP and held loadouts restore between matches. The original
party is restored on defeat or retirement. The generator uses dedicated event
variables and never writes Battle Tower records.

Circuit species rewards arrive every two wins; all 17 are available by win 34.
Eternatus requires those rewards plus win 40. The random circuit remains
playable indefinitely after mastery.

## Current evidence

- Static acquisition and source-lock verifier: passing.
- Production build: passing.
- ROM: 26,829,684 / 33,554,432 bytes (79.96 percent).
- EWRAM: 227,060 / 262,144 bytes (86.62 percent).
- IWRAM: 28,388 / 32,768 bytes (86.63 percent).
- Multi-seed mGBA generator and representative overworld/save tests remain
  release gates rather than assumed complete.
