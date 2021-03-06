//
//    Copyright (C) 2014 Haruki Hasegawa
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//

#ifndef CXXDASP_CONVERTER_F32_TO_S16_STEREO_SSE_SAMPLE_FORMAT_CONVERTER_CORE_OPERATOR_HPP_
#define CXXDASP_CONVERTER_F32_TO_S16_STEREO_SSE_SAMPLE_FORMAT_CONVERTER_CORE_OPERATOR_HPP_

#include <cxxporthelper/compiler.hpp>

#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)

#include <cxxporthelper/cstdint>
#include <cxxporthelper/platform_info.hpp>

#include <cxxdasp/datatype/audio_frame.hpp>

namespace cxxdasp {
namespace converter {

/**
 * SSE optimized
 * Float32 (stereo) -> Signed integer 16 (stereo)
 */
class f32_to_s16_stereo_sse_sample_format_converter_core_operator {

    /// @cond INTERNAL_FIELD
    f32_to_s16_stereo_sse_sample_format_converter_core_operator(
        const f32_to_s16_stereo_sse_sample_format_converter_core_operator &) = delete;
    f32_to_s16_stereo_sse_sample_format_converter_core_operator &
    operator=(const f32_to_s16_stereo_sse_sample_format_converter_core_operator &) = delete;
    /// @endcond

public:
    /**
     * Source audio frame type.
     */
    typedef datatype::audio_frame<float, 2> src_frame_type;

    /**
     * Destination audio frame type.
     */
    typedef datatype::audio_frame<int16_t, 2> dest_frame_type;

    /**
     * Check this operator class is available.
     * @return whether the class is available
     */
    static bool is_supported() CXXPH_NOEXCEPT
    {
        return cxxporthelper::platform_info::support_sse() && cxxporthelper::platform_info::support_sse2();
    }

    /**
     * Constructor.
     */
    f32_to_s16_stereo_sse_sample_format_converter_core_operator() {}

    /**
     * Destructor.
     */
    ~f32_to_s16_stereo_sse_sample_format_converter_core_operator() {}

    /**
     * Perform sample format conversion.
     *
     * @param src [in] source buffer
     * @param dest [out] destination buffer
     * @param n [in] count of samples
     */
    void perform(const src_frame_type *CXXPH_RESTRICT src, dest_frame_type *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT;

private:
    /// @cond INTERNAL_FIELD
    static void perform_impl(const float *CXXPH_RESTRICT src, int16_t *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT;
    /// @endcond
};

inline void f32_to_s16_stereo_sse_sample_format_converter_core_operator::perform(
    const src_frame_type *CXXPH_RESTRICT src, dest_frame_type *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{
    perform_impl(reinterpret_cast<const float *>(src), reinterpret_cast<int16_t *>(dest), n);
}

} // namespace converter
} // namespace cxxdasp

#endif // (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
#endif // CXXDASP_CONVERTER_F32_TO_S16_STEREO_SSE_SAMPLE_FORMAT_CONVERTER_CORE_OPERATOR_HPP_
