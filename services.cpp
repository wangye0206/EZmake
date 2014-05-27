#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "services.h"

long int GetFileLineNum(FILE* file)
{
	long int line_num = 0;
	char current_char, previous_char;
	long int correntPosition = ftell(file);
	bool emptyFile = true;
	fseek( file, 0, SEEK_SET );
	while( (current_char = fgetc(file)) != EOF )
	{
		emptyFile = false;
		if(current_char == '\n')
			line_num++;
		previous_char = current_char;
	}
	if( previous_char != '\n' && (!emptyFile))
		line_num++;
	fseek( file, correntPosition, SEEK_SET );
	return line_num;
}

bool StringEndWith(string string1, string string2)
{
	if( string1.length() < string2.length() )
		return false;
	else
	{
		int temp = string1.compare( string1.length()-string2.length(), string2.length(), string2 );
		return (temp == 0);
	}
}

bool CompareNewer(string file1, string file2)
{
	if(access(file1.c_str(), F_OK) != 0)
	{
		return false;
	}
	else if(access(file2.c_str(), F_OK) != 0)
	{
		return true;
	}
	else
	{	
		struct stat attrib1, attrib2;
		stat(file1.c_str(), &attrib1);
		stat(file2.c_str(), &attrib2);
		double delta = difftime(attrib1.st_mtime, attrib2.st_mtime);
		return (delta > 0.);
	}
}
