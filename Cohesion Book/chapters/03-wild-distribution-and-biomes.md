# 03 — Wild distribution, biomes and usable choice

**Final design:** preserve the great majority of the existing broad roster, fill concrete missing family/form routes, maintain recognizable habitats and make every advertised acquisition truthful. There is no weak-to-strong curriculum and no rule that a Pokémon must wait for its own chapter.

Read [all 138map reviews and exact tables](../appendices/wild-encounter-catalog.md). Each map has an individually written assessment and full ordinary method inventory; changed methods include complete final slot tables. The exact machine specifications are in [wild-distribution.json](../review/wild-distribution.json) and [wild-proposals.json](../review/wild-proposals.json).

## What stays

The opening retains its babies, quick evolutions, Mienfoo, Ferroseed and exciting long-term choices. Dreepy, Bagon and Beldum are not moved merely because they evolve later. Pheromosa and Kartana are not delayed merely because they are strong. Their usefulness and the nearby authored battles must be judged together.

Magnemite remains a New Mauville discovery. Duskull remains a Mt.Pyre family. The volcanic, desert, rainforest, haunted, cold-water and underwater populations retain their recognizable cores. Dondozo and all Tatsugiri forms remain an underwater/submarine-focused discovery.

The user corrected the earlier scarcity-oriented proposal. Consequently this book does not turn Rusturf, Artisan Cave or Mirage Island back into nearly monotypic areas at the expense of existing choices. Their iconic residents, locations and interactions should carry identity while their broader useful rosters remain. The map reviews explicitly discuss this tradeoff rather than pretending broad variety is itself a defect.

Ordinary wild and legendary captures remain singles. `WE_DOUBLE_WILD_CHANCE` stays0 and the generic force-double flag remains disabled. INTRO-01's first Birch rescue is a specifically scripted doubles encounter; it must not enable double random encounters globally.

## Exact new family and form placements

The snapshot already contains Burmy in Petalburg Woods and Deerling in Verdanturf Meadow. It also already supplies local evolution routes for several previously missing forms. Those are preserved.

The slot revisions in the linked catalogue introduce missing regional or exceptional choices mostly by replacing a repeated evolved family that has another practical route. Examples include Galarian Zigzagoon/Squawkabilly on Route118, Galarian Meowth in the Safari Zone, Alolan Grimer on Route121, cold regional forms in Shoal, and the previously absent Paradox species in matching habitats. New availability does not require removing that family's existing rival or trainer showcase.

These are exact species replacements at existing indices. Keep the current minimum/maximum levels and weights. Every record names the displaced family and its remaining route. Additional Antique/Artisan/Tatsugiri/other form-specific entries from the acquisition chapter are integrated into the final catalogue; use that complete manifest when implementing.

The Paldean Wooper route is a deliberate exception to the generator's hardcoded Old Rod Magikarp rule: Route117's Old Rod becomes Wooper-Paldea60% and Tympole40%. Earlier Magikarp access remains in Dewford and many other locations. This adds Clodsire access without removing Tympole or requiring another map/encounter engine.

### WATER-AUTHOR-01 — Small authoring extension

Add one optional `fishing_species_overrides` object to a row in `data/emerald_champions/wild_route_sheet.json`, mapping zero-based native fishing-slot indices to configured species names without the `SPECIES_` prefix. Route117 supplies `{"0":"WOOPER_PALDEA"}`; Route118 supplies `{"8":"BASCULIN_BLUE_STRIPED"}` for the functionally distinct Blue Basculin route.

In `scripts/emerald_champions_rebuild_wild_water.py:build_fishing`, build the existing ten slots first, then validate and apply only each specified species replacement before returning the table. Require integer indices 0–9, no duplicate normalized index, and a configured enabled species. Keep the existing min/max levels. Applying the override after ordinary generation leaves the internal `used` pool and every other slot unchanged.

