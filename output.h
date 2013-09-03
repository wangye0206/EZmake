#ifndef OUTPUT_H_
#define OUTPUT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "environment.h"
#include "target.h"

using namespace std;

class output
{
public:
	output(string programName, string programDir)
		{
			name = programName;
			dir = programDir;
			path = dir+name;
			lgRelink = false;
		}
	void SetRelink(void);
	void Relink(vector<target> targets, environment_variable ev);
	/* return lgRelink value */
	bool NeedRelink(void);
	string GetPath(void);
	
private:
	string name;
	string path;
	string dir;
	bool lgRelink;
};

#endif
