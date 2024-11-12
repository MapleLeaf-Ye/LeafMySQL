// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#include "SQLAccessor/LeafMySQLAccessorBase.h"

#include "LeafMySQL.h"
#include "SQLTableOperation/SQLTableConstructor.h"
#include "SQLTableOperation/SQLTableModifier_AddColumn.h"
#include "SQLTableOperation/SQLTableModifier_ModifyColumn.h"
#include "SQLTableOperation/SQLTableModifier_RemoveColumn.h"
#include "SQLTableOperation/SQLTableModifier_RenameColumn.h"
#include "Interfaces/IPluginManager.h"
#include "UObject/UObjectGlobals.h"


using namespace LeafMySql::Command::DataTable;

DEFINE_LOG_CATEGORY(LogLeafMySQLAccessor);

ULeafMySQLAccessorBase::ULeafMySQLAccessorBase(const FObjectInitializer& Init) : Super(Init)
{
}

bool ULeafMySQLAccessorBase::ConnectMySQL(const FLeafSQLLoginInfo& InLoginInfo, FString& OutErrorMessage)
{
	if (!LeafMySql::IsLeafMySQLValid())
	{
		ensure(0);
		UE_LOG(LogLeafMySQLAccessor, Error, TEXT("MySQL Dll is not ready ok!!!"));
		return false;
	}
	if (!bIsSqlLinking)
	{
		LoginInfo = InLoginInfo;
		LeafConnectMySQL(LoginInfo, LinkInfo, OutErrorMessage);
		bIsSqlLinking = LinkInfo.IsConnectionValid();
		return bIsSqlLinking;
	}
	return false;
}

bool ULeafMySQLAccessorBase::ReConnectMySQL(const FLeafSQLLoginInfo& InLoginInfo, FString& OutErrorMessage)
{
	return ConnectMySQL(InLoginInfo, OutErrorMessage);
}

void ULeafMySQLAccessorBase::DeConnectMySQL()
{
	if (bIsSqlLinking)
	{
		LeafCloseMySQLConnection(LinkInfo);
		bIsSqlLinking = false;
	}
}

bool ULeafMySQLAccessorBase::ShowSystemDetail(TMap<FString, FString>& OutSystemDetail)
{
	if (bIsSqlLinking)
	{
		LeafShowSystemDetail(LinkInfo, OutSystemDetail);
	}
	return false;
}

FString ULeafMySQLAccessorBase::ExecuteCustomSQLCommand(const FString& InSQLCommand, FLeafSQLStringResults& OutResults)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	TArray<TMap<FString, FString>> outInfos;
	const FString resS = LeafExecuteCustomSQLCommand(LinkInfo, InSQLCommand, outInfos);
	OutResults = FLeafSQLStringResults(outInfos);
	return resS;
}

bool ULeafMySQLAccessorBase::ShowAllDataBase(TArray<FString>& OutDataBaseNames)
{
	CHECKLEAFSQLLINKINGWITHRETURN(false);
	DATABASE::LeafShowAllDataBase(LinkInfo, OutDataBaseNames);
	return true;
}

bool ULeafMySQLAccessorBase::SelectDataBaseForName(const FString& InNewDataBaseName)
{
	CHECKLEAFSQLLINKINGWITHRETURN(false);
	DATABASE::LeafSelectDataBaseFromName(LinkInfo, InNewDataBaseName);
	return true;
}

FString ULeafMySQLAccessorBase::ShowCurrentDataBaseAllTable(TArray<FString>& OutDataTableNames)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = DATABASE::LeafShowCurrentDataBaseAllTable(LinkInfo, OutDataTableNames);
	return resS;
}

