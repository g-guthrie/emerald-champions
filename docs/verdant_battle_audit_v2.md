# Verdant Battle Audit v2

This document is the editorial and executable design spine for rebuilding every
physical trainer encounter in Verdant. It replaces checklist-driven team
generation with rolling context, soft long-range anchors, searchable competitive
history, and exact source validation.

The audit is a gate plus an editorial review. It is not a global team allocator.

## North star

Every battle must be hard and memorable. The normal Verdant battle is roughly
8.5/10. Nothing closes below 7.5. A lower-density encounter may be easier to
read, but it must still punish autopilot. Major bosses should make a blind loss
the expected result while teaching the player enough to form a better second
plan.

Interesting strategy stays intact. When a sound battle is overtuned, first
adjust opponent levels relative to the fixed player cap. Do not make the AI
stupid or remove the mechanic merely to lower difficulty.

## Three design horizons

The production order is **anchor first, finish in sequence**. Plan backward only
far enough to protect the campaign's hardest encounters and scarce ideas; close
battles forward so every exact team is judged against the real cap, player
toolkit, evolution phase, story state, and preceding experience.

### Whole campaign: soft anchors

Reserve exact identities only for important encounters:

- Gym Leaders, rivals, villain admins and leaders, Elite Four, and Champion.
- Historic tournament teams or cores that deserve a marquee appearance.
- Major Mega, legendary, rare-Pokemon, reward, or story showcases.
- Faction doctrines such as Magma's elite sun/positioning play.

Ordinary trainers receive environmental and character notes, not mandatory
archetypes. A route may suggest quarry, forest, coastal, industrial, volcanic,
or school themes while leaving the exact team open until its battle is reached.

A major-battle blueprint is not a closed battle. It reserves an identity,
signature reveal, broad strategic mode, historic-reference candidates, and
difficulty intent. Exact sets, levels, AI, and dialogue remain provisional until
their chronological audit.

Use three planning tiers:

- **Marquee anchors:** rivals, Gym Leaders, admins and leaders, Elite Four,
  Champion, and superbosses. Reserve these across the campaign first.
- **Supporting anchors:** important story fights and exceptional route trainers.
  Give them a light identity only after marquee collisions are visible.
- **Chronological encounters:** finalize every physical battle in order. These
  remain locally authored rather than allocated from a global archetype map.

`reserved` protects an idea. `locked` means an implemented encounter has
survived chronological correctness, editorial, and playtest gates.

### Rolling context: previous eight to ten battles

Before authoring a battle, read the rolling experience ledger and ask:

- Which formats, tempos, primary questions, and lead structures appeared?
- Has the player repeatedly had to remove a support lead first?
- Have setup, weather, rooms, terrain, status, hazards, or Choice play clustered?
- Which species families, items, signature moves, and visual motifs are fresh?
- What does this trainer and location naturally suggest?
- Which nearby boss mechanic must remain reserved?

Similarity is a warning that prompts judgment. It is never an automatic ban.
Two nearby weather teams are valid when they create materially different player
decisions.

### Immediate encounter: bespoke construction

For the current trainer, record:

- A one-sentence memory hook.
- The primary player question.
- At most one ordinary supporting wrinkle; marquee battles may have phases.
- The intended first-loss lesson.
- At least three broad counterplay families.
- An intentional weakness the team does not silently erase.
- Why this character and location own this strategy.
- The exact AI behavior needed, if any.

Then query competitive references, inspect the strongest fits in full, author
the team by hand, and validate it against source.

## Difficulty calibration

The numbers are comparative editorial language, not quotas:

- **7.5-8.4:** transparent but dangerous; still beats careless play.
- **8.5-9.4:** normal Verdant challenge requiring adaptation.
- **9.5-9.9:** marquee rival, villain, route, or Gym-trainer threat.
- **10:** boss or gauntlet encounter intended to be nearly overwhelming blind.

Difficulty may come from strategy, level offset, team size, items, information,
switching, a timer, mixed offense, or resource preservation. Do not stack every
knob automatically. Prefer level-offset tuning after the strategy is sound.

## First-loss learnability

A fair first loss should reveal an actionable plan:

- stop or preserve a particular piece;
- reverse or stall a field state;
- change attack category, speed mode, switching pattern, or item plan;
- save a typed answer for a later wave.

A loss must not depend on an undocumented mechanic, one obscure required catch,
repeated accuracy variance, cheating information, or invisible coverage that
invalidates every apparent counter.

## Competitive reference policy

The full corpus remains on disk and searchable. Do not place all raw teams in
the active model context.

For every encounter:

1. Search all indexed references mechanically.
2. Rank by format, strategy, roster, era, location/faction fit, and stage
   adaptability.
