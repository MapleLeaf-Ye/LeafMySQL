// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "CoreMinimal.h"
#include "SQLTableOperation/SQLTableModifier.h"
#include "SQLTableModifier_RemoveColumn.generated.h"

using namespace LeafMySql;

UCLASS(BlueprintType)
class LEAFMYSQL_API USQLTableModifier_RemoveColumn : public USQLTableModifier
{
	GENERATED_UCLASS_BODY()
protected:
	virtual void InitializeTableModifier(
		FLeafMySqlLinkInfo& InLinkInfoPtr,
		const FString& InDataTableName) override;

public:
	/**
	* 为列修改器添加列删除信息
	* @Param InColumnName 要删除的列名字
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|SQLTableModifier|ColumnModify")
	void RemoveNewColumn(const FString& InColumnName);

};