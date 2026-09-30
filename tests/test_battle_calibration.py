"""Battle evidence must preserve exact authoring and fail closed on legal proof."""
import json
from pathlib import Path
import sys
import tempfile
import types
import runpy
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools/agent_player'))
sys.path.insert(0, str(ROOT / 'scripts/playthrough'))
import generate_battle_suite as suite
import battle_calibration as calibration


class FakeDriver:
    def __init__(self, directory, outcome='won', complete=True, fail=False):
        self.directory = directory
        self.directory.mkdir()
        (self.directory / 'session.json').write_text(json.dumps({'map_field': {
            'map': 'Route103', 'weather': 'WEATHER_NONE', 'environment': 'BATTLE_ENVIRONMENT_PLAIN',
            'weather_basis': 'map header'}, 'opponent_identity': {'status': 'verified'}}))
        self.outcome, self.complete, self.fail = outcome, complete, fail
        self.phase = 'await_action'
        self.calls = []

    def event(self, kind):
        with (self.directory / 'events.jsonl').open('a') as stream:
            stream.write(json.dumps({'event': kind, 'log_complete': self.complete}) + '\n')

    def call(self, command, *args):
        self.calls.append((command, args))
        if command == 'start':
            self.event('start')
            return {'phase': self.phase, 'turn': 0, 'seed': 123,
                    'actives': [{'side': 'opponent', 'choosing': {'move_index': 2}}]}
        if command == 'act':
            if self.fail:
                raise calibration.DriverFailure('illegal action')
            self.event('act')
            self.phase = 'ended'
            return {'phase': 'ended', 'turn_after': 1, 'moves': [], 'log_complete': self.complete}
        if command == 'state':
            return {'phase': self.phase, 'turn': 1}
        if command == 'result':
            return {'phase': 'ended', 'outcome': self.outcome, 'turns': 1}
        raise AssertionError(command)


