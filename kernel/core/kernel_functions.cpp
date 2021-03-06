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
// Date:
//      1-9-2015
//
// Authors:
//      Omar Richardson
//      Louis van Harten
//
//////////////////////////////////////////////////////////////////////////

#include "kernel_functions.h"
#include <iostream>
#include <fstream>

using namespace Eigen;
namespace OpenPSTD {
    namespace Kernel {
        RhoArray get_rho_array(const float rho1, const float rho_self, const float rho2) {
            float zn1 = rho1 / rho_self;
            float inv_zn1 = rho_self / rho1;
            float rlw1 = (zn1 - 1) / (zn1 + 1);
            float rlw2 = (inv_zn1 - 1) / (inv_zn1 + 1);
            float tlw1 = (2 * zn1) / (zn1 + 1);
            float tlw2 = (2 * inv_zn1) / (inv_zn1 + 1);

            float zn2 = rho2 / rho_self;
            float inv_zn2 = rho_self / rho2;
            float rrw1 = (zn2 - 1) / (zn2 + 1);
            float rrw2 = (inv_zn2 - 1) / (inv_zn2 + 1);
            float trw1 = (2 * zn2) / (zn2 + 1);
            float trw2 = (2 * inv_zn2) / (inv_zn2 + 1);
            RhoArray result = {};
            result.pressure = ArrayXXf(4, 2);
            result.velocity = ArrayXXf(4, 2);;
            result.pressure << rlw1, rlw2,
                               rrw1, rrw2,
                               tlw1, tlw2,
                               trw1, trw2;

            result.velocity << -rlw1, -rlw2,
                               -rrw1, -rrw2,
                               tlw2, tlw1,
                               trw2, trw1;
            return result;
        }

        int next_2_power(float n) {
            return std::max((int) pow(2, ceil(log2(n))), 1);
        }

        int next_2_power(int n) {
            return std::max((int) pow(2, ceil(log2(n))), 1);
        }

        float get_grid_spacing(PSTDSettings cnf) {
            Array<float, 9, 1> dxv;
            dxv <<
            0.002, 0.005, 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1; //TODO: is there a good reason to disallow other vals?
            float waveLength = cnf.GetSoundSpeed() / cnf.GetMaxFrequency() / 2;
            if (waveLength < 0.002) {
                throw std::invalid_argument("Wavelength (speed/frequency) is too small");
            }
            for (int i = 0; i < dxv.size(); i++) {
                if (dxv(i) >= waveLength) {
                    return dxv(i - 1);
                }
            }
            return dxv(dxv.size() - 1);
        }

        Direction get_opposite(Direction direction) {
            switch (direction) {
                case Direction::TOP:
                    return Direction::BOTTOM;
                case Direction::BOTTOM:
                    return Direction::TOP;
                case Direction::LEFT:
                    return Direction::RIGHT;
                case Direction::RIGHT:
                    return Direction::LEFT;
            }
        }

        CalcDirection get_orthogonal(CalcDirection direction) {
            switch (direction) {
                case CalcDirection::X:
                    return CalcDirection::Y;
                case CalcDirection::Y:
                    return CalcDirection::X;
            }
        }

        CalcDirection direction_to_calc_direction(Direction direction) {
            switch (direction) {
                case Direction::LEFT:
                case Direction::RIGHT:
                    return CalcDirection::X;
                case Direction::TOP:
                case Direction::BOTTOM:
                    return CalcDirection::Y;
            }
        }

        ArrayXXf spatderp3(ArrayXXf p1, ArrayXXf p2,
                           ArrayXXf p3, ArrayXcf derfact,
                           RhoArray rho_array, ArrayXf window, int wlen,
                           CalculationType ct, CalcDirection direct,
                           fftwf_plan plan, fftwf_plan plan_inv) {

            /*//debug stuff
            int writenum;
            static int writenum_px = 0;
            static int writenum_py = 0;
            static int writenum_vx = 0;
            static int writenum_vy = 0;
            if(direct==CalcDirection::Y) {
                if(ct==CalculationType::VELOCITY) {
                    writenum_vy++;
                    writenum = writenum_vy++;
                } else {
                    writenum_py++;
                    writenum = writenum_py++;
                }
            } else {
                if(ct==CalculationType::VELOCITY) {
                    writenum_vx++;
                    writenum = writenum_vx++;
                } else {
                    writenum_px++;
                    writenum = writenum_px++;
                }
            } // end debug stuff */


            //if direct == Y, transpose p1, p2 and p3
            if (direct == CalcDirection::Y) {
                p1.transposeInPlace();
                p2.transposeInPlace();
                p3.transposeInPlace();
            }

            //in the Python code: N1 = fft_batch and N2 = fft_length
            int fft_batch, fft_length;
            ArrayXXf result; //also called Lp in some places in documentation

            fft_batch = p2.rows();
            fft_length = next_2_power((int) p2.cols() + wlen * 2);

            float *in_buffer;
            in_buffer = (float *) fftwf_malloc(sizeof(float) * fft_length * fft_batch);

            fftwf_complex *out_buffer;
            out_buffer = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * ((fft_length / 2) + 1) * fft_batch);

