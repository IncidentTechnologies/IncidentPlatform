#include "Console.h"
#include "SmartBuffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdarg.h>

//#include <conio.h>

// Reserved Functions
// *************************************
RESULT Exit(Console *pc)
{ 
    exit(0);
    return R_EXIT; 
}

RESULT History(Console *pc)
{
	RESULT r = R_SUCCESS;
	CR(pc->DumpOutputHistory(NULL));
Error:
	return R_SUCCESS;
}

RESULT Print(Console *pc, char *pszString)
{
    RESULT r = R_SUCCESS;
	CR(pc->PrintToOutput(pszString));	
	CR(pc->PrintToOutput('\n'));		// Add a new line at the end of the string
Error:
    return r;
}

RESULT Help(Console *pc, char *pszFunctionName)
{
    RESULT r = R_SUCCESS;

    printf("wowa:%s", pszFunctionName);

Error:
    return r;
}

// Will list off all the available functions
RESULT List(Console *pc)
{
	RESULT r = R_SUCCESS;

	list<ConsoleFunction> *pCommandList = pc->GetCommandList();
	for(list<ConsoleFunction>::iterator it = pCommandList->First(); it != NULL; it++)
	{
		CR(pc->PrintToOutput((*it).pszCommand));
		CR(pc->PrintToOutput('\n'));
	}

Error:
	return r;
}
// **************************************

RESULT Console::ConvertToArgumentList(char *d_InputBuffer, char** &n_Args, int &rn_Args_n)
{
    RESULT r = R_SUCCESS;
    list<char*> ArgsList;
    char *TempChar;
    
    rn_Args_n = 0;
    n_Args = 0;

    TempChar = strtok(d_InputBuffer, " -,;");
    while(TempChar != NULL)
    {
        ArgsList.Append(TempChar);  
        TempChar = strtok(NULL, " -,;");
    }

    rn_Args_n = ArgsList.Size();
    n_Args = new char*[rn_Args_n];

    int i = 0;
    while(!ArgsList.Empty())
    {
        CBRM((i < rn_Args_n), "Incompatible array size for parameter passing!");
        n_Args[i] = ArgsList.Pop();
        i++;
    }    

Error:
    //delete d_InputBuffer;
    return r;
}

