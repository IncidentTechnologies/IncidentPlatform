#include "MeshMd3.h"

#include <math.h>

#define PI 4.0*atan(1.0)

RESULT MeshMd3::LoadMesh() {
    RESULT r = R_SUCCESS;
    int BytesRead = 0;
    FILE *file;
    char TempPath[MD3_MAX_PATH];
    int CurSurfaceOffset;

    if(m_pszPath == NULL) {
        file = fopen(m_pszFilename, "rb");    
    
        if(file == NULL) {
            printf("Could not open %s\n", m_pszFilename);
            CBRM(0, "");
        }
    }
    else {
        
        memset(TempPath, 0, sizeof(char) * MD3_MAX_PATH);
        strcpy(TempPath, m_pszPath);
        strcat(TempPath, m_pszFilename);

        printf("%s\n", TempPath);

        file = fopen(TempPath, "rb");        

        if(file == NULL) {
            printf("Could not open %s\n", TempPath);
            CBRM(0, "");
        }   
    }

    BytesRead = fread(&m_header, sizeof(Md3Header), 1, file);
    CR(CheckMd3Header());

    PrintMd3HeaderInfo();

    // Set up the frames
    m_frames = new Md3Frame[m_header.NumFrames];
    memset(m_frames, 0, sizeof(Md3Frame) * m_header.NumFrames);

    // While we should be here anyways we should use the offset
    fseek(file, m_header.FrameOffset, SEEK_SET);

    for(int i = 0; i < m_header.NumFrames; i++)
    {
        BytesRead = fread(&(m_frames[i]), sizeof(Md3Frame), 1, file);
        //printf("Frame:%d name:%s\n", i, m_frames[i].Name);
    }

    // Set up the Tags
    m_tags = new Md3Tag[m_header.NumTags];
    memset(m_tags, 0, sizeof(Md3Tag) * m_header.NumTags);

    // While we should be here anyways we use the offset
    fseek(file, m_header.TagOffset, SEEK_SET);

    for(int i = 0; i < m_header.NumTags; i++)
    {
        BytesRead = fread(&(m_tags[i]), sizeof(Md3Tag), 1, file);
        //printf("Tag:%d name:%s\n", i, m_tags[i].Name);
    }

    // Set up the surfaces and memory stuffs
    m_surfaces = new Md3Surface[m_header.NumSurfaces];
    memset(m_surfaces, 0, sizeof(Md3Surface) * m_header.NumSurfaces);

    m_shaders = new Md3Shader*[m_header.NumSurfaces];
    memset(m_shaders, 0, sizeof(Md3Shader*) * m_header.NumSurfaces);

    m_triangles = new Md3Triangle*[m_header.NumSurfaces];
    memset(m_triangles, 0, sizeof(Md3Triangle*) * m_header.NumSurfaces);

    m_texcoords = new Md3TexCoord*[m_header.NumSurfaces];
    memset(m_texcoords, 0, sizeof(Md3TexCoord*) * m_header.NumSurfaces);

    m_vertices = new Md3Vertex*[m_header.NumSurfaces];
    memset(m_vertices, 0, sizeof(Md3Vertex*) * m_header.NumSurfaces);

    // TEXTURES (TEMP)
    m_ppszTextures = new char*[m_header.NumSurfaces];

    // Use this counter to keep track of the different surface objects
    CurSurfaceOffset = m_header.SurfaceOffset;

    // While we should be here anyways we use the offset
    fseek(file, m_header.SurfaceOffset, SEEK_SET);

    for(int i = 0; i < m_header.NumSurfaces; i++) {
        BytesRead = fread(&(m_surfaces[i]), sizeof(Md3Surface), 1, file);

        // Print MD3 Surface Info
        PrintMd3SurfaceInfo(i);

        // Check surface ok
        CRM(CheckMd3Surface(i), "Surface not ok");

        // While we should already be here we should fast forward
        fseek(file, CurSurfaceOffset + m_surfaces[i].ShaderOffset, SEEK_SET);

        // Set up the shaders
        m_shaders[i] = new Md3Shader[m_surfaces[i].NumShaders];
        memset(m_shaders[i], 0, sizeof(Md3Shader) * m_surfaces[i].NumShaders);

        for(int j = 0; j < m_surfaces[i].NumShaders; j++) {
            BytesRead = fread(&(m_shaders[i][j]), sizeof(Md3Shader), 1, file);            
            printf("Surface:%d Shader:%d Shader Index:%d name:%s\n", i, j, m_shaders[i][j].ShaderIndex, m_shaders[i][j].Name);
        }

        // Fast forward to triangles
        fseek(file, CurSurfaceOffset + m_surfaces[i].TrianglesOffset, SEEK_SET);

        // Set up the triangles
        m_triangles[i] = new Md3Triangle[m_surfaces[i].NumTriangles];
        memset(m_triangles[i], 0, sizeof(Md3Triangle) * m_surfaces[i].NumTriangles);

        for(int j = 0; j < m_surfaces[i].NumTriangles; j++) {
            BytesRead = fread(&(m_triangles[i][j]), sizeof(Md3Triangle), 1, file);
            //printf("Surface:%d Triangle:%d\n", i, j);
        }

        // Fast forward to Texture Coordinates
        fseek(file, CurSurfaceOffset + m_surfaces[i].STOffset, SEEK_SET);

        // Set up the TexCoords
        // There are the same number of TexCoords as there are vertices
        m_texcoords[i] = new Md3TexCoord[m_surfaces[i].NumVertices];
        memset(m_texcoords[i], 0, sizeof(Md3TexCoord) * m_surfaces[i].NumVertices);

        for(int j = 0; j < m_surfaces[i].NumVertices; j++) {
            BytesRead = fread((&m_texcoords[i][j]), sizeof(Md3TexCoord), 1, file);
            //printf("Surface:%d TexCoord:%d\n", i, j);
        }

        // Fast forward to the Vertices
        fseek(file, CurSurfaceOffset + m_surfaces[i].XYZNormalOffset, SEEK_SET);

        // Set up the Vertices
        // There are as many vertices as Frames * Vertices
        // each set of vertices represents a different frame of animation
        m_vertices[i] = new Md3Vertex[m_header.NumFrames * m_surfaces[i].NumVertices];
        memset(m_vertices[i], 0, sizeof(Md3Vertex) * m_header.NumFrames * m_surfaces[i].NumVertices);

        for(int j = 0; j < m_header.NumFrames * m_surfaces[i].NumVertices; j++) {
            BytesRead = fread((&m_vertices[i][j]), sizeof(Md3Vertex), 1, file);
            // printf("Surface:%d Vertex:%d x:%d y:%d z:%d\n", i, j, m_vertices[i][j].Coord[0], m_vertices[i][j].Coord[1], m_vertices[i][j].Coord[2]);
        }            

        // This should be the end of the surface object so we move on to the next one

        // Seek to the end of the surface object
        fseek(file, CurSurfaceOffset + m_surfaces[i].EndOffset, SEEK_SET);
        CurSurfaceOffset = CurSurfaceOffset + m_surfaces[i].EndOffset;            
    }        

    // **********************
    // DONE LOADING FROM FILE
    // **********************

    // Once we are done gathering all of the data out of the file
    // we push it into our own vertex format
    m_DimensionVertices = new vertex*[m_header.NumSurfaces];
    memset(m_DimensionVertices, 0, sizeof(vertex*) * m_header.NumSurfaces);

    for(int i = 0; i < m_header.NumSurfaces; i++)
    {
        // For each surface we fill the dimension vertex buffer
        // NOTE: at the moment we are not doing multiple frames
        m_DimensionVertices[i] = new vertex[m_surfaces[i].NumVertices * m_header.NumFrames];
        memset(m_DimensionVertices[i], 0, sizeof(vertex) * m_surfaces[i].NumVertices * m_header.NumFrames);
        
        for(int j = 0; j < m_surfaces[i].NumVertices * m_header.NumFrames; j++)
        {
            m_DimensionVertices[i][j].set(m_vertices[i][j].Coord[0] * (float)(1.0/64),
                                          m_vertices[i][j].Coord[1] * (float)(1.0/64),
                                          m_vertices[i][j].Coord[2] * (float)(1.0/64));
            // Decode the normals
            float lat = (float)(m_vertices[i][j].Normal[0] * (2.0f * PI)) / 255.0f;
            float lng = (float)(m_vertices[i][j].Normal[1] * (2.0f * PI)) / 255.0f;
            float tempX = cos(lat) * sin(lng);
            float tempY = sin(lat) * sin(lng);
            float tempZ = cos(lng);

            m_DimensionVertices[i][j].SetNormal(tempX, tempY, tempZ);
            m_DimensionVertices[i][j].Normalize();

            // Vertex Color
            m_DimensionVertices[i][j].SetColor(1.0f, 1.0f, 1.0f, 1.0f);

            // Pass in the UV coordinates
            m_DimensionVertices[i][j].SetUV(m_texcoords[i][j % m_surfaces[i].NumVertices].ST[0], m_texcoords[i][j % m_surfaces[i].NumVertices].ST[1]);
        }
    }

    CRM(LoadTextures(), "Could not load textures!");

Error:
    return r;
}

