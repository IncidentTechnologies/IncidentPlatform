#pragma once

// The ImageBMP will simply open a .bmp file format from 
// disk and allow access to it's data

#include "cmnPriv.h"
#include "colord.h"

#define BMP_HEADER_ID 0x4D42

#define BMP_HEADER_SIZE 14
typedef struct BMPHeader
{
    short bmp_id;
    int size;
    short reserved1;
    short reserved2;
    int DataOffset;
} BMP_HEADER;

typedef struct BMPInfoHeaderV3
{
    int size;
    int width;
    int height;
    short planes;
    short BitsPerPixel;
    int CompressionMethod;
    int ImageSize;
    int HResolution;        // pixels per meter
    int VResolution;        // pixels per meter
    int Palette_n;          // number of colors in the palette
    int ImportantColors;    // 0 (usually ignored)
} BMP_INFO_HEADER_V3;

class ImageBMP
{
public:
    ImageBMP(char *pszFilename) :
      m_pszFilename(pszFilename),
      m_ppColorData(NULL)
    {
        // First check that this file exists
        RESULT r = CheckFileExistance(m_pszFilename);
        if(r < 0)
        {
            printf("%s does not exist!\n", m_pszFilename);
        }
        else
        {
            printf("found %s!\n", m_pszFilename);
            r = LoadBmp(m_pszFilename);
        }
    }

    ~ImageBMP()
    {
        /*empty stub*/
    }


    RESULT LoadBmp(char *m_pszFilename);
    RESULT CheckHeader();
    RESULT CheckBMPInfoHeader();

    int GetWidth(){ return m_BMPInfoHeader.width; }
    int GetHeight(){ return m_BMPInfoHeader.height; }

    // Returns the grayscale at a point between 0, 1 for x and y
    unsigned char GetValue(float x, float y);

    RESULT MakeBumpMap(char *pszFilename, bool Overwrite);
    float GetGray(int i, int j);

private:
    char *m_pszFilename;

    BMP_HEADER m_BMPHeader;
    BMP_INFO_HEADER_V3 m_BMPInfoHeader;

    colord **m_ppColorData;

};