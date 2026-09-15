"""Regression checks for misleading generated reference output, not gameplay tests."""
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'scripts'))
import sync_game_book as book
import export_trainer_catalogue as export


class BookReferenceTests(unittest.TestCase):
    def test_invalid_markers_do_not_discard_guide_or_accept_half_written_reference(self):
        for text in [book.END, 'guide\n'+book.BEGIN+'\npartial',book.BEGIN+'\n'+book.END+'\nuser notes']:
            with self.subTest(text=text),self.assertRaises(ValueError):book.split_guide(text)
        self.assertEqual(book.split_guide('guide\n\n'+book.BEGIN+'\nold\n'+book.END),'guide\n\n')

    def test_refresh_preserves_guide_edits_made_during_extraction(self):
        with tempfile.TemporaryDirectory() as d:
            target=Path(d)/'book.txt'
            old='Old guide\n\n'+book.BEGIN+'\nold reference\n'+book.END+'\n'
            target.write_text(old)
            reference=book.BEGIN+'\nnew reference\n'+book.END+'\n'
            def extraction():
                target.write_text(old.replace('Old guide','Concurrent user guide'))
                return reference,{}
            with patch.object(book,'BOOK',target),patch.object(book,'watch_snapshot',return_value={}),patch.object(book,'render_reference',side_effect=extraction),patch.object(sys,'argv',['sync_game_book.py','--write']):
                book.main()
            self.assertTrue(target.read_text().startswith('Concurrent user guide'))
            self.assertIn('new reference',target.read_text())

    def test_source_change_during_extraction_leaves_previous_book_intact(self):
        with tempfile.TemporaryDirectory() as d:
            target=Path(d)/'book.txt';target.write_text('Existing guide\n')
            with patch.object(book,'BOOK',target),patch.object(book,'watch_snapshot',side_effect=[{'src/x.c':1},{'src/x.c':2}]),patch.object(book,'render_reference',return_value=('unused',{})),patch.object(sys,'argv',['sync_game_book.py','--write']):
                with self.assertRaisesRegex(SystemExit,'Source changed'):
                    book.main()
            self.assertEqual(target.read_text(),'Existing guide\n')

    def test_watcher_verifies_already_materialized_team_edits_instead_of_blocking(self):
        from types import SimpleNamespace
        team=str(book.teams.TEAMS.relative_to(book.ROOT))
        old={team:1,'src/data/trainers.party':1,'src/data/emerald_champions_battle_plans.h':1}
        new={team:2,'src/data/trainers.party':2,'src/data/emerald_champions_battle_plans.h':2}
        snapshots=[old,old,old,new,new,new,KeyboardInterrupt()]
        with patch.object(book,'watch_snapshot',side_effect=snapshots),patch.object(book.time,'sleep'),patch.object(book.subprocess,'run',return_value=SimpleNamespace(stdout='PASS')) as runner:
            with self.assertRaises(KeyboardInterrupt):book.watch()
        commands=[call.args[0] for call in runner.call_args_list]
        team_commands=[command for command in commands if command[1].endswith('emerald_champions_teams.py')]
        self.assertEqual(len(team_commands),1)
        self.assertEqual(team_commands[0][-1],'--check')
        self.assertEqual(sum(command[1].endswith('sync_game_book.py') for command in commands),2)

    def test_native_comparison_rejects_a_move_changed_only_in_projection(self):
        branches=book.teams.read_teams();parties=export.parse_parties()
        original=branches[0].mons[0].moves[0]
        parties[branches[0].trainer]['mons'][0]['moves'][0]='MOVE_SPLASH' if original!='SPLASH' else 'MOVE_TACKLE'
        with self.assertRaises(AssertionError):export.native_vs_authoring(parties,branches)

    def test_dialogue_preserves_alternatives_placeholders_shared_text_and_native_strings(self):
        with tempfile.TemporaryDirectory() as d:
            root=Path(d)
            for path in ['data/maps/Town','data/text','data/scripts','src/data']:(root/path).mkdir(parents=True)
            map_path=root/'data/maps/Town/map.json'
            m={'id':'MAP_TOWN','object_events':[{'script':'Town_Npc','x':1,'y':2,'flag':'0'}], 'coord_events':[], 'bg_events':[]}
            map_path.write_text(json.dumps(m))
            (map_path.parent/'scripts.inc').write_text('Town_Npc::\n\tgoto_if_set FLAG_DONE, Town_After\n\tmsgbox Shared_Text\n\tend\nTown_After::\n\tmsgbox Town_Text\n\tend\nTown_Text:\n\t.string "After {PLAYER}!$"\n')
            (root/'data/text/shared.inc').write_text('Shared_Text:\n\t.string "Hello, \\\"friend\\\".\\n"\n\t.string "Bring {STR_VAR_1}.$"\n')
            (root/'src/service.c').write_text('const u8 sMsg[] = _("Native choice.$");\nvoid f(void){Show(COMPOUND_STRING("Your " "reward."));}\n')
            (root/'src/data/service.h').write_text('const u8 sHeader[] = _("Header dialogue.$");\n')
            with patch.object(book,'ROOT',root):lines,paths,counts,routes=book.dialogue_reference([(map_path,m)])
            text='\n'.join(lines)
            self.assertIn('Bring {STR_VAR_1}.',text);self.assertIn('After {PLAYER}!',text)
            self.assertEqual(text.count('Hello,'),1)
            self.assertIn('Native choice.',text);self.assertIn('Your reward.',text);self.assertIn('Header dialogue.',text)
            self.assertEqual(counts['script_literal_blocks'],2)
            self.assertTrue(any('FLAG_DONE' in action for actions in routes.values() for action in actions))
            self.assertNotIn('goto_if_set',text.split('DIALOGUE BRANCH/CALL INDEX')[0])
            self.assertIn('FLAG_DONE',text.split('DIALOGUE BRANCH/CALL INDEX')[1])

    def test_source_pickup_inventory_excludes_trainers_and_other_regions(self):
        with tempfile.TemporaryDirectory() as d:
            root=Path(d);(root/'data/scripts').mkdir(parents=True)
            for name,region in [('Hoenn','REGION_HOENN'),('Other','REGION_KANTO')]:
                p=root/'data/maps'/name;p.mkdir(parents=True)
                (p/'map.json').write_text(json.dumps({'region':region,'id':'MAP_'+name,'object_events':[
                    {'script':'Trainer','trainer_sight_or_berry_tree_id':'ITEM_POTION','x':1,'y':1},
                    {'script':'Common_EventScript_FindItem','trainer_sight_or_berry_tree_id':'ITEM_ETHER','x':2,'y':2}], 'bg_events':[]}))
            with patch.object(book,'ROOT',root):maps,pickups,*_=book.source_inventory()
            self.assertEqual(len(maps),1);self.assertEqual(len(pickups),1);self.assertEqual(pickups[0]['item'],'ITEM_ETHER')

if __name__=='__main__':unittest.main()
