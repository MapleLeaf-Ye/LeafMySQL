// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "Modules/ModuleManager.h"
#include "DataType/LeafMySQLType.h"


DECLARE_LOG_CATEGORY_EXTERN(LogLeafMySQL, Warning, All);

#define Check_LEAFMYSQL() \
checkf(LeafMySql::IsLeafMySQLValid(),TEXT("LeafMySql is not valid!!!"))

class FLeafMySQLModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	FORCEINLINE bool IsMySQLDLLValid() const { return LibcryptoDLLHandle && LibsslDLLHandle; }

private:
	void* LibcryptoDLLHandle;
	void* LibsslDLLHandle;
};

namespace LeafMySql
{
	bool LeafConnectMySQL(
		const FLeafMySqlLoginInfo& InLoginInfo,
		FLeafMySqlLinkInfo& OutLinkInfo,
		FString& OutErrorInfo);

	void LeafCloseMySQLConnection(FLeafMySqlLinkInfo& InLinkInfo);

	bool IsLeafMySQLValid();

	bool LeafShowSystemDetail(FLeafMySqlLinkInfo& InLinkInfo, TMap<FString, FString>& OutSystemDetail);

	FString LeafExecuteCustomSQLCommand(FLeafMySqlLinkInfo& InLinkInfo, const FString& InSQLCommand, TArray<TMap<FString, FString>>& OutResults);

	namespace DATABASE
	{
		bool LeafShowAllDataBase(FLeafMySqlLinkInfo& InLinkInfo, TArray<FString>& OutDataBaseNames);
		bool LeafSelectDataBaseFromName(const FLeafMySqlLinkInfo& InLinkInfo, const FString& InNewDataBaseName);
		FString LeafShowCurrentDataBaseAllTable(FLeafMySqlLinkInfo& InLinkInfo, TArray<FString>& OutDataTableNames);
		FString LeafCreateDataBase(Command::DataBase::FLeafMySqlDataBaseCreate& InCreateInfo);
		FString LeafUpdateDataBaseSettings(Command::DataBase::FLeafMySqlDataBaseUpdateSettings& InUpdateInfo);
		FString LeafDeleteDataBase(Command::DataBase::FLeafMySqlDataBaseDelete& InDeleteInfo);

		FString LeafShowDataBaseDetails(FLeafMySqlLinkInfo& InLinkInfo, TMap<FString, FString>& OutDataBaseDetail);
		FString LeafGetDataBaseStatus(FLeafMySqlLinkInfo& InLinkInfo, TMap<FString, FString>& OutDataBaseStatus);
		FString LeafGetDataBaseErrors(FLeafMySqlLinkInfo& InLinkInfo, TArray<FString>& OutDataTableError);
		FString LeafGetDataBaseWarnings(FLeafMySqlLinkInfo& InLinkInfo, TArray<FString>& OutDataTableWarnings);

	}

	namespace DATATABLE
	{
		FString LeafCreateDataTable(Command::DataTable::FLeafMySqlDataTableCreate& InCreateInfo);
		FString LeafDeleteDataTable(Command::DataTable::FLeafMySqlDataTableDelete& InDaleteInfo);
		FString LeafModifyDataTableStructure_AddColumn(Command::DataTable::FLeafMySqlDataTableModify_AddColumn& InAddColumnInfo);
		FString LeafModifyDataTableStructure_RemoveColumn(Command::DataTable::FLeafMySqlDataTableModify_RemoveColumn& InRemoveColumnInfo);
		FString LeafModifyDataTableStructure_ModifyColumn(Command::DataTable::FLeafMySqlDataTableModify_ModifyColumn& InModifyColumnInfo);
		FString LeafModifyDataTableStructure_RenameColumn(Command::DataTable::FLeafMySqlDataTableModify_RenameColumn& InRenameColumnInfo);
		FString LeafShowDataTableStructure(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, TMap<FString, Command::DataTable::FLeafMySqlDataTableParamDisplay>& OutDataTableStructure);
		FString LeafShowDataTableSQLCreateCommand(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName);
		FString LeafShowTableAllData(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, TArray<TMap<FString,FString>>& OutDatas);
		FString LeafShowTableColumnAllData(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, const FString& InColumnName, TArray<FString>& OutDatas);
		FString LeafShowTableIndex(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, TMap<FString, FString>& OutInfos);
		FString LeafCreateTableIndex(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, const FString& InIndexName, const TArray<FString>& InColumnInfos);
		FString LeafDeleteTableIndex(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, const FString& InIndexName);
		FString LeafShowDataTableStatus(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, TMap<FString, FString>& OutInfos);
	}

