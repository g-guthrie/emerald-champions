"""Shared validation for native EV authoring.

Migration provenance: legacy P became 8*P-4 when positive, otherwise zero.
While the total exceeded 508, four EVs were removed from the smallest positive
investment (ties in HP/Atk/Def/SpA/SpD/Spe order). That one-time conversion is
complete; live data must never be converted again. Live inputs may use all 510.
"""

from pathlib import Path
import re


# Read the small, literal runtime policy header rather than maintaining a
# second set of limits in authoring tools. No C-expression evaluator is needed.
_policy = (Path(__file__).resolve().parents[1] / "include/constants/pokemon_stats.h").read_text()
_limits = dict(re.findall(r"^#define (MAX_[A-Z_]+) ([0-9]+)$", _policy, re.MULTILINE))
EV_PER_STAT_MAX = int(_limits["MAX_PER_STAT_EVS"])
EV_TOTAL_MAX = int(_limits["MAX_TOTAL_EVS"])


def validate_evs(evs: list[int]) -> list[int]:
    """Validate live EV units without converting or redistributing investment."""
    if len(evs) != 6 or any(type(value) is not int or not 0 <= value <= EV_PER_STAT_MAX for value in evs) or sum(evs) > EV_TOTAL_MAX:
        raise ValueError(f"Invalid EV spread: {evs}")
    return evs
