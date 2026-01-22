#pragma once

#pragma once

#include <random>

class Random {
public:
    static void SetSeed(uint32_t seed);
    static uint32_t GetSeed();

    static int   RangeInt(int min, int max); //        [min, max]
    static float RangeFloat(float min, float max); //  [min, max]
    static bool  Chance(float probability); //         0.0f - 1.0f

private:
    static uint32_t     m_seed;
    static std::mt19937 m_engine;
};