FString ULeafMySQLAccessorBase::CreateDataBase(const FString& InDataBaseName, const ELeafCharacterSet InCharacterSet, const FString& InCollate, const bool InCheckIsExists)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	UEnum* sPtr = StaticEnum<ELeafCharacterSet>();
	FString sName = sPtr->GetNameStringByIndex((uint8)InCharacterSet);
	
	Command::DataBase::FLeafMySqlDataBaseCreate createInfo(LinkInfo, InDataBaseName, sName, InCollate, InCheckIsExists);
	const FString resS = DATABASE::LeafCreateDataBase(createInfo);
	return resS;
}

FString ULeafMySQLAccessorBase::CreateDataBase_Custom(const FString& InDataBaseName, const FString& InCharacterSet, const FString& InCollate, const bool InCheckIsExists)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	Command::DataBase::FLeafMySqlDataBaseCreate createInfo(LinkInfo, InDataBaseName, InCharacterSet, InCollate, InCheckIsExists);
	const FString resS = DATABASE::LeafCreateDataBase(createInfo);
	return resS;
}

FString ULeafMySQLAccessorBase::UpdateDataBaseSettings(const FString& InDataBaseName, const ELeafCharacterSet InCharacterSet, const FString& InCollate)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	UEnum* sPtr = StaticEnum<ELeafCharacterSet>();
	FString sName = sPtr->GetNameStringByIndex((uint8)InCharacterSet);

	Command::DataBase::FLeafMySqlDataBaseUpdateSettings updateInfo(LinkInfo, InDataBaseName, sName, InCollate);
	const FString resS = DATABASE::LeafUpdateDataBaseSettings(updateInfo);
	return resS;
}

FString ULeafMySQLAccessorBase::UpdateDataBaseSettings_Custom(const FString& InDataBaseName, const FString& InCharacterSet, const FString& InCollate)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	Command::DataBase::FLeafMySqlDataBaseUpdateSettings updateInfo(LinkInfo, InDataBaseName, InCharacterSet, InCollate);
	const FString resS = DATABASE::LeafUpdateDataBaseSettings(updateInfo);
	return resS;
}

FString ULeafMySQLAccessorBase::DeleteDataBase(const FString& InDataBaseName, const bool InCheckIsExists)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	Command::DataBase::FLeafMySqlDataBaseDelete deleteInfo(LinkInfo, InDataBaseName, InCheckIsExists);
	const FString resS = DATABASE::LeafDeleteDataBase(deleteInfo);
	return resS;
}

FString ULeafMySQLAccessorBase::ShowDataBaseDetails(TMap<FString, FString>& OutDataBaseDetail)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = DATABASE::LeafShowDataBaseDetails(LinkInfo, OutDataBaseDetail);
	return resS;
}

FString ULeafMySQLAccessorBase::GetDataBaseStatus(TMap<FString, FString>& OutDataBaseStatus)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = DATABASE::LeafGetDataBaseStatus(LinkInfo, OutDataBaseStatus);
	return resS;
}

FString ULeafMySQLAccessorBase::GetDataBaseErrors(TArray<FString>& OutDataTableError)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = DATABASE::LeafGetDataBaseErrors(LinkInfo, OutDataTableError);
	return resS;
}

FString ULeafMySQLAccessorBase::GetDataBaseWarnings(TArray<FString>& OutDataTableWarnings)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = DATABASE::LeafGetDataBaseWarnings(LinkInfo, OutDataTableWarnings);
	return resS;
}

FString ULeafMySQLAccessorBase::CreateDataTable(const USQLTableConstructor* InConstructor)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = DATATABLE::LeafCreateDataTable(*InConstructor->GetCreateInfo());
	return resS;
}

USQLTableConstructor* ULeafMySQLAccessorBase::MakeNewTableConstructor(
	const FString& InDataTableName,
	const FString& InCollate,
	const bool InCheckIsExists,
	const FString& InEngine,
	const ELeafCharacterSet InCharset)
{
	CHECKLEAFSQLLINKING();
	USQLTableConstructor* newConstructor = NewObject<USQLTableConstructor>(this);
	if (newConstructor)
	{
		newConstructor->InitializeTableConstructor(LinkInfo, InDataTableName, InCollate, InCheckIsExists, InEngine, InCharset);
		return newConstructor;
	}
	return nullptr;
}

