/*
uint128_t.h
An unsigned 128 bit integer type for C++
Copyright (c) 2014 Jason Lee @ calccrypto at gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

With much help from Auston Sterling

Thanks to Stefan Deigmoller for finding
a bug in operator*.

Thanks to Francois Dessenne for convincing me
to do a general rewrite of this class.
*/

#ifndef __UINT128_T__
#define __UINT128_T__

#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <utility>

class uint128_t{
    private:
        uint64_t UPPER, LOWER;

    public:
        // Constructors
        inline uint128_t(){
         UPPER = 0;
         LOWER = 0;
      }
        inline uint128_t(const uint128_t & rhs){
         UPPER = rhs.UPPER;
         LOWER = rhs.LOWER;
      }

        template <typename T> uint128_t(const T & rhs){
            UPPER = 0;
            LOWER = rhs;
        }

        template <typename S, typename T> uint128_t(const S & upper_rhs, const T & lower_rhs){
            UPPER = upper_rhs;
            LOWER = lower_rhs;
        }

        //  RHS input args only

        // Assignment Operator
      inline uint128_t operator=(const uint128_t & rhs){
         UPPER = rhs.UPPER;
         LOWER = rhs.LOWER;
         return *this;
      }

        template <typename T> uint128_t operator=(const T & rhs){
            UPPER = 0;
            LOWER = rhs;
            return *this;
        }

        // Typecast Operators
        inline operator bool() const{
         return (bool) (UPPER | LOWER);
      }
        operator char() const;
        operator int() const;
        operator uint8_t() const;
        operator uint16_t() const;
        operator uint32_t() const;
        operator uint64_t() const;

        // Bitwise Operators
      inline uint128_t operator&(const uint128_t & rhs) const{
         return uint128_t(UPPER & rhs.UPPER, LOWER & rhs.LOWER);
      }

      inline uint128_t operator|(const uint128_t & rhs) const{
         return uint128_t(UPPER | rhs.UPPER, LOWER | rhs.LOWER);
      }

      inline uint128_t operator^(const uint128_t & rhs) const{
         return uint128_t(UPPER ^ rhs.UPPER, LOWER ^ rhs.LOWER);
      }

      inline uint128_t operator&=(const uint128_t & rhs){
         UPPER &= rhs.UPPER;
         LOWER &= rhs.LOWER;
         return *this;
      }

      inline uint128_t operator|=(const uint128_t & rhs){
         UPPER |= rhs.UPPER;
         LOWER |= rhs.LOWER;
         return *this;
      }

      inline uint128_t operator^=(const uint128_t & rhs){
         UPPER ^= rhs.UPPER;
         LOWER ^= rhs.LOWER;
         return *this;
      }

      inline uint128_t operator~() const{
         return uint128_t(~UPPER, ~LOWER);
      }


        template <typename T> uint128_t operator&(const T & rhs) const{
            return uint128_t(0, LOWER & (uint64_t) rhs);
        }

        template <typename T> uint128_t operator|(const T & rhs) const{
            return uint128_t(UPPER, LOWER | (uint64_t) rhs);
        }

        template <typename T> uint128_t operator^(const T & rhs) const{
            return uint128_t(UPPER, LOWER ^ (uint64_t) rhs);
        }

        template <typename T> uint128_t operator&=(const T & rhs){
            UPPER = 0;
            LOWER &= rhs;
            return *this;
        }

        template <typename T> uint128_t operator|=(const T & rhs){
            LOWER |= (uint64_t) rhs;
            return *this;
        }

        template <typename T> uint128_t operator^=(const T & rhs){
            LOWER ^= (uint64_t) rhs;
            return *this;
        }

        // Bit Shift Operators
      inline uint128_t operator<<(const unsigned int rhs) const{ //INCORRECT!
         return uint128_t((UPPER << rhs) + (LOWER >> (64 - rhs)), LOWER << rhs);
      }
        inline uint128_t operator<<(const uint128_t & rhs) const{
         uint64_t shift = rhs.LOWER;
         if (shift == 0) return *this;
         if (shift < 64) return uint128_t((UPPER << shift) + (LOWER >> (64 - shift)), LOWER << shift);
         return uint128_t(LOWER << (shift - 64), 0);
      }

