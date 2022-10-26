# TODO 

As of 25-Aug-2021:

- Multi-threaded execution
   - Test the saving and restoring of random-number states.

- Scintillator hits
   - It would be handy to provide a two-way map of strip geometry to (x,y,z).
   
- ROOT 
   - Ntuples have their limits. Some form of structured TTrees might be better for MC truth information. 
   - Can ROOT read the Geant4 GDML file directly?
      - Answer: As of ROOT 6.20, the answer is no. It must be parsed with `./gramsg4 --gdmlout` first.

## Resolved issues:

- Multi-threaded execution no longer causes the program to crash. 

- UI display
   - Does OpenGL support display of hit/track information on mouse hover?
      - Answer: OpenGL does not. HepRep does, and we now provide an example of how to make this work. 
   - Can we add some kind of axis labels?
      - Answer: Yes. The G4 macro command is `/vis/scene/add/axes 0 -2.5 0 1 m`. See [`mac/vis.mac`](mac/vis.mac) for details.
