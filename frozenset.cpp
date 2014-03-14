#include "frozenset.h"

namespace hash {

    FrozenSet::FrozenSet(long_int tableSize)
        : m_Size(tableSize)
    {
        firstHash();
        secondHash();
    }

    bool FrozenSet::contains(long_int key) const
    {
        long_int hashKey = hash(m_Parameters.a, m_Parameters.b, m_Parameters.prime, m_Size, key);
        long_int hashKey2(0);

        // collision! more than one element in the bucket
        if (m_Table[hashKey]._e.size() > 1)
            hashKey2 = hash(m_Table[hashKey]._p.a, m_Table[hashKey]._p.b, m_Table[hashKey]._p.prime, m_Table[hashKey]._e.size(), key);

        return m_Table[hashKey]._e[hashKey2] == key;
    }

    long_int FrozenSet::hash(const long_int a, const long_int b, const long_int p, const long_int table_size, const long_int key) const
    {
        return (((a*key + b) % p) % table_size);
    }

    std::function<long_int ()> FrozenSet::random(long_int from, long_int to) const
    {
        std::default_random_engine generator;
        std::uniform_int_distribution<long_int> distribution(from, to);
        return std::bind(distribution, generator);
    }

    bool FrozenSet::isPrime(long_int num) const
    {
        for(int i = 2; i <= std::sqrt(num); ++i)
            if (num % i == 0) return false;

        return true;
    }

    long_int FrozenSet::firstPrimeAfter(long_int num) const
    {
        while (!isPrime(num)) ++num;
        return num;
    }

    bool FrozenSet::noCollision(const Bucket &b) const
    {
        auto size = b._e.size();
        for (size_t i = 0; i < size; ++i) {
            for (size_t j = 0; j < size; ++j) {
                if (i == j) continue;
                if (hash(b._p.a, b._p.b, b._p.prime, size * size, b._e[i]) ==
                    hash(b._p.a, b._p.b, b._p.prime, size * size, b._e[j])) return false;
            }
        }
        return true;
    }

    void FrozenSet::firstHash()
    {
        // setup parameters
        m_Parameters.prime = firstPrimeAfter(m_Size);
        auto r_a = random(0, m_Parameters.prime-1);
        auto r_b = random(1, m_Parameters.prime-1);
        do {
            m_Parameters.a = r_a();
            m_Parameters.b = r_b();
        } while (m_Parameters.a == m_Parameters.b);

        // start hashing
        m_Table.resize(m_Size, {{m_Parameters.prime, 0, 0}, Element()});
        for (long_int i = 0; i < m_Size; ++i) {
            m_Table[hash(m_Parameters.a, m_Parameters.b, m_Parameters.prime, m_Size, i)]._e.push_back(i);
        }
    }

    void FrozenSet::secondHash()
    {
        std::function<long_int()> r_a = nullptr, r_b = nullptr;
        Element e;
        size_t size = 0;
        for (auto &bucket : m_Table) {
            if (bucket._e.size() <= 1) continue; // if we don't have collision. it really helps save time
            bucket._p.prime = firstPrimeAfter(*std::max_element(bucket._e.begin(), bucket._e.end()));
            r_a = random(0, bucket._p.prime-1);
            r_b = random(1, bucket._p.prime-1);

            do {
                do {
                    bucket._p.a = r_a();
                    bucket._p.b = r_b();
                } while (bucket._p.a == bucket._p.b);
            } while (!noCollision(bucket));

            size = bucket._e.size() * bucket._e.size();
            e.resize(size);
            for(size_t i = 0; i < bucket._e.size(); ++i)
                e[hash(bucket._p.a, bucket._p.b, bucket._p.prime, size, bucket._e[i])] = bucket._e[i];
            bucket._e = std::move(e);
        }
    }

}
