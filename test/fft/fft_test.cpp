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

#include "test_common.hpp"

#include <cxxporthelper/aligned_memory.hpp>

#include <cxxdasp/utils/utils.hpp>
#include <cxxdasp/fft/fft.hpp>

using namespace cxxdasp;

// #undef CXXDASP_USE_FFT_BACKEND_PFFFT
// #undef CXXDASP_USE_FFT_BACKEND_FFTS
// #undef CXXDASP_USE_FFT_BACKEND_KISS_FFT
// #undef CXXDASP_USE_FFT_BACKEND_FFTWF
// #undef CXXDASP_USE_FFT_BACKEND_NE10
// #undef CXXDASP_USE_FFT_BACKEND_FFTW
// #undef CXXDASP_USE_FFT_BACKEND_GP_FFT

template <typename TFFTBackend, typename T>
class ForwardFFTTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef typename TFFTBackend::forward fft_backend_type;
    typedef T data_type;

    fft::fft<std::complex<T>, std::complex<T>, fft_backend_type> fft_;
    cxxporthelper::aligned_memory<std::complex<T>> in_;
    cxxporthelper::aligned_memory<std::complex<T>> out_;
};

template <typename TFFTBackend, typename T>
class BackwardFFTTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef typename TFFTBackend::backward fft_backend_type;
    typedef T data_type;

    fft::fft<std::complex<T>, std::complex<T>, fft_backend_type> fft_;
    cxxporthelper::aligned_memory<std::complex<T>> in_;
    cxxporthelper::aligned_memory<std::complex<T>> out_;
};

template <typename TFFTBackend, typename T>
class ForwardRealFFTTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef typename TFFTBackend::forward_real fft_backend_type;
    typedef T data_type;

    fft::fft<T, std::complex<T>, fft_backend_type> fft_;
    cxxporthelper::aligned_memory<T> in_;
    cxxporthelper::aligned_memory<std::complex<T>> out_;
};

template <typename TFFTBackend, typename T>
class BackwardRealFFTTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef typename TFFTBackend::backward_real fft_backend_type;
    typedef T data_type;

    fft::fft<std::complex<T>, T, fft_backend_type> fft_;
    cxxporthelper::aligned_memory<std::complex<T>> in_;
    cxxporthelper::aligned_memory<T> out_;
};

template <typename TForwardFFTTest>
void do_forward_fft_test(TForwardFFTTest *thiz)
{
    typedef typename TForwardFFTTest::data_type data_type;
    typedef std::complex<data_type> complex_type;

    const int n = 16;
    // setup
    thiz->in_.allocate(n);
    thiz->out_.allocate(n);
    thiz->fft_.setup(n, &(thiz->in_[0]), &(thiz->out_[0]));

    std::vector<complex_type> expected;
    std::vector<complex_type> in_data;

    // in_data
    for (int i = 1; i <= n; ++i) {
        in_data.push_back(std::complex<data_type>(static_cast<data_type>(i), static_cast<data_type>(i)));
    }

    // expected
    ASSERT_EQ(1, thiz->fft_.scale());
#define COMPLEX_VALUE(real, imag) complex_type(static_cast<data_type>(real), static_cast<data_type>(imag))
    expected.push_back(COMPLEX_VALUE(136.0, 136.0));
    expected.push_back(COMPLEX_VALUE(-48.218715937006785, 32.218715937006785));
    expected.push_back(COMPLEX_VALUE(-27.31370849898476, 11.313708498984761));
    expected.push_back(COMPLEX_VALUE(-19.97284610132391, 3.9728461013239116));
    expected.push_back(COMPLEX_VALUE(-16.0, 0.0));
    expected.push_back(COMPLEX_VALUE(-13.34542910335439, -2.6545708966456107));
    expected.push_back(COMPLEX_VALUE(-11.313708498984761, -4.686291501015239));
    expected.push_back(COMPLEX_VALUE(-9.591298939037264, -6.408701060962734));
    expected.push_back(COMPLEX_VALUE(-8.0, -8.0));
    expected.push_back(COMPLEX_VALUE(-6.408701060962734, -9.591298939037264));
    expected.push_back(COMPLEX_VALUE(-4.686291501015239, -11.313708498984761));
    expected.push_back(COMPLEX_VALUE(-2.6545708966456107, -13.34542910335439));
    expected.push_back(COMPLEX_VALUE(0.0, -16.0));
    expected.push_back(COMPLEX_VALUE(3.9728461013239116, -19.97284610132391));
    expected.push_back(COMPLEX_VALUE(11.313708498984761, -27.31370849898476));
    expected.push_back(COMPLEX_VALUE(32.218715937006785, -48.218715937006785));
#undef COMPLEX_VALUE

    // perform FFT
    ::memcpy(&(thiz->in_[0]), &in_data[0], sizeof(complex_type) * n);
    thiz->fft_.execute();

    //
    // check
    //

    // input data array have to be preserved
    ASSERT_EQ(0, ::memcmp(&(thiz->in_[0]), &in_data[0], sizeof(complex_type) * n));

    // for (int i = 0; i < n; ++i) {
    //     std::cout << i << " : " << expected[i] << " - " << thiz->out_[i] << std::endl;
    // }

    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_COMPLEX_NEAR(expected[i], thiz->out_[i], static_cast<data_type>(1e-5));
        // ASSERT_AUTO_COMPLEX_EQ(expected[i], thiz->out_[i]);
    }
}

