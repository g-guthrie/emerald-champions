Common_EventScript_FindItem::
	callnative GetItemBallIdAndAmountFromTemplate
	copyvar VAR_0x8000, VAR_RESULT
	copyvar VAR_0x8001, VAR_0x8009
	goto_if_eq VAR_0x800A, FALSE, Common_EventScript_FindOrdinaryItem
	special ConvertEmeraldChampionsFiniteReward
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_FindOrdinaryItem
	lock
	faceplayer
	removeobject VAR_LAST_TALKED
	msgbox Common_Text_DuplicateFiniteReward, MSGBOX_DEFAULT
	release
	end

Common_EventScript_FindOrdinaryItem::
	finditem VAR_0x8000, VAR_0x8001
	end
