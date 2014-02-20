#include "WindowFactory.h"
#include "WindowWin32.h"

WindowFactory::WindowFactory() {
    m_windowType = WINDOW_DEFAULT;
}

WindowFactory::WindowFactory(WINDOW_TYPE wt) {
    m_windowType = wt;
}

WindowFactory::~WindowFactory(){ };

WindowImp* WindowFactory::MakeWindow(WINDOW_TYPE windowType) const {
    switch(windowType) {
        
#ifdef __WIN32__
        case WINDOW_WIN32: {
            return new WindowWin32(false);
            //return new WindowWin32(true);
        } break;
#endif
            
        case WINDOW_X11:    /*breakthrough*/
            
        case WINDOW_MAC:    /*breakthrough*/
            
        default:    return NULL;
            break;
    }
}

WindowImp* WindowFactory::MakeWindow() const {
    return this->MakeWindow(m_windowType);
}
