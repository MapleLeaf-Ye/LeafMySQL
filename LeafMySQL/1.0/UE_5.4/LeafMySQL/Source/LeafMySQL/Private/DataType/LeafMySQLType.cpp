// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

#include "DataType/LeafMySQLType.h"


sql::Connection* LeafMySql::FLeafMySqlLinkInfo::Connect(const struct FLeafMySqlLoginInfo& InLoginInfo)
{
	if (Driver && Connection)
	{
		return Connection;
	}
	Driver = sql::mysql::get_mysql_driver_instance();
	if (!Driver)
	{
		return NULL;
	}
	std::string serverAddress = TCHAR_TO_UTF8(*InLoginInfo.GetLinkSqlServerAddress());
	std::string userName = TCHAR_TO_UTF8(*InLoginInfo.UserName);
	std::string password = TCHAR_TO_UTF8(*InLoginInfo.Password);
	std::string defaultDataBase = TCHAR_TO_UTF8(*InLoginInfo.DefaultDataBase);
	DefaultDataBaseName = defaultDataBase;

	Connection = Driver->connect(serverAddress, userName, password);
	
	if (DefaultDataBaseName.length() > 0)
	{
		Connection->setSchema(DefaultDataBaseName);
	}

	return Connection;
}

sql::Connection* LeafMySql::FLeafMySqlLinkInfo::Connect_ForSocket(const FLeafMySqlLoginInfo& InLoginInfo, const FString& InSocketPath)
{
	if (Driver && Connection)
	{
		return Connection;
	}
	Driver = sql::mysql::get_mysql_driver_instance();
	if (!Driver)
	{
		return NULL;
	}
	sql::SQLString serverAddress = TCHAR_TO_UTF8(*InLoginInfo.SourceSqlServerAddress);
	sql::SQLString userName = TCHAR_TO_UTF8(*InLoginInfo.UserName);
	sql::SQLString password = TCHAR_TO_UTF8(*InLoginInfo.Password);
	sql::SQLString defaultDataBase = TCHAR_TO_UTF8(*InLoginInfo.DefaultDataBase);
	sql::SQLString sockPath = TCHAR_TO_UTF8(*InSocketPath);
	DefaultDataBaseName = defaultDataBase;

	sql::ConnectOptionsMap connection_Properties;

	// 设置连接属性
	connection_Properties["hostName"] = serverAddress;  // 主机名
	connection_Properties["port"] = 0;                // 端口设置为0表示使用Unix域套接字
	connection_Properties["userName"] = userName;  // 用户名
	connection_Properties["password"] = password;  // 密码
	connection_Properties["schema"] = defaultDataBase;    // 数据库名
	connection_Properties["socket"] = sockPath;  // Unix域套接字路径


	Connection = Driver->connect(connection_Properties);

	return Connection;
}

sql::Connection* LeafMySql::FLeafMySqlLinkInfo::Connect_ForPipe(const FLeafMySqlLoginInfo& InLoginInfo, const FString& InPipeName)
{
	if (Driver && Connection)
	{
		return Connection;
	}
	Driver = sql::mysql::get_mysql_driver_instance();
	if (!Driver)
	{
		return NULL;
	}
	sql::SQLString serverAddress = TCHAR_TO_UTF8(*InLoginInfo.SourceSqlServerAddress);
	sql::SQLString userName = TCHAR_TO_UTF8(*InLoginInfo.UserName);
	sql::SQLString password = TCHAR_TO_UTF8(*InLoginInfo.Password);
	sql::SQLString defaultDataBase = TCHAR_TO_UTF8(*InLoginInfo.DefaultDataBase);
	sql::SQLString pipeName = TCHAR_TO_UTF8(*InPipeName);
	DefaultDataBaseName = defaultDataBase;

	sql::ConnectOptionsMap connection_Properties;

	// 设置连接属性
	connection_Properties["hostName"] = serverAddress;  // 主机名
	connection_Properties["port"] = 0;                // 端口设置为0表示使用命名管道
	connection_Properties["userName"] = userName;  // 用户名
	connection_Properties["password"] = password;  // 密码
	connection_Properties["schema"] = defaultDataBase;    // 数据库名
	connection_Properties["pipe"] = pipeName;  // 命名管道名称


	Connection = Driver->connect(connection_Properties);

	return Connection;
}

sql::Connection* LeafMySql::FLeafMySqlLinkInfo::Connect_ForSharedMemory(const FLeafMySqlLoginInfo& InLoginInfo, const FString& InSharedMemoryBaseName)
{
	if (Driver && Connection)
	{
		return Connection;
	}
	Driver = sql::mysql::get_mysql_driver_instance();
	if (!Driver)
	{
		return NULL;
	}
	sql::SQLString serverAddress = TCHAR_TO_UTF8(*InLoginInfo.SourceSqlServerAddress);
	sql::SQLString userName = TCHAR_TO_UTF8(*InLoginInfo.UserName);
	sql::SQLString password = TCHAR_TO_UTF8(*InLoginInfo.Password);
	sql::SQLString defaultDataBase = TCHAR_TO_UTF8(*InLoginInfo.DefaultDataBase);
	sql::SQLString sharedMemoryBaseName = TCHAR_TO_UTF8(*InSharedMemoryBaseName);
	DefaultDataBaseName = defaultDataBase;

	sql::ConnectOptionsMap connection_Properties;

	// 设置连接属性
	connection_Properties["hostName"] = serverAddress;  // 主机名
	connection_Properties["port"] = 0;                // 端口设置为0表示使用共享内存
	connection_Properties["userName"] = userName;  // 用户名
	connection_Properties["password"] = password;  // 密码
	connection_Properties["schema"] = defaultDataBase;    // 数据库名
	connection_Properties["sharedMemoryBaseName"] = sharedMemoryBaseName;  // 共享内存基名称


	Connection = Driver->connect(connection_Properties);

	return Connection;
}

