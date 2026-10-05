"""Walkthrough order: which areas a route trainer's player may draw on.

Owner rule: a route trainer is tuned against what a player walking the
guidebook order has passed - every chapter up to and including the trainer's
own area - not against everything reachable by skipping trainers to a later
route. Gym, League and Champion battles use everything reachable when the
player walks in (the full encounter pool). Story gates (HMs, events) still
come from the reachability model; this only narrows it.

A battle's chapter is the later of its map's chapter and the frontier the
story has reached by its first milestone, so an area revisited later (Trick
House puzzles, hideouts) sees what the player has walked through since.
"""
from __future__ import annotations

# Guidebook order. Each entry is a chapter: map names or map-name prefixes
# (a prefix also covers its interiors, e.g. "PetalburgCity" covers
# PetalburgCity_Gym). The longest matching prefix wins.
CHAPTERS: list[tuple[str, ...]] = [
    ("InsideOfTruck", "LittlerootTown"),                                   # 0
    ("Route101",),                                                         # 1
    ("OldaleTown",),                                                       # 2
    ("Route103", "AlteringCave"),                                          # 3
    ("Route102",),                                                         # 4
    ("PetalburgCity",),                                                    # 5
    ("Route104", "PetalburgWoods"),                                        # 6
    ("RustboroCity",),                                                     # 7
    ("Route116", "RusturfTunnel", "TerraCave"),                            # 8
    ("Route115", "Seaspray"),                                              # 9
    ("DewfordTown", "DewfordManor", "DewfordMeadow", "GraniteCave"),       # 10
    ("Route106", "Route107", "Route105", "IslandCave"),                    # 11
    ("Route109", "SlateportCity", "Route108", "AbandonedShip"),            # 12
    ("Route110", "NewMauville"),                                           # 13
    ("MauvilleCity",),                                                     # 14
    ("Route117", "VerdanturfTown", "VerdanturfMeadow"),                    # 15
    ("Route111", "Route112", "FieryPath", "TrainerHill", "MirageTower",
     "DesertRuins"),                                                       # 16
    ("Route113", "FallarborTown", "Route114", "MeteorFalls",
     "DesertUnderpass", "SandstrewnRuins"),                                # 17
    ("MtChimney", "JaggedPass", "EmberPath", "AshenWoods", "LavaridgeTown",
     "ScorchedSlab"),                                                      # 18
    ("Route118", "Route119"),                                              # 19
    ("FortreeCity", "Route120", "AncientTomb"),                            # 20
    ("Route121", "SafariZone", "LilycoveCity", "ContestHall", "MtPyre",
     "Route122"),                                                          # 21
    ("AquaHideout", "MagmaHideout"),                                       # 22
    ("Route123", "Route124", "MossdeepCity", "ShoalCave", "Underwater",
     "Route125", "Route126", "Route127", "Route128", "SeafloorCavern",
     "MarineCave", "ArtisanCave"),                                         # 23
    ("SootopolisCity", "CaveOfOrigin", "SkyPillar", "Route129", "Route130",
     "Route131", "Route132", "Route133", "Route134", "PacifidlogTown",
     "SealedChamber"),                                                     # 24
    ("EverGrandeCity", "VictoryRoad"),                                     # 25
    ("SSTidal", "SouthernIsland", "BirthIsland", "FarawayIsland",
     "NavelRock", "BattleFrontier"),                                       # 26
]

# The chapter the story frontier has reached when each milestone window
# opens (src/caps.c windows, reference_pool milestone names).
FRONTIER = {
    "start": 0,       # new game
    "badge1": 7,      # Roxanne, Rustboro
    "badge2": 10,     # Brawly, Dewford
    "badge3": 14,     # Wattson, Mauville
    "badge4": 18,     # Flannery, Lavaridge
    "badge5": 18,     # Norman (back in Petalburg; Lavaridge already walked)
    "badge6": 20,     # Winona, Fortree
    "groudon": 22,    # hideouts done
    "badge7": 23,     # Tate & Liza, Mossdeep
    "badge8": 24,     # Juan, Sootopolis
    "champion": 26,   # after the League
}


def chapter_of(map_name: str) -> int | None:
    best, best_len = None, -1
    for index, prefixes in enumerate(CHAPTERS):
        for prefix in prefixes:
            if (map_name == prefix or map_name.startswith(prefix)) and len(prefix) > best_len:
                best, best_len = index, len(prefix)
    return best


def uses_full_pool(map_name: str | None) -> bool:
    """Gyms, the League and the Champion's room draw on everything reachable."""
    if not map_name:
        return True
    return "_Gym" in map_name or map_name.startswith("EverGrandeCity_")


def allowed_maps(map_name: str | None, milestone: str, all_maps) -> set[str] | None:
    """Maps a route trainer's player may draw on, or None for the full pool."""
    if uses_full_pool(map_name):
        return None
    own = chapter_of(map_name)
    if own is None:
        return None
    limit = max(own, FRONTIER.get(milestone, 0))
    return {m for m in all_maps if (c := chapter_of(m)) is not None and c <= limit}
