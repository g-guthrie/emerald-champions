# Emerald Champions

A native Game Boy Advance campaign combining Inclement Emerald's world and
progression with authored doubles battles, shared battle AI and selected
quality-of-life changes. The Inclement integration is incomplete and needs
native validation; a successful build is not campaign acceptance.

Read [working rules](AGENTS.md), [current work](docs/CONTINUE.md), and
[build and verification instructions](docs/VERIFICATION.md).

Trainer authoring lives in
`data/emerald_champions/emerald_champions_battle_teams.txt`.
Run `python3 scripts/emerald_champions_teams.py --write` to materialize changes
and `python3 scripts/emerald_champions_teams.py --check` to verify agreement.
Engine behavior lives in `src/`, `include/`, and `data/`.

Build a normal ROM with `make -j4 release`, then stamp and verify the exact
build as described in the development documentation. Player guides and the
Game Book have been removed; builds do not generate them.

Emerald Studio starts with `python3 tools/studio/cli.py start` and provides
native play, source editing, scene captures and build-bound checkpoints.
Preserve ROM/ELF/save ancestry: resume changed ROMs through a normal battery
save and clean boot, never a savestate from another ROM.

[Credits](CREDITS.md) · [Third-party notices](THIRD_PARTY_NOTICES.md)
