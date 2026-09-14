"""Frame-exact input traces and compact visual evidence from real emulator pixels."""
from collections import deque
import hashlib
import json
from pathlib import Path
import re
import struct
import subprocess
import sys
import time
from PIL import Image

BUTTONS={"A":1,"B":2,"SELECT":4,"START":8,"RIGHT":16,"LEFT":32,"UP":64,"DOWN":128,"R":256,"L":512}
def keys(value):
    if isinstance(value,int):
        if not 0<=value<=1023: raise ValueError("Invalid button mask.")
        return value
    if isinstance(value,str): value=value.replace("+"," ").upper().split()
    result=0
    for key in value or []: result|=BUTTONS[key.upper()]
    return result

class TextDecoder:
    def __init__(self,root):
        self.chars={}
        for line in (root/"charmap.txt").read_text().splitlines():
            match=re.match(r"^'(.*)'\s*=\s*([A-Fa-f0-9]{2})\s*(?:@.*)?$",line.strip())
            if match:
                text=match[1].replace(r"\'","'").replace(r'\"','"')
                if not text.startswith("\\"): self.chars.setdefault(int(match[2],16),text)
    def decode(self,raw):
        out=[];i=0
        while i<len(raw):
            value=raw[i];i+=1
            if value==255: break
            if value in (250,251,254): out.append("\n");continue
            if value==253:
                if i<len(raw): out.append("{VAR_"+str(raw[i])+"}");i+=1
                continue
            if value==252:
                # Preserve controls as annotations rather than guessing visible glyphs.
                if i<len(raw): out.append("{CTRL_"+str(raw[i])+"}");i+=1
                continue
            out.append(self.chars.get(value,"�"))
        return "".join(out)

def packet_state(packet,decoder=None):
    frame,cost,samples,count=struct.unpack_from("<IIII",packet)
    values=struct.unpack_from("<"+"I"*count,packet,153616+samples*4)
    result=dict(frame=frame,ready=bool(values[0]),battle=bool(values[1]),
                group=values[2],num=values[3],x=values[4],y=values[5],facing=values[6],
                npc=values[7],cap=values[8],difficulty=values[9],script=bool(values[11]),
                text_serial=values[30],text_length=values[31],actors=[],text="")
    if count>=256:
        for i in range(16):
            local,graphic,x,y,facing,flags=values[32+i*6:38+i*6]
            if local:
                signed=lambda v:v-4294967296 if v>=2147483648 else v
                result["actors"].append(dict(local_id=local-1,graphic=graphic,x=signed(x),y=signed(y),
                                             facing=facing,invisible=bool(flags&1),moving=bool(flags&4)))
        raw=struct.pack("<128I",*values[128:256])
        if decoder:result["text"]=decoder.decode(raw[:min(values[31],512)])
    return result

class Recorder:
    def __init__(self,root,directory,name,build,initial,portable=None,parent=None):
        directory=directory.resolve()
        self.root,self.directory,self.name,self.build=root,directory,name,build
        directory.mkdir(parents=True,exist_ok=True)
        self.decoder=TextDecoder(root)
        self.initial,self.portable,self.parent=initial,portable,parent
        self.inputs=[];self.frames=[];self.events=[];self.markers=[]
        self.length=0;self.last=None;self.last_pixels=None;self.pending_capture=False
        self.raw=[];self.started=time.time()
    def observe(self,packet,buttons=0,force=False,label=None):
        state=packet_state(packet,self.decoder)
        pixels=bytes(packet[16:153616])
        if self.length and self.inputs and self.inputs[-1]["keys"]==buttons:
            self.inputs[-1]["frames"]+=1
        elif self.length: self.inputs.append(dict(frame=self.length-1,keys=buttons,frames=1))
        changes=[]
        if self.last:
            for key in ("ready","battle","group","num","script","text_serial"):
                if state[key]!=self.last[key]: changes.append(key)
            if state["actors"]!=self.last["actors"]:changes.append("actors")
        if changes:
            self.events.append(dict(frame=self.length,changes=changes,state=state))
            self.pending_capture=True
        capture=force or self.length==0 or self.length%6==0
        digest=hashlib.sha256(pixels).hexdigest()
        if capture and (force or digest!=self.last_pixels):
            item=(self.length,pixels,state,label or "")
            # Keep animation frames for motion, but classify visually substantial
            # changes separately so a blinking prompt never fills a contact sheet.
            self.raw.append(item)
            self.last_pixels=digest;self.pending_capture=False
        self.last=state
        self.length+=1
    def mark(self,label,packet):
        self.markers.append(dict(frame=self.length-1,label=label[:70]))
        self.raw.append((self.length-1,bytes(packet[16:153616]),packet_state(packet,self.decoder),label[:70]))
    def finish(self,packet,outcome=None):
        self.mark("End",packet)
        frames=[]
        for i,(frame,pixels,state,label) in enumerate(self.raw):
            path=self.directory/f"frame-{i:05d}.png"
            Image.frombytes("RGBA",(240,160),pixels).save(path)
            frames.append(dict(frame=frame,path=path.name,label=label,state=state,
                               sha256=hashlib.sha256(path.read_bytes()).hexdigest()))
        spec=dict(schema=1,name=self.name,created=self.started,build=self.build,initial=self.initial,
                  portable=self.portable,parent=self.parent,frames_run=self.length-1,inputs=self.inputs,
                  markers=self.markers,events=self.events,captures=frames,outcome=outcome or {},
                  scope="Synthetic Studio session; native execution and pixels, not earned campaign progress.")
        (self.directory/"recording.json").write_text(json.dumps(spec,ensure_ascii=False,indent=2))
        selected=select_frames(frames,12,self.directory)
        sheets=make_sheets(self.root,self.directory,self.name,self.build,selected,spec["scope"])
        # Lossless motion preview is secondary; the PNGs/contact sheets own visual evidence.
        if len(frames)>1:
            images=[Image.open(self.directory/f["path"]).convert("RGBA") for f in frames]
            durations=[max(17,round((frames[i+1]["frame"]-f["frame"])*1000/59.7275)) if i+1<len(frames) else 600
                       for i,f in enumerate(frames)]
            images[0].save(self.directory/"motion.webp",save_all=True,append_images=images[1:],duration=durations,loop=0,lossless=True)
        result=dict(id=self.directory.name,name=self.name,frames=self.length-1,seconds=round((self.length-1)/59.7275,2),
                    build=self.build["rom_sha256"],recording=str(self.directory/"recording.json"),
                    sheets=sheets,motion=str(self.directory/"motion.webp") if len(frames)>1 else None,
                    outcome=outcome or {},scope=spec["scope"])
        (self.directory/"result.json").write_text(json.dumps(result,indent=2))
        return result

