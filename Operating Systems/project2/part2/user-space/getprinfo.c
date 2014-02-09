
#ifndef _GETPRINFO_
#define _GETPRINFO_

#include <sys/syscall.h>
#include <stdio.h>
#include "prinfo.h"

#define __NR_cs3013_syscall1 349

long getprinfo(struct prinfo* info){
	//printf("GETPRINFO\n");
	return (long)syscall(__NR_cs3013_syscall1, info);
}

#endif

