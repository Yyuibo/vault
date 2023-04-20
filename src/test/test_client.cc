#include <iostream>
#include <cstddef>
#include <emp-sh2pc/emp-sh2pc.h>
#include "encode/encode.h"
#include "key_binding_and_retrieval/polynomials.h"

using namespace std;
using namespace emp;

void bytes_2_uint64(std::byte* bytes, uint64_t &num) {
    num = 0;
    for (int i = 0; i < 8; i++) {
        num |= (uint64_t)bytes[i] << (8 * i);
    }
}

void RecvData(emp::NetIO * io, uint64_t &data) {
    std::byte bytes[8];
    io->recv_data(bytes, 8);
    bytes_2_uint64(bytes, data);
}

int main(int argc, char** argv) {
    std::string file = "../src/encode/embedding_vector_client.txt";
    std::vector<uint8_t> P;
    GetEncodeRes(file, P);
    
    int port, party;
    emp::parse_party_and_port(argv, &party, &port);
    emp::NetIO * io = new emp::NetIO(party == ALICE ? nullptr : "127.0.0.1", port);
    emp::setup_semi_honest(io, party);

    // receive coeff_V.size()
    uint64_t coeff_size;
    RecvData(io, coeff_size);

    std::vector<uint64_t> coeff_V(coeff_size, 0);
    for (int i = 0; i < coeff_size; i++) {
        RecvData(io, coeff_V[i]);
    }
    
    // retrieval k = 2
    std::vector<uint32_t> x;
    std::vector<uint32_t> y;
    for (int i = 0; i < P.size() - 1; i++) {
        x.clear();
        y.clear();
        x.push_back(P[i]);
        y.push_back(calculate_poly(coeff_V, x[0]));
        for (int j = i + 1; j < P.size(); j++) {
            x.push_back(P[j]);
            y.push_back(calculate_poly(coeff_V, x[1]));

            std::vector<uint64_t> coeff_k;
            polynomial_from_points(x, y, coeff_k, 8519681);
            if (coeff_k[0] & 0x00 == 0) {
                cout << "key retrieval: " << coeff_k[0] << endl;
                return 0;
            }
        }
        
    }

    emp::finalize_semi_honest();
}