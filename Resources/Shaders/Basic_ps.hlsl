struct Input {
    float4 pos : SV_POSITION;
};

float4 main(Input input) : SV_TARGET {
    return float4(1, 0, 1, 1);
}