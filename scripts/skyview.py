#!/usr/bin/env python3
# 21-Aug-2024 WGS

# A quick-and-dirty look at the distribution of the origin of primary particles
# in the output of GramsG4.

# It can be used to validate the output of GramsSky, or to understand
# the particles created by Geant4's "General Particle Source".

import platform, ROOT

# We'll also need the GramsSim custom dictionary for its data objects
# (see GramsSim/GramsDataObj/README.md and
# https://www.nevis.columbia.edu/~seligman/root-class/html/appendix/dictionary/index.html).
# The following statement assumes this program is being run from the
# build directory you created by following the directions in
# https://github.com/wgseligman/GramsSim/tree/develop:

if ( platform.system() == "Darwin"):
    ROOT.gSystem.Load("libDictionary.dylib")
else:
    ROOT.gSystem.Load("libDictionary.so")

# Fetch the input file. (If I were fancier, I'd get the
# name of the file from an argument to this program.)
inputFile = ROOT.TFile.Open('gramsg4.root')

# Retrieve the input tree.
inputTree = inputFile.Get('gramsg4')

# Write our output histogram to this file.
outputFile=ROOT.TFile.Open('skyview.root','recreate')

# Define the histogram(s) we're going to create.
skyhist = ROOT.TH3D("skyview","Origin of primary particles in G4 Simulation",
		    100,-300,300,
		    100,-300,300,
		    100,-300,300)

# To get these sizes and offsets, I ran the program, fiddled with
# these values, then used "File->SaveAs" to create a .C macro. Then I
# inspected the macro to get the commands I wanted.
skyhist.GetXaxis().SetTitle("x [cm]")
skyhist.GetYaxis().SetTitle("y [cm]")
skyhist.GetZaxis().SetTitle("z [cm]")
skyhist.GetXaxis().SetTitleSize(0.025)
skyhist.GetYaxis().SetTitleSize(0.025)
skyhist.GetZaxis().SetTitleSize(0.025)
skyhist.GetXaxis().SetLabelSize(0.02)
skyhist.GetYaxis().SetLabelSize(0.02)
skyhist.GetZaxis().SetLabelSize(0.02)
skyhist.GetXaxis().SetTitleOffset(2.4)
skyhist.GetYaxis().SetTitleOffset(2.4)
skyhist.GetZaxis().SetTitleOffset(2.0)
skyhist.GetXaxis().CenterTitle()
skyhist.GetYaxis().CenterTitle()
skyhist.GetZaxis().CenterTitle()

# Loop over all the rows / entries in the input tree.
for event in inputTree:

    # This the only branch in the input tree we're going to use.
    TrackList = inputTree.TrackList;

    # For each track in the list:
    for ( trackID, track ) in TrackList: 
    
        # We're only interested in primary particles.
        if ( track.Process() == "Primary" ):

            # Get the starting point of the particle's trajectory.
            trajectory = track.Trajectory()
            firstPoint = trajectory[0]
            x = firstPoint.X()
            y = firstPoint.Y()
            z = firstPoint.Z()

            # Add these coordinates to the histogram.
            skyhist.Fill(x,y,z)

# Wrap this up: 
skyhist.Write()
outputFile.Write()
outputFile.Close()
