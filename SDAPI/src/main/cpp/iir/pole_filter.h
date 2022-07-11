#ifndef ANDROID_DSP_SERVICE_POLE_FILTER_H
#define ANDROID_DSP_SERVICE_POLE_FILTER_H

#include "common.h"
#include "math_supplement.h"
#include "cascade.h"

namespace Dsp {

/*
 * Base for filters designed via algorithmic placement of poles and zeros.
 *
 * Typically, the filter is first designed as a half-band low pass or
 * low shelf analog filter (s-plane). Then, using a transformation such
 * as the ones from Constantinides, the poles and zeros of the analog filter
 * are calculated in the z-plane.
 *
 */

// Factored implementations to reduce template instantiations

    class PoleFilterBase2 : public Cascade
    {
    public:
        // This gets the poles/zeros directly from the digital
        // prototype. It is used to double check the correctness
        // of the recovery of pole/zeros from biquad coefficients.
        //
        // It can also be used to accelerate the interpolation
        // of pole/zeros for parameter modulation, since a pole
        // filter already has them calculated

#if 1
        // Commenting this out will pass the call to the Cascade,
        // which tries to compute the poles and zeros from the biquad
        // coefficients.
        std::vector<PoleZeroPair> getPoleZeros () const
        {
            std::vector<PoleZeroPair> vpz;
            const int pairs = (m_digitalProto.getNumPoles () + 1) / 2;
            for (int i = 0; i < pairs; ++i)
                vpz.push_back (m_digitalProto[i]);
            return vpz;
        }
#endif

    protected:
        LayoutBase m_digitalProto;
    };

// Serves a container to hold the analog prototype
// and the digital pole/zero layout.
    template <class AnalogPrototype>
    class PoleFilterBase : public PoleFilterBase2
    {
    protected:
        void setPrototypeStorage (const LayoutBase& analogStorage,
                                  const LayoutBase& digitalStorage)
        {
            m_analogProto.setStorage (analogStorage);
            m_digitalProto = digitalStorage;
        }

    protected:
        AnalogPrototype m_analogProto;
    };

//------------------------------------------------------------------------------

// Storage for pole filters
    template <class BaseClass,
            int MaxAnalogPoles,
            int MaxDigitalPoles = MaxAnalogPoles>
    struct PoleFilter : BaseClass
            , CascadeStages <(MaxDigitalPoles + 1) / 2>
    {
        PoleFilter ()
        {
            // This glues together the factored base classes
            // with the templatized storage classes.
            BaseClass::setCascadeStorage (this->getCascadeStorage());
            BaseClass::setPrototypeStorage (m_analogStorage, m_digitalStorage);
        }

    private:
        Layout <MaxAnalogPoles> m_analogStorage;
        Layout <MaxDigitalPoles> m_digitalStorage;
    };

//------------------------------------------------------------------------------

/*
 * s-plane to z-plane transforms
 *
 * For pole filters, an analog prototype is created via placement of
 * poles and zeros in the s-plane. The analog prototype is either
 * a halfband low pass or a halfband low shelf. The poles, zeros,
 * and normalization parameters are transformed into the z-plane
 * using variants of the bilinear transformation.
 *
 */

// low pass to low pass
    class LowPassTransform
    {
    public:
        LowPassTransform (double fc,
                          LayoutBase& digital,
                          LayoutBase const& analog);

    private:
        complex_t transform (complex_t c);

        double f;
    };

//------------------------------------------------------------------------------

// low pass to high pass
    class HighPassTransform
    {
    public:
        HighPassTransform (double fc,
                           LayoutBase& digital,
                           LayoutBase const& analog);

    private:
        complex_t transform (complex_t c);

        double f;
    };

//------------------------------------------------------------------------------

// low pass to band pass transform
    class BandPassTransform
    {

    public:
        BandPassTransform (double fc,
                           double fw,
                           LayoutBase& digital,
                           LayoutBase const& analog);

    private:
        ComplexPair transform (complex_t c);

        double wc;
        double wc2;
        double a;
        double b;
        double a2;
        double b2;
        double ab;
        double ab_2;
    };

//------------------------------------------------------------------------------

// low pass to band stop transform
    class BandStopTransform
    {
    public:
        BandStopTransform (double fc,
                           double fw,
                           LayoutBase& digital,
                           LayoutBase const& analog);

    private:
        ComplexPair transform (complex_t c);

        double wc;
        double wc2;
        double a;
        double b;
        double a2;
        double b2;

    };
}

#endif //ANDROID_DSP_SERVICE_POLE_FILTER_H
