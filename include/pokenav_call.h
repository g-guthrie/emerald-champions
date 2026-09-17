#ifndef GUARD_POKENAV_CALL_H
#define GUARD_POKENAV_CALL_H

// The scripted PokeNav call window (script command `pokenavcall`).
// Match Call itself is removed; this is only the call presentation.
void StartPokenavCallFromScript(const u8 *message);
bool32 IsPokenavCallTaskActive(void);
void RedrawPokenavCallTextBoxBorder(void);

#endif //GUARD_POKENAV_CALL_H
