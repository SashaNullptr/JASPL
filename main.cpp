
//C System-Headers
#include <math.h>
#include <stdio.h>
//C++ System headers
#include <iostream>
#include <chrono>
#include <unistd.h>
//OpenCL Headers
//
//Boost Headers
//
//Qt Headers
#include <QApplication>
//Project specific headers
#include "jVector/jvector.h"
#include "jFFT/jfft.h"
#include "jFFT/jfft_unit_test.h"
#include "jPlot/jplot.h"
#include "jFilter/jlinearfilter.h"
#include "jTypeTraits/jtypetraits.h"
#include "jFilter/jfilter_unit_test.h"
#include "OpenCLBase/openclbase.h"
#include "TaskQueue/taskqueue.h"
#include "TaskItems/LinearConvolution/linearconvolution.h"
#include "jChart/jchart.h"
#include "TaskItems/FFT/fft.h"
#include "TaskItems/PowerSpectrum/powerspectrum.h"
#include "TaskItems/Arithmetic/ScalarAdd/scalaradd.h"
#include "TaskItems/Arithmetic/ScalarMultiply/scalarmultiply.h"

#define TEST_POINTS 1e5
#define TEST_TYPE float

/*! \mainpage J.A.S.P.L. (Just Another Signal Processing Library)
 *
 * \section intro_sec Introduction
 *
 * JASPL is designed to perform signal processing operations on 1 dimensional objects,
 *  with an emphasis on GPGPU acceleration.
 *
 * \section Base Dependencies
 *      \li Boost libraries >= 1.62
 *      \li gnuplot-iostream
 *      \li fftw >= 3.3.5
 *
 * \section GPU-acceleration Dependencies
 *      \li OpenCL installation (e.g. Nvidia CUDA, AMD-APP, Intel)
 *      \li clFFT
 *
 * \section Special Considerations if compling from source
 *      \li OpenMP Installation and compilier that supports OpenMP pragmas
 *      \li C++11 Compliant Compilier ( GCC >= 4.8.2 )
 *
 */

int main(int argc, char *argv[]) {

//  QApplication a(argc, argv);

    uint N = TEST_POINTS;

    std::vector< TEST_TYPE > sin_vect;
    sin_vect.reserve( N );

    for ( uint i = 0; i < N ; i++ ) {

//        sin_vect.push_back( sinf( (N/8)*i *2*M_PI/N) + 2*sinf( (N/4)*i*2*M_PI/N) + 3*sinf( (3*N/8)*i*2*M_PI/N ) );
        uint M = 500;
        sin_vect.push_back( sinf( (M)*i *2*M_PI/N) + 2*sinf( (2*M)*i*2*M_PI/N) + 3*sinf( (3*M)*i*2*M_PI/N ) );
    }

//    jaspl::plot( sin_vect );

    auto test_q = jaspl::ocl::TaskQueue< std::vector< TEST_TYPE > > ( 0 );
    test_q.Load( sin_vect );

    TEST_TYPE fact = static_cast< TEST_TYPE >( 1.0f / 100.0f );

    std::vector< TEST_TYPE > box_vec( 100, fact );
    auto conv_task = jaspl::ocl::LinearConvolution< std::vector< TEST_TYPE > >( box_vec );
    test_q.AddTaskItem( conv_task );

    auto fft = jaspl::ocl::PowerSpectrum< std::vector< TEST_TYPE > >();
    test_q.AddTaskItem( fft );

    auto mult_task = jaspl::ocl::ScalarMultiply< std::vector< TEST_TYPE > >( 2.0f );
    test_q.AddTaskItem( mult_task );

    auto addition_task = jaspl::ocl::ScalarAdd< std::vector< TEST_TYPE > >( 5.0f );
    test_q.AddTaskItem( addition_task );

    test_q.Execute();

    test_q.PrintContents();

    std::vector< TEST_TYPE > processed = test_q.Recall();

    jaspl::plot( processed );

//  return a.exec();

}
