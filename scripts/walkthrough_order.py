"""Walkthrough order: which areas a route trainer's player may draw on.

Owner rule: a route trainer is tuned against what a player walking the
guidebook order has passed - every area before the trainer's own route, never
that route's own new Pokemon or items - not against everything reachable by
skipping trainers to a later route. Gym, League and Champion battles use everything reachable when the
player walks in (the full encounter pool). Story gates (HMs, events) still
come from the reachability model; this only narrows it.

A battle sees every area before its own, or up to the frontier the story
has reached by its first milestone if that is later, so an area revisited later (Trick
House puzzles, hideouts) sees what the player has walked through since.
"""
from __future__ import annotations

# Guidebook order, one area per entry: map names or map-name prefixes (a
# prefix also covers its interiors, e.g. "PetalburgCity" covers
# PetalburgCity_Gym). The longest matching prefix wins.
AREAS: list[tuple[str, ...]] = [
    ("InsideOfTruck", "LittlerootTown"), ("Route101",), ("OldaleTown",), ("Route103",),
    ("Route102",), ("PetalburgCity",), ("Route104",), ("PetalburgWoods",), ("RustboroCity",),
    ("Route116",), ("RusturfTunnel",), ("Route115", "Seaspray"), ("DewfordTown", "DewfordManor", "DewfordMeadow"),
    ("GraniteCave",), ("Route106",), ("Route107",), ("Route105",), ("IslandCave",), ("Route109",),
    ("SlateportCity",), ("Route108",), ("AbandonedShip",), ("Route110",), ("NewMauville",), ("MauvilleCity",),
    ("Route117",), ("VerdanturfTown", "VerdanturfMeadow"), ("TerraCave",), ("Route111",), ("Route112",),
    ("FieryPath",), ("TrainerHill",), ("MirageTower",), ("DesertRuins",), ("Route113",), ("FallarborTown",),
    ("Route114",), ("MeteorFalls",), ("DesertUnderpass",), ("SandstrewnRuins",), ("MtChimney",),
    ("JaggedPass",), ("EmberPath",), ("AshenWoods",), ("LavaridgeTown",), ("ScorchedSlab",), ("Route118",),
    ("Route119",), ("FortreeCity",), ("Route120",), ("AncientTomb",), ("Route121",), ("SafariZone",),
    ("LilycoveCity",), ("ContestHall",), ("MtPyre",), ("Route122",), ("AquaHideout",), ("MagmaHideout",),
    ("Route123",), ("Route124",), ("MossdeepCity",), ("ShoalCave",), ("Underwater",), ("Route125",),
    ("Route126",), ("Route127",), ("Route128",), ("SeafloorCavern",), ("MarineCave",), ("ArtisanCave",),
    ("SootopolisCity",), ("CaveOfOrigin",), ("SkyPillar",), ("Route129",), ("Route130",), ("Route131",),
    ("Route132",), ("Route133",), ("Route134",), ("PacifidlogTown",), ("SealedChamber",), ("AlteringCave",),
    ("EverGrandeCity", "VictoryRoad"), ("SSTidal", "SouthernIsland", "BirthIsland", "FarawayIsland",
                                        "NavelRock", "BattleFrontier"),
]


def _index(prefix: str) -> int:
    return next(i for i, area in enumerate(AREAS) if prefix in area)


# The area the story has walked through when each milestone window opens
# (src/caps.c windows, reference_pool milestone names); a revisited area
# (Trick House puzzles, hideouts) sees everything up to here.
FRONTIER = {
    "start": 0,
    "badge1": _index("RustboroCity"),     # Roxanne
    "badge2": _index("DewfordTown"),      # Brawly
    "badge3": _index("MauvilleCity"),     # Wattson
    "badge4": _index("LavaridgeTown"),    # Flannery
    "badge5": _index("LavaridgeTown"),    # Norman (Lavaridge already walked)
    "badge6": _index("FortreeCity"),      # Winona
    "groudon": _index("MagmaHideout"),    # hideouts done
    "badge7": _index("MossdeepCity"),     # Tate & Liza
    "badge8": _index("SootopolisCity"),   # Juan
    "champion": _index("SSTidal"),        # after the League
}


def chapter_of(map_name: str) -> int | None:
    best, best_len = None, -1
    for index, prefixes in enumerate(AREAS):
        for prefix in prefixes:
            if (map_name == prefix or map_name.startswith(prefix)) and len(prefix) > best_len:
                best, best_len = index, len(prefix)
    return best


def uses_full_pool(map_name: str | None) -> bool:
    """Gyms, the League and the Champion's room draw on everything reachable."""
    if not map_name:
        return True
    return "_Gym" in map_name or map_name.startswith("EverGrandeCity_")


# The first rival battle: so little comes before Route 103 that its own route
# counts too.
OWN_ROUTE_COUNTS = {f"TRAINER_{rival}_ROUTE_103_{starter}"
                    for rival in ("MAY", "BRENDAN") for starter in ("TREECKO", "TORCHIC", "MUDKIP")}

# Trainers met after a later area than their own map's: Route 104's north half
# is reached through Petalburg Woods, so its trainers have walked the Woods.
WALKED_THROUGH = {
    **{f"TRAINER_WINSTON_{n}": "PetalburgWoods" for n in range(1, 6)},
    **{f"TRAINER_GINA_AND_MIA_{n}": "PetalburgWoods" for n in range(1, 3)},
}


def allowed_maps(map_name: str | None, milestone: str, all_maps, trainer: str | None = None) -> set[str] | None:
    """Maps a route trainer's player may draw on, or None for the full pool."""
    if uses_full_pool(map_name):
        return None
    own = chapter_of(map_name)
    if own is None:
        return None
    # The trainer's own route never counts: only the areas walked before it,
    # or the story frontier for an area revisited later.
    walked = _index(WALKED_THROUGH[trainer]) if trainer in WALKED_THROUGH else own - 1
    limit = max(own if trainer in OWN_ROUTE_COUNTS else walked, FRONTIER.get(milestone, 0))
    return {m for m in all_maps if (c := chapter_of(m)) is not None and c <= limit}
