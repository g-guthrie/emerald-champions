# Validated campaign tuning build

Built from the isolated source snapshot at `/private/tmp/ec-campaign-tuning-build-0905` with Arm GNU 15.2. The task-owned source files match the working game source. Other tasks may continue to change the shared working tree, so the included input stamp identifies this specific build.

All configured release gates and 23 selected native tests passed. This is not a full combat-difficulty playthrough. Old-save compatibility was not a requirement; use a new game for playtesting this campaign revision.

The shared root ROM/ELF files were left intact to avoid overwriting concurrent work. This directory contains the validated tuned ROM.
