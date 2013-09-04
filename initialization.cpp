#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "environment.h"
#include "define.h"

using namespace std;

job Initialization( environment_variable &ev, int argc, char** argv )
{
	job myjob = COMPILE;
	
	ev.headerDirs.push_back("./");

	ev.programPath = "./myprogram";

	ev.compiler = "g++";

	ev.flags = "";

	for( int i = 1; i < argc; i++)
	{
		/* set output name */
		if( (strcmp(argv[i], "-o") == 0) || (strcmp(argv[i], "--output") == 0) )
		{
			i++;
			ev.programPath = string(argv[i]);
		}
		/* set compiler */
		else if( (strcmp(argv[i], "-c") == 0) || (strcmp(argv[i], "--compiler") == 0) )
		{
			i++;
			ev.compiler = string(argv[i]);
		}
		/* set header dir */
		else if( (strcmp(argv[i], "-i") == 0) || (strcmp(argv[i], "--include") == 0) )
		{
			i++;
			ev.headerDirs.push_back(string(argv[i]));
		}
		/* set source dir */
		else if( (strcmp(argv[i], "-s") == 0) || (strcmp(argv[i], "--source") == 0) )
		{
			i++;
			ev.sourceDirs.push_back(string(argv[i]));
		}
		/* print help info */
		else if( (strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0) )
		{
			myjob = HELP;
		}
		else if( strcmp(argv[i], "clean") == 0 )
		{
			myjob = CLEAN;
		}
	}

	if( ev.sourceDirs.size() == 0 )
	{	
		ev.sourceDirs = ev.headerDirs;
	}		

	return myjob;
}
