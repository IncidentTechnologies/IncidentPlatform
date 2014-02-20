#include "SoundVST.h"

SoundVST::SoundVST(SoundImpFactory* pSIF, char *pszVSTFilename) :
	m_pSoundImpFactory(pSIF),
	m_pSoundImp(NULL),
	m_BufferSize(SOUND_VST_DEFAULT_SAMPLES),
	m_pAEffect(NULL),
	m_hwndVSTEditor(NULL),
	m_hmoduleVSTPlugin(NULL),
	m_pszFilename(pszVSTFilename),
	//m_CurrentBuffer(0),
	m_fBufferReady(false),
	m_pFloatBuffer(NULL)
{        
	RESULT r = R_OK;

	memset(m_pszEffectName, 0, MAX_VST_STRING_LENGTH);
	memset(m_pszVendorName, 0, MAX_VST_STRING_LENGTH);
	memset(m_pszProductName, 0, MAX_VST_STRING_LENGTH);

	// Initialize the Dimension Implementation for this object
	CNRM(m_pSoundImpFactory, "SoundVST: Cannot create SoundVST without valid SoundImpFactory");

	m_pSoundImp = m_pSoundImpFactory->MakeSoundImp();
	CRM(Initialize(), "SoundVST: Failed to initialize VST");        

	m_pSoundImp->SetStreaming(true);
	m_pSoundImp->Initialize(this);
	m_pSoundImp->PlayBuffer(0, true);       // sound should be looping forever while we update everything           

	Validate();
	return;
Error:
	Invalidate();
	return;
}

// Alternative constructor for when the sound implementation does not belong to the SoundObject
// this is pertinent in the plugin scenario where the plugin will regulate and control the SoundImp
SoundVST::SoundVST(SoundImp* pSI, char *pszVSTFilename) :
	m_pSoundImpFactory(NULL),
	m_pSoundImp(pSI),
	m_BufferSize(SOUND_VST_DEFAULT_SAMPLES),
	m_pAEffect(NULL),
	m_hwndVSTEditor(NULL),
	m_hmoduleVSTPlugin(NULL),
	m_pszFilename(pszVSTFilename),
	//m_CurrentBuffer(0),
	m_fBufferReady(false),
	m_pFloatBuffer(NULL)
{        
	RESULT r = R_OK;

	memset(m_pszEffectName, 0, MAX_VST_STRING_LENGTH);
	memset(m_pszVendorName, 0, MAX_VST_STRING_LENGTH);
	memset(m_pszProductName, 0, MAX_VST_STRING_LENGTH);

	// Initialize the Dimension Implementation for this object
	CNRM(m_pSoundImp, "SoundVST: Invalid, valid m_pSoundImp required");

	CRM(Initialize(), "SoundVST: Invalid, failed to initialize VST");
	m_pSoundImp->SetStreaming(true);
	m_pSoundImp->Initialize(this);
	m_pSoundImp->PlayBuffer(0, true);       // sound should be looping forever while we update everything            

	Validate();
	return;
Error:
	Invalidate();
	return;
}

SoundVST::~SoundVST()
{
	if(m_pAEffect)
	{
		m_pAEffect->dispatcher (m_pAEffect, effClose, 0, 0, 0, 0);
		m_pAEffect = NULL;
	}

	if(m_hmoduleVSTPlugin)
	{
		FreeLibrary(m_hmoduleVSTPlugin);
		m_hmoduleVSTPlugin = NULL;
	}

	delete m_pSoundImp;
	m_pSoundImp = NULL;       
}

