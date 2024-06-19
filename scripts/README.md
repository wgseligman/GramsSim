# GramsSim scripts

This directory contains utility scripts to help work with GramsSim
inputs and outputs. Some of the programs are provided solely to teach
programming concepts for folks new to:

- handling ntuples
- working with dataframes
- C++
- ROOT
- GDML (Geant4 geometry)
- HepMC3 (particle-physics file format)

For examples related to working with Geant4 and GramsG4, see [`GramsSim/mac`](../mac).

- If you add scripts/programs to this directory that require [HepMC3][10], [cfitsio][17], or [healpix][18], be sure to edit the [CMakeLists.txt](./CMakeLists.txt) file. This is to prevent those programs from being compiled on systems that don't have these libraries installed.  

[10]: https://gitlab.cern.ch/hepmc/HepMC3
[17]: https://heasarc.gsfc.nasa.gov/fitsio/
[18]: https://healpix.jpl.nasa.gov/

## Examples

`SimpleAnalysis.C` - a very simple example of how to look at the trees
produced by gramsg4 using a ROOT macro. Note that this macro depends
on [GramsSim/rootlogon.C](../rootlogon.C), which loads the
[GramsDataObj](../GramsDataObj) dictionary when you start an
interactive ROOT session.

`dEdxExample.cc` - An example of how to read a map-based branch in one
of the GramsSim output files.  There are lots of detailed comments in
here, to point the way to users developing code for their own tasks.

`dEdxExample.py` - The same as above, but in Python.

`RadialDistance.py` - a more realistic example of how to look at the
trees produced by gramsg4.

`Hist2Text.C` - converts a ROOT histogram into the simple text format
used by Geant4's General Particle Source system. Any `.root` or `.txt`
files in this directory are used as example inputs and outputs for
`Hist2Text.C`.

`gdmlsearch.cc` - an example of how to use ROOT to search through a
geometry specified in GDML and extract information from it.

## HepMC3 tests

The following C++ programs are intended as simple examples of handling
event creation in HepMC3. Note that the examples in
${HepMC3_ROOT_DIR}/share/doc/HepMC3/examples are better illustrations,
but they don't necessarily produce outputs that are useful for
GramsG4.

The compiled programs are put into the bin/ sub-directory of your
GramsG4 working/build directory. To execute them, you'll want 
something like:

    # Go to your GramsG4 build directory
    ./bin/hepmc-grams-example

The programs are:

`hepmc-grams-example` - Creates a few example events "by hand" and
writes them to a file.

`hepmc-convert` - Converts one HepMC3-format file to another HepMC3
format. See the main README.md file for a list of available formats
and other notes.

`example.hepmc3` - a file of a few events in HepMC3 format.


