"""Execute Rydel's production scripts with controlled key-pocket capacity.

This checks script control flow, not native menus or the bag implementation.
"""
import re
import unittest
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PREFIX = 'MauvilleCity_BikeShop_EventScript_'
RECEIVED = 'FLAG_RECEIVED_BIKE'
DECLINED = 'FLAG_DECLINED_BIKE'
MACH, ACRO = 'ITEM_MACH_BIKE', 'ITEM_ACRO_BIKE'


def run_rydel(state, answer, choice=0):
    source = (ROOT / 'data/maps/MauvilleCity_BikeShop/scripts.inc').read_text()
    common = (ROOT / 'data/event_scripts.s').read_text()
    source += '\n' + re.search(
        r'Common_EventScript_ShowBagIsFull::\n.*?\n\tend', common, re.S
    )[0]
    commands, labels = [], {}
    for line in source.splitlines():
        line = line.split('@')[0].strip()
        label = re.fullmatch(r'(\w+)::?', line)
        if label:
            labels[label[1]] = len(commands)
        elif line:
            commands.append(line)
    pc, result, locked, messages = labels[PREFIX + 'Rydel'], 0, False, []
    values = {'YES': 1, 'NO': 0, 'TRUE': 1, 'FALSE': 0}
    for _ in range(150):
        command, _, tail = commands[pc].partition(' ')
        pc += 1
        args = [part.strip() for part in tail.split(',')]
        if command == 'end':
            assert not locked, 'Rydel left field controls locked'
            return messages
        if command == 'lock':
            locked = True
        elif command == 'release':
            locked = False
        elif command in ('faceplayer', 'waitmessage', 'switch'):
            pass
        elif command in ('msgbox', 'message'):
            messages.append(args[0])
            if command == 'msgbox' and args[-1] == 'MSGBOX_YESNO':
                result = int(answer)
        elif command == 'multichoice':
            assert args == ['21', '8', 'MULTI_BIKE', 'TRUE']
            result = choice
        elif command == 'case':
            if result == int(args[0]):
                pc = labels[args[1]]
        elif command == 'setflag':
            state['flags'].add(args[0])
        elif command == 'goto_if_set':
            if args[0] in state['flags']:
                pc = labels[args[1]]
        elif command == 'goto_if_eq':
            assert args[0] == 'VAR_RESULT'
            if result == values[args[1]]:
                pc = labels[args[2]]
        elif command == 'goto':
            pc = labels[args[0]]
        elif command == 'giveitem':
            result = int(state['space'] > 0)
            if result:
                state['bag'][args[0]] += 1
                state['space'] -= 1
        elif command == 'removeitem':
            assert state['bag'][args[0]] == 1
            state['bag'][args[0]] -= 1
            state['space'] += 1
        elif command == 'checkitem':
            result = int(state['bag'][args[0]] > 0)
        elif command == 'special':
            assert args == ['SwapRegisteredBike']
            state['registration_updates'] += 1
        elif command == 'incrementgamestat':
            assert args == ['GAME_STAT_TRADED_BIKES']
            state['swaps'] += 1
        else:
            raise AssertionError('unsupported reachable command: ' + command)
    raise AssertionError('Rydel script did not terminate')


def fresh(space=1, bag=None, pc=None, received=False):
    return {'flags': {RECEIVED} if received else set(),
            'bag': Counter(bag or {}), 'pc': Counter(pc or {}), 'space': space,
            'swaps': 0, 'registration_updates': 0}


class RydelGiftIntegrity(unittest.TestCase):
    def test_first_refusal_then_grant(self):
        state = fresh()
        run_rydel(state, False)
        self.assertEqual(state['flags'], {DECLINED})
        self.assertEqual(sum(state['bag'].values()), 0)
        run_rydel(state, True)
        self.assertIn(RECEIVED, state['flags'])
        self.assertEqual(state['bag'][MACH], 1)

    def test_both_first_grants(self):
        for choice, bike in enumerate((MACH, ACRO)):
            with self.subTest(bike=bike):
                state = fresh()
                run_rydel(state, True, choice)
                self.assertEqual(state['bag'], Counter({bike: 1}))
                self.assertEqual(state['flags'], {RECEIVED})
                self.assertEqual(state['registration_updates'], 1)

    def test_full_pocket_does_not_claim_delivery_and_can_retry(self):
        for choice, bike in enumerate((MACH, ACRO)):
            for declined_first in (False, True):
                with self.subTest(bike=bike, declined_first=declined_first):
                    state = fresh(space=0)
                    if declined_first:
                        run_rydel(state, False)
                    messages = run_rydel(state, True, choice)
                    self.assertIn('gText_TooBadBagIsFull', messages)
                    self.assertNotIn(RECEIVED, state['flags'])
                    self.assertEqual(sum(state['bag'].values()), 0)
                    self.assertEqual(state['registration_updates'], 0)
                    state['space'] = 1
                    run_rydel(state, True, choice)
                    self.assertEqual(state['bag'], Counter({bike: 1}))
                    self.assertIn(RECEIVED, state['flags'])

    def test_existing_bikes_keep_or_exchange(self):
        for old, new in ((MACH, ACRO), (ACRO, MACH)):
            with self.subTest(old=old):
                state = fresh(space=0, bag={old: 1}, received=True)
                run_rydel(state, False)
                self.assertEqual(state['bag'][old], 1)
                self.assertEqual(state['swaps'], 0)
                run_rydel(state, True)
                self.assertEqual(state['bag'][old], 0)
                self.assertEqual(state['bag'][new], 1)
                self.assertEqual(state['swaps'], 1)
                self.assertEqual(state['registration_updates'], 1)

    def test_existing_pc_bike_is_not_replaced_or_removed(self):
        for bike in (MACH, ACRO):
            with self.subTest(bike=bike):
                state = fresh(pc={bike: 1}, received=True)
                messages = run_rydel(state, True)
                self.assertIn('MauvilleCity_BikeShop_Text_OhYourBikeIsInPC', messages)
                self.assertEqual(state['pc'], Counter({bike: 1}))
                self.assertEqual(sum(state['bag'].values()), 0)
                self.assertEqual(state['flags'], {RECEIVED})
                self.assertEqual(state['registration_updates'], 0)


if __name__ == '__main__':
    unittest.main()
