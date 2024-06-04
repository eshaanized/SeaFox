/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * Imported from:
 * https://chromium.googlesource.com/chromium/src.git/+/master/third_party/WebKit/Source/platform/Decimal.h
 * Check UPSTREAM-GIT-SHA for the commit ID of the last update from Blink core.
 */

#ifndef Decimal_h
#define Decimal_h

#include "mozilla/Assertions.h"
#include <stdint.h>
#include "mozilla/Types.h"

#include <string>

#ifndef ASSERT
#define DEFINED_ASSERT_FOR_DECIMAL_H 1
#define ASSERT MOZ_ASSERT
#endif

#define PLATFORM_EXPORT

// To use USING_FAST_MALLOC we'd need:
// https://chromium.googlesource.com/chromium/src.git/+/master/third_party/WebKit/Source/wtf/Allocator.h
// Since we don't allocate Decimal objects, no need.
#define USING_FAST_MALLOC(type) \
  void ignore_this_dummy_method() = delete

#define DISALLOW_NEW()                                          \
    private:                                                    \
        void* operator new(size_t) = delete;                    \
        void* operator new(size_t, void*) = delete;             \
    public:

namespace blink {

namespace DecimalPrivate {
constexpr int ExponentMax = 1023;
constexpr int ExponentMin = -1023;
constexpr int Precision = 18;

static const uint64_t MaxCoefficient = UINT64_C(0xDE0B6B3A763FFFF);  // 999999999999999999 == 18 9's
class SpecialValueHandler;
}

struct DecimalLiteral {
  int32_t value;
  friend constexpr DecimalLiteral operator*(int32_t lhs, DecimalLiteral rhs) {
    return {lhs * rhs.value};
  }
  constexpr DecimalLiteral operator-() {
    return {-value};
  }
};

constexpr DecimalLiteral operator""_d(unsigned long long value) {
  return {static_cast<int32_t>(value)};
}

// This class represents decimal base floating point number.
//
// FIXME: Once all C++ compiler support decimal type, we should replace this
// class to compiler supported one. See below URI for current status of decimal
// type for C++: // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n1977.html
class PLATFORM_EXPORT Decimal {
    USING_FAST_MALLOC(Decimal);
public:
    enum Sign {
        Positive,
        Negative,
    };

    // You should not use EncodedData other than unit testing.
    class EncodedData {
        DISALLOW_NEW();
        // For accessing FormatClass.
        friend class Decimal;
        friend class DecimalPrivate::SpecialValueHandler;
    public:
     constexpr EncodedData(Sign sign, int exponent, uint64_t coefficient)
         : m_coefficient(0),
           m_exponent(0),
           m_formatClass(coefficient ? ClassNormal : ClassZero),
           m_sign(sign) {
       if (exponent >= DecimalPrivate::ExponentMin &&
           exponent <= DecimalPrivate::ExponentMax) {
         while (coefficient > DecimalPrivate::MaxCoefficient) {
           coefficient /= 10;
           ++exponent;
         }
       }

       if (exponent > DecimalPrivate::ExponentMax) {
         m_formatClass = ClassInfinity;
         return;
       }

       if (exponent < DecimalPrivate::ExponentMin) {
         m_formatClass = ClassZero;
         return;
       }

       m_coefficient = coefficient;
       m_exponent = static_cast<int16_t>(exponent);
     }

        bool operator==(const EncodedData&) const;
        bool operator!=(const EncodedData& another) const { return !operator==(another); }

        uint64_t coefficient() const { return m_coefficient; }
        int countDigits() const;
        int exponent() const { return m_exponent; }
        bool isFinite() const { return !isSpecial(); }
        bool isInfinity() const { return m_formatClass == ClassInfinity; }
        bool isNaN() const { return m_formatClass == ClassNaN; }
        bool isSpecial() const { return m_formatClass == ClassInfinity || m_formatClass == ClassNaN; }
        bool isZero() const { return m_formatClass == ClassZero; }
        Sign sign() const { return m_sign; }
        void setSign(Sign sign) { m_sign = sign; }

    private:
        enum FormatClass {
            ClassInfinity,
            ClassNormal,
            ClassNaN,
            ClassZero,
        };

        constexpr EncodedData(Sign sign, FormatClass formatClass)
            : m_coefficient(0),
              m_exponent(0),
              m_formatClass(formatClass),
              m_sign(sign) {}

        FormatClass formatClass() const { return m_formatClass; }

