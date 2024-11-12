// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#pragma once

#pragma push_macro("check")   // store 'check' macro current definition
#undef check  // undef to avoid conflicts
#define STATIC_CONCPP
#include "jdbc/cppconn/prepared_statement.h"
#include "jdbc/cppconn/driver.h"
#include "jdbc/cppconn/exception.h"
#include "jdbc/mysql_driver.h"
#pragma pop_macro("check")  // restore definition

#include "CoreMinimal.h"
#include "DataType/LeafMySQLBlueprintType.h"

namespace LeafMySql
{
	struct FLeafMySqlLinkInfo
	{
		sql::mysql::MySQL_Driver* Driver = NULL;
		sql::Connection* Connection = NULL;

		FORCEINLINE bool IsConnectionValid() const { return (!Connection->isClosed()) && Connection->isValid(); }
		sql::Connection* Connect(const struct FLeafMySqlLoginInfo& InLoginInfo);
		sql::Connection* Connect_ForSocket(const struct FLeafMySqlLoginInfo& InLoginInfo, const FString& InSocketPath);
		sql::Connection* Connect_ForPipe(const struct FLeafMySqlLoginInfo& InLoginInfo, const FString& InPipeName);
		sql::Connection* Connect_ForSharedMemory(const struct FLeafMySqlLoginInfo& InLoginInfo, const FString& InSharedMemoryBaseName);
		sql::Statement* GetStatement();
		sql::DatabaseMetaData* GetMetaData();
		void Destory();
	private:
		sql::SQLString DefaultDataBaseName = "";
		sql::Statement* Statement = NULL;
		sql::DatabaseMetaData* DS_MetaData = NULL;
	};

	struct FLeafMySqlLoginInfo
	{
		FLeafMySqlLoginInfo()
		{
			Port = 3306;
			SourceSqlServerAddress = TEXT("localhost");
			SqlServerAddress = FString::Printf(TEXT("localhost:%d"), Port);
			UserName = TEXT("root");
			Password = TEXT("");
			DefaultDataBase = TEXT("");
		}
		FLeafMySqlLoginInfo(const FLeafSQLLoginInfo InInfo)
		{
			if (InInfo.Port.IsNumeric())
			{
				int32 i = FCString::Atoi(*InInfo.Port);
				Port = i > 0 && i <= 65535 ? i : 3306;
			}
			else
			{
				ensureMsgf(0, TEXT("Port is not valid! now use default 3306 replace"));
				Port = 3306;
			}

			if (InInfo.SqlServerAddress.IsEmpty())
			{
				checkf(0, TEXT("Sql Server Address is empty!"));
			}
			SourceSqlServerAddress = InInfo.SqlServerAddress;
			SqlServerAddress = FString::Printf(TEXT("%s:%d"), *InInfo.SqlServerAddress, Port);
			UserName = InInfo.UserName.IsEmpty() ? TEXT("root") : InInfo.UserName;
			Password = InInfo.Password.IsEmpty() ? TEXT("") : InInfo.Password;
			DefaultDataBase = InInfo.DefaultDataBase;
		}
		FORCEINLINE FString GetLinkSqlServerAddress()const
		{
			return FString::Printf(TEXT("tcp://%s:%d"), *SourceSqlServerAddress, Port);
		}
		FORCEINLINE void SetSqlServerAddress(const FString& InSqlServerAddress, const unsigned short InPort)
		{
			Port = InPort;
			SqlServerAddress = FString::Printf(TEXT("%s:%d"), *InSqlServerAddress, Port);
		}
		FORCEINLINE void SetPort(const unsigned short InNewPort) { Port = InNewPort; }
		FORCEINLINE unsigned short GetPort() const { return Port; }
		FString UserName;
		FString Password;
		FString DefaultDataBase;
		FString SourceSqlServerAddress;
	private:
		FString SqlServerAddress;
		unsigned short Port;
	};

	namespace EDataBaseCharacterSet
	{
		enum Type : uint8
		{
			utf8mb4,
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
			latin_swedish_ci,
		};

		FString GetDataBaseCharacterSetString(const EDataBaseCharacterSet::Type InCharacterSet);
	}