      inline uint128_t operator>>(const unsigned int rhs) const{ //INCORRECT
         return uint128_t(UPPER >> rhs, (UPPER << (64 - rhs)) + (LOWER >> rhs));
      }
      inline uint128_t operator>>(const uint128_t & rhs) const{
         uint64_t shift = rhs.LOWER;
         if (shift == 0) return *this;
         if (shift < 64) return uint128_t(UPPER >> shift, (UPPER << (64 - shift)) + (LOWER >> shift));
         return uint128_t(0, (UPPER >> (shift - 64)));
      }
        inline uint128_t operator<<=(const uint128_t & rhs);
        inline uint128_t operator>>=(const uint128_t & rhs);

        template <typename T>uint128_t operator<<(const T & rhs) const{
            return *this << uint128_t(rhs);
        }

        template <typename T>uint128_t operator>>(const T & rhs) const{
            return *this >> uint128_t(rhs);
        }

        template <typename T>uint128_t operator<<=(const T & rhs){
            *this = *this << uint128_t(rhs);
            return *this;
        }

        template <typename T>uint128_t operator>>=(const T & rhs){
            *this = *this >> uint128_t(rhs);
            return *this;
        }

        // Logical Operators
        bool operator!() const{
         return !(bool) (UPPER | LOWER);
      }

      bool operator&&(const uint128_t & rhs) const{
         return ((bool) *this && rhs);
      }

      bool operator||(const uint128_t & rhs) const{
          return ((bool) *this || rhs);
      }


        template <typename T> bool operator&&(const T & rhs){
            return ((bool) *this && rhs);
        }

        template <typename T> bool operator||(const T & rhs){
            return ((bool) *this || rhs);
        }

        // Comparison Operators
        inline bool operator==(const uint128_t & rhs) const{
         return ((UPPER == rhs.UPPER) && (LOWER == rhs.LOWER));
      }
        bool operator!=(const uint128_t & rhs) const;
        bool operator>(const uint128_t & rhs) const;
        bool operator<(const uint128_t & rhs) const;
        bool operator>=(const uint128_t & rhs) const;
        bool operator<=(const uint128_t & rhs) const;

        template <typename T> bool operator==(const T & rhs) const{
            return (!UPPER && (LOWER == (uint64_t) rhs));
        }

        template <typename T> bool operator!=(const T & rhs) const{
            return (UPPER | (LOWER != (uint64_t) rhs));
        }

        template <typename T> bool operator>(const T & rhs) const{
            return (UPPER || (LOWER > (uint64_t) rhs));
        }

        template <typename T> bool operator<(const T & rhs) const{
            return (!UPPER)?(LOWER < (uint64_t) rhs):false;
        }

        template <typename T> bool operator>=(const T & rhs) const{
            return ((*this > rhs) | (*this == rhs));
        }

        template <typename T> bool operator<=(const T & rhs) const{
            return ((*this < rhs) | (*this == rhs));
        }

        // Arithmetic Operators
        uint128_t operator+(const uint128_t & rhs) const;
        uint128_t operator+=(const uint128_t & rhs);
        uint128_t operator-(const uint128_t & rhs) const;
        uint128_t operator-=(const uint128_t & rhs);
        uint128_t operator*(const uint128_t & rhs) const;
        uint128_t operator*=(const uint128_t & rhs);

    private:
        std::pair <uint128_t, uint128_t> divmod(const uint128_t & lhs, const uint128_t & rhs) const;

    public:
      uint128_t operator/(const uint128_t & rhs) const;
        uint128_t operator/=(const uint128_t & rhs);
        uint128_t operator%(const uint128_t & rhs) const;
        uint128_t operator%=(const uint128_t & rhs);

        template <typename T> uint128_t operator+(const T & rhs) const{
            return uint128_t(UPPER + ((LOWER + (uint64_t) rhs) < LOWER), LOWER + (uint64_t) rhs);
        }

        template <typename T> uint128_t operator+=(const T & rhs){
            UPPER = UPPER + ((LOWER + rhs) < LOWER);
            LOWER = LOWER + rhs;
            return *this;
        }

        template <typename T> uint128_t operator-(const T & rhs) const{
            return uint128_t((uint64_t) (UPPER - ((LOWER - rhs) > LOWER)), (uint64_t) (LOWER - rhs));
        }

