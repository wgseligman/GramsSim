## GramsSim Dependencies

- [GramsSim Dependencies](#gramssim-dependencies)
  * [Prerequisites](#prerequisites)
    + [conda](#conda)
    + [CentOS packages](#centos-packages)
  * [Prepare your local computer](#prepare-your-local-computer)

<small><i><a href='http://ecotrust-canada.github.io/markdown-toc/'>Table of contents generated with markdown-toc</a></i></small>

### Prerequisites

If you're working on a system of the [Nevis Linux cluster][4], type

    module load cmake root geant4 hepmc3 healpix

and skip to the [section below](#prepare-your-local-computer) on preparing your local computer. Otherwise, read on.

[4]: https://twiki.nevis.columbia.edu/twiki/bin/view/Main/LinuxCluster

You will need recent versions of:

   - [Cmake](https://cmake.org/) (verified to work with version 3.14 and higher)
   - [ROOT](https://root.cern.ch/) (verified to work with ROOT 6.16 and higher)
  
You will also need the development libraries for:

   - [GNU C++](https://gcc.gnu.org/) (version 6.2 or higher, though the compilation might work with [clang](https://clang.llvm.org/); requires C++11 or higher)
   - [Xerces-C](https://xerces.apache.org/xerces-c/)
   
The following libraries are optional. If one or more are not present, then GramsSim will compile without the corresponding features. For example, if HepMC3 is not installed, then [GramsSky](GramsSky) will not be compiled; if Geant4 is not installed, [GramsG4](GramsG4) won't be compiled.
   
   - [Geant4](http://geant4.web.cern.ch/) (verified to work with Geant4 10.7 and higher)
   - [HepMC3](https://gitlab.cern.ch/hepmc/HepMC3) (verified to work with HepMC3 3.2.2)
   - [CFITSIO](https://heasarc.gsfc.nasa.gov/fitsio/) (verified to work with cfitsio 4.0.0)
   - [HEALPix](https://healpix.sourceforge.io/) (verified to work with healpix_cxx 3.8.0)
   
The following optional development libraries are needed for Geant4 visualization:

   - [OpenGL](https://www.opengl.org/)
   - [QT4](https://www.qt.io/)

At Nevis, the approach that fully worked on [CentOS 7][5] was to install recent versions of C++, cmake,
ROOT, Geant4, HepMC3, cfitsio, and healpix_cxx by compiling them from source. There was no need to recompile 
xerces-c, OpenGL, and QT4; the CentOS 7 development packages were sufficient:

    sudo yum -y install freeglut-devel xerces-c-devel \
       qt-devel mesa-libGLw-devel
       
[5]: https://www.centos.org/download/

Note that compiling Geant4 from source may be the only way to reliably use the [OpenGL visualizer][6].

[6]: https://conferences.fnal.gov/g4tutorial/g4cd/Documentation/Visualization/G4OpenGLTutorial/G4OpenGLTutorial.html 

#### conda

You can try to fulfill these requirements using [conda][7]. This *mostly* works,
though it does not include ROOT I/O in HepMC3 and there are some issues with the Geant4
OpenGL display. 

[7]: https://docs.conda.io/projects/conda/en/latest/

On [RHEL][8]-derived systems, this is the one-time setup; 
visit the [EPEL][9] web site for releases other than CentOS 7:

[8]: https://www.redhat.com/en/technologies/linux-platforms/enterprise-linux
[9]: https://fedoraproject.org/wiki/EPEL

     # Install conda
     sudo yum -y install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
     sudo yum -y install conda
     
On any system with conda installed (including [anaconda3][10] and [miniconda][11]), the following will set up a suitable development environment:

[10]: https://www.anaconda.com/products/individual
[11]: https://docs.conda.io/en/latest/miniconda.html
     
     # Add the conda-forge repository
     conda config --add channels conda-forge
     conda config --set channel_priority strict

     # Create a conda environment. The name "grams-devel" is arbitrary.
     conda create -y --name grams-devel compilers cmake root geant4 hepmc3 cfitsio healpix_cxx

Afterwards, the following must be executed once per login session:

     # Activate the environment to modify $PATH and other variables.
     conda activate grams-devel

#### CentOS packages

Another potential solution is to use RPM packages for RHEL-derived
Linux distributions (e.g., Scientific Linux, CentOS). In addition to the EPEL repository,
you will need a more recent version of the GCC compiler than comes with CentOS 7. One
solution is to use the [SCL][12] 
tools (but see the cautions below).

[12]: https://www.softwarecollections.org/en/scls/rhscl/devtoolset-7/

    sudo yum -7 install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
    sudo yum -y install root HepMC3-devel HepMC3-rootIO-devel cfitsio-devel healpix_cxx-devel
    sudo yum -y install gcc-c++ glibc-devel cmake3
    sudo yum -y install centos-release-scl
    
    # Note that as of Sep-2021 the devtoolsets range from from 7 to 10,
    # which corresponds to the major version of GCC they include. 
    sudo yum -y install devtoolset-7-gcc*
    
To enable the SCL version of the GCC compiler installed above:

    scl enable devtoolset-7 bash
       
There are problems with this approach:

   - In the [compilation instructions](README.md), you will want to use `cmake3` instead of just `cmake`.

   - You will still have to
     [download][13] and
     build/install Geant4 on your own.

   - It's important that ROOT, HepMC3, and Geant4 all be compiled with
     a version of the C++ compiler that
     supports C++11 and above. The ROOT, HepMC3, cfitsio, and healpix_cxx packages from EPEL were compiled with
     the "native compiler" of CentOS 7, GCC 4.8.5,
     which does _not_ support C++11. 
     
     [This page](https://stackoverflow.com/questions/33394934/converting-std-cxx11string-to-stdstring) potentially offers a work-around to C++11 incompatibility with older software by adding the following option as shown below when building both Geant4 and GramsSim, but this has not been tested:
```
     cmake ../GramsSim -D_GLIBCXX_USE_CXX11_ABI=0
```

[13]: https://geant4.web.cern.ch/support/download

(If you determine the installation commands needed for
Debian-style distributions (including Ubuntu), please let
wgseligman know so he can update this documentation.)
 
### Prepare your local computer 
   
If you are working remotely (e.g., on a laptop), and you want to use
the [GramsG4](GramsG4/README.md) interactive display, you may also need to
install and/or activate both X-Windows and OpenGL for your local
computer. You can find instructions
[here][14].

[14]: https://twiki.nevis.columbia.edu/twiki/bin/view/Main/X11OnLaptops
