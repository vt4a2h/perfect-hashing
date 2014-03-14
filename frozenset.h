#pragma once

#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <functional>

namespace hash {

    typedef long long int long_int;
    typedef std::vector<long_int> Element;

    struct Parameters
    {
        long_int prime;
        long_int a;
        long_int b;
    };
    typedef std::unordered_map<long_int, Parameters> SecondFuncParameters;

    struct Bucket {
        Parameters _p;
        Element    _e;
    };
    using FrozenTable = std::vector<Bucket>;

    class FrozenSet
    {
    public:
        FrozenSet(long_int tableSize);

        bool contains(long_int key) const;

    private:
        long_int hash(const long_int a, const long_int b, const long_int p, const long_int table_size, const long_int key) const;
        std::function<long_int()> random(long_int from, long_int to) const;
        bool isPrime(long_int num) const;
        long_int firstPrimeAfter(long_int num) const;
        bool noCollision(const Bucket &b) const;

        void firstHash();
        void secondHash();

        long_int m_Size;
        FrozenTable m_Table;

        Parameters m_Parameters;
    };

}
