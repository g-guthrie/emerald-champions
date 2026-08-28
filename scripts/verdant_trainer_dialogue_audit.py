#!/usr/bin/env python3
"""Audit trainer dialogue against Verdant's authored battle formats."""

from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
FORMAT_MANIFEST = ROOT / "docs/verdant_doubles_manifest.json"

DOUBLE_LANGUAGE = re.compile(
    r"\b(double battle|two[- ]on[- ]two|two at once|tag team|pair of pok[eé]mon|two pok[eé]mon|both pok[eé]mon)\b",
    re.I,
)
SINGLE_LANGUAGE = re.compile(
    r"\b(single battle|one[- ]on[- ]one|one pok[eé]mon at a time|solo battle)\b",
    re.I,
)

STRATEGY_CUES = {
    "Route116_Text_JoeyPostBattle": "Crabrawler's Anger Point",
    "Route116_Text_JosePostBattle": "Charjabug's Battery",
    "Route116_Text_DevanPostBattle": "Drilbur races through it",
    "Route116_Text_SarahPostBattle": "Diancie twists the room",
    "Route116_Text_DawsonPostBattle": "Wooloo curls before it rolls",
    "Route116_Text_JanicePostBattle": "Pincurchin electrifies the field",
    "Route116_Text_JerryPostBattle": "Klink leads when our circuits join",
    "Route116_Text_KarenPostBattle": "Wonder Room trades Defense",
    "Route116_Text_ClarkPostBattle": "Gravity makes Rhyhorn",
    "Route116_Text_JohnsonPostBattle": "Friend Guard cushions",
    "RusturfTunnel_Text_GruntTakePackage": "raise every stat",
    "RustboroCity_Text_MayImNotGoingToLose": "Read the forecast",
    "RustboroCity_Text_BrendanIWontGoEasy": "Read the forecast",
    "Route104_Text_MayIntro": "Read the forecast",
    "Route104_Text_BrendanIntro": "Read the forecast",
    "Route104_Text_IvanPostBattle": "schools at level 20",
    "Route104_Text_MiaIntro": "follows each dance",
    "RustboroCity_Gym_Text_JoshPostBattle": "Guard Split lends",
    "RustboroCity_Gym_Text_TommyPostBattle": "Instruct makes my partner repeat",
    "RustboroCity_Gym_Text_MarcPostBattle": "Cursed Body seals that one move",
    "RustboroCity_Gym_Text_RoxannePostBattle": "Safeguard prevents confusion",
    "Route107_Text_BethIntro": "Rain falls first",
    "Route112_Text_ShaylaIntro": "Sunlight and flowers",
    "Route114_Text_LennyIntro": "Four voices answer from these peaks",
    "MeteorFalls_1F_1R_Text_CourtneyLetsBattle": "Trajectory. Collision. Fracture.",
    "Route117_Text_AnnaIntro": "strengthens our footing",
    "Route117_Text_MegIntro": "If Muk joins us",
    "Route117_Text_IsaacIntro": "guardian joins the class",
    "Route117_Text_DylanIntro": "partner sets the pace",
    "Route117_Text_MariaIntro": "Speed above all",
    "Route121_Text_KateIntro": "shut down every Ability",
    "Route121_Text_JoyIntro": "raw power",
    "Route133_Text_LindaIntro": "current turns, then snow",
    "MossdeepCity_Gym_Text_CliffordIntro": "floor carries our current",
    "MossdeepCity_Gym_Text_NicholasIntro": "bends the room",
    "Route110_TrickHousePuzzle4_Text_YujiIntro": "strikes its ally",
    "MtChimney_Text_SawyerIntro": "Sand wakes Dracozolt",
    "MagmaHideout_4F_Text_TabithaIntro": "formation erupts",
}


def label_blocks(files: list[Path]) -> dict[str, tuple[str, Path]]:
    labels = {}
    for path in files:
        source = path.read_text(errors="ignore")
        for match in re.finditer(
            r"^([A-Za-z0-9_]+)(?:::|:)\s*(.*?)(?=^[A-Za-z0-9_]+(?:::|:)|\Z)",
            source,
            re.M | re.S,
        ):
            strings = re.findall(r'\.string\s+"([^"]*)"', match.group(2))
            if strings:
                labels[match.group(1)] = (" ".join(strings), path)
    return labels


