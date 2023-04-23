#include <iostream>
#include <fstream>
#include <cstdlib>
#include <time.h>
#include "encode/encode.h"
#include "key_binding_and_retrieval/polynomials.h"

using namespace std;

void ExpandPoly(std::vector<uint8_t> P, std::vector<uint64_t> &res, uint64_t modulus) {
    int n = P.size();
    res[0] = (uint64_t)P[0];
    res[1] = 1;
    for (int i = 1; i < n; i++) {
        std::vector<uint64_t> m1(n + 1, 0);
        std::vector<uint64_t> m2(n + 1, 0);

        for(int j = 0; j <= n; j++) {
            if(res[j] != 0)
                m1[j + 1] += res[j];
        }
        for(int j = 0; j <= n; j++) {
            if(res[j] != 0)
                m2[j] += res[j] * (uint64_t)P[i];
        }
        for(int j = 0; j <= n; j++) {
            res[j] = (m1[j] + m2[j]) % modulus;
        }
    }
    // (x - a)(x - b) = x^2 -(a + b)x + ab
    for (int i = n - 1; i >= 0; i = i - 2) {
        res[i] = (modulus - res[i]) % modulus;
    }
}

int main(int argc, char** argv) {
    std::string file = "../src/encode/embedding_vector_2.txt";
    std::vector<uint8_t> P;
    GetEncodeRes(file, P);
    
    // the secret polynomial κ of degree smaller than k
    int t = 4;
    uint64_t modulus = 8519681;
    std::vector<uint64_t> coeff_k(t, 0);
    srand((unsigned)time(NULL));
    for (int i = 0; i < coeff_k.size(); i++) {
        coeff_k[i] = rand() % modulus;
    }
    coeff_k[0] &= 0x00000000000f;
    cout << hex;
    cout << "Party: Server; Key binding: 0x" << coeff_k[0] << endl;

    // V(X) of degree = P.size()
    std::vector<uint64_t> coeff_V(P.size() + 1, 0);
    ExpandPoly(P, coeff_V, modulus);
    for (int i = 0; i < coeff_k.size(); i++) {
        coeff_V[i] += coeff_k[i];
    }
    
    std::fstream f;
    f.open("../src/test/coeff_V.txt", ios::out);
     for (int i = 0; i < coeff_V.size(); i++) {
        f << coeff_V[i] << endl;
    }
    f.close();

}