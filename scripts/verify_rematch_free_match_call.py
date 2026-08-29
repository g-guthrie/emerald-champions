#!/usr/bin/env python3
"""Prove Match Call cannot promise disabled campaign rematches."""

from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]

FUTURE_BATTLE = re.compile(
    r"(?:"
    r"\brematch\b|"
    r"\b(?:battle|fight|face|challenge)\s+(?:with\s+)?(?:me|you|us)(?:\s+\w+){0,3}\s+again\b|"
    r"\bwhen we (?:battle|fight|face|challenge) again\b|"
    r"\bnext time(?:\s+\w+){0,5}\s+(?:battle|fight|face|challenge)\s+(?:me|you|us)\b|"
    r"\b(?:want|plan|intend|hope|going)(?:\s+\w+){0,5}\s+"
    r"(?:beat|battle|fight|face|challenge)\s+(?:me|you|us)\b|"
    r"\banother (?:battle|match) with (?:me|you|us)\b|"
    r"\bbattle (?:me|you|us) later\b"
    r")",
    re.IGNORECASE,
)

LEADER_CONTACTS = {
    "MatchCall_Text_Norman_PostRematch",
    "MatchCall_Text_Roxanne_PostRematch",
    "MatchCall_Text_Brawly_PostRematch",
    "MatchCall_Text_Wattson_PostRematch",
    "MatchCall_Text_Flannery_PostRematch",
    "MatchCall_Text_Winona_PostRematch",
    "MatchCall_Text_TateLiza_PostRematch",
    "MatchCall_Text_Juan_PostRematch",
}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def asm_blocks(path: Path) -> dict[str, tuple[int, str]]:
    blocks: dict[str, tuple[int, str]] = {}
    label: str | None = None
    start = 0
    lines: list[str] = []
    for line_number, line in enumerate(path.read_text(errors="ignore").splitlines(), 1):
        match = re.match(r"^([A-Za-z_][A-Za-z0-9_]*):{1,2}\s*$", line)
        if match:
            if label is not None:
                blocks[label] = (start, "\n".join(lines))
            label = match.group(1)
            start = line_number
            lines = []
        elif label is not None:
            lines.append(line)
    if label is not None:
        blocks[label] = (start, "\n".join(lines))
    return blocks


def text_content(block: str) -> str:
    return " ".join(re.findall(r'\.string\s+"([^"]*)"', block))


def exempt(path: Path, label: str) -> bool:
    identity = f"{path}:{label}".lower()
    return any(token in identity for token in ("gabby", "_ty", "evergrande", "elite_four", "league"))


def verify_runtime_guards() -> None:
    config = (ROOT / "include/config/overworld.h").read_text()
    require(
        re.search(r"#define\s+OW_TRAINER_REMATCHES\s+FALSE\b", config) is not None,
        "OW_TRAINER_REMATCHES must remain FALSE",
    )

    battle_setup = (ROOT / "src/battle_setup.c").read_text()
    require(
        re.search(
            r"bool8 ShouldTryRematchBattleForTrainerId\([^)]*\)\s*\{\s*"
            r"if \(!OW_TRAINER_REMATCHES\)\s*return FALSE;",
            battle_setup,
            re.DOTALL,
        )
        is not None,
        "overworld rematch battle guard is missing",
    )
    require(
        re.search(
            r"bool8 IsTrainerReadyForRematch\(void\)\s*\{\s*"
            r"if \(!OW_TRAINER_REMATCHES\).*?TrainerIsMatchCallRegistered\(tableId\)",
            battle_setup,
            re.DOTALL,
        )
        is not None,
        "repeat interaction no longer resolves through Match Call registration state",
    )

    match_call = (ROOT / "src/match_call.c").read_text()
    for function in ("TrainerIsEligibleForRematch", "ShouldTrainerRequestBattle"):
        function_body = re.search(
            rf"static bool32 {function}\([^)]*\)\s*\{{(?P<body>.*?)\n\}}",
            match_call,
            re.DOTALL,
        )
        require(
            function_body is not None
            and re.search(
                r"if \(!OW_TRAINER_REMATCHES\)\s*return FALSE;",
                function_body.group("body"),
            ) is not None,
            f"{function} can still expose a Match Call battle request",
        )

    pokenav = (ROOT / "src/pokenav_match_call_data.c").read_text()
    for function in ("MatchCall_IsRematchable_Trainer", "MatchCall_IsRematchable_Wally"):
        function_body = re.search(
            rf"static bool32 {function}\([^)]*\)\s*\{{(?P<body>.*?)\n\}}",
            pokenav,
            re.DOTALL,
        )
        require(
            function_body is not None
            and re.search(
                r"if \(!OW_TRAINER_REMATCHES\)\s*return FALSE;",
                function_body.group("body"),
            ) is not None,
            f"{function} can still draw a disabled rematch marker",
        )
    require(
        re.search(r"if \(!OW_TRAINER_REMATCHES\)\s*\{.*?i \+= 3;", pokenav, re.DOTALL)
        is not None,
        "Gym Leader Match Call does not select the retrospective contact row",
    )

    macros = (ROOT / "asm/macros/event.inc").read_text()
    macro = re.search(
        r"\.macro trainerbattle_rematch_double\b(?P<body>.*?)\.endm",
        macros,
        re.DOTALL,
    )
    require(macro is not None, "trainerbattle_rematch_double macro is missing")
    require(
        re.search(
            r"\\not_enough_pkmn_text,\s*TRUE,\s*TRUE,\s*FALSE,\s*TRUE,",
            macro.group("body"),
        )
        is not None,
        "trainerbattle_rematch_double lost its Double Battle or rematch flag",
    )


