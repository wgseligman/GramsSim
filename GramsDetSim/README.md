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

As of Sep-2022, the recombination models used in GramsDetSim are based
on the paper [A study of electron recombination using highly ionizing
particles in the ArgoNeuT Liquid Argon TPC][10]. The models
implemented are:

[10]: https://arxiv.org/abs/1306.1712

<img src="RecombinationModel.png" width="20%"/>

See the [options XML](../options.xml) file for a description of the
individual parameters.

### Absorption

### Pair production

## Program options

See `GramsSim/util/README.md` for a description of how to control the
operation of `gramsdetsim` through the `options.xml` file and the
command line.
