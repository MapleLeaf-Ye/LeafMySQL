// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#include "SQLTableOperation/SQLTableConstructor.h"

#include "Kismet/KismetStringLibrary.h"


USQLTableConstructor::USQLTableConstructor(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	CreateInfoPtr = nullptr;
}

void USQLTableConstructor::InitializeTableConstructor(
	FLeafMySqlLinkInfo& InLinkInfoPtr,
	const FString& InDataTableName,
	const FString& InCollate,
	const bool InCheckIsExists, 
	const FString& InEngine,
	const ELeafCharacterSet InCharset)
{
	UEnum* sPtr = StaticEnum<ELeafCharacterSet>();
	FString sName = sPtr->GetNameStringByIndex((uint8)InCharset);
	CreateInfoPtr = new Command::DataTable::FLeafMySqlDataTableCreate(InLinkInfoPtr, InDataTableName, InCheckIsExists, InEngine, sName, InCollate);
}

void USQLTableConstructor::InitializeTableConstructor(
	FLeafMySqlLinkInfo& InLinkInfoPtr,
	const FString& InDataTableName,
	const bool InCheckIsExists,
	const FString& InEngine,
	const FString InCharset,
	const FString& InCollate)
{
	CreateInfoPtr = new Command::DataTable::FLeafMySqlDataTableCreate(InLinkInfoPtr, InDataTableName, InCheckIsExists, InEngine, InCharset, InCollate);
}

USQLTableConstructor::~USQLTableConstructor()
{
	delete CreateInfoPtr;
	CreateInfoPtr = nullptr;
}

void USQLTableConstructor::AddNewIntegerColumn(
	const FString& InColumnName,
	const ELeafIntegerColumnType InIntegerColumnType,
	const bool InUnsigned,
	const bool InNotNull,
	const bool InUnique,
	const bool InIsPrimaryKey,
	const bool InAutoIncrement,
	const bool InSetDefaultValue,
	const int32 InDefaultValue,
	const FString& InComment,
	const FString& InChecker)
{
	if (!CreateInfoPtr)
	{
		return;
	}
	UEnum* sPtr = StaticEnum<ELeafIntegerColumnType>();
	FString sType = sPtr->GetNameStringByIndex((uint8)InIntegerColumnType);
	Command::DataTable::FLeafMySqlDataTableParamInfoBase temp = Command::DataTable::FLeafMySqlDataTableParamInfoBase(
		InColumnName,
		LeafMySql::ELeafMySqlValueType::GetLeafMySqlValueTypeFromString(sType),
		InUnsigned, InNotNull, InUnique, InAutoIncrement, FString(), InComment, InChecker);
	if (InSetDefaultValue)
	{
		temp.SetDefaultParamValue(InDefaultValue);
	}
	CreateInfoPtr->AddOrUpdateParam(temp);
	if (InIsPrimaryKey)
	{
		CreateInfoPtr->AddPrimaryKey(temp);
	}
}

void USQLTableConstructor::AddNewIntegerColumn_Int64(
	const FString& InColumnName,
	const ELeafIntegerColumnType InIntegerColumnType,
	const bool InUnsigned,
	const bool InNotNull,
	const bool InUnique,
	const bool InIsPrimaryKey,
	const bool InAutoIncrement,
	const bool InSetDefaultValue,
	const int64 InDefaultValue, 
	const FString& InComment, 
	const FString& InChecker)
{
	if (!CreateInfoPtr)
	{
		return;
	}
	UEnum* sPtr = StaticEnum<ELeafIntegerColumnType>();
	FString sType = sPtr->GetNameStringByIndex((uint8)InIntegerColumnType);
	Command::DataTable::FLeafMySqlDataTableParamInfoBase temp = Command::DataTable::FLeafMySqlDataTableParamInfoBase(
		InColumnName,
		LeafMySql::ELeafMySqlValueType::GetLeafMySqlValueTypeFromString(sType),
		InUnsigned, InNotNull, InUnique, InAutoIncrement, FString(), InComment, InChecker);
	if (InSetDefaultValue)
	{
		temp.SetDefaultParamValue(InDefaultValue);
	}
	CreateInfoPtr->AddOrUpdateParam(temp);
	if (InIsPrimaryKey)
	{
		CreateInfoPtr->AddPrimaryKey(temp);
	}
}

