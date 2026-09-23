#include "global.h"
#include "line_break.h"
#include "text.h"
#include "malloc.h"
#include "string_util.h"

// Encoded parameter bytes are not visible separators, even when their value
// equals a space, newline, page break or EOS.
static u32 TextTokenLength(const u8 *src)
{
    if (*src == EXT_CTRL_CODE_BEGIN)
        return 1 + max(1, GetExtCtrlCodeLength(src[1]));
    if (*src == CHAR_EXTRA_SYMBOL || *src == CHAR_KEYPAD_ICON
     || *src == CHAR_DYNAMIC || *src == PLACEHOLDER_BEGIN)
        return 2;
    return 1;
}

void StripLineBreaks(u8 *src)
{
    u32 currIndex = 0;
    u32 prevChar = EOS;
    while (src[currIndex] != EOS)
    {
        if (src[currIndex] == CHAR_PROMPT_SCROLL || src[currIndex] == CHAR_NEWLINE)
        {
            if (prevChar == CHAR_HYPHEN)
                src[currIndex] = CHAR_ZWS;
            else
                src[currIndex] = CHAR_SPACE;
        }
        u32 length = TextTokenLength(src + currIndex);
        if (length == 1)
            prevChar = src[currIndex];
        currIndex += length;
    }
}

u32 CountLineBreaks(u8 *src)
{
    u32 currIndex = 0;
    u32 numNewLines = 0;
    while (src[currIndex] != EOS)
    {
        if (src[currIndex] == CHAR_PROMPT_SCROLL || src[currIndex] == CHAR_NEWLINE)
            numNewLines++;
        currIndex += TextTokenLength(src + currIndex);
    }

    return numNewLines;
}

void BreakStringAutomatic(u8 *src, u32 maxWidth, u32 screenLines, u8 fontId, enum ToggleScrollPrompt toggleScrollPrompt)
{
    u32 currIndex = 0;
    u8 *currSrc = src;
    while (src[currIndex] != EOS)
    {
        if (src[currIndex] == CHAR_PROMPT_CLEAR)
        {
            u8 replacedChar = src[currIndex];
            src[currIndex] = EOS;
            BreakSubStringAutomatic(currSrc, maxWidth, screenLines, fontId, toggleScrollPrompt);
            src[currIndex] = replacedChar;
            currSrc = &src[currIndex + 1];
        }
        currIndex += TextTokenLength(src + currIndex);
    }
    BreakSubStringAutomatic(currSrc, maxWidth, screenLines, fontId, toggleScrollPrompt);
}

