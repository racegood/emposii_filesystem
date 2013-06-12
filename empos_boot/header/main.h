#ident "$Id: main.h$"

#ifndef SMART_MAIN_H
#define SMART_MAIN_H

void c_main(void);
int GetCommand(char* commandline);
void RunCommand(char* commandline);
void boot_kernel(void);

#endif
