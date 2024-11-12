// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#include "AsyncAction/LeafMySqlAsync_ExportDataToCSV.h"

#include "LeafMySQL.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"

using namespace LeafMySql;

DEFINE_LOG_CATEGORY(LogLeafMySqlAsync_ExportDataToCSV);

ULeafMySqlAsync_ExportDataToCSV::ULeafMySqlAsync_ExportDataToCSV(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	FileExportPath = TEXT("");
	TableName = TEXT("");
	LimitRowNum = 0;

	TargetHandleCount = -1;
	CurrentHandleCount = -1;
	ExportDataToCSVState = ELeafSQLExportDataToCSVState::None;
}

void ULeafMySqlAsync_ExportDataToCSV::Activate()
{
	UE::Tasks::FTask task = UE::Tasks::Launch(UE_SOURCE_LOCATION, [this]
		{
			TargetHandleCount = 0;
			CurrentHandleCount = 0;
			UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::PreHandle);
			if (!SQLAccessor)
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::Error, TEXT("ExportDataToCSV_Async - SQLAccessor is not valid!"));
				return;
			}
			if (!SQLAccessor->GetSQLConnectionState())
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::Error, TEXT("ExportDataToCSV_Async - SQLAccessor is not Connectting any Sql database"));
				return;
			}

			if (!GetLinkInfo())
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::Error, TEXT("ExportDataToCSV_Async - Cant get LinkInfo"));
				return;
			}

			Check_LEAFMYSQL();

			if (!GetLinkInfo()->IsConnectionValid())
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::Error, TEXT("MySQL connection closed"));
				return;
			}
			FString s = UTF8_TO_TCHAR(GetLinkInfo()->Connection->getSchema().c_str());
			if (s.IsEmpty())
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::Error, TEXT("MySQL : No database selected!"));
				return;
			}

			if (FileExportPath.IsEmpty())
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::Error, TEXT("ExportDataToCSV_Async - FileExportPath is empty!"));
				return;
			}

			if (IFileManager::Get().FileExists(*FileExportPath))
			{
				UE_LOG(LogLeafMySqlAsync_ExportDataToCSV, Warning, TEXT("ExportDataToCSV_Async - The file '%s' is exist."), *FileExportPath);
#if WITH_EDITOR
				EAppReturnType::Type re = FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString(FString::Printf(TEXT("The file '%s' is exist,would you like to override?"), *FileExportPath)));
				if (re == EAppReturnType::No)
				{
					UE_LOG(LogLeafMySqlAsync_ExportDataToCSV, Warning, TEXT("The CSV export is stop.The file '%s' is exist.And wouldnt to override file."), *FileExportPath);
					CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Cancel);
					UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::Cancel);
					return;
				}
#endif // WITH_EDITOR
			}

			if (TableName.IsEmpty())
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::Error, FString(TEXT("ExportDataToCSV_Async - TableName is empty!")));
				return;
			}

			UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::RequestDataTableStruture);
			if (bIsCanceled)
			{
				UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::Cancel);
				return;
			}
			TMap<FString, Command::DataTable::FLeafMySqlDataTableParamDisplay> dataTableStructure;
			FString resStr = DATATABLE::LeafShowDataTableStructure(*GetLinkInfo(), TableName, dataTableStructure);
			if (!(resStr.Equals("Successed") || resStr.IsEmpty()))
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::Error, FString::Printf(TEXT("ExportDataToCSV_Async - Get Table[%s] structure failed."), *TableName));
				return;
			}
			FString tableHead;
			for (const auto tmp : dataTableStructure)
			{
				tableHead += FString::Printf(TEXT("\"%s\","), *FLeafMySqlValueHandler(tmp.Key).GetSource());
			}
			tableHead = tableHead.Left(tableHead.Len() - 1);

			UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::RequestData);
			if (bIsCanceled)
			{
				UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::Cancel);
				return;
			}
			TArray<TArray<FString>> outSelectedDatas;
			resStr = DATA::LeafFindDataFromTable(*GetLinkInfo(), TableName, TEXT("*"), outSelectedDatas, TEXT("None"), TEXT("None"), true, LimitRowNum);
			if (!(resStr.Equals("Successed") || resStr.IsEmpty()))
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
				UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::Error, FString::Printf(TEXT("ExportDataToCSV_Async - Get Table[%s] data failed."), *TableName));
				return;
			}

			TargetHandleCount = outSelectedDatas.Num();
			UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::Handling);
			if (bIsCanceled)
			{
				UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::Cancel);
				return;
			}
			FString finialOutputData = tableHead;
			finialOutputData += TEXT("\n");
			for (int64 i = 0; i < outSelectedDatas.Num(); ++i)
			{
				for (const auto& tmp : outSelectedDatas[i])
				{
					finialOutputData += FLeafMySqlValueHandler(tmp).GetFormatSource();
					finialOutputData += TEXT(",");
				}
				finialOutputData = finialOutputData.Left(finialOutputData.Len() - 1);
				finialOutputData += TEXT("\n");
				++CurrentHandleCount;
				if (bIsCanceled)
				{
					UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::Cancel);
					return;
				}
			}

			UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::SavingToFile);
			if (bIsCanceled)
			{
				UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::Cancel);
				return;
			}
			if (FFileHelper::SaveStringToFile(finialOutputData, *FileExportPath))
			{
				CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Completed);
				UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::Completed);
				return;
			}

			CallStateChangeDelegate(ELeafSQLAsyncActionStateType::Failed);
			UpdateExportDataToCSVState(ELeafSQLExportDataToCSVState::Error, FString::Printf(TEXT("Write csv file failed,path[%s]"), *FileExportPath));
			return;
		});
}

