#!/usr/bin/env python3
# SimpleAnalysis.py
# 25-Jun-2024 William Seligman <seligman@nevis.columbia.edu>

# To run this program, you can do something like this
# from your GramsSim build directory:
#    ./scripts/SimpleAnalysis.py

# This macro assumes you have created a GramsSim build directory
# following the recipe in the documentation:
# https://github.com/wgseligman/GramsSim/blob/develop/README.md and
# are running the script with the above command in that directory. If
# you've moved the script, then you'll have to adjust the gSystem.Load line.

import ROOT

# We'll also need the GramsSim custom dictionary for its data objects
# (see GramsSim/GramsDataObj/README.md and
# https://www.nevis.columbia.edu/~seligman/root-class/html/appendix/dictionary/index.html).

ROOT.gSystem.Load("./libDictionary.so")

# Open the input file and access the n-tuple. 
inputFile = ROOT.TFile("gramsg4.root")
tree = ROOT.gDirectory.Get( 'gramsg4' )

# For each row (or entry) in the tree:
for entry in tree:
 
    # Did this event have any LAr hits?
    numLArHits = len( tree.LArHits )
    if numLArHits > 0:

        # How many tracks were in this event?
        numTracks = len( tree.TrackList )

        # How many of those tracks were in the LAr? Look at each track
        # in the list of Tracks.
        numLArTracks = 0
        for ( trackID, track ) in tree.TrackList:

            # Look at the trajectory of this track.
            trajectory = track.Trajectory()

            # For this simple analysis, we want to look at tracks whose
            # trajectories start in the LAr. Note that _none_ of the
            # primary particles start in the LAr; they start outside the
            # detector.
            trajectoryPoint = trajectory[0]
            identifier = trajectoryPoint.Identifier()

            # In the Identifier scheme documented in GramsSim/grams.gdml,
            # volume ID numbers in the LAr are seven-digit numbers that
            # begin with 1.
            volumeType = int( identifier / 1000000 )
            if volumeType == 1: 
                numLArTracks += 1

        # Display some counts, including those tracks that 
        # started in the active LAr. 
        print ("Event", tree.EventID.Index(), "has", 
               numLArHits, "hits and", 
               numTracks, "tracks;",
               numLArTracks, "of these tracks started in the active LAr" )
