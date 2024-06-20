# TODO 

As of 16-Oct-2023:

- In GramsReadoutSim or GramsElecSim, we may have to model induced charge on adjacent pixels. 
- There's no trigger logic in GramsElecSim; this may have to wait until the optical and 
  veto detector code is implemented. 

- Cluster time 
   - In GramsDetSim, the cluster arrival time at the anode
     can be calculated to be negative in the DiffusionModel. This is
     probably a geometry issue, either in the GDML file definition or
     the value of ReadoutPlaneCoord in the options.xml file.
   - We'll probably investigate this further when we have a better
     idea of the final geometry of the GRAMS detector. 

- Scintillator hits
   - It would be handy to provide a two-way map of strip geometry to (x,y,z).
   
- ROOT 
   - Can ROOT read the Geant4 GDML file directly?
      - Answer: As of ROOT 6.20, the answer is no. It must be parsed with `./gramsg4 --gdmlout` first.

## Resolved issues:

- Analysis features
   - GramsG4 writes a ROOT TGeoManager version of the detector geometry to the output file. 
     Propagate this behavior so that subsequent job steps copy the geometry to their output as well. 
   - Resolved Oct-2023.

- Multi-threaded execution
   - Test the saving and restoring of random-number states. 
   - Resolved Oct-2022.
   
- Multi-threaded execution no longer causes the program to crash. 

- UI display
   - Does OpenGL support display of hit/track information on mouse hover?
      - Answer: OpenGL does not. HepRep does, and we now provide an example of how to make this work. 
   - Can we add some kind of axis labels?
      - Answer: Yes. The G4 macro command is `/vis/scene/add/axes 0 -2.5 0 1 m`. See [`mac/vis.mac`](mac/vis.mac) for details.

- ROOT
   - Ntuples have been replaced by structured TTrees for better-organized MC truth information. 
   - Resolved Jun-2024.
