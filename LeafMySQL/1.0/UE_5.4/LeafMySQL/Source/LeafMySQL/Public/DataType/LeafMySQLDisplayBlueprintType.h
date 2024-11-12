// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "CoreMinimal.h"
#include "LeafMySQLType.h"
#include "LeafMySQLDisplayBlueprintType.generated.h"

USTRUCT(BlueprintType)
struct LEAFMYSQL_API FLeafSQLStringResultPreRow
{
	GENERATED_USTRUCT_BODY()
		FLeafSQLStringResultPreRow() {}
	FLeafSQLStringResultPreRow(TMap<FString, FString>& InInfos)
	{
		InfoMapper = InInfos;
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "LeafMySQL|LeafSQLStringResultPreRow")
	TMap<FString, FString> InfoMapper;
};

USTRUCT(BlueprintType)
struct LEAFMYSQL_API FLeafSQLStringResults
{
	GENERATED_USTRUCT_BODY()
		FLeafSQLStringResults() {}
	FLeafSQLStringResults(TArray<TMap<FString, FString>>& InInfos)
	{
		for (auto& tmp : InInfos)
		{
			Infos.Emplace(FLeafSQLStringResultPreRow(tmp));
		}
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "LeafMySQL|LeafSQLStringResults")
		TArray<FLeafSQLStringResultPreRow> Infos;
};

USTRUCT(BlueprintType)
struct LEAFMYSQL_API FLeafColumnStructureDisplay
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LeafMySQL|LeafColumnStructureDisplay")
	FString ColumnName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LeafMySQL|LeafColumnStructureDisplay")
	FString ColumnType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LeafMySQL|LeafColumnStructureDisplay")
	bool IsPrimaryKey;		//�Ƿ�Ϊ����

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LeafMySQL|LeafColumnStructureDisplay")
	bool NotNull;			//�����Ƿ�����Ϊ��

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LeafMySQL|LeafColumnStructureDisplay")
	bool Unsigned;			//�����Ƿ�Ϊ������ʹ������������

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LeafMySQL|LeafColumnStructureDisplay")
	bool AutoIncrement;		//�����Ƿ���Ҫ�Զ�����������ID

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LeafMySQL|LeafColumnStructureDisplay")
	FString DefaultValue;	//Ĭ��ֵ

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LeafMySQL|LeafColumnStructureDisplay")
	int64 CharLength;		//ֻ�������ַ�����

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LeafMySQL|LeafColumnStructureDisplay")
	int64 M;				//ֻ������DECIMAL��������M

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LeafMySQL|LeafColumnStructureDisplay")
	int64 D;				//ֻ������DECIMAL��������D

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LeafMySQL|LeafColumnStructureDisplay")
	FString DisplayString;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LeafMySQL|LeafColumnStructureDisplay")
	int32 Type;		//��������������ЧΪ-1������0������1������2���ַ�3������4��enum5��set6

	FLeafColumnStructureDisplay()
	{
		ColumnName = TEXT("None");
		ColumnType = TEXT("None");
		IsPrimaryKey = false;
		NotNull = false;
		Unsigned = false;
		AutoIncrement = false;
		DefaultValue = TEXT("None");
		CharLength = -1;
		M = -1;
		D = -1;
		DisplayString = TEXT("None");
		Type = -1;
	}

	FLeafColumnStructureDisplay(const struct LeafMySql::Command::DataTable::FLeafMySqlDataTableParamDisplay& InSourceInfo)
	{
		ColumnName = InSourceInfo.ParamName;
		ColumnType = LeafMySql::ELeafMySqlValueType::GetLeafMySqlValueTypeString(InSourceInfo.ParamType);
		IsPrimaryKey = InSourceInfo.bIsPrimaryKey;
		NotNull = InSourceInfo.bNotNull;
		Unsigned = InSourceInfo.bUnsigned;
		AutoIncrement = InSourceInfo.bAutoIncrement;
		DefaultValue = InSourceInfo.DefaultValue;
		CharLength = InSourceInfo.CharLength;
		M = InSourceInfo.DECIMAL_M;
		D = InSourceInfo.DECIMAL_D;
		DisplayString = InSourceInfo.ToString();
		{
			if (LeafMySql::ELeafMySqlValueType::IsInegertype(InSourceInfo.ParamType))
			{
				Type = 0;
			}
			else if (LeafMySql::ELeafMySqlValueType::IsFloatType(InSourceInfo.ParamType))
			{
				Type = 1;
			}
			else if (LeafMySql::ELeafMySqlValueType::IsDateType(InSourceInfo.ParamType))
			{
				Type = 2;
			}
			else if (LeafMySql::ELeafMySqlValueType::IsCharType(InSourceInfo.ParamType))
			{
				Type = 3;
			}
			else if (LeafMySql::ELeafMySqlValueType::IsSpatialType(InSourceInfo.ParamType))
			{
				Type = 4;
			}
			else if (InSourceInfo.ParamType == LeafMySql::ELeafMySqlValueType::ENUM)
			{
				Type = 5;
			}
			else if (InSourceInfo.ParamType == LeafMySql::ELeafMySqlValueType::SET)
			{
				Type = 6;
			}
			else
			{
				Type = -1;
			}
		}
	}
};