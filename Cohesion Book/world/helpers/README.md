# Book authoring aids

These scripts only read the frozen `baseline/source` and write this world volume (plus its named review summary). They are not game implementation scripts or production test gates.

The shipped reading artifacts are `README.md`, regional chapters, all per-map pages, exact changes, ledgers, coverage and validation. Large temporary script indexes/digests are deliberately omitted; regenerate them only when revising this volume.

Rebuild order, from this directory or using each absolute path:

1. `python3 inventory_world.py`
2. `python3 author_world_proposals.py`
3. `python3 author_world_systems.py`
4. `python3 author_world_supplement.py`
5. `python3 build_world_book.py`
6. `python3 author_regional_chapters.py`
7. `python3 validate_world_book.py`

`compact_review.py`, `semantic_digest.py`, and `unique_review.py` produce optional review digests after inventory generation. They do not decide correctness; the authoring files and explicit common contracts contain the reviewed decisions.

The validator checks book completeness, reference bindings, proposed text/menu targets and specific actor tiles. Its assertion total is not a count of gameplay tests and must never be used as a release-readiness claim.
