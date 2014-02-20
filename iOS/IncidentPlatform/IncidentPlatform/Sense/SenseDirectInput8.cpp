# include "SenseDirectInput8.h"

// needed for dynamic cast
#include "WindowWin32.h"
#include "SenseDeviceKeyboard.h"
#include "SenseDeviceMouse.h"

// Initialize Direct Input Helper Function
RESULT InitDirectInput(LPDIRECTINPUT8 *pdiObject)
{
    RESULT r = R_OK;
    
    CHRM(DirectInput8Create(GetModuleHandle(NULL),
                                 DIRECTINPUT_VERSION,
                                 IID_IDirectInput8,
                                 (void **)pdiObject,
                                 NULL
                           ), "DirectInput8Create() Failed!\nEnding Program\n");

Error:
    return r;
}

// Reference (context) structure for enumerate devices callback
// function which will allow us to poll all of the different 
// devices!  Only use one reference object.
// *********************************************************************************
#define DIENUMREFERENCEOBJECT_ID 42

struct DIEnumReferenceObject
{
    DIOBJECTDATAFORMAT pdiodfKeyboard[SENSE_DEVICE_KEYBOARD_SIZE];
    DIOBJECTDATAFORMAT pdiodfMouse[SENSE_DEVICE_MOUSE_ELEMENT_MAX_FIELDS];  // Mouse only has one device state

    int pdiodfKeyboard_n; 
    int pdiodfMouse_n;
    int ObjectID;
};

RESULT CreateDIEnumKeyboardReferenceObject(DIEnumReferenceObject* &n_pd)
{
    RESULT r = R_OK;
    n_pd = new DIEnumReferenceObject;
    memset(n_pd, 0, sizeof(DIEnumReferenceObject));
    
    //n_pd->pdiodf = new DIOBJECTDATAFORMAT[SENSE_DEVICE_KEYBOARD_SIZE];
    
    n_pd->pdiodfKeyboard_n = 0;
    n_pd->pdiodfMouse_n = 0;
    n_pd->ObjectID = DIENUMREFERENCEOBJECT_ID;
Error:
    return r;
}
// *********************************************************************************

