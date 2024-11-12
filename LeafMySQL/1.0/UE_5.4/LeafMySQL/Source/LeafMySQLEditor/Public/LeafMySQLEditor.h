// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "Modules/ModuleManager.h"
#include "DataType/LeafMySQLType.h"

class FLeafMySQLEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};