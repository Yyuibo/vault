#ifndef SUPER_BIT_H_
#define SUPER_BIT_H_

class SuperBit {
    // private default
    int code_length_;
    int d_;
    double** hyperplanes_;
    static const int DEFAULT_CODE_LENGTH_ = 10000;
    static void Sub(double* a, double* b, int a_length, double* res); 
    static double Norm(double* v, int v_length);
    static void Normalize(double* v, int v_length);
    static void Product(double x, double* v, int v_length, double* res);
    static double DotProduct(double* v1, double* v2, int v1_length);

    public:
        SuperBit();
        SuperBit(int d, int n, int l);
        bool* Signature(double* vector);
        void getHyperplanes(double** res);
        int getCodeLength();
};

#endif