        uint64_t m_coefficient;
        int16_t m_exponent;
        FormatClass m_formatClass;
        Sign m_sign;
    };

    constexpr explicit Decimal(DecimalLiteral i32)
    : m_data(i32.value < 0 ? Negative : Positive, 0,
             i32.value < 0 ? static_cast<uint64_t>(-static_cast<int64_t>(i32.value))
                     : static_cast<uint64_t>(i32.value)) {}

    MFBT_API explicit Decimal(int32_t i32 = 0);
    MFBT_API Decimal(Sign sign, int exponent, uint64_t coefficient);
    MFBT_API Decimal(const Decimal&);

    MFBT_API Decimal& operator=(const Decimal&);
    MFBT_API Decimal& operator+=(const Decimal&);
    MFBT_API Decimal& operator-=(const Decimal&);
    MFBT_API Decimal& operator*=(const Decimal&);
    MFBT_API Decimal& operator/=(const Decimal&);

    MFBT_API Decimal operator-() const;

    MFBT_API bool operator==(const Decimal&) const;
    MFBT_API bool operator!=(const Decimal&) const;
    MFBT_API bool operator<(const Decimal&) const;
    MFBT_API bool operator<=(const Decimal&) const;
    MFBT_API bool operator>(const Decimal&) const;
    MFBT_API bool operator>=(const Decimal&) const;

    MFBT_API Decimal operator+(const Decimal&) const;
    MFBT_API Decimal operator-(const Decimal&) const;
    MFBT_API Decimal operator*(const Decimal&) const;
    MFBT_API Decimal operator/(const Decimal&) const;

    int exponent() const
    {
        ASSERT(isFinite());
        return m_data.exponent();
    }

    bool isFinite() const { return m_data.isFinite(); }
    bool isInfinity() const { return m_data.isInfinity(); }
    bool isNaN() const { return m_data.isNaN(); }
    bool isNegative() const { return sign() == Negative; }
    bool isPositive() const { return sign() == Positive; }
    bool isSpecial() const { return m_data.isSpecial(); }
    bool isZero() const { return m_data.isZero(); }

    MFBT_API Decimal abs() const;
    MFBT_API Decimal ceil() const;
    MFBT_API Decimal floor() const;
    MFBT_API Decimal remainder(const Decimal&) const;
    MFBT_API Decimal round() const;

    MFBT_API double toDouble() const;
    // Note: toString method supports infinity and nan but fromString not.
    MFBT_API std::string toString() const;
    MFBT_API bool toString(char* strBuf, size_t bufLength) const;

    static MFBT_API Decimal fromDouble(double);
    // fromString supports following syntax EBNF:
    //  number ::= sign? digit+ ('.' digit*) (exponent-marker sign? digit+)?
    //          | sign? '.' digit+ (exponent-marker sign? digit+)?
    //  sign ::= '+' | '-'
    //  exponent-marker ::= 'e' | 'E'
    //  digit ::= '0' | '1' | ... | '9'
    // Note: fromString doesn't support "infinity" and "nan".
    static MFBT_API Decimal fromString(const std::string& aValue);
    static MFBT_API Decimal infinity(Sign);
    static constexpr Decimal nan() {
        return Decimal(EncodedData(Positive, EncodedData::ClassNaN));
    }
    static MFBT_API Decimal zero(Sign);

    // You should not use below methods. We expose them for unit testing.
    constexpr explicit Decimal(const EncodedData& data) : m_data(data) {}
    const EncodedData& value() const { return m_data; }

private:
    struct AlignedOperands {
        uint64_t lhsCoefficient;
        uint64_t rhsCoefficient;
        int exponent;
    };

    MFBT_API explicit Decimal(double);
    MFBT_API Decimal compareTo(const Decimal&) const;

    static MFBT_API AlignedOperands alignOperands(const Decimal& lhs, const Decimal& rhs);
    static inline Sign invertSign(Sign sign) { return sign == Negative ? Positive : Negative; }

    Sign sign() const { return m_data.sign(); }

    EncodedData m_data;
};

} // namespace blink

namespace mozilla {
typedef blink::Decimal Decimal;
using blink::operator""_d;
} // namespace mozilla

#undef USING_FAST_MALLOC

#ifdef DEFINED_ASSERT_FOR_DECIMAL_H
#undef DEFINED_ASSERT_FOR_DECIMAL_H
#undef ASSERT
#endif

#endif // Decimal_h