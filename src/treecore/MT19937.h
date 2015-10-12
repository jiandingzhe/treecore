/*
   A C-program for MT19937-64 (2014/2/23 version).
   Coded by Takuji Nishimura and Makoto Matsumoto.

   This is a 64-bit version of Mersenne Twister pseudorandom number
   generator.

   Before using, initialize the state by using init_genrand64(seed)
   or init_by_array64(init_key, key_length).

   Copyright (C) 2004, 2014, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote
        products derived from this software without specific prior written
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   References:
   T. Nishimura, ``Tables of 64-bit Mersenne Twisters''
     ACM Transactions on Modeling and
     Computer Simulation 10. (2000) 348--357.
   M. Matsumoto and T. Nishimura,
     ``Mersenne Twister: a 623-dimensionally equidistributed
       uniform pseudorandom number generator''
     ACM Transactions on Modeling and
     Computer Simulation 8. (Jan. 1998) 3--30.

   Any feedback is very welcome.
   http://www.math.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove spaces)
*/
#ifndef TREECORE_MT19937_H
#define TREECORE_MT19937_H

#include "treecore/Common.h"
#include "treecore/RefCountObject.h"
#include "treecore/IntTypes.h"

class TestFramework;

namespace treecore {

class BigInteger;

#define NN 312

/**
 * @brief pseudo random number generator using 64 bit MT19937
 */
class MT19937: public RefCountObject
{
    friend class ::TestFramework;

public:
    /**
     * @brief create PRNG and seed it automatically
     *
     * @see set_seed()
     */
    MT19937();

    /**
     * @brief create PRNG using specified seed
     *
     * @param seed
     *
     * @see set_seed(uint64 seed)
     */
    MT19937(uint64 seed);

    /**
     * @brief create PRNG using an array of seeds
     *
     * @param seed_array  point to an array of seed values
     * @param len         number of seed values
     *
     * @see set_seed(uint64* seed_array, size_t len)
     */
    MT19937(uint64* seed_array, size_t len);

    /**
     * @brief set PRNG state automatically
     *
     * The seed will be get using system random device, or current date and
     * milliseconds, depends on whether current system has random device.
     */
    void set_seed();

    /**
     * @brief set PRNG state using one seed value
     *
     * @param seed
     */
    void set_seed(uint64 seed);

    /**
     * @brief set PRNG state using an array of seeds
     *
     * @param seed_array  point to an array of seed values
     * @param len         number of seed values
     */
    void set_seed_array(uint64* seed_array, size_t len);

    /**
     * @brief get random boolean value
     * @return random bool value
     */
    bool next_bool();

    /**
     * @brief get 64-bit integer random value
     * @return random integer value
     */
    uint64 next_uint64();

    /**
     * @brief get integer random value within range
     * @param upper  the result will be smaller than this limit
     * @return random integer value
     */
    uint64 next_uint64_in_range(uint64 upper);

    /**
     * @brief get 63-bit integer random value
     *
     * The sign bit will be always zero, thus you get a positive 64-bit integer.
     *
     * @return random positive integer value
     */
    int64 next_int63();

    /**
     * @brief get random value in range [0, 1]
     * @return random float value
     */
    double next_double_yy();

    /**
     * @brief get random value in range [0, 1)
     * @return random float value
     */
    double next_double_yn();

    /**
     * @brief get random value in range (0, 1)
     * @return random float value
     */
    double next_double_nn();

    /**
     * @brief get a series of random contents
     *
     * @param buffer  result will be written here
     * @param size    number of bytes to fill
     */
    void fill_bits_randomly(uint8* buffer, size_t size);

    /**
     * @brief get a series of random contents
     *
     * Note that this function fill content in bits, not in bytes!
     *
     * @param buffer     result will be written here
     * @param start_bit  fill from this position
     * @param num_bits   number of bits to fill
     */
    void fill_bits_randomly(BigInteger& buffer, int start_bit, int num_bits);

    /**
     * @brief a global PRNG instance that can be used directly
     */
    static MT19937 easy;

protected:
    uint64_t mt[NN];
    int mti = NN + 1;
}; // class MT19937

}

#endif // TREECORE_MT19937_H
