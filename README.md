# GramsSim

If you want a formatted (or easier-to-read) version of this file, scroll to the bottom. If you're reading this on github, then it's already formatted. 

- [GramsSim](#gramssim)
  * [Introduction](#introduction)
  * [Installing GramsSim](#installing-gramssim)
    + [Working with github](#working-with-github)
  * [Setting up GramsSim](#setting-up-gramssim)
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
[GRAMS][1] experiment
(Gamma-Ray and AntiMatter Survey). For more on GRAMS, see the
[initial paper][2].

[1]: https://express.northeastern.edu/grams/
[2]: https://inspirehep.net/literature/1713393

This document assumes that you're familiar with basic UNIX concepts. If you don't know what "`cd ..`" means (for example), please see the [UNIX references](#references) below for some tutorials. 

GramsSim consists of several components. Each is described in its own
subdirectory's README.md file:

   - [**GramsSky**](GramsSky): A simulation of particles coming from a spherical (sky) source around the GRAMS detector. 

   - [**GramsG4**](GramsG4): A Geant4 simulation of particle transport in the GRAMS detector. 

   - [**GramsDetSim**](GramsDetSim): Models the GRAMS detector response to the energy deposits recorded by GramsG4. 

   - [**util**](util): Common utilities (e.g., options processing) shared by all components. 

   - [**scripts**](scripts): Examples of how to work with GramsSim components and files.
   
   - [**mac**](mac): Geant4 macro files for use with GramsG4.

Before installing GramsSim, please read [DEPENDENCIES.md](DEPENDENCIES.md) for a list of packages that must be installed for GramsSim to compile.

## Installing GramsSim
    
### Working with github

Visit <https://github.com/> and sign up for an account if you don't already have one.

I strongly advise you to use SSH as the means to access a repository, 
and to [set up an SSH key][3],
otherwise you'll have to type in a password every time you issue a git
request against the repository. 

[3]: https://help.github.com/articles/generating-an-ssh-key/

Once this is done, you can download a copy of the GramsSim repository:

    git clone git@github.com:wgseligman/GramsSim.git
    cd GramsSim
    git fetch
    git checkout develop
    cd ..

*Note:* For now, it is important that you download the `develop` branch *and not the `master` branch*.

## Setting up GramsSim

To make things easier, I'm going to define a variable for the repository
directory in your area. You may want to include a suitably modified version
of this command in one of your shell startup files:

    # $GSDIR is the directory that contains GramsSim, not GramsSim itself.
    # If you've followed these instructions from the beginning, this will
    # be the directory you're in now ($PWD = "print working directory")
    export GSDIR=$PWD

To build/compile:[^make]

    # Set up the tools as appropriate for your system
    # For example, at Nevis type
    module load cmake root geant4 hepmc3 healpix
    
    # Create a separate build/work directory. This directory should
    # not be the GramsG4 directory or a sub-directory of it. This
    # only has to be done once. 
    cd $GSDIR
    mkdir GramsSim-work
    
    # Build
    cd GramsSim-work
    cmake $GSDIR/GramsSim
    make

[^make]: To speed up the build process, consider using `make -jN` where N is two more than the number of processors you have on your computer. Something like `make -j16` will compile all of GramsSim in under a minute. 

To run the programs:

    # After succesfully making the executables, e.g., `gramsg4`, you can
    # run them in the build directory:
    ./gramssky
    ./gramsg4
    ./gramsdetsim

...and so on. Consult the `README.md` in the individual program directories for details. 

**Note**: On Mac OS X Darwin, the names of the programs will have `.exe` appended to them; e.g., `gramsg4.exe`. This is to avoid problems in the build procedure caused by Mac OS X being case-insensitive with respect to file and directory names; e.g., `GramsG4` and `gramsg4` are the same in OS X, so it's safer to make the executable `gramsg4.exe`.

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

The GramsSim programs require several shared input files (e.g., [`grams.gdml`](grams.gdml), [`options.xml`](options.xml)).
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

The detector geometry is defined via [GDML file](grams.gdml)a geometry-definition language [GDML][15]
original designed for Geant4.

[15]: https://gdml.web.cern.ch/GDML/doc/GDMLmanual.pdf

If you want to make changes to the detector geometry (including the colors used for
the `gramsg4 --ui` interactive display), edit `grams.gdml` or supply a different GDML file using the `--gdmfile` option.
If you're having trouble 
understanding the contents of the `grams.gdml`, start by reading the comments within the file.
There's more about GDML in the [References](#references) section below. 

## Program options

Short version: look at [`options.xml`](options.xml). 

For a complete description, see [the Options XML file documentation](util/README.md).

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
   - [GDML manual][15]
   - [Geant4 Applications Guide][48], especially the [geometry section][49] which explains the difference between solids, logical volumes, and physical volumes. 

[48]: http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/
[49]: http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/Detector/Geometry/geometry.html

## Credits

   - Portions of the HepMC3 input code were inspired by the Beam Delivery Simulation (BDSIM), Copyright &copy; Royal Holloway, University of London 2001-2020. 
   
   - That code in turn, is a combination of code from: 

      - the HepMC3 examples in the `share/doc/HepMC3/examples` directory, in particular the contents of the `BasicExamples` and `ConvertExample` directories;
      
      - the code in the Geant4 `examples/extended/eventgenerator/HepMC/HepMCEx01` directory. 

## Viewing a Markdown document

This document is written in
[Markdown][50], a tool for
formatting documents but still keeping the unformatted versions
readable. It's also handy for reading documents on github, since files ending in `.md`
are automatically formatted for the web. 

[50]: https://www.markdownguide.org/basic-syntax

If you want to read a formatted version of this document (so you're
spared the funny backticks and hashtags and whatnot), do a web search on
"Markdown viewer" to find a suitable program. For example, at Nevis, all
the Linux cluster systems have [pandoc][51] installed.
You can view a plain text version of this document with:

[51]: https://pandoc.org/

    pandoc README.md -t plain | less

You can type `man pandoc` to learn more formatting options. For example,
if you want to format this into PDF:

    pandoc README.md -o README.pdf
