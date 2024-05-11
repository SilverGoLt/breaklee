#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(HONOR_MEDAL_UNLOCK_SLOT) {
    Int32 PacketLength = sizeof(C2S_DATA_HONOR_MEDAL_UNLOCK_SLOT) + (Packet->MaterialSlotCount1 + Packet->MaterialSlotCount2) * sizeof(UInt16);
    if (Packet->Length < PacketLength) goto error;

    UInt16* MaterialSlotIndex1 = &Packet->MaterialSlotIndex[0];
    UInt16* MaterialSlotIndex2 = &Packet->MaterialSlotIndex[Packet->MaterialSlotCount1];

    if (Character->Info.Profile.Nation != 1 && Character->Info.Profile.Nation != 2) goto error;

    RTDataHonorMedalSlotOpenCategoryRef HonorMedalSlotOpenCategory = RTRuntimeDataHonorMedalSlotOpenCategoryGet(Runtime->Context, Packet->CategoryIndex);
    if (!HonorMedalSlotOpenCategory) goto error;

    RTDataHonorMedalSlotOpenMedalRef HonorMedalSlotOpenMedal1 = NULL;
    RTDataHonorMedalSlotOpenMedalRef HonorMedalSlotOpenMedal2 = NULL;
    for (Index Index = 0; Index < HonorMedalSlotOpenCategory->HonorMedalSlotOpenMedalCount; Index += 1) {
        RTDataHonorMedalSlotOpenMedalRef HonorMedalSlotOpenMedal = &HonorMedalSlotOpenCategory->HonorMedalSlotOpenMedalList[Index];
        if (HonorMedalSlotOpenMedal->Group != Packet->GroupIndex) continue;

        if (!HonorMedalSlotOpenMedal1) {
            HonorMedalSlotOpenMedal1 = HonorMedalSlotOpenMedal;
            continue;
        }

        if (!HonorMedalSlotOpenMedal2) {
            HonorMedalSlotOpenMedal2 = HonorMedalSlotOpenMedal;
            continue;
        }
    }

    if (HonorMedalSlotOpenMedal1) {
        if (HonorMedalSlotOpenMedal1->ItemCount != Packet->MaterialSlotCount1) goto error;

        for (Index Index = 0; Index < Packet->MaterialSlotCount1; Index += 1) {
            RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, MaterialSlotIndex1[Index]);
            if (!ItemSlot || HonorMedalSlotOpenMedal1->ItemID != ItemSlot->Item.ID) goto error;
        }
    }

    if (HonorMedalSlotOpenMedal2) {
        if (HonorMedalSlotOpenMedal2->ItemCount != Packet->MaterialSlotCount2) goto error;

        for (Index Index = 0; Index < Packet->MaterialSlotCount1; Index += 1) {
            RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, MaterialSlotIndex2[Index]);
            if (!ItemSlot || HonorMedalSlotOpenMedal2->ItemID != ItemSlot->Item.ID) goto error;
        }
    }

    if (Packet->MaterialSlotCount2 > 0 && !HonorMedalSlotOpenMedal2) goto error;
    if (!RTCharacterCanAddHonorMedalSlot(Runtime, Character, Packet->CategoryIndex, Packet->GroupIndex, Packet->SlotIndex)) goto error;

    for (Index Index = 0; Index < Packet->MaterialSlotCount1; Index += 1) {
        RTInventoryClearSlot(Runtime, &Character->InventoryInfo, MaterialSlotIndex1[Index]);
    }

    for (Index Index = 0; Index < Packet->MaterialSlotCount1; Index += 1) {
        RTInventoryClearSlot(Runtime, &Character->InventoryInfo, MaterialSlotIndex2[Index]);
    }

    RTCharacterAddHonorMedalSlot(Runtime, Character, Packet->CategoryIndex, Packet->GroupIndex, Packet->SlotIndex);
    Character->SyncMask.InventoryInfo = true;
    Character->SyncPriority.High = true;

    S2C_DATA_HONOR_MEDAL_UNLOCK_SLOT* Response = PacketBufferInit(Connection->PacketBuffer, S2C, HONOR_MEDAL_UNLOCK_SLOT);
    Response->CategoryIndex = Packet->CategoryIndex;
    Response->GroupIndex = Packet->GroupIndex;
    Response->SlotIndex = Packet->SlotIndex;
    Response->Result = 0;
    SocketSend(Socket, Connection, Response);
    return;

