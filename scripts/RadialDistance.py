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
import os

# Fetch the input file. (If I were fancier, I'd get the
# name of the file from an argument to this program.)
inputFile = ROOT.TFile.Open('gramsg4.root')

# Retrieve the ntuple. 
trackInfo = inputFile.Get('TrackInfo')

# Write our output ntuple to this file.
outputFile=ROOT.TFile.Open('radialdistance.root','recreate')

# Define the very basic ntuple that we're going to write.
ntuple = ROOT.TNtuple("RD","Radial Distance between successive compton scatters","radDist")

# Define the histogram(s) we're going to create.
rdhist = ROOT.TH1D("radialdistance","Radial Difference between sucessive compton scatters in mm",100,0,200)

# To use ROOT::TTree::CopyTree, you need a work directory on disk to
# prevent lots of errors about "memory resident trees". 
workFileName = "workfile.root"
workFile=ROOT.TFile.Open(workFileName,'recreate')

# Select only those rows from TrackInfo that have ProcessName equal to
# "Primary" (i.e., they're the track of primary particles in the
# event) and save them in their own TTree.
primaryParticle = trackInfo.CopyTree('ProcessName=="Primary"')

# For our current study, we're generating only one primary particle
# for each event. If we were to do a different kind of study (e.g.,
# cosmic-ray showers) then that simple assumption would be false.
numberOfEvents = primaryParticle.GetEntriesFast()

# We will 
for entry in range( numberOfEvents ):

    # Get a row from the primary-particle ntuple.
    numberOfBytes = primaryParticle.GetEntry( entry )

    # This is a probably unnecessary check to make sure that we
    # actually read in something.
    if numberOfBytes <= 0:
        continue

    # For this event (and only this event), we want to look at the
    # the tracks of the Compton-scatter electrons.

    # Construct a selection argument for CopyTree.
    run = primaryParticle.Run
    event = primaryParticle.Event
    selection = "Run==" + str(run) + " && Event==" + str(event) \
       + ' && ProcessName=="compt"'

    # This (small) ntuple will only have the compton scatters
    # for this particular primary particle.
    comptons = trackInfo.CopyTree(selection)
    numberOfComptons = comptons.GetEntriesFast()

    # Skip this event if there are no compton scatters... or if
    # there's only one compton scatter. This particular study is
    # looking at the distance between successive scatters, so we need
    # to have at least two.

    if numberOfComptons <= 1:
        continue

    # Construct a list of the (row,time) values for each compton
    # scatter in this event. We'll sort the list by time in just a
    # bit...
    comptonList = list()

    # For each of the compton scatters:
    for centry in range ( numberOfComptons ):

        numberOfBytes = comptons.GetEntry( centry )
        if numberOfBytes <= 0:
            continue

        # Construct a dictionary: The row number in our compton ntuple
        # and the time of the start of its track. 

        # What does "t[0]" mean? Remember that variables like t, x, y,
        # z, Etot, and so on in TrackInfo are vectors (arrays), with
        # each element of the array referring to an individual step
        # along the particle's track. For this analysis, we're only
        # interested in the beginning of the first step, so we're only
        # looking at the first entry in the array: [0].

        comptonInfo = { 'entry': centry, 'time': comptons.t[0] }

        # Append that to our "list of dicts".
        comptonList.append( comptonInfo )

    # Sort our list by time:
    sortedComptons = sorted(comptonList, key=lambda f: f['time'] )
    
    # Scan through the sorted list in pairs, since we want to look at
    # the distance between successive compton scatters.
    for i in range ( numberOfComptons - 1 ):

        # Inspect the first of the pair:
        rowNumber = sortedComptons[i]['entry']
        comptons.GetEntry( rowNumber )

        # If a compton scatter doesn't start in a LAr volume, skip
        # this event. (Events not in the LAr are interesting, but for
        # another study, not the one I'm doing in this program.)

        identifier = comptons.identifier[0]
        firstDigit = int( identifier / 1000000 )
        if firstDigit != 1:
            break

        # Save the (x,y) position of the first of the pair..
        x0 = comptons.x[0]
        y0 = comptons.y[0]

        # Do this again for the second of the pair. This time I'll
        # be more compact in my code.
        comptons.GetEntry( sortedComptons[i+1]['entry'] )
        if int( comptons.identifier[0] / 1000000 ) != 1:
            break;

        x1 = comptons.x[0]
        y1 = comptons.y[0]

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
workFile.Close()

# Get rid of the work file.
os.remove(workFileName)
