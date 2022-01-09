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

## Examples

`SimpleAnalysis.C` - a very simple example of how to look at the
ntuples produced by gramsg4.

`RadialDistance.py` - a more realistic example of how to look at the
ntuples produced by gramsg4, this time in Python.

`dEdxExample.cc` - an example of how to link the output ntuples from
gramsg4 and use those ntuples for a physics calculation. There are
lots of detailed comments in here, to illustrate how to use features
of ROOT's RDataFrame.

`HitRestructure.cc` - an example of how to take the LArHits ntuple
from the gramsg4 output, which has one row per hit, and turn the
collection of hits for a given run/event/trackID into a single row
with vectors of hit information.

`RestructuredEdx.cc` - an example of how to read in an ntuple with
columns that contains vectors (eg., the output of HitRestructure) and
append a new column that also contains a vector.

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


