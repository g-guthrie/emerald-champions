#ifndef GUARD_LANDMARK_H
#define GUARD_LANDMARK_H

const u8 *GetLandmarkName(mapsec_u8_t mapSection, u8 id, u8 count);
u32 GetLandmarkPlaces(mapsec_u16_t mapSection, mapsec_u16_t *places, u32 max);
#if TESTING
bool32 Test_LandmarkPlacesMatchTheirSections(void);
#endif

#endif // GUARD_LANDMARK_H
