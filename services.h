#ifndef SERVICES_H_
#define SERVICES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

extern long int GetFileLineNum(FILE* file);

/* return true if string 1 is end with string 2 */
extern bool StringEndWith(string string1, string string2);

/* return true if file1 is newer than file 2,  */
extern bool CompareNewer(string file1, string file2);

#endif
