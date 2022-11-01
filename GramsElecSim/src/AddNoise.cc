// 25-Oct-2022 Satoshi Takashima

// Add noise to analogue signal

#include "AddNoise.h"
#include "ElecStructure.h"
#include "LoadOptionFile.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/

// For generating random numbers.
#include "TRandom.h"

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

    // Use the ElecStructure and LoadOptionFile utilities for the
    // model options.
    auto optionloader = LoadOptionFile::GetInstance();
    m_header = optionloader->NoiseHeader();

    if (m_verbose_) {
      std::cout << "gramselecsim::AddNoise() - "
		<< " noise_param0 = " << m_header.noise_param0 
		<< " noise_param1 = " << m_header.noise_param1 << std::endl;
      std::cout << " noise_param2 = " << m_header.noise_param2 << std::endl;
    }
  }

  // Generate a noise vector from a gaussian distribution.
  std::vector<double> AddNoise::GenSeedNoiseArray(int vec_length){

    std::vector<double> random_vec(vec_length);

    for (int i=0; i<vec_length; i++) {
      random_vec[i] = gRandom->Gaus(0.0,1.0);
    }

    return random_vec;
  }

  AddNoise::~AddNoise() {}

  std::vector<double> AddNoise::ProcessCurrentNoise(const std::vector<double>& analogue_waveform ) {

    int length_waveform = analogue_waveform.size();
    std::vector<double> waveform_with_noise(length_waveform, 0);
    std::vector<double> noise_array_waveform = GenSeedNoiseArray(length_waveform);
      
    for(int i=0;i<length_waveform;i++){
      waveform_with_noise[i] = analogue_waveform[i] 
	+ waveform_with_noise[i] 
	* std::sqrt( m_header.noise_param0 * m_header.noise_param0 
                   + m_header.noise_param1 * m_header.noise_param1 * analogue_waveform[i]
                   + m_header.noise_param2 * m_header.noise_param2 
		         * analogue_waveform[i] * analogue_waveform[i]);
    }
    return waveform_with_noise;
  }
  
  std::vector<int> AddNoise::ProcessElectronNoise(const std::vector<int>& num_arrival_electron ) {
    
    int length_waveform = num_arrival_electron.size();
    
    std::vector<double> noise_array_waveformlength = GenSeedNoiseArray(length_waveform);
    std::vector<int> waveform_with_noise(length_waveform);
    
    for (int i=0; i<length_waveform; i++){
      waveform_with_noise[i] = std::max(0, 
	int( std::floor( num_arrival_electron[i] 
	 + noise_array_waveformlength[i] 
	   * std::sqrt( m_header.noise_param0 * m_header.noise_param0 
	              + m_header.noise_param1 * m_header.noise_param1 * num_arrival_electron[i]
	              + m_header.noise_param2 * m_header.noise_param2 * num_arrival_electron[i] 
						      * num_arrival_electron[i]) ) ) );
    }

    return waveform_with_noise;
	
  }

} // namespace gramselecsim
