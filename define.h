/* define basic functions and variable structures in this file */
#ifndef DEFINE_H_
#define DEFINE_H_

#include <string>
#include <vector>

using namespace std;

enum job {COMPILE, CLEAN, HELP};

struct environment_variable
{
	/* source dirs in project, first element will always be ./ */
	vector<string> sourceDirs;

	/* header dirs in project, first element will always be ./ */
	vector<string> headerDirs;
	
	/* output name */
	string programPath;

	/* compiler */
	string compiler;

	/* compiler flags */
	string flags;	
};

#endif
