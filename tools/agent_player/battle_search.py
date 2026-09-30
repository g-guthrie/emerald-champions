#!/usr/bin/env python3
"""Bounded native decision search, followed by frozen observable-policy export.

Only discovery can explore multiple branches of a known seed. Exported policies
never receive that seed or a savestate; calibration clean-boots and evaluates
held-out seeds. This is a tactical baseline search, not a proof of impossibility.
"""
from __future__ import annotations

import argparse
import contextlib
import io
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import time
from types import SimpleNamespace

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))
sys.path.insert(0, str(ROOT / 'scripts/playthrough'))
import battle_calibration as calibration
import generate_battle_suite as suite_tools
from doubles_policy import TacticalPolicy


class InlineNativeDriver:
    """Reuse one loaded driver and its cached ELF tables across search nodes.

    Each branch has independent state/metadata/log files. Only read-only ROM and
    ELF bytes are hardlinked. No ROM build or boot occurs when forking a node.
    """
    def __init__(self, directory, step_timeout=900, session=None):
        import battle_driver
        self.module = battle_driver
        self.directory = Path(directory).resolve()
        self.directory.mkdir(parents=True, exist_ok=False)
        self.session = session or battle_driver.Session(self.directory)
        self.timeout = step_timeout
        self.calls = 0

    def fork(self, directory):
        child = InlineNativeDriver(directory, self.timeout)
        for filename in ('scene.gba', 'scene.elf'):
            os.link(self.directory / filename, child.directory / filename)
        for filename in ('current.ss1', 'session.json', 'events.jsonl', 'constants.json',
                         'inputs.json', 'scene.provenance.json'):
            path = self.directory / filename
            if path.exists():
                shutil.copy2(path, child.directory / filename)
        child.session = self.module.Session(child.directory)
        child.session._syms = self.session._syms
        child.session._sizes = self.session._sizes
        child.session.constants = self.session.constants
        return child

    def call(self, command, *arguments):
        defaults = {'run_dir': str(self.directory), 'png': False,
                    'battle_kind': 'trainer', 'trainer': None, 'party': None,
                    'trainer2': None, 'partner': None, 'map': None, 'weather': 'map',
                    'scenario': None, 'baseline': None, 'build_dir': None}
        if command == 'act':
            defaults['commands'] = list(arguments)
        else:
            iterator = iter(arguments)
            for flag in iterator:
                if flag == '--png':
                    defaults['png'] = True
                    continue
                if not flag.startswith('--'):
                    raise calibration.DriverFailure('invalid inline driver argument')
                value = next(iterator)
                key = flag[2:].replace('-', '_')
                defaults[key] = int(value, 0) if key in {'seed', 'cap', 'baseline'} else value
        args = SimpleNamespace(**defaults)
        output, diagnostic = io.StringIO(), io.StringIO()
        original_session = self.module.Session
        original_run = self.module.ui.run
        deadline = time.monotonic() + self.timeout
        def session_factory(path):
            if Path(path).resolve() != self.directory:
                raise calibration.DriverFailure('inline driver attempted a different session')
            return self.session
        def bounded_run(argv, *, timeout=120):
            remaining = deadline - time.monotonic()
            if remaining <= 0:
                raise subprocess.TimeoutExpired(argv, self.timeout)
            return original_run(argv, timeout=min(timeout, remaining))
        self.module.Session = session_factory
        self.module.ui.run = bounded_run
        self.calls += 1
        error = None
        try:
            with contextlib.redirect_stdout(output), contextlib.redirect_stderr(diagnostic):
                function = getattr(self.module, 'command_' + command)
                function(args)
        except (SystemExit, RuntimeError) as failure:
            error = calibration.DriverFailure(str(failure))
        except subprocess.TimeoutExpired as failure:
            error = failure
        finally:
            self.module.Session = original_session
            self.module.ui.run = original_run
            calibration.save(self.directory / f'inline-call-{self.calls:04d}.json', {
                'command': command, 'arguments': arguments,
                'stdout': output.getvalue(), 'stderr': diagnostic.getvalue(),
                'error': str(error) if error else None,
            })
        if error:
            raise error
        try:
            return json.loads(output.getvalue())
        except ValueError as failure:
            raise calibration.DriverFailure('inline native driver returned non-JSON output') from failure


