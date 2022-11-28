
#ifndef PreampProcessor_h
#define PreampProcessor_h

#include <vector>

#include "ElecStructure.h"
#include "LoadOptionFile.h" 

namespace gramselecsim {

    class PreampProcessor
    {
    public:

        PreampProcessor(int);
        virtual ~PreampProcessor();

        std::vector<double> ConvoluteResponse(std::vector<int>&);

    private:

        bool m_verbose;
        bool m_debug;

        int size_waveform_;
        int response_length_bin_;
        int peak_delay_bin_;

        std::vector<int> preamp_time_bin_;
        std::vector<double> preamp_response_;

        general_header                  header_gen_;
        preamp_header                   header_preamp_;

    };
}

#endif