#define MD3_FILE_DELIMS "'()/\\"

RESULT MeshMd3::ParseOutFilename(char* pszPath, char* &n_pszTextureFile)
{
    RESULT r = R_SUCCESS;
    
    char *temp = strtok(pszPath, MD3_FILE_DELIMS);
    char *last;
    while(temp != NULL)
    {
        last = temp;
        temp = strtok(NULL, MD3_FILE_DELIMS);
    }

    // Last one should be the filename
    // check to see that this file exists
    n_pszTextureFile = new char[MD3_MAX_PATH];
    if(m_pszPath != NULL)
    {
        strcpy(n_pszTextureFile, m_pszPath);
        strcat(n_pszTextureFile, last);
    }
    else
    {
        strcpy(n_pszTextureFile, last);
    }

    FILE *TextureFile;
    TextureFile = fopen(n_pszTextureFile, "r");

    if(TextureFile == NULL)
    {
        delete n_pszTextureFile;
        return R_FAIL;
    }
    else
    {
        fclose(TextureFile);
        return R_SUCCESS;
    }

Error:
    return r;
}

RESULT MeshMd3::LoadTextures()
{
    RESULT r = R_SUCCESS;
    
    for(int i = 0; i < m_header.NumSurfaces; i++)
    {
        // Find the shaders (textures)
        for(int j = 0; j < m_surfaces[i].NumShaders; j++)
        {
            char *temp = NULL;
            
            // There seems to be a bug with some of the
            // md3 file formats where the first character of the
            // name is /0 .  Since this is fixed memory it is safe
            // to parse through the block to look for any non zero character
            char *TempName = m_shaders[i][j].Name;
            for(int k = 0; k < MD3_SHADER_NAME_LENGTH; k++)
            {
                if(*TempName == '\0')
                    TempName++;
                else
                    break;
            }
            
            if(*TempName != '\0')
            {
                //ParseOutFilename(m_shaders[i][j].Name, temp);
                ParseOutFilename(TempName, temp);
            }
            
            if(temp == NULL)
            {
                printf("%s does not exist!\n", m_shaders[i][j].Name);
            }
            else
            {
                printf("found %s\n", temp);
                CRM(AddTexture(temp, TEXTURE_DIFFUSE), "Could not add texture FAILURE!");
            }
        }
    }

Error:
    return r;
}