RESULT Console::ExecuteConsoleFunction(char *pszCommand)
{
    RESULT r = R_SUCCESS;
    char TempCharBuffer[MAX_COMMAND];
    list<ConsoleFunction>::iterator it;
    long int CallbackRet = -1;

    // First copy over the buffer to our own thing
    char *TempBuffer = new char[strlen(pszCommand)];
    memset(TempBuffer, 0, strlen(pszCommand));
    strcpy(TempBuffer, pszCommand);

    // Parse the command buffer and extract the cmd and params
    char** Args = NULL;
    int Args_n;
	bool fUseDefaultArguments = false;
    CRM(ConvertToArgumentList(TempBuffer, Args, Args_n), "Failed to convert command to arguments list");

    // Search for the command
    for(it = m_Commands.First(); it != NULL; it++)
    {
        if(strcmp(static_cast<ConsoleFunction>(*it).pszCommand, Args[0]) == 0)        
            break;        
    }

    // Check to see that we found the command
    if(it == NULL)
        goto Error;

    // Check to see that the number of parser arguments matches the function
	// and if not we check if the function has default arguments
    if(static_cast<ConsoleFunction>(*it).arguments != Args_n )
    {
		if(static_cast<ConsoleFunction>(*it).ppszDefaultArguments_n > 0)
		{
			fUseDefaultArguments = true;	
		}
		else
		{
			printf("Expected arguments %d got %d\n", static_cast<ConsoleFunction>(*it).arguments, Args_n);
			goto Error;
		}
    }



    switch(static_cast<ConsoleFunction>(*it).arguments)
    {
        case 1:
        {
            CallbackRet = (long int)((FP1)(static_cast<ConsoleFunction>(*it).Function))(this);
            PrintToOutput("Callback returned:%d\n", CallbackRet);
        } break;

		case 2:
		{
			if(!fUseDefaultArguments)
				CallbackRet = (long int)((FP2)(static_cast<ConsoleFunction>(*it).Function))(this, Args[1]);
			else
				CallbackRet = (long int)((FP2)(static_cast<ConsoleFunction>(*it).Function))(this, static_cast<ConsoleFunction>(*it).ppszDefaultArguments[0]);

			PrintToOutput("Callback returned: %d\n", CallbackRet);
		} break;

		case 3:
		{
			if(!fUseDefaultArguments)
				CallbackRet = (long int)((FP3)(static_cast<ConsoleFunction>(*it).Function))(this, Args[1], Args[2]);
			else
				CallbackRet = (long int)((FP3)(static_cast<ConsoleFunction>(*it).Function))(this, static_cast<ConsoleFunction>(*it).ppszDefaultArguments[0]
																						   , static_cast<ConsoleFunction>(*it).ppszDefaultArguments[1]);

			PrintToOutput("Callback returned: %d\n", CallbackRet);
		} break;

		case 4:
		{
			if(!fUseDefaultArguments)
				CallbackRet = (long int)((FP4)(static_cast<ConsoleFunction>(*it).Function))(this, Args[1], Args[2], Args[3]);
			else
				CallbackRet = (long int)((FP4)(static_cast<ConsoleFunction>(*it).Function))(this, static_cast<ConsoleFunction>(*it).ppszDefaultArguments[0],
																						     static_cast<ConsoleFunction>(*it).ppszDefaultArguments[1],
																							 static_cast<ConsoleFunction>(*it).ppszDefaultArguments[2]);
			PrintToOutput("Callback returned: %d\n", CallbackRet);
		} break;

		case 5:
		{
			if(!fUseDefaultArguments)
				CallbackRet = (long int)((FP5)(static_cast<ConsoleFunction>(*it).Function))(this, Args[1], Args[2], Args[3], Args[4]);
			else
				CallbackRet = (long int)((FP5)(static_cast<ConsoleFunction>(*it).Function))(this, static_cast<ConsoleFunction>(*it).ppszDefaultArguments[0],
																						     static_cast<ConsoleFunction>(*it).ppszDefaultArguments[1],
																							 static_cast<ConsoleFunction>(*it).ppszDefaultArguments[2],
																							 static_cast<ConsoleFunction>(*it).ppszDefaultArguments[3]);
			PrintToOutput("Callback returned: %d\n", CallbackRet);
		} break;

		case 6:
		{
			if(!fUseDefaultArguments)
				CallbackRet = (long int)((FP6)(static_cast<ConsoleFunction>(*it).Function))(this, Args[1], Args[2], Args[3], Args[4], Args[5]);
			else
				CallbackRet = (long int)((FP6)(static_cast<ConsoleFunction>(*it).Function))(this, static_cast<ConsoleFunction>(*it).ppszDefaultArguments[0],
																						     static_cast<ConsoleFunction>(*it).ppszDefaultArguments[1],
																							 static_cast<ConsoleFunction>(*it).ppszDefaultArguments[2],
																							 static_cast<ConsoleFunction>(*it).ppszDefaultArguments[3],
																							 static_cast<ConsoleFunction>(*it).ppszDefaultArguments[4]);
			PrintToOutput("Callback returned: %d\n", CallbackRet);
		} break;

		case 7:
		{
			if(!fUseDefaultArguments)
				CallbackRet = (long int)((FP7)(static_cast<ConsoleFunction>(*it).Function))(this, Args[1], Args[2], Args[3], Args[4], Args[5], Args[6]);
			else
				CallbackRet = (long int)((FP7)(static_cast<ConsoleFunction>(*it).Function))(this, static_cast<ConsoleFunction>(*it).ppszDefaultArguments[0],
																						     static_cast<ConsoleFunction>(*it).ppszDefaultArguments[1],
																							 static_cast<ConsoleFunction>(*it).ppszDefaultArguments[2],
																							 static_cast<ConsoleFunction>(*it).ppszDefaultArguments[3],
																							 static_cast<ConsoleFunction>(*it).ppszDefaultArguments[4],
																							 static_cast<ConsoleFunction>(*it).ppszDefaultArguments[5]);
			PrintToOutput("Callback returned: %d\n", CallbackRet);
		} break;
    
        default:
        {  
            PrintToOutput("Console does not support %d arguments\n", static_cast<ConsoleFunction>(*it).arguments);
        } break;
    }

    //delete TempBuffer;
    return (RESULT)CallbackRet;
Error:
    PrintToOutput("Could not execute:%s\n", pszCommand);
    //delete TempBuffer;
    return r;
}

