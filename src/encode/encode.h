#ifndef ENCODE_H_
#define ENCODE_H_
#include <vector>
#include <string>
#include <netinet/in.h>

#if defined(__linux__)
    #include <netinet/in.h>
    #define htonll(x) ((((uint64_t)htonl(x)) << 32) + htonl((x) >> 32))
    #define ntohll(x) ((((uint64_t)ntohl(x)) << 32) + ntohl((x) >> 32))
#elif defined(__apple__)
    #include <sys/_endian.h>
#endif

void GetEncodeRes(std::string file, std::vector<uint8_t> &res_encode);
void GetEncodeRes(std::string file, int N, std::vector<std::vector<uint8_t>> &res_encode);

#endif