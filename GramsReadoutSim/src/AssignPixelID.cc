#include "AssignPixelID.h"

#include "Options.h"

#include <iostream>
#include <cmath>

namespace gramsreadoutsim {

  AssignPixelID::AssignPixelID()
  {
    // Get the pixel geometry options from the XML file/command
    // line.
    auto options = util::Options::GetInstance();
    
    options->GetOption("verbose",     m_verbose);
    options->GetOption("debug",       m_debug);
    options->GetOption("pixel_sizex", m_pixel_sizex);
    options->GetOption("pixel_sizey", m_pixel_sizey);
    options->GetOption("offset_x",    m_offset_x);
    options->GetOption("offset_y",    m_offset_y);

    if (m_verbose) {
      std::cout << "GramsReadOutSim::AssignPixelID() - initialized" << std::endl;
    }
  }
  
  AssignPixelID::~AssignPixelID() {}

  // Accept the (x,y) positions of the electron clusters at the
  // anode. For each cluster, determine its (x,y) pixel ID
  // numbers.
  
  std::tuple<std::vector<int>, std::vector<int>> 
		   AssignPixelID::Assign(std::vector<double>* x_cl, std::vector<double>* y_cl) 
  {
    // Initialize the electron-cluster vectors.
    int n_cl = x_cl[0].size();
    m_pixel_idx.clear();
    m_pixel_idy.clear();
    m_pixel_idx.resize(n_cl);
    m_pixel_idy.resize(n_cl);
    
    // For each electron cluster...
    for (int i=0; i<n_cl; i++) {
      // Map the cluster's (x,y) position to a particular pixel.
      m_pixel_idx[i] = std::floor((x_cl[0][i] - m_offset_x) / m_pixel_sizex);
      m_pixel_idy[i] = std::floor((y_cl[0][i] - m_offset_y) / m_pixel_sizey);
    }
    
    return std::make_tuple(m_pixel_idx, m_pixel_idy);
  }
  
} // namespace gramsreadoutsim
