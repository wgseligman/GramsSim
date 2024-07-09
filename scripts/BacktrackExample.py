#!/usr/bin/env python3
# BackTrackExample.py
# 08-Jul-2024 William Seligman <seligman@nevis.columbia.edu>

# This code does the "reverse" of AllFilesExample. That program works
# its way forward through the GramsSim classes:
# tracks->hits->clusters->waveforms. This program goes in the other
# direction: waveforms->clusters->hits->tracks.

# This is an overly-commented example program to illustrate how to
# read multiple map-based ROOT files produced by GramsSim as if they
# were all part of the same tree. It is _not_ an example of how to
# perform a meaningful analysis on data. For that, see the ROOT
# tutorial at
# https://www.nevis.columbia.edu/~seligman/root-class/html/

# At this moment, you're tempted to just copy this program and
# blindly place your analysis code in the main program loop. DON'T DO
# THIS WITHOUT THINKING ABOUT WHAT YOU'RE DOING! This program is
# meant as an example of the code needed to access a group of friend
# trees in parallel, and how the GramsSim maps link to each other.

# The odds are high that you don't need to read multiple columns from
# multiple trees in multiple files to accomplish your analysis
# task. Only use the files/trees/columns that you need. The
# 'dEdxExample' programs show this.

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

# Open the input file and access the n-tuple. 
inputFile = ROOT.TFile("gramselecsim.root")
tree = ROOT.gDirectory.Get( 'ElecSim' )

# Now we add additional columns/friends to that tree. (If you ask how
# I knew the names of all these files and trees, I got them from
# GramsSim/options.xml. If I were writing a more complete program, I'd
# get the names below from using the Options class (see
# GramsSim/util).)

tree.AddFriend("ReadoutSim","gramsreadoutsim.root");
tree.AddFriend("DetSim","gramsdetsim.root");
tree.AddFriend("gramsg4","gramsg4.root");

# For each row (or entry) in the tree:
for entry in tree:

    # The following code does not represent a legitimate analysis
    # task. It consists of examples to illustrate how one might scan
    # through the maps, sets, and vectors that make up the data
    # objects.

    # IF YOU'RE COPYING THIS CODE LINE-FOR-LINE AND LOOP-FOR-LOOP,
    # STOP! Most of what the code does has no purpose other than to
    # serve as an example. Look at what the code is doing, and ask if
    # that's what you want to do.

    # For a list of the various methods you can use to access the
    # information in the data objects, see the header files in
    # GramsSim/GramsDataObj/include.

    # For every waveform in the event:
    for ( readoutID, waveform ) in tree.ReadoutWaveforms:
        
        # First, sum the ADC counts in all the bins in the digital
        # waveform. (There is no scientific reason to do this.)
        sumADC = sum( waveform.Digital() )

        # If the sum of the ADC counts is greater than some arbitrary
        # number that I just made up:
        madeUpNumber = 21969;
        if sumADC > madeUpNumber:

            # The data objects in the dictionary all of the C++
            # operator<< defined for them, which simplifies printing
            # them out in C++. Unfortunately, there doesn't seem to be
            # a simple way of calling operator<< from within
            # Python. For these data objects, just display a few
            # details to prove we have access to their information.
            print ( "EventID ", tree.EventID.Index(), "ReadoutID", readoutID.Index(), 
                    "sums to", sumADC, "which is more than", madeUpNumber )

            # Let's assume that this means the waveform is
            # "interesting". Look at all the electron clusters that went
            # into creating this waveform.
            clusterKeys = tree.ReadoutMap[ readoutID ]

            # The cluster keys are, in turn, a set of keys into the
            # ElectronClusters map. The following looks at all the
            # electron clusters associated with those keys.

            # If this seems complex, you can think of this arrangement as:
            #
            # (a) a multi-dimensional array indexed by [trackID][hitID][clusterID][readoutID];
            # (b) if you're into Python, nested dictionaries (dicts of dicts).. 

            for clusterKey in clusterKeys:
                electronCluster = tree.ElectronClusters[ clusterKey ]

                # What can we do with an electron cluster? We might print
                # it out, according to some imaginary non-scientific
                # criteria.
                imaginaryCriteria = 38
                if electronCluster.NumElectrons() < imaginaryCriteria:
                    print ("TrackID", electronCluster.TrackID(), "HitID", electronCluster.HitID(), "ClusterID", 
                           electronCluster.ClusterID(), "energy", electronCluster.EnergyAtAnode())

                    # We can also use it to back-track to a particular
                    # simulated hit in the LAr. The data object MCLArHits is,
                    # once again, a map of (key,value) pairs, with the the key
                    # itself being a pair (tuple) of (trackID,hitID):
                    
                    trackID = electronCluster.TrackID()
                    hitID   = electronCluster.HitID()
                    hitKey = ROOT.std.make_tuple( trackID, hitID )
                    hit = tree.LArHits[ hitKey ]

                    # Now we have the MCLArHit associated with this particular
                    # electron cluster. Let's apply an arbitrary non-scientific
                    # cut on the number of scintillation photons to show how to
                    # work with a hit.
                    numPhotons = hit.NumPhotons()
                    unscientificCut = 200
                    if numPhotons < unscientificCut:

                        # We can print out the hit if we wish.
                        print ("TrackID", hit.TrackID(), "HitID", hit.HitID(), "energy", hit.Energy())

                        # As a final example of back-tracking, let's go to the
                        # track that created the hit.
                        createID = hit.TrackID()
                        track = tree.TrackList[ createID ]

                        # Now that we have a track, let's look at its trajectory.
                        trajectory = track.Trajectory()

                        # Let's print the first point in the trajectory:
                        firstPoint = trajectory[0]
                        print ( "First point in track (x,y,z,t)", 
                                firstPoint.X(), firstPoint.Y(), firstPoint.Z(), firstPoint.T() )