RESULT SoundVST::Initialize()
{
    RESULT r = R_OK;
    PluginEntryProc procMain = NULL;

    m_hmoduleVSTPlugin = LoadLibraryA(m_pszFilename);
    CNRM(m_hmoduleVSTPlugin, "SoundVST: Could not load %s", m_pszFilename);

    procMain = (PluginEntryProc)::GetProcAddress(m_hmoduleVSTPlugin, "VSTPluginMain");
    if(!procMain)
        procMain = (PluginEntryProc) ::GetProcAddress(m_hmoduleVSTPlugin, "main");

    CNRM(procMain, "SoundVST: Could not find the main process of %s", m_pszFilename);

    m_pAEffect = procMain(HostCallback);
    CNRM(m_pAEffect, "SoundVST: Could not create the AEffect object from %s", m_pszFilename);

    // Initialize the effect plug in using
    m_pAEffect->dispatcher (m_pAEffect, effOpen, 0, 0, 0, 0);   
    m_pAEffect->dispatcher (m_pAEffect, effSetSampleRate, 0, 0, 0, (float)GetSampleRate());
    m_pAEffect->dispatcher (m_pAEffect, effSetBlockSize, 0, (int)GetSampleCount(), 0, 0);  

    // Populate some information from plug-in
    m_pAEffect->dispatcher (m_pAEffect, effGetEffectName, 0, 0, m_pszEffectName, 0);
    m_pAEffect->dispatcher (m_pAEffect, effGetVendorString, 0, 0, m_pszVendorName, 0);
    m_pAEffect->dispatcher (m_pAEffect, effGetProductString, 0, 0, m_pszProductName, 0);
    
    printf ("Loaded %s from Vendor %s Product %s\n", m_pszEffectName, m_pszVendorName, m_pszProductName);
    printf ("numPrograms = %d\nnumParams = %d\nnumInputs = %d\nnumOutputs = %d\n\n", 
        m_pAEffect->numPrograms, m_pAEffect->numParams, m_pAEffect->numInputs, m_pAEffect->numOutputs);

    // Iterate through programs 
    // only printing them out to screen for now
    for (int i = 0; i < m_pAEffect->numPrograms; i++)
    {
        char progName[256] = {0};
        if (!m_pAEffect->dispatcher (m_pAEffect, effGetProgramNameIndexed, i, 0, progName, 0))
        {
            m_pAEffect->dispatcher (m_pAEffect, effSetProgram, 0, i, 0, 0); // Note: old program not restored here!
            m_pAEffect->dispatcher (m_pAEffect, effGetProgramName, 0, 0, progName, 0);
        }
        //printf ("Program %03d: %s\n", i, progName);
    }

    // Iterate through the params
    for (int i = 0; i < m_pAEffect->numParams; i++)
    {
        char paramName[256] = {0};
        char paramLabel[256] = {0};
        char paramDisplay[256] = {0};

        m_pAEffect->dispatcher (m_pAEffect, effGetParamName, i, 0, paramName, 0);
        m_pAEffect->dispatcher (m_pAEffect, effGetParamLabel, i, 0, paramLabel, 0);
        m_pAEffect->dispatcher (m_pAEffect, effGetParamDisplay, i, 0, paramDisplay, 0);
        float value = m_pAEffect->getParameter(m_pAEffect, i);

        //printf ("Param %03d: %s [%s %s] (normalized = %f)\n", i, paramName, paramDisplay, paramLabel, value);
    }

    // Can-do nonsense...
    static const char* canDos[] =
    {        
        "receiveVstEvents",
        "receiveVstMidiEvent",
        "midiProgramNames"
    };

    for (int i = 0; i < sizeof (canDos) / sizeof (canDos[0]); i++)
    {
        //printf ("Can do %s... ", canDos[i]);
        VstInt32 result = (VstInt32)m_pAEffect->dispatcher (m_pAEffect, effCanDo, 0, 0, (void*)canDos[i], 0);
        /*switch (result)
        {
        case 0  : printf ("don't know"); break;
        case 1  : printf ("yes"); break;
        case -1 : printf ("definitely not!"); break;
        default : printf ("?????");
        }
        printf ("\n");*/
    }

    //printf ("\n");

    // Set up the editor window and process
    if(m_pAEffect->flags & effFlagsHasEditor)
    {
        MyDLGTEMPLATE dlg;
        dlg.style = WS_POPUPWINDOW | WS_DLGFRAME | DS_MODALFRAME | DS_CENTER;
        dlg.cx = 100;
        dlg.cy = 100;
        //DialogBoxIndirectParam (GetModuleHandle(0), &dlg, 0, (DLGPROC)EditorProc, (LPARAM)g_myEffect);
        m_hwndVSTEditor = CreateDialogIndirectParam( GetModuleHandle(0),      // Handle to Creator Instance
                                                &dlg,                    // Dialog Template
                                                NULL,                    // Parent Window
                                                (DLGPROC)EditorProc,     // Dialog Process
                                                (LPARAM)this );         // Pass self as Parameter to be passed to process

        ShowWindow(m_hwndVSTEditor, SW_SHOW);   // show the VST dialog
    }
    else
    {
        printf("Effect does not have an editor!\n");
        goto Error;
    }

    // Finally turn on the effect
    m_pAEffect->dispatcher (m_pAEffect, effMainsChanged, 0, 1, 0, 0);


Error:
    return r;
}

