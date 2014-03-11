#ifndef _EHM_H
#define _EHM_H
#pragma once

#include "RESULT.h"
#include "DebugConsole.h"

// TODO: Eventually the idea is instead of using error handling macros we will be using 
// this file as an error handling module which will track errors and not necessarily destroy a
// specific method if an error occurs.  Meanwhile the log is output to the display.

// CURRENT: Migrating EHM to RESULT instead of HRESULT

// ERROR HANDLING MACROS (EHM)
// These can be useful to quickly insert inline error checking functionality

#define DEBUG_FILE_LINE


#ifdef DEBUG_FILE_LINE
#define CurrentFileLine "%s line#%d\n", __FILE__, __LINE__
#else
#define CurrentFileLine ""
#endif

// Some Microsoft Defines we like
#ifndef DWORD
typedef unsigned long DWORD;
#endif

// Check HRESULT value.  
// Ensures that HRESULT is successful
#define CHR(h) if( FAILED(h) ) { DEBUG_MSG(CurrentFileLine); r = (RESULT)h; DEBUG_MSG("Error: 0x%x\n", r); goto Error; }

// Check HRESULT value Message  
// Ensures that HRESULT is successful
#ifdef _WINDOWS_DEBUG_CONSOLE
    #define CHRM(h, msg, ...) if( FAILED(h) ) { DEBUG_MSG(CurrentFileLine); DEBUG_MSG(msg, __VA_ARGS__); DEBUG_MSG(" Error: 0x%x\n", h); r = (RESULT)(h); goto Error; }
#elif defined _IOS_DEBUG_CONSOLE
    #define CHRM(h, msg, ...) if( FAILED(h) ) { DEBUG_MSG(CurrentFileLine); DEBUG_MSG(msg, ##__VA_ARGS__); DEBUG_MSG(" Error: 0x%x\n", h); r = (RESULT)(h); goto Error; }
#endif

// Check RESULT value
// Ensures that RESULT is successful
#define CR(res) r = res; if(r & 0x80000000){ DEBUG_MSG(CurrentFileLine); DEBUG_MSG("Error: 0x%x\n", r); goto Error;}

// Check RESULT value
// Ensures that RESULT is successful
#ifdef _WINDOWS_DEBUG_CONSOLE
    #define CRM(res, msg, ...) do{r = res; if(r < 0){ DEBUG_MSG(CurrentFileLine); DEBUG_MSG(msg, __VA_ARGS__); DEBUG_MSG("Error: 0x%x\n", r); goto Error;}}while(0)
#elif defined _IOS_DEBUG_CONSOLE
    #define CRM(res, msg, ...) do{r = res; if(r & 0x80000000){ DEBUG_MSG(CurrentFileLine); DEBUG_MSG(msg, ##__VA_ARGS__); DEBUG_MSG("Error: 0x%x\n", r); goto Error;}}while(0)
#endif

// Check Boolean Result
// Ensures that condition evaluates to true
#define CBHR(condition) if(!condition) { DEBUG_MSG(CurrentFileLine); DEBUG_MSG("Failed condition!\n"); hr = E_FAIL; goto Error; }
#define CBR(condition) if(!condition) { DEBUG_MSG(CurrentFileLine); DEBUG_MSG("Failed condition!\n"); r = R_FAIL; goto Error; }

// Check Boolean Result Message
// Ensures that condition evaluates to true
#ifdef _WINDOWS_DEBUG_CONSOLE
    #define CBRM(condition, msg, ...) if(!condition) { DEBUG_MSG(CurrentFileLine); DEBUG_MSG(msg, __VA_ARGS__); DEBUG_MSG("\n"); r = R_FAIL; goto Error; }
#elif defined _IOS_DEBUG_CONSOLE
    #define CBRM(condition, msg, ...) if(!condition) { DEBUG_MSG(CurrentFileLine); DEBUG_MSG(msg, ##__VA_ARGS__); DEBUG_MSG("\n"); r = R_FAIL; goto Error; }
#endif

// Check Boolean Result Message Error
// Ensures that the condition evaluates to true
// Will set the RESULT to a user specified error
#define CBRME(condition, msg, e) if(!condition) { DEBUG_MSG(CurrentFileLine); DEBUG_MSG(msg); r = e; goto Error; }

// Check Pointer Result 
// Ensures that the pointer is not a NULL
#define CPR(pointer) if(pointer == NULL) { DEBUG_MSG(CurrentFileLine); DEBUG_MSG("Null pointer error!\n"); r = R_ERROR; goto Error; }

#ifdef _WINDOWS_DEBUG_CONSOLE
    #define CPRM(pointer, msg, ...) if(pointer == NULL) { DEBUG_MSG(CurrentFileLine); DEBUG_MSG(msg, __VA_ARGS__); DEBUG_MSG("\n"); r = R_ERROR; goto Error; }
#elif defined _IOS_DEBUG_CONSOLE
    #define CPRM(pointer, msg, ...) if(pointer == NULL) { DEBUG_MSG(CurrentFileLine); DEBUG_MSG(msg, ##__VA_ARGS__); DEBUG_MSG("\n"); r = R_ERROR; goto Error; }
#endif

// Check Handle Message 
// Ensures that the pointer is not a NULL
#ifdef _WINDOWS_DEBUG_CONSOLE
    #define CHM(pointer, msg, ...) if(pointer == NULL) { DEBUG_MSG(CurrentFileLine); DEBUG_MSG(msg, __VA_ARGS__); DEBUG_MSG(" Null pointer error!\n"); r = R_ERROR; goto Error; }
#elif defined _IOS_DEBUG_CONSOLE
    #define CHM(pointer, msg, ...) if(pointer == NULL) { DEBUG_MSG(CurrentFileLine); DEBUG_MSG(msg, ##__VA_ARGS__); DEBUG_MSG(" Null pointer error!\n"); r = R_ERROR; goto Error; }
#endif

// Check NULL Result
// Ensures that the pointer is not a NULL
#define CNR(pointer) if(pointer == NULL) { DEBUG_MSG(CurrentFileLine); DEBUG_MSG("Null error!\n"); r = R_ERROR; goto Error; }

// Check NULL Result Message
// Ensures that the pointer is not a NULL
#ifdef _WINDOWS_DEBUG_CONSOLE
    #define CNRM(pointer, msg, ...) if(pointer == NULL) { DEBUG_MSG(CurrentFileLine); DEBUG_MSG(msg, __VA_ARGS__); DEBUG_MSG("\n"); r = R_ERROR; goto Error; }
#elif defined _IOS_DEBUG_CONSOLE
    #define CNRM(pointer, msg, ...) if(pointer == NULL) { DEBUG_MSG(CurrentFileLine); DEBUG_MSG(msg, ##__VA_ARGS__); DEBUG_MSG("\n"); r = R_ERROR; goto Error; }
#endif

// Calculate the memory offset of a field in a struct
#define STRUCT_FIELD_OFFSET(struct_type, field_name)    ((long)(long*)&(((struct_type *)0)->field_name))

#endif // _EHM_H