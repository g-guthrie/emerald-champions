function artifactUrl(path){return "/artifacts/"+path.split("/work/studio/").pop().split("/").map(encodeURIComponent).join("/");}
function element(tag,text,className){const e=document.createElement(tag);if(text!==undefined)e.textContent=text;if(className)e.className=className;return e;}
function showSheets(container,paths){
  container.replaceChildren();
  for(const path of paths||[]){const link=element("a");link.href=artifactUrl(path);link.target="_blank";const img=element("img");img.src=link.href;img.alt="Native scene contact sheet";link.append(img);container.append(link);}
}
function libraryView(id){
  document.querySelectorAll(".library-view").forEach(e=>e.classList.toggle("hidden",e.id!==id));
  document.querySelectorAll("#libraryTabs button").forEach(e=>e.classList.toggle("active",e.dataset.view===id));
}
document.querySelectorAll("#libraryTabs button").forEach(b=>b.onclick=()=>libraryView(b.dataset.view));
$("closeLibrary").onclick=()=>$("libraryDialog").close();
async function refreshLibrary(){
  const result=await(await fetch("/api/library")).json();
  $("librarySummary").textContent=result.summary.npc_bindings+" NPC bindings · "+result.summary.dialogue_blocks+" dialogue blocks · "+result.summary.assets+" source sprite sheets";
  $("sceneCards").replaceChildren();$("compareBefore").replaceChildren();$("compareAfter").replaceChildren();
  for(const scene of result.scenes){
    const card=element("article",undefined,"scene-card");card.append(element("h3",scene.name));
    card.append(element("p",scene.seconds+" seconds · build "+scene.build.slice(0,12)+" · "+(scene.provenance||"legacy provenance"),"hint"));
    if(scene.sheets?.[0]){const img=element("img");img.src=artifactUrl(scene.sheets[0]);img.alt=scene.name+" contact sheet";const link=element("a");link.href=img.src;link.target="_blank";link.append(img);card.append(link);}
    const row=element("div",undefined,"library-actions");
    for(const[label,mode]of[["Replay original","exact"],["Test latest build","latest"]]){
      const b=element("button",label);b.onclick=async()=>{try{await api("scene.replay",{id:scene.id,mode});toast("Headless scene test started; your live game is untouched.");}catch(e){toast(e.message);}};row.append(b);
    }
    if(scene.motion){const a=element("a","Motion preview");a.href=artifactUrl(scene.motion);a.target="_blank";row.append(a);}
    const a=element("a","Trace");a.href=artifactUrl(scene.recording);a.target="_blank";row.append(a);card.append(row);
    if(scene.outcome?.passed===false)card.append(element("p",scene.outcome.failures.join("; "),"failure"));
    $("sceneCards").append(card);$("compareBefore").add(new Option(scene.name,scene.id));$("compareAfter").add(new Option(scene.name,scene.id));
  }
  $("situations").replaceChildren();
  for(const s of result.situations){
    const b=element("button",s.name+" · "+nice(s.initial.map));
    b.onclick=()=>api("situation.load",{id:s.id}).then(()=>$("libraryDialog").close()).catch(e=>toast(e.message));
    $("situations").append(b);
  }
}
action("openLibrary",async()=>{$("libraryDialog").showModal();await refreshLibrary();});
action("refreshScenes",refreshLibrary);
action("recordScene",async()=>{
  if(state.recording){const r=await api("record.stop");toast("Contact sheets and motion preview are ready.");$("libraryDialog").showModal();libraryView("sceneLibrary");await refreshLibrary();}
  else{await api("record.start",{name:$("sceneName").value||nice(state.map)+" interaction"});toast("Recording native frames and inputs. Play the scene, then stop.");$("screen").focus();}
});
action("markScene",()=>api("record.mark",{label:$("sceneName").value||"Marked moment"}));
action("rewind",async()=>{await api("rewind",{seconds:2});audioNode?.port.postMessage({reset:true});$("screen").focus();});
action("saveSituation",async()=>{await api("situation.save",{name:$("sceneName").value||nice(state.map)+" situation"});await refreshLibrary();});
action("compareScenes",async()=>{const result=await api("scene.compare",{before:$("compareBefore").value,after:$("compareAfter").value});showSheets($("comparisonSheets"),result.sheets);});
action("searchDialogue",async()=>{
  const result=await api("dialogue.search",{query:$("dialogueQuery").value,limit:40});
  $("dialogueResults").replaceChildren(element("p",result.total+" matching dialogue blocks","hint"));
  for(const r of result.results){
    const row=element("article",undefined,"dialogue-result");row.append(element("h3",r.label),element("p",r.path+":"+r.line,"source-path"),element("pre",r.text));
    const owners=element("div",undefined,"library-actions");
    for(const owner of r.owners.slice(0,5)){
      const b=element("button",nice(owner.map)+(owner.kind&&owner.kind!=="object"?" · "+nice(owner.kind):""));
      b.onclick=async()=>{
        await loadMap(owner.map);
        if(!owner.kind||owner.kind==="object"){
          await inspectNpc(owner.index);const i=npcData.dialogues.findIndex(d=>d.label===r.label);
          if(i>=0){$("dialogueSelect").value=i;showDialogue();}$("libraryDialog").close();
        }else{
          const graph=await api("scene.graph",{map:owner.map,index:owner.index,kind:owner.kind});
          libraryView("historyLibrary");$("historyResults").replaceChildren(element("pre",JSON.stringify(graph,null,2)));
        }
      };
      owners.append(b);
    }row.append(owners);$("dialogueResults").append(row);
  }
});
$("dialogueQuery").onkeydown=e=>{if(e.key==="Enter")$("searchDialogue").click();};
action("exportDialogue",async()=>{
  const r=await api("dialogue.export");$("readingLinks").replaceChildren();
  for(const[label,path]of[["Combined reading copy",r.reading],["Dialogue index",r.index],["Cohesion review candidates",r.cohesion]]){
    const a=element("a",label);a.href=artifactUrl(path);a.target="_blank";$("readingLinks").append(a);
  }
});
action("searchAssets",async()=>{
  const r=await api("assets",{query:$("assetQuery").value});$("assetResults").replaceChildren();
  for(const a of r.results){const card=element("article");const img=element("img");img.src="/api/sprite?path="+encodeURIComponent(a.path);img.alt=a.name;card.append(img,element("p",a.path,"source-path"));$("assetResults").append(card);}
});
action("assetSheet",async()=>{const r=await api("assets.sheet",{query:$("assetQuery").value});showSheets($("assetResults"),[r.sheet]);});
action("searchHistory",async()=>{
  const rows=await api("history",{path:$("historyPath").value});$("historyResults").replaceChildren();
  for(const r of rows){const item=element("article",undefined,"dialogue-result");item.append(element("h3",r.subject),element("p",r.date+" · "+r.commit.slice(0,12),"hint"));$("historyResults").append(item);}
});
action("buildHistory",async()=>{
  const r=await api("builds");$("historyResults").replaceChildren();
  for(const b of r.builds){const item=element("article",undefined,"dialogue-result");item.append(element("h3",b.rom_sha256.slice(0,12)),element("p",b.created+" · "+b.provenance_label+(b.commit?" · base "+b.commit.slice(0,12):""),"hint"),element("pre",(b.changed_files||[]).join("\n")));$("historyResults").append(item);}
});
action("sceneGraph",async()=>{
  if(selectedNpc===null)throw Error("Select an NPC first.");
  const graph=await api("scene.graph",{map:map.name,index:selectedNpc});
  $("libraryDialog").showModal();libraryView("historyLibrary");$("historyResults").replaceChildren(element("pre",JSON.stringify(graph,null,2)));
});
