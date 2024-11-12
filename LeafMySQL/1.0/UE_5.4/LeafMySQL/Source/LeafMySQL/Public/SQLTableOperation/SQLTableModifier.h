// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "CoreMinimal.h"
#include "SQLTableOperationBase.h"
#include "SQLTableModifier.generated.h"

using namespace LeafMySql;

UCLASS(Abstract)
class LEAFMYSQL_API USQLTableModifier : public USQLTableOperationBase
{
	GENERATED_UCLASS_BODY()
public:
	virtual void InitializeTableModifier(
		FLeafMySqlLinkInfo& InLinkInfoPtr,
		const FString& InDataTableName);
protected:
	virtual ~USQLTableModifier();

		Command::DataTable::FLeafMySqlDataTableParamInfoBase NativeModifyIntegerColumn(
			const FString& InColumnName,
			const ELeafIntegerColumnType InIntegerColumnType,
			const bool InUnsigned = false,
			const bool InNotNull = false,
			const bool InUnique = false,
			const bool InAutoIncrement = false,
			const bool InSetDefaultValue = false,
			const int32 InDefaultValue = 0,
			const FString& InComment = TEXT(""),
			const FString& InChecker = TEXT(""));

		Command::DataTable::FLeafMySqlDataTableParamInfoBase NativeModifyIntegerColumn_Int64(
			const FString& InColumnName,
			const ELeafIntegerColumnType InIntegerColumnType,
			const bool InUnsigned = false,
			const bool InNotNull = false,
			const bool InUnique = false,
			const bool InAutoIncrement = false,
			const bool InSetDefaultValue = false,
			const int64 InDefaultValue = 0,
			const FString& InComment = TEXT(""),
			const FString& InChecker = TEXT(""));

		Command::DataTable::FLeafMySqlDataTableParamInfoBase NativeModifyFloatColumn(
			const FString& InColumnName,
			const ELeafFloatColumnType InFloatColumnType,
			const bool InUnsigned = false,
			const bool InNotNull = false,
			const bool InUnique = false,
			const int32 InM = 12,
			const int32 InD = 6,
			const bool InAutoIncrement = false,
			const bool InSetDefaultValue = false,
			const double InDefaultValue = 0,
			const FString& InComment = TEXT(""),
			const FString& InChecker = TEXT(""));

		Command::DataTable::FLeafMySqlDataTableParamInfoBase NativeModifyDateColumn(
			const FString& InColumnName,
			const ELeafDateColumnType InDateColumnType,
			const bool InNotNull = false,
			const bool InUnique = false,
			const bool InSetDefaultValue = false,
			const FDateTime InDefaultValue = FDateTime(),
			const FString& InComment = TEXT(""),
			const FString& InChecker = TEXT(""));

		Command::DataTable::FLeafMySqlDataTableParamInfoBase NativeModifyCharColumn(
			const FString& InColumnName,
			const ELeafCharColumnType InCharColumnType,
			const bool InNotNull = false,
			const bool InUnique = false,
			const int64 InCharSize = 100,
			const bool InSetDefaultValue = false,
			const FString& InDefaultValue = TEXT(""),
			const FString& InComment = TEXT(""),
			const FString& InChecker = TEXT(""));

		Command::DataTable::FLeafMySqlDataTableParamInfoBase NativeModifyEnumColumn(
			const FString& InColumnName,
			const TArray<FString>& InEnumElements,
			const bool InNotNull = false,
			const bool InUnique = false,
			const bool InSetDefaultValue = false,
			const FString& InDefaultValue = TEXT(""),
			const FString& InComment = TEXT(""),
			const FString& InChecker = TEXT(""));

		Command::DataTable::FLeafMySqlDataTableParamInfoBase NativeModifySetColumn(
			const FString& InColumnName,
			const TArray<FString>& InSetElements,
			const bool InNotNull = false,
			const bool InUnique = false,
			const bool InSetDefaultValue = false,
			const FString& InDefaultValue = TEXT(""),
			const FString& InComment = TEXT(""),
			const FString& InChecker = TEXT(""));

		Command::DataTable::FLeafMySqlDataTableParamInfoBase NativeModifySpatialColumn(
			const FString& InColumnName,
			const ELeafSpatialColumnType InSpatialColumnType,
			const bool InNotNull = false,
			const bool InUnique = false,
			const bool InSetDefaultValue = false,
			const FString& InDefaultValue = TEXT(""),
			const FString& InComment = TEXT(""),
			const FString& InChecker = TEXT(""));
public:
	FORCEINLINE TArray < Command::DataTable::FLeafMySqlDataTableModify*> GetModifyInfo() const { return InfoPtr; }

protected:
	FString DataTableName;
	TArray<Command::DataTable::FLeafMySqlDataTableModify*> InfoPtr;
	TSharedPtr<Command::DataTable::FLeafMySqlDataTableParamInfoBase> ModifiedColumnParam;
	FLeafMySqlLinkInfo* LinkInfoPtr;
};