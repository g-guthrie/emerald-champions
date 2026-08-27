# Emerald Champions progression contract

This is the campaign-level design contract for rewards, shops, encounters, and
postgame incentives. It exists to keep individual edits from quietly breaking a
different system later in the story.

## Player power is front-loaded on purpose

- Strict level caps define progression; experience grinding does not.
- The reusable Leveler is given beside the Poké Vial and raises every eligible
  party member directly to the current strict cap.
- Rare Candy costs $1,000, raises a Pokémon by up to ten levels, stops at the
  cap or the next crossed level-evolution threshold, and is present in every
  ordinary medicine Mart tier.
- Leveling never changes moves automatically. Level-up and evolution learnsets
  remain legality sources for the Pokémon Center teacher.
- The Pokémon Center teacher offers every move that the selected species can
  legally learn from level-up, Egg, TM/HM, or tutor data. It does not bypass
  species legality.
- The battle-set service keeps one individually authored Set 1 for every
  selectable species/form and offers up to two additional role-labeled,
  locally legal doubles sets when validated alternatives exist. Set choices
  never grant or equip their advisory held item.
- Native ability selection exposes that species' legal abilities without an
  Ability Capsule or Ability Patch tax.
- Every Pokémon Center battle vendor supplies the complete non-progression
  competitive held-item pool for free, in any requested quantity.

The intended challenge is solving battles with a broad tool kit, not repeating
wild battles or waiting for basic competitive equipment.

## The economy has four non-overlapping lanes

1. **Ordinary Poké Marts:** their original medicine, Ball, status-healing, and
   Repel progression, plus Rare Candy. No global battle-item suffix.
2. **Pokémon Center battle vendor:** every ordinary competitive held item and
   Berry is free and unlimited. It is loadout infrastructure, not an economy.
3. **Exploration and story:** Mega Stones, Primal Orbs, form items, rare
   Pokémon, Bottle Caps, valuable non-loadout consumables, and invitations.
4. **Specialty shops:** their existing local identity, such as type boosters,
   weather rocks, evolution items, incense, and gems.

Mega Stones, Primal Orbs, Plates, Drives, Memories, Ogerpon masks, Rusted
weapons, and comparable form/progression items remain singular progression
rewards and never enter the free stock or automated loadouts.

## Reward rules

- A fixed boss reward must be its item's first guaranteed source, unless the
  script explicitly provides a different reward when the player already owns
  or has unlocked it.
- Ordinary held-item rewards are no longer progression. Existing rewards need
  a later incentive redesign; they do not delay this loadout-system change.
- A one-time gift, bundle, or defeated-trainer reward advances its flag or
  state only after the complete reward was delivered. A full Bag must leave a
  truthful retry path.
- NPC dialogue describes the item actually received, not the TM that occupied
  the event in an older build.
- A late, gated item ball cannot contain one ordinary $1,000 Rare Candy merely
  because its former TM was removed. Candy remains appropriate only as early
  convenience or as part of a meaningful quantity.
- Free competitive held items never count as a boss prize or high-gate
  exploration reward.
- Bottle Caps remain meaningful because they feed the bounded IV service.
  Ability-changing items do not remain as rewards because ability selection is
  already free and native.

## Wild-encounter rules

- Every eligible ordinary random catch receives its authored doubles preset
  immediately before entering the party or PC: moves, PP, nature, and ability.
  IVs, EVs, level, personality, shiny status, gender, Ball, held item,
  friendship, and nickname remain individual.
- Every ordinary wild Pokémon uniformly rolls one of its finalized one-to-three
  tutor sets before battle and fights with that complete moves/nature/ability/
  item loadout. The caught Pokémon retains the same roll; no capture-hostile
  move is filtered. Evolutions never silently select a different set.
- Scripted, static, roaming, Frontier, Legendary, and Mythical captures retain
  their authored data. Existing owned Pokémon and later evolutions are never
  silently overwritten.
- Route signs list every ordinary encounter species grouped by encounter
  method. They intentionally omit percentages because the independent method
  pools do not form one meaningful 100-percent total.
- A unique or required species is never confined to one 4% or 5% source.
- Ordinary-wild legendary species use a catch rate of at least 45. Catch-rate-3
  battles are reserved for deliberate static encounters.
- Competitive value is distributed throughout the campaign: immediate tools,
  support Pokémon, weather pieces, speed control, priority, redirection, and
  strong evolution lines all appear before the League.
- Curated Generation 9 species receive deliberate first-access locations.
  Repeating one late-game table across many rooms does not count as meaningful
  distribution.
- A trainer's protected marquee reveal may be the species' exact set, form,
  item, Mega, or AI doctrine; it cannot falsely claim that an already catchable
  species has never been seen.

## Legendary and Mythical structure

Legendary access should use three distinct lanes rather than putting every rare
species into grass:

1. **Native static encounters:** preserve the existing story and exploration
   encounters such as the Hoenn weather trio, Regis, island events, and other
   map-authored battles.
2. **Campaign ecosystem encounters:** a small number of lower-impact or
   environment-perfect rare species may appear at clear 10–15% sources late in
   the relevant chapter. They must not duplicate a protected story reveal or
   use catch rate 3.
3. **Champion invitations:** box legends and Mythicals without a natural Hoenn
   home receive deliberate postgame encounters unlocked by difficult Battle
   Frontier milestones. Darkrai and Genesect belong here, not in anonymous
   Route 121 grass.

The recommended Frontier extension is a separate doubles-focused Champion
Circuit built from the checked-in competitive team corpus. It should reuse the
existing Frontier battle/runtime infrastructure, not embed an external service
or replace the seven facilities. A completed streak grants a named invitation;
the invitation opens a retryable static capture encounter. Exact streaks,
rosters, invitation order, and save flags require their own source-closed design
before implementation.

## Story invariants

- Badge, field-move, villain-team, ferry, weather-crisis, League, and postgame
  flags retain their original causal order unless an explicit design changes
  that story beat.
- Removing a TM object never removes the event, NPC, or story flag that gates
  subsequent progress.
- Mega access precedes the first opposing Mega, but individual Mega Stones can
  still motivate exploration.
- The Pokédex Current Area page is informational only. It does not unlock rods,
  field moves, maps, or encounters.
- The guide and executable ledgers are regenerated after every reward,
  encounter, or shop change.

## Closure evidence

Progression is not considered closed until automated reports can answer, for
every reward and encounter: first possible source, first guaranteed source,
campaign gate, permanent-unlock moment, later duplicates, retry safety,
displayed dialogue, exact encounter odds, and whether the source is reachable.

`scripts/emerald_champions_story_progression_audit.py` is the executable gate
for Gym chronology, first-Mega access, ferry wiring, postgame rival lifecycle,
guide chronology, and irreversible progression-gift retry safety.
