# Decision index

This is the proposed revision to review before implementation. The user's final instructions govern the book: expert trainer doubles throughout, two permanent starters and a doubles Birch rescue, ordinary wild/legendary captures remaining singles, at least four opponents in a standard team, and a Medium floor of the live cap minus two including gyms. Existing multi-party ownership is preserved. Champions Points use the actual flat-stat calculation.

## The resulting game

The player chooses two distinct starters from one generation and keeps both. Their first rescue is doubles. Catching, healing and free competitive preparation are available before the four-member Route103 rival, whose starter is the unchosen member of that generation. Every subsequent trainer has the same expert tactical capabilities; species, composition, levels and party size create the difficulty hierarchy.

Roxanne regains Nosepass, Phoebe regains Dusclops, and Glacia regains Walrein with fully specified competitive roles. Other established signatures remain. The review preserves 501 of 516 complete native trainer loadouts. The 15 changed branches include six opening aliases, these three signature restorations, and six precise timing, ability, form or move repairs. Changes to shared levels and AI still affect preserved rosters.

Exploration keeps the current map topology and habitat identities. Missing ordinary families and useful functional forms receive specific routes, while rare personality forms also have direct alternatives to repeated catching. Collection means obtaining and using supported species/forms within one save over time; the existing 14 boxes are retained. Mega rewards remain discoveries and earned entitlements. Existing world interactions are corrected where their advice, state flow or reward retry no longer matches the game.

## Authoritative contracts

