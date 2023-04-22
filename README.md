# Project1
project1

## Test and Deploy
```
// ubuntu
git submodule init
git submodule update
cd deps
// emp-sh2pc
wget https://raw.githubusercontent.com/emp-toolkit/emp-readme/master/scripts/install.py
python install.py --deps --tool --ot --sh2pc
// SEAL
cmake -S . -B build
cmake --build build
sudo cmake --install build
```
```
cd ..
cmake -S . -B build
cd build
make
./test_client 1 12345 & ./test_server 2 12345
```

