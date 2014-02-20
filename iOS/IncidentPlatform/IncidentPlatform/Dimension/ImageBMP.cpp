#include "ImageBMP.h"
#include "vector.h"
#include "point.h"

RESULT ImageBMP::LoadBmp(char *m_pszFilename)
{
    RESULT r = R_SUCCESS;
    int BytesRead = 0;
    FILE *file;

    file = fopen(m_pszFilename, "rb");

    CNRM(file, "Catastrophic wtf failure time!");
    printf("sizeof BMPH:%d sizeof BMPHI:%d short:%d\n", sizeof(BMP_HEADER), sizeof(BMP_INFO_HEADER_V3), sizeof(short));

    BytesRead = fread(&m_BMPHeader, BMP_HEADER_SIZE, 1, file);
    CR(CheckHeader());

    BytesRead = fread(&m_BMPInfoHeader, sizeof(BMP_INFO_HEADER_V3), 1, file);
    CR(CheckBMPInfoHeader());
    
    // Get the data!
    m_ppColorData = new colord*[m_BMPInfoHeader.width];
    for(int i = 0; i < m_BMPInfoHeader.width; i++)
    {
        m_ppColorData[i] = new colord[m_BMPInfoHeader.height];
        memset(m_ppColorData[i], 0, sizeof(colord) * m_BMPInfoHeader.width);
        
        int BytesSoFar = 0;

        for(int j = 0; j < m_BMPInfoHeader.height; j++)
        {
            if(m_BMPInfoHeader.BitsPerPixel == 24)
            {
                unsigned char r;
                unsigned char g;
                unsigned char b;

                BytesRead = fread(&r, sizeof(char), 1, file);
                BytesRead = fread(&g, sizeof(char), 1, file);
                BytesRead = fread(&b, sizeof(char), 1, file);

                m_ppColorData[i][j].set((char)0xFF, r, g, b);

                BytesSoFar += 3;
            }
            else if(m_BMPInfoHeader.BitsPerPixel == 8)
            {
                unsigned char g;
                BytesRead = fread(&g, sizeof(char), 1, file);
                m_ppColorData[i][j].set((char)(0xFF), g, g, g);
                BytesSoFar++;
            }
        }   

        if(BytesSoFar % 4 != 0)
        {
            unsigned char temp;
            for(int k = 0; k < BytesSoFar % 4; k++)
            {
                BytesRead = fread(&temp, sizeof(char), 1, file);
            }
        }
    }

Error:
    return r;
}

unsigned char ImageBMP::GetValue(float x, float y)
{
    int xi = m_BMPInfoHeader.width * x;
    int yi = m_BMPInfoHeader.height * y;

    unsigned char gray = (char)(m_ppColorData[xi][yi].GetRed() * 0.3f + 
                       m_ppColorData[xi][yi].GetGreen() * 0.59f +
                       m_ppColorData[xi][yi].GetBlue() * 0.11f);

    gray = 255 - gray;

    return gray;
}

RESULT ImageBMP::CheckHeader()
{
    RESULT r = R_SUCCESS;

    CBRM((m_BMPHeader.bmp_id == BMP_HEADER_ID), "BMP Header ID incorrect!");

Error:
    return r;
}

RESULT ImageBMP::CheckBMPInfoHeader()
{
    RESULT r = R_SUCCESS;

    CBRM((m_BMPInfoHeader.size == 40), "Size of BMP Info Header is not V3!");
    CBRM((m_BMPInfoHeader.CompressionMethod == 0), "Compression mode is not BI_RGB, not supported!");

Error:
    return r;
}

float ImageBMP::GetGray(int i, int j)
{
    char g_r = (char)(0.3f * (float)m_ppColorData[i][j].GetRed());
    char g_g = (char)(0.59f * (float)m_ppColorData[i][j].GetGreen());
    char g_b = (char)(0.11f * (float)m_ppColorData[i][j].GetBlue());

    float grey = (float)((g_r + g_g + g_b));
    grey = 255.0f - grey;

    return grey;
}

RESULT ImageBMP::MakeBumpMap(char *pszFilename, bool Overwrite)
{
    RESULT r = R_SUCCESS;
    int BytesWrote = 0;
    FILE *NewFile;
    BMP_HEADER BumpHeader;
    BMP_INFO_HEADER_V3 BumpInfoHeader;
    
    NewFile = fopen(pszFilename, "r");

    if(Overwrite == 0 && NewFile != NULL) {
        printf("%s already exists!\n", pszFilename);
        fclose(NewFile);
        CBRM(0, "");
    }
    else {
        // Create the file
        NewFile = fopen(pszFilename, "wb");
    }    

    // First create the proper header
    BumpHeader = { BMP_HEADER_ID,
                              m_BMPHeader.size,
                              m_BMPHeader.reserved1,
                              m_BMPHeader.reserved2,
                              m_BMPHeader.DataOffset
                            };

    BytesWrote = fwrite(&BumpHeader, 14, 1, NewFile);

    BumpInfoHeader = { m_BMPInfoHeader.size,
                                          m_BMPInfoHeader.width,
                                          m_BMPInfoHeader.height,
                                          m_BMPInfoHeader.planes,
                                          m_BMPInfoHeader.BitsPerPixel,
                                          m_BMPInfoHeader.CompressionMethod,
                                          m_BMPInfoHeader.ImageSize,
                                          m_BMPInfoHeader.HResolution,
                                          m_BMPInfoHeader.VResolution,
                                          m_BMPInfoHeader.Palette_n,
                                          m_BMPInfoHeader.ImportantColors
                                        };

    BytesWrote = fwrite(&BumpInfoHeader, sizeof(BMP_INFO_HEADER_V3), 1, NewFile);

    // Now we write the image data to the file
    // (just copy for test)
    for(int i = 0; i < BumpInfoHeader.width; i++) {
        int BytesSoFar = 0;
        for(int j = 0; j < BumpInfoHeader.height; j++) {
            vector A, B, C, D;

            if(i != BumpInfoHeader.width - 1)
                A = point(i + 1, GetGray(i + 1, j), j) - point(i, GetGray(i, j), j);

            if(j != BumpInfoHeader.height - 1)
                B = point(i, GetGray(i, j + 1), j + 1) - point(i, GetGray(i, j), j);
            
            if(i != 0)
                C = point(i - 1, GetGray(i - 1, j), j) - point(i, GetGray(i, j), j);

            if(j != 0)
                D = point(i, GetGray(i, j - 1), j - 1) - point(i, GetGray(i, j), j);

            vector N = (B % A) + (C % B) + (D % C) + (A % D);           
            N.normalize();

            if(N.tuple[0] < 0 || N.tuple[1] < 0 || N.tuple[2] < 0)
                printf("whoo!");

            char r = vX(N) * 0xFF;
            char g = vY(N) * 0xFF;
            char b = vZ(N) * 0xFF;

            BytesWrote = fwrite(&r, sizeof(char), 1, NewFile);
            BytesWrote = fwrite(&g, sizeof(char), 1, NewFile);
            BytesWrote = fwrite(&b, sizeof(char), 1, NewFile);

            BytesSoFar += 3;
        }

        if(BytesSoFar % 4 != 0) {
            unsigned char temp = 0;;
            for(int k = 0; k < BytesSoFar % 4; k++)
                BytesWrote = fwrite(&temp, sizeof(char), 1, NewFile);
        }
    }

Error:
    if(NewFile != NULL)
        fclose(NewFile);
    return r;
}