RESULT Console::AddConsoleFunction(ConsoleFunction cf)
{
    RESULT r = R_SUCCESS;
    char TempCharBuffer[MAX_COMMAND];   

    for(list<ConsoleFunction>::iterator it = m_Commands.First(); it != NULL; it++)
    {
        if((strcmp(static_cast<ConsoleFunction>(*it).pszCommand, cf.pszCommand) == 0 ) || 
           (static_cast<ConsoleFunction>(*it).Function == cf.Function))
        {
            PrintToOutput("Console Function %s already present!\n", cf.pszCommand);
            CBRM(0, "");
        }

        if(static_cast<ConsoleFunction>(*it).ID == cf.ID)
        {
            PrintToOutput("Console Function ID %d taken, assigned new id %d\n", cf.ID, m_nFunctionID);
            cf.ID = m_nFunctionID;
            m_nFunctionID++;
        }
    }
        
    // Assign an id if one is not given already
    if(cf.ID <= 0)
    {
        cf.ID = m_nFunctionID;
        m_nFunctionID++;
    }

    m_Commands.Push(cf);
    PrintToOutput("Added:%s with %d args\n", cf.pszCommand, cf.arguments);

    ResetCommandBuffer();
        
Error:
    return r;
}

Console::Console() :
    m_InputBuffer_n(0),
    //m_OutputBuffer_n(0),
    m_nFunctionID(CONSOLE_RESERVED_END),
    m_pOutputBuffer(NULL)
{
    m_pOutputBuffer = new SmartBuffer();
    
    memset(m_InputBuffer, 0, sizeof(m_InputBuffer));
    //memset(m_OutputBuffer, 0, sizeof(m_OutputBuffer));

    // Add the reserved functions
    for(int i = 0; i < ReservedFunctions_n; i++)
        AddConsoleFunction(ReservedFunctions[i]);
}

Console::~Console()
{
    // empty
}

RESULT Console::Path(char* &n_pszPath)
{
    RESULT r = R_SUCCESS;
    
    n_pszPath = new char[strlen(">")];
    CNRM(n_pszPath, "Could not allocate the path string");
    strcpy(n_pszPath, ">");

Error:
    return r;
}

RESULT Console::ResetCommandBuffer()
{
    RESULT r = R_SUCCESS;
    char *tempPath = NULL;

    memset(m_InputBuffer, 0, sizeof(m_InputBuffer));
    m_InputBuffer_n = 0;

    CRM(Path(tempPath), "Cant make path!");

    //printf("%s", tempPath);
    //sprintf(TempBuffer, "%s", tempPath);
    PrintToOutput(tempPath);

Error:
    if(tempPath != NULL) 
    {
        //delete tempPath;
    }
    return r;
}

RESULT Console::PrintToOutput(char c)
{
    RESULT r = R_SUCCESS;
    m_pOutputBuffer->Append(c);
Error:
    return r;
}

RESULT Console::PrintToOutput(char* pszOutput)
{
    RESULT r = R_SUCCESS;

    for(int i = 0; i < strlen(pszOutput); i++)
        CRM(PrintToOutput(pszOutput[i]), "Could not print char to output");

Error:
    return r;
}

