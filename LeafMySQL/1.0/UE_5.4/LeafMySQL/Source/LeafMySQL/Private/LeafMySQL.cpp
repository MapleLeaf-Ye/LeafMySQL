// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#include "LeafMySQL.h"

#include "Misc/AssertionMacros.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"

#pragma push_macro("check")   // store 'check' macro current definition
#undef check  // undef to avoid conflicts
#include "jdbc/cppconn/metadata.h"
#pragma pop_macro("check")  // restore definition

DEFINE_LOG_CATEGORY(LogLeafMySQL);

#define LOCTEXT_NAMESPACE "FLeafMySQLModule"

#define LEAFCATCHSQLBEGIN() \
FString error;\
try\
{

#define LEAFCATCHSQLEND() \
}\
catch (const sql::SQLException& e)\
{\
	sql::SQLString sqlError(e.what());\
	error = UTF8_TO_TCHAR(sqlError.c_str());\
}\
catch (const std::exception& e)\
{\
	sql::SQLString stdErrors(e.what());\
	error = UTF8_TO_TCHAR(stdErrors.c_str());\
}

#define LEAFRETURNERRORMESSAGE(ErrorMessage,ResultSet)\
ErrorMessage.IsEmpty() || (ErrorMessage.Equals(TEXT("No result available"),ESearchCase::IgnoreCase)) ? FString(TEXT("Successed")) : ErrorMessage

void FLeafMySQLModule::StartupModule()
{
	FString BaseDir = IPluginManager::Get().FindPlugin("LeafMySQL")->GetBaseDir();

	FString LibraryPath1;
	FString LibraryPath2;
	FString SearchOtherDllPath;
#if PLATFORM_WINDOWS
	LibraryPath1 = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/LeafMySql/Win64/libcrypto-1_1-x64.dll"));
	LibraryPath2 = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/LeafMySql/Win64/libssl-1_1-x64.dll"));
	SearchOtherDllPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/LeafMySql/Win64"));
//#elif PLATFORM_MAC
//	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/sssLibrary/Mac/Release/libExampleLibrary.dylib"));
//#elif PLATFORM_LINUX
//	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/sssLibrary/Linux/x86_64-unknown-linux-gnu/libExampleLibrary.so"));
#endif // PLATFORM_WINDOWS
	FPlatformProcess::PushDllDirectory(*(SearchOtherDllPath));

	LibcryptoDLLHandle = !LibraryPath1.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath1) : nullptr;
	LibsslDLLHandle = !LibraryPath2.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath2) : nullptr;
	
	return;
}

void FLeafMySQLModule::ShutdownModule()
{
	if (LibsslDLLHandle)
	{
		FPlatformProcess::FreeDllHandle(LibsslDLLHandle);
		LibsslDLLHandle = nullptr;
	}

	if (LibcryptoDLLHandle)
	{
		FPlatformProcess::FreeDllHandle(LibcryptoDLLHandle);
		LibcryptoDLLHandle = nullptr;
	}
}

IMPLEMENT_MODULE(FLeafMySQLModule, LeafMySQL)

bool LeafMySql::LeafConnectMySQL(
	const FLeafMySqlLoginInfo& InLoginInfo,
	FLeafMySqlLinkInfo& OutLinkInfo,
	FString& OutErrorInfo)
{
	Check_LEAFMYSQL();

	LeafMySql::FLeafMySqlLinkInfo linkInfo;
	try
	{
		linkInfo.Connect(InLoginInfo);
	}
	catch (const sql::SQLException& e)
	{
		sql::SQLString sqlError(e.what()); 
		OutErrorInfo = UTF8_TO_TCHAR(sqlError.c_str());
		UE_LOG(LogLeafMySQL, Error, TEXT("LeafMySQL Error - %s"), *OutErrorInfo);
#if !UE_BUILD_SHIPPING
		check(0);
#endif
	}
	catch (const std::exception& e)
	{
		sql::SQLString stdErrors(e.what()); 
		OutErrorInfo = UTF8_TO_TCHAR(stdErrors.c_str());
		UE_LOG(LogLeafMySQL, Error, TEXT("LeafMySQL Error - %s"), *OutErrorInfo);
#if !UE_BUILD_SHIPPING
		check(0);
#endif
	}
	
	if (!linkInfo.IsConnectionValid())
	{
		const sql::SQLWarning* w = linkInfo.Connection->getWarnings();
		std::string tmpS = w->getMessage();
		OutErrorInfo = UTF8_TO_TCHAR(tmpS.c_str());
		return false;
	}
	OutLinkInfo = linkInfo;
	return true;
}

void LeafMySql::LeafCloseMySQLConnection(FLeafMySqlLinkInfo& InLinkInfo)
{
	InLinkInfo.Destory();
}

bool LeafMySql::IsLeafMySQLValid()
{
	FLeafMySQLModule& LeafMySQL = FModuleManager::Get().LoadModuleChecked<FLeafMySQLModule>(TEXT("LeafMySQL"));
	return LeafMySQL.IsMySQLDLLValid();
}

bool LeafMySql::DATABASE::LeafShowAllDataBase(FLeafMySqlLinkInfo& InLinkInfo, TArray<FString>& OutDataBaseNames)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("DatabaseMetaData FAILURE - database connection closed"));
		return false;
	}
	sql::DatabaseMetaData* dbcon_meta = InLinkInfo.GetMetaData();
	sql::ResultSet* schemas = dbcon_meta->getSchemas();
	size_t numSchemas = schemas->rowsCount();
	if (numSchemas > 0)
	{
		TArray<FString> outDataBaseNames;
		while (schemas->next())
		{
			sql::SQLString s = schemas->getString("TABLE_SCHEM");
			outDataBaseNames.Emplace(FString(UTF8_TO_TCHAR(s.c_str())));
		}
		OutDataBaseNames = outDataBaseNames;
	}
	return true;
}

bool LeafMySql::DATABASE::LeafSelectDataBaseFromName(const FLeafMySqlLinkInfo& InLinkInfo, const FString& InNewDataBaseName)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("DatabaseMetaData FAILURE - database connection closed"));
		return false;
	}

	if (InNewDataBaseName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("InNewDataBaseName is empty"));
		return false;
	}
	InLinkInfo.Connection->setSchema(TCHAR_TO_UTF8(*InNewDataBaseName));
	return true;
}

FString LeafMySql::DATABASE::LeafShowCurrentDataBaseAllTable(FLeafMySqlLinkInfo& InLinkInfo, TArray<FString>& OutDataTableNames)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("DatabaseMetaData FAILURE - database connection closed"));
		return TEXT("DatabaseMetaData FAILURE - database connection closed");
	}

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
	rs = InLinkInfo.GetStatement()->executeQuery("SHOW TABLES;");
	LEAFCATCHSQLEND()
	
	int count = rs->rowsCount();
	if (count <= 0)
	{
		sql::SQLString tmps = InLinkInfo.Connection->getSchema();
		FString s = UTF8_TO_TCHAR(tmps.c_str());
		UE_LOG(LogLeafMySQL, Error, TEXT("[%s] dont have any table"), *s);
		return FString::Printf(TEXT("[%s] dont have any table"), *s);
	}
	TArray<FString> outDataTableNames;
	while (rs->next())
	{
		sql::SQLString cs = rs->getString(1);
		outDataTableNames.Emplace(UTF8_TO_TCHAR(cs.c_str()));
	}

	OutDataTableNames = outDataTableNames;
	
	return LEAFRETURNERRORMESSAGE(error,rs);
}

