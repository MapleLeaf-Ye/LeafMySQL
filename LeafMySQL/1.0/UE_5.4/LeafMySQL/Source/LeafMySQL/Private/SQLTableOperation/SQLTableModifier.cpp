// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#include "SQLTableOperation/SQLTableModifier.h"

#include "Kismet/KismetStringLibrary.h"


USQLTableModifier::USQLTableModifier(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	InfoPtr = TArray<Command::DataTable::FLeafMySqlDataTableModify*>();
	ModifiedColumnParam = MakeShareable(new Command::DataTable::FLeafMySqlDataTableParamInfoBase(Command::DataTable::FLeafMySqlDataTableParamInfoBase::NoValidType));
	LinkInfoPtr = nullptr;
}

void USQLTableModifier::InitializeTableModifier(
	FLeafMySqlLinkInfo& InLinkInfoPtr, 
	const FString& InDataTableName)
{
	LinkInfoPtr = &InLinkInfoPtr;
	DataTableName = InDataTableName;
}

USQLTableModifier::~USQLTableModifier()
{
	LinkInfoPtr = nullptr;

	if (InfoPtr.Num() > 0)
	{
		for (auto& tmp : InfoPtr)
		{
			if (tmp)
			{
				delete tmp;
				tmp = nullptr;
			}
		}
		InfoPtr.Empty();
	}

	if (ModifiedColumnParam)
	{
		ModifiedColumnParam.Reset();
		ModifiedColumnParam = nullptr;
	}
}

Command::DataTable::FLeafMySqlDataTableParamInfoBase USQLTableModifier::NativeModifyIntegerColumn(
	const FString& InColumnName,
	const ELeafIntegerColumnType InIntegerColumnType,
	const bool InUnsigned, 
	const bool InNotNull,
	const bool InUnique,
	const bool InAutoIncrement,
	const bool InSetDefaultValue,
	const int32 InDefaultValue,
	const FString& InComment,
	const FString& InChecker)
{
	UEnum* sPtr = StaticEnum<ELeafIntegerColumnType>();
	FString sType = sPtr->GetNameStringByIndex((uint8)InIntegerColumnType);
	Command::DataTable::FLeafMySqlDataTableParamInfoBase tmp = Command::DataTable::FLeafMySqlDataTableParamInfoBase(
		InColumnName,
		LeafMySql::ELeafMySqlValueType::GetLeafMySqlValueTypeFromString(sType),
		InUnsigned, InNotNull, InUnique, InAutoIncrement, FString(), InComment, InChecker);

	if (InSetDefaultValue)
	{
		tmp.SetDefaultParamValue(InDefaultValue);
	}

	return tmp;
}

Command::DataTable::FLeafMySqlDataTableParamInfoBase USQLTableModifier::NativeModifyIntegerColumn_Int64(
	const FString& InColumnName,
	const ELeafIntegerColumnType InIntegerColumnType,
	const bool InUnsigned,
	const bool InNotNull,
	const bool InUnique,
	const bool InAutoIncrement,
	const bool InSetDefaultValue,
	const int64 InDefaultValue,
	const FString& InComment,
	const FString& InChecker)
{
	UEnum* sPtr = StaticEnum<ELeafIntegerColumnType>();
	FString sType = sPtr->GetNameStringByIndex((uint8)InIntegerColumnType);
	Command::DataTable::FLeafMySqlDataTableParamInfoBase tmp = Command::DataTable::FLeafMySqlDataTableParamInfoBase(
		InColumnName,
		LeafMySql::ELeafMySqlValueType::GetLeafMySqlValueTypeFromString(sType),
		InUnsigned, InNotNull, InUnique, InAutoIncrement, FString(), InComment, InChecker);

	if (InSetDefaultValue)
	{
		tmp.SetDefaultParamValue(InDefaultValue);
	}

	return tmp;
}

Command::DataTable::FLeafMySqlDataTableParamInfoBase USQLTableModifier::NativeModifyFloatColumn(
	const FString& InColumnName, 
	const ELeafFloatColumnType InFloatColumnType,
	const bool InUnsigned,
	const bool InNotNull, 
	const bool InUnique, 
	const int32 InM,
	const int32 InD,
	const bool InAutoIncrement,
	const bool InSetDefaultValue,
	const double InDefaultValue,
	const FString& InComment,
	const FString& InChecker)
{
	UEnum* sPtr = StaticEnum<ELeafFloatColumnType>();
	FString sType = sPtr->GetNameStringByIndex((uint8)InFloatColumnType);
	Command::DataTable::FLeafMySqlDataTableParamInfoBase tmp = Command::DataTable::FLeafMySqlDataTableParamInfoBase(
		InColumnName,
		InM,
		InD,
		LeafMySql::ELeafMySqlValueType::GetLeafMySqlValueTypeFromString(sType),
		InUnsigned, InNotNull, InUnique, InAutoIncrement, FString(), InComment, InChecker);

	if (InSetDefaultValue)
	{
		tmp.SetDefaultParamValue(InDefaultValue);
	}

	return tmp;
}