	namespace ELeafMySqlValueType
	{
		enum Type : uint8
		{
			None = 0,						// 无效类型
											// 
			/*IntegerType*/					// 
			TINYINT = 1,					// 1 Bytes	(-128，127)	(0，255)	小整数值
			SMALLINT,						// 2 Bytes	(-32 768，32 767)	(0，65 535)	大整数值
			MEDIUMINT,						// 3 Bytes	(-8 388 608，8 388 607)	(0，16 777 215)	大整数值
			INTEGER,						// 4 Bytes	(-2 147 483 648，2 147 483 647)	(0，4 294 967 295)	大整数值
			BIGINT = 5,						// 8 Bytes	(-9,223,372,036,854,775,808，9 223 372 036 854 775 807)	(0，18 446 744 073 709 551 615)	极大整数值
											// 
			/*floatType*/					//	可以为其指定MD值 
			FLOAT = 6,						// 4 Bytes	(-3.402 823 466 E+38，-1.175 494 351 E-38)，0，(1.175 494 351 E-38，3.402 823 466 351 E+38)	0，(1.175 494 351 E-38，3.402 823 466 E+38)	单精度浮点数值
			DOUBLE,							// 8 Bytes	(-1.797 693 134 862 315 7 E+308，-2.225 073 858 507 201 4 E-308)，0，(2.225 073 858 507 201 4 E-308，1.797 693 134 862 315 7 E+308)	0，(2.225 073 858 507 201 4 E-308，1.797 693 134 862 315 7 E+308)	双精度浮点数值
			DECIMAL = 8,					// 对DECIMAL(M,D) ，如果M>D，为M+2否则为D+2	依赖于M和D的值	依赖于M和D的值	小数值
											// 
			/*DateType*/					// 
			DATE = 9,						// 3 Bytes	1000-01-01/9999-12-31	YYYY-MM-DD	日期值
			TIME,							// 3 Bytes	'-838:59:59'/'838:59:59'	HH:MM:SS	时间值或持续时间
			YEAR,							// 1 Bytes	1901/2155	YYYY	年份值
			DATETIME,						// 8 Bytes	'1000-01-01 00:00:00' 到 '9999-12-31 23:59:59'	YYYY-MM-DD hh:mm:ss	混合日期和时间值
		/*------------------------------------------------------------------------------------------------------------------------------------------------------*/
			TIMESTAMP = 13,					// 4 Bytes	'1970-01-01 00:00:01' UTC 到 '2038-01-19 03:14:07' UTC
											//			结束时间是第 2147483647 秒，北京时间 2038 - 1 - 19 11:14 : 07，格林尼治时间 2038年1月19日 凌晨 03 : 14 : 07
											//			YYYY - MM - DD hh : mm:ss	
											//			混合日期和时间值，时间戳
		/*------------------------------------------------------------------------------------------------------------------------------------------------------*/
											// 
			/*CharType*/					// 
			CHAR = 14,						// 0-255 bytes	定长字符串
			VARCHAR,						// 0-65535 bytes	变长字符串
			TINYBLOB,						// 0-255 bytes	不超过 255 个字符的二进制字符串
			TINYTEXT,						// 0-255 bytes	短文本字符串
			BLOB,							// 0-65 535 bytes	二进制形式的长文本数据
			TEXT,							// 0-65 535 bytes	长文本数据
			MEDIUMBLOB,						// 0-16 777 215 bytes	二进制形式的中等长度文本数据
			MEDIUMTEXT,						// 0-16 777 215 bytes	中等长度文本数据
			LONGBLOB,						// 0-4 294 967 295 bytes	二进制形式的极大文本数据
			LONGTEXT = 23,					// 0-4 294 967 295 bytes	极大文本数据
											// 
			/*EnumType*/					// 
			ENUM = 24,						// 枚举类型，用于存储单一值，可以选择一个预定义的集合。
											// 
			/*SetType*/						// 
			SET = 25,						// 集合类型，用于存储多个值，可以选择多个预定义的集合。
											// 
			/*SpatialType*/					// 用于存储空间数据（地理信息、几何图形等）
			GEOMETRY = 26,					// 
			POINT,							// 
			LINESTRING,						// 
			POLYGON,						// 
			MULTIPOINT,						// 
			MULTILINESTRING,				// 
			MULTIPOLYGON,					// 
			GEOMETRYCOLLECTION = 33			// 
		};

