# TODO 

As of 22-May-2021:

- Multi-threaded execution
   - gramsg4 crashes if -t is set higher than 1.
   - Test the saving and restoring of random-number states.

- Scintillator hits
   - Requires a two-way map of strip geometry to (x,y,z).

- Improve SimpleAnalysis.C
   - Perhaps a concrete example of turning the LAr hits to an image format for ML studies.
   
- ROOT 
   - Ntuples will only go so far. 
   - Some form of structured TTrees would be better for MC truth information. 
   - Can ROOT read the Geant4 GDML file directly?
      - Answer: As of ROOT 6.20, the answer is no. It must be parsed with --gdmlout first.

## Resolved issues:

- UI display
   - Does OpenGL support display of hit/track information on mouse hover?
      - Answer: OpenGL does not. There are other visualization formats that do (e.g., HepRep) but it's probably not worth forcing everyone to use them. 
   - Can we add some kind of axis labels?
      - Answer: Yes. The G4 macro command is "/vis/scene/add/axes 0 -2.5 0 1 m". See mac/vis.mac for details.
