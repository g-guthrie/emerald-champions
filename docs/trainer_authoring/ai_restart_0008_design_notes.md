# E0008 Cindy: current-source design review

Historical pre-change notes. Completed native repair evidence and final team:
`ai_restart_0008.md`. The baseline below is not the current party.

Read-only baseline review of `TRAINER_CINDY_1`; no encounter completion or native decision claim. The executable party is `src/data/trainers.h:12162`, attached to the Route104 double battle at `data/maps/Route104/scripts.inc:940`. Current Normal live levels are 12 at cap14. Stored trainer levels are not the live-level authority.

## Current party and native stats

Stats below are HP / Attack / Defense / Special Attack / Special Defense / Speed, calculated with native flat Stat Points, IV31, level12 and the compiled nature. Held-item and ability multipliers are stated separately.

| Pokémon | Native stats | Current role and constraint |
| --- | --- | --- |
| Snubbull | 72 / 64 / 22 / 16 / 18 / 15 | Adamant, Lum Berry, Intimidate; Play Rough / Fire Fang / Thunder Wave / Protect. Attack investment gives the lead a real Helping Hand recipient; Lum protects its physical output from one status event. |
| Milcery | 68 / 16 / 22 / 20 / 55 / 16 | Bold, Eviolite, Aroma Veil; Draining Kiss / Dazzling Gleam / Helping Hand / Recover. Eviolite gives approximately33 Defense/82 SpDef before other modifiers. Excellent special bulk, but only20 SpA makes both attacks and damage-derived healing weak. |
| Furfrou | 75 / 27 / 25 / 21 / 68 / 33 | Careful, Sitrus, Fur Coat; Double-Edge / U-turn / Sucker Punch / Snarl. Fur Coat doubles physical defense in damage calculation; it does not prevent special damage or make27 Attack into a wallbreaker. Special investment complements its ability. |
| Slakoth | 72 / 60 / 25 / 15 / 17 / 15 | Adamant, Choice Band, Truant; Body Slam / Hammer Arm / Night Slash / Rock Slide. Strong immediate physical output with explicit loafing and Choice-lock counterplay. |

All16 moves are present in the current generated preparation arrays: Milcery at1919, Furfrou17018, Snubbull40267 and Slakoth47052. All four assigned abilities are native species options. Furfrou is the natural form through the species alias/shared form definition. These are source checks, not proof of correct action selection.

## Individual design judgments and proposed experiments

- Retain Snubbull's Intimidate plus Milcery's Aroma Veil identity. They defend different mechanics: physical stat pressure versus move-binding effects. Aroma Veil is not blanket status immunity and cannot be credited for blocking damage or sleep.
- Helping Hand's priority lets Milcery support Snubbull despite both being slow. Thunder Wave does **not** automatically make these leads faster: a60–65 Speed foe remains30–32 after paralysis, above Snubbull15/Milcery16. Furfrou33 can exploit that crossing from reserve. Newly inflicted paralysis's12.5% action-denial chance remains a separate native benefit.
- Test Snubbull Fire Fang→Fire Punch. The current preparation array permits both; native Fire Punch is75 power/100 accuracy with10% burn, versus Fang65/95 plus10% burn and10% flinch. This exchanges an unreliable flinch that needs favorable order for reliable coverage. Do not change it solely because it appears stronger on paper; inspect an actual coverage board.
- If baseline confirms Milcery is too passive, compare the current spread with HP32/Def2/SpA16/SpD16, retaining Bold. At level12 this changes SpA20→36 and SpDef55→39 (Eviolite82→58), keeping HP68 and Speed16. It is a meaningful special-bulk cost, not a free improvement. No points were changed during this review.
- Furfrou's Snarl and U-turn need separate native evaluation: immediate special-damage reduction and later Intimidate restoration are not the same benefit. The latter requires a surviving Snubbull reserve and a legal subsequent entry.
- Slakoth repeats Allen exactly in species, item, ability, nature, points and all four moves; the stored offset differs but both currently reach level12. The other three Cindy species are their first campaign-trainer appearances. This is a real repetition concern, not an automatic reason to replace an otherwise useful reserve before seeing its decisions.
- Do not propose Milcery Entrainment as a Truant-removal combo. Although Milcery can learn it, native `ABILITY_TRUANT.cantBeOverwritten` is TRUE and `BS_TryEntrainment` explicitly rejects such targets. This would be an invalid design.

## Executable-AI boundaries to investigate

Current `ScoreFastPair` models Helping Hand, Thunder Wave, draining attacks and Sitrus, but skips Recover's own-HP restoration and Snarl's same-turn Special Attack drop. Their native single-move opinions still exist. These source omissions identify useful native probes; they do not establish that Cindy has already chosen a losing move.

Use actual opening-access opponents and actual generated Cindy records. Useful controls include physical pressure with Intimidate immunity, special pressure against Furfrou, an injured Milcery recovery window and a negative recovery window where it faints before acting. Keep opposing commands hidden from the AI. Follow-up tests and a design rating belong after parent-owned native results, not in this source-only note.