		FString GetLeafMySqlValueTypeString(const ELeafMySqlValueType::Type InValueType);
		ELeafMySqlValueType::Type GetLeafMySqlValueTypeFromString(const FString& InStr);

		//无效类型
		FORCEINLINE bool IsNotValidType(const ELeafMySqlValueType::Type InValueType) { return InValueType <= 0; }
		//包括 float 和 int
		FORCEINLINE bool IsNumericType(const ELeafMySqlValueType::Type InValueType) { return InValueType >= 1 && InValueType <= 8; }
		//所有的整型
		FORCEINLINE bool IsInegertype(const ELeafMySqlValueType::Type InValueType) { return InValueType >= 1 && InValueType <= 5; }
		//所有的浮点
		FORCEINLINE bool IsFloatType(const ELeafMySqlValueType::Type InValueType) { return InValueType >= 6 && InValueType <= 8; }
		//时间类型
		FORCEINLINE bool IsDateType(const ELeafMySqlValueType::Type InValueType) { return InValueType >= 9 && InValueType <= 13; }
		//字符类型
		FORCEINLINE bool IsCharType(const ELeafMySqlValueType::Type InValueType) { return InValueType >= 14 && InValueType <= 23; }
		//空间类型
		FORCEINLINE bool IsSpatialType(const ELeafMySqlValueType::Type InValueType) { return InValueType >= 26 && InValueType <= 33; }
	}

	namespace ELeafMySqlTransactionIsolationLevelType
	{
		enum Type : uint8
		{
			None,
			Read_Commited,
			Repeatable_Read,
			Serializable,
			Read_Uncommitted
		};

		FString GetLeafMySqlTransactionIsolationLevelTypeeString(const ELeafMySqlTransactionIsolationLevelType::Type InTransactionIsolationLevelTypeType);
		ELeafMySqlTransactionIsolationLevelType::Type GetLeafMySqlTransactionIsolationLevelTypeFromString(const FString& InStr);
	}

	namespace Command
	{
		struct FLeafMySqlCommandBase
		{
		public:
			FLeafMySqlCommandBase(FLeafMySqlLinkInfo& InLinkInfoPtr)
				:LinkInfoPtr(&InLinkInfoPtr), Error(FString()), MySqlLinkError(TEXT("LinkInfoPtr is not valid!!!"))
			{}
			virtual ~FLeafMySqlCommandBase() {}
			virtual FString GetExecutedCommand() const = 0;
			virtual FString ExeCommand();
			
		protected:
			FLeafMySqlLinkInfo* LinkInfoPtr;
		public:
			FString Error;
			FString MySqlLinkError;
		private:
			FLeafMySqlCommandBase() {}
		};

		namespace DataBase
		{
			struct FLeafMySqlDataBaseCreate : public FLeafMySqlCommandBase
			{
				FLeafMySqlDataBaseCreate(FLeafMySqlLinkInfo& InLinkInfoPtr, const FString& InDataBaseName)
					:FLeafMySqlCommandBase(InLinkInfoPtr), DataBaseName(InDataBaseName),
					CharacterSet(TEXT("utf8mb4")), Collate(TEXT("utf8mb4_general_ci")), bCheckIsExists(true)
				{}

				FLeafMySqlDataBaseCreate(FLeafMySqlLinkInfo& InLinkInfoPtr, const FString& InDataBaseName,
					const EDataBaseCharacterSet::Type InCharacterSet, const FString& InCollate, const bool bInCheckIsExists = true)
					:FLeafMySqlCommandBase(InLinkInfoPtr), DataBaseName(InDataBaseName),
					CharacterSet(GetDataBaseCharacterSetString(InCharacterSet)), Collate(InCollate), bCheckIsExists(bInCheckIsExists)
				{}

