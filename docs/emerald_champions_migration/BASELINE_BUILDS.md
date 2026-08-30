# Baseline builds

> **Historical migration measurements, not current artifacts.** The source
> commits, ROM digests, and memory figures below identify the two foundations
> that were compared. They must not be reused as evidence for the current
> working tree or a future release candidate.

Both baselines were built in `devkitpro/devkitarm:latest` with `make -j4`.
The Docker container installed only the host build dependencies `libpng-dev`
and `pkg-config`; neither dependency is linked into the GBA ROM.

## Expansion master comparison

- Source: `b05016d5b48e8ca9376245e19ef71d5496f84232`
- SHA-256: `a1fedee97ac3e9a9dd07cf3d8cc2f0bd2470f0e69d442050429c5416e4a3cffe`
- Padded ROM file: 33,554,432 bytes
- Linked ROM used: 26,495,284 bytes (78.96%)
- EWRAM used: 226,584 / 262,144 bytes (86.43%)
- IWRAM used: 28,388 / 32,768 bytes (86.63%)

## Emerald Champions GEN_CHAMPIONS foundation

- Source: `a994c5e8613bf54479a24b5d9710844ab63a8ef0`
- Configuration: `GEN_LATEST = GEN_CHAMPIONS`, expansion intro disabled
- SHA-256: `ceabcb24c3ed70a9fed773c4e728769271fa38181b4d018f7e17312b8d54399c`
- Padded ROM file: 33,554,432 bytes
- Linked ROM used: 26,724,236 bytes (79.64%)
- EWRAM used: 226,424 / 262,144 bytes (86.37%)
- IWRAM used: 28,388 / 32,768 bytes (86.63%)
- Remaining linked ROM: 6,830,196 bytes (about 6.51 MiB)
- Remaining EWRAM: 35,720 bytes (about 34.88 KiB)
- Remaining IWRAM: 4,380 bytes (about 4.28 KiB)

The Champions rules/data delta adds 228,952 ROM bytes relative to master and
uses 160 fewer EWRAM bytes. IWRAM is unchanged. No pruning is justified at
this checkpoint.
