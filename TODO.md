# TODO 

As of 03-Aug-2020:

- Multi-threaded execution
   - Understand the effects of multiple threads on ROOT outputs (probably need to upgrade to Geant4 10.6).
   - Test the saving and restoring of random-number states.

- Scintillator hits
   - Requires a two-way map of strip geometry to (x,y,z).
   - Create an additional ntuple with hits in the scintillator strips. 

- Improve SimpleAnalysis.C
   - Perhaps a concrete example of turning the LAr hits to an image format for ML studies.

- UI display
   - Does OpenGL support display of hit/track information on mouse hover?
   
- Improve ROOT outputs
   - Ntuples will only go so far. 
   - Some form of structured TTrees would be better for MC truth information. 