// Note that at the moment everything is being "flattened" to pure
// ASCII but in the long run we should implement a system similar to
// direct input so more HID capabilities can be taken advantage of!
// This would involve a more sophisticated logic in SenseDeviceKeyboard 
ASCII_VALUE ConvertDirectInputInstanceToASCII(int DIInstance)
{
    #define CDIITA_CASE_DINSTANCE_TO_ASCII(val, ascii) case val: ret = ascii; break;
    ASCII_VALUE ret = ASCII_INVALID;
    
    switch(DIInstance)
    {
        // Letters
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_Z, ASCII_Z);
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_X, ASCII_X);
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_C, ASCII_C);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_V, ASCII_V);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_B, ASCII_B);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_N, ASCII_N);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_M, ASCII_M);  
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_Q, ASCII_Q);
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_W, ASCII_W);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_E, ASCII_E);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_R, ASCII_R);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_T, ASCII_T);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_Y, ASCII_Y);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_U, ASCII_U);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_I, ASCII_I);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_O, ASCII_O);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_P, ASCII_P);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_A, ASCII_A);               
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_S, ASCII_S);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_D, ASCII_D);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_F, ASCII_F);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_G, ASCII_G);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_H, ASCII_H);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_J, ASCII_J);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_K, ASCII_K);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_L, ASCII_L); 
        // Numbers
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_1, ASCII_1);
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_2, ASCII_2);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_3, ASCII_3);               
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_4, ASCII_4);
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_5, ASCII_5);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_6, ASCII_6);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_7, ASCII_7);     
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_8, ASCII_8);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_9, ASCII_9);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_0, ASCII_0);   
        // Other Stuff
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_ESCAPE, ASCII_ESC);
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_MINUS, ASCII_DASH);        /* - on main keyboard */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_EQUALS, ASCII_EQUAL);
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_BACK, ASCII_BACKSPACE);                /* backspace */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_TAB, ASCII_TAB);
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_LBRACKET, ASCII_LBRACKET);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_RBRACKET, ASCII_RBRACKET);
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_RETURN, ASCII_CARRIAGE_RETURN);              /* Enter on main keyboard */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_SEMICOLON, ASCII_SEMICOLON);       
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_APOSTROPHE, ASCII_APOSTROPHE);
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_BACKSLASH, ASCII_BSLASH);       
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_COMMA, ASCII_COMMA);    
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_PERIOD, ASCII_PERIOD);              /* . on main keyboard */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_SLASH, ASCII_FSLASH);               /* / on main keyboard */       
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_SPACE, ASCII_SPACE);           
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_AT, ASCII_AT);                  /*                     (NEC PC98) */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_COLON, ASCII_COLON); /*                     (NEC PC98) */   
        // Number Pad
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_DIVIDE, ASCII_FSLASH);              /* / on numeric keypad */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_NUMPADENTER, ASCII_CARRIAGE_RETURN);         /* Enter on numeric keypad */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_MULTIPLY, ASCII_ASTERIX);            /* * on numeric keypad */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_NUMPAD7, ASCII_7);         
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_NUMPAD8, ASCII_8);
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_NUMPAD9, ASCII_9);
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_SUBTRACT, ASCII_DASH);            /* - on numeric keypad */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_NUMPAD4, ASCII_4);
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_NUMPAD5, ASCII_5);
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_NUMPAD6, ASCII_6);
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_ADD, ASCII_PLUS);                 /* + on numeric keypad */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_NUMPAD1, ASCII_1);
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_NUMPAD2, ASCII_2);
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_NUMPAD3, ASCII_3);
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_NUMPAD0, ASCII_0);
        // Arrow Keypad (Same thing)
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_DECIMAL, ASCII_PERIOD);             /* . on numeric keypad */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_INSERT, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/              /* Insert on arrow keypad */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_DELETE, ASCII_DEL);              /* Delete on arrow keypad */ 
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_HOME, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                /* Home on arrow keypad */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_UP, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                  /* UpArrow on arrow keypad */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_PRIOR, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/               /* PgUp on arrow keypad */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_LEFT, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                /* LeftArrow on arrow keypad */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_RIGHT, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/               /* RightArrow on arrow keypad */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_END, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                 /* End on arrow keypad */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_DOWN, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                /* DownArrow on arrow keypad */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_NEXT, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                /* PgDn on arrow keypad */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_NUMPADCOMMA, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/         /* , on numeric keypad (NEC PC98) */
        // Control Keys
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_LMENU, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/               /* left Alt */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_NUMLOCK, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/         
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_SCROLL, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/              /* Scroll Lock */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_LCONTROL, ASCII_INVALID);            /*SenseDeviceKeyboard FIX!!*/   
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_RCONTROL, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/        
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_CAPITAL, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/  
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_RSHIFT, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/   
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_LSHIFT, ASCII_INVALID); /*SenseDeviceKeyboard FIX!!*/
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_RMENU, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/               /* right Alt */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_LWIN, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                /* Left Windows key */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_RWIN, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                /* Right Windows key */
        // Function Keys
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_F1, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/              
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_F2, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/              
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_F3, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/              
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_F4, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/              
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_F5, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/              
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_F6, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/              
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_F7, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/              
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_F8, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/              
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_F9, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/              
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_F10, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/     
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_F11, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/  
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_F12, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/             
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_F13, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                 /*                     (NEC PC98) */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_F14, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                 /*                     (NEC PC98) */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_F15, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                 /*                     (NEC PC98) */
        // Extended Functionality Keys
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_NEXTTRACK, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/           /* Next Track */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_MUTE, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                /* Mute */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_CALCULATOR, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/          /* Calculator */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_PLAYPAUSE, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/           /* Play / Pause */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_MEDIASTOP, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/           /* Media Stop */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_VOLUMEDOWN, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/          /* Volume - */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_VOLUMEUP, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/            /* Volume + */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_WEBHOME, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/             /* Web home */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_PAUSE, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/               /* Pause */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_APPS, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                /* AppMenu key */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_POWER, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/               /* System Power */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_SLEEP, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/               /* System Sleep */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_WAKE, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                /* System Wake */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_WEBSEARCH, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/           /* Web Search */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_WEBFAVORITES, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/        /* Web Favorites */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_WEBREFRESH, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/          /* Web Refresh */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_WEBSTOP, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/             /* Web Stop */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_WEBFORWARD, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/          /* Web Forward */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_WEBBACK, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/             /* Web Back */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_MYCOMPUTER, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/          /* My Computer */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_MAIL, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                /* Mail */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_MEDIASELECT, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/         /* Media Select */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_SYSRQ, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/    
        // Unsupported          
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_GRAVE, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/               /* accent grave */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_OEM_102, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/             /* <> or \| on RT 102-key keyboard (Non-U.S.) */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_KANA, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                /* (Japanese keyboard)            */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_ABNT_C1, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/             /* /? on Brazilian keyboard */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_CONVERT, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/             /* (Japanese keyboard)            */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_NOCONVERT, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/           /* (Japanese keyboard)            */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_YEN, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                 /* (Japanese keyboard)            */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_ABNT_C2, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/             /* Numpad . on Brazilian keyboard */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_NUMPADEQUALS, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/        /* = on numeric keypad (NEC PC98) */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_PREVTRACK, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/           /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_UNDERLINE, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/           /*                     (NEC PC98) */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_KANJI, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/               /* (Japanese keyboard)            */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_STOP, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                /*                     (NEC PC98) */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_AX, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/                  /*                     (Japan AX) */
        CDIITA_CASE_DINSTANCE_TO_ASCII(DIK_UNLABELED, ASCII_INVALID);   /*SenseDeviceKeyboard FIX!!*/           /*                        (J3100) */      
        default: ret = ASCII_INVALID;
    }
    return ret;
}
#undef CDIITA_CASE_DINSTANCE_TO_ASCII

