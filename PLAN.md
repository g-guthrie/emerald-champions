# Ground-up conversion plan

This is the expanded architecture proposal requested in the conversation. It describes the intended replacement, not a claim that the current rewrite implements it. Numerical inventories below are snapshots from the planning discussion. Executable definitions and build output remain authoritative for current implementation state.

1. **The reference game and the conversion contract**

   The first step is to establish the exact behavior we are preserving. “Reproduce 7.0 or 7.1” cannot remain ambiguous while systems change underneath it. I would use one frozen 7.1 revision and its working ROM as the primary reference, with explicit exceptions for approved changes.

   The conversion needs an inventory of **game behavior**, rather than an inventory of files. A gift, for example, includes its location, dialogue, prerequisites, Pokémon construction, party/PC delivery, claim flag, and any consequences elsewhere.

   ```mermaid
   flowchart LR
       A["Frozen reference source"] --> C["Behavior inventory"]
       B["Working reference ROM"] --> C
       C --> D["Canonical definitions"]
       C --> E["Kernel operations"]
       C --> F["Preserved assets"]
       G["Approved design changes"] --> D
       G --> E
       D --> H["Replacement game"]
       E --> H
       F --> H
   ```

   Each inventoried behavior gets an explicit disposition: represented by a canonical definition, implemented by a shared operation, or removed for a demonstrated reason. “Its old file disappeared” is not a disposition.

   This inventory includes the optional campaign branches and facilities, not merely the main story. It also includes details that affect fidelity: textbox timing, movement sequences, battle order, menus, sound cues, and save behavior.

   **The replacement criterion:** the new game accounts for the behavior and reproduces it, except where we intentionally changed the design. That is when the corresponding old implementation becomes unnecessary.

2. **The fundamental shape of the new game**

   The architecture has three authored responsibilities: the game program, the compiler, and the hardware/runtime kernel.

   ```mermaid
   flowchart TD
       A["Authored game program<br/>facts, scenes, rules, dialogue"] --> B["Compiler"]
       B --> C["Packed runtime data"]
       B --> D["Dependency and availability graph"]
       B --> E["Complete LLM context"]
       C --> F["GBA kernel"]
       G["Pixel and audio assets"] --> F
       F --> H["World and story execution"]
       F --> I["Battles and AI"]
       F --> J["Presentation and persistence"]
   ```

   The **game program** owns what makes this particular game this particular game: characters, scenes, progression, species, teams, encounters, economy, and game-specific mechanics.

   The **compiler** resolves references and inheritance, reconciles dependent selections, checks necessary relationships, and packs data. It also generates the graph and context view from those same definitions.

   The **kernel** executes transitions, resolves battles, draws the screen, plays audio, reads input, and saves state. It should not contain a separate handwritten implementation of “the NPC who gives this particular Pokémon.”

   The separation is about ownership, not isolated development. A change to the transaction model must account for every shop, gift, exchange, and reward across the whole campaign.

   Physical files can be divided by domain for tooling, but the complete authored program, compiler, and kernel must assemble into one lossless context. Splitting files must not mean splitting understanding.

3. **Canonical ownership and directional dependencies**

   “Everything references everything” would produce a graph so connected that it stops being useful. The goal is that **every real dependency exists, has a meaning, and points in the correct direction**.

   ```mermaid
   flowchart TD
       P["Species move pool"] --> L["Permitted learning"]
       P --> S["Permitted set selections"]
       S --> T["Trainer's chosen build"]
       S --> G["Gift's chosen build"]
       T --> A["Strategy requirements"]
       G --> D["Dialogue claims"]
       A --> R["Design review when broken"]
       D --> R
   ```

   A field needs one owner. Other places either reference it, derive a value from it, select from it, or make a claim about it.

   These relationships behave differently:

   | Relationship | Required consequence |
   |---|---|
   | Derived value | Recalculate it |
   | Selection from an allowed set | Remove choices that are no longer allowed |
   | Shared definition | Update its deliberate users |
   | Prerequisite | Recalculate availability |
   | Factual wording | Update the bound wording |
   | Strategic or thematic assumption | Identify the affected design decisions |
   | Geographic adjacency | Update relevant navigation relationships |

   The graph must also be **field-aware**. Changing an NPC’s coordinates is different from changing the gift they provide. Changing a species’ pool is different from changing its sprite palette.

   A change report should therefore say what changed, which rule caused each consequence, and where interpretation remains necessary. It should not merely print every record reachable through an arbitrary chain of references.

