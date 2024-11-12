// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#include "SQLTableOperation/SQLTableDestroyer.h"



USQLTableDestroyer::USQLTableDestroyer(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	DeleteInfoPtr = nullptr;
}

void USQLTableDestroyer::InitializeTableDestroyer(
	FLeafMySqlLinkInfo& InLinkInfoPtr,
	const FString& InDataTableName,
	const bool InCheckIsExists, 
	const bool InJustClearData)
{
	DeleteInfoPtr = new Command::DataTable::FLeafMySqlDataTableDelete(InLinkInfoPtr, InDataTableName, InCheckIsExists, InJustClearData);
}

USQLTableDestroyer::~USQLTableDestroyer()
{
	delete DeleteInfoPtr;
	DeleteInfoPtr = nullptr;
}
