#!/usr/bin/env python3
"""Static gates and an executable model for safe task-slot exhaustion."""

from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]
NUM_TASKS = 16
NUM_TASK_DATA = 16
HEAD_SENTINEL = 0xFE
TAIL_SENTINEL = 0xFF


def read(path: str) -> str:
    return (ROOT / path).read_text()


def section(text: str, start: str, end: str) -> str:
    begin = text.index(start)
    finish = text.index(end, begin + len(start))
    return text[begin:finish]


task_h = read("include/task.h")
task_c = read("src/task.c")
destroy = section(task_c, "void DestroyTask", "void RunTasks")
run = section(task_c, "void RunTasks", "static u8 FindFirstActiveTask")
find_first = section(task_c, "static u8 FindFirstActiveTask", "void TaskDummy")
set_followup = section(task_c, "void SetTaskFuncWithFollowupFunc", "void SwitchTaskToFollowupFunc")
switch_followup = section(task_c, "void SwitchTaskToFollowupFunc", "bool8 FuncIsActiveTask")
set_word = section(task_c, "void SetWordTaskArg", "u32 GetWordTaskArg")
get_word = task_c[task_c.index("u32 GetWordTaskArg"):]

checks = {
    "HEAD, TAIL, and TASK_NONE retain their legacy values": all(
        token in task_h
        for token in (
            "#define HEAD_SENTINEL 0xFE",
            "#define TAIL_SENTINEL 0xFF",
            "#define TASK_NONE TAIL_SENTINEL",
        )
    ),
    "Task storage exposes one in-bounds overflow sentinel": (
        "extern struct Task gTasks[NUM_TASKS + 1];" in task_h
        and "struct Task gTasks[NUM_TASKS + 1];" in task_c
    ),
    "One shared validity contract defines every schedulable task ID": (
        "bool8 IsTaskIdValid(u8 taskId);" in task_h
        and "bool8 IsTaskIdValid(u8 taskId)" in task_c
        and "return taskId < NUM_TASKS;" in task_c
    ),
    "Overflow initialization is inactive and detached": all(
        token in section(task_c, "static void ResetOverflowTask", "u8 CreateTask")
        for token in (
            "gTasks[NUM_TASKS].func = TaskDummy;",
            "gTasks[NUM_TASKS].isActive = FALSE;",
            "gTasks[NUM_TASKS].prev = HEAD_SENTINEL;",
            "gTasks[NUM_TASKS].next = TAIL_SENTINEL;",
            "memset(gTasks[NUM_TASKS].data, 0",
        )
    ),
    "CreateTask resets and returns the overflow slot after scanning only real slots": (
        "for (i = 0; i < NUM_TASKS; i++)" in section(task_c, "u8 CreateTask", "static void InsertTask")
        and "ResetOverflowTask();\n    return NUM_TASKS;" in section(task_c, "u8 CreateTask", "static void InsertTask")
    ),
    "CreateTask rejects NULL callbacks into the detached overflow slot": (
        "if (func == NULL)" in section(task_c, "u8 CreateTask", "static void InsertTask")
        and section(task_c, "u8 CreateTask", "static void InsertTask").count("return NUM_TASKS;") == 2
    ),
    "Reset initializes real slots then the detached overflow slot": (
        "for (i = 0; i < NUM_TASKS; i++)" in section(task_c, "void ResetTasks", "static void ResetOverflowTask")
        and "ResetOverflowTask();" in section(task_c, "void ResetTasks", "static void ResetOverflowTask")
    ),
    "Scheduler discovery and execution remain bounded to 16 tasks": (
        "for (taskId = 0; taskId < NUM_TASKS; taskId++)" in find_first
        and "if (taskId != NUM_TASKS)" in run
        and "taskId != TAIL_SENTINEL" in run
    ),
    "DestroyTask ignores overflow and legacy sentinel IDs": (
        "if (!IsTaskIdValid(taskId))\n        return;" in destroy
    ),
    "Follow-up APIs ignore overflow and reconstruct unsigned pointer halves": (
        "if (!IsTaskIdValid(taskId))\n        return;" in set_followup
        and "if (!IsTaskIdValid(taskId))\n        return;" in switch_followup
        and "((u32)(u16)gTasks[taskId].data[followupFuncIndex + 1] << 16)" in switch_followup
    ),
    "Word argument APIs validate task and data bounds": (
        "if (IsTaskIdValid(taskId) && dataElem < NUM_TASK_DATA - 1)" in set_word
        and "if (IsTaskIdValid(taskId) && dataElem < NUM_TASK_DATA - 1)" in get_word
        and "((u32)(u16)gTasks[taskId].data[dataElem + 1] << 16)" in get_word
        and "return 0;" in get_word
    ),
    "FindTaskIdByFunc still reports TASK_NONE, not the overflow slot": (
        "return TASK_NONE; // No task was found." in task_c
    ),
}


