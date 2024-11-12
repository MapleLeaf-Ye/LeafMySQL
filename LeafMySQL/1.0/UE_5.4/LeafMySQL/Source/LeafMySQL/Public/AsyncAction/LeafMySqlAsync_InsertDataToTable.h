// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "CoreMinimal.h"
#include "AsyncAction/LeafMySqlAsyncActionBase.h"
#include "LeafMySqlAsync_InsertDataToTable.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLeafMySqlAsync_InsertDataToTable, Warning, All);

UCLASS()
class LEAFMYSQL_API ULeafMySqlAsync_InsertDataToTable : public ULeafMySqlAsyncActionBase
{
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLeafMySqlAsync_InsertDataToTableDelegate, const FString, OutError);
	GENERATED_BODY()
public:
	ULeafMySqlAsync_InsertDataToTable(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;

	/**
	* 通过该函数可以异步的为数据库访问器当前操作（选择）的数据库的数据表插入多行数据
	* @Param InSQLAccessor 传入的要操作的数据库访问器
	* @Param InTableName 传入的要操作的数据表名称
	* @Param InInsertColumnNames 填写插入数据的列名通过逗号隔开例如: id,name,email
	* @Param InInsertDatas 填写插入数据的列值通过逗号隔开例如: 1,aaa,1234565@qq.com，每个数组元素都应遵守这个格式约定
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "LeafSQLInsertDataToTable_Async", Category = "LeafMySQL|AsyncAction", meta = (BlueprintInternalUseOnly = "true"))
	static ULeafMySqlAsync_InsertDataToTable* InsertDataToTable_Async(
		UPARAM(Ref)ULeafMySQLAccessorBase*& InSQLAccessor,
		const FString& InTableName,
		const FString& InInsertColumnNames,
		const TArray<FString>& InInsertDatas);

private:
	void CallUpdateDelegate_GameThread(const FString& InError);

public:
	/**
	* 如果遇到错误由此委托传递出
	*/
	UPROPERTY(BlueprintAssignable)
	FLeafMySqlAsync_InsertDataToTableDelegate OnErrorOut;

	UPROPERTY()
	FString TableName;
	UPROPERTY()
	FString InsertColumnNames;
	UPROPERTY()
	TArray<FString> InsertDatas;

};