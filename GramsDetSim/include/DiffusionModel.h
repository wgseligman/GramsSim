// 26-Nov-2021 WGS 

// Implement a model of diffusion in the LAr for the GRAMS
// detector.

#ifndef DiffusionModel_h
#define DiffusionModel_h

#include <vector>
#include <tuple>

// ROOT includes
#include "TTreeReader.h"
#include "TTreeReaderValue.h"


namespace gramsdetsim {

  class DiffusionModel
  {
  public:

    // Constructor. It requires a pointer to the TTreeReader that's
    // used to read the ntuple.
    DiffusionModel( TTreeReader* );

    // Destructor. It's standard practice to declare all destructors
    // as virtual in C++, though it's only relevant if we were ever to
    // create a class that inherits from this one.
    virtual ~DiffusionModel();

    // The meat of this routine: Given the variables in the current
    // entry of the ntuple, calculate a revised energy.
     std::tuple<std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double> >
     Calculate(double energy);

  private:

    // Note that it's a convention to prefix variables defined in a
    // class header with "m_" as a visual clue in the ".cc" file that
    // the variable is defined in this ".h" file.

    // Used to read the ntuple.
    TTreeReader* m_reader;

    // These will point to the variables in the ntuple that we need
    // for this calculation.
    TTreeReaderValue<float>* m_xStart;
    TTreeReaderValue<float>* m_yStart;
    TTreeReaderValue<float>* m_zStart;
    TTreeReaderValue<float>* m_xEnd;
    TTreeReaderValue<float>* m_yEnd;
    TTreeReaderValue<float>* m_zEnd;

    // The constants required for the calculation. We'll get those as
    // program options.
    double m_MeVToElectrons;
    double m_LongitudinalDiffusion;
    double m_TransverseDiffusion;
    double m_readout_plane_coord;
    double m_DriftVel;
    double m_RecipDriftVel;
    int m_ElectronClusterSize;
    int m_MinNumberOfElCluster;

    // The output arrays from this model. These vectors contain one
    // entry for each electron cluster. Units ultimately come from the
    // options XML file. 
    std::vector<Double_t> m_LongDiff;    // Amount of longitudinal diffusion.
    std::vector<Double_t> m_TransDiff1;  // Amount of cluster diffusion along the "1-axis"
    std::vector<Double_t> m_TransDiff2;  // Amount of cluster diffusion along the "2-axis"
    std::vector<Double_t> m_nElDiff;     // Number of electrons in each cluster.
    std::vector<Double_t> m_nEnDiff;     // Amount of energy in each cluster. 
    std::vector<Double_t> m_ArrivalTime; // The arrival time of a cluster at the anode.

    // Save the verbose and debug options.
    bool m_verbose;
    bool m_debug;
  };

} // namespace gramsdetsim

#endif // DiffusionModel_h
