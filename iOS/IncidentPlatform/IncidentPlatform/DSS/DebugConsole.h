#pragma once

// Debug console
#include <stdio.h>	

#undef _WINDOWS_DEBUG_CONSOLE
#define _IOS_DEBUG_CONSOLE

#ifdef _WINDOWS_DEBUG_CONSOLE
    //#define DLog(__FORMAT__, ...) do { printf((@"%s [Line %d] " __FORMAT__), __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__); } while(0)

    #define DEBUG_MSG(x, ...) do { printf(x, VA_ARGS); } while(0)
    #define DEBUG_LINEOUT(x, ...) do{printf(x, VA_ARGS); printf("\n");} while(0)

#elif defined _IOS_DEBUG_CONSOLE
    //#define DLog(__FORMAT__, ...) do { printf((@"%s [Line %d] " __FORMAT__), __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__); } while(0)

    #define DEBUG_MSG(x, ...) do { printf(x, ##__VA_ARGS__); } while(0)
 //   #define DEBUG_MSG_NA(x) do { printf(x); } while(0)

    #define DEBUG_LINEOUT(x, ...) do{printf(x, ##__VA_ARGS__); printf("\n");} while(0)

#endif



