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

`crab.mac` - a crude approximation of gammas from the Crab Nebula
directed at the top of the GRAMS detector.

`crab-45.mac` - a crude approximation of gammas from the Crab Nebula
directed at the top of the GRAMS detector at 45-degree angle.

`vis.mac` - setup for interactive display. Particles are aimed along
the negative z-direction randomly across the top face of the inner
detector.

`vis-sphere.mac` - another interactive display. The particles are
generated on the surface of an imaginary sphere surrounding the
detector (the 'sky') and are focused on the center of the TPC. The
energy spectrum comes from a histogram (see scripts/Hist2Text.C).

`outline.mac` - just shows an outline of the detector, and is colored
so that it's more suitable for inclusion in talks. Give this one some
extra time to load, since it's turning off the visibility for a number
of volumes.

`switch-views.mac` - a fancy demonstration of how to switch
visualization parameters via menu at the top of the G4
visualizer. This macro file invokes the `draw-axes.mac`,
`angle-view.mac`, `{x,y,z}-views.mac` "sub-files" from within its
menu.

`debug-geom.mac` - run Geant4's geometry validation (takes a long time!)

`hepmc3.mac` - how to run the simulation using an input file of
generated events.

`vis-hepmc3.mac` - run the simulation with both an input events file
and visualization.

