#include <stdio.h>
#include <iostream>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include "StbDb.h"

using namespace std;

struct globalArgs_t
{
    const char *source;         // -s source file
    const char *database;       // -d database name can inlcude path

} globalArgs;

static const char *DATABASE_NAME = "STB.db";
static const char *optString = "s:d:h?";

void print_help()
{
	cout << endl << endl;
	cout << "Usage: importer" << endl;
	cout << "-s         source file containing record for import." << endl;
	cout << "-d         database name (defaults to " << DATABASE_NAME << ")." << endl;
	cout << "-h or -?   this help screen." << endl << endl;
	cout << "Example:" << endl << endl;
	cout << "	./importer -s stb_import1.txt";
	cout << endl << endl;
	exit(EXIT_FAILURE);
}

void init(void)
{
	// Initialize globalArgs before we get to work.
	globalArgs.source = NULL;
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
			// source
			globalArgs.source = optarg;
			break;

		case 'd':
			// database
			globalArgs.database = optarg;
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

	if (NULL != globalArgs.source && NULL != globalArgs.database)
	{
		// made it here, so run
		cout << "Source:   " << globalArgs.source << endl;
		cout << "Database: " << globalArgs.database << endl << endl;

		StbDb* sdb = new StbDb(globalArgs.database);

		if (sdb->Initialize() == 0)
			sdb->Import(globalArgs.source);

		delete sdb;
	}
	else
	{
		print_help();
	}

	return EXIT_SUCCESS;
}