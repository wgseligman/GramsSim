// 26-Nov-2021 WGS

// Implement analogue-digital convertion

#include "ADConvert.h"
#include "ElecStructure.h"
#include "LoadOptionFile.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/

// C++ includes
#include <iostream>
#include <cmath>

namespace gramselecsim {

    // Constructor: Initializes the class.
    ADConvert::ADConvert()
    {
        // Get the options class. This contains all the program options
        // from options.xml and the command line.
        auto options = util::Options::GetInstance();

        // Fetch the options we want from the class. 

        options->GetOption("verbose",m_verbose);
        options->GetOption("debug",m_debug);

        optionloader_    = new gramselecsim::LoadOptionFile();
        header_gen_     = optionloader_->GeneralHeader();
        header_adc_     = optionloader_->ADCHeader();

        lsb_ = (header_adc_.input_max - header_adc_.input_min) 
                / std::pow(2.0, header_adc_.bit_resolution);

        r_adcbin_width_to_origin_width_ = ( 1000.0 / header_adc_.sample_freq ) / header_gen_.timebin_width;

        if (m_verbose) {
          std::cout << "gramselecsim::ADConvert - "
        	<< " Resolution = " <<  header_adc_.bit_resolution << " bit, LSB = "  << lsb_  << std::endl;
          std::cout << " Min range = " <<  header_adc_.input_min << ", Max range = `" << header_adc_.input_max << std::endl;
        }
    }

    ADConvert::~ADConvert() {}

    std::vector<int> ADConvert::Process( std::vector<double>& analog_waveform ) {

        Double_t analog_val;
        int length_waveform = analog_waveform.size() / r_adcbin_width_to_origin_width_;
        std::vector<int> digital_waveform(length_waveform, 0);

        for(int i=0;i<length_waveform;i++){

            analog_val = 0.0;

            for(int j=0;j<r_adcbin_width_to_origin_width_;j++){
                analog_val += std::min( std::max(
                        analog_waveform[i * r_adcbin_width_to_origin_width_ + j], 
                        header_adc_.input_min), 
                        header_adc_.input_max);
            }
            analog_val /= r_adcbin_width_to_origin_width_;
            digital_waveform[i] = std::floor( (analog_val - header_adc_.input_min) / lsb_);
        }

      return digital_waveform;
    }

} // namespace gramselecsim
