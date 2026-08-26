# Verdant marquee collision review

Phase: `pokemon_league_main_story`

Designed: 5/5
Unique species: 30
Formats: {'double': 4, 'single': 1}

## Hard errors

- None.

## Advisories

- **PREMIUM_ITEM_REPETITION** — `{"code": "PREMIUM_ITEM_REPETITION", "uses": {"ITEM_LEFTOVERS": ["ELITE_FOUR_DRAKE", "ELITE_FOUR_GLACIA", "ELITE_FOUR_PHOEBE"]}}`
- **SIGNATURE_MOVE_REPETITION** — `{"code": "SIGNATURE_MOVE_REPETITION", "uses": {"MOVE_CLOSE_COMBAT": ["ELITE_FOUR_GLACIA", "ELITE_FOUR_PHOEBE", "ELITE_FOUR_SIDNEY"], "MOVE_KNOCK_OFF": ["CHAMPION_WALLACE", "ELITE_FOUR_GLACIA", "ELITE_FOUR_SIDNEY"], "MOVE_SHADOW_BALL": ["ELITE_FOUR_DRAKE", "ELITE_FOUR_GLACIA", "ELITE_FOUR_PHOEBE"], "MOVE_U_TURN": ["CHAMPION_WALLACE", "ELITE_FOUR_DRAKE", "ELITE_FOUR_SIDNEY"]}}`
- **HISTORIC_REFERENCE_REUSE** — `{"code": "HISTORIC_REFERENCE_REUSE", "uses": {"elite:marco-silva:turin-2026": ["ELITE_FOUR_PHOEBE", "ELITE_FOUR_SIDNEY"], "showdown:gen9randomdoublesbattle:002": ["ELITE_FOUR_DRAKE", "ELITE_FOUR_GLACIA"]}}`
- **MODE_CLUSTER** — `{"code": "MODE_CLUSTER", "uses": {"choice-pressure": ["ELITE_FOUR_DRAKE", "ELITE_FOUR_GLACIA", "ELITE_FOUR_SIDNEY"], "fast-control": ["CHAMPION_WALLACE", "ELITE_FOUR_GLACIA", "ELITE_FOUR_SIDNEY"]}}`

## Resource tax by battle

- `ELITE_FOUR_SIDNEY` — The roster applies direct HP and faint pressure plus Will-O-Wisp burn from Mega Absol. Nothing is restored automatically after victory; the player may spend carried healing, status recovery, and revival stock before Phoebe.
- `ELITE_FOUR_PHOEBE` — Perish Song itself ends with battle, but any fainted Pokemon remain fainted, Spore sleep and Will-O-Wisp burn persist, and direct damage carries forward. Control returns after victory, so the player may spend carried Awakenings, Full Heals, recovery, and Revives before Glacia; no automatic repair occurs.
- `ELITE_FOUR_GLACIA` — Glacia enters third without automatic healing. The readable lead taxes Protect and disruption rather than surprise knowledge; trapping, poison, Knock Off, speed control, priority, and final Glastrier pressure tax HP, status cures, held items, and the Ice or Fighting answer the player must preserve before Drake.
- `ELITE_FOUR_DRAKE` — Drake enters fourth without automatic healing. Stealth Rock, one Scarf lock, mixed coverage, Glare, Haze, and the final Multiscale Mega tax HP, status control, hazard removal, scouting, and the physical or special answer the player preserves before Wallace.
- `CHAMPION_WALLACE` — The fight taxes weather replacement, Wide Guard or spread mitigation, speed-control PP, physical Grass/Electric coverage, Taunt, item planning, and whatever Revives or healing the player chooses to spend after Drake. It does not erase the manual healing window.

Hard collisions block the current phase. Advisories require judgment and written disposition; they are not scores, quotas, or automatic bans.