def visible_line_lengths(raw_text: str) -> list[int]:
    return [
        len(re.sub(r"\{[^}]+\}", "PLAYER", segment).replace("$", "").strip())
        for segment in re.split(r"\\[npl]", raw_text)
        if segment.strip()
    ]


def main() -> None:
    formats = json.loads(FORMAT_MANIFEST.read_text())["formats"]
    files = list((ROOT / "data").rglob("*.inc"))
    labels = label_blocks(files)
    references: list[tuple[str, list[str], Path]] = []
    for path in files:
        source = path.read_text(errors="ignore")
        for line in source.splitlines():
            standard = re.search(
                r"\btrainerbattle_(?:single|double|rematch|rematch_double)\s+((?:TRAINER|MAY|BRENDAN)_[A-Z0-9_]+)\s*,\s*([A-Za-z0-9_]+)\s*,\s*([A-Za-z0-9_]+)",
                line,
            )
            no_intro = re.search(
                r"\btrainerbattle_no_intro\s+((?:TRAINER|MAY|BRENDAN)_[A-Z0-9_]+)\s*,\s*([A-Za-z0-9_]+)",
                line,
            )
            generic = re.search(
                r"\btrainerbattle\s+TRAINER_BATTLE_[A-Z0-9_]+\s*,\s*((?:TRAINER|MAY|BRENDAN)_[A-Z0-9_]+)\s*,\s*[^,]+\s*,\s*([A-Za-z0-9_]+)\s*,\s*([A-Za-z0-9_]+)",
                line,
            )
            multi = re.search(
                r"\bmulti_2_vs_2\s+(TRAINER_[A-Z0-9_]+)\s*,\s*([A-Za-z0-9_]+)\s*,\s*(TRAINER_[A-Z0-9_]+)\s*,\s*([A-Za-z0-9_]+)",
                line,
            )
            if standard:
                references.append((standard.group(1), [standard.group(2), standard.group(3)], path))
            elif no_intro:
                references.append((no_intro.group(1), [no_intro.group(2)], path))
            elif generic:
                references.append((generic.group(1), [generic.group(2), generic.group(3)], path))
            if multi:
                references.append((multi.group(1), [multi.group(2)], path))
                references.append((multi.group(3), [multi.group(4)], path))

    problems = []
    for trainer_id, dialogue_labels, path in references:
        if trainer_id not in formats:
            continue
        for label in dialogue_labels:
            if label not in labels:
                problems.append(f"{path.relative_to(ROOT)}: missing dialogue label {label}")
                continue
            text, text_path = labels[label]
            battle_format = formats[trainer_id]["format"]
            if battle_format == "single" and DOUBLE_LANGUAGE.search(text):
                problems.append(f"{trainer_id}: singles battle has doubles dialogue at {text_path.relative_to(ROOT)}:{label}")
            if battle_format == "double" and SINGLE_LANGUAGE.search(text):
                problems.append(f"{trainer_id}: doubles battle has singles dialogue at {text_path.relative_to(ROOT)}:{label}")

    for label, cue in STRATEGY_CUES.items():
        if label not in labels or cue.lower() not in labels[label][0].lower():
            problems.append(f"missing authored strategy cue: {label}")
        elif max(visible_line_lengths(labels[label][0]), default=0) > 36:
            problems.append(f"strategy cue exceeds conservative line budget: {label}")

    covered_trainers = {reference[0] for reference in references}
    if len(references) < 700 or len(covered_trainers) < 480:
        problems.append(f"trainer dialogue coverage unexpectedly low: refs={len(references)}, trainers={len(covered_trainers)}")
    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))
    print(f"PASS: {len(references)} scripted battle-side dialogue references / {len(covered_trainers)} trainer records resolve")
    print("PASS: no singles/doubles dialogue contradictions")
    print(f"PASS: {len(STRATEGY_CUES)} bespoke strategy cues fit the native line budget")


if __name__ == "__main__":
    main()
