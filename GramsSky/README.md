# GramsSky

This program is a simulation of particles coming from spherical (sky) backgrounds around the GRAMS detector. Using the [options XML file](../util/README.md), the user can specify position and energy distributions coming from a "celestial sphere" around the detector. The output consists of events in [HepMC3][10] format, which can be used as input to [GramsG4](../GramsG4/README.md). 

[10]: https://gitlab.cern.ch/hepmc/HepMC3

This is similar to the [General Particle Source][62] available in [Geant4][60]. The principle difference in the random "translation" of the generated particle to simulate that it was generated at "infinity" (or at least several light-years away). Understanding that process is crucial to supply meaningful [options](../options.xml) to the program, so it will be addressed first.

[60]: https://geant4.web.cern.ch/
[62]: http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/GettingStarted/generalParticleSource.html

The input and output units for the parameters described below are determined by parameters __`LengthUnit`__ and __`EnergyUnit`__. Normally these options are in the `<global>` section of the options XML file. 

## The geometry of GramsSky generation

![Sky Diagram](SkyDiagram.jpg)

These are the steps taken to generate an event in GramsSky. In the following description, names in __`this format`__ are parameters in the [options XML file](../options.xml). 

   - The particle is generated at an original (x,y,z) as determined by the position algorithm selected by the user; see the list of possible algorithms below. Note that the particle's origin is on the surface of a sphere of radius __`RadiusSphere`__ and center __`OriginSphere`__ in the detector coordinate system.

      - While nominally __`OriginSphere`__ would be the center of the detector, the definition of the "center" of the GRAMS detector may depend on the type of study being done: As of 11-Jan-2022, the origin of the z-axis is set to be the bottom of the anode plate (to be partially compatible with [LArSoft][15]). In this coordinate system and with the geometry in [`grams.gdml`](../grams.gdml) as of 11-Jan-2022, the center of the LArTPC is (0,0,-14.783); the center of the overall detector is at (0,0,45.217).
   
[15]: https://github.com/LArSoft
   
   - The particle's energy _E_ is generated according to the energy algorithm selected by the user; again, see below.
   
   - The magnitude of the particle's momentum is calculated by _p = sqrt(E^2 - m^2)_, where _m_ is the particle's mass from ROOT's [TParticlePDG][10] database based on the value of __`PrimaryPDG`__.
   