def verify_dialogue() -> tuple[int, int]:
    sources = [ROOT / "data/text/trainers.inc", ROOT / "data/text/match_call.inc"]
    sources.extend(
        path
        for path in sorted((ROOT / "data/maps").glob("*/scripts.inc"))
        if "_Frlg" not in path.parts[-2]
    )
    blocks_by_path = {path: asm_blocks(path) for path in sources}
    all_blocks: dict[str, tuple[Path, int, str]] = {}
    for path, blocks in blocks_by_path.items():
        for label, (line, block) in blocks.items():
            all_blocks[label] = (path, line, block)

    # A registration prompt is reachable when the same event-script block
    # performs register_matchcall. This catches labels whose historical names
    # still contain "Rematch" without treating dead rematch branches as live.
    registration_texts: set[str] = set()
    for path, blocks in blocks_by_path.items():
        if "data/maps" not in str(path):
            continue
        for _, (_, block) in blocks.items():
            if "register_matchcall" not in block:
                continue
            registration_texts.update(
                re.findall(r"\b(?:msgbox|message|pokenavcall)\s+([A-Za-z_][A-Za-z0-9_]*)", block)
            )

    checked: set[str] = set()
    violations: list[str] = []

    def check(label: str, reason: str) -> None:
        row = all_blocks.get(label)
        require(row is not None, f"{reason} references missing text label {label}")
        path, line, block = row
        if exempt(path, label):
            return
        checked.add(label)
        dialogue = text_content(block)
        if FUTURE_BATTLE.search(dialogue):
            violations.append(f"{path.relative_to(ROOT)}:{line}: {label}: {dialogue}")

    for label in sorted(registration_texts):
        check(label, "Match Call registration")

    # These are the only generic trainer-call families selected after battle
    # requests are disabled. Request-topic rows intentionally remain as dead
    # vanilla data, so restoring rematches later does not require rebuilding it.
    for label in sorted(all_blocks):
        if label.startswith(
            (
                "MatchCall_WildBattleText",
                "MatchCall_NegativeBattleText",
                "MatchCall_PositiveBattleText",
                "MatchCall_PersonalizedText",
                "MatchCall_BattleFrontier",
                "MatchCall_BattleDomeText",
                "MatchCall_BattlePikeText",
                "MatchCall_BattlePyramidText",
            )
        ):
            check(label, "reachable Match Call topic")

    for label in sorted(LEADER_CONTACTS):
        check(label, "Gym Leader retrospective contact")

    # Ordinary repeated interaction must not explicitly advertise a rematch.
    # Rematch-named blocks are dead unless captured above as a registration
    # prompt; League replay and Gabby/Ty are intentional exceptions.
    for path, blocks in blocks_by_path.items():
        for label, (line, block) in blocks.items():
            if (
                label in checked
                or "Rematch" in label
                or "BattleRequestText" in label
                or label.endswith(("_Preparing", "_PreparingPostGame", "_RematchReady"))
                or exempt(path, label)
            ):
                continue
            dialogue = text_content(block)
            if re.search(r"\brematch\b", dialogue, re.IGNORECASE):
                violations.append(f"{path.relative_to(ROOT)}:{line}: {label}: {dialogue}")

    require(not violations, "reachable dialogue promises a disabled rematch:\n" + "\n".join(violations))
    return len(registration_texts), len(checked)


def main() -> None:
    verify_runtime_guards()
    registrations, total = verify_dialogue()
    print(
        "PASS: rematches are disabled; "
        f"{registrations} registration prompts and {total} reachable Match Call texts are promise-free"
    )


if __name__ == "__main__":
    main()
