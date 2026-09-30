#ifndef GUARD_CENTER_GUIDE_H
#define GUARD_CENTER_GUIDE_H

// The Pokémon Center local guide. Common_EventScript_CenterLocalGuide
// (data/scripts/emerald_champions.inc) selects a topic in VAR_0x8005 and
// calls BufferNextCenterLegendaryLead, which dispatches here for the
// non-legend topics. Keep these values in sync with the .set lines there.
#define CENTER_GUIDE_TOPIC_LEGENDS 0
#define CENTER_GUIDE_TOPIC_TIPS    1
#define CENTER_GUIDE_TOPIC_STORY   2

// VAR_0x8004 is the cursor (start at 0). Fills gStringVar4 with the next
// local side-quest tip for this city; VAR_RESULT = FALSE when none remain.
void BufferNextCenterGuideTip(void);
// Fills gStringVar4 with the next mandatory destination; VAR_RESULT = TRUE
// for all campaign and finale stages. Shared finale callers use the same tree.
void BufferCenterGuideDirections(void);
// Brief next objective from the same progression tree; no puzzle solution.
void BufferCenterGuideObjective(void);

#endif // GUARD_CENTER_GUIDE_H