USQLTableConstructor* ULeafMySQLAccessorBase::MakeNewTableConstructorWithString(
	const FString& InDataTableName,
	const bool InCheckIsExists,
	const FString& InEngine,
	const FString InCharset,
	const FString& InCollate)
{
	CHECKLEAFSQLLINKING();
	USQLTableConstructor* newConstructor = NewObject<USQLTableConstructor>(this);
	if (newConstructor)
	{
		newConstructor->InitializeTableConstructor(LinkInfo, InDataTableName, InCheckIsExists, InEngine, InCharset, InCollate);
		return newConstructor;
	}
	return nullptr;
}

FString ULeafMySQLAccessorBase::DeleteDataTable(const FString& InDataTableName, const bool bInCheckIsExists, const bool bInJustClearData)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	Command::DataTable::FLeafMySqlDataTableDelete info(LinkInfo, InDataTableName, bInCheckIsExists, bInJustClearData);
	const FString resS = DATATABLE::LeafDeleteDataTable(info);
	return resS;
}

FString ULeafMySQLAccessorBase::RemoveColumn(const FString& InDataTableName, const FString& InRemoveColumnName)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	Command::DataTable::FLeafMySqlDataTableModify_RemoveColumn info(LinkInfo, InDataTableName, InRemoveColumnName);
	const FString resS = DATATABLE::LeafModifyDataTableStructure_RemoveColumn(info);
	return resS;
}

FString ULeafMySQLAccessorBase::RenameColumn(const FString& InDataTableName, const FString& InModifyColumnName, const FString& InModifyNewColumnName)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	Command::DataTable::FLeafMySqlDataTableParamInfoBase paramInfo(Command::DataTable::FLeafMySqlDataTableParamInfoBase::NoValidType);
	paramInfo.SetParamName(InModifyColumnName);
	Command::DataTable::FLeafMySqlDataTableModify_RenameColumn info(LinkInfo, InDataTableName, InModifyNewColumnName, paramInfo);
	const FString resS = DATATABLE::LeafModifyDataTableStructure_RenameColumn(info);
	return resS;
}

FString ULeafMySQLAccessorBase::ModifyDataTable_AddColumn(USQLTableModifier_AddColumn* InModifier)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	FString resS = TEXT("Successed");
	if (InModifier->GetModifyInfo().Num() > 0)
	{
		for (auto tmp : InModifier->GetModifyInfo())
		{
			resS = DATATABLE::LeafModifyDataTableStructure_AddColumn(*(FLeafMySqlDataTableModify_AddColumn*)(tmp));
			if (!resS.IsEmpty() && !resS.Equals(TEXT("Successed"), ESearchCase::IgnoreCase))
			{
				return resS;
			}
		}
	}
	return resS;
}

FString ULeafMySQLAccessorBase::ModifyDataTable_RemoveColumn(USQLTableModifier_RemoveColumn* InModifier)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	FString resS = TEXT("Successed");
	if (InModifier->GetModifyInfo().Num() > 0)
	{
		for (auto tmp : InModifier->GetModifyInfo())
		{
			resS = DATATABLE::LeafModifyDataTableStructure_RemoveColumn(*(FLeafMySqlDataTableModify_RemoveColumn*)(tmp));
			if (!resS.IsEmpty() && !resS.Equals(TEXT("Successed"), ESearchCase::IgnoreCase))
			{
				return resS;
			}
		}
	}
	return resS;
}

FString ULeafMySQLAccessorBase::ModifyDataTable_ModifyColumn(USQLTableModifier_ModifyColumn* InModifier)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	FString resS = TEXT("Successed");
	if (InModifier->GetModifyInfo().Num() > 0)
	{
		for (auto tmp : InModifier->GetModifyInfo())
		{
			resS = DATATABLE::LeafModifyDataTableStructure_ModifyColumn(*(FLeafMySqlDataTableModify_ModifyColumn*)(tmp));
			if (!resS.IsEmpty() && !resS.Equals(TEXT("Successed"), ESearchCase::IgnoreCase))
			{
				return resS;
			}
		}
	}
	return resS;
}

