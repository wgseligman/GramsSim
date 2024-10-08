#!/usr/bin/env python3
# AllFilesExample.py
# 20-Jun-2024 William Seligman <seligman@nevis.columbia.edu>

# This is an overly-commented example program to illustrate how to
# read multiple map-based ROOT files produced by GramsSim as if they
# were all part of the same tree. It is _not_ an example of how to
# perform a meaningful analysis on data. For that, see the ROOT
# tutorial at
# https://www.nevis.columbia.edu/~seligman/root-class/html/

# At this moment, you're tempted to copy this program and blindly
# place your analysis code in the main program loop. DON'T DO THIS
# WITHOUT THINKING ABOUT WHAT YOU'RE DOING! Look at the smaller and
# perhaps more realistic examples in SimpleAnalysis, dEdxExample, and
# RadialDistance.

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

    # For a list of the various methods you can use to access the
    # information in the data objects, see the header files in
    # in the include/ sub-directory.

    # Let's pick an arbitrary event. How about run=0, event=3? (Note
    # how by using the prefix "ROOT." we have access to all the C++
    # methods in the GramsDataObj dictionary; in particular, we can
    # construct a new grams::EventID object. The C++ namespace
    # "grams::" is replaced by "grams." in Python.)
    arbitraryEvent = ROOT.grams.EventID(0,3)

    # For only that event:
    if ( tree.EventID == arbitraryEvent ):

        # For this event, let's go through all the tracks. Most of the
        # data objects based on maps, which are like Python dicts in
        # that they are (key,value) pairs. This is an example of how to
        # loop over all the (trackID, MCTrack) pairs in a MCTrackList.
      
        # A list to save track IDs.
        primaries = list()
        for ( trackID, track ) in tree.TrackList:
            
            # Let's look only for primary particles in the event.
            if ( track.Process() == "Primary" ):
                # Save the ID of this track.
                primaries.append( trackID )
                
        # Now look through all the hits.
        for ( hitID, hit ) in tree.LArHits:

            # See if this hit was produced by a primary particle. (The
            # answer is likely to be "no" if the primary particle is a
            # photon.)
            trackID = hit.TrackID()
            if trackID in primaries:
                # If we found one (not likely) print it out.
                print ("TrackID", hit.TrackID(), "HitID", hit.HitID(), "energy", hit.Energy())

        # For all the electron clusters in the event:
        for ( key, cluster ) in tree.ElectronClusters:
                  
            # See if this cluster was produced by a primary
            # particle. (Again, probably not.)
            trackID = cluster.TrackID()
            if trackID in primaries:
                print ("TrackID", cluster.TrackID(), "HitID", cluster.HitID(), "ClusterID", 
                       cluster.ClusterID(), "energy", cluster.EnergyAtAnode())

        # Let's look through all the readout channels in event.
        for (  readoutID, waveform ) in tree.ReadoutWaveforms:

            # Did any primary particle contribute to this waveform?
            # That's a trickier question to answer. There's a separate
            # object, ReadoutMaps, that links the waveforms to the
            # electron clusters. Look for this readoutID in that list.
            clusterKeys = tree.ReadoutMap[ readoutID ];

            # Let's look through the list of keys. A "cluster key" is
            # a triplet (trackID, hitID, clusterID). The last two are
            # arbitrary, and we're only interested in the first value.
            for ckey in clusterKeys:

                # ckey is a C++ tuple, not a python tuple. The access
                # rules can be different. We access the 0th, 1st, 2nd, ...
                # elements of a C++ tuple with get[0], get[1], get[2], ...
                trackID = ROOT.std.get[0]( ckey )
                
                # If the waveform came from a primary particle, print out
                # the waveform and exit the loop (there's no point in
                # printing the waveform for every matching cluster).
                if trackID in primaries:
                    print ("ReadoutID", readoutID.Index())
                    break
