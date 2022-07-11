#ifndef ANDROID_DSP_SERVICE_DESIGN_H
#define ANDROID_DSP_SERVICE_DESIGN_H

#include "common.h"
#include "params.h"

namespace Dsp {

    struct DesignBase
    {
        // Sampling rate is the first param for every Design filter
        static const ParamInfo getParamInfo_0 ()
        {
            return ParamInfo::defaultSampleRateParam ();
        }

        // These should never get called
        static const ParamInfo getParamInfo_1 () { return ParamInfo(); }
        static const ParamInfo getParamInfo_2 () { return ParamInfo(); }
        static const ParamInfo getParamInfo_3 () { return ParamInfo(); }
        static const ParamInfo getParamInfo_4 () { return ParamInfo(); }
        static const ParamInfo getParamInfo_5 () { return ParamInfo(); }
        static const ParamInfo getParamInfo_6 () { return ParamInfo(); }
        static const ParamInfo getParamInfo_7 () { return ParamInfo(); }
    };

}

#endif //ANDROID_DSP_SERVICE_DESIGN_H