|Decision|Final instruction|Implementation authority|
|---|---|---|
|INTRO-01|Two selected permanent starters; doubles rescue; four-member rival; early supplies/preparation; exact regional rivals and legacy-save handling.|[Opening](chapters/01-opening-and-starters.md)|
|DIFF-01|Set normal trainer level to `max(authored level, live cap)` before the single Easy/Medium/Hard reduction of 4/2/0. Include gyms; preserve higher authored levels.|[Competition rules](chapters/02-competition-rules.md)|
|TEAM-01|Standard opposing teams contain at least four. Preserve explicit three-plus-three half multis and full six-plus-six story multis. Ordinary captures remain singles.|[Competition rules](chapters/02-competition-rules.md)|
|POINTS-01|Use effective IV31, 66 total flat Stat Points and 32 per stat; evaluate actual early-level thresholds and native integer rounding.|[Competition rules](chapters/02-competition-rules.md) and [early benchmarks](battles/early/points-benchmarks.md)|
|SAI-01|One explicit expert campaign profile across trainer classes and difficulties, with a defined information boundary.|[Shared AI](chapters/06-shared-ai.md)|
|SAI-02–12|Repair damage/critical forecasts, simulated-state contamination, entry effects, switch/ace/Mega evaluation and pair coordination. Use a bounded one-turn action-pair search with cached expensive calculations.|[Final AI contracts](review/shared-ai-final.json)|
|Authored trainer revisions|Apply only the complete final teams and stated shared obligations. Retain the reviewed successful strategies.|[Early](battles/early/README.md), [late](battles/late/README.md), [all branches](review/all-trainers.json)|
|Wild slots and form additions|Apply 29 exact slots across 23 maps; preserve other tables, species-family alternatives, ordinary encounter floors and habitat roles.|[Wild volume](chapters/03-wild-distribution-and-biomes.md)|
|WATER-AUTHOR-01|Use one optional `fishing_species_overrides` object after all fishing slots are generated, for Route117 Paldean Wooper and Route118 blue-striped Basculin.|[Wild volume](chapters/03-wild-distribution-and-biomes.md)|
|GUIDE-01|Show reachable ordinary encounter methods. Suppress the hidden-method advertisement while DexNav is disabled.|[Wild volume](chapters/03-wild-distribution-and-biomes.md)|
|WILD-ENGINE-01|Check encounter-generation success before starting the ordinary Sweet Scent battle, including the caught-Pheromosa rejection path.|[Wild volume](chapters/03-wild-distribution-and-biomes.md)|
|FORM-01–12|Maintain explicit conditional routes for every active configured form; distinguish cosmetic services, costumes, evolutions, transformations and excluded legacy gimmicks.|[Acquisition/forms](chapters/04-acquisition-evolutions-forms.md)|
|New form resources|Seven prepared gifts, eight supplemental wild slots already included in the 29 above, and 14 nonprotected form tools in the current evolution archive.|[Acquisition/forms](chapters/04-acquisition-evolutions-forms.md)|
|Galarian birds|Append Sign IDs 82–84, preserving earlier IDs and receipts. Use exact species/form lookup and ownership rather than shared base-species caught status.|[Acquisition/forms](chapters/04-acquisition-evolutions-forms.md)|
|Mega rewards|Preserve all reviewed world placements and genuine Ring/form prerequisites. Extend starter receipts coherently for the second starter; preserve retry and protected relic capacity.|[Mega rewards](chapters/05-mega-stones-and-legendary-rewards.md)|
|PREP-02|Allow the Leveler to perform an already-ready normal evolution at the cap using existing conditions, cancellation and scenes; grant no extra EXP or levels.|[Preparation](chapters/07-preparation-and-evolution-convenience.md)|
|PREP-02-B|Allow ready battle-only evolution after an eligible successful battle when EXP is disabled; filter out ordinary evolutions and temporary facility normalization.|[Preparation](chapters/07-preparation-and-evolution-convenience.md)|
|FORM-11-NATURE|Toxel follows the edited Nature used by the builder and stats. Preserve unrelated personality-dependent form rules.|[Acquisition/forms](chapters/04-acquisition-evolutions-forms.md)|
|PREP-03|Preserve intentional native expanded move access. Resolve reference mismatches explicitly; do not purge existing sets because a narrower reference lacks a move.|[Preparation](chapters/07-preparation-and-evolution-convenience.md)|
|PREP-04/05|Expose the specified nonprotected form tools; make the Surf-forgetting guard follow actual unlocked party capability.|[Preparation](chapters/07-preparation-and-evolution-convenience.md)|
|PREP-06|Offer optional friendship adjustment through the existing specialist and rater, with preview, confirmation, Egg rejection and unchanged evolution conditions.|[Preparation](chapters/07-preparation-and-evolution-convenience.md)|
|World repairs|Apply the 27 proposals' exact targets: truthful story/Sign advice, defined NPC choreography, Spiritomb access, Trick House delivery, current facilities and optional Wallace restoration.|[World changes](world/changes.md)|
|Gym dialogue|Replace 40 named texts whose unconditional strategy claims or species references no longer fit actual play.|[Gym dialogue](battles/early/gym-dialogue.json)|
|FAC-01|Make all three Battle Tents local six-member competitive doubles exhibitions using the existing generator, cap normalization and their existing prizes.|[Competition rules](chapters/02-competition-rules.md)|
|FAC-02|Keep Trainer Hill geography, time attack and rewards; generate one coordinated six-member opposing side per floor with correct paired ownership.|[Competition rules](chapters/02-competition-rules.md)|
|FAC-03|Keep the shared Frontier Circuit route and truthful current records. Record a new observed best; never invent a historical maximum.|[Competition rules](chapters/02-competition-rules.md)|
|LINK-01|Offer Double/Multi/Info/Exit with unchanged protocol meanings. Retire native imported Secret Base battles while preserving profiles, records and decoration.|[Competition rules](chapters/02-competition-rules.md)|
|Save version 4|One migration owns all reclaimed variables, gift/Trick receipts, opening phases, second-starter stone bits, new Sign bits and recorded Circuit best.|[State allocation](appendices/state-allocation.md) and [opening](chapters/01-opening-and-starters.md)|
|Evidence policy|Keep checks for real behavior and data integrity; update changed requirements; retire arbitrary showcase, duplication and strategy quotas. Native tests and rendered scenes follow implementation.|[Tests and evidence](chapters/09-tests-and-evidence.md)|

## Preservation and rejected changes

The book does not add a general player Item Clause, change Champions' stat formula, enlarge storage/save structures, require every Mega or legendary to appear on an NPC team, or distribute novel strategies by badge. Ordinary wild captures remain singles. Prepared sets remain starting suggestions within the native expanded move policy.

Current source fixes for Meltan, Ursaluna, Kingambit, Runerigus and regional evolution paths are retained. High-tide Shoal layout names do not justify extra encounter headers: the current map/header routing already supplies them. Heatran's physical hide flag is not proof of a capture-ownership bug. Sitrus-to-flavor-berry replacement was rejected where the actual HP parity prevents the proposed activation. Unbound legacy scripts are identified as inactive rather than assumed to be live broken NPCs.

Implementation starts by comparing relevant current files to the frozen manifest. Apply the [nine integration units](chapters/08-implementation-sequence.md), with each shared concern retaining its named owner. The book defines a complete design proposal; it does not certify that a future implementation is balanced, traversable or ready to release.
