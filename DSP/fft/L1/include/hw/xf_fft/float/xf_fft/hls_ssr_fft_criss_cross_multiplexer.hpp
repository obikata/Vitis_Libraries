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

// Filename::hls_ssr_fft_criss_cross_multiplexer.hpp
#ifndef HLS_SSR_FFT_CRISS_CROSS_MULTIPLEXER_H_
#define HLS_SSR_FFT_CRISS_CROSS_MULTIPLEXER_H_

/*
 =========================================================================================
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 -_- The output data generated by SSR FFT is digit reversed in time direction and      -_-
 -_- also shuffled in SSR dimension. To re order the data it is required to buffer     -_-
 -_- data and do time reversal and also re-shuffle along SSR dimension to bring data   -_-
 -_- in original order. The data re ordering is done in 4 phases. CrissCroosMultiplex  -_-
 -_- reads in R memories and shuffles the output connections to output streams during  -_-
 -_- writing.  CissCrossMultiplexer generates R if else branches every branch  defines -_-
 -_- single cycle mapping of R memories to R output streams. The memory to stream      -_-
 -_- mapping changes every cycle.                                                      -_-
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 -_-                                                                                   -_-
 ========================================================================================
 */

#include "xf_fft/hls_ssr_fft_utilities.hpp"
#include "xf_fft/hls_ssr_fft_name_space_selector.hpp"

namespace xf {
namespace dsp {
namespace fft {

template <int t_stage>
struct CrissCrossMultiplexerClass {
    template <int t_R, int t_L, typename T_in, typename T_out>
    void crissCrossMultiplexer(int p_timeIndexAddressOffset,
                               int p_ssrDimensionAddressOffset,
                               T_in p_in[t_R][t_L / t_R],
                               T_out p_out[t_R]);
};

template <int t_stage>
template <int t_R, int t_L, typename T_in, typename T_out>
void CrissCrossMultiplexerClass<t_stage>::crissCrossMultiplexer(int p_timeIndexAddressOffset,
                                                                int p_ssrDimensionAddressOffset,
                                                                T_in p_in[t_R][t_L / t_R],
                                                                T_out p_out[t_R]) {
#pragma HLS INLINE

    if (p_ssrDimensionAddressOffset == (t_stage - 1)) {
    SSR_LOOP:
        for (unsigned int r = 0; r < t_R; r++) {
#pragma HLS UNROLL
            /*This expression is replaced below*/ // int pingPongSuperSampleIndex = ((t_stage-1) + r) % t_R;
            int pingPongSuperSampleIndex = ((t_stage - 1) + r) & ssrFFTLog2BitwiseAndModMask<t_R>::val;

            int pingPongTimeIndex = r + p_timeIndexAddressOffset;
            p_out[r] = p_in[pingPongSuperSampleIndex][pingPongTimeIndex];
        }
    } else {
        CrissCrossMultiplexerClass<t_stage - 1> obj;
        obj.template crissCrossMultiplexer<t_R, t_L, T_in, T_out>(p_timeIndexAddressOffset, p_ssrDimensionAddressOffset,
                                                                  p_in, p_out);
    }
}

template <>
template <int t_R, int t_L, typename T_in, typename T_out>
void CrissCrossMultiplexerClass<1>::crissCrossMultiplexer(int p_timeIndexAddressOffset,
                                                          int p_ssrDimensionAddressOffset,
                                                          T_in p_in[t_R][t_L / t_R],
                                                          T_out p_out[t_R]) {
#pragma HLS INLINE

SSR_LOOP:
    for (unsigned int r = 0; r < t_R; r++) {
#pragma HLS UNROLL
        int pingPongSuperSampleIndex = ((1 - 1) + r) & ssrFFTLog2BitwiseAndModMask<t_R>::val;
        int pingPongTimeIndex = r + p_timeIndexAddressOffset;
        p_out[r] = p_in[pingPongSuperSampleIndex][pingPongTimeIndex];
    }
}

} // end namespace fft
} // end namespace dsp
} // end namespace xf

#endif // HLS_SSR_FFT_CRISS_CROSS_MULTIPLEXER_H_