def select_frames(frames,limit,directory=None):
    if directory and len(frames)>limit:
        candidates=[frames[0]]
        last=Image.open(directory/frames[0]["path"]).convert("RGB")
        for f in frames[1:]:
            current=Image.open(directory/f["path"]).convert("RGB")
            changed=sum(a!=b for a,b in zip(last.getdata(),current.getdata()))
            if f["label"] or (changed>450 and f["frame"]-candidates[-1]["frame"]>=24):
                candidates.append(f);last=current
        if candidates[-1] is not frames[-1]:candidates.append(frames[-1])
        frames=candidates
    if len(frames)<=limit:return frames
    chosen={0,len(frames)-1}
    chosen.update(i for i,f in enumerate(frames) if f["label"])
    # Keep the last available view of each dialogue buffer before it changes.
    for i,f in enumerate(frames[:-1]):
        if f["state"]["text_serial"]!=frames[i+1]["state"]["text_serial"]:chosen.add(i)
    remaining=max(0,limit-len(chosen))
    if remaining:
        chosen.update(round(i*(len(frames)-1)/(remaining+1)) for i in range(1,remaining+1))
    # Long conversations are split into readable pages, never tiny thumbnails.
    return [frames[i] for i in sorted(chosen)]

def make_sheets(root,directory,title,build,frames,scope,columns=3):
    sheets=[]
    for page,start in enumerate(range(0,len(frames),12),1):
        panels=[]
        for f in frames[start:start+12]:
            label=f["label"] or ("Dialogue" if f["state"]["script"] else "Movement / field")
            panels.append(dict(path=str(directory/f["path"]),label=f'{label[:48]} · +{f["frame"]}f'))
        manifest=directory/f"sheet-{page:02d}-input.json"
        manifest.write_text(json.dumps(dict(title=title[:70],scope=scope[:110],build=build,panels=panels,
                                           evidence=[str(directory/"recording.json")]),indent=2))
        out=directory/f"sheet-{page:02d}.png"
        subprocess.run([sys.executable,str(root/"scripts/audit/render_contact_sheet.py"),str(manifest),
                        "--out",str(out),"--columns",str(columns),"--scale","2"],check=True,capture_output=True)
        sheets.append(str(out))
    return sheets

def compare(root,before,after,directory):
    a=json.loads((before/"recording.json").read_text());b=json.loads((after/"recording.json").read_text())
    directory.mkdir(parents=True,exist_ok=True)
    panels=[];changes=[]
    # Explicit marker labels align semantic moments across differently timed runs.
    marks={f["label"]:f for f in b["captures"] if f["label"]}
    for i,f in enumerate(select_frames(a["captures"],10,before)):
        match=marks.get(f["label"]) if f["label"] else None
        alignment="marker" if match else "relative frame"
        if not match:
            target=f["frame"]*max(1,b["frames_run"])/max(1,a["frames_run"])
            match=min(b["captures"],key=lambda x:abs(x["frame"]-target))
        for side,base,item in (("Before",before,f),("After",after,match)):
            im=Image.open(base/item["path"]).convert("RGB")
            path=directory/f"pair-{i:02d}-{side.lower()}.png";im.save(path)
            panels.append(dict(frame=item["frame"],path=path.name,label=side+" · "+(f["label"] or "sample"),
                               state=item["state"]))
        old=Image.open(before/f["path"]).convert("RGB");new=Image.open(after/match["path"]).convert("RGB")
        different=sum(x!=y for x,y in zip(old.getdata(),new.getdata()))
        changes.append(dict(before_frame=f["frame"],after_frame=match["frame"],alignment=alignment,
                            changed_pixels=different,total_pixels=38400))
    build=b["build"]
    sheets=make_sheets(root,directory,"Before / after · "+b["name"],build,panels,
                       "Left: original build. Right: revised build. Original pixels; changes need visual judgment.",2)
    result=dict(before=str(before),after=str(after),before_build=a["build"],after_build=b["build"],
                sheets=sheets,changes=changes,scope="Pixel differences locate changes; they do not decide visual quality.")
    (directory/"comparison.json").write_text(json.dumps(result,indent=2))
    return result
