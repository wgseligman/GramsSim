#include "IsotropicPositionGenerator.h"
#include "PositionGenerator.h" // For GetTransform
#include "EnergyGenerator.h" 
#include "TransformCoordinates.h"
#include "Options.h" // in util

// ROOT includes
#include "TRandom.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"

// C++ includes
#include <cmath>
#include <vector>
#include <memory>
#include <iostream>

namespace gramssky {

  IsotropicPositionGenerator::IsotropicPositionGenerator()
    : PositionGenerator()
  {
    // Get the values from the Options XML file.
    auto options = util::Options::GetInstance();

    // Debug flag from Options.
    bool debug;
    options->GetOption("debug",debug);

    std::vector<double> thetaMinMax;
    options->GetOption("ThetaMinMax",thetaMinMax);
    if ( thetaMinMax.size() != 2 ) {
      std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
		<< "Iso: ThetaMinMax must be a vector of exactly two values"
		<< std::endl;
      exit(EXIT_FAILURE);
    }
    m_cosThetaMin = std::cos(thetaMinMax[0]);
    m_cosThetaMax = std::cos(thetaMinMax[1]);

    options->GetOption("PhiMinMax",m_phiRange);
    if ( m_phiRange.size() != 2 ) {
      std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
		<< "Iso: PhiMinMax must be a vector of exactly two values"
		<< std::endl;
      exit(EXIT_FAILURE);
    }

    options->GetOption("RadiusSphere",m_radius);

    // Get the PDG code; from that get the mass of the particle.
    options->GetOption("PrimaryPDG",m_PDG);
    TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(m_PDG);
    m_mass = 0.;
    if ( particle != NULL ) {
      m_mass = particle->Mass();
      if (debug) {
	std::cout << "IsotropicPositionGenerator: PDG code "
		  << m_PDG << " found in ROOT particle database, mass = "
		  << m_mass << std::endl;
      }
    }
    else {
      if (debug) {
	std::cout << "IsotropicPositionGenerator: PDG code "
		  << m_PDG << " not found in ROOT particle database! "
		  << " mass set to 0" << std::endl;
      }
    }
  }

  IsotropicPositionGenerator::~IsotropicPositionGenerator()
  {}

  std::shared_ptr<ParticleInfo> IsotropicPositionGenerator::Generate()
  {
    // Create a new particle.
    auto particle = std::make_shared<ParticleInfo>();

    // Use the values from the Options XML file.
    particle->SetPDG(m_PDG);

    // Set position based on random theta and phi values chosen from
    // the ranges. For an isotropic distribution, we want to
    // distribute evenly in cos(theta).
    double cosTheta = m_cosThetaMin 
      + (m_cosThetaMax - m_cosThetaMin) * gRandom->Uniform();
    double phi = m_phiRange[0] 
      + (m_phiRange[1] - m_phiRange[0]) * gRandom->Uniform();

    double sinTheta = std::sqrt( 1.0 - cosTheta*cosTheta );

    // Convert to cartesian coordinates.
    double x = m_radius * sinTheta * std::cos(phi);
    double y = m_radius * sinTheta * std::sin(phi);
    double z = m_radius * cosTheta;

    // Save the results.
    particle->SetX(x);
    particle->SetY(y);
    particle->SetZ(z);

    // GetEnergyGenerator is defined in PositionGenerator.h
    double energy = GetEnergyGenerator()->Generate();
    particle->SetE(energy);

    // We don't have to set correct particle direction here, since the
    // Transform routine will rotate the momentum vector to point in
    // the direction of the detector's center. But we do want to get
    // the magnitude right.
    double momentum = std::sqrt( energy*energy - m_mass*m_mass );
    particle->SetPz(-momentum);

    // Transform the particle position and direction from the
    // celestial sphere to the detector coordinates, applying a random
    // shift to simulate a flux from infinity.
    auto transformed = GetTransform()->Transform( particle );

    return transformed;
  }

} // namespace gramssky
