import json
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))

import retune_recheck  # noqa: E402


class RetuneRecheckTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        for name, cap, species in (("02-TRAINER_CALVIN_1", 14, "SPECIES_PACHIRISU"),
                                   ("03-TRAINER_RICK", 14, "SPECIES_MIMIKYU_DISGUISED"),
                                   ("22-TRAINER_DAWSON", 20, "SPECIES_MARILL")):
            directory = self.root / name
            directory.mkdir()
            (directory / "party.json").write_text(json.dumps({"party": [{"species": species}]}))
            (directory / "receipt.json").write_text(json.dumps(
                {"trainer": name.split("-", 1)[1], "cap": cap, "d_star": 2}))
        (self.root / "01-TRAINER_MAY_ROUTE_103_MUDKIP").mkdir()  # in progress: no receipt yet

    def tearDown(self):
        self.tmp.cleanup()

    def test_only_parties_using_a_moved_resource_need_a_rerun(self):
        def checker(trainer, cap, party):
            return ["Mimikyu is not legal here"] if party["party"][0]["species"] == "SPECIES_MIMIKYU_DISGUISED" else []

        rows = retune_recheck.recheck(retune_recheck.receipts(self.root), checker)
        self.assertEqual([(r["battle"], r["status"]) for r in rows],
                         [("02-TRAINER_CALVIN_1", "stands"), ("03-TRAINER_RICK", "rerun"),
                          ("22-TRAINER_DAWSON", "stands")])

    def test_cap_filter_and_missing_party(self):
        (self.root / "02-TRAINER_CALVIN_1" / "party.json").unlink()
        rows = retune_recheck.recheck(retune_recheck.receipts(self.root, cap=14), lambda *a: [])
        self.assertEqual([(r["battle"], r["status"]) for r in rows],
                         [("02-TRAINER_CALVIN_1", "no party"), ("03-TRAINER_RICK", "stands")])


if __name__ == "__main__":
    unittest.main()