# Only task.c may mutate list-membership fields. This proves legacy direct
# callers can write overflow data/func fields but cannot insert slot 16 into
# the active list behind the scheduler's back.
membership_assignment = re.compile(
    r"gTasks\[[^\]]+\]\.(?:isActive|prev|next|priority)\s*=(?!=)"
)
outside_membership_writes = []
for folder in (ROOT / "src", ROOT / "gflib"):
    for path in folder.glob("*.c"):
        if path == ROOT / "src/task.c":
            continue
        if membership_assignment.search(path.read_text(errors="ignore")):
            outside_membership_writes.append(str(path.relative_to(ROOT)))
checks["No caller outside task.c can activate or link the overflow slot"] = not outside_membership_writes

all_task_sources = "\n".join(
    path.read_text(errors="ignore")
    for folder in (ROOT / "src", ROOT / "gflib")
    for path in folder.glob("*.c")
)
create_task_call_count = len(re.findall(r"\bCreateTask\s*\(", all_task_sources))
checks["Caller census covers the full CreateTask surface"] = create_task_call_count >= 500
checks["No FindTaskIdByFunc result directly indexes gTasks"] = not re.search(
    r"gTasks\s*\[\s*FindTaskIdByFunc\s*\(", all_task_sources
)

checks["High-impact animation counters only track scheduled tasks"] = all(
    token in read("src/battle_anim.c")
    for token in (
        "if (taskId < NUM_TASKS)",
        "if (taskId < NUM_TASKS)\n        {\n            taskFunc(taskId);\n            gAnimVisualTaskCount++;",
        "if (taskId < NUM_TASKS)\n    {\n        func(taskId);\n        gAnimSoundTaskCount++;",
    )
)
checks["Battle animation watchers skip cleanly when no task is available"] = (
    read("src/battle_gfx_sfx_util.c").count("if (!IsTaskIdValid(taskId))") >= 2
    and "return TRUE;" in read("src/battle_gfx_sfx_util.c")
)
checks["Field-effect failures remove their active markers"] = all(
    token in read("src/field_effect.c")
    for token in (
        "FieldEffectActiveListRemove(FLDEFF_POKECENTER_HEAL);",
        "FieldEffectActiveListRemove(FLDEFF_HALL_OF_FAME_RECORD);",
        "FieldEffectActiveListRemove(FLDEFF_USE_WATERFALL);",
        "FieldEffectActiveListRemove(FLDEFF_USE_DIVE);",
    )
)
checks["Stop-surfing allocates before mutating avatar or script state"] = (
    section(
        read("src/field_player_avatar.c"),
        "static void CreateStopSurfingTask(u8 direction)\n{",
        "static void Task_StopSurfingInit(u8 taskId)\n{",
    ).index("u8 taskId = CreateTask(Task_StopSurfingInit")
    < section(
        read("src/field_player_avatar.c"),
        "static void CreateStopSurfingTask(u8 direction)\n{",
        "static void Task_StopSurfingInit(u8 taskId)\n{",
    ).index("ScriptContext2_Enable();")
)
checks["Slot-machine entry and reel setup have explicit exhaustion exits"] = all(
    token in read("src/slot_machine.c")
    for token in (
        "if (GetTaskCount() > NUM_TASKS - 2)",
        "SetMainCallback2(exitCallback);",
        "if (!CreateSlotReelTasks())",
        "while (i != 0)",
    )
)
checks["Battle EXP and sendout waits have explicit fallback paths"] = (
    read("src/battle_controller_player.c").count("IsTaskIdValid(taskId)") >= 2
    and read("src/battle_controller_player_partner.c").count("IsTaskIdValid(taskId)") >= 2
    and all(
        "IsTaskIdValid(taskId)" in read(path)
        for path in (
            "src/battle_controller_wally.c",
            "src/battle_controller_recorded_player.c",
            "src/battle_controller_link_partner.c",
            "src/battle_controller_opponent.c",
            "src/battle_controller_recorded_opponent.c",
            "src/battle_controller_link_opponent.c",
        )
    )
)