RESULT Console::PrintToOutput(const char* output_format, ...)
{
    RESULT r = R_SUCCESS;

    va_list ap;
    va_start(ap, output_format);

    for(int i = 0; i < strlen(output_format); i++)
    {
        if(output_format[i] == '%')
        {
            // first increment i to skip this letter
            i++;
            switch(output_format[i])
            {            
                case 'd':
                {
                    int InputInt = va_arg(ap, int);
                    char IntBuffer[MAX_COMMAND];
                    //itoa(InputInt, IntBuffer, 10);
                    snprintf(IntBuffer, MAX_COMMAND, "%d", InputInt);
                    CRM(PrintToOutput(IntBuffer), "Could not print int buffer!");
                } break;   

                case 'c':
                {
                    char InputChar = va_arg(ap, char);
                    CRM(PrintToOutput(InputChar), "Could not print char to buffer!");
                } break;

                case 'h':
                {
                    char InputValue = va_arg(ap, int);
                    char HexBuffer[MAX_COMMAND];
                    //itoa(InputValue, HexBuffer, 16);
                    snprintf(HexBuffer, MAX_COMMAND, "%x", InputValue);
                    CRM(PrintToOutput(HexBuffer), "Could not print hex buffer!");
                } break;

                // For wide strings
                case 'S':
                {
                    wchar_t *wpszInput = va_arg(ap, wchar_t*);
                    char WideBuffer[MAX_COMMAND];
                    int Converted_n = wcstombs(WideBuffer, wpszInput, MAX_COMMAND);
                    CRM(PrintToOutput(WideBuffer), "Could not print wide string buffer!");
                } break;

                case 's':
                {
                    char* InputBuffer = va_arg(ap, char*);
                    CRM(PrintToOutput(InputBuffer), "Could not print input buffer!");
                } break;

                default:
                {
                    CBRM(0, "Unknown format specifier");
                }
            }
        }
        else if(output_format[i] == '\\')
        {
            i++;
            switch(output_format[i])
            {
                case 'n':
                {
                    CRM(PrintToOutput('\n'), "Could not print newline to output");
                } break;

                default:
                {
                    CBRM(0, "Unknown special character");
                } break;
            }
        }
        else
        {
            CRM(PrintToOutput(output_format[i]), "Could not print char to output!");
        }
    }

Error:
    va_end(ap);
    return r;
}

RESULT Console::ReceiveInput(char c)
{
    RESULT r = R_SUCCESS;
    char TempChar[MAX_COMMAND];    
	
	//printf("%d", c);

    switch(c)
    {
        case '\n':
        case '\r':
        {
            PrintToOutput('\n');
			
			// Add the current command to the history buffer
			char *pszInputHistory = new char[m_InputBuffer_n];
			memset(pszInputHistory, 0, m_InputBuffer_n);
			strcpy(pszInputHistory, m_InputBuffer);
			           
            m_InputBufferHistory.Append(pszInputHistory);
            
			if(m_InputBuffer_n > 0)
			{
				// Execute the command here
				r = ExecuteConsoleFunction(m_InputBuffer);
			}

            // Reset the command buffer
            ResetCommandBuffer();

        } break;

        case 8:
        {
            if(m_InputBuffer_n != 0)
            {            
                // back space then erase the character and then backspace
                // again
                sprintf(TempChar, "%c %c", 8, 8);
                PrintToOutput(TempChar);
                DELETE_CHAR_FROM_STATIC_BUFFER(m_InputBuffer);
            }
        } break;

		case 24:
		{
			printf("up!");
		} break;

        default:
        {
            // Add the char to the command buffer
            ADD_CHAR_TO_STATIC_BUFFER(m_InputBuffer, c);
            PrintToOutput(c);
        } break;
    }

Error:
    return r;
}

// This function will receive input as a string of some kind
// and will in turn parse the input and update the command 
// buffer
// Note this function will destroy the buffer that it is passed
RESULT Console::ReceiveInput( char* d_input, int d_input_n )
{
    RESULT r = R_SUCCESS;

    for(int i = 0; i < d_input_n; i++)
        CRM(ReceiveInput(d_input[i]), "Since character receive failed");

Error:
	delete d_input;
    return r;
}

RESULT Console::CheckForOutput()
{
    return (m_pOutputBuffer->Length() > 0) ? R_YES : R_NO;
    //return (m_OutputBuffer_n > 0) ? R_YES : R_NO;
}

