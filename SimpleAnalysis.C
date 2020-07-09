{
  // A very simple look at the ntuples produced by
  // gramsg4. 

  gROOT->Reset();
  TFile inputFile("gramsg4.root");

  TNtuple* larhits = (TNtuple*)inputFile.Get("LArHits");
  TNtuple* trackinfo = (TNtuple*)inputFile.Get("TrackInfo");

  // This will be used as a selection term later on.
  const TString findLArTracks("VolNameStart==\"volTPCActive_PV\"");

  // The following assumes that 1000 events were generated in
  // gramsg4. 
  for ( int event = 0; event != 1000; ++event ) {
    // Select only those events that had hits. Note that
    // if the simulation was run in multi-threaded mode,
    // the event numbers will _not_ be in ascending order.
    TString selection = "Event==" + std::to_string(event);
    TTree* subTree = larhits->CopyTree(selection);

    // Did this event have any LAr hits?
    auto numLArHits = subTree->GetEntriesFast();

    if ( numLArHits > 0 ) {
      // Get the corresponding track info for that event.
      // Note that whether or not gramsg4 is run multi-threaded,
      // the track IDs will not be numerically sorted. 
      TTree* tracks = trackinfo->CopyTree(selection);

      // Display some counts, including those tracks that 
      // started in the active LAr. 
      std::cout << "Event " << event << " has "
		<< numLArHits << " hits and "
		<< tracks->GetEntriesFast() << " tracks; "
		<< tracks->GetEntries(findLArTracks) 
		<< " of these tracks started in the active LAr" << std::endl;

      // Clean up. 
      delete tracks;
    }

    delete subTree;
  }
}
