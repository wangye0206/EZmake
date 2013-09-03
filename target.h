#ifndef TARGET_H_
#define TARGET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "environment.h"

using namespace std;

class target
{
public:
	target(string fileDir, string fileName)
		{
			dir = fileDir;
			name = fileName;
			path = dir + name;
			unsigned suffixPosition = fileName.find_last_of(".");
			objectName = string (fileName, 0, suffixPosition) + ".o";
			objectPath = dir + objectName;
			lgRecompile = false;
			lgFileOK = true;
		}
	/* return 1 if this target file has problem,
	 * return 2 if target is recompiled,
	 * return 0 if done nothing */
	int CheckTarget(environment_variable ev);
	string GetTargetPath(void);
	string GetObjectPath(void);

private:
	string path;
	string name;
	string dir;
	string objectName;
	string objectPath;
	/* set to true if this file need to be recompiled */
	bool lgRecompile;
	/* if this file has problem, lgFileOK is set to false */
	bool lgFileOK;
	
	void CheckDependence(environment_variable ev);
	void Recompile(environment_variable ev);
	
	/* check .h.
	 * set lgRecompile to true if header file newer than .o*/
	void CheckHeader(FILE* sourceFile, FILE* objectFile, environment_variable ev);
	/* analyze /, if this is a comment then move the indicator
	 * to the char after the comment or the char next to /. */
	void AnalyzeSlash(FILE* thisFile);
	/* analyze quote, move the indicator after the quote. */
	void AnalyzeQuote(FILE* thisFile, char quoteType);
	/* analyze #.  Move the indicator after the included file
	 * return the name of the included file if include a .h file,
	 * return NULL otherwise.*/
	string AnalyzeHash(FILE* thisFile);
	
};
#endif
