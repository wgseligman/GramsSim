// 21-Sep-2022 Satoshi Takashima, William Seligman
// Implement a diffusion model calculation.

#include "DiffusionModel.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/

// ROOT includes
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TRandom.h"

// C++ includes
#include <iostream>
#include <cmath>
#include <vector>
#include <tuple>


namespace gramsdetsim {

  // Constructor: Initializes the class.
  DiffusionModel::DiffusionModel(TTreeReader* reader)
    : m_reader(reader)
  {
      // Get the options class. This contains all the program options
      // from options.xml and the command line.
      auto options = util::Options::GetInstance();

      // Fetch the options we want from the class. 

      options->GetOption("verbose",            m_verbose);
      options->GetOption("debug",              m_debug);

      options->GetOption("MeVToElectrons",        m_MeVToElectrons);
      //Only for diffusion constant, the units are mm^2/ms
      options->GetOption("LongitudinalDiffusion", m_LongitudinalDiffusion);
      options->GetOption("TransverseDiffusion",   m_TransverseDiffusion);

      options->GetOption("ElectronClusterSize",   m_ElectronClusterSize);
      options->GetOption("ReadoutPlaneOffset",    m_readout_plane_offset);
      options->GetOption("ElectronDriftVelocity", m_DriftVel);
      options->GetOption("MinNumberOfElCluster",  m_MinNumberOfElCluster);

      m_RecipDriftVel = 1.0 / m_DriftVel;

      if (m_verbose) {
          std::cout << "gramsdetsim::DiffusionModel - "
              << "LongitudinalDiffusion= " << m_LongitudinalDiffusion
              << " TransverseDiffusion= " << m_TransverseDiffusion
              << " ElectronClusterSize=" << m_ElectronClusterSize 
              << std::endl;
      }

    // These are the columns in the ntuple that we'll require for our
    // calculation.

    // The names of the columns, and which columns are available, come
    // from GramsG4/src/GramsG4WriteNtuplesAction.cc.
    m_xStart = new TTreeReaderValue<Float_t>(*m_reader, "xStart");
    m_yStart = new TTreeReaderValue<Float_t>(*m_reader, "yStart");
    m_zStart = new TTreeReaderValue<Float_t>(*m_reader, "zStart");
    m_xEnd   = new TTreeReaderValue<Float_t>(*m_reader, "xEnd");
    m_yEnd   = new TTreeReaderValue<Float_t>(*m_reader, "yEnd");
    m_zEnd   = new TTreeReaderValue<Float_t>(*m_reader, "zEnd");

  }

  // Destructor: clean up the class
  DiffusionModel::~DiffusionModel() {
      // Delete any pointers we created.
      delete m_xStart;
      delete m_yStart;
      delete m_zStart;
      delete m_xEnd;
      delete m_yEnd;
      delete m_zEnd;
  }

  // Note that the "a_" prefix is a convention to remind us that the
  // variable was an argument in this method.

  // Also note that this method returns a tuple of six vectors.

std::tuple<std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double> >
  DiffusionModel::Calculate(double a_energy) {

    // The TTreeReaderValue variables are "in sync" with the
    // TTreeReader. So as the main routine uses TTreeReader to go
    // through the ntuple, the TTreeReaderValue variables in this
    // program will automatically point to the current row within
    // TTreeReader.

    // This is a tricky part: Normally a TTreeReaderValue acts like a
    // pointer, e.g., if you have "TTreeReaderValue<double> energy"
    // then you refer to its value as "*energy". But in this routine,
    // m_zStart (for example) is a pointer itself, so to get its value
    // you need "**m_zStart" (a pointer to a pointer). 

    double z_mean = 0.5 * (**m_zStart + **m_zEnd);
    //A/I z_offset should be defined later;
    double DriftDistance = m_readout_plane_offset - z_mean;
    double mean_TDrift = std::abs(DriftDistance * m_RecipDriftVel);
    double SqrtT = std::sqrt(mean_TDrift);

    double fLDiff_const = std::sqrt(2. * m_LongitudinalDiffusion);
    double fTDiff_const = std::sqrt(2. * m_TransverseDiffusion);

    double LDiffSig = (1.0e-3 * SqrtT) * fLDiff_const;
    double TDiffSig = (1.0e-3 * SqrtT) * fTDiff_const;

    // Break up the ionization energy into separate electron clusters.

    const double nElectrons = a_energy * m_MeVToElectrons;
    double electronclsize = m_ElectronClusterSize;

    int nClus = (int)std::ceil(nElectrons / m_ElectronClusterSize);
    if (nClus < m_MinNumberOfElCluster) {
        electronclsize = nElectrons / m_MinNumberOfElCluster;
        if (electronclsize < 1.0) { electronclsize = 1.0; }
        nClus = (int)std::ceil(nElectrons / electronclsize);
    }

    // Empty and resize the electron-cluster vectors.
    m_LongDiff.clear();
    m_TransDiff1.clear();
    m_TransDiff2.clear();
    m_nElDiff.clear();
    m_nEnDiff.clear();
    m_ArrivalTime.clear();
    m_LongDiff.resize(nClus);
    m_TransDiff1.resize(nClus);
    m_TransDiff2.resize(nClus);
    m_nElDiff.resize(nClus, electronclsize);
    m_nEnDiff.resize(nClus);
    m_ArrivalTime.resize(nClus);

    m_nElDiff.back() = nElectrons - (nClus - 1) * electronclsize;

    for (size_t xx = 0; xx < m_nElDiff.size(); ++xx) {
      if (nElectrons > 0)
        m_nEnDiff[xx] = a_energy / nElectrons * m_nElDiff[xx];
      else
        m_nEnDiff[xx] = 0.;
    }

    double averagetransversePos1  = 0.5 * (**m_xStart + **m_xEnd);
    double averagetransversePos2  = 0.5 * (**m_yStart + **m_yEnd);
    double averagelongitudinalPos = 0.5 * (**m_zStart + **m_zEnd);

    // Transverse diffusion of the electron clusters.
    if (TDiffSig > 0.0) {
        for(int i=0;i<nClus;i++) {
	  m_TransDiff1[i] = gRandom->Gaus(averagetransversePos1, TDiffSig);
	  m_TransDiff2[i] = gRandom->Gaus(averagetransversePos2, TDiffSig);
        }
    }
    else {
        m_TransDiff1.assign(nClus, averagetransversePos1);
        m_TransDiff2.assign(nClus, averagetransversePos2);
    }

    // Longitudinal diffusion of the electron clusters. 
    if (LDiffSig > 0.0) {
        double sample_sigL = 0.0;

        for(int i=0;i<nClus;i++) {
	  sample_sigL = gRandom->Gaus(0.0,  LDiffSig);
	  m_ArrivalTime[i] = (DriftDistance + sample_sigL) * m_RecipDriftVel;
	  m_LongDiff[i]  = averagelongitudinalPos + sample_sigL;
        }   
    }
    else {
        m_LongDiff.assign(nClus, DriftDistance);
        m_ArrivalTime.assign(nClus, DriftDistance * m_RecipDriftVel);
    }

    // energy, xyzt
    return std::make_tuple(m_nEnDiff, m_nElDiff, m_TransDiff1, m_TransDiff2, m_LongDiff, m_ArrivalTime);
  }

} // namespace gramsdetsim