error:
    {
        S2C_DATA_HONOR_MEDAL_UNLOCK_SLOT* Response = PacketBufferInit(Connection->PacketBuffer, S2C, HONOR_MEDAL_UNLOCK_SLOT);
        Response->CategoryIndex = Packet->CategoryIndex;
        Response->GroupIndex = Packet->GroupIndex;
        Response->SlotIndex = Packet->SlotIndex;
        Response->Result = 1;
        SocketSend(Socket, Connection, Response);
        return;
    }
}

CLIENT_PROCEDURE_BINDING(HONOR_MEDAL_ROLL_SLOT) {
    RTHonorMedalSlotRef Slot = RTCharacterGetHonorMedalSlot(Runtime, Character, Packet->CategoryIndex, Packet->GroupIndex, Packet->SlotIndex);
    if (!Slot) goto error;
    if (Slot->ForceEffectIndex > 0 || !Slot->IsUnlocked) goto error;

    RTDataHonorMedalSlotPriceCategoryRef PriceCategory = RTRuntimeDataHonorMedalSlotPriceCategoryGet(Runtime->Context, Packet->CategoryIndex);
    if (!PriceCategory) goto error;

    RTDataHonorMedalSlotPriceMedalRef PriceMedal = RTRuntimeDataHonorMedalSlotCountMedalGet(PriceCategory, Packet->GroupIndex);
    if (!PriceMedal) goto error;

    RTDataHonorMedalUpgradeCategoryRef Category = RTRuntimeDataHonorMedalUpgradeCategoryGet(Runtime->Context, Packet->CategoryIndex);
    if (!Category) goto error;

    RTDataHonorMedalUpgradeGroupRef Group = RTRuntimeDataHonorMedalUpgradeGroupGet(Category, Packet->GroupIndex);
    if (!Group) goto error;

    if (Character->Info.Ability.Point < PriceMedal->AP) goto error;
    if (Character->Info.Honor.Exp < PriceMedal->WarExp) goto error;

    Int32 Seed = (Int32)PlatformGetTickCount();
    Int32 DropRate = RandomRange(&Seed, 0, 1000);
    Int32 DropRateOffset = 0;
    for (Int32 Index = 0; Index < Group->HonorMedalUpgradeMedalCount; Index += 1) {
        if (DropRate <= Group->HonorMedalUpgradeMedalList[Index].Rate + DropRateOffset) {
            Slot->ForceEffectIndex = Group->HonorMedalUpgradeMedalList[Index].ForceEffectIndex;
            break;
        }

        DropRateOffset += Group->HonorMedalUpgradeMedalList[Index].Rate;
    }

    Character->Info.Ability.Point -= PriceMedal->AP;
    Character->Info.Honor.Exp -= PriceMedal->WarExp;
    Character->SyncMask.Info = true;
    Character->SyncMask.HonorMedalInfo = true;
    Character->SyncPriority.High = true;

    S2C_DATA_HONOR_MEDAL_ROLL_SLOT* Response = PacketBufferInit(Connection->PacketBuffer, S2C, HONOR_MEDAL_ROLL_SLOT);
    Response->CategoryIndex = Packet->CategoryIndex;
    Response->GroupIndex = Packet->GroupIndex;
    Response->SlotIndex = Packet->SlotIndex;
    Response->ForceEffectIndex = Slot->ForceEffectIndex;
    Response->WExp = Character->Info.Honor.Exp;
    Response->AP = Character->Info.Ability.Point;
    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}