FString ULeafMySQLAccessorBase::ModifyDataTable_RenameColumn(USQLTableModifier_RenameColumn* InModifier)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	FString resS = TEXT("Successed");
	if (InModifier->GetModifyInfo().Num() > 0)
	{
		for (auto tmp : InModifier->GetModifyInfo())
		{
			resS = DATATABLE::LeafModifyDataTableStructure_RenameColumn(*(FLeafMySqlDataTableModify_RenameColumn*)(tmp));
			if (!resS.IsEmpty() && !resS.Equals(TEXT("Successed"), ESearchCase::IgnoreCase))
			{
				return resS;
			}
		}
	}
	return resS;
}

USQLTableModifier* ULeafMySQLAccessorBase::MakeTableModifier(
	TSubclassOf<USQLTableModifier> InModifierClass,
	const FString& InDataTableName)
{
	CHECKLEAFSQLLINKING();
	USQLTableModifier* newModifier = NewObject<USQLTableModifier>(this, InModifierClass);
	if (newModifier)
	{
		newModifier->InitializeTableModifier(LinkInfo, InDataTableName);
		return newModifier;
	}
	return nullptr;
}

FString ULeafMySQLAccessorBase::ShowDataTableStructure(const FString& InTableName, TMap<FString, FLeafColumnStructureDisplay>& OutDataTableStructure)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	TMap<FString, Command::DataTable::FLeafMySqlDataTableParamDisplay> outDataTableStructure;
	const FString resS = DATATABLE::LeafShowDataTableStructure(LinkInfo, InTableName, outDataTableStructure);
	for (auto& tmp : outDataTableStructure)
	{
		OutDataTableStructure.Add(tmp.Key, FLeafColumnStructureDisplay(tmp.Value));
	}
	return resS;
}

FString ULeafMySQLAccessorBase::ShowDataTableSQLCreateCommand(const FString& InTableName)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = DATATABLE::LeafShowDataTableSQLCreateCommand(LinkInfo, InTableName);
	return resS;
}

FString ULeafMySQLAccessorBase::ShowTableAllData(const FString& InTableName, TArray<FLeafSQLStringResultPreRow>& OutDatas)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	TArray<TMap<FString, FString>> outDatas;
	const FString resS = DATATABLE::LeafShowTableAllData(LinkInfo, InTableName, outDatas);
	for (auto& tmp : outDatas)
	{
		OutDatas.Emplace(tmp);
	}
	return resS;
}

FString ULeafMySQLAccessorBase::ShowTableColumnAllData(const FString& InTableName, const FString& InColumnName, TArray<FString>& OutDatas)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = DATATABLE::LeafShowTableColumnAllData(LinkInfo, InTableName, InColumnName, OutDatas);
	return resS;
}

FString ULeafMySQLAccessorBase::ShowTableIndex(const FString& InTableName, TMap<FString, FString>& OutInfos)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = DATATABLE::LeafShowTableIndex(LinkInfo, InTableName, OutInfos);
	return resS;
}

FString ULeafMySQLAccessorBase::ShowDataTableStatus(const FString& InTableName, TMap<FString, FString>& OutInfos)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = DATATABLE::LeafShowDataTableStatus(LinkInfo, InTableName, OutInfos);
	return resS;
}

FString ULeafMySQLAccessorBase::CreateTableIndex(const FString& InTableName, const FString& InIndexName, const TArray<FString>& InColumnInfos)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = DATATABLE::LeafCreateTableIndex(LinkInfo, InTableName, InIndexName, InColumnInfos);
	return resS;
}