class CalibrationTests(unittest.TestCase):
    def run_fake(self, temp, policy=None, **kwargs):
        driver = FakeDriver(Path(temp) / 'run', **kwargs)
        policy = calibration.ScriptedPolicy(policy or {
            'schema_version': 1, 'kind': 'scripted',
            'decisions': [{'commands': ['0:move0@1'], 'expected': {'turn': 0}}],
        })
        result = calibration.run_seed(driver, {'trainer_id': 'TRAINER_TEST', 'difficulty': 'hard',
            'level_cap': 14}, Path('party.json'), Path('scenario.json'), Path('build'), policy, 123)
        return result, driver

    def test_exact_authored_members_including_suffix_attributes(self):
        opponents = suite.opponent_catalogue()
        branches = suite.teams.read_teams()
        self.assertEqual([opponent['trainer_id'] for opponent in opponents],
                         [branch.trainer for branch in branches])
        self.assertGreater(len(opponents), 300)
        for opponent, branch in zip(opponents, branches):
            self.assertEqual(len(opponent['team']), len(branch.mons))
            for member, mon in zip(opponent['team'], branch.mons):
                self.assertEqual(member['ivs'], list(map(int, mon.ivs.split('/'))))
                self.assertEqual(member['friendship'], mon.friendship)
                self.assertEqual(member['moves'], ['MOVE_' + value for value in mon.moves])
                self.assertEqual(member['level_offset'], mon.offset)
                self.assertNotIn('level', member)  # historical strict_cap cannot invent a live level

    def test_source_identity_covers_mechanics_and_actual_progression(self):
        paths = {str(path.relative_to(ROOT)) for path in suite.build_inputs.build_inputs()}
        for expected in ('src/caps.c', 'src/difficulty.c', 'src/battle_ai_main.c',
                         'src/data/pokemon/species_info.h', 'src/data/wild_encounters.json',
                         'data/maps/Route103/scripts.inc', 'src/data/trainers.party'):
            self.assertIn(expected, paths)
        with patch.object(suite, 'input_hashes', return_value={'native_build_inputs': 'first'}):
            first = suite.source_fingerprint()
        with patch.object(suite, 'input_hashes', return_value={'native_build_inputs': 'changed-mechanics'}):
            self.assertNotEqual(first, suite.source_fingerprint())

    def test_zero_scenarios_remains_catalogue_not_fake_legal_coverage(self):
        with patch.object(suite, 'input_hashes', return_value={'native': 'test'}):
            catalogue = suite.generate()
        self.assertEqual(catalogue['puzzles'], [])
        self.assertEqual(len(catalogue['unbound_trainers']), catalogue['opponent_count'])

    def test_stale_duplicate_and_ungraded_arsenal_inputs_refused(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / 'index.json'
            scenario = {'trainer_id': 'TRAINER_TEST', 'scenario_id': 'opening', 'difficulty': 'hard',
                        'legality_status': 'unresolved'}
            data = {'schema_version': 2, 'source_generated': True, 'source_fingerprint': 'current',
                    'scenarios': [scenario]}
            path.write_text(json.dumps(data))
            self.assertEqual(suite.load_arsenals(path, 'current'), [scenario])
            with self.assertRaises(ValueError):
                suite.load_arsenals(path, 'other-source')
            data['scenarios'].append(scenario)
            path.write_text(json.dumps(data))
            with self.assertRaises(ValueError):
                suite.load_arsenals(path, 'current')
            data['scenarios'] = [{key: value for key, value in scenario.items() if key != 'legality_status'}]
            path.write_text(json.dumps(data))
            with self.assertRaises(ValueError):
                suite.load_arsenals(path, 'current')

    def test_no_command_or_rng_or_seed_leak_to_policy(self):
        state = {'phase': 'await_action', 'seed': 3, 'rng': [1, 2], 'selection_state': [4],
                 'actives': [{'side': 'opponent', 'choosing': {'target': 2}, 'hp': 30}]}
        observed = calibration.public_observation(state)
        self.assertEqual(observed, {'phase': 'await_action', 'actives': [{'side': 'opponent', 'hp': 30}]})
        self.assertIn('choosing', state['actives'][0])

    def test_evaluation_seeds_are_unique_and_held_out(self):
        self.assertEqual(calibration.validate_seeds({'discovery_seeds': [0], 'evaluation_seeds': [1, 2]}), ([0], [1, 2]))
        for data in ({'evaluation_seeds': []}, {'discovery_seeds': [1], 'evaluation_seeds': [1]},
                     {'evaluation_seeds': [1, 1]}, {'evaluation_seeds': [True]},
                     {'evaluation_seeds': [2**32]}):
            with self.assertRaises(ValueError):
                calibration.validate_seeds(data)

    def test_complete_win_is_candidate_until_replayed(self):
        with tempfile.TemporaryDirectory() as temp:
            result, driver = self.run_fake(temp)
            self.assertEqual(result['category'], 'won')
            self.assertTrue(result['evidence_complete'])
            self.assertTrue(result['battle_field_exact'])
            self.assertFalse(result['witness_verified'])
            self.assertEqual([call[0] for call in driver.calls], ['start', 'act', 'state', 'result'])
            self.assertTrue((driver.directory / 'policy-trace.json').exists())

    def test_incomplete_logs_never_complete_win_evidence(self):
        with tempfile.TemporaryDirectory() as temp:
            result, _ = self.run_fake(temp, complete=False)
            self.assertEqual(result['category'], 'won')
            self.assertFalse(result['evidence_complete'])

    def test_approximated_weather_is_not_exact_field_evidence(self):
        with tempfile.TemporaryDirectory() as temp:
            driver = FakeDriver(Path(temp) / 'run')
            (driver.directory / 'session.json').write_text(json.dumps({'map_field': {
                'map': 'Route119', 'weather': 'WEATHER_RAIN', 'weather_cycle': ['WEATHER_RAIN', 'WEATHER_NONE']}}))
            policy = calibration.ScriptedPolicy({'schema_version': 1, 'kind': 'scripted',
                'decisions': [{'commands': ['0:move0@1']}]})
            result = calibration.run_seed(driver, {'trainer_id': 'TRAINER_TEST', 'difficulty': 'hard',
                'level_cap': 14}, Path('party'), Path('scenario'), Path('build'), policy, 1)
            self.assertEqual(result['category'], 'won')
            self.assertFalse(result['battle_field_exact'])

    def test_loss_invalid_and_unsupported_outcome_stay_distinct(self):
        for kwargs, expected in (({'outcome': 'lost'}, 'lost'), ({'fail': True}, 'invalid'),
                                 ({'outcome': 'ran'}, 'unresolved')):
            with tempfile.TemporaryDirectory() as temp:
                result, _ = self.run_fake(temp, **kwargs)
                self.assertEqual(result['category'], expected)
                if expected != 'lost':
                    self.assertFalse(result['evidence_complete'])
                    self.assertFalse(result['witness_verified'])

    def test_timeout_preserves_partial_logs_without_complete_experiment(self):
        class TimeoutDriver(FakeDriver):
            def call(self, command, *arguments):
                if command == 'act':
                    import subprocess
                    raise subprocess.TimeoutExpired('native', 1)
                return super().call(command, *arguments)
        with tempfile.TemporaryDirectory() as temp:
            driver = TimeoutDriver(Path(temp) / 'run')
            policy = calibration.ScriptedPolicy({'schema_version': 1, 'kind': 'scripted',
                'decisions': [{'commands': ['0:move0@1']}]})
            result = calibration.run_seed(driver, {'trainer_id': 'TRAINER_TEST', 'difficulty': 'hard',
                'level_cap': 14}, Path('party'), Path('scenario'), Path('build'), policy, 1)
            self.assertEqual(result['category'], 'timeout')
            self.assertTrue(result['partial_logs_complete'])
            self.assertFalse(result['evidence_complete'])
            self.assertFalse(result['witness_verified'])

    def test_process_timeout_is_logged_and_terminates_the_native_child_group(self):
        import subprocess
        process = types.SimpleNamespace(pid=234, returncode=-9)
        process.communicate = unittest.mock.Mock(side_effect=[subprocess.TimeoutExpired('driver', 1),
                                                               ('partial stdout', 'native stderr')])
        with tempfile.TemporaryDirectory() as temp, \
             patch.object(calibration.subprocess, 'Popen', return_value=process) as popen, \
             patch.object(calibration.os, 'killpg') as kill:
            driver = calibration.NativeDriver(Path(temp) / 'run', 1)
            with self.assertRaises(subprocess.TimeoutExpired):
                driver.call('state')
            kill.assert_called_once_with(234, calibration.signal.SIGKILL)
            self.assertTrue(popen.call_args.kwargs['start_new_session'])
            saved = json.loads((driver.directory / 'driver-call-0001.json').read_text())
            self.assertTrue(saved['timed_out'])
            self.assertEqual(saved['stderr'], 'native stderr')
            self.assertEqual(saved['stdout'], 'partial stdout')

    def test_policy_guard_mismatch_is_unresolved_without_submitting_commands(self):
        with tempfile.TemporaryDirectory() as temp:
            result, driver = self.run_fake(temp, policy={'schema_version': 1, 'kind': 'scripted',
                'decisions': [{'commands': ['0:move0@1'], 'expected': {'turn': 99}}]})
            self.assertEqual(result['category'], 'unresolved')
            self.assertTrue(result['partial_logs_complete'])
            self.assertFalse(result['evidence_complete'])
            self.assertEqual([call[0] for call in driver.calls], ['start'])

    def test_cli_module_boundary_catches_tactical_unresolved_exception(self):
        from doubles_policy import TacticalPolicy, PolicyUnresolved
        # The CLI is executed under a distinct module identity. Its imported
        # policy exception must remain the same class as the policy's raiser.
        namespace = runpy.run_path(str(ROOT / 'tools/agent_player/battle_calibration.py'),
                                   run_name='calibration_cli_probe')
        self.assertIs(namespace['PolicyUnresolved'], PolicyUnresolved)
        with tempfile.TemporaryDirectory() as temp:
            driver = FakeDriver(Path(temp) / 'run')
            policy = TacticalPolicy(metadata={'moves': {}, 'types': {}, 'speeds': {}, 'chart': {}})
            result = namespace['run_seed'](driver, {'trainer_id': 'TRAINER_TEST', 'difficulty': 'hard',
                'level_cap': 14}, Path('party'), Path('scenario'), Path('build'), policy, 1)
            self.assertEqual(result['category'], 'unresolved')
            self.assertFalse(result['evidence_complete'])
            saved = json.loads((driver.directory / 'evaluation.json').read_text())
            self.assertEqual(saved['category'], 'unresolved')

    def test_actual_cli_entry_continues_after_unresolved_tactical_seed(self):
        # Execute the actual __main__ entry point with only its external native
        # process/build/certificate boundaries mocked. An empty legal-decision
        # board must produce two unresolved records, rather than crash the CLI.
        with tempfile.TemporaryDirectory() as temp:
            directory = Path(temp)
            build = directory / 'build'
            build.mkdir()
            for name in ('pokeemerald-headless.gba', 'pokeemerald-headless.elf'):
                (build / name).write_bytes(b'fixture artifact')
            (build / 'pokeemerald-headless.inputs.json').write_text('{}')
            puzzle = {'puzzle_id': 'test', 'scenario': {'trainer_id': 'TRAINER_TEST',
                'difficulty': 'hard', 'level_cap': 14, 'legality_status': 'proven', 'expected_opponent': {'team': []}}}
            puzzle['content_sha256'] = suite.digest_bytes(suite.canonical(puzzle))
            for name, data in {
                'suite.json': {'schema_version': 2, 'source_fingerprint': 'current', 'puzzles': [puzzle]},
                'party.json': {}, 'policy.json': {'schema_version': 1, 'kind': 'tactical'},
                'config.json': {'schema_version': 1, 'suite': 'suite.json', 'puzzle_id': 'test',
                    'party': 'party.json', 'policy': 'policy.json', 'build_dir': 'build',
                    'discovery_seeds': [0], 'evaluation_seeds': [1, 2]},
            }.items():
                (directory / name).write_text(json.dumps(data))
            process = types.SimpleNamespace(returncode=0)
            process.communicate = unittest.mock.Mock(return_value=(json.dumps({
                'phase': 'await_action', 'turn': 0, 'actives': [], 'pending_decision': []}), ''))
            fake_arsenal = types.SimpleNamespace(validate_party=lambda scenario, party: None)
            with patch.object(sys, 'argv', ['battle_calibration.py', '--config', str(directory / 'config.json'),
                                           '--run-dir', str(directory / 'evaluation')]), \
                 patch.object(suite, 'source_fingerprint', return_value='current'), \
                 patch.object(calibration.stamp_release_inputs, 'digest_tree', return_value=('source', 1)), \
                 patch.object(calibration.stamp_release_inputs, 'verify_stamp'), \
                 patch.object(calibration.build_provenance, 'verify', return_value={'status': 'verified'}), \
                 patch.object(calibration.build_provenance, 'carry', return_value={'status': 'verified'}), \
                 patch.object(calibration.subprocess, 'Popen', return_value=process), \
                 patch('prepare_party.protocol', return_value=({}, [])), \
                 patch('doubles_policy.source_metadata', return_value={'moves': {}, 'types': {}, 'speeds': {}, 'chart': {}}), \
                 patch.dict(sys.modules, {'battle_arsenal': fake_arsenal}):
                with self.assertRaises(SystemExit) as exited:
                    runpy.run_path(str(ROOT / 'tools/agent_player/battle_calibration.py'), run_name='__main__')
                self.assertEqual(exited.exception.code, 0)
            summary = json.loads((directory / 'evaluation/summary.json').read_text())
            self.assertEqual(summary['categories'], {'unresolved': 2})
            self.assertEqual(summary['verified_legal_winning_witnesses'], 0)
            for seed in (1, 2):
                self.assertTrue((directory / f'evaluation/seed-{seed:08x}/evaluation.json').exists())

    def test_malformed_preparation_metadata_is_refused_before_snapshot_or_boot(self):
        # Keep the independent source certificate boundary permissive here;
        # the real preparation encoder must catch transport-invalid metadata.
        puzzle = {'puzzle_id': 'test', 'scenario': {'trainer_id': 'TRAINER_TEST',
            'difficulty': 'hard', 'level_cap': 14, 'legality_status': 'proven',
            'expected_opponent': {'team': []}}}
        puzzle['content_sha256'] = suite.digest_bytes(suite.canonical(puzzle))
        manifest = {'encounter': 'test', 'availability_audit': {'legality_status': 'proven'},
                    'party': [{'species': 'SPECIES_PIKACHU', 'ability': 'ABILITY_STATIC',
                        'item': 'ITEM_NONE', 'nature': 'NATURE_TIMID', 'evs': [252, 52, 52, 52, 52, 50],
                        'moves': ['MOVE_THUNDERBOLT', 'MOVE_QUICK_ATTACK', 'MOVE_PROTECT', 'MOVE_TACKLE'],
                        'availability': 'source fixture', 'role': 'preflight fixture'}]}
        catalogue = {'schema_version': 2, 'source_fingerprint': 'current', 'puzzles': [puzzle]}
        for missing in ('encounter', 'availability_audit'):
            with self.subTest(missing=missing), tempfile.TemporaryDirectory() as temp:
                directory = Path(temp)
                malformed = dict(manifest); del malformed[missing]
                fixtures = {'party.json': malformed,
                    'suite.json': catalogue,
                    'policy.json': {'schema_version': 1, 'kind': 'scripted',
                                    'decisions': [{'commands': ['0:move0@1']}]},
                    'config.json': {'schema_version': 1, 'suite': 'suite.json', 'puzzle_id': 'test',
                        'party': 'party.json', 'policy': 'policy.json', 'build_dir': 'build',
                        'evaluation_seeds': [1]}}
                for name, data in fixtures.items():
                    (directory / name).write_text(json.dumps(data))
                fake_arsenal = types.SimpleNamespace(validate_party=lambda scenario, party: None)
                with patch.object(suite, 'source_fingerprint', return_value='current'), \
                     patch.dict(sys.modules, {'battle_arsenal': fake_arsenal}), \
                     patch.object(calibration, 'snapshot_build') as snapshot, \
                     patch.object(calibration, 'NativeDriver') as native:
                    with self.assertRaisesRegex(ValueError, 'encounter and reachable-pool audit'):
                        calibration.evaluate(directory / 'config.json', directory / 'evaluation')
                    snapshot.assert_not_called()
                    native.assert_not_called()
                self.assertFalse((directory / 'evaluation').exists())
        # The actual encoder must also accept the same complete preparation;
        # a preflight that rejects every manifest would not satisfy this gate.
        fake_arsenal = types.SimpleNamespace(validate_party=lambda scenario, party: None)
        with patch.object(suite, 'source_fingerprint', return_value='current'), \
             patch.dict(sys.modules, {'battle_arsenal': fake_arsenal}):
            self.assertIs(calibration.validate_puzzle(catalogue, 'test', manifest), puzzle)

    def test_rescue_factory_certificate_skips_prepared_party_protocol(self):
        import battle_scripted_wild_arsenal as wild
        scenario = wild.rescue_scenario(internal_fingerprint='current')
        puzzle = {'puzzle_id': 'rescue', 'scenario': scenario}
        puzzle['content_sha256'] = suite.digest_bytes(suite.canonical(puzzle))
        catalogue = {'schema_version': 2, 'source_fingerprint': 'current', 'puzzles': [puzzle]}
        with patch.object(suite, 'source_fingerprint', return_value='current'), \
             patch('prepare_party.protocol', side_effect=AssertionError('rescue must use native factory')) as protocol:
            self.assertIs(calibration.validate_puzzle(catalogue, 'rescue', None), puzzle)
            protocol.assert_not_called()

    def test_legal_status_requires_joint_roster_certificate_validator(self):
        puzzle = {'puzzle_id': 'test', 'scenario': {'legality_status': 'unresolved'}}
        puzzle['content_sha256'] = suite.digest_bytes(suite.canonical(puzzle))
        data = {'schema_version': 2, 'source_fingerprint': 'current', 'puzzles': [puzzle]}
        with patch.object(suite, 'source_fingerprint', return_value='current'):
            with self.assertRaisesRegex(ValueError, 'legality is unresolved'):
                calibration.validate_puzzle(data, 'test', {})
            puzzle['scenario']['legality_status'] = 'proven'
            puzzle['scenario']['expected_opponent'] = {'team': []}
            puzzle['content_sha256'] = suite.digest_bytes(suite.canonical({k: v for k, v in puzzle.items() if k != 'content_sha256'}))
            def refuse(scenario, party):
                raise ValueError('one stone cannot equip two members')
            with patch.dict(sys.modules, {'battle_arsenal': types.SimpleNamespace(validate_party=refuse)}):
                with self.assertRaisesRegex(ValueError, 'one stone'):
                    calibration.validate_puzzle(data, 'test', {})


class ScenarioTransportTests(unittest.TestCase):
    def test_explicit_flags_do_not_infer_earlier_badges(self):
        import battle_driver
        class Session:
            syms = {'gEcStudioArgs': 100, 'gEcStudioCommand': 200}
            def run_at_frames(self, **kwargs):
                self.writes = kwargs['writes']
            def read_view(self, advance=False):
                words = [0] * 24
                words[22] = 30
                return words, 0, True
        session = Session()
        ids = {'FLAG_BADGE01_GET': 1, 'FLAG_BADGE02_GET': 2, 'VAR_ROUTE110_STATE': 3}
        with patch.object(battle_driver, 'campaign_milestones', return_value=[('FLAG_BADGE01_GET', 20), ('FLAG_BADGE02_GET', 30)]), \
             patch.object(battle_driver, 'resolve_defines', side_effect=lambda names: {name: ids[name] for _, name in names}):
            flags = battle_driver.apply_scenario(session, {
                'progression_flags': {'FLAG_BADGE02_GET': True},
                'progression_vars': {'VAR_ROUTE110_STATE': 7}}, 30)
        self.assertEqual(flags, {'FLAG_BADGE01_GET': False, 'FLAG_BADGE02_GET': True})
        self.assertIn((0, 104, 0), session.writes)
        self.assertIn((4, 104, 1), session.writes)
        self.assertIn((8, 100, 3), session.writes)
        self.assertIn((8, 104, 7), session.writes)

    def test_bad_flag_value_is_refused_before_native_write(self):
        import battle_driver
        with self.assertRaises(SystemExit):
            battle_driver.apply_scenario(None, {'progression_flags': {'FLAG_BADGE01_GET': 1}}, 20)


if __name__ == '__main__':
    unittest.main()
