#include <cstdint>
#include <iostream>
#include <cmath>

// 浮点数与定点数之间的转化
uint32_t float2fix(float kkkk);

float fix2float(uint32_t x);

uint64_t float_to_uint64_t(float Src, int Q);

float uint64_t_to_float(uint64_t Src, int Q);