4. **The languages and compact notation**

   I would use C for the native kernel, minimal assembly where hardware startup requires it, and Python’s standard library for the compiler. The game program would use a compact domain language.

   ```mermaid
   flowchart LR
       A["Showdown-style builds"] --> D["Parsed game model"]
       B["Scene and transaction notation"] --> D
       C["Mechanical rules and world data"] --> D
       D --> E["Reference resolution"]
       E --> F["Reconciliation"]
       F --> G["Packed GBA representation"]
   ```

   Showdown-style notation is useful for Pokémon builds because it already expresses a dense, familiar vocabulary. An illustrative build could look like:

   ```text
   squirtle.support:
     Squirtle @ Eviolite
     Torrent / Bold / 252 HP 252 Def 4 SpD
     Follow Me / Icy Wind / Helping Hand / Protect
   ```

   The final syntax should be shorter where defaults and shared definitions make that worthwhile. The important part is that species, item, ability, nature, and moves resolve to canonical identities.

   Scenes and transactions need their own compact syntax. For example, conceptually:

   ```text
   gift.research:
     require research.unlocked & !research.claimed
     grant mon(research.partner)
     commit research.claimed
     say research.receipt
   ```

   These examples illustrate the intended representation; they are not descriptions of a completed parser.

   The compiler should eliminate repeated defaults and resolve aliases once. It should not carry C expressions, macro names, pointer declarations, or old script command encodings into the authored game merely because those were convenient to import.

   The language should have a small vocabulary of real game operations. It should not become an enormous general-purpose framework.

5. **Pokémon, move pools, forms, and construction**

   Pokémon need a clear distinction between **species definitions**, **build choices**, and **individual Pokémon state**.

   ```mermaid
   flowchart TD
       A["Species definition"] --> B["Authoritative move pool"]
       A --> C["Stats, types, abilities and forms"]
       B --> D["Learning routes"]
       B --> E["Build selections"]
       E --> F["Pokémon construction"]
       C --> F
       G["Encounter or reward definition"] --> F
       F --> H["Individual Pokémon"]
       H --> I["Party, storage or battle"]
   ```

   The species owns its base facts and permitted pool. Level-up learning, eggs, tutors, preparation, and selected builds describe how permitted moves become available or get selected.

   **Legality and availability remain different.** A move can belong to a species’ pool while requiring a later tutor or service. Removing it from the pool removes permission everywhere. Moving its tutor to a later checkpoint changes access timing.

   Forms should inherit facts deliberately. Shared traits belong to a shared definition; genuinely different traits are overrides. Equal values alone do not prove that two facts should share an owner.

   Evolution becomes an ordered rule with prerequisites, target, and costs. A requirement such as “knows this move” directly references that move. Removing it from the source species’ pool exposes an unreachable evolution requirement.

   One construction operation handles starters, gifts, encounters, trades, trainers, and facilities. It takes the species, level rule, build policy, origin, and permitted individual choices. Stats, initial moves, ability resolution, and delivery behavior come from shared implementations.

   Existing saved Pokémon also need an explicit policy when the design changes. The architecture must identify invalid stored choices and migrate them coherently rather than allowing saves to become an alternate source of legality.

6. **Progression, flags, checkpoints, and availability**

   Progression should be a dependency graph with optional branches, not a single chapter counter.

   ```mermaid
   flowchart TD
       A["Opening completed"] --> B["Research service available"]
       A --> C["First challenge available"]
       C --> D["Capability unlocked"]
       B --> E["Optional research branch"]
       D --> F["New acquisition routes"]
       E --> G["Research reward"]
       F --> H["Later checkpoint"]
       G --> H
   ```

   This is a conceptual structure, not a claim about the exact current campaign sequence.

   Persistent facts describe things that happened: a battle was won, a character was met, a reward was claimed. Capabilities derive from those facts: a route can be entered, a service can be used, a mechanic can be activated.

   The model should store a fact once. If “research service unlocked” can be derived from existing story facts, it does not need a second independently maintained flag.

   However, **ever received**, **currently owns**, and **already claimed** are different facts. Selling an item should not accidentally reopen a unique reward.

   A checkpoint is a query over the reachable game state:

   - Which areas and scenes can the player reach?
   - Which services and battles are available?
   - What is the current level cap?
   - Which acquisition paths have opened?
   - Which optional choices could have changed the player’s resources?

   The availability analysis must distinguish “obtainable on at least one valid route” from “guaranteed by this point.” Those are different balancing assumptions.

   Large state spaces may require conservative analysis. When the analysis cannot prove something, it should say so rather than presenting an approximation as an exact campaign simulation.

