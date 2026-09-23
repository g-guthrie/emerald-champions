# Save/load audit — September 19, 2026

Partial review: physical save-slot validation, logical sector IDs, save counter
selection, fallback recovery, copied sector data and SaveBlock3 chunks; load_save.c
ASLR move/rekey, party/object persistence and legacy Bag snapshot restoration.
Full incremental/link/error-screen/interrupt behavior remains to audit.

Consolidated the two identical save-slot scanning loops into ValidateSaveSlot.
Retained signature and ID bounds checks, checksummed logical payload, counter
selection order, status codes and the existing recovery policy. Save layout,
checksum formula and bytes written are unchanged. Removed 41 net source lines.

Native tests use disposable emulator flash, not user saves. Save recovery test
writes two campaign states, erases one sector of the newest physical slot, checks
fallback returns SAVE_STATUS_ERROR while restoring the older full state, then
writes and reloads a repaired state successfully. Existing sign-state flash test
preserves all six groups of unlock/caught variables. Both pass; evidence in
work/save-audit-20260919/tests.log. Release logs share that directory.

The extension-checksum gap identified in this initial pass is addressed by the
versioned SaveBlock3 protection below. Legacy saves remain readable; newly written
sectors require the updated ROM. Full save validation and power-cut coverage are
still incomplete.

## Save-slot counter coherence repair

Native disposable-flash baseline proved ValidateSaveSlot accepted all14 valid
logical IDs/checksums even when one sector carried a different save counter.
It returned SAVE_STATUS_OK instead of recovering the older complete state.
Validation now rejects counter disagreement among checksum-valid sectors. Corrupt
or unrecognized sectors do not establish the reference counter. No format,
checksum formula, sector placement or counter-wrap policy changed.

Writer scan confirms full/incremental saves use one new counter across14sectors;
partial link/record-mixing saves retain the existing counter alongside untouched
PC sectors. The recovery fixture alters only the first physical sector footer,
checks older-state fallback, repairs via full save and verifies a subsequent
SAVE_LINK reload retains its counter and updated state. Both native save-recovery
groups pass, including erased-sector fallback. Initial test stalled from a4KiB
stack-local sector exceeding harness headroom; fixed by using gSaveDataBuffer.
That stalled fixture is not production evidence; the corrected baseline fails
specifically at acceptance status1 vs255. Evidence save-counter-* under
work/ability-audit-20260920. No user save files were edited.

The separate SaveBlock3 extension-checksum gap remains open and requires a
versioned compatibility plan; this patch does not claim full save corruption or
power-interruption coverage. Full core/campaign/battle/wild objective unfinished.

Strict release build/gates pass, stampc8b567c6ede0.

## Versioned SaveBlock3 checksum protection

Resolved the earlier extension-checksum gap for newly written sectors. New normal
save-slot signature0x08022025 checksums all4084payload bytes (3968data+116extension),
including deterministic zero padding. Legacy0x08012025 retains its original logical
payload checksum and remains readable. Shared PrepareSaveSector replaces duplicate
normal/incremental preparation. Footer/layout/sector sizes are unchanged; low commit
byte0x25 is compile-time checked equal, and HOF/special-sector formats stay legacy.
Counter coherence is still enforced, while legacy/new signatures may coexist in a
valid partial-upgrade slot. Existing legacy extension corruption cannot be detected
retroactively. New sectors are not readable by older ROM builds; after both slots
are rewritten, old-ROM downgrade cannot load this save. No user save was modified.

Native baseline proves a flipped extension byte was accepted as statusOK. All5
native recovery groups now pass: extension corruption falls back with both logical
and extension state restored; independent reconstruction of14legacy sectors loads;
SAVE_LINK yields5new+9legacy sectors and loads; next full save upgrades all14; full
incremental writer retains erased0xFF commit byte until final0x25 write and loads.
Prior mixed-counter and erased-sector fallbacks remain passing. Evidence under
work/ability-audit-20260920/save-extension-*. These are disposable-flash targeted
checks, not exhaustive electrical fault/power-cut or every UI save-path coverage.
Full core/campaign/battle/wild objective remains unfinished.

Strict release build/gates pass, stampc753e372acd2. Reviewed the worker's only edit,
tests/test_save_integrity.py: restored extraction of the shared validation helper
and added new signature/checksum helpers. Four host tests pass, including first/
last extension-byte corruption across all14sectors, mixed-version same-counter
loading, unknown signature rejection and counter disagreement. Native checks remain
the flash/writer evidence; host tests mock flash and UI, not electrical timing.

## Partial-save expanded Bag persistence repair

Native baseline: addSlowbronite after a full save, perform SAVE_LINK, reload;
the stone disappears because the old partial writer only persisted logical0..4
while expanded Bag data occupies SaveBlock3 chunks beside PC sectors5..13.
Synchronous and task-driven partial saves now update all extension chunks. For
PC sectors, read and validate the existing signature/id/counter/checksum, retain
its full saved PC payload, replace the extension chunk, recalculate the versioned
checksum and use the shared commit-last writer. Invalid source-sector validation
fails before erasing it. Existing saved PC snapshot and save counter stay intact.

Extracted WritePreparedSaveSector so full/incremental preparation and extension-
only preparation share the existing flash/commit path. Async pipeline now stages
through13 and commits13 on the completion call. Source-audited StartMenu, record
mixing and BerryBlender callers all wait forTRUE without fixed iteration limits.
Synchronous partials perform14writes instead of5; longer pause is expected. Older
peer BerryBlender link timing is not verified (additional standby rounds); do not
claim cross-version link support. This does not alter authored battle/AI content.

All6 native save groups pass, including both partial APIs retaining new MegaStone,
keeping old boxedPikachu despite RAM-only Eevee edit, and unchanged counters. Legacy
partial-save test now correctly expects all14sectors upgraded; manual mixed-version
compatibility remains covered by host tests. Four host save-integrity tests pass.
Evidence work/ability-audit-20260920/partial-bag-*. Full power-loss/failure injection,
link UI and complete game/campaign/battle/wild audit remain unfinished.

Strict release build/gates pass, stamp4f3a216047c6.
