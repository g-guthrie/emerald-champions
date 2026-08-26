# Verdant battle-design operating system

This is the canonical production contract for authoring every physical trainer
encounter in Verdant. It supplements `verdant_battle_audit_v2.md`: that document
defines the design philosophy; this one defines the durable record, state
machine, and evidence required to move a battle forward.

The system supplies memory and evidence. It never allocates teams globally and
never replaces encounter-by-encounter judgment.

## Current agreed baseline and change control

Every design begins from an accurately observed current agreed baseline:
healing, Bag access, format routing, level caps, Mega access, rewards, shops,
encounter access, story progression, and save behavior. The baseline is evidence,
not doctrine, a frozen template, or a promise that the present rule is best. A
designer is encouraged to challenge it
when a different rule could make the game substantially better.

If a designer believes one of those rules should change, the dossier records a
`mechanics_proposal` with evidence, player impact, and alternatives. That keeps
the idea alive without silently mutating the campaign. Source changes wait for
the user's explicit approval of that exact proposal; battle-team authorship does
not imply permission to alter campaign rules.

## Battle lifecycle

Every encounter advances through these states in order:

1. **blueprint** — identity and scarce reveals are protected campaign-wide.
2. **authored-draft** — exact team, variants, AI, counterplay, references, and
   presentation exist, but the author has not certified its fun.
3. **author-self-checked** — the author has recorded the team's strongest part
   and weakest link and repaired any material weakness.
4. **design-complete** — the author has finished the design; source is
   still untouched.
5. **source-implemented** — the approved design is represented in game source.
6. **static-validated** — legality, format, scripts, AI flags, dialogue width,
   branches, guide, and source contracts pass.
7. **runtime-playtested** — representative real-ROM tests establish actual
   behavior and observed difficulty.
8. **closed** — every reachable variant passes and no required work remains.

`design-complete` must never be described as implemented, validated in the ROM,
or playtested. `target_difficulty` is an intention; `observed_difficulty` remains
null until runtime evidence exists.

## Author self-check

Every new dossier records `author_self_check.strongest_part` and
`author_self_check.weakest_link`. The first names the player-facing interaction
that earns the battle's place in the game. The second names the most generic,
fragile, or exploitable element and states whether it was repaired or retained
as healthy counterplay.

There is no external review loop. This is intentionally small enough to apply to
every required, optional, paired, branched, rematch, and postgame encounter.
Legality and source invariants remain hard gates; actual difficulty and AI
quality are established through representative runtime tests, then tuned with
levels before weakening sound strategy.

## Complete dossier

Each dossier contains the following sections.

### Identity and campaign state

- Stable `anchor_id` or canonical physical `encounter_id`.
- Canonical stage, location, strict cap, and cap-relative level semantics.
- Player catch pool and preparation tools actually available.
- Mega access and the current evolution phase.
- Healing, Bag, party-lock, save, and between-battle rules.
- Every reachable format, branch, paired state, rematch, or starter/gender
  variant.

### Rolling and campaign context

- Previous eight-to-ten physical encounters when chronological context exists.
- Nearby protected boss mechanics and future reveals.
- Species, family, Mega, legendary, item, move, tempo, and player-question
  overlap.
- Historic-team reservations and whether this encounter spends or preserves
  them.
- Any deliberate repetition and its written justification.

### Competitive research

- Exact competitive-index version, SHA-256, and record count.
- Mechanical queries used against the entire index.
- Ranked candidate reference IDs inspected in full.
- For every leading candidate: completeness, selected or rejected decision, and
  reason.
- Exact imported team, core, role, mechanic, or principle.
- Original gimmick dependencies and how non-Mega mechanics are removed.
- Public evidence URLs for curated elite records.

The full raw corpus stays on disk. The active dossier contains a compact digest
and expands only the strongest candidates.

### Exact battle design

- One-sentence memory hook and trainer/location story fit.
- Primary player question, primary mode, credible secondary mode, and preview
  pressure.
- Exact party order and intended leads/reserve sequence.
- For every Pokémon: species/form, level offset, item, ability and slot, spread,
  four moves, role, and Mega candidacy.
- Exactly one usable Mega at most. Tera, Z-Moves, Dynamax, Gigantamax, and
  Primal Reversion are never imported as Verdant gimmicks.
- AI flags, reusable AI requirements, custom state-machine requirements, and
  explicitly forbidden behaviors.

### Difficulty and fairness

- Target difficulty and rationale, separate from observed difficulty.
- Pressure sources and resource tax.
- Intentional weakness that the team does not silently erase.
- At least three broad counterplay classes.
- First-loss lesson and the information revealed by that loss.
- Unacceptable failure modes: hidden required catch, cheating information,
  one-line lock-and-key answer, excessive accuracy dependence, or AI that
  cannot execute the advertised strategy.
- Preferred tuning order, beginning with cap-relative levels when the strategy
  itself is sound.

Every independently reachable branch must clear the 7.5 floor. Ordinary
Verdant battles normally target roughly 8.5; marquee bosses target 10.

### Presentation and guide

- Intro, defeat, post-battle, and hint concepts truthful to the implemented
  strategy.
- Native line-width status.
- Guide summary, exact party table, behavior explanation, counterplay, and
  difficulty explanation generated from the same dossier.

### Evidence and status

- Source status, legality status, script/format status, AI status, dialogue
  status, guide status, and runtime status.
- Exact commands or source facts supporting each verified claim.
- Known blockers and unresolved engine dependencies.
- `mechanics_proposal`, if any, held for explicit approval.
- One non-empty `author_self_check` with strongest part and weakest link.

## Authoring loop

For each battle:

1. Observe and record the current mechanics baseline from source; challenge it
   freely in prose, but do not mutate it without approval.
2. Load the campaign blueprint and previous-ten context.
3. Query the complete competitive index and inspect top candidates.
4. Write the primary question and intentional weakness before finalizing six
   sets.
5. Author exact order, team, AI, counterplay, dialogue, and guide material.
6. Run hard dossier validation and advisory collision analysis.
7. Record the author self-check; repair the weakest link once if material.
8. Report what makes the encounter memorable, why it targets its difficulty,
   what informed it, what the AI must execute, and what the first loss teaches.
9. Mark only `design-complete`; wait for its chronological implementation turn.
10. When its turn arrives, implement, statically validate, playtest, tune, and
   close every reachable branch.

After every route or Gym, review overlapping previous-ten windows and the whole
anchor board. This is a judgment pass, not a quota or score.

## Backward design, forward closure

The marquee order is:

1. Elite Four and Wallace as one connected attrition arc.
2. Gyms backward from Juan through Wattson, preserving Roxanne and Brawly.
3. Final Maxie and Archie backward through both villain command structures.
4. Rival milestones, Steven, rematches, and superbosses.
5. Campaign-wide collision and coverage review.

Exact source implementation then resumes at Battle 29 and advances through
every required, optional, paired, branched, rematch, and postgame encounter in
canonical order.
