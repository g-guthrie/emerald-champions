"""Conservative dynamic map evidence follows source mutations and flags unknowns."""
import importlib.util
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location('map_dynamic_inventory', ROOT / 'scripts/audit/map_dynamic_inventory.py')
D = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(D)


class DynamicInventoryTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.constants = D.read_constants(ROOT)
        cls.inventory = D.build_inventory(ROOT)

    def test_current_source_owners_and_absolute_ranges(self):
        inventory = self.inventory
        self.assertEqual(inventory['invalid_animation_count'], 0)
        self.assertIn([508, 512], inventory['animation_ranges_by_tileset']['gTileset_General'])
        self.assertIn([608, 612], inventory['animation_ranges_by_tileset']['gTileset_Mauville'])
        self.assertTrue(inventory['c_metatile_writes'])
        self.assertTrue(all(row['function'] and row['line'] > 0 for row in inventory['c_metatile_writes']))
        self.assertEqual(inventory['unresolved_counts']['c_map_associations'], len(inventory['c_metatile_writes']))

    def mutate_animation(self, before, after):
        original = Path.read_text
        def read(path, *args, **kwargs):
            text = original(path, *args, **kwargs)
            return text.replace(before, after) if path.name == 'tileset_anims.c' else text
        with patch.object(Path, 'read_text', read):
            return D.animation_inventory(ROOT, self.constants)

    def test_out_of_vram_mutation_is_invalid_and_not_allowed(self):
        writes, ranges = self.mutate_animation('TILE_OFFSET_4BPP(508)', 'TILE_OFFSET_4BPP(1023)')
        bad = [row for row in writes if row['status'] == 'invalid']
        self.assertTrue(bad)
        self.assertNotIn([1023, 1027], ranges['gTileset_General'])

    def test_unknown_expression_is_not_a_resolved_allowance(self):
        writes, ranges = self.mutate_animation('TILE_OFFSET_4BPP(508)', 'TILE_OFFSET_4BPP(dynamicTile)')
        self.assertTrue(any(row['status'] == 'unresolved' for row in writes))
        self.assertNotIn([508, 512], ranges['gTileset_General'])

    def test_table_mutation_changes_possible_range(self):
        _, ranges = self.mutate_animation('TILE_OFFSET_4BPP(NUM_TILES_IN_PRIMARY + 96)', 'TILE_OFFSET_4BPP(NUM_TILES_IN_PRIMARY + 97)')
        self.assertIn([609, 613], ranges['gTileset_Mauville'])
        self.assertNotIn([608, 612], ranges['gTileset_Mauville'])

    def test_callback_mutation_removes_owner(self):
        writes, ranges = self.mutate_animation('sPrimaryTilesetAnimCallback = TilesetAnim_General;', 'sPrimaryTilesetAnimCallback = UnknownCallback;')
        self.assertEqual(ranges['gTileset_General'], [])
        self.assertTrue(any(row['function'] == 'QueueAnimTiles_General_Flower' and row['status'] == 'unresolved' for row in writes))

    def test_missing_inputs_fail(self):
        with tempfile.TemporaryDirectory() as directory:
            with self.assertRaises(FileNotFoundError):
                D.build_inventory(Path(directory))

    def test_nested_call_arguments_and_source_lines(self):
        text = D.strip_comments('// MapGridSetMetatileIdAt(0, 0, 0);\nvoid Test(void)\n{\n MapGridSetMetatileIdAt(3 + MAP_OFFSET, f(1, 2), METATILE_X);\n}\n')
        name, offset, operands = next(D.calls(text, ['MapGridSetMetatileIdAt']))
        row = D.record_at(text, offset, 'fixture.c', D.functions(text), operands)
        self.assertEqual(row['line'], 4)
        self.assertEqual(row['function'], 'Test')
        self.assertEqual(operands, ['3 + MAP_OFFSET', 'f(1, 2)', 'METATILE_X'])
        self.assertEqual(D.numeric(operands[0], self.constants), 10)
        self.assertIsNone(D.numeric(operands[1], self.constants))

    def test_c_literals_do_not_change_comments_braces_or_calls(self):
        text = D.strip_comments(r'''void Safe(void)
{
    const char *s = "// not a comment } MapGridSetMetatileIdAt(9,9,9)";
    char brace = '}';
    MapGridSetMetatileEntryAt(1, 2, 3);
    MapGridSetMetatileImpassabilityAt(1, 2, 1);
}
''')
        spans = D.functions(text)
        self.assertEqual(set(spans), {'Safe'})
        rows = list(D.calls(text, ['MapGridSetMetatileIdAt', 'MapGridSetMetatileEntryAt', 'MapGridSetMetatileImpassabilityAt']))
        self.assertEqual(len(rows), 2)
        for _, offset, operands in rows:
            self.assertEqual(D.record_at(text, offset, 'test.c', spans, operands)['function'], 'Safe')
        self.assertEqual(D.arguments('"a,b", fn(1, 2), 3'), ['"a,b"', 'fn(1, 2)', '3'])

    def test_unknown_owner_call_is_retained_and_public_setters_inventoried(self):
        original = Path.read_text
        def read(path, *args, **kwargs):
            text = original(path, *args, **kwargs)
            if path.name == 'fieldmap.c':
                text += '\nCUSTOM_FUNCTION(Unknown)\n{\n MapGridSetMetatileEntryAt(1, 2, 3);\n}\n'
            return text
        with patch.object(Path, 'read_text', read):
            inventory = D.build_inventory(ROOT)
        self.assertTrue(any(row['function'] is None and row['api'] == 'MapGridSetMetatileEntryAt' for row in inventory['c_metatile_writes']))
        self.assertIn('MapGridSetMetatileImpassabilityAt', {row['api'] for row in inventory['c_metatile_writes']})

    def test_transfer_inventory_and_categories_remain_unproven(self):
        inventory = self.inventory
        self.assertTrue(inventory['memory_transfers'])
        self.assertTrue(inventory['direct_vram_references'])
        self.assertEqual(inventory['unresolved_counts']['memory_transfer_ranges'], len(inventory['memory_transfers']))
        self.assertTrue(all(row['map_association'] == 'unresolved' for row in inventory['memory_transfers']))
        self.assertTrue(any(row['category'] == 'fixed_coordinate' for row in inventory['c_metatile_writes']))
        self.assertTrue(any(row['category'] == 'generic_loader' for row in inventory['dynamic_loaders']))
        self.assertTrue(any(row['category'] == 'explicit_vram_destination' for row in inventory['memory_transfers']))

    def test_no_arbitrary_expression_execution(self):
        self.assertIsNone(D.numeric('__import__("os").system("false")', self.constants))
        self.assertIsNone(D.numeric('1 / 0', self.constants))


class ScriptContextsTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        (self.root / 'data/maps/WrongFile').mkdir(parents=True)
        (self.root / 'data/scripts').mkdir(parents=True)
        (self.root / 'data/maps/WrongFile/scripts.inc').write_text('''Actual_MapScripts::
    map_script MAP_SCRIPT_ON_LOAD, Actual_Load
    .byte 0
Actual_Load::
    call Shared
    goto Actual_Exit
WrongUnreachable::
    setmetatile 999, 999, 0, 0
    end
Actual_Exit::
    end
''')
        self.shared = self.root / 'data/scripts/shared.inc'
        self.shared.write_text('''Shared::
    setvar VAR_TEST, 1
AliasFallthrough::
    call_if_eq VAR_TEST, 1, SharedWrite
    return
SharedWrite::
    setmetatile 1, 1, 0, 0
    return
''')
        self.maps = {'Actual': {}}

    def derive(self):
        diagnostics = {}
        contexts = D.derive_script_contexts(self.root, self.maps, diagnostics=diagnostics)
        return contexts, diagnostics

    def test_wrong_file_and_shared_fallthrough_follow_callers(self):
        contexts, _ = self.derive()
        self.assertEqual(contexts['SharedWrite'], ['Actual'])
        self.assertEqual(contexts['AliasFallthrough'], ['Actual'])
        self.assertEqual(contexts['WrongUnreachable'], [])

    def test_text_references_are_not_control_edges(self):
        self.shared.write_text('''Shared::
    msgbox TextOnly
    end
TextOnly::
    .string "goto SharedWrite"
SharedWrite::
    setmetatile 1, 1, 0, 0
    return
''')
        contexts, _ = self.derive()
        self.assertEqual(contexts['SharedWrite'], [])
        self.assertEqual(contexts['TextOnly'], [])

    def test_dynamic_edges_are_explicit(self):
        self.shared.write_text('''Shared::
    call VAR_0x8000
    callnative NativeCallback
    return
''')
        contexts, diagnostics = self.derive()
        self.assertEqual(contexts['Shared'], ['Actual'])
        self.assertEqual(len(diagnostics['unresolved_edges']), 2)
        self.assertEqual(diagnostics['unresolved_edges'][0]['target'], 'VAR_0x8000')

    def test_switch_and_multiple_roots_are_possible_contexts(self):
        self.maps['Other'] = {'shared_scripts_map': 'Actual'}
        self.shared.write_text('''Shared::
    switch VAR_TEST
    case 0, Branch
    end
Branch::
    return
''')
        contexts, diagnostics = self.derive()
        self.assertEqual(contexts['Branch'], ['Actual', 'Other'])
        self.assertIn('Branch', diagnostics['ambiguous_labels'])

    def test_build_conditionals_do_not_cross_version_dispatch(self):
        self.shared.write_text('''Shared::
#if IS_FRLG
    call FrlgOnly
#else
    call EmeraldOnly
#endif
    end
FrlgOnly::
    end
EmeraldOnly::
.if !IS_FRLG
    call EmeraldNested
.else
    call FrlgOnly
.endif
    end
EmeraldNested::
    end
''')
        contexts, diagnostics = self.derive()
        self.assertEqual(contexts['FrlgOnly'], [])
        self.assertEqual(contexts['EmeraldNested'], ['Actual'])
        self.assertEqual(diagnostics['unresolved_build_conditions'], [])
        frlg = D.derive_script_contexts(self.root, self.maps, is_frlg=True)
        self.assertEqual(frlg['FrlgOnly'], ['Actual'])
        self.assertEqual(frlg['EmeraldOnly'], [])

    def test_unknown_build_condition_is_reported(self):
        self.shared.write_text('Shared::\n#if UNKNOWN\n call Missing\n#endif\n return\n')
        _, diagnostics = self.derive()
        self.assertEqual(diagnostics['unresolved_build_conditions'][0]['condition'], 'UNKNOWN')

    def test_common_assembly_roots_and_null_scripts(self):
        self.maps['Actual']['object_events'] = [{'script': 'Common'}, {'script': '0x0'}]
        (self.root / 'data/event_scripts.s').write_text('Common::\n call SharedWrite\n return\n')
        contexts, diagnostics = self.derive()
        self.assertEqual(contexts['Common'], ['Actual'])
        self.assertEqual(diagnostics['missing_roots'], [])

    def test_object_coord_and_bg_roots(self):
        self.maps = {'Actual': {'object_events': [{'script': 'Object'}],
                    'coord_events': [{'script': 'Coord'}], 'bg_events': [{'script': 'Sign'}]}}
        self.shared.write_text('Object::\n return\nCoord::\n return\nSign::\n return\n')
        contexts, _ = self.derive()
        for label in ('Object', 'Coord', 'Sign'):
            self.assertEqual(contexts[label], ['Actual'])


if __name__ == '__main__':
    unittest.main()