#define SCROLL_PROMPT_WIDTH 8
void BreakSubStringAutomatic(u8 *src, u32 maxWidth, u32 screenLines, u8 fontId, enum ToggleScrollPrompt toggleScrollPrompt)
{
    //  If the string already has line breaks, don't interfere with them
    if (StringHasManualBreaks(src))
        return;
    //  Sanity check
    if (src[0] == EOS)
        return;
    u32 numChars = 0;
    u32 numWords = 0;
    bool32 inWord = FALSE;
    while (src[numChars] != EOS)
    {
        bool32 splitting = IsWordSplittingChar(src, numChars);
        if (!splitting && !inWord)
            numWords++;
        inWord = !splitting;
        numChars += TextTokenLength(src + numChars);
    }
    if (numWords <= 1)
        return;

    struct StringWord *allWords = Alloc(numWords * sizeof(*allWords));
    u32 currWordIndex = 0;
    for (u32 i = 0; i < numChars;)
    {
        if (IsWordSplittingChar(src, i))
        {
            i++;
            continue;
        }
        u32 start = i;
        while (i < numChars && !IsWordSplittingChar(src, i))
            i += TextTokenLength(src + i);
        allWords[currWordIndex].startIndex = start;
        allWords[currWordIndex].length = i - start;
        allWords[currWordIndex].width = 0;
        currWordIndex++;
    }

    //  Fill in individual word widths
    for (u32 i = 0; i < numWords; i++)
    {
        for (u32 j = 0; j < allWords[i].length;)
        {
            const u8 *token = src + allWords[i].startIndex + j;
            if (*token == CHAR_EXTRA_SYMBOL)
                allWords[i].width += GetGlyphWidth(token[1] | 0x100, FALSE, fontId);
            else if (*token == CHAR_KEYPAD_ICON)
                allWords[i].width += GetKeypadIconWidth(token[1]);
            else if (TextTokenLength(token) == 1)
                allWords[i].width += GetGlyphWidth(*token, FALSE, fontId);
            j += TextTokenLength(token);
        }
    }

    //  Step 1: Does it all fit one one line? Then no break
    //  Step 2: Try to split across minimum number of lines
    u32 spaceWidth = GetGlyphWidth(CHAR_SPACE, FALSE, fontId);
    u32 totalWidth = allWords[0].width;
    //  Calculate total widths without any line breaks
    for (u32 i = 1; i < numWords; i++)
        totalWidth += allWords[i].width + spaceWidth;

    if (toggleScrollPrompt == SHOW_SCROLL_PROMPT)
        totalWidth += SCROLL_PROMPT_WIDTH;

    //  If it doesn't fit on 1 line, do fancy line break calculation
    //  NOTE: Currently the line break calculation isn't fancy
    if (totalWidth > maxWidth)
    {
        //  Figure out how many lines are needed with naive method
        u32 currLineWidth = 0;
        u32 totalLines = 1;
        bool32 shouldTryAgain;
        for (currWordIndex = 0; currWordIndex < numWords; currWordIndex++)
        {
            if (toggleScrollPrompt == SHOW_SCROLL_PROMPT && currWordIndex + 1 == numWords)
                currLineWidth += SCROLL_PROMPT_WIDTH;
            if (currLineWidth + allWords[currWordIndex].length > maxWidth)
            {
                totalLines++;
                currLineWidth = allWords[currWordIndex].width;
            }
            else
            {
                currLineWidth += allWords[currWordIndex].width + spaceWidth;
            }
        }

        if (currLineWidth > maxWidth)
            totalLines++;

        //  LINE LAYOUT STARTS HERE
        struct StringLine *stringLines;
        do
        {
            shouldTryAgain = FALSE;
            u16 targetLineWidth = totalWidth/totalLines;
            stringLines = Alloc(totalLines*sizeof(struct StringLine));
            for (u32 lineIndex = 0; lineIndex < totalLines; lineIndex++)
            {
                stringLines[lineIndex].numWords = 0;
                stringLines[lineIndex].spaceWidth = spaceWidth;
                stringLines[lineIndex].extraSpaceWidth = 0;
            }
            currWordIndex = 0;
            u16 currLineIndex = 0;
            stringLines[currLineIndex].words = &allWords[currWordIndex];
            stringLines[currLineIndex].numWords = 1;
            currLineWidth = allWords[currWordIndex].width;
            currWordIndex++;
            while (currWordIndex < numWords)
            {
                if (currLineWidth + spaceWidth + allWords[currWordIndex].width + ((toggleScrollPrompt == SHOW_SCROLL_PROMPT) ? SCROLL_PROMPT_WIDTH : 0) > maxWidth)
                {
                    //  go to next line
                    currLineIndex++;
                    if (currLineIndex == totalLines)
                    {
                        totalLines++;
                        Free(stringLines);
                        shouldTryAgain = TRUE;
                        break;
                    }
                    stringLines[currLineIndex].words = &allWords[currWordIndex];
                    stringLines[currLineIndex].numWords = 1;
                    currLineWidth = allWords[currWordIndex].width;
                    currWordIndex++;
                }
                else if (currLineWidth > targetLineWidth)
                {
                    //  go to next line
                    currLineIndex++;
                    if (currLineIndex == totalLines)
                    {
                        totalLines++;
                        Free(stringLines);
                        shouldTryAgain = TRUE;
                        break;
                    }
                    stringLines[currLineIndex].words = &allWords[currWordIndex];
                    stringLines[currLineIndex].numWords = 1;
                    currLineWidth = allWords[currWordIndex].width;
                    currWordIndex++;
                }
                else
                {
                    //  continue on current line
                    //  add word and space width
                    currLineWidth += spaceWidth + allWords[currWordIndex].width;
                    stringLines[currLineIndex].numWords++;
                    currWordIndex++;
                }
            }
            if (!shouldTryAgain)
                totalLines = currLineIndex + 1;
        } while (shouldTryAgain);
        BuildNewString(stringLines, totalLines, screenLines, src, toggleScrollPrompt);
        Free(stringLines);
    }

    Free(allWords);
}

//  Only allow word splitting on allowed chars
bool32 IsWordSplittingChar(const u8 *src, u32 index)
{
    switch (src[index])
    {
    case CHAR_ZWS:
    case CHAR_SPACE:
        return TRUE;
    default:
        return FALSE;
    }
}

//  Build the new string from the data stored in the StringLine structs
void BuildNewString(struct StringLine *stringLines, u32 numLines, u32 maxLines, u8 *str, enum ToggleScrollPrompt toggleScrollPrompt)
{
    for (u32 lineIndex = 0; lineIndex + 1 < numLines; lineIndex++)
    {
        // Replace the separator immediately before the next line's first word,
        // rather than assuming all gaps have exactly one byte.
        u32 separator = stringLines[lineIndex + 1].words[0].startIndex - 1;
        if (lineIndex >= maxLines - 1 && numLines > maxLines && toggleScrollPrompt == SHOW_SCROLL_PROMPT)
            str[separator] = CHAR_PROMPT_SCROLL;
        else
            str[separator] = CHAR_NEWLINE;
    }
}

bool32 StringHasManualBreaks(u8 *src)
{
    u32 charIndex = 0;
    while (src[charIndex] != EOS)
    {
        if (src[charIndex] == CHAR_PROMPT_SCROLL || src[charIndex] == CHAR_NEWLINE)
            return TRUE;
        charIndex += TextTokenLength(src + charIndex);
    }
    return FALSE;
}
#undef SCROLL_PROMPT_WIDTH
