"""scripts/tuning_pool_check.py: a tuning party manifest against a milestone pool."""
import copy
import json
from pathlib import Path
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import tuning_pool_check as checker  # noqa: E402


def species(sp, restricted=None, abilities=("ABILITY_A", "ABILITY_B")):
    return dict(species=sp, name=sp, gate="start", bst=500, stats=[80] * 6, abilities=list(abilities),
                restricted_class=restricted, source=dict(kind="wild", detail="", where="", cite=""))


def item(it, unlimited=True):
    return dict(item=it, gate="start", vendor_catalogue=unlimited, unlimited=unlimited,
                source=dict(kind="mart" if unlimited else "item_ball", detail="", where="", cite=""))


POOL = dict(
    milestone="badge4", cap=45, milestone_order=["start", "badge1", "badge2", "badge3", "badge4", "badge5"],
    game_corner_gate="badge2",
    friendship=dict(max_this_milestone=255),
    mega_ring=dict(available=True, gate="badge4"),
    all_mega_stones=["ITEM_GARCHOMPITE", "ITEM_AGGRONITE"],
    species=[species("SPECIES_GARCHOMP"), species("SPECIES_AGGRON"), species("SPECIES_ROTOM_WASH"),
             species("SPECIES_ENTEI", "legendary"), species("SPECIES_GREAT_TUSK", "paradox"),
             species("SPECIES_TREECKO"), species("SPECIES_CHIKORITA")],
    megas=[dict(species="SPECIES_GARCHOMP_MEGA", base="SPECIES_GARCHOMP", stone="ITEM_GARCHOMPITE")],
    items=[item("ITEM_LIFE_ORB"), item("ITEM_LEFTOVERS"), item("ITEM_GARCHOMPITE", unlimited=False),
           item("ITEM_ASSAULT_VEST", unlimited=False), item("ITEM_AGGRONITE", unlimited=False)],
    starter_lines={"SPECIES_TREECKO": dict(region=3, species=["SPECIES_TREECKO"], pick_only=True),
                   "SPECIES_CHIKORITA": dict(region=2, species=["SPECIES_CHIKORITA"], pick_only=True)},
)


def mon(sp, item_="ITEM_NONE", ability="ABILITY_A", **extra):
    m = dict(species=sp, nature="NATURE_JOLLY", ability=ability, item=item_,
             moves=["MOVE_A", "MOVE_B", "MOVE_C", "MOVE_D"], evs=[4, 252, 0, 0, 0, 252],
             availability="work/tuning-20260930/pools/pool-badge4.json", role="test")
    m.update(extra)
    return m


def manifest(*party):
    return dict(encounter="E0000", availability_audit="pool-badge4", party=list(party))


