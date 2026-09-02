#!/usr/bin/env python3
"""Export a self-contained design brief for an external model (ChatGPT Pro etc.).

Writes docs/chatgpt_battle_design_prompt.txt: the mission, every rule the
static gates enforce, the exact output format the teams compiler accepts, the
complete battle list with caps, formats and the current draft teams, the full
species roster with types, Abilities, base stats and evolution levels, every
Mega Stone, the legendary availability table, and the held-item universe.
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import emerald_champions_teams as teams  # noqa: E402
import audit_emerald_champions_master_battles as audit  # noqa: E402

OUT = ROOT / "docs" / "chatgpt_battle_design_prompt.txt"


def species_roster() -> list[str]:
    rows = []
    evo = {}
    for path in (ROOT / "src/data/pokemon/species_info").glob("gen_*_families.h"):
        for level, evolved in re.findall(r"\{EVO_LEVEL,\s*(\d+),\s*SPECIES_([A-Z0-9_]+)", path.read_text()):
            evo[evolved] = min(evo.get(evolved, 999), int(level))
    types = audit.species_types()
    for path in sorted((ROOT / "src/data/pokemon/species_info").glob("*.h")):
        text = path.read_text(errors="ignore")
        for block in re.split(r"\n    \[SPECIES_", text)[1:]:
            name = block.split("]")[0].strip()
            if name == "NONE" or "_MEGA" in name or name.endswith("_GIGANTAMAX") or "TOTEM" in name:
                continue
            ab = re.search(r"\.abilities\s*=\s*\{([^}]*)\}", block)
            stats = [re.search(rf"\.base{k}\s*=\s*(\d+)", block) for k in ("HP", "Attack", "Defense", "SpAttack", "SpDefense", "Speed")]
            if not ab or not all(stats):
                continue
            abilities = [t.strip()[8:] for t in ab.group(1).split(",") if t.strip() and "NONE" not in t]
            typ = "/".join(t[5:] for t in types.get("SPECIES_" + name, ())) or "?"
            st = "/".join(m.group(1) for m in stats)
            rows.append(f"{name} [{typ}] {'|'.join(abilities)} {st}" + (f" evo@{evo[name]}" if name in evo else ""))
    return rows


def megas() -> list[str]:
    pairs = set()
    for path in (ROOT / "src/data").rglob("*.h"):
        for species, item in re.findall(r"MEGA_EVOLUTION[A-Z_]*,\s*SPECIES_([A-Z0-9_]+),\s*ITEM_([A-Z0-9_]+)", path.read_text(errors="ignore")):
            pairs.add((species.replace("_MEGA", ""), item))
    return sorted(f"{item}: {species}" for species, item in pairs)


def legendary_table() -> list[str]:
    text = (ROOT / "src/data/pokemon/legendary_signs.h").read_text()
    rows = []
    for macro, args in re.findall(r"(WILD_SIGN|OTHER_SIGN|VISIBLE_SIGN|ORDINARY_WILD_SIGN)\(([^)]*)\)", text):
        parts = [p.strip() for p in args.split(",")]
        mon = parts[1]
        if macro == "WILD_SIGN":
            rows.append(f"{mon}: conditional wild in {parts[2]} after badge {parts[5]} (needs {parts[7]} in party)")
        elif macro == "VISIBLE_SIGN":
            rows.append(f"{mon}: visible static encounter in {parts[2]} after badge {parts[3]}")
        elif macro == "ORDINARY_WILD_SIGN":
            rows.append(f"{mon}: ORDINARY wild encounter in {parts[2]} (available as soon as the area is)")
        else:
            rows.append(f"{mon}: {parts[2].replace('LEGENDARY_SOURCE_', '').lower()} (circuit = after the Elite Four; mastery = late postgame; game corner/breeding = mid-game)")
    return sorted(rows)


def battle_list() -> list[str]:
    branches = teams.read_teams()
    master = teams.MASTER.read_text()
    _prefix, blocks = teams.split_encounters(master)
    party = (ROOT / "src/data/trainers.party").read_text()
    classes, names = {}, {}
    for block in party.split("=== TRAINER_")[1:]:
        trainer = "TRAINER_" + block.split(" ===", 1)[0]
        m = re.search(r"(?m)^Class: (.*)$", block); classes[trainer] = m.group(1) if m else ""
        m = re.search(r"(?m)^Name: (.*)$", block); names[trainer] = m.group(1) if m else ""
    by_trainer = {b.trainer: b for b in branches}
    rows = []
    for number, block in blocks:
        cap = teams.line_value(block, "strict_cap")
        rows.append("")
        rows.append(f"### ENCOUNTER {number:04d} | chapter: {teams.line_value(block, 'chapter')} | location: {teams.line_value(block, 'location')} | level cap: {cap} | requirement: {teams.line_value(block, 'requirement')}")
        for mark in teams.BRANCH_RE.finditer(block):
            trainer = mark.group(1)
            seg = block[mark.start():]
            fmt = teams.line_value(seg, "format")
            b = by_trainer[trainer]
            rows.append(f"## E{number:04d} {trainer} class={b.cls}   # {classes.get(trainer, '?')} {names.get(trainer, '?')} / {fmt} battle")
            rows.append(f"plan: {b.plan}")
            rows.append(f"crack: {b.crack}")
            for mon in b.mons:
                rows.append(f"{mon.species} @{mon.item} {mon.ability} {mon.nature} {mon.points} {mon.offset:+d} | {', '.join(mon.moves)}")
    return rows


PROMPT = """
================================================================================
EMERALD CHAMPIONS — FULL CAMPAIGN BATTLE DESIGN BRIEF
================================================================================

