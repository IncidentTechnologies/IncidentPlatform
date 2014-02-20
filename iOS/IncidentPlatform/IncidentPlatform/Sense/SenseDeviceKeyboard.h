#pragma once

// SenseDeviceKeyboard is a device implementation for the input sub system that
// allows for abstraction of the Keyboard Device and will allow for blind reading of the
// status based on ASCII values.  Will also have some reserved keys for special keyboard keys (like volume+/-)

#define SENSE_DEVICE_KEYBOARD_SIZE 256
#define SENSE_DEVICE_KEYBOARD_MAX_NAME 10
#define KEYBOARD_RANGE 256

#define SENSE_DEVICE_KEYBOARD_MIN 0
#define SENSE_DEVICE_KEYBOARD_MAX SENSE_DEVICE_KEYBOARD_SIZE
#define SENSE_DEVICE_KEYBOARD_RANGE (SENSE_DEVICE_KEYBOARD_MAX - SENSE_DEVICE_KEYBOARD_MAX)

typedef struct SenseDeviceKeyboardElement
{
    SENSE_DEVICE_ELEMENT_ID id;     // should always be SENSE_DEVICE_ELEMENT_ID_CHARACTER
    SenseCallback callback;         // callback for specific message
    char KeyState;
} SENSE_DEVICE_KEYBOARD_ELEMENT;

typedef enum AsciiValues
{
    ASCII_NULL,                 ASCII_START_OF_HEADING,     ASCII_START_OF_TEXT,        ASCII_END_OF_TEXT, 
    ASCII_END_OF_TRANSMISSION,  ASCII_ENQUIRY,              ASCII_ACK,                  ASCII_BELL,   
    ASCII_BACKSPACE,            ASCII_TAB,                  ASCII_LINE_FEED,            ASCII_VERTICAL_TAB,    
    ASCII_NEW_PAGE,             ASCII_CARRIAGE_RETURN,      ASCII_SHIFT_OUT,            ASCII_SHIFT_IN,    
    ASCII_DATA_LINK_ESCAPE,     ASCII_DEVICE_CONTROL_1,     ASCII_DEVICE_CONTROL_2,     ASCII_DEVICE_CONTROL_3,   
    ASCII_DEVICE_CONTROL_4,     ASCII_NAK,                  ASCII_SYN,                  ASCII_ETB,   
    ASCII_CANCEL,               ASCII_END_OF_MEDIUM,        ASCII_SUBSTITUTE,           ASCII_ESC,   
    ASCII_FILE_SEPARATOR,       ASCII_GROUP_SEPARATOR,      ASCII_RECORD_SEPARATOR,     ASCII_UNIT_SEPARATOR, ASCII_SPACE,
    ASCII_EXCLAIM,      // !
    ASCII_QUOTE,        // "
    ASCII_POUND,        // #
    ASCII_DOLLAR,         // $
    ASCII_PERCENT,      // %
    ASCII_AND,          // &
    ASCII_APOSTROPHE,   // '
    ASCII_LPAREN,       // (
    ASCII_RPAREN,       // )
    ASCII_ASTERIX,      // *
    ASCII_PLUS,         // +
    ASCII_COMMA,        // ,
    ASCII_DASH,         // -
    ASCII_PERIOD,       // .
    ASCII_FSLASH,       // /
    ASCII_0, ASCII_1, ASCII_2, ASCII_3, ASCII_4, 
    ASCII_5, ASCII_6, ASCII_7, ASCII_8, ASCII_9, 
    ASCII_COLON,        // :
    ASCII_SEMICOLON,    // ;
    ASCII_LTHAN,        // <
    ASCII_EQUAL,        // = 
    ASCII_GTHAN,        // >
    ASCII_QUEST,        // ?
    ASCII_AT,           // @
    ASCII_A, ASCII_B, ASCII_C, ASCII_D, ASCII_E, ASCII_F, ASCII_G, ASCII_H, ASCII_I, ASCII_J, 
    ASCII_K, ASCII_L, ASCII_M, ASCII_N, ASCII_O, ASCII_P, ASCII_Q, ASCII_R, ASCII_S, ASCII_T,
    ASCII_U, ASCII_V, ASCII_W, ASCII_X, ASCII_Y, ASCII_Z,
    ASCII_LBRACKET,     // [
    ASCII_BSLASH,       // \ 
    ASCII_RBRACKET,     // ]
    ASCII_CARET,        // ^
    ASCII_UNDERSCORE,   // _
    ASCII_PRIME,        // `
    ASCII_a, ASCII_b, ASCII_c, ASCII_d, ASCII_e, ASCII_f, ASCII_g, ASCII_h, ASCII_i, ASCII_j,
    ASCII_k, ASCII_l, ASCII_m, ASCII_n, ASCII_o, ASCII_p, ASCII_q, ASCII_r, ASCII_s, ASCII_t,
    ASCII_u, ASCII_v, ASCII_w, ASCII_x, ASCII_y, ASCII_z,
    ASCII_LCBRACE,      // {
    ASCII_OR,           // |
    ASCII_RCBRACE,      // }
    ASCII_TILDE,        // ~    
    ASCII_DEL,
    ASCII_INVALID       // Not a valid ASCII value!!
} ASCII_VALUE;

