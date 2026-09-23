# Item transactions pass — September 19, 2026

Partial item.c review: shared pocket insertion/count/removal, PC stack behavior,
compaction, and PC-menu/campaign handoff consumers. Preserved existing uncommitted
bundle-preview work. Remaining scope includes full pocket migration/registration,
Pyramid special inventory, all item metadata and all shop/reward interactions.

Fixed PC removal inspecting the pre-removal quantity when deciding to compact.
Removal now checks the resulting quantity, rejects invalid indices/excess counts
without mutation, and centralizes compaction. Removed redundant campaign handoff
compaction; PC menu list rebuilding still deliberately normalizes old contents.

Deleted the separate first-stack-only PC quantity helper and reused the existing
aggregate pocket check. AddPCItem can split stacks; ownership queries now agree
with that representation. Invalid items/zero requests are rejected. All active
campaign ownership callers request one item; no caller depends on a single stack
or an un-compacted hole. Save layout, prices and authored AI are unchanged.

Native before.log reproduces both defects. after.log passes two PC regressions
(split quantities; partial/full removal with invalid-input preservation), existing
Bag sorting/compaction and legacy pocket migration. Strict release logs are under
work/item-audit-20260919/. These are scoped checks, not complete item-system or
campaign certification.
