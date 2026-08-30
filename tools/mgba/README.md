# mGBA

The binaries in this folder are built from `mGBA`, an emulator for running Game Boy Advance games. The source code is available here: <https://github.com/mgba-emu/mgba>.
The source code for these specific builds is available from:

 - Windows: <https://github.com/mgba-emu/mgba/tree/7ee2be6c96222dca12a9a579b747fe5ff1829def>
 - Linux: <https://github.com/mgba-emu/mgba/tree/dbffb46c4e7d2e7a2cbed7c3488cece4c2176d4c>
 - Mac: <https://github.com/mgba-emu/mgba/tree/daf01b03d5316dac966acd4b05318a225cab12f5>

## Emerald Champions visual harness

The complete official mGBA 0.10.5 source at commit
`26b7884bc25a5933960f3cdcd98bac1ae14d42e2` is vendored in
`tools/mgba-source/` under its MPL-2.0 license. The deterministic host runner
is `tests/headless/emerald_champions_mgba_runner.c`; it links against the
installed native libmGBA and provides fixed-frame inputs, memory reads/writes,
scratch saves, video hashes, and PNG screenshots.

Build the test-only fixture ROM with `EC_HEADLESS_FIXTURES=1`, then render the
review matrix with:

```sh
python3 scripts/render_emerald_champions_ui.py all
```

The fixture bridge is compiled out of production (`EC_HEADLESS_FIXTURES=0`).
Never point mGBA automation at a writable user save. The renderer copies the
ROM into a private scratch directory and rejects nonresident zero-block input.

### macOS iCloud warning

An adjacent Desktop save marked `compressed,dataless` can make mGBA appear to
crash while APFS materializes it. The emulator and ROM are not at fault. Keep
the live ROM/save pair outside iCloud (the project `playtest/` directory is the
canonical location) and do not use the Desktop copy for automated runs.