				FLeafMySqlDataBaseCreate(FLeafMySqlLinkInfo& InLinkInfoPtr, const FString& InDataBaseName,
					const FString& InCharacterSet, const FString& InCollate, const bool bInCheckIsExists = true)
					:FLeafMySqlCommandBase(InLinkInfoPtr), DataBaseName(InDataBaseName),
					CharacterSet(InCharacterSet), Collate(InCollate), bCheckIsExists(bInCheckIsExists)
				{}

				virtual FString GetExecutedCommand() const override;

				FString DataBaseName;
				FString CharacterSet;	//默认字符集
				FString Collate;	//默认排序规则
				bool bCheckIsExists; //检查库是否已经存在
			private:

				FLeafMySqlDataBaseCreate();
			};

			struct FLeafMySqlDataBaseUpdateSettings : public FLeafMySqlCommandBase
			{
				FLeafMySqlDataBaseUpdateSettings(FLeafMySqlLinkInfo& InLinkInfoPtr, const FString& InDataBaseName,
					const EDataBaseCharacterSet::Type InCharacterSet, const FString& InCollate)
					:FLeafMySqlCommandBase(InLinkInfoPtr), DataBaseName(InDataBaseName),
					CharacterSet(GetDataBaseCharacterSetString(InCharacterSet)), Collate(InCollate)
				{}

				FLeafMySqlDataBaseUpdateSettings(FLeafMySqlLinkInfo& InLinkInfoPtr, const FString& InDataBaseName,
					const FString& InCharacterSet, const FString& InCollate)
					:FLeafMySqlCommandBase(InLinkInfoPtr), DataBaseName(InDataBaseName),
					CharacterSet(InCharacterSet), Collate(InCollate)
				{}

				virtual FString GetExecutedCommand() const override;

				FString DataBaseName;
				FString CharacterSet;	//默认字符集
				FString Collate;	//默认排序规则
			private:
				FLeafMySqlDataBaseUpdateSettings();
			};

			struct FLeafMySqlDataBaseDelete : public FLeafMySqlCommandBase
			{
				FLeafMySqlDataBaseDelete(FLeafMySqlLinkInfo& InLinkInfoPtr, const FString& InDataBaseName,
					const bool bInCheckIsExists = true)
					:FLeafMySqlCommandBase(InLinkInfoPtr), DataBaseName(InDataBaseName),
					bCheckIsExists(bInCheckIsExists)
				{}

				virtual FString GetExecutedCommand() const override;

				FString DataBaseName;
				bool bCheckIsExists; //检查库是否已经存在
			private:

				FLeafMySqlDataBaseDelete();
			};
		}

		namespace DataTable
		{
			struct FLeafMySqlDataTableParamInfoBase
			{
				FLeafMySqlDataTableParamInfoBase(
					const FString& InParamName,
					const ELeafMySqlValueType::Type InParamType,
					const bool bInUnsigned = false,
					const bool bInNotNull = false,
					const bool bInUnique = false,
					const bool bInAutoIncrement = false,
					const FString& InDefaultValue = FString(),
					const FString& InComment = FString(),
					const FString& InChecker = FString()
					)
					:
					ParamName(InParamName), ParamType(InParamType),
					bUnsigned(bInUnsigned), bNotNull(bInNotNull), bUnique(bInUnique),
					bAutoIncrement(bInAutoIncrement), IsDefaultValueValid(false),
					CharLength(0), DECIMAL_M(0), DECIMAL_D(0), EnumOrSet_Init(FString()),
					Comment(InComment), Checker(InChecker)
				{
					SetDefaultParamValue(InDefaultValue);
				}

				FLeafMySqlDataTableParamInfoBase(
					const FString& InParamName,
					const uint64 InCharLength,
					const ELeafMySqlValueType::Type InParamType,
					const bool bInUnsigned = false,
					const bool bInNotNull = false,
					const bool bInUnique = false,
					const bool bInAutoIncrement = false,
					const FString& InDefaultValue = FString(),
					const FString& InComment = FString(),
					const FString& InChecker = FString()
				)
					:
					ParamName(InParamName), ParamType(InParamType),
					bUnsigned(bInUnsigned), bNotNull(bInNotNull), bUnique(bInUnique),
					bAutoIncrement(bInAutoIncrement), IsDefaultValueValid(false),
					CharLength(InCharLength), DECIMAL_M(0), DECIMAL_D(0), EnumOrSet_Init(FString()),
					Comment(InComment), Checker(InChecker)
				{
					SetDefaultParamValue(InDefaultValue);
				}

