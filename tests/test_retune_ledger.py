import json
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))

import retune_ledger  # noqa: E402


class RetuneLedgerUsageTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.saved = retune_ledger.RETUNE
        retune_ledger.RETUNE = Path(self.tmp.name)
        for name, cap, species in (("02-TRAINER_CALVIN_1", 14, ["SPECIES_PACHIRISU", "SPECIES_MARILL"]),
                                   ("03-TRAINER_RICK", 14, ["SPECIES_PACHIRISU", "SPECIES_LITTEN"]),
                                   ("22-TRAINER_DAWSON", 20, ["SPECIES_MARILL"])):
            directory = retune_ledger.RETUNE / name
            directory.mkdir()
            (directory / "party.json").write_text(json.dumps({"party": [{"species": s} for s in species]}))
            (directory / "receipt.json").write_text(json.dumps({"trainer": name.split("-", 1)[1], "cap": cap}))
        # Benchmarking in progress: a party but no receipt, so it is not counted.
        busy = retune_ledger.RETUNE / "04-TRAINER_ALLEN"
        busy.mkdir()
        (busy / "party.json").write_text(json.dumps({"party": [{"species": "SPECIES_PACHIRISU"}]}))

    def tearDown(self):
        retune_ledger.RETUNE = self.saved
        self.tmp.cleanup()

    def test_tally_counts_finished_parties_per_cap(self):
        tally = retune_ledger.usage()
        self.assertEqual(tally[14], (2, [("SPECIES_PACHIRISU", 2), ("SPECIES_LITTEN", 1), ("SPECIES_MARILL", 1)]))
        self.assertEqual(tally[20], (1, [("SPECIES_MARILL", 1)]))
        self.assertEqual(list(retune_ledger.usage(20)), [20])

    def test_render_lists_the_tally(self):
        text = "\n".join(retune_ledger.render_usage(retune_ledger.usage(14)))
        self.assertIn("- Cap 14 (2 battles): Pachirisu 2/2, Litten 1/2, Marill 1/2", text)


if __name__ == "__main__":
    unittest.main()
