// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#include "AsyncAction/LeafMySqlAsync_InsertDataToTable.h"

#include "LeafMySQL.h"


DEFINE_LOG_CATEGORY(LogLeafMySqlAsync_InsertDataToTable);

ULeafMySqlAsync_InsertDataToTable::ULeafMySqlAsync_InsertDataToTable(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	TableName = TEXT("");
	InsertColumnNames = TEXT("");
	InsertDatas = TArray<FString>();
}

void ULeafMySqlAsync_InsertDataToTable::Activate()
{
	UE::Tasks::FTask task = UE::Tasks::Launch(UE_SOURCE_LOCATION, [this]
		{
			if (!SQLAccessor)
			{
				UE_LOG(LogLeafMySqlAsync_InsertDataToTable, Error, TEXT("InsertDataToTable_Async - SQLAccessor is not valid!"));
				CallUpdateDelegate_GameThread(TEXT("InsertDataToTable_Async - SQLAccessor is not valid!"));
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				return;
			}
			if (!SQLAccessor->GetSQLConnectionState())
			{
				UE_LOG(LogLeafMySqlAsync_InsertDataToTable, Error, TEXT("InsertDataToTable_Async - SQLAccessor is not Connectting any Sql database"));
				CallUpdateDelegate_GameThread(TEXT("InsertDataToTable_Async - SQLAccessor is not Connectting any Sql database"));
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				return;
			}

			if (!GetLinkInfo())
			{
				UE_LOG(LogLeafMySqlAsync_InsertDataToTable, Error, TEXT("InsertDataToTable_Async - Cant get LinkInfo"));
				CallUpdateDelegate_GameThread(TEXT("InsertDataToTable_Async - Cant get LinkInfo"));
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				return;
			}
			
			const FString resS = DATA::LeafInsertDataToTable_Trunk(
				*GetLinkInfo(),
				TableName,
				InsertColumnNames,
				InsertDatas);
			if (resS.IsEmpty() || resS.Equals(TEXT("Successed"), ESearchCase::IgnoreCase))
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Completed);
			}
			else
			{
				UE_LOG(LogLeafMySqlAsync_InsertDataToTable, Error, TEXT("InsertDataToTable_Async - %s"), *resS);
				CallUpdateDelegate_GameThread(FString::Printf(TEXT("InsertDataToTable_Async - %s"), *resS));
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
			}
		}
	);
}

ULeafMySqlAsync_InsertDataToTable* ULeafMySqlAsync_InsertDataToTable::InsertDataToTable_Async(UPARAM(Ref)ULeafMySQLAccessorBase*& InSQLAccessor, const FString& InTableName, const FString& InInsertColumnNames, const TArray<FString>& InInsertDatas)
{
	ULeafMySqlAsync_InsertDataToTable* MyObj = NewObject<ULeafMySqlAsync_InsertDataToTable>();
	MyObj->SQLAccessor = InSQLAccessor;
	MyObj->TableName = InTableName;
	MyObj->InsertColumnNames = InInsertColumnNames;
	MyObj->InsertDatas = InInsertDatas;

	return MyObj;
}

void ULeafMySqlAsync_InsertDataToTable::CallUpdateDelegate_GameThread(const FString& InError)
{
	if (!IsInGameThread())
	{
		AsyncTask(ENamedThreads::GameThread, [InError, this]()
			{
				CallUpdateDelegate_GameThread(InError);
			});
	}
	else
	{
		OnErrorOut.Broadcast(InError);
	}
}
