// SampleFromPowerLaw.h
// 06-Feb-2022 WGS
//
// External definition of a function that randomly samples from a
// power-law distribution.
//
#ifndef SampleFromPowerLaw_H
#define SampleFromPowerLaw_H

namespace gramssky {
  extern double SampleFromPowerLaw(const double photonIndex, 
				   const double energyMin,
				   const double energyMax);
}

#endif // SampleFromPowerLaw_H