This single small authoring extension serves both new fishing routes. Do not add separate Old Rod and Super Rod override systems, change shared encounter-rate arrays, invent another water authoring file, or reorder maps and thereby rotate unrelated generated rosters. Compare all 10slots and all unaffected maps with the snapshot after materializing. The complete final tables in the catalogue are the intended result.

## Method access and physical identity

The catalogue separates walking, Surf, Rock Smash, Old Rod, Good Rod and Super Rod. A route's grass access does not imply that its Surf or rod catches are already obtainable. Route115's northern grass and southern Seaspray access are separate physical cases. Riding past Route105 on Briney's boat does not prove access to its grass tiles. The ruined and underwater rooms require their actual path, puzzle and story prerequisites.

The existing timing helper contains broad map-prefix fallbacks and cannot serve as an exact progression oracle. The world volume owns physical routes; the Mega volume separately examines pickup access and usable evolution timing.

A suspected Shoal defect was rejected: high tide changes layouts while retaining the low-tide saved map IDs, so the existing low-tide water/fishing headers still serve the flooded rooms. Do not add redundant high-tide tables or shift generator ordering based on layout names alone.

The nine Altering Cave selector rows are conditional alternatives, not nine simultaneous populations. Keep their compatibility, but do not inflate ordinary coverage from their union. The dedicated expanded cave rooms remain separate real maps.

## GUIDE-01 — Advertise usable methods

`BufferCurrentMapRouteSignSpecies` currently appends Hidden entries even though `DEXNAV_ENABLED` isFALSE. The hidden tables also retain historical encounter-rate values20, whereas DexNav's detector switch interprets0/1 as terrain modes. Those rows are not ordinary encounter methods.

Final change: suppress the Hidden section when its actual encounter feature is disabled. Keep the ordinary route sign derived from active native tables, including the three rod groups, and retain the separate Route119 Feebas clue. Do not enable DexNav, repurpose its rate field or make a new detector merely to justify obsolete display rows.

Acceptance: a route with inactive Hidden data lists only its usable methods; route entries still match the final tables; no dormant-only species is used to close a roster gap. A future explicitly approved detector feature would need its own semantics and access review.

## WILD-ENGINE-01 — Respect failed Sweet Scent generation

Source evidence: normal land/water branches of `SweetScentWildEncounterInner` call `TryGenerateWildMon` and then unconditionally start a battle. `TryGenerateWildMon` can returnFALSE before creating a Pokémon, including when an ordinary Sign species was already caught or is unavailable. Pike/Pyramid branches already check its result.

Final change: on normal land, if there is no successful outbreak and `TryGenerateWildMon` returnsFALSE, returnFALSE before starting the battle. On normal water, similarly require successful generation before starting. Preserve existing roamer/outbreak handling, wrapper restoration of the Sweet Scent selection state, ordinary single-capture format, repel behavior and other generation guards. Do not add an unbounded reroll loop or silently bypass capture eligibility.

Acceptance: seed an old enemy party, force a caught Pheromosa draw, and prove that no battle starts and no stale species is exposed. Also test a valid ordinary draw, a valid uncaught Sign draw, successful outbreak, roamer, land/water paths and wrapper cleanup after failure. This is a source-backed control-flow repair; the book does not claim a reproduced live exploit.

## Probability and convenience

Keep the ordinary5%minimum per species per method, aggregating repeated slots. Check both normal and Sweet Scent reversed selection. Encounter frequency, conditional rejected Sign draws and capture success are separate quantities; a5%slot is not a5%chance on every walking step.

Specialness comes from location, species, story and strategy as well as probability. Preserve generous preparation and the cap-respecting Leveler. PREP-02 ensures a Pokémon already at its cap can still receive a ready normal evolution through that same convenient tool.

Implementation order is WILD-ENGINE-01/GUIDE-01, the exact slot manifest plus WATER-AUTHOR-01, acquisition/form dependencies, then local capture/path fixtures. The battle volumes remain the authored challenge baseline against which these new player options are exercised.
