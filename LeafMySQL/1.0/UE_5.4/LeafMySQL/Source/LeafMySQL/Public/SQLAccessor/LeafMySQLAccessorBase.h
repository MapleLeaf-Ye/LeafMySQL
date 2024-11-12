// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "CoreMinimal.h"
#include "DataType/LeafMySQLBlueprintType.h"
#include "DataType/LeafMySQLDisplayBlueprintType.h"
#include "DataType/LeafMySQLType.h"
#include "Templates/SubclassOf.h"
#include "LeafMySQLAccessorBase.generated.h"

using namespace LeafMySql;

class ULeafMySQLAccessorBase;
class USQLTableModifier;

DECLARE_LOG_CATEGORY_EXTERN(LogLeafMySQLAccessor, Warning, All);

#define CHECKLEAFSQLLINKING()\
if (!bIsSqlLinking)\
{\
	UE_LOG(LogLeafMySQLAccessor, Error, TEXT("[%s] is not linking with mysql! cant use api."), *this->GetFName().ToString());\
	ensure(0);\
}

#define CHECKLEAFSQLLINKINGWITHRETURN(InReturn)\
if (!bIsSqlLinking)\
{\
	UE_LOG(LogLeafMySQLAccessor, Error, TEXT("[%s] is not linking with mysql! cant use api."), *this->GetFName().ToString());\
	ensure(0);\
	return InReturn;\
}