// Extended ASCII values are keys on the keyboard that are not 
// in the normal ASCII set.  Keyboards shouldn't have more than
// 256 values but certain keys such as (Left Shift / Right Shift)
// need to be tracked as well as the function keys, arrow keys,
// and the numerical pad
typedef enum ExtendedAsciiValues
{
    // Function Keys
    EXTENDED_ASCII_F1,
    EXTENDED_ASCII_F2,
    EXTENDED_ASCII_F3,
    EXTENDED_ASCII_F4,
    EXTENDED_ASCII_F5,
    EXTENDED_ASCII_F6,
    EXTENDED_ASCII_F7,
    EXTENDED_ASCII_F8,
    EXTENDED_ASCII_F9,
    EXTENDED_ASCII_F10,
    EXTENDED_ASCII_F11,
    EXTENDED_ASCII_F12,
    EXTENDED_ASCII_F13,
    EXTENDED_ASCII_F14,
    EXTENDED_ASCII_F15,
    // Arrow Keys
    EXTENDED_ASCII_UP,
    EXTENDED_ASCII_DOWN,
    EXTENDED_ASCII_LEFT,
    EXTENDED_ASCII_RIGHT,
    // Control Keys

    // Number Pad
    EXTENDED_ASCII_NUMPAD_0,
    EXTENDED_ASCII_NUMPAD_1,
    EXTENDED_ASCII_NUMPAD_2,
    EXTENDED_ASCII_NUMPAD_3,
    EXTENDED_ASCII_NUMPAD_4,
    EXTENDED_ASCII_NUMPAD_5,
    EXTENDED_ASCII_NUMPAD_6,
    EXTENDED_ASCII_NUMPAD_7,
    EXTENDED_ASCII_NUMPAD_8,
    EXTENDED_ASCII_NUMPAD_9,
    EXTENDED_ASCII_NUMPAD_ENTER,
    EXTENDED_ASCII_NUMPAD_PLUS,
    EXTENDED_ASCII_NUMPAD_MINUS,
    EXTENDED_ASCII_NUMPAD_MULTIPLY,
    EXTENDED_ASCII_NUMPAD_DIVIDE,
    EXTENDED_ASCII_NUMPAD_PERIOD,
    EXTENDED_ASCII_INVALID
} EXTENDED_ASCII_VALUES;

