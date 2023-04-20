# flpsi
flpsi

## Test and Deploy
```
// ubuntu
//          |-emp-ot/
//          |-emp-sh2pc/
//          |-emp-tool/
//          |-SEAL/
// project1 |-project1 |-src
//                     |-CMakeLists.txt
//                     |-README.md

cmake -S . -B build
cd build
make
./test_client 1 12345 & ./test_server 2 12345
```

