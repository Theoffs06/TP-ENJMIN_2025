struct Input {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D tex : register(t0); // t0 représente le slot 0 de
SamplerState samplerState : register(s0);

float4 main(Input input) : SV_TARGET {
    return tex.Sample(samplerState, input.uv);
}