FString LeafMySql::DATABASE::LeafCreateDataBase(Command::DataBase::FLeafMySqlDataBaseCreate& InCreateInfo)
{
	const FString s = InCreateInfo.ExeCommand();
	if (!s.IsEmpty() && !s.Equals(TEXT("Successed"), ESearchCase::IgnoreCase))
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("%s"), *s);
	}
	else
	{
		UE_LOG(LogLeafMySQL, Display, TEXT("%s"), *s);
	}
	return s;
}

FString LeafMySql::DATABASE::LeafUpdateDataBaseSettings(Command::DataBase::FLeafMySqlDataBaseUpdateSettings& InUpdateInfo)
{
	const FString s = InUpdateInfo.ExeCommand();
	if (!s.IsEmpty() && !s.Equals(TEXT("Successed"), ESearchCase::IgnoreCase))
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("%s"), *s);
	}
	else
	{
		UE_LOG(LogLeafMySQL, Display, TEXT("%s"), *s);
	}
	return s;
}

FString LeafMySql::DATABASE::LeafDeleteDataBase(Command::DataBase::FLeafMySqlDataBaseDelete& InDeleteInfo)
{
	const FString s = InDeleteInfo.ExeCommand();
	if (!s.IsEmpty() && !s.Equals(TEXT("Successed"), ESearchCase::IgnoreCase))
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("%s"), *s);
	}
	else
	{
		UE_LOG(LogLeafMySQL, Display, TEXT("%s"), *s);
	}
	return s;
}

FString LeafMySql::DATABASE::LeafShowDataBaseDetails(FLeafMySqlLinkInfo& InLinkInfo, TMap<FString, FString>& OutDataBaseDetail)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("DatabaseMetaData FAILURE - database connection closed"));
		return TEXT("DatabaseMetaData FAILURE - database connection closed");
	}
	sql::SQLString dataBaseName = InLinkInfo.Connection->getSchema();
	FString fDataBaseName = UTF8_TO_TCHAR(dataBaseName.c_str());
	FString command = FString::Printf(TEXT("SHOW CREATE DATABASE %s;"), *fDataBaseName);

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	int count = rs->rowsCount();
	if (count <= 0)
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("[%s] dont have any status info"), *fDataBaseName);
		return FString::Printf(TEXT("[%s] dont have any status info"), *fDataBaseName);
	}
	TMap<FString, FString> outDataBaseDetail;
	while (rs->next())
	{
		sql::SQLString cs = rs->getString(1);
		sql::SQLString cs2 = rs->getString(2);
		outDataBaseDetail.Add(UTF8_TO_TCHAR(cs.c_str()), UTF8_TO_TCHAR(cs2.c_str()));
	}
	OutDataBaseDetail = outDataBaseDetail;
	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATABASE::LeafGetDataBaseStatus(FLeafMySqlLinkInfo& InLinkInfo, TMap<FString, FString>& OutDataBaseStatus)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("DatabaseMetaData FAILURE - database connection closed"));
		return TEXT("DatabaseMetaData FAILURE - database connection closed");
	}
	
	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery("SHOW STATUS;");
	LEAFCATCHSQLEND()

	int count = rs->rowsCount();
	if (count <= 0)
	{
		sql::SQLString tmps = InLinkInfo.Connection->getSchema();
		FString s = UTF8_TO_TCHAR(tmps.c_str());
		UE_LOG(LogLeafMySQL, Error, TEXT("[%s] dont have any status info"), *s);
		return FString::Printf(TEXT("[%s] dont have any status info"), *s);
	}
	TMap<FString, FString> outDataBaseStatus;
	while (rs->next())
	{
		sql::SQLString cs = rs->getString(1);
		sql::SQLString cs2 = rs->getString(2);
		outDataBaseStatus.Add(UTF8_TO_TCHAR(cs.c_str()), UTF8_TO_TCHAR(cs2.c_str()));
	}
	OutDataBaseStatus = outDataBaseStatus;
	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATABASE::LeafGetDataBaseErrors(FLeafMySqlLinkInfo& InLinkInfo, TArray<FString>& OutDataTableError)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("DatabaseMetaData FAILURE - database connection closed"));
		return TEXT("DatabaseMetaData FAILURE - database connection closed");
	}

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery("SHOW ERRORS;");
	LEAFCATCHSQLEND()

	int count = rs->rowsCount();
	if (count <= 0)
	{
		sql::SQLString tmps = InLinkInfo.Connection->getSchema();
		FString s = UTF8_TO_TCHAR(tmps.c_str());
		UE_LOG(LogLeafMySQL, Warning, TEXT("[%s] dont have any errors"), *s);
		return FString::Printf(TEXT("[%s] dont have any errors"), *s);
	}
	TArray<FString> outDataTableError;
	while (rs->next())
	{
		sql::SQLString cs = rs->getString(1);
		outDataTableError.Emplace(UTF8_TO_TCHAR(cs.c_str()));
	}

	OutDataTableError = outDataTableError;

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATABASE::LeafGetDataBaseWarnings(FLeafMySqlLinkInfo& InLinkInfo, TArray<FString>& OutDataTableWarnings)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("DatabaseMetaData FAILURE - database connection closed"));
		return TEXT("DatabaseMetaData FAILURE - database connection closed");
	}

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery("SHOW WARNINGS;");
	LEAFCATCHSQLEND()

	int count = rs->rowsCount();
	if (count <= 0)
	{
		sql::SQLString tmps = InLinkInfo.Connection->getSchema();
		FString s = UTF8_TO_TCHAR(tmps.c_str());
		UE_LOG(LogLeafMySQL, Warning, TEXT("[%s] dont have any warnings"), *s);
		return FString::Printf(TEXT("[%s] dont have any warnings"), *s);
	}
	TArray<FString> outDataTableWarnings;
	while (rs->next())
	{
		sql::SQLString cs = rs->getString(1);
		outDataTableWarnings.Emplace(UTF8_TO_TCHAR(cs.c_str()));
	}

	OutDataTableWarnings = outDataTableWarnings;

	return LEAFRETURNERRORMESSAGE(error, rs);
}

