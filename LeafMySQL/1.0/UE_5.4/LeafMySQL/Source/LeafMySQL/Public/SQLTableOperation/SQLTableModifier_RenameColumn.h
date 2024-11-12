// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "CoreMinimal.h"
#include "SQLTableOperation/SQLTableModifier.h"
#include "SQLTableModifier_RenameColumn.generated.h"

using namespace LeafMySql;

UCLASS(BlueprintType)
class LEAFMYSQL_API USQLTableModifier_RenameColumn : public USQLTableModifier
{
	GENERATED_UCLASS_BODY()
protected:
	virtual void InitializeTableModifier(
		FLeafMySqlLinkInfo& InLinkInfoPtr,
		const FString& InDataTableName) override;

public:

	/**
	* 为列修改器添加列重命名信息
	* @Param InColumnName 列源名字
	* @Param InColumnNewName 列新名字
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableModifier|ColumnModify")
	void RenameNewColumn(const FString& InColumnName, const FString& InColumnNewName);

};