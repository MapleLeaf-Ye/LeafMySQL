// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "LeafMySQLSubsystem.generated.h"

class ULeafMySQLAccessorBase;

UCLASS()
class LEAFMYSQL_API ULeafMySQLSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	ULeafMySQLSubsystem();

	/**
	* 连接到MySql的函数
	* @Param InLoginInfo 传入的连接数据库参数
	* @Param OutError 输出错误信息，当连接失败时，如果连接成功则为空
	* @Return 返回MySQL访问器对象，你可以通过该对象对数据库操作
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLSubsystem|System")
	ULeafMySQLAccessorBase* ConnectMySQL(const FLeafSQLLoginInfo& InLoginInfo, FString& OutError);
};