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
#include <iostream>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include "StbDb.h"

using namespace std;

struct globalArgs_t
{
    const char *select;			// -s selected fields
    const char *order;		    // -o order fields, comma dilimited
	const char *filter;		    // -f order fields, comma dilimited
	const char *database;		// -d database name
	const char *group;			// -g group by, comma dilimited

} globalArgs;

static const char *DATABASE_NAME = "STB.db";
static const char *optString = "s:o:f:d:g:h?";

void print_help()
{
	cout << endl << endl;
	cout << "Usage: query" << endl;
	cout << "-s         selected fields comma delimited." << endl;
	cout << "-o         order fields comma delimited." << endl;
	cout << "-f         filter to apply." << endl;
	cout << "-g         group by" << endl;
	cout << "-d         database name (defaults to "<< DATABASE_NAME << ")." << endl;
	cout << "-h or -?   this help screen." << endl << endl;
	cout << "Example:" << endl;
	cout << "	./query -s TITLE,REV,DATE -o DATE,TITLE -f \"TITLE='the matrix'\" -g DATE" << endl << endl;
	cout << "Aggregates: MIN, MAX, SUM, COUNT, GROUP_CONCAT." << endl;
	cout << "Example:" << endl;
	cout << "	./query -s \"TITLE, SUM(REV), DATE, GROUP_CONCAT(STB)\" -o DATE,TITLE -g STB" << endl << endl;
	
	cout << endl << endl;
	exit(EXIT_FAILURE);
}

void init(void)
{
	// Initialize globalArgs before we get to work.
	globalArgs.select = NULL;
	globalArgs.order = NULL;
	globalArgs.filter = NULL;
	globalArgs.group = NULL;
	globalArgs.database = DATABASE_NAME;
}

int main(int argc, char *argv[])
{
	int opt = 0;

	if (argc < 1)
		print_help();

	init();

	/* Process the arguments with getopt(), then
	* populate globalArgs.
	*/
	opt = getopt(argc, argv, optString);
	while (opt != -1)
	{
		switch (opt)
		{
		case 's':
			// select
			globalArgs.select = optarg;
			break;

		case 'o':
			// order
			globalArgs.order = optarg;
			break;

		case 'f':
			// filter
			globalArgs.filter = optarg;
			break;

		case 'd':
			// database
			globalArgs.database = optarg;
			break;

		case 'g':
			// group by
			globalArgs.group = optarg;
			break;

		case 'h':	/* fall-through is intentional */
		case '?':
			print_help();
			break;

		default:
			/* You won't actually get here. */
			break;
		}

		opt = getopt(argc, argv, optString);
	}

	if (false)
	{
		printf("Pausing for debugging. Press return to continue...");
		getchar();
	}

	if (NULL != globalArgs.select && NULL != globalArgs.database)
	{
		// made it here, so run
		cout << "Select:   " << globalArgs.select << endl;
		if (globalArgs.order != NULL)
			cout << "Order:    " << globalArgs.order << endl;
		if (globalArgs.filter != NULL)
			cout << "Filter:   " << globalArgs.filter << endl;
		if (globalArgs.group != NULL)
			cout << "Group:   " << globalArgs.group << endl;
		cout << "Database: " << globalArgs.database << endl << endl;

		StbDb* sdb = new StbDb(globalArgs.database);

		if (sdb->Initialize() == 0)
			sdb->Select(globalArgs.select, globalArgs.filter, globalArgs.order
				, globalArgs.group);

		delete sdb;
	}
	else
	{
		print_help();
	}

	return EXIT_SUCCESS;
}
