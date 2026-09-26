#!/usr/bin/env python3
"""Start the local Studio or send a command to its live sandbox."""
import argparse
import json
import os
from pathlib import Path
import subprocess
import sys
import time
import urllib.request

ROOT = Path(__file__).resolve().parents[2]
WORK = ROOT / "work/studio"
PYTHON = ROOT / ".venv-studio/bin/python"

def request(op=None):
    config = json.loads((WORK/"server.json").read_text())
    req = urllib.request.Request(config["url"] + ("/api/command" if op else "/api/state"),
        data=json.dumps(op).encode() if op else None,
        headers={"Content-Type":"application/json", "X-Studio-Token":config["token"]})
    with urllib.request.urlopen(req, timeout=120) as response: return json.load(response)

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("action", choices=["start","status","command"], nargs="?", default="start")
    p.add_argument("json", nargs="?")
    p.add_argument("--file",type=Path,help="Read a command object from JSON instead of shell quoting.")
    args=p.parse_args()
    if args.action != "start":
        command=json.loads(args.file.read_text() if args.file else args.json) if args.action=="command" else None
        print(json.dumps(request(command), indent=2)); return
    WORK.mkdir(parents=True, exist_ok=True)
    try:
        request(); print(json.loads((WORK/"server.json").read_text())["url"]); return
    except (OSError, ValueError): pass
    if not PYTHON.exists():
        subprocess.run([sys.executable,"-m","venv",str(PYTHON.parent.parent)], check=True)
        subprocess.run([str(PYTHON),"-m","pip","install","-r",str(ROOT/"tools/studio/requirements.txt")], check=True)
    if not (ROOT/"pokeemerald-headless.gba").exists():
        cmd=["make","-j6","BUILD_NAME=emerald-headless","EC_HEADLESS_FIXTURES=1","TEST=0"]
        toolchain=Path.home()/".local/share/arm-gnu-toolchain-15.2-20260718/Payload"
        if toolchain.exists():cmd.append("DEVKITARM="+str(toolchain))
        subprocess.run(cmd+["pokeemerald-headless.gba"], cwd=ROOT, check=True)
    with (WORK/"server.log").open("ab") as log:
        child=subprocess.Popen([str(PYTHON),"-u",str(ROOT/"tools/studio/server.py")],
            cwd=ROOT, stdin=subprocess.DEVNULL, stdout=log, stderr=log, start_new_session=True)
    for _ in range(120):
        if child.poll() is not None: raise SystemExit("Studio failed to start. See "+str(WORK/"server.log"))
        try:
            request(); print(json.loads((WORK/"server.json").read_text())["url"]); return
        except (OSError,ValueError):time.sleep(.25)
    raise SystemExit("Studio is still starting. See "+str(WORK/"server.log"))

if __name__=="__main__":main()
