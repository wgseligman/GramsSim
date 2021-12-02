# GramsDetSim

## Overview

This program will read in an ntuple file created by the GramsG4
simulation and create a "friend ntuple" to the GramsG4's "hits"
ntuple. In other words, the ntuple created by this program will
contain, row-by-row, the hit energy adjusted by the detector response.

From your project build directory (see `GramsSim/README.md`) you can
run this program with

```
./gramsdetsim
```

## Detector-response functions

### Recombination

Based off the modified box model used by MicroBooNE, among others

### Absorption

### Pair production

## Program options

See `GramsSim/util/README.md` for a description of how to control the
operation of `gramsdetsim` through the `options.xml` file and the
command line. The file `GramsSim/options.xml` contains an example
section to get you started.