template <typename TBackwardFFTTest>
void do_backward_fft_test(TBackwardFFTTest *thiz)
{
    typedef typename TBackwardFFTTest::fft_backend_type fft_backend_type;
    typedef typename TBackwardFFTTest::data_type data_type;
    typedef std::complex<data_type> complex_type;

    const int n = 16;
    // setup
    thiz->in_.allocate(n);
    thiz->out_.allocate(n);
    thiz->fft_.setup(n, &(thiz->in_[0]), &(thiz->out_[0]));

    std::vector<complex_type> expected;
    std::vector<complex_type> in_data;

// in_data
#define COMPLEX_VALUE(real, imag) complex_type(static_cast<data_type>(real), static_cast<data_type>(imag))
    in_data.push_back(COMPLEX_VALUE(136.0, 136.0));
    in_data.push_back(COMPLEX_VALUE(-48.218715937006785, 32.218715937006785));
    in_data.push_back(COMPLEX_VALUE(-27.31370849898476, 11.313708498984761));
    in_data.push_back(COMPLEX_VALUE(-19.97284610132391, 3.9728461013239116));
    in_data.push_back(COMPLEX_VALUE(-16.0, 0.0));
    in_data.push_back(COMPLEX_VALUE(-13.34542910335439, -2.6545708966456107));
    in_data.push_back(COMPLEX_VALUE(-11.313708498984761, -4.686291501015239));
    in_data.push_back(COMPLEX_VALUE(-9.591298939037264, -6.408701060962734));
    in_data.push_back(COMPLEX_VALUE(-8.0, -8.0));
    in_data.push_back(COMPLEX_VALUE(-6.408701060962734, -9.591298939037264));
    in_data.push_back(COMPLEX_VALUE(-4.686291501015239, -11.313708498984761));
    in_data.push_back(COMPLEX_VALUE(-2.6545708966456107, -13.34542910335439));
    in_data.push_back(COMPLEX_VALUE(0.0, -16.0));
    in_data.push_back(COMPLEX_VALUE(3.9728461013239116, -19.97284610132391));
    in_data.push_back(COMPLEX_VALUE(11.313708498984761, -27.31370849898476));
    in_data.push_back(COMPLEX_VALUE(32.218715937006785, -48.218715937006785));
#undef COMPLEX_VALUE

    // expected
    const int scale = thiz->fft_.scale();
    for (int i = 1; i <= n; ++i) {
        expected.push_back(complex_type(static_cast<data_type>(i * scale), static_cast<data_type>(i * scale)));
    }

    // perform FFT
    ::memcpy(&(thiz->in_[0]), &in_data[0], sizeof(complex_type) * n);
    thiz->fft_.execute();

    //
    // check
    //

    // input data array have to be preserved
    ASSERT_EQ(0, ::memcmp(&(thiz->in_[0]), &in_data[0], sizeof(complex_type) * n));

    // for (int i = 0; i < n; ++i) {
    //     std::cout << i << " : " << expected[i] << " - " << thiz->out_[i] << std::endl;
    // }

    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_COMPLEX_EQ(expected[i], thiz->out_[i]);
    }
}

