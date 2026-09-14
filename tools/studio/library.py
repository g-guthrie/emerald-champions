"""Source-backed dialogue, scene dependencies, asset discovery and Git history."""
from collections import defaultdict
import hashlib
import io
import json
from pathlib import Path
import re
import subprocess
from PIL import Image

class Library:
    def __init__(self, root, catalogue):
        self.root, self.cat = root, catalogue
        self.records = {}
        self.edges = {}
        self.specials = {}
        self.bindings = []
        files={}
        for label, (path, start, end, body) in catalogue.blocks.items():
            code = re.sub(r"(?m)@.*$", "", body)
            self.edges[label] = list(dict.fromkeys(t for t in re.findall(r"\b[A-Za-z_]\w*\b", code)
                                                  if t in catalogue.blocks and t != label))
            self.specials[label] = re.findall(r"\bspecial(?:var\s+\w+,)?\s+(\w+)", code)
            strings = re.findall(r'\.string\s+"((?:\\.|[^"])*)"', body)
            if strings:
                text = "".join(strings).rstrip("$").replace(r'\"', '"')
                text = text.replace(r"\p", "\n\n").replace(r"\n", "\n").replace(r"\l", "\n")
                if path not in files:files[path]=path.read_text()
                self.records[label] = dict(label=label, text=text, path=str(path.relative_to(root)),
                    line=files[path][:start].count("\n")+1, owners=[])
        self.closures = {}
        for name, m in catalogue.maps.items():
            for index, obj in enumerate(m["object_events"]):
                script = obj.get("script", "")
                if script not in catalogue.blocks: continue
                labels, visited, handlers = self.reachable(script)
                owner = dict(map=name,index=index,script=script,flag=obj.get("flag","0"),
                             trainer=obj.get("trainer_type",""),x=obj["x"],y=obj["y"])
                binding = dict(owner,texts=labels,native_handlers=handlers)
                self.bindings.append(binding)
                for label in labels: self.records[label]["owners"].append(owner)
        self.active = [r for r in self.records.values() if r["owners"]]
        self.assets = sorted(root.glob("graphics/object_events/pics/**/*.png"))

    def reachable(self, script):
        if script in self.closures: return self.closures[script]
        pending, visited, texts, handlers = [script], set(), set(), set()
        while pending:
            label = pending.pop()
            if label in visited: continue
            visited.add(label)
            if label in self.records:
                texts.add(label)
                continue
            handlers.update(self.specials.get(label, []))
            pending.extend(self.edges.get(label, []))
        result=(sorted(texts), sorted(visited), sorted(handlers))
        self.closures[script]=result
        return result

    def summary(self):
        return dict(maps=len(self.cat.maps), npc_bindings=len(self.bindings), dialogue_blocks=len(self.active),
                    all_text_blocks=len(self.records), assets=len(self.assets),
                    scope="Static Hoenn NPC bindings; conditional branches are candidates, not proof of runtime reachability.")

    def search(self, query="", map_name="", limit=20):
        terms=query.casefold().split()
        matches=[]
        for record in self.active:
            if map_name and not any(o["map"]==map_name for o in record["owners"]): continue
            haystack=(record["label"]+" "+record["text"]+" "+" ".join(o["map"] for o in record["owners"])).casefold()
            if all(term in haystack for term in terms): matches.append(record)
        return dict(total=len(matches),results=matches[:min(limit,100)],scope=self.summary()["scope"])

    def graph(self, map_name, index):
        obj=self.cat.maps[map_name]["object_events"][index]
        texts, visited, handlers=self.reachable(obj["script"])
        nodes=[]
        for label in visited:
            path,start,end,body=self.cat.blocks[label]
            conditions=[line.strip() for line in body.splitlines()
                        if re.search(r"\b(?:goto_if|call_if|switch|case |checkitem|checkflag)",line)]
            nodes.append(dict(label=label,path=str(path.relative_to(self.root)),conditions=conditions,
                              targets=self.edges.get(label,[]),text=self.records.get(label,{}).get("text")))
        return dict(map=map_name,index=index,script=obj["script"],nodes=nodes,native_handlers=handlers,
                    scope="Static script references and native-handler boundaries, not executed path coverage.")

    def export(self, directory):
        directory.mkdir(parents=True,exist_ok=True)
        index=directory/"dialogue-index.json"
        index.write_text(json.dumps(dict(summary=self.summary(),dialogue=self.active,bindings=self.bindings),ensure_ascii=False,indent=2))
        by_map=defaultdict(list); shared=[]
        for r in self.active:
            maps=sorted({o["map"] for o in r["owners"]})
            if len(maps)>1: shared.append(r)
            else: by_map[maps[0]].append(r)
        lines=["# Emerald Studio — dialogue reading copy",
               "", "Generated from current NPC script bindings. Edit the source and canonical Game Book; this is a reading index.",
               "Conditional dialogue is included. Native-handler text and actual visual delivery need runtime inspection.", ""]
        for name in sorted(by_map):
            lines += ["## "+name,""]
            for r in sorted(by_map[name],key=lambda r:r["label"]):
                lines += ["### "+r["label"],r["path"]+":"+str(r["line"]),"",
                          "Actors: "+", ".join(sorted({o["script"] for o in r["owners"]})),"",r["text"],""]
        lines += ["## Shared service dialogue",""]
        for r in sorted(shared,key=lambda r:r["label"]):
            lines += ["### "+r["label"],r["path"]+":"+str(r["line"]),"",
                      "Maps: "+", ".join(sorted({o["map"] for o in r["owners"]})),"",r["text"],""]
        reading=directory/"dialogue-reading.md";reading.write_text("\n".join(lines))
        duplicate=defaultdict(list)
        for r in self.active:
            normalized=" ".join(r["text"].casefold().split())
            if len(normalized)>35: duplicate[normalized].append(r["label"])
        report=directory/"cohesion-review.json"
        report.write_text(json.dumps(dict(duplicate_text_candidates=[v for v in duplicate.values() if len(v)>1],
            native_handler_boundaries=[dict(map=b["map"],script=b["script"],handlers=b["native_handlers"])
                                       for b in self.bindings if b["native_handlers"]],
            scope="Review candidates only. Repeated service text can be intentional; no automatic deletion."),indent=2))
        return dict(index=str(index),reading=str(reading),cohesion=str(report),summary=self.summary())

    def asset_search(self, query="",limit=24):
        terms=query.casefold().split()
        matches=[p for p in self.assets if all(t in str(p.relative_to(self.root)).casefold() for t in terms)]
        return dict(total=len(matches),results=[dict(path=str(p.relative_to(self.root)),name=p.stem)
                                              for p in matches[:min(limit,100)]])

    def asset_sheet(self, paths, output):
        from importlib.util import spec_from_file_location,module_from_spec
        spec=spec_from_file_location("sheet_font",self.root/"scripts/audit/render_contact_sheet.py")
        module=module_from_spec(spec);spec.loader.exec_module(module)
        from PIL import ImageDraw
        tiles=[]
        for path in paths[:24]:
            p=(self.root/path).resolve()
            if p not in self.assets: raise ValueError("Choose an indexed sprite asset.")
            im=Image.open(p).convert("RGBA")
            # Source sprite sheets, never presented as captured gameplay.
            ratio=min(3,220/im.width,150/im.height)
            im=im.resize((max(1,int(im.width*ratio)),max(1,int(im.height*ratio))),Image.Resampling.NEAREST)
            tiles.append((path,im,hashlib.sha256(p.read_bytes()).hexdigest()))
        if not tiles: raise ValueError("No assets matched.")
        out=Image.new("RGB",(720,70+((len(tiles)+2)//3)*200),"#17241c");draw=ImageDraw.Draw(out)
        draw.text((15,15),"Source sprite atlas — not gameplay evidence",font=module.font(20),fill="white")
        for i,(path,im,digest) in enumerate(tiles):
            x=10+(i%3)*240;y=60+(i//3)*200
            out.paste(im,(x,y),im)
            draw.text((x,y+155),Path(path).stem[:28],font=module.font(14),fill="white")
        out.save(output)
        output.with_suffix(".json").write_text(json.dumps(dict(assets=[dict(path=p,sha256=h) for p,im,h in tiles],
            scope="Source sprite sheets with integer/fitted nearest-neighbor previews; not native scene evidence."),indent=2))
        return str(output)

    def history(self, path="", limit=20):
        if path:
            p=(self.root/path).resolve()
            if not p.is_relative_to(self.root): raise ValueError("Path outside project.")
            path=str(p.relative_to(self.root))
        cmd=["git","log","-n",str(min(limit,100)),"--format=%H%x00%aI%x00%s","--"]
        if path: cmd.append(path)
        out=subprocess.check_output(cmd,cwd=self.root,text=True)
        return [dict(commit=a,date=b,subject=c) for a,b,c in (line.split("\0",2) for line in out.splitlines())]