bool LeafMySql::LeafShowSystemDetail(FLeafMySqlLinkInfo& InLinkInfo, TMap<FString, FString>& OutSystemDetail)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("DatabaseMetaData FAILURE - database connection closed"));
		return false;
	}
	sql::DatabaseMetaData* dbcon_meta = InLinkInfo.GetMetaData();
	TMap<FString, FString> outSystemDetail;
	auto GetInfoFromMetaData = [&](const FString& inKey,const sql::SQLString& inInfo)
	{
		FString v = FString(UTF8_TO_TCHAR(inInfo.c_str()));
		outSystemDetail.Add(inKey, v);
	};
	auto GetBoolInfoFromMetaData = [&](const FString& inKey, const bool inInfo)
	{
		FString v = inInfo ? TEXT("True") : TEXT("False");
		outSystemDetail.Add(inKey, v);
	};
	auto GetIntInfoFromMetaData = [&](const FString& inKey, const int64 inInfo)
	{
		FString v = FString::FromInt(inInfo);
		outSystemDetail.Add(inKey, v);
	};
	GetInfoFromMetaData(TEXT("Database Product Name"), dbcon_meta->getDatabaseProductName());
	GetInfoFromMetaData(TEXT("Database Product Version"), dbcon_meta->getDatabaseProductVersion());
	GetInfoFromMetaData(TEXT("Database User Name"), dbcon_meta->getUserName());
	GetInfoFromMetaData(TEXT("Driver name"), dbcon_meta->getDriverName());
	GetInfoFromMetaData(TEXT("Driver version"), dbcon_meta->getDriverVersion());
	GetBoolInfoFromMetaData(TEXT("Database in Read-Only Mode"), dbcon_meta->isReadOnly());
	GetBoolInfoFromMetaData(TEXT("Supports Transactions"), dbcon_meta->supportsTransactions());
	GetBoolInfoFromMetaData(TEXT("Supports DML Transactions only"), dbcon_meta->supportsDataManipulationTransactionsOnly());
	GetBoolInfoFromMetaData(TEXT("Supports Batch Updates"), dbcon_meta->supportsBatchUpdates());
	GetBoolInfoFromMetaData(TEXT("Supports Outer Joins"), dbcon_meta->supportsOuterJoins());
	GetBoolInfoFromMetaData(TEXT("Supports Multiple Transactions"), dbcon_meta->supportsMultipleTransactions());
	GetBoolInfoFromMetaData(TEXT("Supports Named Parameters"), dbcon_meta->supportsNamedParameters());
	GetBoolInfoFromMetaData(TEXT("Supports Statement Pooling"), dbcon_meta->supportsStatementPooling());
	GetBoolInfoFromMetaData(TEXT("Supports Stored Procedures"), dbcon_meta->supportsStoredProcedures());
	GetBoolInfoFromMetaData(TEXT("Supports Union"), dbcon_meta->supportsUnion());
	GetIntInfoFromMetaData(TEXT("Maximum Connections"), dbcon_meta->getMaxConnections());
	GetIntInfoFromMetaData(TEXT("Maximum Columns per Table"), dbcon_meta->getMaxColumnsInTable());
	GetIntInfoFromMetaData(TEXT("Maximum Columns per Index"), dbcon_meta->getMaxColumnsInIndex());
	GetIntInfoFromMetaData(TEXT("Maximum Row Size per Table"), dbcon_meta->getMaxRowSize());
	OutSystemDetail = outSystemDetail;
	return true;
}

FString LeafMySql::LeafExecuteCustomSQLCommand(FLeafMySqlLinkInfo& InLinkInfo, const FString& InSQLCommand, TArray<TMap<FString, FString>>& OutResults)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return TEXT("MySQL connection closed");
	}
	FString command = InSQLCommand;
	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()
	int32 resultRowNum = rs->rowsCount();
	int32 resultColumnNum = rs->getMetaData()->getColumnCount();

	if (resultRowNum > 0 && resultColumnNum)
	{
		TMap<FString, FString> preRowDataRaw;
		for (int32 i = 0; i < resultColumnNum; i++)
		{
			preRowDataRaw.Add(UTF8_TO_TCHAR(rs->getMetaData()->getColumnLabel(i + 1).c_str()), TEXT(""));
		}
		while (rs->next())
		{
			TMap<FString, FString> preRowData;
			for (auto tmp : preRowDataRaw)
			{

				preRowData.Add(tmp.Key, UTF8_TO_TCHAR(rs->getString(TCHAR_TO_UTF8(*tmp.Key)).c_str()));
			}
			OutResults.Add(preRowData);
		}
	}
	
	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATATABLE::LeafCreateDataTable(Command::DataTable::FLeafMySqlDataTableCreate& InCreateInfo)
{
	const FString s = InCreateInfo.ExeCommand();
	if (!s.IsEmpty() && !s.Equals(TEXT("Successed"), ESearchCase::IgnoreCase))
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("%s"),*s);
	}
	else
	{
		UE_LOG(LogLeafMySQL, Display, TEXT("%s"), *s);
	}
	return s;
}

FString LeafMySql::DATATABLE::LeafDeleteDataTable(Command::DataTable::FLeafMySqlDataTableDelete& InDaleteInfo)
{
	const FString s = InDaleteInfo.ExeCommand();
	if (!s.IsEmpty() && !s.Equals(TEXT("Successed"), ESearchCase::IgnoreCase))
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("%s"), *s);
	}
	else
	{
		UE_LOG(LogLeafMySQL, Display, TEXT("%s"), *s);
	}
	return s;
}

FString LeafMySql::DATATABLE::LeafModifyDataTableStructure_AddColumn(Command::DataTable::FLeafMySqlDataTableModify_AddColumn& InAddColumnInfo)
{
	const FString s = InAddColumnInfo.ExeCommand();
	if (!s.IsEmpty() && !s.Equals(TEXT("Successed"), ESearchCase::IgnoreCase))
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("%s"), *s);
	}
	else
	{
		UE_LOG(LogLeafMySQL, Display, TEXT("%s"), *s);
	}
	return s;
}

FString LeafMySql::DATATABLE::LeafModifyDataTableStructure_RemoveColumn(Command::DataTable::FLeafMySqlDataTableModify_RemoveColumn& InRemoveColumnInfo)
{
	const FString s = InRemoveColumnInfo.ExeCommand();
	if (!s.IsEmpty() && !s.Equals(TEXT("Successed"), ESearchCase::IgnoreCase))
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("%s"), *s);
	}
	else
	{
		UE_LOG(LogLeafMySQL, Display, TEXT("%s"), *s);
	}
	return s;
}

FString LeafMySql::DATATABLE::LeafModifyDataTableStructure_ModifyColumn(Command::DataTable::FLeafMySqlDataTableModify_ModifyColumn& InModifyColumnInfo)
{
	const FString s = InModifyColumnInfo.ExeCommand();
	if (!s.IsEmpty() && !s.Equals(TEXT("Successed"), ESearchCase::IgnoreCase))
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("%s"), *s);
	}
	else
	{
		UE_LOG(LogLeafMySQL, Display, TEXT("%s"), *s);
	}
	return s;
}

FString LeafMySql::DATATABLE::LeafModifyDataTableStructure_RenameColumn(Command::DataTable::FLeafMySqlDataTableModify_RenameColumn& InRenameColumnInfo)
{
	const FString s = InRenameColumnInfo.ExeCommand();
	if (!s.IsEmpty() && !s.Equals(TEXT("Successed"), ESearchCase::IgnoreCase))
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("%s"), *s);
	}
	else
	{
		UE_LOG(LogLeafMySQL, Display, TEXT("%s"), *s);
	}
	return s;
}

