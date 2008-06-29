#ifndef ARITHMETIX_RESOURCE_HPP
#define ARITHMETIX_RESOURCE_HPP

#include <stdio.h>

FILE * get_resource_file(const char * resource);
void * get_resource_mem(const char * resource, int * length);

#endif
