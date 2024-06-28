#!/usr/bin/env python3
# 22-May-2021 WGS

# A quick-and-dirty analysis program to look at radial distances from
# one compton scatter to another within the output of gramsg4.

# This is intended as an example of how to use ROOT directly from
# within a python script. It is _not_ optimized for either memory or
# execution efficiency (as I discovered when I ran it for 10,000
# events).

import ROOT
import math

# We'll also need the GramsSim custom dictionary for its data objects
# (see GramsSim/GramsDataObj/README.md and
# https://www.nevis.columbia.edu/~seligman/root-class/html/appendix/dictionary/index.html).
# The following statement assumes this program is being run from the
# build directory you created by following the directions in
# https://github.com/wgseligman/GramsSim/tree/develop:

ROOT.gSystem.Load("./libDictionary.so")

# Fetch the input file. (If I were fancier, I'd get the
# name of the file from an argument to this program.)
inputFile = ROOT.TFile.Open('gramsg4.root')

# Retrieve the input tree.
inputTree = inputFile.Get('gramsg4')

# Write our output ntuple to this file.
outputFile=ROOT.TFile.Open('radialdistance.root','recreate')

# Define the very basic ntuple that we're going to write.
ntuple = ROOT.TNtuple("RD","Radial Distance between successive compton scatters","radDist")

# Define the histogram(s) we're going to create.
rdhist = ROOT.TH1D("radialdistance","Radial Difference between sucessive compton scatters in mm",100,0,200)

# Loop over all the rows / entries in the input tree.
for event in inputTree:

    # There are three branches in the input tree. We'll only use one
    # of them, but I'll show all three here so you know how the
    # process works. You can see the properties of all these data
    # objects in GramsSim/GramsDataObj/include.
    eventID = inputTree.EventID;
    TrackList = inputTree.TrackList;
    LArHits = inputTree.LArHits;

    # Build a list of the start of compton-scatter trajectories for
    # primary particles in this event.

    # This code assumes that there's only one primary particle in this event.
    # Start with an empty list: No Comptons yet!
    comptons = list()

    # Most of the branches in GramSim files (such as LArHits and
    # TrackHits) contain data objects in the form of C++ maps; a "map"
    # is like a Python dict, in that it's a container with (key,value)
    # pairs.
    for ( trackID, track ) in TrackList:
        if track.Process() == "Primary" :

            # Loop through all the daughters of this primary particle,
            # looking for Compton scatters.
            for daughterID in track.Daughters():
                daughter = TrackList[daughterID]
                if daughter.Process() == "compt" :

                    # We've found a compton scatter. What we want to
                    # save is the first point in its trajectory.
                    trajectory = daughter.Trajectory();
                    firstTrajectoryPoint = trajectory[0]

                    # Save the 4D position of the first point in
                    # the Compton scatter's trajectory.
                    comptons.append( firstTrajectoryPoint )

            # We've processed the primary particle for this event, so
            # we can stop looping.
            break;

    # We're only interesting tracks with successive Compton
    # scatters. If there was one (or fewer) Compton scatters, skip
    # this event.
    if len( comptons ) < 2:
        continue

    # We want to sort the list of Compton scatters in ascending order
    # by time. Probably they're already in time order, but there's no
    # guarantee of that. (See GramsSim/GramsDataObj/include/MCTrackLists.h
    # for all the MCTrajectoryPoint methods.)
    sortedComptons = sorted( comptons, key=lambda trajPoint: trajPoint.T() )
    
    # Scan through the sorted list in pairs, since we want to look at
    # the distance between successive compton scatters.
    numberOfComptons = len( sortedComptons )
    for i in range ( numberOfComptons - 1 ):

        # Inspect the first of the pair:
        firstPoint = sortedComptons[i]

        # If a compton scatter doesn't start in a LAr volume, skip
        # this event. (Events not in the LAr are interesting, but for
        # another study, not the one I'm doing in this program.)

        identifier = firstPoint.Identifier()
        firstDigit = int( identifier / 1000000 )
        if firstDigit != 1:
            break

        # Save the (x,y) position of the first of the pair.
        x0 = firstPoint.X()
        y0 = firstPoint.Y()

        # Do this again for the second of the pair. This time I'll
        # be more compact in my code.
        secondPoint = sortedComptons[i+1]
        if int( secondPoint.Identifier() / 1000000 ) != 1:
            break;

        x1 = secondPoint.X()
        y1 = secondPoint.Y()

        # What I'm interested in is the radial distance between
        # successive compton scatters in the LAr.
        radialDistance = math.sqrt( (x1-x0)**2 + (y1-y0)**2 )

        # Add a row to the output ntuple. It must be the form of an
        # "iterable". We convert the value(s) to add into a list, then
        # pass the address of that list. (No, this was not easy for me
        # to figure out!)
        ntuple.Fill(*[radialDistance])

        # Add the value to the histogram.
        rdhist.Fill(radialDistance)

# Wrap this up: Close any output files we've written to.
outputFile.Write()
outputFile.Close()
