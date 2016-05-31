//////////////////////////////////////////////////////////////////////////
// This file is part of openPSTD.                                       //
//                                                                      //
// openPSTD is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// openPSTD is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Date: 31-5-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif

#include <boost/test/unit_test.hpp>
#include <kernel/DG/Vandermonde.h>
#include <kernel/DG/Jacobi.h>

using namespace OpenPSTD::Kernel::DG;
using namespace std;
using namespace Eigen;

BOOST_AUTO_TEST_SUITE(Vandermonde)
    BOOST_AUTO_TEST_CASE(Vandermonde1D_10)
    {
        int N = 10;
        auto r = JacobiGL<double>(0, 0, N);
        auto result = Vandermonde1D(N, r);

        MatrixX<double> expected(N+1,N+1);
        expected <<
        0.707106781186547,  -1.224744871391589,   1.581138830084190,  -1.870828693386972,   2.121320343559644,  -2.345207879911717,   2.549509756796395,  -2.738612787525833,   2.915475947422653, -3.082207001484492,   3.240370349203935,
        0.707106781186547,  -1.143913461764679,   1.278410840057922,  -1.189769359259603,   0.918684889730180,  -0.514340055026662,   0.037345289289855,   0.445234040636364,  -0.867440186771424, 1.172461003612425,  -1.319725816371981,
        0.707106781186547,  -0.960792111060395,   0.669013802939367,  -0.056556459349990,  -0.585142911407216,   0.970185823457145,  -0.930849687546147,   0.485656540226566,   0.170919561604255, -0.753609342590742,   1.009939414963782,
        0.707106781186547,  -0.692269067867950,  -0.032830035665075,   0.741566610079666,  -0.798709549745601,   0.154709421619249,   0.625655045980613,  -0.860330934437440,   0.344646519622632, 0.471673849769253,  -0.877294382862032,
        0.707106781186547,  -0.362228259732442,  -0.583109276735000,   0.708969554995619,   0.170664567884623,  -0.811437143018306,   0.307381042200370,   0.630833052850318,  -0.679984208232962, -0.229492070445874,   0.815762676406661,
        0.707106781186547,   0.000000000000000,  -0.790569415042095,  -0.000000000000001,   0.795495128834866,   0.000000000000001,  -0.796721798998873,  -0.000000000000001,   0.797200454373381, 0.000000000000002,  -0.797434890624405,
        0.707106781186547,   0.362228259732442,  -0.583109276735000,  -0.708969554995619,   0.170664567884624,   0.811437143018306,   0.307381042200369,  -0.630833052850320,  -0.679984208232962, 0.229492070445876,   0.815762676406661,
        0.707106781186547,   0.692269067867950,  -0.032830035665075,  -0.741566610079666,  -0.798709549745601,  -0.154709421619249,   0.625655045980613,   0.860330934437440,   0.344646519622632, -0.471673849769253,  -0.877294382862032,
        0.707106781186547,   0.960792111060395,   0.669013802939367,   0.056556459349991,  -0.585142911407216,  -0.970185823457145,  -0.930849687546148,  -0.485656540226567,   0.170919561604254, 0.753609342590742,   1.009939414963783,
        0.707106781186547,   1.143913461764679,   1.278410840057922,   1.189769359259602,   0.918684889730179,   0.514340055026661,   0.037345289289854,  -0.445234040636366,  -0.867440186771426, -1.172461003612426,  -1.319725816371982,
        0.707106781186547,   1.224744871391589,   1.581138830084190,   1.870828693386972,   2.121320343559644,   2.345207879911717,   2.549509756796395,   2.738612787525833,   2.915475947422653, 3.082207001484492,   3.240370349203935;

        BOOST_CHECK(result.isApprox(expected));
    }

    BOOST_AUTO_TEST_CASE(Vandermonde1D_2)
    {
        int N = 2;
        auto r = JacobiGL<double>(0, 0, N);
        auto result = Vandermonde1D(N, r);

        MatrixX<double> expected(N+1,N+1);
        expected <<
            0.707106781186547,  -1.224744871391589,   1.581138830084190,
            0.707106781186547,                   0,  -0.790569415042095,
            0.707106781186547,   1.224744871391589,   1.581138830084190;

        BOOST_CHECK(result.isApprox(expected));
    }

    BOOST_AUTO_TEST_CASE(GradVandermonde1D_2)
    {
        int N = 2;
        auto r = JacobiGL<double>(0, 0, N);
        auto result = GradVandermonde1D(N, r);

        MatrixX<double> expected(N+1,N+1);
        expected <<
            0,   1.224744871391589,  -4.743416490252569,
            0,   1.224744871391589,                   0,
            0,   1.224744871391589,   4.743416490252569;

        BOOST_CHECK(result.isApprox(expected));
    }
BOOST_AUTO_TEST_SUITE_END()