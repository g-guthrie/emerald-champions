# Emerald Champions Official Field Guide

A print guide for this game, built in the page grammar of the 2005 Prima
Pokémon Emerald guide: location panels, item checklists, mart and encounter
tables, tip callouts, numbered event blocks with captures, and a folio.

Everything here is our own text and art. No Prima or Nintendo logo, artwork or
copy is reproduced; the wordmark is the game's own title-screen asset.

## Build

    guide/style.css          the print design system
    guide/assets/            logo, captures, map plates
    guide/out/pilot.html     the pilot spread
    guide/out/pilot.pdf      rendered with headless Chrome

Render:

    "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome" \
      --headless --disable-gpu --no-pdf-header-footer \
      --print-to-pdf=out/pilot.pdf out/pilot.html

## Data sources already in the repo

    src/data/wild_encounters.json          encounter tables per map and method
    data/maps/<Map>/scripts.inc            mart stock
    src/data/items.h                       item display names and prices
    Game Blueprint/..._Game_Book.txt       every pickup, hidden item and event, per map
    data/emerald_champions/..._teams.txt   all 341 authored trainer parties
    work/playtest/battle_index.json        campaign order, class, cap window, location

## Captures

Every picture in the book is taken from the running game. `guide/capture.py`
boots the headless build, warps to a map and tile, optionally walks and presses
buttons, and saves the framebuffer:

    .venv-studio/bin/python guide/capture.py guide/shots/petalburg.json

A shot names a map directory (`PetalburgCity`, not the MAP_ constant), a tile, a
facing, how long to settle, and optionally `chapter` for the campaign state it
needs and `steps` for input. Failures are reported per shot and written to the
manifest, so this pass doubles as a sweep of the game: a map that will not load,
a warp into a wall, or an actor who is not where the book says all surface here.

## Still to build

    1. Flag staging         a shot option to set or clear story flags, so an
                            event can be photographed in the state it happens in.
                            Norman is hidden in every fixture chapter today.
    2. Map plate renderer   composite data/layouts tiles into a town plate, then
                            label the buildings the way the original does
    3. Page compiler        location JSON -> HTML, so 240 pages are generated
                            rather than hand-set
