#!/usr/bin/env python3
"""Exercise Studio against the real ROM in a separate disposable native session."""
import asyncio
import hashlib
import json
import statistics
import struct
import time
from pathlib import Path
from PIL import Image
import server

async def main():
    server.WORK = server.ROOT / "work/studio/verification"
    studio = server.Studio(0)
    checks = []
    async def tick(n=1, keys=0):
        studio.ingest(await studio.core.tick(keys=keys, frames=n))
    async def settle():
        for _ in range(100):
            await tick(10)
            if studio.state[0]: return
        raise AssertionError("Did not return to idle field")
    def record(name): checks.append(name); print("PASS", name, flush=True)
    await studio.start()
    try:
        await studio.command(dict(op="chapter", value=3))
        assert studio.current_map == "OldaleTown_PokemonCenter_1F"
        assert studio.state[4:6] == [8,6]
        record("native sandbox boot")
        cp = await studio.checkpoint("Verification position")
        await tick(30,32); await settle()
        assert studio.state[4] < 8
        await studio.restore(cp)
        assert studio.state[4:6] == [8,6]
        record("movement and exact same-build restore")
        await studio.command(dict(op="party",party=["SPECIES_AERODACTYL","SPECIES_SHAYMIN","SPECIES_AZUMARILL"]))
        await settle()
        assert studio.state[10] == 3 and studio.state[12] == studio.cat.constants["SPECIES_AERODACTYL"]
        assert studio.state[13] == 14
        record("native party preparation at actual cap")
        await studio.command(dict(op="difficulty",value=2)); await tick()
        assert studio.state[9] == 2
        record("native difficulty change")
        await studio.command(dict(op="warp",map="OldaleTown_PokemonCenter_1F",x=12,y=7,facing=2))
        await settle(); await tick()
        assert studio.state[4:7] == [12,7,2]
        record("warp preserves requested tile and facing")
        await studio.checkpoint("Before reload")
        await studio.reload_build(await studio.stage())
        assert studio.state[4:7] == [12,7,2] and studio.state[10] == 3 and studio.state[9] == 2
        record("native save and clean boot restore location, party and difficulty")
        previous_build=studio.build_id
        try:
            await studio.reload_build(dict(studio.build,abi="incompatible"))
            raise AssertionError("Incompatible save layout accepted")
        except ValueError as e:
            assert "Save layout changed" in str(e)
        assert studio.build_id==previous_build and studio.state[4:7]==[12,7,2]
        record("incompatible update retains the running build and checkpoint")
        times=[]; peaks=[]
        for _ in range(180):
            await tick()
            _, us, samples, _ = struct.unpack_from("<IIII",studio.packet)
            times.append(us/1000)
            if samples:
                pcm=struct.unpack_from("<"+"h"*(samples*2),studio.packet,153616)
                peaks.append(max(map(abs,pcm)))
        assert max(peaks)>0
        record("native video and non-silent stereo PCM")
        before=await studio.checkpoint("Before trial battle")
        await studio.command(dict(op="trainer",id="TRAINER_ROXANNE_1"))
        for _ in range(30):
            await tick(30)
            if studio.state[1]: break
        assert studio.state[1]
        await tick(120)
        Image.frombytes("RGBA",(240,160),studio.packet[16:153616]).save(server.WORK/"battle.png")
        await studio.restore(before); assert not studio.state[1]
        record("native trainer launch and prebattle retry")
        await studio.command(dict(op="chapter",value=4)); await settle()
        assert studio.current_map=="PetalburgCity_Gym" and studio.state[4:6]==[4,108]
        record("C04 Norman and Wally setup")
        await studio.command(dict(op="chapter",value=3))
        await studio.command(dict(op="setup",flags={"FLAG_BADGE01_GET":True},items={"ITEM_OLD_AMBER":1}))
        await tick()
        assert studio.state[8]==20
        record("explicit scenario prerequisites use native flag and inventory operations")
        situation=await studio.command(dict(op="situation.save",name="Verification ready"))
        await studio.command(dict(op="warp",map="OldaleTown_PokemonCenter_1F",x=12,y=7,facing=2))
        await settle()
        await studio.command(dict(op="situation.load",id=situation["id"]))
        assert studio.state[4:6]==[8,6]
        record("named situation restores its original position and build")
        recording=await studio.command(dict(op="record.start",name="Tool verification"))
        await studio.command(dict(op="advance",frames=30,hold="LEFT",label="Moved left"))
        recorded=await studio.command(dict(op="record.stop"))
        assert Path(recorded["sheets"][0]).exists()
        import scenes
        trace=json.loads(Path(recorded["recording"]).read_text())
        assert sum(s["frames"] for s in trace["inputs"])==30
        record("recorded input duration and native contact sheet generation")
        Image.frombytes("RGBA",(240,160),studio.packet[16:153616]).save(server.WORK/"c04.png")
        result=dict(checks=checks,rom=studio.build_id,native_frame_ms=dict(median=statistics.median(times),p95=sorted(times)[170],max=max(times)),
                    pcm_peak=max(peaks),scope="Synthetic native tool verification; no earned campaign progress")
        (server.WORK/"result.json").write_text(json.dumps(result,indent=2))
        print(json.dumps(result,indent=2))
    finally:
        await studio.core.close()

if __name__=="__main__":asyncio.run(main())
