#include <stdio.h>
#include <stdlib.h>
#include "environment.h"

void InitializeEV( environment_variable &ev )
{
	ev.sourceDirs.push_back("./");

	ev.headerDirs = ev.sourceDirs;

	ev.programName = "myprogram";

	ev.programDir = "./";

	ev.compiler = "clang++";

	ev.flags = "";

	return;
}

