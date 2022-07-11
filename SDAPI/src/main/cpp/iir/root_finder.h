#ifndef ANDROID_DSP_SERVICE_ROOT_FINDER_H
#define ANDROID_DSP_SERVICE_ROOT_FINDER_H

#include "common.h"
#include "types.h"

namespace Dsp {

//
// Finds the complex roots of the given polynomial with
// complex-valued coefficients using a numerical method.
//

    class RootFinderBase
    {
    public:
        struct Array
        {
            Array (int max, complex_t* values)
            // : m_max (max)
            // , m_values (values)
            {
            }

            //complex_t& operator[] (int index)
            //{
            //};
        };

        //
        // Find roots of polynomial f(x)=a[0]+a[1]*x+a[2]*x^2...+a[degree]*x^degree
        // The input coefficients are set using coef()[].
        // The solutions are placed in roots.
        //
        void solve (int degree,
                    bool polish = true,
                    bool doSort = true);

        // Evaluates the polynomial at x
        complex_t eval (int degree,
                        const complex_t& x);

        // Direct access to the input coefficient array of size degree+1.
        complex_t* coef()
        {
            return m_a;
        }

        // Direct access to the resulting roots array of size degree
        complex_t* root()
        {
            return m_root;
        }

        // sort the roots by descending imaginary part
        void sort (int degree);

    private:
        // Improves x as a root using Laguerre's method.
        // The input coefficient array has degree+1 elements.
        void laguerre (int degree,
                       complex_t a[],
                       complex_t& x,
                       int& its);

    protected:
        int m_maxdegree;
        complex_t* m_a;		// input coefficients (m_maxdegree+1 elements)
        complex_t* m_ad;	// copy of deflating coefficients
        complex_t* m_root; // array of roots (maxdegree elements)
    };

//------------------------------------------------------------------------------

    template<int maxdegree>
    struct RootFinder : RootFinderBase
    {
        RootFinder()
        {
            m_maxdegree = maxdegree;
            m_a  = m_a0;
            m_ad = m_ad0;
            m_root = m_r;
        }

    private:
        complex_t m_a0 [maxdegree+1];
        complex_t m_ad0[maxdegree+1];
        complex_t m_r  [maxdegree];
    };

}

#endif //ANDROID_DSP_SERVICE_ROOT_FINDER_H
