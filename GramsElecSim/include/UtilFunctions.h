
#ifndef UtilFunctions_h
#define UtilFunctions_h

#include <vector>
#include <cmath>

namespace gramselecsim {

    inline double LogNormGauss(double x, double mu, double sigma){
        return (1.0/std::sqrt(2*M_PI*sigma*sigma)) 
                * std::exp(-std::pow( (std::log(x) - mu), 2.0) / (2 * sigma * sigma));
    }

    inline double NormGauss(double x, double mu, double sigma){
        return (1.0/std::sqrt(2*M_PI*sigma*sigma)) 
                * std::exp(-1.0 * (x - mu) * (x - mu) / (2 * sigma * sigma));
    }

    inline double LogGauss(double x, double mu, double sigma){
        return std::exp(-std::pow( (std::log(x) - mu), 2.0) / (2 * sigma * sigma));
    }

    inline double Gauss(double x, double mu, double sigma){
        return std::exp(-1.0 * (x - mu) * (x - mu) / (2 * sigma * sigma));
    }

    inline double TwoExp(double x, double tau1, double tau2){
        double t0 = tau1 * tau2 * ( std::log(tau2) - std::log(tau1) ) / (tau2 - tau1);
        return (tau1 * std::exp(-1.0 * (x - t0) / tau1) - tau2 * std::exp(-1.0 * (x - t0) / tau2) ) / (tau1 - tau2);
    }


    class HashVI {  //Hash function object
    public:
        size_t operator()(const std::vector<int> &x) const {
            const int C = 997;      // prime number
            size_t t = 0;
            for (size_t i = 0; i != x.size(); ++i) {
                t = t * C + x[i];
            }
            return t;
        }
    };
}

#endif
