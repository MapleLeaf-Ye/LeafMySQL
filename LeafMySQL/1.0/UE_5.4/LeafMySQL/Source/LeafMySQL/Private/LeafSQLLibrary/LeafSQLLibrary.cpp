// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#include "LeafSQLLibrary/LeafSQLLibrary.h"

#include "DataType/LeafMySQLType.h"

using namespace LeafMySql;

FLeafSQLWhereString ULeafSQLFunctionLibrary::LeafSQL_MakeTableColumn(const FString& InTableName, const FString& InColumnName)
{
	return FString::Printf(TEXT("%s.%s"), *InTableName, *InColumnName);
}

FLeafSQLWhereString ULeafSQLFunctionLibrary::LeafSQLCompareColumnValue(const FLeafSQLWhereString& InColumnName, const ELeafSqlWhereValueOperator InOperator, const FLeafSQLWhereString& InCompareValue)
{
    FString operatorStr;
	if (InOperator == ELeafSqlWhereValueOperator::Equals)
	{
		operatorStr = TEXT("=");
	}
	else if (InOperator == ELeafSqlWhereValueOperator::NotEquals)
	{
		operatorStr = TEXT("!=");
	}
	else if (InOperator == ELeafSqlWhereValueOperator::GreaterThan)
	{
		operatorStr = TEXT(">");
	}
	else if (InOperator == ELeafSqlWhereValueOperator::LessThan)
	{
		operatorStr = TEXT("<");
	}
	else if (InOperator == ELeafSqlWhereValueOperator::GreaterThanOrEqualTo)
	{
		operatorStr = TEXT(">=");
	}
	else
	{
		operatorStr = TEXT("<=");
	}
	return FString::Printf(TEXT("%s%s%s"), *InColumnName, *operatorStr, *FLeafMySqlValueHandler(InCompareValue.WhereString).Get(true));
}

FLeafSQLWhereString ULeafSQLFunctionLibrary::LeafSQLCombineCompareCondition(const FLeafSQLWhereString& InLeft, const ELeafSqlWhereLogicOperator InOperator, const FLeafSQLWhereString& InRight)
{
	FString str = InOperator == ELeafSqlWhereLogicOperator::AND ? TEXT("AND") : TEXT("OR");
	return FString::Printf(TEXT("%s %s %s"), *InLeft, *str, *InRight);
}

FLeafSQLWhereString ULeafSQLFunctionLibrary::LeafSQL_Like(const FLeafSQLWhereString& InColumnName, const FLeafSQLWhereString& InLikeFunction)
{
	return FString::Printf(TEXT("%s LIKE %s"), *InColumnName, *FLeafMySqlValueHandler(InLikeFunction.WhereString).Get());
}

FLeafSQLWhereString ULeafSQLFunctionLibrary::LeafSQL_In(const FLeafSQLWhereString& InColumnName, const TArray<FString>& InInValues)
{
	FString inStr = TEXT("(");
	for (const auto tmp : InInValues)
	{
		inStr += FLeafMySqlValueHandler(tmp).Get();
		inStr += TEXT(",");
	}
	inStr = inStr.Left(inStr.Len() - 1);
	inStr += TEXT(")");
	return FString::Printf(TEXT("%s IN %s"), *InColumnName, *inStr);
}

FLeafSQLWhereString ULeafSQLFunctionLibrary::LeafSQL_In_ChildSelect(const FLeafSQLWhereString& InColumnName, const FLeafSQLWhereString& InChildSelect)
{
	return FString::Printf(TEXT("%s IN (%s)"), *InColumnName, *InChildSelect);
}

FLeafSQLWhereString ULeafSQLFunctionLibrary::LeafSQL_Not(const FLeafSQLWhereString& InColumnName, const FLeafSQLWhereString& InNotValues)
{
	return FString::Printf(TEXT("NOT %s = %s"), *InColumnName, *FLeafMySqlValueHandler(InNotValues.WhereString).Get());
}

FLeafSQLWhereString ULeafSQLFunctionLibrary::LeafSQL_BetweenAnd(const FLeafSQLWhereString& InColumnName, const FLeafSQLWhereString& InStartValue, const FLeafSQLWhereString& InEndValue)
{
	return FString::Printf(TEXT("%s BETWEEN %s AND %s"), *InColumnName, *FLeafMySqlValueHandler(InStartValue.WhereString).Get(), *FLeafMySqlValueHandler(InEndValue.WhereString).Get());
}

FLeafSQLWhereString ULeafSQLFunctionLibrary::LeafSQL_IsNull(const FLeafSQLWhereString& InColumnName)
{
	return FString::Printf(TEXT("%s IS NULL"), *InColumnName);
}

FLeafSQLWhereString ULeafSQLFunctionLibrary::LeafSQL_IsNotNull(const FLeafSQLWhereString& InColumnName)
{
	return FString::Printf(TEXT("%s IS NOT NULL"), *InColumnName);
}

FLeafSQLWhereString ULeafSQLFunctionLibrary::LeafSQL_ChildSelect(
	const FString& InChildSelectColumnName, 
	const FString& InChildSelectTableName,
	const FLeafSQLWhereString& InChildSelectWhereCondition)
{
	return FString::Printf(TEXT("SELECT %s FROM %s WHERE %s"), *InChildSelectColumnName, *InChildSelectTableName, *InChildSelectWhereCondition);
}

FLeafSQLWhereString ULeafSQLFunctionLibrary::LeafSQL_ChildSelect_Sum(
	const FString& InChildSelectColumnName,
	const FString& InChildSelectTableName,
	const FLeafSQLWhereString& InChildSelectWhereCondition)
{
	return FString::Printf(TEXT("SELECT SUM(%s) FROM %s WHERE %s"), *InChildSelectColumnName, *InChildSelectTableName, *InChildSelectWhereCondition);
}
