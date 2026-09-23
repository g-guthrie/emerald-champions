# Event-state API audit — September 20, 2026

Reviewed all event_data.c entrypoints: initialization, temp/daily reset spans,
Dex/RTC enable state, saved/special/test variable and flag addressing, and
set/toggle/clear/read behavior. Related definition/table scan confirms saved vars
0x4000..0x40FF, special vars0x8000..0x8015 (22 pointers), saved flags1..0x95F,
special flags0x4000..0x407F, testing vars0x9000..0x9007 and flags0x5000..0x5007.
END constants are inclusive. No legitimate aliases require the intervening gaps.

Added bounds before indexing each backing array. Previously a variable immediately
after VARS_END returned an address past saved vars; corresponding flag/special
ranges also lacked upper limits. Native before.log proves the invalid pointer;
after.log verifies null for gaps/out-of-range and independent first/last valid
entries. VarGet's literal-on-null behavior is preserved, as are VarSet failure
and flag no-op/false semantics. Simplified equivalent FlagGet boolean logic.

This is malformed-ID hardening: no active campaign operand was identified as
currently exploiting the gaps. Direct script commands that dereference a NULL
GetVarPointer still require valid bytecode; no claim of arbitrary malformed-ROM
safety is made. Save layout and valid flag/variable identities are unchanged.

Two native API groups and six compiled campaign-gate groups pass. Evidence:
work/event-data-audit-20260920/. Broader save migration/corruption, all callers,
and campaign state transitions remain separate coverage requirements.
