#ifndef GUARD_LINE_BREAK_H
#define GUARD_LINE_BREAK_H

struct StringWord {
    u16 startIndex;
    u16 length;
    u16 width;
};

struct StringLine {
    struct StringWord *words;
    u16 numWords;
    u8 spaceWidth;
    u8 extraSpaceWidth;
};

enum ToggleScrollPrompt
{
    SHOW_SCROLL_PROMPT,
    HIDE_SCROLL_PROMPT,
};

void StripLineBreaks(u8 *src);
u32 CountLineBreaks(u8 *src);
void BreakStringAutomatic(u8 *src, u32 maxWidth, u32 screenLines, u8 fontId, enum ToggleScrollPrompt toggleScrollPrompt);
void BreakSubStringAutomatic(u8 *src, u32 maxWidth, u32 screenLines, u8 fontId, enum ToggleScrollPrompt toggleScrollPrompt);


bool32 IsWordSplittingChar(const u8 *src, u32 index);
void BuildNewString(struct StringLine *stringLines, u32 numLines, u32 maxLines, u8 *str, enum ToggleScrollPrompt toggleScrollPrompt);
bool32 StringHasManualBreaks(u8 *src);

#endif // GUARD_LINE_BREAK_H
