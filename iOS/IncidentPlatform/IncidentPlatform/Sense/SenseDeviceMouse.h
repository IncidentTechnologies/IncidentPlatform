// Sense Device Mouse Implementation
// this will allow the use of a mouse
// as an input device!

#pragma once

// The other limits are defined by the size and minimum 
// of the device
#define SENSE_DEVICE_MOUSE_SIZE 1
#define SENSE_DEVICE_MOUSE_MIN 0

#define SENSE_DEVICE_MOUSE_MAX (SENSE_DEVICE_MOUSE_SIZE + SENSE_DEVICE_MOUSE_MIN)
#define SENSE_DEVICE_MOUSE_RANGE (SENSE_DEVICE_MOUSE_MAX - SENSE_DEVICE_MOUSE_MIN)

#define SENSE_DEVICE_MOUSE_ELEMENT_FIELDS 10
#define SENSE_DEVICE_MOUSE_ELEMENT_MAX_FIELDS 64

typedef enum SenseDeviceMouseField
{
    SDM_XAXIS,
    SDM_YAXIS,
    SDM_ZAXIS,
    SDM_BUTTON0,
    SDM_BUTTON1,
    SDM_BUTTON2,
    SDM_BUTTON3,
    SDM_BUTTON4,
    SDM_BUTTON5,
    SDM_BUTTON6,
    SDM_BUTTON7,
    SDM_INVALID
} SENSE_DEVICE_MOUSE_FIELD;

// Since the mouse consists of a hybrid combination of inputs some of which are
// we do not use elements but rather one larger structure that encapsulates the 
// device completely
// Must be of 4 byte alignment
typedef struct SenseDeviceMouseElement
{
    SENSE_DEVICE_ELEMENT_ID id;     // should always be SENSE_DEVICE_ELEMENT_ID_MOUSE
    SenseCallback callback;         // callback
    /*NOTE: Should these be floats?*/
    long xaxis;    // horizontal axis
    long yaxis;    // vertical axis
    long wheel;    // Jog-Wheel position
    /*/NOTE*/
    char lbutton;   // button 0
    char rbutton;   // button 1
    char mbutton;   // button 2
    char button3;   // button 3
    char button4;   // button 4
    char button5;   // button 5
    char button6;   // button 6
    char button7;   // button 7
    //char PaddingByte[5];   // Padding Byte
} SENSE_DEVICE_MOUSE_ELEMENT;