template <typename TForwardFFTTest>
void do_forward_real_fft_test(TForwardFFTTest *thiz)
{
    typedef typename TForwardFFTTest::data_type data_type;
    typedef std::complex<data_type> complex_type;

    const int n = 32;
    const int n2 = utils::forward_fft_real_num_outputs(n);

    ASSERT_EQ((32 / 2 + 1), n2);

    // setup
    thiz->in_.allocate(n);
    thiz->out_.allocate(n2);
    thiz->fft_.setup(n, &(thiz->in_[0]), &(thiz->out_[0]));

    std::vector<complex_type> expected;
    std::vector<data_type> in_data;

    // in_data
    for (int i = 1; i <= n; ++i) {
        in_data.push_back(static_cast<data_type>(i));
    }

    // expected
    ASSERT_EQ(1, thiz->fft_.scale());
#define COMPLEX_VALUE(real, imag) complex_type(static_cast<data_type>(real), static_cast<data_type>(imag))
    expected.push_back(COMPLEX_VALUE(528.0, 0.0));
    expected.push_back(COMPLEX_VALUE(-16.0, 162.45072620174176));
    expected.push_back(COMPLEX_VALUE(-16.0, 80.43743187401357));
    expected.push_back(COMPLEX_VALUE(-16.0, 52.74493134301312));
    expected.push_back(COMPLEX_VALUE(-16.0, 38.62741699796952));
    expected.push_back(COMPLEX_VALUE(-16.0, 29.933894588630228));
    expected.push_back(COMPLEX_VALUE(-16.0, 23.945692202647823));
    expected.push_back(COMPLEX_VALUE(-16.0, 19.496056409407625));
    expected.push_back(COMPLEX_VALUE(-16.0, 16.0));
    expected.push_back(COMPLEX_VALUE(-16.0, 13.130860653258562));
    expected.push_back(COMPLEX_VALUE(-16.0, 10.690858206708779));
    expected.push_back(COMPLEX_VALUE(-16.0, 8.55217817521267));
    expected.push_back(COMPLEX_VALUE(-16.0, 6.627416997969522));
    expected.push_back(COMPLEX_VALUE(-16.0, 4.8535469377174785));
    expected.push_back(COMPLEX_VALUE(-16.0, 3.1825978780745316));
    expected.push_back(COMPLEX_VALUE(-16.0, 1.5758624537146346));
    expected.push_back(COMPLEX_VALUE(-16.0, 0.0));
#undef COMPLEX_VALUE

    // perform FFT
    ::memcpy(&(thiz->in_[0]), &in_data[0], sizeof(data_type) * n);
    thiz->fft_.execute();

    //
    // check
    //

    // input data array have to be preserved
    ASSERT_EQ(0, ::memcmp(&(thiz->in_[0]), &in_data[0], sizeof(data_type) * n));

    // for (int i = 0; i < n2; ++i) {
    //     std::cout << i << " : " << expected[i] << " - " << thiz->out_[i] << std::endl;
    // }

    for (int i = 0; i < n2; ++i) {
        ASSERT_AUTO_COMPLEX_NEAR(expected[i], thiz->out_[i], static_cast<data_type>(0.0001));
    }
}

template <typename TBackwardFFTTest>
void do_backward_real_fft_test(TBackwardFFTTest *thiz)
{
    typedef typename TBackwardFFTTest::fft_backend_type fft_backend_type;
    typedef typename TBackwardFFTTest::data_type data_type;
    typedef std::complex<data_type> complex_type;

    const int n = 32;
    const int n2 = utils::forward_fft_real_num_outputs(n);

    ASSERT_EQ((32 / 2 + 1), n2);

    // setup
    thiz->in_.allocate(n2);
    thiz->out_.allocate(n);
    thiz->fft_.setup(n, &(thiz->in_[0]), &(thiz->out_[0]));

    std::vector<data_type> expected;
    std::vector<complex_type> in_data;

// in_data
#define COMPLEX_VALUE(real, imag) complex_type(static_cast<data_type>(real), static_cast<data_type>(imag))
    in_data.push_back(COMPLEX_VALUE(528.0, 0.0));
    in_data.push_back(COMPLEX_VALUE(-16.0, 162.45072620174176));
    in_data.push_back(COMPLEX_VALUE(-16.0, 80.43743187401357));
    in_data.push_back(COMPLEX_VALUE(-16.0, 52.74493134301312));
    in_data.push_back(COMPLEX_VALUE(-16.0, 38.62741699796952));
    in_data.push_back(COMPLEX_VALUE(-16.0, 29.933894588630228));
    in_data.push_back(COMPLEX_VALUE(-16.0, 23.945692202647823));
    in_data.push_back(COMPLEX_VALUE(-16.0, 19.496056409407625));
    in_data.push_back(COMPLEX_VALUE(-16.0, 16.0));
    in_data.push_back(COMPLEX_VALUE(-16.0, 13.130860653258562));
    in_data.push_back(COMPLEX_VALUE(-16.0, 10.690858206708779));
    in_data.push_back(COMPLEX_VALUE(-16.0, 8.55217817521267));
    in_data.push_back(COMPLEX_VALUE(-16.0, 6.627416997969522));
    in_data.push_back(COMPLEX_VALUE(-16.0, 4.8535469377174785));
    in_data.push_back(COMPLEX_VALUE(-16.0, 3.1825978780745316));
    in_data.push_back(COMPLEX_VALUE(-16.0, 1.5758624537146346));
    in_data.push_back(COMPLEX_VALUE(-16.0, 0.0));
#undef COMPLEX_VALUE

    // expected
    const int scale = thiz->fft_.scale();
    for (int i = 1; i <= n2; ++i) {
        expected.push_back(static_cast<data_type>(i * scale));
    }

    // perform FFT
    ::memcpy(&(thiz->in_[0]), &in_data[0], sizeof(complex_type) * n2);
    thiz->fft_.execute();

    //
    // check
    //

    // input data array have to be preserved
    ASSERT_EQ(0, ::memcmp(&(thiz->in_[0]), &in_data[0], sizeof(complex_type) * n2));

    // for (int i = 0; i < n2; ++i) {
    //     std::cout << i << " : " << expected[i] << " - " << thiz->out_[i] << std::endl;
    // }

    for (int i = 0; i < n2; ++i) {
        ASSERT_AUTO_FLOATING_POINT_NEAR(expected[i], thiz->out_[i], static_cast<data_type>(0.0001));
    }
}

