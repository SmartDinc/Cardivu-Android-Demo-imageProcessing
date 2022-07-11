#ifndef ANDROID_DSP_SERVICE_TYPES_H
#define ANDROID_DSP_SERVICE_TYPES_H

#include "common.h"
#include "math_supplement.h"

namespace Dsp {

// a conjugate or real pair
    struct ComplexPair : complex_pair_t
    {
        ComplexPair()
        {
        }

        explicit ComplexPair(const complex_t& c1)
                : complex_pair_t (c1, 0.)
        {
            assert (isReal());
        }

        ComplexPair(const complex_t& c1,
                    const complex_t& c2)
                : complex_pair_t (c1, c2)
        {
        }

        bool isConjugate() const
        {
            return second == std::conj(first);
        }

        bool isReal() const
        {
            return first.imag() == 0 && second.imag() == 0;
        }

        // Returns true if this is either a conjugate pair,
        // or a pair of reals where neither is zero.
        bool isMatchedPair () const
        {
            if (first.imag() != 0)
                return second == std::conj (first);
            else
                return second.imag () == 0 &&
                       second.real () != 0 &&
                       first.real () != 0;
        }

        bool is_nan () const
        {
            return Dsp::is_nan (first) || Dsp::is_nan (second);
        }
    };

// A pair of pole/zeros. This fits in a biquad (but is missing the gain)
    struct PoleZeroPair
    {
        ComplexPair poles;
        ComplexPair zeros;

        PoleZeroPair () { }

        // single pole/zero
        PoleZeroPair (const complex_t& p, const complex_t& z)
                : poles (p), zeros (z)
        {
        }

        // pole/zero pair
        PoleZeroPair (const complex_t& p1, const complex_t& z1,
                      const complex_t& p2, const complex_t& z2)
                : poles (p1, p2)
                , zeros (z1, z2)
        {
        }

        bool isSinglePole () const
        {
            return poles.second == 0. && zeros.second == 0.;
        }

        bool is_nan () const
        {
            return poles.is_nan() || zeros.is_nan();
        }
    };

// Identifies the general class of filter
    enum Kind
    {
        kindLowPass,
        kindHighPass,
        kindBandPass,
        kindBandStop,
        kindLowShelf,
        kindHighShelf,
        kindBandShelf,
        kindOther
    };

}

#endif //ANDROID_DSP_SERVICE_TYPES_H
