
#ifndef AddNoise_h
#define AddNoise_h

#include <vector>

namespace gramselecsim {

    class AddNoise
    {
    public:

        AddNoise();
        std::vector<double> GenSeedNoiseArray(int);
        std::vector<double> ProcessCurrentNoise(const std::vector<double>&);
        std::vector<int> ProcessElectronNoise(const std::vector<int>&);

        virtual ~AddNoise();

    private:

        bool m_verbose_;
        bool m_debug_;

        double m_noise_param0_;
        double m_noise_param1_;
        double m_noise_param2_;
    };
}

#endif
