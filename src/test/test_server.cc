#include <iostream>
#include <cstddef>
#include <map>
#include <emp-sh2pc/emp-sh2pc.h>
#include "encode/encode.h"
// #include "key_binding_and_retrieval/cs_functions.h"
#include "key_binding_and_retrieval/polynomials.h"

using namespace std;
using namespace emp;

void uint64_2_bytes(uint64_t num, std::byte* bytes) {
    for (int i = 0; i < 8; i++) {
        bytes[i] = (std::byte)((num >> 8 * i) & 0xFF);
    }
}

void SendData(emp::NetIO * io, uint64_t data) {
    std::byte bytes[8];
    uint64_2_bytes(data, bytes);
    io->send_data(bytes, 8);
}


void ExpandPoly(std::vector<uint8_t> P, std::vector<uint64_t> &res, uint64_t module) {
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
            res[j] = (m1[j] + m2[j]) % module;
        }
    }
    // (x - a)(x - b) = x^2 -(a + b)x + ab
    for (int i = n - 1; i >= 0; i = i - 2) {
        res[i] = (module - res[i]) % module;
        // res[i + 1] = res[i] % module;
    }
}

uint64_t res_cmn = 0;
void Cmn(int depth, std::vector<uint64_t> res, std::vector<uint64_t> P) {
    if (depth == 1) {
        // cout << res_cmn << endl;
        for (int i = 0; i < res.size(); i++) {
            res_cmn += res[i];
            res_cmn %= 8519681;
        } 
        return;
    }
    
    for (int i = 0; i < res.size() - 1; i++) {
        std::vector<uint64_t> tmp(P.size() - 1);
        for (int j = 1; j < P.size(); j++) {
            tmp[j - 1] = (res[i] * P[j]) % 8519681;
        }
        P.erase(P.begin());
        Cmn(depth - 1, tmp, P);
    }
}

int main(int argc, char** argv) {
    std::string file = "../src/encode/embedding_vector_server.txt";
    std::vector<uint8_t> P;
    GetEncodeRes(file, P);
 
    // the secret polynomial κ of degree smaller than k
    std::vector<uint32_t> x = {0, 1};
    std::vector<uint32_t> y = {0x0001, 0x03};
    std::vector<uint64_t> coeff_k;
    polynomial_from_points(x, y, coeff_k, 8519681);

    // V(X) of degree = P.size()
    std::vector<uint64_t> coeff_V(P.size() + 1, 0);
    ExpandPoly(P, coeff_V, 8519681);
    for (int i = 0; i < coeff_k.size(); i++) {
        coeff_V[i] += coeff_k[i];
    }


    int port, party;
    emp::parse_party_and_port(argv, &party, &port);
    emp::NetIO * io = new emp::NetIO(party == ALICE ? nullptr : "127.0.0.1", port);
    emp::setup_semi_honest(io, party);

    // send coeff_V.size()
    SendData(io, coeff_V.size());

    for (int i = 0; i < coeff_V.size(); i++) {
        SendData(io, coeff_V[i]);
    }
    

    emp::finalize_semi_honest();

}