UCLASS(BlueprintType)
class LEAFMYSQL_API ULeafMySQLAccessorBase : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*--------------------------------------------------------------------System Methods-------------------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

	/**
	* 连接到MySql的内部函数，由LeafMySQLSubsystem子系统的ConnectMySQL函数调用
	* @Param InLoginInfo 传入的连接数据库参数
	* @Param OutErrorMessage 输出错误信息，当连接失败时，如果连接成功则为空
	* @Return 表示数据库是否连接成功
	*/
	bool ConnectMySQL(const FLeafSQLLoginInfo& InLoginInfo, FString& OutErrorMessage);

	/**
	* 在访问对象未连接任何数据区下可使用此函数再次连接数据库
	* @Param InLoginInfo 传入的连接数据库参数
	* @Param OutErrorMessage 输出错误信息，当连接失败时，如果连接成功则为空
	* @Return 表示数据库是否连接成功
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|System")
	bool ReConnectMySQL(const FLeafSQLLoginInfo& InLoginInfo, FString& OutErrorMessage);

	/**
	* 用于断开MySql连接，断开后该Sql访问对象并不等于失效，可以再次调用ReConnectMySQL再次连接数据库
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|System")
	void DeConnectMySQL();

	/**
	* 获取访问对象是否连接数据库
	* @Return 返回访问对象是否已经连接数据库
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|LeafMySQLAccessor|System")
	FORCEINLINE bool GetSQLConnectionState() const { return bIsSqlLinking; };

	/**
	* 获取到数据库系统的的产品信息，版本信息，驱动信息，数据库支持等信息
	* @Param OutSystemDetail 返回的信息
	* @Return 是否成功获取
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|LeafMySQLAccessor|System")
	bool ShowSystemDetail(TMap<FString, FString>& OutSystemDetail);

	/**
	* 执行自定义的SQL命令
	* @Param InSQLCommand 输入的自定义SQL命令
	* @Param OutResults 返回的结果，如果这个SQL命令有返回结果，例如查询语句
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|System")
	FString ExecuteCustomSQLCommand(const FString& InSQLCommand, FLeafSQLStringResults& OutResults);


	/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*--------------------------------------------------------------------DataBase Methods-----------------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

	/**
	* 获取连接到的数据主机持有的所有数据库
	* @Param OutDataBaseNames 返回的所有数据库名称
	* @Return 获取成功与否
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|LeafMySQLAccessor|DataBase")
	bool ShowAllDataBase(TArray<FString>& OutDataBaseNames);

	/**
	* 选择或切换数据库，具体作用类似SQL语句的USE语句，如果你想切换操作的数据库使用这个函数
	* @Param InNewDataBaseName 输入要选择的数据库名称
	* @Return 操作成功与否
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataBase")
	bool SelectDataBaseForName(const FString& InNewDataBaseName);

	/**
	* 获取当前操作数据库的全部数据表
	* @Param OutDataTableNames 输出当前数据库持有的所有数据表
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|LeafMySQLAccessor|DataBase")
	FString ShowCurrentDataBaseAllTable(TArray<FString>& OutDataTableNames);

	/**
	* 通过该函数可以为当前连接的数据主机创建一个新的数据库
	* @Param InDataBaseName 创建新的数据库的名称
	* @Param InCharacterSet 数据库默认的字符集
	* @Param InCollate 数据库默认的排序方式，这个则根据所选的字符集决定
	* @Param InCheckIsExists 是否判断新建的数据库是否已经存在，如果创建的数据库已经存在通过这个参数可以判断，则不会产生错误
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataBase")
	FString CreateDataBase(const FString& InDataBaseName, 
		const ELeafCharacterSet InCharacterSet = ELeafCharacterSet::utf8mb4,
		const FString& InCollate = TEXT("utf8mb4_general_ci"),
		const bool InCheckIsExists = true);

	/**
	* 通过该函数可以为当前连接的数据主机创建一个新的数据库,这个方法可以自定义枚举外的字符集
	* @Param InDataBaseName 创建新的数据库的名称
	* @Param InCharacterSet 数据库默认的字符集
	* @Param InCollate 数据库默认的排序方式，这个则根据所选的字符集决定
	* @Param InCheckIsExists 是否判断新建的数据库是否已经存在，如果创建的数据库已经存在通过这个参数可以判断，则不会产生错误
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataBase")
	FString CreateDataBase_Custom(const FString& InDataBaseName,
		const FString& InCharacterSet = TEXT("utf8mb4"),
		const FString& InCollate = TEXT("utf8mb4_general_ci"),
		const bool InCheckIsExists = true);

	/**
	* 该函数用于更新已存在的数据库的设置
	* @Param InDataBaseName 数据库的名称
	* @Param InCharacterSet 要修改的数据库默认的字符集
	* @Param InCollate 要修改的数据库默认的排序方式，这个则根据所选的字符集决定
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataBase")
	FString UpdateDataBaseSettings(const FString& InDataBaseName,
		const ELeafCharacterSet InCharacterSet, const FString& InCollate);

	/**
	* 该函数用于更新已存在的数据库的设置,这个方法可以自定义枚举外的字符集
	* @Param InDataBaseName 数据库的名称
	* @Param InCharacterSet 要修改的数据库默认的字符集
	* @Param InCollate 要修改的数据库默认的排序方式，这个则根据所选的字符集决定
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataBase")
	FString UpdateDataBaseSettings_Custom(const FString& InDataBaseName, const FString& InCharacterSet, const FString& InCollate);

	/**
	* 通过该函数可以删除当前连接数据主机中的一个数据库
	* @Param InDataBaseName 要删除数据库的名称
	* @Param InCheckIsExists 是否判断删除的数据库是否不存在，如果不存在不会删除，则不会产生错误
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataBase")
	FString DeleteDataBase(const FString& InDataBaseName, const bool InCheckIsExists = true);

	/**
	* 通过该函数可以获取当前选择的数据库的SQL创建命令
	* @Param OutDataBaseDetail 返回的数据库创建命令
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|LeafMySQLAccessor|DataBase", meta=(DisplayName = "ShowDataBaseCreateDetails"))
	FString ShowDataBaseDetails(TMap<FString, FString>& OutDataBaseDetail);

	/**
	* 通过该函数可以获取当前选择的数据库的状态细节，例如允许同时连接的客户端数量等
	* @Param OutDataBaseStatus 返回的数据库状态细节信息
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|LeafMySQLAccessor|DataBase")
	FString GetDataBaseStatus(TMap<FString, FString>& OutDataBaseStatus);

	/**
	* 通过该函数可以获取数据库产生的错误信息
	* @Param OutDataTableError 返回的数据库的错误
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|LeafMySQLAccessor|DataBase")
	FString GetDataBaseErrors(TArray<FString>& OutDataTableError);

	/**
	* 通过该函数可以获取数据库产生的警告信息
	* @Param OutDataTableWarnings 返回的数据库的警告
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|LeafMySQLAccessor|DataBase")
	FString GetDataBaseWarnings(TArray<FString>& OutDataTableWarnings);


	/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*--------------------------------------------------------------------DataTable Methods----------------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

	/**
	* 通过该函数可以为当前操作（选择）的数据库创建一张新的数据表
	* @Param InConstructor 传入的数据表构建器
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|Create")
	FString CreateDataTable(const class USQLTableConstructor* InConstructor);

	/**
	* 通过该函数可以创建一个数据表构建器,创建后同样可以调用其中的参数添加数据表的列，设置列的默认值等
	* @Param InDataTableName 传入的新建数据表的名称
	* @Param InCollate 传入的新建数据表的默认排序类型，注意函数形参的默认值只只用于参数InCharset默认值utf8mb4的情况，如果InCharset更改了InCollate可能需要一起修改
	* @Param InCheckIsExists 检测新建的数据表是否已经存在，如果存在则不传进，防止报错
	* @Param InEngine 传入的存储引擎
	* @Param InCharset 传入的新建数据表的字符集
	* @Return 返回的数据表构建器
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|Create")
	class USQLTableConstructor* MakeNewTableConstructor(
		const FString& InDataTableName,
		const FString& InCollate = TEXT("utf8mb4_general_ci"),
		const bool InCheckIsExists = true,
		const FString& InEngine = TEXT("InnoDB"),
		const ELeafCharacterSet InCharset = ELeafCharacterSet::utf8mb4);

	/**
	* 通过该函数可以创建一个数据表构建器,创建后同样可以调用其中的参数添加数据表的列，设置列的默认值等，该方法通过字符串方式指定字符集
	* @Param InDataTableName 传入的新建数据表的名称
	* @Param InCheckIsExists 检测新建的数据表是否已经存在，如果存在则不传进，防止报错
	* @Param InEngine 传入的存储引擎
	* @Param InCharset 传入的新建数据表的字符集
	* @Param InCollate 传入的新建数据表的默认排序类型，注意函数形参的默认值只只用于参数InCharset默认值utf8mb4的情况，如果InCharset更改了InCollate可能需要一起修改
	* @Return 返回的数据表构建器
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|Create")
	class USQLTableConstructor* MakeNewTableConstructorWithString(
		const FString& InDataTableName,
		const bool InCheckIsExists = true,
		const FString& InEngine = TEXT("InnoDB"),
		const FString InCharset = TEXT("utf8mb4"),
		const FString& InCollate = TEXT("utf8mb4_general_ci"));

	/**
	* 通过该函数可以为当前操作（选择）的数据库删除一张数据表
	* @Param InDataTableName 要删除的数据表名称
	* @Param bInCheckIsExists 是否检查数据表存在，如果存在则删除，防止报错
	* @Param bInJustClearData 是否只删除数据表的数据保留数据表
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|Delete")
	FString DeleteDataTable(
		const FString& InDataTableName,
		const bool bInCheckIsExists = true,
		const bool bInJustClearData = false);

	/**
	* 通过该函数可以为当前操作（选择）的数据库的数据表移除一列数据项
	* @Param InDataTableName 要删除列的数据表名称
	* @Param InRemoveColumnName 要移除的数据表列的名称
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|Modify")
	FString RemoveColumn(const FString& InDataTableName, const FString& InRemoveColumnName);

	/**
	* 通过该函数可以为当前操作（选择）的数据库的数据表重命名一列数据项的名称
	* @Param InDataTableName 要重命名列的数据表名称
	* @Param InModifyColumnName 要重命名的数据表列的名称
	* @Param InModifyNewColumnName 数据表列的新名称
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|Modify")
	FString RenameColumn(
		const FString& InDataTableName,
		const FString& InModifyColumnName,
		const FString& InModifyNewColumnName);

	/**
	* 通过该函数可以为当前操作（选择）的数据库的数据表添加新的数据项
	* @Param InModifier 传入的数据表列添加修改器
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|Modify")
	FString ModifyDataTable_AddColumn(class USQLTableModifier_AddColumn* InModifier);

	/**
	* 通过该函数可以为当前操作（选择）的数据库的数据表移除列项
	* @Param InModifier 传入的数据表列删除修改器
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|Modify")
	FString ModifyDataTable_RemoveColumn(class USQLTableModifier_RemoveColumn* InModifier);

	/**
	* 通过该函数可以为当前操作（选择）的数据库的数据表修改列项
	* @Param InModifier 传入的数据表列修改器
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|Modify")
	FString ModifyDataTable_ModifyColumn(class USQLTableModifier_ModifyColumn* InModifier);

	/**
	* 通过该函数可以为当前操作（选择）的数据库的数据表重命名列项
	* @Param InModifier 传入的数据表列重命名修改器
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|Modify")
	FString ModifyDataTable_RenameColumn(class USQLTableModifier_RenameColumn* InModifier);

	/**
	* 通过该函数可以创建一个新的列修改器，根据创建列修改器的类不同，内部功能函数也将不同
	* @Param InModifierClass 传入的列修改器类
	* @Param InDataTableName 传入的数据表名称
	* @Return 返回的列修改器
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|Modify", meta = (DeterminesOutputType = "InModifierClass"))
	class USQLTableModifier* MakeTableModifier(
		TSubclassOf<USQLTableModifier> InModifierClass,
		const FString& InDataTableName);

	/**
	* 通过该函数可以获取当前操作（选择）的数据库的数据表的表结构（数据表有什么列，对应什么类型，约束等）
	* @Param InTableName 传入的要查询结构的数据表的名称
	* @Param OutDataTableStructure 返回的数据表结构数据
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|DataTableInfo")
	FString ShowDataTableStructure(const FString& InTableName, TMap<FString, FLeafColumnStructureDisplay>& OutDataTableStructure);

	/**
	* 通过该函数可以获取当前操作（选择）的数据库的数据表的表创建SQL命令
	* @Param InTableName 传入的要查询数据表的名称
	* @Return 错误信息，如果成功执行返回创建的表SQL创建命令
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|DataTableInfo")
	FString ShowDataTableSQLCreateCommand(const FString& InTableName);

	/**
	* 通过该函数可以获取当前操作（选择）的数据库的数据表的全部数据，该函数在数据表数据量大的时候慎重使用！！！
	* 没有加查询限制，如果数据表有上十万百万数据一下子查询可想而知
	* 在数据表数据特别大的情况下，可以使用FindDataFromTable函数添加限制查询行数
	* @Param InTableName 传入的要查询数据表的名称
	* @Param OutDatas 返回的数据
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|DataTableInfo")
	FString ShowTableAllData(const FString& InTableName, TArray<FLeafSQLStringResultPreRow>& OutDatas);

	/**
	* 通过该函数可以获取当前操作（选择）的数据库的数据表指定列的全部数据，该函数在数据表数据量大的时候慎重使用！！！
	* 没有加查询限制，如果数据表有上十万百万数据一下子查询可想而知
	* 在数据表数据特别大的情况下，可以使用FindDataFromTable函数添加限制查询行数
	* @Param InTableName 传入的要查询数据表的名称
	* @Param InColumnName 传入的要查询数据表的列名称
	* @Param OutDatas 返回的数据
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|DataTableInfo")
	FString ShowTableColumnAllData(const FString& InTableName, const FString& InColumnName, TArray<FString>& OutDatas);

	/**
	* 通过该函数可以获取当前操作（选择）的数据库的数据表的索引
	* @Param InTableName 传入的要查询索引的数据表名称
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|DataTableInfo")
	FString ShowTableIndex(const FString& InTableName, TMap<FString, FString>& OutInfos);

	/**
	* 通过该函数可以获取当前操作（选择）的数据库的数据表的细节信息，例如数据表的存储引擎，目前的行数，数据量等
	* @Param InTableName 传入的要查询的数据表名称
	* @Param OutInfos 返回的信息
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|DataTableInfo")
	FString ShowDataTableStatus(const FString& InTableName, TMap<FString, FString>& OutInfos);

	/**
	* 通过该函数可以为当前操作（选择）的数据库的数据表创建索引
	* @Param InTableName 传入的要操作的数据表名称
	* @Param InIndexName 新的索引名称
	* @Param InColumnInfos 传入新索引依据创建的列
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|DataTableIndex")
	FString CreateTableIndex(const FString& InTableName, const FString& InIndexName, const TArray<FString>& InColumnInfos);

	/**
	* 通过该函数可以为当前操作（选择）的数据库的数据表删除索引
	* @Param InTableName 传入的要操作的数据表名称
	* @Param InIndexName 删除的索引名称
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|DataTable|DataTableIndex")
	FString DeleteTableIndex(const FString& InTableName, const FString& InIndexName);


	/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*--------------------------------------------------------------------Data Methods---------------------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

	/**
	* 通过该函数可以为当前操作（选择）的数据库的数据表插入数据
	* @Param InTableName 传入的要操作的数据表名称
	* @Param InInsertDatas 插入的数据，通过字典key填写列名，value填对应要插入的数据
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Data")
	FString InsertDataToTable(const FString& InTableName, const TMap<FString, FString>& InInsertDatas);

	/**
	* 通过该函数可以为当前操作（选择）的数据库的数据表插入数据
	* @Param InTableName 传入的要操作的数据表名称
	* @Param InInsertColumnNames 填写插入数据的列名通过逗号隔开例如: id,name,email
	* @Param InInsertDatas 填写插入数据的列值通过逗号隔开例如: 1,aaa,1234565@qq.com
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Data")
	FString InsertDataToTableForString(const FString& InTableName, const FString& InInsertColumnNames, const FString& InInsertDatas);

	/**
	* 通过该函数可以为当前操作（选择）的数据库的数据表插入多行数据
	* @Param InTableName 传入的要操作的数据表名称
	* @Param InInsertColumnNames 填写插入数据的列名通过逗号隔开例如: id,name,email
	* @Param InInsertDatas 填写插入数据的列值通过逗号隔开例如: 1,aaa,1234565@qq.com，每个数组元素都应遵守这个格式约定
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Data")
	FString InsertDataToTable_Trunk(const FString& InTableName, const FString& InInsertColumnNames, const TArray<FString>& InInsertDatas);

	/**
	* 通过该函数可以查找当前操作（选择）的数据库的数据表的数据，并且支持一系列的限制条件
	* @Param InTableName 传入的要操作的数据表名称
	* @Param InSelectColumns 要查询的列
	* @Param InWhereCondition 如果为空或者None则不起效，查询的条件语句，例如id='1'，使用方法就像SQL语句那样
	* @Param InOrderColumnName 如果为空或者None则不起效，填写了则会按照这列进行顺序排序
	* @Param bInIsASC InOrderColumnName起效的情况下控制是升序还是降序
	* @Param InLimitRowNum 查询数据限制的行数，针对数据量大的数据表可以使用，如果为0则不限制
	* @Param OutSelectedDatas 返回的查询数据
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Data")
	FString FindDataFromTable(const FString& InTableName,
		const TArray<FString>& InSelectColumns, TArray<FLeafSQLStringResultPreRow>& OutSelectedDatas,
		const FLeafSQLWhereString InWhereCondition = FLeafSQLWhereString(), const FString& InOrderColumnName = TEXT("None"),
		const bool bInIsASC = true, const int64 InLimitRowNum = 0);

	/**
	* 通过该函数可以查找当前操作（选择）的数据库的数据表的数据，并且支持一系列的限制条件
	* @Param InTableName 传入的要操作的数据表名称
	* @Param InSelectColumns 要查询的列,通过逗号隔开例如: id,name,email
	* @Param InWhereCondition 如果为空或者None则不起效，查询的条件语句，例如id='1'，使用方法就像SQL语句那样
	* @Param InOrderColumnName 如果为空或者None则不起效，填写了则会按照这列进行顺序排序
	* @Param bInIsASC InOrderColumnName起效的情况下控制是升序还是降序
	* @Param InLimitRowNum 查询数据限制的行数，针对数据量大的数据表可以使用，如果为0则不限制
	* @Param OutSelectedDatas 返回的查询数据
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Data")
	FString FindDataFromTableForString(const FString& InTableName,
		const FString& InSelectColumns, TArray<FLeafSQLStringResultPreRow>& OutSelectedDatas,
		const FLeafSQLWhereString InWhereCondition = FLeafSQLWhereString(), const FString& InOrderColumnName = TEXT("None"),
		const bool bInIsASC = true, const int64 InLimitRowNum = 0);

	/**
	* 通过该函数可以为当前操作（选择）的数据库的数据表更新数据
	* @Param InTableName 传入的要操作的数据表名称
	* @Param InUpdateDatas 要更新的信息，通过字典key填写列名，value填对应要更新的数据
	* @Param InWhereCondition 不可以为空，查询的条件语句，例如id='1'，使用方法就像SQL语句那样
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Data")
	FString UpdateDataFromTable(const FString& InTableName,
		const TMap<FString, FString>& InUpdateDatas, const FLeafSQLWhereString& InWhereCondition);

	/**
	* 通过该函数可以为当前操作（选择）的数据库的数据表更新数据
	* @Param InTableName 传入的要操作的数据表名称
	* @Param InUpdateDatas 要更新的信息，格式为列名=值逗号隔开如果有多个值，例如id='1',name='aaa',email='46789@qq.com'
	* @Param InWhereCondition 不可以为空，查询的条件语句，例如id='1'，使用方法就像SQL语句那样
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Data")
	FString UpdateDataFromTableForString(const FString& InTableName,
		const FString& InUpdateDatas, const FLeafSQLWhereString& InWhereCondition);

	/**
	* 通过该函数可以为当前操作（选择）的数据库的数据表删除数据
	* @Param InTableName 传入的要操作的数据表名称
	* @Param InWhereCondition 不可以为空，查询的条件语句，例如id='1'，使用方法就像SQL语句那样
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Data")
	FString DeleteDataFromTable(const FString& InTableName, const FLeafSQLWhereString& InWhereCondition);


	/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*--------------------------------------------------------------------Event Methods--------------------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

	/**
	* 设置数据库事务系统是否自动提交
	* @Param InNewAuto 是否开启自动提交
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Event")
	FString SetEventCommitAuto(const bool InNewAuto = false);

	/**
	* 获取数据库事务系统是否自动提交
	* @Return 返回是否自动提交
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|LeafMySQLAccessor|Event")
	bool GetEventCommitAuto() const;

	/**
	* 开启数据库的事务
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Event")
	FString BeginEvent();

	/**
	* 提交（结束）数据库的事务
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Event")
	FString CommitEvent();

	/**
	* 添加数据库的事务的回滚点，根据回滚点可以让事务回滚到特定的操作点
	* @Param InNewSavePointName 添加新的回滚点
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Event")
	FString AddSavePoint(const FString& InNewSavePointName);

	/**
	* 执行事务的回滚
	* @Param InSavePointName 输入的回滚点，如果为空则默认回滚上一步操作
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Event")
	FString RollBackEvent(const FString& InSavePointName = TEXT(""));

	/**
	* 获取当前事务系统的状态
	* @Param OutInfos 返回的信息
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LeafMySQL|LeafMySQLAccessor|Event")
	FString ShowCurrentEventStatus(TArray<FLeafSQLStringResultPreRow>& OutInfos);

	/**
	* 锁定数据表的写操作，这在执行一些事务的时候很有用
	* @Param InTableName 锁定写操作的数据表名称
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Event")
	FString LockTable_Write(const FString& InTableName);

	/**
	* 锁定数据表的读操作，这在执行一些事务的时候很有用
	* @Param InTableName 锁定读操作的数据表名称
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Event")
	FString LockTable_Read(const FString& InTableName);

	/**
	* 解除锁定数据表的读以及写操作
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Event")
	FString UnLockTable();

	/**
	* 设置事务的隔离级别
	* @Param InLevel 输入的隔离等级
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Event")
	FString SetTransactionIsolationLevel(const ELeafSqlTransactionIsolationLevel InLevel);


	/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*--------------------------------------------------------------------Other Methods--------------------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

	/**
	* 通过该函数可以为当前操作（选择）的数据库的数据表导入CSV表格数据
	* @Param InFilePath CSV文件的路径
	* @Param InTableName 导入表格的名字
	* @Param InOverrideSourceData 如果为true则会情况表格本身数据再导入CSV的数据，为false则在原有数据基础上附加CSV数据
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Other")
	FString ImportDataToDataTableFromCSV(const FString& InFilePath, const FString& InTableName, const bool InOverrideSourceData = false);

	/**
	* 通过该函数可以为当前操作（选择）的数据库的数据表导入CSV表格数据，通过批量打包的方式导入，性能好于ImportDataToDataTableFromCSV
	* @Param InFilePath CSV文件的路径
	* @Param InTableName 导入表格的名字
	* @Param InOverrideSourceData 如果为true则会情况表格本身数据再导入CSV的数据，为false则在原有数据基础上附加CSV数据
	* @Param InTrunkSize 一次性为数据库中插入的数据量
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Other")
	FString ImportDataToDataTableFromCSV_Trunk(const FString& InFilePath, const FString& InTableName, const bool InOverrideSourceData = false, const int32 InTrunkSize = 1000);

	/**
	* 通过该函数可以将当前操作（选择）的数据库的数据表的数据导出为CSV文件
	* @Param InFileExportPath CSV文件的路径
	* @Param InTableName 导出表格的名字
	* @Param InLimitRowNum 限制导出的行数，如果为0则全部导出
	* @Return 错误信息，如果成功执行返回Successed或者空
	*/
	UFUNCTION(BlueprintCallable, Category = "LeafMySQL|LeafMySQLAccessor|Other")
	FString ExportDataAsCSVFromDataTable(const FString& InFileExportPath, const FString& InTableName, const int64 InLimitRowNum = 0);


protected:
	virtual ~ULeafMySQLAccessorBase();

public:
	FORCEINLINE FLeafMySqlLinkInfo& GetLinkInfo() { return LinkInfo; }
private:
	FLeafMySqlLoginInfo LoginInfo;
	FLeafMySqlLinkInfo LinkInfo;
	bool bIsSqlLinking;
};