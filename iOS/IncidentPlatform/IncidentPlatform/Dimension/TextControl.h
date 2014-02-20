#pragma once

// Common Text Control - Flat Object
// This control is used for displaying text on the screen.  
// It exposes methods that allow for easy access of the text similar to
// edit controls.  

#include "FlatObject.h"
#include "IComposite.h"

#define TEXT_CONTROL_DEFAULT_TEXT "Text Control Default Text"

typedef enum TextControlAttributes
{
    TEXT_CTRL_FOREGROUND,
    TEXT_CTRL_BACKGROUND,
    TEXT_CTRL_BUFFER,
    TEXT_CTRL_TEXT,         // The actual TEXT object
    TEXT_CTRL_WIDTH,
    TEXT_CTRL_HEIGHT,
    TEXT_CTRL_POSITION,
    TEXT_CTRL_PADDING,      // The padding around the text in the control (all-direction, uni-directional padding not yet supported)
    TEXT_CTRL_FONT,         // Set/Get the current font of the control.  Text controls only support one font
    TEXT_CTRL_FIXED,        // The fixed attribute when set will fix the size/dimensions of the text control
    TEXT_CTRL_CLIP,         // This will clip the text in the control to the rectangle (only applicable when fixed is enabled)
    TEXT_CTRL_WRAP,         // Enables wrapping in the text control (only applicable when fixed is enabled)
    TEXT_CTRL_INVALID
} TEXT_CTRL_ATTRIBUTE;

