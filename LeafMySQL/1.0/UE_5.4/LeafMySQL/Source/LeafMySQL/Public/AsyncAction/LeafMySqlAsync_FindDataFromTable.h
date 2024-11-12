// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "CoreMinimal.h"
#include "AsyncAction/LeafMySqlAsyncActionBase.h"
#include "LeafMySqlAsync_FindDataFromTable.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLeafMySqlAsync_FindDataFromTable, Warning, All);

UCLASS()
class LEAFMYSQL_API ULeafMySqlAsync_FindDataFromTable : public ULeafMySqlAsyncActionBase
{
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLeafMySqlAsync_FindDataFromTableDelegate, const TArray<FLeafSQLStringResultPreRow>&, OutSelectedDatas, const FString, OutError);
	GENERATED_BODY()
public:
	ULeafMySqlAsync_FindDataFromTable(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;

	/**
	* 通过该函数可以异步的查找数据库访问器当前操作（选择）的数据库的数据表的数据，并且支持一系列的限制条件。注意查找数据通过OnResultOrErrorGet引脚返回
	* @Param InSQLAccessor 传入的要操作的数据库访问器
	* @Param InTableName 传入的要操作的数据表名称
	* @Param InSelectColumns 要查询的列
	* @Param InWhereCondition 如果为空或者None则不起效，查询的条件语句，例如id='1'，使用方法就像SQL语句那样
	* @Param InOrderColumnName 如果为空或者None则不起效，填写了则会按照这列进行顺序排序
	* @Param bInIsASC InOrderColumnName起效的情况下控制是升序还是降序
	* @Param InLimitRowNum 查询数据限制的行数，针对数据量大的数据表可以使用，如果为0则不限制
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "LeafSQLFindDataFromTable_Async", Category = "LeafMySQL|AsyncAction", meta = (BlueprintInternalUseOnly = "true"))
	static ULeafMySqlAsync_FindDataFromTable* FindDataFromTable_Async(
		UPARAM(Ref)ULeafMySQLAccessorBase*& InSQLAccessor,
		const FString& InTableName,
		const TArray<FString>& InSelectColumns,
		const FLeafSQLWhereString InWhereCondition,
		const FString& InOrderColumnName,
		const bool bInIsASC,
		const int64 InLimitRowNum);

private:
	void CallUpdateDelegate_GameThread(const TArray<FLeafSQLStringResultPreRow>& InSelectedDatas, const FString& InError);

public:
	/**
	* 该委托将传递出查找结果，查找遇到的错误
	*/
	UPROPERTY(BlueprintAssignable)
	FLeafMySqlAsync_FindDataFromTableDelegate OnResultOrErrorGet;

	UPROPERTY()
	FString TableName;
	UPROPERTY()
	TArray<FString> SelectColumns;
	/**
	* 查到到的数据
	*/
	UPROPERTY(BlueprintReadOnly, Category = "LeafMySQL|AsyncAction")
	TArray<FLeafSQLStringResultPreRow> FoundDatas;
	UPROPERTY()
	FString WhereCondition;
	UPROPERTY()
	FString OrderColumnName;
	UPROPERTY()
	bool bIsASC;
	UPROPERTY()
	int64 LimitRowNum;
};