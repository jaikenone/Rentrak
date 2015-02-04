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

#include "StbDb.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <fstream>

using namespace std;

// public
int StbDb::Initialize()
{
	int retval = 0;
	if ((retval = OpenDb()) == SQLITE_OK)
	{
		if ((retval = CreateTable()) != SQLITE_OK)
		{
			// error
			LogError("Missing table.");
		}
	}
	else
	{
		// error
		LogError("Unable to open database.");
	}

	return retval;
} // end StbDb::Initialize


int StbDb::Select(const char* select, const char* filter, const char* order
	, const char* group)
{
	int retval = 0;
	char* errMsg = 0;
	const char* data = "";
	
	// select 
	stringstream sql;
	sql << "SELECT " << select << " FROM " << TABLE_NAME;

	// where
	if (filter != NULL)
	{
		sql << " WHERE " << filter;
	}

	// group by
	if (group != NULL)
	{
		sql << " GROUP BY " << group;
	}

	// order by
	if (order != NULL)
	{
		sql << " ORDER BY " << order;
	}

	// all done
	sql << ";";

	cout << "QUERY: " << sql.str() << endl << endl;

	if ((retval = sqlite3_exec(db, (sql.str()).c_str(), callback, (void*)data, &errMsg)) != SQLITE_OK)
	{
		stringstream ss;
		ss << "SQL Error: " << errMsg;
		LogError((ss.str()).c_str());
	}
	cout << endl;

	return retval;
} // end StbDb::Select


int StbDb::Import(const char* fileName)
{
	int retval = 0;
	int count = 0;

	ifstream file(fileName);
	if (file.good())
	{
		string line;

		while (getline(file, line))
		{
			stringstream linestream(line);
			string stb, title, provider, date, rev, view_time;

			getline(linestream, stb, '|');
			getline(linestream, title, '|');
			getline(linestream, provider, '|');
			getline(linestream, date, '|');
			getline(linestream, rev, '|');
			getline(linestream, view_time, '|');

			// ignore the header row
			if (date.compare("DATE"))
			{
				if (UpdateOrInsert(stb.c_str(), title.c_str(), provider.c_str()
					, date.c_str(), rev.c_str(), view_time.c_str()) == 0)
					count++;
			}
		}
	}
	else
	{
		retval = -1;
	}

	if (count == 1)
		cout << "1 record found." << endl;
	else
		cout << count << " records found." << endl;

	file.close();
	return retval;
} // end StbDb::Import


int StbDb::FixUpTime(const char* view_time)
{
	stringstream time(view_time);
	string hours, minutes;
	getline(time, hours, ':');
	getline(time, minutes, ':');
	return (atoi(hours.c_str()) * 60 + atoi(minutes.c_str()));
} // end StbDb::FixUpTime

int StbDb::UpdateOrInsert(const char* stb, const char* title, const char* provider
	, const char* date, const char* rev, const char* view_time)
{
	int retval = 0;
	char* errMsg = 0;

	// fix up time
	ostringstream ominutes;
	ominutes << FixUpTime(view_time);

	stringstream sql;
	sql << "UPDATE " << TABLE_NAME << " SET PROVIDER = '" << provider
		<< "', REV=" << rev << ", VIEW_TIME=" << ominutes.str()
		<< " WHERE STB='" << stb << "' AND TITLE='" << title << "' AND DATE='" << date << "';";

	if ((retval = sqlite3_exec(db, (sql.str()).c_str(), NULL, 0, &errMsg)) != SQLITE_OK)
	{
		stringstream ss;
		ss << "SQL Error: " << errMsg;
		LogError((ss.str()).c_str());
	}
	else
	{
		int cnt = sqlite3_changes(db);
		if (cnt == 0)
		{
			sql.clear();
			// insert instead
			sql << "INSERT INTO " << TABLE_NAME << " (STB, TITLE, PROVIDER, DATE, REV, VIEW_TIME) "
				<< "VALUES ('" << stb << "', '" << title << "', '" << provider << "', '" 
				<< date << "', " << rev << ", " << ominutes.str() << ");";

			

			if ((retval = sqlite3_exec(db, (sql.str()).c_str(), NULL, 0, &errMsg)) != SQLITE_OK)
			{
				stringstream ss;
				ss << "SQL Error: " << errMsg;
				LogError((ss.str()).c_str());
			}
		}
	}
	return retval;
} // end StbDb::UpdateOrInsert

void StbDb::CloseDb()
{
	// close it and eat any errors
	sqlite3_close(db);
	
} // end StbDb::CloseDb()

// private
int StbDb::OpenDb()
{
	int retval = 0;

	retval = sqlite3_open(dbName.c_str(), &db);

	return retval;
} // StbDb::OpenDb()

int StbDb::CreateTable()
{
	int retval = 0;
	char* errMsg = 0;
	stringstream sql;
	sql << "CREATE TABLE IF NOT EXISTS " << TABLE_NAME
		<< "(STB CHAR(64) NOT NULL"
		<< ", TITLE CHAR(64) NOT NULL" 
		<< ", PROVIDER CHAR(64) NOT NULL" 
		<< ", DATE TEXT NOT NULL" 
		<< ", REV REAL NOT NULL" 
		<< ", VIEW_TIME INT NOT NULL);";  // store as minutes need to be clarified, spec said hours:seconds.

	if ((retval = sqlite3_exec(db, (sql.str()).c_str(), NULL, 0, &errMsg)) != SQLITE_OK)
	{
		stringstream ss;
		ss << "SQL Error: " << errMsg;
		LogError((ss.str()).c_str());
	}
	return retval;
} // end StbDb::CreateTable()


void StbDb::LogError(const char* errorMsg)
{
	cout << "***ERROR: " << errorMsg << endl;
	cout << sqlite3_errmsg(db) << endl;
}  // end StbDb::LogError


static int callback(void *data, int argc, char **argv, char **azColName)
{
	int i;
	//fprintf(stderr, "%s: ", (const char*)data);
	for (i = 0; i<argc; i++)
	{
		if (i != 0)
			cout << ", ";
		cout << (argv[i] ? argv[i] : "NULL");
	}
	cout << endl;
	return 0;
} // end callback