sql::Statement* LeafMySql::FLeafMySqlLinkInfo::GetStatement()
{
	if (!Statement)
	{
		Statement = Connection->createStatement();
	}
	return Statement;
}

sql::DatabaseMetaData* LeafMySql::FLeafMySqlLinkInfo::GetMetaData()
{
	DS_MetaData = Connection->getMetaData();
	return DS_MetaData;
}

void LeafMySql::FLeafMySqlLinkInfo::Destory()
{
	DS_MetaData = NULL;
	if (Statement)
	{
		Statement->close();
		if (Statement)
		{
			delete Statement;
		}
		Statement = NULL;
	}
	Connection->clearWarnings();
	Connection->close();
	if (Connection)
	{
		delete Connection;
	}
	Connection = NULL;
	/*if (Driver)
	{
		delete Driver;
	}
	Driver = NULL;*/
}

FString LeafMySql::Command::DataBase::FLeafMySqlDataBaseCreate::GetExecutedCommand() const
{
	FString command;
	if (bCheckIsExists)
	{
		command = FString::Printf(TEXT("CREATE DATABASE IF NOT EXISTS %s CHARACTER SET %s COLLATE %s;"), *DataBaseName, *CharacterSet, *Collate);
	}
	else
	{
		command = FString::Printf(TEXT("CREATE DATABASE %s CHARACTER SET %s COLLATE %s;"), *DataBaseName, *CharacterSet, *Collate);
	}
	return command;
}

FString LeafMySql::Command::DataBase::FLeafMySqlDataBaseUpdateSettings::GetExecutedCommand() const
{
	FString command;

	command = FString::Printf(TEXT("ALTER DATABASE %s DEFAULT CHARACTER SET %s DEFAULT COLLATE %s;"), *DataBaseName, *CharacterSet, *Collate);

	return command;
}

FString LeafMySql::Command::DataBase::FLeafMySqlDataBaseDelete::GetExecutedCommand() const
{
	FString command;
	if (bCheckIsExists)
	{
		command = FString::Printf(TEXT("DROP DATABASE IF EXISTS %s;"), *DataBaseName);
	}
	else
	{
		command = FString::Printf(TEXT("DROP DATABASE %s;"), *DataBaseName);
	}
	return command;
}

FString LeafMySql::Command::FLeafMySqlCommandBase::ExeCommand()
{
	if (!LinkInfoPtr)
	{
		Error = MySqlLinkError;
		return Error;
	}

	FString currentCommand = GetExecutedCommand();
	if (currentCommand.IsEmpty())
	{
		Error = TEXT("Current want execute sql command is not valid!!!");
		return Error;
	}
	sql::ResultSet* outRS = NULL;
	try
	{
		outRS = LinkInfoPtr->GetStatement()->executeQuery(TCHAR_TO_UTF8(*currentCommand));
	}
	catch (const sql::SQLException& e)
	{
		Error = UTF8_TO_TCHAR(e.what());
		if (Error.Equals(TEXT("No result available"), ESearchCase::IgnoreCase))
		{
			return FString(TEXT("Successed"));
		}
		return Error;
	}
	catch (const std::exception& e)
	{
		Error = UTF8_TO_TCHAR(e.what());
		if (Error.Equals(TEXT("No result available"), ESearchCase::IgnoreCase))
		{
			return FString(TEXT("Successed"));
		}
		return Error;
	}
	Error = FString();
	if (outRS)
	{
		Error = UTF8_TO_TCHAR(outRS->getString(1).c_str());
		if (Error.Equals(TEXT("No result available"),ESearchCase::IgnoreCase))
		{
			return FString(TEXT("Successed"));
		}
		return Error;
	}
	return FString(TEXT("Successed"));
}

FString LeafMySql::EDataBaseCharacterSet::GetDataBaseCharacterSetString(const EDataBaseCharacterSet::Type InCharacterSet)
{
	if (InCharacterSet == EDataBaseCharacterSet::utf8mb4)
	{
		return TEXT("utf8mb4");
	}
	else if (InCharacterSet == EDataBaseCharacterSet::utf8mb3)
	{
		return TEXT("utf8mb3");
	}
	else if (InCharacterSet == EDataBaseCharacterSet::utf32)
	{
		return TEXT("utf32");
	}
	else if (InCharacterSet == EDataBaseCharacterSet::utf16)
	{
		return TEXT("utf16");
	}
	else if (InCharacterSet == EDataBaseCharacterSet::utf16le)
	{
		return TEXT("utf16le");
	}
	else if (InCharacterSet == EDataBaseCharacterSet::gbk)
	{
		return TEXT("gbk");
	}
	else if (InCharacterSet == EDataBaseCharacterSet::gb2312)
	{
		return TEXT("gb2312");
	}
	else if (InCharacterSet == EDataBaseCharacterSet::big5)
	{
		return TEXT("big5");
	}
	else if (InCharacterSet == EDataBaseCharacterSet::ascii)
	{
		return TEXT("ascii");
	}
	else if (InCharacterSet == EDataBaseCharacterSet::latin1)
	{
		return TEXT("latin1");
	}
	else if (InCharacterSet == EDataBaseCharacterSet::latin_swedish_ci)
	{
		return TEXT("latin_swedish_ci");
	}
	return TEXT("utf8mb4");
}

