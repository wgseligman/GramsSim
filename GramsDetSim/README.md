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
[`options XML file`](../util/README.md) to control its processing. The
parameters that control the program are under active development, so
it's a good idea to read the file [`options.xml`](../options.xml) and
pay attention to the comments.

In particular, many of the parameters used in the models described
below depend on the choice of units. It's up to you to make sure that
that units used for (e.g.,) option `birks_kB` match the options
`LengthUnit`, `TimeUnit`, and `EnergyUnit` at the top of the XML file.

## Running `GramsDetSim`

From your project build directory (see
[`GramsSim/README.md`](../README.md)) you can run this program with

```
./gramsdetsim
```

This program will read in an ntuple file created by the `GramsG4`
simulation and create a "friend ntuple" to the `GramsG4`'s "hits"
ntuple. In other words, the ntuple created by this program will
contain, row-by-row, the hit energy adjusted by the detector response.

## Detector-response functions

### Recombination

As of Sep-2022, the recombination models and parameters used in GramsDetSim are based
on these papers:

- [A study of electron recombination using highly ionizing
particles in the ArgoNeuT Liquid Argon TPC][10]

- [Study of electron recombination in liquid argon with the ICARUS TPC][11]. 

[10]: https://arxiv.org/abs/1306.1712
[11]: https://www.sciencedirect.com/science/article/pii/S0168900204000506

There of recombination model used is selected by the parameter `RecombinationModel` in [`options.xml`](../options.xml). The functional forms of these models are:

- Modified box model

<img src="BoxModel.png" width="25%"/>


- Birk's model

<img src="BirksModel.png" width="35%"/>

where:

- _&beta;'_, _&alpha;_, A<sub>B</sub>, and k<sub>B</sub> are material- and detector-based parameters that must be measured empirically;

- _&epsilon;_ is the electric field;

- _&xi;_ is the effective electric field;

- _&rho;_ is the density of the liquid argon;

- dE/dx is the ionization energy per unit length.

(Note: Here, dE/dx is the quantity computed in the program code, that is, energy per unit length. In the papers referenced above, dE/dx is the "stopping power", defined as deposited energy divided by material density per unit length. To go from one to the other, (1/_&rho;_)(dE/dx)<sub>code</sub> = (dE/dx)<sub>papers</sub>.)

See the [options XML](../options.xml) file for the values of the
individual parameters.

### Absorption

### Diffusion

## Program options

See `GramsSim/util/README.md` for a description of how to control the
operation of `gramsdetsim` through the `options.xml` file and the
command line.