FString ULeafMySQLAccessorBase::DeleteTableIndex(const FString& InTableName, const FString& InIndexName)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = DATATABLE::LeafDeleteTableIndex(LinkInfo, InTableName, InIndexName);
	return resS;
}

FString ULeafMySQLAccessorBase::InsertDataToTable(
	const FString& InTableName, 
	const TMap<FString, FString>& InInsertDatas)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = DATA::LeafInsertDataToTable(
		LinkInfo, 
		InTableName, 
		InInsertDatas);
	return resS;
}

FString ULeafMySQLAccessorBase::InsertDataToTableForString(const FString& InTableName, const FString& InInsertColumnNames, const FString& InInsertDatas)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = DATA::LeafInsertDataToTable(
		LinkInfo,
		InTableName,
		InInsertColumnNames,
		InInsertDatas);
	return resS;
}

FString ULeafMySQLAccessorBase::InsertDataToTable_Trunk(const FString& InTableName, const FString& InInsertColumnNames, const TArray<FString>& InInsertDatas)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = DATA::LeafInsertDataToTable_Trunk(
		LinkInfo,
		InTableName,
		InInsertColumnNames,
		InInsertDatas);
	return resS;
}

FString ULeafMySQLAccessorBase::FindDataFromTable(
	const FString& InTableName,
	const TArray<FString>& InSelectColumns, 
	TArray<FLeafSQLStringResultPreRow>& OutSelectedDatas,
	const FLeafSQLWhereString InWhereCondition,
	const FString& InOrderColumnName, 
	const bool bInIsASC, 
	const int64 InLimitRowNum)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	TArray<TMap<FString, FString>> outSelectedDatas;
	const FString resS = DATA::LeafFindDataFromTable(
		LinkInfo, 
		InTableName, 
		InSelectColumns,
		outSelectedDatas,
		InWhereCondition.WhereString,
		InOrderColumnName,
		bInIsASC,
		InLimitRowNum);
	for (auto& tmp : outSelectedDatas)
	{
		OutSelectedDatas.Emplace(tmp);
	}
	return resS;
}

FString ULeafMySQLAccessorBase::FindDataFromTableForString(
	const FString& InTableName,
	const FString& InSelectColumns,
	TArray<FLeafSQLStringResultPreRow>& OutSelectedDatas,
	const FLeafSQLWhereString InWhereCondition,
	const FString& InOrderColumnName,
	const bool bInIsASC,
	const int64 InLimitRowNum)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	TArray<TMap<FString, FString>> outSelectedDatas;
	const FString resS = DATA::LeafFindDataFromTable(
		LinkInfo,
		InTableName,
		InSelectColumns,
		outSelectedDatas,
		InWhereCondition.WhereString,
		InOrderColumnName,
		bInIsASC,
		InLimitRowNum);
	for (auto& tmp : outSelectedDatas)
	{
		OutSelectedDatas.Emplace(tmp);
	}
	return resS;
}

FString ULeafMySQLAccessorBase::UpdateDataFromTable(
	const FString& InTableName,
	const TMap<FString, FString>& InUpdateDatas,
	const FLeafSQLWhereString& InWhereCondition)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = DATA::LeafUpdateDataFromTable(
		LinkInfo,
		InTableName,
		InUpdateDatas,
		InWhereCondition.WhereString);
	return resS;
}

FString ULeafMySQLAccessorBase::UpdateDataFromTableForString(const FString& InTableName, const FString& InUpdateDatas, const FLeafSQLWhereString& InWhereCondition)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = DATA::LeafUpdateDataFromTable(
		LinkInfo,
		InTableName,
		InUpdateDatas,
		InWhereCondition.WhereString);
	return resS;
}

FString ULeafMySQLAccessorBase::DeleteDataFromTable(const FString& InTableName, const FLeafSQLWhereString& InWhereCondition)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = DATA::LeafDeleteDataFromTable(
		LinkInfo,
		InTableName,
		InWhereCondition.WhereString);
	return resS;
}

