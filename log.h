#ifndef _LOGVIEW360_H_
#define _LOGVIEW360_H_

#include <stdio.h>

extern FILE* debugfile;

#define print_log(x) \
	if(debugfile) fprintf(debugfile, x); 
#endif
