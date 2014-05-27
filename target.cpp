#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <queue>
#include <set>
#include "target.h"
#include "services.h"
#include "define.h"

using namespace std;

Target::Target(string dir, string name)
{
	targetName = dir + name;
	unsigned suffixPosition = name.find_last_of(".");
	objectName = string(name, 0, suffixPosition) + ".o";
}

Target::~Target()
{};

Target::Target(const Target& src)
{
	targetName = src.TargetName();
	objectName = src.ObjectName();
}

Target& Target::operator= (const Target& src)
{
	targetName = src.TargetName();
	objectName = src.ObjectName();
	return *this;
}

bool Target::operator< (const Target& rhs) const
{
	string s2 = rhs.TargetName();
	if(targetName.length() < s2.length())
	{
		return true;
	}
	else if(targetName.length() > s2.length())
	{
		return false;
	}
	else
	{
		for(int i = 0; i < targetName.length(); i++)
		{
			if(targetName[i] == s2[i])
			{
				continue;
			}
			else
			{
				return int(targetName[i]) < int(s2[i]);
			}
		}
	}
	return false;
}

bool Target::operator== (const Target& src) const
{
	return (targetName == src.TargetName());
}

void Target::Compile(const environment_variable& ev) const
{
	string shellCommand;
	/* set compiler */
	shellCommand += (ev.compiler + " ");
	/* set flag */
	shellCommand += (ev.flags + " ");
	/* set include pathes */
	for( int i = 1; i < ev.headerDirs.size(); i++ )
		shellCommand += ("-I" + ev.headerDirs[i] + " ");
	/* output obj file */
	shellCommand += ("-c ");
	/* set file need to be compile */
	shellCommand += (targetName);

	printf("%s\n", shellCommand.c_str());
	if(system(shellCommand.c_str()) != 0 )
		exit(EXIT_FAILURE);
	
	return;
}

string Target::ObjectName() const
{
	return objectName;
}

string Target::TargetName() const
{
	return targetName;
}

bool Target::AnalyzeTarget(const environment_variable& ev, vector<string>& neglect) const
{
	bool result = false;
	set<string> analyzed;
	queue<string> wait;
	wait.push(targetName);
	while(wait.size() > 0)
	{
		string thisFile = wait.front();
		wait.pop();
		AnalyzeFile(thisFile, wait, analyzed, result, neglect, ev);
	}
	return result;	
}

void Target::AnalyzeFile(string thisFile, queue<string>& wait, set<string>& analyzed, bool& result, vector<string>& neglect, const environment_variable& ev) const
{
	/* files need to compare with thisFile */
	vector<string> needCompare;
	needCompare.push_back(thisFile);
	set<string>::iterator it = analyzed.find(thisFile);
	if(it != analyzed.end())
	{
		return;
	}
	analyzed.insert(thisFile);
	
	FILE* filePtr = fopen(thisFile.c_str(), "r");
	if(filePtr == NULL)
	{
		printf("Cannot open %s, please check the file access mode.\n", thisFile.c_str());
		exit(EXIT_FAILURE);
	}
	while(feof(filePtr) == 0)
	{
		char currChar = fgetc(filePtr);
		/* find out whether current char belongs to comment, if it is comment, move file indicator to the end of the comment */
		if(currChar == '/')
		{
			AnalyzeSlash(filePtr);
		}
		/* find out whether current char is in a pair of quotes, if yes, move file indicator to the end */
		else if(currChar == '\'' || currChar == '\"')
		{
			AnalyzeQuote(filePtr, currChar);
		}
		else if(currChar == '#')
		{
			string includeFileName = AnalyzeHash(filePtr, ev);
			
			if(includeFileName.length() > 0)
			{
				if(StringEndWith(includeFileName, ".c") || StringEndWith(includeFileName, ".cpp"))
				{
					neglect.push_back(includeFileName);
				}
				needCompare.push_back(includeFileName);
				wait.push(includeFileName);
			}
		}
		else
		{
			continue;
		}
	}
	fclose(filePtr);
	for(int i = 0; !result && i < needCompare.size(); i++)
	{
		if(CompareNewer(needCompare[i], objectName))
		{
			result = true;
		}
	}
	return;
}

void Target::AnalyzeSlash(FILE* thisFile) const
{
	char thisChar = fgetc(thisFile);
			
	if( thisChar == '/' )
	{
		while( (thisChar = fgetc(thisFile)) != '\n' )
		{
			continue;
		}
	}
	else if(thisChar == '*')
	{
		while(feof(thisFile) == 0)
		{
			thisChar = fgetc(thisFile);
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

void Target::AnalyzeQuote(FILE* thisFile, char quoteType) const
{
	char thisChar;
	while(feof(thisFile) == 0 && (thisChar = fgetc(thisFile)) != quoteType)
	{
		if(thisChar == '\\' && feof(thisFile) == 0)
		{
			fgetc(thisFile);
		}
	}
	return;
}

string Target::AnalyzeHash(FILE* thisFile, const environment_variable& ev) const
{
	char tempString[8];
	char thisChar;
	fgets(tempString, 8, thisFile);
	if(strcmp(tempString, "include") == 0)
	{
		while(true)
		{
			if(feof(thisFile) == 0)
			{
				thisChar = fgetc(thisFile);
				/* do not check < > */
				if(thisChar == '<')
				{
					return "";
				}
				else if(thisChar == '\"')
				{
					break;
				}
				else
				{
					continue;
				}
			}
			else
			{
				return "";
			}
		}
		long int begin = ftell(thisFile);
		while(true)
		{
			if(feof(thisFile) == 0)
			{
				thisChar = fgetc(thisFile);
				if(thisChar == '\"' )
				{
					break;
				}
				else if(thisChar == '\n')
				{
					return "";
				}
				else
				{
					continue;
				}
			}
			else
			{
				return "";
			}
		}
		long int end = ftell( thisFile );
		/* fileNameLength includes the '\0' in the end */
		int fileNameLength = end - begin;
		char* includeFileName = new char[fileNameLength];
		fseek(thisFile, begin, SEEK_SET);
		fgets(includeFileName, fileNameLength, thisFile);
		fseek(thisFile, end, SEEK_SET);
		string result = string(includeFileName);
		delete[] includeFileName;
		/*findout the full path of the result */
		if(StringEndWith(result, ".h"))
		{
			for(int i = 0; i < ev.headerDirs.size(); i++)
			{
				string tmpName = ev.headerDirs[i]+result;
				if(access(tmpName.c_str(), F_OK)==0)
				{
					result = tmpName;
					break;
				}
				if(i == ev.headerDirs.size()-1)
				{
					return "";
				}
			}
		}
		else if(StringEndWith(result, ".c") || StringEndWith(result, ".cpp"))
		{
			for(int i = 0; i < ev.sourceDirs.size(); i++)
			{
				string tmpName = ev.sourceDirs[i]+result;
				if(access(tmpName.c_str(), F_OK)==0)
				{
					result = tmpName;
					break;
				}
				if(i == ev.sourceDirs.size()-1)
				{
					return "";
				}
			}
		}	
		return result;
	}
	return "";
}
