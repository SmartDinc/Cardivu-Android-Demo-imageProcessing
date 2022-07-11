#include "common.h"
#include "filter.h"

namespace Dsp {

    Params Filter::getDefaultParams() const
    {
        Params params;

        params.clear();

        for (int i = 0; i < getNumParams(); ++i)
            params[i] = getParamInfo(i).getDefaultValue();

        return params;
    }

    Filter::~Filter()
    {
    }

    int Filter::findParamId (int paramId)
    {
        int index = -1;

        for (int i = getNumParams(); --i >= 0;)
        {
            if (getParamInfo (i).getId () == paramId)
            {
                index = i;
                break;
            }
        }

        return index;
    }

    void Filter::setParamById (int paramId, double nativeValue)
    {
        for (int i = getNumParams(); --i >= 0;)
        {
            if (getParamInfo (i).getId () == paramId)
            {
                setParam (i, nativeValue);
                return;
            }
        }

        assert (0);
    }

    void Filter::copyParamsFrom (Dsp::Filter const* other)
    {
        // first, set reasonable defaults
        m_params = getDefaultParams ();

        if (other)
        {
            // now loop
            for (int i = 0; i < getNumParams (); ++i)
            {
                const ParamInfo& paramInfo = getParamInfo (i);

                // find a match
                for (int j = 0; j < other->getNumParams(); ++j)
                {
                    const ParamInfo& otherParamInfo = other->getParamInfo (j);

                    if (paramInfo.getId() == otherParamInfo.getId())
                    {
                        // match!
                        m_params [i] = paramInfo.clamp (other->getParam (j));
                        break;
                    }
                }
            }
        }

        doSetParams (m_params);
    }

}