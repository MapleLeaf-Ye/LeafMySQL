// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "CoreMinimal.h"
#include "SQLTableOperation/SQLTableModifier.h"
#include "SQLTableModifier_AddColumn.generated.h"

using namespace LeafMySql;

UCLASS(BlueprintType)
class LEAFMYSQL_API USQLTableModifier_AddColumn : public USQLTableModifier
{
	GENERATED_UCLASS_BODY()
protected:
	virtual void InitializeTableModifier(
		FLeafMySqlLinkInfo& InLinkInfoPtr,
		const FString& InDataTableName) override;

public:
	/**
	* 为列修改器添加列（整型类型）添加信息
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableModifier|ColumnModify")
	void ModifyIntegerColumn(
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

	/**
	* 为列修改器添加列（整型64类型）添加信息
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableModifier|ColumnModify")
	void ModifyIntegerColumn_Int64(
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

	/**
	* 为列修改器添加列（浮点类型）添加信息
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableModifier|ColumnModify")
	void ModifyFloatColumn(
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

	/**
	* 为列修改器添加列（时间类型）添加信息
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableModifier|ColumnModify")
	void ModifyDateColumn(
		const FString& InColumnName,
		const ELeafDateColumnType InDateColumnType,
		const bool InNotNull = false,
		const bool InUnique = false,
		const bool InSetDefaultValue = false,
		const FDateTime InDefaultValue = FDateTime(),
		const FString& InComment = TEXT(""),
		const FString& InChecker = TEXT(""));

	/**
	* 为列修改器添加列（字符类型）添加信息
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableModifier|ColumnModify")
	void ModifyCharColumn(
		const FString& InColumnName,
		const ELeafCharColumnType InCharColumnType,
		const bool InNotNull = false,
		const bool InUnique = false,
		const int64 InCharSize = 100,
		const bool InSetDefaultValue = false,
		const FString& InDefaultValue = TEXT(""),
		const FString& InComment = TEXT(""),
		const FString& InChecker = TEXT(""));

	/**
	* 为列修改器添加列（枚举类型）添加信息
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableModifier|ColumnModify")
	void ModifyEnumColumn(
		const FString& InColumnName,
		const TArray<FString>& InEnumElements,
		const bool InNotNull = false,
		const bool InUnique = false,
		const bool InSetDefaultValue = false,
		const FString& InDefaultValue = TEXT(""),
		const FString& InComment = TEXT(""),
		const FString& InChecker = TEXT(""));

	/**
	* 为列修改器添加列（集合类型）添加信息
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableModifier|ColumnModify")
	void ModifySetColumn(
		const FString& InColumnName,
		const TArray<FString>& InSetElements,
		const bool InNotNull = false,
		const bool InUnique = false,
		const bool InSetDefaultValue = false,
		const FString& InDefaultValue = TEXT(""),
		const FString& InComment = TEXT(""),
		const FString& InChecker = TEXT(""));

	/**
	* 为列修改器添加列（地理信息类型）添加信息
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableModifier|ColumnModify")
	void ModifySpatialColumn(
		const FString& InColumnName,
		const ELeafSpatialColumnType InSpatialColumnType,
		const bool InNotNull = false,
		const bool InUnique = false,
		const bool InSetDefaultValue = false,
		const FString& InDefaultValue = TEXT(""),
		const FString& InComment = TEXT(""),
		const FString& InChecker = TEXT(""));

};