RESULT Console::DispatchOutput( char* &n_output, int &rn_output_n )
{
    RESULT r = R_SUCCESS;

    /*if(m_OutputBuffer_n > 0)
    {
        // Copy over the contents of the output buffer
        rn_output_n = m_OutputBuffer_n;
        n_output = new char[m_OutputBuffer_n];
        strcpy(n_output, m_OutputBuffer);
    }
    else
    {
        rn_output_n = 0;
        n_output = NULL;
    }*/

    if(m_pOutputBuffer->Length() > 0)
    {
        rn_output_n = m_pOutputBuffer->Length();
        n_output = m_pOutputBuffer->CreateBufferCopy();
    }
    else
    {
        rn_output_n = 0;
        n_output = NULL;
    }

Error:
    // Reset the output buffer
    m_pOutputBuffer->Reset();
    
    //m_OutputBuffer_n = 0;
    //memset(m_OutputBuffer, 0, sizeof(m_OutputBuffer));
    return r;
}

// This will dump the output history to either a file as named in
// pszFilename or to the output buffer if pszFilename is set to NULL
RESULT Console::DumpOutputHistory( char* pszFilename )
{
    RESULT r = R_SUCCESS;

	list<char*>::iterator it;
	for(it = m_InputBufferHistory.First(); it != NULL; it++)
	{
		if(pszFilename == NULL)
		{
			CR(PrintToOutput(*it));	
			CR(PrintToOutput('\n'));		// Add a new line at the end of the string
		}
	}

Error:
    return r;
}

// Args can only be char*
ConsoleFunction Console::CreateConsoleFunction( int ID, void *fun, char *pszCommand, int arguments, int nDefaultArgs, ... )
{
    va_list ap;
    va_start(ap, nDefaultArgs);

    ConsoleFunction cf = {ID, fun, pszCommand, NULL, 0, arguments};

    if(nDefaultArgs >= 0 )
    {
        char **ppszDefaultArgs = new char*[nDefaultArgs];

        for(int i = 0; i < nDefaultArgs; i++)            
            ppszDefaultArgs[i] = va_arg(ap, char*);          

        cf.ppszDefaultArguments = ppszDefaultArgs;
        cf.ppszDefaultArguments_n = nDefaultArgs;
    }

    va_end(ap);

    return cf;
}

RESULT Console::AddConsoleFunction(void *fun, char *pszCommand, int arguments, int nDefaultArgs, ... )
{
    RESULT r = R_SUCCESS;

    va_list ap;
    va_start(ap, nDefaultArgs);

    ConsoleFunction cf = {0, fun, pszCommand, NULL, 0, arguments};

    if(nDefaultArgs > 0 )
    {
        char **ppszDefaultArgs = new char*[nDefaultArgs];

        for(int i = 0; i < nDefaultArgs; i++)            
            ppszDefaultArgs[i] = va_arg(ap, char*);          

        cf.ppszDefaultArguments = ppszDefaultArgs;
        cf.ppszDefaultArguments_n = nDefaultArgs;
    }
    else
    {
        cf.ppszDefaultArguments = NULL;
        cf.ppszDefaultArguments_n = 0;
    }

    CRM(AddConsoleFunction(cf), "Failed to add console function %s", *pszCommand);

Error:
    va_end(ap);
    return r;
}

/*
RESULT TestConsole()
{
    RESULT r = R_SUCCESS;
    bool done = false;

    Console *pConsole = new Console();

    while(!done)
    {
        if(kbhit())
        {
            char *c = new char;
            c[0] = getch();
            r = pConsole->ReceiveInput(c, 1);
            if(r == R_EXIT)
            {
                done = true;
            }
        }

        // If output buffer is not empty then spit it out!
        if(pConsole->CheckForOutput() == R_YES)
        {
            char *TempOutputBuffer;
            int TempOutputBuffer_n;
            pConsole->DispatchOutput(TempOutputBuffer, TempOutputBuffer_n);
            if(TempOutputBuffer_n > 0)
            {
                printf("%s", TempOutputBuffer);
            }

        }
    }

Error:
    return r;
}
*/