class TuningPoolCheckTests(unittest.TestCase):
    def check(self, *party, pool=POOL):
        return checker.check(manifest(*party), pool)

    def test_legal_party_passes(self):
        self.assertEqual(self.check(mon("SPECIES_GARCHOMP", "ITEM_GARCHOMPITE", level=45, friendship=255),
                                    mon("SPECIES_ENTEI", "ITEM_LIFE_ORB"), mon("SPECIES_ROTOM_WASH", "ITEM_LEFTOVERS"),
                                    mon("SPECIES_AGGRON", "ITEM_LIFE_ORB")), [])

    def test_species_not_in_pool_fails(self):
        problems = self.check(mon("SPECIES_DRAGAPULT"))
        self.assertTrue(any("not obtainable by badge4" in p for p in problems), problems)

    def test_mega_form_species_and_wrong_stone_fail(self):
        self.assertTrue(any("base form" in p for p in self.check(mon("SPECIES_GARCHOMP_MEGA"))))
        self.assertTrue(any("does not Mega Evolve" in p for p in self.check(mon("SPECIES_GARCHOMP", "ITEM_AGGRONITE"))))

    def test_mega_stone_needs_ring(self):
        pool = copy.deepcopy(POOL)
        pool["mega_ring"] = dict(available=False, gate="badge4")
        problems = self.check(mon("SPECIES_GARCHOMP", "ITEM_GARCHOMPITE"), pool=pool)
        self.assertTrue(any("Mega Ring" in p for p in problems), problems)

    def test_item_gate_and_duplicate_single_copy(self):
        self.assertTrue(any("ITEM_CHOICE_BAND" in p for p in self.check(mon("SPECIES_AGGRON", "ITEM_CHOICE_BAND"))))
        problems = self.check(mon("SPECIES_AGGRON", "ITEM_ASSAULT_VEST"), mon("SPECIES_GARCHOMP", "ITEM_ASSAULT_VEST"))
        self.assertTrue(any("held by 2 members" in p for p in problems), problems)
        self.assertEqual(self.check(mon("SPECIES_AGGRON", "ITEM_LIFE_ORB"), mon("SPECIES_GARCHOMP", "ITEM_LIFE_ORB")), [])

    def test_one_restricted_pokemon(self):
        problems = self.check(mon("SPECIES_ENTEI"), mon("SPECIES_GREAT_TUSK"))
        self.assertTrue(any("more than one Legendary" in p for p in problems), problems)

    def test_level_friendship_ability_and_availability(self):
        problems = self.check(mon("SPECIES_AGGRON", level=46, friendship=256, ability="ABILITY_Z", availability=""))
        joined = "\n".join(problems)
        for needle in ("level 46 must equal", "friendship 256", "not a party-menu Ability", "missing `availability`"):
            self.assertIn(needle, joined)

    def test_evs_bounds(self):
        problems = self.check(mon("SPECIES_AGGRON", evs=[252, 252, 252, 0, 0, 0]))
        self.assertTrue(any("EVs" in p for p in problems), problems)

    def test_starter_pick_before_game_corner(self):
        pool = copy.deepcopy(POOL)
        pool["milestone"] = "start"
        pool["cap"] = 14
        problems = self.check(mon("SPECIES_TREECKO"), mon("SPECIES_CHIKORITA"), pool=pool)
        self.assertTrue(any("starter pick" in p for p in problems), problems)
        self.assertEqual(self.check(mon("SPECIES_TREECKO"), mon("SPECIES_CHIKORITA")), [])

    def test_load_pool_by_name_and_cap(self):
        with tempfile.TemporaryDirectory() as tmp:
            (Path(tmp) / "pool-badge4.json").write_text(json.dumps(POOL))
            self.assertEqual(checker.load_pool("badge4", Path(tmp))["cap"], 45)
            self.assertEqual(checker.load_pool("45", Path(tmp))["milestone"], "badge4")
            with self.assertRaises(SystemExit):
                checker.load_pool("badge9", Path(tmp))

    def test_cli_reports_pass_and_fail(self):
        with tempfile.TemporaryDirectory() as tmp:
            (Path(tmp) / "pool-badge4.json").write_text(json.dumps(POOL))
            good = Path(tmp) / "good.json"
            good.write_text(json.dumps(manifest(mon("SPECIES_AGGRON", "ITEM_LIFE_ORB"))))
            bad = Path(tmp) / "bad.json"
            bad.write_text(json.dumps(manifest(mon("SPECIES_DRAGAPULT"))))
            self.assertEqual(checker.main([str(good), "--milestone", "badge4", "--pools", tmp, "--window-only"]), 0)
            self.assertEqual(checker.main([str(bad), "--milestone", "45", "--pools", tmp, "--window-only"]), 1)
            self.assertEqual(checker.main([str(good), "--milestone", "badge4", "--pools", tmp]), 1)

    def test_acquisition_ivs_and_ivy_menu_limits(self):
        pool = copy.deepcopy(POOL)
        pool["iv_service_available"] = False
        self.assertEqual(self.check(mon("SPECIES_AGGRON"), pool=pool), [])
        self.assertTrue(any("IV spread" in p for p in self.check(
            mon("SPECIES_AGGRON", ivs=[31, 0, 31, 31, 31, 0]), pool=pool)))
        pool["iv_service_available"] = True
        self.assertEqual(self.check(mon("SPECIES_AGGRON", ivs=[31, 0, 31, 31, 31, 0]), pool=pool), [])
        # Display index 3 is Sp. Attack, not the native Speed stat id.
        self.assertTrue(any("IV spread" in p for p in self.check(
            mon("SPECIES_AGGRON", ivs=[31, 0, 31, 0, 31, 31]), pool=pool)))
        # The native Fighting Hidden Power preset remains available.
        self.assertEqual(self.check(mon("SPECIES_AGGRON", ivs=[31, 0, 30, 30, 30, 30]), pool=pool), [])

    def test_encounter_game_corner_access_overrides_broad_window(self):
        pool = copy.deepcopy(POOL)
        pool["game_corner_available"] = False
        self.assertTrue(any("starter pick" in p for p in self.check(
            mon("SPECIES_TREECKO"), mon("SPECIES_CHIKORITA"), pool=pool)))

    def test_hot_spring_pokerus_needs_actual_access(self):
        pool = copy.deepcopy(POOL)
        pool["hot_spring_available"] = False
        self.assertEqual(self.check(mon("SPECIES_AGGRON", pokerus=0xFE), pool=pool), [])
        self.assertTrue(any("Pokérus state" in p for p in self.check(
            mon("SPECIES_AGGRON", pokerus=0xFB), pool=pool)))
        pool["hot_spring_available"] = True
        self.assertEqual(self.check(mon("SPECIES_AGGRON", pokerus=0xFB), pool=pool), [])


