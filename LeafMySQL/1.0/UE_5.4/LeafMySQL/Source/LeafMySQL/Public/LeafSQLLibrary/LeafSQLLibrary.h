// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataType/LeafMySQLBlueprintType.h"
#include "LeafSQLLibrary.generated.h"

UCLASS()
class LEAFMYSQL_API ULeafSQLFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	* 用于制作一个列参数类似users.name，其中users是表名，name是列名
	*（针对查询条件中涉及到多表格，通过这个指定列更准确），返回值可以使用在Where节点中
	* @Param InTableName 表名
	* @Param InColumnName 列名
	* @Return 规范的查询条件字符
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|WhereCondition")
	static FLeafSQLWhereString LeafSQL_MakeTableColumn(const FString& InTableName, const FString& InColumnName);

	/**
	* 通过这个函数可以制作运算符类的查询条件，类似id>1,id=100等
	* @Param InColumnName 列名
	* @Param InOperator 运算符
	* @Param InCompareValue 对比的值
	* @Return 规范的查询条件字符
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|WhereCondition")
	static FLeafSQLWhereString LeafSQLCompareColumnValue(const FLeafSQLWhereString& InColumnName, const ELeafSqlWhereValueOperator InOperator, const FLeafSQLWhereString& InCompareValue);

	/**
	* 通过这个函数可以组合查询条件，类似id>1 AND id<=100，相当于MySQL中的 AND OR
	* @Param InLeft 左边表达式
	* @Param InOperator 运算符
	* @Param InRight 右边表达式
	* @Return 规范的查询条件字符
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|WhereCondition")
	static FLeafSQLWhereString LeafSQLCombineCompareCondition(const FLeafSQLWhereString& InLeft, const ELeafSqlWhereLogicOperator InOperator, const FLeafSQLWhereString& InRight);

	/**
	* 通过这个函数可以组合查询条件，模糊匹配，结合正则表达式使用
	* @Param InColumnName 列名
	* @Param InLikeFunction 表达式
	* @Return 规范的查询条件字符
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|WhereCondition")
	static FLeafSQLWhereString LeafSQL_Like(const FLeafSQLWhereString& InColumnName, const FLeafSQLWhereString& InLikeFunction);

	/**
	* 类似MySql的IN
	* @Param InColumnName 列名
	* @Param InInValues 匹配的值
	* @Return 规范的查询条件字符
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|WhereCondition")
	static FLeafSQLWhereString LeafSQL_In(const FLeafSQLWhereString& InColumnName, const TArray<FString>& InInValues);

	/**
	* 如果要使用子查询语句删除数据可以结合这个IN
	* @Param InColumnName 列名
	* @Param InChildSelect 子查询
	* @Return 规范的查询条件字符
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|WhereCondition")
	static FLeafSQLWhereString LeafSQL_In_ChildSelect(const FLeafSQLWhereString& InColumnName, const FLeafSQLWhereString& InChildSelect);

	/**
	* 类似MySql的NOT
	* @Param InColumnName 列名
	* @Param InNotValues 匹配的值
	* @Return 规范的查询条件字符
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|WhereCondition")
	static FLeafSQLWhereString LeafSQL_Not(const FLeafSQLWhereString& InColumnName, const FLeafSQLWhereString& InNotValues);

	/**
	* 类似MySql的BETWEEN AND，匹配一个范围内的值
	* @Param InColumnName 列名
	* @Param InStartValue 匹配的开始值（也可以是最小）
	* @Param InEndValue 匹配的结束值（也可以是最大）
	* @Return 规范的查询条件字符
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|WhereCondition")
	static FLeafSQLWhereString LeafSQL_BetweenAnd(const FLeafSQLWhereString& InColumnName, const FLeafSQLWhereString& InStartValue, const FLeafSQLWhereString& InEndValue);

	/**
	* 类似MySql的IS NULL,找到列表中为空的值
	* @Param InColumnName 列名
	* @Return 规范的查询条件字符
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|WhereCondition")
	static FLeafSQLWhereString LeafSQL_IsNull(const FLeafSQLWhereString& InColumnName);

	/**
	* 类似MySql的IS NOT NULL,找到列表中不为空的值
	* @Param InColumnName 列名
	* @Return 规范的查询条件字符
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|WhereCondition")
	static FLeafSQLWhereString LeafSQL_IsNotNull(const FLeafSQLWhereString& InColumnName);

	/**
	* 用于制作一个子查询，返回值可以使用在Where节点中
	* @Param InChildSelectColumnName 子查询的列名
	* @Param InChildSelectTableName 子查询的表名
	* @Param InChildSelectWhereCondition 子查询的查询条件
	* @Return 规范的查询条件字符
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|WhereCondition")
	static FLeafSQLWhereString LeafSQL_ChildSelect(
		const FString& InChildSelectColumnName, 
		const FString& InChildSelectTableName,
		const FLeafSQLWhereString& InChildSelectWhereCondition);

	/**
	* 用于制作一个子查询并将查询的值相加，返回值可以使用在Where节点中
	* @Param InChildSelectColumnName 子查询的列名
	* @Param InChildSelectTableName 子查询的表名
	* @Param InChildSelectWhereCondition 子查询的查询条件
	* @Return 规范的查询条件字符
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|WhereCondition")
	static FLeafSQLWhereString LeafSQL_ChildSelect_Sum(
		const FString& InChildSelectColumnName,
		const FString& InChildSelectTableName,
		const FLeafSQLWhereString& InChildSelectWhereCondition);
};