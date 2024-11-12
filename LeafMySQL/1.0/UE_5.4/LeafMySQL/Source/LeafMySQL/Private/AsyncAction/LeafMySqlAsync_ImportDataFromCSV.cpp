// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#include "AsyncAction/LeafMySqlAsync_ImportDataFromCSV.h"

#include "LeafMySQL.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"


DEFINE_LOG_CATEGORY(LogLeafMySqlAsync_ImportDataToCSV);

ULeafMySqlAsync_ImportDataFromCSV::ULeafMySqlAsync_ImportDataFromCSV(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	FilePath = TEXT("");
	TableName = TEXT("");
	bOverrideSourceData = false;
	TrunkSize = 1000;

	TargetHandleCount = -1;
	CurrentHandleCount = -1;
	ImportDataToCSVState = ELeafSQLImportDataFromCSVState::None;
}

void ULeafMySqlAsync_ImportDataFromCSV::Activate()
{
	UE::Tasks::FTask task = UE::Tasks::Launch(UE_SOURCE_LOCATION, [this]
		{
			TargetHandleCount = 0;
			CurrentHandleCount = 0;
			UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::PreHandle);
			if (!SQLAccessor)
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Error, TEXT("ImportDataFromCSV_Async - SQLAccessor is not valid!"));
				return;
			}
			if (!SQLAccessor->GetSQLConnectionState())
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Error, TEXT("ImportDataFromCSV_Async - SQLAccessor is not Connectting any Sql database"));
				return;
			}

			if (!GetLinkInfo())
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Error, TEXT("ImportDataFromCSV_Async - Cant get LinkInfo"));
				return;
			}

			Check_LEAFMYSQL();

			if (!GetLinkInfo()->IsConnectionValid())
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Error, TEXT("MySQL connection closed"));
				return;
			}
			FString s = UTF8_TO_TCHAR(GetLinkInfo()->Connection->getSchema().c_str());
			if (s.IsEmpty())
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Error, TEXT("MySQL : No database selected!"));
				return;
			}

			if (FilePath.IsEmpty())
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Error, TEXT("ImportDataFromCSV_Async - FileExportPath is empty!"));
				return;
			}

			if (!IFileManager::Get().FileExists(*FilePath))
			{
				UE_LOG(LogLeafMySqlAsync_ImportDataToCSV, Warning, TEXT("ImportDataFromCSV_Async - The file '%s' is not exist."), *FilePath);
#if WITH_EDITOR
				EAppReturnType::Type re = FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(FString::Printf(TEXT("ImportDataFromCSV_Async - The file '%s' is not exist,the action will be fail,please check the file path right."), *FilePath)));
#endif // WITH_EDITOR
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Error, FString::Printf(TEXT("ImportDataFromCSV_Async - The file '%s' is not exist,the action will be fail,please check the file path right."), *FilePath));
				return;
			}

			if (TableName.IsEmpty())
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Error, FString(TEXT("ImportDataFromCSV_Async - TableName is empty!")));
				return;
			}

			UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::ReadingCSVFile);
			if (bIsCanceled)
			{
				UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Cancel);
				return;
			}
			FString filePath = IFileManager::Get().ConvertToRelativePath(*FilePath);
			FString Data;
			if (FFileHelper::LoadFileToString(Data, *filePath, FFileHelper::EHashOptions::None, FILEREAD_AllowWrite | FILEWRITE_AllowRead))
			{
				if (Data.IsEmpty())
				{
					CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
					UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Error, FString(TEXT("ImportDataFromCSV_Async - CSV file content is empty!")));
					return;
				}

				TArray<FString> dataPreRows;
				Data.ParseIntoArray(dataPreRows, TEXT("\n"));
				if (dataPreRows.Num() <= 1)
				{
					CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
					UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Error, FString(TEXT("ImportDataFromCSV_Async - CSV file content is empty,just have table head(1 row)!")));
					return;
				}

				UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::HandlingHead);
				if (bIsCanceled)
				{
					UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Cancel);
					return;
				}
				FString tableHead_Unhandle = dataPreRows[0];
				TArray<FString> tableHeadArray_Unhandle;
				tableHead_Unhandle.ParseIntoArray(tableHeadArray_Unhandle, TEXT(","));
				if (tableHeadArray_Unhandle.Num() <= 1)
				{
					CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
					UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Error, FString(TEXT("ImportDataFromCSV_Async - CSV file table head is not valid!")));
					return;
				}
				FString tableHead;
				for (const auto tmp : tableHeadArray_Unhandle)
				{
					tableHead += FLeafMySqlValueHandler(tmp).GetSource();
					tableHead += TEXT(",");
				}
				tableHead = tableHead.Left(tableHead.Len() - 1);

				if (bOverrideSourceData)
				{
					UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::ClearingSourceData);
					if (bIsCanceled)
					{
						UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Cancel);
						return;
					}
					Command::DataTable::FLeafMySqlDataTableDelete daleteInfo(*GetLinkInfo(), TableName, true, true);
					FString resStr = DATATABLE::LeafDeleteDataTable(daleteInfo);
					if (!(resStr.Equals("Successed") || resStr.IsEmpty()))
					{
						CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
						UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Error, FString::Printf(TEXT("ImportDataFromCSV_Async - Clear dataTable[%s] failed."), *TableName));
						return;
					}
				}

				UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::HandlingData);
				if (bIsCanceled)
				{
					UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Cancel);
					return;
				}
				TargetHandleCount = dataPreRows.Num() - 1;
				TArray<FString> errorArrs;
				TArray<FString> tempArrs;
				tempArrs.AddDefaulted(TrunkSize);
				for (int32 i = 1; i < dataPreRows.Num();)
				{
					for (int32 k = 0; k < TrunkSize; k++)
					{
						if (k + i >= dataPreRows.Num())
						{
							tempArrs.SetNum(k);
							break;
						}
						tempArrs[k] = dataPreRows[i + k];
					}
					FString resStr = DATA::LeafInsertDataToTable_Trunk(*GetLinkInfo(), TableName, tableHead, tempArrs);
					if (!(resStr.Equals("Successed") || resStr.IsEmpty()))
					{
						errorArrs.Emplace(resStr);
					}
					i += TrunkSize;
					CurrentHandleCount += tempArrs.Num();
					UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Importing);
					if (bIsCanceled)
					{
						UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Cancel);
						return;
					}
				}
				UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Checking);
				FString resFullStr;
				if (errorArrs.Num() > 0)
				{
					for (const auto tmp : errorArrs)
					{
						resFullStr += tmp;
						resFullStr += TEXT("\n");
					}
				}
				
				if (!resFullStr.IsEmpty())
				{
					UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::UnChangeState, resFullStr);
				}
				UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Completed, resFullStr);
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Completed);
				return;
			}
			CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
			UpdateImportDataFromCSVState(ELeafSQLImportDataFromCSVState::Error, FString::Printf(TEXT("ImportDataFromCSV_Async - CSV data read failed! If you opened [%s]CSV file,please close."), *FilePath));
			return;
		}
	);
}

