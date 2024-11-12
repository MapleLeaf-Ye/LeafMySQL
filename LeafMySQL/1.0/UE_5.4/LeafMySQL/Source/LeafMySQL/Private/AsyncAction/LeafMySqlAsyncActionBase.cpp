// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#include "AsyncAction/LeafMySqlAsyncActionBase.h"



ULeafMySqlAsyncActionBase::ULeafMySqlAsyncActionBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SQLAccessor = nullptr;
	bIsCanceled = false;
}

void ULeafMySqlAsyncActionBase::BeginDestroy()
{
	SQLAccessor = nullptr;
	Super::BeginDestroy();
}

void ULeafMySqlAsyncActionBase::Cancel()
{
	bIsCanceled = true;
	UCancellableAsyncAction::Cancel();
}

void ULeafMySqlAsyncActionBase::CallStateChangeDelegate(const ELeafSQLAsyncActionStateType InType)
{
	if (!IsInGameThread())
	{
		AsyncTask(ENamedThreads::GameThread, [InType, this]()
			{
				CallStateChangeDelegate(InType);
			});
	}
	else
	{
		if (InType == ELeafSQLAsyncActionStateType::Failed)
		{
			OnFailed.Broadcast();
		}
		else if (InType == ELeafSQLAsyncActionStateType::Completed)
		{
			OnCompleted.Broadcast();
		}
		else
		{
			OnCanceled.Broadcast();
		}
	}
}
