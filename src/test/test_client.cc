#include <iostream>
#include <cstddef>
#include <fstream>
#include <map>
#include "encode/encode.h"
#include "key_binding_and_retrieval/polynomials.h"

using namespace std;

std::vector<std::vector<uint8_t>> pos;
void sour(uint8_t t, uint8_t n, std::vector<uint8_t> pos_sub) {
    if (t == 0 || n == 0) {
        if (t == 0) pos.push_back(pos_sub);
        return;
    }
    sour(t, n - 1, pos_sub);
    pos_sub.push_back(n);
    sour(t - 1, n - 1, pos_sub);
}

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
    int t = 4;
    uint64_t modulus = 8519681;
    std::vector<uint8_t> pos_sub;
    sour(t, P.size(), pos_sub);
    std::vector<uint32_t> x;
    std::vector<uint32_t> y;
    for (int i = 0; i < pos.size(); i++) {
        // Cmn
        x.clear();
        y.clear();
        for (int j = 0; j < t; j++) {
            x.push_back(P[pos[i][j]]);
            y.push_back(calculate_poly(coeff_V, x[j]));
            // cout << (int)pos[i][j] << endl; 
        }
        std::vector<uint64_t> coeff_k;
        polynomial_from_points(x, y, coeff_k, (uint32_t)modulus);
        if ((coeff_k[0] & 0xfffffffffffffff0) == (uint64_t)0) {
            cout << hex;
            cout << "Party: Client; Key retrieval: 0x" << coeff_k[0] << endl;
            break;
        }
    }
    // for (int i = 0; i < P.size() - 1; i++) {
    //     x.clear();
    //     y.clear();
    //     x.push_back(P[i]);
    //     y.push_back(calculate_poly(coeff_V, x[0]));
    //     for (int j = i + 1; j < P.size(); j++) {
    //         x.push_back(P[j]);
    //         y.push_back(calculate_poly(coeff_V, x[1]));

    //         std::vector<uint64_t> coeff_k;
    //         polynomial_from_points(x, y, coeff_k, 8519681);
    //         if (coeff_k[0] & 0x00 == 0) {
    //             cout << "key retrieval: " << coeff_k[0] << endl;
    //             return 0;
    //         }
    //     }
        
    // }
}