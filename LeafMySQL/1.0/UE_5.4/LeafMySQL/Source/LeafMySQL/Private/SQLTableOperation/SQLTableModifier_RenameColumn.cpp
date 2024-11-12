// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#include "SQLTableOperation/SQLTableModifier_RenameColumn.h"


using namespace LeafMySql::Command::DataTable;

USQLTableModifier_RenameColumn::USQLTableModifier_RenameColumn(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	InfoPtr = TArray<Command::DataTable::FLeafMySqlDataTableModify*>();
	ModifiedColumnParam = MakeShareable(new Command::DataTable::FLeafMySqlDataTableParamInfoBase(Command::DataTable::FLeafMySqlDataTableParamInfoBase::NoValidType));
	LinkInfoPtr = nullptr;
}

void USQLTableModifier_RenameColumn::InitializeTableModifier(
	FLeafMySqlLinkInfo& InLinkInfoPtr,
	const FString& InDataTableName)
{
	Super::InitializeTableModifier(InLinkInfoPtr, InDataTableName);
}

void USQLTableModifier_RenameColumn::RenameNewColumn(const FString& InColumnName, const FString& InColumnNewName)
{
	if (!LinkInfoPtr)
	{
		ensureMsgf(LinkInfoPtr, TEXT("LinkInfo is not valid!"));
		return;
	}

	FLeafMySqlDataTableModify_RenameColumn* renameInfo = new FLeafMySqlDataTableModify_RenameColumn(*LinkInfoPtr, DataTableName, InColumnName, InColumnNewName);
	InfoPtr.Add(renameInfo);
}
