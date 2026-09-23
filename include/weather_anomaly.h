#ifndef GUARD_WEATHER_ANOMALY_H
#define GUARD_WEATHER_ANOMALY_H

#include "legendary_signs.h"

// Weather anomalies: from the Weather Institute rescue until the sky calms in
// Sootopolis, up to four visiting legends are wild on their home maps, each
// for about 1500 steps under its own weather. See src/weather_anomaly.c.
#define WEATHER_ANOMALY_SLOT_COUNT        4
#define WEATHER_ANOMALY_DURATION_STEPS    1500
#define WEATHER_ANOMALY_TICK_STEPS        50
#define WEATHER_ANOMALY_DURATION_TICKS    (WEATHER_ANOMALY_DURATION_STEPS / WEATHER_ANOMALY_TICK_STEPS)
#define WEATHER_ANOMALY_ENCOUNTER_PERCENT 20
#define WEATHER_ANOMALY_EMPTY             0xFF

#define WEATHER_ANOMALY_WINDOW_START_FLAG FLAG_HIDE_ROUTE_119_TEAM_AQUA
#define WEATHER_ANOMALY_WINDOW_END_FLAG   FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE

bool32 IsWeatherAnomalyWindowOpen(void);
bool32 IsWeatherAnomalyVisitor(enum LegendarySignId signId);
// Called once per player step, right after GAME_STAT_STEPS is incremented.
void UpdateWeatherAnomaliesOnStep(void);
// Live slot contents: a LegendarySignId, or WEATHER_ANOMALY_EMPTY.
u8 GetWeatherAnomalySlotSignId(u32 slot);
u16 GetWeatherAnomalySlotStepsRemaining(u32 slot);
// Direct slot write (debug and tests). steps is rounded up to whole ticks.
void SetWeatherAnomalySlot(u32 slot, u8 signId, u16 steps);
u8 GetWeatherAnomalyCooldownSignId(void);
void ClearWeatherAnomalies(void);
bool32 IsWeatherAnomalyLive(enum LegendarySignId signId);
// The live anomaly on a map, or LEGENDARY_SIGN_COUNT.
enum LegendarySignId GetLiveWeatherAnomalyOnMap(u8 mapGroup, u8 mapNum);
// The anomaly weather of the current map, or WEATHER_NONE.
u8 GetWeatherAnomalyWeatherForCurrentMap(void);
// A visitor's own table slot is inert until the window has closed.
bool32 IsWeatherAnomalyVisitorSlotInert(enum Species species);
// The flat 20% visitor roll for a land/water encounter on the current map.
enum Species TryRollWeatherAnomalyEncounter(enum WildPokemonArea area);
// Special: gStringVar4 = "{route}: {legend}" per live anomaly, VAR_RESULT = count.
void BufferWeatherAnomalyReport(void);

#endif // GUARD_WEATHER_ANOMALY_H
