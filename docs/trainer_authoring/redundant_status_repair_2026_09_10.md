# Repeated status as a false guard — 2026-09-10

Lung's isolated old Shedinja/Venomoth pair exposed an actual repeated
Will-O-Wisp attempt. Against Earthquake Dugtrio/Air Slash Xatu, Wisp succeeded
on1 (Dugtrio status16, burn; PP15→14), then targeted the already-burned Dugtrio
on2 (PP14→13). This was not just a printed next-turn intention.
See `work/lung-gen9-status-review.log`.

The native opinion recognized the bad status: the post-turn1 score probe gives
Wisp90 against burned Dugtrio, versus Poltergeist110, Shadow Sneak102 and
X-Scissor102. However, the pair evaluator also forecasts known Sucker Punch:
its58 damage is lethal to1-HP Shedinja, and Dugtrio is faster at equal priority.
The failed status action could therefore beat attacks by avoiding the forecast
Sucker Punch. The old Shedinja set lacked Protect. This combines poor team
options with an action-filter gap, rather than an unknown burn flag or a need
to simulate additional turns.

## Correction

The existing `BuildPairActions` known-failure filter now also excludes an
immediate, single-target, pure nonvolatile-status move aimed at an already
statused foe, when another candidate remains. This covers the demonstrated
burn repeat and the same structural mistake for other immediate statuses.
It does not model future cures or claim perfect target prediction.

Damaging moves with status secondaries, allied actions, spread/random/smart
targeting and delayed Yawn retain their previous handling. If every candidate
would be filtered, the original legal options remain: never invent Struggle
or an unavailable move. Other status failures (ability immunity, reflection,
terrain, etc.) retain existing native scoring; this is not a claim to have
hard-filtered every possible failed status action.

The check runs in the existing bounded candidate pass, not inside a new
simulation or recursive search. No new buffer, runtime state or allocation.
ROM size growth was not measured and no release ROM was built.

Lung separately gains actual Protect on Shedinja, plus targeted defensive
changes described in [his review](e0110_lung_gen9_2026_09_10.md). Those local
improvements do not substitute for the shared correction.

## Necessary regression evidence

Two small retained groups in `test/battle/ai/emerald_champions_plans.c`:

- Frozen pre-authoring two-turn native reproduction. First burn must remain
  applied, but Wisp PP must be14 rather than13 after two turns. It fails before
  the filter and passes afterward; no specific replacement attack is forced.
- Two controls: a damaging Flamethrower remains usable against burned targets;
  a Pokémon with only Wisp retains that legal fallback when both foes are burned.

`work/status-legality-before.log`: reproduction fails13≠14; controls pass.
`work/status-legality-after.log`: both groups/all3 cases pass.
`work/ai-and-lung-final.log`:61/61 groups pass, including Lung's two scratch
groups/36 cases and59 retained groups. The shared clean run after scratch removal
passes59/59 groups in `work/ai-shared-through-lung-final.log`.
Lung's complete six-player-bench sample is60 frames, with25 isolated and58
initial. This is bounded native evidence, not campaign-wide latency certification
or proof that every previously reviewed battle was replayed after the change.
