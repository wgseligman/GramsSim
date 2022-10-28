
#ifndef LoadOptionFile_h
#define LoadOptionFile_h

#include "ElecStructure.h"

namespace gramselecsim {

    class LoadOptionFile
    {
    public:

        LoadOptionFile();
        general_header GeneralHeader();
        preamp_header PreampHeader();
        noise_header NoiseHeader();
        adc_header ADCHeader();

        virtual ~LoadOptionFile();

    private:

        double    m_timebin_width_;
        double    m_time_window_;
        int       m_electron_cluster_size_;

        int       m_preamp_func_;
        double    m_preamp_prior_time_;
        double    m_preamp_post_time_;
        double    m_peak_delay_;
        double    m_preamp_mu_;
        double    m_preamp_sigma_;
        double    m_preamp_tau1_;
        double    m_preamp_tau2_;
        double    m_preamp_gain_;

        double    m_noise_param0_;
        double    m_noise_param1_;
        double    m_noise_param2_;

        int       m_bit_resolution_;
        double    m_input_min_;
        double    m_input_max_;
        double    m_sample_freq_;
    };
}

#endif
