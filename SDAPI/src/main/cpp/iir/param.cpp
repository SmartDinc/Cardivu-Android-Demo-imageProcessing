#include "common.h"
#include "design.h"

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace Dsp {

    ParamInfo::ParamInfo ()
    {
        throw std::logic_error ("invalid usage of ParamInfo");
    }

    double ParamInfo::clamp (double nativeValue) const
    {
        const double minValue = toNativeValue (0);
        const double maxValue = toNativeValue (1);
        if (nativeValue < minValue)
            nativeValue = minValue;
        else if (nativeValue > maxValue)
            nativeValue = maxValue;
        return nativeValue;
    }

//------------------------------------------------------------------------------

    double ParamInfo::Int_toControlValue (double nativeValue) const
    {
        return (nativeValue - m_arg1) / (m_arg2 - m_arg1);
    }

    double ParamInfo::Int_toNativeValue (double controlValue) const
    {
        return std::floor (m_arg1 + controlValue * (m_arg2 - m_arg1) + 0.5);
    }

//------------------------------------------------------------------------------

    double ParamInfo::Real_toControlValue (double nativeValue) const
    {
        return (nativeValue - m_arg1) / (m_arg2 - m_arg1);
    }

    double ParamInfo::Real_toNativeValue (double controlValue) const
    {
        return m_arg1 + controlValue * (m_arg2 - m_arg1);
    }

//------------------------------------------------------------------------------

    double ParamInfo::Log_toControlValue (double nativeValue) const
    {
        const double base = 1.5;
        double l0 = log (m_arg1) / log (base);
        double l1 = log (m_arg2) / log (base);
        return (log (nativeValue) / log(base) - l0) / (l1 - l0);
    }

    double ParamInfo::Log_toNativeValue (double controlValue) const
    {
        const double base = 1.5;
        double l0 = log (m_arg1) / log (base);
        double l1 = log (m_arg2) / log (base);
        return pow (base, l0 + controlValue * (l1 - l0));
    }

//------------------------------------------------------------------------------

    double ParamInfo::Pow2_toControlValue (double nativeValue) const
    {
        return ((log (nativeValue) / log (2.)) - m_arg1) / (m_arg2 - m_arg1);
    }

    double ParamInfo::Pow2_toNativeValue (double controlValue) const
    {
        return pow (2., (controlValue * (m_arg2 - m_arg1)) + m_arg1);
    }

//------------------------------------------------------------------------------

    std::string ParamInfo::Int_toString (double nativeValue) const
    {
        std::ostringstream os;
        os << int (nativeValue);
        return os.str();
    }

    std::string ParamInfo::Hz_toString (double nativeValue) const
    {
        std::ostringstream os;
        os << int (nativeValue) << " Hz";
        return os.str();
    }

    std::string ParamInfo::Real_toString (double nativeValue) const
    {
        std::ostringstream os;
        os << std::fixed << std::setprecision(3) << nativeValue;
        return os.str();
    }

    std::string ParamInfo::Db_toString (double nativeValue) const
    {
        const double af = fabs (nativeValue);
        int prec;
        if (af < 1)
            prec = 3;
        else if (af < 10)
            prec = 2;
        else
            prec = 1;
        std::ostringstream os;
        os << std::fixed << std::setprecision (prec) << nativeValue << " dB";
        return os.str();
    }

//------------------------------------------------------------------------------

    ParamInfo ParamInfo::defaultSampleRateParam ()
    {
        return ParamInfo (idSampleRate, "Fs", "Sample Rate",
                          11025, 192000, 44100,
                          &ParamInfo::Real_toControlValue,
                          &ParamInfo::Real_toNativeValue,
                          &ParamInfo::Hz_toString);
    }

    ParamInfo ParamInfo::defaultCutoffFrequencyParam ()
    {
        return ParamInfo (idFrequency, "Fc", "Cutoff Frequency",
                          10, 22040, 2000,
                          &ParamInfo::Log_toControlValue,
                          &ParamInfo::Log_toNativeValue,
                          &ParamInfo::Hz_toString);
    }

    ParamInfo ParamInfo::defaultCenterFrequencyParam ()
    {
        return ParamInfo (idFrequency, "Fc", "Center Frequency",
                          10, 22040, 2000,
                          &ParamInfo::Log_toControlValue,
                          &ParamInfo::Log_toNativeValue,
                          &ParamInfo::Hz_toString);
    }

    ParamInfo ParamInfo::defaultQParam ()
    {
        return ParamInfo (idQ, "Q", "Resonance",
                          -4, 4, 1,
                          &ParamInfo::Pow2_toControlValue,
                          &ParamInfo::Pow2_toNativeValue,
                          &ParamInfo::Real_toString);
    }

    ParamInfo ParamInfo::defaultBandwidthParam ()
    {
        return ParamInfo (idBandwidth, "BW", "Bandwidth (Octaves)",
                          -4, 4, 1,
                          &ParamInfo::Pow2_toControlValue,
                          &ParamInfo::Pow2_toNativeValue,
                          &ParamInfo::Real_toString);
    }

    ParamInfo ParamInfo::defaultBandwidthHzParam ()
    {
        return ParamInfo (idBandwidthHz, "BW", "Bandwidth (Hz)",
                          10, 22040, 1720,
                          &ParamInfo::Log_toControlValue,
                          &ParamInfo::Log_toNativeValue,
                          &ParamInfo::Hz_toString);
    }

    ParamInfo ParamInfo::defaultGainParam ()
    {
        return ParamInfo (idGain, "Gain", "Gain",
                          -24, 24, -6,
                          &ParamInfo::Real_toControlValue,
                          &ParamInfo::Real_toNativeValue,
                          &ParamInfo::Db_toString);
    }

    ParamInfo ParamInfo::defaultSlopeParam ()
    {
        return ParamInfo (idSlope, "Slope", "Slope",
                          -2, 2, 1,
                          &ParamInfo::Pow2_toControlValue,
                          &ParamInfo::Pow2_toNativeValue,
                          &ParamInfo::Real_toString);
    }

    ParamInfo ParamInfo::defaultRippleDbParam ()
    {
        return ParamInfo (idRippleDb, "Ripple", "Ripple dB",
                          0.001, 12, 0.01,
                          &ParamInfo::Real_toControlValue,
                          &ParamInfo::Real_toNativeValue,
                          &ParamInfo::Db_toString);
    }

    ParamInfo ParamInfo::defaultStopDbParam ()
    {
        return ParamInfo (idStopDb, "Stop", "Stopband dB",
                          3, 60, 48,
                          &ParamInfo::Real_toControlValue,
                          &ParamInfo::Real_toNativeValue,
                          &ParamInfo::Db_toString);
    }

    ParamInfo ParamInfo::defaultRolloffParam ()
    {
        return ParamInfo (idRolloff, "W", "Transition Width",
                          -16, 4, 0,
                          &ParamInfo::Real_toControlValue,
                          &ParamInfo::Real_toNativeValue,
                          &ParamInfo::Real_toString);
    }

    ParamInfo ParamInfo::defaultPoleRhoParam ()
    {
        return ParamInfo (idPoleRho, "Pd", "Pole Distance",
                          0, 1, 0.5,
                          &ParamInfo::Real_toControlValue,
                          &ParamInfo::Real_toNativeValue,
                          &ParamInfo::Real_toString);
    }

    ParamInfo ParamInfo::defaultPoleThetaParam ()
    {
        return ParamInfo (idPoleTheta, "Pa", "Pole Angle",
                          0, double_pi, double_pi/2,
                          &ParamInfo::Real_toControlValue,
                          &ParamInfo::Real_toNativeValue,
                          &ParamInfo::Real_toString);
    }

    ParamInfo ParamInfo::defaultZeroRhoParam ()
    {
        return ParamInfo (idZeroRho, "Pd", "Zero Distance",
                          0, 1, 0.5,
                          &ParamInfo::Real_toControlValue,
                          &ParamInfo::Real_toNativeValue,
                          &ParamInfo::Real_toString);
    }

    ParamInfo ParamInfo::defaultZeroThetaParam ()
    {
        return ParamInfo (idZeroTheta, "Pa", "Zero Angle",
                          0, double_pi, double_pi/2,
                          &ParamInfo::Real_toControlValue,
                          &ParamInfo::Real_toNativeValue,
                          &ParamInfo::Real_toString);
    }

    ParamInfo ParamInfo::defaultPoleRealParam ()
    {
        return ParamInfo (idPoleReal, "A1", "Pole Real",
                          -1, 1, 0.25,
                          &ParamInfo::Real_toControlValue,
                          &ParamInfo::Real_toNativeValue,
                          &ParamInfo::Real_toString);
    }

    ParamInfo ParamInfo::defaultZeroRealParam ()
    {
        return ParamInfo (idZeroReal, "B1", "Zero Real",
                          -1, 1, -0.25,
                          &ParamInfo::Real_toControlValue,
                          &ParamInfo::Real_toNativeValue,
                          &ParamInfo::Real_toString);
    }

}