ULeafMySqlAsync_ImportDataFromCSV* ULeafMySqlAsync_ImportDataFromCSV::ImportDataFromCSV_Async(UPARAM(Ref)ULeafMySQLAccessorBase*& InSQLAccessor, const FString& InFilePath, const FString& InTableName, const bool InOverrideSourceData, const int32 InTrunkSize)
{
	ULeafMySqlAsync_ImportDataFromCSV* MyObj = NewObject<ULeafMySqlAsync_ImportDataFromCSV>();
	MyObj->SQLAccessor = InSQLAccessor;
	MyObj->FilePath = InFilePath;
	MyObj->TableName = InTableName;
	MyObj->bOverrideSourceData = InOverrideSourceData;
	MyObj->TrunkSize = InTrunkSize;

	return MyObj;
}

void ULeafMySqlAsync_ImportDataFromCSV::UpdateImportDataFromCSVState(const ELeafSQLImportDataFromCSVState NewState, const FString& OutError)
{
	if (ImportDataToCSVState != NewState && NewState != ELeafSQLImportDataFromCSVState::UnChangeState)
	{
		ImportDataToCSVState = NewState;

		if (!IsInGameThread())
		{
			AsyncTask(ENamedThreads::GameThread, [NewState, OutError, this]()
				{
					OnUpdateStateAndCount.Broadcast(NewState, CurrentHandleCount, TargetHandleCount, OutError);

					if (ImportDataToCSVState == ELeafSQLImportDataFromCSVState::Error && !OutError.IsEmpty())
					{
						UE_LOG(LogLeafMySqlAsync_ImportDataToCSV, Error, TEXT("%s"), *OutError);
					}
					if (ImportDataToCSVState == ELeafSQLImportDataFromCSVState::Error &&
						ImportDataToCSVState == ELeafSQLImportDataFromCSVState::Completed)
					{
						this->SetReadyToDestroy();
					}
					else if (ImportDataToCSVState == ELeafSQLImportDataFromCSVState::Cancel)
					{
						//this->Cancel();
						this->SetReadyToDestroy();
					}
				});
		}
		else
		{
			OnUpdateStateAndCount.Broadcast(NewState, CurrentHandleCount, TargetHandleCount, OutError);

			if (ImportDataToCSVState == ELeafSQLImportDataFromCSVState::Error && !OutError.IsEmpty())
			{
				UE_LOG(LogLeafMySqlAsync_ImportDataToCSV, Error, TEXT("%s"), *OutError);
			}
			if (ImportDataToCSVState == ELeafSQLImportDataFromCSVState::Error &&
				ImportDataToCSVState == ELeafSQLImportDataFromCSVState::Completed)
			{
				this->SetReadyToDestroy();
			}
			else if (ImportDataToCSVState == ELeafSQLImportDataFromCSVState::Cancel)
			{
				//this->Cancel();
				this->SetReadyToDestroy();
			}
		}
	}
	else
	{
		OnUpdateStateAndCount.Broadcast(ImportDataToCSVState, CurrentHandleCount, TargetHandleCount, OutError);

		if (ImportDataToCSVState == ELeafSQLImportDataFromCSVState::Error && !OutError.IsEmpty())
		{
			UE_LOG(LogLeafMySqlAsync_ImportDataToCSV, Error, TEXT("%s"), *OutError);
		}
	}
}
