/* Console.h
 * The Incident Console is a general purpose console shell which
 * can be used as a debug console or whatever other purposes required
 * This code is abstracted from its respective implementation both for
 * input/output and display
 *
 * NOTE: Console is designed as an ASCII console and is not intended for
 * other languages as such
 */ 

#pragma once

#include "dss_list.h"
#include "SmartBuffer.h"

using namespace dss;

class Console;

#define MAX_COMMAND 256
#define MAX_OUTPUT

// Buffer manipulation macros
#define ADD_CHAR_TO_STATIC_BUFFER(buffer, c) buffer[buffer ## _n] = c; buffer ## _n++;
#define DELETE_CHAR_FROM_STATIC_BUFFER(buffer) if(buffer ## _n > 0)                 \
                                               {                                    \
                                                   buffer ## _n--;                  \
                                                   buffer[buffer ## _n] = '\0';     \
                                               }

// Function Pointer templates
// we treat these as possibly having any input/output values with a 
// void* pointer. It is up to the function implementation / call to handle these
// since Console does this it is handled then.  Currently we only handle
// a maximum of 7 input arguments

// TODO: Change this to be variadic

typedef void*(*FP0)();
typedef void*(*FP1)(void*);
typedef void*(*FP2)(void*, void*);
typedef void*(*FP3)(void*, void*, void*);
typedef void*(*FP4)(void*, void*, void*, void*);
typedef void*(*FP5)(void*, void*, void*, void*, void*);
typedef void*(*FP6)(void*, void*, void*, void*, void*, void*);
typedef void*(*FP7)(void*, void*, void*, void*, void*, void*, void*);

// Reserved Functions
// All functions must take one input which is a reference to the Console itself
RESULT Exit(Console* pc);
RESULT History(Console* pc);
RESULT Print(Console* pc, char *pszString);
RESULT List(Console *pc);
RESULT Help(Console* pc, char *pszFunctionName);

struct ConsoleFunction
{
    int ID;
    void *Function;
    char *pszCommand;
	char **ppszDefaultArguments;	// Default arguments
	int ppszDefaultArguments_n;		// Number of default arguments
    int arguments;    
};

typedef enum ConsoleReservedCommands
{
    CONSOLE_EXIT,
	CONSOLE_HISTORY,
    CONSOLE_PRINT,
	CONSOLE_LIST,
    CONSOLE_HELP,
    CONSOLE_RESERVED_END
} CONSOLE_RESERVED_COMMANDS;

static ConsoleFunction ReservedFunctions[] =
{
    {
        CONSOLE_EXIT,
        (void*)Exit,
        "exit",
		NULL,
		0,
        1
    },
	{
		CONSOLE_HISTORY,
		(void*)History,
		"history",
		NULL,
		0,
		1
	},
    {
        CONSOLE_PRINT,
        (void*)Print,
        "print",
		NULL,
		0,
        2
    },
	{
		CONSOLE_LIST,
		(void*)List,
		"list",
		NULL,
		0,
		1
	},
    {
        CONSOLE_HELP,
        (void*)Help,
        "help",
		NULL,
		0,
        2
    }
};
static int ReservedFunctions_n = sizeof(ReservedFunctions) / sizeof(ReservedFunctions[0]);

class Console
{
public:
    Console();
    
    ~Console();

    RESULT ReceiveInput(char* d_input, int d_input_n);
    RESULT ReceiveInput(char c);

    RESULT CheckForOutput();
    RESULT DispatchOutput(char* &n_output, int &rn_output_n);
    RESULT DumpOutputHistory(char* pszFilename);

    // Console maintains the output buffer
    // through these functions
    RESULT PrintToOutput(char* pszOutput);
    RESULT PrintToOutput(char c);
    RESULT PrintToOutput(const char* output_format, ...);

    RESULT AddConsoleFunction(ConsoleFunction cf);
    RESULT AddConsoleFunction(void *fun, char *pszCommand, int arguments, int nDefaultArgs, ... );
    
    RESULT ExecuteConsoleFunction(char *pszCommand);

    RESULT ConvertToArgumentList(char *d_InputBuffer, char** &n_Args, int &rn_Args_n);


	RESULT GetInputBufferHistory(list<char*>* &r_pInputBufferHistory)
	{
		r_pInputBufferHistory = &m_InputBufferHistory;
		return R_SUCCESS;
	}

    ConsoleFunction CreateConsoleFunction(int ID, void *fun, char *pszCommand, int arguments, int nDefaultArgs, ... );

	list<ConsoleFunction>* GetCommandList(){ return &m_Commands; }

private:
    RESULT ResetCommandBuffer();
    RESULT Path(char* &n_pszPath);

private:

    char m_InputBuffer[MAX_COMMAND];
    int m_InputBuffer_n;
    list<char*> m_InputBufferHistory;

    SmartBuffer *m_pOutputBuffer;

    //char m_OutputBuffer[MAX_COMMAND];
    //int m_OutputBuffer_n;
    list<char*> m_OutputBufferHistory;

    list<ConsoleFunction> m_Commands;

    long int m_nFunctionID;
};

//RESULT TestConsole();