        template <typename T> uint128_t operator-=(const T & rhs){
            *this = *this - rhs;
            return *this;
        }

        template <typename T> uint128_t operator*(const T & rhs) const{
            return (*this) * (uint128_t(rhs));
        }

        template <typename T> uint128_t operator*=(const T & rhs){
            *this = *this * uint128_t(rhs);
            return *this;
        }

        template <typename T> uint128_t operator/(const T & rhs) const{
            return *this / uint128_t(rhs);
        }

        template <typename T> uint128_t operator/=(const T & rhs){
            *this = *this / uint128_t(rhs);
            return *this;
        }

        template <typename T> uint128_t operator%(const T & rhs) const{
            return *this - (rhs * (*this / rhs));
        }

        template <typename T> uint128_t operator%=(const T & rhs){
            *this = *this % uint128_t(rhs);
            return *this;
        }

        // Increment Operator
        uint128_t operator++();
        uint128_t operator++(int);

        // Decrement Operator
        uint128_t operator--();
        uint128_t operator--(int);

        // Get private values
      inline uint64_t upper() const{
         return UPPER;
      }

      inline uint64_t lower() const{
         return LOWER;
      }

        // Get bitsize of value
        uint8_t bits() const;

        // Get string representation of value
        std::string str(uint8_t base = 10, const unsigned int & len = 0) const;
};

// Useful values
extern const uint128_t uint128_0;
extern const uint128_t uint128_1;
extern const uint128_t uint128_64;
extern const uint128_t uint128_128;

// lhs type T as first arguemnt
// If the output is not a bool, casts to type T

// Bitwise Operators
template <typename T> T operator&(const T & lhs, const uint128_t & rhs){
    return (T) (lhs & (T) rhs.lower());
}

template <typename T> T operator|(const T & lhs, const uint128_t & rhs){
    return (T) (lhs | (T) rhs.lower());
}

template <typename T> T operator^(const T & lhs, const uint128_t & rhs){
    return (T) (lhs ^ (T) rhs.lower());
}

template <typename T> T operator&=(T & lhs, const uint128_t & rhs){
    lhs &= (T) rhs.lower(); return lhs;
}

template <typename T> T operator|=(T & lhs, const uint128_t & rhs){
    lhs |= (T) rhs.lower(); return lhs;
}

template <typename T> T operator^=(T & lhs, const uint128_t & rhs){
    lhs ^= (T) rhs.lower(); return lhs;
}

// Comparison Operators
template <typename T> bool operator==(const T & lhs, const uint128_t & rhs){
    return (!rhs.upper() && ((uint64_t) lhs == rhs.lower()));
}

template <typename T> bool operator!=(const T & lhs, const uint128_t & rhs){
    return (rhs.upper() | ((uint64_t) lhs != rhs.lower()));
}

template <typename T> bool operator>(const T & lhs, const uint128_t & rhs){
    return (!rhs.upper()) && ((uint64_t) lhs > rhs.lower());
}

template <typename T> bool operator<(const T & lhs, const uint128_t & rhs){
    if (rhs.upper()){
        return true;
    }
    return ((uint64_t) lhs < rhs.lower());
}

template <typename T> bool operator>=(const T & lhs, const uint128_t & rhs){
    if (rhs.upper()){
            return false;
    }
    return ((uint64_t) lhs >= rhs.lower());
}

template <typename T> bool operator<=(const T & lhs, const uint128_t & rhs){
    if (rhs.upper()){
            return true;
    }
    return ((uint64_t) lhs <= rhs.lower());
}

// Arithmetic Operators
template <typename T> T operator+(const T & lhs, const uint128_t & rhs){
    return (T) (rhs + lhs);
}

template <typename T> T & operator+=(T & lhs, const uint128_t & rhs){
    lhs = (T) (rhs + lhs);
    return lhs;
}

template <typename T> T operator-(const T & lhs, const uint128_t & rhs){
    return (T) (uint128_t(lhs) - rhs);
}

template <typename T> T & operator-=(T & lhs, const uint128_t & rhs){
    lhs = (T) (uint128_t(lhs) - rhs);
    return lhs;
}

template <typename T> T operator*(const T & lhs, const uint128_t & rhs){
    return lhs * (T) rhs.lower();
}

template <typename T> T & operator*=(T & lhs, const uint128_t & rhs){
    lhs *= (T) rhs.lower();
    return lhs;
}