// We need to translate the device properties to ASCII for keyboard
BOOL WINAPI DIEnumKeyboardDeviceObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
    DIEnumReferenceObject *diero = reinterpret_cast<DIEnumReferenceObject*>(pvRef);

    if(diero == NULL || diero->ObjectID != DIENUMREFERENCEOBJECT_ID)
    {
        printf("Passed in incorrect reference object!\n");
        return false;
    }

    ASCII_VALUE av = ConvertDirectInputInstanceToASCII(DIDFT_GETINSTANCE(lpddoi->dwType));
    
    printf("KeyboardEnum: Found Direct Input Device:%S:%d type:%d\n", lpddoi->tszName, diero->pdiodfKeyboard_n, DIDFT_GETINSTANCE(lpddoi->dwType));
    
    if(av != ASCII_INVALID)
    {
        diero->pdiodfKeyboard[diero->pdiodfKeyboard_n].pguid = &lpddoi->guidType;       // GUID type
        diero->pdiodfKeyboard[diero->pdiodfKeyboard_n].dwOfs = (DWORD)av;               // Offset
        diero->pdiodfKeyboard[diero->pdiodfKeyboard_n].dwType = lpddoi->dwType;         // DI Type
        diero->pdiodfKeyboard[diero->pdiodfKeyboard_n].dwFlags = lpddoi->dwFlags;       // Flags                                     
        diero->pdiodfKeyboard_n++;
    }
    else 
    {
        printf("ASCII Value Invalid and not supported\n");
    }

    return true;
}

#define DI_ENUM_MOUSE_DEVICE_XAXIS 0
#define DI_ENUM_MOUSE_DEVICE_YAXIS 1
#define DI_ENUM_MOUSE_DEVICE_WHEEL 2
#define DI_ENUM_MOUSE_DEVICE_BUTTON(x) ((DI_ENUM_MOUSE_DEVICE_WHEEL + 1) + x)

