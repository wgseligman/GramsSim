#!/usr/bin/env python3
# dEdxExample.py
# 19-Jun-2024 William Seligman <seligman@nevis.columbia.edu>

# This is an overly-commented example program to illustrate how to
# read the map-based ROOT files produced by GramsSim. It is _not_ an
# example of how to perform a meaningful analysis on data. For that,
# see the ROOT tutorial at
# https://www.nevis.columbia.edu/~seligman/root-class/html/

# You can copy this program and make changes to suit your work.

# The ROOT classes we'll need:
from ROOT import TFile, TTree, gSystem, gDirectory, TH1D

# We'll also need the GramsSim custom dictionary for its data objects
# (see GramsSim/GramsDataObj/README.md and
# https://www.nevis.columbia.edu/~seligman/root-class/html/appendix/dictionary/index.html).
# The following statement assumes this program is being run from the
# build directory you created by following the directions in
# https://github.com/wgseligman/GramsSim/tree/develop:

gSystem.Load("./libDictionary.so")

# Open the input file and access the n-tuple. 
inputFile = TFile("gramsg4.root")
tree = gDirectory.Get( 'gramsg4' )

# The vague purpose of this example program is to calculate
# dE/dx. What we do with that value is up to us. As an example,
# we'll just make a histogram. Note that we're not doing anything
# with that histogram, not even writing or drawing it.

dEdxHistogram = TH1D("dEdx","Histogram of dE/dx",100,0.0,0.005);

# For each row (or entry) in the tree:
for entry in tree:

    # Most of the branches in GramSim files contain data objects in
    # the form of C++ maps; a "map" is like a Python dict, in that it's
    # a container with (key,value) pairs.

    # For this calculation, we only need the 'LArHits" branch within
    # the tree, You can look up the structure of MCLArHits in
    # GramsSim/GramsDataObj/include/MCLArHits.h. What's relevant for
    # this example is that, for the (key,value) pairs that make up
    # this map/dict, the value has the type 'MCLArHit'.

    for ( key, LArHit ) in tree.LArHits:

      # For the list of ways to access the information in
      # grams::MCLArHit, again see
      # GramsSim/GramsDataObj/include/MCLArHits.h.

      energy = LArHit.Energy()
      start = LArHit.Start4D()
      end = LArHit.End4D()

      # We've got the "dE" in dE/dx, now we have to compute the
      # "dx". Take the different between the 4-vectors:
      diff = start - end;

      # Now take the 3D magnitude of the difference.
      dx = diff.R();

      if  dx > 0. :
          dEdx = energy / dx
          dEdxHistogram.Fill( dEdx )