            //non-domains don't have a wisdomcache, so this is needed. TODO Perhaps put it in the Scene itself.
            if (plan == NULL || true) { //always use this one for now, mainly debugging purposes TODO change that
                int shape[] = {fft_length};
                int istride = 1; //distance between two elements in one fft-able array
                int ostride = istride;
                int idist = fft_length; //distance between first element of different arrays
                int odist = (fft_length / 2) + 1;
                #pragma omp critical
                plan = fftwf_plan_many_dft_r2c(1, shape, fft_batch, in_buffer, NULL, istride, idist,
                                               out_buffer, NULL, ostride, odist, FFTW_ESTIMATE);
                #pragma omp critical
                plan_inv = fftwf_plan_many_dft_c2r(1, shape, fft_batch, out_buffer, NULL, ostride, odist,
                                                   in_buffer, NULL, istride, idist, FFTW_ESTIMATE);
            }

            //the pressure is calculated for len(p2)+1, velocity for len(p2)-1
            //slicing and the values pulled from the Rmatrix is slightly different for the two branches
            if (ct == CalculationType::PRESSURE) {
                result.resize(fft_batch, p2.cols() + wlen * 2 + 1);

                //window the outer domains, add a portion of the middle one to the sides and concatenate them all
                ArrayXf window_left = window.head(wlen);
                ArrayXf window_right = window.tail(wlen);

                if (wlen > p1.cols() || wlen > p3.cols()) {
                    std::cout << "CAREFUL: WINDOW IS BIGGER THAN SIDES" << std::endl;
                }

                ArrayXXf dom1(fft_batch, wlen);
                ArrayXXf dom3(fft_batch, wlen);
                ArrayXXf windowed_data(fft_batch, fft_length);
                ArrayXXf fft_input_data(fft_length, fft_batch);
                ArrayXXf zero_pad(fft_batch, fft_length - 2 * wlen - p2.cols());
                zero_pad = ArrayXXf::Zero(fft_batch, fft_length - 2 * wlen - p2.cols());

                //this looks inefficient, but the compiler optimizes almost all of it away
                dom1 = p1.rightCols(wlen) * rho_array.pressure(2, 1) +
                        (p2.leftCols(wlen)).rowwise().reverse() * rho_array.pressure(0, 0);
                dom3 = p3.leftCols(wlen) * rho_array.pressure(3, 1) +
                        (p2.rightCols(wlen)).rowwise().reverse() * rho_array.pressure(1, 0);
                dom1 = dom1.rowwise() * window_left.transpose();
                dom3 = dom3.rowwise() * window_right.transpose();
                windowed_data << dom1, p2, dom3, zero_pad;

                /*//debug
                if(direct==CalcDirection::X)
                    write_array_to_file(windowed_data, "windowed_data_cpp_p", writenum); */

                //TODO maybe optimize this away later (rearrange fft input or change calls above)
                fft_input_data = windowed_data.transpose();

                //perform the fft
                memcpy(in_buffer, fft_input_data.data(), sizeof(float) * fft_batch * fft_length);
                fftwf_execute_dft_r2c(plan, in_buffer, out_buffer);

                //map the results back into an eigen array
                typedef Matrix<std::complex<float>, Dynamic, Dynamic, RowMajor> ArrayXXcfrm;
                Map<ArrayXXcfrm> spectrum_array((std::complex<float> *) out_buffer[0], fft_batch, fft_length / 2 + 1);

                //apply the spectral derivative
                spectrum_array = spectrum_array.array().rowwise() * derfact.topRows(fft_length / 2 + 1).transpose();
                fftwf_execute_dft_c2r(plan_inv, out_buffer, in_buffer);


                Matrix<float, Dynamic, Dynamic, RowMajor> derived_array =
                        Map<Matrix<float, Dynamic, Dynamic, RowMajor>>(in_buffer, fft_batch, fft_length).array();

                //ifft result contains the outer domains, so slice
                result = derived_array.leftCols(wlen + p2.cols() + 1).rightCols(p2.cols() + 1);
                result = result / fft_length; // normalize to compensate for fftw roundtrip gain
            }
                //repeat for velocity calculation (staggered grid, so various different offsets)
            else {
                result.resize(fft_batch, p2.cols() + wlen * 2 - 1);

                //window the outer domains, add a portion of the middle one to the sides and concatenate them all
                ArrayXf window_left = window.head(wlen);
                ArrayXf window_right = window.tail(wlen);

                if (wlen > p1.cols() || wlen > p3.cols()) {
                    //TODO error (or just warn) if this happens and give user feedback.
                }

                ArrayXXf dom1(fft_batch, wlen);
                ArrayXXf dom3(fft_batch, wlen);
                ArrayXXf windowed_data(fft_batch, fft_length);
                ArrayXXf fft_input_data(fft_length, fft_batch);
                ArrayXXf zero_pad(fft_batch, fft_length - 2 * wlen - p2.cols());
                zero_pad = ArrayXXf::Zero(fft_batch, fft_length - 2 * wlen - p2.cols());
                dom1 = p1.rightCols(wlen + 1).leftCols(wlen) * rho_array.velocity(2, 1) +
                        (p2.leftCols(wlen+1).rightCols(wlen)).rowwise().reverse() * rho_array.velocity(0, 0);
                dom3 = p3.leftCols(wlen + 1).rightCols(wlen) * rho_array.velocity(3, 1) +
                        (p2.rightCols(wlen+1).leftCols(wlen)).rowwise().reverse() * rho_array.velocity(1, 0);
                dom1 = dom1.rowwise() * window_left.transpose();
                dom3 = dom3.rowwise() * window_right.transpose();
                windowed_data << dom1, p2, dom3, zero_pad;

                /*//debug
                if(direct==CalcDirection::X)
                    write_array_to_file(windowed_data, "windowed_data_cpp_v", writenum); */

                //debug
                //write_array_to_file(windowed_data, "windowed_data_cpp_v", 0);

                //maybe optimize this away later (rearrange fft input or change calls above)
                fft_input_data = windowed_data.transpose();

                //perform the fft
                memcpy(in_buffer, fft_input_data.data(), sizeof(float) * fft_batch * fft_length);
                fftwf_execute_dft_r2c(plan, in_buffer, out_buffer);

                //map the results back into an eigen array
                typedef Matrix<std::complex<float>, Dynamic, Dynamic, RowMajor> ArrayXXcfrm;
                Map<ArrayXXcfrm> spectrum_array((std::complex<float> *) out_buffer[0], fft_batch, fft_length / 2 + 1);

                //debug
                //write_array_to_file(spectrum_array.array().real(), "cppspecr", 0);
                //write_array_to_file(spectrum_array.array().imag(), "cppspeci", 0);

                //apply the spectral derivative
                spectrum_array = spectrum_array.array().rowwise() * derfact.topRows(fft_length / 2 + 1).transpose();

                fftwf_execute_dft_c2r(plan_inv, out_buffer, in_buffer);

                Matrix<float, Dynamic, Dynamic, RowMajor> derived_array =
                        Map<Matrix<float, Dynamic, Dynamic, RowMajor>>(in_buffer, fft_batch, fft_length).array();

                //debug
                //write_array_to_file(derived_array, "cppderarr", 0);

                //ifft result contains the outer domains, so slice
                result = derived_array.leftCols(wlen + p2.cols()-1).rightCols(p2.cols() - 1);
                result = result / fft_length; // normalize to compensate for fftw roundtrip gain
            }
            if (direct == CalcDirection::Y) {
                result.transposeInPlace();
            }
            fftwf_free(in_buffer);
            fftwf_free(out_buffer);
            if (plan == NULL || true) { //always use local plan for now, mainly debugging purposes TODO change that
                fftwf_destroy_plan(plan);
                fftwf_destroy_plan(plan_inv);
            }
            return result;
        }

