#include "AnimaMastery.h"
#include "Character.h"
#include "Force.h"
#include "Runtime.h"

Void RTCharacterInitializeAnimaMastery(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    if (Character->Data.AnimaMasteryInfo.Info.PresetCount <= Character->Data.AnimaMasteryInfo.Info.ActivePresetIndex) return;

    RTAnimaMasteryPresetDataRef PresetData = &Character->Data.AnimaMasteryInfo.PresetData[Character->Data.AnimaMasteryInfo.Info.ActivePresetIndex];

    for (Int32 Index = 0; Index < RUNTIME_MAX_ANIMA_MASTERY_CATEGORY_COUNT; Index += 1) {
        RTAnimaMasteryPresetSlot PresetSlot = PresetData->CategoryOrder[Index];
        RTAnimaMasteryCategoryDataRef CategoryData = RTCharacterAnimaMasteryGetCategoryData(Runtime, Character, PresetSlot.StorageIndex, PresetSlot.CategoryIndex);
        if (!CategoryData) continue;

        for (Int32 SlotIndex = 0; SlotIndex < RUNTIME_MAX_ANIMA_MASTERY_SLOT_COUNT; SlotIndex += 1) {
            UInt8 ForceEffectOrder = CategoryData->MasterySlots[SlotIndex];
            if (ForceEffectOrder < 1) break;

            RTDataAnimaMasteryValueRef AnimaMasteryValue = RTRuntimeDataAnimaMasteryValueGet(Runtime->Context, PresetSlot.CategoryIndex, ForceEffectOrder);
            if (!AnimaMasteryValue) continue;

            RTCharacterApplyForceEffect(
                Runtime,
                Character,
                AnimaMasteryValue->ForceEffectIndex,
                AnimaMasteryValue->ForceValue,
                (AnimaMasteryValue->ForceValueType == 1) ? RUNTIME_FORCE_VALUE_TYPE_ADDITIVE : RUNTIME_FORCE_VALUE_TYPE_MULTIPLICATIVE
            );
        }
    }
}

UInt8 RTCharacterAnimaMasteryTrainSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex,
    Int32 MasterySlotIndex,
    Int32 MaterialSlotCount,
    UInt16* MaterialSlotIndex
) {
    if (CategoryIndex < 0 || CategoryIndex >= RUNTIME_MAX_ANIMA_MASTERY_CATEGORY_COUNT) return 0;
    if (MasterySlotIndex < 0 || MasterySlotIndex >= RUNTIME_MAX_ANIMA_MASTERY_SLOT_COUNT) return 0;

    RTAnimaMasteryPresetDataRef PresetData = &Character->Data.AnimaMasteryInfo.PresetData[Character->Data.AnimaMasteryInfo.Info.ActivePresetIndex];
    Int32 StorageIndex = PresetData->CategoryOrder[CategoryIndex].StorageIndex;

    RTAnimaMasteryCategoryDataRef CategoryData = RTCharacterAnimaMasteryGetCategoryData(Runtime, Character, StorageIndex, CategoryIndex);
    if (!CategoryData) return 0;

    if (MasterySlotIndex > 0 && CategoryData->MasterySlots[MasterySlotIndex - 1] < 1) return 0;

    RTDataAnimaMasteryPriceRef Price = RTRuntimeDataAnimaMasteryPriceGet(Runtime->Context, CategoryIndex);
    if (!Price) return 0;

    if (Price->SlotPrice > 0 && Character->Data.Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] < Price->SlotPrice) return 0;

    if (Price->RequiredItemID > 0 && Price->RequiredItemCount > 0) {
        Int32 ConsumableItemCount = 0;
        for (Int32 Index = 0; Index < MaterialSlotCount; Index += 1) {
            RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, MaterialSlotIndex[Index]);
            if (!ItemSlot) return 0;

            RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
            if (!ItemData) return 0;

            UInt64 StackSizeMask = RTItemDataGetStackSizeMask(ItemData);
            UInt64 StackSize = ItemSlot->ItemOptions & StackSizeMask;
            ConsumableItemCount += StackSize;
        }

        if (ConsumableItemCount < Price->RequiredItemCount) return 0;
    }

    if (Price->SlotPrice > 0) {
        Character->Data.Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] -= Price->SlotPrice;
        Character->SyncMask.Info = true;
    }

    if (Price->RequiredItemID > 0 && Price->RequiredItemCount > 0) {
        Int32 RemainingItemCount = Price->RequiredItemCount;
        for (Int32 Index = 0; Index < MaterialSlotCount; Index += 1) {
            RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, MaterialSlotIndex[Index]);
            if (!ItemSlot) return 0;

            RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
            if (!ItemData) return 0;

            UInt64 StackSizeMask = RTItemDataGetStackSizeMask(ItemData);
            Int64 StackSize = ItemSlot->ItemOptions & StackSizeMask;
            Int64 ConsumedCount = MIN(RemainingItemCount, StackSize);
            Int64 RemainingStackSize = StackSize - ConsumedCount;
            if (RemainingStackSize <= 0) {
                RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, MaterialSlotIndex[Index]);
            }
            else {
                ItemSlot->ItemOptions |= (ItemSlot->ItemOptions & ~StackSizeMask) | RemainingStackSize;
            }

            RemainingItemCount -= ConsumedCount;
        }

        Character->SyncMask.InventoryInfo = true;
    }

    Int32 ForceEffectCount = 0;
    for (Int32 Index = 0; Index < Runtime->Context->AnimaMasteryValueCount; Index += 1) {
        if (Runtime->Context->AnimaMasteryValueList[Index].CategoryIndex != CategoryIndex) continue;

        ForceEffectCount += 1;
    }

    Int32 Seed = (Int32)PlatformGetTickCount();
    Int32 RandomRate = RandomRange(&Seed, 0, ForceEffectCount * 1000);
    Int32 RandomRateOffset = 0;
    Int32 ForceEffectOrder = 0;
    for (Int32 Index = 0; Index < Runtime->Context->AnimaMasteryValueCount; Index += 1) {
        if (Runtime->Context->AnimaMasteryValueList[Index].CategoryIndex != CategoryIndex) continue;

        RandomRateOffset += 1000;
        if (RandomRate <= RandomRateOffset) {
            ForceEffectOrder = Runtime->Context->AnimaMasteryValueList[Index].ForceEffectOrder;
            break;
        }
    }

    assert(ForceEffectOrder > 0);
    CategoryData->MasterySlots[MasterySlotIndex] = ForceEffectOrder;
    Character->SyncMask.AnimaMasteryInfo = true;
    RTCharacterInitializeAttributes(Runtime, Character);

    return CategoryData->MasterySlots[MasterySlotIndex];
}

Bool RTCharacterAnimaMasteryResetSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex,
    Int32 StorageIndex
) {
    if (!RTCharacterAnimaMasteryIsCategoryUnlocked(Runtime, Character, CategoryIndex)) return false;
    if (StorageIndex < 0 || StorageIndex >= RUNTIME_MAX_ANIMA_MASTERY_STORAGE_COUNT) return false;

    RTDataAnimaMasteryPriceRef Price = RTRuntimeDataAnimaMasteryPriceGet(Runtime->Context, CategoryIndex);
    if (!Price) return false;

    // TODO: Consume force gem or reset kit!

    RTAnimaMasteryCategoryDataRef CategoryData = RTCharacterAnimaMasteryGetCategoryData(Runtime, Character, StorageIndex, CategoryIndex);
    if (!CategoryData) return false;

    memset(CategoryData->MasterySlots, 0, sizeof(CategoryData->MasterySlots));
    RTCharacterInitializeAttributes(Runtime, Character);

    return true;
}

Bool RTCharacterAnimaMasteryIsCategoryUnlocked(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex
) {
    if (Character->Data.AnimaMasteryInfo.Info.UnlockedCategoryFlags & (1 << CategoryIndex)) return true;

    RTDataAnimaMasteryPriceRef Price = RTRuntimeDataAnimaMasteryPriceGet(Runtime->Context, CategoryIndex);
    if (!Price) return false;

    return Price->OpenPrice <= 0;
}

Bool RTCharacterAnimaMasteryUnlockCategory(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex
) {
    if (RTCharacterAnimaMasteryIsCategoryUnlocked(Runtime, Character, CategoryIndex)) return true;

    RTDataAnimaMasteryPriceRef Price = RTRuntimeDataAnimaMasteryPriceGet(Runtime->Context, CategoryIndex);
    if (!Price) return false;

    if (Price->OpenPrice > 0) {
        if (Character->Data.Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] < Price->OpenPrice) return false;

        Character->Data.Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] -= Price->OpenPrice;
        Character->SyncMask.Info = true;
    }

    Character->Data.AnimaMasteryInfo.Info.UnlockedCategoryFlags |= (1 << CategoryIndex);
    Character->SyncMask.AnimaMasteryInfo = true;
    return true;
}

