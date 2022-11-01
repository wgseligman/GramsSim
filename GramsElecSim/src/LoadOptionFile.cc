// Define methods to load the parameters for the various
// electronics-simulation models.
// 25-Oct-2022 Satoshi Takashima

#include "LoadOptionFile.h"
#include "ElecStructure.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/

// C++ includes
#include <iostream>
#include <string>

namespace gramselecsim {

    // Load the options from the XML file and the command line. 
    bool LoadOptionFile::Load()
    {
        // Get the options class. This contains all the program options
        // from options.xml and the command line.
        auto options = util::Options::GetInstance();

        // Fetch the options we want from the class. 

        options->GetOption("timebin_width",         m_timebin_width_);
        options->GetOption("time_window",           m_time_window_);
        options->GetOption("ElectronClusterSize",   m_electron_cluster_size_);

        options->GetOption("preamp_func",           m_preamp_func_);
        options->GetOption("preamp_prior_time",     m_preamp_prior_time_);
        options->GetOption("preamp_post_time",      m_preamp_post_time_);
        options->GetOption("peak_delay",            m_peak_delay_);
        options->GetOption("preamp_mu",             m_preamp_mu_);
        options->GetOption("preamp_sigma",          m_preamp_sigma_);
        options->GetOption("preamp_tau1",           m_preamp_tau1_);
        options->GetOption("preamp_tau2",           m_preamp_tau2_);
        options->GetOption("preamp_gain",           m_preamp_gain_);

        options->GetOption("noise_param0",          m_noise_param0_);
        options->GetOption("noise_param1",          m_noise_param1_);
        options->GetOption("noise_param2",          m_noise_param2_);

        options->GetOption("bit_resolution",        m_bit_resolution_);
        options->GetOption("input_min",             m_input_min_);
        options->GetOption("input_max",             m_input_max_);
        options->GetOption("sample_freq",           m_sample_freq_);

	// May modify this in the future if we want to include the
	// possibility that this method will fail.
	return true; 
    }

    LoadOptionFile::~LoadOptionFile() {}

    general_header LoadOptionFile::GeneralHeader() {

        general_header header;

        header.timebin_width        = m_timebin_width_;
        header.time_window          = m_time_window_;

        return header;
    }

    preamp_header LoadOptionFile::PreampHeader() {

        preamp_header header;

        header.preamp_func        = m_preamp_func_;
        header.preamp_prior_time  = m_preamp_prior_time_;
        header.preamp_post_time   = m_preamp_post_time_;
        header.peak_delay         = m_peak_delay_;

        header.preamp_mu          = m_preamp_mu_;
        header.preamp_sigma       = m_preamp_sigma_;
        header.preamp_tau1        = m_preamp_tau1_;
        header.preamp_tau2        = m_preamp_tau2_;
        header.preamp_gain        = m_preamp_gain_;

        return header;
    }
     
    noise_header LoadOptionFile::NoiseHeader() {

        noise_header header;

        header.noise_param0     = m_noise_param0_;
        header.noise_param1     = m_noise_param1_;
        header.noise_param2     = m_noise_param2_;

        return header;
    }

    adc_header LoadOptionFile::ADCHeader() {

        adc_header header;

        header.bit_resolution   = m_bit_resolution_;
        header.input_min        = m_input_min_;
        header.input_max        = m_input_max_;
        header.sample_freq      = m_sample_freq_;

        return header;
    }

} // namespace gramsdetsim
