# Verdant logical audit

This is the design and code gate for the mostly-doubles checkpoint. The audit is executable:

```sh
python3 scripts/verdant_logical_audit.py
```

The build is accepted only when all thirty questions pass, followed by the broader `scripts/verify_verdant.py` suite and a full ROM compile.

## Progression and friction

1. Is every new game unambiguously Challenge Mode with strict caps? **Yes.** Both new-game initialization paths set those values.
2. Is Set battle style mandatory, with the selector removed? **Yes.** Set is initialized and the option is absent.
3. Does one Rare Candy add ten levels but stop at the current cap? **Yes.** The target is `min(level + 10, cap)`.
4. Does that jump preserve move learning and chained evolutions? **Yes.** Every crossed level is checked and evolution callbacks can continue.
5. Can leveling stay frictionless without being free? **Yes.** Rare Candies cost $1,000.
6. Are the ten foundational held items sold by every normal Poké Mart? **Yes.** Normal Mart stock is safely augmented in code.
7. Can a long Mart inventory or item name corrupt the Cancel row? **No.** The shared stock buffer is bounded and names use `ITEM_NAME_LENGTH`.

## Rewards and team construction

8. Are obsolete TM pickups, gifts, and shop entries completely gone? **Yes.** TM objects are no longer distributed; legal moves live at the teacher.
9. Do the old TM specialty vendors still have a purpose? **Yes.** Slateport sells early type boosters; Lilycove sells advanced type and weather items.
10. Do campaign gifts duplicate the ten always-stocked core items? **No.** Those gifts and pickups were replaced.
11. Does finding or receiving a non-core battle item permanently unlock it? **Yes.** Bag acquisition records the unlock and Marts rebuild from it.
12. Can the Pokémon Center teacher offer every legal move source? **Yes.** Level-up, Egg, TM/HM, and tutor lists are merged and deduplicated.
13. Is that complete move list available before the first Badge? **Yes.** The live teacher path has no Badge gate.
14. Can abilities be changed natively without consumables? **Yes.** The party action menu lists only that species' unique legal ability slots.
15. Does the Day Care still create eggs through normal compatibility rules? **Yes.** Species, gender, Ditto, and Egg Group checks remain.
16. Does breeding retain incentives beyond move access? **Yes.** Species acquisition, Everstone nature inheritance, Destiny Knot IV inheritance, and ability inheritance remain.
17. Are IV and EV services bounded, priced by real gain, and stat-safe? **Yes.** Per-stat and total caps are enforced, actual gain is charged, and stats recalculate.

## Battle structure and difficulty

18. Is every real trainer record represented in the authored format manifest? **Yes: 848 of 848.**
19. Is the campaign genuinely mostly doubles? **Yes: 632 doubles and 216 intentional singles.**
20. Can every doubles battle deploy safely? **Yes.** Every double has exactly four or six Pokémon.
21. Do singles remain as pacing contrast? **Yes.** Norman and Drake are signature singles anchors, with selected ordinary singles retained.
22. Do marquee bosses exceed the cap at the ace and carry complete teams? **Yes.** All thirteen have six held-item, four-move sets and a positive ace offset.
23. Do doubles teams use protection and speed control? **Yes.** At least 350 use Protect; at least 90 use Tailwind or Trick Room.
24. Do they create real spread-pressure decisions? **Yes.** More than 300 teams carry a major spread move.
25. Does trainer AI understand foes, partners, and tactical switching? **Yes.** Foe-aware AI is universal, with partner and smart-switch flags on the relevant tiers.
26. Are the repaired AI decisions guarded against prior defects? **Yes.** Integrated switch ranking, hazard rejection, initialized move typing, and deterministic tactical switches are regression-tested.

## Megas, encounters, and presentation

27. Are Megas constrained and deliberately showcased? **Yes.** Marquee teams cannot carry multiple Mega items, and every new Mega appears during campaign progression.
28. Does the player receive Mega access early enough to experiment? **Yes.** Steven gives the Bracelet and Hoenn starter stones in Granite Cave, with a Norman fallback.
29. Are exciting early encounters accessible without one-percent hunting? **Yes.** Six early areas each have two new high-value species at 4% or better, while all native slot counts remain intact.
30. Are rewritten rewards, dialogue, and menus visually and semantically clean? **Yes.** Stale TM speech is gone, rewritten reward lines have a conservative 36-character limit, and item menus use the canonical name width.

## Concrete findings fixed during this audit

- Rewrote stale TM dialogue left behind by held-item reward substitutions.
- Replaced the last two TM shop inventories with useful held-item strategy stock.
- Reflowed the touched reward dialogue to a conservative in-game line budget.
- Replaced Wattson's already-available Cell Battery reward with Wise Glasses so the Gym prize remains meaningful.
- Added a permanent thirty-question audit to the main Verdant verifier.
