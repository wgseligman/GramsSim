// Define methods to load the parameters for the various
// electronics-simulation models.
// 25-Oct-2022 Satoshi Takashima

#ifndef LoadOptionFile_h
#define LoadOptionFile_h

#include "ElecStructure.h"

namespace gramselecsim {

  class LoadOptionFile
  {
  public:
    
    // This is a singleton class.
    // According to <https://stackoverflow.com/questions/12248747/singleton-with-multithreads>
    // this method is compatible with multi-threaded running. 
    static LoadOptionFile* GetInstance()
    {
      static LoadOptionFile instance;
      return &instance;
    }

    // Load the parameters from the Options XML file. This should be
    // called exactly once, from the main routine.
    bool Load();
    
    general_header GeneralHeader();
    preamp_header PreampHeader();
    noise_header NoiseHeader();
    adc_header ADCHeader();
    
    virtual ~LoadOptionFile();
    

  protected:
    // Standard null constructor for a singleton class.
    LoadOptionFile() {}

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
