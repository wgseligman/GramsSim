# GramsElecSim
*Author: Satoshi Takashima*

_If you want a formatted (or easier-to-read) version of this file, scroll to the bottom of [`GramsSim/README.md`](../README.md) for instructions. If you're reading this on github, then it's already formatted._

- [GramsElecSim](#gramselecsim)
  * [`GramsElecSim` simulation parameters](#gramselecsim-simulation-parameters)
    + [Noise fluctuations](#noise-fluctuations)
    + [Shaping and pre-amplification](#shaping-and-pre-amplification)
    + [Analog-to-digital conversion](#analog-to-digital-conversion)
  * [grams::ReadoutWaveforms](#gramsreadoutwaveforms)
  * [Design note](#design-note)

<small><i><a href='http://ecotrust-canada.github.io/markdown-toc/'>Table of contents generated with markdown-toc</a></i></small>

This program is a simulation of the electronics response to the detector readout, including noise and shaping. It sums the energy deposited into each readout pixel as a function of time, and generates waveform simulations for each pixel. 

Note that `gramselecsim` uses a random-number generator for its noise and pre-amp operations. When running this program as part of grid or batch job, you probably want to set up a process-based value for option `rngseed` as mentioned in [GramsSim/README.md](../README.md). For example, assuming that the individual process ID is stored in variable `${Process}`:

    ./gramselecsim --rngseed=${Process}
    
## `GramsElecSim` simulation parameters

See `GramsSim/util/README.md` for a description of how to control the
operation of `gramsdetsim` through the [`options.xml`](../options.xml) file and the
command line.

- `timebin_width`: The minimal time interval over which the readout can can respond to a charge.

- `time_window`: The total time interval over which charge would be sampled once the electronics are triggered. 

There are three segments of the electronics response modeled by `gramselecsim`.

### Noise fluctuations

The number of electrons arriving at given pixel is adjusted by:

<img src="images/NoiseEq.png" width="286" />

where

- &Nu;<sub>i</sub> is the number of electrons in an electron cluster;
- &Nu;<sub>i</sub><sup>'</sup> is the noise-adjusted number of electrons;
- <em>N</em>(0,1) is a normal distribution with a mean of 0 and width of 1;
- <em>c</em><sub>0</sub>, <em>c</em><sub>1</sub>, and <em>c</em><sub>2</sub> are the parameters `noise_param0`, `noise_param1`, and `noise_param2` respectively. 

### Shaping and pre-amplification

Each group of electrons is treated as a delta function which must be shaped by a response function. The parameters that affect this process are:

- `preamp_prior_time`: The rise time of the response function.
- `preamp_post_time`: The decay time of the response function.
- `peak_delay`: The delay between the arrival of the electron cluster and the peak of the response function.
- `preamp_gain`: The amplitude of the response function. 

The parameter `preamp_func` selects which response function to use:

0 -> A gaussian distribution normalized to unity: 

<img src="images/NormGauss.png" width="100" />

1 -> A gaussian distribution:

<img src="images/Gauss.png" width="46" />

2 -> The log of a normalized gaussian distribution:

<img src="images/LogNormGauss.png" width="118" />

3 -> The log of a gaussian:

<img src="images/LogGauss.png" width="62" />

4 -> A double exponential function:

<img src="images/TwoExp.png" width="180" />

where <em>&sigma;</em>, <em>&tau;</em><sub>1</sub>, and <em>&tau;</em><sub>2</sub> are the parameters `preamp_sigma`, `preamp_tau1`, and `preamp_tau2` respectively. 

### Analog-to-digital conversion

The last step is to take the summed response functions for the accumulated electrons and apply the effects of analog-to-digital (ADC) conversion. 

The parameters that affect this procedure are:

- `sample_freq`: The analog values are scaled from time bins whose width was determined by `timebin_width` to the ADC bins whose width is determined by `sample_freq`. 

- `input_max` and `input_min`: The signal is then clipped at the high and low ends by these two parameters. The parameter `input_min` also effectively serves as the ADC pedestal. 

- `bit_resolution`: The last step is to convert the floating-point value from the previous steps into a number of ADC counts, as determined by the `bit_resolution` parameter.

## grams::ReadoutWaveforms

As you look through the description below, consult the [GramsDataObj/include](../GramsDataObj/include) directory for the header files. These are the files that define the methods for accessing the values stored in this object. Documentation may be inaccurate; the code is actual definition. If it helps, a [std::map][130] is a container whose elements are stored in (key,value) pairs. If you're familiar with Python, they're similar to [dicts][140]. 

[130]: https://cplusplus.com/reference/map/map/
[140]: https://www.w3schools.com/python/python_dictionaries.asp

|         <img src="../GramsDataObj/images/grams_ReadoutWaveforms.png" width="60%" />      |
|                                 :--------:                                         | 
| <small><strong>Sketch of the grams::ReadoutWaveforms data object.</strong></small> |

The [`grams::ReadoutWaveforms`](../GramsDataObj/include/ReadoutWaveforms.h) data object represents an approximation to the electronic signals expected to come from the detector. `ReadoutWaveforms` is a [map][3050] containing `grams::ReadoutWaveform` objects for an event. Each ReadoutWaveform has two [vectors][3060] (similar to a [Numpy array][3070]), one for the signal that's input to the analog-to-digital conversion (ADC) and one for the digitized version of the analog signal. 

[3050]: https://cplusplus.com/reference/map/map/
[3060]: https://cplusplus.com/reference/vector/vector/
[3070]: https://www.w3schools.com/python/numpy/numpy_creating_arrays.asp

The analog and digital versions of the waveforms are created by summing the charges (electron clusters) accumulated at each readout channel. Units of the analog waveform are millivolts; units of the digital waveform are ADC counts. Note that the length of these two vectors are _not_ the same; the length of the "digital" vector is scaled from the "analog" vector using the `sample_freq` parameter described above. 

## Design note

It's reasonable to ask why the functions of GramsDetSim,
GramsReadoutSim, and GramsElecSim are in three separate programs.

Functionally, each of these programs relates to a different aspect of
an experiment's simulation:

   - GramsDetSim relates to the physics of charge transport in the detector.

   - GramsReadoutSim relates to the geometry of the readout anode.

   - GramsElecSim relates to the design of the data-acquisition electronics.

Experience has taught us that for the purpose of planning, testing,
studies, and maintenance, it's best to have these functions in
separate programs, rather than one large program.