Command::DataTable::FLeafMySqlDataTableParamInfoBase USQLTableModifier::NativeModifyDateColumn(
	const FString& InColumnName, 
	const ELeafDateColumnType InDateColumnType,
	const bool InNotNull,
	const bool InUnique,
	const bool InSetDefaultValue,
	const FDateTime InDefaultValue,
	const FString& InComment,
	const FString& InChecker)
{
	UEnum* sPtr = StaticEnum<ELeafDateColumnType>();
	FString sType = sPtr->GetNameStringByIndex((uint8)InDateColumnType);
	Command::DataTable::FLeafMySqlDataTableParamInfoBase tmp = Command::DataTable::FLeafMySqlDataTableParamInfoBase(
		InColumnName,
		LeafMySql::ELeafMySqlValueType::GetLeafMySqlValueTypeFromString(sType),
		false, InNotNull, InUnique, false, FString(), InComment, InChecker);

	if (InSetDefaultValue)
	{
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

		tmp.SetDefaultParamValue(sDefaultValue);
	}

	return tmp;
}

Command::DataTable::FLeafMySqlDataTableParamInfoBase USQLTableModifier::NativeModifyCharColumn(
	const FString& InColumnName,
	const ELeafCharColumnType InCharColumnType,
	const bool InNotNull,
	const bool InUnique, 
	const int64 InCharSize,
	const bool InSetDefaultValue,
	const FString& InDefaultValue,
	const FString& InComment, 
	const FString& InChecker)
{
	UEnum* sPtr = StaticEnum<ELeafCharColumnType>();
	FString sType = sPtr->GetNameStringByIndex((uint8)InCharColumnType);
	Command::DataTable::FLeafMySqlDataTableParamInfoBase tmp = 
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
		tmp.SetDefaultParamValue(InDefaultValue);
	}

	return tmp;
}

Command::DataTable::FLeafMySqlDataTableParamInfoBase USQLTableModifier::NativeModifyEnumColumn(
	const FString& InColumnName,
	const TArray<FString>& InEnumElements,
	const bool InNotNull,
	const bool InUnique,
	const bool InSetDefaultValue,
	const FString& InDefaultValue,
	const FString& InComment,
	const FString& InChecker)
{
	//FString enumInit = UKismetStringLibrary::JoinStringArray(InEnumElements, TEXT(","));
	FString enumInit;
	for (const auto tmp : InEnumElements)
	{
		enumInit += FLeafMySqlValueHandler(tmp).Get();
		enumInit += TEXT(",");
	}
	enumInit = enumInit.Left(enumInit.Len() - 1);
	Command::DataTable::FLeafMySqlDataTableParamInfoBase tmp = 
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
		tmp.SetDefaultParamValue(InDefaultValue);
	}

	return tmp;
}

Command::DataTable::FLeafMySqlDataTableParamInfoBase USQLTableModifier::NativeModifySetColumn(
	const FString& InColumnName, 
	const TArray<FString>& InSetElements,
	const bool InNotNull, 
	const bool InUnique,
	const bool InSetDefaultValue,
	const FString& InDefaultValue,
	const FString& InComment,
	const FString& InChecker)
{
	//FString setInit = UKismetStringLibrary::JoinStringArray(InSetElements, TEXT(","));
	FString setInit;
	for (const auto tmp : InSetElements)
	{
		setInit += FLeafMySqlValueHandler(tmp).Get();
		setInit += TEXT(",");
	}
	setInit = setInit.Left(setInit.Len() - 1);
	Command::DataTable::FLeafMySqlDataTableParamInfoBase tmp = 
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
		tmp.SetDefaultParamValue(InDefaultValue);
	}

	return tmp;
}

Command::DataTable::FLeafMySqlDataTableParamInfoBase USQLTableModifier::NativeModifySpatialColumn(
	const FString& InColumnName,
	const ELeafSpatialColumnType InSpatialColumnType,
	const bool InNotNull, 
	const bool InUnique,
	const bool InSetDefaultValue,
	const FString& InDefaultValue,
	const FString& InComment, 
	const FString& InChecker)
{
	UEnum* sPtr = StaticEnum<ELeafSpatialColumnType>();
	FString sType = sPtr->GetNameStringByIndex((uint8)InSpatialColumnType);
	Command::DataTable::FLeafMySqlDataTableParamInfoBase tmp = 
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
		tmp.SetDefaultParamValue(InDefaultValue);
	}

	return tmp;
}