class Task:
    def __init__(self) -> None:
        self.func = "dummy"
        self.is_active = False
        self.prev = 0
        self.next = 0
        self.priority = 0xFF
        self.data = [0] * NUM_TASK_DATA


class TaskModel:
    """Host-executable model of src/task.c's list and public ID rules."""

    def __init__(self) -> None:
        self.tasks = [Task() for _ in range(NUM_TASKS + 1)]
        self.reset()

    def reset_overflow(self) -> None:
        task = self.tasks[NUM_TASKS]
        task.func = "dummy"
        task.is_active = False
        task.prev = HEAD_SENTINEL
        task.next = TAIL_SENTINEL
        task.priority = 0xFF
        task.data[:] = [0] * NUM_TASK_DATA

    def reset(self) -> None:
        for i in range(NUM_TASKS):
            task = self.tasks[i]
            task.func = "dummy"
            task.is_active = False
            task.prev = i
            task.next = i + 1
            task.priority = 0xFF
            task.data[:] = [0] * NUM_TASK_DATA
        self.tasks[0].prev = HEAD_SENTINEL
        self.tasks[NUM_TASKS - 1].next = TAIL_SENTINEL
        self.reset_overflow()

    def first_active(self) -> int:
        for i in range(NUM_TASKS):
            if self.tasks[i].is_active and self.tasks[i].prev == HEAD_SENTINEL:
                return i
        return NUM_TASKS

    def insert(self, new_id: int) -> None:
        task_id = self.first_active()
        if task_id == NUM_TASKS:
            self.tasks[new_id].prev = HEAD_SENTINEL
            self.tasks[new_id].next = TAIL_SENTINEL
            return
        while True:
            if self.tasks[new_id].priority < self.tasks[task_id].priority:
                self.tasks[new_id].prev = self.tasks[task_id].prev
                self.tasks[new_id].next = task_id
                if self.tasks[task_id].prev != HEAD_SENTINEL:
                    self.tasks[self.tasks[task_id].prev].next = new_id
                self.tasks[task_id].prev = new_id
                return
            if self.tasks[task_id].next == TAIL_SENTINEL:
                self.tasks[new_id].prev = task_id
                self.tasks[new_id].next = TAIL_SENTINEL
                self.tasks[task_id].next = new_id
                return
            task_id = self.tasks[task_id].next

    def create(self, func: str, priority: int) -> int:
        if func is None:
            self.reset_overflow()
            return NUM_TASKS
        for i in range(NUM_TASKS):
            if not self.tasks[i].is_active:
                self.tasks[i].func = func
                self.tasks[i].priority = priority
                self.insert(i)
                self.tasks[i].data[:] = [0] * NUM_TASK_DATA
                self.tasks[i].is_active = True
                return i
        self.reset_overflow()
        return NUM_TASKS

    def destroy(self, task_id: int) -> None:
        if task_id >= NUM_TASKS:
            return
        task = self.tasks[task_id]
        if not task.is_active:
            return
        task.is_active = False
        if task.prev == HEAD_SENTINEL:
            if task.next != TAIL_SENTINEL:
                self.tasks[task.next].prev = HEAD_SENTINEL
        elif task.next == TAIL_SENTINEL:
            self.tasks[task.prev].next = TAIL_SENTINEL
        else:
            self.tasks[task.prev].next = task.next
            self.tasks[task.next].prev = task.prev

    def set_followup(self, task_id: int, func: str, followup: int) -> None:
        if task_id >= NUM_TASKS:
            return
        self.tasks[task_id].func = func
        self.set_word(task_id, NUM_TASK_DATA - 2, followup)

    def switch_followup(self, task_id: int) -> None:
        if task_id >= NUM_TASKS:
            return
        self.tasks[task_id].func = self.get_word(task_id, NUM_TASK_DATA - 2)

    def set_word(self, task_id: int, elem: int, value: int) -> None:
        if task_id < NUM_TASKS and elem < NUM_TASK_DATA - 1:
            self.tasks[task_id].data[elem] = value & 0xFFFF
            self.tasks[task_id].data[elem + 1] = (value >> 16) & 0xFFFF

    def get_word(self, task_id: int, elem: int) -> int:
        if task_id < NUM_TASKS and elem < NUM_TASK_DATA - 1:
            return self.tasks[task_id].data[elem] | (self.tasks[task_id].data[elem + 1] << 16)
        return 0

    def run_order(self) -> list[int]:
        order = []
        task_id = self.first_active()
        if task_id != NUM_TASKS:
            while task_id != TAIL_SENTINEL:
                order.append(task_id)
                task_id = self.tasks[task_id].next
        return order