ULeafMySqlAsync_ExportDataToCSV* ULeafMySqlAsync_ExportDataToCSV::ExportDataToCSV_Async(
	UPARAM(Ref)ULeafMySQLAccessorBase*& InSQLAccessor, 
	const FString& InFileExportPath, 
	const FString& InTableName, 
	const int64 InLimitRowNum)
{
	ULeafMySqlAsync_ExportDataToCSV* MyObj = NewObject<ULeafMySqlAsync_ExportDataToCSV>();
	MyObj->SQLAccessor = InSQLAccessor;
	MyObj->FileExportPath = InFileExportPath;
	MyObj->TableName = InTableName;
	MyObj->LimitRowNum = InLimitRowNum;

	return MyObj;
}

void ULeafMySqlAsync_ExportDataToCSV::UpdateExportDataToCSVState(const ELeafSQLExportDataToCSVState NewState, const FString& OutError)
{
	if (ExportDataToCSVState != NewState)
	{
		ExportDataToCSVState = NewState;

		if (!IsInGameThread())
		{
			AsyncTask(ENamedThreads::GameThread, [NewState, OutError, this]()
				{
					OnUpdateStateAndCount.Broadcast(NewState, CurrentHandleCount, TargetHandleCount, OutError);

					if (ExportDataToCSVState == ELeafSQLExportDataToCSVState::Error && !OutError.IsEmpty())
					{
						UE_LOG(LogLeafMySqlAsync_ExportDataToCSV, Error, TEXT("%s"), *OutError);
					}
					if (ExportDataToCSVState == ELeafSQLExportDataToCSVState::Error &&
						ExportDataToCSVState == ELeafSQLExportDataToCSVState::Completed)
					{
						this->SetReadyToDestroy();
					}
					else if (ExportDataToCSVState == ELeafSQLExportDataToCSVState::Cancel)
					{
						//this->Cancel();
						this->SetReadyToDestroy();
					}
				});
		}
		else
		{
			OnUpdateStateAndCount.Broadcast(NewState, CurrentHandleCount, TargetHandleCount, OutError);

			if (ExportDataToCSVState == ELeafSQLExportDataToCSVState::Error && !OutError.IsEmpty())
			{
				UE_LOG(LogLeafMySqlAsync_ExportDataToCSV, Error, TEXT("%s"), *OutError);
			}
			if (ExportDataToCSVState == ELeafSQLExportDataToCSVState::Error &&
				ExportDataToCSVState == ELeafSQLExportDataToCSVState::Completed)
			{
				this->SetReadyToDestroy();
			}
			else if (ExportDataToCSVState == ELeafSQLExportDataToCSVState::Cancel)
			{
				//this->Cancel();
				this->SetReadyToDestroy();
			}
		}
	}
	else
	{
		OnUpdateStateAndCount.Broadcast(ExportDataToCSVState, CurrentHandleCount, TargetHandleCount, OutError);

		if (ExportDataToCSVState == ELeafSQLExportDataToCSVState::Error && !OutError.IsEmpty())
		{
			UE_LOG(LogLeafMySqlAsync_ExportDataToCSV, Error, TEXT("%s"), *OutError);
		}
	}
}