[10]: https://root.cern.ch/doc/master/classTDatabasePDG.html
[11]: https://pdg.lbl.gov/2007/reviews/montecarlorpp.pdf

   - __`MapDirection`__ is the direction of "north pole" of the map with respect to the z-axis of the detector coordinate system; this parameter is necessary because the sky maps are normally oriented with (&theta;,&phi;)=(90&deg;,0) at the galactic north pole, while a detector will have some other orientation with respect to the earth's surface. 
   
      __`MapDirection`__ is used to rotate the original (x,y,z) in the celestial sphere coordinate system to (x',y',z') in the detector coordinate system. 
    
   - The momentum direction is taken to be of magnitude _p_ and in the direction of the center of celestial sphere at __`OriginSphere`__. Call this (px,py,pz).
   
   - The direction of (x',y',z') is assumed to be located at a very large distance from the detector. Therefore, all particles from that source appear to be coming from that direction independent of the location within the detector. To simulate this effect, the source of the particles is taken to be a disc with radius __`RadiusDisc`__ that is tangent to the celestial sphere at (x',y',z').
   
      - In the XML file, if __`RadiusDisc`__ less than or equal to zero, the program will use the value of __`RadiusSphere`__. Note that, given the [grams.gdml](../grams.gdml) file as of 11-Jan-022, a radius of 267 _cm_ is sufficient to cover the entire GRAMS outer detector; a radius of 108 _cm_ will cover the LArTPC. (Make sure the value of __`OriginSphere`__ is consistent with the __`RadiusDisc`__ you choose.)

   - The vector with origin (x',y',z') and vector (px,py,pz) is translated along the plane of the disc to a random (r,&theta;). These translated values are what's written as the particle's information in the `gramssky` output. 
     
      - Note that `gramssky` does _not_ check to make sure that the translated vertex is within the world volume defined by the [GDML](../grams.gdml) file. If a particle's vertex is outside the world volume, [`gramsg4`](../GramsG4) will not crash, but it will skip over the particle with a warning message. It's a good idea to verify that the celestial sphere and any potential tangent discs do not lie wholly or partially outside the world volume. One possible fix is to increase the size of the world volume in [the GDML file](../grams.gdml), at a cost of a modest increase in the execution time of `gramsg4`.

## Position generators

These are the available generators for (x,y,z) as 11-Jan-2022. They are selected by option __`PositionGeneration`__. 

### Fixed point

If __`PositionGeneration`__ has the value `Fixed`, then the user must supply the option __`PositionGeneration`__ with a vector of values (x,y,z). All the particles are generated with an initial vertex at that point, then undergo the geometric transformations described above. 

   - The position (x,y,z) will be translated to the surface of the celestial sphere automatically. There's no need to do elaborate calculations to make sure (x,y,z) is exactly __`RadiusSphere`__ from __`OriginSphere`__; the program takes care of that. 

### Isotropic on the inner surface of the celestial sphere

This position generator is selected by setting __`PositionGeneration`__ to `Iso`. The controlling options are:

   - __`ThetaMinMax`__: the value is a two-element vector with minimum and maximum &theta;.
   - __`PhiMinMax`__: the value is a two-element vector with minimum and maximum &phi;.

Setting __`ThetaMinMax`__ to `(0,1.571)` and __`PhiMinMax`__ to `(0,6.283)` will generate particle isotropically in a hemisphere above the detector (subject to the geometry transforms described above).

## Energy generators

These are the available generators for _E_ as 11-Jan-2022. They are selected by option __`EnergyGeneration`__. 

### Fixed energy

As the name implies, if __`EnergyGeneration`__ has the value `Fixed` then the value of _E_ comes from the option `FixedEnergy`.

### Uniform energy distribution

If __`EnergyGeneration`__ is `Flat`, then _E_ is generated uniformly between the values of parameters __`EnergyMin`__ and __`EnergyMax`__.

### Gaussian energy distribution

If __`EnergyGeneration`__ is `Gaus`, then _E_ is generated according to a Gaussian distribution with mean of __`GausMean`__ and width __`GausWidth`__. 

   - Note that the limits in parameters __`EnergyMin`__ and __`EnergyMax`__ still apply to this generator. This is to keep the value of _E_ from going negative, which would cause problems in both `gramssky` and `gramsg4`.
   
### Black-body energy distribution

For __`EnergyGeneration`__ = `BlackBody`, _E_ is generated according to a black-body distribution with radiation temperature (kT) given by parameter __`RadTemp`__. 

   - Again, the limits in parameters __`EnergyMin`__ and __`EnergyMax`__ still apply to this generator.
   
### Power-law energy distribution

If __`EnergyGeneration`__ is `PowerLaw`, then _E_ is generated according to a power-law distribution with a photon index given by __`PhotonIndex`__.

   - The limits in parameters __`EnergyMin`__ and __`EnergyMax`__ still apply to this generator.
   
### Energy distribution from a histogram

For __`EnergyGeneration`__ = "Hist", the program expects two parameters: __`HistFile`__ with the name of a ROOT file, and __`HistName`__ with the name of a histogram within that file. 

   - Both __`HistFile`__ and __`HistName`__ can contain path specifiers for directories either on the computer system or within the ROOT file. 
   
   - For this generator, the values of __`EnergyMin`__ and __`EnergyMax`__ are ignored. Instead, the energy limits effectively come from the bin limits of the histogram. 
   