model = TaskModel()
checks["Model: NULL task functions fail into the overflow sentinel"] = (
    model.create(None, 0) == NUM_TASKS
    and not model.tasks[NUM_TASKS].is_active
)
created_ids = []
for i in range(NUM_TASKS):
    created_ids.append(model.create(f"task-{i}", NUM_TASKS - i))
checks["Model: the first 16 creations occupy exactly the real slots"] = (
    created_ids == list(range(NUM_TASKS))
)
task_zero_before = (
    model.tasks[0].func,
    model.tasks[0].is_active,
    model.tasks[0].prev,
    model.tasks[0].next,
    model.tasks[0].priority,
    model.tasks[0].data[:],
)
overflow_id = model.create("seventeenth", 0)

# Simulate the pervasive legacy pattern that initializes the returned slot.
model.tasks[overflow_id].func = "legacy-direct-write"
model.tasks[overflow_id].data[0] = 0x1234
task_zero_after = (
    model.tasks[0].func,
    model.tasks[0].is_active,
    model.tasks[0].prev,
    model.tasks[0].next,
    model.tasks[0].priority,
    model.tasks[0].data[:],
)
checks["Model: the 17th creation returns slot 16 without mutating task 0"] = (
    overflow_id == NUM_TASKS and task_zero_before == task_zero_after
)
checks["Model: overflow is absent from the active list and run order"] = (
    len(model.run_order()) == NUM_TASKS
    and set(model.run_order()) == set(range(NUM_TASKS))
    and NUM_TASKS not in model.run_order()
    and sum(task.is_active for task in model.tasks[:NUM_TASKS]) == NUM_TASKS
)

# Invalid public API calls must neither touch task 0 nor index beyond storage.
invalid_reads = []
for invalid_id in (NUM_TASKS, TAIL_SENTINEL):
    model.destroy(invalid_id)
    model.set_followup(invalid_id, "bad", 0x89ABCDEF)
    model.switch_followup(invalid_id)
    model.set_word(invalid_id, 0, 0x89ABCDEF)
    invalid_reads.append(model.get_word(invalid_id, 0))
model.set_word(0, NUM_TASK_DATA - 1, 0x89ABCDEF)
checks["Model: invalid public task IDs and word offsets are safe no-ops"] = (
    task_zero_after
    == (
        model.tasks[0].func,
        model.tasks[0].is_active,
        model.tasks[0].prev,
        model.tasks[0].next,
        model.tasks[0].priority,
        model.tasks[0].data[:],
    )
    and model.get_word(0, NUM_TASK_DATA - 1) == 0
    and invalid_reads == [0, 0]
)

# Unsigned high halves must round-trip even when bit 31 is set.
model.set_word(0, 4, 0x89ABCDEF)
checks["Model: 32-bit word task arguments round-trip unsigned"] = (
    model.get_word(0, 4) == 0x89ABCDEF
)

# Destroy/reuse and reset must never make the overflow slot schedulable.
model.destroy(0)
replacement_id = model.create("replacement", 3)
model.reset()
checks["Model: destroy/reuse/reset ordering keeps overflow detached"] = (
    replacement_id == 0
    and model.run_order() == []
    and not model.tasks[NUM_TASKS].is_active
    and model.tasks[NUM_TASKS].func == "dummy"
    and model.tasks[NUM_TASKS].prev == HEAD_SENTINEL
    and model.tasks[NUM_TASKS].next == TAIL_SENTINEL
    and model.tasks[NUM_TASKS].data == [0] * NUM_TASK_DATA
)


failed = [name for name, passed in checks.items() if not passed]
for name, passed in checks.items():
    print(f"{'PASS' if passed else 'FAIL'}: {name}")

if outside_membership_writes:
    print("Unexpected task membership writers:", ", ".join(outside_membership_writes))
if failed:
    raise SystemExit(f"{len(failed)} task exhaustion checks failed")

print(f"PASS: audited {create_task_call_count} CreateTask call sites")
print(f"PASS: {len(checks)} task exhaustion checks")
