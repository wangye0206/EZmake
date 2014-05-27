#ifndef _TARGET_H_
#define _TARGET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include "define.h"

using namespace std;

class Target
{
public:
	Target(string dir, string name);
	~Target();
	Target(const Target& src);
	Target& operator= (const Target& src);
	bool operator< (const Target& rhs) const;
	bool operator== (const Target& src) const;
	void Compile(const environment_variable& ev) const;
	string ObjectName() const;
	string TargetName() const;
	/*return true if this target need to be compiled */
	bool AnalyzeTarget(const environment_variable& ev, vector<string>& neglect) const;
	
private:
	/* target name (full path) */
	string targetName;
	/* just name, no need full path (always current path) */
	string objectName;
	void AnalyzeFile(string thisFile, queue<string>& wait, set<string>& analyzed, bool& result, vector<string>& neglect, const environment_variable& ev) const;
	/* analyze /, if this is a comment then move the indicator
	 * to the char after the comment or the char next to /. */
	void AnalyzeSlash(FILE* thisFile) const;
	/* analyze quote, move the indicator after the quote. */
	void AnalyzeQuote(FILE* thisFile, char quoteType) const;
	/* analyze #.  Move the indicator after the included file
	 * return the name of the included file,
	 * return '\0' otherwise.*/
string AnalyzeHash(FILE* thisFile, const environment_variable& ev) const;
};

struct ClassTargetLess
{
	bool operator() (const Target& t1, const Target& t2);
};
#endif