You are designing every trainer battle in "Pokémon Emerald Champions", a
pokeemerald-expansion ROM hack that turns Hoenn into a hard, no-grind, doubles
focused Pokémon Champions experience. The player receives competitive Pokémon,
free held items, a teambuilder for Stat Points, Mega Evolution after the second
Gym, and can catch nearly every species, every Mega Stone and every legendary
(via "Legendary Signs") over the campaign. By the end of the game the player
should be able to say two things: "I am very good at Pokémon" and "I had the
Pokémon experience of a lifetime — I saw every generation, every rare Pokémon,
every Mega, every legendary, every real competitive strategy."

YOUR JOB
--------
Produce a complete, bespoke, hand-designed team for every one of the 561
trainer branches listed under BATTLE LIST, in the exact OUTPUT FORMAT below,
so that the file can be dropped straight into the game's compiler and pass its
legality gates. Think about every battle individually and in the context of
the whole arc. Do not template. Do not reuse the same set on many trainers.
Every battle must be interesting: a real doubles (or singles) plan, smart item
choices, coherent Abilities, natures and Stat Points, and a "crack" the player
can discover.

DESIGN PHILOSOPHY (from the game's author)
------------------------------------------
1. Every battle is interesting. Even a "casual" trainer has a real plan (a
   gimmick, a synergy, a lesson). Nobody is a pushover because they are stupid.
   All trainers get sharp AI; bosses get omniscient AI.
2. The difficulty curve is enforced through LEVELS and TEAM SIZE first, and
   through strategy quality and Pokémon rarity second. Casual trainers sit a
   few levels below the cap with 2-3 Pokémon. Route aces, Gym trainers and
   Team Magma/Aqua sit at the cap with 3-4. Admins and rivals sit above the
   cap with 4-6. Gym Leaders, the Elite Four and the Champion sit well above the
   cap with 6 and the best strategies in the game.
3. The coolest Pokémon belong to the coolest battles. Rarity scales with
   importance: box legendaries (Kyogre, Groudon, Dialga, Palkia, Giratina,
   Reshiram, Zekrom, Kyurem, Xerneas, Yveltal, Zygarde, Eternatus, Koraidon,
   Miraidon, Lunala, Necrozma, Calyrex, Arceus...) belong ONLY to the villain
   leaders, the Elite Four, the Champion and postgame bosses. Sub-legendaries
   (Cobalion trio, beasts, Tapus, genies, lake trio, Regis, Ultra Beasts...)
   belong to aces, Gym trainers, admins and Gym Leaders, and only from the
   point in the story where the player could catch them (see LEGENDARY
   AVAILABILITY). Mythicals go to memorable one-off trainers late in a chapter.
   Ordinary Youngsters and Swimmers never carry a legendary.
4. Custom Megas (the non-canon ones in the MEGA STONE list, e.g. Absolite Z,
   Garchompite Z, Lucarionite Z, Raichunite X/Y, Mewtwonite, Zeraorite,
   Zygardite, Darkranite, Heatranite, Magearnite, Tatsugirinite, Chandelurite,
   Pyroarite, Emboarite, Golurkite, Skarmorite, Staraptite, Baxcalibrite,
   Scovillainite, Chimechite, Floettite, Falinksite, Meowsticite, Drampanite,
   Crabominite, Feraligite, Greninjite, Eelektrossite, Meganiumite, Dragalgite,
   Chesnaughtite, Delphoxite, Froslassite, Glimmoranite, Golisopite, Hawluchanite,
   Malamarite, Scolipite, Scraftinite, Starminite, Victreebelite, Dragoninite,
   Audinite, Clefablite) are reserved for Gym Leaders, admins, the Elite Four,
   the Champion and postgame bosses. Canon Megas can appear on route aces,
   Gym trainers and grunt aces from level cap 30 onward.
5. Trainer identity matters. A Bird Keeper uses birds, a Hex Maniac uses
   Ghosts, a Ninja Boy uses Poison/Bug/Dark tricks, a Kindler uses Fire, a
   Fisherman uses fish, Team Aqua uses Water/Dark/Poison and Team Magma uses
   Fire/Ground/Dark/Poison, Psychics use Psychic types, Twins and couples run
   pair synergies, Pokéfans run pampered pets, Breeders run hatchlings, Rich
   Boys and Ladies run rare or expensive Pokémon, etc. Gym trainers must use
   the Gym's type for at least half their team. The rival's team must GROW
   across their five battles (same core, evolving, gaining members). Wally's
   team is built around his Ralts line and grows to a Mega Gallade.
6. Real doubles strategy: weather (Drizzle/Drought/Sand Stream/Snow Warning),
   terrain, Trick Room, Tailwind, redirection (Follow Me/Rage Powder),
   Fake Out tempo, Intimidate cycling, Helping Hand, screens, priority,
   Perish trapping, Beat Up + Justified, Instruct, Commander, Power Spot,
   Plus/Minus pairs, Weakness Policy, Sitrus/Figy berries, Eject Button,
   Choice items, Life Orb, Focus Sash, Eviolite on first stages, etc.
   Vary strategies between consecutive encounters. Never put the same species
   in two of any three consecutive encounters.
7. Difficulty modes subtract levels (Easy -4, Normal -2, Hard 0) and never
   touch the AI or the teams, so design for the Hard levels as written.
8. Every legendary in LEGENDARY AVAILABILITY and every Mega Stone in MEGA
   STONES must appear on at least one opponent somewhere in the campaign.
   Place them thoughtfully.

HARD RULES THE COMPILER ENFORCES (a violation rejects the whole file)
---------------------------------------------------------------------
- Use only species, items, Abilities, natures and moves from the lists in
  this brief. Write them as bare constant names (PIKACHU, LIGHT_BALL,
  LIGHTNING_ROD, JOLLY, VOLT_TACKLE).
- Every move must be legal for the species in the pinned Pokémon Showdown
  learnsets (Champions mod for the 232 Champions species, otherwise the
  species' most recent official mainline generation; evolved forms inherit
  the pre-evolution's legal moves). Assume Gen 9 rules: Scald, Return,
  Hidden Power, Pursuit, Toxic and Knock Off are NOT available on most species
  that do not learn them by level-up or egg move in Scarlet/Violet. Prefer
  level-up, TM and egg moves that exist in Scarlet/Violet, Sword/Shield or the
  species' last mainline appearance. Smeargle is exempt.
- 1 to 6 Pokémon per branch; doubles/multi branches need at least 2.
- Stat Points: six values HP/Atk/Def/SpA/SpD/Spe, each 0-32, total <= 66.
  Attack points require at least one physical move, Sp. Atk points require a
  special move, and points must not oppose a dominant category (do not give
  Attack points to a 3-special-move set or vice versa). Natures must not lower
  the only attacking category.
- Item Clause: no two Pokémon on one branch hold the same item. NONE allowed.
- Species Clause: no duplicate species on one branch.
- At most one Mega Stone per branch, and no Mega Stone at level cap 14 or 20.
- Assault Vest holders cannot have status moves. Choice item holders cannot
  have Protect/Detect or setup moves. No duplicate same-purpose status moves
  on one Pokémon (two sleep moves, Protect + Detect, two defense boosts...).
- Berry-dependent Abilities (Gluttony, Ripen, Cheek Pouch, Harvest) need a
  Berry; Poison Heal/Toxic Boost need a Toxic Orb; Flare Boost needs a Flame
  Orb; Guts/Quick Feet/Marvel Scale or Facade justify a Flame/Toxic Orb; Fire
  types cannot hold a Flame Orb; Unburden and Acrobatics need a consumable
  item (Sash, Berry, Gem, White Herb, Seed...) or no item.
- Field-dependent Abilities need a source on the same branch: Swift Swim /
  Rain Dish / Hydration need Drizzle or Rain Dance; Chlorophyll / Solar Power
  / Flower Gift need Drought or Sunny Day; Sand Rush / Sand Force need Sand
  Stream or Sandstorm; Slush Rush / Ice Body need Snow Warning or Snowscape;
  Surge Surfer needs Electric Terrain; Solar Beam needs sun or a Power Herb.
  Plus and Minus need a partner with Plus or Minus on the same branch.
  Commander needs a Dondozo on the branch.
- Move-dependent Abilities must be used: Technician needs a <=60 BP move,
  Iron Fist a punch, Strong Jaw a bite, Mega Launcher a pulse, Skill Link a
  multi-hit move, Sharpness a slicing move, Punk Rock a sound move, Reckless
  and Rock Head a recoil move (Brave Bird, Double-Edge, Head Smash, Flare
  Blitz, Wild Charge, Wood Hammer, Wave Crash...), Aerilate/Pixilate/
  Refrigerate/Galvanize a Normal move, Triage a healing move, Prankster a
  status move.
- Choice Band / Muscle Band need a physical move; Choice Specs / Wise Glasses
  a special move; type Gems a move of that type; Light Clay a screen; Power
  Herb a charge move; Throat Spray a sound move; Booster Energy needs
  Protosynthesis or Quark Drive; Eject Pack a self-lowering move; Terrain
  Seeds need the matching terrain setter on the branch.
- Do not combine Trick Room and Tailwind on one branch. Do not put a weather
  setter beside Cloud Nine or Air Lock on one branch. Attract is banned.
  Wishiwashi must be level 20 or higher. Explosion, Self-Destruct, Final
  Gambit, Memento and Healing Wish are allowed (the AI is told it may use them).
- Ally-only moves (Helping Hand, Coaching, Ally Switch, Heal Pulse, Decorate,
  Aromatic Mist) are not allowed on SINGLE-format branches. Partner-only
  Abilities (Friend Guard, Power Spot, Battery, Plus, Minus, Telepathy...) are
  not allowed on singles.
- Level: write an OFFSET from the encounter's level cap (e.g. -2, 0, +3),
  range -10..+10; cap + offset must stay within 1..100. At caps 45 and below,
  a Pokémon that evolves by level must be at or above its evolution level
  (see evo@ in the roster). Do not use stone/trade evolutions absurdly early
  (no Arcanine at level 12).
- Gym Leader / Elite Four / Champion / Maxie / Archie / Steven / Cynthia
  branches are class=leader or class=elite. Rival (Brendan/May/Wally) branches
  are class=rival. Magma/Aqua admins (Tabitha, Courtney, Matt, Shelly) are
  class=admin. Frontier Brain previews (Greta, Lucy, Spenser) are class=brain.
  Grunts are class=grunt. Trainers inside a Gym are class=gym. Notable route
  trainers (Cooltrainer, Expert, Pokémon Ranger, Dragon Tamer, Breeder,
  Winstrate) are class=ace. Deliberate breathers are class=casual (2-3
  Pokémon, levels -3 to -2), and between 20% and 40% of ordinary (non-boss)
  encounters must be casual. Everything else is class=regular. All branches of
  one encounter must share a class. Suggested level offsets by class:
  casual -3..-2 | regular -2..0 | grunt -1..+1 | gym -1..+1 | ace 0..+2 |
  brain +1..+3 | admin +1..+3 | rival +1..+3 (ace +3/+4) | leader +1..+5 |
  elite +2..+6 | postgame (cap 100) 0.

OUTPUT FORMAT (exact; this is the compiler's input format)
-----------------------------------------------------------
One block per branch, in the same order as the BATTLE LIST:

## E0002 TRAINER_CALVIN_1 class=regular
plan: one or two sentences describing what the team is trying to do.
crack: one or two sentences describing how a good player beats it.
POOCHYENA @FOCUS_SASH RATTLED JOLLY 2/32/0/0/0/32 -2 | CRUNCH, PLAY_ROUGH, SUCKER_PUNCH, HOWL
ZIGZAGOON @SITRUS_BERRY GLUTTONY ADAMANT 2/32/0/0/0/32 -1 | BELLY_DRUM, EXTREME_SPEED, SEED_BOMB, PROTECT
TAILLOW @TOXIC_ORB GUTS JOLLY 2/32/0/0/0/32 0 | FACADE, BRAVE_BIRD, QUICK_ATTACK, PROTECT

Line grammar: SPECIES @ITEM ABILITY NATURE HP/ATK/DEF/SPA/SPD/SPE OFFSET | MOVE, MOVE, MOVE, MOVE
The Stat Point column also accepts these shorthands: PS=2/32/0/0/0/32
SS=2/0/0/32/0/32 PB=32/32/2/0/0/0 SB=32/0/2/32/0/0 WD=32/0/32/0/2/0
WS=32/0/2/0/32/0 WM=32/0/16/0/18/0 FS=32/0/2/0/0/32 MX=2/32/0/32/0/0.
When an encounter has several branches (the rival's starter branches, the two
trainers of a multi battle), write plan/crack on the first branch only; later
branches inherit them. Keep the ## header exactly as given in the BATTLE LIST
(encounter number and TRAINER id), changing only the class if you disagree
with the draft class. Output every one of the 561 branches. Do not skip any.

AI PROFILES (chosen automatically from the class; for your information)
-------------------------------------------------------------------
sharp (everyone): full move evaluation, HP awareness, 2HKO preference, smart
switching, predicts switches, incoming Pokémon and moves, PP-stall prevention,
prefers field effects when they matter, knows the player's party species.
master (admins, rivals, brains, leaders, elite): everything above plus full
omniscience of the player's moves, items and Abilities.

CHAPTER LEVEL CAPS
------------------
Opening rival battle 14 | Stone Badge 14 | Knuckle Badge 20 | Dynamo Badge 30 |
Heat Badge 40 | Balance Badge 45 | Feather Badge 55 | Mind Badge 60 |
Rain Badge 70 | Pokémon League 80 | Postgame 100. Trick House puzzles use the
cap of the chapter they unlock in (listed per encounter). Badge N is earned by
beating Gym Leader N: Roxanne 1, Brawly 2, Wattson 3, Flannery 4, Norman 5,
Winona 6, Tate & Liza 7, Juan 8.
"""


def main() -> None:
    parts = [PROMPT.strip(), ""]
    parts.append("LEGENDARY AVAILABILITY (when the player can first obtain each legendary)")
    parts.append("-" * 72)
    parts.extend(legendary_table())
    parts.append("")
    parts.append("Non-Sign legendaries also in the game (story/static encounters, use only on bosses): "
                 "GROUDON, KYOGRE, RAYQUAZA, REGIROCK, REGICE, REGISTEEL, LATIAS, LATIOS, JIRACHI, DEOXYS forms, "
                 "MEW, MEWTWO, ARTICUNO, ZAPDOS, MOLTRES, HO_OH, LUGIA, HEATRAN, REGIGIGAS, MAGEARNA, DIANCIE, "
                 "URSHIFU forms, LUNALA, SOLGALEO, CALYREX_ICE, CALYREX_SHADOW, NECROZMA forms, HOOPA_UNBOUND, "
                 "KYUREM_BLACK, KYUREM_WHITE, GLASTRIER, SPECTRIER, ENAMORUS, LANDORUS_THERIAN, etc. "
                 "Coverage rule: the 76 Sign species above must each appear on some opponent (base form counts).")
    parts.append("")
    parts.append("MEGA STONES (item: species) — all 92 must appear on some opponent, at most one per branch, none before cap 30")
    parts.append("-" * 72)
    parts.extend(megas())
    parts.append("")
    parts.append("BATTLE LIST — every branch, with the current DRAFT team as a starting point (improve or replace freely)")
    parts.append("=" * 72)
    parts.extend(battle_list())
    parts.append("")
    parts.append("SPECIES ROSTER — NAME [types] ability1|ability2|hidden HP/Atk/Def/SpA/SpD/Spe evo@level")
    parts.append("=" * 72)
    parts.extend(species_roster())
    parts.append("")
    parts.append("HELD ITEMS AVAILABLE (bare constant names)")
    parts.append("=" * 72)
    items = sorted(audit.ITEMS)
    parts.append(", ".join(i[5:] for i in items))
    parts.append("")
    parts.append("NATURES: " + ", ".join(sorted(n[7:] for n in audit.NATURES)))
    parts.append("")
    parts.append("END OF BRIEF. Now produce all 561 branches in the OUTPUT FORMAT, in order, as one plain-text block.")
    OUT.write_text("\n".join(parts) + "\n")
    print(f"wrote {OUT} ({OUT.stat().st_size // 1024} KB)")


if __name__ == "__main__":
    main()