RTAnimaMasteryCategoryDataRef RTCharacterAnimaMasteryGetCategoryData(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 StorageIndex,
    Int32 CategoryIndex
) {
    if (StorageIndex < 0 || StorageIndex >= RUNTIME_MAX_ANIMA_MASTERY_STORAGE_COUNT) return NULL;
    if (CategoryIndex < 0 || CategoryIndex >= RUNTIME_MAX_ANIMA_MASTERY_CATEGORY_COUNT) return NULL;
    
    for (Int32 Index = 0; Index < Character->Data.AnimaMasteryInfo.Info.StorageCount; Index += 1) {
        RTAnimaMasteryCategoryDataRef CategoryData = &Character->Data.AnimaMasteryInfo.CategoryData[Index];
        if (CategoryData->CategoryIndex != CategoryIndex || CategoryData->StorageIndex != StorageIndex) continue;

        return CategoryData;
    }

    assert(Character->Data.AnimaMasteryInfo.Info.StorageCount < RUNTIME_MAX_ANIMA_MASTERY_STORAGE_COUNT * RUNTIME_MAX_ANIMA_MASTERY_CATEGORY_COUNT);

    Int32 InsertionIndex = -1;
    for (Int32 Index = 0; Index < Character->Data.AnimaMasteryInfo.Info.StorageCount; Index += 1) {
        RTAnimaMasteryCategoryDataRef CategoryData = &Character->Data.AnimaMasteryInfo.CategoryData[Index];
        if (CategoryData->StorageIndex < StorageIndex) continue;
        if (CategoryData->CategoryIndex < CategoryIndex) continue;

        InsertionIndex = Index;
        break;
    }

    if (InsertionIndex >= 0) {
        Int32 TailLength = Character->Data.AnimaMasteryInfo.Info.StorageCount - InsertionIndex;
        if (TailLength > 0) {
            memmove(&Character->Data.AnimaMasteryInfo.CategoryData[InsertionIndex + 1], &Character->Data.AnimaMasteryInfo.CategoryData[InsertionIndex], TailLength * sizeof(struct _RTAnimaMasteryCategoryData));
        }
    }
    else {
        InsertionIndex = Character->Data.AnimaMasteryInfo.Info.StorageCount;
    }
    
    RTAnimaMasteryCategoryDataRef CategoryData = &Character->Data.AnimaMasteryInfo.CategoryData[InsertionIndex];
    CategoryData->CategoryIndex = CategoryIndex;
    CategoryData->StorageIndex = StorageIndex;
    memset(CategoryData->MasterySlots, 0, sizeof(CategoryData->MasterySlots));
    Character->Data.AnimaMasteryInfo.Info.StorageCount += 1;
    Character->SyncMask.AnimaMasteryInfo = true;
    return CategoryData;
}

Bool RTCharacterAnimaMasterySetActiveStorageIndex(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex,
    Int32 StorageIndex
) {
    if (!RTCharacterAnimaMasteryIsCategoryUnlocked(Runtime, Character, CategoryIndex)) return false;
    if (StorageIndex < 0 || StorageIndex >= RUNTIME_MAX_ANIMA_MASTERY_STORAGE_COUNT) return false;

    if (!RTCharacterAnimaMasteryGetCategoryData(Runtime, Character, StorageIndex, CategoryIndex)) return false;

    RTAnimaMasteryPresetDataRef PresetData = &Character->Data.AnimaMasteryInfo.PresetData[Character->Data.AnimaMasteryInfo.Info.ActivePresetIndex];
    PresetData->CategoryOrder[CategoryIndex].StorageIndex = StorageIndex;
    PresetData->CategoryOrder[CategoryIndex].CategoryIndex = CategoryIndex;
    Character->SyncMask.AnimaMasteryInfo = true;
    RTCharacterInitializeAttributes(Runtime, Character);
    return true;
}

Bool RTCharacterAnimaMasterySetActivePresetIndex(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PresetIndex
) {
    if (PresetIndex < 0 || PresetIndex >= Character->Data.AnimaMasteryInfo.Info.PresetCount) return false;
    
    Character->Data.AnimaMasteryInfo.Info.ActivePresetIndex = PresetIndex;
    Character->SyncMask.AnimaMasteryInfo = true;
    RTCharacterInitializeAttributes(Runtime, Character);
    return true;
}
