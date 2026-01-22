#include "pch.h"
#include "Random.h"

uint32_t Random::m_seed = std::random_device{}();
std::mt19937 Random::m_engine(m_seed);

void Random::SetSeed(uint32_t seed) {
    m_seed = seed;
    m_engine.seed(seed);
}

uint32_t Random::GetSeed(){
    return m_seed;
}

int Random::RangeInt(int min, int max){
    std::uniform_int_distribution dist(min, max);
    return dist(m_engine);
}

float Random::RangeFloat(float min, float max) {
    std::uniform_real_distribution dist(min, max);
    return dist(m_engine);
}

bool Random::Chance(float probability) {
    std::bernoulli_distribution dist(probability);
    return dist(m_engine);
}
