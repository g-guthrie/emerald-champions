# E0009 Lyle — current-code design reconnaissance

Source-only review while parent owns native lead execution and another agent owns reserves. This is not encounter completion, a tactical rating, or proof that any particular AI decision is wrong. No production or canonical records were edited.

## Verified input and native context

`src/data/trainers.h:53826` is the compiled Normal `TRAINER_LYLE` four-member doubles party. Petalburg Woods invokes it at `data/maps/PetalburgWoods/scripts.inc:275`. Current canonical block starts at `data/emerald_champions/emerald_champions_battle_teams.txt:132`. All four move lists, items, abilities, natures and points agree. The Normal cap14 level floor gives live12; stored levels12/11/13/13 alone do not establish live levels.

Native flat Stat Points/IV31/nature calculation, before item/ability multipliers:

| Member | HP / Atk / Def / SpA / SpD / Speed | Item, ability, role |
| --- | --- | --- |
| Nymble | 35 / 51 / 18 / 11 / 14 / 56 | Sash, Tinted Lens, Jolly: First Impression / Leech Life / Sucker Punch / Protect |
| Sewaddle | 68 / 58 / 27 / 16 / 23 / 18 | Eviolite, Overcoat, Adamant: Bug Bite / Seed Bomb / Struggle Bug / Protect |
| Anorith | 38 / 63 / 20 / 16 / 20 / 63 | Life Orb, Battle Armor, Jolly: Rock Blast / X-Scissor / Aqua Jet / Protect |
| Dottler | 69 / 15 / 29 / 57 / 30 / 15 | Leftovers, Swarm, Modest: Psychic / Struggle Bug / Reflect / Helping Hand |

All16 moves were checked in the actual generated preparation arrays: Sewaddle7293, Dottler20865, Nymble36292, Anorith72879. Configured species definitions admit all four abilities. These are legal current preparation choices, not claims about vanilla level-up access. All four species are their first campaign-trainer appearance; later Anorith repeats on another team, but this encounter introduces it. Nymble is also an opening Route102 land resident; Sewaddle appears in the first Woods grass table. The theme repeats Rick's insect battle, not Rick's species or exact roles.

## Individual review and bounded candidates

### Nymble

Keep Sash and the existing Attack/Speed points: native35HP and low defenses need protection against one strong hit, while56Speed supports ordinary attacks. Tinted Lens increases damage into resistance; it does not turn a quarter-resisted move into neutral damage or defeat immunity. Current Champions First Impression is **100** power,100 accuracy,+2 priority (`moves_info.h:16639`), not the older90-power version. Fake Out+3 still acts first. Native first-turn eligibility is checked at `battle_move_resolution.c:1245`; a real switch entry resets `isFirstTurn` at `battle_main.c:2941`.

The most distinctive candidate is **Sucker Punch→U-turn**, retaining First Impression, Leech Life and Protect. U-turn is explicitly in Nymble's current preparation array. It provides a legal damaging exit followed by a later fresh-entry First Impression, while sacrificing conditional Dark-priority coverage and an immediate finishing option. Native Tinted Lens Bug damage already covers some resisted situations. This is not a free pivot: contact punishment, Protect, lack of healthy reserves, switch-in hazards and the incoming reserve's matchup matter. The current short paired forecast does not prove it plans a full exit/re-entry cycle. Compare an actual first-turn attack, second-turn pivot opportunity and later native re-entry; do not force U-turn merely because the slot exists.

### Sewaddle

Its58 Attack versus16 SpA makes Seed Bomb/Bug Bite meaningful and Struggle Bug primarily support. Eviolite adds physical and special bulk; Overcoat blocks powder/weather damage, not its4× Fire/Flying weaknesses. Speed18 normally cannot protect Nymble from a fast special attack on the same turn by lowering SpA first. A slow or already-slowed opposing special attacker, or future-turn survival, is required for that benefit.

Do not remove Struggle Bug just because raw damage is low. It lowers both foes' SpA and cannot be Taunted. If native baseline shows Sewaddle's support is consistently too late, a narrowly testable alternative is **Struggle Bug→Skitter Smack**, which is native legal and attacks physically while lowering one foe's SpA; it loses spread coverage and accuracy. Lunge is also legal but lowers Attack, changing rather than preserving this team's special-defense support. Moving16 HP points toSpeed gives native34Speed and52HP instead of18/68, a substantial survival trade, not an automatic optimization. Choose at most one candidate after observing baseline; do not combine several changes to hide the cause.

### Anorith

Retain pending baseline: its63Speed and63 Attack, Life Orb, priority Aqua Jet and strong Rock coverage add an immediate response to Flying foes. Battle Armor avoids critical damage; it does not make a38HP Life Orb attacker a wall. Rock Blast's native variable strike count can break Sash but is not a guaranteed maximum-hit KO. Rock Slide would add spread pressure and flinch chance but lose the distinct multi-hit role and repeat earlier Dwebble coverage. No source-only reason to make that substitution now.

### Dottler

Its57 SpA gives Psychic a genuine special-attacking role that the other three lack. Helping Hand acts at priority+5, so its15Speed does not prevent helping Anorith. Reflect is different: ordinary priority means a fast physical attacker can hit before it; the screen may still benefit a slower second attacker or subsequent turns. Leftovers restores only floor69/16=4HP per normal end turn, not Eviolite-like bulk.

The first candidate, if actual pressure proves the screen slot ineffective, is **Reflect→Protect**. Protect can preserve Dottler while its partner removes a threat and works with Leftovers, but loses team-wide mitigation and is not reliable when repeated. Keep Psychic and Helping Hand. Native-legal Recover is another possible longevity move, but it is also slow and should not be added solely because the shared Recover forecast was just repaired. Do not add Trick Room casually: it would fight fast Nymble/Anorith and require a different team design.

## Shared AI boundaries relevant to native probes

- The repaired Snarl cache currently checks `MOVE_SNARL` specifically; it does not yet give Sewaddle/Dottler Struggle Bug's native same-turn SpA-drop continuation. Reuse would need non-sound eligibility rather than copying Soundproof checks.
- Reflect's new physical mitigation is not currently represented in the local paired HP continuation. A bad actual decision needs a native counterfactual; source omission alone does not mean every Reflect choice is wrong.
- Native U-turn entry and First Impression eligibility exist, but that does not establish a bounded scorer's multi-turn pivot planning quality.
- Existing single-move legality must continue rejecting First Impression on later turns and Sucker Punch into status actions. Retain priority, Sash/multi-hit and contact-damage controls without forcing a preferred attack.

Source review suggests preserving all four species. Highest-value team experiments are one Nymble U-turn comparison and, only if baseline warrants it, one Dottler protection or Sewaddle support comparison. No difficulty spike, whole-team rewrite or speculative new combo is justified yet.