void USQLTableConstructor::AddNewFloatColumn(
	const FString& InColumnName,
	const ELeafFloatColumnType InFloatColumnType,
	const bool InUnsigned,
	const bool InNotNull,
	const bool InUnique,
	const bool InIsPrimaryKey,
	const int32 InM,
	const int32 InD,
	const bool InAutoIncrement,
	const bool InSetDefaultValue,
	const double InDefaultValue,
	const FString& InComment, 
	const FString& InChecker)
{
	if (!CreateInfoPtr)
	{
		return;
	}
	UEnum* sPtr = StaticEnum<ELeafFloatColumnType>();
	FString sType = sPtr->GetNameStringByIndex((uint8)InFloatColumnType);
	Command::DataTable::FLeafMySqlDataTableParamInfoBase temp = Command::DataTable::FLeafMySqlDataTableParamInfoBase(
		InColumnName,
		InM,
		InD,
		LeafMySql::ELeafMySqlValueType::GetLeafMySqlValueTypeFromString(sType),
		InUnsigned, InNotNull, InUnique, InAutoIncrement, FString(), InComment, InChecker);
	if (InSetDefaultValue)
	{
		temp.SetDefaultParamValue(InDefaultValue);
	}
	CreateInfoPtr->AddOrUpdateParam(temp);
	if (InIsPrimaryKey)
	{
		CreateInfoPtr->AddPrimaryKey(temp);
	}
}

void USQLTableConstructor::AddNewDateColumn(
	const FString& InColumnName,
	const ELeafDateColumnType InDateColumnType,
	const bool InNotNull,
	const bool InUnique,
	const bool InIsPrimaryKey,
	const bool InSetDefaultValue,
	const FDateTime InDefaultValue,
	const FString& InComment,
	const FString& InChecker)
{
	if (!CreateInfoPtr)
	{
		return;
	}
	UEnum* sPtr = StaticEnum<ELeafDateColumnType>();
	FString sType = sPtr->GetNameStringByIndex((uint8)InDateColumnType);
	Command::DataTable::FLeafMySqlDataTableParamInfoBase temp = Command::DataTable::FLeafMySqlDataTableParamInfoBase(
		InColumnName,
		LeafMySql::ELeafMySqlValueType::GetLeafMySqlValueTypeFromString(sType),
		false, InNotNull, InUnique, false, FString(), InComment, InChecker);
	FString sDefaultValue;
	if (InDateColumnType == ELeafDateColumnType::DATE)
	{
		sDefaultValue = FString::Printf(TEXT("%d-%d-%d"), InDefaultValue.GetYear(), InDefaultValue.GetMonth(), InDefaultValue.GetDay());
	}
	else if (InDateColumnType == ELeafDateColumnType::TIME)
	{
		sDefaultValue = FString::Printf(TEXT("%d:%d:%d"), InDefaultValue.GetHour(), InDefaultValue.GetMinute(), InDefaultValue.GetSecond());
	}
	else if (InDateColumnType == ELeafDateColumnType::YEAR)
	{
		sDefaultValue = FString::Printf(TEXT("%d"), InDefaultValue.GetYear());
	}
	else
	{
		sDefaultValue = FString::Printf(TEXT("%d-%d-%d %d:%d:%d"), 
			InDefaultValue.GetYear(), InDefaultValue.GetMonth(), InDefaultValue.GetDay(),
			InDefaultValue.GetHour(), InDefaultValue.GetMinute(), InDefaultValue.GetSecond());
	}
	if (InSetDefaultValue)
	{
		temp.SetDefaultParamValue(sDefaultValue);
	}
	CreateInfoPtr->AddOrUpdateParam(temp);
	if (InIsPrimaryKey)
	{
		CreateInfoPtr->AddPrimaryKey(temp);
	}
}

void USQLTableConstructor::AddNewCharColumn(
	const FString& InColumnName,
	const ELeafCharColumnType InCharColumnType,
	const bool InNotNull,
	const bool InUnique, 
	const bool InIsPrimaryKey,
	const int64 InCharSize, 
	const bool InSetDefaultValue,
	const FString& InDefaultValue,
	const FString& InComment,
	const FString& InChecker)
{
	if (!CreateInfoPtr)
	{
		return;
	}
	UEnum* sPtr = StaticEnum<ELeafCharColumnType>();
	FString sType = sPtr->GetNameStringByIndex((uint8)InCharColumnType);
	Command::DataTable::FLeafMySqlDataTableParamInfoBase temp =
		Command::DataTable::FLeafMySqlDataTableParamInfoBase(
			InColumnName,
			InCharSize,
			ELeafMySqlValueType::GetLeafMySqlValueTypeFromString(sType),
			false,
			InNotNull,
			InUnique,
			false,
			FString(),
			InComment,
			InChecker);
	if (InSetDefaultValue)
	{
		temp.SetDefaultParamValue(InDefaultValue);
	}
	CreateInfoPtr->AddOrUpdateParam(temp);
	if (InIsPrimaryKey)
	{
		CreateInfoPtr->AddPrimaryKey(temp);
	}
}

