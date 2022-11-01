// Implement charge sensitive amp and shaper amp

// 25-Oct-2022 Satoshi Takashima

#include "UtilFunctions.h"
#include "PreampProcessor.h"
#include "ElecStructure.h"
#include "LoadOptionFile.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/

// C++ includes
#include <iostream>
#include <cmath>
#include <numeric>

namespace gramselecsim {

  // Constructor: Initializes the class.
  PreampProcessor::PreampProcessor(int size_waveform)
  {
    // Get the options class. This contains all the program options
    // from options.xml and the command line.
    auto options = util::Options::GetInstance();

    // Fetch the options we want from the class. 
    options->GetOption("verbose",m_verbose);
    options->GetOption("debug",m_debug);

    // Use the definitions in LoadOptionFile and ElecStructure for
    // the remaining options.

    auto optionloader = gramselecsim::LoadOptionFile::GetInstance();
    header_gen_     = optionloader->GeneralHeader();
    header_preamp_  = optionloader->PreampHeader();

    if (m_verbose) {
      std::cout << "gramselecsim::PreampProcessor - ";
      std::cout << "preamp func = "     << header_preamp_.preamp_func << std::endl;
      std::cout << "preamp prior time duration" << header_preamp_.preamp_prior_time << "ns" << std::endl;
      std::cout << "preamp post time duration" << header_preamp_.preamp_post_time   << "ns" << std::endl;

      std::cout << "preamp mu = "       << header_preamp_.preamp_mu    << "ns" << std::endl;
      std::cout << "preamp sigma = "    << header_preamp_.preamp_sigma << "ns" << std::endl;
      std::cout << "preamp tau1 = "     << header_preamp_.preamp_tau1  << "ns" << std::endl;
      std::cout << "preamp tau2 = "     << header_preamp_.preamp_tau2  << "ns" << std::endl;
      std::cout << "preamp gain = "     << header_preamp_.preamp_gain << "mV/fC" << std::endl;
    }

    preamp_time_bin_.resize(2);
    size_waveform_ = size_waveform;

    preamp_time_bin_[0]     = std::floor( header_preamp_.preamp_prior_time  /   header_gen_.timebin_width );
    preamp_time_bin_[1]     = std::floor( header_preamp_.preamp_post_time   /   header_gen_.timebin_width ); 

    response_length_bin_ = std::floor( ( header_preamp_.preamp_prior_time + header_preamp_.preamp_post_time )/ header_gen_.timebin_width );
    preamp_response_.resize( response_length_bin_ );

    // Which preamp response function should be used? Pre-compute this
    // for every time bin.
    for(int i=0;i<response_length_bin_;i++){

      double t = header_gen_.timebin_width * i;

      if(header_preamp_.preamp_func==0){
	preamp_response_[i] = gramselecsim::NormGauss(t, 0.0, header_preamp_.preamp_sigma);
      } else if(header_preamp_.preamp_func==1){
	preamp_response_[i] = gramselecsim::Gauss(t, 0.0, header_preamp_.preamp_sigma);
      } else if(header_preamp_.preamp_func==2){
	preamp_response_[i] = gramselecsim::LogNormGauss(t, 0.0, header_preamp_.preamp_sigma);
      } else if(header_preamp_.preamp_func==3){
	preamp_response_[i] = gramselecsim::LogGauss(t, 0.0, header_preamp_.preamp_sigma);
      } else if(header_preamp_.preamp_func==4){
	preamp_response_[i] = gramselecsim::TwoExp(t, header_preamp_.preamp_tau1, header_preamp_.preamp_tau2);
      }   else{
	std::cout << "gramselecsim::PreampProcessor() - "
		  << "not validated preamp response function flag" << std::endl;
      }
    }

    //peak_delay_bin is the number of bins between peak time of a
    //response function and when an e- cluster arrives
    peak_delay_bin_ = static_cast<int>(std::floor(header_preamp_.peak_delay / header_gen_.timebin_width));
  }

  PreampProcessor::~PreampProcessor(){}

  // Compute the analog waveform based on the number of electrons seen
  // at the pixel.
  std::vector<double> PreampProcessor::ConvoluteResponse( std::vector<int>& num_arrival_electron ) {

    std::vector<double> output_waveform(size_waveform_, 0.0);
    int event_start_bin;

    // For every time bin:
    for (int i=0; i<size_waveform_; i++){
      int num_electron = num_arrival_electron[i];

      if (num_electron==0) {
	continue;
      }

      event_start_bin = i + peak_delay_bin_;

      if( (event_start_bin - preamp_time_bin_[0]) < 0){
	for(int j=0;j<event_start_bin;j++){
	  output_waveform[j] += preamp_response_[preamp_time_bin_[0] - event_start_bin + j] * num_electron;
	}
      }   else if ( (event_start_bin + preamp_time_bin_[1]) > (size_waveform_ - 1) ){
	for(int j = (event_start_bin - preamp_time_bin_[0]);j<size_waveform_;j++){
	  output_waveform[j] += preamp_response_[j-event_start_bin + preamp_time_bin_[0]] * num_electron;
	}
      }   else{
	for(int j=(event_start_bin - preamp_time_bin_[0]);j<(event_start_bin + preamp_time_bin_[1]);j++){
	  output_waveform[j] += preamp_response_[j - event_start_bin + preamp_time_bin_[0]] * num_electron;
	}
      }
    }

    // Apply the gain of the pre-amp. 
    for (int k=0; k<size_waveform_; k++) {
      output_waveform[k] *= header_preamp_.preamp_gain;
    }

    return output_waveform;
  }

} // namespace gramselecsim
