#ifndef OS_PLUGIN_H
#define OS_PLUGIN_H

#include "ittia/os/os_config.h"

C_HEADER_BEGIN

void * os_plugin_open(const char * dl_name);
void (*os_plugin_proc(void * dl_handle, const char * dl_name))(void);
int    os_plugin_close(void * dl_handle);

C_HEADER_END

#endif /* OS_PLUGIN_H */
