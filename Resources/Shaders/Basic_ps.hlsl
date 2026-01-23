Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

struct Input {
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

float4 main(Input input) : SV_TARGET {
    float4 res = tex.Sample(samplerState, input.uv);
    
	float3 lightIntensity = float3(0.2, 0.15, 0.4); // ambient
    lightIntensity += saturate(dot(input.normal, float3(-1, -1, -1))) * float3(0.98, 0.87, 0.34); // diffuse
    res.rgb *= saturate(lightIntensity);

    return res;
}