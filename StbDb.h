#include <stdio.h>
#include <sqlite3.h> 
#include <iostream>
#include <string>

using namespace std;

static const char* TABLE_NAME = "STB_TABLE";
static int callback(void *data, int argc, char **argv, char **azColName);

class StbDb
{
public:

	StbDb(const char* name)
	{
		dbName = name;
	}

	~StbDb()
	{
		CloseDb();
	}

	int Initialize();
	int Select(const char* select, const char* filter, const char* order
		, const char* group);
	int Import(const char* fileName);
	int UpdateOrInsert(const char* stb, const char* title, const char* provider
		, const char* date, const char* rev, const char* view_time);
	void CloseDb();

private:
	string dbName;
	sqlite3 *db;
	int OpenDb();
	int CreateTable();
	void LogError(const char* errorMsg);
	int FixUpTime(const char* view_time);
};