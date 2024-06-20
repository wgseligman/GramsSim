#!/usr/bin/env python3
# AllFilesExample.py
# 20-Jun-2024 William Seligman <seligman@nevis.columbia.edu>

# This is an overly-commented example program to illustrate how to
# read multiple map-based ROOT files produced by GramsSim as if they
# were all part of the same tree. It is _not_ an example of how to
# perform a meaningful analysis on data. For that, see the ROOT
# tutorial at
# https://www.nevis.columbia.edu/~seligman/root-class/html/

#    TTTTT   H   H   IIIII   N   N   K   K 
#      T     H   H     I     NN  N   K  K  
#      T     HHHHH     I     N N N   KKK   
#      T     H   H     I     N  NN   K  K  
#      T     H   H   IIIII   N   N   K   K 

# At this moment, you're tempted to just copy this program and
# blindly place your analysis code in the main program loop. DON'T DO
# THIS WITHOUT THINKING ABOUT WHAT YOU'RE DOING! This program is
# meant as an example of the code needed to access a group of friend
# trees in parallel, and how the GramsSim maps link to each other.

# The odds are high that you don't need to read multiple columns from
# multiple trees in multiple files to accomplish your analysis
# task. Only use the files/trees/columns that you need. The
# 'dEdxExample' programs show this.

# If you just copy-and-page this code and automatically read every
# GramsSim file in the analysis without considering which
# files/trees/columns you need, you'll get a program that will run
# slowly, use more memory, and be more error prone. THINK! THINK!
# THINK!

import ROOT

# We'll also need the GramsSim custom dictionary for its data objects
# (see GramsSim/GramsDataObj/README.md and
# https://www.nevis.columbia.edu/~seligman/root-class/html/appendix/dictionary/index.html).
# The following statement assumes this program is being run from the
# build directory you created by following the directions in
# https://github.com/wgseligman/GramsSim/tree/develop:

ROOT.gSystem.Load("./GramsDataObj/libGramsSimProjectDataObj.so")

# Open the input file and access the n-tuple. 
inputFile = ROOT.TFile("gramselecsim.root")
tree = ROOT.gDirectory.Get( 'ElecSim' )

# Now we add additional columns/friends to that tree. (If you ask
# how I knew the names of all these files and trees, I got them
# from GramsSim/options.xml. If I were writing a more complete
# program, I'd get them from using the Options class (see
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
    # GramsSim/GramsDataObj/include.

    # Let's pick an arbitrary event. How about run=0, event=3,
    # which corresponds to a grams::EventID.Index() of 3? 
    arbitraryEvent = 3

    # For only that event:
    if ( tree.EventID.Index() == arbitraryEvent ):

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
                # If we found one (not likely) print it out. Note that the
                # data objects all have output operators defined.
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

            # Let's look through the list of keys. A "cluster key" is a
            # triplet (std::tuple) of numbers: (trackID, hitID,
            # clusterID). The last two are arbitrary, and we're only
            # interested in the first value.
            for ( trackID, hitID, clusterID ) in clusterKeys:

                # If the waveform came from a primary particle, print out
                # the waveform and exit the loop (there's no point in
                # printing the waveform for every matching cluster).
                if trackID in primaries:
                    print ("ReadoutID", readoutID.Index())
                    break
                    
    # The above shows how to "go forward" through the
    # tracks.hits.clusters.readout chain of information. What
    # follows is the reverse: How one might back-track up the chain.

    # For every waveform in the event:
    for ( readoutID, waveform ) in tree.ReadoutWaveforms:

        # Sum the ADC counts in all the bins in the digital
        # waveform. (There is no scientific reason to do this.)
        sumADC = sum( waveform.Digital() )

        # If the sum of the ADC counts is greater than some arbitrary
        # number that I just made up:
        madeUpNumber = 219697;
        if sumADC > madeUpNumber:

            # The data objects in the dictionary all of the C++ operator<<
            # defined for them, which simplifies print them out in C++. Unfortunately, 
            # there doesn't seem to be a simple way of calling operator<< from within 
            # Python. For these data objects, just display a few details to prove
            # we have access to their information.
            print ( "EventID ", tree.EventID.Index(), "ReadoutID", readoutID.Index(), 
                    "sums to", sumADC, "which is more than", madeUpNumber )

            # Let's assume that this means the waveform is
            # "interesting". Look at all the electron clusters that went
            # into creating this waveform.
            # 'ReadoutMap' is a map of (readoutID, clusterKeys). The
            # cluster keys are the second element in this pair.
            clusterKeys = tree.ReadoutMap[ readoutID ]

            # The cluster keys are, in turn, a set of keys into the
            # ElectronClusters map. The following looks at all the
            # electron clusters associated with those keys.

            # If this seems complex, you can think of this arrangement as:
            #
            # (a) a multi-dimensional array of indexed by [trackID][hitID][clusterID][readoutID];
            # (b) if you're into Python, nested dictionaries (dicts of dicts).. 

            for clusterKey in clusterKeys:
                electronCluster = tree.ElectronClusters[ clusterKey ]

                # What can we do with an electron cluster? We might print
                # it out, according to some imaginary non-scientific
                # criteria.
                if electronCluster.NumElectrons() < 38:
                    print ("TrackID", electronCluster.TrackID(), "HitID", electronCluster.HitID(), "ClusterID", 
                           electronCluster.ClusterID(), "energy", electronCluster.EnergyAtAnode())

                    # We can also use it to back-track to a particular
                    # simulated hit in the LAr. The data object MCLArHits is,
                    # once again, a map of (key,value) pairs, with the the key
                    # itself being a pair of (trackID,hitID):
                    
                    trackID = electronCluster.TrackID()
                    hitID   = electronCluster.HitID()
                    hitKey = ROOT.std.make_tuple( trackID, hitID )
                    hit = tree.LArHits[ hitKey ]

                    # Now we have the MCLArHit associated with this particular
                    # electron cluster. Let's apply an arbitrary non-scientific
                    # cut on the number of scintillation photons to show how to
                    # work with a hit.
                    numPhotons = hit.NumPhotons()
                    if numPhotons < 200:

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
