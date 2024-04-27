#include "IPCProtocol.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(W2P, PARTY_INVITE_CANCEL) {
    Index* PartyInvitationPoolIndex = DictionaryLookup(Context->CharacterToPartyInvite, &Packet->TargetCharacterIndex);
    if (!PartyInvitationPoolIndex) goto error;

    RTPartyInvitationRef Invitation = (RTPartyInvitationRef)MemoryPoolFetch(Context->PartyInvitationPool, *PartyInvitationPoolIndex);
    if (!Invitation) goto error;

    RTPartyRef Party = ServerGetPartyByCharacter(Context, Packet->SourceCharacterIndex);
    if (!Party) goto error;

    IPC_P2W_DATA_PARTY_INVITE_TIMEOUT* Notification = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INVITE_TIMEOUT);
    Notification->Header.Source = Server->IPCSocket->NodeID;
    Notification->Header.Target.Group = Context->Config.PartySvr.GroupIndex;
    Notification->Header.Target.Index = Packet->TargetNodeIndex;
    Notification->Header.Target.Type = IPC_TYPE_WORLD;
    Notification->CharacterIndex = Packet->TargetCharacterIndex;
    Notification->IsAccept = 0;
    Notification->IsCancel = true;
    IPCSocketUnicast(Socket, Notification);

    MemoryPoolRelease(Context->PartyInvitationPool, *PartyInvitationPoolIndex);
    DictionaryRemove(Context->CharacterToPartyInvite, &Packet->TargetCharacterIndex);

    if (Party->PartyType == RUNTIME_PARTY_TYPE_TEMPORARY) {
        ServerDestroyParty(Context, Party);
    }

    IPC_P2W_DATA_PARTY_INVITE_CANCEL* Response = IPCPacketBufferInit(Connection->PacketBuffer, P2W, PARTY_INVITE_CANCEL);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target = Packet->Header.Source;
    Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
    IPCSocketUnicast(Socket, Response);
    return;

error:
    return;
}