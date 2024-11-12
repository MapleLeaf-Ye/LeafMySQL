// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#include "AsyncAction/LeafMySqlAsync_FindDataFromTable.h"

#include "LeafMySQL.h"

DEFINE_LOG_CATEGORY(LogLeafMySqlAsync_FindDataFromTable);

ULeafMySqlAsync_FindDataFromTable::ULeafMySqlAsync_FindDataFromTable(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
		TableName = TEXT("");
		SelectColumns = TArray<FString>();
		FoundDatas = TArray<FLeafSQLStringResultPreRow>();
		WhereCondition = TEXT("");
		OrderColumnName = TEXT("");
		bIsASC = true;
		LimitRowNum = 0;
}

void ULeafMySqlAsync_FindDataFromTable::Activate()
{
	UE::Tasks::FTask task = UE::Tasks::Launch(UE_SOURCE_LOCATION, [this]
		{
			if (!SQLAccessor)
			{
				UE_LOG(LogLeafMySqlAsync_FindDataFromTable, Error, TEXT("FindDataFromTable_Async - SQLAccessor is not valid!"));
				CallUpdateDelegate_GameThread(TArray<FLeafSQLStringResultPreRow>(), TEXT("FindDataFromTable_Async - SQLAccessor is not valid!"));
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				return;
			}
			if (!SQLAccessor->GetSQLConnectionState())
			{
				UE_LOG(LogLeafMySqlAsync_FindDataFromTable, Error, TEXT("FindDataFromTable_Async - SQLAccessor is not Connectting any Sql database"));
				CallUpdateDelegate_GameThread(TArray<FLeafSQLStringResultPreRow>(), TEXT("FindDataFromTable_Async - SQLAccessor is not Connectting any Sql database"));
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				return;
			}

			if (!GetLinkInfo())
			{
				UE_LOG(LogLeafMySqlAsync_FindDataFromTable, Error, TEXT("FindDataFromTable_Async - Cant get LinkInfo"));
				CallUpdateDelegate_GameThread(TArray<FLeafSQLStringResultPreRow>(), TEXT("FindDataFromTable_Async - Cant get LinkInfo"));
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				return;
			}

			TArray<TMap<FString, FString>> outSelectedDatas;
			const FString resS = DATA::LeafFindDataFromTable(
				*GetLinkInfo(),
				TableName,
				SelectColumns,
				outSelectedDatas,
				WhereCondition,
				OrderColumnName,
				bIsASC,
				LimitRowNum);
			if (resS.IsEmpty() || resS.Equals(TEXT("Successed"), ESearchCase::IgnoreCase))
			{
				for (auto& tmp : outSelectedDatas)
				{
					FoundDatas.Emplace(tmp);
				}
				if (FoundDatas.Num() <= 0)
				{
					CallUpdateDelegate_GameThread(TArray<FLeafSQLStringResultPreRow>(), TEXT("Successed,but not data found."));
				}
				else
				{
					CallUpdateDelegate_GameThread(FoundDatas, TEXT("Successed"));
				}
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Completed);
			}
			else
			{
				UE_LOG(LogLeafMySqlAsync_FindDataFromTable, Error, TEXT("FindDataFromTable_Async - %s"), *resS);
				CallUpdateDelegate_GameThread(TArray<FLeafSQLStringResultPreRow>(), FString::Printf(TEXT("FindDataFromTable_Async - %s"), *resS));
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
			}
		}
	);
}

ULeafMySqlAsync_FindDataFromTable* ULeafMySqlAsync_FindDataFromTable::FindDataFromTable_Async(
	UPARAM(Ref)ULeafMySQLAccessorBase*& InSQLAccessor,
	const FString& InTableName,
	const TArray<FString>& InSelectColumns,
	const FLeafSQLWhereString InWhereCondition,
	const FString& InOrderColumnName,
	const bool bInIsASC,
	const int64 InLimitRowNum)
{
	ULeafMySqlAsync_FindDataFromTable* MyObj = NewObject<ULeafMySqlAsync_FindDataFromTable>();
	MyObj->SQLAccessor = InSQLAccessor;
	MyObj->TableName = InTableName;
	MyObj->SelectColumns = InSelectColumns;
	MyObj->WhereCondition = InWhereCondition.WhereString;
	MyObj->OrderColumnName = InOrderColumnName;
	MyObj->bIsASC = bInIsASC;
	MyObj->LimitRowNum = InLimitRowNum;

	return MyObj;
}

void ULeafMySqlAsync_FindDataFromTable::CallUpdateDelegate_GameThread(const TArray<FLeafSQLStringResultPreRow>& InSelectedDatas, const FString& InError)
{
	if (!IsInGameThread())
	{
		AsyncTask(ENamedThreads::GameThread, [InError, this]()
			{
				OnResultOrErrorGet.Broadcast(FoundDatas, InError);
			});
	}
	else
	{
		OnResultOrErrorGet.Broadcast(InSelectedDatas, InError);
	}
}