template <typename T> T operator/(const T & lhs, const uint128_t & rhs){
    return (T) (uint128_t(lhs) / rhs);
}

template <typename T> T & operator/=(T & lhs, const uint128_t & rhs){
    lhs = (T) (uint128_t(lhs) / rhs);
    return lhs;
}

template <typename T> T operator%(const T & lhs, const uint128_t & rhs){
    return (T) (uint128_t(lhs) % rhs);
}

template <typename T> T & operator%=(T & lhs, const uint128_t & rhs){
    lhs = (T) (uint128_t(lhs) % rhs);
    return lhs;
}

// IO Operator
std::ostream & operator<<(std::ostream & stream, const uint128_t & rhs);

const uint128_t uint128_0(0);
const uint128_t uint128_1(1);
const uint128_t uint128_64(64);
const uint128_t uint128_128(128);

uint128_t::operator char() const{
    return (char) LOWER;
}
uint128_t::operator int() const{
    return (int) LOWER;
}

uint128_t::operator uint8_t() const{
    return (uint8_t) LOWER;
}

uint128_t::operator uint16_t() const{
    return (uint16_t) LOWER;
}

uint128_t::operator uint32_t() const{
    return (uint32_t) LOWER;
}

uint128_t::operator uint64_t() const{
    return (uint64_t) LOWER;
}

uint128_t uint128_t::operator<<=(const uint128_t & rhs){
    *this = *this << rhs;
    return *this;
}

uint128_t uint128_t::operator>>=(const uint128_t & rhs){
    *this = *this >> rhs;
    return *this;
}

bool uint128_t::operator!=(const uint128_t & rhs) const{
    return ((UPPER != rhs.UPPER) | (LOWER != rhs.LOWER));
}

bool uint128_t::operator>(const uint128_t & rhs) const{
    if (UPPER == rhs.UPPER){
        return (LOWER > rhs.LOWER);
    }
    return (UPPER > rhs.UPPER);
}

bool uint128_t::operator<(const uint128_t & rhs) const{
    if (UPPER == rhs.UPPER){
        return (LOWER < rhs.LOWER);
    }
    return (UPPER < rhs.UPPER);
}

bool uint128_t::operator>=(const uint128_t & rhs) const{
    return ((*this > rhs) | (*this == rhs));
}

bool uint128_t::operator<=(const uint128_t & rhs) const{
    return ((*this < rhs) | (*this == rhs));
}

uint128_t uint128_t::operator+(const uint128_t & rhs) const{
    return uint128_t(UPPER + rhs.UPPER + ((LOWER + rhs.LOWER) < LOWER), LOWER + rhs.LOWER);
}

uint128_t uint128_t::operator+=(const uint128_t & rhs){
    UPPER = rhs.UPPER + UPPER + ((LOWER + rhs.LOWER) < LOWER);
    LOWER += rhs.LOWER;
    return *this;
}

uint128_t uint128_t::operator-(const uint128_t & rhs) const{
    return uint128_t(UPPER - rhs.UPPER - ((LOWER - rhs.LOWER) > LOWER), LOWER - rhs.LOWER);
}

uint128_t uint128_t::operator-=(const uint128_t & rhs){
    *this = *this - rhs;
    return *this;
}

uint128_t uint128_t::operator*(const uint128_t & rhs) const{
    // split values into 4 32-bit parts
    uint64_t top[4] ={UPPER >> 32, UPPER & 0xffffffff, LOWER >> 32, LOWER & 0xffffffff};
    uint64_t bottom[4] ={rhs.UPPER >> 32, rhs.UPPER & 0xffffffff, rhs.LOWER >> 32, rhs.LOWER & 0xffffffff};
    uint64_t products[4][4];

    for(int y = 3; y > -1; y--){
        for(int x = 3; x > -1; x--){
            products[3 - x][y] = top[x] * bottom[y];
        }
    }

    // initial row
    uint64_t fourth32 = products[0][3] & 0xffffffff;
    uint64_t third32 = (products[0][2] & 0xffffffff) + (products[0][3] >> 32);
    uint64_t second32 = (products[0][1] & 0xffffffff) + (products[0][2] >> 32);
    uint64_t first32 = (products[0][0] & 0xffffffff) + (products[0][1] >> 32);

    // second row
    third32 += products[1][3] & 0xffffffff;
    second32 += (products[1][2] & 0xffffffff) + (products[1][3] >> 32);
    first32 += (products[1][1] & 0xffffffff) + (products[1][2] >> 32);

    // third row
    second32 += products[2][3] & 0xffffffff;
    first32 += (products[2][2] & 0xffffffff) + (products[2][3] >> 32);

    // fourth row
    first32 += products[3][3] & 0xffffffff;

    // combines the values, taking care of carry over
    return uint128_t(first32 << 32, 0) + uint128_t(third32 >> 32, third32 << 32) + uint128_t(second32, 0) + uint128_t(fourth32);
}

