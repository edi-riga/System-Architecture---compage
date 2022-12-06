#ifndef _COMPAGE_CONFIG_H_
#define _COMPAGE_CONFIG_H_

char* compage_cfg_get_string(char *buf, size_t bufSize, size_t type, void *addr);
int   compage_cfg_set_value(void *dst, const void *src, size_t type);
void compage_cfg_get_value(void *dst, const void *src, size_t type);

#endif