7. **The complete economy**

   Every resource source and sink belongs in the same transaction model.

   ```mermaid
   flowchart TD
       P["Progression requirements"] --> T["Available transaction"]
       I["Inventory and balances"] --> T
       S["Stock and claim state"] --> T
       T --> C["Costs"]
       T --> G["Grants"]
       C --> N["Committed new state"]
       G --> N
       N --> A["Updated acquisition possibilities"]
   ```

   A transaction specifies requirements, consumed resources, granted resources, stock behavior, and committed state changes. It covers shops, gifts, pickups, exchanges, rewards, tutors, and services.

   Quantities, prices, and reward identities must come from their owners. An NPC should not independently embed a price in dialogue while a shop table owns a different number.

   Item valuation needs separate concepts. Buy price, sell price, exchange cost, and availability are not one interchangeable “value.”

   For each checkpoint, the economy view should show:

   | Question | Data needed |
   |---|---|
   | Can this item be obtained? | Reachable sources and prerequisites |
   | Is the supply renewable? | Repeatability and stock |
   | What does acquisition consume? | Money, items, counters, or opportunities |
   | Can exchanges generate resources indefinitely? | Reachable cycles and net changes |
   | Does an early reward undermine later scarcity? | Timing, quantity, and intended role |

   Mega Stone acquisition policy belongs upstream. A Bracelet requirement should govern relevant acquisition routes centrally, rather than requiring individual fixes in shops, rewards, and dialogue.

   A free early gift and a later shop can be intentional. The system should expose their combined effect; it should not mechanically decide that one must disappear.

8. **NPCs, services, and scene execution**

   An NPC becomes a coherent actor with placements, scenes, and services.

   ```mermaid
   flowchart LR
       A["Map placement"] --> B["Actor"]
       B --> C["Scene selection"]
       D["World facts"] --> C
       C --> E["Dialogue and choices"]
       E --> F["Shared operation"]
       F --> G["State transition"]
       G --> C
   ```

   The actor owns its identity. A placement says where that actor appears and how it behaves spatially. Scenes describe what interaction occurs under particular conditions.

   A shopkeeper who moves between towns should not become two disconnected copies of the same character. Multiple placements can refer to one actor, while scene conditions decide which placement is active.

   A gift scene should directly express: explain the offer, obtain the player’s choice, attempt delivery, commit the claim when delivery succeeds, and select the appropriate response.

   This replaces chains of temporary variables and special-function calls with shared operations whose results have explicit meanings.

   Movement, facing, camera work, text pauses, and sound cues remain part of scene choreography. Simplifying the source does not mean throwing away presentation timing.

   The same transaction executor should handle the economic consequence whether the player initiated it through an NPC, a pickup, or a menu.

9. **Every dialogue line and the whole story**

   Dialogue needs both exhaustive recovery and meaningful ownership.

   ```mermaid
   flowchart TD
       A["Script text"] --> E["Canonical dialogue inventory"]
       B["Native-function text"] --> E
       C["Dynamic buffers"] --> E
       D["Menus and special systems"] --> E
       E --> F["Speaker and scene"]
       E --> G["Conditions"]
       E --> H["Facts and narrative premises"]
       H --> I["Change impact"]
       I --> J["Generated factual updates"]
       I --> K["Authored scene revision"]
   ```

   The existing 11,973 text records are a starting inventory. They do not prove that all native-generated text, menu text, conditional fragments, or special-system dialogue has been captured.

   Every line should connect to its speaker or presentation context, the scene that uses it, and its availability conditions. Relevant factual and narrative dependencies must be explicit.

   There are several different cases:

   | Dialogue content | Canonical treatment |
   |---|---|
   | Item name or price | Read the owning definition |
   | Gift species or selected move | Read the relevant gift/build definition |
   | Statement about access timing | Depend on the progression requirement |
   | Character’s knowledge | Depend on what the character has learned |
   | Deliberate lie or mistaken belief | Represent that belief separately from world truth |
   | Story theme or emotional premise | Link the affected authored scenes for review |

   That distinction prevents a serious mistake: automatically making every character omniscient and truthful because the global facts changed.

   Consider a hypothetical research gift:

   ```mermaid
   flowchart TD
       A["Gift becomes easily obtainable"] --> B["Rarity premise changes"]
       B --> C["Early researcher dialogue"]
       B --> D["Rival's objection"]
       B --> E["Reward scene"]
       B --> F["Later recognition"]
       C --> G["Whole affected arc in context"]
       D --> G
       E --> G
       F --> G
   ```

   Names and quantities can update mechanically. The emotional meaning of those scenes needs interpretation.

   If a reward originally justified a sacrifice, changing that reward might weaken the entire arc. The change report should collect the relevant scenes and their dependencies so they can be revised together.

   Exact baseline wording, line breaks, page boundaries, control codes, and timing remain preserved where fidelity requires them. Future edits use the actual font metrics and dialogue layout rules.

