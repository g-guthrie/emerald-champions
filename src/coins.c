#include "global.h"
#include "coins.h"
#include "text.h"
#include "window.h"
#include "strings.h"
#include "string_util.h"
#include "menu.h"
#include "international_string_util.h"
#include "constants/coins.h"
#include "money.h"

static EWRAM_DATA u8 sCoinsWindowId = 0;

void PrintCoinsString(u32 coinAmount)
{
    u32 xAlign;

    ConvertIntToDecimalStringN(gStringVar1, coinAmount, STR_CONV_MODE_RIGHT_ALIGN, MAX_COIN_DIGITS);
    StringExpandPlaceholders(gStringVar4, gText_Coins);

    xAlign = GetStringRightAlignXOffset(FONT_NORMAL, gStringVar4, 0x40);
    AddTextPrinterParameterized(sCoinsWindowId, FONT_NORMAL, gStringVar4, xAlign, 1, 0, NULL);
}

void ShowCoinsWindow(u32 coinAmount, u8 x, u8 y)
{
    struct WindowTemplate template;
    SetWindowTemplateFields(&template, 0, x + 1, y + 1, 8, 2, 0xF, 0x141);
    sCoinsWindowId = AddWindow(&template);
    FillWindowPixelBuffer(sCoinsWindowId, PIXEL_FILL(0));
    PutWindowTilemap(sCoinsWindowId);
    DrawStdFrameWithCustomTileAndPalette(sCoinsWindowId, FALSE, 0x214, 0xE);
    PrintCoinsString(coinAmount);
}

void HideCoinsWindow(void)
{
    ClearStdWindowAndFrame(sCoinsWindowId, TRUE);
    RemoveWindow(sCoinsWindowId);
}

// Emerald Champions: there is no coin currency. The Game Corner machines play
// for ordinary money; a coin on the counter is COIN_VALUE money, and the Coin
// Case is only the licence to sit down. SaveBlock1.coins is unused.
u16 GetCoins(void)
{
    u32 coins = GetMoney(&gSaveBlock1Ptr->money) / COIN_VALUE;
    return coins > MAX_COINS ? MAX_COINS : coins;
}

void SetCoins(u16 coinAmount)
{
    s32 delta = (s32)coinAmount - (s32)GetCoins();
    if (delta > 0)
        AddMoney(&gSaveBlock1Ptr->money, (u32)delta * COIN_VALUE);
    else if (delta < 0)
        RemoveMoney(&gSaveBlock1Ptr->money, (u32)(-delta) * COIN_VALUE);
}

bool8 AddCoins(u16 toAdd)
{
    u16 newAmount;
    u16 ownedCoins = GetCoins();
    if (ownedCoins >= MAX_COINS)
        return FALSE;
    // check overflow, can't have less coins than previously
    if (ownedCoins > ownedCoins + toAdd)
    {
        newAmount = MAX_COINS;
    }
    else
    {
        ownedCoins += toAdd;
        if (ownedCoins > MAX_COINS)
            ownedCoins = MAX_COINS;
        newAmount = ownedCoins;
    }
    SetCoins(newAmount);
    return TRUE;
}

bool8 RemoveCoins(u16 toSub)
{
    u16 ownedCoins = GetCoins();
    if (ownedCoins >= toSub)
    {
        SetCoins(ownedCoins - toSub);
        return TRUE;
    }
    return FALSE;
}