RESULT MeshMd3::PrintMd3HeaderInfo()
{
    RESULT r = R_SUCCESS;

    printf("Md3 File\n**********\n");
    printf("version:%d\n", m_header.Version);
    printf("name:%s\n", m_header.Name);
    printf("NumFrames:%d\n", m_header.NumFrames);
    printf("NumTags:%d\n", m_header.NumTags);
    printf("NumSurfaces:%d\n", m_header.NumSurfaces);
    printf("NumSkins:%d\n", m_header.NumSkins);

Error:
    return r;
}

RESULT MeshMd3::PrintMd3SurfaceInfo(int i)
{
    RESULT r = R_SUCCESS;

    printf("Md3 Surface:%d\n***********\n", i);
    printf("Name:%s\n", m_surfaces[i].Name);
    printf("NumFrames:%d\n", m_surfaces[i].NumFrames);
    printf("NumShaders:%d\n", m_surfaces[i].NumShaders);
    printf("NumTriangles:%d\n", m_surfaces[i].NumTriangles);
    printf("NumVerts:%d\n", m_surfaces[i].NumVertices);

Error:
    return r;
}

RESULT MeshMd3::CheckMd3Surface(int i)
{
    RESULT r = R_SUCCESS;

    CBRM(m_surfaces[i].ID == MD3_IDENT, "Md3Surface ID incorrect");
    CBRM(m_surfaces[i].NumFrames > MD3_MAX_FRAMES, "Md3Surface too many frames!");
    CBRM(m_surfaces[i].NumFrames == m_header.NumFrames, "Md3Surface mismatch header frames");
    CBRM(m_surfaces[i].NumShaders > MD3_MAX_SHADERS, "Md3Surface too many shaders!");
    CBRM(m_surfaces[i].NumTriangles > MD3_MAX_TRIANGLES, "Md3Surface too many triangles!");
    CBRM(m_surfaces[i].NumVertices > MD3_MAX_VERTICES, "Md3Surface too many vertices!");

Error:
    return r;
}

RESULT MeshMd3::CheckMd3Header()
{
    RESULT r = R_SUCCESS;

    CBRM(m_header.ID == MD3_IDENT, "Md3Header ID incorrect");
    CBRM(m_header.NumFrames > MD3_MAX_FRAMES, "Md3Header too many frames!");
    CBRM(m_header.NumSurfaces > MD3_MAX_SURFACES, "Md3Header too many surfaces!");
    CBRM(m_header.NumTags > MD3_MAX_TAGS, "Md3Header too many tags!");

Error:
    return r;
}

RESULT MeshMd3::SetAnimation(float AnimationSpeed, int StartFrame, int FrameLength)
{
    m_AnimationSpeed = AnimationSpeed;
    m_AnimationStartFrame = StartFrame;
    m_AnimationFrameLength = FrameLength;
    m_Animation_f = true;   

    return R_SUCCESS;
}

RESULT MeshMd3::SetFrame(int frame)
{
    m_AnimationStartFrame = frame;

    return R_SUCCESS;
}