RESULT SoundVST::GetSamples(long &SampleCount, void * &n_pVSTBuffer)
{
	RESULT r = R_OK;

	float **ppFloatVSTBuffer = NULL;

	// Retrieve buffer from VST
	if(m_pAEffect->numOutputs > 0)
	{               
		ppFloatVSTBuffer = new float*[m_pAEffect->numOutputs];
		for(int i = 0; i < m_pAEffect->numOutputs; i++)                
			ppFloatVSTBuffer[i] = new float[SampleCount];         

		m_pAEffect->processReplacing(m_pAEffect, NULL, ppFloatVSTBuffer, SampleCount);      

		n_pVSTBuffer = (void*)ppFloatVSTBuffer[0];
		
		if(m_pAEffect->numOutputs > 1)
		{
			for(int i = 1; i < m_pAEffect->numOutputs; i++)
			{
				delete [] ppFloatVSTBuffer[i];
				ppFloatVSTBuffer[i] = NULL;
			}
		}
	}

Error:
	return r;
}

RESULT SoundVST::Update()
{
    RESULT r = R_OK;

	int offset = 0;

    if(m_pSoundImp->StreamHungry(offset) == R_SOUND_STREAM_HUNGRY )
    {		
		void *pBuffer;
		long numSamples = GetSampleCount();
		CRM(GetSamples(numSamples, pBuffer), "SoundVST::Update: GetSamples failed");

		long BufferSize = (numSamples * GetBitsPerSample()) >> 3;
		m_pSoundImp->StreamBufferToImp(pBuffer, BufferSize, offset);
		
    }


Error:
    return r;
}
// VST Call Back
VstIntPtr VSTCALLBACK HostCallback (AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
    VstIntPtr result = 0;

    // Filter idle calls...
    bool filtered = false;
    if (opcode == audioMasterIdle)
    {
        static bool wasIdle = false;
        if (wasIdle)
            filtered = true;
        else
        {
            printf ("(Future idle calls will not be displayed!)\n");
            wasIdle = true;
        }
    }

    if (!filtered)
    {
        //printf ("PLUG> HostCallback (opcode %d)\n index = %d, value = %p, ptr = %p, opt = %f\n", opcode, index, FromVstPtr<void> (value), ptr, opt);
    }

    switch (opcode)
    {
    case audioMasterVersion :
        result = kVstVersion;
        break;
    }

    return result;
}

//VST Editor Windows Process Callback
INT_PTR CALLBACK EditorProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static SoundVST *pSoundVst = NULL;
    AEffect *pAEffect = NULL;
    
    if(pSoundVst)
        pAEffect = pSoundVst->GetAEffect();
    
    switch(msg)
    {
        //-----------------------
        case WM_INITDIALOG :
        {
            pSoundVst = (SoundVST *)lParam;
            if(pSoundVst)
                pAEffect = pSoundVst->GetAEffect(); 
            
            SetWindowText (hwnd, L"VST Editor");
            SetTimer (hwnd, 1, 20, 0);

            if (pAEffect)
            {
                // Open Editor
                pAEffect->dispatcher (pAEffect, effEditOpen, 0, 0, hwnd, 0);

                // Get Editor Rectangle
                ERect* eRect = 0;
                pAEffect->dispatcher (pAEffect, effEditGetRect, 0, 0, &eRect, 0);
                if (eRect)
                {
                    int width = eRect->right - eRect->left;
                    int height = eRect->bottom - eRect->top;

                    if (width < 100)
                        width = 100;
                    if (height < 100)
                        height = 100;

                    RECT wRect;
                    SetRect (&wRect, 0, 0, width, height);
                    AdjustWindowRectEx (&wRect, GetWindowLong (hwnd, GWL_STYLE), FALSE, GetWindowLong (hwnd, GWL_EXSTYLE));
                    width = wRect.right - wRect.left;
                    height = wRect.bottom - wRect.top;

                    SetWindowPos (hwnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE);
                }
            }
        }	break;

        //-----------------------
        case WM_TIMER :
        {
            if (pAEffect)
            {
                pAEffect->dispatcher (pAEffect, effEditIdle, 0, 0, 0, 0);
            }
        } break;

        //-----------------------
        case WM_CLOSE :
        {
            KillTimer (hwnd, 1);

            // Close Window
            if (pAEffect)
                pAEffect->dispatcher (pAEffect, effEditClose, 0, 0, 0, 0);

            EndDialog (hwnd, IDOK);
        }	break;
    }

    return 0;
}

// The test function
RESULT TestSoundVST(Console *pConsole, char *pszVSTFilename)
{
    RESULT r = R_OK;

    SoundImpFactory sif(SOUND_IMP_DIRECTSOUND);
    SoundVST *myVST = new SoundVST(&sif, pszVSTFilename);

    // Create a wanna be message loop for the test function
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {                       
        myVST->Update();
        myVST->HandleEditorMessage(msg);    
    }

Error:
    return r;
}