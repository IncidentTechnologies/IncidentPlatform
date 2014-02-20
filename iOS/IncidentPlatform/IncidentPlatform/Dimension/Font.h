#pragma once

#include "dirty.h"

#define DEFAULT_FONT_NAME "Arial"
#define DEFAULT_FONT_HEIGHT 16 
#define DEFAULT_FONT_WIDTH 0
#define DEFAULT_FONT_WEIGHT 4   // Based on the windows GDI, zero is taken as a dont care
#define DEFAULT_FONT_ITALICS false

class font :
    public dirty
{
public:
    font(const char* pszName = DEFAULT_FONT_NAME, int height = DEFAULT_FONT_HEIGHT, int width = DEFAULT_FONT_WIDTH, int weight = DEFAULT_FONT_WEIGHT,
         bool italics = DEFAULT_FONT_ITALICS) :
      m_pszName(NULL),
          m_Height(height),
          m_Width(width),
          m_Weight(weight),
          m_Italics(italics)
      {
          // Local copy of the font name
          RESULT r = SetName(pszName);   
          m_Color = color(1.0f, 0.0f, 0.0f, 0.0f);
      }

      ~font()
      {
          /*empty stub*/
      }    

      char *GetName(){ return m_pszName; }
      RESULT SetName(const char *pszName)
      {
          m_pszName = new char[strlen(pszName)];
          strcpy(m_pszName, pszName);    
          SetDirty();
          return (m_pszName == NULL) ? R_OUT_OF_MEMORY : R_SUCCESS;


      }

      int GetHeight(){ return m_Height; }
      RESULT SetHeight(int height)
      { 
          m_Height = height; 
          SetDirty();
          return R_SUCCESS; 
      }

      int GetWidth(){ return m_Width; }
      RESULT SetWidth(int width)
      { 
          m_Width = width; 
          SetDirty();
          return R_SUCCESS;
      }

      color GetColor(){ return m_Color; }
      RESULT SetColor(color c)
      { 
          m_Color = c; 
          SetDirty();
          return R_SUCCESS; 
      }

      int GetWeight(){ return m_Weight; }
      RESULT SetWeight(int w)
      {
          m_Weight = w;
          SetDirty();
          return R_SUCCESS;
      }

public:
    char* m_pszName;
    int m_Height;
    int m_Width;

    int m_Weight;       // Font weight 
    bool m_Italics;     // Flag whether or not font is italicized 

    color m_Color;
};