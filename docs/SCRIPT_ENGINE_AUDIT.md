# Script engine pass — September 19, 2026

Source inspected: script.c initialization, bytecode/native dispatch, return
stacks, script transfers, byte readers, field-control lifecycle, map script
lookup/callbacks, RAM-script validation and effect-analysis loop. Connected
scrcmd.c comparisons/native waits and battle_setup.c generated script sequences.
These are scoped source checks; not every assembled NPC path has been executed.

Fixed incomplete InitScriptContext reset by clearing the whole structure before
setting mode and command bounds. Prior comparison and wait bits no longer leak
between contexts. Both return-stack implementations now accept all 20 slots and
reject the next push without mutation. Their opposite boolean success conventions
were intentionally preserved. Context transfer requires no reserved sentinel
slot and retains existing nested calls beneath trainer snippets.

Consolidated read-and-advance helpers with the corresponding peek decoders;
bytewise little-endian decoding still handles unaligned script operands.
Native before.log reproduces both initialization and capacity failures. after.log
passes both plus unaligned/read-offset coverage. The larger campaign test exposed
an outdated expected state: Aurora Ticket receipt is required independently of
Steven's win. Expanded its exhaustive mask to test both receipt states rather
than weakening the actual receipt gate; reset every flag changed by that test.

Evidence: work/script-audit-20260919/. Strict normal release compilation and
all deterministic release gates passed. Campaign regression results are in
campaign-tests.log. No authored AI changes. This does not certify every script
command, map path, malformed ROM input or the entire core goal as complete.
