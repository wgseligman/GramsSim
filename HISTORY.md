# HISTORY

A description of major development milestones. The most recent are at
the top. 

- For a detailed list of all changes (basically, every time
someone typed "git commit"), see the output of "git log".

- For a list of tagged commits (important milestones), use "git tag -n1".

- [More on git.](https://git-scm.com/book/en/v2)

Jul-2024

   - Create an include/ sub-directory within the build directory, and
     populate it with the header files that the scripts need. Refine
     the scripts their comments to remove "relative" directory
     references like `../GramsSim/GramsDataObj/include` to make life
     easier.

   - git tag -a v2.1 to reflect Mac OS compatibility relese. 

   - Improved rootlogon.C to be a bit more OS-independent. 

   - Refine the examples a bit. `AllFilesExample` (in both C++ and
     Python) is split into two programs: AllFilesExample and
     BacktrackExample. The first shows how to go "forward" in the data
     objects (tracks->hits->clusters->waveforms); the latter shows how
     to backtrack through those objects.

Jun-2024

   - The dictionary now works on Mac OS X; check footnote in [GramsSim/GramsDataObj/README.md](./GramsDataObj) for files to copy if you're working outside your build area. 

   - **MAJOR BREAKING CHANGE**: The n-tuples used in the previous GramsSim files have been
     replaced with a data-object ROOT-based TTree format. There is a new package,
     GramsDataObj, that contains all the data objects used by GramsSim. 
     
   - The examples in GramsSim/scripts and the overall documentation in README.md files
     was revised for this change. 
     
   - Read [GramsSim/GramsDataObj/README.md](./GramsDataObj) for the files to copy if you're working outside your build area. 

   - git tag -a v2.0 <-- The first tag assigned for the start of the
     tree-based output format.

   - From this point forward, tags will be assigned in the classic
     scheme: vX.Y.Z, where X is a breaking change, Y is a feature
     change, and Z is a minor or bug-fix change.

   - git tag -a v1.6 <-- The last tag assigned for the code that uses
     the older n-tuple output format.

Apr-2024

   - From a suggestion by Mihir Shetty: modified the Options
     processing to allow the user to specify lower and upper limits
     for numeric parameters in the Options XML file.
   - git tag -a v1.5.1

Mar-2024

   - Mihir Shetty changed the input parameters for GramsReadoutSim
     from the pixel sizes to the number of channels along the edge of
     the readout plane.

Feb-2024

   - Added an _incorrect_ pGRAMS detector description, just to get things started. 
   - GramsG4 now accumulates Cerenkov photons (in addition to the existing accumulation
     of scintillation photons). This new field is passed on and through GramsDetSim
     and GramsReadoutSim. 
   - git tag -a v1.5

Nov-2023

   - Mihir Shetty found a bug in gramsdetsim.cc. It's been fixed.

Oct-2023

   - A new feature is added to GramsG4: By default, a copy of the
     detector geometry is written to the output file as a TGeoManager
     object.
   - Every existing subsequent GramsSim job step (gramsdetsim, gramsreadoutsim, gramselecsim) 
     copies the geometry (as a TGeoManager object) from the input file to the output file.
   - Restructure the GDML file slightly to allow for a cryostat that is not a "skin-tight" fit
     around the TPC, and for a potential offset of the TPC from the geometric center of the
     cryostat. 
   - git tag -a v1.4-Options-Geometry

Sep-2023

   - The instructions for installing the external packages and
     working with branches have been revised to be consistent
     with working in AlmaLinux 9, the RHEL derivative that
     CERN and Fermilab have chosen as a successor to CentOS. 

May-2023

   - With the help of Hans Wenzel, improved specification of 
     LAr optical properties so that the Scintillation process
     works properly in Geant4.11. 

Dec-2022

   - Add showoptions script, so a user can quickly display the
     contents of the options ntuple. 

Nov-2022

   - Added GramsReadoutSim and GramsElecsim to the repository.
   - Revised Options class to better support saving and restoring
     options along an entire analysis chain. 
   - git tag -a v1.3-GramsReadoutSim-GramsElecSim

Oct-2022

   - Restructured the saving and loading of the option ntuple in a
     .root output file. Adding a feature to copy the option ntuple in
     the input file to the output file, to preserve a "history" of
     the options that were used. 
   - Multi-threaded saving and restoring of random numbers now works properly. 
   - Enable the control of physics and other parameters from within the G4
     macro file. This requires that /run/initialize be present in every
     top-level macro file.

Sep-2022

   - GramsDetSim functionally complete and documented (though the
     values of the parameters in options.xml will probably change).

Aug-2022

   - Fixed bugs in GDML file (the low-z scintillators were not being placed).
     Also simplified the GDML code to not require lots of different
     index variables. 
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
   - git tag -a v1.1-GramsSky
   - git tag -a v1.2-GramsDetSim

Dec-2021:

   - Created [`scripts/gdmlsearch.cc`](scripts/gdmlsearch.cc) to provide an example of how to search through the geometry of a ROOT-compatible GDML file. 
   

Nov-2021:

   - Create a "skeleton" for GramsDetSim.
   - Restructure the directories. Each program now has its own directory with a README: GramsG4, GramsDetSim. The overall repository is now GramsSim.
   - Created util::WriteOptions, to save a program's options in its output file.
   - git tag -a v1.0

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
   - git tag -a v0.5
   
Aug-2021:

   - Use Geant4's General Particle Source (GPS) to generate/control primary events.
   
May-2021:

   - Initial set-up of GramsG4.
   - git tag -a v0.1