10. **Battle mechanics and shared calculations**

    The battle model should express actions and reactions through a small set of mechanical operations.

    ```mermaid
    flowchart TD
        A["Chosen actions"] --> B["Order and priority"]
        B --> C["Target and eligibility"]
        C --> D["Mechanical effects"]
        D --> E["Ability and item reactions"]
        E --> F["Updated battle state"]
        F --> G["End-of-action processing"]
        G --> H["End-of-turn processing"]
        F --> I["Presentation events"]
    ```

    Moves select effects and parameters. Abilities and items contribute conditional reactions. The kernel owns execution order, interruptions, targeting, randomness, and state mutation.

    Repeated effects such as damage, healing, stat changes, weather changes, switching, and status application should have shared implementations.

    Unique mechanics still need explicit rules. Compressing everything into an abstraction that requires dozens of exceptions would defeat the purpose.

    The same mechanics must serve actual battles, previews, and AI evaluation. A second damage formula inside AI would be another source of drift.

    Presentation consumes events emitted by battle resolution: damage occurred, an ability activated, a Pokémon fainted, a message should appear. The animation does not independently decide mechanical outcomes.

    GBA performance remains a real constraint. The implementation can cache derived results and bound expensive work, while keeping the authoritative calculation in one place.

11. **Trainer teams and bespoke AI**

    Trainer identity, team selection, and strategy need separate but connected ownership.

    ```mermaid
    flowchart TD
        A["Trainer and encounter"] --> B["Team selection"]
        B --> C["Legal builds"]
        D["Strategy definition"] --> E["Action evaluation"]
        C --> E
        F["Shared battle mechanics"] --> E
        G["Knowledge and search limits"] --> E
        E --> H["Chosen action"]
        C --> I["Strategy feasibility"]
        D --> I
    ```

    A trainer’s strategy can express objectives such as maintaining rain, enabling a sweeper, preserving a support Pokémon, or activating Steam Engine.

    Shared tactical evaluation considers legal actions using the real battle model. Trainer-specific data controls objectives, priorities, knowledge, and computational budget.

    A Steam Engine strategy, for example, should reference the ability and activation conditions. It should not merely be a string named `STEAM_ENGINE` attached to unrelated custom C code.

    When a move disappears, the team must reconcile mechanically. The strategy then needs a feasibility check: can its intended activation, setup, protection, or finishing sequence still happen?

    If not, the affected design decision is explicit. We can choose another legal move, another teammate, or another strategy while seeing the consequences.

    A team passing move legality does not prove that its AI still makes sense. The current shortened sets illustrate that distinction.

12. **Maps, graphics, sound, and saves**

    Map content needs separate ownership for geography, appearance, and behavior.

    ```mermaid
    flowchart TD
        A["Map identity"] --> B["Topology and collision"]
        A --> C["Actor placements"]
        A --> D["Scene triggers"]
        A --> E["Visual asset references"]
        B --> F["Navigation"]
        C --> G["World presentation"]
        D --> H["Story execution"]
        E --> G
        F --> I["Persistent world state"]
        H --> I
    ```

    Collision, elevation, warps, encounter regions, and triggers remain readable game data. Tiles and palettes remain assets, with their selection and behavioral metadata represented explicitly.

    Moving an NPC should update its placement. It should not require reconstructing its dialogue, gift logic, and identity.

    Changing a route connection should affect navigation and availability. It should not cause every neighboring NPC to appear in a move-pool change report.

    The hardware kernel must replace rendering, input, sound, scheduling, and persistence. Keeping the old engine beneath a new content layer would not satisfy the requested rewrite.

    Saves store authoritative mutable state: Pokémon, resources, completed events, claims, location, and other necessary persistent facts. Derived values should be recalculated unless a measured runtime requirement justifies a cache.

    Stable identities and explicit migrations are needed when authored content changes. A record moving to another position in a file must not make a saved item become a different item.

