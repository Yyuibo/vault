#include <iostream>
#include "super_bit_lsh.h"

using namespace std;

LshSuperBit::LshSuperBit() {

}

LshSuperBit::LshSuperBit(int stages, int buckets, int dimensions): Lsh(stages, buckets) {
    int code_length = stages * buckets / 2;
    int super_bit = ComputeSuperBit(stages, buckets, dimensions);
    this->sb_ = SuperBit(dimensions, super_bit, code_length / super_bit);
}

int* LshSuperBit::hash(double* vector) {
    bool* sig = sb_.Signature(vector);
    int* hash = HashSignature(sig, sb_.getCodeLength());
    return hash;
}

// int* LshSuperBit::hash_2_vectors(double* vector_1, double* vector_2) {
//     bool* sig_1 = sb_.Signature(vector_1);
//     bool* sig_2 = sb_.Signature(vector_2);
//     int* hash_1 = HashSignature(sig_1, sb_.getCodeLength());
//     int* hash_2 = HashSignature(sig_2, sb_.getCodeLength());
//     int* hash = new int[2];
//     hash[0], hash[1] = hash_1[0], hash_2[0];
//     return hash;
// }

int LshSuperBit::ComputeSuperBit(int stages, int buckets, int dimensions) {
    // super_bit code length
    int code_length = stages * buckets / 2;
    // super_bit value
    int super_bit;
    for (super_bit = dimensions; super_bit >= 1; super_bit--) {
        if (code_length % super_bit == 0) {
            break;
        }
    }
    if (super_bit == 0) {
        throw ("Superbit is 0 with parameters: s=" + std::to_string(stages) 
            + " b=" + std::to_string(buckets) + " n=" + std::to_string(dimensions));
    }
    return super_bit;
}
