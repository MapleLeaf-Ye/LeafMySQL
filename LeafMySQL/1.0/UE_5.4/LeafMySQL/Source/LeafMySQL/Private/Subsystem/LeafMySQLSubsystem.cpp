// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#include "Subsystem/LeafMySQLSubsystem.h"

#include "SQLAccessor/LeafMySQLAccessorBase.h"


ULeafMySQLSubsystem::ULeafMySQLSubsystem()
{
	
}

ULeafMySQLAccessorBase* ULeafMySQLSubsystem::ConnectMySQL(const FLeafSQLLoginInfo& InLoginInfo, FString& OutError)
{
	ULeafMySQLAccessorBase* linkingAccessor = nullptr;
	linkingAccessor = NewObject<ULeafMySQLAccessorBase>(this);
	if (linkingAccessor->ConnectMySQL(InLoginInfo, OutError))
	{
		return linkingAccessor;
	}
	return nullptr;
}

