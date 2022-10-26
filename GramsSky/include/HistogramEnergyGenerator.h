// 09-Jan-2022 WGS 

// Generate energy according to histogram distribution.

#ifndef Grams_HistogramEnergyGenerator_h
#define Grams_HistogramEnergyGenerator_h

#include "EnergyGenerator.h"

// ROOT includes
#include "TFile.h"
#include "TH1.h"

namespace gramssky {

  class HistogramEnergyGenerator : public EnergyGenerator
  {
  public:

    // Constructor. 
    HistogramEnergyGenerator();

    // Destructor.
    virtual ~HistogramEnergyGenerator();

    // Generate an energy.
    virtual double Generate();

  private:
    // The parameters for the histogram distribution.
    TFile* m_histFile;
    TH1* m_histogram;
  };

} // namespace gramssky

#endif // Grams_HistogramEnergyGenerator_h
