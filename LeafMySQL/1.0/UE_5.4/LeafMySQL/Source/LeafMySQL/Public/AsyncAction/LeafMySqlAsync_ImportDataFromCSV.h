// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "CoreMinimal.h"
#include "AsyncAction/LeafMySqlAsyncActionBase.h"
#include "LeafMySqlAsync_ImportDataFromCSV.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLeafMySqlAsync_ImportDataToCSV, Warning, All);

UENUM(BlueprintType)
enum class ELeafSQLImportDataFromCSVState :uint8
{
	None,
	PreHandle,
	ReadingCSVFile,
	HandlingHead,
	HandlingData,
	ClearingSourceData,
	Importing,
	Checking,
	Completed,
	Error,
	Cancel,
	UnChangeState
};

UCLASS()
class LEAFMYSQL_API ULeafMySqlAsync_ImportDataFromCSV : public ULeafMySqlAsyncActionBase
{
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FLeafMySqlAsync_ImportDataFromCSVDelegate, const ELeafSQLImportDataFromCSVState, CurrentState, const int64, CurrentHandleCount, const int64, TargetHandleCount, const FString, OutError);
	GENERATED_BODY()
public:
	ULeafMySqlAsync_ImportDataFromCSV(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;

	/**
	* 通过该函数可以异步的为数据库访问器当前操作（选择）的数据库的数据表导入CSV表格数据，通过批量打包的方式导入
	* @Param InSQLAccessor 传入的要操作的数据库访问器
	* @Param InFilePath CSV文件的路径
	* @Param InTableName 导入表格的名字
	* @Param InOverrideSourceData 如果为true则会情况表格本身数据再导入CSV的数据，为false则在原有数据基础上附加CSV数据
	* @Param InTrunkSize 一次性为数据库中插入的数据量
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "LeafSQLImportDataFromCSV_Async", Category = "LeafMySQL|AsyncAction", meta = (BlueprintInternalUseOnly = "true"))
	static ULeafMySqlAsync_ImportDataFromCSV* ImportDataFromCSV_Async(
		UPARAM(Ref)ULeafMySQLAccessorBase*& InSQLAccessor,
		const FString& InFilePath,
		const FString& InTableName,
		const bool InOverrideSourceData = false,
		const int32 InTrunkSize = 1000);

	void UpdateImportDataFromCSVState(const ELeafSQLImportDataFromCSVState NewState, const FString& OutError = TEXT(""));

	/**
	* 该委托将传递出导入进度，导入状态，导入遇到的错误
	*/
	UPROPERTY(BlueprintAssignable)
	FLeafMySqlAsync_ImportDataFromCSVDelegate OnUpdateStateAndCount;

public:
	UPROPERTY()
	FString FilePath;
	UPROPERTY()
	FString TableName;
	UPROPERTY()
	bool bOverrideSourceData;
	UPROPERTY()
	int32 TrunkSize;

private:
	UPROPERTY()
	int64 TargetHandleCount;
	UPROPERTY()
	int64 CurrentHandleCount;
	UPROPERTY()
	ELeafSQLImportDataFromCSVState ImportDataToCSVState;
};