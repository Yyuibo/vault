#ifndef SUPER_BIT_LSH_H_
#define SUPER_BIT_LSH_H_
#include "lsh.h"
#include "super_bit.h"

class LshSuperBit: public Lsh {
    // private default
    SuperBit sb_;
    int ComputeSuperBit(int stages, int buckets, int dimensions);

    public:
        LshSuperBit();
        LshSuperBit(int stages, int buckets, int dimensions);
        int* hash(double* vector);
        // int* hash_2_vectors(double* vector_1, double* vector_2);
};

#endif