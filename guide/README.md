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

## Still to build

    1. Map plate renderer   composite data/layouts tiles into a town plate, then
                            label the buildings the way the original does
    2. Capture pipeline     a scene recipe per event, rendered through
                            tools/studio/run_scene.py against the release ROM
    3. Page compiler        location JSON -> HTML, so 240 pages are generated
                            rather than hand-set
