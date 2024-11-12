// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#include "CoreMinimal.h"
#include "LeafMySQLBlueprintType.generated.h"

USTRUCT(BlueprintType)
struct LEAFMYSQL_API FLeafSQLLoginInfo
{
	GENERATED_USTRUCT_BODY()
	FLeafSQLLoginInfo() {}

	//登录数据库所需的用户名，例如默认的root
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "LeafMySQL|LeafSQLLoginInfo")
	FString UserName;
	//登录数据库所需的密码，对应输入的用户名
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "LeafMySQL|LeafSQLLoginInfo")
	FString Password;
	//登录数据库库默认使用的数据库，可为空，登录后再使用SelectDataBaseForName函数选择数据库
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "LeafMySQL|LeafSQLLoginInfo")
	FString DefaultDataBase;
	//登录数据库的地址
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "LeafMySQL|LeafSQLLoginInfo")
	FString SqlServerAddress;
	//登录数据库的端口
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "LeafMySQL|LeafSQLLoginInfo")
	FString Port;
};

UENUM(BlueprintType)
enum class ELeafCharacterSet : uint8
{
	utf8mb4 = 0,
	utf8,
	utf8mb3,
	utf32,
	utf16,
	utf16le,
	gbk,
	gb2312,
	big5,
	ascii,
	latin1,
	latin_swedish_ci
};

UENUM(BlueprintType)
enum class ELeafIntegerColumnType : uint8
{
	/*IntegerType*/					 
	TINYINT = 0				UMETA(DisplayName = "Tinyint"),				// 1 Bytes	(-128，127)	(0，255)	小整数值
	SMALLINT				UMETA(DisplayName = "Smallint"),			// 2 Bytes	(-32 768，32 767)	(0，65 535)	大整数值
	MEDIUMINT				UMETA(DisplayName = "Mediumint"),			// 3 Bytes	(-8 388 608，8 388 607)	(0，16 777 215)	大整数值
	INTEGER					UMETA(DisplayName = "Integer"),				// 4 Bytes	(-2 147 483 648，2 147 483 647)	(0，4 294 967 295)	大整数值
	BIGINT = 4				UMETA(DisplayName = "Bigint"),				// 8 Bytes	(-9,223,372,036,854,775,808，9 223 372 036 854 775 807)	(0，18 446 744 073 709 551 615)	极大整数值
};

UENUM(BlueprintType)
enum class ELeafFloatColumnType : uint8
{
	/*floatType*/														//	可以为其指定MD值 
	FLOAT = 0				UMETA(DisplayName = "Float"),				// 4 Bytes	(-3.402 823 466 E+38，-1.175 494 351 E-38)，0，(1.175 494 351 E-38，3.402 823 466 351 E+38)	0，(1.175 494 351 E-38，3.402 823 466 E+38)	单精度浮点数值
	DOUBLE					UMETA(DisplayName = "Double"),				// 8 Bytes	(-1.797 693 134 862 315 7 E+308，-2.225 073 858 507 201 4 E-308)，0，(2.225 073 858 507 201 4 E-308，1.797 693 134 862 315 7 E+308)	0，(2.225 073 858 507 201 4 E-308，1.797 693 134 862 315 7 E+308)	双精度浮点数值
	DECIMAL = 2				UMETA(DisplayName = "Decimal"),				// 对DECIMAL(M,D) ，如果M>D，为M+2否则为D+2	依赖于M和D的值	依赖于M和D的值	小数值
};

UENUM(BlueprintType)
enum class ELeafDateColumnType : uint8
{
	/*DateType*/					
	DATE = 0				UMETA(DisplayName = "Date"),				// 3 Bytes	1000-01-01/9999-12-31	YYYY-MM-DD	日期值
	TIME					UMETA(DisplayName = "Time"),				// 3 Bytes	'-838:59:59'/'838:59:59'	HH:MM:SS	时间值或持续时间
	YEAR					UMETA(DisplayName = "Year"),				// 1 Bytes	1901/2155	YYYY	年份值
	DATETIME				UMETA(DisplayName = "Datetime"),			// 8 Bytes	'1000-01-01 00:00:00' 到 '9999-12-31 23:59:59'	YYYY-MM-DD hh:mm:ss	混合日期和时间值
	/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	TIMESTAMP = 4			UMETA(DisplayName = "Timestamp"),			// 4 Bytes	'1970-01-01 00:00:01' UTC 到 '2038-01-19 03:14:07' UTC
																		//			结束时间是第 2147483647 秒，北京时间 2038 - 1 - 19 11:14 : 07，格林尼治时间 2038年1月19日 凌晨 03 : 14 : 07
																		//			YYYY - MM - DD hh : mm:ss	
																		//			混合日期和时间值，时间戳
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
};

