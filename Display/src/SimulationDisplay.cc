// SimulationDisplay.cc
// 02-Aug-2024 WGS

// This is an evolving program to function as a rough display for
// GramsSim output. It started as an example by Andrea Pellot Jimenez
// that displayed information from the electron clusters arriving at
// the anode. I (WGS) added the user interface and additional plots
// for the results after each stage of the simulation.

// Elements of this code were taken from files in the following
// directory. I ran the different examples, and if I saw the kinds of
// things I wanted to do, I examined the code. It's confusing, but
// eventually the code started to make sense.

// `root-config --tutdir`/gui/

// This page was also quite helpful:
// https://root.cern/root/htmldoc/guides/users-guide/WritingGUI.html

#include "SimulationDisplay.h"

// GramsSim includes
#include "Options.h"
#include "EventID.h"
#include "MCTrackList.h"
#include "MCLArHits.h"
#include "ElectronClusters.h"
#include "ReadoutID.h"
#include "ReadoutMap.h"
#include "ReadoutWaveforms.h"

// ROOT includes
#include <TStyle.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TPaletteAxis.h>
#include <TColor.h>
#include <TGeoManager.h>
#include <TRegexp.h>
#include <TGeoBBox.h>
#include <TDatabasePDG.h>
#include <TParticlePDG.h>
#include <Math/Vector3D.h>
#include <Math/Vector4D.h>
#include <TPolyLine.h>
#include <TPolyLine3D.h>
#include <TApplication.h>
#include <TGClient.h>
#include <TGButton.h>
#include <TGNumberEntry.h>
#include <TGLabel.h>
#include <TGFrame.h>
#include <TGLayout.h>
#include <TGWindow.h>
#include <TGFileDialog.h>
#include <TGDockableFrame.h>
#include <TGNumberEntry.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TRootEmbeddedCanvas.h>
#include <Riostream.h>

// C++ includes
#include <iostream>
#include <string>
#include <map>
#include <cmath>

// ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....

bool debug=false;

namespace grams {

