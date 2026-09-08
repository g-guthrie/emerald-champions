"""Reviewed-move generation keeps only retained rows and derives their count."""
import json
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'scripts'))
import generate_emerald_champions_battle_sets as generator


class ReviewCountIntegrity(unittest.TestCase):
    def test_retained_rows_and_count(self):
        review = {'reviewed_assignment_count': 3, 'assignments': [
            {'species': 'SPECIES_FIRST', 'move': 'MOVE_FIRST', 'action': 'retain_official'},
            {'species': 'SPECIES_REPLACED', 'move': 'MOVE_REPLACED', 'action': 'replace'},
            {'species': 'SPECIES_SECOND', 'move': 'MOVE_SECOND', 'action': 'retain_official_champions'},
        ]}
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            fixture = root / 'review.json'
            fixture.write_text(json.dumps(review))
            with patch.object(generator, 'MOVE_ACCESS_REVIEW', fixture):
                header = generator.render_move_access_review_c()
                self.assertIn('#define EC_REVIEWED_MOVE_ACCESS_COUNT 2\n', header)
                self.assertNotIn('SPECIES_REPLACED', header)
                self.assertLess(header.index('SPECIES_FIRST'), header.index('SPECIES_SECOND'))
                review['reviewed_assignment_count'] = 4
                fixture.write_text(json.dumps(review))
                with self.assertRaises(AssertionError):
                    generator.render_move_access_review_c()


if __name__ == '__main__':
    unittest.main()