BOOL WINAPI DIEnumMouseDeviceObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
    RESULT r = R_OK;
    DIEnumReferenceObject *diero = reinterpret_cast<DIEnumReferenceObject*>(pvRef);

    if(diero == NULL || diero->ObjectID != DIENUMREFERENCEOBJECT_ID)
    {
        printf("Passed in incorrect reference object!\n");
        return false;
    }

    printf("MouseEnum: Found Direct Input Device:%S:%d type:%d\n", lpddoi->tszName, diero->pdiodfKeyboard_n, DIDFT_GETINSTANCE(lpddoi->dwType));

    int offset = 0;

    switch(DIDFT_GETINSTANCE(lpddoi->dwType))
    {
        case DI_ENUM_MOUSE_DEVICE_XAXIS:    
        {
            offset = STRUCT_FIELD_OFFSET(SenseDeviceMouseElement, xaxis);
        } break;

        case DI_ENUM_MOUSE_DEVICE_YAXIS:    
        {
            offset = STRUCT_FIELD_OFFSET(SenseDeviceMouseElement, yaxis);
        } break;

        case DI_ENUM_MOUSE_DEVICE_WHEEL:    
        {
            offset = STRUCT_FIELD_OFFSET(SenseDeviceMouseElement, wheel);
        } break;

        case DI_ENUM_MOUSE_DEVICE_BUTTON(0):    
        {
            offset = STRUCT_FIELD_OFFSET(SenseDeviceMouseElement, lbutton);
        } break;

        case DI_ENUM_MOUSE_DEVICE_BUTTON(1):    
        {
            offset = STRUCT_FIELD_OFFSET(SenseDeviceMouseElement, rbutton);
        } break;

        case DI_ENUM_MOUSE_DEVICE_BUTTON(2):    
        {
            offset = STRUCT_FIELD_OFFSET(SenseDeviceMouseElement, mbutton);
        } break;

        case DI_ENUM_MOUSE_DEVICE_BUTTON(3):    
        {
            offset = STRUCT_FIELD_OFFSET(SenseDeviceMouseElement, button3);
        } break;

        case DI_ENUM_MOUSE_DEVICE_BUTTON(4):    
        {
            offset = STRUCT_FIELD_OFFSET(SenseDeviceMouseElement, button4);
        } break;

        case DI_ENUM_MOUSE_DEVICE_BUTTON(5):    
        {
            offset = STRUCT_FIELD_OFFSET(SenseDeviceMouseElement, button5);
        } break;

        case DI_ENUM_MOUSE_DEVICE_BUTTON(6):    
        {
            offset = STRUCT_FIELD_OFFSET(SenseDeviceMouseElement, button6);
        } break;

        default:
        {
            CBRM(0, "MouseEnum: Invalid Device Instance");
        } break;
                                        
    }

    CBRM((offset >= 0), "Invalid offset!");

    printf("offset:%d\n", offset);

    // GUID and Type
    if(lpddoi->guidType == GUID_XAxis)        
        diero->pdiodfMouse[diero->pdiodfMouse_n].pguid = &GUID_XAxis;            
    else if(lpddoi->guidType == GUID_YAxis)    
        diero->pdiodfMouse[diero->pdiodfMouse_n].pguid = &GUID_YAxis;           
    else if(lpddoi->guidType == GUID_ZAxis)    
        diero->pdiodfMouse[diero->pdiodfMouse_n].pguid = &GUID_ZAxis;           
    else if(lpddoi->guidType == GUID_Button)    
        diero->pdiodfMouse[diero->pdiodfMouse_n].pguid = &GUID_Button;               
    else
    {
        printf("Invalid GUID!\n");
        return false;
    }

    diero->pdiodfMouse[diero->pdiodfMouse_n].dwType = lpddoi->dwType;
    diero->pdiodfMouse[diero->pdiodfMouse_n].dwOfs = (DWORD)offset;
    diero->pdiodfMouse[diero->pdiodfMouse_n].dwFlags = lpddoi->dwFlags;               //Flags
    diero->pdiodfMouse_n++;

    return true;

Error:
    return false;
}

RESULT SenseDeviceElementDataFormat_Keyboard(DIOBJECTDATAFORMAT *pdiodf, int NumObjects, DIDATAFORMAT* &n_pdidf)
{
    RESULT r = R_OK;
    
    n_pdidf = new DIDATAFORMAT;
    memset(n_pdidf, 0, sizeof(DIDATAFORMAT));
       
    n_pdidf->dwSize = sizeof(DIDATAFORMAT);             // Self Size
    n_pdidf->dwObjSize = sizeof(DIOBJECTDATAFORMAT);    // Side of Object Data Format Structure
    n_pdidf->dwFlags = 0;                               // Special Flags
    n_pdidf->dwDataSize = sizeof(SENSE_DEVICE_KEYBOARD_ELEMENT) * SENSE_DEVICE_KEYBOARD_SIZE;    
    n_pdidf->dwNumObjs = NumObjects;                    // Number of objects
    n_pdidf->rgodf = pdiodf;                            // The DIOBJECTDATAFORMAT array

Error:
    return r;
}

