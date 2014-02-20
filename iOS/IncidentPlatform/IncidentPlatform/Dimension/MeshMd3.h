#pragma once

// MeshMd3 is the Mesh Implementation for the Md3 file format
// it is used by MeshLoader and is a dimension object which can
// be converted into a DimensionImp object to be rendered as well

#define MD3_IDENT 0x33504449
#define MD3_MAX_FRAMES 1024
#define MD3_MAX_TAGS 16
#define MD3_MAX_SURFACES 32
#define MD3_MAX_SHADERS 256
#define MD3_MAX_VERTICES 4096
#define MD3_MAX_TRIANGLES 8192

#include "DimObject.h"
#include "vertex.h"
#include "RESULT.h"
#include "EHM.h"

#define MD3_MAX_PATH 256

// help us out!
#define MESHMD3(m) static_cast<MeshMd3*>(m)

struct Md3Vector3
{
    float triplet[3];
};

struct Md3Header
{
    int ID;             // Should be equal to MD3_IDENT
    int Version;        // MD3 version umber
    char Name[64];      // ASCII Character string name
    int Flags;          // ? 
    int NumFrames;      // Number of frame objects
    int NumTags;        // Number of tag objects
    int NumSurfaces;    // Number of surface objects
    int NumSkins;       // Number of skin objects
    int FrameOffset;    // Relative offset of frame object begins
    int TagOffset;      // Relative offset of tag object begins
    int SurfaceOffset;  // Relative offset of surface object begins
    int EOFOffset;      // Offset between start and end of md3 file
};

struct Md3Frame
{
    Md3Vector3 MinBounds;       // First corner of bounding box
    Md3Vector3 MaxBounds;       // Second corner of the bounding box 
    Md3Vector3 LocalOrigin;     // Local Origin (0, 0, 0)
    float Radius;               // Radius of bounding sphere
    char Name[16];              // ASCII null terminated frame name
};

struct Md3Tag
{
    char Name[64];      // Name of Tag object
    Md3Vector3 Origin;  // Coordinates of tag object
    Md3Vector3 Axis[3]; // 3x3 rotation matrix associated with tag
};

struct Md3Surface
{
    int ID;                 // Should always be equal to MD3_IDENT
    char Name[64];         // ASCII null terminated name of surface object
    int Flags;              // ?
    int NumFrames;          // Number of animation frames (should match header)
    int NumShaders;         // Number of shaders defined in surface
    int NumVertices;        // Number of Vertex objects
    int NumTriangles;       // Number of triangle objects
    int TrianglesOffset;    // Offset for Triangles (OFFSETS ARE RELATIVE TO SURFACE START)
    int ShaderOffset;       // Offset for shaders
    int STOffset;           // Offset for S-T objects (texture coords)
    int XYZNormalOffset;    // Offset for XYZN vertices's
    int EndOffset;          // Offset of end of surface 
};

#define MD3_SHADER_NAME_LENGTH 64

struct Md3Shader
{
    char Name[MD3_SHADER_NAME_LENGTH];      // Shader name
    int ShaderIndex;    // Index of shader
};

struct Md3Triangle
{
    int Indices[3];     // Indices of the triangle verts
};

struct Md3TexCoord
{
    float ST[2];        // S and T texture coordinates, normalized to [0 1] range.  Map to top left corner as 0
};

struct Md3Vertex
{
    short Coord[3];     // x, y, z coordinates in left-handed 3 space scaled down by 1.0/64 (multiply by 1.0/64 to get original)
    char Normal[2];     // Zenith and azimuth angles of normal vector (255 represents 2*pi in spherical coords)
};