				FLeafMySqlDataTableParamInfoBase(
					const FString& InParamName,
					const uint64 InDECIMAL_M,
					const uint64 InDECIMAL_D,
					const ELeafMySqlValueType::Type InParamType,
					const bool bInUnsigned = false,
					const bool bInNotNull = false,
					const bool bInUnique = false,
					const bool bInAutoIncrement = false,
					const FString& InDefaultValue = FString(),
					const FString& InComment = FString(),
					const FString& InChecker = FString()
				)
					:
					ParamName(InParamName), ParamType(InParamType),
					bUnsigned(bInUnsigned), bNotNull(bInNotNull), bUnique(bInUnique),
					bAutoIncrement(bInAutoIncrement), IsDefaultValueValid(false),
					CharLength(0), DECIMAL_M(InDECIMAL_M), DECIMAL_D(InDECIMAL_D), EnumOrSet_Init(FString()),
					Comment(InComment), Checker(InChecker)
				{
					SetDefaultParamValue(InDefaultValue);
				}

				FLeafMySqlDataTableParamInfoBase(
					const FString& InParamName,
					const ELeafMySqlValueType::Type InParamType,
					const FString& InEnumOrSet_Init,
					const bool bInUnsigned = false,
					const bool bInNotNull = false,
					const bool bInUnique = false,
					const bool bInAutoIncrement = false,
					const FString& InDefaultValue = FString(),
					const FString& InComment = FString(),
					const FString& InChecker = FString()
				)
					:
					ParamName(InParamName), ParamType(InParamType),
					bUnsigned(bInUnsigned), bNotNull(bInNotNull), bUnique(bInUnique),
					bAutoIncrement(bInAutoIncrement), 
					DefaultValue(InDefaultValue),IsDefaultValueValid(false),
					CharLength(0), DECIMAL_M(0), DECIMAL_D(0), EnumOrSet_Init(InEnumOrSet_Init),
					Comment(InComment), Checker(InChecker)
				{}

				
				virtual ~FLeafMySqlDataTableParamInfoBase() {}

				virtual FString GetParamString() const;

				bool IsNumericType() const;

				void SetDefaultParamValue(const FString& InNewDefaultParamValue);
				void SetDefaultParamValue(const int32 InNewDefaultParamValue);
				void SetDefaultParamValue(const int64 InNewDefaultParamValue);
				void SetDefaultParamValue(const float InNewDefaultParamValue);
				void SetDefaultParamValue(const double InNewDefaultParamValue);
				void ClearDefaultParamValue();

				FORCEINLINE void SetParamName(const FString& InNewParamName) { ParamName = InNewParamName; }
				FORCEINLINE void SetParamType(const ELeafMySqlValueType::Type InNewParamType) { ParamType = InNewParamType; }
				FORCEINLINE void SetUnsigned(const bool InNewUnsigned) { bUnsigned = InNewUnsigned; }
				FORCEINLINE void SetNotNull(const bool InNewNotNull) { bNotNull = InNewNotNull; }
				FORCEINLINE void SetUnique(const bool InNewUnique) { bUnique = InNewUnique; }
				FORCEINLINE void SetAutoIncrement(const bool InNewAutoIncrement) { bAutoIncrement = InNewAutoIncrement; }
				FORCEINLINE void SetCharLength(const uint64 InNewCharLength) { CharLength = InNewCharLength; }
				FORCEINLINE void SetDECIMAL_M(const uint64 InNewDECIMAL_M) { DECIMAL_M = InNewDECIMAL_M; }
				FORCEINLINE void SetDECIMAL_D(const uint64 InNewDECIMAL_D) { DECIMAL_D = InNewDECIMAL_D; }
				FORCEINLINE void SetEnumOrSet_Init(const FString& InNewEnumOrSet_Init) { EnumOrSet_Init = InNewEnumOrSet_Init; }
				FORCEINLINE void SetComment(const FString& InNewComment) { Comment = InNewComment; }
				FORCEINLINE void SetChecker(const FString& InNewChecker) { Checker = InNewChecker; }