//
// PFFFT
//
#if CXXDASP_USE_FFT_BACKEND_PFFFT
typedef ForwardFFTTest<fft::backend::f::pffft, float> ForwardFFTTest_PFFFT_Float;
TEST_F(ForwardFFTTest_PFFFT_Float, forward) { do_forward_fft_test(this); }

typedef BackwardFFTTest<fft::backend::f::pffft, float> BackwardFFTTest_PFFFT_Float;
TEST_F(BackwardFFTTest_PFFFT_Float, backward) { do_backward_fft_test(this); }

typedef ForwardRealFFTTest<fft::backend::f::pffft, float> ForwardRealFFTTest_PFFFT_Float;
TEST_F(ForwardRealFFTTest_PFFFT_Float, forward_real) { do_forward_real_fft_test(this); }

typedef BackwardRealFFTTest<fft::backend::f::pffft, float> BackwardRealFFTTest_PFFFT_Float;
TEST_F(BackwardRealFFTTest_PFFFT_Float, backward_real) { do_backward_real_fft_test(this); }
#endif

//
// FFTS
//
#if CXXDASP_USE_FFT_BACKEND_FFTS
typedef ForwardFFTTest<fft::backend::f::ffts, float> ForwardFFTTest_FFTS_Float;
TEST_F(ForwardFFTTest_FFTS_Float, forward) { do_forward_fft_test(this); }

typedef BackwardFFTTest<fft::backend::f::ffts, float> BackwardFFTTest_FFTS_Float;
TEST_F(BackwardFFTTest_FFTS_Float, backward) { do_backward_fft_test(this); }

typedef ForwardRealFFTTest<fft::backend::f::ffts, float> ForwardRealFFTTest_FFTS_Float;
TEST_F(ForwardRealFFTTest_FFTS_Float, forward_real) { do_forward_real_fft_test(this); }

typedef BackwardRealFFTTest<fft::backend::f::ffts, float> BackwardRealFFTTest_FFTS_Float;
TEST_F(BackwardRealFFTTest_FFTS_Float, backward_real) { do_backward_real_fft_test(this); }
#endif

//
// Kiss FFT
//
#if CXXDASP_USE_FFT_BACKEND_KISS_FFT
typedef ForwardFFTTest<fft::backend::f::kiss_fft, float> ForwardFFTTest_KissFFT_Float;
TEST_F(ForwardFFTTest_KissFFT_Float, forward) { do_forward_fft_test(this); }

typedef BackwardFFTTest<fft::backend::f::kiss_fft, float> BackwardFFTTest_KissFFT_Float;
TEST_F(BackwardFFTTest_KissFFT_Float, backward) { do_backward_fft_test(this); }

typedef ForwardRealFFTTest<fft::backend::f::kiss_fft, float> ForwardRealFFTTest_KissFFT_Float;
TEST_F(ForwardRealFFTTest_KissFFT_Float, forward_real) { do_forward_real_fft_test(this); }

