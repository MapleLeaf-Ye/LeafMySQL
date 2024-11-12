// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "CoreMinimal.h"
#include "AsyncAction/LeafMySqlAsyncActionBase.h"
#include "LeafMySqlAsync_DeleteDataFromTable.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLeafMySqlAsync_DeleteDataFromTable, Warning, All);

UCLASS()
class LEAFMYSQL_API ULeafMySqlAsync_DeleteDataFromTable : public ULeafMySqlAsyncActionBase
{
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLeafMySqlAsync_DeleteDataFromTableDelegate, const FString, OutError);
	GENERATED_BODY()
public:
	ULeafMySqlAsync_DeleteDataFromTable(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;

	/**
	* 通过该函数可以异步的为数据库访问器当前操作（选择）的数据库的数据表删除数据
	* @Param InSQLAccessor 传入的要操作的数据库访问器
	* @Param InTableName 传入的要操作的数据表名称
	* @Param InWhereCondition 不可以为空，查询的条件语句，例如id='1'，使用方法就像SQL语句那样
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "LeafSQLDeleteDataFromTable_Async", Category = "LeafMySQL|AsyncAction", meta = (BlueprintInternalUseOnly = "true"))
	static ULeafMySqlAsync_DeleteDataFromTable* DeleteDataFromTable_Async(
		UPARAM(Ref)ULeafMySQLAccessorBase*& InSQLAccessor,
		const FString& InTableName,
		const FLeafSQLWhereString& InWhereCondition);

private:
	void CallUpdateDelegate_GameThread(const FString& InError);

public:
	/**
	* 如果遇到错误由此委托传递出
	*/
	UPROPERTY(BlueprintAssignable)
	FLeafMySqlAsync_DeleteDataFromTableDelegate OnErrorOut;

	UPROPERTY()
	FString TableName;
	UPROPERTY()
	FString WhereCondition;
};