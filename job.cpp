#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <map>
#include "define.h"
#include "target.h"
#include "services.h"

using namespace std;

job Initialization(environment_variable &ev, int argc, char** argv)
{
	job myjob = COMPILE;

	ev.programPath = "./myprogram";

	ev.compiler = "g++";

	ev.flags = "";

	ev.headerDirs.push_back("./");
	ev.sourceDirs.push_back("./");

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
		}/* set header dir */
		else if( (strcmp(argv[i], "-i") == 0) || (strcmp(argv[i], "--include") == 0) )
		{
			i++;
			string tmp = argv[i];
			if(tmp[tmp.length()-1] != '/')
			{
				tmp += "/";
			}
			ev.headerDirs.push_back(tmp);
		}
		/* set source dir */
		else if( (strcmp(argv[i], "-s") == 0) || (strcmp(argv[i], "--source") == 0) )
		{
			i++;
			string tmp = argv[i];
			if(tmp[tmp.length()-1] != '/')
			{
				tmp += "/";
			}
			ev.sourceDirs.push_back(tmp);
			ev.sourceDirs.push_back(tmp);
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
		else
		{
			printf("ERROR: no option \"%s\". \nPlease use \"-h\" pr \"--help\" option to check EZmake usage.\n", argv[i]);
			exit(EXIT_FAILURE);
		}
	}

	if( ev.sourceDirs.size() == 0 )
	{	
		ev.sourceDirs = ev.headerDirs;
	}

	return myjob;
}

void Clean(environment_variable ev)
{
	string shellCommand;
	shellCommand = "rm -f *.o";
	printf("%s\n", shellCommand.c_str());
	system(shellCommand.c_str());
	//shellCommand = "rm -f " + ev.programPath;
	//system(shellCommand.c_str());
	return;
}

void Help()
{
	printf("Help Info:\n");
	printf("-o: Set output path and name. Default value is \"./myprogram\".\n");
	printf("-c: Set compiler. Default value is g++.\n");
	printf("-i: Set header dir. Default value is \"./\".\n");
	printf("-s: Set source dir. By default, it set to be same as header dir.\n");
	printf("-h: Print help info.\n");
	printf("clean: Remove object files and excutable file. Need to use -o to specify excutable file.\n");
	return;
}

void Compile(environment_variable ev)
{
	map<Target, bool> targets;
	/* no need to compile, included in other source file */
	vector<string> neglect;
	/* whether need to relink */
	bool reLink = false;
	for(int i = 0; i < ev.sourceDirs.size(); i++)
	{
		DIR* dirFile = opendir( ev.sourceDirs[i].c_str() );
		if( dirFile )
		{
			struct dirent* hFile;
			while( (hFile = readdir( dirFile )) != NULL )
			{
				if( StringEndWith(string(hFile->d_name), ".cpp") || StringEndWith( string(hFile->d_name), ".c" ) )
				{
					targets[Target(ev.sourceDirs[i], string(hFile->d_name))] = false;
				}
			}
			closedir( dirFile);
		}
		else
		{
			printf("I cannot open %s. This may be this dir is not accessible or memory is not enough to hold the whole dir.\n", ev.sourceDirs[i].c_str() );
			exit( EXIT_FAILURE );
		}
	}

	map<Target, bool, ClassTargetLess>::iterator it;
	for(it = targets.begin(); it != targets.end(); it++)
	{
		it->second = it->first.AnalyzeTarget(ev, neglect);
	}
	for(int i = 0; i < neglect.size(); i++)
	{
		unsigned suffixPosition = neglect[i].find_last_of("/");
		string dirName = string(neglect[i], 0, suffixPosition);
		string fileName = string(neglect[i], suffixPosition, neglect[i].length()-suffixPosition);
		targets.erase(Target(dirName, fileName));
	}

	/* compile targets */
	vector<string> objects;
	for(it = targets.begin(); it != targets.end(); it++)
	{
		objects.push_back(it->first.ObjectName());
		if(it->second)
		{
			it->first.Compile(ev);
		}
		if(CompareNewer(it->first.ObjectName(), ev.programPath))
		{
			reLink = true;
		}
	}

	if(reLink)
	{
		string linkCommand = ev.compiler + " -o " + ev.programPath;
		for(int i = 0; i < objects.size(); i++)
		{
			linkCommand += (" " + objects[i]);
		}
		printf("%s\n", linkCommand.c_str());
		if(system(linkCommand.c_str()) != 0)
		{
			exit(EXIT_FAILURE);
		}
	}
	return;
}
