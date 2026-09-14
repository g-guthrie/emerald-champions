const $ = id => document.getElementById(id);
const ctx = $("screen").getContext("2d", {alpha:false, desynchronized:true});
ctx.imageSmoothingEnabled = false;
let socket, state={}, catalog={}, map=null, selectedNpc=null, npcData=null, audioContext, audioNode, soundOn=false;
let keyMask=0, keyboardMask=0, padMask=0, rendered=0, firstFrame=true, lastMap="", lastHistory="";
const KEY = {KeyZ:1, KeyX:2, ShiftLeft:4, ShiftRight:4, Enter:8, ArrowRight:16, ArrowLeft:32, ArrowUp:64, ArrowDown:128, KeyE:256, KeyQ:512};
let toastTimer;
let mapImage=null;
let lastBuild="";
function toast(text){$("toast").textContent=text;$("toast").style.display="block";clearTimeout(toastTimer);toastTimer=setTimeout(()=>$("toast").style.display="none",6000);}
async function api(op, extra={}) {
  const res=await fetch("/api/command",{method:"POST",headers:{"Content-Type":"application/json","X-Studio-Token":window.STUDIO_TOKEN},body:JSON.stringify({op,...extra})});
  const data=await res.json();if(!res.ok)throw Error(data.error||res.statusText);return data;
}
function action(id,fn){$(id).onclick=async()=>{try{await fn();await poll();}catch(e){toast(e.message);}};}
function connect(){
  socket=new WebSocket("ws://"+location.host+"/ws?token="+encodeURIComponent(window.STUDIO_TOKEN));socket.binaryType="arraybuffer";
  socket.onopen=()=>{$("connection").textContent="Connected";$("connectionDot").classList.add("on");sendKeys(true);};
  socket.onclose=()=>{$("connection").textContent="Reconnecting";$("connectionDot").classList.remove("on");setTimeout(connect,1500);};
  socket.onmessage=({data})=>{
    if(!(data instanceof ArrayBuffer))return;
    const view=new DataView(data),samples=view.getUint32(8,true);
    ctx.putImageData(new ImageData(new Uint8ClampedArray(data,16,153600),240,160),0,0);rendered++;
    if(firstFrame){firstFrame=false;$("screenMessage").classList.add("hidden");}
    if(soundOn&&audioContext?.state==="running"&&state.speed===1&&samples){
      const pcm=data.slice(153616,153616+samples*4);audioNode.port.postMessage(pcm,[pcm]);
    }
    socket.send('{"ack":true}');
  };
}
function sendKeys(force=false){
  const mask=keyboardMask|padMask;
  if(socket?.readyState===1&&(force||mask!==keyMask)){keyMask=mask;socket.send(JSON.stringify({keys:mask}));}
}
const held=new Set();
function editable(target){return /INPUT|TEXTAREA|SELECT/.test(target.tagName)||target.isContentEditable;}
window.addEventListener("keydown",e=>{if(editable(e.target)||!KEY[e.code])return;e.preventDefault();held.add(e.code);keyboardMask=[...held].reduce((m,k)=>m|KEY[k],0);sendKeys();});
window.addEventListener("keyup",e=>{if(!KEY[e.code])return;held.delete(e.code);keyboardMask=[...held].reduce((m,k)=>m|KEY[k],0);sendKeys();});
function release(){held.clear();keyboardMask=padMask=0;sendKeys(true);audioNode?.port.postMessage({reset:true});}
window.addEventListener("blur",release);
document.addEventListener("visibilitychange",()=>{if(document.hidden)release();});
$("screen").onclick=()=>{$("screen").focus();};
function gamepad(){
  const pad=navigator.getGamepads?.()[0];let mask=0;
  if(pad&&!document.hidden&&!editable(document.activeElement)){
    [[0,1],[1,2],[8,4],[9,8],[12,64],[13,128],[14,32],[15,16],[4,512],[5,256]].forEach(([b,k])=>{if(pad.buttons[b]?.pressed)mask|=k;});
    if(pad.axes[0]>.5)mask|=16;if(pad.axes[0]<-.5)mask|=32;if(pad.axes[1]>.5)mask|=128;if(pad.axes[1]<-.5)mask|=64;
  }
  if(mask!==padMask){padMask=mask;sendKeys();}requestAnimationFrame(gamepad);
}
document.querySelectorAll("[data-key]").forEach(b=>{
  b.onpointerdown=e=>{e.preventDefault();b.setPointerCapture(e.pointerId);keyboardMask|=+b.dataset.key;sendKeys();};
  b.onpointerup=b.onpointercancel=()=>{keyboardMask&=~+b.dataset.key;sendKeys();};
});
action("sound",async()=>{
  if(!audioContext){audioContext=new AudioContext({latencyHint:"interactive"});await audioContext.audioWorklet.addModule("/audio.js");audioNode=new AudioWorkletNode(audioContext,"emerald-audio",{outputChannelCount:[2]});audioNode.connect(audioContext.destination);}
  soundOn=!soundOn;audioNode.port.postMessage({reset:true});
  if(soundOn)await audioContext.resume();else await audioContext.suspend();
  $("sound").textContent=soundOn?"♪ Sound on":"♪ Enable sound";$("screen").focus();
});
action("pause",async()=>{await api("pause");audioNode?.port.postMessage({reset:true});});
action("retry",async()=>{await api("retry");audioNode?.port.postMessage({reset:true});$("screen").focus();});
action("build",async()=>{await api("build");toast("Building in the background. Keep playing.");});
action("capture",async()=>{const r=await api("screenshot");toast("Screenshot saved: "+r.path);});
action("bookmark",async()=>{await api("checkpoint",{label:$("bookmarkName").value||"Bookmark"});$("bookmarkName").value="";toast("Bookmark saved.");});
action("heal",async()=>{await api("heal");toast("Party healed.");});
$("speed").onchange=async()=>{try{await api("speed",{value:+$("speed").value});audioNode?.port.postMessage({reset:true});}catch(e){toast(e.message);}};
$("difficulty").onchange=async()=>{try{await api("difficulty",{value:+$("difficulty").value});}catch(e){toast(e.message);}};
document.querySelectorAll(".tab").forEach(b=>b.onclick=()=>{
  document.querySelectorAll(".tab").forEach(x=>x.classList.toggle("active",x===b));
  document.querySelectorAll(".panel").forEach(x=>x.classList.toggle("hidden",x.id!==b.dataset.panel));
});
function nice(name){return name.replace(/_/g," ").replace(/([a-z])([A-Z])/g,"$1 $2");}
async function loadMap(name){
  if(!catalog.maps?.includes(name))return;
  map=await(await fetch("/api/map?name="+encodeURIComponent(name))).json();
  mapImage=null;
  const picture=new Image();
  picture.onload=()=>{if(map?.name===name){mapImage=picture;drawMap();}};
  picture.src="/api/map-image?name="+encodeURIComponent(name);
  $("mapTitle").textContent=nice(name);$("mapSearch").value=name;
  $("npcSelect").replaceChildren(new Option("Select an NPC",""));
  for(const o of map.objects)if(o.script&&!o.script.startsWith("0x"))
    $("npcSelect").add(new Option(nice(o.script.replace(/.*EventScript_/,"")),o.index));
  selectedNpc=null;drawMap();
}
function drawMap(){
  if(!map)return;
  const c=$("mapCanvas"),scale=5;c.width=map.width*scale;c.height=map.height*scale;const g=c.getContext("2d");
  for(let y=0;y<map.height;y++)for(let x=0;x<map.width;x++){g.fillStyle=map.collision[y*map.width+x]?"#344038":((x+y)%2?"#19271e":"#1c2b21");g.fillRect(x*scale,y*scale,scale,scale);}
  if(mapImage){g.imageSmoothingEnabled=false;g.drawImage(mapImage,0,0,c.width,c.height);}
  for(const w of map.warps){g.fillStyle="#85b5db";g.fillRect(w.x*scale,w.y*scale,scale,scale);}
  for(const o of map.objects){g.fillStyle=o.index===selectedNpc?"#ffffff":o.trainer_type==="TRAINER_TYPE_NORMAL"?"#dfa36e":"#b6c89b";g.fillRect(o.x*scale,o.y*scale,scale,scale);}
  if(state.map===map.name){g.fillStyle="#75f4a4";g.beginPath();g.arc(state.x*scale+2.5,state.y*scale+2.5,3,0,7);g.fill();}
}
$("mapCanvas").onclick=async e=>{
  if(!map)return;const r=e.target.getBoundingClientRect(),x=Math.floor((e.clientX-r.left)/r.width*map.width),y=Math.floor((e.clientY-r.top)/r.height*map.height);
  try{if(e.shiftKey){await api("warp",{map:map.name,x,y});$("screen").focus();}
  else{const o=map.objects.find(o=>o.x===x&&o.y===y);if(o)await inspectNpc(o.index);else toast("Shift-click to move to this tile.");}}catch(err){toast(err.message);}
};
$("mapSearch").onchange=()=>loadMap($("mapSearch").value).catch(e=>toast(e.message));
action("follow",()=>loadMap(state.map));
$("npcSelect").onchange=()=>{if($("npcSelect").value!=="")inspectNpc(+$("npcSelect").value).catch(e=>toast(e.message));};
async function inspectNpc(index){
  document.querySelector("main").classList.add("inspecting");
  selectedNpc=index;$("npcSelect").value=index;drawMap();
  npcData=await(await fetch("/api/npc?map="+encodeURIComponent(map.name)+"&index="+index)).json();
  $("npcTitle").textContent=nice(npcData.object.script.replace(/.*EventScript_/,""));
  $("npcHint").textContent=nice(map.name)+" · "+npcData.object.x+", "+npcData.object.y;
  $("dialogueSelect").replaceChildren();
  npcData.dialogues.forEach((d,i)=>$("dialogueSelect").add(new Option(nice(d.label.replace(/.*Text_/,"")),i)));
  $("editor").classList.toggle("hidden",!npcData.dialogues.length);
  if(npcData.dialogues.length)showDialogue();else $("npcHint").textContent+=" · No directly editable dialogue found.";
}
function showDialogue(){const d=npcData.dialogues[+$("dialogueSelect").value];$("dialogue").value=d.text;$("sourcePath").textContent=d.path+"\n"+d.label;}
$("backWorld").onclick=()=>document.querySelector("main").classList.remove("inspecting");
$("dialogueSelect").onchange=showDialogue;
action("goNpc",async()=>{
  if(selectedNpc===null)throw Error("Select an NPC first.");const o=map.objects.find(x=>x.index===selectedNpc);
  for(const [dx,dy,facing]of[[0,1,2],[0,-1,1],[1,0,3],[-1,0,4]]){
    const x=o.x+dx,y=o.y+dy;
    if(x>=0&&y>=0&&x<map.width&&y<map.height&&!map.collision[y*map.width+x]&&!map.objects.some(a=>a.x===x&&a.y===y)){
      await api("warp",{map:map.name,x,y,facing});$("screen").focus();return;
    }
  }throw Error("No clear adjacent tile. Choose a nearby tile on the map.");
});
action("applyDialogue",async()=>{
  const d=npcData.dialogues[+$("dialogueSelect").value];
  await api("dialogue",{label:d.label,hash:d.hash,text:$("dialogue").value});toast("Dialogue saved. Building and returning to your interaction…");
});
action("applyParty",async()=>{
  const names=[...document.querySelectorAll("#partySlots input")].map(i=>i.value.trim()).filter(Boolean);
  const party=names.map(n=>{const s=catalog.species.find(s=>s.name.toLowerCase()===n.toLowerCase()||s.id===n);if(!s)throw Error("Unknown Pokémon: "+n);return s.id;});
  await api("party",{party});toast("Party prepared at the current level cap.");
});
action("jumpChapter",()=>api("chapter",{value:+$("chapter").value}));
function selectedTrainer(){return catalog.trainers.find(t=>t.id===$("trainer").value||t.name+" · "+t.id===$("trainer").value);}
$("trainer").onchange=()=>{const t=selectedTrainer();$("trainerTeam").textContent=t?t.team.map(p=>p.species+" · "+p.item).join("\n"):"";};
action("startBattle",async()=>{const t=selectedTrainer();if(!t)throw Error("Choose a trainer from the list.");await api("trainer",{id:t.id});$("screen").focus();});
async function poll(){
  try{state=await(await fetch("/api/state")).json();
    $("location").textContent=nice(state.map||"Emerald Champions");$("status").textContent=state.status;
    $("gameState").textContent=state.paused?"PAUSED":state.battle?"IN BATTLE":state.ready?"EXPLORING":"INTERACTION";
    $("details").textContent="Level cap "+state.cap+" · "+(["Easy","Medium","Hard"][state.difficulty]||"Medium")+" · Automatic checkpoint before each interaction";
    $("performance").textContent=state.native_ms+" ms / frame";
    $("recordScene").textContent=state.recording?"■ Stop · "+(state.recording.frames/59.7275).toFixed(1)+"s":"● Record";
    $("recordScene").classList.toggle("recording",!!state.recording);
    $("markScene").disabled=!state.recording;
    $("liveText").textContent=state.observed?.text||"No dialogue observed.";
    $("sceneJobs").textContent=(state.jobs||[]).map(j=>j.name+" · "+j.status+(j.error?" · "+j.error:"")).join("\n");
    $("buildId").textContent="BUILD "+state.build;$("coords").textContent=state.map+" · "+state.x+", "+state.y;
    $("pause").textContent=state.paused?"▶ Resume":"Ⅱ Pause";$("build").disabled=$("applyDialogue").disabled=state.building;
    $("build").textContent=state.building?"Building…":"↻ Build & apply";$("buildLog").textContent=state.log||"No build running.";
    if(lastBuild&&state.build!==lastBuild&&npcData&&selectedNpc!==null){
      const fresh=await(await fetch("/api/npc?map="+encodeURIComponent(map.name)+"&index="+selectedNpc)).json();
      for(const d of npcData.dialogues){const current=fresh.dialogues.find(x=>x.label===d.label);if(current)d.hash=current.hash;}
    }
    lastBuild=state.build;
    if(document.activeElement!==$("difficulty"))$("difficulty").value=state.difficulty;
    $("partyReadout").replaceChildren(...state.party.map(p=>{const d=document.createElement("div");d.textContent=p.species+" · Lv. "+p.level+" · "+p.hp+" HP";return d;}));
    if(state.map&&state.map!==lastMap){lastMap=state.map;await loadMap(state.map);}else drawMap();
    const history=JSON.stringify(state.snapshots);
    if(history!==lastHistory){lastHistory=history;$("bookmarks").replaceChildren(...[...state.snapshots].reverse().map(s=>{
      const b=document.createElement("button");b.className="bookmark-row";b.textContent=s.label;
      const small=document.createElement("small");small.textContent=nice(s.map)+" · "+s.x+", "+s.y;b.append(small);
      if(s.build.slice(0,12)!==state.build)small.textContent+=" · earlier build";
      b.onclick=()=>api("restore",{id:s.id}).catch(e=>toast(e.message));return b;
    }));}
    window.studioMetrics={rendered,nativeMs:state.native_ms,audioState:audioContext?.state,connected:socket?.readyState===1};
  }catch(e){$("status").textContent="Waiting for the local studio…";}
}
async function init(){
  catalog=await(await fetch("/api/catalog")).json();
  $("mapNames").replaceChildren(...catalog.maps.map(n=>new Option(n,n)));
  catalog.species.sort((a,b)=>a.name.localeCompare(b.name));
  $("trainerNames").replaceChildren(...catalog.trainers.map(t=>new Option(t.name+" · "+t.id,t.name+" · "+t.id)));
  $("speciesNames").replaceChildren(...catalog.species.map(s=>new Option(s.name,s.name)));
  for(let i=0;i<6;i++){const input=document.createElement("input");input.setAttribute("list","speciesNames");input.placeholder="Pokémon "+(i+1);input.setAttribute("aria-label","Party member "+(i+1));$("partySlots").append(input);}
  const locations=[["Oldale Center","OldaleTown_PokemonCenter_1F",8,6],["Rustboro","RustboroCity",27,20],["Route 116","Route116",8,12],["Rusturf Tunnel","RusturfTunnel",4,9],["Dewford","DewfordTown",8,18],["Roxanne","RustboroCity_Gym",5,4]];
  for(const[label,name,x,y]of locations){const b=document.createElement("button");b.textContent=label+" ↗";b.onclick=()=>api("warp",{map:name,x,y}).catch(e=>toast(e.message));$("locations").append(b);}
  await poll();connect();gamepad();setInterval(poll,700);
}
init().catch(e=>toast(e.message));
