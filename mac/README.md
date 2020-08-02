# Example GramsG4 macro files

To get you started, here are examples of how to set up event
generation and the interactive display for GramsG4. To use one of
these files, e.g., mac/test.mac:

    ./gramsg4 --macrofile mac/test.mac

To run an interactive job with, e.g., mac/vis.mac

    ./gramsg4 --ui --uimacrofile mac/vis.mac

See README.md in the parent directory for more information. 

`run.mac` - shoots 1-MeV gammas along the z-axis, y-axis, and x-axis.

`vis.mac` - setup for interactive display. 

`debug-geom.mac` - run Geant4's geometry validation (takes a long time!)

`batch.mac` - shoots 1000 1-MeV gamma along the z-axis.
