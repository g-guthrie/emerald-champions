#!/usr/bin/env python3
"""Run a scene recipe headlessly, using the same native core as the live Studio."""
import argparse
import asyncio
import hashlib
import json
from pathlib import Path
import struct
import traceback
import server
from scenes import Recorder,keys,packet_state

async def run(spec,out):
    out=out.resolve()
    server.WORK=out/"runtime"
    studio=server.Studio(0)
    start=spec.get("start",{"chapter":3})
    replay=None
    latest=await studio.stage()
    if "recording" in start:
        folder=Path(start["recording"])
        replay=json.loads((folder/"recording.json").read_text())
        b=replay["build"]
        if start.get("mode","exact")=="latest":
            portable=replay.get("portable")
            if not portable:raise ValueError("This recording began mid-interaction. Record from idle field to test a new build.")
            if portable["abi"]!=latest["abi"]:raise ValueError("The recording's save layout differs from the latest build.")
            build=latest
            studio.core,packet=await studio.boot(build,Path(portable["path"]))
        else:
            build=dict(id=b["rom_sha256"],rom=Path(b["rom"]),elf=Path(b["elf"]),abi=b["abi"])
            if server.sha(build["rom"])!=build["id"]:raise ValueError("Recorded ROM identity changed.")
            studio.core=await server.Core.open(build["rom"],build["elf"])
            await studio.core.rpc(3,replay["initial"]["state"].encode())
            packet=await studio.core.tick(frames=0)
    elif "snapshot" in start:
        cp=start["snapshot"];b=cp["build_info"]
        build=dict(id=b["rom_sha256"],rom=Path(b["rom"]),elf=Path(b["elf"]),abi=b["abi"])
        studio.core=await server.Core.open(build["rom"],build["elf"])
        await studio.core.rpc(3,cp["state"].encode());packet=await studio.core.tick(frames=0)
    else:
        build=latest
        if "fixture" in start:
            header=(server.ROOT/"include/emerald_champions_headless.h").read_text()
            import re
            enums=re.findall(r"EC_HEADLESS_SCENARIO_\w+",header.split("enum EmeraldChampionsHeadlessScenario")[1].split("};")[0])
            ident="EC_HEADLESS_SCENARIO_"+start["fixture"]
            if ident not in enums:raise ValueError("Unknown native fixture.")
            studio.core=await server.Core.open(build["rom"],build["elf"])
            packet=await studio.core.tick(frames=60)
            await studio.core.write([(studio.core.syms["gEcHeadlessFixtureParam"],int(start.get("param",0))),
                                     (studio.core.syms["gEcHeadlessFixtureScenario"],enums.index(ident))])
            for _ in range(int(start.get("settle_frames",360))//30):
                packet=await studio.core.tick(frames=30)
        else:
            studio.core,packet=await studio.boot(build,chapter=int(start.get("chapter",3)))
    studio.build,studio.build_id=build,build["id"];studio.ingest(packet)
    try:
        if spec.get("setup"):await studio.command(dict(op="setup",**spec["setup"]))
        if spec.get("party"):
            await studio.command(dict(op="party",party=spec["party"]))
        if "difficulty" in spec:await studio.command(dict(op="difficulty",value=int(spec["difficulty"])))
        if "warp" in spec:
            await studio.command(dict(op="warp",**spec["warp"]))
            for _ in range(120):
                studio.ingest(await studio.core.tick(frames=5))
                if studio.state[0]:break
        forced=spec.get("battle_resolution","native")
        if forced not in ("native","fixture_win","fixture_defeat","fixture_loss"):raise ValueError("Unknown battle-resolution mode.")
        if forced!="native":
            import re
            header=(server.ROOT/"include/emerald_champions_headless.h").read_text()
            enums=re.findall(r"EC_HEADLESS_SCENARIO_\w+",header.split("enum EmeraldChampionsHeadlessScenario")[1].split("};")[0])
            scenario="EC_HEADLESS_SCENARIO_BOOK_RESEARCH" if forced=="fixture_defeat" else "EC_HEADLESS_SCENARIO_CAMPAIGN_AUTOWIN"
            await studio.core.write([(studio.core.syms["gEcHeadlessFixtureActiveScenario"],enums.index(scenario)),
                                     (studio.core.syms["gEcHeadlessFixtureParam"],0),
                                     (studio.core.syms["gEcHeadlessCampaignForceLoss"],int(forced=="fixture_loss"))])
        initial,portable=await studio.scene_start_files(out,save_portable=not replay)
        recorder=Recorder(server.ROOT,out,spec.get("name","Scene test"),studio.build_info(),initial,portable,
                          parent=start.get("recording"))
        recorder.observe(studio.packet,force=True,label="Start")
        start_state=packet_state(studio.packet,recorder.decoder)
        markers={m["frame"]:m["label"] for m in replay["markers"]} if replay else {}
        steps=spec.get("steps",replay["inputs"] if replay else [])
        total=0
        for step in steps:
            frames=int(step.get("frames",120))
            if not 0<=frames<=18000 or total+frames>18000:raise ValueError("A scene may run at most 18,000 frames.")
            hold=keys(step.get("hold",step.get("keys",0)));press=keys(step.get("press",0))
            tap=keys(step.get("tap",0));every=max(2,int(step.get("every",90)))
            active_seen=not bool(studio.state[0])
            idle_frames=0
            for f in range(frames):
                mask=hold|(press if f==0 else 0)|(tap if f%every==0 else 0)
                studio.ingest(await studio.core.tick(mask,frames=1))
                recorder.observe(studio.packet,mask)
                total+=1
                if total in markers:recorder.mark(markers[total],studio.packet)
                active_seen|=not bool(studio.state[0])
                idle_frames=idle_frames+1 if studio.state[0] else 0
                if f>=int(step.get("min_frames",30)):
                    condition=step.get("until")
                    if condition=="idle" and active_seen and idle_frames>=12:break
                    if condition=="battle" and studio.state[1]:break
                    if condition=="dialogue" and studio.state[11] and studio.state[30]!=start_state["text_serial"]:break
            if step.get("label"):recorder.mark(step["label"],studio.packet)
        final=packet_state(studio.packet,recorder.decoder)
        final["map"]=studio.current_map
        query_values={}
        for name,query in spec.get("queries",{}).items():
            kind=int(query["kind"])
            ident=query.get("id",0)
            if isinstance(ident,str):ident=studio.cat.constants[ident]
            syms=studio.core.syms
            await studio.core.write([(syms["gEcHeadlessCampaignQueryKind"],kind),
                                     (syms["gEcHeadlessCampaignQueryId"],int(ident))])
            for _ in range(3):
                studio.ingest(await studio.core.tick(frames=1))
                recorder.observe(studio.packet,0)
            raw=await studio.core.rpc(4,server.words(syms["gEcHeadlessCampaignQueryValue"],4))
            query_values[name]=struct.unpack("<I",raw)[0]
        final["queries"]=query_values
        failures=[]
        for field,wanted in spec.get("expect",{}).items():
            if field=="contains_text":
                if wanted not in final["text"]:failures.append("Displayed text did not contain "+repr(wanted))
            elif field=="forbidden_text":
                printed="\n".join(state["text"] for _,_,state,_ in recorder.raw)
                found=[text for text in wanted if text in printed]
                if found:failures.append("Unexpected dialogue: "+repr(found))
            elif field=="visited_maps":
                visited={studio.cat.map_ids.get((state["group"],state["num"])) for _,_,state,_ in recorder.raw}
                missing=set(wanted)-visited
                if missing:failures.append("Native trace never visited "+", ".join(sorted(missing)))
            elif final.get(field)!=wanted:failures.append(f"{field}: expected {wanted!r}, observed {final.get(field)!r}")
        exact=None
        if replay and start.get("mode","exact")=="exact":
            expected=replay["captures"][-1]["sha256"]
            from PIL import Image
            import io
            # Compare pixels, not PNG encoding settings.
            old=Image.open(Path(start["recording"])/replay["captures"][-1]["path"]).convert("RGBA").tobytes()
            exact=old==studio.packet[16:153616]
            if not exact:failures.append("Recorded-build replay ended on different pixels.")
        result=await asyncio.to_thread(recorder.finish,studio.packet,
                 dict(passed=not failures,failures=failures,final=final,exact_replay_pixels=exact,
                      battle_resolution=forced))
        (out/"recipe.json").write_text(json.dumps(spec,indent=2))
        return result
    finally:await studio.core.close()

if __name__=="__main__":
    parser=argparse.ArgumentParser()
    parser.add_argument("recipe",type=Path);parser.add_argument("--out",type=Path,required=True)
    args=parser.parse_args();args.out.mkdir(parents=True,exist_ok=True)
    try:print(json.dumps(asyncio.run(run(json.loads(args.recipe.read_text()),args.out)),indent=2))
    except Exception as e:
        (args.out/"error.json").write_text(json.dumps({"error":str(e),"traceback":traceback.format_exc()},indent=2))
        raise
