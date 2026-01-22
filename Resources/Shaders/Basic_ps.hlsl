Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

struct Input {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(Input input) : SV_TARGET {    
    return tex.Sample(samplerState, input.uv);
}