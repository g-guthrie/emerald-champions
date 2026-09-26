// Full Ability descriptions for the Pokédex Abilities page, read through
// GetAbilityFullDescription (src/ability_text.c). Each entry describes what
// the Ability does in this game's code, Champions rules and custom Abilities
// included, and is pre-wrapped with \n into at most six lines of at most
// 134 px in FONT_NARROW; test/ability_text.c measures every entry with the
// real font tables. Abilities without an entry use gAbilitiesInfo[].description.

    [ABILITY_NONE] = COMPOUND_STRING(
        "This Pokémon has no Ability."),

    [ABILITY_STENCH] = COMPOUND_STRING(
        "Damaging moves without a\n"
        "flinch effect have a 1 in 10\n"
        "chance to make the target\n"
        "flinch. In the lead: halves\n"
        "wild encounters."),

    [ABILITY_DRIZZLE] = COMPOUND_STRING(
        "On entering battle, starts\n"
        "rain for five turns, or eight\n"
        "with a Damp Rock."),

    [ABILITY_SPEED_BOOST] = COMPOUND_STRING(
        "Raises its Speed by one stage\n"
        "at each turn's end. It does\n"
        "not activate on the turn it\n"
        "switches in after battle has\n"
        "started."),

    [ABILITY_BATTLE_ARMOR] = COMPOUND_STRING(
        "Prevents attacks from\n"
        "landing critical hits on it,\n"
        "including moves that normally\n"
        "always land one."),

    [ABILITY_STURDY] = COMPOUND_STRING(
        "At full HP, it survives a\n"
        "knockout hit with one HP. It\n"
        "also blocks one-hit KO moves.\n"
        "Mold Breaker can bypass it."),

    [ABILITY_DAMP] = COMPOUND_STRING(
        "Prevents explosive moves\n"
        "such as Explosion,\n"
        "Self-Destruct and Misty\n"
        "Explosion, and blocks\n"
        "Aftermath damage."),

    [ABILITY_LIMBER] = COMPOUND_STRING(
        "Prevents paralysis. Gaining\n"
        "this Ability also cures\n"
        "paralysis already affecting\n"
        "it."),

    [ABILITY_SAND_VEIL] = COMPOUND_STRING(
        "In sandstorms, cuts incoming\n"
        "moves' accuracy by a fifth\n"
        "and takes no sand damage. In\n"
        "the lead: halves encounters\n"
        "during map sandstorms."),

    [ABILITY_STATIC] = COMPOUND_STRING(
        "Contact hits have a 3 in 10\n"
        "chance to paralyze the\n"
        "attacker. In the lead: draws\n"
        "more Electric-type wild\n"
        "Pokémon."),

    [ABILITY_VOLT_ABSORB] = COMPOUND_STRING(
        "Electric-type moves have no\n"
        "effect on it and restore a\n"
        "quarter of its max HP\n"
        "instead. Still blocks them at\n"
        "full HP."),

    [ABILITY_WATER_ABSORB] = COMPOUND_STRING(
        "Water-type moves have no\n"
        "effect on it and restore a\n"
        "quarter of its max HP\n"
        "instead. Still blocks them at\n"
        "full HP."),

    [ABILITY_OBLIVIOUS] = COMPOUND_STRING(
        "Prevents infatuation and\n"
        "Taunt, and blocks\n"
        "Intimidate's Attack drop.\n"
        "Gaining it also removes\n"
        "existing infatuation or\n"
        "Taunt."),

    [ABILITY_CLOUD_NINE] = COMPOUND_STRING(
        "While it is in battle, weather\n"
        "has no effect. The weather\n"
        "itself stays in place."),

    [ABILITY_COMPOUND_EYES] = COMPOUND_STRING(
        "Raises its moves' accuracy by\n"
        "three tenths."),

    [ABILITY_INSOMNIA] = COMPOUND_STRING(
        "Prevents sleep, including\n"
        "Rest. Gaining this Ability\n"
        "wakes it if it is already\n"
        "asleep."),

    [ABILITY_COLOR_CHANGE] = COMPOUND_STRING(
        "After a damaging hit, it\n"
        "changes to the move's type.\n"
        "Hits on a substitute and\n"
        "Struggle do not trigger it."),

    [ABILITY_IMMUNITY] = COMPOUND_STRING(
        "Prevents regular and bad\n"
        "poison. Gaining this Ability\n"
        "also cures poison already\n"
        "affecting it."),

    [ABILITY_FLASH_FIRE] = COMPOUND_STRING(
        "Fire moves cannot hurt it.\n"
        "Absorbing one gives its Fire\n"
        "attacks half more power until\n"
        "it leaves battle. Repeated\n"
        "Fire hits do not stack the\n"
        "boost."),

    [ABILITY_SHIELD_DUST] = COMPOUND_STRING(
        "Blocks added effects of\n"
        "damaging moves that hit it,\n"
        "such as burns or flinching. It\n"
        "does not block status moves\n"
        "or the attacker's own boosts."),

    [ABILITY_OWN_TEMPO] = COMPOUND_STRING(
        "Prevents confusion and\n"
        "blocks Intimidate's Attack\n"
        "drop. Gaining this Ability\n"
        "also cures confusion already\n"
        "affecting it."),

    [ABILITY_SUCTION_CUPS] = COMPOUND_STRING(
        "Prevents moves from forcing\n"
        "it to switch or flee. It can\n"
        "still switch by choice. In the\n"
        "lead: improves fishing bites."),

    [ABILITY_INTIMIDATE] = COMPOUND_STRING(
        "On entry, lowers each foe's\n"
        "Attack by one stage.\n"
        "Abilities that block stat\n"
        "drops or Intimidate can\n"
        "prevent this."),

    [ABILITY_SHADOW_TAG] = COMPOUND_STRING(
        "Prevents foes from switching\n"
        "or fleeing. Ghost Pokémon and\n"
        "other Shadow Tag users can\n"
        "escape. Shed Shell and\n"
        "switching moves can bypass\n"
        "it."),

    [ABILITY_ROUGH_SKIN] = COMPOUND_STRING(
        "A contact hit costs the\n"
        "attacker an eighth of its max\n"
        "HP. Magic Guard and effects\n"
        "that avoid contact stop this\n"
        "damage."),

    [ABILITY_WONDER_GUARD] = COMPOUND_STRING(
        "Blocks damaging moves unless\n"
        "they are super effective.\n"
        "Status, weather and other\n"
        "indirect damage still work.\n"
        "Mold Breaker can bypass it."),

    [ABILITY_LEVITATE] = COMPOUND_STRING(
        "Floats above Ground attacks,\n"
        "Spikes, Toxic Spikes, Sticky\n"
        "Web and terrain. Gravity or\n"
        "effects that ground it\n"
        "remove this protection."),

    [ABILITY_EFFECT_SPORE] = COMPOUND_STRING(
        "Contact hits may poison,\n"
        "paralyze or put the attacker\n"
        "to sleep: a combined 3 in 10\n"
        "chance. Grass-types,\n"
        "Overcoat and Safety Goggles\n"
        "block this."),

    [ABILITY_SYNCHRONIZE] = COMPOUND_STRING(
        "When another Pokémon burns,\n"
        "paralyzes or poisons it,\n"
        "gives the same status back if\n"
        "possible. Bad poison is\n"
        "reflected as bad poison."),

    [ABILITY_CLEAR_BODY] = COMPOUND_STRING(
        "Other Pokémon cannot lower\n"
        "its stat stages. Its own\n"
        "moves can still lower them."),

    [ABILITY_NATURAL_CURE] = COMPOUND_STRING(
        "Cures its major status\n"
        "condition when it leaves\n"
        "battle. It does not heal HP."),

    [ABILITY_LIGHTNING_ROD] = COMPOUND_STRING(
        "Draws single-target Electric\n"
        "moves to itself. Electric\n"
        "moves do no damage and raise\n"
        "its Sp. Atk by one stage\n"
        "instead."),

    [ABILITY_SERENE_GRACE] = COMPOUND_STRING(
        "Doubles the chance of its\n"
        "moves' added effects, such as\n"
        "burns or flinching. It does\n"
        "not raise its critical-hit\n"
        "chance."),

    [ABILITY_SWIFT_SWIM] = COMPOUND_STRING(
        "Doubles its Speed in rain. Its\n"
        "Utility Umbrella prevents\n"
        "this boost, as does an Ability\n"
        "that suppresses weather."),

    [ABILITY_CHLOROPHYLL] = COMPOUND_STRING(
        "Doubles its Speed in\n"
        "sunlight. A held Utility\n"
        "Umbrella prevents this boost."),

    [ABILITY_ILLUMINATE] = COMPOUND_STRING(
        "Prevents other Pokémon from\n"
        "lowering its accuracy and\n"
        "ignores the target's raised\n"
        "evasion. Leading the party:\n"
        "wild encounters become more\n"
        "frequent."),

    [ABILITY_TRACE] = COMPOUND_STRING(
        "Copies an eligible foe's\n"
        "Ability on entering battle.\n"
        "Picks at random if both\n"
        "qualify. Its own Ability\n"
        "Shield prevents copying."),

    [ABILITY_HUGE_POWER] = COMPOUND_STRING(
        "Doubles its Attack for\n"
        "physical attacks. Its Sp. Atk\n"
        "is unchanged."),

    [ABILITY_POISON_POINT] = COMPOUND_STRING(
        "Contact hits have a 3 in 10\n"
        "chance to poison the\n"
        "attacker. Pokémon immune to\n"
        "poison are unaffected."),

    [ABILITY_INNER_FOCUS] = COMPOUND_STRING(
        "Prevents flinching and\n"
        "blocks Intimidate's Attack\n"
        "drop. It does not prevent\n"
        "other effects that stop a\n"
        "turn."),

    [ABILITY_MAGMA_ARMOR] = COMPOUND_STRING(
        "Prevents freezing and thaws\n"
        "it if already frozen. In the\n"
        "party, eggs hatch twice as\n"
        "fast."),

    [ABILITY_WATER_VEIL] = COMPOUND_STRING(
        "Prevents burns and cures a\n"
        "burn it already has. It does\n"
        "not reduce Fire damage."),

    [ABILITY_MAGNET_PULL] = COMPOUND_STRING(
        "Prevents opposing\n"
        "Steel-types from switching\n"
        "or fleeing normally.\n"
        "Ghost-types, Shed Shell and\n"
        "switching moves can escape."),

    [ABILITY_SOUNDPROOF] = COMPOUND_STRING(
        "Sound moves do not affect it,\n"
        "including an ally's sound\n"
        "moves. It does not stop it\n"
        "from using its own sound\n"
        "moves."),

    [ABILITY_RAIN_DISH] = COMPOUND_STRING(
        "At each turn's end in rain,\n"
        "restores a sixteenth of its\n"
        "max HP. Heal Block and Utility\n"
        "Umbrella prevent this\n"
        "healing."),

    [ABILITY_SAND_STREAM] = COMPOUND_STRING(
        "On entry, starts a sandstorm\n"
        "for five turns, or eight with\n"
        "Smooth Rock."),

    [ABILITY_PRESSURE] = COMPOUND_STRING(
        "Moves that target it cost an\n"
        "extra PP. In doubles, hitting\n"
        "two Pressure holders costs\n"
        "two extra PP."),

    [ABILITY_THICK_FAT] = COMPOUND_STRING(
        "Halves damage from Fire-type\n"
        "and Ice-type moves. It does\n"
        "not prevent burns or\n"
        "freezing."),

    [ABILITY_EARLY_BIRD] = COMPOUND_STRING(
        "Its sleep counter falls twice\n"
        "as fast, so it wakes sooner.\n"
        "Also works with Rest."),

    [ABILITY_FLAME_BODY] = COMPOUND_STRING(
        "Contact hits have a 3 in 10\n"
        "chance to burn the attacker.\n"
        "In the party, eggs hatch\n"
        "twice as fast."),

    [ABILITY_RUN_AWAY] = COMPOUND_STRING(
        "Lets it flee ordinary wild\n"
        "battles, even when trapped.\n"
        "It does not let it switch out\n"
        "of trapping effects or flee\n"
        "trainer battles."),

    [ABILITY_KEEN_EYE] = COMPOUND_STRING(
        "Other Pokémon cannot lower\n"
        "its accuracy. Its moves also\n"
        "ignore the target's raised\n"
        "evasion, but can still miss\n"
        "normally."),

    [ABILITY_HYPER_CUTTER] = COMPOUND_STRING(
        "Other Pokémon cannot lower\n"
        "its Attack. Its own Attack\n"
        "drops still work, and other\n"
        "stats are not protected."),

    [ABILITY_PICKUP] = COMPOUND_STRING(
        "If empty-handed, may collect\n"
        "another battler's used item\n"
        "at turn's end. After battle,\n"
        "has a 1 in 10 chance to find a\n"
        "level-dependent item."),

    [ABILITY_TRUANT] = COMPOUND_STRING(
        "It can use a move only every\n"
        "other turn. It loafs around\n"
        "on the turns between, but can\n"
        "still switch out."),

    [ABILITY_HUSTLE] = COMPOUND_STRING(
        "Gives its physical attacks\n"
        "half more power, but cuts\n"
        "their accuracy by a fifth.\n"
        "Special and status moves\n"
        "keep their normal accuracy."),

    [ABILITY_CUTE_CHARM] = COMPOUND_STRING(
        "A contact hit has a 3 in 10\n"
        "chance to infatuate an\n"
        "attacker of the opposite\n"
        "gender. It must survive the\n"
        "hit."),

    [ABILITY_PLUS] = COMPOUND_STRING(
        "While its ally has Plus or\n"
        "Minus, its Sp. Atk is raised\n"
        "by half. The ally must be in\n"
        "battle for the boost."),

    [ABILITY_MINUS] = COMPOUND_STRING(
        "While its ally has Plus or\n"
        "Minus, its Sp. Atk is raised\n"
        "by half. The ally must be in\n"
        "battle for the boost."),

    [ABILITY_FORECAST] = COMPOUND_STRING(
        "Castform becomes Fire-type\n"
        "in sun, Water-type in rain\n"
        "and Ice-type in hail or snow.\n"
        "Without active weather, it\n"
        "returns to its Normal form."),

    [ABILITY_STICKY_HOLD] = COMPOUND_STRING(
        "Stops others from stealing,\n"
        "swapping or destroying its\n"
        "held item. Mold Breaker can\n"
        "bypass it. In the lead:\n"
        "improves fishing bites."),

    [ABILITY_SHED_SKIN] = COMPOUND_STRING(
        "At each turn's end, has a 1 in\n"
        "3 chance to cure its sleep,\n"
        "poison, paralysis, burn or\n"
        "freeze. It does not cure\n"
        "confusion."),

    [ABILITY_GUTS] = COMPOUND_STRING(
        "With a major status\n"
        "condition, its physical\n"
        "attacks gain half more power.\n"
        "Burn does not reduce their\n"
        "damage, but still hurts each\n"
        "turn."),

    [ABILITY_MARVEL_SCALE] = COMPOUND_STRING(
        "While it has a major status\n"
        "condition, its Defense is\n"
        "raised by half. Its Sp. Def is\n"
        "unchanged."),

    [ABILITY_LIQUID_OOZE] = COMPOUND_STRING(
        "A Pokémon draining its HP\n"
        "loses the HP it would have\n"
        "recovered instead. This\n"
        "includes draining attacks\n"
        "and Leech Seed."),

    [ABILITY_OVERGROW] = COMPOUND_STRING(
        "At a third of its max HP or\n"
        "less, its Grass attacks gain\n"
        "half more power. Healing\n"
        "above that threshold removes\n"
        "the boost."),

    [ABILITY_BLAZE] = COMPOUND_STRING(
        "At a third of its max HP or\n"
        "less, its Fire attacks deal\n"
        "half again as much damage."),

    [ABILITY_TORRENT] = COMPOUND_STRING(
        "At a third of its max HP or\n"
        "less, boosts the damage of\n"
        "its Water-type moves by half."),

    [ABILITY_SWARM] = COMPOUND_STRING(
        "At a third of its max HP or\n"
        "less, boosts the damage of\n"
        "its Bug-type moves by half."),

    [ABILITY_ROCK_HEAD] = COMPOUND_STRING(
        "Prevents recoil from its\n"
        "moves, including Chloroblast.\n"
        "It does not prevent crash\n"
        "damage, Struggle recoil or\n"
        "Life Orb damage."),

    [ABILITY_DROUGHT] = COMPOUND_STRING(
        "On entering battle, starts\n"
        "sunlight for five turns, or\n"
        "eight with a Heat Rock."),

    [ABILITY_ARENA_TRAP] = COMPOUND_STRING(
        "Prevents grounded foes from\n"
        "fleeing or switching\n"
        "normally. Ghost types can\n"
        "still leave."),

    [ABILITY_VITAL_SPIRIT] = COMPOUND_STRING(
        "Prevents sleep, including\n"
        "Yawn and Rest. In the lead:\n"
        "makes higher-level wild\n"
        "Pokémon more likely."),

    [ABILITY_WHITE_SMOKE] = COMPOUND_STRING(
        "Prevents other Pokémon from\n"
        "lowering its stats. Its own\n"
        "moves can still lower them. In\n"
        "the lead: halves wild\n"
        "encounters."),

    [ABILITY_PURE_POWER] = COMPOUND_STRING(
        "Doubles its Attack when\n"
        "dealing physical move damage."),

    [ABILITY_SHELL_ARMOR] = COMPOUND_STRING(
        "Prevents critical hits\n"
        "against it. Mold Breaker can\n"
        "bypass this protection."),

    [ABILITY_AIR_LOCK] = COMPOUND_STRING(
        "While it is in battle, weather\n"
        "has no effect. The weather\n"
        "itself stays in place."),

    [ABILITY_TANGLED_FEET] = COMPOUND_STRING(
        "While it is confused, halves\n"
        "the accuracy of moves aimed\n"
        "at it. Moves that always hit\n"
        "still do so."),

    [ABILITY_MOTOR_DRIVE] = COMPOUND_STRING(
        "Electric moves cannot hurt\n"
        "it. Instead, they raise its\n"
        "Speed by one stage. It does\n"
        "not redirect moves aimed at\n"
        "its ally."),

    [ABILITY_RIVALRY] = COMPOUND_STRING(
        "Boosts move power by a\n"
        "quarter against the same\n"
        "gender, but cuts it by a\n"
        "quarter against the opposite\n"
        "gender. Genderless targets\n"
        "are unaffected."),

    [ABILITY_STEADFAST] = COMPOUND_STRING(
        "Flinching raises its Speed by\n"
        "one stage. It still loses that\n"
        "turn's move."),

    [ABILITY_SNOW_CLOAK] = COMPOUND_STRING(
        "In hail or snow, cuts incoming\n"
        "moves' accuracy by a fifth. It\n"
        "also takes no hail damage."),

    [ABILITY_GLUTTONY] = COMPOUND_STRING(
        "It eats low-HP Berries\n"
        "earlier: at half its max HP\n"
        "plus one, rather than a\n"
        "quarter. It never eats an\n"
        "HP-triggered Berry at full\n"
        "HP."),

    [ABILITY_ANGER_POINT] = COMPOUND_STRING(
        "Surviving a critical hit\n"
        "raises its Attack to the max.\n"
        "A hit on its substitute does\n"
        "not trigger it."),

    [ABILITY_UNBURDEN] = COMPOUND_STRING(
        "Doubles its Speed after its\n"
        "held item is used up or lost.\n"
        "The boost ends if it gets\n"
        "another item or switches out."),

    [ABILITY_HEATPROOF] = COMPOUND_STRING(
        "Fire attacks deal half damage\n"
        "to it, and burn hurts half as\n"
        "much each turn. It can still\n"
        "be burned."),

    [ABILITY_SIMPLE] = COMPOUND_STRING(
        "Doubles the number of stages\n"
        "of its stat rises and drops,\n"
        "including accuracy and\n"
        "evasion. The usual stage\n"
        "limits still apply."),

    [ABILITY_DRY_SKIN] = COMPOUND_STRING(
        "Water hits heal a quarter HP.\n"
        "Fire hits deal a quarter\n"
        "more. Each turn, rain heals an\n"
        "eighth HP and sun removes an\n"
        "eighth."),

    [ABILITY_DOWNLOAD] = COMPOUND_STRING(
        "On entry, raises Attack one\n"
        "stage if foes' total Defense\n"
        "is below their Sp. Def.\n"
        "Otherwise raises Sp. Atk one\n"
        "stage."),

    [ABILITY_IRON_FIST] = COMPOUND_STRING(
        "Its punching moves, such as\n"
        "Mach Punch, Drain Punch and\n"
        "Ice Punch, gain a fifth more\n"
        "power."),

    [ABILITY_POISON_HEAL] = COMPOUND_STRING(
        "When poisoned, restores an\n"
        "eighth of its max HP each\n"
        "turn instead of taking poison\n"
        "damage. It stays poisoned."),

    [ABILITY_ADAPTABILITY] = COMPOUND_STRING(
        "Its same-type attack bonus\n"
        "doubles damage instead of\n"
        "raising it by half."),

    [ABILITY_SKILL_LINK] = COMPOUND_STRING(
        "Its two-to-five-hit moves\n"
        "hit five times. Multi-hit\n"
        "moves, including Population\n"
        "Bomb and Triple Axel, only\n"
        "check accuracy for the first\n"
        "hit."),

    [ABILITY_HYDRATION] = COMPOUND_STRING(
        "At the end of each turn in\n"
        "rain, cures its major status\n"
        "condition. Cloud Nine and Air\n"
        "Lock prevent the weather\n"
        "effect."),

    [ABILITY_SOLAR_POWER] = COMPOUND_STRING(
        "In sunlight, boosts its Sp.\n"
        "Atk by half but costs it an\n"
        "eighth of its max HP each\n"
        "turn. Utility Umbrella blocks\n"
        "both effects."),

    [ABILITY_QUICK_FEET] = COMPOUND_STRING(
        "While it has a status problem,\n"
        "boosts Speed by half and\n"
        "ignores paralysis's Speed\n"
        "cut. In the lead: halves wild\n"
        "encounter frequency."),

    [ABILITY_NORMALIZE] = COMPOUND_STRING(
        "Its moves become Normal-type\n"
        "and gain a fifth more power.\n"
        "Some moves that determine\n"
        "their own type are exempt."),

    [ABILITY_SNIPER] = COMPOUND_STRING(
        "Boosts damage from its\n"
        "critical hits by half, on top\n"
        "of the normal critical-hit\n"
        "bonus. It does not make\n"
        "critical hits more likely."),

    [ABILITY_MAGIC_GUARD] = COMPOUND_STRING(
        "Prevents poison, burn,\n"
        "weather, hazard and most\n"
        "recoil damage. Direct\n"
        "attacks, confusion and\n"
        "Struggle recoil still hurt it.\n"
        "Status can still affect it."),

    [ABILITY_NO_GUARD] = COMPOUND_STRING(
        "Its attacks and attacks\n"
        "aimed at it bypass accuracy\n"
        "checks, even during moves\n"
        "such as Fly or Dig. Type\n"
        "immunities and Protect still\n"
        "work."),

    [ABILITY_STALL] = COMPOUND_STRING(
        "Moves after other Pokémon\n"
        "using moves of the same\n"
        "priority, even in Trick Room.\n"
        "It does not lower the\n"
        "priority of its moves."),

    [ABILITY_TECHNICIAN] = COMPOUND_STRING(
        "Boosts moves with base power\n"
        "of 60 or less by half. Checks\n"
        "power after the move's own\n"
        "power-changing effects."),

    [ABILITY_LEAF_GUARD] = COMPOUND_STRING(
        "In sun, prevents major status\n"
        "conditions, including sleep\n"
        "from Rest. It does not cure a\n"
        "status condition it already\n"
        "has."),

    [ABILITY_KLUTZ] = COMPOUND_STRING(
        "Most held items have no\n"
        "effect while it is in battle.\n"
        "It still holds the item and\n"
        "can lose it. It cannot use\n"
        "Fling."),

    [ABILITY_MOLD_BREAKER] = COMPOUND_STRING(
        "Its moves ignore many\n"
        "defensive Abilities, such as\n"
        "Levitate and Sturdy. It does\n"
        "not ignore every Ability, and\n"
        "Ability Shield blocks this\n"
        "effect."),

    [ABILITY_SUPER_LUCK] = COMPOUND_STRING(
        "Raises its critical-hit ratio\n"
        "by one stage. In the lead:\n"
        "wild Pokémon are more likely\n"
        "to hold items, including rare\n"
        "ones."),

    [ABILITY_AFTERMATH] = COMPOUND_STRING(
        "If a contact hit knocks it\n"
        "out, the attacker loses a\n"
        "quarter of its max HP. Damp\n"
        "prevents this."),

    [ABILITY_ANTICIPATION] = COMPOUND_STRING(
        "On entering battle, it\n"
        "shudders if a foe has a\n"
        "super-effective move or a\n"
        "one-hit knockout move."),

    [ABILITY_FOREWARN] = COMPOUND_STRING(
        "On entry, reveals one of the\n"
        "foes' strongest moves.\n"
        "Fixed-damage and one-hit\n"
        "knockout moves use special\n"
        "power ratings for this\n"
        "choice."),

    [ABILITY_UNAWARE] = COMPOUND_STRING(
        "Ignores the other Pokémon's\n"
        "Attack, Defense, Sp. Atk, Sp.\n"
        "Def, accuracy and evasion\n"
        "changes when attacking or\n"
        "taking hits. Speed still\n"
        "counts."),

    [ABILITY_TINTED_LENS] = COMPOUND_STRING(
        "Doubles the damage of its\n"
        "not-very-effective moves. It\n"
        "does not let moves hit a\n"
        "target that is immune."),

    [ABILITY_FILTER] = COMPOUND_STRING(
        "Super-effective attacks deal\n"
        "a quarter less damage to it.\n"
        "Neutral and resisted attacks\n"
        "are unchanged. Mold Breaker\n"
        "ignores this."),

    [ABILITY_SLOW_START] = COMPOUND_STRING(
        "Halves its Attack and Speed\n"
        "for five turns after it\n"
        "enters battle. Switching out\n"
        "resets the count."),

    [ABILITY_SCRAPPY] = COMPOUND_STRING(
        "Its Normal and Fighting moves\n"
        "can hit Ghost Pokémon.\n"
        "Intimidate cannot lower its\n"
        "Attack."),

    [ABILITY_STORM_DRAIN] = COMPOUND_STRING(
        "Draws single-target Water\n"
        "moves to itself. Water moves\n"
        "have no effect and raise its\n"
        "Sp. Atk by one stage instead."),

    [ABILITY_ICE_BODY] = COMPOUND_STRING(
        "At each turn's end in hail or\n"
        "snow, restores a sixteenth of\n"
        "its max HP. It also takes no\n"
        "hail damage."),

    [ABILITY_SOLID_ROCK] = COMPOUND_STRING(
        "Cuts damage from\n"
        "supereffective moves by a\n"
        "quarter. It does not change\n"
        "type immunities. Mold Breaker\n"
        "can bypass it."),

    [ABILITY_SNOW_WARNING] = COMPOUND_STRING(
        "On entry, starts snow for\n"
        "five turns, or eight with Icy\n"
        "Rock. Snow does not cause\n"
        "hail damage."),

    [ABILITY_HONEY_GATHER] = COMPOUND_STRING(
        "When empty-handed after\n"
        "battle, it may find Honey.\n"
        "Higher levels improve the\n"
        "chance, up to half the time at\n"
        "level 91 or above."),

    [ABILITY_FRISK] = COMPOUND_STRING(
        "On entry, reveals the held\n"
        "items of the foes currently\n"
        "in battle. It does not remove\n"
        "or copy those items."),

    [ABILITY_RECKLESS] = COMPOUND_STRING(
        "Boosts moves that cause\n"
        "recoil or crash damage by a\n"
        "fifth. It does not prevent\n"
        "the damage it takes from\n"
        "using them."),

    [ABILITY_MULTITYPE] = COMPOUND_STRING(
        "Arceus changes type to match\n"
        "its held Plate. Its Judgment\n"
        "move matches that type.\n"
        "Without a Plate, Arceus is\n"
        "Normal-type."),

    [ABILITY_FLOWER_GIFT] = COMPOUND_STRING(
        "In sun, Cherrim takes\n"
        "Sunshine Form and gives\n"
        "itself and its ally half more\n"
        "Attack and Sp. Def. The\n"
        "boosts end when the sun is\n"
        "gone."),

    [ABILITY_BAD_DREAMS] = COMPOUND_STRING(
        "At each turn's end, sleeping\n"
        "foes lose an eighth of their\n"
        "max HP. Also affects\n"
        "Comatose. Magic Guard blocks\n"
        "it."),

    [ABILITY_PICKPOCKET] = COMPOUND_STRING(
        "When empty-handed, steals\n"
        "the attacker's held item\n"
        "after a contact hit. Items\n"
        "that cannot be stolen and\n"
        "Sticky Hold can prevent this."),

    [ABILITY_SHEER_FORCE] = COMPOUND_STRING(
        "Boosts moves with added\n"
        "effects by three tenths, but\n"
        "removes those effects and\n"
        "their Life Orb recoil.\n"
        "Berserk and Pickpocket can\n"
        "still activate."),

    [ABILITY_CONTRARY] = COMPOUND_STRING(
        "Reverses changes to its stat\n"
        "stages. A stat rise becomes a\n"
        "drop, and a drop becomes a\n"
        "rise."),

    [ABILITY_UNNERVE] = COMPOUND_STRING(
        "Stops foes from eating their\n"
        "held Berries on their own.\n"
        "Forced Berry use, such as\n"
        "Teatime, still works."),

    [ABILITY_DEFIANT] = COMPOUND_STRING(
        "When a foe lowers one of its\n"
        "stats, its Attack rises by\n"
        "two stages."),

    [ABILITY_DEFEATIST] = COMPOUND_STRING(
        "At half its max HP or less,\n"
        "its Attack and Sp. Atk are\n"
        "halved."),

    [ABILITY_CURSED_BODY] = COMPOUND_STRING(
        "A damaging hit has a 3 in 10\n"
        "chance to Disable the\n"
        "attacker's move for four\n"
        "turns. Contact is not needed."),

    [ABILITY_HEALER] = COMPOUND_STRING(
        "At each turn's end, it has a\n"
        "half chance to cure its ally's\n"
        "major status condition. It\n"
        "does not cure itself."),

    [ABILITY_FRIEND_GUARD] = COMPOUND_STRING(
        "Its ally takes a quarter less\n"
        "damage from attacks while it\n"
        "is in battle. It does not\n"
        "protect itself or reduce\n"
        "confusion damage."),

    [ABILITY_WEAK_ARMOR] = COMPOUND_STRING(
        "When a physical hit damages\n"
        "it, lowers its Defense by one\n"
        "stage and raises its Speed by\n"
        "two stages. Contact is not\n"
        "required."),

    [ABILITY_HEAVY_METAL] = COMPOUND_STRING(
        "Doubles its weight. This\n"
        "changes weight-based\n"
        "attacks such as Low Kick,\n"
        "Grass Knot, Heavy Slam and\n"
        "Heat Crash."),

    [ABILITY_LIGHT_METAL] = COMPOUND_STRING(
        "Halves its weight. This\n"
        "changes weight-based\n"
        "attacks such as Low Kick,\n"
        "Grass Knot, Heavy Slam and\n"
        "Heat Crash."),

    [ABILITY_MULTISCALE] = COMPOUND_STRING(
        "At full HP, attacks deal half\n"
        "damage to it. Losing any HP\n"
        "removes this protection\n"
        "until it is fully healed."),

    [ABILITY_TOXIC_BOOST] = COMPOUND_STRING(
        "While poisoned or badly\n"
        "poisoned, boosts physical\n"
        "move power by half. It still\n"
        "takes poison damage."),

    [ABILITY_FLARE_BOOST] = COMPOUND_STRING(
        "While it is burned, its\n"
        "special moves gain half more\n"
        "power. It still takes burn\n"
        "damage each turn."),

    [ABILITY_HARVEST] = COMPOUND_STRING(
        "If empty-handed, it has a\n"
        "half chance each turn to\n"
        "regrow its last used Berry.\n"
        "In sun it always regrows it.\n"
        "Stolen or destroyed Berries\n"
        "cannot regrow."),

    [ABILITY_TELEPATHY] = COMPOUND_STRING(
        "Avoids damaging moves used\n"
        "by its ally, including spread\n"
        "attacks. Allies' status moves\n"
        "can still affect it."),

    [ABILITY_MOODY] = COMPOUND_STRING(
        "Each turn's end, raises a\n"
        "random stat by two stages\n"
        "and lowers a different stat\n"
        "by one. Accuracy and evasion\n"
        "are never chosen."),

    [ABILITY_OVERCOAT] = COMPOUND_STRING(
        "Prevents damage from\n"
        "sandstorm and hail, and\n"
        "blocks powder moves and\n"
        "Effect Spore. It does not\n"
        "block all weather effects."),

    [ABILITY_POISON_TOUCH] = COMPOUND_STRING(
        "Its damaging contact moves\n"
        "have a 3 in 10 chance to\n"
        "poison their target. Shield\n"
        "Dust and poison immunity can\n"
        "prevent this."),

    [ABILITY_REGENERATOR] = COMPOUND_STRING(
        "When it switches out,\n"
        "restores a third of its max\n"
        "HP."),

    [ABILITY_BIG_PECKS] = COMPOUND_STRING(
        "Other Pokémon cannot lower\n"
        "its Defense. It can still\n"
        "lower its own Defense with a\n"
        "move."),

    [ABILITY_SAND_RUSH] = COMPOUND_STRING(
        "Doubles its Speed in a\n"
        "sandstorm. It also takes no\n"
        "sandstorm damage."),

    [ABILITY_WONDER_SKIN] = COMPOUND_STRING(
        "Caps the base hit chance of\n"
        "status moves aimed at it at\n"
        "half. Accuracy modifiers\n"
        "still apply. Always-hit moves\n"
        "still hit."),

    [ABILITY_ANALYTIC] = COMPOUND_STRING(
        "If it moves last, its attacks\n"
        "gain three tenths more power.\n"
        "This does not boost Future\n"
        "Sight or Doom Desire."),

    [ABILITY_ILLUSION] = COMPOUND_STRING(
        "Appears as the last other\n"
        "healthy Pokémon in its party\n"
        "until an attack damages it.\n"
        "Its actual type, stats, moves\n"
        "and Ability stay unchanged."),

    [ABILITY_IMPOSTER] = COMPOUND_STRING(
        "On entry, transforms into the\n"
        "opposing Pokémon, copying its\n"
        "form, stats, Ability, stat\n"
        "changes and moves. Its own HP\n"
        "remains unchanged."),

    [ABILITY_INFILTRATOR] = COMPOUND_STRING(
        "Its moves bypass the foe's\n"
        "Substitute, Reflect, Light\n"
        "Screen, Aurora Veil,\n"
        "Safeguard and Mist."),

    [ABILITY_MUMMY] = COMPOUND_STRING(
        "When a contact move damages\n"
        "it, replaces the attacker's\n"
        "Ability with Mummy. Protected\n"
        "Abilities and Ability Shield\n"
        "prevent the change."),

    [ABILITY_MOXIE] = COMPOUND_STRING(
        "Each Pokémon it knocks out\n"
        "with an attack raises its\n"
        "Attack by one stage. Indirect\n"
        "damage does not activate it."),

    [ABILITY_JUSTIFIED] = COMPOUND_STRING(
        "When a Dark-type move\n"
        "damages it, its Attack rises\n"
        "by one stage. The move still\n"
        "deals damage."),

    [ABILITY_RATTLED] = COMPOUND_STRING(
        "When a damaging Bug, Ghost or\n"
        "Dark move hits it, raises its\n"
        "Speed by one stage.\n"
        "Intimidate also raises its\n"
        "Speed by one stage."),

    [ABILITY_MAGIC_BOUNCE] = COMPOUND_STRING(
        "Reflects many targeted\n"
        "status moves, including\n"
        "hazards, back at their user.\n"
        "It does not reflect every\n"
        "status move or reflect an\n"
        "already bounced move."),

    [ABILITY_SAP_SIPPER] = COMPOUND_STRING(
        "Grass moves do not affect it.\n"
        "When one targets it, its\n"
        "Attack rises by one stage\n"
        "instead. This also works with\n"
        "Grass status moves."),

    [ABILITY_PRANKSTER] = COMPOUND_STRING(
        "Its status moves get +1\n"
        "priority. Opposing\n"
        "Dark-types are immune to\n"
        "status moves given priority\n"
        "by this Ability."),

    [ABILITY_SAND_FORCE] = COMPOUND_STRING(
        "In a sandstorm, boosts its\n"
        "Rock, Ground and Steel moves\n"
        "by three tenths. It also\n"
        "takes no sandstorm damage."),

    [ABILITY_IRON_BARBS] = COMPOUND_STRING(
        "When a contact move damages\n"
        "it, the attacker loses an\n"
        "eighth of its max HP. Magic\n"
        "Guard and effects that avoid\n"
        "contact prevent the damage."),

    [ABILITY_ZEN_MODE] = COMPOUND_STRING(
        "At turn's end, Darmanitan\n"
        "enters Zen Mode at half HP or\n"
        "less. It changes back above\n"
        "half HP or when it switches\n"
        "out."),

    [ABILITY_VICTORY_STAR] = COMPOUND_STRING(
        "Boosts its own and its ally's\n"
        "move accuracy by a tenth. Two\n"
        "allies with this Ability boost\n"
        "each other again."),

    [ABILITY_TURBOBLAZE] = COMPOUND_STRING(
        "Its moves bypass defensive\n"
        "Abilities that can be\n"
        "ignored, such as Levitate and\n"
        "Sturdy. Ability Shield\n"
        "protects the target's\n"
        "Ability."),

    [ABILITY_TERAVOLT] = COMPOUND_STRING(
        "Its moves bypass defensive\n"
        "Abilities that can be\n"
        "ignored, such as Levitate and\n"
        "Sturdy. Ability Shield\n"
        "protects the target's\n"
        "Ability."),

    [ABILITY_AROMA_VEIL] = COMPOUND_STRING(
        "Protects it and allies from\n"
        "infatuation, Taunt, Encore,\n"
        "Disable, Torment and effects\n"
        "that block healing."),

    [ABILITY_FLOWER_VEIL] = COMPOUND_STRING(
        "Protects Grass-type Pokémon\n"
        "on its side from other\n"
        "Pokémon's stat drops and\n"
        "major status conditions. It\n"
        "protects itself only if it is\n"
        "Grass-type."),

    [ABILITY_CHEEK_POUCH] = COMPOUND_STRING(
        "Eating a Berry also restores\n"
        "a third of its max HP. Heal\n"
        "Block prevents this\n"
        "recovery."),

    [ABILITY_PROTEAN] = COMPOUND_STRING(
        "Before using a move, it\n"
        "becomes that move's type. It\n"
        "can change type once each\n"
        "time it enters battle.\n"
        "Struggle cannot trigger it."),

    [ABILITY_FUR_COAT] = COMPOUND_STRING(
        "Doubles its Defense when\n"
        "taking an attack. This also\n"
        "protects against special\n"
        "moves that use the target's\n"
        "Defense, such as Psyshock."),

    [ABILITY_MAGICIAN] = COMPOUND_STRING(
        "When empty-handed, steals a\n"
        "held item from a Pokémon it\n"
        "damages with a move. Sticky\n"
        "Hold and items that cannot be\n"
        "stolen can prevent this."),

    [ABILITY_BULLETPROOF] = COMPOUND_STRING(
        "Blocks ball and bomb moves\n"
        "aimed at it, such as Shadow\n"
        "Ball, Sludge Bomb and Aura\n"
        "Sphere."),

    [ABILITY_COMPETITIVE] = COMPOUND_STRING(
        "When a foe lowers one of its\n"
        "stats, its Sp. Atk rises by\n"
        "two stages."),

    [ABILITY_STRONG_JAW] = COMPOUND_STRING(
        "Boosts the power of biting\n"
        "moves by half. It does not\n"
        "boost other contact moves."),

    [ABILITY_REFRIGERATE] = COMPOUND_STRING(
        "Turns its Normal moves into\n"
        "Ice moves and boosts their\n"
        "power by a fifth. Moves with\n"
        "their own changing type, such\n"
        "as Weather Ball, are\n"
        "excluded."),

    [ABILITY_SWEET_VEIL] = COMPOUND_STRING(
        "Protects itself and its\n"
        "allies from falling asleep,\n"
        "including from Yawn. It also\n"
        "prevents them from using\n"
        "Rest."),

    [ABILITY_STANCE_CHANGE] = COMPOUND_STRING(
        "Aegislash changes to Blade\n"
        "Forme before a damaging move\n"
        "and Shield Forme before\n"
        "King's Shield. Other status\n"
        "moves do not change its form."),

    [ABILITY_GALE_WINGS] = COMPOUND_STRING(
        "At full HP, its Flying moves\n"
        "have +1 priority. Losing any\n"
        "HP removes the boost until it\n"
        "is fully healed."),

    [ABILITY_MEGA_LAUNCHER] = COMPOUND_STRING(
        "Its pulse and aura attacks\n"
        "gain half more power. Heal\n"
        "Pulse restores three\n"
        "quarters of the target's max\n"
        "HP instead of half."),

    [ABILITY_GRASS_PELT] = COMPOUND_STRING(
        "While Grassy Terrain is\n"
        "active, its Defense is raised\n"
        "by half. It does not need to\n"
        "be grounded for this boost."),

    [ABILITY_SYMBIOSIS] = COMPOUND_STRING(
        "Passes its held item to an\n"
        "ally after the ally uses up\n"
        "its own. It cannot pass an\n"
        "item that either Pokémon\n"
        "cannot give or receive."),

    [ABILITY_TOUGH_CLAWS] = COMPOUND_STRING(
        "Boosts the power of contact\n"
        "moves by three tenths. A move\n"
        "must actually make contact\n"
        "to get the boost."),

    [ABILITY_PIXILATE] = COMPOUND_STRING(
        "Its Normal-type moves become\n"
        "Fairy-type and gain a fifth\n"
        "more power."),

    [ABILITY_GOOEY] = COMPOUND_STRING(
        "When a contact move damages\n"
        "it, the attacker's Speed\n"
        "falls by one stage. Effects\n"
        "that avoid contact prevent\n"
        "this."),

    [ABILITY_AERILATE] = COMPOUND_STRING(
        "Its Normal moves become\n"
        "Flying moves. Their power\n"
        "rises by a fifth."),

    [ABILITY_PARENTAL_BOND] = COMPOUND_STRING(
        "Eligible attacks hit twice.\n"
        "The second hit deals a\n"
        "quarter of normal damage.\n"
        "Multi-hit, charging and\n"
        "attacks hitting multiple\n"
        "targets are exempt."),

    [ABILITY_DARK_AURA] = COMPOUND_STRING(
        "Raises everyone's Dark move\n"
        "power by a third. Aura Break\n"
        "reverses this to a quarter\n"
        "reduction."),

    [ABILITY_FAIRY_AURA] = COMPOUND_STRING(
        "While it is in battle, all\n"
        "Pokémon's Fairy moves gain a\n"
        "third more power. Aura Break\n"
        "instead cuts their power by a\n"
        "quarter."),

    [ABILITY_AURA_BREAK] = COMPOUND_STRING(
        "Dark Aura and Fairy Aura\n"
        "reduce their types' move\n"
        "power by a quarter instead of\n"
        "boosting it. Affects\n"
        "everyone."),

    [ABILITY_PRIMORDIAL_SEA] = COMPOUND_STRING(
        "Brings heavy rain while it\n"
        "remains in battle. Fire\n"
        "attacks fail, Water attacks\n"
        "gain half more power, and\n"
        "ordinary weather cannot\n"
        "replace it."),

    [ABILITY_DESOLATE_LAND] = COMPOUND_STRING(
        "Calls harsh sunlight while it\n"
        "remains in battle. Damaging\n"
        "Water moves fail, and\n"
        "ordinary weather cannot\n"
        "replace it."),

    [ABILITY_DELTA_STREAM] = COMPOUND_STRING(
        "Calls strong winds while it\n"
        "remains in battle. Moves lose\n"
        "the extra damage caused by a\n"
        "Flying-type weakness."),

    [ABILITY_STAMINA] = COMPOUND_STRING(
        "Each damaging hit raises its\n"
        "Defense by one stage if it\n"
        "survives. Hits on its\n"
        "substitute do not trigger it."),

    [ABILITY_WIMP_OUT] = COMPOUND_STRING(
        "When its HP falls from above\n"
        "half to half or less, switches\n"
        "out if possible. In a wild\n"
        "battle, it flees instead."),

    [ABILITY_EMERGENCY_EXIT] = COMPOUND_STRING(
        "When damage takes it from\n"
        "above half HP to half or less,\n"
        "it switches out if a\n"
        "replacement is available. In a\n"
        "wild battle, it flees instead."),

    [ABILITY_WATER_COMPACTION] = COMPOUND_STRING(
        "When a Water hit damages it,\n"
        "raises its Defense by two\n"
        "stages. The Water move still\n"
        "deals damage."),

    [ABILITY_MERCILESS] = COMPOUND_STRING(
        "Its attacks against poisoned\n"
        "targets are always critical\n"
        "hits, unless an effect such\n"
        "as Battle Armor prevents\n"
        "critical hits."),

    [ABILITY_SHIELDS_DOWN] = COMPOUND_STRING(
        "Minior's shell blocks status\n"
        "problems. At turn's end it\n"
        "becomes Core Form at half HP\n"
        "or less, and Meteor Form\n"
        "above half HP."),

    [ABILITY_STAKEOUT] = COMPOUND_STRING(
        "Doubles its damage against a\n"
        "target that switched in this\n"
        "turn. This works with both\n"
        "physical and special attacks."),

    [ABILITY_WATER_BUBBLE] = COMPOUND_STRING(
        "Doubles the power of its\n"
        "Water-type moves, halves\n"
        "Fire damage it takes and\n"
        "prevents burns."),

    [ABILITY_STEELWORKER] = COMPOUND_STRING(
        "Boosts the power of its\n"
        "Steel-type moves by half. It\n"
        "does not change its type."),

    [ABILITY_BERSERK] = COMPOUND_STRING(
        "A damaging hit taking it from\n"
        "above half HP to half or less\n"
        "raises its Sp. Atk by one\n"
        "stage."),

    [ABILITY_SLUSH_RUSH] = COMPOUND_STRING(
        "Doubles its Speed in hail or\n"
        "snow. It does not protect it\n"
        "from hail damage."),

    [ABILITY_LONG_REACH] = COMPOUND_STRING(
        "Its attacks never make\n"
        "contact. Contact-triggered\n"
        "Abilities and items do not\n"
        "activate against its attacks."),

    [ABILITY_LIQUID_VOICE] = COMPOUND_STRING(
        "Its sound-based moves become\n"
        "Water-type. This does not\n"
        "give them an extra power\n"
        "boost."),

    [ABILITY_TRIAGE] = COMPOUND_STRING(
        "Gives its healing moves three\n"
        "extra levels of priority,\n"
        "including draining attacks.\n"
        "Ordinary speed ties still\n"
        "apply."),

    [ABILITY_GALVANIZE] = COMPOUND_STRING(
        "Its Normal-type moves become\n"
        "Electric-type and gain a\n"
        "fifth more power."),

    [ABILITY_SURGE_SURFER] = COMPOUND_STRING(
        "Doubles its Speed while\n"
        "Electric Terrain is active. It\n"
        "does not need to be on the\n"
        "ground."),

    [ABILITY_SCHOOLING] = COMPOUND_STRING(
        "At level 20 or higher,\n"
        "Wishiwashi forms a school\n"
        "above a quarter of its max\n"
        "HP. At a quarter or less, it\n"
        "returns to Solo Form at\n"
        "turn's end."),

    [ABILITY_DISGUISE] = COMPOUND_STRING(
        "Mimikyu's disguise absorbs\n"
        "one damaging hit. Breaking it\n"
        "costs an eighth of its max HP.\n"
        "It does not restore on\n"
        "switching."),

    [ABILITY_BATTLE_BOND] = COMPOUND_STRING(
        "Once per battle, when\n"
        "Greninja knocks a Pokémon\n"
        "out, its Attack, Sp. Atk and\n"
        "Speed rise by one stage."),

    [ABILITY_POWER_CONSTRUCT] = COMPOUND_STRING(
        "At turn's end with half HP or\n"
        "less, Zygarde becomes\n"
        "Complete Form, gaining a\n"
        "larger HP pool. It stays\n"
        "Complete for the rest of the\n"
        "battle."),

    [ABILITY_CORROSION] = COMPOUND_STRING(
        "It can poison Poison and\n"
        "Steel types. Poison attacks\n"
        "still cannot damage Steel\n"
        "types, and poison-blocking\n"
        "Abilities still work."),

    [ABILITY_COMATOSE] = COMPOUND_STRING(
        "It is treated as asleep but\n"
        "can act normally. It cannot\n"
        "gain another major status.\n"
        "Sleep-targeting moves can\n"
        "affect it."),

    [ABILITY_QUEENLY_MAJESTY] = COMPOUND_STRING(
        "Blocks foes' priority moves\n"
        "targeting it or its allies. It\n"
        "does not stop priority moves\n"
        "that affect the field or the\n"
        "foe's own side."),

    [ABILITY_INNARDS_OUT] = COMPOUND_STRING(
        "If an attack knocks it out,\n"
        "the attacker loses HP equal\n"
        "to the HP it had before that\n"
        "hit. Magic Guard blocks this\n"
        "damage."),

    [ABILITY_DANCER] = COMPOUND_STRING(
        "When another Pokémon uses a\n"
        "dance move, it copies that\n"
        "move immediately without\n"
        "using its own turn."),

    [ABILITY_BATTERY] = COMPOUND_STRING(
        "Raises its ally's special move\n"
        "power by three tenths. It\n"
        "does not boost its own moves."),

    [ABILITY_FLUFFY] = COMPOUND_STRING(
        "Contact attacks deal half\n"
        "damage to it, while Fire\n"
        "attacks deal double. A Fire\n"
        "attack that makes contact\n"
        "deals normal damage."),

    [ABILITY_DAZZLING] = COMPOUND_STRING(
        "Blocks foes' priority moves\n"
        "aimed at it or its allies.\n"
        "Moves aimed at the whole\n"
        "field can still work."),

    [ABILITY_SOUL_HEART] = COMPOUND_STRING(
        "Whenever another Pokémon\n"
        "faints, raises its Sp. Atk by\n"
        "one stage. This includes\n"
        "allies, even if it did not\n"
        "cause the knockout."),

    [ABILITY_TANGLING_HAIR] = COMPOUND_STRING(
        "When a contact hit damages\n"
        "it, lowers the attacker's\n"
        "Speed by one stage. Effects\n"
        "that avoid contact prevent\n"
        "this."),

    [ABILITY_RECEIVER] = COMPOUND_STRING(
        "When its ally faints, it\n"
        "copies that ally's Ability.\n"
        "Some Abilities cannot be\n"
        "copied, and holding an Ability\n"
        "Shield prevents the change."),

    [ABILITY_POWER_OF_ALCHEMY] = COMPOUND_STRING(
        "When its ally faints, copies\n"
        "that ally's Ability. Some\n"
        "special Abilities cannot be\n"
        "copied."),

    [ABILITY_BEAST_BOOST] = COMPOUND_STRING(
        "Each Pokémon it knocks out\n"
        "raises its highest stat,\n"
        "other than HP, by one stage."),

    [ABILITY_RKS_SYSTEM] = COMPOUND_STRING(
        "Silvally's type matches its\n"
        "held Memory. With no Memory,\n"
        "it is Normal. The Memory also\n"
        "sets Multi-Attack's type."),

    [ABILITY_ELECTRIC_SURGE] = COMPOUND_STRING(
        "On entry, sets Electric\n"
        "Terrain for five turns.\n"
        "Grounded Pokémon cannot fall\n"
        "asleep and their Electric\n"
        "moves gain three tenths more\n"
        "power."),

    [ABILITY_PSYCHIC_SURGE] = COMPOUND_STRING(
        "On entry, sets Psychic\n"
        "Terrain for five turns. It\n"
        "boosts grounded Pokémon's\n"
        "Psychic moves by three\n"
        "tenths and blocks foes'\n"
        "priority against them."),

    [ABILITY_MISTY_SURGE] = COMPOUND_STRING(
        "On entry, sets Misty Terrain\n"
        "for five turns. Grounded\n"
        "Pokémon resist major status\n"
        "and confusion, and take half\n"
        "damage from Dragon attacks."),

    [ABILITY_GRASSY_SURGE] = COMPOUND_STRING(
        "On entry, sets Grassy Terrain\n"
        "for five turns. It heals\n"
        "grounded Pokémon each turn\n"
        "and strengthens their Grass\n"
        "moves."),

    [ABILITY_FULL_METAL_BODY] = COMPOUND_STRING(
        "Other Pokémon cannot lower\n"
        "its stats. Its own\n"
        "stat-lowering moves still\n"
        "work. Mold Breaker cannot\n"
        "bypass this protection."),

    [ABILITY_SHADOW_SHIELD] = COMPOUND_STRING(
        "At full HP, halves damage\n"
        "from moves. Mold Breaker\n"
        "cannot bypass it. It does not\n"
        "reduce damage from weather\n"
        "or entry hazards."),

    [ABILITY_PRISM_ARMOR] = COMPOUND_STRING(
        "Super-effective attacks deal\n"
        "a quarter less damage to it.\n"
        "Mold Breaker cannot bypass\n"
        "this protection."),

    [ABILITY_NEUROFORCE] = COMPOUND_STRING(
        "Its super-effective attacks\n"
        "deal a quarter more damage.\n"
        "Neutral and resisted attacks\n"
        "are unchanged."),

    [ABILITY_INTREPID_SWORD] = COMPOUND_STRING(
        "On its first entry each\n"
        "battle, raises its Attack by\n"
        "one stage. Switching out and\n"
        "returning does not give\n"
        "another boost."),

    [ABILITY_DAUNTLESS_SHIELD] = COMPOUND_STRING(
        "The first time it enters\n"
        "battle, its Defense rises by\n"
        "one stage. This happens only\n"
        "once per battle."),

    [ABILITY_LIBERO] = COMPOUND_STRING(
        "Before using a move, changes\n"
        "its type to that move's type.\n"
        "It can change type only once\n"
        "each time it enters battle.\n"
        "Struggle does not trigger it."),

    [ABILITY_BALL_FETCH] = COMPOUND_STRING(
        "Once per battle, if it holds\n"
        "no item after a failed catch,\n"
        "it retrieves a thrown Ball as\n"
        "its held item."),

    [ABILITY_COTTON_DOWN] = COMPOUND_STRING(
        "When a damaging move hits it,\n"
        "every other Pokémon in\n"
        "battle loses one Speed stage,\n"
        "including its ally."),

    [ABILITY_PROPELLER_TAIL] = COMPOUND_STRING(
        "Its moves ignore redirection\n"
        "from Follow Me, Rage Powder,\n"
        "Lightning Rod and Storm\n"
        "Drain. It still faces the\n"
        "chosen target's immunities."),

    [ABILITY_MIRROR_ARMOR] = COMPOUND_STRING(
        "Reflects other Pokémon's\n"
        "stat drops back at the\n"
        "Pokémon causing them. It does\n"
        "not reflect its own stat\n"
        "drops."),

    [ABILITY_GULP_MISSILE] = COMPOUND_STRING(
        "After Surf or Dive, its next\n"
        "hit spits prey for a quarter\n"
        "of the attacker's max HP.\n"
        "Loading above half HP drops\n"
        "Defense one stage; otherwise\n"
        "it paralyzes."),

    [ABILITY_STALWART] = COMPOUND_STRING(
        "Its moves ignore redirection\n"
        "from Follow Me, Rage Powder,\n"
        "Lightning Rod and Storm\n"
        "Drain. It still faces the\n"
        "chosen target's immunities."),

    [ABILITY_STEAM_ENGINE] = COMPOUND_STRING(
        "When a Fire or Water move\n"
        "hits it, its Speed rises six\n"
        "stages. Water moves are\n"
        "never super effective on it.\n"
        "In the party, eggs hatch\n"
        "twice as fast."),

    [ABILITY_PUNK_ROCK] = COMPOUND_STRING(
        "Boosts its sound moves by\n"
        "three tenths and halves\n"
        "damage it takes from sound\n"
        "moves. It does not block\n"
        "their other effects."),

    [ABILITY_SAND_SPIT] = COMPOUND_STRING(
        "When a damaging move hits it,\n"
        "starts a sandstorm for five\n"
        "turns, or eight with Smooth\n"
        "Rock."),

    [ABILITY_ICE_SCALES] = COMPOUND_STRING(
        "Special attacks deal half\n"
        "damage to it. Physical\n"
        "attacks are unchanged. Mold\n"
        "Breaker ignores this\n"
        "protection."),

    [ABILITY_RIPEN] = COMPOUND_STRING(
        "Doubles its Berries' healing,\n"
        "PP recovery and stat boosts.\n"
        "Resistance Berries cut\n"
        "damage to a quarter instead\n"
        "of half. Status cures stay\n"
        "the same."),

    [ABILITY_ICE_FACE] = COMPOUND_STRING(
        "Eiscue's ice head blocks one\n"
        "physical hit and breaks into\n"
        "Noice Form. Entering hail or\n"
        "snow restores the head.\n"
        "Special attacks bypass it."),

    [ABILITY_POWER_SPOT] = COMPOUND_STRING(
        "While it is in battle, its\n"
        "ally's damaging moves gain\n"
        "three tenths more power. It\n"
        "does not boost its own moves."),

    [ABILITY_MIMICRY] = COMPOUND_STRING(
        "Changes its type to match\n"
        "active terrain: Electric,\n"
        "Grass, Fairy or Psychic. When\n"
        "terrain ends, its original\n"
        "types return."),

    [ABILITY_SCREEN_CLEANER] = COMPOUND_STRING(
        "On entry, removes Reflect,\n"
        "Light Screen and Aurora Veil\n"
        "from both sides. It leaves\n"
        "other barriers and entry\n"
        "hazards in place."),

    [ABILITY_STEELY_SPIRIT] = COMPOUND_STRING(
        "Boosts its own and its ally's\n"
        "Steel-type moves by half. Two\n"
        "allies with this Ability boost\n"
        "each other again."),

    [ABILITY_PERISH_BODY] = COMPOUND_STRING(
        "A contact hit starts a\n"
        "three-turn perish count on\n"
        "it and the attacker. Each\n"
        "faints when its count ends\n"
        "unless it switches out first."),

    [ABILITY_WANDERING_SPIRIT] = COMPOUND_STRING(
        "Contact hits swap its Ability\n"
        "with the attacker's if that\n"
        "Ability can be swapped.\n"
        "Either Pokémon's Ability\n"
        "Shield blocks the swap."),

    [ABILITY_GORILLA_TACTICS] = COMPOUND_STRING(
        "Gives its physical attacks\n"
        "half more power, but locks it\n"
        "into its first chosen move\n"
        "until it leaves battle."),

    [ABILITY_NEUTRALIZING_GAS] = COMPOUND_STRING(
        "While it is in battle,\n"
        "suppresses most other\n"
        "Pokémon's Abilities,\n"
        "including its ally's. Some\n"
        "special Abilities and Ability\n"
        "Shield are exempt."),

    [ABILITY_PASTEL_VEIL] = COMPOUND_STRING(
        "Prevents poison for itself\n"
        "and its ally. On entry, it also\n"
        "cures their existing poison.\n"
        "Other status conditions are\n"
        "unaffected."),

    [ABILITY_HUNGER_SWITCH] = COMPOUND_STRING(
        "Morpeko changes between Full\n"
        "Belly and Hangry Form each\n"
        "turn. Aura Wheel is Electric\n"
        "in Full Belly Form and Dark in\n"
        "Hangry Form."),

    [ABILITY_QUICK_DRAW] = COMPOUND_STRING(
        "Its damaging moves have a 3\n"
        "in 10 chance to go first\n"
        "among moves of the same\n"
        "priority. Status moves get no\n"
        "benefit."),

    [ABILITY_UNSEEN_FIST] = COMPOUND_STRING(
        "Its contact moves strike\n"
        "through Protect, Detect and\n"
        "similar shields, but deal only\n"
        "a quarter of their damage.\n"
        "Spiky Shield and the like\n"
        "still punish the contact."),

    [ABILITY_CURIOUS_MEDICINE] = COMPOUND_STRING(
        "On entering battle, it resets\n"
        "its ally's stat stages to\n"
        "normal, removing both boosts\n"
        "and drops."),

    [ABILITY_TRANSISTOR] = COMPOUND_STRING(
        "Boosts the damage of its\n"
        "Electric-type moves by three\n"
        "tenths."),

    [ABILITY_DRAGONS_MAW] = COMPOUND_STRING(
        "Raises damage from its\n"
        "Dragon attacks by half."),

    [ABILITY_CHILLING_NEIGH] = COMPOUND_STRING(
        "Each Pokémon it knocks out\n"
        "raises its Attack by one\n"
        "stage."),

    [ABILITY_GRIM_NEIGH] = COMPOUND_STRING(
        "Each Pokémon it knocks out\n"
        "with an attack raises its Sp.\n"
        "Atk by one stage. Indirect\n"
        "damage does not activate it."),

    [ABILITY_AS_ONE_ICE_RIDER] = COMPOUND_STRING(
        "Foes cannot eat Berries. Each\n"
        "Pokémon it knocks out raises\n"
        "its Attack by one stage."),

    [ABILITY_AS_ONE_SHADOW_RIDER] = COMPOUND_STRING(
        "Foes cannot eat Berries. Each\n"
        "Pokémon it knocks out raises\n"
        "its Sp. Atk by one stage."),

    [ABILITY_LINGERING_AROMA] = COMPOUND_STRING(
        "When a contact move damages\n"
        "it, replaces the attacker's\n"
        "Ability with Lingering Aroma.\n"
        "Protected Abilities and\n"
        "Ability Shield prevent the\n"
        "change."),

    [ABILITY_SEED_SOWER] = COMPOUND_STRING(
        "When a damaging move hits it,\n"
        "sets Grassy Terrain for five\n"
        "turns, or eight with Terrain\n"
        "Extender. Hits on its\n"
        "substitute do not trigger it."),

    [ABILITY_THERMAL_EXCHANGE] = COMPOUND_STRING(
        "When a Fire hit damages it,\n"
        "raises its Attack by one\n"
        "stage. Prevents burns, but\n"
        "does not reduce Fire damage."),

    [ABILITY_ANGER_SHELL] = COMPOUND_STRING(
        "A hit taking it from above\n"
        "half HP to half or less raises\n"
        "Attack, Sp. Atk and Speed one\n"
        "stage, and lowers Defense and\n"
        "Sp. Def one."),

    [ABILITY_PURIFYING_SALT] = COMPOUND_STRING(
        "Prevents sleep, poison,\n"
        "paralysis, burns and\n"
        "freezing. Halves damage from\n"
        "Ghost moves. It does not\n"
        "prevent confusion."),

    [ABILITY_WELL_BAKED_BODY] = COMPOUND_STRING(
        "Fire-type moves have no\n"
        "effect on it and raise its\n"
        "Defense by two stages\n"
        "instead. Still blocks them at\n"
        "maximum Defense."),

    [ABILITY_WIND_RIDER] = COMPOUND_STRING(
        "Wind moves have no effect and\n"
        "raise its Attack by one\n"
        "stage. Its side's Tailwind\n"
        "also raises Attack, including\n"
        "when it enters battle."),

    [ABILITY_GUARD_DOG] = COMPOUND_STRING(
        "Intimidate raises its Attack\n"
        "by one stage instead of\n"
        "lowering it. Moves and items\n"
        "that force a switch cannot\n"
        "push it out."),

    [ABILITY_ROCKY_PAYLOAD] = COMPOUND_STRING(
        "Boosts the power of its\n"
        "Rock-type moves by half."),

    [ABILITY_WIND_POWER] = COMPOUND_STRING(
        "A damaging wind hit or its\n"
        "side's Tailwind charges it,\n"
        "doubling its next Electric\n"
        "move's power. It is not immune\n"
        "to wind moves."),

    [ABILITY_ZERO_TO_HERO] = COMPOUND_STRING(
        "Palafin becomes Hero Form\n"
        "after switching out. It keeps\n"
        "that form when it returns,\n"
        "until the battle ends."),

    [ABILITY_COMMANDER] = COMPOUND_STRING(
        "Tatsugiri hides in its ally\n"
        "Dondozo and cannot act.\n"
        "Dondozo gains two stages in\n"
        "all five battle stats.\n"
        "Neither can switch normally."),

    [ABILITY_ELECTROMORPHOSIS] = COMPOUND_STRING(
        "When a move damages it, it\n"
        "becomes charged. Its next\n"
        "Electric move has double\n"
        "power. The charge does not\n"
        "stack."),

    [ABILITY_PROTOSYNTHESIS] = COMPOUND_STRING(
        "In sunlight or with Booster\n"
        "Energy, boosts its highest\n"
        "stat other than HP by three\n"
        "tenths, or by half if Speed.\n"
        "The item boost ends on\n"
        "switching."),

    [ABILITY_QUARK_DRIVE] = COMPOUND_STRING(
        "In Electric Terrain or with\n"
        "Booster Energy, boosts its\n"
        "highest non-HP stat by three\n"
        "tenths, or by half if Speed.\n"
        "The item boost ends on\n"
        "switching."),

    [ABILITY_GOOD_AS_GOLD] = COMPOUND_STRING(
        "Blocks status moves that\n"
        "target it, including helpful\n"
        "ones from allies. Moves that\n"
        "affect the whole field can\n"
        "still work."),

    [ABILITY_VESSEL_OF_RUIN] = COMPOUND_STRING(
        "While it is in battle, cuts\n"
        "other Pokémon's Sp. Atk by a\n"
        "quarter, including allies.\n"
        "Others with this Ability are\n"
        "exempt."),

    [ABILITY_SWORD_OF_RUIN] = COMPOUND_STRING(
        "While it is in battle, cuts\n"
        "other Pokémon's Defense by a\n"
        "quarter, including allies.\n"
        "Others with this Ability are\n"
        "exempt."),

    [ABILITY_TABLETS_OF_RUIN] = COMPOUND_STRING(
        "While it is in battle, cuts\n"
        "other Pokémon's Attack by a\n"
        "quarter, including allies.\n"
        "Others with this Ability are\n"
        "exempt."),

    [ABILITY_BEADS_OF_RUIN] = COMPOUND_STRING(
        "Cuts the Sp. Def of other\n"
        "Pokémon in battle by a\n"
        "quarter. Other holders of\n"
        "this Ability are immune."),

    [ABILITY_ORICHALCUM_PULSE] = COMPOUND_STRING(
        "On entry, brings sun for five\n"
        "turns. While sun is active,\n"
        "its Attack rises by a third,\n"
        "in addition to the weather's\n"
        "usual effects."),

    [ABILITY_HADRON_ENGINE] = COMPOUND_STRING(
        "On entry, sets Electric\n"
        "Terrain for five turns. While\n"
        "Electric Terrain is active,\n"
        "its Sp. Atk rises by a third,\n"
        "even if it is not grounded."),

    [ABILITY_OPPORTUNIST] = COMPOUND_STRING(
        "When a foe raises its stats,\n"
        "copies those increases onto\n"
        "itself. It does not copy an\n"
        "ally's boosts or a foe's stat\n"
        "drops."),

    [ABILITY_CUD_CHEW] = COMPOUND_STRING(
        "After eating a Berry, it\n"
        "repeats that Berry's effect\n"
        "at the end of the following\n"
        "turn."),

    [ABILITY_SHARPNESS] = COMPOUND_STRING(
        "Boosts the power of its\n"
        "slicing moves by half."),

    [ABILITY_SUPREME_OVERLORD] = COMPOUND_STRING(
        "On entering battle, gains a\n"
        "tenth more move power per\n"
        "fainted teammate, up to five.\n"
        "The boost lasts until it\n"
        "switches out."),

    [ABILITY_COSTAR] = COMPOUND_STRING(
        "On entering battle, it copies\n"
        "its ally's stat stages and\n"
        "critical-hit boosts,\n"
        "replacing its own."),

    [ABILITY_TOXIC_DEBRIS] = COMPOUND_STRING(
        "When a physical hit damages\n"
        "it, lays one layer of Toxic\n"
        "Spikes on the foes' side, up\n"
        "to two. Contact is not\n"
        "required."),

    [ABILITY_ARMOR_TAIL] = COMPOUND_STRING(
        "Blocks foes' priority moves\n"
        "aimed at it or its allies.\n"
        "Moves aimed at the whole\n"
        "field can still work."),

    [ABILITY_EARTH_EATER] = COMPOUND_STRING(
        "Ground moves cannot hurt it\n"
        "and instead restore a\n"
        "quarter of its max HP. Heal\n"
        "Block prevents the recovery."),

    [ABILITY_MYCELIUM_MIGHT] = COMPOUND_STRING(
        "Its status moves act last\n"
        "within their priority\n"
        "bracket, but ignore\n"
        "defensive Abilities as Mold\n"
        "Breaker does. Damaging moves\n"
        "are unchanged."),

    [ABILITY_HOSPITALITY] = COMPOUND_STRING(
        "On entry, restores a quarter\n"
        "of its ally's max HP. It does\n"
        "not heal itself, and Heal\n"
        "Block prevents the recovery."),

    [ABILITY_MINDS_EYE] = COMPOUND_STRING(
        "Its Normal and Fighting moves\n"
        "can hit Ghost-types. It\n"
        "ignores raised evasion and\n"
        "prevents others from\n"
        "lowering its accuracy."),

    [ABILITY_301] = COMPOUND_STRING(
        "Has no effect in this game."),

    [ABILITY_302] = COMPOUND_STRING(
        "Has no effect in this game."),

    [ABILITY_303] = COMPOUND_STRING(
        "Has no effect in this game."),

    [ABILITY_304] = COMPOUND_STRING(
        "Has no effect in this game."),

    [ABILITY_TOXIC_CHAIN] = COMPOUND_STRING(
        "Damaging hits have a 3 in 10\n"
        "chance to badly poison the\n"
        "target. Hits absorbed by a\n"
        "substitute do not trigger it."),

    [ABILITY_SUPERSWEET_SYRUP] = COMPOUND_STRING(
        "On entering battle, lowers\n"
        "both foes' evasion by one\n"
        "stage. This happens only once\n"
        "per battle."),

    [ABILITY_TERRA] = COMPOUND_STRING(
        "On entry it awakens into its\n"
        "Awakened Form, raising its\n"
        "base stat total from 450 to\n"
        "600. It stays that way until\n"
        "it faints or the battle ends."),

    [ABILITY_308] = COMPOUND_STRING(
        "Has no effect in this game."),

    [ABILITY_309] = COMPOUND_STRING(
        "Has no effect in this game."),

    [ABILITY_POISON_PUPPETEER] = COMPOUND_STRING(
        "When Pecharunt poisons a\n"
        "target with a move, it also\n"
        "confuses that target if\n"
        "confusion is possible."),

    [ABILITY_PIERCING_DRILL] = COMPOUND_STRING(
        "Its contact moves strike\n"
        "through Protect, Detect and\n"
        "similar shields, but deal only\n"
        "a quarter of their damage.\n"
        "Spiky Shield and the like\n"
        "still punish the contact."),

    [ABILITY_DRAGONIZE] = COMPOUND_STRING(
        "Its Normal-type moves become\n"
        "Dragon-type and get a fifth\n"
        "more power."),

    [ABILITY_EELEVATE] = COMPOUND_STRING(
        "It floats, untouched by\n"
        "Ground moves, Spikes, Sticky\n"
        "Web and terrain. Each Pokémon\n"
        "it knocks out raises its\n"
        "highest stat by one stage.\n"
        "Mold Breaker can't ground it."),

    [ABILITY_314] = COMPOUND_STRING(
        "Has no effect in this game."),

    [ABILITY_MEGA_SOL] = COMPOUND_STRING(
        "Its own moves act as if the\n"
        "sun is out, in any weather:\n"
        "Fire up by half, Water down by\n"
        "half, Solar Beam skips\n"
        "charging, and Thunder and\n"
        "Hurricane hit half the time."),

    [ABILITY_FIRE_MANE] = COMPOUND_STRING(
        "Its Fire-type moves gain half\n"
        "more power, at any HP and in\n"
        "any weather."),

    [ABILITY_317] = COMPOUND_STRING(
        "Has no effect in this game."),

    [ABILITY_SPICY_SPRAY] = COMPOUND_STRING(
        "When any move damages it, the\n"
        "attacker is burned, contact\n"
        "or not. Fire-types and foes\n"
        "that can't be burned are\n"
        "spared."),

    [ABILITY_BLITZ_BOXER] = COMPOUND_STRING(
        "Its punching moves, such as\n"
        "Mach Punch, Drain Punch and\n"
        "Ice Punch, get +1 priority at\n"
        "any HP. Psychic Terrain and\n"
        "Dazzling-style Abilities can\n"
        "block them."),

    [ABILITY_POWER_FISTS] = COMPOUND_STRING(
        "Its punching moves get a\n"
        "fifth more power and hit the\n"
        "target's Sp. Def instead of\n"
        "its Defense. They still use\n"
        "its Attack stat and stay\n"
        "physical."),

    [ABILITY_SAND_SONG] = COMPOUND_STRING(
        "Its sound moves, such as\n"
        "Hyper Voice and Boomburst,\n"
        "become Ground-type. They\n"
        "gain no extra power, and\n"
        "Flying-types and Levitate\n"
        "avoid them."),

    [ABILITY_PRISM_SCALES] = COMPOUND_STRING(
        "Special moves deal three\n"
        "tenths less damage to it.\n"
        "Physical moves are not\n"
        "reduced. Mold Breaker\n"
        "ignores this."),

    [ABILITY_CHLOROPLAST] = COMPOUND_STRING(
        "Its sun moves act as if the\n"
        "sun is out in any weather:\n"
        "Solar Beam and Solar Blade\n"
        "skip charging, Growth raises\n"
        "two stages, Synthesis-style\n"
        "moves heal two thirds."),

    [ABILITY_WHITEOUT] = COMPOUND_STRING(
        "In snow or hail, its Ice moves\n"
        "gain half more power. Cloud\n"
        "Nine and Air Lock switch this\n"
        "off."),

    [ABILITY_PYROMANCY] = COMPOUND_STRING(
        "Its Fire moves are five times\n"
        "as likely to burn.\n"
        "Flamethrower and Heat Wave\n"
        "burn half the time, and Lava\n"
        "Plume always burns."),

    [ABILITY_KEEN_EDGE] = COMPOUND_STRING(
        "Its slicing moves, such as\n"
        "Slash, Leaf Blade, Night Slash\n"
        "and Air Slash, get three\n"
        "tenths more power."),

    [ABILITY_RAMPAGE] = COMPOUND_STRING(
        "When a move that needs a\n"
        "recharge turn, such as Hyper\n"
        "Beam or Giga Impact, knocks\n"
        "out its target, it skips the\n"
        "recharge and can act next\n"
        "turn."),

    [ABILITY_VENGEANCE] = COMPOUND_STRING(
        "Its Ghost moves always get a\n"
        "fifth more power. When its HP\n"
        "is at a third or less, they\n"
        "gain half more instead."),