typedef BackwardRealFFTTest<fft::backend::f::kiss_fft, float> BackwardRealFFTTest_KissFFT_Float;
TEST_F(BackwardRealFFTTest_KissFFT_Float, backward_real) { do_backward_real_fft_test(this); }
#endif

//
// FFTWF
//
#if CXXDASP_USE_FFT_BACKEND_FFTWF
typedef ForwardFFTTest<fft::backend::f::fftw, float> ForwardFFTTest_FFTWF_Float;
TEST_F(ForwardFFTTest_FFTWF_Float, forward) { do_forward_fft_test(this); }

typedef BackwardFFTTest<fft::backend::f::fftw, float> BackwardFFTTest_FFTWF_Float;
TEST_F(BackwardFFTTest_FFTWF_Float, backward) { do_backward_fft_test(this); }

typedef ForwardRealFFTTest<fft::backend::f::fftw, float> ForwardRealFFTTest_FFTWF_Float;
TEST_F(ForwardRealFFTTest_FFTWF_Float, forward_real) { do_forward_real_fft_test(this); }

typedef BackwardRealFFTTest<fft::backend::f::fftw, float> BackwardRealFFTTest_FFTWF_Float;
TEST_F(BackwardRealFFTTest_FFTWF_Float, backward_real) { do_backward_real_fft_test(this); }
#endif

//
// Ne10
//
#if CXXDASP_USE_FFT_BACKEND_NE10
typedef ForwardFFTTest<fft::backend::f::ne10, float> ForwardFFTTest_Ne10_Float;
TEST_F(ForwardFFTTest_Ne10_Float, forward) { do_forward_fft_test(this); }

typedef BackwardFFTTest<fft::backend::f::ne10, float> BackwardFFTTest_Ne10_Float;
TEST_F(BackwardFFTTest_Ne10_Float, backward) { do_backward_fft_test(this); }

typedef ForwardRealFFTTest<fft::backend::f::ne10, float> ForwardRealFFTTest_Ne10_Float;
TEST_F(ForwardRealFFTTest_Ne10_Float, forward_real) { do_forward_real_fft_test(this); }

typedef BackwardRealFFTTest<fft::backend::f::ne10, float> BackwardRealFFTTest_Ne10_Float;
TEST_F(BackwardRealFFTTest_Ne10_Float, backward_real) { do_backward_real_fft_test(this); }
#endif

//
// FFTW
//
#if CXXDASP_USE_FFT_BACKEND_FFTW
typedef ForwardFFTTest<fft::backend::d::fftw, double> ForwardFFTTest_FFTW_Double;
TEST_F(ForwardFFTTest_FFTW_Double, forward) { do_forward_fft_test(this); }

typedef BackwardFFTTest<fft::backend::d::fftw, double> BackwardFFTTest_FFTW_Double;
TEST_F(BackwardFFTTest_FFTW_Double, backward) { do_backward_fft_test(this); }

typedef ForwardRealFFTTest<fft::backend::d::fftw, double> ForwardRealFFTTest_FFTW_Double;
TEST_F(ForwardRealFFTTest_FFTW_Double, forward_real) { do_forward_real_fft_test(this); }

typedef BackwardRealFFTTest<fft::backend::d::fftw, double> BackwardRealFFTTest_FFTW_Double;
TEST_F(BackwardRealFFTTest_FFTW_Double, backward_real) { do_backward_real_fft_test(this); }
#endif

//
// General purpose FFT package
//
#if CXXDASP_USE_FFT_BACKEND_GP_FFT
typedef ForwardFFTTest<fft::backend::d::gp_fft, double> ForwardFFTTest_GP_FFT_Double;
TEST_F(ForwardFFTTest_GP_FFT_Double, forward) { do_forward_fft_test(this); }

typedef BackwardFFTTest<fft::backend::d::gp_fft, double> BackwardFFTTest_GP_FFT_Double;
TEST_F(BackwardFFTTest_GP_FFT_Double, backward) { do_backward_fft_test(this); }

typedef ForwardRealFFTTest<fft::backend::d::gp_fft, double> ForwardRealFFTTest_GP_FFT_Double;
TEST_F(ForwardRealFFTTest_GP_FFT_Double, forward_real) { do_forward_real_fft_test(this); }

typedef BackwardRealFFTTest<fft::backend::d::gp_fft, double> BackwardRealFFTTest_GP_FFT_Double;
TEST_F(BackwardRealFFTTest_GP_FFT_Double, backward_real) { do_backward_real_fft_test(this); }
#endif
