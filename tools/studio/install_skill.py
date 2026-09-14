#!/usr/bin/env python3
"""Install the repository-owned Emerald Studio skill in personal Codex skills."""
import json,os,shutil
from pathlib import Path
root=Path(__file__).resolve().parents[2]
dest=Path(os.environ.get('CODEX_HOME',str(Path.home()/'.codex')))/'skills/emerald-studio'
dest.mkdir(parents=True,exist_ok=True)
for path in (root/'tools/studio/skill').rglob('*'):
    if path.is_file() and '__pycache__' not in path.parts:
        target=dest/path.relative_to(root/'tools/studio/skill');target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(path,target)
(dest/'project.json').write_text(json.dumps({'root':str(root)},indent=2)+'\n')
print(dest/'SKILL.md')
