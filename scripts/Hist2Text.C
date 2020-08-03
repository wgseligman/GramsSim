{
  // Convert a standard ROOT histogram into the text format used by
  // Geant4's General Particle Source ("gps").

  gROOT->Reset();

  // 'sillyEnergyFlux' is just a gaussian. Don't use it for real
  // physics!
  std::string inputFileName("sillyEnergyFlux.root");
  std::string histName("energyflux");
  std::string outputFileName("energyflux.txt");

  TFile* inputFile = new TFile(inputFileName.c_str());
  if (inputFile == NULL) {
    std::cerr << "Cannot open file '" << inputFileName << "'"
	      << std::endl;
  }
  else {
    TH1* hist = (TH1*)inputFile->Get(histName.c_str());
    if ( hist == NULL ) {
      std::cerr << "Cannot get histogram '" << histName
		<< "' from file '" << inputFileName << "'"
		<< std::endl;
    }
    else {
      // Create output file.
      std::ofstream outputFile(outputFileName.c_str());

      // For each histogram bin:
      Int_t numberBins = hist->GetNbinsX();
      for ( Int_t i = 0; i != numberBins; ++ i ) {
	// Write the bin's x-value in G4 units (MeV, mm, ns)
	// and the bin's value.
	auto binCenter = hist->GetBinCenter(i);
	auto binContent = hist->GetBinContent(i);
	outputFile << binCenter << " "
		   << binContent 
		   << std::endl;
      } // for each bin

      outputFile.close();

    } // get histogram
  } // open file

}
