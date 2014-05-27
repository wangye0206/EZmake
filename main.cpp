#include "define.h"
#include "job.h"

using namespace std;

int main(int argc, char** argv)
{
	/* initialize environment variables and output info */
	environment_variable ev;
	job myjob = Initialization(ev, argc, argv);
	switch( myjob )
	{
		case CLEAN:
			Clean(ev);
			break;
		case HELP:
			Help();
			break;
		case COMPILE:
			Compile(ev);
			break;
	}
	return 0;
}	