UENUM(BlueprintType)
enum class ELeafCharColumnType : uint8
{
	/*CharType*/					 
	CHAR = 0				UMETA(DisplayName = "Char"),				// 0-255 bytes	定长字符串
	VARCHAR					UMETA(DisplayName = "Varchar"),				// 0-65535 bytes	变长字符串
	TINYBLOB				UMETA(DisplayName = "Tinyblob"),			// 0-255 bytes	不超过 255 个字符的二进制字符串
	TINYTEXT				UMETA(DisplayName = "Tinytext"),			// 0-255 bytes	短文本字符串
	BLOB					UMETA(DisplayName = "Blob"),				// 0-65 535 bytes	二进制形式的长文本数据
	TEXT					UMETA(DisplayName = "Text"),				// 0-65 535 bytes	长文本数据
	MEDIUMBLOB				UMETA(DisplayName = "Mediumblob"),			// 0-16 777 215 bytes	二进制形式的中等长度文本数据
	MEDIUMTEXT				UMETA(DisplayName = "Mediumtext"),			// 0-16 777 215 bytes	中等长度文本数据
	LONGBLOB				UMETA(DisplayName = "Longblob"),			// 0-4 294 967 295 bytes	二进制形式的极大文本数据
	LONGTEXT = 9			UMETA(DisplayName = "Longtext"),			// 0-4 294 967 295 bytes	极大文本数据
};

UENUM(BlueprintType)
enum class ELeafSpatialColumnType : uint8
{
	/*SpatialType*/														// 用于存储空间数据（地理信息、几何图形等）
	GEOMETRY = 0			UMETA(DisplayName = "Geometry"),					
	POINT					UMETA(DisplayName = "Point"),							
	LINESTRING				UMETA(DisplayName = "Linestring"),						
	POLYGON					UMETA(DisplayName = "Polygon"),							
	MULTIPOINT				UMETA(DisplayName = "Multipoint"),						
	MULTILINESTRING			UMETA(DisplayName = "Multilinestring"),					
	MULTIPOLYGON			UMETA(DisplayName = "Multipolygon"),					
	GEOMETRYCOLLECTION = 7	UMETA(DisplayName = "Geometrycollection")			
};

UENUM(BlueprintType)
enum class ELeafSqlTransactionIsolationLevel : uint8
{
	None = 0,
	Read_Commited,
	Repeatable_Read,
	Serializable,
	Read_Uncommitted
};

UENUM(BlueprintType)
enum class ELeafSqlWhereValueOperator : uint8
{
	Equals = 0				UMETA(DisplayName = "="),
	NotEquals				UMETA(DisplayName = "!="),
	GreaterThan				UMETA(DisplayName = ">"),
	LessThan				UMETA(DisplayName = "<"),
	GreaterThanOrEqualTo	UMETA(DisplayName = ">="),
	LessThanOrEqualTo		UMETA(DisplayName = "<=")
};

UENUM(BlueprintType)
enum class ELeafSqlWhereLogicOperator : uint8
{
	AND = 0			UMETA(DisplayName = "and"),
	OR				UMETA(DisplayName = "or")
};

USTRUCT(BlueprintType)
struct LEAFMYSQL_API FLeafSQLWhereString
{
	GENERATED_USTRUCT_BODY()
	FLeafSQLWhereString() : WhereString(TEXT("None")) {}
	FLeafSQLWhereString(const FString& InStr) : WhereString(InStr) {}
	FLeafSQLWhereString(const TCHAR* InStr) : WhereString(InStr) {}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "LeafMySQL|LeafSQLWhereString")
	FString WhereString;

	[[nodiscard]] FORCEINLINE const TCHAR* operator*() const
	{
		return WhereString.IsEmpty() ? TEXT("") : *WhereString;
	}
};