RUNTIME_DATA_FILE_BEGIN(Myth.enc)

RUNTIME_DATA_TYPE_BEGIN(MythMain, "Myth.Myth_main.Condition", 1)
	RUNTIME_DATA_PROPERTY(Int32, OverlordLevel, "Olv")
	RUNTIME_DATA_PROPERTY(Int32, RequiredQuest, "Prequest")
RUNTIME_DATA_TYPE_END(MythMain)
RUNTIME_DATA_TYPE_INDEX_SINGLE(MythMain)

RUNTIME_DATA_TYPE_BEGIN(MythLevel, "Myth.Myth_Level.Myth_Exp", 100)
	RUNTIME_DATA_PROPERTY(Int32, MythLevel, "MLV")
	RUNTIME_DATA_PROPERTY(UInt64, RequiredExp, "ReqExp")
	RUNTIME_DATA_PROPERTY(Int32, MythPoints, "Myth_point")
RUNTIME_DATA_TYPE_END(MythLevel)
RUNTIME_DATA_TYPE_INDEX(MythLevel, Int32, MythLevel)

RUNTIME_DATA_TYPE_BEGIN(MythReset, "Myth.Reset.Reset_condition", 1)
	RUNTIME_DATA_PROPERTY(Int32, RequiredLevel, "Lv")
RUNTIME_DATA_TYPE_END(MythReset)
RUNTIME_DATA_TYPE_INDEX_SINGLE(MythReset)

RUNTIME_DATA_TYPE_BEGIN(MythRepeat, "Myth.Repeat.Repeat_Bonus", 1)
	RUNTIME_DATA_PROPERTY(Int32, RepeatCondition, "Repeat_condition")
	RUNTIME_DATA_PROPERTY(Int32, BonusPoints, "Bonus_Pt")
RUNTIME_DATA_TYPE_END(MythRepeat)
RUNTIME_DATA_TYPE_INDEX_SINGLE(MythRepeat)

RUNTIME_DATA_TYPE_BEGIN(MythRepeatPenalty, "Myth.RepeatPenalty.Penaltyinfo", 1)
	RUNTIME_DATA_PROPERTY(Int32, AddMxp, "AddMxp")
	RUNTIME_DATA_PROPERTY(Int32, Limit, "Limit")
RUNTIME_DATA_TYPE_END(MythRepeatPenalty)
RUNTIME_DATA_TYPE_INDEX_SINGLE(MythRepeatPenalty)

RUNTIME_DATA_TYPE_BEGIN(MythRestoreItem, "Myth.Restore.RestoreItem", 1)
	RUNTIME_DATA_PROPERTY(UInt32, ItemID, "itemidx")
RUNTIME_DATA_TYPE_END(MythRestoreItem)
RUNTIME_DATA_TYPE_INDEX_SINGLE(MythRestoreItem)

RUNTIME_DATA_FILE_END