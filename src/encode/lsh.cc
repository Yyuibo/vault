#include <algorithm>
#include <climits>
#include "lsh.h"

using namespace std;

Lsh::Lsh() {
    this->stages_ = DEFAULT_STAGES_;
    this->buckets_ = DEFAULT_BUCKETS_;
}

Lsh::Lsh(int stages, int buckets) {
    this->stages_ = stages;
    this->buckets_ = buckets;
}

int* Lsh::HashSignature(bool* signature, int length) {
    // Create an accumulator for each stage
    long acc[stages_];
    for (int i = 0; i < stages_; i++) {
        acc[i] = 0;
    }
    // Number of rows per stage
    int rows = length / stages_;
    for (int i = 0; i < length; i++) {
        long v = 0;
        if (signature[i]) {
            v = (i + 1) * LARGE_PRIME_;
        }
        // current stage
        int j = min(i / rows, stages_ - 1);
        acc[j] = (acc[j] + v) % INT_MAX;
    }

    int* hash = new int[stages_];
    for (int i = 0; i < stages_; i++) {
        hash[i] = (int) (acc[i] % buckets_);
    }
    return hash;
}