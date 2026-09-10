# Emerald Champions — playtest snapshot

Built from the current local worktree on September 9, 2026, at the user's
request. This is an in-progress playtest, not a completed campaign audit.

Open the included `.gba` in your emulator. Start a new save: old-save
compatibility is not supported by this revision. Preserve backups of any
existing ROMs and saves; this snapshot does not replace them.

Includes the current Gen9 EV / universal31IV stat system, editable EVs,
HP-threshold berry convenience, trainer improvements through Joey and the
current in-progress Jose pressure preference. Marlene's Gothita has the
user-approved trainer-only Dazzling Gleam. Joey now has Friend Guard Clefairy
supporting protected Belly Drum and boosted Extreme Speed.

Audit coverage at snapshot:24/516 trainer branches,19/468 encounter records
complete under bounded native strategy review. Jose's pressure preference has
passed22 current-stat parameter cases, but his encounter review is unfinished.
The rest of the campaign remains playable content, not verified-complete
trainer coverage. Known tactical imperfections remain.

Decision target: preferably under1second, at most1.2seconds for the complete
opponent decision. Sample measurements are not a campaign-wide timing guarantee.

If something breaks, report where it happened, your party and moves, the
opposing trainer/turn, and whether it was a crash, freeze or bad decision.
Preserve the save/state around the issue if possible.

The dated ROM is an immutable snapshot; subsequent trainer reviews do not
silently update this download. Build and artifact verification results are
recorded separately with this snapshot.