def board_score(state):
    if state.get('phase') == 'ended':
        return 100000 if state.get('outcome') == 'won' else -100000
    player_health = sum(mon.get('hp', 0) / max(mon.get('max_hp', 1), 1)
                        for mon in state.get('player_reserves', []))
    foe_health = sum(mon.get('hp', 0) / max(mon.get('max_hp', 1), 1)
                     for mon in state.get('actives', []) if mon.get('side') == 'opponent' and mon.get('alive'))
    return state.get('opponent_faints', 0) * 100 - state.get('player_faints', 0) * 115 \
        + player_health * 14 - foe_health * 14


def frozen_policy(trace):
    return {'schema_version': 1, 'kind': 'tactical', 'overrides': [
        {'when': entry['observation'], 'commands': entry['commands']} for entry in trace],
        'scope': 'Observable overrides found by bounded native discovery; tactical baseline fallback. '
                 'Fresh native replay and held-out evaluation remain required.'}


def beam_search(driver, initial, policy, directory, *, beam_width=4, actions_per_node=4,
                max_nodes=80, max_depth=16, timeout_seconds=3600):
    """All limits are independent; exhausted search remains unresolved."""
    values = (beam_width, actions_per_node, max_nodes, max_depth, timeout_seconds)
    if any(type(value) is not int or value <= 0 for value in values):
        raise ValueError('native search budgets must be positive integers')
    directory = Path(directory)
    directory.mkdir(exist_ok=True)
    deadline = time.monotonic() + timeout_seconds
    beam = [{'driver': driver, 'state': initial, 'policy': policy, 'trace': [], 'score': board_score(initial)}]
    nodes, failures, solution = 0, [], None
    for depth in range(max_depth):
        candidates = []
        for parent in beam:
            if parent['state'].get('phase') == 'ended':
                if parent['state'].get('outcome') == 'won':
                    solution = parent
                    break
                continue
            observation = calibration.public_observation(parent['state'])
            ranked = parent['policy'].ranked(observation, actions_per_node)
            for action in ranked:
                if nodes >= max_nodes or time.monotonic() >= deadline:
                    break
                nodes += 1
                child = parent['driver'].fork(directory / f'node-{nodes:05d}')
                try:
                    event = child.call('act', *action['commands'])
                    state = child.call('state')
                    trace = parent['trace'] + [{'observation': observation, 'commands': action['commands'], 'event': event}]
                    node = {'driver': child, 'state': state, 'policy': parent['policy'].clone(),
                            'trace': trace, 'score': board_score(state)}
                    if state.get('phase') == 'ended' and state.get('outcome') == 'won':
                        solution = node
                        break
                    if state.get('phase') in {'await_action', 'await_switch'}:
                        candidates.append(node)
                except (calibration.DriverFailure, subprocess.TimeoutExpired) as error:
                    failures.append({'node': nodes, 'reason': str(error)})
            if solution or nodes >= max_nodes or time.monotonic() >= deadline:
                break
        if solution or nodes >= max_nodes or time.monotonic() >= deadline:
            break
        candidates.sort(key=lambda node: (-node['score'], [entry['commands'] for entry in node['trace']]))
        beam = candidates[:beam_width]
        if not beam:
            break
    result = {'schema_version': 1, 'kind': 'bounded_native_battle_discovery',
              'category': 'candidate_win' if solution else 'unresolved', 'expanded_nodes': nodes,
              'failures': failures, 'verified_legal_winning_witness': False,
              'limitations': ['Branch exploration can exploit the discovery seed; the frozen policy must '
                             'clean-boot replay and evaluate held-out seeds.',
                             'The tactical baseline and finite beam do not prove a fight impossible.']}
    if solution:
        result['solution_session'] = str(solution['driver'].directory)
        result['decisions'] = len(solution['trace'])
        calibration.save(directory / 'discovered-trace.json', solution['trace'])
        calibration.save(directory / 'discovered-policy.json', frozen_policy(solution['trace']))
    calibration.save(directory / 'search-result.json', result)
    return result


