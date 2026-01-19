struct Input {
    float4 pos : SV_POSITION;
    float3 col : COLOR0;
};

float4 main(Input input) : SV_TARGET {
    return float4(cos(normalize(input.pos)) + input.col, 1);
}