#include <iostream>
#include <cstddef>
#include <fstream>
#include <algorithm>
#include <vector>
#include "encode/encode.h"
#include "key_binding_and_retrieval/polynomials.h"

using namespace std;

bool isDone(int t, int n, std::vector<bool> index) {
    for (int i = n - 1; i >= n - t; i--) {
        if (!index[i])  return false;
    }
    return true;
}

void Combination(int t, int n, std::vector<std::vector<uint8_t>> &pos) {
    std::vector<bool> index(n, false);
    for (int i = 0; i < t; i++) {
        index[i] = true;
    }
    while (!isDone(t, n, index)) {
        for (int i = 0; i < n - 1; i++) {
            if (index[i] && !index[i + 1]) {
                index[i] = false;
                index[i + 1] = true;

                int count = 0;
                for (int j = 0; j < i; j++) {
                    if (index[j]) {
                        index[j] = false;
                        index[count++] = true;
                    }
                }
                std::vector<uint8_t> pos1;
                for (int j = 0; j < n; j++) {
                    if (index[j])   pos1.push_back(j);
                }
                pos.push_back(pos1);
                break;
            }
        }
    }
}

// void sour(uint8_t t, uint8_t n, std::vector<uint8_t> pos_sub) {
//     if (t == 0 || n == 0) {
//         if (t == 0) pos.push_back(pos_sub);
//         return;
//     }
//     sour(t, n - 1, pos_sub);
//     pos_sub.push_back(n);
//     sour(t - 1, n - 1, pos_sub);
// }

int main(int argc, char** argv) {
    std::string file = "../src/encode/embedding_vector_1.txt";
    std::vector<uint8_t> P;
    GetEncodeRes(file, P);
    

    // get coeff_V.size()
    std::vector<uint64_t> coeff_V;
    std::fstream f;
    f.open("../src/test/coeff_V.txt", ios::in);
    string s;
    while (f >> s) {
        coeff_V.push_back((uint64_t)std::stoull(s));
    }
    f.close();

    
    // // retrieval (t n)
    int t = 15;
    uint64_t modulus = 8519681;
    // init index = {000000...11111}
    std::vector<int> index(P.size() - t, 0);
    for (int i = 0; i < t; i++) {
        index.push_back(1);
    }
    do {
        std::vector<uint32_t> x;
        std::vector<uint32_t> y;
        std::vector<uint8_t> pos;
        for (int i = 0; i < index.size(); i++) {
            if (index[i] == 1) {
                x.push_back(P[i]);
                y.push_back(calculate_poly(coeff_V, x.back()));
            }
        }
        std::vector<uint64_t> coeff_k;
        polynomial_from_points(x, y, coeff_k, (uint32_t)modulus);
        if ((coeff_k[0] & 0xfffffffffffffff0) == (uint64_t)0) {
            cout << hex;
            cout << "Party: Client; Key retrieval: 0x" << coeff_k[0] << endl;
            break;
        }
    } while (next_permutation(index.begin(), index.end()));

}