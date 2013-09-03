#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

using namespace std;

struct environment_variable
{
	/* source dirs in project, first element will always be ./ */
	vector<string> sourceDirs;

	/* header dirs in project, first element will always be ./ */
	vector<string> headerDirs;

	/* output name */
	string programName;

	/* output Dir */
	string programDir;

	/* compiler */
	string compiler;

	/* compiler flags */
	string flags;
	
};

extern void InitializeEV( environment_variable &ev );

#endif