FString LeafMySql::DATATABLE::LeafShowDataTableStructure(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, TMap<FString, Command::DataTable::FLeafMySqlDataTableParamDisplay>& OutDataTableStructure)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return TEXT("MySQL connection closed");
	}

	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return TEXT("MySQL : No database selected!");
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return TEXT("TableName is empty!");
	}

	FString command = FString::Printf(TEXT("DESC %s;"), *InTableName);

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	int count = rs->rowsCount();
	if (count <= 0)
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("[%s] dont have any sructure infos!"), *s);
		return FString::Printf(TEXT("[%s] dont have any sructure infos!"), *s);
	}
	TMap<FString, Command::DataTable::FLeafMySqlDataTableParamDisplay> outDataTableStructure;
	while (rs->next())
	{
		Command::DataTable::FLeafMySqlDataTableParamDisplay DTPD(rs);
		outDataTableStructure.Add(DTPD.ParamName, DTPD);
	}
	OutDataTableStructure = outDataTableStructure;
	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATATABLE::LeafShowDataTableSQLCreateCommand(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT(""));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT(""));
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return FString(TEXT(""));
	}

	FString command = FString::Printf(TEXT("SHOW CREATE TABLE %s;"), *InTableName);

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	FString outStr;
	while (rs->next())
	{
		outStr = UTF8_TO_TCHAR(rs->getString(2).c_str());
	}
	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATATABLE::LeafShowTableAllData(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, TArray<TMap<FString, FString>>& OutDatas)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	FString command = FString::Printf(TEXT("SELECT * FROM %s;"), *InTableName);

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	FString outStr;
	TMap<FString, FString> preRowDataRaw;
	for (uint32 i = 0; i < rs->getMetaData()->getColumnCount(); i++)
	{
		preRowDataRaw.Add(UTF8_TO_TCHAR(rs->getMetaData()->getColumnLabel(i + 1).c_str()), TEXT(""));
	}
	while (rs->next())
	{
		TMap<FString, FString> preRowData;
		for (auto tmp : preRowDataRaw)
		{
			
			preRowData.Add(tmp.Key,UTF8_TO_TCHAR(rs->getString(TCHAR_TO_UTF8(*tmp.Key)).c_str()));
		}
		OutDatas.Add(preRowData);
	}
	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATATABLE::LeafShowTableColumnAllData(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, const FString& InColumnName, TArray<FString>& OutDatas)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	if (InColumnName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableColumnName is empty!"));
		return FString(TEXT("TableColumnName is empty!"));
	}

	FString command = FString::Printf(TEXT("SELECT * FROM %s;"), *InTableName);

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	FString outStr;

	while (rs->next())
	{

		OutDatas.Add(UTF8_TO_TCHAR(rs->getString(TCHAR_TO_UTF8(*InColumnName)).c_str()));
	}
	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATATABLE::LeafShowTableIndex(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, TMap<FString, FString>& OutInfos)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	FString command = FString::Printf(TEXT("SHOW INDEX FROM %s;"), *InTableName);

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	FString outStr;
	TMap<FString, FString> outInfos;
	for (uint32 i = 0; i < rs->getMetaData()->getColumnCount(); i++)
	{
		outInfos.Add(UTF8_TO_TCHAR(rs->getMetaData()->getColumnLabel(i + 1).c_str()), TEXT(""));
	}
	TArray<FString> outInfosKey;
	outInfos.GenerateKeyArray(outInfosKey);
	while (rs->next())
	{

		for (auto tmp : outInfosKey)
		{
			FString v = UTF8_TO_TCHAR(rs->getString(TCHAR_TO_UTF8(*tmp)).c_str());
			if (v.IsEmpty())
			{
				v = TEXT("NULL");
			}
			outInfos.Add(tmp, v);
		}
	}
	OutInfos = outInfos;
	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATATABLE::LeafCreateTableIndex(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, const FString& InIndexName, const TArray<FString>& InColumnInfos)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	if (InIndexName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("IndexName is empty!"));
		return FString(TEXT("IndexName is empty!"));
	}

	if (InColumnInfos.Num() <= 0)
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("ColumnInfos is empty!"));
		return FString(TEXT("ColumnInfos is empty!"));
	}

	FString columnInfos;
	if (InColumnInfos.Num() <= 1)
	{
		columnInfos = FString::Printf(TEXT("(%s)"), *InColumnInfos[0]);
	}
	else
	{
		columnInfos += TEXT("(");
		for (const auto tmp : InColumnInfos)
		{
			columnInfos += FString::Printf(TEXT("%s,"), *tmp);
		}
		columnInfos = columnInfos.Left(columnInfos.Len() - 1);
		columnInfos += TEXT(")");
	}

	FString command = FString::Printf(TEXT("CREATE INDEX %s ON %s %s;"), *InIndexName,*InTableName,*columnInfos);

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATATABLE::LeafDeleteTableIndex(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, const FString& InIndexName)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	if (InIndexName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("IndexName is empty!"));
		return FString(TEXT("IndexName is empty!"));
	}

	FString command = FString::Printf(TEXT("DROP INDEX %s ON %s;"), *InIndexName, *InTableName);

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATATABLE::LeafShowDataTableStatus(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, TMap<FString, FString>& OutInfos)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	FString command = FString::Printf(TEXT("SHOW TABLE STATUS LIKE %s;"), *FLeafMySqlValueHandler(InTableName).Get());

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	FString outStr;
	TMap<FString, FString> outInfos;
	for (uint32 i = 0; i < rs->getMetaData()->getColumnCount(); i++)
	{
		outInfos.Add(UTF8_TO_TCHAR(rs->getMetaData()->getColumnLabel(i + 1).c_str()), TEXT(""));
	}
	TArray<FString> outInfosKey;
	outInfos.GenerateKeyArray(outInfosKey);
	while (rs->next())
	{

		for (auto tmp : outInfosKey)
		{
			FString v = UTF8_TO_TCHAR(rs->getString(TCHAR_TO_UTF8(*tmp)).c_str());
			if (v.IsEmpty())
			{
				v = TEXT("NULL");
			}
			outInfos.Add(tmp, v);
		}
	}
	OutInfos = outInfos;
	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATA::LeafInsertDataToTable(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, const TMap<FString, FString>& InInsertDatas)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	if (InInsertDatas.Num() <= 0)
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("InInsertDatas is empty!"));
		return FString(TEXT("InInsertDatas is empty!"));
	}

	FString params, values;
	if (InInsertDatas.Num() <= 1)
	{
		TArray<FString> keys;
		InInsertDatas.GenerateKeyArray(keys);
		params = keys[0];

		values = InInsertDatas.FindRef(keys[0]);
		values = FLeafMySqlValueHandler(values).Get();
	}
	else
	{
		for (const auto tmp : InInsertDatas)
		{
			params += tmp.Key;
			params += TEXT(",");

			/*values += TEXT("'");
			values += tmp.Value;
			values += TEXT("',");*/
			values += FLeafMySqlValueHandler(tmp.Value).Get();
			values += TEXT(",");
		}

		params = params.Left(params.Len() - 1);
		values = values.Left(values.Len() - 1);
	}

	FString command = FString::Printf(TEXT("INSERT INTO %s (%s) VALUES (%s);"), *InTableName, *params, *values);

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()
	
	
	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATA::LeafInsertDataToTable(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, const FString& InInsertColumnNames, const FString& InInsertDatas)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	if (InInsertDatas.IsEmpty() || InInsertColumnNames.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("InInsertDatas or InInsertColumnNames is empty!"));
		return FString(TEXT("InInsertDatas or InInsertColumnNames is empty!"));
	}

	TArray<FString> paramArray;
	InInsertColumnNames.ParseIntoArray(paramArray, TEXT(","));
	TArray<FString> valueArray;
	InInsertDatas.ParseIntoArray(valueArray, TEXT(","));

	if ((paramArray.Num() <= 0 && valueArray.Num() <= 0) && paramArray.Num() == valueArray.Num())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("InInsertDatas haven't valid data or column num != value num!"));
		return FString(TEXT("InInsertDatas haven't valid data or column num != value num!"));
	}

	FString params, values;
	for (int32 i = 0; i< paramArray.Num(); i++)
	{
		params += paramArray[i];
		params += TEXT(",");

		/*values += TEXT("'");
		values += valueArray[i];
		values += TEXT("',");*/
		values += FLeafMySqlValueHandler(valueArray[i]).Get();
		values += TEXT(",");
	}
	params = params.Left(params.Len() - 1);
	values = values.Left(values.Len() - 1);

	FString command = FString::Printf(TEXT("INSERT INTO %s (%s) VALUES (%s);"), *InTableName, *params, *values);

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATA::LeafInsertDataToTable_Trunk(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, const FString& InInsertColumnNames, const TArray<FString>& InInsertDatas)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	if (InInsertDatas.Num() <= 0 || InInsertColumnNames.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("InInsertDatas or InInsertColumnNames is empty!"));
		return FString(TEXT("InInsertDatas or InInsertColumnNames is empty!"));
	}

	FString params, values;
	TArray<FString> paramArray;
	InInsertColumnNames.ParseIntoArray(paramArray, TEXT(","));
	for (int32 i = 0; i < InInsertDatas.Num(); i++)
	{
		if (InInsertDatas[i].IsEmpty())
		{
			break;
		}
		FString tmpValues = TEXT("(");
		TArray<FString> valueArray;
		
		FString tmpStr = InInsertDatas[i];
		tmpStr = tmpStr.Replace(TEXT("\n"), TEXT(""));
		tmpStr = tmpStr.Replace(TEXT("\r"), TEXT(""));
		tmpStr.ParseIntoArray(valueArray, TEXT(","));
		//InInsertDatas[i].ParseIntoArray(valueArray, TEXT(","));

		if ((paramArray.Num() <= 0 && valueArray.Num() <= 0) && paramArray.Num() == valueArray.Num())
		{
			UE_LOG(LogLeafMySQL, Error, TEXT("InInsertDatas[%d] haven't valid data or column num != value num!"),i);
			return FString::Printf(TEXT("InInsertDatas[%d] haven't valid data or column num != value num!"),i);
		}

		for (int32 k = 0; k < valueArray.Num(); k++)
		{
			tmpValues += FLeafMySqlValueHandler(valueArray[k]).Get();
			tmpValues += TEXT(",");
		}
		tmpValues = tmpValues.Left(tmpValues.Len() - 1);
		tmpValues += TEXT("),");
		values += tmpValues;
	}
	values = values.Left(values.Len() - 1);

	
	for (int32 i = 0; i < paramArray.Num(); i++)
	{
		params += paramArray[i];
		params += TEXT(",");
	}
	params = params.Left(params.Len() - 1);

	FString command = FString::Printf(TEXT("INSERT INTO %s (%s) VALUES %s;"), *InTableName, *params, *values);

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATA::LeafFindDataFromTable(FLeafMySqlLinkInfo& InLinkInfo,
	const FString& InTableName, const TArray<FString>& InSelectColumns,
	TArray<TMap<FString, FString>>& OutSelectedDatas, const FString& InWhereCondition, 
	const FString& InOrderColumnName, const bool bInIsASC, const int64 InLimitRowNum)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	/*if (InSelectColumns.Num() <= 0)
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("InSelectColumns is empty!"));
		return FString(TEXT("InSelectColumns is empty!"));
	}*/

	FString columnNames;
	if (InSelectColumns.Num() <= 0)
	{
		columnNames = TEXT("*");
	}
	else if (InSelectColumns.Num() == 1)
	{
		columnNames = InSelectColumns[0];
	}
	else
	{
		for (const auto tmp : InSelectColumns)
		{
			columnNames += tmp;
			columnNames += TEXT(",");
		}

		columnNames = columnNames.Left(columnNames.Len() - 1);
	}

	FString command = FString::Printf(TEXT("SELECT %s FROM %s"), *columnNames, *InTableName);

	if (!(InWhereCondition.Equals(TEXT("None"),ESearchCase::IgnoreCase) || InWhereCondition.IsEmpty()))
	{
		command += FString::Printf(TEXT(" WHERE %s"), *InWhereCondition);
	}

	if (!(InOrderColumnName.Equals(TEXT("None"), ESearchCase::IgnoreCase) || InOrderColumnName.IsEmpty()))
	{
		command += FString::Printf(TEXT(" ORDER BY %s %s"), *InOrderColumnName, bInIsASC ? *FString(TEXT("ASC")) : *FString(TEXT("DESC")));
	}

	if (InLimitRowNum > 0)
	{
		command += FString::Printf(TEXT(" LIMIT %ld"), InLimitRowNum);
	}

	command += TEXT(";");

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()
	if (rs == NULL)
	{
		return FString::Printf(TEXT("Where command is not valid condition. Current condition is [%s]"), *InWhereCondition);
	}
	TMap<FString, FString> preRowDataRaw;
	for (uint32 i = 0; i < rs->getMetaData()->getColumnCount(); i++)
	{
		preRowDataRaw.Add(UTF8_TO_TCHAR(rs->getMetaData()->getColumnLabel(i + 1).c_str()), TEXT(""));
	}
	
	while (rs->next())
	{
		TMap<FString, FString> preRowData;
		for (auto tmp : preRowDataRaw)
		{

			preRowData.Add(tmp.Key, UTF8_TO_TCHAR(rs->getString(TCHAR_TO_UTF8(*tmp.Key)).c_str()));
		}
		OutSelectedDatas.Add(preRowData);
	}

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATA::LeafFindDataFromTable(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, const FString& InSelectColumns, TArray<TMap<FString, FString>>& OutSelectedDatas, const FString& InWhereCondition, const FString& InOrderColumnName, const bool bInIsASC, const int64 InLimitRowNum)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	FString columnNames;
	if (InSelectColumns.IsEmpty())
	{
		columnNames = TEXT("*");
	}
	else
	{
		columnNames = InSelectColumns;
	}

	FString command = FString::Printf(TEXT("SELECT %s FROM %s"), *columnNames, *InTableName);

	if (!(InWhereCondition.Equals(TEXT("None"), ESearchCase::IgnoreCase) || InWhereCondition.IsEmpty()))
	{
		command += FString::Printf(TEXT(" WHERE %s"), *InWhereCondition);
	}

	if (!(InOrderColumnName.Equals(TEXT("None"), ESearchCase::IgnoreCase) || InOrderColumnName.IsEmpty()))
	{
		command += FString::Printf(TEXT(" ORDER BY %s %s"), *InOrderColumnName, bInIsASC ? *FString(TEXT("ASC")) : *FString(TEXT("DESC")));
	}

	if (InLimitRowNum > 0)
	{
		command += FString::Printf(TEXT(" LIMIT %ld"), InLimitRowNum);
	}

	command += TEXT(";");

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()
	if (rs == NULL)
	{
		return FString::Printf(TEXT("Where command is not valid condition. Current condition is [%s]"), *InWhereCondition);
	}
	TMap<FString, FString> preRowDataRaw;
	for (uint32 i = 0; i < rs->getMetaData()->getColumnCount(); i++)
	{
		preRowDataRaw.Add(UTF8_TO_TCHAR(rs->getMetaData()->getColumnLabel(i + 1).c_str()), TEXT(""));
	}

	while (rs->next())
	{
		TMap<FString, FString> preRowData;
		for (auto tmp : preRowDataRaw)
		{

			preRowData.Add(tmp.Key, UTF8_TO_TCHAR(rs->getString(TCHAR_TO_UTF8(*tmp.Key)).c_str()));
		}
		OutSelectedDatas.Add(preRowData);
	}

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATA::LeafFindDataFromTable(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName,
	const FString& InSelectColumns, TArray<TArray<FString>>& OutSelectedDatas,
	const FString& InWhereCondition, const FString& InOrderColumnName,
	const bool bInIsASC, const int64 InLimitRowNum)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	FString columnNames;
	if (InSelectColumns.IsEmpty())
	{
		columnNames = TEXT("*");
	}
	else
	{
		columnNames = InSelectColumns;
	}

	FString command = FString::Printf(TEXT("SELECT %s FROM %s"), *columnNames, *InTableName);

	if (!(InWhereCondition.Equals(TEXT("None"), ESearchCase::IgnoreCase) || InWhereCondition.IsEmpty()))
	{
		command += FString::Printf(TEXT(" WHERE %s"), *InWhereCondition);
	}

	if (!(InOrderColumnName.Equals(TEXT("None"), ESearchCase::IgnoreCase) || InOrderColumnName.IsEmpty()))
	{
		command += FString::Printf(TEXT(" ORDER BY %s %s"), *InOrderColumnName, bInIsASC ? *FString(TEXT("ASC")) : *FString(TEXT("DESC")));
	}

	if (InLimitRowNum > 0)
	{
		command += FString::Printf(TEXT(" LIMIT %ld"), InLimitRowNum);
	}

	command += TEXT(";");

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()
	if (rs == NULL)
	{
		return FString::Printf(TEXT("Where command is not valid condition. Current condition is [%s]"), *InWhereCondition);
	}
	TArray<FString> preRowDataRaw;
	preRowDataRaw.AddDefaulted(rs->getMetaData()->getColumnCount());

	while (rs->next())
	{
		for (int32 i = 0; i < preRowDataRaw.Num(); ++i)
		{

			preRowDataRaw[i] = UTF8_TO_TCHAR(rs->getString(i + 1).c_str());
		}
		OutSelectedDatas.Emplace(preRowDataRaw);
	}

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATA::LeafUpdateDataFromTable(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, 
	const TMap<FString, FString>& InUpdateDatas, const FString& InWhereCondition)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	if (InUpdateDatas.Num() <= 0)
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("InUpdateDatas is empty!"));
		return FString(TEXT("InUpdateDatas is empty!"));
	}

	if (InWhereCondition.Equals(TEXT("None"), ESearchCase::IgnoreCase) || InWhereCondition.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("WhereCondition is empty!"));
		return FString(TEXT("WhereCondition is empty!"));
	}

	FString updateInfos;
	for (const auto tmp : InUpdateDatas)
	{
		//updateInfos += FString::Printf(TEXT("%s = '%s'"), *tmp.Key, *tmp.Value);
		updateInfos += FString::Printf(TEXT("%s = (%s)"), *tmp.Key, *FLeafMySqlValueHandler(tmp.Value).Get(true));
		updateInfos += TEXT(",");
	}

	updateInfos = updateInfos.Left(updateInfos.Len() - 1);

	FString command = FString::Printf(TEXT("UPDATE %s SET %s WHERE %s;"), *InTableName, *updateInfos, *InWhereCondition);

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATA::LeafUpdateDataFromTable(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, const FString& InUpdateDatas, const FString& InWhereCondition)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	if (InUpdateDatas.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("InUpdateDatas is empty!"));
		return FString(TEXT("InUpdateDatas is empty!"));
	}

	if (InWhereCondition.Equals(TEXT("None"), ESearchCase::IgnoreCase) || InWhereCondition.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("WhereCondition is empty!"));
		return FString(TEXT("WhereCondition is empty!"));
	}

	TArray<FString> updateInfoArray_Unhandle;
	TArray<FString> updateInfoArray;
	InUpdateDatas.ParseIntoArray(updateInfoArray_Unhandle, TEXT(","));
	if (updateInfoArray_Unhandle.Num() <= 0)
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("InUpdateDatas have not valid data!"));
		return FString(TEXT("InUpdateDatas have not valid data!"));
	}
	for (int32 i = 0; i < updateInfoArray_Unhandle.Num(); i++)
	{
		FString tmp, tmpColumnName;
		int32 locStr;
		locStr = updateInfoArray_Unhandle[i].Find(TEXT("="));
		if (locStr == -1)
		{
			continue;
		}
		tmp = updateInfoArray_Unhandle[i].Right(updateInfoArray_Unhandle[i].Len() - locStr - 1);
		tmpColumnName = updateInfoArray_Unhandle[i].Left(locStr);
		updateInfoArray.Emplace(FString::Printf(TEXT("%s=(%s)"), *tmpColumnName, *FLeafMySqlValueHandler(tmp).Get(true)));
	}
	FString updateInfos;
	for (const auto tmp : updateInfoArray)
	{
		updateInfos += tmp;
		updateInfos += TEXT(",");
	}

	updateInfos = updateInfos.Left(updateInfos.Len() - 1);

	FString command = FString::Printf(TEXT("UPDATE %s SET %s WHERE %s;"), *InTableName, *updateInfos, *InWhereCondition);

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::DATA::LeafDeleteDataFromTable(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName, const FString& InWhereCondition)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	if (InWhereCondition.Equals(TEXT("None"), ESearchCase::IgnoreCase) || InWhereCondition.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("WhereCondition is empty!"));
		return FString(TEXT("WhereCondition is empty!"));
	}

	FString command = FString::Printf(TEXT("DELETE FROM %s WHERE %s;"), *InTableName, *InWhereCondition);

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::EVENT::LeafSetEventCommitAuto(FLeafMySqlLinkInfo& InLinkInfo, const bool InNewAuto)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	InLinkInfo.Connection->setAutoCommit(InNewAuto);
	return TEXT("Sucessed");
}