				FORCEINLINE FString GetParamName() const { return ParamName; }

				FORCEINLINE bool IsParamValid() const { return !(ParamName.IsEmpty() && ParamType == ELeafMySqlValueType::None); }

				static FLeafMySqlDataTableParamInfoBase NoValidType;
				
				
			private:
				FString ParamName;
				ELeafMySqlValueType::Type ParamType;
				//bool bPrimaryKey;		//是否为主键，如果设置了多个将会变为复合键*
				bool bUnsigned;			//参数是否不为负数，使用于数字类型
				bool bNotNull;			//参数是否不允许为空,与bUnique不能同时为true
				bool bUnique;			//唯一性，唯一约束可以有多个也可以没有*
				bool bAutoIncrement;	//参数是否需要自动增长，类似ID，使用在数字类型上且一个表只有一列（参数）可以设置，一般与主键以前使用，一个表只可以有一列
				FString DefaultValue;	//默认值
				bool IsDefaultValueValid;
				uint64 CharLength;		//只适用于字符类型
				uint64 DECIMAL_M;		//只适用于DECIMAL，float，double，设置其M
				uint64 DECIMAL_D;		//只适用于DECIMAL，float，double，设置其D
				FString EnumOrSet_Init;	//用于枚举字段初始化
				FString Comment;		//字段注释*
				FString Checker;		//检查字段信息比如性别可以判断gender == M OR gender == F*
				
				FLeafMySqlDataTableParamInfoBase()
				{
					//NoValidType Constuct
					ParamType = ELeafMySqlValueType::None;
					CharLength = 0;
					DECIMAL_M = 0;
					DECIMAL_D = 0;
				}
			};

			struct FLeafMySqlDataTableParamDisplay
			{
				FString ParamName;
				ELeafMySqlValueType::Type ParamType;
				bool bIsPrimaryKey;		//是否为主键
				bool bNotNull;			//参数是否不允许为空
				bool bUnique;			//唯一性，唯一约束可以有多个也可以没有*
				bool bUnsigned;			//参数是否不为负数，使用于数字类型
				bool bAutoIncrement;	//参数是否需要自动增长，类似ID
				FString DefaultValue;	//默认值
				uint64 CharLength;		//只适用于字符类型
				uint64 DECIMAL_M;		//只适用于DECIMAL，设置其M
				uint64 DECIMAL_D;		//只适用于DECIMAL，设置其D

				FLeafMySqlDataTableParamDisplay(
					const FString& InParamName,
					const ELeafMySqlValueType::Type InParamType,
					const bool bInIsPrimaryKey = false,
					const bool bInNotNull = false,
					const bool bInUnsigned = false,
					const bool bInAutoIncrement = false,
					const FString& InDefaultValue = TEXT(""),
					const uint64 InCharLength = 0,
					const uint64 InDECIMAL_M = 0,
					const uint64 InDECIMAL_D = 0
				);

				FLeafMySqlDataTableParamDisplay(const sql::ResultSet* rs);

				FString ToString() const;
			};

			struct FLeafMySqlDataTableCreate : public FLeafMySqlCommandBase
			{
				FLeafMySqlDataTableCreate(FLeafMySqlLinkInfo& InLinkInfoPtr, const FString& InDataTableName,
					const bool bInCheckIsExists = true, const FString& InEngine = TEXT("InnoDB"),
					const FString InCharset = TEXT("utf8mb4"), const FString& InCollate = TEXT("utf8mb4_general_ci"))
					:FLeafMySqlCommandBase(InLinkInfoPtr), DataTableName(InDataTableName),
					bCheckIsExists(bInCheckIsExists), Engine(InEngine),
					Collate(InCollate),Charset(InCharset), PrimaryKeyName(FName())
				{}

				FLeafMySqlDataTableCreate(FLeafMySqlLinkInfo& InLinkInfoPtr, const FString& InDataTableName,
					const FString& InCollate,
					const bool bInCheckIsExists = true, const FString& InEngine = TEXT("InnoDB"),
					const  EDataBaseCharacterSet::Type InCharset = EDataBaseCharacterSet::utf8mb4)
					:FLeafMySqlCommandBase(InLinkInfoPtr), DataTableName(InDataTableName),
					bCheckIsExists(bInCheckIsExists), Engine(InEngine),
					Collate(InCollate),Charset(GetDataBaseCharacterSetString(InCharset)), PrimaryKeyName(FName())
				{}

