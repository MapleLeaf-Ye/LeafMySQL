// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "CoreMinimal.h"
#include "SQLTableOperationBase.h"
#include "SQLTableConstructor.generated.h"

using namespace LeafMySql;

UCLASS(BlueprintType)
class LEAFMYSQL_API USQLTableConstructor : public USQLTableOperationBase
{
	GENERATED_UCLASS_BODY()
public:
	void InitializeTableConstructor(
		FLeafMySqlLinkInfo& InLinkInfoPtr,
		const FString& InDataTableName,
		const FString& InCollate,
		const bool InCheckIsExists = true,
		const FString& InEngine = TEXT("InnoDB"),
		const ELeafCharacterSet InCharset = ELeafCharacterSet::utf8mb4);

	void InitializeTableConstructor(
		FLeafMySqlLinkInfo& InLinkInfoPtr,
		const FString& InDataTableName,
		const bool InCheckIsExists = true,
		const FString& InEngine = TEXT("InnoDB"),
		const FString InCharset = TEXT("utf8mb4"),
		const FString& InCollate = TEXT("utf8mb4_general_ci"));

	virtual ~USQLTableConstructor();

	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableConstructor|ColumnAdditive")
	void AddNewIntegerColumn(
		const FString& InColumnName,
		const ELeafIntegerColumnType InIntegerColumnType,
		const bool InUnsigned = false,
		const bool InNotNull = false,
		const bool InUnique = false,
		const bool InIsPrimaryKey = false,
		const bool InAutoIncrement = false,
		const bool InSetDefaultValue = false,
		const int32 InDefaultValue = 0,
		const FString& InComment = TEXT(""),
		const FString& InChecker = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableConstructor|ColumnAdditive")
	void AddNewIntegerColumn_Int64(
		const FString& InColumnName,
		const ELeafIntegerColumnType InIntegerColumnType,
		const bool InUnsigned = false,
		const bool InNotNull = false,
		const bool InUnique = false,
		const bool InIsPrimaryKey = false,
		const bool InAutoIncrement = false,
		const bool InSetDefaultValue = false,
		const int64 InDefaultValue = 0,
		const FString& InComment = TEXT(""),
		const FString& InChecker = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableConstructor|ColumnAdditive")
	void AddNewFloatColumn(
		const FString& InColumnName,
		const ELeafFloatColumnType InFloatColumnType,
		const bool InUnsigned = false,
		const bool InNotNull = false,
		const bool InUnique = false,
		const bool InIsPrimaryKey = false,
		const int32 InM = 12,
		const int32 InD = 6,
		const bool InAutoIncrement = false,
		const bool InSetDefaultValue = false,
		const double InDefaultValue = 0,
		const FString& InComment = TEXT(""),
		const FString& InChecker = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableConstructor|ColumnAdditive")
	void AddNewDateColumn(
		const FString& InColumnName,
		const ELeafDateColumnType InDateColumnType,
		const bool InNotNull = false,
		const bool InUnique = false,
		const bool InIsPrimaryKey = false,
		const bool InSetDefaultValue = false,
		const FDateTime InDefaultValue = FDateTime(),
		const FString& InComment = TEXT(""),
		const FString& InChecker = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableConstructor|ColumnAdditive")
	void AddNewCharColumn(
		const FString& InColumnName,
		const ELeafCharColumnType InCharColumnType,
		const bool InNotNull = false,
		const bool InUnique = false,
		const bool InIsPrimaryKey = false,
		const int64 InCharSize = 100,
		const bool InSetDefaultValue = false,
		const FString& InDefaultValue = TEXT(""),
		const FString& InComment = TEXT(""),
		const FString& InChecker = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableConstructor|ColumnAdditive")
	void AddNewEnumColumn(
		const FString& InColumnName,
		const TArray<FString>& InEnumElements,
		const bool InNotNull = false,
		const bool InUnique = false,
		const bool InIsPrimaryKey = false,
		const bool InSetDefaultValue = false,
		const FString& InDefaultValue = TEXT(""),
		const FString& InComment = TEXT(""),
		const FString& InChecker = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableConstructor|ColumnAdditive")
	void AddNewSetColumn(
		const FString& InColumnName,
		const TArray<FString>& InSetElements,
		const bool InNotNull = false,
		const bool InUnique = false,
		const bool InIsPrimaryKey = false,
		const bool InSetDefaultValue = false,
		const FString& InDefaultValue = TEXT(""),
		const FString& InComment = TEXT(""),
		const FString& InChecker = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableConstructor|ColumnAdditive")
	void AddNewSpatialColumn(
		const FString& InColumnName,
		const ELeafSpatialColumnType InSpatialColumnType,
		const bool InNotNull = false,
		const bool InUnique = false,
		const bool InIsPrimaryKey = false,
		const bool InSetDefaultValue = false,
		const FString& InDefaultValue = TEXT(""),
		const FString& InComment = TEXT(""),
		const FString& InChecker = TEXT(""));

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|SQLTableConstructor|Getter")
	FORCEINLINE bool HasPrimaryKey() const { return CreateInfoPtr->HasPrimaryKey(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|SQLTableConstructor|Getter")
	FORCEINLINE FString GetDataTableName() const { return CreateInfoPtr->DataTableName; }
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableConstructor|Setter")
	FORCEINLINE void SetDataTableName(const FString& InNewDataTableName) { CreateInfoPtr->DataTableName = InNewDataTableName; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|SQLTableConstructor|Getter")
	FORCEINLINE bool GetCheckIsExists() const { return CreateInfoPtr->bCheckIsExists; }
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableConstructor|Setter")
	FORCEINLINE void SetCheckIsExists(const bool InNewCheckIsExists) { CreateInfoPtr->bCheckIsExists = InNewCheckIsExists; }

	UFUNCTION(BlueprintCallable,BlueprintPure, Category = "LeafMySQL|SQLTableConstructor|Getter")
	FORCEINLINE FString GetEngine() const { return CreateInfoPtr->Engine; }
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableConstructor|Setter")
	FORCEINLINE void SetEngine(const FString& InNewEngine) { CreateInfoPtr->Engine = InNewEngine; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|SQLTableConstructor|Getter")
	FORCEINLINE FString GetCollate() const { return CreateInfoPtr->Collate; }
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableConstructor|Setter")
	FORCEINLINE void SetCollate(const FString& InNewCollate) { CreateInfoPtr->Collate = InNewCollate; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|SQLTableConstructor|Getter")
	FORCEINLINE FString GetCharset() const { return CreateInfoPtr->Charset; }
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableConstructor|Setter")
	FORCEINLINE void SetCharsetWithString(const FString& InNewCharset) { CreateInfoPtr->Charset = InNewCharset; }
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableConstructor|Setter")
	void SetCharset(const ELeafCharacterSet InNewCharset);

	FORCEINLINE Command::DataTable::FLeafMySqlDataTableCreate* GetCreateInfo() const { return CreateInfoPtr; }
private:
	Command::DataTable::FLeafMySqlDataTableCreate* CreateInfoPtr;
};