// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/CancellableAsyncAction.h"
#include "SQLAccessor/LeafMySQLAccessorBase.h"
#include "Async/TaskGraphInterfaces.h"
#include "Async/Async.h"
#include "Tasks/Task.h"
#include "LeafMySqlAsyncActionBase.generated.h"

enum class ELeafSQLAsyncActionStateType :uint8
{
	Failed,
	Completed,
	Cancel
};

UCLASS(MinimalAPI)
class ULeafMySqlAsyncActionBase : public UCancellableAsyncAction
{
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLeafMySqlAsyncActionDelegate);
	GENERATED_BODY()
public:
	ULeafMySqlAsyncActionBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable)
	FLeafMySqlAsyncActionDelegate OnCompleted;
	UPROPERTY(BlueprintAssignable)
	FLeafMySqlAsyncActionDelegate OnCanceled;
	UPROPERTY(BlueprintAssignable)
	FLeafMySqlAsyncActionDelegate OnFailed;

protected:
	virtual void BeginDestroy() override;
	virtual void Cancel() override;

	void CallStateChangeDelegate(const ELeafSQLAsyncActionStateType InType);

	FORCEINLINE struct FLeafMySqlLinkInfo* GetLinkInfo() 
	{
		if (!SQLAccessor)
		{
			return nullptr;
		}
		if (!SQLAccessor->GetSQLConnectionState())
		{
			return nullptr;
		}
		return &SQLAccessor->GetLinkInfo();
	}

protected:
	ULeafMySQLAccessorBase* SQLAccessor;
	bool bIsCanceled;
};