class SenseDeviceKeyboard :
    public SenseDevice
{
public:
    SenseDeviceKeyboard() :
      m_DefaultCallback(NULL),
      m_pDefaultCallbackContext(NULL),
      m_DeviceID(-1)
    {
        memset(m_SenseDeviceKeyboardState, 0, sizeof(SENSE_DEVICE_KEYBOARD_ELEMENT) * SENSE_DEVICE_KEYBOARD_SIZE);

        for(int i = 0; i < SENSE_DEVICE_KEYBOARD_SIZE; i++)
        {
            m_SenseDeviceKeyboardState[i].id = SENSE_DEVICE_ELEMENT_ID_CHARACTER;
        }
    }

    ~SenseDeviceKeyboard();

    RESULT GetElement(int index, SENSE_DEVICE_ELEMENT_TYPE sdet, void* &n_pVal)
    {
        RESULT r = R_OK;
        CBRM(index > SENSE_DEVICE_KEYBOARD_SIZE, "Keyboard Index out of Range");

        switch(sdet)
        {
            case SENSE_DEVICE_ELEMENT_STATUS:
            {
                n_pVal = new char; 
                n_pVal = (void*)m_SenseDeviceKeyboardState[index].KeyState;
            } break;

            case SENSE_DEVICE_ELEMENT_NAME:
            {
                n_pVal = new char;
                n_pVal = (void*)index;
            } break;

            default: 
            {
                printf("Invalid Sense Device Element Type!\n");
                r = R_SENSE_INVALID_PARAM;
            }
        }

Error:
        return r;
    }

    RESULT RegisterDeviceInputCallback(int index, SenseCallback Callback, void *pContext)
    {
        RESULT r = R_OK;

        CBRM((index > SENSE_DEVICE_KEYBOARD_SIZE), "Mouse element Index out of Range");

        // Allow to overwrite callbacks but output warning message
        if(m_SenseDeviceKeyboardState[index].callback != NULL)
        {
            printf("warning:MIDI Device callback overwrite\n");
        }

        m_SenseDeviceKeyboardState[index].callback = Callback;
        m_pDefaultCallbackContext = pContext;
Error:
        return r;
    }

    RESULT RegisterDefaultDeviceInputCallback(SenseCallback Callback, void *pContext)
    {
        RESULT r = R_OK;

        CNRM(Callback, "SenseDeviceKeyboard: Cannot set the callback to null");

        if(m_DefaultCallback != NULL)
            printf("warning: Overwriting Keyboard device default callback\n");

        m_DefaultCallback = Callback;
        m_pDefaultCallbackContext = pContext;

Error:
        return r;
    }

private:
    RESULT SetElement(int index, SENSE_DEVICE_ELEMENT_TYPE sdet, void* pVal)
    {
        RESULT r = R_OK;
        SenseDeviceKeyboardElement *pElement = reinterpret_cast<SenseDeviceKeyboardElement*>(pVal);

        if(pElement != NULL)
        {        
            CBRME((pElement->id == SENSE_DEVICE_ELEMENT_ID_CHARACTER), "SenseDeviceKeyboard::SetElement incorrect type", R_SENSE_INVALID_ELEMENT_ID);
            CBRM((index > SENSE_DEVICE_KEYBOARD_SIZE), "Keyboard Index out of Range");

            m_SenseDeviceKeyboardState[index].KeyState = pElement->KeyState;

            if(m_SenseDeviceKeyboardState[index].callback != NULL)
            {
                m_SenseDeviceKeyboardState[index].callback(SENSE_DEVICE_KEYBOARD, pVal, index, m_DeviceID, m_pDefaultCallbackContext);  // FIX THIS!!
            }
            else if(m_DefaultCallback != NULL)
            {
                m_DefaultCallback(SENSE_DEVICE_KEYBOARD, pVal, index, m_DeviceID, m_pDefaultCallbackContext);
            }

            /*
            switch(sdet)
            {
                case SENSE_DEVICE_ELEMENT_STATUS:
                    {
                        m_SenseDeviceKeyboardState[index].KeyState = (char)pVal;
                    } break;
                
                default: 
                {
                    printf("Invalid Sense Device Element Type!\n");
                    r = R_SENSE_INVALID_PARAM;
                } break;
            }
            */
        }
   
Error:
        return r;
    }

    void* GetElements()
    {   
        return (void*)m_SenseDeviceKeyboardState;
    }

    int GetDeviceRange()
    {
        return SENSE_DEVICE_KEYBOARD_SIZE;
    }

    int GetDeviceMin()
    {
        return SENSE_DEVICE_KEYBOARD_MIN;
    }

    int GetDeviceMax()
    {
        return SENSE_DEVICE_KEYBOARD_MAX;
    }

    SENSE_DEVICE_TYPE GetDeviceType()
    {
        return SENSE_DEVICE_KEYBOARD;
    }

    int GetDeviceID(){ return m_DeviceID; }
    RESULT SetDeviceID(int id){ m_DeviceID = id; return R_OK; }

private:
//public:
    SENSE_DEVICE_KEYBOARD_ELEMENT m_SenseDeviceKeyboardState[SENSE_DEVICE_KEYBOARD_SIZE];
    SenseCallback m_DefaultCallback;        // This is to allow registration of one call back, preempted by specific callbacks
    void *m_pDefaultCallbackContext;
    int m_DeviceID;
};