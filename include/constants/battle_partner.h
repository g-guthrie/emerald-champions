
#ifndef GUARD_CONSTANTS_BATTLE_PARTNERS_H
#define GUARD_CONSTANTS_BATTLE_PARTNERS_H

#define PARTNER_NONE 0
#define PARTNER_STEVEN 1

// ---------------------------------------------------------------------------
// Meteor Falls 2-vs-2 (MeteorFalls_1F_1R), restored from Inclement Emerald.
// The rival fights alongside the player against Courtney + a Magma grunt, with
// a team that depends on which starter the rival took. Inclement kept these as
// ordinary trainers in include/constants/opponents.h (MAY_TREECKO_METEOR_FALLS
// etc., ids 63/70/87 and 104/113/122) with parties in src/data/trainers.h.
// This engine keeps in-game partners in their own namespace instead, so the
// donor ids are mapped onto PARTNER_* slots here, matching how
// MossdeepCity_SpaceCenter_2F was repointed to PARTNER_STEVEN.
//
// !! The parties themselves are NOT defined yet. src/data/battle_partners.party
// !! needs a `=== PARTNER_MAY_TREECKO_METEOR_FALLS ===` section (and the other
// !! five) or gBattlePartners leaves those slots zeroed and the battle starts
// !! with a partner that has no Pokemon. The donor's teams are in
// !! inclement-game-source/src/data/trainers.h under the ids listed above.
// ---------------------------------------------------------------------------
#define PARTNER_MAY_TREECKO_METEOR_FALLS     2
#define PARTNER_MAY_TORCHIC_METEOR_FALLS     3
#define PARTNER_MAY_MUDKIP_METEOR_FALLS      4
#define PARTNER_BRENDAN_TREECKO_METEOR_FALLS 5
#define PARTNER_BRENDAN_TORCHIC_METEOR_FALLS 6
#define PARTNER_BRENDAN_MUDKIP_METEOR_FALLS  7

#define PARTNER_COUNT 8

// Names used verbatim by the restored data/maps/MeteorFalls_1F_1R/scripts.inc.
#define MAY_TREECKO_METEOR_FALLS     PARTNER_MAY_TREECKO_METEOR_FALLS
#define MAY_TORCHIC_METEOR_FALLS     PARTNER_MAY_TORCHIC_METEOR_FALLS
#define MAY_MUDKIP_METEOR_FALLS      PARTNER_MAY_MUDKIP_METEOR_FALLS
#define BRENDAN_TREECKO_METEOR_FALLS PARTNER_BRENDAN_TREECKO_METEOR_FALLS
#define BRENDAN_TORCHIC_METEOR_FALLS PARTNER_BRENDAN_TORCHIC_METEOR_FALLS
#define BRENDAN_MUDKIP_METEOR_FALLS  PARTNER_BRENDAN_MUDKIP_METEOR_FALLS

#endif  // GUARD_CONSTANTS_BATTLE_PARTNERS_H
