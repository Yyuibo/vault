#ifndef LSH_H_
#define LSH_H_

class Lsh {
    // private default
    int stages_;
    int buckets_;
    static const int DEFAULT_STAGES_ = 3;
    static const int DEFAULT_BUCKETS_ = 10;

    protected:
        static const long LARGE_PRIME_ = 433494437;
    
    public:
        Lsh();
        Lsh(int stages, int buckets);
    
        // Hash a signature.
        // The signature is divided in s stages (or bands). 
        // Each stage is hashed to one of the b buckets.
        // return An vector of s integers (between 0 and b-1)
        int* HashSignature(bool* signature, int length);

};

#endif