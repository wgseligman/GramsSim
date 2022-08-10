# HISTORY

A description of major development milestones. The most recent are at
the top. For a detailed list of all changes (basically, every time
someone typed "git commit"), see the output of "git log".

Aug-2022

   - Switch to using the "cube" geometry (the LAr TPC is roughly cube-shaped)
     instead of the "flat" geometry; the latter is archived in gdml/grams-flat.gdml. 
   - The units for LAr TPC step size in GramsG4 now from the LengthUnit parameter
     in options.xml.
   - Fixed a tricky issue in Options.cc when compiled on a Macintosh. 

Apr-2022

   - GramsG4 will now compile for both the latest Geant4 11.0, and earlier G4 versions such as Geant4.10.5 and 4.10.7. This requires several `#if G4VERSION_NUMBER...` several statements in the code; let's hope this doesn't get too confusing. 

Mar-2022:

   - Added --run and --startEvent options to both GramsSky and GramsG4. These options let the user set the run number and the starting event number for the respective output files. 

Jan-2022:

   - Added first pass for GramsSky.
   - Added links to the Markdown documentation in .md files. Now, when viewing files in a Markdown viewer such as the GitHub web page, the user can click on a link like [`GramsG4/README.md`](GramsG4/README.md) to view that file directly. 
   - Improved the build process so that it can continue if the libraries for Geant4, HepMC3, and/or FITSIO/HEALPix cannot be detected on the system. 
   - `healpix-maps.cc` is a simple bit of code that tests reading HEALPix maps from a FITS file.
   - Added unit options to Options XML file and modified GramsSky and GramsG4 to insure consistency of units between the programs. 

Dec-2021:

   - Created [`scripts/gdmlsearch.cc`](scripts/gdmlsearch.cc) to provide an example of how to search through the geometry of a ROOT-compatible GDML file. 
   

Nov-2021:

   - Create a "skeleton" for GramsDetSim.
   - Restructure the directories. Each program now has its own directory with a README: GramsG4, GramsDetSim. The overall repository is now GramsSim.
   - Created util::WriteOptions, to save a program's options in its output file.

Oct-2021:

   - Replace the wires with a tile anode in the GDML file. 
   - The SiPMs can no longer be on the high-z side of the anode. Move the SiPMs to the middle of the high-x side of the cell until we determine a better placement. 
   
Sep-2021:

   - The build process now works on Mac OS X. 

Aug-2021:

   - Some HepMC3 packages don't provide the option for compiling HepMC3 with ROOT I/O enabled; for example, you can't turn on ROOTIO in the conda HepMC3 package. To allow for this, GramsG4 was revised so it will compile without errors if HepMC3 ROOTIO is not present. 
   - Same some disk space by storing position variables (x,y,z) as single-precision instead of double-precision. 
   - Expand the documentation in the `mac/` directory to include a discussion of visualization. 
   - Include some issues with large-volume running (e.g. a description of `rngseed`) in [`README.md`](GramsG4/README.md).

Jul-2021:

   - Added another example program, RestructuredEdx.
   - Fixed issue with compiled ROOT-based HepMC3. (It required making sure that ROOT, HepMC3, and Geant4 were all compiled with the same version of the GCC compiler.)
   - Multi-threaded execution no longer causes the program to crash. (The solution was to use G4's mutex mechanism in GramsG4WriteNtupleAction.cc.) 

Jun-2021: 

   - Increased the z-depth of the LArTPC to 30cm. 
   - Tweaked compilation process; now assumes that ROOT and Geant4 were compiled with the same C++ version (instead of imposing C++17).
   - Added more example programs (dEdxExample and HitRestructure).

Mar-2021:

   - Added the concept of "Identifiers"; see [`grams.gdml`](grams.gdml) for a detailed explanation. 
   - Include scintillator hits in the output n-tuple.
   - Updated to use/require latest versions of ROOT (6.22.06) and Geant4 (10.7).
   - Remove ROOT-based HepMC3 due to a compilation issue. 

Feb-2021:

   - New files in the [`mac/`](mac/) sub-directory show how to add menus to the Geant4 visualizer. 
   
   - A "breaking change" to the output file format: Include full track trajectories. Previously only the start and end values for the kinematics were included in the output n-tuple. 
   
Jan-2021:

   - Added the ability to read primary events from files using HepMC3. 
   
Aug-2021:

   - Use Geant4's General Particle Source (GPS) to generate/control primary events.
   
May-2021:

   - Initial set-up of GramsG4.
   