class EncounterAvailabilityTests(unittest.TestCase):
    """Exercise the actual world graph, including the reported false witness."""
    @classmethod
    def setUpClass(cls):
        import reference_pool as reference
        cls.reference = reference
        cls.builder = reference.Builder()
        cls.encounters = reference.Encounters(cls.builder)

    def pool(self, trainer, milestone):
        return self.reference.encounter_pool(trainer, milestone,
            builder=self.builder, encounters=self.encounters)

    def test_first_rival_cannot_use_post_rival_resources(self):
        pool = self.pool("TRAINER_BRENDAN_ROUTE_103_MUDKIP", "start")
        for flag in ("FLAG_DEFEATED_RIVAL_ROUTE103", "FLAG_ADVENTURE_STARTED", "HAS_OLD_ROD"):
            self.assertNotIn(flag, pool["story_flags"])
        self.assertFalse(pool["hot_spring_available"])
        species = {s["species"] for s in pool["species"]}
        self.assertIn("SPECIES_EEVEE", species)
        self.assertIn("SPECIES_PACHIRISU", species)
        self.assertNotIn("SPECIES_LUCARIO", species)
        self.assertNotIn("SPECIES_NAGANADEL", species)
        old = ROOT / "work/tuning-20260930/runs/c000/E0001/party_f0.json"
        if old.exists():
            problems = checker.check(json.loads(old.read_text()), pool, checker.species_aliases())
            self.assertTrue(any("SPECIES_LUCARIO is not obtainable" in p for p in problems), problems)
            self.assertTrue(any("SPECIES_NAGANADEL is not obtainable" in p for p in problems), problems)

    def test_cannot_check_manifest_for_a_different_trainer(self):
        pool = self.pool("TRAINER_BRENDAN_ROUTE_103_MUDKIP", "start")
        m = manifest(mon("SPECIES_PACHIRISU", ability="ABILITY_PICKUP"))
        m["encounter"] = "TRAINER_ROXANNE_1"
        self.assertTrue(any("not the checked trainer" in p for p in checker.check(m, pool)))

    def test_post_rival_resources_still_available_before_roxanne(self):
        pool = self.pool("TRAINER_ROXANNE_1", "start")
        self.assertIn("HAS_OLD_ROD", pool["story_flags"])
        self.assertIn("SPECIES_LUCARIO", {s["species"] for s in pool["species"]})
        self.assertNotIn("FLAG_BADGE01_GET", pool["story_flags"])

    def test_norman_pre_battle_mega_gift_is_available(self):
        pool = self.pool("TRAINER_NORMAN_1", "badge4")
        # Norman gives the Ring before launching his battle, not on victory.
        self.assertTrue(pool["mega_ring"]["available"])
        self.assertTrue(pool["megas"])
        self.assertNotIn("FLAG_BADGE05_GET", pool["story_flags"])

    def test_flannery_pre_battle_hot_spring_is_available(self):
        pool = self.pool("TRAINER_FLANNERY_1", "badge3")
        self.assertTrue(pool["hot_spring_available"])
        self.assertNotIn("FLAG_BADGE04_GET", pool["story_flags"])

    def test_earned_mega_rayquaza_remains_available_in_finale(self):
        pool = self.pool("TRAINER_STEVEN", "champion")
        self.assertTrue(pool["mega_ring"]["available"])
        self.assertIn("SPECIES_RAYQUAZA_MEGA", {s["species"] for s in pool["megas"]})

    def test_reject_wrong_post_victory_window(self):
        with self.assertRaisesRegex(ValueError, "cannot still be pending"):
            self.pool("TRAINER_ROXANNE_1", "badge1")


if __name__ == "__main__":
    unittest.main()