bool LeafMySql::EVENT::LeafGetEventCommitAuto(const FLeafMySqlLinkInfo& InLinkInfo)
{
	return InLinkInfo.Connection->getAutoCommit();
}

FString LeafMySql::EVENT::LeafBeginEvent(FLeafMySqlLinkInfo& InLinkInfo)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	FString command = FString::Printf(TEXT("BEGIN;"));

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::EVENT::LeafCommitEvent(FLeafMySqlLinkInfo& InLinkInfo)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	FString command = FString::Printf(TEXT("COMMIT;"));

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::EVENT::LeafAddSavePoint(FLeafMySqlLinkInfo& InLinkInfo, const FString& InNewSavePointName)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	FString command = FString::Printf(TEXT("SAVEPOINT %s;"), *InNewSavePointName);

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::EVENT::LeafRollBackEvent(FLeafMySqlLinkInfo& InLinkInfo, const FString& InSavePointName)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	FString command;
	if (InSavePointName.IsEmpty())
	{
		command = FString::Printf(TEXT("ROLLBACK;"));
	}
	else
	{
		command = FString::Printf(TEXT("ROLLBACK TO SAVEPOINT %s;"), *InSavePointName);
	}
	
	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::EVENT::LeafShowCurrentEventStatus(FLeafMySqlLinkInfo& InLinkInfo, TArray<TMap<FString,FString>>& OutInfos)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	FString command = FString::Printf(TEXT("SHOW ENGINE INNODB STATUS;"));

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()
	int32 rowCount = rs->rowsCount();
	const FString column1 = UTF8_TO_TCHAR(rs->getMetaData()->getColumnLabel(1).c_str());
	const FString column2 = UTF8_TO_TCHAR(rs->getMetaData()->getColumnLabel(2).c_str());
	const FString column3 = UTF8_TO_TCHAR(rs->getMetaData()->getColumnLabel(3).c_str());
	if (rowCount > 1)
	{
		TMap<FString, FString> infoPreRow;
		infoPreRow.Add(column1, UTF8_TO_TCHAR(rs->getString(1).c_str()));
		infoPreRow.Add(column1, UTF8_TO_TCHAR(rs->getString(2).c_str()));
		infoPreRow.Add(column1, UTF8_TO_TCHAR(rs->getString(3).c_str()));
		OutInfos.Add(infoPreRow);
	}
	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::EVENT::LeafLockTable_Write(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	FString command;

	command = FString::Printf(TEXT("LOCK TABLES %s WRITE;"), *InTableName);

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::EVENT::LeafLockTable_Read(FLeafMySqlLinkInfo& InLinkInfo, const FString& InTableName)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	FString command;

	command = FString::Printf(TEXT("LOCK TABLES %s READ;"), *InTableName);

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::EVENT::LeafUnLockTable(FLeafMySqlLinkInfo& InLinkInfo)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	FString command;

	command = FString::Printf(TEXT("UNLOCK TABLES;"));

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::EVENT::LeafSetTransactionIsolationLevel(FLeafMySqlLinkInfo& InLinkInfo, const LeafMySql::ELeafMySqlTransactionIsolationLevelType::Type InLevel)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InLevel == LeafMySql::ELeafMySqlTransactionIsolationLevelType::None)
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("In transaction isolation level type is not valid, current:None!"));
		return FString(TEXT("In transaction isolation level type is not valid, current:None!"));
	}

	FString command;

	command = FString::Printf(TEXT("SET TRANSACTION ISOLATION LEVEL %s;"), *LeafMySql::ELeafMySqlTransactionIsolationLevelType::GetLeafMySqlTransactionIsolationLevelTypeeString(InLevel));

	sql::ResultSet* rs = NULL;
	LEAFCATCHSQLBEGIN()
		rs = InLinkInfo.GetStatement()->executeQuery(TCHAR_TO_UTF8(*command));
	LEAFCATCHSQLEND()

	return LEAFRETURNERRORMESSAGE(error, rs);
}

