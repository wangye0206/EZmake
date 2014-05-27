/* declear basic functions */
#ifndef _JOB_H_
#define _JOB_H_

#include "define.h"

extern job Initialization(environment_variable &ev, int argc, char** argv);
extern void Clean(environment_variable ev);
extern void Help(void);
extern void Compile(environment_variable ev);

#endif