class TextControl :
    public FlatObject,
    public IComposite
{
public:
    TextControl(point ptOrigin = point(0.0f, 0.0f), float height = 0.0f, float width = 0.0f, BLIT_TYPE type = BLIT_SCREEN, 
                float padding = 0.0f, bool fixed = false) :
        FlatObject(ptOrigin, FLAT_COMMON_CONTROL),
        m_Type(COMMON_CONTROL_TEXT),
        m_padding(padding),
        m_fixed(fixed)
    {
        m_pBlit = new blit(ptOrigin, height, width, type);
        m_pText = new text(ptOrigin, TEXT_CONTROL_DEFAULT_TEXT);
    }


    RESULT ConvertToScreen()
    {
        RESULT r = R_SUCCESS;
        
        CBRM(m_Screen_f, "TextControl: Trying to convert to screen without setting screen specs!");
        CRM(m_pBlit->ConvertToScreen(), "TextControl: Failed to convert blit to screen");
        CRM(m_pText->ConvertToScreen(), "TextControl: Failed to convert text to screen");

Error:
        return r;
    }

    RESULT ConvertToScreen(int ScreenWidth, int ScreenHeight)
    {
        RESULT r = R_SUCCESS;

        m_Screen_f = true;
        m_ScreenHeight = ScreenHeight;
        m_ScreenWidth = ScreenWidth;

        CRM(reinterpret_cast<FlatObject*>(m_pBlit)->ConvertToScreen(ScreenWidth, ScreenHeight), "TextControl: Failed to convert blit to screen");
        CRM(reinterpret_cast<FlatObject*>(m_pText)->ConvertToScreen(ScreenWidth, ScreenHeight), "TextControl: Failed to convert text to screen");

Error:
        return r;
    }

    const char *ToString(){ return "Text Common Control"; }

    // Composite Objects / Common Controls use SetParameter / GetParameter for this functionality
    vertex* GetVertexBuffer(){ return NULL; }
    int GetVertexCount(){ return 0; }

    // Update function for the text control manipulates the text / blit objects if any changes 
    // have been made to the control that need to be propagated (change of text results in change
    // of blit or change of blit can result in a need to change the text etc)
    RESULT Update()
    {
        RESULT r = R_SUCCESS;

        // Fit Blit to Text
        blit TextBlit = m_pText->GetBlit();

        m_pBlit->SetPosition(TextBlit.m_ptOrigin - point(m_padding, m_padding));
        m_pBlit->SetWidthHeight(TextBlit.GetWidth() + 2 * m_padding, TextBlit.GetHeight() + 2 * m_padding);

Error:
        return r;
    }

    RESULT Translate(FlatTranslator *pFlatTranslator)
    {
        RESULT r = R_SUCCESS;
        
        FlatObject *pTempObject = NULL;

        CRM(pFlatTranslator->TranslateObject(reinterpret_cast<FlatObject*&>(m_pBlit), pTempObject), "TextControl: Failed to translate blit");
        CPRM(pTempObject, "TextControl: Blit is NULL!");
        m_pBlit = dynamic_cast<blit*>(pTempObject);
        pTempObject = NULL;

        CRM(pFlatTranslator->TranslateObject(reinterpret_cast<FlatObject*&>(m_pText), pTempObject), "TextControl: Failed to translate text");
        CPRM(pTempObject, "TextControl: Text is NULL!");
        m_pText = dynamic_cast<text*>(pTempObject);
        pTempObject = NULL;

Error:
        return r;
    }

    blit GetBlit()
    {
        return blit(*m_pBlit);
    }

    // Don't implement this method since this is strictly a FLAT object composition
    RESULT Translate(DimensionTranslator *pDimTranslator)
    {       
        return R_NOT_IMPLEMENTED;
    }

    RESULT InitializeFlatObject(void *pContext)
    {
        RESULT r = R_SUCCESS;
        
        // We pass the initialization down to the children objects
        CRM(m_pText->InitializeFlatObject(pContext), "TextControl: Failed to initialze text");
        CRM(m_pBlit->InitializeFlatObject(pContext), "TextControl: Failed to initialize blit");        

Error:
        return r;
    }

    RESULT Render(DimensionImp *pDimImp)
    {
        RESULT r = R_SUCCESS;

        // Update 
        CRM(Update(), "TextControl: Update failed!");

        // first try the better one (the new one) eventually migrate it all over (material manager stuff)
        CRM(m_pBlit->Render(pDimImp), "TextControl: Blit failed to render");
        CRM(m_pText->Render(pDimImp), "TextControl: Text failed to render");        

Error:
        return r;
    }

    RESULT Initialize(DimensionImp *pDimImp)
    {
        RESULT r = R_SUCCESS;
        
        CRM(m_pBlit->Initialize(pDimImp), "TextControl: Blit failed to initialize");
        CRM(m_pText->Initialize(pDimImp), "TextControl: Text failed to initialize");

Error:
        return r;
    }


    RESULT SetAttribute(int tca, void *pContext)
    {
        RESULT r = R_SUCCESS;

        CBRM(tca >= TEXT_CTRL_INVALID, "TextControl: Trying to set invalid attribute!");
        CNRM(pContext, "TextControl: Context passed to SetAttribute is null");

        switch((TEXT_CTRL_ATTRIBUTE)tca)
        {
            case TEXT_CTRL_BACKGROUND:
            {
                m_pBlit->GetTexture(TEXTURE_DIFFUSE).SetColor(*((color*)(pContext)));
            } break;

            case TEXT_CTRL_FOREGROUND:
            {
                color *c = reinterpret_cast<color*>(pContext);
                CRM(m_pText->SetFontColor(*c), "TextControl: Failed to set font color");
            } break;

            case TEXT_CTRL_HEIGHT:
            {
                
            } break;

            case TEXT_CTRL_WIDTH:
            {

            } break;

            case TEXT_CTRL_FONT:
            {
                font *f = reinterpret_cast<font*>(pContext);
                CRM(m_pText->SetFont(f), "TextControl: failed to set the font");
            } break;

            case TEXT_CTRL_PADDING:
            {
                float padding = *(static_cast<float*>(pContext));
                m_padding = padding;
                Update();
            } break;    

            case TEXT_CTRL_POSITION:
            {
                point *p = reinterpret_cast<point*>(pContext);
                point a = p->ToScreenCoordinates(m_ScreenWidth, m_ScreenHeight);
                //m_ptOrigin = a;
                m_pText->m_ptOrigin = *p;
                m_pBlit->m_ptOrigin = a;
            } break;

            case TEXT_CTRL_BUFFER:
            {
                char* c_str = static_cast<char*>(pContext);
                CRM(m_pText->SetBuffer(c_str), "TextControl: Failed to set text!");
                // Might need to resize the text control but need to add the text-follow / blit-follow flags first
                Update();
            } break;

            case TEXT_CTRL_FIXED:
            {
                bool *pFixed  = static_cast<bool*>(pContext);
                m_fixed = *pFixed;
                Update();
            } break;

            default:
            {
                CBRM(0, "TextControl: Trying to set invalid attribute: %d!", tca);
            } break;
        }

Error:
        return r;
    }

    RESULT GetAttribute(int tca, void * &r_pContext)
    {
        RESULT r = R_SUCCESS;

        CBRM(tca >= TEXT_CTRL_INVALID, "TextControl: Trying to get invalid attribute!");

        switch((TEXT_CTRL_ATTRIBUTE)tca)
        {
        case TEXT_CTRL_BACKGROUND:
            {

            } break;

        case TEXT_CTRL_FOREGROUND:
            {

            } break;

        case TEXT_CTRL_HEIGHT:
            {

            } break;

        case TEXT_CTRL_FONT:
        {
            r_pContext = (void*)(m_pText->GetFont());
        } break;

        case TEXT_CTRL_WIDTH:
        {

        } break;

        // Returns the TEXT BUFFER not Text object
        case TEXT_CTRL_BUFFER:
        {
            r_pContext = (void*)(m_pText->GetBuffer());
        } break;

        case TEXT_CTRL_TEXT:
        {
            r_pContext = (void*)(m_pText);      // returns the actual text object
        } break;

        case TEXT_CTRL_FIXED:
        {
            r_pContext = (void*)(&m_fixed);
        } break;

        default:
            {
                CBRM(0, "TextControl: Trying to get invalid attribute: %d!", tca);
            } break;
        }

Error:
        return r;
    }

public:
    CommonControlType GetType(){ return m_Type; }

private:
    blit *m_pBlit;                  // The blit containing the TextControl
    text *m_pText;                  // Text to be written (text buffer)
    CommonControlType m_Type;       // Should always be COMMON_CONTROL_TEXT

    float m_padding;
    bool m_fixed;       // not set by default
};