FString LeafMySql::OTHER::LeafImportDataToDataTableFromCSV(FLeafMySqlLinkInfo& InLinkInfo, const FString& InFilePath, const FString& InTableName, const bool InOverrideSourceData)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (!IFileManager::Get().FileExists(*InFilePath))
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("ImportDataToDataTableFromCSV - The file '%s' doesn't exist."), *InFilePath);
		return FString::Printf(TEXT("The file '%s' doesn't exist."), *InFilePath);
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("ImportDataToDataTableFromCSV - TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	FString FilePath = IFileManager::Get().ConvertToRelativePath(*InFilePath);
	FString Data;
	if (FFileHelper::LoadFileToString(Data, *FilePath, FFileHelper::EHashOptions::None, FILEREAD_AllowWrite | FILEWRITE_AllowRead))
	{
		if (Data.IsEmpty())
		{
			return FString(TEXT("ImportDataToDataTableFromCSV - CSV file content is empty!"));
		}

		TArray<FString> DataPreRows;
		Data.ParseIntoArray(DataPreRows, TEXT("\n"));
		if (DataPreRows.Num() <= 1)
		{
			UE_LOG(LogLeafMySQL, Error, TEXT("ImportDataToDataTableFromCSV - CSV file content is empty,just have table head(1 row)!"));
			return FString(TEXT("CSV file content is empty,just have table head(1 row)!"));
		}

		FString tableHead_Unhandle = DataPreRows[0];
		TArray<FString> tableHeadArray_Unhandle;
		tableHead_Unhandle.ParseIntoArray(tableHeadArray_Unhandle, TEXT(","));
		if (tableHeadArray_Unhandle.Num() <= 1)
		{
			UE_LOG(LogLeafMySQL, Error, TEXT("ImportDataToDataTableFromCSV - CSV file table head is not valid!"));
			return FString(TEXT("CSV file table head is not valid!"));
		}
		FString tableHead;
		for (const auto tmp : tableHeadArray_Unhandle)
		{
			tableHead += FLeafMySqlValueHandler(tmp).GetSource();
			tableHead += TEXT(",");
		}
		tableHead = tableHead.Left(tableHead.Len() - 1);

		if (InOverrideSourceData)
		{
			Command::DataTable::FLeafMySqlDataTableDelete daleteInfo(InLinkInfo, InTableName, true, true);
			FString resStr = DATATABLE::LeafDeleteDataTable(daleteInfo);
			if (!(resStr.Equals("Successed") || resStr.IsEmpty()))
			{
				UE_LOG(LogLeafMySQL, Error, TEXT("ImportDataToDataTableFromCSV - Clear dataTable[%s] failed."), *InTableName);
				return resStr;
			}
		}

		TArray<FString> errorArrs;
		for (int32 i = 1; i < DataPreRows.Num(); i++)
		{
			FString resStr = DATA::LeafInsertDataToTable(InLinkInfo, InTableName, tableHead, DataPreRows[i]);
			if (!(resStr.Equals("Successed") || resStr.IsEmpty()))
			{
				errorArrs.Emplace(resStr);
			}
		}
		FString resFullStr;
		if (errorArrs.Num() > 0)
		{
			for (const auto tmp : errorArrs)
			{
				resFullStr += tmp;
				resFullStr += TEXT("\n");
			}
		}
		return resFullStr.IsEmpty() ? FString(TEXT("Successed")) : resFullStr;
	}
	UE_LOG(LogLeafMySQL, Error, TEXT("ImportDataToDataTableFromCSV - CSV data read failed! If you opened [%s]CSV file,please close."), *InFilePath);
	return FString::Printf(TEXT("CSV data read failed! If you opened [%s]CSV file,please close."), *InFilePath);
}

