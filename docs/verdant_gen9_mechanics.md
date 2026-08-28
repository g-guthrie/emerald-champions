# Verdant curated Gen 9 mechanics

Verdant remains an Inclement Emerald engine with Mega Evolution as its only
battle gimmick. The selective Gen 9 backport adds the mechanics needed to keep
the 25 chosen endpoints recognizable without importing Terastallization.

## Ability behavior

- Armor Tail blocks opposing priority for both allies.
- Beads, Sword, and Vessel of Ruin apply their native field-wide stat
  reductions; all three are Mold Breaker-aware.
- Commander uses its canonical doubles behavior. Tatsugiri enters Dondozo,
  becomes invisible and untargetable, gives Dondozo +2 Attack, Defense, Sp. Atk,
  Sp. Def, and Speed, and prevents either partner from switching until Dondozo
  leaves. Order Up raises Attack for Curly, Defense for Droopy, or Speed for
  Stretchy, including after a swallowed Tatsugiri faints. The activation and
  Order Up animations use the three native Emerald Expansion Tatsugiri sprites.
- Cud Chew replays a Berry the holder actually ate on the following turn.
- Good as Gold blocks status moves used by other battlers, and trainer AI avoids
  wasting those moves into it.
- Protosynthesis and Quark Drive boost the holder's highest non-HP stat in sun
  or Electric Terrain. Booster Energy sustains the same boost while held rather
  than being consumed; it does nothing for Pokémon without one of those two
  abilities.
- Purifying Salt prevents major status and halves incoming Ghost damage.
- Supreme Overlord adds 10 percent damage per fainted ally, up to 50 percent.
- Toxic Debris lays up to two Toxic Spikes layers after physical damage.
- Zero to Hero changes Palafin's stored party form when it switches out. AI
  Palafin deliberately makes one safe switch, Hero stats materialize on re-entry,
  and the party form returns to Zero after battle.

Identity abilities cannot be copied or reassigned where the modern rules forbid
it, and the six modern breakable abilities participate in Mold Breaker logic.
All 13 abilities have explicit AI ratings.

## Signature moves

Twenty moves are appended without changing any existing move ID: Burning
Bulwark, Electro Shot, Flower Trick, Headlong Rush, Hydro Steam, Ivy Cudgel, Jet
Punch, Kowtow Cleave, Make It Rain, Mortal Spin, Order Up, Rage Fist, Raging
Fury, Ruination, Salt Cure, Snowscape, Thunderclap, Torch Song, Twin Beam, and
Wave Crash.

Their core identity behavior is implemented natively, including four mask types
for Ivy Cudgel, rain-skipped Electro Shot charging, one-stage Make It Rain recoil,
spread-target Mortal Spin poison and cleanup, persistent Rage Fist hit counts,
Water/Steel Salt Cure damage, and contact burn from Burning Bulwark. Snowscape
uses Verdant's existing hail field rather than importing a second snow-weather
subsystem.

`scripts/verify_verdant_gen9_battle_mechanics.py` is the focused regression gate
for these behaviors. It is also run by the full `scripts/verify_verdant.py` suite.
