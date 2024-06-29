// To run this ROOT macro, you can do something like this:
//    root scripts/SimpleAnalysis.C

// This macro assumes you have created a GramsSim build directory
// following the recipe in the documentation:
// https://github.com/wgseligman/GramsSim/blob/develop/README.md and
// are running the script with the above command in that directory. If
// you've moved the script, then you'll have to copy and adjust the
// contents of rootlogon.C.

void SimpleAnalysis() {
  // A very simple look at the tree produced by gramsg4.

  // Open the input file.
  auto input = TFile::Open("gramsg4.root");

  // Create a "reader" for the tree in the input file.
  auto reader = new TTreeReader("gramsg4", input);

  // Create a TTreeReaderValue for each column in the tree whose value
  // we'll use. 

  // Note that the definitions for these custom classes (e.g.,
  // "grams::EventID") come from the GramsDataObj dictionary; see
  // GramsSim/GramsDataObj/README.md for details. They were loaded
  // into the ROOT intepreter by the file rootlogon.C.

  // Also note that these behave like pointers; e.g., you would use
  // *EventID in the code below.
  TTreeReaderValue<grams::EventID>     EventID     = {*reader, "EventID"};
  TTreeReaderValue<grams::MCTrackList> mcTrackList = {*reader, "TrackList"};
  TTreeReaderValue<grams::MCLArHits>   mcLArHits   = {*reader, "LArHits"};

  // Read every entry (row) in the input tree. 
  while ( (*reader).Next() ) {
 
    // Did this event have any LAr hits?
    auto numLArHits = mcLArHits->size();
    if ( numLArHits > 0 ) {

      // How many tracks were in this event?
      auto numTracks = mcTrackList->size();

      // How many of those tracks were in the LAr? Look at each track
      // in the list of Tracks.
      int numLArTracks = 0;
      for ( const auto& [ trackID, track ] : (*mcTrackList) ) {

	// Look at the trajectory of this track.
	auto trajectory = track.Trajectory();

	// For this simple analysis, we want to look at tracks whose
	// trajectories start in the LAr. Note that _none_ of the
	// primary particles start in the LAr; they start outside the
	// detector.
	auto trajectoryPoint = trajectory[0];
	auto identifier = trajectoryPoint.Identifier();

	// In the Identifier scheme documented in GramsSim/grams.gdml,
	// volume ID numbers in the LAr are seven-digit numbers that
	// begin with 1.
	auto volumeType = identifier / 1000000;
	if ( volumeType == 1 ) 
	  ++numLArTracks;

      } // loop over tracks.

      // Display some counts, including those tracks that 
      // started in the active LAr. 
      std::cout << "Event " << (*EventID) << " has "
		<< numLArHits << " hits and "
		<< numTracks << " tracks; "
		<< numLArTracks 
		<< " of these tracks started in the active LAr" << std::endl;

    } // If there were hits
  } // For each row in the tree
}
