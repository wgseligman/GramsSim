# GramsSim display programs
*Principle Maintainer: [William Seligman](https://github.com/wgseligman)*

_If you want a formatted (or easier-to-read) version of this file, scroll to the bottom of [`GramsSim/README.md`](../README.md) for instructions. If you're reading this on github, then it's already formatted._

The `GramsSim/Display` directory contains programs used to display results of the simulation. (As of Aug-2024, there's just one: `simd`).

## `simd`

`simd` is short for "Simulation Display". It shows histograms of key values that come from the main stages of [GramsSim](../README.md).

- Note that `simd` is _not_ an event display. It does not show the detector geometry. The "boxes" that you see in the plot are histograms, not images of the LArTPC. 

- The main purpose of this tool is to understand the simulation, diagnose problems, and improve our understanding of the detector. It's not intended to produce plots for publication. 

### Invoking `simd`

#### The basic command

In your build directory (`GramsSim-work` if you followed the [installation directions exactly](../README.md)), try just typing this:
```
./simd
```

#### `simd` inputs

If you don't modify the GramsSim options, either in [the Options xml file](../util/README.md) or on the command line, the files created by a complete run of the simulation chain are (as of Aug-2024):

```
gramsg4.root
gramsdetsim.root
gramsreadoutsim.root
gramselecsim.root
```

If you just type this command, `simd` will read those files by default and display plots:
```
./simd
```

#### Changing the defaults

`simd` gets these filenames and other parameters from [`options.xml`](../options.xml). If you'd like to see plots from a different combination of files, you can modify `options.xml` and just run `simd`. 

If you're doing several different runs of the simulation chain with different sets of files, you can either:

- Create several Options XML files. `simd` accepts options on the command line [in the same way as the main GramsSim programs](../util/README.md), so you can give an alternate options file as an argument to the command; e.g.,

      ./simd options-other.xml
      
- You can specify the names of all the files created by the simulations chain on the command line, overriding the values in `options.xml` [as you can with the main GramsSim programs](../util/README.md); e.g.,

      ./simd \
          --outputG4File=g4-other.root \
          --outputDetSimFile=detsim-other.root \
          --outputReadoutFile=readout-other.root \
          --outputElecFile=elecsim-other.root

  This second approach is more verbose, but it may be useful in some cases. 
          
  The above example assumes that all other options are the same as those found in `options.xml`. For example, `simd` gets the name of the original GDML geometry file from option `gdmlfile`. If you're running GramsSim multiple times  with different geometries, while you can add `--gdmlfile=geometry-other.gdml` to the command line, the first approach is probably the better one to take. 
  
  
### Using `simd`

When you start `simd`, you'll see a view that looks something like this. Note that the details will differ depending on the contents of the GramsSim output files. 

| <img src="images/simd_starting_view.png" width="75%" /> |


### Warnings

#### Speed issues

#### User-interface issues

#### Consistency between files

### `simd` code tips

