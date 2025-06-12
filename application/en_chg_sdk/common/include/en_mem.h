#ifndef  _en_mem_H
#define _en_mem_H
#include "en_common.h"


void sys_print_memory(void);
void sys_print_memory_used_info(void);

void *MALLOC(u32 size);
void FREE(void *ptr);


#endif