	namespace DATA
	{
		FString LeafInsertDataToTable(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, const TMap<FString, FString>& InInsertDatas);
		FString LeafInsertDataToTable(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, const FString& InInsertColumnNames, const FString& InInsertDatas);
		FString LeafInsertDataToTable_Trunk(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, const FString& InInsertColumnNames, const TArray<FString>& InInsertDatas);

		FString LeafFindDataFromTable(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, 
			const TArray<FString>& InSelectColumns, TArray<TMap<FString,FString>>& OutSelectedDatas,
			const FString& InWhereCondition = TEXT("None"), const FString& InOrderColumnName = TEXT("None"),
			const bool bInIsASC = true, const int64 InLimitRowNum = 0);
		FString LeafFindDataFromTable(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName,
			const FString& InSelectColumns, TArray<TMap<FString, FString>>& OutSelectedDatas,
			const FString& InWhereCondition = TEXT("None"), const FString& InOrderColumnName = TEXT("None"),
			const bool bInIsASC = true, const int64 InLimitRowNum = 0);
		FString LeafFindDataFromTable(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName,
			const FString& InSelectColumns, TArray<TArray<FString>>& OutSelectedDatas,
			const FString& InWhereCondition = TEXT("None"), const FString& InOrderColumnName = TEXT("None"),
			const bool bInIsASC = true, const int64 InLimitRowNum = 0);

		FString LeafUpdateDataFromTable(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName,
			const TMap<FString, FString>& InUpdateDatas, const FString& InWhereCondition);
		FString LeafUpdateDataFromTable(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName,
			const FString& InUpdateDatas, const FString& InWhereCondition);

		FString LeafDeleteDataFromTable(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, const FString& InWhereCondition);
	}

	namespace EVENT
	{
		FString LeafSetEventCommitAuto(FLeafMySqlLinkInfo& InLinkInfo, const bool InNewAuto = false);
		bool LeafGetEventCommitAuto(const FLeafMySqlLinkInfo& InLinkInfo);
		FString LeafBeginEvent(FLeafMySqlLinkInfo& InLinkInfo);
		FString LeafCommitEvent(FLeafMySqlLinkInfo& InLinkInfo);
		FString LeafAddSavePoint(FLeafMySqlLinkInfo& InLinkInfo,const FString& InNewSavePointName);
		FString LeafRollBackEvent(FLeafMySqlLinkInfo& InLinkInfo,const FString& InSavePointName = TEXT(""));
		FString LeafShowCurrentEventStatus(FLeafMySqlLinkInfo& InLinkInfo, TArray<TMap<FString, FString>>& OutInfos);
		FString LeafLockTable_Write(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName);
		FString LeafLockTable_Read(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName);
		FString LeafUnLockTable(FLeafMySqlLinkInfo& InLinkInfo);
		FString LeafSetTransactionIsolationLevel(FLeafMySqlLinkInfo& InLinkInfo, const LeafMySql::ELeafMySqlTransactionIsolationLevelType::Type InLevel);
	}

	namespace OTHER
	{
		FString LeafImportDataToDataTableFromCSV(FLeafMySqlLinkInfo& InLinkInfo, const FString& InFilePath, const FString& InTableName, const bool InOverrideSourceData = false);
		FString LeafImportDataToDataTableFromCSV_Trunk(FLeafMySqlLinkInfo& InLinkInfo, const FString& InFilePath, const FString& InTableName, const bool InOverrideSourceData = false, const int32 InTrunkSize = 100);
		FString LeafExportDataAsCSVFromDataTable(FLeafMySqlLinkInfo& InLinkInfo, const FString& InFileExportPath, const FString& InTableName, const int64 InLimitRowNum = 0);
	}
}
