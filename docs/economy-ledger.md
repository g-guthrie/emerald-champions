# Economy ledger

Rewritten September 17, 2026 for the Inclement conversion. Money is no longer a
near-dead currency: battle items are bought rather than handed over, treasure is
worth what Inclement paid for it, trainers pay their authored tier in full, and
milestones pay nothing at all.

## What changed in this pass

| | Before | After |
|---|---|---|
| Milestone stipends | 38,000 over 14 milestones | none; milestones move the cap only |
| Prize multiplier | ~40% of the authored tier | the authored tier, in full |
| Treasure (Nugget, Star Piece, ...) | `.price = 0`, unsellable | Inclement's prices, sellable at half |
| Battle items | free from the vendor | bought; 57 prices restored to Inclement's |
| EV training | impossible (`EV_CAP_NO_GAIN`) | earned in battle again (`EV_CAP_NONE`) |

Prize income across the 368 priced encounters goes from **72,318** to **180,795**.
Lifetime income goes from **116,318** (6,000 start + 38,000 stipends + prizes) to
**186,795** (6,000 start, no stipends, prizes in full) - and that is before any
treasure is sold, which is now real income rather than zero.

Against that, money now has to cover a 195-entry battle-item catalogue at roughly
4,000 a piece on top of the evolution catalogue, so the surplus that used to sit
unspent at the end of a run is gone.

## Model

Income sources counted here, all from source:

- Starting money 6,000 (`src/new_game.c`).
- No milestone stipends: every entry in `sCampaignMilestones` (`src/caps.c`) pays 0.
- First-clear campaign prize money, `campaignLevelCap * campaignPrizeMultiplier`
  (`GetCampaignBattleMoneyReward`, `src/battle_setup.c`). Return fights and rematches pay 0
  (`campaignRewardEligible`); defeat deducts nothing.

The 341 encounters of `docs/trainer-review-index.json` are ordered by E-group and the level cap is
modelled as a straight ramp from 14 to 96 across that order; each encounter pays at the modelled
cap. This is the same model the audit (`work/systems-audit/08-money-and-shops.md`) used, so the
"before" column below reproduces its figures to within rounding.

Not counted (all removed or zeroed by this pass and its siblings): field treasure resale, the Shoal
Big Pearl tail, Pay Day on rematches, the Amulet Coin, the PokeNews 50% shop discount.

## Prize multipliers

The authored per-trainer `Prize Multiplier` in `src/data/trainers.party` is the design *tier*.
`CampaignPrizeMultiplier` in `src/battle_setup.c` maps each tier to the money it pays per point of
the live cap:

| Tier | Encounters | Before | After |
|---|---|---|---|
| ordinary | 163 | 5 | 2 |
| ace, Gym member, rival | 149 | 10 | 4 |
| Gym Leader, team admin | 24 | 25 | 10 |
| Elite Four | 4 | 40 | 16 |
| Champion | 1 | 50 | 20 |

## Cumulative money by cap milestone

"Cleared" is the number of modelled encounters that fall at or below that cap. Cumulative money
assumes every encounter is fought once and nothing is spent.

| Cap | Stipend | Cleared | Cumulative before | Cumulative after |
|---|---|---|---|---|
| 20 | 3,000 | 25 | 11,280 | 9,912 |
| 24 | 2,000 | 17 | 17,000 | 13,400 |
| 30 | 3,000 | 25 | 24,575 | 18,230 |
| 36 | 2,000 | 25 | 33,160 | 22,864 |
| 42 | 3,000 | 25 | 44,045 | 29,018 |
| 48 | 3,000 | 24 | 56,915 | 35,966 |
| 54 | 2,000 | 25 | 69,285 | 42,114 |
| 60 | 3,000 | 25 | 85,125 | 50,250 |
| 68 | 3,000 | 33 | 104,250 | 59,700 |
| 76 | 3,000 | 34 | 131,320 | 72,328 |
| 84 | 3,000 | 33 | 159,460 | 85,384 |
| 90 | 3,000 | 25 | 185,485 | 97,594 |
| 96 | 0 | 25 | 224,525 | 113,210 |
| 100 (Champion) | 5,000 | 0 | 229,525 | 118,210 |

Lifetime income: **118,210** (44,000 stipends + 74,210 prizes), down from 229,525.

## The paid catalogue

`sPaidEvolutionItems` (`src/data/emerald_champions_paid_evolution_items.h`), 45 entries:

| Class | Count | Unit | Total |
|---|---|---|---|
| Evolution stones | 10 | 500 | 5,000 |
| Evolution tools | 32 | 1,000 | 32,000 |
| Reusable devices (Linking Cord, Scroll of Darkness, Scroll of Waters) | 3 | 3,000 | 9,000 |
| **Whole catalogue** | **45** | | **46,000** |

Sold in three tiers: Rustboro six common stones + Linking Cord (6,000 of shelf), Slateport ten
stones + the imported tools, Lilycove 4F everything.

## Money against catalogue

| Cap | Money after | Catalogue bought outright | Left over |
|---|---|---|---|
| 30 | 18,230 | 40% | — |
| 48 | 35,966 | 78% | — |
| 68 | 59,700 | 100% | 13,700 |
| 100 | 118,210 | 100% | 72,210 |

Before the retune the whole catalogue was covered at cap 42 and the run ended with a 4x surplus.
After it, the catalogue is not fully affordable until roughly cap 68, so caps 20 to 60 are spent
choosing which evolutions to buy first, and the surplus after that is real but finite.

## Sinks past the catalogue

- **Mauville Starter Archive**: 27 regional starter lines at 500 Coins = 10,000 money each
  (`MauvilleCity_GameCorner/scripts.inc:5-8`, `EventScript_PrepareStarterPrize`), one per species.
  270,000 to claim them all, so the Archive alone outlasts any realistic bankroll: the 72,210 left
  at the end of a full run buys seven of them.
- Balls: 100 / 300 / 600 ordinary, 600 specialist, 3,000 Luxury.
- Decorations and glass furniture (Slateport, Fortree, Route 104, Route 113, Lilycove 5F/rooftop).

Every one of these is optional expression or species delivery. There is no required purchase.

## First Rustboro purchase

The Rustboro clerk opens on first arrival with the six common stones (500 each) and the Linking
Cord (3,000): 6,000 for the whole shelf. Starting money is 6,000 on its own, before the first
stipend (Badge 1, 3,000) and before any prize money, so no Rustboro purchase can be blocked even
by a player who reaches the city having fought nothing. Modelled income at that point is higher
still.

## Treasure is not income

Nugget, Big Nugget, Tiny/Big/Balm Mushroom, Pearl, Big Pearl, Pearl String, Stardust, Star Piece,
Comet Shard, Heart Scale and Rare Bone all carry `.price = 0` in `src/data/items.h`, so
`GetItemSellPrice` returns 0 for every one of them. They remain in the item table for Fling and as
wild held items, and their descriptions no longer promise a payday.
