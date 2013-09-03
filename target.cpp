#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "target.h"
#include "environment.h"
#include "services.h"

using namespace std;

/* Public Function */

int target::CheckTarget(environment_variable ev)
{
	CheckDependence(ev);
	if( !lgFileOK )
		return 1;
	if( lgRecompile )
	{
		Recompile(ev);
		return 2;
	}
	return 0;
}

string target::GetTargetPath(void)
{
	return path;
}

string target::GetObjectPath(void)
{
	return objectPath;
}

/* Private Function */

void target::CheckDependence(environment_variable ev)
{
	/* first check .o. if .o does not exist, set lgRecompile to true */
	if( access(objectPath.c_str(), F_OK) == -1 )
	{
		lgRecompile = true;
	}
	else
	{
		FILE* objectFile = fopen(objectPath.c_str(), "r");
		FILE* sourceFile = fopen(path.c_str(), "r");
		/* if sourceFile can not be open, ask user to check this file */
		if( sourceFile == NULL )
		{
			lgFileOK = false;
		}
		else
		{
			/* second check source file (such as .cpp).
			 * set lgRecompile to true if souce file newer than .o */
			if( CompareNewer( path, objectPath ) )
				lgRecompile = true;
			else
			{
				CheckHeader(sourceFile, objectFile, ev);
			}
		}
		fclose( objectFile );
		fclose( sourceFile );
	}
	return;
}

void target::Recompile(environment_variable ev)
{
	string shellCommand;
	/* set compiler */
	shellCommand += (ev.compiler + " ");
	/* set flag */
	if( ev.flags.length() != 0 )
		shellCommand += (ev.flags + " ");
	/* set include pathes */
	for( int i = 1; i < ev.headerDirs.size(); i++ )
		shellCommand += ("-I" + ev.headerDirs[i] + " ");
	/* set object name */
	shellCommand += ("-c ");
	/* set file need to be compile */
	shellCommand += (path);

	printf("\n%s\n", shellCommand.c_str());
	system(shellCommand.c_str());
	return;
}

void target::AnalyzeSlash(FILE* thisFile)
{
	char thisChar = fgetc(thisFile);
			
	if( thisChar == '/' )
	{
		while( (thisChar = fgetc(thisFile)) != '\n' )
		{
			continue;
		}
	}
	else if( thisChar == '*' )
	{
		while( feof(thisFile) == 0 )
		{
			thisChar = fgetc( thisFile );
			if( thisChar == '*' )
			{
				thisChar = fgetc(thisFile);
				if( thisChar  == '/'  )
					break;
			}
		}
	}
	else
	{
		long int currentPosition = ftell(thisFile);
		fseek(thisFile, currentPosition - 1, SEEK_SET);
	}
	return;
}

void target::AnalyzeQuote(FILE* thisFile, char quoteType)
{
	char thisChar;
	while( (thisChar = fgetc( thisFile )) != quoteType )
	{
		if( thisChar == '\\' )
			fgetc( thisFile );
		if( feof(thisFile) != 0 )
		{
			lgFileOK = false;
			return;
		}
	}
	return;
}

string target::AnalyzeHash(FILE* thisFile)
{
	char tempString[8];
	char thisChar;
	fgets( tempString, 8, thisFile );
	if( strcmp( tempString, "include" ) == 0 )
	{
		while( true )
		{
			thisChar = fgetc(thisFile);
			if( thisChar == '<' || thisChar == '\"' )
				break;
			
			if( feof( thisFile ) != 0 )
			{
				lgFileOK = false;// error, need debug
				return "\0";
			}
		}
		/* do not check <> */
		if( thisChar == '<' )
		{
			return "\0";
		}
		long int begin = ftell(thisFile);
		while( true )
		{
			thisChar = fgetc(thisFile);
			if( thisChar == '>' || thisChar == '\"' )
				break;
			if( feof( thisFile ) != 0 )
			{
				lgFileOK = false;
				return "\0";
			}
		}
		long int end = ftell( thisFile );
		/* fileNameLength includes the '\0' in the end */
		int fileNameLength = end - begin;
		char includeFileName[fileNameLength];
		fseek( thisFile, begin, SEEK_SET );
		fgets( includeFileName, fileNameLength, thisFile );
		fseek( thisFile, end, SEEK_SET );
		if( strlen(includeFileName) != (fileNameLength - 1) )
		{
			lgFileOK = false;
		}
		else
		{
			if( StringEndWith(string(includeFileName), ".h") )
				return includeFileName;
		}
	}
	return "\0";
}

void target::CheckHeader(FILE* sourceFile, FILE* objectFile, environment_variable ev)
{	
	while( feof(sourceFile) == 0 && (!lgRecompile) && lgFileOK )
	{
		char currentChar = fgetc(sourceFile);
		/* find out whether current char belongs to comment */
		if( currentChar == '/' )
		{
			AnalyzeSlash(sourceFile);
		}
		/* find out whether current char is in a pair of quotes */
		else if( currentChar == '\'' || currentChar == '\"')
		{
			AnalyzeQuote(sourceFile, currentChar);
		}
		else if( currentChar == '#' )
		{
			string headerFileName = AnalyzeHash(sourceFile);
			if( headerFileName.length() != 0 )
			{
				for( int i = 0; i<ev.headerDirs.size(); i++)
				{
					string headerFilePath = ev.headerDirs[i] + headerFileName;
					FILE* headerFile = fopen(headerFilePath.c_str(), "r");
					if( headerFile != NULL )
					{
						if( CompareNewer(headerFilePath, objectPath) )
							lgRecompile = true;
					}
					fclose(headerFile);
				}
			}
		}
		else
			continue;
	}
	return;
}
