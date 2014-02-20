////////////////////////
// FLAT VERTEX SHADER //
////////////////////////
// This vertex shader will take the input position
// and output it directly so that a point is converted
// directly to screen coordinates

// Vertex shader input structure
struct VS_INPUT
{
    float4 Position   : POSITION;
    float4 Normal     : NORMAL0;
    float4 Tangent	  : TANGENT0;	
    float4 Color      : COLOR0;
    float2 Texture    : TEXCOORD0;
};


// Vertex shader output structure
// Output no need for normals
struct VS_OUTPUT
{
    float4 Position   : POSITION;
    float4 Color	  : COLOR0;
    float2 Texture    : TEXCOORD0;
};


// Global variables
float4x4 g_WorldViewProj;       // don't really need this
float4x4 g_Transform;
float3 g_Origin;


// Name: Simple Vertex Shader
// Type: Vertex shader
// Desc: Vertex transformation and texture coord pass-through
//
VS_OUTPUT vs_main( in VS_INPUT In )
{
    VS_OUTPUT Out;                      //create an output vertex

    //Out.Position = In.Position + float4(g_Origin, 0.0);
    Out.Position = mul(In.Position, g_Transform);
    Out.Texture  = In.Texture;          //copy original texcoords       
    Out.Color = In.Color;    

    return Out;                         //return output vertex
}