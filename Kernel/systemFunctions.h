#ifndef KERNEL_SYSTEM_FUNCTIONS
#define KERNEL_SYSTEM_FUNCTIONS

int getCfgString(char *section, char *keyname, char *defaultres, char *data, int size, char *cfgfilename);
int getCfgInt(char *section, char *keyname, int defaultres, char *cfgfilename);

#endif