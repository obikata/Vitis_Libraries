/*
 * Copyright 2019 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// File Name : hls_ssr_fft_twiddle_table_traits.hpp
#ifndef HLS_SSR_FFT_TWIDDLE_TABLE_TRAITS_H_
#define HLS_SSR_FFT_TWIDDLE_TABLE_TRAITS_H_
/*
 =========================================================================================
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 -_-                                                                                   -_-

 This file defines traits for twiddle table and  exp table
 Twiddle table can take different forms depending on the
 stage where it is used also it has variations for FFTs
 when length is integer power of radix or not.

 TwiddleTableLENTraits <int t_L, int t_R> : defines
 twiddle table length for the case when t_L is
 integer power of t_R

 ComplexExpTableLENTraits< int t_outputForkingFactor,int t_L, int t_R>
 : defines exp table length where the t_outputForkingFactor
 parameters defines if the t_L is integer power of t_R and
 if not what is the ratio of t_L and next integer power of t_R

 ComplexExpTableLENTraits::EXP_TALBE_LENGTH_LAST_STAGE : defined the exp
 table length for last stage, for the case when t_L not integer
 power of t_R last stage has different length then other stages

 InputBasedTwiddleTraits<>:: Gives the type of twiddle and exp
 table when the input sample type is passed in template input


 -_-                                                                                   -_-
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 ========================================================================================
 */
//#include <complex>
#include <ap_fixed.h>

#include "xf_fft/hls_ssr_fft_utilities.hpp"
#include "xf_fft/fft_complex.hpp"

#ifndef HLS_SSR_FFT_MIN_SIN_COS_TABLE_LEN
#define HLS_SSR_FFT_MIN_SIN_COS_TABLE_LEN 16
#endif

namespace xf {
namespace dsp {
namespace fft {

template <int t_L, int t_R>
struct TwiddleTableLENTraits {
    // static const int TWIDDLE_TALBE_LENGTH = (t_R-1)*(t_L/t_R-1)+1;
    static const int TWIDDLE_TALBE_LENGTH = t_L / 4;
    static const int EXTENDED_TWIDDLE_TALBE_LENGTH = TWIDDLE_TALBE_LENGTH > HLS_SSR_FFT_MIN_SIN_COS_TABLE_LEN
                                                         ? TWIDDLE_TALBE_LENGTH
                                                         : HLS_SSR_FFT_MIN_SIN_COS_TABLE_LEN;
};

template <int t_outputForkingFactor, int t_L, int t_R>
struct ComplexExpTableLENTraits {
    static const int EXP_TALBE_LENGTH = t_R;
    static const int EXTENDED_EXP_TALBE_LENGTH =
        EXP_TALBE_LENGTH > HLS_SSR_FFT_MIN_SIN_COS_TABLE_LEN ? EXP_TALBE_LENGTH : HLS_SSR_FFT_MIN_SIN_COS_TABLE_LEN;

