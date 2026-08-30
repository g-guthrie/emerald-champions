# Champions Circuit source audit

## Executable contract

The postgame Circuit is an on-demand C port of Pokémon Showdown's Champions
random-doubles generator, pinned to commit
`bb179fbf8449e3c31632bd56f671ffb4404fa6e7`. The imported source file is
`data/random-battles/champions/doubles-sets.json`, SHA-256
`851114e68805aafbecbed0aaee7994164d199d890deed78bafc2beaf3c2221d8`.
The ROM contains 311 form variants and 444 role templates. Teams are assembled
at runtime; they are not selected from a finite bank of prebuilt parties.

`src/champions_circuit.c` preserves the important Showdown composition rules:
Species Clause by National Dex family, at most one Mega, Mega preference until
one is selected, no more than two of a type, weakness and double-weakness
limits, Freeze-Dry limits, weather and support incompatibilities, Zoroark lead
ordering, role-based move enforcement, paired moves, Ability selection,
role/item selection, and reverse insertion for Showdown's lead convention.

The Circuit deliberately retains Showdown Random Doubles' item behavior rather
than imposing a separate Item Clause. It does enforce one Mega because Mega
Evolution is Emerald Champions' only accessible battle gimmick.

## Emerald Champions adaptations

- Player and opponent start at level 80. After each win, one additional enemy
  slot rises by one level; after six wins the whole team has risen by one.
- Medium and Easy preserve the generated team and reduce every enemy by two or
  four levels through the same live difficulty path as the campaign.
- The player's exact prepared party is backed up before normalization and
  restored on defeat, retirement, or generation failure. HP and held loadouts
  are restored after every match.
- Every opponent receives the strongest doubles AI profile.
- Twelve finite legendary rewards arrive every two wins, followed by Eternatus
  at win 40 only after all twelve have been delivered.
- Showdown's official Ability roster is translated through nine explicit
  Inclement-rebalance mappings, affecting eleven template assignments. The
  mappings are stored in the generated manifest rather than hidden in a
  runtime fallback.

## Defects found and repaired in this pass

1. Eleven template Ability assignments were no longer legal after the selected
   Inclement Ability rebalances. Runtime silently used slot zero. The generator
   now emits Triage Meganium, Solid Rock Torterra, Motor Drive Rotom-Fan,
   Competitive Pyroar, Gooey Goodra, and Insomnia Gourgeist variants. Both the
   static gate and native test enumerate every template assignment.
2. Circuit Ability resolution no longer searches for an arbitrary fallback
   slot. An impossible assignment asserts, while the release gate prevents it
   from reaching a shipping build.
3. Move-type inference only understood Pixilate and Liquid Voice and looked at
   base template Abilities during Mega set construction. It now uses the Mega
   form Ability and handles Aerilate, Galvanize, Normalize, Pixilate,
   Refrigerate, and Liquid Voice, matching Showdown's STAB-enforcement intent.
4. Froslass could receive Triple Axel without Showdown's Wide Lens rule. The
   priority-item port now includes that condition.
5. Toucannon could receive Sheer Force even when the pinned template exposed
   only Keen Eye and Skill Link. Non-Skill-Link sets now retain the template's
   authored first Ability.
6. The lobby could call a team "balanced" when Trick Room existed outside the
   first two generated records. Presentation now scans all six sets.

## Proven checks

- The pinned commit, source hash, variant count, template count, generated C
  table, third-party notice, unbiased sampling calls, dedicated save variables,
  and reward schedule are statically gated.
- All template Abilities resolve against the configured species table.
- Native tests generate 96 complete live sets across 16 seeds, require six
  unique families, four ordered moves or an explicitly complete shorter
  Showdown set, 66 Stat Points, perfect legal Abilities, level escalation,
  difficulty reduction, and at most one Mega.
- A separate native test proves exact player-party restoration after a run.

## Remaining human-only risk

Static and deterministic tests prove generation legality and state safety, not
the subjective quality of an indefinitely long random run. Release playtesting
must still sample many seeds, weather modes, Trick Room teams, Illusion leads,
Mega variants, reward milestones, defeat, retirement, and a full-party/full-PC
reward failure on real emulator timing.