				void AddPrimaryKey(const FLeafMySqlDataTableParamInfoBase& InPrimaryKey);
				void AddOrUpdateParam(const FLeafMySqlDataTableParamInfoBase InNewParam);

				FORCEINLINE bool HasPrimaryKey() const { return (!PrimaryKeyName.IsNone()) && Params.Contains(PrimaryKeyName) && PrimaryKeyNames.Num() > 0; }

				virtual FString GetExecutedCommand() const override;

				FString DataTableName;
				bool bCheckIsExists;	//检查表是否已经存在
				FString Engine;			//储存引擎
				FString Collate;		//排序规则
				FString Charset;		//字符集,编码格式

			private:
				FLeafMySqlDataTableCreate();

				FName PrimaryKeyName;			//主键
				TArray<FName> PrimaryKeyNames;	//如果设置了多个将会变为复合键
				TMap<FName, FLeafMySqlDataTableParamInfoBase> Params;
			};

			struct FLeafMySqlDataTableDelete : public FLeafMySqlCommandBase
			{
				FLeafMySqlDataTableDelete(FLeafMySqlLinkInfo& InLinkInfoPtr, const FString& InDataTableName,
					const bool bInCheckIsExists = true, const bool bInJustClearData = false)
					:FLeafMySqlCommandBase(InLinkInfoPtr), DataTableName(InDataTableName),
					bCheckIsExists(bInCheckIsExists), bJustClearData(bInJustClearData)
				{}

				virtual FString GetExecutedCommand() const override;

				FString DataTableName;
				bool bCheckIsExists;	//检查表是否已经存在
				bool bJustClearData;	//清空表中的所有数据，但不会删除表本身

			private:
				FLeafMySqlDataTableDelete();
			};
			
			struct FLeafMySqlDataTableModify : public FLeafMySqlCommandBase
			{
				FLeafMySqlDataTableModify(FLeafMySqlLinkInfo& InLinkInfoPtr, 
					const FString& InDataTableName, const FString& InModifyColumnName)
					:FLeafMySqlCommandBase(InLinkInfoPtr), DataTableName(InDataTableName),
					ModifyColumnName(InModifyColumnName)
				{}

			protected:
				virtual FString GetModifyString() const =0;
				virtual FString GetModifyParamString() const =0;
			public:
				FString DataTableName;
				FString ModifyColumnName;	//需要修改的数据表列名

			private:
				FLeafMySqlDataTableModify();
				virtual FString GetExecutedCommand() const override;
			};

			struct FLeafMySqlDataTableModify_AddColumn : public FLeafMySqlDataTableModify
			{
				FLeafMySqlDataTableModify_AddColumn(FLeafMySqlLinkInfo& InLinkInfoPtr,
					const FString& InDataTableName,
					const FLeafMySqlDataTableParamInfoBase& InAddedColumnInfo)
					:FLeafMySqlDataTableModify(InLinkInfoPtr, InDataTableName, TEXT("")),
					NewColumnParam(InAddedColumnInfo) {}

				virtual FString GetModifyString() const override { return TEXT("ADD"); }
				virtual FString GetModifyParamString() const override { return NewColumnParam.IsParamValid() ? NewColumnParam.GetParamString() : FString(); }

				FORCEINLINE void SetNewColumnParam(const FLeafMySqlDataTableParamInfoBase& InNewParam) { NewColumnParam = InNewParam; }
			private:
				FLeafMySqlDataTableModify_AddColumn();
				FLeafMySqlDataTableParamInfoBase NewColumnParam;
			};

			struct FLeafMySqlDataTableModify_RemoveColumn : public FLeafMySqlDataTableModify
			{
				FLeafMySqlDataTableModify_RemoveColumn(FLeafMySqlLinkInfo& InLinkInfoPtr,
					const FString& InDataTableName, const FString& InModifyColumnName)
					:FLeafMySqlDataTableModify(InLinkInfoPtr, InDataTableName, InModifyColumnName)
					{}

