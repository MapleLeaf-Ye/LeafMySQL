// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#include "AsyncAction/LeafMySqlAsync_DeleteDataFromTable.h"

#include "LeafMySQL.h"

DEFINE_LOG_CATEGORY(LogLeafMySqlAsync_DeleteDataFromTable);

ULeafMySqlAsync_DeleteDataFromTable::ULeafMySqlAsync_DeleteDataFromTable(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	TableName = TEXT("");
	WhereCondition = TEXT("");
}

void ULeafMySqlAsync_DeleteDataFromTable::Activate()
{
	UE::Tasks::FTask task = UE::Tasks::Launch(UE_SOURCE_LOCATION, [this]
		{
			if (!SQLAccessor)
			{
				UE_LOG(LogLeafMySqlAsync_DeleteDataFromTable, Error, TEXT("DeleteDataFromTable_Async - SQLAccessor is not valid!"));
				CallUpdateDelegate_GameThread(TEXT("DeleteDataFromTable_Async - SQLAccessor is not valid!"));
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				return;
			}
			if (!SQLAccessor->GetSQLConnectionState())
			{
				UE_LOG(LogLeafMySqlAsync_DeleteDataFromTable, Error, TEXT("DeleteDataFromTable_Async - SQLAccessor is not Connectting any Sql database"));
				CallUpdateDelegate_GameThread(TEXT("DeleteDataFromTable_Async - SQLAccessor is not Connectting any Sql database"));
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				return;
			}

			if (!GetLinkInfo())
			{
				UE_LOG(LogLeafMySqlAsync_DeleteDataFromTable, Error, TEXT("DeleteDataFromTable_Async - Cant get LinkInfo"));
				CallUpdateDelegate_GameThread(TEXT("DeleteDataFromTable_Async - Cant get LinkInfo"));
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				return;
			}

			const FString resS = DATA::LeafDeleteDataFromTable(
				*GetLinkInfo(),
				TableName,
				WhereCondition);
			if (resS.IsEmpty() || resS.Equals(TEXT("Successed"), ESearchCase::IgnoreCase))
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Completed);
			}
			else
			{
				UE_LOG(LogLeafMySqlAsync_DeleteDataFromTable, Error, TEXT("DeleteDataFromTable_Async - %s"), *resS);
				CallUpdateDelegate_GameThread(FString::Printf(TEXT("DeleteDataFromTable_Async - %s"), *resS));
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
			}
		}
	);
}

ULeafMySqlAsync_DeleteDataFromTable* ULeafMySqlAsync_DeleteDataFromTable::DeleteDataFromTable_Async(UPARAM(Ref)ULeafMySQLAccessorBase*& InSQLAccessor, const FString& InTableName, const FLeafSQLWhereString& InWhereCondition)
{
	ULeafMySqlAsync_DeleteDataFromTable* MyObj = NewObject<ULeafMySqlAsync_DeleteDataFromTable>();
	MyObj->SQLAccessor = InSQLAccessor;
	MyObj->TableName = InTableName;
	MyObj->WhereCondition = InWhereCondition.WhereString;

	return MyObj;
}

void ULeafMySqlAsync_DeleteDataFromTable::CallUpdateDelegate_GameThread(const FString& InError)
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
