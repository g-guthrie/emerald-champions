#ifndef GUARD_LINK_MENU_TEXT_H
#define GUARD_LINK_MENU_TEXT_H

// Message box, yes/no prompt and list-window helpers shared by the Union Room
// screens. They were part of the Mystery Gift menu, which is removed; the names
// are kept so the Union Room code reads unchanged.
void MG_DrawTextBorder(u8 windowId);
void MG_AddMessageTextPrinter(const u8 *str);
bool32 PrintMysteryGiftMenuMessage(u8 *textState, const u8 *str);
s8 DoMysteryGiftYesNo(u8 *textState, u16 *windowId, bool8 yesNoBoxPlacement, const u8 *str);
u16 GetMysteryGiftBaseBlock(void);

#endif //GUARD_LINK_MENU_TEXT_H