				virtual FString GetModifyString() const override { return TEXT("DROP"); }
				virtual FString GetModifyParamString() const override { return FString(TEXT("")); }
			private:
				FLeafMySqlDataTableModify_RemoveColumn();
			};

			struct FLeafMySqlDataTableModify_ModifyColumn : public FLeafMySqlDataTableModify
			{
				FLeafMySqlDataTableModify_ModifyColumn(FLeafMySqlLinkInfo& InLinkInfoPtr,
					const FString& InDataTableName,
					const FLeafMySqlDataTableParamInfoBase& InModifyColumnParam)
					:FLeafMySqlDataTableModify(InLinkInfoPtr, InDataTableName, TEXT("")),
					WantModifyColumnParam(InModifyColumnParam) {}
				virtual FString GetModifyString() const override { return TEXT("MODIFY"); }
				virtual FString GetModifyParamString() const override { return WantModifyColumnParam.IsParamValid() ? WantModifyColumnParam.GetParamString() : FString(); }

				FORCEINLINE void SetNewColumnParam(const FLeafMySqlDataTableParamInfoBase& InNewParam) { WantModifyColumnParam = InNewParam; }
			private:
				FLeafMySqlDataTableModify_ModifyColumn();
				FLeafMySqlDataTableParamInfoBase WantModifyColumnParam;
			};

			struct FLeafMySqlDataTableModify_RenameColumn : public FLeafMySqlDataTableModify
			{
				FLeafMySqlDataTableModify_RenameColumn(
					FLeafMySqlLinkInfo& InLinkInfoPtr,
					const FString& InDataTableName,
					const FString& InOldColumnName,
					const FString& InNewColumnName)
					:FLeafMySqlDataTableModify(InLinkInfoPtr, InDataTableName, InOldColumnName),
					NewColumnName(InNewColumnName), WantModifyColumnParam(FLeafMySqlDataTableParamInfoBase::NoValidType)
				{}

				FLeafMySqlDataTableModify_RenameColumn(
					FLeafMySqlLinkInfo& InLinkInfoPtr,
					const FString& InDataTableName,
					const FString& InNewColumnName, 
					const FLeafMySqlDataTableParamInfoBase& InModifyColumnParam)
					:FLeafMySqlDataTableModify(InLinkInfoPtr, InDataTableName, TEXT("")),
					NewColumnName(InNewColumnName), WantModifyColumnParam(InModifyColumnParam)
				{}

				virtual FString GetModifyString() const override { return WantModifyColumnParam.IsParamValid() ? TEXT("CHANGE") : TEXT("RENAME COLUMN"); }
				virtual FString GetModifyParamString() const override;

				FORCEINLINE void SetNewColumnParam(const FLeafMySqlDataTableParamInfoBase& InNewParam) { WantModifyColumnParam = InNewParam; }
				FORCEINLINE void SetNewNewColumnName(const FString& InNewNewColumnName) { NewColumnName = InNewNewColumnName; }
				FORCEINLINE FString GetNewColumnName() const { return NewColumnName; }
			private:
				FLeafMySqlDataTableModify_RenameColumn();
				FString NewColumnName;
				FLeafMySqlDataTableParamInfoBase WantModifyColumnParam;
			};
		}
	}

	struct FLeafMySqlValueHandler
	{
		FLeafMySqlValueHandler(const FString& InVal);
		FLeafMySqlValueHandler(const int32 InVal);
		FLeafMySqlValueHandler(const int64 InVal);
		FLeafMySqlValueHandler(const float InVal);
		FLeafMySqlValueHandler(const double InVal);

		FORCEINLINE FString Get(const bool InNeedCheckOperator = false) const 
		{ 
			return InNeedCheckOperator ?
			CheckHaveOperation() ? SourceStr : OutputeValue 
				: OutputeValue; 
		}
		FORCEINLINE FString GetSource() const { return SourceStr; }

		FString GetFormatSource() const;

	private:
		bool CheckHaveOperation() const;
		FLeafMySqlValueHandler();
		FString OutputeValue;
		FString SourceStr;
	};
}