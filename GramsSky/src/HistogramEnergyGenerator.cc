#include "HistogramEnergyGenerator.h"
#include "Options.h" // in util

// ROOT includes
#include "TFile.h"
#include "TH1.h"

// C++ includes
#include <iostream>

namespace gramssky {

  HistogramEnergyGenerator::HistogramEnergyGenerator()
    : EnergyGenerator()
  {
    // Get the parameters from the Options XML file.
    auto options = util::Options::GetInstance();
    std::string histFile, histName;
    options->GetOption("HistFile",histFile);
    options->GetOption("HistName",histName);

    // Open the file.
    m_histFile = new TFile(histFile.c_str());
    if ( m_histFile == NULL ) {
      std::cerr << "File " << __FILE__ << " Line " << __LINE__ << ":" << std::endl
		<< " Could not open file '" << histFile
		<< "'; Aborting job"
		<< std::endl;
      exit(EXIT_FAILURE);
    }

    // Fetch the histogram.
    m_histogram = (TH1*) m_histFile->Get( histName.c_str() );
    if ( m_histogram == NULL ) {
      std::cerr << "File " << __FILE__ << " Line " << __LINE__ << ":" << std::endl
		<< " Could not find histogram '" << histName
		<< "' in file '" << histFile
		<< "'; Aborting job"
		<< std::endl;
      exit(EXIT_FAILURE);
    }

  }

  HistogramEnergyGenerator::~HistogramEnergyGenerator()
  {
    // Close file and cleanup pointer.
    m_histFile->Close();
    delete m_histFile;
  }

  double HistogramEnergyGenerator::Generate()
  {
    // Generate random value from histogram. 
    double energy = m_histogram->GetRandom();
    return energy;
  }

} // namespace gramssky