RESULT SenseDeviceElementDataFormat_Mouse(DIOBJECTDATAFORMAT *pdiodf, int NumObjects, DIDATAFORMAT* &n_pdidf)
{
    RESULT r = R_OK;

    n_pdidf = new DIDATAFORMAT;
    memset(n_pdidf, 0, sizeof(DIDATAFORMAT));

    n_pdidf->dwSize = sizeof(DIDATAFORMAT);                                                 // Self Size
    n_pdidf->dwObjSize = sizeof(DIOBJECTDATAFORMAT);                                        // Side of Object Data Format Structure
    n_pdidf->dwFlags = DIDF_ABSAXIS;                                                        // Special flags need for mouse  
    n_pdidf->dwDataSize = sizeof(SENSE_DEVICE_MOUSE_ELEMENT);                               // Size of the structure passed to it
    n_pdidf->dwNumObjs = NumObjects;                                                        // Number of objects
    n_pdidf->rgodf = pdiodf;                                                                // The DIOBJECTDATAFORMAT array

Error:
    return r;
}

RESULT InitializeDirectInputDevices(LPDIRECTINPUT8 pdiObject, 
                             LPDIRECTINPUTDEVICE8 *ppdiKeyboard, 
                             LPDIRECTINPUTDEVICE8 *ppdiMouse,
                             WindowImp *pWindowImp)
{
    RESULT r = R_OK;    
    HRESULT hr = S_OK;
    
    // Keyboard First (Move into own function?)
    // *******************************************
    CHRM(pdiObject->CreateDevice(GUID_SysKeyboard,
                                      ppdiKeyboard,
                                      NULL
                                 ), "Create Keyboard Device Failed!");

    // Set up the data format to match out
    // SenseDevice structure
    LPDIRECTINPUTDEVICE8 pdidKeyboard;
    CNR(ppdiKeyboard);
    pdidKeyboard = *ppdiKeyboard;  
    
    // Create the Reference Object Device Format
    DIEnumReferenceObject *pdiero = NULL;
    CRM(CreateDIEnumKeyboardReferenceObject(pdiero), "Create DI Enum Reference Object failed");

    // Enumerate Keyboard Device
    CHRM(pdidKeyboard->EnumObjects((LPDIENUMDEVICEOBJECTSCALLBACK) DIEnumKeyboardDeviceObjectsCallback,   // Callback
                                 pdiero,                                                          // Reference Object (context)
                                 DIDFT_ALL 
                                ), "Could not enumerate keyboard devices!");


    DIDATAFORMAT *pdidfKeyboard = NULL;

    CHR(SenseDeviceElementDataFormat_Keyboard(pdiero->pdiodfKeyboard, pdiero->pdiodfKeyboard_n, pdidfKeyboard));

    //CHRM(pdidKeyboard->SetDataFormat(&c_dfDIKeyboard), "Set Data Format failed!\nEnding Program\n");
    CHRM(pdidKeyboard->SetDataFormat(pdidfKeyboard), "Set Keyboard Data Format failed!\n");

    CHRM(pdidKeyboard->SetCooperativeLevel((static_cast<WindowWin32 *>(pWindowImp))->GetHwnd(),
                                               DISCL_BACKGROUND | 
                                               DISCL_NONEXCLUSIVE
                                        ), "SetCooperativeLevel (keyboard) failed!\n");

    CHRM(pdidKeyboard->Acquire(), "Keyboard Acquire Failed!\n");

    // Mouse Second (Move into own function?)
    // *********************************************
    CHRM(pdiObject->CreateDevice(GUID_SysMouse,
                                 ppdiMouse,
                                 NULL
                                ), "Create Mouse Device Failed!");

    LPDIRECTINPUTDEVICE8 pdidMouse;
    CNR(ppdiMouse);
    pdidMouse = *ppdiMouse;
    
    // Create the Mouse Device Format
    CNRM(pdiero, "DIEnumReferenceOBject is Null!");

    // Enumerate Mouse Device
    CHRM(pdidMouse->EnumObjects((LPDIENUMDEVICEOBJECTSCALLBACK) DIEnumMouseDeviceObjectsCallback,   // Callback
                                pdiero,                                                             // Reference Object (context)
                                DIDFT_ALL                                                           // Enumerate all devices
                               ), "Could not enumerate mouse devices!");
    
    DIDATAFORMAT *pdidfMouse = NULL; 

    CHR(SenseDeviceElementDataFormat_Mouse(pdiero->pdiodfMouse, pdiero->pdiodfMouse_n, pdidfMouse));

    //CHRM(pdidMouse->SetDataFormat(pdidfMouse), "Set Mouse Data Format Failed!");
    CHRM(pdidMouse->SetDataFormat(&c_dfDIMouse2), "Set Mouse Data Format Failed!");

    CHRM(pdidMouse->SetCooperativeLevel((static_cast<WindowWin32 *>(pWindowImp))->GetHwnd(),
                                        DISCL_BACKGROUND | DISCL_NONEXCLUSIVE
                                       ), "SetCooperativeLevel (Mouse) failed!\n");

    CHRM(pdidMouse->Acquire(), "Mouse Acquire Failed!\n");    

    return r;
Error:
    //ppdiKeyboard = NULL;
    //ppdiMouse = NULL;
    return r;
}

