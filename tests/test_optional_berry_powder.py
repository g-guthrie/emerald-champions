"""Keep solo purchases accessible while preserving earned multiplayer rewards."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]


def script(path):
    commands = []
    labels = {}
    for line in (ROOT / path).read_text().splitlines():
        line = line.split('@', 1)[0].strip()
        label = re.fullmatch(r'(\w+)::?', line)
        if label:
            labels[label[1]] = len(commands)
        elif line and not line.startswith(('.', '"')):
            commands.append(line)
    return commands, labels


class OptionalPowderTests(unittest.TestCase):
    def merchant(self, powder, offer=True, bag_space=True):
        commands, labels = script('data/maps/SlateportCity/scripts.inc')
        at = labels['SlateportCity_EventScript_BerryPowderClerk']
        values = {'VAR_RESULT': 0}
        bag = {}
        menus = []
        comparison = False
        for _ in range(100):
            line = commands[at]
            at += 1
            operation, _, arguments = line.partition(' ')
            args = [x.strip() for x in arguments.split(',')]
            def value(arg):
                return {'TRUE': 1, 'FALSE': 0, 'YES': 1, 'NO': 0}.get(arg, values.get(arg, int(arg) if arg.isdecimal() else arg))
            if operation == 'setvar':
                values[args[0]] = value(args[1])
            elif operation == 'copyvar':
                values[args[0]] = value(args[1])
            elif operation == 'compare':
                comparison = value(args[0]) == value(args[1])
            elif operation == 'goto_if_eq':
                if (comparison if len(args) == 1 else value(args[0]) == value(args[1])):
                    at = labels[args[-1]]
            elif operation == 'goto':
                at = labels[args[0]]
            elif operation == 'specialvar':
                self.assertEqual(args[1], 'HasEnoughBerryPowder')
                values[args[0]] = int(powder >= values['VAR_0x8004'])
            elif operation == 'msgbox':
                if len(args) > 1 and args[1] == 'MSGBOX_YESNO':
                    menus.append(args[0])
                    values['VAR_RESULT'] = int(offer) if args[0] == 'SlateportCity_Text_OfferEarnedPowder' else int(args[0] == 'SlateportCity_Text_ExchangeBerryPowderForItem')
            elif operation == 'special':
                if args[0] == 'ShowScrollableMultichoice':
                    values['VAR_RESULT'] = 0  # First exchange row: Energy Powder.
                elif args[0] == 'TakeBerryPowder':
                    powder -= values['VAR_0x8004']
                else:
                    self.assertIn(args[0], ('DisplayBerryPowderVendorMenu', 'PrintPlayerBerryPowderAmount', 'RemoveBerryPowderVendorMenu'))
            elif operation == 'case':
                if values['VAR_RESULT'] == value(args[0]):
                    at = labels[args[1]]
            elif operation == 'giveitem':
                values['VAR_RESULT'] = int(bag_space)
                if bag_space:
                    bag[value(args[0])] = bag.get(value(args[0]), 0) + 1
            elif operation == 'pokemart':
                return dict(shop=args[0], powder=powder, bag=bag, menus=menus)
            elif operation == 'end':
                return dict(shop=None, powder=powder, bag=bag, menus=menus)
            else:
                self.assertIn(operation, ('lock', 'faceplayer', 'message', 'waitmessage', 'bufferitemname', 'release', 'switch'))
        self.fail('Merchant did not terminate')

    def test_no_powder_opens_cash_shop_without_an_offer_or_jar_gate(self):
        self.assertEqual(self.merchant(0), dict(shop='SlateportCity_Pokemart_HerbalMedicine', powder=0, bag={}, menus=[]))

    def test_earned_powder_offer_can_be_declined_for_the_cash_shop(self):
        result = self.merchant(1000, offer=False)
        self.assertEqual(result['shop'], 'SlateportCity_Pokemart_HerbalMedicine')
        self.assertEqual(result['powder'], 1000)
        self.assertEqual(result['bag'], {})

    def test_powder_exchange_debits_only_after_successful_delivery(self):
        result = self.merchant(1000)
        self.assertEqual(result['powder'], 950)
        self.assertEqual(result['bag'], {'ITEM_ENERGY_POWDER': 1})
        full = self.merchant(1000, bag_space=False)
        self.assertEqual(full['powder'], 1000)
        self.assertEqual(full['bag'], {})

    def test_wireless_berry_crush_is_selectable_and_supplies_its_jar_before_link_start(self):
        text = (ROOT / 'data/scripts/cable_club.inc').read_text()
        select = text.split('CableClub_EventScript_DirectCornerSelectService::', 1)[1].split('CableClub_EventScript_DirectCornerSelectAllServices::', 1)[0]
        self.assertNotIn('ITEM_POWDER_JAR', select)
        self.assertIn('CableClub_EventScript_WirelessBerryCrush', select)
        entry = text.split('CableClub_EventScript_WirelessBerryCrush::', 1)[1].split('CableClub_EventScript_NeedBerryForBerryCrush::', 1)[0]
        self.assertLess(entry.index('HasAtLeastOneBerry'), entry.index('giveitem ITEM_POWDER_JAR'))
        self.assertLess(entry.index('giveitem ITEM_POWDER_JAR'), entry.index('LINK_GROUP_BERRY_CRUSH'))
        self.assertIn('goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull', entry)
        self.assertIn('goto_if_eq VAR_RESULT, TRUE, CableClub_EventScript_StartWirelessBerryCrush', entry)
        self.assertLess(entry.index('setflag FLAG_RECEIVED_POWDER_JAR'), entry.index('LINK_GROUP_BERRY_CRUSH'))


if __name__ == '__main__':
    unittest.main()
