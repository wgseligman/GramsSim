// 26-Nov-2021 WGS

// Add noise to analogue signal

#include "AddNoise.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/


// C++ includes
#include <iostream>
#include <random>
#include <cmath>

namespace gramselecsim {

    // Constructor: Initializes the class.
    AddNoise::AddNoise()
    {
        // Get the options class. This contains all the program options
        // from options.xml and the command line.
        auto options = util::Options::GetInstance();

        // Fetch the options we want from the class. 
        options->GetOption("verbose",m_verbose_);
        options->GetOption("debug",m_debug_);

        options->GetOption("noise_param0",  m_noise_param0_);
        options->GetOption("noise_param1",  m_noise_param1_);
        options->GetOption("noise_param2",  m_noise_param2_);

        if (m_verbose_) {
            std::cout << "gramselecsim::AddNoise - "
              << " noise_param0 = " << m_noise_param0_ << " noise_param1 = "  << m_noise_param1_  << std::endl;
            std::cout << "noise_param2 = " << m_noise_param2_ << std::endl;
        }
    }

    std::vector<double> AddNoise::GenSeedNoiseArray(int vec_length){

        std::random_device seed_gen;
        std::mt19937 engine(seed_gen());
        std::vector<double> random_vec(vec_length);
        std::normal_distribution<> dist_normal(0.0, 1.0);

        for(int i=0;i<vec_length;i++){
            random_vec[i] = dist_normal(engine);
        }
        return random_vec;
    }

    AddNoise::~AddNoise() {}

    std::vector<double> AddNoise::ProcessCurrentNoise(const std::vector<double>& analogue_waveform ) {

        int length_waveform = analogue_waveform.size();
        std::vector<double>    waveform_with_noise(length_waveform, 0);
        std::vector<double> noise_array_waveform = GenSeedNoiseArray(length_waveform);

        for(int i=0;i<length_waveform;i++){
            waveform_with_noise[i] = analogue_waveform[i] 
                    + waveform_with_noise[i] * std::sqrt( m_noise_param0_ * m_noise_param0_ 
                    + m_noise_param1_ * m_noise_param1_ * analogue_waveform[i]
                    + m_noise_param2_ * m_noise_param2_ * analogue_waveform[i] * analogue_waveform[i]);
        }
        return waveform_with_noise;
    }

    std::vector<int> AddNoise::ProcessElectronNoise(const std::vector<int>& num_arrival_electron ) {

        int length_waveform = num_arrival_electron.size();

        std::vector<double> noise_array_waveformlength = GenSeedNoiseArray(length_waveform);
        std::vector<int> waveform_with_noise(length_waveform);

        for(int i=0;i<length_waveform;i++){
            waveform_with_noise[i] = std::max(0, 
                    int( std::floor( num_arrival_electron[i] 
                    + noise_array_waveformlength[i] * std::sqrt( m_noise_param0_ * m_noise_param0_ 
                    + m_noise_param1_ * m_noise_param1_ * num_arrival_electron[i]
                    + m_noise_param2_ * m_noise_param2_ * num_arrival_electron[i] * num_arrival_electron[i]) ) ) );
        }

        return waveform_with_noise;

    }

} // namespace gramselecsim
