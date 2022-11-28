// Define:
//  - Groups of option values used by the various 
//    electronics-simulations models. 
//  - The ROOT ntuple specification for how these values
//    are to be stored in an ntuple. 

// 25-Oct-2022 Satoshi Takashima

#ifndef ElecStructure_h
#define ElecStructure_h

#include <string>

namespace gramselecsim {

    struct general_header {

        double    timebin_width;
        double    time_window;

    };

    struct preamp_header {

        double    preamp_prior_time;
        double    preamp_post_time;
        double    peak_delay;
        double    preamp_mu;
        double    preamp_sigma;
        double    preamp_tau1;
        double    preamp_tau2;
        double    preamp_gain;
        int       preamp_func;
    };

    struct noise_header {

        double    noise_param0;
        double    noise_param1;
        double    noise_param2;
    };

    struct adc_header {

        double    input_min;
        double    input_max;
        double    sample_freq;
        int       bit_resolution;
    };

    inline std::string GeneralHeaderType() {
        return std::string("timebin_width/D:time_window/D");
    }

    inline std::string PreampHeaderType() { 
        return std::string("prior_time_preamp/D:post_time_preamp/D:peak_delay/D")
        + std::string(":preamp_mu/D:preamp_sigma/D:preamp_tau1/D:preamp_tau2/D:preamp_func/I");
    }

    inline std::string NoiseHeaderType() { 
        return std::string("noise_param0/D:noise_param1/D:noise_param2/D");
    }

    inline std::string ADCHeaderType() { 
        return std::string("input_min/D:input_max/D:sample_freq/D:bit_resolution/I");
    }
}

#endif
