#include <iostream>
#include <cstddef>
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
    
    for (int i = 0; i < res.size(); i++) {
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
    // for test
    P.resize(20);
    
    // the secret polynomial κ of degree smaller than k
    std::vector<uint32_t> x = {0, 1};
    std::vector<uint32_t> y = {0x0001, 0x03};
    std::vector<uint64_t> coeff_k;
    polynomial_from_points(x, y, coeff_k, 8519681);

    // V(X)
    std::vector<uint64_t> coeff_V(P.size() + 1, 0);
    cout << "polynomial expansion" << endl;
    std::vector<uint64_t> res;
    std::vector<uint64_t> P1;
    res.resize(P.size());
    P1.resize(P.size());

    // polynomial expansion
    for (int i = 0; i < coeff_V.size() - 1; i++) {
        res_cmn = 0;
        for (int i = 0; i < res.size(); i++) {
            res[i] = (uint64_t)P[i];
            P1[i] = (uint64_t)P[i];
        }
        Cmn(i + 1, res, P1);
        if (i % 2 == 0) {
            coeff_V[coeff_V.size() - i - 2] = 8519681 - res_cmn;
        }
        else {
            coeff_V[coeff_V.size() - i - 2] = res_cmn;
        }
    }
    for (int i = 0; i < coeff_k.size(); i++) {
        coeff_V[i] += coeff_k[i];
    }
    coeff_V[coeff_V.size() - 1] = 1;
    // for (int i = 0; i < coeff_V.size(); i++) {
    //     cout << coeff_V[i] << endl;
    // }
    // cout << (uint32_t)P[17] << endl;
    // cout << calculate_poly(coeff_V, (uint32_t)P[17]) << endl;

    int port, party;
    emp::parse_party_and_port(argv, &party, &port);
    emp::NetIO * io = new emp::NetIO(party == ALICE ? nullptr : "127.0.0.1", port);
    emp::setup_semi_honest(io, party);

    // send coeff_V.size()
    SendData(io, coeff_V.size());

    for (int i = 0; i < coeff_V.size(); i++) {
        SendData(io, coeff_V[i]);
    }
    

    // emp::finalize_semi_honest();

}