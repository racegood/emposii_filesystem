#ifndef _DEBUG_MSG_H
#define _DEBUG_MSG_H

#ifdef _DEBUG_PRINT

#define dbg(format, arg...) printf("[DEBUG_MSG] " format "\n" ,  ## arg)
#else
#define dbg(format, arg...) do {} while (0)
#endif

#endif
