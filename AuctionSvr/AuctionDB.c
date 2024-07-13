#include "AuctionDB.h"

static StatementRef kStatementTable[AUCTIONDB_STATEMENT_COUNT];

Void AuctionDBPrepareStatements(
	DatabaseRef Database
) {
#define STATEMENT(__NAME__, __QUERY__)										\
	Info("Create Statement: %s", #__NAME__);	\
	kStatementTable[__NAME__] = DatabaseCreateStatement(					\
    	Database,															\
    	__QUERY__															\
	);
#include "AuctionDBStatements.h"
}

StatementRef AuctionDBGetStatement(
	DatabaseRef Database,
	Int32 StatementID
) {
	assert(0 <= StatementID && StatementID < AUCTIONDB_STATEMENT_COUNT);

	return kStatementTable[StatementID];
}

Bool AuctionDBInsertAccount(
	DatabaseRef Database,
	CString Username,
	CString Email,
	UInt8* Salt,
	Int32 SaltLength,
	UInt8* Hash,
	Int32 HashLength
) {
	assert(SaltLength == MAX_PASSWORD_SALT_LENGTH);
	assert(HashLength == MAX_PASSWORD_HASH_LENGTH);

	StatementRef Statement = AuctionDBGetStatement(Database, AUCTIONDB_INSERT_ACCOUNT);
	StatementBindParameterString(Statement, 0, Username);
	StatementBindParameterString(Statement, 1, Email);
	StatementBindParameterBinary(Statement, 2, Salt, SaltLength);
	StatementBindParameterBinary(Statement, 3, Hash, HashLength);

	if (!StatementExecute(Statement)) return false;

	return true;
}

Bool AuctionDBSelectAccountByID(
	DatabaseRef Database,
	Int64 AccountID,
	AUCTIONDB_DATA_ACCOUNT* Result
) {
	StatementRef Statement = AuctionDBGetStatement(Database, AUCTIONDB_SELECT_ACCOUNT_BY_ID);
	StatementBindParameterInt64(Statement, 0, AccountID);

	if (!StatementExecute(Statement)) return false;
	if (!StatementFetchResult(Statement)) return false;

	Int64 SaltLength;
	Int64 HashLength;

	StatementReadResultInt32(Statement, 0, &Result->ID);
	StatementReadResultString(Statement, 1, sizeof(Result->Username), Result->Username);
	StatementReadResultString(Statement, 2, sizeof(Result->Email), Result->Email);
	StatementReadResultBool(Statement, 3, &Result->EmailVerified);
	StatementReadResultBinary(Statement, 4, sizeof(Result->Salt), Result->Salt, &SaltLength);
	StatementReadResultBinary(Statement, 5, sizeof(Result->Hash), Result->Hash, &HashLength);
	StatementReadResultTimestamp(Statement, 6, &Result->CreatedAt);
	StatementReadResultTimestamp(Statement, 7, &Result->UpdatedAt);
	StatementReadResultTimestamp(Statement, 8, &Result->DeletedAt);

	assert(SaltLength == MAX_PASSWORD_SALT_LENGTH);
	assert(HashLength == MAX_PASSWORD_HASH_LENGTH);

	StatementFlushResults(Statement);

	return true;
}

Bool AuctionDBSelectAccountByUsername(
	DatabaseRef Database,
	CString Username,
	AUCTIONDB_DATA_ACCOUNT* Result
) {
	StatementRef Statement = AuthDBGetStatement(Database, AUCTIONDB_SELECT_ACCOUNT_BY_USERNAME);
	StatementBindParameterString(Statement, 0, Username);

	if (!StatementExecute(Statement)) return false;
	if (!StatementFetchResult(Statement)) return false;

	Int64 SaltLength = 0;
	Int64 HashLength = 0;

	StatementReadResultInt32(Statement, 0, &Result->ID);
	StatementReadResultString(Statement, 1, sizeof(Result->Username), Result->Username);
	StatementReadResultString(Statement, 2, sizeof(Result->Email), Result->Email);
	StatementReadResultBool(Statement, 3, &Result->EmailVerified);
	StatementReadResultBinary(Statement, 4, sizeof(Result->Salt), Result->Salt, &SaltLength);
	StatementReadResultBinary(Statement, 5, sizeof(Result->Hash), Result->Hash, &HashLength);
	StatementReadResultTimestamp(Statement, 6, &Result->CreatedAt);
	StatementReadResultTimestamp(Statement, 7, &Result->UpdatedAt);
	StatementReadResultTimestamp(Statement, 8, &Result->DeletedAt);

	assert(SaltLength == MAX_PASSWORD_SALT_LENGTH);
	assert(HashLength == MAX_PASSWORD_HASH_LENGTH);

	StatementFlushResults(Statement);

	return true;
}

Bool AuctionDBUpsertSession(
	DatabaseRef Database,
	Int32 AccountID,
	Bool Online,
	CString SessionKey,
	CString SessionIP
) {
	StatementRef Statement = AuctionDBGetStatement(Database, AUCTIONDB_UPSERT_SESSION);
	StatementBindParameterInt32(Statement, 0, AccountID);
	StatementBindParameterBool(Statement, 1, Online);
	StatementBindParameterString(Statement, 2, SessionKey);
	StatementBindParameterString(Statement, 3, SessionIP);
	StatementBindParameterBool(Statement, 4, Online);
	StatementBindParameterString(Statement, 5, SessionKey);
	StatementBindParameterString(Statement, 6, SessionIP);

	if (!StatementExecute(Statement)) return false;

	return true;
}

Bool AuctionDBSelectSession(
	DatabaseRef Database,
	Int32 AccountID,
	AUCTIONDB_DATA_SESSION* Result
) {
	StatementRef Statement = AuthDBGetStatement(Database, AUCTIONDB_SELECT_SESSION);
	StatementBindParameterInt32(Statement, 0, AccountID);

	if (!StatementExecute(Statement)) return false;
	if (!StatementFetchResult(Statement)) return false;

	StatementReadResultInt32(Statement, 0, &Result->AccountID);
	StatementReadResultBool(Statement, 1, &Result->Online);
	StatementReadResultString(Statement, 2, sizeof(Result->SessionKey), Result->SessionKey);
	StatementReadResultString(Statement, 3, sizeof(Result->SessionIP), Result->SessionIP);
	StatementReadResultTimestamp(Statement, 4, &Result->SessionAt);
	StatementReadResultTimestamp(Statement, 5, &Result->CreatedAt);
	StatementReadResultTimestamp(Statement, 6, &Result->UpdatedAt);

	StatementFlushResults(Statement);

	return true;
}

Bool AuctionDBInsertBlacklistAccount(
	DatabaseRef Database,
	Int32 AccountID,
	CString Description
) {
	StatementRef Statement = AuctionDBGetStatement(Database, AUCTIONDB_INSERT_BLACKLIST_ACCOUNT);
	StatementBindParameterInt32(Statement, 0, AccountID);
	StatementBindParameterString(Statement, 1, Description);

	if (!StatementExecute(Statement)) return false;

	return true;
}

Bool AuctionDBInsertBlacklistAddress(
	DatabaseRef Database,
	CString AddressIP,
	CString Description
) {
	StatementRef Statement = AuctionDBGetStatement(Database, AUCTIONDB_INSERT_BLACKLIST_ADDRESS);
	StatementBindParameterString(Statement, 0, AddressIP);
	StatementBindParameterString(Statement, 1, Description);

	if (!StatementExecute(Statement)) return false;

	return true;
}

Bool AuctionDBSelectBlacklistByAccount(
	DatabaseRef Database,
	Int32 AccountID,
	AUCTIONDB_DATA_BLACKLIST* Result
) {
	StatementRef Statement = AuctionDBGetStatement(Database, AUCTIONDB_SELECT_BLACKLIST_BY_ID);
	StatementBindParameterInt32(Statement, 0, AccountID);

	if (!StatementExecute(Statement)) return false;
	if (!StatementFetchResult(Statement)) return false;

	StatementReadResultInt32(Statement, 0, &Result->ID);
	StatementReadResultInt32(Statement, 1, &Result->AccountID);
	StatementReadResultString(Statement, 2, sizeof(Result->AddressIP), Result->AddressIP);
	StatementReadResultString(Statement, 3, sizeof(Result->Description), Result->Description);
	StatementReadResultTimestamp(Statement, 4, &Result->CreatedAt);
	StatementReadResultTimestamp(Statement, 5, &Result->UpdatedAt);

	StatementFlushResults(Statement);

	return true;
}

Bool AuctionDBSelectBlacklistByAddress(
	DatabaseRef Database,
	CString AddressIP,
	AUCTIONDB_DATA_BLACKLIST* Result
) {
	StatementRef Statement = AuctionDBGetStatement(Database, AUCTIONDB_SELECT_BLACKLIST_BY_IP);
	StatementBindParameterString(Statement, 0, AddressIP);

	if (!StatementExecute(Statement)) return false;
	if (!StatementFetchResult(Statement)) return false;

	StatementReadResultInt32(Statement, 0, &Result->ID);
	StatementReadResultInt32(Statement, 1, &Result->AccountID);
	StatementReadResultString(Statement, 2, sizeof(Result->AddressIP), Result->AddressIP);
	StatementReadResultString(Statement, 3, sizeof(Result->Description), Result->Description);
	StatementReadResultTimestamp(Statement, 4, &Result->CreatedAt);
	StatementReadResultTimestamp(Statement, 5, &Result->UpdatedAt);

	StatementFlushResults(Statement);

	return true;
}

Bool AuctionDBUpsertCharacter(
	DatabaseRef Database,
	Int32 AccountID,
	UInt8 WorldID,
	UInt8 CharacterCount
) {
	StatementRef Statement = AuctionDBGetStatement(Database, AUCTIONDB_UPSERT_CHARACTER);
	StatementBindParameterInt32(Statement, 0, AccountID);
	StatementBindParameterUInt8(Statement, 1, WorldID);
	StatementBindParameterUInt8(Statement, 2, CharacterCount);
	StatementBindParameterUInt8(Statement, 3, CharacterCount);

	if (!StatementExecute(Statement)) return false;

	return true;
}

StatementRef AuctionDBSelectCharacterByID(
	DatabaseRef Database,
	Int32 AccountID
) {
	StatementRef Statement = AuctionDBGetStatement(Database, AUCTIONDB_SELECT_CHARACTER_BY_ID);
	StatementBindParameterInt32(Statement, 0, AccountID);

	if (!StatementExecute(Statement)) return NULL;

	return Statement;
}

Bool AuctionDBSelectCharacterFetchNext(
	DatabaseRef Database,
	StatementRef Statement,
	AUCTIONDB_DATA_CHARACTER* Result
) {
	assert(Statement);

	if (StatementFetchResult(Statement)) {
		StatementReadResultInt32(Statement, 0, &Result->AccountID);
		StatementReadResultUInt8(Statement, 1, &Result->ServerID);
		StatementReadResultUInt8(Statement, 2, &Result->CharacterCount);
		StatementReadResultTimestamp(Statement, 3, &Result->CreatedAt);
		StatementReadResultTimestamp(Statement, 4, &Result->UpdatedAt);
		return true;
	}

	return false;
}