FString LeafMySql::OTHER::LeafImportDataToDataTableFromCSV_Trunk(FLeafMySqlLinkInfo& InLinkInfo, const FString& InFilePath, const FString& InTableName, const bool InOverrideSourceData, const int32 InTrunkSize)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (!IFileManager::Get().FileExists(*InFilePath))
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("ImportDataToDataTableFromCSV_Trunk - The file '%s' doesn't exist."), *InFilePath);
		return FString::Printf(TEXT("The file '%s' doesn't exist."), *InFilePath);
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("ImportDataToDataTableFromCSV_Trunk - TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	FString FilePath = IFileManager::Get().ConvertToRelativePath(*InFilePath);
	FString Data;
	if (FFileHelper::LoadFileToString(Data, *FilePath, FFileHelper::EHashOptions::None, FILEREAD_AllowWrite | FILEWRITE_AllowRead))
	{
		if (Data.IsEmpty())
		{
			return FString(TEXT("ImportDataToDataTableFromCSV_Trunk - CSV file content is empty!"));
		}

		TArray<FString> DataPreRows;
		Data.ParseIntoArray(DataPreRows, TEXT("\n"));
		if (DataPreRows.Num() <= 1)
		{
			UE_LOG(LogLeafMySQL, Error, TEXT("ImportDataToDataTableFromCSV_Trunk - CSV file content is empty,just have table head(1 row)!"));
			return FString(TEXT("CSV file content is empty,just have table head(1 row)!"));
		}

		FString tableHead_Unhandle = DataPreRows[0];
		TArray<FString> tableHeadArray_Unhandle;
		tableHead_Unhandle.ParseIntoArray(tableHeadArray_Unhandle, TEXT(","));
		if (tableHeadArray_Unhandle.Num() <= 1)
		{
			UE_LOG(LogLeafMySQL, Error, TEXT("ImportDataToDataTableFromCSV_Trunk - CSV file table head is not valid!"));
			return FString(TEXT("CSV file table head is not valid!"));
		}
		FString tableHead;
		for (const auto tmp : tableHeadArray_Unhandle)
		{
			tableHead += FLeafMySqlValueHandler(tmp).GetSource();
			tableHead += TEXT(",");
		}
		tableHead = tableHead.Left(tableHead.Len() - 1);

		if (InOverrideSourceData)
		{
			Command::DataTable::FLeafMySqlDataTableDelete daleteInfo(InLinkInfo, InTableName, true, true);
			FString resStr = DATATABLE::LeafDeleteDataTable(daleteInfo);
			if (!(resStr.Equals("Successed") || resStr.IsEmpty()))
			{
				UE_LOG(LogLeafMySQL, Error, TEXT("ImportDataToDataTableFromCSV_Trunk - Clear dataTable[%s] failed."), *InTableName);
				return resStr;
			}
		}

		TArray<FString> errorArrs;
		TArray<FString> tempArrs;
		tempArrs.AddDefaulted(InTrunkSize);
		for (int32 i = 1; i < DataPreRows.Num();)
		{
			for (int32 k = 0; k < InTrunkSize; k++)
			{
				if (k + i >= DataPreRows.Num())
				{
					tempArrs.SetNum(k);
					break;
				}
				tempArrs[k] = DataPreRows[i + k];
			}
			FString resStr = DATA::LeafInsertDataToTable_Trunk(InLinkInfo, InTableName, tableHead, tempArrs);
			if (!(resStr.Equals("Successed") || resStr.IsEmpty()))
			{
				errorArrs.Emplace(resStr);
			}
			i += InTrunkSize;
		}
		FString resFullStr;
		if (errorArrs.Num() > 0)
		{
			for (const auto tmp : errorArrs)
			{
				resFullStr += tmp;
				resFullStr += TEXT("\n");
			}
		}
		return resFullStr.IsEmpty() ? FString(TEXT("Successed")) : resFullStr;
	}
	UE_LOG(LogLeafMySQL, Error, TEXT("ImportDataToDataTableFromCSV_Trunk - CSV data read failed! If you opened [%s]CSV file,please close."), *InFilePath);
	return FString::Printf(TEXT("CSV data read failed! If you opened [%s]CSV file,please close."), *InFilePath);
}

