// Vertex shader input structure
struct VS_INPUT
{
    float4 Position   : POSITION;
    float4 Normal     : NORMAL0;
    uint Color		  : COLOR0;
    float2 Texture    : TEXCOORD0;
};


// Vertex shader output structure
// Output no need for normals
struct VS_OUTPUT
{
    float4 Position   : POSITION;
    uint Color		  : COLOR0;
    float2 Texture    : TEXCOORD0;
};


// Global variables
float4x4 g_WorldViewProj;


// Name: Simple Vertex Shader
// Type: Vertex shader
// Desc: Vertex transformation and texture coord pass-through
//
VS_OUTPUT vs_main( in VS_INPUT In )
{
    VS_OUTPUT Out;                      //create an output vertex

    Out.Position = mul(In.Position, g_WorldViewProj);  //apply vertex transformation
    
    Out.Texture  = In.Texture;          //copy original texcoords   
    
    Out.Color = In.Color;    

    return Out;                         //return output vertex
}