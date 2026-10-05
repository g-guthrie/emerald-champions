"""Player star policy shared by source pools and benchmark validation."""
import re
from functools import lru_cache
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


@lru_cache(maxsize=1)
def restricted_exceptions():
    source = (ROOT / "src/pokemon.c").read_text()
    body = source.split("enum RestrictedPartyClass GetRestrictedPartyClass(", 1)[1].split("info = &gSpeciesInfo", 1)[0]
    return set(re.findall(r"species == (SPECIES_\w+)", body)) - {"SPECIES_NONE", "SPECIES_EGG"}


def blocked_mega_slots(party, restricted, aliases=None):
    from mega_register import item_forms, base_species
    from verify_trainer_ability_legality import resolve_species
    aliases = aliases or {}
    special = [i for i, mon in enumerate(party)
               if resolve_species(mon["species"], aliases) in restricted]
    if not special:
        return []
    pairs = {(resolve_species(base_species(form), aliases), item)
             for item, forms in item_forms().items() for form in forms}
    return [i for i, mon in enumerate(party)
            if i not in special and mon.get("mega_disabled") is not True
            and (resolve_species(mon["species"], aliases), mon.get("item")) in pairs]
