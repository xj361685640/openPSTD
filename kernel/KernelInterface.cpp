//
// Created by michiel on 24-1-2016.
//

#include "KernelInterface.h"

namespace OpenPSTD
{
    namespace Kernel
    {

        const char *PSTDKernelNotConfiguredException::what() const noexcept
        {
            return "Kernel is not yet configured";
        }


        float PSTDSettings::GetGridSpacing()
        {
            return this->gridSpacing;
        }

        void PSTDSettings::SetGridSpacing(float value)
        {
            this->gridSpacing = value;
            //TODO edit max frequency here as well
        }

        float PSTDSettings::GetPatchError()
        {
            return this->patcherror;
        }

        void PSTDSettings::SetPatchError(float value)
        {
            this->patcherror = value;
        }

        float PSTDSettings::GetRenderTime()
        {
            return this->calctime;
        }

        void PSTDSettings::SetRenderTime(float value)
        {
            this->calctime = value;
        }

        int PSTDSettings::GetPMLCells()
        {
            return this->PMLCells;
        }

        void PSTDSettings::SetPMLCells(int value)
        {
            this->PMLCells = value;
        }

        float PSTDSettings::GetAttenuationOfPMLCells()
        {
            return this->ampMax;
        }

        void PSTDSettings::SetAttenuationOfPMLCells(float value)
        {
            this->ampMax = value;
        }

        float PSTDSettings::GetDensityOfAir()
        {
            return this->rho;
        }

        void PSTDSettings::SetDensityOfAir(float value)
        {
            this->rho = value;
        }

/**
 * TODO: add dynamic behavior to setters of freqMax and grid spacing
 */
        float PSTDSettings::GetMaxFrequency()
        {
            return this->freqMax;
        }

        void PSTDSettings::SetMaxFrequency(float value)
        {
            this->freqMax = value;
            //TODO edit grid spacing as well
        }

        float PSTDSettings::GetSoundSpeed()
        {
            return this->c1;
        }

        void PSTDSettings::SetSoundSpeed(float value)
        {
            this->c1 = value;
        }

        float PSTDSettings::GetFactRK()
        {
            return this->tfactRK;
        }

        void PSTDSettings::SetFactRK(float value)
        {
            this->tfactRK = value;
        }

        int PSTDSettings::GetSaveNth()
        {
            return this->SaveNth;
        }

        void PSTDSettings::SetSaveNth(int value)
        {
            this->SaveNth = value;
        }

        float PSTDSettings::GetBandWidth()
        {
            return this->band_width;
        }

        void PSTDSettings::SetBandWidth(float value)
        {
            this->band_width = value;
        }

        float PSTDSettings::GetWaveLength()
        {
            return this->wave_length;
        }

        void PSTDSettings::SetWaveLength(float value)
        {
            this->wave_length = value;
        }

        bool PSTDSettings::GetSpectralInterpolation()
        {
            return this->spectral_interpolation;
        }

        void PSTDSettings::SetSpectralInterpolation(bool value)
        {
            this->spectral_interpolation = value;
        }

        bool PSTDSettings::GetGPUAccel()
        {
            return this->gpu;
        }

        void PSTDSettings::SetGPUAccel(bool value)
        {
            this->gpu = value;
        }

        bool PSTDSettings::GetMultiThread()
        {
            return this->multithread;
        }

        void PSTDSettings::SetMultiThread(bool value)
        {
            this->multithread = value;
        }

        void PSTDSettings::SetWindow(Eigen::ArrayXf A)
        {
            this->window = A;
        }

        Eigen::ArrayXf PSTDSettings::GetWindow()
        {
            return this->window;
        }

        float PSTDSettings::GetTimeStep()
        {
            return this->tfactRK * this->gridSpacing / this->c1;
        }

        int PSTDSettings::GetWindowSize()
        {
            //directly translated from original Python implementation
            int tmp = std::round((this->patcherror * 0.7 - 17) / 2.);
            tmp = std::round(tmp * 2);
            return tmp;
        }

        std::vector<float> PSTDSettings::GetRKCoefficients()
        {
            return this->rk_coefficients;
        }

        void PSTDSettings::SetRKCoefficients(std::vector<float> coef)
        {
            this->rk_coefficients = coef; //Todo: Set somewhere.

        }

        std::shared_ptr<PSTDConfiguration> PSTDConfiguration::CreateDefaultConf()
        {
            std::shared_ptr<PSTDConfiguration> conf = std::make_shared<PSTDConfiguration>();
            conf->Settings.SetRenderTime(1.0f);
            conf->Settings.SetSoundSpeed(340.0f);
            conf->Settings.SetAttenuationOfPMLCells(20000.0f);
            conf->Settings.SetDensityOfAir(1.2f);
            conf->Settings.SetPatchError(70.0f);
            conf->Settings.SetFactRK(0.5f);
            conf->Settings.SetGridSpacing(0.2);
            conf->Settings.SetPMLCells(50);
            conf->Settings.SetSaveNth(1);

            //todo: fix good default values for these parameters
            conf->Settings.SetBandWidth(1.0f);
            conf->Settings.SetRKCoefficients(std::vector<float>());
            conf->Settings.SetWindow(Eigen::ArrayXf());
            conf->Settings.SetSpectralInterpolation(true);

            conf->Speakers.push_back(QVector3D(4, 5, 0));
            conf->Receivers.push_back(QVector3D(6, 5, 0));

            DomainConf d;
            d.TopLeft = QVector2D(0, 0);
            d.Size = QVector2D(10, 10);
            d.T.Absorption = 0;
            d.B.Absorption = 0;
            d.L.Absorption = 0;
            d.R.Absorption = 0;

            d.T.LR = false;
            d.B.LR = false;
            d.L.LR = false;
            d.R.LR = false;

            conf->Domains.push_back(d);

            return conf;
        }
    }
}
