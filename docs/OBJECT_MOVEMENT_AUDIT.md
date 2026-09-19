# Object movement audit — September19

Scope: event_object_movement.c and the directly connected map-template, sprite
allocation and debug-preview callers. Solo source review; no campaign playthrough.
Existing convenience/XP/title work was preserved. Authored AI and teams unchanged.

## Fixed

1. Generated-map lookup disagreed with spawning. A shared current-template count
   now handles ordinary maps, Battle Pyramid and Trainer Hill in both spawn loops
   and script/flag lookup. Missing templates/event data return NULL or zero rather
   than dereferencing a missing object. Local IDs remain IDs, not array positions.
2. The direction-to-action macro accepted index==table length. It now uses element
   count and the existing DIR_NONE/downward fallback. Supported cardinal/diagonal
   entries retain their exact movement IDs.
3. Graphics-sheet replacement treated tile zero as an absent allocation. It now
   detaches using TAG_NONE and releases a zero-based sheet only when unshared.
   Retry checks use registered-sheet existence, distinguishing valid tile zero
   from the loader's zero failure return. A native allocator test failed before
   the repair and passes afterward for both shared and unshared ownership.
4. The connected trainer-debug preview read a freed temporary sprite template.
   It now reads the installed palette tag from sprite-palette bookkeeping.

## Simplified

- Removed38 per-call direction-table copies; routes are explicitly read-only.
- Removed temporary camera/speed callback tables and jump-displacement copies.
- Direction-action wrappers index their immutable tables directly.
- Made the private route-step helper static; removed a duplicate include and
  redundant map-number assignment.
- Preserved callback order, RNG calls, movement timers, paths, collision priorities,
  sprite coordinates, local IDs and asset metadata. No broad file split or new
  rendering framework.

The file went from11534 to11478 lines and426770 to423301 bytes before the final
small tile-sentinel correction. The normal ROM decreased by896 bytes versus the
preceding convenience release; EWRAM/IWRAM usage did not increase.

## Evidence

Deus codebase-memory-mcp refreshed the repository index (87605 nodes,409664 edges).
Its inbound trace identified25 callers across trainer scripts, followers, boulder
handling, decoration and object spawning. These links were cross-checked in source.
The index reports partial parses and cannot establish script-DSL completeness.
Raw MCP receipts: work/object-movement-audit-20260919/{index,symbols,template-callers,complexity}.json.

Native regression groups:3 PASS (direction boundary; static/generated/missing map
lookup; shared/unshared graphics-sheet ownership). Logs: test-final.log. The tile
leak negative result is preserved in tile-before.log. Normal build and deterministic
release gates PASS; release-gates.log binds the final ROM/ELF/input stamp.

## Limits and follow-up seams

This is a scoped source/refactor pass, not proof every overworld scene is flawless.
Sprite-template lifetime and allocation-failure policy span the general sprite
engine; its existing borrowed stack-template conventions and low-memory TODOs were
not broadly redesigned here. The proven debug consumer was repaired locally.
Disabled follower/compatibility paths and script table bindings were surveyed for
impact, not exhaustively executed. Full choreography and campaign acceptance were
not claimed or restarted.
