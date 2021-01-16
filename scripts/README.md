# GramsG4 scripts

This directory contains utility scripts to help work with GramsG4
inputs and outputs.

`SimpleAnalysis.C` - a very simple example of how to look at the
ntuples produced by gramsg4.

`Hist2Text.C` - converts a ROOT histogram into the simple text format
used by Geant4's General Particle Source system. Any `.root` or `.txt`
files in this directory are used as example inputs and outputs for
`Hist2Text.C`.

## HepMC3 tests

The following C++ programs are intended as simple examples of handling
event creation in HepMC3. Note that the examples in
${HepMC3_ROOT_DIR}/share/doc/HepMC3/examples are better illustrations,
but they don't necessarily produce outputs that are useful for
GramsG4.

These programs aren't included in the CMake compilation process,
mainly because the programmer (William Seligman) isn't facile enough
with CMake to add them to CMakeLists.txt. To compile the programs:

    prog=[program name]
    g++ ${prog}.cc \
       `root-config --cflags --libs` \
       `HepMC3-config --cflags --libs --rootIO` \
       -o ${prog}

The two programs are:

`hepmc-grams-example` - Creates a few example events "by hand" and
write them to a file.

`hepmc-convert` - Converts one HepMC3-format file to another HepMC3
format. See the main README.md file for a list of available formats
and other notes.

`example.hepmc3` - an file of a few events in HepMC3 format. It was
created with a combination of `hepmc-grams-example` (to generate a
`.root` file) and `hepmc-convert` (to convert it to a `.hepmc3` file).

