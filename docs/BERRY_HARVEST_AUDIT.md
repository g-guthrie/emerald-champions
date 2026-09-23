# Berry harvest failure pass — September 19, 2026

Active bug: ObjectEventInteractionPickBerryTree returns 4 if the finite harvest
credit pouch cannot accept the crop. The shared tree script only rejected zero,
so it removed the plant and incremented daily picking despite receiving nothing.
The script now sends result 4 to a dedicated pouch-full explanation before any
success bookkeeping. Result 0 still reports full Bag; result 2 (mutation delivery
failure/rollback) also follows full-Bag handling. Success codes 1 and 3 retain the
existing collection/removal sequence. Mutation mechanics remain disabled in the
current configuration; no claim is made of mutation-mode runtime acceptance.

Native test executes the real special and comparison/jump opcodes up to their
selected handler, verifies failed paths never reach success bookkeeping, and
checks crop bytes, Bag quantities and harvest credits for successful, pouch-full
and Bag-full cases. All three parameter cases pass. Evidence and release logs:
work/berry-harvest-20260919/. No battle AI or encounter changes in this pass.

Remaining scope: harvest trade/credit economy and world distribution require the
broader campaign audit. This is a targeted crop-loss fix, not complete berry.c
or overall gameplay certification.
