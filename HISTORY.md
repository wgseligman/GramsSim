# HISTORY

A description of major development milestones. The most recent are at
the top. For a detailed list of all changes (basically, every time
someone typed "git commit"), see the output of "git log".

Jun-2021: 

   - Increased the z-depth of the LArTPC to 30cm. 

Mar-2021:

   - Added the concept of "Identifiers"; see grams.gdml for a detailed explanation. 
   - Include scintillator hits in the output n-tuple.
   - Updated to use/require latest versions of ROOT (6.22.06) and Geant4 (10.7).
   - Remove ROOT-based HepMC3 due to a compilation issue. 

Feb-2021:

   - New files in the `mac/` sub-directory show how to add menus to the Geant4 visualizer. 
   
   - A "breaking change" to the output file format: Include full track trajectories. Previously only the start and end values for the kinematics were included in the output n-tuple. 
   
Jan-2021:

   - Added the ability to read primary events from files using HepMC3. 
   
Aug-2021:

   - Use Geant4's General Particle Source (GPS) to generate/control primary events.
   
May-2021:

   - Initial set-up of GramsG4.
   