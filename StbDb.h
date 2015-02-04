/*
    Copyright 2014, Paul Wong

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

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
