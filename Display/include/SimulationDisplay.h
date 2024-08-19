// SimulationDisplay.h
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

#ifndef _SIMULATIONDISPLAY_H_
#define _SIMULATIONDISPLAY_H_

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
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH3D.h>
#include <TGeoManager.h>
#include <TDatabasePDG.h>
#include <TParticlePDG.h>
#include <TPaveText.h>
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

namespace grams {

  // This enum defines the IDs (signals) that can be sent by the
  // various widgets and elements. Not all widgets require signal IDs
  // to be defined; all the menu elements do.
  enum ESignal {
    M_VIEW_3D,
    M_VIEW_X,
    M_VIEW_Y,
    M_VIEW_Z,
    M_VIEW_FINE,
    M_VIEW_MEDIUM,
    M_VIEW_COARSE,
    M_VIEW_ADD_TRACKS,
    M_RUN_ENTRY,
    M_EVENT_ENTRY,
    M_SAVEAS_C,
    M_SAVEAS_ROOT,
    M_SAVEAS_PDF,
    M_SAVEAS_GIF,
    M_SAVEAS_PNG,
    M_SAVEAS_JPG,
    M_SAVEAS_TIFF,
    M_SAVEAS_EPS,
    M_SAVEAS_PS,
    M_SAVEAS_SVG,
    M_SAVEAS_TEX,
    M_SAVEAS_XML,
    M_SAVEAS_JSON,
    M_SAVEAS_DIALOG,
    M_PLOT_TRACKS,
    M_PLOT_HITS,
    M_PLOT_CLUSTERS,
    M_PLOT_READOUT,
    M_PLOT_WAVEFORMS,
    M_FILE_EXIT,
  };

  // ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....
  
  // This is the main frame. This class defines all the widgets and
  // elements that are displayed within the window that the user
  // sees.
  class SimulationDisplay : public TGMainFrame {

  private:
    // The interface elements used to display the menus at the top of
    // the frame.
    TGDockableFrame* fMenuDock;
    TGMenuBar*       fMenuBar;
    TGPopupMenu*     MyMenuFile;
    TGPopupMenu*     MyMenuPlot;
    TGPopupMenu*     MyMenuSave;
    TGPopupMenu*     MyMenuView;
    TGLayoutHints*   fMenuBarLayout;
    TGLayoutHints*   fMenuBarItemLayout;

    // For the buttons at the bottom of the frame. 
    TGHorizontalFrame* MyButtonFrame;

    // This will be used when we define the File->Save menu. It
    // matches the "SAVEAS" signals defined above with the filename
    // extensions.
    std::map< int, std::string > MyFileTypeMap;

    // This structure groups together text strings and axis limits
    // associated with different kinds of plots. Remember, here
    // (x,y,z) refers to histogram axes, not detector coordinates.
    struct MyPlotsSpecifications {
      std::string menuDesc;
      std::string savePrefix;
      std::string plotTitle;
      std::string anodeLocation;
      std::string xValueTitle;
      std::string yValueTitle;
      std::string zValueTitle;
      std::string zValueTitle3D;
      std::string paletteTitle;
      // The limits of the histogram axes.
      double xLow;
      double xHigh;
      double yLow;
      double yHigh;
      double zLow;
      double zHigh;
      // The number of bins for the histogram axes.
      int xBins; 
      int yBins;
      int zBins;
    };
    
    // Associate a given signal with the MyPlotsSpecifications struct;
    // e.g. an electron-cluster histogram has it own set of text
    // strings and histogram limits.
    std::map< int, MyPlotsSpecifications > MyPlotsInfo;

    // We have "view resolutions": fine, medium, and coarse. This map
    // holds the scale factors we'll use for each resolution. (The
    // assumption here is that the same scale factor applies to all
    // the different kinds of plots.)
    std::map< int, int > MyScaleFactor;
    
    // The vectors from which we'll populate the histogram. IMPORTANT:
    // The "x,y,z" that you see here are the AXES OF THE HISTOGRAMS,
    // _not_ the (x,y,z) coordinates within the detector.
    std::vector<double> x_value;
    std::vector<double> y_value;
    std::vector<double> z_value;
    std::vector<double> e_val;

    // The vectors that will be used to hold information about the
    // primary particle(s) in an event.
    std::vector<double> pdg;
    std::vector<ROOT::Math::PxPyPzEVector> momentum4D;
    std::vector<ROOT::Math::XYZTVector>    position4D;

    // More pointers to the global objects we'll use.
    TRootEmbeddedCanvas* MyCanvasWidget;
    TCanvas* MyCanvas;
    TFile* MyFile;
    TTree* MyTree;
    TH1* MyHist;
    TPaveText* MyTextBox;
    TPaveText* MyDescBox;
    TPaveText* MyEmptyBox;
    TGNumberEntry* MyRunEntry;
    TGNumberEntry* MyEventEntry;
    std::vector< TObject* > MyPlotLines;
    util::Options* options;

    grams::EventID* MyEventID;
    grams::MCTrackList*      MyTrackList;
    grams::MCLArHits*        MyLArHits;
    grams::ElectronClusters* MyClusters;
    grams::ReadoutMap*       MyReadoutMap;
    grams::ReadoutWaveforms* MyWaveforms;

    // These are parameters associated with defining the displayed
    // histogram.
    double DriftVelocity;     // Parameters to be read via the Options utility
    int XChannels;            // The size of readout in wires/pixels/whatever
    int YChannels;
    int TimeBins;             // The maximum number of time bins we'll distinguis
    std::string GeometryFile; // The name of the geometry file used
    // Derived from the TGeoManager object in the input file:
    double AnodePlaneXExtent;
    double AnodePlaneYExtent;
    double TPCZSize;          
    double MaxDriftTime;      // Calculated from TPCSize and DriftVelocity      
    int HistView;             // The signal for the current histogram view
    int PlotKind;             // The signal for the kind of plot.
    int WaveformSize;      // The size (length) of a waveform vector in ReadoutWaveforms.
    int ScaleID;              // This the id of the scale factor the user has selected in the View menu. 
    
    // The routines for creating different histograms for different
    // views and kinds. 
    TH1* HistogramView3D();
    TH1* HistogramViewX();
    TH1* HistogramViewY();
    TH1* HistogramViewZ();
    void AddPlotLines();
    void AddTrackLines();
    
  public:
    // Define constructor and destructor.
    SimulationDisplay(const TGWindow *p);
    ~SimulationDisplay() override;

    // Update the canvas with current histogram view.
    void UpdateDisplay();

    // The set-up for quantities that will define the histograms is
    // complex enough that I decided to put it in its own routine.
    void SetUpHistogram();

    // The current entry we're reading from the input tree(s).
    int CurrentEntry;

    // Slots: These are the routines that are called when the widgets
    // and elements receive a signal.
    void DoExit(void);
    void HandleFileMenu(Int_t id);
    void HandlePlotMenu(Int_t id);
    void HandleViewMenu(Int_t id);
    void HandleKeypress(Event_t*); 
    void PrevEvent();
    void NextEvent();
    void SetRunEntry();
    void SetEventEntry();

    // Routines associated with accumulating values from the ROOT
    // tree.
    void FindPrimaries();
    void AccumulateHits();
    void AccumulateClusters();
    void AccumulateReadout();
    void AccumulateWaveforms();
  };

} // namespace grams

#endif // _SIMULATIONDISPLAY_H_