uint128_t uint128_t::operator*=(const uint128_t & rhs){
    *this = *this * rhs;
    return *this;
}

std::pair <uint128_t, uint128_t> uint128_t::divmod(const uint128_t & lhs, const uint128_t & rhs) const{
    // Save some calculations /////////////////////
    if (rhs == uint128_0){
        throw std::runtime_error("Error: division or modulus by 0");
    }
    else if (rhs == uint128_1){
        return std::pair <uint128_t, uint128_t> (lhs, uint128_0);
    }
    else if (lhs == rhs){
        return std::pair <uint128_t, uint128_t> (uint128_1, uint128_0);
    }
    else if ((lhs == uint128_0) || (lhs < rhs)){
        return std::pair <uint128_t, uint128_t> (uint128_0, lhs);
    }

    std::pair <uint128_t, uint128_t> qr(uint128_0, lhs);
    uint128_t copyd = rhs << (lhs.bits() - rhs.bits());
    uint128_t adder = uint128_1 << (lhs.bits() - rhs.bits());
    if (copyd > qr.second){
        copyd >>= uint128_1;
        adder >>= uint128_1;
    }
    while (qr.second >= rhs){
        if (qr.second >= copyd){
            qr.second -= copyd;
            qr.first |= adder;
        }
        copyd >>= uint128_1;
        adder >>= uint128_1;
    }
    return qr;
}

uint128_t uint128_t::operator/(const uint128_t & rhs) const{
    return divmod(*this, rhs).first;
}

uint128_t uint128_t::operator/=(const uint128_t & rhs){
    *this = *this / rhs;
    return *this;
}

uint128_t uint128_t::operator%(const uint128_t & rhs) const{
    return *this - (rhs * (*this / rhs));
}

uint128_t uint128_t::operator%=(const uint128_t & rhs){
    *this = *this % rhs;
    return *this;
}

uint128_t uint128_t::operator++(){
    *this += uint128_1;
    return *this;
}

uint128_t uint128_t::operator++(int){
    uint128_t temp(*this);
    ++*this;
    return temp;
}

uint128_t uint128_t::operator--(){
    *this -= uint128_1;
    return *this;
}

uint128_t uint128_t::operator--(int){
    uint128_t temp(*this);
    --*this;
    return temp;
}

uint8_t uint128_t::bits() const{
    uint8_t out = 0;
    if (UPPER){
        out = 64;
        uint64_t up = UPPER;
        while (up){
            up >>= 1;
            out++;
        }
    }
    else{
        uint64_t low = LOWER;
        while (low){
            low >>= 1;
            out++;
        }
    }
    return out;
}

std::string uint128_t::str(uint8_t base, const unsigned int & len) const{
    if ((base < 2) || (base > 16)){
        throw std::invalid_argument("Base must be in th range 2-16");
    }
    std::string out = "";
    if (!(*this)){
        out = "0";
    }
    else{
        std::pair <uint128_t, uint128_t> qr(*this, uint128_0);
        do{
            qr = divmod(qr.first, base);
            out = "0123456789abcdef"[(uint8_t) qr.second] + out;
        } while (qr.first);
    }
    if (out.size() < len){
        out = std::string(len - out.size(), '0') + out;
    }
    return out;
}

std::ostream & operator<<(std::ostream & stream, const uint128_t & rhs){
    if (stream.flags() & stream.oct){
        stream << rhs.str(8);
    }
    else if (stream.flags() & stream.dec){
        stream << rhs.str(10);
    }
    else if (stream.flags() & stream.hex){
        stream << rhs.str(16);
    }
    return stream;
}
#endif