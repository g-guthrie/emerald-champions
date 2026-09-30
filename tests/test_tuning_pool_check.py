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
            self.assertEqual(checker.main([str(good), "--milestone", "badge4", "--pools", tmp]), 0)
            self.assertEqual(checker.main([str(bad), "--milestone", "45", "--pools", tmp]), 1)


if __name__ == "__main__":
    unittest.main()
