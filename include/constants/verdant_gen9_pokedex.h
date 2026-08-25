#ifndef GUARD_CONSTANTS_VERDANT_GEN9_POKEDEX_H
#define GUARD_CONSTANTS_VERDANT_GEN9_POKEDEX_H

// National-Dex numbers 1-898 retain the original SaveBlock1 bitfields. The
// curated package uses official numbers through 1021, but stores those flags
// separately inside the existing unused Pokedex filler so no save offsets
// move.
#define VERDANT_LEGACY_NATIONAL_DEX_COUNT     NATIONAL_DEX_CALYREX
#define VERDANT_LEGACY_POKEMON_SLOTS_NUMBER  (VERDANT_LEGACY_NATIONAL_DEX_COUNT + 1)
#define VERDANT_GEN9_DEX_FIRST                899
#define VERDANT_GEN9_DEX_LAST                 1021
#define VERDANT_GEN9_DEX_RANGE_COUNT          (VERDANT_GEN9_DEX_LAST - VERDANT_GEN9_DEX_FIRST + 1)
#define VERDANT_GEN9_DEX_FLAG_BYTES           ((VERDANT_GEN9_DEX_RANGE_COUNT + 7) / 8)
#define VERDANT_GEN9_DEX_SAVE_MAGIC           0x56473931 // "1 9 G V" in little-endian storage
#define VERDANT_AVAILABLE_NATIONAL_DEX_COUNT  (VERDANT_LEGACY_NATIONAL_DEX_COUNT + 34)

#define NATIONAL_DEX_SPRIGATITO               906
#define NATIONAL_DEX_FLORAGATO                907
#define NATIONAL_DEX_MEOWSCARADA              908
#define NATIONAL_DEX_FUECOCO                  909
#define NATIONAL_DEX_CROCALOR                 910
#define NATIONAL_DEX_SKELEDIRGE               911
#define NATIONAL_DEX_NACLI                    932
#define NATIONAL_DEX_NACLSTACK                933
#define NATIONAL_DEX_GARGANACL                934
#define NATIONAL_DEX_FINIZEN                  963
#define NATIONAL_DEX_PALAFIN                  964
#define NATIONAL_DEX_DONDOZO                  977
#define NATIONAL_DEX_TATSUGIRI                978
#define NATIONAL_DEX_ANNIHILAPE               979
#define NATIONAL_DEX_FARIGIRAF                981
#define NATIONAL_DEX_KINGAMBIT                983
#define NATIONAL_DEX_GREAT_TUSK               984
#define NATIONAL_DEX_FLUTTER_MANE             987
#define NATIONAL_DEX_IRON_BUNDLE              991
#define NATIONAL_DEX_IRON_HANDS               992
#define NATIONAL_DEX_GLIMMET                  969
#define NATIONAL_DEX_GLIMMORA                 970
#define NATIONAL_DEX_GIMMIGHOUL               999
#define NATIONAL_DEX_GHOLDENGO                1000
#define NATIONAL_DEX_CHIEN_PAO                1002
#define NATIONAL_DEX_TING_LU                  1003
#define NATIONAL_DEX_CHI_YU                   1004
#define NATIONAL_DEX_ROARING_MOON             1005
#define NATIONAL_DEX_IRON_VALIANT             1006
#define NATIONAL_DEX_WALKING_WAKE             1009
#define NATIONAL_DEX_OGERPON                  1017
#define NATIONAL_DEX_ARCHALUDON               1018
#define NATIONAL_DEX_GOUGING_FIRE             1020
#define NATIONAL_DEX_RAGING_BOLT              1021

#endif // GUARD_CONSTANTS_VERDANT_GEN9_POKEDEX_H