RESULT SenseDirectInput8::InitializeDirectInput()
{
    RESULT r = R_OK;

    // Initialize Direct Input first
    CRM(InitDirectInput(&m_pDIObject), "Could not initialize direct input object");
    CRM(InitializeDirectInputDevices(m_pDIObject, &m_pDIKeyboard, &m_pDIMouse, m_pWindowImp), "Could not initialize direct input keyboard object");

    Validate();
    return r;

Error:
    Invalidate();
    Release();
    return r;
}

// Constructor
SenseDirectInput8::SenseDirectInput8(SENSE_IMP_TYPE sit, WindowImp *pWindowImp) :
    m_pDIObject(NULL),
    m_pDIKeyboard(NULL),
    m_pWindowImp(pWindowImp)
    //m_pSenseDeviceKeyboard(NULL),
    //m_pSenseDeviceMouse(NULL)
{
    this->m_SENSE_IMP_TYPE = sit;
    
    // First Initialize the Sense Devices (since this doesn't cost us anything!)
    /*m_pSenseDeviceKeyboard = new SenseDeviceKeyboard();
    m_pSenseDeviceMouse = new SenseDeviceMouse();*/
    m_pSenseDevices[SENSE_DIRECT_INPUT_DEVICE_KEYBOARD] = new SenseDeviceKeyboard();
    m_pSenseDevices[SENSE_DIRECT_INPUT_DEVICE_MOUSE] = new SenseDeviceMouse();

    m_pSenseDeviceKeyboard = m_pSenseDevices[SENSE_DIRECT_INPUT_DEVICE_KEYBOARD];
    m_pSenseDeviceMouse = m_pSenseDevices[SENSE_DIRECT_INPUT_DEVICE_MOUSE];
    
    InitializeDirectInput();
}

// Destructor
SenseDirectInput8::~SenseDirectInput8()
{
    RESULT r = Release();
}

RESULT SenseDirectInput8::Release()
{
    RESULT r = R_OK;
    
    // Release Keyboard Device First
    if(m_pDIKeyboard != NULL)
    {
        m_pDIKeyboard->Unacquire();
        m_pDIKeyboard->Release();
        m_pDIKeyboard = NULL;
    }

    if(m_pDIMouse != NULL)
    {
        m_pDIMouse->Unacquire();
        m_pDIMouse->Release();
        m_pDIMouse = NULL;
    }

    if(m_pDIObject != NULL)
    {
        m_pDIObject->Release();
        m_pDIObject = NULL;
    }

Error:
    Invalidate();
    return R_OK;
}

RESULT SenseDirectInput8::CheckUserInput()
{
    RESULT r = R_OK;
    char *pKeystate;
    long *pXAxis;

    DIMOUSESTATE2        MouseState;


    // Get the state of the keyboard
    CHRM(m_pDIKeyboard->GetDeviceState(SENSE_DEVICE_KEYBOARD_SIZE * sizeof(SENSE_DEVICE_KEYBOARD_ELEMENT), (LPVOID)m_pSenseDeviceKeyboard->GetElements()), "GetDeviceState (keyboard) failed");

    // Get the state of the mouse
    //CHRM(m_pDIMouse->GetDeviceState(sizeof(SENSE_DEVICE_MOUSE_ELEMENT), (LPVOID)m_pSenseDeviceMouse->GetElements()), "GetDeviceState (mouse) failed");
    CHRM(m_pDIMouse->GetDeviceState(sizeof(MouseState), (LPVOID)&MouseState), "GetDeviceState (mouse) failed");

    //m_pSenseDeviceMouse->GetElement(SDM_BUTTON0, SENSE_DEVICE_ELEMENT_STATUS, (void*&)pKeystate);
    //m_pSenseDeviceMouse->GetElement(SDM_XAXIS, SENSE_DEVICE_ELEMENT_STATUS, (void*&)pXAxis);

    //if(pKeystate != 0)
    if(MouseState.rgbButtons[0] != 0)
    {
        printf("space:%d!\n\n", MouseState.lX);
    }

    return r;
Error:
    if(r == DIERR_INVALIDPARAM) printf("Invalid Parameter\n");
    else if(r == DIERR_INPUTLOST) printf("Input Lost\n");
    else if(r == DIERR_NOTACQUIRED) printf("Not Acquired\n");
    else if(r == DIERR_NOTINITIALIZED) printf("Not Initialized\n");
    else if(r == E_PENDING) printf("Pending\n");
    return r;
}

