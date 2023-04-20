#include <random>
#include <algorithm>
#include "super_bit.h"

using namespace std;

SuperBit::SuperBit() {

}

// d: data space dimension.
// n: Super-Bit depth.
// l: number of Super-Bit.
SuperBit::SuperBit(int d, int n, int l) {
    if (d <= 0) {
        throw("Dimension d must be >= 1");
    }

    if (n < 1 || n > d) {
        throw("Super-Bit depth N must be 1 <= N <= d");
    }

    if (l < 1) {
        throw("Number of Super-Bit L must be >= 1");
    }

    // Input: Data space dimension d, Super-Bit depth 1 <= N <= d,
    // number of Super-Bit L >= 1,
    // resulting code length K = N * L

    // Generate a random matrix H with each element sampled independently
    // from the normal distribution N (0, 1), 
    // with each column normalized to unit length.
    // Denote H = [v1, v2, ..., vK].
    this->code_length_ = n * l;
    this->d_ = d;
    double v[code_length_][d_];

    // Normal distribution N(0, 1)
    random_device rd;
    // mt19937 rg(rd());
    mt19937 rg((uint_fast64_t) 1);
    normal_distribution<double> nd(0.0, 1.0);

    // Generate (code_length * d) random matrix H
    for (int i = 0; i < code_length_; i++) {
        double vector[d_];
        for (int j = 0; j < d_; j++) {
            vector[j] = nd(rg);
        }

        Normalize(vector, d_);
        copy(&vector[0], &vector[0] + d_, &v[i][0]);
    }
    
    // for i = 0 to L - 1 do
    //    for j = 1 to N do
    //       w_{iN+j} = v_{iN+j}
    //       for k = 1 to j - 1 do
    //          w_{iN+j} = w_{iN+j} - w_{iN+k} w^T_{iN+k} v_{iN+j}
    //       end for
    //       wiN+j = wiN+j / | wiN+j |
    //     end for
    //   end for
    // Output: H˜ = [w1, w2, ..., wK]
    double w[code_length_][d_];
    for (int i = 0; i <= l - 1; i++) {
        for (int j = 1; j <= n; j++) {
            copy(&v[i * n + j - 1][0], &v[i * n + j - 1][0] + d_, &w[i * n + j - 1][0]);

            for (int k = 1; k <= (j - 1); k++) {
                double dotProduct = DotProduct(w[i * n + k - 1], v[i * n + j - 1], d);
                double product[d_], sub[d_];
                Product(dotProduct, w[i * n + k - 1], d_, product);
                Sub(w[i * n + j - 1], product, d_, sub);
                copy(&sub[0], &sub[0] + d_, &w[i * n + j - 1][0]);
            }

            Normalize(w[i * n + j - 1], d_);
        }
    }

    // Allocate space for hyperplanes_
    hyperplanes_ = new double*[code_length_];
    for(int i = 0; i < code_length_; i++) {
        hyperplanes_[i] = new double[d_];
    }
    copy(&w[0][0], &w[0][0] + code_length_ * d_, &hyperplanes_[0][0]);
}


void SuperBit::getHyperplanes(double** res) {
    copy(&hyperplanes_[0][0], &hyperplanes_[0][0] + code_length_ * d_, &res[0][0]);
}

int SuperBit::getCodeLength() {
    return code_length_;
}

bool* SuperBit::Signature(double* vector) {
    bool* sig = new bool[code_length_];
    for (int i = 0; i < code_length_; i++) {
        sig[i] = (DotProduct(hyperplanes_[i], vector, d_) >= 0);
    }
    return sig;
}

void SuperBit::Sub(double* a, double* b, int a_length, double* res) {
    for (int i = 0; i < a_length; i++) {
        res[i] = a[i] - b[i];
    }
}

double SuperBit::Norm(double* v, int v_length) {
    double agg = 0;
    for (int i = 0; i < v_length; i++) {
        agg += (v[i] * v[i]);
    }
    return sqrt(agg);
}

void SuperBit::Normalize(double* v, int v_length) {
    double norm = Norm(v, v_length);
    for (int i = 0; i < v_length; i++) {
        v[i] = v[i] / norm;
    }
}

void SuperBit::Product(double x, double* v, int v_length, double* res) {
    for (int i = 0; i < v_length; i++) {
        res[i] = x * v[i];
    }
}

double SuperBit::DotProduct(double* v1, double* v2, int v1_length) {
    double agg = 0;
    for (int i = 0; i < v1_length; i++) {
        agg += (v1[i] * v2[i]);
    }
    return agg;
}