    static const int EXP_TALBE_LENGTH_LAST_STAGE = t_R / t_outputForkingFactor;
    static const int EXTENDED_EXP_TALBE_LENGTH_LAST_STAGE =
        EXP_TALBE_LENGTH_LAST_STAGE > HLS_SSR_FFT_MIN_SIN_COS_TABLE_LEN ? EXP_TALBE_LENGTH_LAST_STAGE
                                                                        : HLS_SSR_FFT_MIN_SIN_COS_TABLE_LEN;
};

template <int t_L, int t_R>
struct ComplexExpTableLENTraits<0, t_L, t_R> {
    static const int EXP_TALBE_LENGTH = t_R;
    static const int EXTENDED_EXP_TALBE_LENGTH =
        EXP_TALBE_LENGTH > HLS_SSR_FFT_MIN_SIN_COS_TABLE_LEN ? EXP_TALBE_LENGTH : HLS_SSR_FFT_MIN_SIN_COS_TABLE_LEN;
};

template <int t_L>
struct ComplexExpTableLENTraits<0, t_L, 2> {
    static const int EXP_TALBE_LENGTH =
        4; // Keep The minimum exp table length as 4 because for smaller length the HLS will not infer ROM
    static const int EXTENDED_EXP_TALBE_LENGTH =
        EXP_TALBE_LENGTH > HLS_SSR_FFT_MIN_SIN_COS_TABLE_LEN ? EXP_TALBE_LENGTH : HLS_SSR_FFT_MIN_SIN_COS_TABLE_LEN;
};

template <typename T>
struct TwiddleTraits {};

template <typename T>
struct TwiddleTraits<complex_wrapper<T> > {
    typedef complex_wrapper<T> T_twiddleType;
    typedef T T_twiddleInnerType;
};

template <typename t_paramStruct, typename T>
struct InputBasedTwiddleTraits {};
template <typename t_paramStruct>
struct InputBasedTwiddleTraits<t_paramStruct, complex_wrapper<float> > {
    // typedef complex_wrapper<T> T_twiddleType;
    // typedef complex_wrapper<T> T_expTabType;
    typedef float T_twiddleTableInnerType;
    typedef T_twiddleTableInnerType T_expTableInnerType;
    typedef complex_wrapper<T_twiddleTableInnerType> T_twiddleType;
    typedef complex_wrapper<T_expTableInnerType> T_expTabType;
};
template <typename t_paramStruct>
struct InputBasedTwiddleTraits<t_paramStruct, complex_wrapper<double> > {
    // typedef complex_wrapper<T> T_twiddleType;
    // typedef complex_wrapper<T> T_expTabType;
    typedef double T_twiddleTableInnerType;
    typedef T_twiddleTableInnerType T_expTableInnerType;
    typedef complex_wrapper<T_twiddleTableInnerType> T_twiddleType;
    typedef complex_wrapper<T_expTableInnerType> T_expTabType;
};

template <typename t_paramStruct, typename T>
struct InputBasedTwiddleTraits<t_paramStruct, complex_wrapper<T> > {
    // typedef complex_wrapper<T> T_twiddleType;
    // typedef complex_wrapper<T> T_expTabType;
    typedef ap_fixed<t_paramStruct::twiddle_table_word_length, t_paramStruct::twiddle_table_intger_part_length>
        T_twiddleTableInnerType;
    typedef T_twiddleTableInnerType T_expTableInnerType;
    typedef complex_wrapper<T_twiddleTableInnerType> T_twiddleType;
    typedef complex_wrapper<T_expTableInnerType> T_expTabType;
};

template <typename T_twiddleType>
struct TwiddleTypeCastingTraits {};

template <>
struct TwiddleTypeCastingTraits<complex_wrapper<float> > {
    typedef complex_wrapper<float> T_roundingBasedCastType;
    typedef complex_wrapper<float> T_truncationBasedCastType;
};
template <>
struct TwiddleTypeCastingTraits<complex_wrapper<double> > {
    typedef complex_wrapper<double> T_truncationBasedCastType;
    typedef complex_wrapper<double> T_roundingBasedCastType;
};

template <int IL, int FL>
struct TwiddleTypeCastingTraits<complex_wrapper<ap_fixed<IL, FL> > > {
    typedef complex_wrapper<ap_fixed<IL, FL> > T_truncationBasedCastType;
    typedef complex_wrapper<ap_fixed<IL, FL, AP_RND, AP_SAT> > T_roundingBasedCastType;
};

template <int IL, int FL, ap_q_mode A_Q>
struct TwiddleTypeCastingTraits<complex_wrapper<ap_fixed<IL, FL, A_Q> > > {
    typedef complex_wrapper<ap_fixed<IL, FL> > T_truncationBasedCastType;
    typedef complex_wrapper<ap_fixed<IL, FL, AP_RND, AP_SAT> > T_roundingBasedCastType;
};

template <int IL, int FL, ap_q_mode A_Q, ap_o_mode A_OVF>
struct TwiddleTypeCastingTraits<complex_wrapper<ap_fixed<IL, FL, A_Q, A_OVF> > > {
    typedef complex_wrapper<ap_fixed<IL, FL> > T_truncationBasedCastType;
    typedef complex_wrapper<ap_fixed<IL, FL, AP_RND, AP_SAT> > T_roundingBasedCastType;
};
template <int IL, int FL, ap_q_mode A_Q, ap_o_mode A_OVF, int N>
struct TwiddleTypeCastingTraits<complex_wrapper<ap_fixed<IL, FL, A_Q, A_OVF, N> > > {
    typedef complex_wrapper<ap_fixed<IL, FL> > T_truncationBasedCastType;
    typedef complex_wrapper<ap_fixed<IL, FL, AP_RND, AP_SAT> > T_roundingBasedCastType;
};

} // end namespace fft
} // end namespace dsp
} // end namespace xf

#endif // HLS_SSR_FFT_TWIDDLE_TABLE_TRAITS_H_
