#include <stdio.h>
#include <stdlib.h>
#include "output.h"
#include "environment.h"
#include "target.h"

void output::SetRelink(void)
{
	lgRelink = true;
	return;
}

void output::Relink(vector<target> targets, environment_variable ev)
{
	string shellCommand = ev.compiler + " -o " + path;
	for(int i = 0; i < targets.size(); i++)
	{
		shellCommand += (" " + targets[i].GetObjectPath());
	}
	printf("%s\n", shellCommand.c_str());
	system(shellCommand.c_str());
	return;
}

bool output::NeedRelink(void)
{
	return lgRelink;
}

string output::GetPath(void)
{
	return path;
}