void USQLTableConstructor::AddNewEnumColumn(
	const FString& InColumnName,
	const TArray<FString>& InEnumElements,
	const bool InNotNull,
	const bool InUnique,
	const bool InIsPrimaryKey,
	const bool InSetDefaultValue,
	const FString& InDefaultValue,
	const FString& InComment,
	const FString& InChecker)
{
	if (!CreateInfoPtr)
	{
		return;
	}
	//FString enumInit = UKismetStringLibrary::JoinStringArray(InEnumElements, TEXT(","));
	FString enumInit;
	for (const auto tmp : InEnumElements)
	{
		enumInit += FLeafMySqlValueHandler(tmp).Get();
		enumInit += TEXT(",");
	}
	enumInit = enumInit.Left(enumInit.Len() - 1);
	Command::DataTable::FLeafMySqlDataTableParamInfoBase temp =
		Command::DataTable::FLeafMySqlDataTableParamInfoBase(
			InColumnName,
			ELeafMySqlValueType::ENUM,
			enumInit,
			false,
			InNotNull,
			InUnique,
			false,
			FString(),
			InComment,
			InChecker);
	if (InSetDefaultValue)
	{
		temp.SetDefaultParamValue(InDefaultValue);
	}
	CreateInfoPtr->AddOrUpdateParam(temp);
	if (InIsPrimaryKey)
	{
		CreateInfoPtr->AddPrimaryKey(temp);
	}
}

void USQLTableConstructor::AddNewSetColumn(
	const FString& InColumnName,
	const TArray<FString>& InSetElements,
	const bool InNotNull,
	const bool InUnique, 
	const bool InIsPrimaryKey,
	const bool InSetDefaultValue,
	const FString& InDefaultValue,
	const FString& InComment,
	const FString& InChecker)
{
	if (!CreateInfoPtr)
	{
		return;
	}
	//FString setInit = UKismetStringLibrary::JoinStringArray(InSetElements, TEXT(","));
	FString setInit;
	for (const auto tmp : InSetElements)
	{
		setInit += FLeafMySqlValueHandler(tmp).Get();
		setInit += TEXT(",");
	}
	setInit = setInit.Left(setInit.Len() - 1);
	Command::DataTable::FLeafMySqlDataTableParamInfoBase temp =
		Command::DataTable::FLeafMySqlDataTableParamInfoBase(
			InColumnName,
			ELeafMySqlValueType::SET,
			setInit,
			false,
			InNotNull,
			InUnique,
			false,
			FString(),
			InComment,
			InChecker);
	if (InSetDefaultValue)
	{
		temp.SetDefaultParamValue(InDefaultValue);
	}
	CreateInfoPtr->AddOrUpdateParam(temp);
	if (InIsPrimaryKey)
	{
		CreateInfoPtr->AddPrimaryKey(temp);
	}
}

void USQLTableConstructor::AddNewSpatialColumn(
	const FString& InColumnName,
	const ELeafSpatialColumnType InSpatialColumnType,
	const bool InNotNull,
	const bool InUnique,
	const bool InIsPrimaryKey,
	const bool InSetDefaultValue,
	const FString& InDefaultValue,
	const FString& InComment,
	const FString& InChecker)
{
	if (!CreateInfoPtr)
	{
		return;
	}
	UEnum* sPtr = StaticEnum<ELeafSpatialColumnType>();
	FString sType = sPtr->GetNameStringByIndex((uint8)InSpatialColumnType);
	Command::DataTable::FLeafMySqlDataTableParamInfoBase temp =
		Command::DataTable::FLeafMySqlDataTableParamInfoBase(
			InColumnName,
			ELeafMySqlValueType::GetLeafMySqlValueTypeFromString(sType),
			false,
			InNotNull,
			InUnique,
			false,
			FString(),
			InComment,
			InChecker);
	if (InSetDefaultValue)
	{
		temp.SetDefaultParamValue(InDefaultValue);
	}
	CreateInfoPtr->AddOrUpdateParam(temp);
	if (InIsPrimaryKey)
	{
		CreateInfoPtr->AddPrimaryKey(temp);
	}
}

void USQLTableConstructor::SetCharset(const ELeafCharacterSet InNewCharset)
{
	if (!CreateInfoPtr)
	{
		return;
	}
	UEnum* sPtr = StaticEnum<ELeafCharacterSet>();
	FString sName = sPtr->GetNameStringByIndex((uint8)InNewCharset);
	CreateInfoPtr->Charset = sName;
}
