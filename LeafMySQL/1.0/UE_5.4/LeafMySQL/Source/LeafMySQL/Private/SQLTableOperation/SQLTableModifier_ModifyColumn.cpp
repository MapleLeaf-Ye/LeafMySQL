// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#include "SQLTableOperation/SQLTableModifier_ModifyColumn.h"


using namespace LeafMySql::Command::DataTable;

USQLTableModifier_ModifyColumn::USQLTableModifier_ModifyColumn(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	InfoPtr = TArray<Command::DataTable::FLeafMySqlDataTableModify*>();
	ModifiedColumnParam = MakeShareable(new Command::DataTable::FLeafMySqlDataTableParamInfoBase(Command::DataTable::FLeafMySqlDataTableParamInfoBase::NoValidType));
	LinkInfoPtr = nullptr;
}

void USQLTableModifier_ModifyColumn::InitializeTableModifier(
	FLeafMySqlLinkInfo& InLinkInfoPtr,
	const FString& InDataTableName)
{
	Super::InitializeTableModifier(InLinkInfoPtr, InDataTableName);
}

void USQLTableModifier_ModifyColumn::ModifyIntegerColumn(
	const FString & InColumnName,
	const ELeafIntegerColumnType InIntegerColumnType,
	const bool InUnsigned,
	const bool InNotNull,
	const bool InUnique,
	const bool InAutoIncrement,
	const bool InSetDefaultValue,
	const int32 InDefaultValue,
	const FString & InComment,
	const FString & InChecker)
{
	if (!LinkInfoPtr)
	{
		ensureMsgf(LinkInfoPtr, TEXT("LinkInfo is not valid!"));
		return;
	}
	auto tmp = NativeModifyIntegerColumn(
		InColumnName,
		InIntegerColumnType,
		InUnsigned,
		InNotNull,
		InUnique,
		InAutoIncrement,
		InSetDefaultValue,
		InDefaultValue,
		InComment,
		InChecker);

	FLeafMySqlDataTableModify_ModifyColumn* modifyInfo = new FLeafMySqlDataTableModify_ModifyColumn(*LinkInfoPtr, DataTableName, tmp);
	InfoPtr.Add(modifyInfo);
}

void USQLTableModifier_ModifyColumn::ModifyIntegerColumn_Int64(
	const FString & InColumnName,
	const ELeafIntegerColumnType InIntegerColumnType,
	const bool InUnsigned,
	const bool InNotNull,
	const bool InUnique,
	const bool InAutoIncrement,
	const bool InSetDefaultValue,
	const int64 InDefaultValue,
	const FString & InComment,
	const FString & InChecker)
{
	if (!LinkInfoPtr)
	{
		ensureMsgf(LinkInfoPtr, TEXT("LinkInfo is not valid!"));
		return;
	}
	auto tmp = NativeModifyIntegerColumn_Int64(
		InColumnName,
		InIntegerColumnType,
		InUnsigned,
		InNotNull,
		InUnique,
		InAutoIncrement,
		InSetDefaultValue,
		InDefaultValue,
		InComment,
		InChecker);

	FLeafMySqlDataTableModify_ModifyColumn* modifyInfo = new FLeafMySqlDataTableModify_ModifyColumn(*LinkInfoPtr, DataTableName, tmp);
	InfoPtr.Add(modifyInfo);
}

void USQLTableModifier_ModifyColumn::ModifyFloatColumn(
	const FString & InColumnName,
	const ELeafFloatColumnType InFloatColumnType,
	const bool InUnsigned,
	const bool InNotNull,
	const bool InUnique,
	const int32 InM,
	const int32 InD,
	const bool InAutoIncrement,
	const bool InSetDefaultValue,
	const double InDefaultValue,
	const FString & InComment,
	const FString & InChecker)
{
	if (!LinkInfoPtr)
	{
		ensureMsgf(LinkInfoPtr, TEXT("LinkInfo is not valid!"));
		return;
	}
	auto tmp = NativeModifyFloatColumn(
		InColumnName,
		InFloatColumnType,
		InUnsigned,
		InNotNull,
		InUnique,
		InM,
		InD,
		InAutoIncrement,
		InSetDefaultValue,
		InDefaultValue,
		InComment,
		InChecker);

	FLeafMySqlDataTableModify_ModifyColumn* modifyInfo = new FLeafMySqlDataTableModify_ModifyColumn(*LinkInfoPtr, DataTableName, tmp);
	InfoPtr.Add(modifyInfo);
}

