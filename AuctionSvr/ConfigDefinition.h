#ifndef CONFIG_BEGIN
#define CONFIG_BEGIN(__NAMESPACE__)
#endif

#ifndef CONFIG_PARAMETER
#define CONFIG_PARAMETER(__TYPE__, __NAME__, __PATH__, __DEFAULT__)
#endif

#ifndef CONFIG_PARAMETER_ARRAY
#define CONFIG_PARAMETER_ARRAY(__TYPE__, __LENGTH__, __NAME__, __PATH__, __DEFAULT__)
#endif

#ifndef CONFIG_END
#define CONFIG_END(__NAMESPACE__)
#endif

CONFIG_BEGIN(AuctionSvr)
CONFIG_PARAMETER(UInt8, GroupIndex, "AuctionSvr.GroupIndex", 21)
CONFIG_PARAMETER(Bool, Maintenance, "AuctionSvr.Maintenance", 0)
CONFIG_PARAMETER(UInt16, Port, "AuctionSvr.Port", 38141)
CONFIG_END(AuctionSvr)

CONFIG_BEGIN(AuctionDB)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "AuctionSvrDB.Host", 127.0.0.1)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Username, "AuctionSvrDB.Username", root)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Password, "AuctionDB.Password", cabal)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Database, "AuctionDB.Database", auction)
CONFIG_PARAMETER(UInt16, Port, "AuctionDB.Port", 3310)
CONFIG_PARAMETER(Bool, AutoReconnect, "AuctionDB.AutoReconnect", 1)
CONFIG_END(AuctionDB)

CONFIG_BEGIN(MasterSvr)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "MasterSvr.Host", 127.0.0.1)
CONFIG_PARAMETER(UInt16, Port, "MasterSvr.Port", 38161)
CONFIG_PARAMETER(UInt64, Timeout, "MasterSvr.Timeout", 1000)
CONFIG_END(MasterSvr)

CONFIG_BEGIN(NetLib)
CONFIG_PARAMETER(UInt16, ProtocolIdentifier, "NetLib.ProtocolIdentifier", 0xB7D9)
CONFIG_PARAMETER(UInt16, ProtocolVersion, "NetLib.ProtocolVersion", 0x0009)
CONFIG_PARAMETER(UInt16, ProtocolExtension, "NetLib.ProtocolExtension", 0x1111)
CONFIG_PARAMETER(Index, ReadBufferSize, "NetLib.ReadBufferSize", 0x1FFFF)
CONFIG_PARAMETER(Index, WriteBufferSize, "NetLib.WriteBufferSize", 0x1FFFF)
CONFIG_PARAMETER(Bool, LogPackets, "NetLib.LogPackets", 0)
CONFIG_PARAMETER(Bool, UseEncryption, "NetLib.UseEncryption", 1)
CONFIG_END(NetLib)

#undef CONFIG_BEGIN
#undef CONFIG_PARAMETER
#undef CONFIG_PARAMETER_ARRAY
#undef CONFIG_END