FString LeafMySql::ELeafMySqlValueType::GetLeafMySqlValueTypeString(const ELeafMySqlValueType::Type InValueType)
{
	if (InValueType == ELeafMySqlValueType::None)
	{
		return TEXT("None");
	}
	else if (InValueType == ELeafMySqlValueType::TINYINT)
	{
		return TEXT("TINYINT");
	}
	else if (InValueType == ELeafMySqlValueType::SMALLINT)
	{
		return TEXT("SMALLINT");
	}
	else if (InValueType == ELeafMySqlValueType::MEDIUMINT)
	{
		return TEXT("MEDIUMINT");
	}
	else if (InValueType == ELeafMySqlValueType::INTEGER)
	{
		return TEXT("INTEGER");
	}
	else if (InValueType == ELeafMySqlValueType::BIGINT)
	{
		return TEXT("BIGINT");
	}
	else if (InValueType == ELeafMySqlValueType::FLOAT)
	{
		return TEXT("FLOAT");
	}
	else if (InValueType == ELeafMySqlValueType::DOUBLE)
	{
		return TEXT("DOUBLE");
	}
	else if (InValueType == ELeafMySqlValueType::DECIMAL)
	{
		return TEXT("DECIMAL");
	}
	else if (InValueType == ELeafMySqlValueType::DATE)
	{
		return TEXT("DATE");
	}
	else if (InValueType == ELeafMySqlValueType::TIME)
	{
		return TEXT("TIME");
	}
	else if (InValueType == ELeafMySqlValueType::YEAR)
	{
		return TEXT("YEAR");
	}
	else if (InValueType == ELeafMySqlValueType::DATETIME)
	{
		return TEXT("DATETIME");
	}
	else if (InValueType == ELeafMySqlValueType::TIMESTAMP)
	{
		return TEXT("TIMESTAMP");
	}
	else if (InValueType == ELeafMySqlValueType::CHAR)
	{
		return TEXT("CHAR");
	}
	else if (InValueType == ELeafMySqlValueType::VARCHAR)
	{
		return TEXT("VARCHAR");
	}
	else if (InValueType == ELeafMySqlValueType::TINYBLOB)
	{
		return TEXT("TINYBLOB");
	}
	else if (InValueType == ELeafMySqlValueType::TINYTEXT)
	{
		return TEXT("TINYTEXT");
	}
	else if (InValueType == ELeafMySqlValueType::BLOB)
	{
		return TEXT("BLOB");
	}
	else if (InValueType == ELeafMySqlValueType::TEXT)
	{
		return TEXT("TEXT");
	}
	else if (InValueType == ELeafMySqlValueType::MEDIUMBLOB)
	{
		return TEXT("MEDIUMBLOB");
	}
	else if (InValueType == ELeafMySqlValueType::MEDIUMTEXT)
	{
		return TEXT("MEDIUMTEXT");
	}
	else if (InValueType == ELeafMySqlValueType::LONGBLOB)
	{
		return TEXT("LONGBLOB");
	}
	else if (InValueType == ELeafMySqlValueType::LONGTEXT)
	{
		return TEXT("LONGTEXT");
	}
	else if (InValueType == ELeafMySqlValueType::ENUM)
	{
		return TEXT("ENUM");
	}
	else if (InValueType == ELeafMySqlValueType::SET)
	{
		return TEXT("SET");
	}
	else if (InValueType == ELeafMySqlValueType::GEOMETRY)
	{
		return TEXT("GEOMETRY");
	}
	else if (InValueType == ELeafMySqlValueType::POINT)
	{
		return TEXT("POINT");
	}
	else if (InValueType == ELeafMySqlValueType::LINESTRING)
	{
		return TEXT("LINESTRING");
	}
	else if (InValueType == ELeafMySqlValueType::POLYGON)
	{
		return TEXT("POLYGON");
	}
	else if (InValueType == ELeafMySqlValueType::MULTIPOINT)
	{
		return TEXT("MULTIPOINT");
	}
	else if (InValueType == ELeafMySqlValueType::MULTILINESTRING)
	{
		return TEXT("MULTILINESTRING");
	}
	else if (InValueType == ELeafMySqlValueType::MULTIPOLYGON)
	{
		return TEXT("MULTIPOLYGON");
	}
	else if (InValueType == ELeafMySqlValueType::GEOMETRYCOLLECTION)
	{
		return TEXT("GEOMETRYCOLLECTION");
	}

	return TEXT("None");
}

