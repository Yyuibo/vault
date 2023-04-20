#include "encode.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "super_bit_lsh.h"

using namespace std;

void ReadTxt_256_1(std::string file, double embedding_vector_1[256][2]) {
    std::ifstream in_file;
    in_file.open(file, std::ios_base::in);
    std::string line;
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 2; j++) {
            std::getline(in_file, line);
            // cout << file << endl;  
            // cout <<"line:" << line << endl;
            embedding_vector_1[i][j] = stod(line);
        }
    }
    in_file.close();
}

void ReadTxt_256_N(std::string file, int N, double embedding_vectors[][256][2]) {
    std::ifstream in_file;
    in_file.open(file, std::ios_base::in);
    std::string line;
    for (int k = 0; k < N; k++) {
        for (int i = 0; i < 256; i++) {
            for (int j = 0; j < 2; j++) {
                std::getline(in_file, line);
                // cout << file << endl;  
                // cout <<"line:" << line << endl;
                embedding_vectors[k][i][j] = stod(line);
            }
        }
    }
    in_file.close();
}

void GetEncodeRes(std::string file, std::vector<uint8_t> &res_encode) {
    double embedding_vector_1[256][2];
    ReadTxt_256_1(file, embedding_vector_1);

    int count = 256;  
    // R^n
    int n = 2;
    int stages = 4;
    int buckets = 2;
    int cnt = 0;
    LshSuperBit lsb = LshSuperBit(stages, buckets, n);
    
    // Compute a SuperBit signature, and a LSH hash
    uint64_t tmp = 0;
    for (int i = 0; i < count; i++) {
        double vector_1[n];
        std::copy(&embedding_vector_1[i][0], &embedding_vector_1[i][0] + n, &vector_1[0]);

        if (lsb.hash(vector_1)[0]) {
            res_encode.push_back(i);
        }
    }
}

void GetEncodeRes(std::string file, int N, std::vector<std::vector<uint8_t>> &res_encode) {
    double embedding_vectors[N][256][2];
    ReadTxt_256_N(file, N, embedding_vectors);

    int count = 256;  
    // R^n
    int n = 2;
    int stages = 4;
    int buckets = 2;
    int cnt = 0;
    LshSuperBit lsb = LshSuperBit(stages, buckets, n);

    // Compute a SuperBit signature, and a LSH hash
    for (int k = 0; k < N; k++) {
        uint64_t tmp = 0;
        for (int i = 0; i < count; i++) {
            double vector_1[n];
            std::copy(&embedding_vectors[k][i][0], &embedding_vectors[k][i][0] + n, &vector_1[0]);
            
            if (lsb.hash(vector_1)[0]) {
                res_encode[k].push_back(i);
            }
        }
    }
}