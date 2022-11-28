// Convert an analog waveform to a digital waveform.
// 25-Oct-2022 Satoshi Takashima

#ifndef ADConvert_h
#define ADConvert_h

#include "ElecStructure.h"
#include "LoadOptionFile.h"

#include <vector>

namespace gramselecsim {

    class ADConvert
    {
    public:

        ADConvert();
        std::vector<int> Process(std::vector<double>&);

        virtual ~ADConvert();

    private:

        general_header header_gen_;
        adc_header header_adc_;
        int r_adcbin_width_to_origin_width_;
        double lsb_;

        bool m_verbose;
        bool m_debug;
    };
}

#endif