LeafMySql::ELeafMySqlValueType::Type LeafMySql::ELeafMySqlValueType::GetLeafMySqlValueTypeFromString(const FString& InStr)
{
	int32 unsignedPos = InStr.Find(TEXT("unsigned"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	if (unsignedPos != -1)
	{
		FString newStr = InStr.Left(unsignedPos - 1);
		return GetLeafMySqlValueTypeFromString(newStr);
	}
	if (InStr.Equals(TEXT("none"), ESearchCase::IgnoreCase) || InStr.Equals(TEXT(""), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::None;
	}
	else if (InStr.Equals(TEXT("tinyint"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::TINYINT;
	}
	else if (InStr.Equals(TEXT("smallint"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::SMALLINT;
	}
	else if (InStr.Equals(TEXT("mediumint"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::MEDIUMINT;
	}
	else if (InStr.Equals(TEXT("integer"), ESearchCase::IgnoreCase) || InStr.Equals(TEXT("int"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::INTEGER;
	}
	else if (InStr.Equals(TEXT("bigint"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::BIGINT;
	}
	else if (InStr.Equals(TEXT("float"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::FLOAT;
	}
	else if (InStr.Equals(TEXT("double"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::DOUBLE;
	}
	else if (InStr.Equals(TEXT("decimal"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::DECIMAL;
	}
	else if (InStr.Equals(TEXT("date"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::DATE;
	}
	else if (InStr.Equals(TEXT("time"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::TIME;
	}
	else if (InStr.Equals(TEXT("year"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::YEAR;
	}
	else if (InStr.Equals(TEXT("datetime"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::DATETIME;
	}
	else if (InStr.Equals(TEXT("timestamp"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::TIMESTAMP;
	}
	else if (InStr.Equals(TEXT("char"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::CHAR;
	}
	else if (InStr.Equals(TEXT("varchar"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::VARCHAR;
	}
	else if (InStr.Equals(TEXT("tinyblob"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::TINYBLOB;
	}
	else if (InStr.Equals(TEXT("tinytext"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::TINYTEXT;
	}
	else if (InStr.Equals(TEXT("blob"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::BLOB;
	}
	else if (InStr.Equals(TEXT("text"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::TEXT;
	}
	else if (InStr.Equals(TEXT("mediumblob"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::MEDIUMBLOB;
	}
	else if (InStr.Equals(TEXT("mediumtext"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::MEDIUMTEXT;
	}
	else if (InStr.Equals(TEXT("longblob"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::LONGBLOB;
	}
	else if (InStr.Equals(TEXT("longtext"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::LONGTEXT;
	}
	else if (InStr.Equals(TEXT("enum"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::ENUM;
	}
	else if (InStr.Equals(TEXT("set"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::SET;
	}
	else if (InStr.Equals(TEXT("geometry"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::GEOMETRY;
	}
	else if (InStr.Equals(TEXT("point"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::POINT;
	}
	else if (InStr.Equals(TEXT("linestring"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::LINESTRING;
	}
	else if (InStr.Equals(TEXT("polygon"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::POLYGON;
	}
	else if (InStr.Equals(TEXT("multipoint"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::MULTIPOINT;
	}
	else if (InStr.Equals(TEXT("multilinestring"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::MULTILINESTRING;
	}
	else if (InStr.Equals(TEXT("multipolygon"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::MULTIPOLYGON;
	}
	else if (InStr.Equals(TEXT("geometrycollection"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlValueType::GEOMETRYCOLLECTION;
	}
	
	return LeafMySql::ELeafMySqlValueType::None;
}

FString LeafMySql::Command::DataTable::FLeafMySqlDataTableParamInfoBase::GetParamString() const
{
	FString outString = ParamName;

	if (ParamType == ELeafMySqlValueType::None)
	{
		return FString();
	}

	if (ParamType == ELeafMySqlValueType::VARCHAR || ParamType == ELeafMySqlValueType::CHAR)
	{
		outString += TEXT(" ");
		outString += FString::Printf(TEXT("%s(%d)"), *ELeafMySqlValueType::GetLeafMySqlValueTypeString(ParamType), CharLength);
	}
	else if (ParamType == ELeafMySqlValueType::DECIMAL || 
		((ParamType == ELeafMySqlValueType::FLOAT || ParamType == ELeafMySqlValueType::DOUBLE) && (DECIMAL_M > 0 || DECIMAL_D > 0)))
	{
		outString += TEXT(" ");
		outString += FString::Printf(TEXT("%s(%d,%d)"), *ELeafMySqlValueType::GetLeafMySqlValueTypeString(ParamType), DECIMAL_M, DECIMAL_D);
	}
	else if (ParamType == ELeafMySqlValueType::ENUM || ParamType == ELeafMySqlValueType::SET)
	{
		outString += TEXT(" ");
		outString += FString::Printf(TEXT("%s(%s)"), *ELeafMySqlValueType::GetLeafMySqlValueTypeString(ParamType), *EnumOrSet_Init);
	}
	else if (ParamType != ELeafMySqlValueType::VARCHAR || ParamType != ELeafMySqlValueType::DECIMAL)
	{
		outString += TEXT(" ");
		outString += ELeafMySqlValueType::GetLeafMySqlValueTypeString(ParamType);
	}

	if (bUnsigned && IsNumericType())
	{
		outString += TEXT(" UNSIGNED");
	}

	if (bNotNull)
	{
		outString += TEXT(" NOT NULL");
	}
	else
	{
		outString += TEXT(" NULL");
	}

	if (bUnique)
	{
		outString += TEXT(" UNIQUE");
	}

	if (bAutoIncrement && IsNumericType())
	{
		outString += TEXT(" AUTO_INCREMENT");
	}

	if (IsDefaultValueValid)
	{
		outString += FString::Printf(TEXT(" DEFAULT %s"), *FLeafMySqlValueHandler(DefaultValue).Get());
	}

	if (!Comment.IsEmpty())
	{
		outString += FString::Printf(TEXT(" COMMENT %s"), *FLeafMySqlValueHandler(Comment).Get());
	}

	if (!Checker.IsEmpty())
	{
		outString += FString::Printf(TEXT(" CHECK(%s)"), *Checker);
	}

	return outString;
}

bool LeafMySql::Command::DataTable::FLeafMySqlDataTableParamInfoBase::IsNumericType() const
{
	/*return	(ParamType == ELeafMySqlValueType::TINYINT)		||
			(ParamType == ELeafMySqlValueType::SMALLINT)	||
			(ParamType == ELeafMySqlValueType::MEDIUMINT)	||
			(ParamType == ELeafMySqlValueType::INTEGER)		||
			(ParamType == ELeafMySqlValueType::BIGINT)		||
			(ParamType == ELeafMySqlValueType::FLOAT)		||
			(ParamType == ELeafMySqlValueType::DOUBLE)		||
			(ParamType == ELeafMySqlValueType::DECIMAL);*/
	return ELeafMySqlValueType::IsNumericType(ParamType);
}

void LeafMySql::Command::DataTable::FLeafMySqlDataTableParamInfoBase::SetDefaultParamValue(const FString& InNewDefaultParamValue)
{
	DefaultValue = InNewDefaultParamValue;
	IsDefaultValueValid = !DefaultValue.IsEmpty();
}

void LeafMySql::Command::DataTable::FLeafMySqlDataTableParamInfoBase::SetDefaultParamValue(const int32 InNewDefaultParamValue)
{
	if (IsNumericType())
	{
		DefaultValue = FString::Printf(TEXT("%d"), InNewDefaultParamValue);
		IsDefaultValueValid = true;
		return;
	}
	IsDefaultValueValid = false;
}

void LeafMySql::Command::DataTable::FLeafMySqlDataTableParamInfoBase::SetDefaultParamValue(const int64 InNewDefaultParamValue)
{
	if (IsNumericType())
	{
		DefaultValue = FString::Printf(TEXT("%lld"), InNewDefaultParamValue);
		IsDefaultValueValid = true;
		return;
	}
	
}

void LeafMySql::Command::DataTable::FLeafMySqlDataTableParamInfoBase::SetDefaultParamValue(const float InNewDefaultParamValue)
{
	if (IsNumericType())
	{
		DefaultValue = FString::SanitizeFloat(InNewDefaultParamValue);
		IsDefaultValueValid = true;
		return;
	}
}

void LeafMySql::Command::DataTable::FLeafMySqlDataTableParamInfoBase::SetDefaultParamValue(const double InNewDefaultParamValue)
{
	if (IsNumericType())
	{
		DefaultValue = FString::SanitizeFloat(InNewDefaultParamValue);
		IsDefaultValueValid = true;
		return;
	}
}

void LeafMySql::Command::DataTable::FLeafMySqlDataTableParamInfoBase::ClearDefaultParamValue()
{
	DefaultValue = FString(TEXT(""));
	IsDefaultValueValid = false;
	return;
}

LeafMySql::Command::DataTable::FLeafMySqlDataTableParamInfoBase LeafMySql::Command::DataTable::FLeafMySqlDataTableParamInfoBase::NoValidType = FLeafMySqlDataTableParamInfoBase();

void LeafMySql::Command::DataTable::FLeafMySqlDataTableCreate::AddPrimaryKey(const FLeafMySqlDataTableParamInfoBase& InPrimaryKey)
{
	if (InPrimaryKey.IsParamValid())
	{
		if (!Params.Contains(*InPrimaryKey.GetParamName()))
		{
			Params.Add(*InPrimaryKey.GetParamName(), InPrimaryKey);
			PrimaryKeyName = *InPrimaryKey.GetParamName();
			PrimaryKeyNames.Add(*InPrimaryKey.GetParamName());
		}
		else
		{
			PrimaryKeyNames.Add(*InPrimaryKey.GetParamName());
			PrimaryKeyName = PrimaryKeyNames[0];
		}
	}
}

void LeafMySql::Command::DataTable::FLeafMySqlDataTableCreate::AddOrUpdateParam(const FLeafMySqlDataTableParamInfoBase InNewParam)
{
	if (InNewParam.IsParamValid())
	{
		Params.Add(*InNewParam.GetParamName(), InNewParam);
	}
}

FString LeafMySql::Command::DataTable::FLeafMySqlDataTableCreate::GetExecutedCommand() const
{
	FString command;
	FString currentDataBase = UTF8_TO_TCHAR(LinkInfoPtr->Connection->getSchema().c_str());
	if (DataTableName.IsEmpty())
	{
		ensureMsgf(0, TEXT("Create new DataTable for [%s] fail,because new DataTable name is empty!"), *currentDataBase);
		return command;
	}
	if (!HasPrimaryKey())
	{
		ensureMsgf(0, TEXT("Create new DataTable for [%s] fail,because new DataTable dont have PrimaryKey,please use AddPrimaryKey method add!"), *currentDataBase);
		return command;
	}
	if (Params.Num() <= 0)
	{
		ensureMsgf(0, TEXT("Create new DataTable for [%s] fail,because new DataTable dont have any params,please use AddOrUpdateParam method add!"), *currentDataBase);
		return command;
	}

	if (bCheckIsExists)
	{
		command = FString::Printf(TEXT("CREATE TABLE IF NOT EXISTS %s ("), *DataTableName);
	}
	else
	{
		command = FString::Printf(TEXT("CREATE TABLE %s ("), *DataTableName);
	}

	for (auto& tmp : Params)
	{
		command += tmp.Value.GetParamString();
		command += TEXT(",");
	}
	FString primaryKeyName;
	if (PrimaryKeyNames.Num() > 1)
	{
		for (const auto tmp : PrimaryKeyNames)
		{
			primaryKeyName += tmp.ToString();
			primaryKeyName += TEXT(",");
		}
		primaryKeyName = primaryKeyName.Left(primaryKeyName.Len() - 1);
	}
	else
	{
		primaryKeyName = PrimaryKeyName.ToString();
	}
	command += FString::Printf(TEXT("PRIMARY KEY (%s))"), *primaryKeyName);
	command += FString::Printf(TEXT("ENGINE=%s DEFAULT CHARSET=%s COLLATE %s;"), *Engine, *Charset, *Collate);
	return command;
}

FString LeafMySql::Command::DataTable::FLeafMySqlDataTableDelete::GetExecutedCommand() const
{
	FString command;
	FString currentDataBase = UTF8_TO_TCHAR(LinkInfoPtr->Connection->getSchema().c_str());
	if (DataTableName.IsEmpty())
	{
		ensureMsgf(0, TEXT("Create new DataTable for [%s] fail,because new DataTable name is empty!"), *currentDataBase);
		return command;
	}

	if (bJustClearData)
	{
		command = FString::Printf(TEXT("TRUNCATE TABLE %s;"), *DataTableName);
		return command;
	}

	if (bCheckIsExists)
	{
		command = FString::Printf(TEXT("DROP TABLE IF EXISTS %s;"), *DataTableName);
	}
	else
	{
		command = FString::Printf(TEXT("DROP TABLE %s;"), *DataTableName);
	}
	return command;
}

FString LeafMySql::Command::DataTable::FLeafMySqlDataTableModify::GetExecutedCommand() const
{
	FString command;
	FString currentDataBase = UTF8_TO_TCHAR(LinkInfoPtr->Connection->getSchema().c_str());
	if (DataTableName.IsEmpty())
	{
		ensureMsgf(0, TEXT("Modify DataTable for [%s] fail,because the DataTable name is empty!"), *currentDataBase);
		return command;
	}
	/*if (ModifyColumnName.IsEmpty())
	{
		ensureMsgf(0, TEXT("Modify table [%s], from [%s] fail,because the DataTable column name is empty!"),*DataTableName, *currentDataBase);
		return command;
	}*/

	command = FString::Printf(TEXT("ALTER TABLE %s %s %s%s;"), *DataTableName, *GetModifyString(), *ModifyColumnName, *GetModifyParamString());

	return command;
}

FString LeafMySql::Command::DataTable::FLeafMySqlDataTableModify_RenameColumn::GetModifyParamString() const
{
	if (WantModifyColumnParam.IsParamValid())
	{
		return NewColumnName.IsEmpty() ? FString(TEXT("")) : FString::Printf(TEXT("%s %s"), *NewColumnName, *WantModifyColumnParam.GetParamString());
	}
	return NewColumnName.IsEmpty() ? FString(TEXT("")) : FString::Printf(TEXT(" TO %s"), *NewColumnName);
}

LeafMySql::Command::DataTable::FLeafMySqlDataTableParamDisplay::FLeafMySqlDataTableParamDisplay(const FString& InParamName, const ELeafMySqlValueType::Type InParamType, const bool bInIsPrimaryKey, const bool bInNotNull, const bool bInUnsigned, const bool bInAutoIncrement, const FString& InDefaultValue, const uint64 InCharLength, const uint64 InDECIMAL_M, const uint64 InDECIMAL_D)
	:ParamName(InParamName), ParamType(InParamType), bIsPrimaryKey(bInIsPrimaryKey),
	bNotNull(bInNotNull), bUnsigned(bInUnsigned), bAutoIncrement(bInAutoIncrement),
	DefaultValue(InDefaultValue), CharLength(InCharLength), DECIMAL_M(InDECIMAL_M), DECIMAL_D(InDECIMAL_D)
{}

LeafMySql::Command::DataTable::FLeafMySqlDataTableParamDisplay::FLeafMySqlDataTableParamDisplay(const sql::ResultSet * rs)
{
	ParamName = TEXT("ErrorType");
	ParamType = ELeafMySqlValueType::None;
	bIsPrimaryKey = false;
	bNotNull = false;
	bUnique = false;
	bUnsigned = false;
	bAutoIncrement = false;
	DefaultValue = TEXT("");
	CharLength = 0;
	DECIMAL_M = 0;
	DECIMAL_D = 0;
	if (!rs)
	{
		return;
	}
	
	ParamName = UTF8_TO_TCHAR(rs->getString(1).c_str());
	FString sParamType = UTF8_TO_TCHAR(rs->getString(2).c_str());
	FString sNotNull = UTF8_TO_TCHAR(rs->getString(3).c_str());
	FString sIsPrimaryKey = UTF8_TO_TCHAR(rs->getString(4).c_str());
	DefaultValue = UTF8_TO_TCHAR(rs->getString(5).c_str());
	FString sExtra = UTF8_TO_TCHAR(rs->getString(6).c_str());

	{	//Prase param type
		int32 left = INDEX_NONE;
		left = sParamType.Find(TEXT("("));
		if (left == INDEX_NONE)
		{
			bUnsigned = sParamType.Find(TEXT("unsigned"), ESearchCase::IgnoreCase, ESearchDir::FromEnd) != -1;
			ParamType = LeafMySql::ELeafMySqlValueType::GetLeafMySqlValueTypeFromString(sParamType);
			CharLength = 0;
			DECIMAL_M = 0;
			DECIMAL_D = 0;
		}
		else
		{
			//type varchar,DECIMAL
			FString realType = sParamType.Left(left);
			ParamType = LeafMySql::ELeafMySqlValueType::GetLeafMySqlValueTypeFromString(realType);
			FString sParamSize = sParamType.Right(sParamType.Len() - left - 1);
			int32 mid = INDEX_NONE;
			mid = sParamType.Find(TEXT(","));
			if (mid == INDEX_NONE)
			{
				//type varchar
				FString sSize = sParamSize.Left(sParamSize.Len() - 1);
				if (sSize.IsNumeric())
				{
					CharLength = FCString::Atoi64(*sSize);
				}
				else
				{
					CharLength = 0;
				}
			}
			else
			{
				//type DECIMAL
				FString sM = sParamType.Left(mid);
				FString sD = sParamType.Right(sParamType.Len() - mid);
				sD = sD.Left(sD.Len() - 1);
				if (sM.IsNumeric())
				{
					DECIMAL_M = FCString::Atoi64(*sM);
				}
				else
				{
					DECIMAL_M = 0;
				}

				if (sD.IsNumeric())
				{
					DECIMAL_D = FCString::Atoi64(*sD);
				}
				else
				{
					DECIMAL_D = 0;
				}
			}
		}
	}
	
	bIsPrimaryKey = sIsPrimaryKey.Equals(TEXT("PRI"), ESearchCase::IgnoreCase);
	bUnique = sIsPrimaryKey.Equals(TEXT("UNI"), ESearchCase::IgnoreCase);
	bNotNull = sNotNull.Equals(TEXT("YES"), ESearchCase::IgnoreCase);
	//bUnsigned = sExtra.Find(TEXT("unsigned")) != INDEX_NONE;
	bAutoIncrement = sExtra.Find(TEXT("auto_increment")) != INDEX_NONE;
}

FString LeafMySql::Command::DataTable::FLeafMySqlDataTableParamDisplay::ToString() const
{
	FString outInfo;
	if (ParamType == ELeafMySqlValueType::None)
	{
		return TEXT("Is a not valid Param!!!");
	}

	outInfo += FString::Printf(TEXT("[ "));
	outInfo += FString::Printf(TEXT("ParamName : %s"), *ParamName);
	outInfo += FString::Printf(TEXT(" --- "));

	if (ParamType != ELeafMySqlValueType::DECIMAL)
	{
		if (CharLength != 0)
		{
			outInfo += FString::Printf(TEXT("ParamType : %s(%ld)"), *GetLeafMySqlValueTypeString(ParamType), CharLength);
		}
		else
		{
			outInfo += FString::Printf(TEXT("ParamType : %s"), *GetLeafMySqlValueTypeString(ParamType));
		}
	}
	else
	{
		outInfo += FString::Printf(TEXT("ParamType : %s(%ld,%ld)"), *GetLeafMySqlValueTypeString(ParamType), DECIMAL_M, DECIMAL_D);
	}
	outInfo += FString::Printf(TEXT(" --- "));

	outInfo += FString::Printf(TEXT("IsPrimaryKey : %s --- "), *(bIsPrimaryKey ? FString(TEXT("True")) : FString(TEXT("False"))));
	outInfo += FString::Printf(TEXT("IsUniqueKey : %s --- "), *(bUnique ? FString(TEXT("True")) : FString(TEXT("False"))));
	outInfo += FString::Printf(TEXT("IsNotNull : %s --- "), *(bNotNull ? FString(TEXT("True")) : FString(TEXT("False"))));
	outInfo += FString::Printf(TEXT("IsUnsigned : %s --- "), *(bUnsigned ? FString(TEXT("True")) : FString(TEXT("False"))));
	outInfo += FString::Printf(TEXT("IsAutoIncrement : %s --- "), *(bAutoIncrement ? FString(TEXT("True")) : FString(TEXT("False"))));

	outInfo += FString::Printf(TEXT("DefaultValue : %s ]"), *(DefaultValue.IsEmpty() ? FString(TEXT("Not Value")) : *DefaultValue));
	
	return outInfo;
}

FString LeafMySql::ELeafMySqlTransactionIsolationLevelType::GetLeafMySqlTransactionIsolationLevelTypeeString(const ELeafMySqlTransactionIsolationLevelType::Type InTransactionIsolationLevelTypeType)
{
	if (InTransactionIsolationLevelTypeType == ELeafMySqlTransactionIsolationLevelType::Read_Commited)
	{
		return TEXT("READ COMMITED");
	}
	else if (InTransactionIsolationLevelTypeType == ELeafMySqlTransactionIsolationLevelType::Repeatable_Read)
	{
		return TEXT("REPEATABLE READ");
	}
	else if (InTransactionIsolationLevelTypeType == ELeafMySqlTransactionIsolationLevelType::Serializable)
	{
		return TEXT("SERIALIZABLE");
	}
	else if (InTransactionIsolationLevelTypeType == ELeafMySqlTransactionIsolationLevelType::Read_Uncommitted)
	{
		return TEXT("READ UNCOMMITTED");
	}
	else
	{
		return TEXT("None");
	}
}

LeafMySql::ELeafMySqlTransactionIsolationLevelType::Type LeafMySql::ELeafMySqlTransactionIsolationLevelType::GetLeafMySqlTransactionIsolationLevelTypeFromString(const FString& InStr)
{
	if (InStr.Equals(TEXT("read commited"), ESearchCase::IgnoreCase) || InStr.Equals(TEXT("ReadCommited"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlTransactionIsolationLevelType::Read_Commited;
	}
	else if (InStr.Equals(TEXT("repeatable read"), ESearchCase::IgnoreCase) || InStr.Equals(TEXT("RepeatableRead"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlTransactionIsolationLevelType::Repeatable_Read;
	}
	else if (InStr.Equals(TEXT("serializable"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlTransactionIsolationLevelType::Serializable;
	}
	else if (InStr.Equals(TEXT("read uncommitted"), ESearchCase::IgnoreCase) || InStr.Equals(TEXT("ReadUncommitted"), ESearchCase::IgnoreCase))
	{
		return LeafMySql::ELeafMySqlTransactionIsolationLevelType::Read_Uncommitted;
	}
	else
	{
		return LeafMySql::ELeafMySqlTransactionIsolationLevelType::None;
	}
}

LeafMySql::FLeafMySqlValueHandler::FLeafMySqlValueHandler(const FString& InVal)
{
	if (InVal.Len() < 2)
	{
		if (InVal.IsEmpty())
		{
			SourceStr = OutputeValue = TEXT("0");
			return;
		}
		else
		{
			OutputeValue = FString::Printf(TEXT("'%s'"), *InVal);
			SourceStr = InVal;
			return;
		}
	}
	FString lStr = InVal.Left(1);
	FString rStr = InVal.Right(1);
	if (lStr.Equals(TEXT("'"),ESearchCase::IgnoreCase) && rStr.Equals(TEXT("'"), ESearchCase::IgnoreCase))
	{
		OutputeValue = InVal;
		FString tmp = InVal.Left(InVal.Len() - 1);
		SourceStr = tmp.Right(tmp.Len() - 1);
		return;
	}
	else if (lStr.Equals(TEXT("\""), ESearchCase::IgnoreCase) && rStr.Equals(TEXT("\""), ESearchCase::IgnoreCase))
	{
		OutputeValue = InVal;
		FString tmp = InVal.Left(InVal.Len() - 1);
		SourceStr = tmp.Right(tmp.Len() - 1);
		return;
	}
	else
	{
		if (lStr.Equals(TEXT("'"), ESearchCase::IgnoreCase))
		{
			OutputeValue = FString::Printf(TEXT("%s'"), *InVal);
			FString tmp = InVal;
			SourceStr = tmp.Right(tmp.Len() - 1);
			return;
		}
		else if (rStr.Equals(TEXT("'"), ESearchCase::IgnoreCase))
		{
			OutputeValue = FString::Printf(TEXT("'%s"), *InVal);
			FString tmp = InVal;
			SourceStr = tmp.Left(tmp.Len() - 1);
			return;
		}
		else if (lStr.Equals(TEXT("\""), ESearchCase::IgnoreCase))
		{
			OutputeValue = FString::Printf(TEXT("%s\""), *InVal);
			FString tmp = InVal;
			SourceStr = tmp.Right(tmp.Len() - 1);
			return;
		}
		else if (rStr.Equals(TEXT("\""), ESearchCase::IgnoreCase))
		{
			OutputeValue = FString::Printf(TEXT("\"%s"), *InVal);
			FString tmp = InVal;
			SourceStr = tmp.Left(tmp.Len() - 1);
			return;
		}
		else
		{
			OutputeValue = FString::Printf(TEXT("'%s'"), *InVal);
			SourceStr = InVal;
			return;
		}
	}
}

LeafMySql::FLeafMySqlValueHandler::FLeafMySqlValueHandler(const int32 InVal)
{
	FLeafMySqlValueHandler(FString::Printf(TEXT("%d"), InVal));
}

LeafMySql::FLeafMySqlValueHandler::FLeafMySqlValueHandler(const int64 InVal)
{
	FLeafMySqlValueHandler(FString::Printf(TEXT("%lld"), InVal));
}

LeafMySql::FLeafMySqlValueHandler::FLeafMySqlValueHandler(const float InVal)
{
	FLeafMySqlValueHandler(FString::SanitizeFloat(InVal));
}

LeafMySql::FLeafMySqlValueHandler::FLeafMySqlValueHandler(const double InVal)
{
	FLeafMySqlValueHandler(FString::SanitizeFloat(InVal));
}

FString LeafMySql::FLeafMySqlValueHandler::GetFormatSource() const
{
	return SourceStr.IsNumeric() ? SourceStr : OutputeValue.Replace(TEXT("'"),TEXT("\""));
}

bool LeafMySql::FLeafMySqlValueHandler::CheckHaveOperation() const
{
	FDateTime date;
	const bool isDate = FDateTime::ParseIso8601(*SourceStr,date);
	if (isDate)
	{
		return false;
	}
	return SourceStr.Find(TEXT("+")) != -1 || SourceStr.Find(TEXT("-")) != -1 || SourceStr.Find(TEXT("*")) != -1 || SourceStr.Find(TEXT("/")) != -1;
}
