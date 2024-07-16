// 21-Sep-2022 Satoshi Takashima, William Seligman
// Implement a diffusion model calculation.

// Most of this code originally came from LArSoft:
// https://github.com/LArSoft/larsim/blob/develop/larsim/ElectronDrift/SimDriftElectrons_module.cc

#include "DiffusionModel.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/

// From GramsDataObj
#include "MCLArHits.h"
#include "ElectronClusters.h"

// ROOT includes
#include "Math/Vector4D.h"
#include "TRandom.h"

// C++ includes
#include <iostream>
#include <cmath>
#include <vector>

namespace gramsdetsim {

  // Constructor: Initializes the class.
  DiffusionModel::DiffusionModel()
  {
    // Get the options class. This contains all the program options
    // from options.xml and the command line.
    auto options = util::Options::GetInstance();

    // Fetch the options we want from the class. 

    options->GetOption("verbose",            m_verbose);
    options->GetOption("debug",              m_debug);

    options->GetOption("MeVToElectrons",        m_MeVToElectrons);
    options->GetOption("LongitudinalDiffusion", m_LongitudinalDiffusion);
    options->GetOption("TransverseDiffusion",   m_TransverseDiffusion);
    options->GetOption("ElectronClusterSize",   m_ElectronClusterSize);
    options->GetOption("ReadoutPlaneCoord",     m_readout_plane_coord);
    options->GetOption("ElectronDriftVelocity", m_DriftVel);
    options->GetOption("MinNumberOfElCluster",  m_MinNumberOfElCluster);

    m_RecipDriftVel = 1.0 / m_DriftVel;

    if (m_verbose || m_debug) {
      std::cout << "gramsdetsim::DiffusionModel - "
		<< "LongitudinalDiffusion= " << m_LongitudinalDiffusion
		<< " TransverseDiffusion= " << m_TransverseDiffusion
		<< " ElectronClusterSize=" << m_ElectronClusterSize 
		<< " m_MeVToElectrons=" << m_MeVToElectrons
		<< " m_DriftVel=" << m_DriftVel
		<< " m_RecipDriftVel=" << m_RecipDriftVel
		<< std::endl;
    }
  }

  // Note that the "a_" prefix is a convention to remind us that the
  // variable was an argument in this method.

  // Also note that clusterID is an int&. That's to make sure that
  // when we increment the clusterID in this routine, it's reflected
  // in the calling routine. That's to keep the clusterID increasing
  // over all the hits in the events.
  
  std::vector< grams::ElectronCluster > 
      DiffusionModel::Calculate(double a_energy, 
				const grams::MCLArHit& a_hit,
				int& a_clusterID ) {

    // Note that we're drifting along the z-axis, so the cluster
    // positions on the 1 and 2 axes (the x and y axes) are diffused
    // using "transverse" rules, while the cluster position on the
    // z-axis is drifting with "longitudinal" diffusion. For other
    // detector setups or coordinate systems, the following code must
    // be adjusted.

    double z_mean = 0.5 * (a_hit.StartZ() + a_hit.EndZ());

    double DriftDistance = m_readout_plane_coord - z_mean;
    double mean_TDrift = std::abs(DriftDistance * m_RecipDriftVel);
    double SqrtT = std::sqrt(mean_TDrift);
  
    double fLDiff_const = std::sqrt(2. * m_LongitudinalDiffusion);
    double fTDiff_const = std::sqrt(2. * m_TransverseDiffusion);
  
    double LDiffSig = SqrtT * fLDiff_const;
    double TDiffSig = SqrtT * fTDiff_const;

    if ( m_debug ) {
      std::cout << "gramsdetsim::DiffusionModel::Calculate - "
		<< " DriftDistance=" << DriftDistance
		<< " mean_TDrift=" << mean_TDrift
		<< " SqrtT=" << SqrtT
		<< " fLDiff_const=" << fLDiff_const
		<< " fTDiff_const=" << fTDiff_const
		<< " LDiffSig=" << LDiffSig
		<< " TDiffSig=" << TDiffSig
		<< std::endl;
    }
  
    // Break up the ionization energy into separate electron clusters.

    // Compute total number of electrons.
    const double nElectrons = a_energy * m_MeVToElectrons;
    double electronclsize = m_ElectronClusterSize;

    // Compute the number of electron clusters. This will depend on
    // m_ElectronClusterSize, the number of electrons per cluster,
    // which comes from the options XML file.
    int nClus = (int)std::ceil(nElectrons / m_ElectronClusterSize);

    // Adjust values if the number of electron cluster calculated
    // above is less than the user-supplied minimum number.
    if (nClus < m_MinNumberOfElCluster) {
      electronclsize = nElectrons / m_MinNumberOfElCluster;
      if (electronclsize < 1.0) { electronclsize = 1.0; }
      nClus = (int)std::ceil(nElectrons / electronclsize);
    }

    // The vector of electron clusters that will be returned by this
    // model.
    std::vector< grams::ElectronCluster > vecCluster(nClus);

    double averagetransversePos1  = 0.5 * (a_hit.StartX() + a_hit.EndX());
    double averagetransversePos2  = 0.5 * (a_hit.StartY() + a_hit.EndY());
    double averagelongitudinalPos = z_mean;

    // For each cluster:
    int clusterCount = 0;
    for ( auto& cluster : vecCluster ) {
      cluster.trackID = a_hit.trackID;
      cluster.hitID = a_hit.hitID;
      // An arbitrary assignment of cluster ID, incremented across all
      // the clusters in the event.
      cluster.clusterID = a_clusterID++;
      cluster.numElectrons = electronclsize;

      // For most of the clusters, the number of electrons in each
      // cluster is the same (electronclsize). However, the last
      // cluster will contain the "leftover" electrons after dividing
      // the total number of electrons by the electron-cluster size.
      clusterCount++;
      if ( clusterCount == vecCluster.size() )
	cluster.numElectrons = nElectrons - (nClus - 1) * electronclsize;

      if (nElectrons > 0)
        cluster.energy = a_energy / nElectrons * cluster.numElectrons;
      else
        cluster.energy = 0.;

      double xDiff, yDiff, zDiff, tDiff;

      // Transverse diffusion of the electron clusters.
      if (TDiffSig > 0.0) {
	xDiff = gRandom->Gaus(averagetransversePos1, TDiffSig);
	yDiff = gRandom->Gaus(averagetransversePos2, TDiffSig);
      }
      else {
	xDiff = averagetransversePos1;
	yDiff = averagetransversePos2;
      }

      // Longitudinal diffusion of the electron clusters. 
      if (LDiffSig > 0.0) {
	double sample_sigL = gRandom->Gaus(0.0,  LDiffSig);
	tDiff = (DriftDistance + sample_sigL) * m_RecipDriftVel;
	zDiff  = averagelongitudinalPos + sample_sigL;
      }   
      else {
	zDiff = DriftDistance;
	tDiff = DriftDistance * m_RecipDriftVel;
      }

      cluster.position 
	= ROOT::Math::XYZTVector( xDiff, yDiff, zDiff, tDiff );

    } // for each cluster

    // Return the vector containing the per-cluster values (units
    // defined in the options XML file):
    return vecCluster;
  }

} // namespace gramsdetsim
