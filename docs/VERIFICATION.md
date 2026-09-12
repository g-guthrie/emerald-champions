# Build and verification

Run commands from the repository root. [The CI workflow](../.github/workflows/build.yml)
provisions ARM GCC/binutils/newlib, native build tools, Python, libpng and libmGBA.
The [Makefile](../Makefile) defines the build. Put the ARM toolchain on `PATH`.

```sh
make -j4 release
python3 scripts/stamp_release_inputs.py
python3 scripts/verify_emerald_champions_release.py
```

Stamp the tree that actually built the ROM and ELF. For a container build, copy
both artifacts and their stamp together. Check copied artifacts with
`python3 scripts/stamp_release_inputs.py --check`. The release checker validates
configured data, ROM/ELF agreement, header/checksum, fixture exclusion and memory
bounds. Use `--allow-source-bundle` only for an intentional export without Git
metadata, and run `git diff --check` in the actual checkout.

## Runtime checks

Use existing focused tests that exercise the changed behavior. The native runner
supports selected filters and separate build/run environments:

```sh
python3 scripts/run_emerald_champions_runtime_gates.py --help
```

A test ELF built elsewhere needs its matching input stamp. Keep failed output
and audit the fixture, configuration, assertion and artifact identity before
changing code. Run no blanket suite merely to increase coverage totals.

| Purpose | Entry point | Limit |
| --- | --- | --- |
| Campaign traversal | `scripts/run_emerald_champions_campaign.py` | Automatically resolves battles; does not assess combat difficulty |
| Native battle play | `tools/agent_player/agent_player.py`, `scripts/battle_checkpoint_lab.py` | Requires campaign battle automation disabled and current artifacts |
| Visual scenarios | `scripts/render_emerald_champions_ui.py`, `scripts/verify_emerald_champions_visual_runtime.py` | Requires the test-only fixture build and screenshot inspection |

The campaign runner's legacy `battle_automation` manifest labels do not disable
its automatic wins. Agent-player success events are reported outcomes, not
independent proof. Preserve both pipelines and their distinct interpretation.
Use scratch ROM/save copies for automation, never a writable player save.

Current games target fresh saves. Preserve current-run rewards, failed-delivery
retries, save integrity and temporary-party restoration. Record exact build
identity and scoped results under ignored `work/`; a release check alone is not
a full campaign playthrough, visual review or balance assessment.