  // Constructor. Define all the widgets that will appear within the
  // main frame.
  SimulationDisplay::SimulationDisplay(const TGWindow* a_window)
    : TGMainFrame(a_window, 10, 10, kMainFrame | kVerticalFrame)
    {
      // These numbers come a size that looks nice on my screen. This
      // may not be ideal for everyone. Fortunately, the size of the
      // window can be adjusted by the user by dragging on its lower
      // right-hand corner.
      int canvasWidth=814;
      int canvasHeight=778;
      
      // When this display exits, clean up all the widgets it contains. 
      this->SetCleanup(kDeepCleanup);

      // The routine that's executed when the frame's window is closed.
      this->Connect("CloseWindow()", "grams::SimulationDisplay", this, "DoExit()");

      // Make sure no other CloseWindows methods are called except for
      // ours.
      this->DontCallClose();

      /////////////////////////////////////////////////
      // Create a menu bar at the top of the window. The hint objects
      // are used to place and group the different menu widgets with
      // respect to each other. The last four numbers supplied to
      // TGLayoutHints are the padding on the left, right, top, and
      // bottom.

      fMenuDock = new TGDockableFrame(this);
      this->AddFrame(fMenuDock, new TGLayoutHints(kLHintsExpandX, 0, 0, 1, 0));
      fMenuDock->SetWindowName("SimulationDisplay Menu");
      
      fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX);
      fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);

      // Create the pop-up menus that will appear in the menu bar.

      /////////////////////////////////////////////////
      // This map is used in the File->Save menu. It defines a match
      // between a file-related signal and the file type associated
      // with it.
      MyFileTypeMap[ M_SAVEAS_C    ] = ".C";
      MyFileTypeMap[ M_SAVEAS_ROOT ] = ".root";
      MyFileTypeMap[ M_SAVEAS_PS   ] = ".ps";
      MyFileTypeMap[ M_SAVEAS_EPS  ] = ".eps";
      MyFileTypeMap[ M_SAVEAS_PDF  ] = ".pdf";
      MyFileTypeMap[ M_SAVEAS_SVG  ] = ".svg";
      MyFileTypeMap[ M_SAVEAS_TEX  ] = ".tex";
      MyFileTypeMap[ M_SAVEAS_GIF  ] = ".gif";
      MyFileTypeMap[ M_SAVEAS_PNG  ] = ".png";
      MyFileTypeMap[ M_SAVEAS_JPG  ] = ".jpg";
      MyFileTypeMap[ M_SAVEAS_TIFF ] = ".tiff";
      MyFileTypeMap[ M_SAVEAS_XML  ] = ".xml";
      MyFileTypeMap[ M_SAVEAS_JSON ] = ".json";
      
      // This creates the "File" menu.
      MyMenuFile = new TGPopupMenu(a_window);

      // The "Save" menu entry will have a sub-list with all the
      // different formats that I want to support; in other words, too
      // many. The map defines all the allowed formats, so we can loop
      // over the map to define the menu items.

      // The definitions in this loop are temporary. They will be
      // overridden as the plot/event description changes.
      MyMenuSave = new TGPopupMenu(a_window);
      for ( const auto& [ signal, extension ] : MyFileTypeMap ) {
	std::string entryName = "display" + extension;
	MyMenuSave->AddEntry(entryName.c_str(), signal);
      }

      // This creates the File->Save sub-menu. 
      MyMenuFile->AddPopup("Save", MyMenuSave);

      // The rest of the File menu:
      MyMenuFile->AddEntry("Save as...",   M_SAVEAS_DIALOG);
      MyMenuFile->AddSeparator();
      MyMenuFile->AddEntry("Exit",         M_FILE_EXIT);

      // The menu handler for the File menu.
      MyMenuFile->Connect("Activated(Int_t)", "grams::SimulationDisplay", this,
			  "HandleFileMenu(Int_t)");

      /////////////////////////////////////////////////
      // This defines the labels and limits that change depending on
      // the kind of plot we show. At this point, just define those
      // strings that appear in the menus; the rest are defined in
      // SetupHistogram.
      MyPlotsInfo[ M_PLOT_TRACKS ]   .menuDesc   = "MC Tracks";
      MyPlotsInfo[ M_PLOT_TRACKS ]   .savePrefix = "tracks_";
      MyPlotsInfo[ M_PLOT_HITS ]     .menuDesc   = "LAr MC Hits";
      MyPlotsInfo[ M_PLOT_HITS ]     .savePrefix = "hits_";
      MyPlotsInfo[ M_PLOT_CLUSTERS ] .menuDesc   = "Electron Clusters";
      MyPlotsInfo[ M_PLOT_CLUSTERS ] .savePrefix = "clusters_";
      MyPlotsInfo[ M_PLOT_READOUT ]  .menuDesc   = "Energy in Readout Channels";
      MyPlotsInfo[ M_PLOT_READOUT ]  .savePrefix = "readout_";
      MyPlotsInfo[ M_PLOT_WAVEFORMS ].menuDesc   = "ADC counts in Readout Channels";
      MyPlotsInfo[ M_PLOT_WAVEFORMS ].savePrefix = "waveforms_";
      
      // This defines the "Plot" menu.
      MyMenuPlot = new TGPopupMenu(a_window);

      // The "Plot" menu is defined by all the different kinds of
      // plots this program can make.
      for ( const auto& [ signal, plotStrings ] : MyPlotsInfo ) {
	std::string entryName = plotStrings.menuDesc;
	MyMenuPlot->AddEntry(entryName.c_str(), signal);
      }

      // The menu handler for the Plot menu.
      MyMenuPlot->Connect("Activated(Int_t)", "grams::SimulationDisplay", this,
			  "HandlePlotMenu(Int_t)");

      /////////////////////////////////////////////////
      // This defines the "View" menu.
      MyMenuView = new TGPopupMenu(a_window);
      MyMenuView->AddEntry("3D View",      M_VIEW_3D);
      MyMenuView->AddEntry("View along X", M_VIEW_X);
      MyMenuView->AddEntry("View along Y", M_VIEW_Y);
      MyMenuView->AddEntry("View along Z", M_VIEW_Z);
      MyMenuView->AddSeparator();
      MyMenuView->AddEntry("Fine",         M_VIEW_FINE);
      MyMenuView->AddEntry("Medium",       M_VIEW_MEDIUM);
      MyMenuView->AddEntry("Coarse",       M_VIEW_COARSE);
      MyMenuView->AddSeparator();
      MyMenuView->AddEntry("Show tracks",  M_VIEW_ADD_TRACKS);

      // The menu handler for the View menu. 
      MyMenuView->Connect("Activated(Int_t)", "grams::SimulationDisplay", this,
			  "HandleViewMenu(Int_t)");

      // Add the menu(s) to the menu bar. 
      fMenuBar = new TGMenuBar(fMenuDock, 1, 1, kHorizontalFrame);
      fMenuBar->AddPopup("File", MyMenuFile, fMenuBarItemLayout);
      fMenuBar->AddPopup("Plot", MyMenuPlot, fMenuBarItemLayout);
      fMenuBar->AddPopup("View", MyMenuView, fMenuBarItemLayout);
      fMenuDock->AddFrame(fMenuBar, fMenuBarLayout);

      /////////////////////////////////////////////////
      // Create canvas widget. The histograms will be displayed within
      // this canvas.
      MyCanvasWidget = new TRootEmbeddedCanvas("SimulationDisplay",this,canvasWidth,canvasHeight);
      MyCanvas = MyCanvasWidget->GetCanvas();
      this->AddFrame(MyCanvasWidget, new TGLayoutHints(kLHintsExpandX |
						       kLHintsExpandY, 10,10,10,1));

      /////////////////////////////////////////////////
      // Create a horizontal frame widget for the buttons.
      MyButtonFrame = new TGHorizontalFrame(this,canvasWidth,40);
      this->AddFrame(MyButtonFrame, new TGLayoutHints(kLHintsBottom | kLHintsExpandX,
						      2,2,2,2));

      // Define the buttons. Note that each button has to be connected
      // to a routine that will be activated when the button is
      // clicked.
      TGTextButton* PrevEvent = new TGTextButton(MyButtonFrame,"Prev Event");
      PrevEvent->Connect("Clicked()", "grams::SimulationDisplay", this, "PrevEvent()");
      PrevEvent->SetToolTipText("Display the previous event");
      MyButtonFrame->AddFrame(PrevEvent, new TGLayoutHints(kLHintsLeft,5,5,3,4));      

      TGTextButton* NextEvent = new TGTextButton(MyButtonFrame,"Next Event");
      NextEvent->Connect("Clicked()", "grams::SimulationDisplay", this, "NextEvent()");
      NextEvent->SetToolTipText("Display the next event");
      MyButtonFrame->AddFrame(NextEvent, new TGLayoutHints(kLHintsLeft,5,5,3,4));      

      TGTextButton* ExitButton = new TGTextButton(MyButtonFrame,"Exit");
      ExitButton->Connect("Clicked()", "grams::SimulationDisplay", this, "DoExit()");
      ExitButton->SetToolTipText("Quit the program");
      MyButtonFrame->AddFrame(ExitButton, new TGLayoutHints(kLHintsRight,5,5,3,4));      

      /////////////////////////////////////////////////
      // Define the entry boxes for the run and event number. The idea
      // is to both display the current run and event number, and
      // allow the user to enter a specific run/event if they choose.

      // To group the run/event elements together, create a "frame within a frame".
      auto entryFrame = new TGHorizontalFrame(MyButtonFrame);

      auto runLabel = new TGLabel(entryFrame, "Run: ");
      entryFrame->AddFrame(runLabel, new TGLayoutHints(kLHintsCenterX,0,0,5,0));
      MyRunEntry = new TGNumberEntry(entryFrame, 0, 4, M_RUN_ENTRY,
				     TGNumberFormat::kNESInteger,
				     TGNumberFormat::kNEANonNegative,
				     TGNumberFormat::kNELLimitMin,0);
      entryFrame->AddFrame(MyRunEntry, new TGLayoutHints(kLHintsCenterX));      

      // A TGNumberEntry field needs two signals to be set: One for
      // when the user clicks the arrows in the field, and one for
      // when the user enters a new number and hits "return".
      MyRunEntry->Connect("ValueSet(Long_t)", "grams::SimulationDisplay", this, "SetRunEntry()");
      (MyRunEntry->GetNumberEntry())->Connect("ReturnPressed()", "grams::SimulationDisplay", this,
					      "SetRunEntry()");

      auto eventLabel = new TGLabel(entryFrame, "Event: ");
      entryFrame->AddFrame(eventLabel, new TGLayoutHints(kLHintsCenterX,15,0,5,0));
      MyEventEntry = new TGNumberEntry(entryFrame, 0, 9, M_EVENT_ENTRY,
				       TGNumberFormat::kNESInteger,
				       TGNumberFormat::kNEANonNegative,
				       TGNumberFormat::kNELLimitMin,0);
      entryFrame->AddFrame(MyEventEntry, new TGLayoutHints(kLHintsCenterX));

      MyEventEntry->Connect("ValueSet(Long_t)", "grams::SimulationDisplay", this, "SetEventEntry()");
      (MyEventEntry->GetNumberEntry())->Connect("ReturnPressed()", "grams::SimulationDisplay", this,
						"SetEventEntry()");

      MyButtonFrame->AddFrame(entryFrame, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

      /////////////////////////////////////////////////
      // Intercept "events" (not physics events, mouse/keyboard
      // events) within the frame. The idea is to go to the
      // previous/next physics event in response to the arrow keys.
      gClient->Connect("ProcessedEvent(Event_t*, Window_t)",
		       "grams::SimulationDisplay", this,
		       "HandleKeypress(Event_t*)");
      
      /////////////////////////////////////////////////
      // This defines the name of the main frame. It's also the
      // default name of any files saved by the user if they do the
      // unusual: Right-click on the canvas and select "SaveAs" from
      // ROOT's pop-up menu.
      this->SetName("SimulationDisplay");

      // The name shown at the top of the main frame. 
      this->SetWindowName("Simulation Display");

      // Map all subwindows of main frame. This causes all the of the
      // above widgets to appear in the main frame.
      this->MapSubwindows();
      
      // Initialize the layout algorithm. This determines the layout
      // of widgets within the main frame; this includes interpreting
      // all the layout hints.
      this->Resize(this->GetDefaultSize());
      
      // Map main frame. This causes the main frame itself to appear
      // on the user's screen.
      this->MapWindow();
      
      /////////////////////////////////////////////////
      // Initialize default values for reading and displaying events.
      CurrentEntry = 0;
      MyTree = nullptr;
      MyFile = nullptr;
      MyHist = nullptr;
      MyTextBox = nullptr;
      MyDescBox = nullptr;
      MyEmptyBox = nullptr;

      WaveformSize = 0;
      
      // Define the branches we'll want from this tree. We're defining
      // them separately, instead of using the TTreeReader
      // method. TTreeReader is limited to scanning a tree from
      // beginning to end; in this macro we're going to read events
      // randomly from the input.

      MyEventID    = new grams::EventID;
      MyTrackList  = new grams::MCTrackList;
      MyLArHits    = new grams::MCLArHits;
      MyClusters   = new grams::ElectronClusters;
      MyReadoutMap = new grams::ReadoutMap;
      MyWaveforms  = new grams::ReadoutWaveforms;
      
      // Open the first file and its tree.
      MyFile = TFile::Open("gramselecsim.root");
      MyTree = MyFile->Get<TTree>("ElecSim");
   
      // Declare the friend trees; that is, the files that contain
      // the other columns for this tree.
      MyTree->AddFriend("gramsg4",    "gramsg4.root");
      MyTree->AddFriend("DetSim",     "gramsdetsim.root");
      MyTree->AddFriend("ReadoutSim", "gramsreadoutsim.root");
   
      // Define the branches we'll read from the collection of friend
      // trees.
      MyTree->SetBranchAddress("EventID",          &MyEventID);
      MyTree->SetBranchAddress("ElectronClusters", &MyClusters);
      MyTree->SetBranchAddress("LArHits",          &MyLArHits);
      MyTree->SetBranchAddress("TrackList",        &MyTrackList);
      MyTree->SetBranchAddress("ReadoutMap",       &MyReadoutMap);
      MyTree->SetBranchAddress("ReadoutWaveforms", &MyWaveforms);

      // Set up the Options class, so we can get the options that were
      // used to generate these trees.
      options = util::Options::GetInstance();
      // Copy the Options n-tuple from the first ROOT file we opened
      // above.
      options->CopyInputNtuple(MyFile);
      if (debug) {
	options->PrintOptions();
      }

      // Set up the histogram parameters.
      SetUpHistogram();

      // Display the initial histogram: a fine 3D view of entry 0. 
      UpdateDisplay();
    };

  // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....

  SimulationDisplay::~SimulationDisplay()
    {
      // Destructor. This deletes all the frames that were created in
      // the constructor.
      this->Cleanup();
    }

  // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....

  void SimulationDisplay::DoExit()
  {
    // Exit this application via the Exit button or the Window Manager.
    gApplication->Terminate();   // to exit and close the ROOT session
  }

  // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....

  // Find the primary particle(s) in the current event. Note that
  // though the code is written to "find all primaries", as of
  // Aug-2024 all events are generated with a single primary particle.
  void SimulationDisplay::FindPrimaries() {

    if (debug) std::cout << "FindPrimaries" << std::endl;

    // Clear the lists we'll fill.
    pdg.clear();
    position4D.clear();
    momentum4D.clear();

    // Iterate over tracks.
    for (const auto& [trackID, track] : *MyTrackList) {

      // If this is a primpary particle:
      if (track.Process() == "Primary") {

	// Get the particles trajectory through the simulation.
	const auto& trajectory = track.Trajectory();
	if (!trajectory.empty()) {
	
	  // Save the particle's kinematics and PDG code at the start
	  // of its trajectory. Since this is a primary particle, this
	  // will be the original generated energy of the particle.

	  auto start = trajectory[0];
	  position4D.push_back( start.Position4D() );
	  momentum4D.push_back( start.Momentum4D() );
	  pdg.push_back( track.PDGCode() );
	}
      } // If primary particle
    } // Loop over tracks
  } // FindPrimaries

  // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....

  // For the hits plot, accumulate the values associated with the
  // ionization energy deposits in the liquid argon.
  void SimulationDisplay::AccumulateHits() {

    if (debug) std::cout << "AccumulateHits" << std::endl;
    
    // Iterate over all hits.
    for (const auto& [key, hit] : *MyLArHits) {
      // Save the values for each hit.
      e_val.push_back( hit.E() );
      auto midpoint = ( hit.Start4D() + hit.End4D() ) / 2.;
      x_value.push_back( midpoint.X() );
      y_value.push_back( midpoint.Y() );
      z_value.push_back( midpoint.Z() );
    } // loop over hits
  } // AccumulateHits

  // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....
  
  // For the cluster plot, accumulate the values associated with the
  // electron clusters.
  void SimulationDisplay::AccumulateClusters() {

    if (debug) std::cout << "AccumulateClusters" << std::endl;
    
    // Iterate over all clusters.
    for (const auto& [key, cluster] : *MyClusters) {

      // It's possible for a cluster to have zero energy (none of the
      // electrons reach the anode). If so, don't record the cluster's
      // information.
      double energy_at_anode = cluster.EnergyAtAnode();
      if ( energy_at_anode > 0 ) {
	e_val.push_back(energy_at_anode);
	auto pos4D = cluster.PositionAtAnode4D();
	
	// Extract the (x,y,z,t) values for this cluster.
	x_value.push_back(pos4D.X());
	y_value.push_back(pos4D.Y());
	// The units of time are ns (as defined in options.xml), but
	// for our display microseconds are more convenient. We also
	// want to reverse the values on the time axis, so the anode
	// is at the top of the plot.
	z_value.push_back(-pos4D.T() / 1000);
      } // anode energy > 0
    } // loop over clusters
  } // AccumulateClusters

  // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....
  
  // For the readout channel plot, accumulate the values associated
  // with the output of GramsReadoutSim.
  void SimulationDisplay::AccumulateReadout() {

    if (debug) std::cout << "AccumulateReadout" << std::endl;
    
    // Iterate over all readout channels.
    for (const auto& [readoutID, ckeys] : *MyReadoutMap) {

      auto xID = readoutID.X();
      auto yID = readoutID.Y();

      // Loop over all the clusters that arrive in this readout channel.
      for ( const auto& ckey : ckeys ) {
	// "ckey" is a key for the electron-cluster list.
	const auto search = MyClusters->find( ckey );

	// If the search failed, we've got a problem: Somehow a
	// cluster key got added to the readout but the corresponding
	// cluster does not exist.
	if ( search == MyClusters->cend() ) {
	  const auto& [ trackID, hitID, clusterID ] = ckey;
	  std::cerr << "AccumulateReadout - ERROR - "
		    << "Readout channel " << readoutID
		    << " contains a non-existent cluster "
		    << "[" << trackID << "," << hitID << "," << clusterID << "]"
		    << std::endl;
	  break;
	}

	// Recall that a map consists of (key,value) pairs referred to
	// by (first,second). For the ElectronClusters map, 'second'
	// is an ElectronCluster.
	auto cluster = (*search).second;
            
	// It's possible for a cluster to have zero energy (none of the
	// electrons reach the anode). If so, don't record the cluster's
	// information.
	double energy_at_anode = cluster.EnergyAtAnode();
	if ( energy_at_anode > 0 ) {
	  e_val.push_back(energy_at_anode);
	
	  // For (x,y), use the ID from the readout channel.
	  x_value.push_back(xID);
	  y_value.push_back(yID);
	  // The units of time are ns (as defined in options.xml), but
	  // for our display microseconds are more convenient. We also
	  // want to reverse the values on the time axis, so the anode
	  // is at the top of the plot.
	  z_value.push_back(-cluster.TAtAnode() / 1000);
	} // anode energy > 0
      } // loop over clusters
    } // loop over readout channels
  } // AccumulateReadout

  // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....
  
  // For the readout waveform plot, accumulate the values associated
  // with the output of GramsElecSim.
  void SimulationDisplay::AccumulateWaveforms() {

    if (debug) std::cout << "AccumulateWaveforms" << std::endl;

    // Assumption: The length of the digital (ADC) waveform will be
    // the same for all channels. This might change; e.g., if we start
    // saving only "regions of interest" (ROI) in the waveforms. For
    // now, save the length of waveform in the first channel, to use
    // when creating the histogram.
    WaveformSize = 0;
    
    // Iterate over all digital waveform channels.
    for (const auto& [readoutID, waveform] : *MyWaveforms) {

      auto const& digital = waveform.Digital();
      auto xID = readoutID.X();
      auto yID = readoutID.Y();

      // Determine the number of bins on the z-axis from the size of
      // ADC vector.
      int numADCbins = digital.size();
      if ( numADCbins > WaveformSize ) {
	if (debug) std::cout << "AccumulateWaveforms "
			     << " WaveformSize=" << WaveformSize
			     << " numADCbins=" << numADCbins
			     << std::endl;
	WaveformSize = numADCbins;
      }
      
      // For the purposes of this plot, what we want are the ADC
      // counts as a function of time bin.
      
      for ( int i = 0; i != numADCbins; ++i ) {
	auto ADCcounts = digital[i];
	if ( ADCcounts > 0 ) {
	  x_value.push_back( xID );
	  y_value.push_back( yID );
	  // We have to accumulate negative "z" for the anode to appear
	  // at the top of the plot.
	  z_value.push_back( -i ); 
	  e_val.push_back( ADCcounts );
	} // ADC counts > 0
      } // loop over digital waveform
    } // loop over readout channels

    // The length of the waveform vector determines the number of bins
    // on the z-axis of the histogram.
    MyPlotsInfo[ M_PLOT_WAVEFORMS ].zLow  = -WaveformSize;
    MyPlotsInfo[ M_PLOT_WAVEFORMS ].zBins =  WaveformSize;

    if (debug) std::cout << "AccumulateWaveforms final WaveformSize="
			 << WaveformSize << std::endl;
  } // AccumulateWaveforms

  // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....

  // The "HistogramViewXX" routines process the different views of the
  // same data.  Each routine takes the accumulated data for an event
  // (see AccumulateXX()) and uses it to fill a histogram.

  TH1* SimulationDisplay::HistogramView3D() {

    if (debug) std::cout << "HistogramView3D"
			 << " xBins=" << MyPlotsInfo[ PlotKind ].xBins
			 << " xLow="  << MyPlotsInfo[ PlotKind ].xLow
			 << " xHigh=" << MyPlotsInfo[ PlotKind ].xHigh
			 << " yBins=" << MyPlotsInfo[ PlotKind ].yBins
			 << " yLow="  << MyPlotsInfo[ PlotKind ].yLow
			 << " yHigh=" << MyPlotsInfo[ PlotKind ].yHigh
			 << " zBins=" << MyPlotsInfo[ PlotKind ].zBins
			 << " zLow="  << MyPlotsInfo[ PlotKind ].zLow
			 << " zHigh=" << MyPlotsInfo[ PlotKind ].zHigh
			 << " scale=" << MyScaleFactor[ ScaleID ]
			 << std::endl;
    
    auto theHist = new TH3D("hist", "", 
			    MyPlotsInfo[ PlotKind ].xBins / MyScaleFactor[ ScaleID ],
			    MyPlotsInfo[ PlotKind ].xLow,
			    MyPlotsInfo[ PlotKind ].xHigh,
			    MyPlotsInfo[ PlotKind ].yBins / MyScaleFactor[ ScaleID ],
			    MyPlotsInfo[ PlotKind ].yLow,
			    MyPlotsInfo[ PlotKind ].yHigh,
			    MyPlotsInfo[ PlotKind ].zBins / MyScaleFactor[ ScaleID ],
			    MyPlotsInfo[ PlotKind ].zLow,
			    MyPlotsInfo[ PlotKind ].zHigh );
    for (size_t i = 0; i < e_val.size(); i++) {
      theHist->Fill(x_value[i], y_value[i], z_value[i], e_val[i]);
    }

    // These parameters were determined interactively: I created the
    // histogram, moved the display elements using the mouse, saved
    // the canvas by right-clicking on it and selecting SaveAs, then
    // examining the .C file.
    theHist->SetStats(kFALSE);
    theHist->GetXaxis()->SetTitle(MyPlotsInfo[ PlotKind ].xValueTitle.c_str());
    theHist->GetYaxis()->SetTitle(MyPlotsInfo[ PlotKind ].yValueTitle.c_str());
    theHist->GetZaxis()->SetTitle(MyPlotsInfo[ PlotKind ].zValueTitle3D.c_str());
    theHist->GetXaxis()->SetTitleSize(0.025);
    theHist->GetYaxis()->SetTitleSize(0.025);
    theHist->GetZaxis()->SetTitleSize(0.025);
    theHist->GetXaxis()->SetLabelSize(0.02);
    theHist->GetYaxis()->SetLabelSize(0.02);
    theHist->GetZaxis()->SetLabelSize(0.02);
    theHist->GetXaxis()->SetTitleOffset(2.4);
    theHist->GetYaxis()->SetTitleOffset(2.4);
    theHist->GetZaxis()->SetTitleOffset(2.0);
    theHist->GetXaxis()->CenterTitle();
    theHist->GetYaxis()->CenterTitle();
    theHist->GetZaxis()->CenterTitle();

    MyCanvas->SetRightMargin(0.13);

    // Only draw a palette if the histogram has entries.
    std::string drawOptions;
    if ( theHist->GetEntries() > 0 )
      drawOptions="LEGO2Z 0";
    theHist->Draw(drawOptions.c_str());

    return theHist;
  } // HistogramView3D

  // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....
  
  TH1* SimulationDisplay::HistogramViewX() {

    if (debug) std::cout << "HistogramViewX "
			 << " xBins=" << MyPlotsInfo[ PlotKind ].xBins
			 << " xLow="  << MyPlotsInfo[ PlotKind ].xLow
			 << " xHigh=" << MyPlotsInfo[ PlotKind ].xHigh
			 << " zBins=" << MyPlotsInfo[ PlotKind ].zBins
			 << " zLow="  << MyPlotsInfo[ PlotKind ].zLow
			 << " zHigh=" << MyPlotsInfo[ PlotKind ].zHigh
			 << " scale=" << MyScaleFactor[ ScaleID ]
			 << std::endl;

    auto theHist = new TH2D("hist", "", 
			    MyPlotsInfo[ PlotKind ].xBins / MyScaleFactor[ ScaleID ],
			    MyPlotsInfo[ PlotKind ].xLow,
			    MyPlotsInfo[ PlotKind ].xHigh,
			    MyPlotsInfo[ PlotKind ].zBins / MyScaleFactor[ ScaleID ],
			    MyPlotsInfo[ PlotKind ].zLow,
			    MyPlotsInfo[ PlotKind ].zHigh );
    for (size_t i = 0; i < e_val.size(); i++) {
      theHist->Fill(y_value[i], z_value[i], e_val[i]);
    }

    theHist->SetStats(kFALSE);
    theHist->GetXaxis()->SetTitle(MyPlotsInfo[ PlotKind ].yValueTitle.c_str());
    theHist->GetYaxis()->SetTitle(MyPlotsInfo[ PlotKind ].zValueTitle.c_str());
    theHist->GetZaxis()->SetTitle(MyPlotsInfo[ PlotKind ].paletteTitle.c_str());
    theHist->GetXaxis()->SetTitleSize(0.025);
    theHist->GetYaxis()->SetTitleSize(0.025);
    theHist->GetZaxis()->SetTitleSize(0.025);
    theHist->GetXaxis()->SetLabelSize(0.025);
    theHist->GetYaxis()->SetLabelSize(0.02);
    theHist->GetZaxis()->SetLabelSize(0.02);
    theHist->GetXaxis()->SetTitleOffset(1.5);
    theHist->GetYaxis()->SetTitleOffset(2.0);
    theHist->GetZaxis()->SetTitleOffset(1.75);
    theHist->GetXaxis()->CenterTitle();
    theHist->GetYaxis()->CenterTitle();

    MyCanvas->SetRightMargin(0.13);

    // Only draw a palette if the histogram has entries.
    std::string drawOptions;
    if ( theHist->GetEntries() > 0 )
      drawOptions="COLZ";
    theHist->Draw(drawOptions.c_str());

    return theHist;
  } // HistogramViewX

  // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....
  
  TH1* SimulationDisplay::HistogramViewY() {

    if (debug) std::cout << "HistogramViewY"
			 << " yBins=" << MyPlotsInfo[ PlotKind ].yBins / MyScaleFactor[ ScaleID ]
			 << " yLow="  << MyPlotsInfo[ PlotKind ].yLow
			 << " yHigh=" << MyPlotsInfo[ PlotKind ].yHigh
			 << " zBins=" << MyPlotsInfo[ PlotKind ].zBins / MyScaleFactor[ ScaleID ]
			 << " zLow="  << MyPlotsInfo[ PlotKind ].zLow
			 << " zHigh=" << MyPlotsInfo[ PlotKind ].zHigh
			 << " scale=" << MyScaleFactor[ ScaleID ]
			 << std::endl;

    auto theHist = new TH2D("hist", "", 
			    MyPlotsInfo[ PlotKind ].yBins,
			    MyPlotsInfo[ PlotKind ].yLow,
			    MyPlotsInfo[ PlotKind ].yHigh,
			    MyPlotsInfo[ PlotKind ].zBins,
			    MyPlotsInfo[ PlotKind ].zLow,
			    MyPlotsInfo[ PlotKind ].zHigh );
    for (size_t i = 0; i < e_val.size(); i++) {
      theHist->Fill(x_value[i], z_value[i], e_val[i]);
    }

    theHist->SetStats(kFALSE);
    theHist->GetXaxis()->SetTitle(MyPlotsInfo[ PlotKind ].xValueTitle.c_str());
    theHist->GetYaxis()->SetTitle(MyPlotsInfo[ PlotKind ].zValueTitle.c_str());
    theHist->GetZaxis()->SetTitle(MyPlotsInfo[ PlotKind ].paletteTitle.c_str());
    theHist->GetXaxis()->SetTitleSize(0.025);
    theHist->GetYaxis()->SetTitleSize(0.025);
    theHist->GetZaxis()->SetTitleSize(0.025);
    theHist->GetXaxis()->SetLabelSize(0.02);
    theHist->GetYaxis()->SetLabelSize(0.02);
    theHist->GetZaxis()->SetLabelSize(0.02);
    theHist->GetXaxis()->SetTitleOffset(1.5);
    theHist->GetYaxis()->SetTitleOffset(2.0);
    theHist->GetZaxis()->SetTitleOffset(1.75);
    theHist->GetXaxis()->CenterTitle();
    theHist->GetYaxis()->CenterTitle();

    MyCanvas->SetRightMargin(0.13);

    // Only draw a palette if the histogram has entries.
    std::string drawOptions;
    if ( theHist->GetEntries() > 0 )
      drawOptions="COLZ";
    theHist->Draw(drawOptions.c_str());

    return theHist;
  } // HistogramViewY

  // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....

  TH1* SimulationDisplay::HistogramViewZ() {

    if (debug) std::cout << "HistogramViewZ"
			 << " xBins=" << MyPlotsInfo[ PlotKind ].xBins
			 << " xLow="  << MyPlotsInfo[ PlotKind ].xLow
			 << " xHigh=" << MyPlotsInfo[ PlotKind ].xHigh
			 << " yBins=" << MyPlotsInfo[ PlotKind ].yBins
			 << " yLow="  << MyPlotsInfo[ PlotKind ].yLow
			 << " yHigh=" << MyPlotsInfo[ PlotKind ].yHigh
			 << " scale=" << MyScaleFactor[ ScaleID ]
			 << std::endl;

    auto theHist = new TH2D("hist", "", 
			    MyPlotsInfo[ PlotKind ].xBins / MyScaleFactor[ ScaleID ],
			    MyPlotsInfo[ PlotKind ].xLow,
			    MyPlotsInfo[ PlotKind ].xHigh,
			    MyPlotsInfo[ PlotKind ].yBins / MyScaleFactor[ ScaleID ],
			    MyPlotsInfo[ PlotKind ].yLow,
			    MyPlotsInfo[ PlotKind ].yHigh );
    for (size_t i = 0; i < e_val.size(); i++) {
      theHist->Fill(x_value[i], y_value[i], e_val[i]);
    }

    theHist->SetStats(kFALSE);
    theHist->GetXaxis()->SetTitle(MyPlotsInfo[ PlotKind ].xValueTitle.c_str());
    theHist->GetYaxis()->SetTitle(MyPlotsInfo[ PlotKind ].yValueTitle.c_str());
    theHist->GetZaxis()->SetTitle(MyPlotsInfo[ PlotKind ].paletteTitle.c_str());
    theHist->GetXaxis()->SetTitleSize(0.025);
    theHist->GetYaxis()->SetTitleSize(0.025);
    theHist->GetZaxis()->SetTitleSize(0.025);
    theHist->GetXaxis()->SetLabelSize(0.02);
    theHist->GetYaxis()->SetLabelSize(0.02);
    theHist->GetZaxis()->SetLabelSize(0.02);
    theHist->GetXaxis()->SetTitleOffset(1.5);
    theHist->GetYaxis()->SetTitleOffset(2.0);
    theHist->GetZaxis()->SetTitleOffset(1.75);
    theHist->GetXaxis()->CenterTitle();
    theHist->GetYaxis()->CenterTitle();

    MyCanvas->SetRightMargin(0.13);

    // Only draw a palette if the histogram has entries.
    std::string drawOptions;
    if ( theHist->GetEntries() > 0 )
      drawOptions="COLZ";
    theHist->Draw(drawOptions.c_str());

    return theHist;
  } // HistogramViewZ
  
  // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....

  // Draw the trajectories within the histogram. 
  void SimulationDisplay::AddTrackLines() {

    // Iterate over tracks.
    for (const auto& [trackID, track] : *MyTrackList) {

      auto trajectory = track.Trajectory();
      if ( ! trajectory.empty() ) {

	// PLaying with object inheritance: Create generic line and object.
	auto line = new TAttLine();
	auto object = new TObject();
	
	if ( HistView == M_VIEW_3D ) {
	  // Create a 3D line.
	  auto polyline = new TPolyLine3D( trajectory.size() );

	  for ( size_t i = 0; i != trajectory.size(); ++i ) {
	    auto point = trajectory[i];
	    polyline->SetPoint(i, point.X(), point.Y(), point.Z() );
	  }
	  line = (TAttLine*) polyline;
	  object = (TObject*) polyline;
	}
	else {
	  // Create a 2D line.
	  auto polyline = new TPolyLine( trajectory.size() );

	  for ( size_t i = 0; i != trajectory.size(); ++i ) {
	    auto point = trajectory[i];

	    if ( HistView == M_VIEW_X ) {
	      polyline->SetPoint(i, point.Y(), point.Z() );
	    }
	    else if ( HistView == M_VIEW_Y ) {
	      polyline->SetPoint(i, point.X(), point.Z() );
	    }
	    else if ( HistView == M_VIEW_Z ) {
	      polyline->SetPoint(i, point.X(), point.Y() );
	    }
	  } // loop over trajectory

	  line = (TAttLine*) polyline;
	  object = (TObject*) polyline;
	}
	
	// Look up the track's PDG code in ROOT's particle database.
	auto particle = TDatabasePDG::Instance()->GetParticle( track.PDGCode() );

	// Adjust the line's color based on the particle's charge. 
	auto charge = particle->Charge();
	if ( charge > 0 ) {
	  line->SetLineColor(kBlue);
	  line->SetLineWidth(4);
	}
	else if ( charge < 0 ) {
	  line->SetLineColor(kRed);
	  line->SetLineWidth(4);
	}
	else {
	  line->SetLineColor(kGreen);
	line->SetLineWidth(2);
	}

	MyPlotLines.push_back( object );

      } // trajectory exists
    } // loop over tracks
  } // AddTrackLines
  
  // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....

  // Handle the plotting of lines (as opposed to histograms).
  void SimulationDisplay::AddPlotLines() {
    
    // Special case: For the view of the hits, draw the tracks only if
    // the user has selected that View option.
    if ( PlotKind == M_PLOT_HITS ) {

      // Enable the menu entry that allows the user to toggle this
      // feature.
      MyMenuView->EnableEntry( M_VIEW_ADD_TRACKS );

      // Only show the plot lines if the user has checked it.
      if ( MyMenuView->IsEntryChecked( M_VIEW_ADD_TRACKS ) ) {

	AddTrackLines();

      } // Menu view was enabled.
    } // View/plot selection

    if ( PlotKind == M_PLOT_TRACKS ) {
      AddTrackLines();
    }

  } // AddPlotLines
  
  // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....
  
  // Accumulate the information for the current event, create the
  // histogram, and display it.
  void SimulationDisplay::UpdateDisplay() {

    if (debug) std::cout << "UpdateDisplay" << std::endl;
    if (debug) std::cout << "Trace 0200" << std::endl;

    // Validate the value of CurrentEntry. 
    auto lastEntry = MyTree->GetEntriesFast() - 1;
    if ( CurrentEntry > lastEntry ) CurrentEntry = lastEntry;
    if ( CurrentEntry < 0 ) CurrentEntry = 0;

    // Get the current entry.
    MyTree->GetEntry( CurrentEntry );

    // Set the number fields on the display.
    MyRunEntry->SetNumber( MyEventID->Run() );
    MyEventEntry->SetNumber( MyEventID->Event() );
    
    if (debug) std::cout << "Trace 0500" << std::endl;

    // Replace the "Save" menu entries with ones that reflect the
    // current plot type and event index.
    auto index = MyEventID->Index();
    
    // For each of the Save sub-menu's signals:
    for ( const auto& [ signal, extension ] : MyFileTypeMap ) {
      // Remove the old menu entry for that signal. 
      MyMenuSave->DeleteEntry( signal );

      // Create a new menu entry for that signal. 
      std::ostringstream menuLine;
      menuLine << MyPlotsInfo[ PlotKind ].savePrefix << index << extension;
      std::string entryName = menuLine.str();

      MyMenuSave->AddEntry(entryName.c_str(), signal);
    }

    // By default, the menu option to display the primary-particle's
    // trace is disabled. It's only enable for the hit-energy 3D plot.
    MyMenuView->DisableEntry( M_VIEW_ADD_TRACKS );
    
    // Locate all the primary particles in this physics event.
    FindPrimaries();

    // Clear the vectors in which we accumulate values. Remember, here
    // (x,y,z) refer to the axes of the histogram, not the coordinates
    // in the detector.
    x_value.clear();
    y_value.clear();
    z_value.clear();
    e_val.clear();

    // Accumulate the values for histogram (x,y,z) from the data for
    // the current event.
    switch (PlotKind) {

    case M_PLOT_TRACKS:
      // We don't "accumulate" track information, just plot it.
      break;
      
    case M_PLOT_HITS:
      AccumulateHits();
      break;

    case M_PLOT_CLUSTERS:
      AccumulateClusters();
      break;

    case M_PLOT_READOUT:
      AccumulateReadout();
      break;

    case M_PLOT_WAVEFORMS:
      AccumulateWaveforms();
      break;

    default:
      std::cerr << "Unknown plot id " << PlotKind << std::endl;
    }
    
    if (debug) std::cout << "Trace 2100" << std::endl;
    
    if (debug) {
      for (size_t i = 0; i < x_value.size(); i++) {
	std::cout << " entry " << i 
		  << " x=" << x_value[i]
		  << " y=" << y_value[i]
		  << " z=" << z_value[i]
		  << " energy=" << e_val[i]
		  << std::endl;
      }
    } // end debug

    // Get ready for a new plot. 
    MyCanvas->Clear();
    MyPlotLines.clear();

    // Redefine the histogram, then fill it with the accumulated values.
    if (MyHist != nullptr) delete MyHist;
    switch (HistView) {

    case M_VIEW_X:
      MyHist = HistogramViewX();
      break;

    case M_VIEW_Y:
      MyHist = HistogramViewY();
      break;

    case M_VIEW_Z:
      MyHist = HistogramViewZ();
      break;
      
    default:
      MyHist = HistogramView3D();
    }

    // Are there any special lines needed for this plot?
    AddPlotLines();
    for ( size_t line = 0; line != MyPlotLines.size(); ++line ) {
      MyPlotLines[line]->Draw();
    }
    
    if (debug) std::cout << "Trace 6000"
			 << " MyCanvas=" << MyCanvas
			 << " MyHist=" << MyHist
			 << std::endl;

    // Create the text boxes for the canvas.  I found the coordinates
    // for TPaveText interactively. I ran the program, used the mouse
    // to move and resize the text box, then right-clicked on the
    // canvas and selected "SaveAs". I examined the output .C macro
    // and pulled the values from there.

    // "brNDC" = bottom and right shadows are drawn; coordinates are in
    // NDC. NDC means that the coordinates are normalized with respect
    // to the canvas; 0,0 is bottom left, coordinates range from 0 to
    // 1.

    if (debug) std::cout << "Trace 8000" << std::endl;

    if (MyDescBox == nullptr) {
      MyDescBox = new TPaveText(0.002,0.909,0.704,0.985,"NDC");
      if (debug) std::cout << "Trace 8250" << std::endl;
    }
    if (debug) std::cout << "Trace 8300"
			 << " MyDescBox=" << MyDescBox
			 << std::endl;

    MyDescBox->Clear();
    
    // Histogram description text box.
    auto MyGraphDesc = MyPlotsInfo[ PlotKind ].plotTitle;;
    auto graphText = MyDescBox->AddText(MyGraphDesc.c_str());
    graphText->SetTextSize(0.035);

    // As of Aug-2024, events in GramsSim have only one primary
    // particle. However, that may change. Therefore, allow for the
    // possibility of multiple primary particles in an event.

    if ( pdg.empty() ) {
      auto primText  = MyDescBox->AddText("Missing primary track");
      primText->SetTextSize(0.03);
    }
    else {
      for ( size_t i = 0; i != pdg.size(); ++i ) {

	// Create the label text for the primary particle's energy.
	double primaryEnergy = momentum4D[i].E();
	std::stringstream ss;
	ss << "Primary: " << primaryEnergy << " MeV ";
	std::string primDesc = ss.str();;
	  
	// Convert the primary particle's PDG code into a name. 
	auto pdgCode = pdg[0];
	auto particle = TDatabasePDG::Instance()->GetParticle(pdgCode);
	std::string particleText;
	if ( particle == nullptr ) {
	  std::stringstream sk;
	  sk << "PDG code " << pdgCode << " unknown";
	  particleText = sk.str();
	}
	else {
	  particleText = particle->GetName();
	}
	primDesc += particleText;
	auto primText = MyDescBox->AddText(primDesc.c_str());
	primText->SetTextSize(0.03);

      } // for each primary particle
    } // one or more primaries
    
    MyDescBox->SetBorderSize(0);
    MyDescBox->SetFillColor(0);
    MyDescBox->SetFillStyle(0);
    MyDescBox->SetTextAlign(kHAlignLeft + kVAlignCenter);
    MyDescBox->SetTextFont(42);
    MyDescBox->Draw();

    // Define the supplementary text box in the canvas.
    std::string geomDesc = "Geometry: " + GeometryFile;

    if (MyTextBox == nullptr)
      MyTextBox = new TPaveText(0.723,0.006,0.996,0.062,"brNDC");
    MyTextBox->Clear();
    auto geomText  = MyTextBox->AddText(geomDesc.c_str());
    // The description of the anode location depends on the kind of the plot. 
    auto aText = MyPlotsInfo[PlotKind].anodeLocation;
    auto anodeText = MyTextBox->AddText(aText.c_str());
    
    auto alignment = kHAlignLeft+kVAlignCenter;
    geomText->SetTextAlign(alignment);
    anodeText->SetTextAlign(alignment);

    MyTextBox->Draw();

    // If there are no entries or lines in the histogram, display an
    // obvious info message.
    if ( MyHist->GetEntries() == 0.  &&  PlotKind != M_PLOT_TRACKS ) {
      if (MyEmptyBox == nullptr) {
	MyEmptyBox = new TPaveText(0.313,0.481,0.686,0.519,"brNDC");
	auto emptyText = MyEmptyBox->AddText("No energy deposited in LAr");
	emptyText->SetTextAlign( kHAlignCenter+kVAlignCenter );
      }
      MyEmptyBox->Draw();
    }
    
    if (debug) std::cout << "Trace 8500" << std::endl;

    MyCanvas->Modified();
    MyCanvas->Update();

    // Get the palette on the right-hand side of the display.  We
    // can't get the palette until after that canvas update.
    auto palette = (TPaletteAxis*)MyHist->GetListOfFunctions()->FindObject("palette");
    
    // If there is a palette, set its width to be a bit less than the
    // default. This gives some extra room for axis labels.
    if ( palette != nullptr ) {
      // palette->SetX1NDC(0.875);
      // palette->SetX2NDC(0.895);
      if (debug) std::cout << "Trace 9500"
			   << " palette=" << palette
			   << " GetX1NDC=" << palette->GetX1NDC()
			   << " GetX2NDC=" << palette->GetX2NDC()
			   << " GetY1NDC=" << palette->GetY1NDC()
			   << " GetY2NDC=" << palette->GetY2NDC()
			   << std::endl;
    }
    if (debug) std::cout << "Trace 7100"
			 << " MyTextBox=" << MyTextBox
			 << " GetX1NDC=" << MyTextBox->GetX1NDC()
			 << " GetX2NDC=" << MyTextBox->GetX2NDC()
			 << " GetY1NDC=" << MyTextBox->GetY1NDC()
			 << " GetY2NDC=" << MyTextBox->GetY2NDC()
			 << std::endl;

    // Now that the palette has been modified, update the canvas again.
    MyCanvas->Modified();
    MyCanvas->Update();

    if (debug) std::cout << "Trace 9000" << std::endl;
  }  

  // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....

  void SimulationDisplay::SetUpHistogram() {

    // Set up the histogram for the display. We want the histogram
    // limits to be based on the size of the anode. Instead of
    // hard-coding the value, which will vary as the geometry changes,
    // let's get this from the copy of the geometry stored in the input
    // file.
    gGeoManager->SetVerboseLevel(0); // avoid pointless Info messages
    auto geom = MyFile->Get<TGeoManager>("GRAMSgeometry");
    if ( geom == nullptr ) {
      std::cerr << "Geometry not found" << std::endl;
      exit(EXIT_FAILURE);
    }

    // Search the geometry for the anode volume.
    TString volName = "volTilePlane";
    TRegexp searchFor(volName);

    // construct an interator around the top level volume
    auto startVol = geom->GetTopVolume();
    TGeoIterator next(startVol);

    // the current Node in the tree that you will be traversing
    TGeoNode* current;

    // Place to store the name of the current node
    TString nodePath;

    // output x/y extend of the Anode Plane
    AnodePlaneXExtent  = -1;
    AnodePlaneYExtent  = -1;

    // Run through the geometry tree structure via the iterator
    while ((current = next())) {
      // grab the node name
      next.GetPath(nodePath);
      // if the volume contains the regex expression...
      if (nodePath.Contains(searchFor)) {
	// Grab the bounding box of the Anode Plane (which for GRAMS,
	// just coincides with the Tile since it is just a
	// parallelpiped)
	auto volume = current->GetVolume();
	auto box = dynamic_cast<TGeoBBox*>(volume->GetShape());
	// GetDX() gets half width along the X axis, which is what we want for the histogram.
	AnodePlaneXExtent = box->GetDX();
	AnodePlaneYExtent = box->GetDY();
	break;
      } // volume found
    } // loop over volumes in the geometry

    if ( ! ( AnodePlaneXExtent > 0  &&  AnodePlaneYExtent > 0 ) ) {
      std::cerr << "Invalid geometry results: "
		<< "AnodePlaneXExtent=" << AnodePlaneXExtent
		<< "AnodePlaneYExtent=" << AnodePlaneYExtent
		<< std::endl;
      exit(EXIT_FAILURE);
    }

    // That takes care of the X/Y size of the readout, but what about
    // the z-limits?
    TPCZSize  = -1;

    // Search the geometry (again) for the TPC volume.
    next = TGeoIterator(startVol);
    volName = "volTPC";
    searchFor = TRegexp(volName);

    // Run through the geometry tree structure via the iterator
    while ((current = next())) {
      // grab the node name
      next.GetPath(nodePath);
      // if the volume contains the regex expression...
      if (nodePath.Contains(searchFor)) {
	// Grab the bounding box. 
	auto volume = current->GetVolume();
	auto box = dynamic_cast<TGeoBBox*>(volume->GetShape());
	// GetDZ() gets half width along the Z axis, so double it for the full height.
	TPCZSize = box->GetDZ() * 2.0;
	break;
      } // volume found
    } // loop over volumes in the geometry

    if ( ! ( TPCZSize > 0 ) ) {
      std::cerr << "Invalid geometry results: "
		<< "TPCZSize=" << TPCZSize
		<< std::endl;
      exit(EXIT_FAILURE);
    }

    // Save the name of the geometry file to be displayed with the
    // histogram.
    options->GetOption("gdmlfile",GeometryFile);

    // Now we want to translate that Z distance into time. For that, we
    // need the drift velocity in LAr. That value is stored in the
    // option ntuple within the input file.

    options->GetOption("ElectronDriftVelocity",DriftVelocity);
    MaxDriftTime = TPCZSize / DriftVelocity;
    // The time units of GramsSim are ns (as defined in options.xml),
    // but microseconds are more convenient for the plot.
    MaxDriftTime = MaxDriftTime / 1000;

    if (debug ) {
      options->PrintOptions();
      std::cout << "TPCZSize = " << TPCZSize
		<< " DriftVelocity = " << DriftVelocity
		<< " MaxDriftTime = " << MaxDriftTime
		<< std::endl;
    }

    // The x- and y- number of readout channels.
    options->GetOption("x_resolution",XChannels);
    options->GetOption("y_resolution",YChannels);
    if ( XChannels <= 0  || YChannels <= 0 ) {
      std::cerr << "Invalid number of readout channels: "
		<< " XChannels=" << XChannels
		<< " YChannels=" << YChannels
		<< std::endl;
      exit(EXIT_FAILURE);
    }

    // For time bins, use a fixed value for now. Later this should be
    // replaced by the time resolution of the anode... perhaps.
    TimeBins = 100;
    
    // Definitions: Set up the histogram text and axes for each kind
    // of plot. Remember, the (x,y,z) in the MyPlotsInfo structures
    // refer to the histograms' axes, not the location or dimensions of
    // the detector.

    // For the X- and Y- views, the lower limit of the time axis is
    // offset by 1 unit. If it's set to 0, there's some bizarre artifact that
    // causes a line to be drawn across the display. I tried to track
    // down the cause of the problem, but the only thing that I found
    // that worked is to end the t-axis at 1 instead of 0 (or
    // values near 0).

    MyPlotsInfo[ M_PLOT_TRACKS ].plotTitle     = "MC Tracks";
    MyPlotsInfo[ M_PLOT_TRACKS ].anodeLocation = "anode at z=0";
    MyPlotsInfo[ M_PLOT_TRACKS ].xValueTitle   = "x [cm]";
    MyPlotsInfo[ M_PLOT_TRACKS ].yValueTitle   = "y [cm]";
    MyPlotsInfo[ M_PLOT_TRACKS ].zValueTitle   = "z [cm]";
    MyPlotsInfo[ M_PLOT_TRACKS ].zValueTitle3D = "z [cm]";
    MyPlotsInfo[ M_PLOT_TRACKS ].paletteTitle  = "E [MeV]";
    MyPlotsInfo[ M_PLOT_TRACKS ].xLow          = -AnodePlaneXExtent;
    MyPlotsInfo[ M_PLOT_TRACKS ].xHigh         =  AnodePlaneXExtent;
    MyPlotsInfo[ M_PLOT_TRACKS ].yLow          = -AnodePlaneYExtent;
    MyPlotsInfo[ M_PLOT_TRACKS ].yHigh         =  AnodePlaneYExtent;
    MyPlotsInfo[ M_PLOT_TRACKS ].zLow          = -TPCZSize;
    MyPlotsInfo[ M_PLOT_TRACKS ].zHigh         = 1; // If this is 0, there's a plot error.
    MyPlotsInfo[ M_PLOT_TRACKS ].xBins         = XChannels;
    MyPlotsInfo[ M_PLOT_TRACKS ].yBins         = YChannels;
    MyPlotsInfo[ M_PLOT_TRACKS ].zBins         = 100;

    MyPlotsInfo[ M_PLOT_HITS ].plotTitle     = "MC Hit energy within LAr";
    MyPlotsInfo[ M_PLOT_HITS ].anodeLocation = "anode at z=0";
    MyPlotsInfo[ M_PLOT_HITS ].xValueTitle   = "x [cm]";
    MyPlotsInfo[ M_PLOT_HITS ].yValueTitle   = "y [cm]";
    MyPlotsInfo[ M_PLOT_HITS ].zValueTitle   = "z [cm]";
    MyPlotsInfo[ M_PLOT_HITS ].zValueTitle3D = "z [cm]     color=E [MeV]";
    MyPlotsInfo[ M_PLOT_HITS ].paletteTitle  = "E [MeV]";
    MyPlotsInfo[ M_PLOT_HITS ].xLow          = -AnodePlaneXExtent;
    MyPlotsInfo[ M_PLOT_HITS ].xHigh         =  AnodePlaneXExtent;
    MyPlotsInfo[ M_PLOT_HITS ].yLow          = -AnodePlaneYExtent;
    MyPlotsInfo[ M_PLOT_HITS ].yHigh         =  AnodePlaneYExtent;
    MyPlotsInfo[ M_PLOT_HITS ].zLow          = -TPCZSize;
    MyPlotsInfo[ M_PLOT_HITS ].zHigh         = 1; // If this is 0, there's a plot error.
    MyPlotsInfo[ M_PLOT_HITS ].xBins         = XChannels;
    MyPlotsInfo[ M_PLOT_HITS ].yBins         = YChannels;
    MyPlotsInfo[ M_PLOT_HITS ].zBins         = 100;

    // For the plots in which the z-axis (histogram) is the value of t
    // (= simulation time), the plots look more consistent if we plot
    // "-t" instead of "t". This leaves the anode at the top of the
    // plot in all the differents kinds of histograms.
    
    MyPlotsInfo[ M_PLOT_CLUSTERS ].plotTitle     = "Electron cluster energy at anode";
    MyPlotsInfo[ M_PLOT_CLUSTERS ].anodeLocation = "anode at t=0";
    MyPlotsInfo[ M_PLOT_CLUSTERS ].xValueTitle   = "x(anode) [cm]";
    MyPlotsInfo[ M_PLOT_CLUSTERS ].yValueTitle   = "y(anode) [cm]";
    MyPlotsInfo[ M_PLOT_CLUSTERS ].zValueTitle   = "Negative t(anode) [#mus]";
    MyPlotsInfo[ M_PLOT_CLUSTERS ].zValueTitle3D = "Negative t(anode) [#mus]    color=E(anode) [MeV]";
    MyPlotsInfo[ M_PLOT_CLUSTERS ].paletteTitle  = "E(anode) [MeV]";
    MyPlotsInfo[ M_PLOT_CLUSTERS ].xLow          = -AnodePlaneXExtent;
    MyPlotsInfo[ M_PLOT_CLUSTERS ].xHigh         =  AnodePlaneXExtent;
    MyPlotsInfo[ M_PLOT_CLUSTERS ].yLow          = -AnodePlaneYExtent;
    MyPlotsInfo[ M_PLOT_CLUSTERS ].yHigh         =  AnodePlaneYExtent;
    MyPlotsInfo[ M_PLOT_CLUSTERS ].zLow          = -MaxDriftTime;
    MyPlotsInfo[ M_PLOT_CLUSTERS ].zHigh         = 1; // If this is set to 0 there is a plot error. 
    MyPlotsInfo[ M_PLOT_CLUSTERS ].xBins         = XChannels;
    MyPlotsInfo[ M_PLOT_CLUSTERS ].yBins         = YChannels;
    MyPlotsInfo[ M_PLOT_CLUSTERS ].zBins         = TimeBins;

    // For the readout channels, I don't yet know exactly what scheme
    // will be used to identify the channels. For the moment, use the
    // scheme that's presently defined in GramsReadoutSim: (0,0) is
    // center-most readout channels, with negative numbers allowed.
    
    MyPlotsInfo[ M_PLOT_READOUT ].plotTitle     = "Energy arriving in each readout channel";
    MyPlotsInfo[ M_PLOT_READOUT ].anodeLocation = "anode at t=0";
    MyPlotsInfo[ M_PLOT_READOUT ].xValueTitle   = "x channel ID";
    MyPlotsInfo[ M_PLOT_READOUT ].yValueTitle   = "y channel ID";
    MyPlotsInfo[ M_PLOT_READOUT ].zValueTitle   = "Negative t(anode) [#mus]";
    MyPlotsInfo[ M_PLOT_READOUT ].zValueTitle3D = "Negative t(anode) [#mus]    color=E(anode) [MeV]";
    MyPlotsInfo[ M_PLOT_READOUT ].paletteTitle  = "E(anode) [MeV]";
    MyPlotsInfo[ M_PLOT_READOUT ].xLow          = -XChannels/2;
    MyPlotsInfo[ M_PLOT_READOUT ].xHigh         =  XChannels/2;
    MyPlotsInfo[ M_PLOT_READOUT ].yLow          = -YChannels/2;
    MyPlotsInfo[ M_PLOT_READOUT ].yHigh         =  YChannels/2;
    MyPlotsInfo[ M_PLOT_READOUT ].zLow          = -MaxDriftTime;
    MyPlotsInfo[ M_PLOT_READOUT ].zHigh         = 1; // If this is set to 0 there is a plot error. 
    MyPlotsInfo[ M_PLOT_READOUT ].xBins         = XChannels;
    MyPlotsInfo[ M_PLOT_READOUT ].yBins         = YChannels;
    MyPlotsInfo[ M_PLOT_READOUT ].zBins         = TimeBins;
    
    MyPlotsInfo[ M_PLOT_WAVEFORMS ].plotTitle     = "ADC counts in each readout channel";
    MyPlotsInfo[ M_PLOT_WAVEFORMS ].anodeLocation = "anode bin=0";
    MyPlotsInfo[ M_PLOT_WAVEFORMS ].xValueTitle   = "x channel ID";
    MyPlotsInfo[ M_PLOT_WAVEFORMS ].yValueTitle   = "y channel ID";
    MyPlotsInfo[ M_PLOT_WAVEFORMS ].zValueTitle   = "Negative time bin";
    MyPlotsInfo[ M_PLOT_WAVEFORMS ].zValueTitle3D = "Negative time bin    color=ADC counts";
    MyPlotsInfo[ M_PLOT_WAVEFORMS ].paletteTitle  = "ADC counts";
    MyPlotsInfo[ M_PLOT_WAVEFORMS ].xLow          = -XChannels/2;
    MyPlotsInfo[ M_PLOT_WAVEFORMS ].xHigh         =  XChannels/2;
    MyPlotsInfo[ M_PLOT_WAVEFORMS ].yLow          = -YChannels/2;
    MyPlotsInfo[ M_PLOT_WAVEFORMS ].yHigh         =  YChannels/2;
    MyPlotsInfo[ M_PLOT_WAVEFORMS ].zLow          = 0; // This is set in AccumulateWaveforms
    MyPlotsInfo[ M_PLOT_WAVEFORMS ].zHigh         = 1; // If this is set to 0 there is a plot error. 
    MyPlotsInfo[ M_PLOT_WAVEFORMS ].xBins         = XChannels;
    MyPlotsInfo[ M_PLOT_WAVEFORMS ].yBins         = YChannels;
    MyPlotsInfo[ M_PLOT_WAVEFORMS ].zBins         = 0; // This is set in AccumulateWaveforms

    // Define the different scale factors that the user can select
    // from the View menu. These are the adjustments to the number of
    // bins on each axis (x,y,z).
    MyScaleFactor[ M_VIEW_FINE   ] = 1;
    MyScaleFactor[ M_VIEW_MEDIUM ] = 2;
    MyScaleFactor[ M_VIEW_COARSE ] = 4;
    
    // Let the initial view be the 3D view.
    HistView = M_VIEW_3D;
    MyMenuView->CheckEntry(M_VIEW_3D);

    // Let the initial resolution be the fine view.
    ScaleID = M_VIEW_FINE;
    MyMenuView->CheckEntry(M_VIEW_FINE);

    // At the start, uncheck and disable the View option that displays
    // the tracks on the hits views.
    MyMenuView->UnCheckEntry( M_VIEW_ADD_TRACKS );
    MyMenuView->DisableEntry( M_VIEW_ADD_TRACKS );
    
    // What should the initial plot kind be? For now, assume electron
    // clusters; in Aug-2024 it's the kind of plot that we're most
    // interested in.
    PlotKind = M_PLOT_CLUSTERS;
    
    // Use this color scheme for the histogram palettes. 
    gStyle->SetPalette(kLightTemperature);
    
  } // SetUpHistogram

    // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....

    // Implement the code that will be called by the navigation widgets. .
  void SimulationDisplay::PrevEvent() {
    --CurrentEntry;
    UpdateDisplay();
  }
  void SimulationDisplay::NextEvent() {
    ++CurrentEntry;
    UpdateDisplay();
  }

  // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....

  // These routines are called whenever the user either clicks the
  // arrows in the run or event fields, or when they type a new number
  // into those fields and hit "return".
  void SimulationDisplay::SetEventEntry() {
    // Get the run/event numbers from the fields.
    auto run = MyRunEntry->GetNumberEntry()->GetIntNumber();
    auto event = MyEventEntry->GetNumberEntry()->GetIntNumber();

    // Create a "dummy" EventID, and convert it an index number.
    grams::EventID theEventID(run,event);
    auto index = theEventID.Index();

    // Tell the tree to search its index and return the closest entry
    // to what was entered.
    CurrentEntry = MyTree->GetEntryNumberWithIndex( index );

    // Update the display with this best entry. 
    UpdateDisplay();
  }

  // It's not likely the user will change the run number from the
  // default (0), but give them the option.
  void SimulationDisplay::SetRunEntry() {
    auto run = MyRunEntry->GetNumberEntry()->GetIntNumber();
    auto event = MyEventEntry->GetNumberEntry()->GetIntNumber();

    grams::EventID theEventID(run,event);
    auto index = theEventID.Index();

    CurrentEntry = MyTree->GetEntryNumberWithIndex( index );
    UpdateDisplay();
  }

  // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....
  
  // This routine is called whenever the user selects an item in the
  // File menu. The 'id' tells us which item within the menu was
  // selected by the user.
  void SimulationDisplay::HandleFileMenu(Int_t id)
  {
    if (debug) {
      std::cout << "HandleFileMenu id = " << id << std::endl;
    }
    
    // Determine which line in the menu was selected.
    switch (id) {

    case M_SAVEAS_DIALOG:
      {
	// The pointers vs. objects vs. memory management of these
	// lines comes from guitest.C in the ROOT examples. If you
	// don't handle the pointers the exact way shown, the program
	// will crash. Don't ask me why or how! Let's hope this
	// doesn't create memory leaks.
	
	// To use ROOT's built-in dialog box, we have to define a
	// TGFileInfo object.
	TGFileInfo fileInfo;
	
	// Set the initial directory to be displayed in the dialog box
	// to be the current directory.
	fileInfo.SetIniDir(".");

	// Offer the users some filters to restrict the list of files
	// displayed in the dialog.
	  
	// This character array is used in HandleFileMenu. It defines the
	// filters that a user can select to restrict the files that are
	// shown in the file dialog box.
	const char* fileTypes[] =
	  { "All files",     "*",
	    "ROOT files",    "*.root",
	    "ROOT macros",   "*.C",
	    "PDF files",     "*.[pP][dD][fF]",
	    nullptr,         nullptr };
	fileInfo.fFileTypes = fileTypes;
	
	// Display the dialog box.
	new TGFileDialog(gClient->GetRoot(), this, kFDSave, &fileInfo);

	// The results of the dialog: the full path to the file.
	auto filename = fileInfo.fFilename;

	// If the result is null, then the user clicked the "Cancel"
	// button in the dialog.
	if ( filename != nullptr) {
	  // It's up to the user to make sure the filename has an
	  // extension that reflects the format they want; e.g., ".C"
	  // if they want a ROOT macro. SaveAs will automatically
	  // select the correct format based on the extension. There's
	  // a full list here (search for SaveAs):
	  // https://root.cern/doc/master/classTPad.html

	  if (debug) std::cout << "filename = '" << filename << "'" << std::endl;
	  MyCanvas->SaveAs( filename, "");
	}
      } // M_SAVEAS_DIALOG
      break;
      
    case M_FILE_EXIT:
      DoExit();
      break;
      
    default:
      // Look up the signal and see if we recognize it as one of the
      // entries in the File->Save menu.
      auto const result = MyFileTypeMap.find( id );

      if ( result != MyFileTypeMap.cend() ) {
	// We found it. Get the file name from the menu entry.
	auto menuEntry = MyMenuSave->GetEntry( id );
	auto filename = menuEntry->GetLabel();
	// Save the canvas.
	MyCanvas->SaveAs( filename->Data(), "" );
      }
      else {
	std::cerr << "Error - unknown menu item "
		  << id << " selected"
		  << std::endl;
      }
      break;
    } // switch on id    
  } // HandleFileMenu

    // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....
  
    // This routine is called whenever the user selects an item in the
    // Plot menu. The 'id' tells us which item within the menu was
    // selected by the user.
  void SimulationDisplay::HandlePlotMenu(Int_t id)
  {
    if (debug) {
      std::cout << "HandlePlotMenu id = " << id << std::endl;
    }
    
    // Determine which line in the menu was selected.  Look up the
    // signal and see if we recognize it as one of the entries in the
    // File->Save menu.
    auto const result = MyPlotsInfo.find( id );

    if ( result != MyPlotsInfo.cend() ) {
      // We found it.
      PlotKind = id;
      MyMenuPlot->UnCheckEntries();
      MyMenuPlot->CheckEntry( id );
      UpdateDisplay();
      if (debug) {
	std::cout << "HandlePlotMenu found id = " << id << std::endl;
      }
    }
    else {
      std::cerr << "Error - unknown menu item "
		<< id << " selected"
		<< std::endl;
    }
  } // HandlePlotMenu

    // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....
  
    // This routine is called whenever the user selects an item in the
    // View menu. The 'id' tells us which item within the menu was
    // selected by the user.
  void SimulationDisplay::HandleViewMenu(Int_t id)
  {

    if (debug) {
      std::cout << "HandleViewMenu id = " << id << std::endl;
    }
    
    // Determine which line in the menu was selected. Adjust the
    // menu's check marks appropriately, then set the appropriate
    // parameter and update the display.
    switch (id) {

    case M_VIEW_3D:
      HistView = id;
      MyMenuView->CheckEntry(M_VIEW_3D);
      MyMenuView->UnCheckEntry(M_VIEW_X);
      MyMenuView->UnCheckEntry(M_VIEW_Y);
      MyMenuView->UnCheckEntry(M_VIEW_Z);
      UpdateDisplay();
      break;

    case M_VIEW_X:
      HistView = id;
      MyMenuView->UnCheckEntry(M_VIEW_3D);
      MyMenuView->CheckEntry(M_VIEW_X);
      MyMenuView->UnCheckEntry(M_VIEW_Y);
      MyMenuView->UnCheckEntry(M_VIEW_Z);
      UpdateDisplay();
      break;

    case M_VIEW_Y:
      HistView = id;
      MyMenuView->UnCheckEntry(M_VIEW_3D);
      MyMenuView->UnCheckEntry(M_VIEW_X);
      MyMenuView->CheckEntry(M_VIEW_Y);
      MyMenuView->UnCheckEntry(M_VIEW_Z);
      UpdateDisplay();
      break;

    case M_VIEW_Z:
      HistView = id;
      MyMenuView->UnCheckEntry(M_VIEW_3D);
      MyMenuView->UnCheckEntry(M_VIEW_X);
      MyMenuView->UnCheckEntry(M_VIEW_Y);
      MyMenuView->CheckEntry(M_VIEW_Z);
      UpdateDisplay();
      break;

      // For the (fine, medium, coarse) selection, adjust the menu to
      // indicate the scale selected.
    case M_VIEW_FINE:
    case M_VIEW_MEDIUM:
    case M_VIEW_COARSE:
      ScaleID = id;
      for ( const auto& [ signal, scale ]: MyScaleFactor )	
	MyMenuView->UnCheckEntry(signal);
      MyMenuView->CheckEntry(id);
      UpdateDisplay();
      break;

    case M_VIEW_ADD_TRACKS:
      // Toggle the selection: If on, turn it off and vice-versa.
      if ( MyMenuView->IsEntryChecked( M_VIEW_ADD_TRACKS ) )
	MyMenuView->UnCheckEntry( M_VIEW_ADD_TRACKS );
      else
	MyMenuView->CheckEntry( M_VIEW_ADD_TRACKS );
      UpdateDisplay();
      break;
      
    default:
      std::cerr << "Error - unknown menu item "
		<< id << " selected"
		<< std::endl;
      break;
    } // switch on id
  } // HandleViewMenu

    // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....

    // This routine looks for keyboard/mouse events (not "physics
    // events") that occur while the program runs. For information about Event_t:
    // https://root.cern.ch/doc/master/structEvent__t.html
  void SimulationDisplay::HandleKeypress(Event_t* event) {

    // What kind of event was this? See this page and search for EGEventType:
    // https://root.cern.ch/doc/master/GuiTypes_8h.html
    auto eventType = event->fType;
    
    // We're only interested in keypresses. 
    if ( eventType == kGKeyPress ) {
      // Get the code of the keypress that initiated this event.
      auto code = event->fCode;

      if (debug) std::cout << "Keypress code = " << code << std::endl;
      
      // I found these key codes experimentally. 
      const decltype(code) up    = 63232;
      const decltype(code) down  = 63233;
      const decltype(code) left  = 63234;
      const decltype(code) right = 63235;

      if (debug) std::cout << "HandleKeypress code = " << code << std::endl;
      
      // The left and right arrow keys navigate between the
      // events. The up and down arrow keys switch between types of
      // plots.
      
      switch ( code ) {
      case left:
	if (debug) std::cout << "HandleKeypress left" << std::endl;

	PrevEvent();
	break;

      case right:
	if (debug) std::cout << "HandleKeypress right" << std::endl;

	NextEvent();
	break;
	
      case down:
	{
	  if (debug) std::cout << "HandleKeypress down"
			       << " old PlotKind = " << PlotKind
			       << std::endl;

	  auto search = MyPlotsInfo.find( PlotKind );
	  if ( search == MyPlotsInfo.end() ) {
	    // We're in big trouble; the current PlotKind points to a
	    // non-existent item in the list.
	    std::cerr << "HandleKeyPress ERROR: PlotKind "
		      << PlotKind << " does not exist in MyPlotsInfo"
		      << std::endl;
	  }
	  else {
	    search = std::next( search );
	    if ( search == MyPlotsInfo.end() ) {
	      // We've gone past the end of the list. Start at the beginning.
	      search = MyPlotsInfo.begin();
	    }
	    // Recall that the MyPlotsInfo map consists of pairs
	    // (plotKind, plotsSpecifications), referred by
	    // (first,second).
	    PlotKind = (*search).first;
	  }
	  if (debug) std::cout << "HandleKeypress down"
			       << " new PlotKind = " << PlotKind
			       << std::endl;

	  MyMenuPlot->UnCheckEntries();
	  MyMenuPlot->CheckEntry( PlotKind );
	  UpdateDisplay();
	} // down
	break;

      case up:
	{
	  if (debug) std::cout << "HandleKeypress up"
			       << " old PlotKind = " << PlotKind
			       << std::endl;

	  auto search = MyPlotsInfo.find( PlotKind );
	  if ( search == MyPlotsInfo.end() ) {
	    // We're in big trouble; the current PlotKind points to a
	    // non-existent item in the list.
	    std::cerr << "HandleKeyPress ERROR: PlotKind "
		      << PlotKind << " does not exist in MyPlotsInfo"
		      << std::endl;
	  }
	  else {
	    if ( search == MyPlotsInfo.begin() ) {
	      // We're at the top of the list. Go to the end.
	      search = std::prev( MyPlotsInfo.end() );
	    }
	    else 
	      search = std::prev( search );

	    // Recall that the MyPlotsInfo map consists of pairs
	    // (plotKind, plotsSpecifications), referred by
	    // (first,second).
	    PlotKind = (*search).first;
	  }
	  if (debug) std::cout << "HandleKeypress up"
			       << " new PlotKind = " << PlotKind
			       << std::endl;

	  MyMenuPlot->UnCheckEntries();
	  MyMenuPlot->CheckEntry( PlotKind );
	  UpdateDisplay();
	} // up
	break;

      default:
	// Do nothing. The user has typed a keypress that has nothing
	// to do with us.
	{
	  if (debug) std::cout << "HandleKeypress misc" << std::endl;
	}

      } // switch code
    } // event type = keypress
  } // HandleKeypress
  
} // namespace grams
