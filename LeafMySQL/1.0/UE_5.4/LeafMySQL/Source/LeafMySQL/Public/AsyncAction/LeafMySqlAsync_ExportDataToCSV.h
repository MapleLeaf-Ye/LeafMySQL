// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "CoreMinimal.h"
#include "AsyncAction/LeafMySqlAsyncActionBase.h"
#include "LeafMySqlAsync_ExportDataToCSV.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLeafMySqlAsync_ExportDataToCSV, Warning, All);

UENUM(BlueprintType)
enum class ELeafSQLExportDataToCSVState :uint8
{
	None,
	PreHandle,
	RequestDataTableStruture,
	RequestData,
	Handling,
	SavingToFile,
	Completed,
	Error,
	Cancel
};

UCLASS()
class LEAFMYSQL_API ULeafMySqlAsync_ExportDataToCSV : public ULeafMySqlAsyncActionBase
{
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FLeafMySqlAsync_ExportDataToCSVDelegate, const ELeafSQLExportDataToCSVState, CurrentState, const int64, CurrentHandleCount, const int64, TargetHandleCount, const FString, OutError);
	GENERATED_BODY()
public:
	ULeafMySqlAsync_ExportDataToCSV(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;

	/**
	* 通过该函数可以异步的将数据库访问器当前操作（选择）的数据库的数据表的数据导出为CSV文件
	* @Param InSQLAccessor 传入的要操作的数据库访问器
	* @Param InFileExportPath 传入的导出CSV文件的路径
	* @Param InTableName 传入的要操作的数据表名称
	* @Param InLimitRowNum 限制导出的行数，如果为0则全部导出
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "LeafSQLExportDataToCSV_Async", Category = "LeafMySQL|AsyncAction", meta = (BlueprintInternalUseOnly = "true"))
	static ULeafMySqlAsync_ExportDataToCSV* ExportDataToCSV_Async(
		UPARAM(Ref)ULeafMySQLAccessorBase*& InSQLAccessor,
		const FString& InFileExportPath,
		const FString& InTableName, 
		const int64 InLimitRowNum = 0);

	void UpdateExportDataToCSVState(const ELeafSQLExportDataToCSVState NewState, const FString& OutError = TEXT(""));

	/**
	* 该委托将传递出导出进度，导出状态，导出遇到的错误
	*/
	UPROPERTY(BlueprintAssignable)
	FLeafMySqlAsync_ExportDataToCSVDelegate OnUpdateStateAndCount;

public:
	UPROPERTY()
	FString FileExportPath;
	UPROPERTY()
	FString TableName;
	UPROPERTY()
	int64 LimitRowNum;

private:
	UPROPERTY()
	int64 TargetHandleCount;
	UPROPERTY()
	int64 CurrentHandleCount;
	UPROPERTY()
	ELeafSQLExportDataToCSVState ExportDataToCSVState;
};