void USQLTableModifier_ModifyColumn::ModifyDateColumn(
	const FString & InColumnName,
	const ELeafDateColumnType InDateColumnType,
	const bool InNotNull,
	const bool InUnique,
	const bool InSetDefaultValue,
	const FDateTime InDefaultValue,
	const FString & InComment,
	const FString & InChecker)
{
	if (!LinkInfoPtr)
	{
		ensureMsgf(LinkInfoPtr, TEXT("LinkInfo is not valid!"));
		return;
	}
	auto tmp = NativeModifyDateColumn(
		InColumnName,
		InDateColumnType,
		InNotNull,
		InUnique,
		InSetDefaultValue,
		InDefaultValue,
		InComment,
		InChecker);

	FLeafMySqlDataTableModify_ModifyColumn* modifyInfo = new FLeafMySqlDataTableModify_ModifyColumn(*LinkInfoPtr, DataTableName, tmp);
	InfoPtr.Add(modifyInfo);
}

void USQLTableModifier_ModifyColumn::ModifyCharColumn(
	const FString & InColumnName,
	const ELeafCharColumnType InCharColumnType,
	const bool InNotNull,
	const bool InUnique,
	const int64 InCharSize,
	const bool InSetDefaultValue,
	const FString & InDefaultValue,
	const FString & InComment,
	const FString & InChecker)
{
	if (!LinkInfoPtr)
	{
		ensureMsgf(LinkInfoPtr, TEXT("LinkInfo is not valid!"));
		return;
	}
	auto tmp = NativeModifyCharColumn(
		InColumnName,
		InCharColumnType,
		InNotNull,
		InUnique,
		InCharSize,
		InSetDefaultValue,
		InDefaultValue,
		InComment,
		InChecker);

	FLeafMySqlDataTableModify_ModifyColumn* modifyInfo = new FLeafMySqlDataTableModify_ModifyColumn(*LinkInfoPtr, DataTableName, tmp);
	InfoPtr.Add(modifyInfo);
}

void USQLTableModifier_ModifyColumn::ModifyEnumColumn(
	const FString & InColumnName,
	const TArray<FString>&InEnumElements,
	const bool InNotNull,
	const bool InUnique,
	const bool InSetDefaultValue,
	const FString & InDefaultValue,
	const FString & InComment,
	const FString & InChecker)
{
	if (!LinkInfoPtr)
	{
		ensureMsgf(LinkInfoPtr, TEXT("LinkInfo is not valid!"));
		return;
	}
	auto tmp = NativeModifyEnumColumn(
		InColumnName,
		InEnumElements,
		InNotNull,
		InUnique,
		InSetDefaultValue,
		InDefaultValue,
		InComment,
		InChecker);

	FLeafMySqlDataTableModify_ModifyColumn* modifyInfo = new FLeafMySqlDataTableModify_ModifyColumn(*LinkInfoPtr, DataTableName, tmp);
	InfoPtr.Add(modifyInfo);
}

void USQLTableModifier_ModifyColumn::ModifySetColumn(
	const FString & InColumnName,
	const TArray<FString>&InSetElements,
	const bool InNotNull,
	const bool InUnique,
	const bool InSetDefaultValue,
	const FString & InDefaultValue,
	const FString & InComment,
	const FString & InChecker)
{
	if (!LinkInfoPtr)
	{
		ensureMsgf(LinkInfoPtr, TEXT("LinkInfo is not valid!"));
		return;
	}
	auto tmp = NativeModifySetColumn(
		InColumnName,
		InSetElements,
		InNotNull,
		InUnique,
		InSetDefaultValue,
		InDefaultValue,
		InComment,
		InChecker);

	FLeafMySqlDataTableModify_ModifyColumn* modifyInfo = new FLeafMySqlDataTableModify_ModifyColumn(*LinkInfoPtr, DataTableName, tmp);
	InfoPtr.Add(modifyInfo);
}

void USQLTableModifier_ModifyColumn::ModifySpatialColumn(
	const FString & InColumnName,
	const ELeafSpatialColumnType InSpatialColumnType,
	const bool InNotNull,
	const bool InUnique,
	const bool InSetDefaultValue,
	const FString & InDefaultValue,
	const FString & InComment,
	const FString & InChecker)
{
	if (!LinkInfoPtr)
	{
		ensureMsgf(LinkInfoPtr, TEXT("LinkInfo is not valid!"));
		return;
	}
	auto tmp = NativeModifySpatialColumn(
		InColumnName,
		InSpatialColumnType,
		InNotNull,
		InUnique,
		InSetDefaultValue,
		InDefaultValue,
		InComment,
		InChecker);

	FLeafMySqlDataTableModify_ModifyColumn* modifyInfo = new FLeafMySqlDataTableModify_ModifyColumn(*LinkInfoPtr, DataTableName, tmp);
	InfoPtr.Add(modifyInfo);
}