class MeshMd3 :
    public DimObject
{
public:
    MeshMd3() :
      DimObject(DIM_MESH_MD3),
      m_frames(NULL),
      m_tags(NULL),
      m_surfaces(NULL),
      m_shaders(NULL),
      m_triangles(NULL),
      m_texcoords(NULL),
      m_vertices(NULL),
      m_pszFilename(NULL),
      m_pszPath(NULL),
      m_ppszTextures(NULL),
      m_Animation_f(false),
      m_AnimationProgress(0.0f),
      m_AnimationSpeed(0.0f),
      m_AnimationStartFrame(0),
      m_AnimationFrameLength(0)  
    {
        memset(&m_header, 0, sizeof(Md3Header));
    }

    MeshMd3(char *pszFilename) :
        DimObject(DIM_MESH_MD3),
        m_frames(NULL),
        m_tags(NULL),
        m_surfaces(NULL),
        m_shaders(NULL),
        m_triangles(NULL),
        m_texcoords(NULL),
        m_vertices(NULL),
        m_pszFilename(pszFilename),
        m_pszPath(NULL),
        m_ppszTextures(NULL),
        m_Animation_f(false),
        m_AnimationProgress(0.0f),
        m_AnimationSpeed(0.0f),
        m_AnimationStartFrame(0),
        m_AnimationFrameLength(0)  
    {
        memset(&m_header, 0, sizeof(Md3Header));
        RESULT r = LoadMesh();
    }

    MeshMd3(char *pszPath, char *pszFilename) :
        DimObject(DIM_MESH_MD3),
        m_frames(NULL),
        m_tags(NULL),
        m_surfaces(NULL),
        m_shaders(NULL),
        m_triangles(NULL),
        m_texcoords(NULL),
        m_vertices(NULL),
        m_pszFilename(pszFilename),
        m_pszPath(pszPath),
        m_ppszTextures(NULL),
        m_Animation_f(false),
        m_AnimationProgress(0.0f),
        m_AnimationSpeed(0.0f),
        m_AnimationStartFrame(0),
        m_AnimationFrameLength(0)  
    {
        memset(&m_header, 0, sizeof(Md3Header));
        RESULT r = LoadMesh();
    }

    ~MeshMd3()
    {
        /* empty */
    }

    RESULT CheckMd3Header();
    RESULT PrintMd3HeaderInfo();

    RESULT CheckMd3Surface(int i);
    RESULT PrintMd3SurfaceInfo(int i);

    RESULT LoadMesh();

    char* GetFilename(){ return m_pszFilename; }
    char* GetPath(){ return m_pszPath; }

    RESULT StopAnimaton(){ m_Animation_f = false; return R_SUCCESS; }
    RESULT StartAnimation(){ m_Animation_f = true; return R_SUCCESS; }
    RESULT SetAnimation(float AnimationSpeed, int StartFrame, int FrameLength);
    RESULT SetFrame(int frame);
    RESULT LoadTextures();

    // Help me out here
    RESULT ParseOutFilename(char* pszPath, char* &n_pszTextureFile);

    const char *ToString() 
    {
        return "MD3 Mesh";
    }

    // This will return a pointer to the FIRST vertex
    // and in turn should be enough information to access that memory block
    vertex* GetVertexBuffer()
    {
        return &(m_DimensionVertices[0][0]);
    }
    int GetVertexCount()
    {
        int VertexCount = 0;
        for(int i = 0; i < m_header.NumSurfaces; i++)
        {
            for(int j = 0; j < m_surfaces[i].NumTriangles; j++)
            {
                VertexCount += 3;
            }
        }
        return VertexCount;
    }

private:
    char *m_pszFilename;
    char *m_pszPath;

public:
    Md3Header m_header;
    Md3Frame *m_frames;
    Md3Tag *m_tags;
    // Array of Md3 surfaces
    Md3Surface *m_surfaces;
    // Each Md3 surface has an array of shaders, triangles etc
    Md3Shader **m_shaders;
    Md3Triangle **m_triangles;
    Md3TexCoord **m_texcoords;
    Md3Vertex **m_vertices;

    // Texture Names
    // This is temporary until we add the texture interface 
    // to the engine
    char **m_ppszTextures;

public:
    // This is the actual dimension vertex information
    // There are a few vertex buffers (number of surfaces)
    // and eventually it will be so that frames can be
    // adjusted but at the moment only the first frame is
    // displayed
    vertex **m_DimensionVertices;

// Animation Stuff  (note that as of now this does not contain information about the different animation configs)
public:
    bool m_Animation_f;             // internal state regarding animation
    float m_AnimationProgress;      // internal state regarding animation progress
    float m_AnimationSpeed;
    int m_AnimationStartFrame;
    int m_AnimationFrameLength;    

};