struct VS_INPUT
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

cbuffer Matrices
{
    float4x4 WorldViewProj;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    output.pos = mul(WorldViewProj, float4(input.pos, 1));
    output.tex = input.tex;
    return output;
}

SamplerState Sampler;

Texture2D Texture;

float4 PS(PS_INPUT input) : SV_TARGET
{
    return Texture.Sample(Sampler, input.tex);
}