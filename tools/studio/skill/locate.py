#!/usr/bin/env python3
"""Locate the installed Studio checkout without scanning unrelated projects."""
import json
from pathlib import Path
here=Path(__file__).resolve().parent
config=here/'project.json'
if config.exists():
    root=Path(json.loads(config.read_text())['root'])
else:
    root=here.parents[2]
if not (root/'tools/studio/cli.py').exists():
    raise SystemExit('The configured checkout moved. Use the project supplied by the user and reinstall its Studio skill.')
receipt=root/'work/studio/server.json'
result={'root':str(root),'start':'python3 tools/studio/cli.py start'}
if receipt.exists():result['url']=json.loads(receipt.read_text())['url']
print(json.dumps(result,indent=2))
