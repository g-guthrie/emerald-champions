# Targeted map diagnostics (2026-09-08)

The historical Inclement baseline diff and reachability helpers were retired.
Current game code, not a pinned historical map or redesign snapshot, is authority.
`textwidth.py` remains an optional heuristic: it does not resolve concatenated
assembly strings, dynamic variables, or cursor/font controls and cannot certify
dialogue layout.

`map_integrity.py` replaces the earlier ad-hoc map sweep with a source-derived
tile and geometry inventory. Build the canonical graphics converter first, then
run:

```sh
make tools
python3 scripts/audit/map_integrity.py --out work/audits/map_tile_inventory.json
```

The required release gate runs only the compiled structural check, with dynamic
inventories disabled. It distinguishes registered source
maps from the Hoenn maps and Emerald layouts actually emitted by `mapjson`.
Invalid compiled references fail; dormant findings remain visible in the report
and also fail when `--all-data` is requested. The report keeps exact packed map
cells, engine masks, shared metatile definitions, converter-derived payloads,
palette sources and possible animation ownership. It uses the real converter's
`-Wnum_tiles` behavior rather than assuming a requested truncation occurred.

The optional `map_dynamic_inventory.py` helper supplies script-root contexts, animation ranges,
public map setters, graphics loaders and memory-transfer source locations.
Shared contexts are possible call paths, not demonstrated execution. Dynamic
coordinates, layout changes, conditional branches and generic memory operations
remain explicitly unresolved where their state cannot be derived. A successful
structural check does not certify those states or visual fidelity. Its source
expression parser has known limitations documented in the static-check audit;
these inferred dynamic ranges are not release certification.

Render a selected map range from a freshly built fixture tree:

```sh
make -j4 BUILD_NAME=emerald-headless EC_HEADLESS_FIXTURES=1
python3 scripts/stamp_release_inputs.py --stamp pokeemerald-headless.inputs.json
python3 scripts/audit/map_sweep_render.py work/map-sweep 137 138 \
  --rom pokeemerald-headless.gba --elf pokeemerald-headless.elf \
  --stamp pokeemerald-headless.inputs.json
```

Run the sweep against the same source snapshot used to build and stamp the
artifacts. It rejects source or artifact changes, invalid selections and failed
native map/position observations. `map_sweep_report.json` records every attempted
map; any failed map makes the command fail. A screenshot and correct initial map
position do not prove campaign traversal or visual fidelity. Map-entry scenes
that move the player need dedicated scenarios rather than relaxed assertions.
