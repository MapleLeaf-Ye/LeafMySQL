// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#include "SQLTableOperation/SQLTableModifier_RemoveColumn.h"


using namespace LeafMySql::Command::DataTable;

USQLTableModifier_RemoveColumn::USQLTableModifier_RemoveColumn(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	InfoPtr = TArray<Command::DataTable::FLeafMySqlDataTableModify*>();
	ModifiedColumnParam = MakeShareable(new Command::DataTable::FLeafMySqlDataTableParamInfoBase(Command::DataTable::FLeafMySqlDataTableParamInfoBase::NoValidType));
	LinkInfoPtr = nullptr;
}

void USQLTableModifier_RemoveColumn::InitializeTableModifier(
	FLeafMySqlLinkInfo& InLinkInfoPtr, 
	const FString& InDataTableName)
{
	Super::InitializeTableModifier(InLinkInfoPtr, InDataTableName);
}

void USQLTableModifier_RemoveColumn::RemoveNewColumn(const FString& InColumnName)
{
	if (!LinkInfoPtr)
	{
		ensureMsgf(LinkInfoPtr, TEXT("LinkInfo is not valid!"));
		return;
	}

	FLeafMySqlDataTableModify_RemoveColumn* removeInfo = new FLeafMySqlDataTableModify_RemoveColumn(*LinkInfoPtr, DataTableName, InColumnName);
	InfoPtr.Add(removeInfo);
}
