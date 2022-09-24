# GramsDetSim

[`GramsG4`](../GramsG4) propagates particles as they pass through the
detector, recording the amount of ionization they deposit in the
liquid argon. The next step in modeling the detector is to propagate
that ionization to the GRAMS readout.

In theory, one might set up Geant4 to transport each individual
electron liberated by ionization to the readout plane. In practice,
this would take too long. Instead, we use `GramsDetSim` to model the
drifting of the ionization electrons in the LArTPC.

## Overview

Like the other [GramsSim](../) programs, `GramsDetSim` uses the
[`options XML`](../util/README.md) to control its processing. The
parameters that control the program are under active development, so
it's a good idea to read the file [`options.xml`](../options.xml) and
pay attention to the comments.

In particular, many of the parameters used in the models described
below depend on the choice of units. It's up to you to make sure that
that units used for (e.g.,) option `birks_kB` match the options
`LengthUnit`, `TimeUnit`, and `EnergyUnit` at the top of the XML file.



This program will read in an ntuple file created by the `GramsG4`
simulation and create a "friend ntuple" to the `GramsG4`'s "hits"
ntuple. In other words, the ntuple created by this program will
contain, row-by-row, the hit energy adjusted by the detector response.

From your project build directory (see
[`GramsSim/README.md`](../README.md)) you can run this program with

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

### Diffusion

## Program options

See `GramsSim/util/README.md` for a description of how to control the
operation of `gramsdetsim` through the `options.xml` file and the
command line.