FString LeafMySql::OTHER::LeafExportDataAsCSVFromDataTable(FLeafMySqlLinkInfo& InLinkInfo, const FString& InFileExportPath, const FString& InTableName, const int64 InLimitRowNum)
{
	Check_LEAFMYSQL();

	if (!InLinkInfo.IsConnectionValid())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL connection closed"));
		return FString(TEXT("MySQL connection closed"));
	}
	FString s = UTF8_TO_TCHAR(InLinkInfo.Connection->getSchema().c_str());
	if (s.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("MySQL : No database selected!"));
		return FString(TEXT("MySQL : No database selected!"));
	}

	if (InFileExportPath.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("ExportDataAsCSVFromDataTable - InFileExportPath is empty!"));
		return FString(TEXT("InFileExportPath is empty!"));
	}

	if (IFileManager::Get().FileExists(*InFileExportPath))
	{
		UE_LOG(LogLeafMySQL, Warning, TEXT("ExportDataAsCSVFromDataTable - The file '%s' is exist."), *InFileExportPath);
#if WITH_EDITOR
		EAppReturnType::Type re = FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString(FString::Printf(TEXT("The file '%s' is exist,would you like to override?"), *InFileExportPath)));
		if (re == EAppReturnType::No)
		{
			return FString::Printf(TEXT("The CSV export is stop.The file '%s' is exist.And wouldnt to override file."), *InFileExportPath);
		}
#endif // WITH_EDITOR
	}

	if (InTableName.IsEmpty())
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("ExportDataAsCSVFromDataTable - TableName is empty!"));
		return FString(TEXT("TableName is empty!"));
	}

	TMap<FString, Command::DataTable::FLeafMySqlDataTableParamDisplay> dataTableStructure;
	FString resStr = DATATABLE::LeafShowDataTableStructure(InLinkInfo, InTableName, dataTableStructure);
	if (!(resStr.Equals("Successed") || resStr.IsEmpty()))
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("ExportDataAsCSVFromDataTable - Get Table[%s] structure failed."), *InTableName);
		return FString::Printf(TEXT("Get Table[%s] structure failed."), *InTableName);
	}
	FString tableHead;
	for (const auto tmp : dataTableStructure)
	{
		tableHead += FString::Printf(TEXT("\"%s\","), *FLeafMySqlValueHandler(tmp.Key).GetSource());
	}
	tableHead = tableHead.Left(tableHead.Len() - 1);
	TArray<TMap<FString, FString>> outSelectedDatas;
	resStr = DATA::LeafFindDataFromTable(InLinkInfo, InTableName, TEXT("*"), outSelectedDatas, TEXT("None"), TEXT("None"), true, InLimitRowNum);
	if (!(resStr.Equals("Successed") || resStr.IsEmpty()))
	{
		UE_LOG(LogLeafMySQL, Error, TEXT("ExportDataAsCSVFromDataTable - Get Table[%s] data failed."), *InTableName);
		return FString::Printf(TEXT("Get Table[%s] data failed."), *InTableName);
	}
	FString finialOutputData = tableHead;
	finialOutputData += TEXT("\n");
	for (int64 i = 0; i < outSelectedDatas.Num(); i++)
	{
		for (const auto& tmp : outSelectedDatas[i])
		{
			finialOutputData += FLeafMySqlValueHandler(tmp.Value).GetFormatSource();
			finialOutputData += TEXT(",");
		}
		finialOutputData = finialOutputData.Left(finialOutputData.Len() - 1);
		finialOutputData += TEXT("\n");
	}
	if (FFileHelper::SaveStringToFile(finialOutputData, *InFileExportPath))
	{
		return FString(TEXT("Successed"));
	}
	return FString::Printf(TEXT("Write csv file failed,path[%s]"), *InFileExportPath);
}

#undef LOCTEXT_NAMESPACE
