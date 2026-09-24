# Emerald Champions

A native Game Boy Advance campaign combining Inclement Emerald's world and
progression with authored doubles battles and shared battle AI.

Rules for working in this tree: [AGENTS.md](AGENTS.md).

- Build: `make -j4 release`
- Trainers: edit `data/emerald_champions/emerald_champions_battle_teams.txt`, then
  `python3 scripts/emerald_champions_teams.py --write` and `--check`
- Emerald Studio: `python3 tools/studio/cli.py start`

[Credits](CREDITS.md) · [Third-party notices](THIRD_PARTY_NOTICES.md)
