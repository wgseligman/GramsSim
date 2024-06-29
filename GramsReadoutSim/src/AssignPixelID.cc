#include "AssignPixelID.h"

// From GramsDataObj
#include "ReadoutID.h"
#include "ElectronClusters.h"

#include "Options.h"

#include <iostream>
#include <cmath>
#include <cassert>
#include <TGeoManager.h>
#include <TGeoVolume.h>
#include <TGeoBBox.h>
#include <TString.h>
#include <TRegexp.h>

namespace gramsreadoutsim {

  AssignPixelID::AssignPixelID()
  {
    auto options = util::Options::GetInstance();

    options->GetOption("verbose",     m_verbose);
    options->GetOption("debug",       m_debug);

    // Avoid pointless ROOT geometry information messages. 
    if (m_debug || m_verbose)
      gGeoManager->SetVerboseLevel(1);
    else
      gGeoManager->SetVerboseLevel(0); // avoid pointless Info messages

    // Get the gdml file that was reformatted by GramsG4, and load
    // Geometry, and the volume of the anode plane,
    std::string GramsG4_gdml;
    options->GetOption("gdml",GramsG4_gdml);
    std::string anodeTileVolume;
    options->GetOption("anodeTileVolume",anodeTileVolume);

    auto geom = TGeoManager::Import(GramsG4_gdml.c_str());

    // get the top level volume of the bounding volume hierarchy so that you can find the AnodePlane volume
    TGeoVolume* startVol = geom->GetTopVolume();

    // Construct a (very simple) Regex expression to try and pattern
    // match the bounding box names you encounter as you traverse the
    // hierarchy
    TString volName = anodeTileVolume;
    TRegexp searchFor(volName);

    // construct an interator around the top level volume
    TGeoIterator next(startVol);

    // the current Node in the tree that you will be traversing
    TGeoNode* current;

    // Place to store the name of the current node
    TString nodePath;

    // output x/y extend of the Anode Plane
    double AnodePlaneXLength  = -1;
    double AnodePlaneYLength  = -1;

    // Run through the geometry tree structure via the iterator
    while ((current= next())) {
      // grab the node name
      next.GetPath(nodePath);
      // if the volume contains the regex expression...
      if (nodePath.Contains(searchFor)) {
        // Grab the bounding box of the Anode Plane (which for Grams,
        // just coincides with the Tile since it is just a
        // parallelpiped)
        auto volume = current->GetVolume();
        auto box = dynamic_cast<TGeoBBox*>(volume->GetShape());
        // GetDX() gets half width along the X axis, so multiply by 2 to get full width
        AnodePlaneXLength = box->GetDX() * 2.0;
        AnodePlaneYLength = box->GetDY() * 2.0;
        break;
      } // volume found
    } // loop over volumes in the geometry

    // PANIC if the dimensions aren't physical
    if(AnodePlaneXLength<=0 || AnodePlaneYLength<=0){
      std::cerr << std::endl
		<< "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
		<< "GramsReadoutSim::AssignPixelID: Could not find volume '" << anodeTileVolume << "'" 
		<< " in '" <<  GramsG4_gdml << "'." << " Check anodeTileVolume value"
		<< std::endl;
      exit(EXIT_FAILURE);
    }

    // Get the pixel geometry options from the XML file/command line.
    options->GetOption("readout_centerx",    m_offset_x);
    options->GetOption("readout_centery",    m_offset_y);

    int x_resolution;
    int y_resolution;
    options->GetOption("x_resolution", x_resolution);
    options->GetOption("y_resolution", y_resolution);
    if(x_resolution<=0 || y_resolution<=0){
      std::cerr << std::endl
		<< "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
		<< "GramsReadoutSim::AssignPixelID: either x_resolution or y_resolution is not positive. "
		<< "Check '" << GramsG4_gdml << "'"
		<< std::endl;
        exit(EXIT_FAILURE);
    }
    m_pixel_sizex = AnodePlaneXLength/static_cast<double>(x_resolution);
    m_pixel_sizey = AnodePlaneYLength/static_cast<double>(y_resolution);

    if (m_verbose) {
      std::cout << "GramsReadOutSim::AssignPixelID() - initialized" << std::endl;
    }
  }
  
  AssignPixelID::~AssignPixelID() {}

  // Accept the (x,y) position of the electron cluster at the
  // anode. Determine its (x,y) pixel ID numbers.
  
  const grams::ReadoutID AssignPixelID::Assign(const grams::ElectronCluster& cluster) 
  {
    int pixel_idx = std::floor((cluster.XAtAnode() - m_offset_x) / m_pixel_sizex);
    int pixel_idy = std::floor((cluster.YAtAnode() - m_offset_y) / m_pixel_sizey);
   
    if (m_debug) {
      std::cout << "gramsreadoutsim::AssignPixelID - "
		<< " m_offset_x=" << m_offset_x
		<< " m_offset_y=" << m_offset_y
		<< " m_pixel_sizex=" << m_pixel_sizex
		<< " m_pixel_sizey=" << m_pixel_sizey
		<< std::endl
		<< " cluster.XAtAnode()=" << cluster.XAtAnode()
		<< " cluster.YAtAnode()=" << cluster.YAtAnode()
		<< " pixel_idx=" << pixel_idx
		<< " pixel_idy=" << pixel_idy
		<< std::endl;
    }    
 
    return grams::ReadoutID(pixel_idx, pixel_idy);
  }
  
} // namespace gramsreadoutsim
