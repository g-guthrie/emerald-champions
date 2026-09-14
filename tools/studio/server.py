#!/usr/bin/env python3
"""Emerald Studio: local native mGBA, browser pixels/audio, explicit game commands."""
import argparse
import asyncio
import contextlib
import hashlib
import io
import json
import os
from pathlib import Path
import re
import secrets
import shutil
import struct
import subprocess
import sys
import time
import zipfile
from collections import deque
from datetime import datetime, timezone

from aiohttp import web, WSMsgType
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
WORK = ROOT / "work/studio"
BUILD_STORE = WORK / "builds"
BOOK = ROOT / "Game Blueprint/Emerald_Champions_Game_Book.txt"
sys.path.insert(0, str(ROOT / "scripts"))
from native_tools import build_runner, symbols
from rom_artifacts import verify_rom_elf_pair
from stamp_release_inputs import digest_tree
from scenes import Recorder,TextDecoder,packet_state,keys as button_mask,compare as compare_scenes
from library import Library

FPS = 16777216 / 280896
U32 = struct.Struct("<I")
def sha(path): return hashlib.sha256(path.read_bytes()).hexdigest()
def words(*values): return struct.pack("<" + "I" * len(values), *values)
def title(name): return name.replace("SPECIES_", "").replace("_", " ").title()

