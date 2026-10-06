#!/usr/bin/env python3
"""Run a scene recipe headlessly, using the same native core as the live Studio."""
import argparse
import asyncio
import hashlib
import json
from pathlib import Path
import traceback
import server
from scenes import Recorder,keys,packet_state
from native_tools import read_game_query

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
            problem=server.compatible_saves(portable["abi"],latest["abi"])
            save_note=server.legacy_save_match(portable["abi"],latest) if problem else "compiled save layout matches"
            if problem and not save_note:
                if not str(portable["abi"]).startswith("layout1:"):
                    problem+=" This recording's portable save predates compiled save-layout ids (legacy provenance) and its headers have changed; record it again from idle field."
                raise ValueError("The recording's save cannot start the latest build. "+problem)
            build=latest
            studio.core,packet=await studio.boot(build,Path(portable["path"]))
        else:
            build=server.recorded_build(b)
            if server.sha(build["rom"])!=build["id"]:raise ValueError("Recorded ROM identity changed.")
            studio.core=await server.Core.open(build["rom"],build["elf"])
            await studio.core.rpc(3,replay["initial"]["state"].encode())
            packet=await studio.core.tick(frames=0)
    elif "snapshot" in start:
        cp=start["snapshot"];b=cp["build_info"]
        build=server.recorded_build(b)
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
        elif start.get("cold_boot"):
            # A real new game: power on to the title screen with no fixture at all.
            studio.core=await server.Core.open(build["rom"],build["elf"])
            packet=await studio.core.tick(frames=int(start.get("settle_frames",120)))
        elif "save" in start:
            # Continue a playthrough from the save an earlier leg ended on.
            # Boot a private copy: the core writes the save back in place, and the
            # previous leg's end.sav must never change.
            import shutil
            out.mkdir(parents=True,exist_ok=True)
            source_save=out/"from-previous-leg.sav"
            shutil.copy2(start["save"],source_save)
            studio.core,packet=await studio.boot(build,source_save)
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
        walk_failure=None
        bags={};interactions=[]
        class _Steps:
            def __init__(self,items):self.items=list(items)
            def extend_front(self,more):self.items[0:0]=list(more)
            def __iter__(self):return self
            def __next__(self):
                if not self.items:raise StopIteration
                return self.items.pop(0)
        steps_iter=_Steps(steps)
        for step in steps_iter:
            if "bag" in step and walk_failure is None:
                # Full Bag snapshot read back from the game: every item id, nonzero counts kept.
                ids=json.loads((server.ROOT/"artifacts/playthrough/item_ids.json").read_text())
                async def _adv():
                    nonlocal total
                    studio.ingest(await studio.core.tick(frames=1));recorder.observe(studio.packet,0);total+=1
                bag={}
                for name in ids:
                    v=await read_game_query(studio.core,4,int(studio.cat.constants[name]),_adv)
                    if v:bag[name]=v
                bags[step["bag"]]=bag
                (out/f"bag-{step['bag']}.json").write_text(json.dumps(bag,indent=1))
                recorder.mark("Bag "+step["bag"],studio.packet)
                continue
            if ("talk_id" in step or "inspect" in step) and walk_failure is None:
                # Walk next to a live actor (talk_id) or a tile (inspect), face it, press A, then
                # tap the step's button until the game is idle again.
                import sys as _sys
                _sys.path.insert(0,str(server.ROOT/"artifacts"/"playthrough"))
                import route_steps
                live=packet_state(studio.packet,recorder.decoder)["actors"]
                if "talk_id" in step:
                    hit=[a for a in live if a["local_id"]==int(step["talk_id"]) and not a["invisible"]]
                    if not hit and step.get("near") and not step.get("_approached"):
                        # The game only loads objects near the camera: walk to the closest
                        # reachable tile to where the map places this one, then look again.
                        here=(studio.state[4],studio.state[5]);near=tuple(step["near"])
                        best=route_steps.closest_reachable(studio.current_map,here,near)
                        if best and best!=here:
                            steps_iter.extend_front([{"walk_to":list(best)},dict(step,_approached=True)])
                            continue
                    if not hit:
                        recorder.mark(f"ABSENT actor {step['talk_id']} {step.get('label','')}",studio.packet)
                        interactions.append(dict(label=step.get("label",""),target=step["talk_id"],status="absent"))
                        continue
                    target=(hit[0]["x"],hit[0]["y"])
                else:
                    target=tuple(step["inspect"])
                occupied={(a["x"],a["y"]) for a in live if a["local_id"]!=255 and not a["invisible"]}
                here=(studio.state[4],studio.state[5])
                options=[]
                for dx,dy,face in ((0,1,"UP"),(0,-1,"DOWN"),(1,0,"LEFT"),(-1,0,"RIGHT"),(0,2,"UP"),(0,-2,"DOWN"),(2,0,"LEFT"),(-2,0,"RIGHT")):
                    spot=(target[0]+dx,target[1]+dy)
                    if abs(dx)==2 or abs(dy)==2:
                        # across a counter: the tile between must be a counter
                        mid=(target[0]+dx//2,target[1]+dy//2)
                        if not route_steps.is_counter(studio.current_map,mid):continue
                    mv=[] if spot==here else route_steps.path(studio.current_map,here,spot,block=occupied-{spot})
                    if mv is not None:options.append((len(mv),spot,face))
                if not options:
                    recorder.mark(f"UNREACHABLE {target} {step.get('label','')}",studio.packet)
                    interactions.append(dict(label=step.get("label",""),target=list(target),status="unreachable"))
                    continue
                options.sort();_,spot,face=options[0]
                serial0=studio.state[30];bag_label=step.get("label","")
                idx=len(interactions)
                interactions.append(dict(label=bag_label,target=list(target),status="visited",stand=list(spot)))
                sub=[{"walk_to":list(spot),"face":face,"label":"At "+bag_label[:24]},{"note":idx,"edge":"start"},{"press":"A","frames":40},
                     {"tap":step.get("mode","A"),"every":40,"frames":int(step.get("limit",4000)),"min_frames":20,"until":"idle"},
                     {"note":idx,"edge":"end"}]
                steps_iter.extend_front(sub)
                continue
            if "note" in step:
                interactions[step["note"]][step["edge"]]=recorder.length if hasattr(recorder,"length") else total
                continue
            if ("walk_to" in step or "walk" in step) and walk_failure is None:
                async def settle_scene(label):
                    # A script took control (trigger, trainer, gift): tap A through it, then
                    # wait for twelve idle frames. Battles resolve per battle_resolution.
                    nonlocal total
                    recorder.mark(label,studio.packet);idle=0
                    for f in range(12000):
                        m=keys("A") if f%30==0 else 0
                        studio.ingest(await studio.core.tick(m,frames=1));recorder.observe(studio.packet,m);total+=1
                        idle=idle+1 if studio.state[0] else 0
                        if idle>=12:break
                    recorder.mark("Scene over",studio.packet)
                async def walk_moves(dirs,face_last=False):
                    # Closed loop: hold each direction until the game reports one tile of progress
                    # (position or map). Returns None, or why the walk stopped.
                    nonlocal total
                    for i,d in enumerate(dirs):
                        mask=keys(d);before=(studio.state[2],studio.state[3],studio.state[4],studio.state[5])
                        origin=before;moved=False
                        for attempt in range(2):
                            for f in range(int(step.get("tile_limit",90))):
                                studio.ingest(await studio.core.tick(mask,frames=1));recorder.observe(studio.packet,mask);total+=1
                                if (studio.state[2],studio.state[3],studio.state[4],studio.state[5])!=before:moved=True;break
                                if not studio.state[0] and f>4:break
                            if moved:break
                            if not studio.state[0]:
                                await settle_scene(f"Scene during {d} at {before[2]},{before[3]}")
                                before=(studio.state[2],studio.state[3],studio.state[4],studio.state[5])
                                if before!=origin:moved=True;break  # the scene moved us (warp, cutscene)
                        if not moved and face_last and i==len(dirs)-1:return None  # only turning to face
                        if not moved:
                            recorder.mark("STUCK "+d,studio.packet)
                            return f"move {i+1} ({d}) made no progress at {before[2]},{before[3]}"
                        # let the tile finish; play through any script the step triggered
                        for f in range(16):
                            studio.ingest(await studio.core.tick(0,frames=1));recorder.observe(studio.packet,0);total+=1
                            if studio.state[0] and f>=2:break
                        if not studio.state[0]:
                            await settle_scene(f"Scene after {d} at {studio.state[4]},{studio.state[5]}")
                    return None
                if "walk_to" in step:
                    # Plan from the game's reported position and live actors; re-plan when a
                    # wandering NPC blocks the way.
                    import sys as _sys
                    _sys.path.insert(0,str(server.ROOT/"artifacts"/"playthrough"))
                    import route_steps
                    goal=tuple(step["walk_to"]);start_map=studio.current_map;failure=None
                    for tries in range(5):
                        here=(studio.state[4],studio.state[5])
                        if here==goal or studio.current_map!=start_map:failure=None;break
                        live=packet_state(studio.packet,recorder.decoder)["actors"]
                        occupied={(a["x"],a["y"]) for a in live if a["local_id"]!=255 and not a["invisible"]}
                        moves=route_steps.path(studio.current_map,here,goal,block=occupied)
                        if moves is None:
                            failure=f"no path on {studio.current_map} from {here} to {goal}"
                        else:
                            failure=await walk_moves([route_steps.DIRS[d] for d in moves])
                            if failure is None:break
                        for f in range(40):
                            studio.ingest(await studio.core.tick(0,frames=1));recorder.observe(studio.packet,0);total+=1
                    if failure is None and step.get("face"):failure=await walk_moves([step["face"]],face_last=True)
                    if failure:walk_failure=f"walk_to {step.get('label','')}: {failure}"
                else:
                    failure=await walk_moves(step["walk"])
                    if failure:walk_failure=f"walk {step.get('label','')}: {failure}"
                if step.get("label"):recorder.mark(step["label"],studio.packet)
                if total>18000:raise ValueError("A scene may run at most 18,000 frames.")
                continue
            if walk_failure is not None:break
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
                    if step.get("until_text") and f%6==0 and step["until_text"] in packet_state(studio.packet,recorder.decoder)["text"]:break
            if step.get("label"):recorder.mark(step["label"],studio.packet)
        final=packet_state(studio.packet,recorder.decoder)
        final["map"]=studio.current_map
        if studio.state[0]:
            # The next leg of a playthrough starts from this save.
            studio.ingest(await studio.core.field_command(1))
            await studio.core.rpc(6,str(out/"end.sav").encode())
            final["end_save"]=str(out/"end.sav")
        query_values={}
        async def advance_query():
            studio.ingest(await studio.core.tick(frames=1))
            recorder.observe(studio.packet,0)
        for name,query in spec.get("queries",{}).items():
            kind=int(query["kind"])
            ident=query.get("id",0)
            if isinstance(ident,str):ident=studio.cat.constants[ident]
            query_values[name]=await read_game_query(studio.core,kind,int(ident),advance_query)
        final["queries"]=query_values
        final["interactions"]=interactions
        if bags:final["bags"]=sorted(bags)
        failures=[walk_failure] if walk_failure else []
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
        outcome=dict(passed=not failures,failures=failures,final=final,exact_replay_pixels=exact,
                     battle_resolution=forced)
        if replay:
            # Recordings made before build stamps stay replayable; say so rather than imply proof.
            outcome["parent_provenance"]=replay["build"].get("provenance_label") or server.provenance.label(None)
            if start.get("mode","exact")=="latest":outcome["save_compatibility"]=save_note
        result=await asyncio.to_thread(recorder.finish,studio.packet,outcome)
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