        ArrayXXf spatderp3(ArrayXXf p1, ArrayXXf p2,
                           ArrayXXf p3, ArrayXcf derfact,
                           RhoArray rho_array, ArrayXf window, int wlen,
                           CalculationType ct, CalcDirection direct) {
            return spatderp3(p1, p2, p3, derfact, rho_array, window, wlen, ct, direct, NULL, NULL);
        }

        ArrayXf get_window_coefficients(int window_size, int patch_error) {
            float window_alpha = (patch_error - 40) / 20.0 + 1;
            ArrayXf window_coefficients = (
                    (ArrayXf::LinSpaced(2 * window_size + 1, -window_size, window_size) / window_size).square().cube() *
                    log(10) * window_alpha * -1).exp(); // Need to go to power 6 (^2^3)
            return window_coefficients;
        }

        void debug(std::string msg) {
#if 1
            std::cout << msg << std::endl;
#endif
        }

        bool is_approx(float a, float b) {
            if (a == 0) {
                return fabs(b) < EPSILON;
            }
            else {
                return (fabs((a - b) / a) < EPSILON);

            }

        }

        void write_array_to_file(Eigen::ArrayXXf array, std::string filename, unsigned long kk) {
            std::string data_folder = "testdata";
            std::string fullname = "cpp.m";
            std::ofstream data_stream(data_folder + "/" + fullname, std::ios::app);
            data_stream << filename << "(:,:," << kk + 1 << ") = [";
            for (int i = 0; i < array.rows(); i++) {
                for (int j = 0; j < array.cols(); j++) {
                    data_stream << array(i, j) << " ";
                }
                data_stream << ';' << '\n';
            }
            data_stream << "];\n";
        }
    }
}
