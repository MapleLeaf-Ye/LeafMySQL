// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "CoreMinimal.h"
#include "SQLTableOperationBase.h"
#include "SQLTableDestroyer.generated.h"

using namespace LeafMySql;

UCLASS(BlueprintType)
class LEAFMYSQL_API USQLTableDestroyer : public USQLTableOperationBase
{
	GENERATED_UCLASS_BODY()
public:
	void InitializeTableDestroyer(
		FLeafMySqlLinkInfo& InLinkInfoPtr,
		const FString& InDataTableName,
		const bool InCheckIsExists = true,
		const bool InJustClearData = false);

	virtual ~USQLTableDestroyer();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|SQLTableDestroyer|Getter")
	FORCEINLINE FString GetDataTableName() const { return DeleteInfoPtr->DataTableName; }
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableDestroyer|Setter")
	FORCEINLINE void SetDataTableName(const FString& InNewDataTableName) { DeleteInfoPtr->DataTableName = InNewDataTableName; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|SQLTableDestroyer|Getter")
	FORCEINLINE bool GetCheckIsExists() const { return DeleteInfoPtr->bCheckIsExists; }
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableDestroyer|Setter")
	FORCEINLINE void SetCheckIsExists(const bool InNewCheckIsExists) { DeleteInfoPtr->bCheckIsExists = InNewCheckIsExists; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|SQLTableDestroyer|Getter")
	FORCEINLINE bool GetJustClearData() const { return DeleteInfoPtr->bJustClearData; }
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableDestroyer|Setter")
	FORCEINLINE void SetJustClearData(const bool InNewJustClearData) { DeleteInfoPtr->bJustClearData = InNewJustClearData; }

	FORCEINLINE Command::DataTable::FLeafMySqlDataTableDelete* GetDeleteInfo() const { return DeleteInfoPtr; }
private:
	Command::DataTable::FLeafMySqlDataTableDelete* DeleteInfoPtr;
};