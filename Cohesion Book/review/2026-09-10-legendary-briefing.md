# Mandatory legendary briefing

Added September 10 after the opening/first-badge reviews, at the user's request.

- Birch's early Pokédex handoff connects unusual weather, changing habitats and increased legendary sightings. He poses their connection as a research question, preserving the later mystery.
- Birch's mandatory post-rival send-off explains route rosters, reading past the fishing lists for legendaries, ordinary wild discoveries, and the local guide near each Center's PC. This reaches players before Route 102's Shaymin.
- The Devon researcher, after the mandatory Woods battle, connects his papers to the same investigation and explains special requirements, Caught labels and one-time legendary catches. His Shroomish detour and urgent Devon destination remain intact.

These replace three existing text blocks. No new triggers, menus, flags, encounter rules or rewards were introduced. The script call sites already run these conversations on the mandatory path; both Woods approach branches share the post-battle text.

The edited blocks are 4, 6 and 6 pages. Their widest lines measure 191, 177 and 181 pixels against a 216-pixel standard dialogue window. All explicit line/scroll/page controls passed the focused source check. Checked explanations against the C roster/capture logic and shared Center-guide script. This text-only follow-up was source/layout checked and release-built; it was not a new full campaign playthrough.

Evidence: `work/legendary-briefing-20260910/text-check.json`, `build.log`, and `verification.log`.