3. Read the complete highest-value candidates.
4. Import a full team only when its identity naturally fits.
5. Otherwise adapt a core or role and record why the full import was rejected.

Historic championship teams should be reserved preferentially for bosses,
admins, major rivals, Elite Four, and high-status route trainers. Wolfe Glick
and other elite-player teams are tagged by event, player, strategy, core,
positioning demands, and Verdant adaptation fit. Public sources only; paywalled
material is never treated as verified evidence.

Do not reduce elite teams to an archetype label. Preserve, where known:

- the primary mode and credible secondary mode;
- preview pressure created even when a visible mode is not selected;
- role compression and partner dependencies;
- variance profile and the counter-meta problem the team targeted;
- whether Verdant can import the full team, only a core, or only a mechanic;
- the AI state machine and engine interactions required to make it authentic.

A famous roster is not automatically a good encounter. It earns a place only
when its decision structure survives removal of Tera, Z-Moves, Dynamax, Primals,
or other non-Mega gimmicks and naturally belongs to the trainer and campaign
stage.

## Correctness gates

These remain binary:

- Canonical encounter identity, campaign state, strict cap, and player access.
- Legal species, stage, form, move, ability slot, item, spread, and level.
- Correct singles, solo-double, native-pair, rematch, and branch behavior.
- No Mega before reachable player access.
- AI can execute the strategy without a forced hidden script.
- Declines and doubles guards cannot advance story or rewards.
- Dialogue, rewards, shop unlocks, and guide are truthful and native-width.
- Runtime branches and native-pair halves remain independently viable.

## Editorial review and closure

Before closure, compare the candidate with the previous ten encounters:

- Is its primary question genuinely different or deliberately revisited?
- Is its complexity proportionate to the trainer's importance?
- Does every party member reinforce the memory hook or provide necessary pacing?
- Could removing a mechanic improve the encounter?
- Is the intentional weakness still real?
- Would the player remember this trainer specifically?

Static checks establish legality and coherence. At chapter checkpoints, build
the ROM and play representative encounters to verify AI execution, pacing,
actual difficulty, first-loss clarity, and visual presentation.

Before every new team becomes design-complete, the author records exactly two
short judgments:

- **Strongest part:** the interaction, reveal, or decision that makes this
  trainer worth remembering.
- **Weakest link:** the most generic, fragile, or exploitable part of the team,
  plus whether it needs one repair or is healthy counterplay.

There is no external critic gate. The authoring pass already owns campaign
state, exact sets, legality, AI, references, and counterplay; repeating that work
in a second review loop does not scale across the full game. Exhaustive matchup
proof belongs to representative runtime testing. Difficulty is not weakened at
the design desk merely to satisfy a number; levels remain the later tuning
valve.

Current mechanics are the current agreed baseline, not a frozen or untouchable
rule. A dossier may propose a better rule, but battle authorship never silently grants authority
to change healing, access, format, caps, rewards, or progression. Record the
proposal and obtain explicit approval before mutating that campaign behavior.

## Scalable workflow

1. Extend the canonical physical-encounter sequence.
2. Read the soft campaign anchor and rolling last-ten ledger.
3. Write the encounter brief before selecting a roster.
4. Query the compact competitive index; inspect relevant full teams.
5. Author exact sets and contextual, reusable AI only where source needs it.
6. Run correctness gates and rolling-similarity warnings.
7. Generate the guide entry from source and brief.
8. After a route or Gym, perform a comparative chapter review and playtest.
9. Commit the coherent, verified chapter checkpoint.

The pipeline should make later battles faster to design without making them
formulaic: durable state supplies memory, while local judgment supplies the
battle.

### Legacy ownership boundary

`verdant_doubles_manifest.json` remains a record of established formats, party
sizes, cap-relative level semantics, and already-imported marquee source facts.
It is not the authoring system for future teams. During the bespoke march,
`verdant_doubles_conversion.py --check` may verify those invariants, but its
global import/apply path must not allocate archetypes, synthesize parties, or
overwrite a hand-closed encounter. New roster design lives in the chronological
encounter workflow above.

## Reference commands

Rebuild and verify the deterministic corpus index:

```sh
python3 scripts/build_competitive_team_index.py --write
python3 scripts/build_competitive_team_index.py --check
```

Request a small digest from the whole corpus, then expand only the strongest
candidates:

```sh
python3 scripts/search_competitive_teams.py --style doubles --tag trick-room --query "redirection" --limit 12
python3 scripts/search_competitive_teams.py --player "Wolfe Glick" --query "primary secondary" --json --limit 3
```

The generated `competitive_team_index.meta.json` records the exact index version,
record counts, source counts, and SHA-256 digest. Each closed encounter stores
that digest, its query, the reference IDs actually examined, and why candidates
were selected or rejected.
