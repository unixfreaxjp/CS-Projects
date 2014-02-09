#ifndef DOIT_H
#define DOIT_H

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

long timeval_to_ms(struct timeval *t);
void doit(char** argv);

#endif
