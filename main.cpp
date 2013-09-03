#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include "environment.h"
#include "output.h"
#include "target.h"
#include "services.h"

using namespace std;

int main(int argc, char* argv[])
{
	/* initialize environment variables and output info */
	environment_variable ev;
	InitializeEV(ev);
	output program( ev.programName, ev.programDir );

	vector<target> targets;

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
	
	return 0;
}

