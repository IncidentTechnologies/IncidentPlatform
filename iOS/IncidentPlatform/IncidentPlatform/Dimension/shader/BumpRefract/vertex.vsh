//////////////////////////////////////////////
// PHONG REFRACTION WITH BUMP VERTEX SHADER //
//////////////////////////////////////////////

// Vertex shader input structure
struct VS_INPUT
{
    float4 Position   	: POSITION;
    float4 Normal     	: NORMAL0;
    float4 Tangent		: TANGENT0;	
    float3 Color		: COLOR0;
    float2 Texture    	: TEXCOORD0;
};


// Vertex shader output structure
// Output no need for normals
struct VS_OUTPUT
{
    float4 Position   	: POSITION;
    float2 Texture    	: TEXCOORD0;
    float4 Refraction   : TEXCOORD1;
    float3 Normal     	: TEXCOORD2;	// vertex normal
    float3 Tangent		: TEXCOORD3;	// Tangent Normal
    float3 View			: TEXCOORD4;	// view vector
    float3 light0		: TEXCOORD5;	// light0 vector
    float3 Color		: COLOR0;
};


// Global variables
float4x4 g_WorldViewProj;
float4x4 g_Transform;

float3 g_ViewPosition;
float3 g_light0;

float4x4 g_RefractionProjectionMatrix;


// Name: Simple Vertex Shader
// Type: Vertex shader
// Desc: Vertex transformation and texture coord pass-through
//
VS_OUTPUT vs_main( in VS_INPUT In )
{
    VS_OUTPUT Out;                      //create an output vertex

	// Pixel Position
    float4 ClipPos = mul(In.Position, g_WorldViewProj);
    
    Out.Position =  mul(In.Position, g_WorldViewProj);	//apply vertex transformation (Vertex clip position)

    float4 TransPos = mul(In.Position, g_Transform);	
    
    // View Vector
    //Out.View = normalize(g_ViewPosition - TransPos);
    
    // Texture Coordinates
    Out.Texture  = In.Texture;          // Texture Coordinates for color texture

    //Out.Refraction = mul(In.Position, g_RefractionProjectionMatrix);
    Out.Refraction = mul(TransPos, g_RefractionProjectionMatrix);
    
    // Color
    Out.Color = In.Color;    
    
    // Interpolated Normals
    Out.Normal = normalize(mul(In.Normal, g_Transform));
    Out.Tangent = normalize(mul(In.Tangent, g_Transform));
    float3 Binormal = normalize(cross(Out.Tangent, Out.Normal));
    
    // Tangent Space
    float3x3 TangentSpace;
    TangentSpace[0] = normalize(mul(In.Tangent, (float3x3)g_Transform));
    //TangentSpace[0] = normalize(In.Tangent);    
    TangentSpace[2] = normalize(mul(In.Normal, (float3x3)g_Transform));
    TangentSpace[1] = normalize(cross(TangentSpace[0], TangentSpace[2]));
    
    // Light Vectors
    float3 ToLight = normalize(g_light0 - TransPos);		// light 0 unit vector
    Out.light0 = mul(TangentSpace, ToLight);
    
    // View Vectors
    float3 ToEye = normalize(g_ViewPosition - TransPos);
    Out.View = mul(TangentSpace, ToEye);

    return Out;                         //return output vertex
}