class Catalogue:
    def __init__(self):
        groups = json.loads((ROOT / "data/maps/map_groups.json").read_text())
        self.maps = {}
        for g, group in enumerate(groups["group_order"]):
            for n, name in enumerate(groups[group]):
                m = json.loads((ROOT / "data/maps" / name / "map.json").read_text())
                if m["region"] == "REGION_HOENN":
                    self.maps[name] = dict(m, group=g, num=n)
        self.layouts = {x["id"]: x for x in json.loads((ROOT / "data/layouts/layouts.json").read_text())["layouts"]}
        self.map_ids = {(m["group"], m["num"]): name for name, m in self.maps.items()}
        self.constants = self.load_constants()
        self.species = {v: title(k) for k, v in self.constants.items() if k.startswith("SPECIES_") and 0 < v < 3000}
        self.scan_scripts()
        self.map_images = {}

    def load_constants(self):
        headers = ["species", "moves", "items", "abilities", "pokemon", "flags", "vars", "emerald_champions", "opponents"]
        names = set()
        for h in headers:
            text = (ROOT / f"include/constants/{h}.h").read_text()
            names |= set(re.findall(r"(?m)^#define\s+((?:SPECIES|MOVE|ITEM|ABILITY|NATURE|FLAG|VAR|EC_OPENING|TRAINER)_[A-Z0-9_]+)\s", text))
            names |= set(re.findall(r"(?m)^\s+((?:SPECIES|MOVE|ITEM|ABILITY|NATURE)_[A-Z0-9_]+)\s*(?:=|,)", text))
        source = '#include <stdio.h>\n' + "".join(f'#include "constants/{h}.h"\n' for h in headers)
        source += "int main(void) {\n" + "".join(f'printf("{n} %u\\n", (unsigned){n});\n' for n in sorted(names)) + "}\n"
        exe = WORK / "constants"
        subprocess.run(["cc", "-I", str(ROOT / "include"), "-x", "c", "-", "-o", str(exe)], input=source, text=True, check=True, capture_output=True)
        return {k: int(v) for k, v in (line.split() for line in subprocess.check_output([str(exe)], text=True).splitlines())}

    def scan_scripts(self):
        self.trainers = {}
        for block in re.split(r"(?m)^=== ", (ROOT/"src/data/trainers.party").read_text())[1:]:
            ident = block.split(" ===",1)[0]
            party = re.findall(r"(?m)^SPECIES_(\w+) @ ITEM_(\w+)", block)
            if not party or ident not in self.constants: continue
            name = re.search(r"(?m)^Name: (.+)", block)
            self.trainers[ident] = dict(id=ident, name=name[1].title() if name else ident,
                team=[dict(species=title(s), item=title(i)) for s,i in party])
        self.blocks = {}
        for base in ("data/maps", "data/scripts", "data/text"):
            for path in (ROOT / base).rglob("*.inc"):
                text = path.read_text()
                matches = list(re.finditer(r"(?m)^([A-Za-z_]\w*):{1,2}\s*(?:@[^\n]*)?$", text))
                for i, m in enumerate(matches):
                    end = matches[i + 1].start() if i + 1 < len(matches) else len(text)
                    self.blocks[m[1]] = (path, m.start(), end, text[m.end():end])

    def npc(self, map_name, index):
        obj = self.maps[map_name]["object_events"][index]
        pending, seen, texts = [obj["script"]], set(), []
        while pending and len(seen) < 100:
            label = pending.pop(0)
            if label in seen or label not in self.blocks: continue
            seen.add(label)
            path, start, end, body = self.blocks[label]
            strings = re.findall(r'\.string\s+"((?:\\.|[^"])*)"', body)
            if strings:
                raw = "".join(strings).replace(r'\"', '"')
                plain = raw.rstrip("$").replace(r"\p", "\n\n").replace(r"\n", "\n").replace(r"\l", "\n")
                texts.append(dict(label=label, text=plain, raw=raw, path=str(path.relative_to(ROOT)), hash=sha(path)))
                continue
            for token in re.findall(r"\b[A-Za-z_]\w*\b", body):
                if token in self.blocks and token not in seen and not token.startswith("Common_"):
                    pending.append(token)
        return dict(object=obj, dialogues=texts)

    def map_data(self, name):
        m = self.maps[name]; layout = self.layouts[m["layout"]]
        data = (ROOT / layout["blockdata_filepath"]).read_bytes()
        cells = struct.unpack("<" + "H" * (len(data) // 2), data)
        return dict(name=name, width=layout["width"], height=layout["height"],
                    collision=[(x >> 10) & 3 for x in cells],
                    objects=[dict(o, index=i) for i, o in enumerate(m["object_events"])],
                    warps=m["warp_events"], connections=m.get("connections", []))

    def map_image(self, name):
        if name in self.map_images: return self.map_images[name]
        layout = self.layouts[self.maps[name]["layout"]]
        headers = (ROOT/"src/data/tilesets/headers.h").read_text()
        graphics = (ROOT/"src/data/tilesets/graphics.h").read_text()
        metatiles = (ROOT/"src/data/tilesets/metatiles.h").read_text()
        banks = []
        for ident in (layout["primary_tileset"], layout["secondary_tileset"]):
            body = re.search(r"struct Tileset " + ident + r"\s*=\s*\{(.*?)\};", headers, re.S)[1]
            refs = dict(re.findall(r"\.(tiles|palettes|metatiles)\s*=\s*(\w+)", body))
            tiles_path = re.search(refs["tiles"] + r'\[\].*?\("([^"]+)"', graphics)[1]
            palette_body = re.search(refs["palettes"] + r"\[\]\[16\]\s*=\s*\{(.*?)\};", graphics, re.S)[1]
            palettes = []
            for path in re.findall(r'INCGFX_U16\("([^"]+)"', palette_body):
                rows = (ROOT/path).read_text().splitlines()[3:19]
                palettes.append([tuple(map(int, row.split())) for row in rows])
            meta_path = re.search(refs["metatiles"] + r'\[\].*?\("([^"]+)"', metatiles)[1]
            banks.append((Image.open(ROOT/tiles_path), palettes, (ROOT/meta_path).read_bytes()))
        blocks = (ROOT/layout["blockdata_filepath"]).read_bytes()
        result = Image.new("RGB", (layout["width"]*16, layout["height"]*16))
        cache = {}
        for i, (block,) in enumerate(struct.iter_unpack("<H", blocks)):
            block &= 1023
            if block not in cache:
                bank, index = (0, block) if block < 512 else (1, block-512)
                image = Image.new("RGB", (16,16))
                meta = banks[bank][2]
                if index*16+16 <= len(meta):
                    for j, tile in enumerate(struct.unpack_from("<8H", meta, index*16)):
                        tile_id, palette = tile & 1023, tile >> 12
                        tb, ti = (0,tile_id) if tile_id < 512 else (1,tile_id-512)
                        sheet = banks[tb][0]; per_row = sheet.width//8
                        tx, ty = (ti % per_row)*8, (ti//per_row)*8
                        colors = banks[0 if palette < 6 else 1][1][palette]
                        if ty+8 > sheet.height: continue
                        for py in range(8):
                            for px in range(8):
                                value = sheet.getpixel((tx+(7-px if tile&1024 else px), ty+(7-py if tile&2048 else py)))
                                if not isinstance(value,int): raise ValueError("Tiles must use indexed pixels.")
                                if j < 4 or value & 15:
                                    image.putpixel(((j%2)*8+px, ((j%4)//2)*8+py), colors[value&15])
                cache[block] = image
            result.paste(cache[block], ((i%layout["width"])*16,(i//layout["width"])*16))
        out = io.BytesIO(); result.save(out, format="PNG")
        self.map_images[name] = out.getvalue()
        return out.getvalue()

class Core:
    def __init__(self, proc, syms):
        self.proc, self.syms = proc, syms
        self.addresses = [syms["gEcStudioState"] + 4 * i for i in range(32)]
        if "gEcStudioActors" in syms and "gEcStudioText" in syms:
            self.addresses += [syms["gEcStudioActors"]+4*i for i in range(96)]
            self.addresses += [syms["gEcStudioText"]+4*i for i in range(128)]

    @classmethod
    async def open(cls, rom, elf, save=None):
        exe = build_runner(HERE / "core.c", ROOT / "build/studio/core")
        proc = await asyncio.create_subprocess_exec(str(exe), str(rom), str(save or "-"),
            stdin=asyncio.subprocess.PIPE, stdout=asyncio.subprocess.PIPE,
            stderr=open(WORK / "core.log", "ab"))
        return cls(proc, symbols(elf, ROOT))

    async def rpc(self, op, data=b""):
        self.proc.stdin.write(words(op, len(data)) + data)
        await self.proc.stdin.drain()
        status, count = struct.unpack("<II", await asyncio.wait_for(self.proc.stdout.readexactly(8), 15))
        out = await self.proc.stdout.readexactly(count)
        if status: raise ValueError(f"Native operation {op} failed ({status})")
        return out

    async def tick(self, keys=0, frames=1):
        return await self.rpc(1, words(frames, keys, len(self.addresses), *self.addresses))

    async def write(self, pairs):
        await self.rpc(5, words(*(v for pair in pairs for v in pair)))

    async def field_command(self, command, args=()):
        await self.write([(self.syms["gEcStudioArgs"] + i * 4, v) for i, v in enumerate(args)] +
                         [(self.syms["gEcStudioCommand"], command)])
        # Native flash programming spans several hundred emulated frames.
        # Advance without wall-clock pacing during this atomic operation.
        for _ in range(1200):
            packet = await self.tick(frames=1)
            pending = U32.unpack(await self.rpc(4, words(self.syms["gEcStudioCommand"], 4)))[0]
            result = U32.unpack(await self.rpc(4, words(self.syms["gEcStudioResult"], 4)))[0]
            if not pending and result: break
        else: raise ValueError("The game did not acknowledge the command.")
        if result != 1: raise ValueError("Finish the conversation or movement first." if result == 2 else f"Game command failed ({result})")
        return packet

    async def close(self):
        self.proc.stdin.close()
        try: await asyncio.wait_for(self.proc.wait(), 3)
        except asyncio.TimeoutError:
            self.proc.kill(); await self.proc.wait()

class Studio:
    def __init__(self, port):
        WORK.mkdir(parents=True, exist_ok=True)
        self.port, self.token = port, secrets.token_urlsafe(32)
        self.cat = Catalogue()
        self.lock = asyncio.Lock()
        self.core = None
        self.packet = b""
        self.state = [0] * 32
        self.keys = 0
        self.key_pulses = 0
        self.paused = False
        self.speed = 1
        self.clients = {}
        self.snapshots = []
        index = WORK / "checkpoints/index.json"
        if index.exists(): self.snapshots = json.loads(index.read_text())
        self.status = "Starting native mGBA…"
        self.building = False
        self.build_task = None
        self.build_log = ""
        self.build_id = ""
        self.frame_us = []
        self.current_map = ""
        self.started = time.monotonic()
        self.frame_count = 0
        self.last_interaction = None
        self.recorder = None
        self.jobs = []
        self.library = None
        self.decoder = TextDecoder(ROOT)
        self.rewind_buffer=deque(maxlen=16)
        self.rewind_serial=0
        self.last_rewind_frame=0

    def ingest(self, packet):
        self.packet = packet
        frame, us, samples, count = struct.unpack_from("<IIII", packet)
        self.state = list(struct.unpack_from("<" + "I" * count, packet, 16 + 240*160*4 + samples*4))
        self.current_map = self.cat.map_ids.get(tuple(self.state[2:4]), "")
        self.frame_us.append(us); self.frame_us = self.frame_us[-240:]
        self.frame_count += 1

    async def stage(self):
        rom, elf = ROOT / "pokeemerald-headless.gba", ROOT / "pokeemerald-headless.elf"
        await asyncio.to_thread(verify_rom_elf_pair, rom, elf)
        identity = sha(rom)
        dest = BUILD_STORE / identity[:16]
        dest.mkdir(parents=True, exist_ok=True)
        for src in (rom, elf):
            target = dest / src.name
            if not target.exists(): shutil.copy2(src, target)
        # Check save-struct layout separately from source/code addresses.
        abi_files = ["include/global.h", "include/global.fieldmap.h", "include/pokemon.h",
                     "include/pokemon_storage_system.h", "include/constants/flags.h", "include/constants/vars.h"]
        abi = hashlib.sha256(b"".join((ROOT / p).read_bytes() for p in abi_files)).hexdigest()
        (dest / "abi.json").write_text(json.dumps(dict(abi=abi)))
        if not (dest/"source.json").exists():
            commit=subprocess.check_output(["git","rev-parse","HEAD"],cwd=ROOT,text=True).strip()
            changed=subprocess.check_output(["git","diff","--name-only","HEAD"],cwd=ROOT,text=True).splitlines()
            untracked=subprocess.check_output(["git","ls-files","--others","--exclude-standard"],cwd=ROOT,text=True).splitlines()
            files=sorted(set(changed+untracked))
            diff=subprocess.check_output(["git","diff","--no-ext-diff","--binary","HEAD"],cwd=ROOT)
            (dest/"workspace.patch").write_bytes(diff)
            with zipfile.ZipFile(dest/"changed-source.zip","w",zipfile.ZIP_DEFLATED) as archive:
                for name in files:
                    path=ROOT/name
                    if path.is_file() and not path.is_symlink():archive.write(path,name)
            (dest/"source.json").write_text(json.dumps(dict(commit=commit,created=datetime.now(timezone.utc).isoformat(),
                changed_files=files,rom_sha256=identity,elf_sha256=sha(elf),abi=abi,
                scope="Base Git commit plus workspace patch and changed/untracked source archive."),indent=2))
        return dict(id=identity, rom=dest / rom.name, elf=dest / elf.name, abi=abi)

    def build_info(self):
        return dict(rom_sha256=self.build_id,elf_sha256=sha(self.build["elf"]),
                    rom=str(self.build["rom"]),elf=str(self.build["elf"]),abi=self.build["abi"])

    async def scene_start_files(self,directory,save_portable=True):
        directory.mkdir(parents=True,exist_ok=True)
        portable=None
        if self.state[0] and save_portable:
            self.ingest(await self.core.field_command(1))
            save=directory/"start.sav"
            await self.core.rpc(6,str(save).encode())
            portable=dict(path=str(save),abi=self.build["abi"])
        path=directory/"start.ss1"
        await self.core.rpc(2,str(path).encode())
        return dict(state=str(path),build=self.build_id,build_info=self.build_info(),map=self.current_map,
                    x=self.state[4],y=self.state[5],facing=self.state[6],ready=bool(self.state[0]),
                    position=self.state[4:7]),portable

    async def get_library(self):
        if self.library is None:self.library=await asyncio.to_thread(Library,ROOT,self.cat)
        return self.library

    def recent_scenes(self):
        result=[]
        for path in sorted((WORK/"scenes").glob("*/result.json"),key=lambda p:p.stat().st_mtime,reverse=True)[:20]:
            data=json.loads(path.read_text())
            result.append(data)
        return result

    async def queue_scene(self,spec):
        if sum(job["status"]=="running" for job in self.jobs)>=2:
            raise ValueError("Two scene workers are already running. Let one finish to keep live play responsive.")
        ident=str(time.time_ns())
        directory=WORK/"scenes"/ident
        directory.mkdir(parents=True,exist_ok=True)
        if spec.get("start",{}).get("live"):
            async with self.lock:
                initial,portable=await self.scene_start_files(directory)
                spec=dict(spec,start={"snapshot":initial})
        path=directory/"request.json";path.write_text(json.dumps(spec,indent=2))
        job=dict(id=ident,name=spec.get("name","Scene test"),status="running",directory=str(directory))
        self.jobs.append(job)
        async def execute():
            with (directory/"run.log").open("wb") as log:
                process=await asyncio.create_subprocess_exec(sys.executable,str(HERE/"run_scene.py"),str(path),"--out",str(directory),
                    cwd=ROOT,stdout=log,stderr=log)
                code=await process.wait()
            if code:
                error=directory/"error.json"
                job.update(status="failed",error=json.loads(error.read_text()).get("error") if error.exists() else "Scene worker failed")
            else:
                result=json.loads((directory/"result.json").read_text())
                job.update(status="complete" if result["outcome"].get("passed",True) else "failed",result=result)
        asyncio.create_task(execute())
        return job

    async def boot(self, build, save=None, chapter=3):
        core = await Core.open(build["rom"], build["elf"], save)
        header = (ROOT / "include/emerald_champions_headless.h").read_text()
        enum = re.findall(r"EC_HEADLESS_SCENARIO_\w+", header.split("enum EmeraldChampionsHeadlessScenario")[1].split("};")[0])
        scenario = enum.index("EC_HEADLESS_SCENARIO_STUDIO_RESUME" if save else "EC_HEADLESS_SCENARIO_STUDIO_NEW")
        await core.tick(frames=60)
        await core.write([(core.syms["gEcHeadlessFixtureParam"], chapter),
                          (core.syms["gEcHeadlessFixtureScenario"], scenario)])
        for _ in range(35):
            packet = await core.tick(frames=30)
            _, _, samples, count = struct.unpack_from("<IIII", packet)
            state = struct.unpack_from("<" + "I"*count, packet, 16 + 153600 + samples*4)
            if state[0]:
                return core, packet
        await core.close()
        raise ValueError("The new build did not reach an idle field. Previous session preserved.")

    async def start(self):
        self.build = await self.stage()
        self.build_id = self.build["id"]
        restore = WORK / "resume.json"
        previous = json.loads(restore.read_text()) if restore.exists() else None
        if previous and previous["build"] != self.build_id:
            saved_build = BUILD_STORE / previous["build"][:16]
            if (saved_build / "pokeemerald-headless.elf").exists():
                abi_file = saved_build / "abi.json"
                self.build = dict(id=previous["build"], rom=saved_build/"pokeemerald-headless.gba",
                                  elf=saved_build/"pokeemerald-headless.elf",
                                  abi=json.loads(abi_file.read_text())["abi"] if abi_file.exists() else self.build["abi"])
                self.build_id = previous["build"]
        if previous and previous["build"] == self.build_id and Path(previous["state"]).exists():
            self.core = await Core.open(self.build["rom"], self.build["elf"])
            await self.core.rpc(3, previous["state"].encode())
            self.ingest(await self.core.tick(frames=1))
        else:
            self.core, packet = await self.boot(self.build)
            self.ingest(packet)
        self.status = "Ready"
        self.last_interaction = next((s for s in reversed(self.snapshots)
                                      if s["build"] == self.build_id and s["label"] in ("Before interaction", "Before trainer test")), None)
        await self.checkpoint("Session start")
        (WORK / "server.json").write_text(json.dumps(dict(url=f"http://127.0.0.1:{self.port}", token=self.token, pid=os.getpid())))
        os.chmod(WORK / "server.json", 0o600)

    async def checkpoint(self, label):
        ident = f"{time.time_ns()}"
        path = WORK / "checkpoints" / f"{ident}.ss1"
        path.parent.mkdir(exist_ok=True)
        await self.core.rpc(2, str(path).encode())
        record = dict(id=ident, label=label[:80], build=self.build_id, state=str(path),
                      map=self.current_map, x=self.state[4], y=self.state[5], facing=self.state[6], ready=bool(self.state[0]))
        self.snapshots.append(record)
        self.snapshots = self.snapshots[-100:]
        (WORK / "resume.json").write_text(json.dumps(record))
        (WORK / "checkpoints/index.json").write_text(json.dumps(self.snapshots, indent=2))
        return record

    async def restore(self, record):
        if record["build"] != self.build_id:
            dest = BUILD_STORE / record["build"][:16]
            rom, elf = dest/"pokeemerald-headless.gba", dest/"pokeemerald-headless.elf"
            if not rom.exists() or sha(rom) != record["build"]:
                raise ValueError("The bookmark's original ROM is unavailable.")
            new = await Core.open(rom, elf)
            try:
                await new.rpc(3, record["state"].encode())
                packet = await new.tick(frames=1)
            except Exception:
                await new.close(); raise
            old=self.core; self.core=new
            abi_file=dest/"abi.json"
            self.build=dict(id=record["build"],rom=rom,elf=elf,
                            abi=json.loads(abi_file.read_text())["abi"] if abi_file.exists() else "unknown")
            self.build_id=record["build"];self.ingest(packet)
            await old.close()
            self.keys=self.key_pulses=0
            self.last_interaction=None
            self.status="Earlier build restored · workspace source unchanged"
            return
        await self.core.rpc(3, record["state"].encode())
        self.keys = self.key_pulses = 0
        self.ingest(await self.core.tick(frames=1))

    def info(self):
        avg = sum(self.frame_us) / max(1, len(self.frame_us))
        return dict(status=self.status, build=self.build_id[:12], building=self.building,
                    paused=self.paused, map=self.current_map, x=self.state[4], y=self.state[5], facing=self.state[6],
                    ready=bool(self.state[0]), battle=bool(self.state[1]), npc=self.state[7],
                    cap=self.state[8], difficulty=self.state[9],
                    party=[dict(species=self.cat.species.get(self.state[12+i*3], str(self.state[12+i*3])),
                                level=self.state[13+i*3], hp=self.state[14+i*3]) for i in range(min(6, self.state[10]))],
                    native_ms=round(avg/1000, 2), fps=round(FPS, 2),
                    frame=struct.unpack_from("<I", self.packet)[0] if self.packet else 0, snapshots=self.snapshots[-12:],
                    recording=dict(id=self.recorder.directory.name,name=self.recorder.name,frames=self.recorder.length-1) if self.recorder else None,
                    jobs=self.jobs[-8:],observed=packet_state(self.packet,self.decoder) if self.packet else {},
                    rewind_seconds=min(30,len(self.rewind_buffer)*2),
                    log=self.build_log[-5000:], speed=self.speed)

    async def loop(self):
        deadline = time.monotonic()
        while True:
            if self.core and not self.paused and self.clients:
                async with self.lock:
                    current_keys=self.keys | self.key_pulses
                    self.ingest(await self.core.tick(current_keys, self.speed))
                    self.key_pulses = 0
                    if self.recorder:
                        self.recorder.observe(self.packet,current_keys)
                        if self.recorder.length>=18000:
                            recorder,self.recorder=self.recorder,None
                            asyncio.create_task(asyncio.to_thread(recorder.finish,self.packet))
                    else:
                        frame=struct.unpack_from("<I",self.packet)[0]
                        if (frame-self.last_rewind_frame)&0xffffffff>=120:
                            directory=WORK/"rewind";directory.mkdir(exist_ok=True)
                            path=directory/f"{self.rewind_serial%16:02d}.ss1"
                            await self.core.rpc(2,str(path).encode())
                            self.rewind_buffer.append(dict(state=str(path),build=self.build_id,frame=frame))
                            self.rewind_serial+=1;self.last_rewind_frame=frame
                # Never queue video behind a slow/inactive tab. One frame in flight.
                for ws, ready in list(self.clients.items()):
                    if ready and not ws.closed:
                        self.clients[ws] = False
                        await ws.send_bytes(self.packet)
            deadline += 1 / FPS
            now = time.monotonic()
            if deadline < now - .1: deadline = now
            await asyncio.sleep(max(0, deadline - now))

    async def wait_field(self):
        self.keys = 0
        for _ in range(60):
            if self.state[0]: return
            self.ingest(await self.core.tick(frames=1))
        raise ValueError("Close the menu or retry the last interaction before applying this operation.")

    async def reload_build(self, build):
        async with self.lock:
            before = await self.checkpoint("Before build switch")
            new_core = None
            try:
                if not self.state[0]:
                    if not self.last_interaction: raise ValueError("Finish the current interaction first.")
                    await self.restore(self.last_interaction)
                await self.wait_field()
                if self.build["abi"] != build["abi"]:
                    raise ValueError("Save layout changed. Start a new sandbox; current session is preserved.")
                location = self.state[2:7]
                self.ingest(await self.core.field_command(1))
                save = WORK / ("reload-" + str(time.time_ns()) + ".sav")
                await self.core.rpc(6, str(save).encode())
                new_core, packet = await self.boot(build, save)
                _, _, samples, count = struct.unpack_from("<IIII", packet)
                restored = list(struct.unpack_from("<" + "I"*count, packet, 153616+samples*4))
                if restored[2:7] != location:
                    await new_core.field_command(2, location)
                    for _ in range(120):
                        packet = await new_core.tick(frames=5)
                        _, _, samples, count = struct.unpack_from("<IIII", packet)
                        restored = list(struct.unpack_from("<" + "I"*count, packet, 153616+samples*4))
                        if restored[0] and restored[2:7] == location: break
                    else: raise ValueError("New build did not restore the requested position.")
                old = self.core
                self.core, self.build, self.build_id = new_core, build, build["id"]
                self.ingest(packet)
                self.keys = self.key_pulses = 0
                await old.close()
                self.last_interaction = None
                await self.checkpoint("Build applied")
                self.status = "Updated · same location"
            except Exception:
                if new_core is not None and self.core is not new_core: await new_core.close()
                if self.build_id == before["build"]: await self.restore(before)
                raise

    async def build_latest(self):
        self.building = True; self.status = "Building in background"; self.build_log = ""
        try:
            source_before = await asyncio.to_thread(digest_tree)
            env = os.environ.copy()
            toolchain = Path("/Users/gguthrie/.local/share/arm-gnu-toolchain-15.2-20260718/Payload")
            cmd = ["make", "-j6", "BUILD_NAME=emerald-headless", "MAP_VERSION=emerald", "EC_HEADLESS_FIXTURES=1", "TEST=0"]
            if toolchain.exists(): cmd.append(f"DEVKITARM={toolchain}")
            cmd.append("pokeemerald-headless.gba")
            with (WORK / "latest-build.log").open("wb") as log:
                proc = await asyncio.create_subprocess_exec(*cmd, cwd=ROOT, env=env, stdout=asyncio.subprocess.PIPE, stderr=asyncio.subprocess.STDOUT)
                while line := await proc.stdout.readline():
                    log.write(line); self.build_log = (self.build_log + line.decode(errors="replace"))[-12000:]
                if await proc.wait(): raise ValueError("Build failed. The running game has not changed.")
            if source_before != await asyncio.to_thread(digest_tree):
                raise ValueError("Source changed during compilation. Build again; the running game is unchanged.")
            stamp = await asyncio.create_subprocess_exec(sys.executable, "scripts/stamp_release_inputs.py",
                "--stamp", "pokeemerald-headless.inputs.json", cwd=ROOT,
                stdout=asyncio.subprocess.PIPE, stderr=asyncio.subprocess.STDOUT)
            output,_=await stamp.communicate()
            if stamp.returncode: raise ValueError(output.decode()[-1000:])
            build = await self.stage()
            await self.reload_build(build)
            self.cat = await asyncio.to_thread(Catalogue)
            self.library=None
        except Exception as e: self.status = str(e)
        finally: self.building = False

    def wrap_dialogue(self, text):
        if not text.strip() or len(text) > 4000 or "$" in text or "\\" in text:
            raise ValueError("Use ordinary text and paragraph breaks, without $ or backslash control codes.")
        # Conservative 28-character lines fit the normal font; preserve substitutions.
        lines = []
        for paragraph in text.strip().split("\n\n"):
            current = ""
            wrapped = []
            for word in paragraph.split():
                if len(word) > 28: raise ValueError("A word is too long for the dialogue box.")
                if len(current) + len(word) + bool(current) > 28:
                    wrapped.append(current); current = word
                else: current += (" " if current else "") + word
            if current: wrapped.append(current)
            for i, line in enumerate(wrapped):
                ending = r"\n" if i == 0 and len(wrapped) > 1 else r"\l"
                if i == len(wrapped)-1: ending = r"\p"
                lines.append(line.replace('"', r'\"') + ending)
        lines[-1] = lines[-1][:-2] + "$"
        return "".join('\t.string "' + line + '"\n' for line in lines)

    async def command(self, data):
        op = data.get("op")
        if self.recorder and op not in ("record.stop","record.mark","advance","pause","screenshot"):
            raise ValueError("Stop the recording before changing the scene or build.")
        if self.building and op in ("record.start","chapter","situation.load","restore"):
            raise ValueError("Wait for the pending build before changing the session.")
        if op=="dialogue.search":
            return (await self.get_library()).search(data.get("query",""),data.get("map",""),int(data.get("limit",20)))
        if op=="dialogue.export":
            return await asyncio.to_thread((await self.get_library()).export,WORK/"library")
        if op=="scene.graph":
            return (await self.get_library()).graph(data["map"],int(data["index"]))
        if op=="history":
            return await asyncio.to_thread((await self.get_library()).history,data.get("path",""),int(data.get("limit",20)))
        if op=="assets":
            return (await self.get_library()).asset_search(data.get("query",""),int(data.get("limit",24)))
        if op=="assets.sheet":
            library=await self.get_library();out=WORK/"library"/("assets-"+str(time.time_ns())+".png");out.parent.mkdir(exist_ok=True)
            paths=data.get("paths") or [r["path"] for r in library.asset_search(data.get("query",""))["results"]]
            return dict(sheet=await asyncio.to_thread(library.asset_sheet,paths,out))
        if op=="scene.run":
            return await self.queue_scene(data["recipe"])
        if op=="scene.replay":
            directory=WORK/"scenes"/str(data["id"])
            if directory.parent!=WORK/"scenes" or not (directory/"recording.json").exists():raise ValueError("Unknown scene.")
            original=json.loads((directory/"recording.json").read_text())
            return await self.queue_scene(dict(name=original["name"]+" · "+data.get("mode","exact"),
                start=dict(recording=str(directory),mode=data.get("mode","exact"))))
        if op=="scene.compare":
            before=WORK/"scenes"/str(data["before"]);after=WORK/"scenes"/str(data["after"])
            if any(p.parent!=WORK/"scenes" or not (p/"recording.json").exists() for p in (before,after)):raise ValueError("Unknown scene.")
            return await asyncio.to_thread(compare_scenes,ROOT,before,after,WORK/"scenes"/("compare-"+str(time.time_ns())))
        if op=="scenes":return dict(scenes=self.recent_scenes(),jobs=self.jobs[-8:])
        if op=="situations":
            return dict(situations=[json.loads(p.read_text()) for p in sorted((WORK/"situations").glob("*/situation.json"),reverse=True)])
        if op=="record.stop":
            async with self.lock:
                if not self.recorder:raise ValueError("No recording is running.")
                recorder,self.recorder=self.recorder,None
                packet=self.packet
            return await asyncio.to_thread(recorder.finish,packet)
        if op=="builds":
            return dict(builds=[dict(json.loads(p.read_text()),directory=str(p.parent))
                               for p in sorted(BUILD_STORE.glob("*/source.json"),key=lambda p:p.stat().st_mtime,reverse=True)[:30]])
        if op == "build":
            if self.building: raise ValueError("A build is already running.")
            self.building = True
            self.build_task = asyncio.create_task(self.build_latest())
            return self.info()
        if op == "dialogue":
            if self.building: raise ValueError("Wait for the current build.")
            label = data["label"]
            if label not in self.cat.blocks: raise ValueError("Unknown dialogue.")
            path, start, end, body = self.cat.blocks[label]
            if sha(path) != data["hash"]: raise ValueError("Source changed. Refresh this NPC before editing.")
            if not re.search(r'\.string\s+"', body): raise ValueError("This is not a dialogue label.")
            replacement = self.wrap_dialogue(data["text"])
            full = path.read_text()
            body_start = full.index("\n", start) + 1
            matches = list(re.finditer(r'(?m)^[ \t]*\.string\s+"(?:\\.|[^"])*"[ \t]*\n?', full[body_start:end]))
            if not matches: raise ValueError("No editable text.")
            first, last = body_start+matches[0].start(), body_start+matches[-1].end()
            (WORK / f"dialogue-{time.time_ns()}.json").write_text(json.dumps(dict(path=str(path), before=full, label=label)))
            path.write_text(full[:first] + replacement + full[last:])
            # Keep one current record per edited label in the canonical book.
            book = BOOK.read_text()
            marker = f"STUDIO DIALOGUE {label}: "
            record = marker + json.dumps(data["text"].strip(), ensure_ascii=False) + f" (source: {path.relative_to(ROOT)})"
            if marker in book: book = re.sub(r"(?m)^" + re.escape(marker) + r".*$", lambda _: record, book)
            else: book = book.replace("18. COHESION CHECKS", record + "\n\n18. COHESION CHECKS", 1)
            BOOK.write_text(book)
            self.building = True
            proc = await asyncio.create_subprocess_exec(sys.executable, "scripts/emerald_champions_teams.py", "--write", cwd=ROOT,
                                                       stdout=asyncio.subprocess.PIPE, stderr=asyncio.subprocess.STDOUT)
            output, _ = await proc.communicate()
            if proc.returncode:
                self.building = False
                raise ValueError(output.decode()[-1800:])
            self.cat.scan_scripts()
            self.build_task = asyncio.create_task(self.build_latest())
            return self.info()
        async with self.lock:
            if op=="setup":
                await self.wait_field()
                flags=data.get("flags",{});variables=data.get("vars",{});items=data.get("items",{})
                if len(flags)+len(variables)+len(items)>200:raise ValueError("Use a bounded scenario setup.")
                for prefix,entries in (("FLAG_",flags),("VAR_",variables),("ITEM_",items)):
                    for name,value in entries.items():
                        if not name.startswith(prefix) or name not in self.cat.constants:raise ValueError("Unknown prerequisite "+name)
                        if prefix=="FLAG_" and not isinstance(value,bool):raise ValueError("Flag values must be booleans.")
                        if prefix!="FLAG_" and (type(value)!=int or not 0<=value<=65535):raise ValueError("Use unsigned 16-bit values.")
                before=await self.checkpoint("Before scenario setup")
                try:
                    for command,entries in ((6,flags),(7,variables),(8,items)):
                        for name,value in entries.items():
                            self.ingest(await self.core.field_command(command,[self.cat.constants[name],int(value)]))
                except Exception:
                    await self.restore(before);raise
            elif op=="rewind":
                frame=struct.unpack_from("<I",self.packet)[0]
                target=frame-int(float(data.get("seconds",2))*FPS)
                records=[r for r in self.rewind_buffer if r["build"]==self.build_id and r["frame"]<=target]
                if not records:raise ValueError("Let the scene run a little longer before rewinding.")
                await self.checkpoint("Before rewind")
                await self.restore(records[-1])
            elif op=="situation.save":
                directory=WORK/"situations"/str(time.time_ns())
                initial,portable=await self.scene_start_files(directory)
                result=dict(id=directory.name,name=data.get("name","Saved situation"),tags=data.get("tags",[]),
                            initial=initial,portable=portable,scope="Synthetic sandbox situation; no earned progression claim.")
                (directory/"situation.json").write_text(json.dumps(result,indent=2))
                return result
            elif op=="situation.load":
                directory=WORK/"situations"/str(data["id"])
                if directory.parent!=WORK/"situations":raise ValueError("Unknown situation.")
                saved=json.loads((directory/"situation.json").read_text())
                await self.checkpoint("Before loading situation")
                if data.get("mode","exact")=="exact":await self.restore(saved["initial"])
                else:
                    build=await self.stage()
                    if not saved["portable"] or saved["portable"]["abi"]!=build["abi"]:raise ValueError("No compatible portable starting save.")
                    new,packet=await self.boot(build,Path(saved["portable"]["path"]))
                    old=self.core;self.core=new;self.build=build;self.build_id=build["id"];self.ingest(packet);await old.close()
                self.keys=self.key_pulses=0
            elif op=="record.start":
                directory=WORK/"scenes"/str(time.time_ns())
                initial,portable=await self.scene_start_files(directory)
                self.recorder=Recorder(ROOT,directory,data.get("name","Recorded scene"),self.build_info(),initial,portable)
                self.recorder.observe(self.packet,force=True,label="Start")
                self.speed=1;self.paused=False
                return dict(id=directory.name,portable=portable is not None)
            elif op=="record.mark":
                if not self.recorder:raise ValueError("Start a recording first.")
                self.recorder.mark(data.get("label","Moment"),self.packet)
            elif op=="advance":
                frames=int(data.get("frames",120))
                if not 1<=frames<=1800:raise ValueError("Advance one to 1800 frames.")
                hold=button_mask(data.get("hold",0));press=button_mask(data.get("press",0))
                if (hold|press)&1 and self.state[0]:
                    self.last_interaction=await self.checkpoint("Before interaction")
                self.keys=self.key_pulses=0
                for f in range(frames):
                    buttons=hold|(press if f==0 else 0)
                    self.ingest(await self.core.tick(buttons,1))
                    if self.recorder:self.recorder.observe(self.packet,buttons)
                if data.get("label") and self.recorder:self.recorder.mark(data["label"],self.packet)
                return dict(observed=packet_state(self.packet,self.decoder),map=self.current_map)
            elif op == "chapter":
                chapter = int(data["value"])
                if chapter not in (3,4,7): raise ValueError("Choose a supported chapter setup.")
                await self.checkpoint("Before chapter jump")
                build = await self.stage()
                new, packet = await self.boot(build, chapter=chapter)
                old=self.core; self.core=new; self.build=build; self.build_id=build["id"]
                self.keys=self.key_pulses=0; self.last_interaction=None
                self.ingest(packet); await old.close()
                await self.checkpoint("C%02d sandbox" % chapter)
                self.status="Chapter setup ready"
            elif op == "trainer":
                ident=data["id"]
                if ident not in self.cat.trainers: raise ValueError("Choose a retained trainer.")
                await self.wait_field()
                self.last_interaction=await self.checkpoint("Before trainer test")
                self.ingest(await self.core.field_command(5,[self.cat.constants[ident]]))
                self.status="Native trainer battle · sandbox"
            elif op == "pause": self.paused = not self.paused; self.keys = self.key_pulses = 0
            elif op == "speed": self.speed = int(data["value"]); assert self.speed in (1, 2, 4)
            elif op == "checkpoint": await self.checkpoint(data.get("label", "Bookmark"))
            elif op == "restore":
                record = next(x for x in self.snapshots if x["id"] == data["id"])
                await self.restore(record)
            elif op == "retry":
                if not self.last_interaction: raise ValueError("No interaction checkpoint yet. Walk up to an NPC and press A.")
                await self.restore(self.last_interaction)
            elif op in ("warp", "heal", "difficulty"):
                await self.wait_field()
                await self.checkpoint("Before " + op)
                if op == "warp":
                    m = self.cat.maps[data["map"]]; layout = self.cat.layouts[m["layout"]]
                    x, y = int(data["x"]), int(data["y"])
                    if not (0 <= x < layout["width"] and 0 <= y < layout["height"]): raise ValueError("Outside the map.")
                    cells = (ROOT / layout["blockdata_filepath"]).read_bytes()
                    block = struct.unpack_from("<H", cells, (y * layout["width"] + x)*2)[0]
                    if (block >> 10) & 3: raise ValueError("Choose a walkable tile.")
                    if any(o["x"] == x and o["y"] == y for o in m["object_events"]): raise ValueError("Choose a tile beside the NPC.")
                    self.ingest(await self.core.field_command(2, [m["group"], m["num"], x, y, int(data.get("facing", 1))]))
                elif op == "heal": self.ingest(await self.core.field_command(3))
                else: self.ingest(await self.core.field_command(4, [int(data["value"])]))
            elif op == "party":
                await self.wait_field()
                entries = data["party"]
                if not 1 <= len(entries) <= 6: raise ValueError("Choose one to six Pokémon.")
                c = self.cat.constants; syms = self.core.syms
                pairs = [(syms["gEcAgentPrepPartyCount"], len(entries))]
                for i, name in enumerate(entries):
                    if name not in c or not name.startswith("SPECIES_"): raise ValueError("Unknown Pokémon.")
                    for field, value in [("Species", c[name]), ("Preset", 0), ("Format", 0), ("Level", 0xffffffff),
                                         ("Nature", 0xffffffff), ("Ability", 0xffffffff), ("Item", 0xffffffff)]:
                        pairs.append((syms["gEcAgentPrep"+field]+4*i, value))
                    for field, count in [("Moves", 4), ("Evs", 6)]:
                        for j in range(count): pairs.append((syms["gEcAgentPrep"+field]+4*(i*count+j), 0xffffffff))
                await self.checkpoint("Before party preparation")
                await self.core.write(pairs + [(syms["gEcAgentPrepCommand"], 1)])
                for _ in range(1200):
                    self.ingest(await self.core.tick(frames=1))
                    pending=U32.unpack(await self.core.rpc(4, words(syms["gEcAgentPrepCommand"], 4)))[0]
                    result=U32.unpack(await self.core.rpc(4, words(syms["gEcAgentPrepResult"], 4)))[0]
                    if not pending and result: break
                else: raise ValueError("Party preparation timed out.")
                if result != 1: raise ValueError(f"Party preparation failed ({result}); previous party retained.")
            elif op == "screenshot":
                path = WORK / f"capture-{time.time_ns()}.png"
                Image.frombytes("RGBA", (240,160), self.packet[16:153616]).save(path)
                return dict(path=str(path))
            else: raise ValueError("Unknown command.")
            return self.info()

def make_app(studio):
    @web.middleware
    async def local_only(request, handler):
        if request.host not in (f"127.0.0.1:{studio.port}", f"localhost:{studio.port}"):
            raise web.HTTPForbidden()
        if request.method != "GET" and request.headers.get("X-Studio-Token") != studio.token:
            raise web.HTTPForbidden()
        try: return await handler(request)
        except (ValueError, KeyError, StopIteration, AssertionError) as e:
            return web.json_response({"error": str(e)}, status=400)
    app = web.Application(middlewares=[local_only], client_max_size=100000)
    async def index(request):
        return web.Response(text=(HERE/"index.html").read_text().replace("__TOKEN__", studio.token), content_type="text/html",
                            headers={"Cache-Control": "no-store"})
    async def info(request): return web.json_response(studio.info())
    async def catalog(request):
        return web.json_response(dict(maps=sorted(studio.cat.maps), trainers=list(studio.cat.trainers.values()),
                                     species=[dict(id=k, name=title(k)) for k,v in studio.cat.constants.items() if k.startswith("SPECIES_") and 0 < v < 3000]))
    async def map_data(request): return web.json_response(studio.cat.map_data(request.query["name"]))
    async def map_image(request):
        return web.Response(body=await asyncio.to_thread(studio.cat.map_image, request.query["name"]), content_type="image/png")
    async def npc(request): return web.json_response(studio.cat.npc(request.query["map"], int(request.query["index"])))
    async def library(request):
        lib=await studio.get_library()
        return web.json_response(dict(summary=lib.summary(),scenes=studio.recent_scenes(),
                                      situations=[json.loads(p.read_text()) for p in sorted((WORK/"situations").glob("*/situation.json"),reverse=True)]))
    async def artifact(request):
        path=(WORK/request.match_info["path"]).resolve()
        if not any(path.is_relative_to(WORK/base) for base in ("scenes","library","situations")):
            raise web.HTTPForbidden()
        if path.suffix not in (".png",".webp",".json",".md",".txt") or not path.is_file():raise web.HTTPNotFound()
        return web.FileResponse(path)
    async def sprite(request):
        lib=await studio.get_library();path=(ROOT/request.query["path"]).resolve()
        if path not in lib.assets:raise web.HTTPNotFound()
        return web.FileResponse(path)
    async def command(request): return web.json_response(await studio.command(await request.json()))
    async def asset(request):
        name = request.match_info["name"]
        if name not in ("app.js", "style.css", "audio.js","library-ui.js"): raise web.HTTPNotFound()
        return web.FileResponse(HERE / name, headers={"Cache-Control":"no-cache"})
    async def websocket(request):
        if request.query.get("token") != studio.token: raise web.HTTPForbidden()
        ws = web.WebSocketResponse(compress=False, heartbeat=20, max_msg_size=1024)
        await ws.prepare(request)
        studio.clients[ws] = True
        try:
            async for msg in ws:
                if msg.type == WSMsgType.TEXT:
                    data = json.loads(msg.data)
                    if data.get("ack"): studio.clients[ws] = True
                    if "keys" in data:
                        keys = int(data["keys"]) & 1023
                        async with studio.lock:
                            if keys & 1 and not studio.keys & 1 and studio.state[0]:
                                studio.last_interaction = await studio.checkpoint("Before interaction")
                            studio.key_pulses |= keys & ~studio.keys
                            studio.keys = keys
        finally:
            studio.clients.pop(ws, None); studio.keys = 0
        return ws
    async def lifecycle(app):
        await studio.start()
        task = asyncio.create_task(studio.loop())
        yield
        task.cancel()
        with contextlib.suppress(asyncio.CancelledError): await task
        async with studio.lock:
            if studio.recorder:
                await asyncio.to_thread(studio.recorder.finish,studio.packet)
            await studio.checkpoint("Session closed")
            await studio.core.close()
    app.cleanup_ctx.append(lifecycle)
    async def shutdown(app):
        await asyncio.gather(*(ws.close() for ws in list(studio.clients)), return_exceptions=True)
    app.on_shutdown.append(shutdown)
    app.router.add_get("/", index)
    app.router.add_get("/api/state", info)
    app.router.add_get("/api/catalog", catalog)
    app.router.add_get("/api/map", map_data)
    app.router.add_get("/api/map-image", map_image)
    app.router.add_get("/api/npc", npc)
    app.router.add_get("/api/library",library)
    app.router.add_get("/api/sprite",sprite)
    app.router.add_get("/artifacts/{path:.*}",artifact)
    app.router.add_post("/api/command", command)
    app.router.add_get("/ws", websocket)
    app.router.add_get("/{name}", asset)
    return app

if __name__ == "__main__":
    p = argparse.ArgumentParser()
    p.add_argument("--port", type=int, default=8766)
    args = p.parse_args()
    web.run_app(make_app(Studio(args.port)), host="127.0.0.1", port=args.port, print=print, shutdown_timeout=2)
