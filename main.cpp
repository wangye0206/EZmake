#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include "environment.h"
#include "initialization.h"
#include "output.h"
#include "target.h"
#include "services.h"
#include "define.h"

using namespace std;

void Clean( environment_variable ev);
void Help(void);
void Compile( environment_variable ev);

int main(int argc, char** argv)
{
	/* initialize environment variables and output info */
	environment_variable ev;
	job myjob = Initialization(ev, argc, argv);
	switch( myjob )
	{
		case CLEAN:
			Clean(ev);
			break;
		case HELP:
			Help();
			break;
		case COMPILE:
			Compile(ev);
			break;
	}
	return 0;
}

void Clean( environment_variable ev)
{
	string shellCommand;
	for( int i = 0; i < ev.sourceDirs.size(); i++ )
	{
		shellCommand = "rm -f " + ev.sourceDirs[i] + "*.o";
		system(shellCommand.c_str());
	}
	shellCommand = "rm -f " + ev.programPath;
	system(shellCommand.c_str());
	return;
}

void Help(void)
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

void Compile( environment_variable ev)
{
	vector<target> targets;
	output program( ev.programPath );
	
	for( int i = 0; i < ev.sourceDirs.size(); i++ )
	{
		DIR* dirFile = opendir( ev.sourceDirs[i].c_str() );
		if( dirFile )
		{
			struct dirent* hFile;
			while( (hFile = readdir( dirFile )) != NULL )
			{
				if( !strcmp( hFile->d_name, "." ))
					continue;
				if( !strcmp( hFile->d_name, ".."))
					continue;
				if( hFile->d_name[0] == '.' )
					continue;
				if( StringEndWith(string(hFile->d_name), ".cpp") || StringEndWith( string(hFile->d_name), "c" ) )
				{
					targets.push_back( target(ev.sourceDirs[i], string(hFile->d_name)) );
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

	for( int i = 0; i < targets.size(); i++ )
	{
		int tempTargetResult = targets[i].CheckTarget(ev);
		if( tempTargetResult == 1 )
		{
			printf("Something wrong in %s, please have a check.\n", targets[i].GetTargetPath().c_str());
			exit( EXIT_FAILURE );
		}
		else if( tempTargetResult == 2 )
			program.SetRelink();
		else
		{
			if( CompareNewer( targets[i].GetObjectPath(), program.GetPath() ) )
				program.SetRelink();
		}
	}
	
	if( program.NeedRelink() )
		program.Relink(targets, ev);
	
	return;
}		