// Return if sense object is valid
bool SenseDirectInput8::ValidSense()
{
    // Ensure devices initialized correctly
    return (m_pDIObject != NULL) && 
           (m_pDIKeyboard != NULL) && IsValid();
}

int SenseDirectInput8::GetNumSenseDevices()
{
    return SENSE_DIRECT_INPUT_NUM_DEVICES;
}

SenseDevice* SenseDirectInput8::GetSenseDevice( int ID )
{
    switch(ID)
    {
        case SENSE_DIRECT_INPUT_DEVICE_KEYBOARD:
        {
            return m_pSenseDeviceKeyboard;
        } break;

        case SENSE_DIRECT_INPUT_DEVICE_MOUSE:
        {
            return m_pSenseDeviceMouse;
        } break;

        default: 
        {
            printf("Invalid SenseDevice ID\n");
            return NULL;
        } break;
    }
}

RESULT SenseDirectInput8::GetSenseDeviceInfo( int ID, SENSE_DEVICE_INFO sdi, void* n_SDInfo )
{
    RESULT r = R_OK;

    CBRM((ID >= 0), "SenseWinMM: GetSenseInfo: Device ID cannot be less than 0");

    if(ID > SENSE_DIRECT_INPUT_NUM_DEVICES)
    {
        // While not a failure, we should not continue
        return R_OK;
    }

    switch(sdi)
    {        
        case SENSE_DEVICE_INFO_NAME:
        {
            // This will return a null terminated string
            if(ID == SENSE_DIRECT_INPUT_DEVICE_KEYBOARD)
            {
                n_SDInfo = new char[strlen(SENSE_DIRECT_INPUT_DEVICE_KEYBOARD_NAME)];
                strcpy((char*)(n_SDInfo), SENSE_DIRECT_INPUT_DEVICE_KEYBOARD_NAME);
            }
            else if(ID == SENSE_DIRECT_INPUT_DEVICE_MOUSE)
            {
                n_SDInfo = new char[strlen(SENSE_DIRECT_INPUT_DEVICE_MOUSE_NAME)];
                strcpy((char*)(n_SDInfo), SENSE_DIRECT_INPUT_DEVICE_MOUSE_NAME);
            }
        } break;

        case SENSE_DEVICE_INFO_MAX:
        {
            n_SDInfo = new int(m_pSenseDevices[ID]->GetDeviceMax());
        } break;

        case SENSE_DEVICE_INFO_MIN:
        {
            n_SDInfo = new int(m_pSenseDevices[ID]->GetDeviceMin());
        } break;

        case SENSE_DEVICE_INFO_RANGE:
        {
            n_SDInfo = new int(m_pSenseDevices[ID]->GetDeviceRange());
        } break;

        case SENSE_DEVICE_INFO_TYPE:
        {
            // This will return a SENSE_DEVICE_TYPE defined            
            n_SDInfo = new SENSE_DEVICE_TYPE(m_pSenseDevices[ID]->GetDeviceType());
        } break;

        default:
        {
            n_SDInfo = NULL;
        } break;
    }    

Error:
    return r;
}

/*
RESULT SenseDirectInput8::SetSenseDevice(SenseDevice *pSenseDevice)
{
    RESULT r = R_OK;

    CRM(Release(), "SenseDirectInput8::SetSenseDevice");

    m_pSenseDevice = pSenseDevice;
    
    CRM(InitializeDirectInput(), "SenseDirectInput8::InitializeDirectInput");

Error:
    return r;
}
*/