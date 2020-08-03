# Example GramsG4 macro files

To get you started, here are examples of how to set up event
generation and the interactive display for GramsG4. To use one of
these files, e.g., mac/test.mac:

    ./gramsg4 --macrofile mac/test.mac

To run an interactive job with, e.g., mac/vis.mac

    ./gramsg4 --ui --uimacrofile mac/vis.mac

See README.md in the parent directory for more information. 

`run.mac` - shoots 20 1-MeV gammas along the z-axis, y-axis, and x-axis.

`batch.mac` - shoots 1000 1-MeV gamma along the z-axis.

`sky.mac` - shoots 1000 gammas from an imaginary sphere surrounding
the detector (the 'sky') focused on the center of the TPC. The energy
spectrum comes from a histogram (see scripts/Hist2Text.C).

`vis.mac` - setup for interactive display. Particles are aimed along
the negative z-direction randomly across the top face of the inner
detector.

`vis-sphere.mac` - another interactive display. The particles are
generated on the surface of an imaginary sphere surrounding the
detector (the 'sky') and are focused on the center of the TPC. The
energy spectrum comes from a histogram (see scripts/Hist2Text.C).

`debug-geom.mac` - run Geant4's geometry validation (takes a long time!)