13. **The whole-game conversion process**

    The work proceeds through global structural passes, with each pass covering the entire game domain it changes.

    ```mermaid
    flowchart LR
        A["Reference inventory"] --> B["Canonical identities"]
        B --> C["Whole-game domain conversion"]
        C --> D["Shared operation replacement"]
        D --> E["Kernel integration"]
        E --> F["Fidelity comparison"]
        F --> G["Removal of old machinery"]
    ```

    Each pass has a concrete output:

    | Pass | Output |
    |---|---|
    | Inventory | Accounted-for behaviors and content |
    | Identity resolution | One identity per actual entity |
    | Domain conversion | Complete scenes, economy, species, teams, and world definitions |
    | Operation replacement | Shared mechanics replacing native special cases |
    | Integration | The complete model executing on the GBA kernel |
    | Comparison | Evidence of preserved behavior and presentation |
    | Removal | No required dependency on obsolete systems |

    This does not mean every domain can be implemented in the same keystroke. It means the architecture and conversion scope remain whole-game, rather than declaring one small playable area to be the new foundation while ignoring the rest.

    Codebase-memory MCP traces the old and new code relationships. The domain compiler generates the content graph. Neither graph alone proves coverage.

    The current imported catalog must be treated as working material. Where it preserves the old structure or contains incorrect bindings, it should be replaced or corrected rather than protected as an architectural investment.

14. **Where the deletion actually comes from**

    The meaningful deletion comes from replacing repeated systems with shared mechanics and canonical definitions.

    ```mermaid
    flowchart LR
        A["Shop code"] --> E["Transactions"]
        B["Gift code"] --> E
        C["Exchange code"] --> E
        D["Reward code"] --> E
        F["NPC-specific scripts"] --> G["Scenes and operations"]
        H["Copied calculations"] --> I["Shared battle mechanics"]
        J["Repeated species data"] --> K["Definitions and inheritance"]
    ```

    The current tracked deletions include roughly 1.34 million lines under `src`, 0.91 million under `data`, and 1.21 million under `tools`. Those are gross counts containing different mixtures of executable code, declarations, generated content, and tooling.

    They cannot be presented as completed replacement work.

    The final accounting should distinguish:

    - Old implementation genuinely replaced.
    - Generated text replaced by packed data.
    - Assets retained in a different representation.
    - Tooling no longer needed.
    - Duplicate content removed.
    - Required behavior still awaiting implementation.

    The strongest reductions should come from eliminating per-feature implementations of transactions, Pokémon construction, script state handling, repeated battle calculations, and copied content definitions.

    Binary packing improves the ROM representation. Compact notation improves the authored context. Shared ownership reduces the number of places that must change. Those are different measurements, and all three matter.

15. **Context size, completion evidence, and the editing workflow**

    The complete context must contain the actual authored gameplay model and executable source.

    ```mermaid
    flowchart TD
        A["Requested design edit"] --> B["Owning canonical definition"]
        B --> C["Automatic reconciliation"]
        B --> D["Dependency analysis"]
        C --> E["Updated game program"]
        D --> F["Affected strategic and narrative decisions"]
        F --> E
        E --> G["Compile and compare"]
        G --> H["Complete context and change report"]
    ```

    The context includes dialogue, progression, maps’ behavioral data, mechanics, compiler, and kernel. Pixel textures and audio samples can remain binary assets. Gameplay rules cannot be hidden in opaque blobs merely to meet the token target.

    The current representation is around 6M tokens. Reaching the 1M target requires major structural reduction, not a prettier summary of the same source.

    I would measure the actual costs by domain: dialogue, scenes, builds, species, world data, compiler, and kernel. This makes the next reduction concrete rather than speculative.

    For a typical edit, the intended workflow is:

    1. Change the owning fact.
    2. Recompute derived values and reconcile permitted selections.
    3. Recalculate relevant availability and economic consequences.
    4. Collect affected strategies and narrative premises.
    5. Resolve those design consequences in the same context.
    6. Build the game and inspect the relevant behavior.

    Necessary checks should verify real invariants without freezing arbitrary design choices. They should not demand that a particular trainer forever use a particular move.

    Full completion requires the complete playable replacement ROM, demonstrated fidelity, accounted-for campaign content, measured context size, and removal of the remaining obsolete architecture. No individual diagram, passing check, or deletion count substitutes for that evidence.

The central design principle across all fifteen points is **one owner per fact, explicit directional dependencies, automatic handling of mechanical consequences, and deliberate handling of strategic or narrative meaning**.