def search(config_path, run_dir):
    config_path, run_dir = Path(config_path).resolve(), Path(run_dir).resolve()
    config = calibration.load(config_path)
    if config.get('schema_version') != 1:
        raise ValueError('search config requires schema 1')
    discovery, heldout = calibration.validate_seeds(config)
    if len(discovery) != 1:
        raise ValueError('bounded search takes exactly one discovery seed; evaluation seeds remain held out')
    resolve = lambda name: (config_path.parent / config[name]).resolve()
    suite = calibration.load(resolve('suite'))
    party = calibration.load(resolve('party'))
    puzzle = calibration.validate_puzzle(suite, config['puzzle_id'], party)
    run_dir.mkdir(parents=True, exist_ok=False)
    build = run_dir / 'build'
    provenance = calibration.snapshot_build(resolve('build_dir'), build)
    for filename, value in (('config.json', config), ('puzzle.json', puzzle), ('party.json', party),
                            ('scenario.json', puzzle['scenario'])):
        calibration.save(run_dir / filename, value)
    identities = {str(path.relative_to(run_dir)): suite_tools.digest_file(path)
                  for path in [*build.iterdir(), *(run_dir / name for name in
                      ('config.json', 'puzzle.json', 'party.json', 'scenario.json'))] if path.is_file()}
    driver = InlineNativeDriver(run_dir / 'initial')
    scenario = puzzle['scenario']
    state = driver.call('start', '--trainer', scenario['trainer_id'], '--difficulty', scenario['difficulty'],
                        '--cap', str(scenario['level_cap']), '--seed', str(discovery[0]),
                        '--scenario', str(run_dir / 'scenario.json'), '--party', str(run_dir / 'party.json'),
                        '--build-dir', str(build))
    result = beam_search(driver, state, TacticalPolicy(), run_dir / 'search', **config.get('search_budgets', {}))
    result['source_fingerprint'] = suite['source_fingerprint']
    result['discovery_seed'] = discovery[0]
    result['held_out_seeds'] = heldout
    result['build_provenance'] = provenance
    if result['category'] == 'candidate_win':
        policy_data = calibration.load(run_dir / 'search/discovered-policy.json')
        replay = calibration.run_seed(InlineNativeDriver(run_dir / 'discovery-replay'), scenario,
            run_dir / 'party.json', run_dir / 'scenario.json', build, TacticalPolicy(policy_data), discovery[0],
            config.get('budgets', {}).get('max_decisions', 300),
            config.get('budgets', {}).get('timeout_seconds', 3600))
        expected = calibration.trace_digest(calibration.load(run_dir / 'search/discovered-trace.json'))
        result['replay'] = replay
        result['verified_legal_winning_witness'] = replay['category'] == 'won' and replay['evidence_complete'] \
            and replay['battle_field_exact'] and replay['opponent_identity_verified'] and replay['trace_sha256'] == expected
    result['input_sha256'] = identities
    result['inputs_unchanged'] = all(suite_tools.digest_file(run_dir / name) == digest for name, digest in identities.items())
    result['source_current'] = suite_tools.source_fingerprint() == suite['source_fingerprint']
    if not result['source_current'] or not result['inputs_unchanged']:
        result['category'] = 'invalid'
        result['invalid_reason'] = 'source or immutable inputs changed during discovery'
        result['verified_legal_winning_witness'] = False
    calibration.save(run_dir / 'summary.json', result)
    if result['category'] == 'candidate_win':
        # Make the next required step directly executable with frozen inputs.
        calibration.save(run_dir / 'suite.json', {'schema_version': 2,
            'source_fingerprint': suite['source_fingerprint'], 'puzzles': [puzzle]})
        calibration.save(run_dir / 'evaluate-discovery.json', {
            'schema_version': 1, 'suite': 'suite.json', 'puzzle_id': puzzle['puzzle_id'],
            'party': 'party.json', 'policy': 'search/discovered-policy.json', 'build_dir': 'build',
            'discovery_seeds': discovery, 'evaluation_seeds': heldout,
            'budgets': config.get('budgets', {})})
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--config', type=Path, required=True)
    parser.add_argument('--run-dir', type=Path, required=True)
    args = parser.parse_args()
    try:
        result = search(args.config, args.run_dir)
    except (ValueError, OSError, KeyError, calibration.DriverFailure) as error:
        parser.exit(1, f'native discovery refused: {error}\n')
    print(json.dumps(result, indent=2))
    return 0 if result['category'] == 'candidate_win' else 1


if __name__ == '__main__':
    raise SystemExit(main())
