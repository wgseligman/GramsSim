# GramsSim

If you want a formatted (or easier-to-read) version of this file, scroll to the bottom. If you're reading this on github, then it's already formatted. 

- [GramsSim](#gramssim)
  * [Introduction](#introduction)
  * [Installing GramsSim](#installing-gramssim)
    + [Working with github](#working-with-github)
    + [Prerequisites](#prerequisites)
      - [conda](#conda)
      - [CentOS packages](#centos-packages)
    + [Prepare your local computer](#prepare-your-local-computer)
  * [Running GramsSim](#running-gramssim)
  * [Making changes](#making-changes)
    + [Work files](#work-files)
    + [Development "flow"](#development--flow-)
  * [Detector geometry](#detector-geometry)
  * [Program options](#program-options)
  * [References](#references)
  * [Credits](#credits)
  * [Viewing a Markdown document](#viewing-a-markdown-document)

<small><i><a href='http://ecotrust-canada.github.io/markdown-toc/'>Table of contents generated with markdown-toc</a></i></small>


## Introduction

**GramsSim** is a simulation of the detector for the
[GRAMS](https://express.northeastern.edu/grams/) experiment
(Gamma-Ray and AntiMatter Survey). For more on GRAMS, see the
[initial paper](https://inspirehep.net/literature/1713393).

This document assumes that you're familiar with basic UNIX concepts. If you don't know what "`cd ..`" means (for example), please see the [UNIX references](#references) below for some tutorials. 

GramsSim consists of several components. Each is described in its own
subdirectory's README.md file:

   - **GramsG4**: A Geant4 simulation of particle transport in the GRAMS detector. 

   - **GramsDetSim**: Models the detector response to the energy deposits recorded by GramsG4. 

   - **util**: Common utilities (e.g., options processing) shared by all components. 

## Installing GramsSim
    
### Working with github

Visit <https://github.com/> and sign up for an account if you don't already have one.

I strongly advise you to use SSH as the means to access a repository, 
and to [set up an SSH key](https://help.github.com/articles/generating-an-ssh-key/),
otherwise you'll have to type in a password every time you issue a git
request against the repository. 

Once this is done, you can download a copy of the GramsSim repository:

    git clone git@github.com:wgseligman/GramsSim.git
    cd GramsSim
    git fetch
    git checkout develop
    cd ..

*Note:* For now, it is important that you download the `develop` branch *and not the `master` branch*.

### Prerequisites

If you're working on a system of the [Nevis Linux cluster](https://twiki.nevis.columbia.edu/twiki/bin/view/Main/LinuxCluster), type

    module load cmake root geant4 hepmc3

and skip to the next section. Otherwise, read on.

You will need recent versions of:

   - [Cmake](https://cmake.org/) (verified to work with version 3.14 and higher)
   - [ROOT](https://root.cern.ch/) (verified to work with ROOT 6.16 and higher)
   - [Geant4](http://geant4.web.cern.ch/) (verified to work with Geant4 10.7 and higher)
   - [HepMC3](https://gitlab.cern.ch/hepmc/HepMC3) 
   
You will also need the development libraries for:

   - [GNU C++](https://gcc.gnu.org/) (version 6.2 or higher, though the compilation might work with [clang](https://clang.llvm.org/); requires C++11 or higher)
   - [Xerces-C](https://xerces.apache.org/xerces-c/)
   - [OpenGL](https://www.opengl.org/)
   - [QT4](https://www.qt.io/)

At Nevis, the approach that fully worked on [CentOS 7](https://www.centos.org/download/) was to install recent versions of C++, cmake,
ROOT, Geant4, and HepMC3 by compiling them from source. There was no need to recompile 
xerces-c, OpenGL, and QT4; the CentOS 7 development packages were sufficient:

    sudo yum -y install freeglut-devel xerces-c-devel \
       qt-devel mesa-libGLw-devel
       
Note that compiling Geant4 from source may be the only way to reliably use the [OpenGL visualizer](https://conferences.fnal.gov/g4tutorial/g4cd/Documentation/Visualization/G4OpenGLTutorial/G4OpenGLTutorial.html). 

#### conda

You can try to fulfill these requirements using [conda](https://docs.conda.io/projects/conda/en/latest/). This *mostly* works,
though it does not include ROOT I/O in HepMC3 and there are some issues with the Geant4
OpenGL display. 

On [RHEL](https://www.redhat.com/en/technologies/linux-platforms/enterprise-linux)-derived systems, this is the one-time setup; 
visit the [EPEL](https://fedoraproject.org/wiki/EPEL) web site for releases other than CentOS 7:

     # Install conda
     sudo yum -y install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
     sudo yum -y install conda
     
On any system with conda installed (including [anaconda3](https://www.anaconda.com/products/individual) and [miniconda](https://docs.conda.io/en/latest/miniconda.html)), the following will set up a suitable development environment:
     
     # Add the conda-forge repository
     conda config --add channels conda-forge
     conda config --set channel_priority strict

     # Create a conda environment. The name "grams-devel" is arbitrary.
     conda create -y --name grams-devel compilers cmake root geant4 hepmc3

Afterwards, the following must be executed once per login session:

     # Activate the environment to modify $PATH and other variables.
     conda activate grams-devel

#### CentOS packages

Another potential solution is to use RPM packages for RHEL-derived
Linux distributions (e.g., Scientific Linux, CentOS). In addition to the EPEL repository,
you will need a more recent version of the GCC compiler than comes with CentOS 7. One
solution is to use the [SCL](https://www.softwarecollections.org/en/scls/rhscl/devtoolset-7/) 
tools (but see the cautions below).

    sudo yum -7 install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
    sudo yum -y install root HepMC3-devel HepMC3-rootIO-devel 
    sudo yum -y install gcc-c++ glibc-devel cmake3
    sudo yum -y install centos-release-scl
    
    # Note that as of Sep-2021 the devtoolsets range from from 7 to 10,
    # which corresponds to the major version of GCC they include. 
    sudo yum -y install devtoolset-7-gcc*
    
To enable the SCL version of the GCC compiler installed above:

    scl enable devtoolset-7 bash
       
There are problems with this approach:

   - In the following instructions, you will want to use `cmake3` instead of just `cmake`.

   - You will still have to
     [download](https://geant4.web.cern.ch/support/download) and
     build/install Geant4 on your own.

   - It's important that ROOT, HepMC3, and Geant4 all be compiled with
     the same version of the C++ compiler that
     supports C++11 and above. The ROOT and HepMC3 packages from EPEL were compiled with
     the "native compiler" of CentOS 7, GCC 4.8.5,
     which does _not_ support C++11. 

(If you determine the installation commands needed for
Debian-style distributions (including Ubuntu), please let
wgseligman know so he can update this documentation.)
 
### Prepare your local computer 
   
If you are working remotely (e.g., on a laptop), and you want to use
the GramsG4 interactive display, you may also need to
install and/or activate both X-Windows and OpenGL for your local
computer. You can find instructions
[here](https://twiki.nevis.columbia.edu/twiki/bin/view/Main/X11OnLaptops).

## Setting up GramsSim

To make things easier, I'm going to define a variable for the repository
directory in your area. You may want to include a suitably modified version
of this command in one of your shell startup files:

    # $GSDIR is the directory that contains GramsSim, not GramsSim itself.
    # If you've followed these instructions from the beginning, this will
    # be the directory you're in now ($PWD = "print working directory")
    export GSDIR=$PWD

To build/compile:

    # Set up the tools as appropriate for your system
    # For example, at Nevis type
    module load cmake root geant4 hepmc3
    
    # Create a separate build/work directory. This directory should
    # not be the GramsG4 directory or a sub-directory of it. This
    # only has to be done once. 
    cd $GSDIR
    mkdir GramsSim-work
    
    # Build
    cd GramsSim-work
    cmake $GSDIR/GramsSim
    make

To run the programs:

    # After succesfully making the executables, e.g., `gramsg4`, you can
    # run them in the build directory:
    ./gramsg4
    ./gramsdetsim

...and so on. Consult the `README.md` in the individual program directories for details. 

## Making changes

Obviously, you can make any changes you want to GramsSim for your own use. This section is for when you want to start making changes to be added to the official repository. Before you edit files for the first time:

    cd $GSDIR/GramsSim
    git flow init 

You'll be asked a bunch of questions. Each time you're prompted for an answer
whose default is `master` please enter `develop`. Accept the defaults for everything else.

If you get a message that git flow is not recognized, it means that
the gitflow package has to be installed on your machine.   

You only have to type `git flow init` once, the first time you edit
code in that directory. Don't type it again, unless you git clone a
brand-new repository.

When you want to start on a new task:

    git flow feature start $USER_MyMagnificentFeatureName

The '$USER_' means that your feature name should begin with your 
account name. That way you know who is doing what. `MyMagnificentFeatureName` can be anything that identifies your task. 

To "bookmark" changes you've made to your copy of the `$GSDIR/GramsSim` directory:

    git add my_file.cc # if you create a new file
    git commit -a -m "Comment about your changes"

To incorporate changes that other people have made and checked into
the repository:

    git fetch origin
    git rebase origin develop

When you've finished working on your task (after testing it, putting
in the comments, writing the documentation, discussing it at a group
meeting, etc.):

    git flow feature finish
    git push origin develop

If you want other people to see your feature work without making
changes to the develop branch:

    git flow feature publish $USER_MyMagnificentFeatureName

### Work files

The GramsSim programs require several shared input files (e.g., `grams.gdml`, `options.xml`).
These are copied from the GramsSim directory to your work/build directory when you executed 
the `cmake` command. Go ahead and make any changes you want to these work files; they won't 
affect the original files in `$GSDIR/GramsSim`. 

If you feel that your changes to these work files should become part of the git
repository, be sure to copy the changed work files to `$GSDIR/GramsSim` and
"bookmark" them as described above. 

### Development "flow"

What may not be clear from the above is that you'll typically "sit" in your
work/build directory. You'll run programs, make changes, then run them again. 

If you want to change the program code, edit the C++ files in `$GSDIR/GramsSim`;
e.g.,

    cd $GSDIR/GramsSim-work
    emacs ../GramsSim/GramsG4/include/MyPhysicsList.hh
    make
    ./gramsg4 
    
If you want to change the work files, just edit them in your build directory:

    emacs grams.gdml
    
You do *not* have to execute the `cmake` command if you edit the C++ files. 
However, if you edit or change the work files in `$GSDIR/GramsSim`, you *will* have 
to type

    cd $GSDIR/GramsSim-work
    cmake $GSDIR/GramsSim

again to copy the revised work files.

When you've made your changes and wish to "bookmark" them:

    cd $GSDIR/GramsSim
    git commit -a -m "Comment about your changes"

*Because I am lazy, I usually do something like this:*

    (cd ../GramsSim; git commit -a)
    
*and use the up-arrow key to re-invoke that command after each development milestone.*

## Detector geometry

A version of the detector geometry is defined in `grams.gdml`. As the extension implies,
it is written in the geometry-definition language [GDML](http://lcgapp.cern.ch/project/simu/framework/GDML/doc/GDMLmanual.pdf).

If you want to make changes to the detector geometry (including the colors used for
the `gramsg4 --ui` interactive display), edit `grams.gdml`. If you're having trouble 
understanding the contents of the file, start by reading the comments within the file.
There's more about GDML in the [References](#references) section below. 

## Program options

Short version: look at `options.xml`. 

For a complete description, see `GramsSim/util/README.md`.

## References

Understanding UNIX:
   - [UNIX tutorial for beginners](http://www.ee.surrey.ac.uk/Teaching/Unix/)
   - [Learn UNIX](https://www.tutorialspoint.com/unix/index.htm)

Version control system:
   - git: <https://git-scm.com/doc> 
   - git flow: <http://danielkummer.github.io/git-flow-cheatsheet/>

Some git tips from other collaborations:  
   - [git flow quick start guide](https://cdcvs.fnal.gov/redmine/projects/cet-is-public/wiki/Git_flow_quick_start)
   - [while you are developing](https://cdcvs.fnal.gov/redmine/projects/uboonecode/wiki/Uboone_guide#While-you-are-developing)

Toolkits:
   - [Geant4 Manual](http://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/ForApplicationDeveloper/html/)
   - [ROOT Tutorial](https://www.nevis.columbia.edu/~seligman/root-class/)

GDML detector geometry description
   - [GDML manual](http://lcgapp.cern.ch/project/simu/framework/GDML/doc/GDMLmanual.pdf)
   - [Geant4 Applications Guide](http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/), especially the [geometry section](http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/Detector/Geometry/geometry.html) which explains the difference between solids, logical volumes, and physical volumes. 

## Credits

   - Portions of the HepMC3 input code were inspired by the Beam Delivery Simulation (BDSIM), Copyright &copy; Royal Holloway, University of London 2001-2020. 
   
   - That code in turn, is a combination of code from: 

      - the HepMC3 examples in the `share/doc/HepMC3/examples` directory, in particular the contents of the `BasicExamples` and `ConvertExample` directories;
      
      - the code in the Geant4 `examples/extended/eventgenerator/HepMC/HepMCEx01` directory. 

## Viewing a Markdown document

This document is written in
[Markdown](https://www.markdownguide.org/cheat-sheet/), a tool for
formatting documents but still keeping the unformatted versions
readable. It's also handy for reading documents on github, since files ending in `.md`
are automatically formatted for the web. 

If you want to read a formatted version of this document (so you're
spared the funny backticks and hashtags and whatnot), do a web search on
"Markdown viewer" to find a suitable program. For example, at Nevis, all
the Linux cluster systems have [pandoc](https://pandoc.org/) installed.
You can view a plain text version of this document with:

    pandoc README.md -t plain | less

You can type `man pandoc` to learn more formatting options. For example,
if you want to format this into PDF:

    pandoc README.md -o README.pdf
