#include "Character.h"
#include "Collection.h"

RTCollectionSlotRef RTCharacterGetCollectionSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt8 TypeID,
	UInt8 CollectionID,
	UInt16 MissionID
) {
	for (Int32 Index = 0; Index < Character->Data.CollectionInfo.Count; Index += 1) {
		RTCollectionSlotRef Slot = &Character->Data.CollectionInfo.Slots[Index];
		if (Slot->TypeID != TypeID) continue;
		if (Slot->CollectionID != CollectionID) continue;
		if (Slot->MissionID != MissionID) continue;

		return Slot;
	}

	assert(Character->Data.CollectionInfo.Count + 1 < RUNTIME_CHARACTER_MAX_COLLECTION_SLOT_COUNT);
	RTCollectionSlotRef Slot = &Character->Data.CollectionInfo.Slots[Character->Data.CollectionInfo.Count];
	Character->Data.CollectionInfo.Count += 1;
	Character->SyncMask.CollectionInfo = true;
	memset(Slot, 0, sizeof(struct _RTCollectionSlot));
	Slot->TypeID = TypeID;
	Slot->CollectionID = CollectionID;
	Slot->MissionID = MissionID;
	return Slot;
}