FString ULeafMySQLAccessorBase::SetEventCommitAuto(const bool InNewAuto)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = EVENT::LeafSetEventCommitAuto(LinkInfo, InNewAuto);
	return resS;
}

bool ULeafMySQLAccessorBase::GetEventCommitAuto() const
{
	CHECKLEAFSQLLINKING();
	return EVENT::LeafGetEventCommitAuto(LinkInfo);
}

FString ULeafMySQLAccessorBase::BeginEvent()
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = EVENT::LeafBeginEvent(LinkInfo);
	return resS;
}

FString ULeafMySQLAccessorBase::CommitEvent()
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = EVENT::LeafCommitEvent(LinkInfo);
	return resS;
}

FString ULeafMySQLAccessorBase::AddSavePoint(const FString& InNewSavePointName)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = EVENT::LeafAddSavePoint(LinkInfo, InNewSavePointName);
	return resS;
}

FString ULeafMySQLAccessorBase::RollBackEvent(const FString& InSavePointName)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = EVENT::LeafRollBackEvent(LinkInfo, InSavePointName);
	return resS;
}

FString ULeafMySQLAccessorBase::ShowCurrentEventStatus(TArray<FLeafSQLStringResultPreRow>& OutInfos)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	TArray<TMap<FString, FString>> outInfos;
	const FString resS = EVENT::LeafShowCurrentEventStatus(LinkInfo, outInfos);
	for (auto& tmp : outInfos)
	{
		OutInfos.Emplace(tmp);
	}
	return resS;
}

FString ULeafMySQLAccessorBase::LockTable_Write(const FString& InTableName)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = EVENT::LeafLockTable_Write(LinkInfo, InTableName);
	return resS;
}

FString ULeafMySQLAccessorBase::LockTable_Read(const FString& InTableName)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = EVENT::LeafLockTable_Read(LinkInfo, InTableName);
	return resS;
}

FString ULeafMySQLAccessorBase::UnLockTable()
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = EVENT::LeafUnLockTable(LinkInfo);
	return resS;
}

FString ULeafMySQLAccessorBase::SetTransactionIsolationLevel(const ELeafSqlTransactionIsolationLevel InLevel)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	UEnum* ePtr = StaticEnum<ELeafSqlTransactionIsolationLevel>();
	const FString eS = ePtr->GetNameStringByValue((uint8)InLevel);
	const FString resS = EVENT::LeafSetTransactionIsolationLevel(LinkInfo, LeafMySql::ELeafMySqlTransactionIsolationLevelType::GetLeafMySqlTransactionIsolationLevelTypeFromString(eS));
	return resS;
}

FString ULeafMySQLAccessorBase::ImportDataToDataTableFromCSV(const FString& InFilePath, const FString& InTableName, const bool InOverrideSourceData)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = OTHER::LeafImportDataToDataTableFromCSV(LinkInfo, InFilePath, InTableName, InOverrideSourceData);
	return resS;
}

FString ULeafMySQLAccessorBase::ImportDataToDataTableFromCSV_Trunk(const FString& InFilePath, const FString& InTableName, const bool InOverrideSourceData, const int32 InTrunkSize)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = OTHER::LeafImportDataToDataTableFromCSV_Trunk(LinkInfo, InFilePath, InTableName, InOverrideSourceData, InTrunkSize);
	return resS;
}

FString ULeafMySQLAccessorBase::ExportDataAsCSVFromDataTable(const FString& InFileExportPath, const FString& InTableName, const int64 InLimitRowNum)
{
	CHECKLEAFSQLLINKINGWITHRETURN(TEXT("Failed"));
	const FString resS = OTHER::LeafExportDataAsCSVFromDataTable(LinkInfo, InFileExportPath, InTableName, InLimitRowNum);
	return resS;
}

ULeafMySQLAccessorBase::~ULeafMySQLAccessorBase()
{
	DeConnectMySQL();
}
