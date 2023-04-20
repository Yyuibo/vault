#include <cassert>
#include <set>

#include "key_binding_and_retrieval/polynomials.h"
#include "key_binding_and_retrieval/point_conversion.h"
#include "key_binding_and_retrieval/cs_functions.h"
#include <algorithm>

uint64_t modexp(uint64_t base, uint64_t exponent, uint64_t modulus) {
    uint64_t result = 1;
    while (exponent > 0) {
        if (exponent & 1) {
            result = MUL_MOD(result, base, modulus);
        }
        base = MUL_MOD(base, base, modulus);
        exponent = (exponent >> 1);
    }
    return result;
}

uint64_t modinv(uint64_t x, uint64_t modulus) {  // 此求逆方法运用欧拉定理，限定modulus为素数
    return modexp(x, modulus - 2, modulus);
}

void polynomial_from_roots(vector<uint32_t>& roots, vector<uint64_t>& coeffs, uint32_t modulus) {
    coeffs.clear();
    coeffs.resize(roots.size() + 1);
    coeffs[0] = 1;

    for (size_t i = 0; i < roots.size(); i++) {
        // multiply coeffs by (x - root)
        uint32_t neg_root = modulus - (roots[i] % modulus);

        for (size_t j = i + 1; j > 0; j--) {
            coeffs[j] = (coeffs[j - 1] + MUL_MOD(neg_root, coeffs[j], modulus)) % modulus;
        }
        coeffs[0] = MUL_MOD(coeffs[0], neg_root, modulus);
    }
}

void polynomial_from_points(vector<uint32_t>& xs,
    vector<uint32_t>& ys,
    vector<uint64_t>& coeffs,
    uint32_t modulus)
{
    assert(xs.size() == ys.size());
    coeffs.clear(); // 此处clear只能将size置0，打印coeffs.size()是0。但是coeffs里的元素不变，coeffs[0]依然能打印出原值
    coeffs.resize(xs.size()); // resize以后coeffs就变成xs.size()个0了。最终coeffs[i]是x^i的系数

    if (xs.size() == 0) {
        return;
    }

    // at iteration i of the loop, basis contains the coefficients of the basis
    // polynomial (x - xs[0]) * (x - xs[1]) * ... * (x - xs[i - 1])
    vector<uint32_t> basis(xs.size());
    basis[0] = 1;

    // at iteration i of the loop, ddif[j] contains the divided difference
    // [ys[j], ys[j + 1], ..., ys[j + i]]. thus initially, when i = 0,
    // ddif[j] = [ys[j]] = ys[j]
    vector<uint32_t> ddif = ys;

    for (size_t i = 0; i < xs.size(); i++) {
        for (size_t j = 0; j < i + 1; j++) {
            coeffs[j] = (coeffs[j] + MUL_MOD(ddif[0], basis[j], modulus)) % modulus;
        }

        if (i < xs.size() - 1) {
            // update basis: multiply it by (x - xs[i])
            uint32_t neg_x = modulus - (xs[i] % modulus);

            for (size_t j = i + 1; j > 0; j--) {
                basis[j] = (basis[j - 1] + MUL_MOD(neg_x, basis[j], modulus)) % modulus;
            }
            basis[0] = MUL_MOD(basis[0], neg_x, modulus);

            // update ddif: compute length-(i + 1) divided differences
            for (size_t j = 0; j + i + 1 < xs.size() + 1; j++) {
                // dd_{j,j+i+1} = (dd_{j+1, j+i+1} - dd_{j, j+i}) / (x_{j+i+1} - x_j)
                uint32_t num = (ddif[j + 1] - ddif[j] + modulus) % modulus;
                uint32_t den = (xs[j + i + 1] - xs[j] + modulus) % modulus;
                ddif[j] = MUL_MOD(num, modinv(den, modulus), modulus);
            }
        }
    }
}

uint32_t calculate_poly(vector<uint64_t> coeffs, uint32_t x)
{
    reverse(coeffs.begin(), coeffs.end()); //coeffs是从低到高的系数，需要先逆序
    uint32_t value = coeffs[0];
    for (int i = 0; i < coeffs.size() - 1; i++)
    {
        value = (value * x + coeffs[i + 1]) % 8519681;
    }
    return value;
}

// 获取分组插值的系数。coeffs结构为vec_len * n 的矩阵。其中n为参与插值的点数（即分组大小），vec_len为特征向量长度
void get_group_coeffs(vector<vector<uint64_t>> &coeffs, int n, int mod, vector<point_rt> &points, int vec_len) {
    coeffs.resize(vec_len, vector<uint64_t>());
    for (size_t m = 0;m < vec_len; m++){
        for (size_t i = 0;i < points.size();i+=n) {
            vector<uint32_t> T;
            vector<uint32_t> R;
            for (size_t j = i; j < i+n; j++) {
                T.push_back(float2fix(points[j].ty[m]));
                R.push_back(points[j].rx);
            }
            vector<uint64_t> coeff;
            polynomial_from_points(R, T, coeff, mod);
            coeffs[m].insert(coeffs[m].end(), coeff.begin(), coeff.end());
        }
    }
}

void testInterpolation() {
    uint32_t x[5] = {11,2,13,4,16};
    uint32_t y[5] = {6,17,8,19,23};
    vector<uint32_t> xs(x,x+5);
    vector<uint32_t> ys(y,y+5);
    vector<uint64_t> coeff;
    polynomial_from_points(xs,ys,coeff,13);
    uint32_t y_res[5];
    for(int i=0;i<5;i++){
        y_res[i] = calculate_poly(coeff,x[i]);
    }

    cout << "------------test interpolation------------" << endl;
    cout << "modulus = 13" << endl;
    cout << "x contains: ";
    for(int i=0;i<5;i++){
        cout << xs[i] << " ";
    }cout << endl;
    cout << "y contains: ";
    for(int i=0;i<5;i++){
        cout << ys[i] << " ";
    }cout << endl;
    cout << "x % 13 contains: ";
    for(int i=0;i<5;i++){
        cout << x[i]%13 << " ";
    }cout << endl;
    cout << "y % 13 contains: ";
    for(int i=0;i<5;i++){
        cout << y[i]%13 << " ";
    }cout << endl;
    cout << "coeffs of polynomial from x and y: ";
    for(int i=0;i<coeff.size();i++){
        cout << coeff[i] << " ";
    }cout << endl;
    cout << "------------verify the result------------" << endl;
    for(int i=0;i<5;i++){
        cout << "when x = " << setw(2) << x[i] <<", ";
        cout << "y_res = " << setw(6) << y_res[i] <<", ";
        cout << "y_res % 13 = " << setw(1) << y_res[i]%13;
        cout << endl;
    }
    cout << "-----------interpolation right-----------" << endl;
}
