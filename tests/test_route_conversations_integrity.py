"""Negative cases for conversation conversion, independent of corpus size."""
import importlib.util
import json
from pathlib import Path
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("route_conversations", ROOT / "scripts/verify_route_conversations.py")
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


class RouteConversationsTests(unittest.TestCase):
    def test_live_conversations(self):
        self.assertEqual(module.verify(ROOT), [])

    def test_bad_conversions_fail(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            original = dict(script="Route102_EventScript_Local", trainer_type="TRAINER_TYPE_NORMAL",
                            trainer_sight_or_berry_tree_id="3", x=5, y=6)
            files = {
                "data/emerald_champions/route_conversations.json": json.dumps({"retired": [dict(
                    trainer="TRAINER_LOCAL", map="Route102", object_ids=[1], original_objects=[original],
                    scripts=[original["script"]], retained_species=["SPECIES_PIKACHU"])]}),
                "data/emerald_champions/emerald_champions_master_battle_design.txt":
                    "--- BRANCH TRAINER_RETAINED ---\n  1. SPECIES_PIKACHU @ ITEM_NONE\n",
                "src/data/trainers.party": "=== TRAINER_RETAINED ===\n",
                "data/maps/Route102/map.json": json.dumps({"object_events": [{**original,
                    "trainer_type": "TRAINER_TYPE_NONE", "trainer_sight_or_berry_tree_id": "0"}]}),
                "data/maps/Route102/scripts.inc":
                    'Route102_EventScript_Local::\n\tmsgbox LocalText, MSGBOX_NPC\n\tend\n\n'
                    'LocalText:\n\t.string "Hello!$"\n',
            }
            for name, content in files.items():
                path=root/name; path.parent.mkdir(parents=True, exist_ok=True); path.write_text(content)
            self.assertEqual(module.verify(root), [])
            for name, change, expected in [
                ("data/maps/Route102/map.json", lambda s:s.replace('TRAINER_TYPE_NONE','TRAINER_TYPE_NORMAL'), 'sight battles'),
                ("data/maps/Route102/map.json", lambda s:s.replace('"x": 5','"x": 9'), 'identity'),
                ("data/maps/Route102/scripts.inc", lambda s:s.replace('msgbox LocalText, MSGBOX_NPC','trainerbattle_single TRAINER_LOCAL, Intro, Defeat'), 'invoked'),
                ("data/maps/Route102/scripts.inc", lambda s:s.replace('Hello!$','Hello!'), 'unterminated'),
                ("data/emerald_champions/emerald_champions_master_battle_design.txt", lambda s:s.replace('SPECIES_PIKACHU','SPECIES_EEVEE'), 'species lost'),
            ]:
                with self.subTest(expected=expected):
                    (root/name).write_text(change(files[name]))
                    self.assertTrue(any(expected in error for error in module.verify(root)))
                    (root/name).write_text(files[name])


if __name__ == "__main__":
    unittest.main()