class SenseDeviceMouse :
    public SenseDevice
{
public:
    SenseDeviceMouse() :
      m_DefaultCallback(NULL),
      m_pDefaultCallbackContext(NULL),
      m_DeviceID(-1)
    {
        //memset(m_SenseDeviceMouseState, 0, sizeof(SENSE_DEVICE_MOUSE_ELEMENT) * SENSE_DEVICE_MOUSE_SIZE);
        m_pSenseDeviceMouseState = new SENSE_DEVICE_MOUSE_ELEMENT[SENSE_DEVICE_MOUSE_SIZE];
        memset(m_pSenseDeviceMouseState, 0, sizeof(SENSE_DEVICE_MOUSE_ELEMENT) * SENSE_DEVICE_MOUSE_SIZE);

        for(int i = 0; i < SENSE_DEVICE_MOUSE_SIZE; i++)
        {
            m_pSenseDeviceMouseState[i].id = SENSE_DEVICE_ELEMENT_ID_MOUSE;
        }
    }

    ~SenseDeviceMouse(){/*empty stub*/}

    RESULT GetElement(int index, SENSE_DEVICE_ELEMENT_TYPE sdet, void* &n_pVal)
    {
        RESULT r = R_OK;
        CBRM((index > SENSE_DEVICE_MOUSE_ELEMENT_FIELDS), "Mouse Index out of Range");

        switch(sdet)
        {
            case SENSE_DEVICE_ELEMENT_STATUS:
            {
                switch((SENSE_DEVICE_MOUSE_FIELD)(index))
                {
                    case SDM_XAXIS:
                    {
                        n_pVal = new long;
                        n_pVal = (void*)m_pSenseDeviceMouseState->xaxis;
                    } break;

                    case SDM_YAXIS:
                    {
                        n_pVal = new long;
                        n_pVal = (void*)m_pSenseDeviceMouseState->yaxis;
                    } break;

                    case SDM_ZAXIS:
                    {
                        n_pVal = new long;
                        n_pVal = (void*)m_pSenseDeviceMouseState->wheel;
                    } break;

                    case SDM_BUTTON0:
                    {
                        n_pVal = new char;
                        n_pVal = (void*)m_pSenseDeviceMouseState->lbutton;
                    } break;

                    case SDM_BUTTON1:
                    {
                        n_pVal = new char;
                        n_pVal = (void*)m_pSenseDeviceMouseState->rbutton;
                    } break;

                    case SDM_BUTTON2:
                    {
                        n_pVal = new char;
                        n_pVal = (void*)m_pSenseDeviceMouseState->mbutton;
                    } break;

                    case SDM_BUTTON3:
                    {
                        n_pVal = new char;
                        n_pVal = (void*)m_pSenseDeviceMouseState->button3;
                    } break;

                    case SDM_BUTTON4:
                    {
                        n_pVal = new char;
                        n_pVal = (void*)m_pSenseDeviceMouseState->button4;
                    } break;

                    case SDM_BUTTON5:
                    {
                        n_pVal = new char;
                        n_pVal = (void*)m_pSenseDeviceMouseState->button5;
                    } break;

                    case SDM_BUTTON6:
                    {
                        n_pVal = new char;
                        n_pVal = (void*)m_pSenseDeviceMouseState->button6;
                    } break;

                    case SDM_BUTTON7:
                    {
                        n_pVal = new char;
                        n_pVal = (void*)m_pSenseDeviceMouseState->button7;
                    } break;
                }
            } break;

            case SENSE_DEVICE_ELEMENT_NAME:
            {
                //n_pVal = new char;
                //n_pVal = (void*)index;
                return R_NOT_IMPLEMENTED;
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

    RESULT SetElement(int index, SENSE_DEVICE_ELEMENT_TYPE sdet, void* pVal)
    {
        RESULT r = R_NOT_IMPLEMENTED;

        /*TODO!*/
Error:
        return r;
    }

    RESULT RegisterDeviceInputCallback(int index, SenseCallback Callback, void *pContext)
    {
        RESULT r = R_OK;

        CBRM((index > SENSE_DEVICE_MOUSE_SIZE), "Mouse element Index out of Range");

        // Allow to overwrite callbacks but output warning message
        if(m_pSenseDeviceMouseState[index].callback != NULL)
        {
            printf("warning:MIDI Device callback overwrite\n");
        }

        m_pSenseDeviceMouseState[index].callback = Callback;
        m_pDefaultCallbackContext = pContext;
Error:
        return r;
    }

    RESULT RegisterDefaultDeviceInputCallback(SenseCallback Callback, void *pContext)
    {
        RESULT r = R_OK;

        CNRM(Callback, "SenseDeviceMouse: Cannot set the callback to null");

        if(m_DefaultCallback != NULL)
            printf("warning: Overwriting Mouse device default callback\n");

        m_DefaultCallback = Callback;
        m_pDefaultCallbackContext = pContext;

Error:
        return r;
    }

    void* GetElements()
    {
        return (void*)m_pSenseDeviceMouseState;
    }

    int GetDeviceMax(){ return SENSE_DEVICE_MOUSE_MAX; }
    int GetDeviceMin(){ return SENSE_DEVICE_MOUSE_MIN; }
    int GetDeviceRange(){ return SENSE_DEVICE_MOUSE_RANGE; }
    SENSE_DEVICE_TYPE GetDeviceType(){ return SENSE_DEVICE_MOUSE; }

    int GetDeviceID(){ return m_DeviceID; }
    RESULT SetDeviceID(int id){ m_DeviceID = id; return R_OK; }

private:
    // Need only one element
    SENSE_DEVICE_MOUSE_ELEMENT *m_pSenseDeviceMouseState;//[SENSE_DEVICE_MOUSE_SIZE];
    SenseCallback m_DefaultCallback;        // This is to allow registration of one call back, preempted by specific callbacks
    void *m_pDefaultCallbackContext;
    int m_DeviceID;
};