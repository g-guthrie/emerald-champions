#!/usr/bin/env python3
"""Keyless unit checks for the agent-player state and observation contract."""

from __future__ import annotations

import importlib.util
import json
import tempfile
import unittest
from pathlib import Path


MODULE_PATH = Path(__file__).with_name("agent_player.py")
SPEC = importlib.util.spec_from_file_location("agent_player", MODULE_PATH)
assert SPEC and SPEC.loader
agent_player = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(agent_player)


class ContractTests(unittest.TestCase):
    def test_atomic_json_and_jsonl(self) -> None:
        with tempfile.TemporaryDirectory() as raw:
            root = Path(raw)
            agent_player.atomic_json(root / "state.json", {"b": 2, "a": 1})
            self.assertEqual(json.loads((root / "state.json").read_text()), {"a": 1, "b": 2})
            agent_player.append_jsonl(root / "events.jsonl", {"kind": "step"})
            self.assertEqual(json.loads((root / "events.jsonl").read_text()), {"kind": "step"})

    def test_buttons_are_only_gba_inputs_plus_wait(self) -> None:
        self.assertEqual(agent_player.BUTTONS, {"A", "B", "START", "SELECT", "UP", "DOWN", "LEFT", "RIGHT", "L", "R", "WAIT"})

    def test_metric_roles(self) -> None:
        session = object.__new__(agent_player.Session)
        session.probes = [
            {"name": "battle", "role": "battle_counter"},
            {"name": "dead", "role": "death_active"},
            {"name": "map", "role": "progress_value"},
        ]
        meta = {"metrics": {"attempts": 1, "deaths": 0, "battles": 0, "progress_events": 0}, "last_probe_values": {"battle": 2, "dead": 0, "map": 5}}
        session._update_metrics(meta, {"battle": 4, "dead": 1, "map": 6})
        self.assertEqual(meta["metrics"], {"attempts": 1, "deaths": 1, "battles": 2, "progress_events": 1})


if __name__ == "__main__":
    unittest.main()
