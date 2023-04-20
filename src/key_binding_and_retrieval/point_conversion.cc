#include "point_conversion.h"

using namespace std;

#define SCALE_NUM 1 << 14


uint32_t float2fix(float kkkk) {
    float t = kkkk * 16384;
    uint32_t res = static_cast<uint32_t>(t);
    return res;
}

float fix2float(uint32_t x) {
    int32_t t = static_cast<int32_t>(x);
    float res = static_cast<float>(t);
    return res / 16384;
}

//以下函数弃用
uint64_t float_to_uint64_t(float Src, int Q) //Q表示Src二进制表示后取几位小数
{
    uint64_t temp;
    uint32_t* ptr = (uint32_t*)&Src; //*ptr为Src的二进制表示无小数点值，相当于Q0，但是Q0精度太低了

    int32_t sign_bit = ((*ptr & 0x80000000) >> 31);
    int32_t exponent = ((*ptr & 0x7f800000) >> 23) - 127 - 23 + Q;//自行选择对齐的小数点位置
    uint32_t mantissa = (*ptr & 0x007fffff) + 0x00800000;

    temp = (exponent > 0) ? (mantissa << exponent) : (mantissa >> -exponent);
    // temp = (sign_bit) ? -temp : temp; // 负数问题没想好怎么处理，先验证完正数再说

    return temp;
}

float uint64_t_to_float(uint64_t Src, int Q)
{
    uint64_t temp;
    uint32_t* ptr = (uint32_t*)&Src;
    int32_t sign_bit = ((*ptr & 0x80000000) >> 31);
    temp = (sign_bit) ? ~(Src - 1) : Src; // 负数则减一取反
    float res = temp * pow(2, -Q);
    res = (sign_bit) ? -res : res;
    return res;
}
