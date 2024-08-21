// To run this ROOT macro, you can do something like this:
//    root scripts/SimpleAnalysis.C

// This macro assumes that you're moderately familiar with
// SimpleAnalysis.C. I don't repeat every single comment in that
// macro.

bool debug = false;

void skyview() {

  // A quick-and-dirty look at the distribution of the origin of primary particles
  // in the output of GramsG4.

  // It can be used to validate the output of GramsSky, or to understand
  // the particles created by Geant4's "General Particle Source".

  // Open the input file.
  auto input = TFile::Open("gramsg4.root");

  // Create a "reader" for the tree in the input file.
  auto reader = new TTreeReader("gramsg4", input);

  // These are the only branches that we're interested in.
  TTreeReaderValue<grams::EventID>     EventID  (*reader, "EventID");
  TTreeReaderValue<grams::MCTrackList> TrackList(*reader, "TrackList");

  // In addition to drawing it, we're going to write our output histogram to this file.
  auto outputFile = TFile::Open("skyview.root","recreate");

  // Define the histogram(s) we're going to create.
  auto skyhist = new TH3D("skyview","Origin of primary particles in G4 Simulation",
			  100,-300,300,
			  100,-300,300,
			  100,-300,300);
    
  // To get these sizes and offsets, I ran the program, fiddled with
  // these values, then used "File->SaveAs" to create a .C macro. Then I
  // inspected the macro to get the commands I wanted.
  skyhist->GetXaxis()->SetTitle("x [cm]");
  skyhist->GetYaxis()->SetTitle("y [cm]");
  skyhist->GetZaxis()->SetTitle("z [cm]");
  skyhist->GetXaxis()->SetTitleSize(0.025);
  skyhist->GetYaxis()->SetTitleSize(0.025);
  skyhist->GetZaxis()->SetTitleSize(0.025);
  skyhist->GetXaxis()->SetLabelSize(0.02);
  skyhist->GetYaxis()->SetLabelSize(0.02);
  skyhist->GetZaxis()->SetLabelSize(0.02);
  skyhist->GetXaxis()->SetTitleOffset(2.4);
  skyhist->GetYaxis()->SetTitleOffset(2.4);
  skyhist->GetZaxis()->SetTitleOffset(2.0);
  skyhist->GetXaxis()->CenterTitle();
  skyhist->GetYaxis()->CenterTitle();
  skyhist->GetZaxis()->CenterTitle();

  // Read every entry (row) in the input tree. 
  while ( (*reader).Next() ) {

    // For each track in the list:
    for ( const auto& [ trackID, track ] : (*TrackList) ) {
    
        // We're only interested in primary particles.
        if ( track.Process() == "Primary" ) {

	  if (debug) 
	    std::cout << "Found primary for event " << (*EventID) << std::endl;

	  // Get the starting point of the particle's trajectory.

	  auto trajectory = track.Trajectory();
	  auto firstPoint = trajectory[0];
	  auto x = firstPoint.X();
	  auto y = firstPoint.Y();
	  auto z = firstPoint.Z();

	  if (debug) 
	    std::cout << "   (x,y,z) = (" << x
		      << "," << y
		      << "," << z
		      << ")" << std::endl;

	  // Add these coordinates to the histogram.
	  skyhist->Fill(x,y,z);

	} // if primary particle
      } // loop over tracks in the event
  } // read events in tree

  // Let's take a look. (This isn't in the python program, but works
  // nicely in interactive ROOT.)
  skyhist->Draw();
  
  // Wrap this up: 
  skyhist->Write();
  outputFile->Write();
} // skyview macro
