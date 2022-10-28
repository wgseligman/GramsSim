// gramsreadoutsim.cc
// 25-Nov-2011 William Seligman

// Our function(s) for the detector response.
#include "AssignPixelID.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/

//#include "

// ROOT includes
#include "TFile.h"
#include "TTree.h"

// C++ includes
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <numeric>

///////////////////////////////////////
int main(int argc,char **argv)
{
    auto options = util::Options::GetInstance();

    auto result = options->ParseOptions(argc, argv, "gramsreadoutsim");

    // Abort if we couldn't parse the job options.
    if (! result) {
      std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
            << "gramsreadoutsim: Aborting job due to failure to parse options"
            << std::endl;
      exit(EXIT_FAILURE);
    }

    bool debug;
    options->GetOption("debug",debug);

    bool help;
    options->GetOption("help",help);
    if (help) {
      options->PrintHelp();
      exit(EXIT_SUCCESS);
    }

    bool verbose;
    options->GetOption("verbose",verbose);
    if (verbose) {
      // Display all program options.
      options->PrintOptions();
    }

    // Get the options associated with the input file and ntuple..
    std::string inputFileName;
    options->GetOption("inputfile",inputFileName);
    
    std::string inputNtupleName;
    options->GetOption("inputntuple",inputNtupleName);


    if (verbose)
      std::cout << "gramsreadoutsim: input file = '" << inputFileName
            << "', input ntuple = '" << inputNtupleName
            << "'" << std::endl;


    gramsreadoutsim::AssignPixelID* assignPixelID = NULL;
    assignPixelID = new gramsreadoutsim::AssignPixelID();

    int run;
    int event;
    int trackID;
    int pDGCode;
    int numPhotons;
    int identifier;

    std::vector<double>* penergyAtAnode = 0;
    std::vector<double>* pelectronAtAnode = 0;
    std::vector<double>* pxPosAtAnode = 0;
    std::vector<double>* pyPosAtAnode = 0;
    std::vector<double>* pzPosAtAnode = 0;
    std::vector<double>* ptimeAtAnode = 0;

    auto input = TFile::Open(inputFileName.c_str());
    if (!input || input->IsZombie()) {
        std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
            << "gramsreadoutsim: Could not open file '" << inputFileName << "'"
            << std::endl;
        exit(EXIT_FAILURE);
    }
        
    double driftVel;
    double pixel_plane_offset;
    gDirectory->cd("header");
    TTree* inheadertree = (TTree*)input->Get("header/metatree");
    inheadertree->SetBranchAddress("DriftVel",              &driftVel);
    inheadertree->SetBranchAddress("pixel_plane_offset",    &pixel_plane_offset);
    inheadertree->GetEntry(0);
    delete inheadertree;
    inheadertree = nullptr;

    gDirectory->cd();
    gDirectory->cd("../data");
    TTree* intree = (TTree*)input->Get("data/DetSim");

    intree->SetBranchAddress("Run",           &run);
    intree->SetBranchAddress("Event",         &event);
    intree->SetBranchAddress("TrackID",       &trackID);
    intree->SetBranchAddress("PDGCode",       &pDGCode);
    intree->SetBranchAddress("numPhotons",    &numPhotons);
    intree->SetBranchAddress("energy",        &penergyAtAnode);
    intree->SetBranchAddress("numElectrons",  &pelectronAtAnode);
    intree->SetBranchAddress("x",             &pxPosAtAnode);
    intree->SetBranchAddress("y",             &pyPosAtAnode);
    intree->SetBranchAddress("z",             &pzPosAtAnode);
    intree->SetBranchAddress("timeAtAnode",   &ptimeAtAnode);
    intree->SetBranchAddress("identifier",    &identifier);

    // Now read in the options associated with the output file and ntuple. 
    std::string outputFileName;
    options->GetOption("outputfile", outputFileName);

    std::string outputNtupleName;
    options->GetOption("outputntuple", outputNtupleName);


    if (verbose)
      std::cout << "gramsreadoutsim: output file = '" << outputFileName
            << "', output ntuple = '" << outputNtupleName
            << "'" << std::endl;

    // Open the output file.
    auto output = TFile::Open(outputFileName.c_str(),"RECREATE");

    output->mkdir("header");
    gDirectory->cd();
    gDirectory->cd("header");

    TTree* metatree = new TTree("metatree","metatree");

    double readout_centerx;
    double readout_centery;
    double pixel_sizex;
    double pixel_sizey;
    options->GetOption("readout_centerx",   readout_centerx);
    options->GetOption("readout_centery",   readout_centery);
    options->GetOption("pixel_sizex",       pixel_sizex);
    options->GetOption("pixel_sizey",       pixel_sizey);

    metatree->Branch("DriftVel",            &driftVel,              "DriftVel/D" );
    metatree->Branch("pixel_plane_offset",  &pixel_plane_offset,    "pixel_plane_offset/D" );
    metatree->Branch("readout_centerx",     &readout_centerx,       "readout_centerx/D" );
    metatree->Branch("readout_centery",     &readout_centery,       "readout_centery/D" );
    metatree->Branch("pixel_sizex",         &pixel_sizex,           "pixel_sizex/D" );
    metatree->Branch("pixel_sizey",         &pixel_sizey,           "pixel_sizey/D" );
    metatree->Fill();
    metatree->Write();

    // Write the options to the output file, so we have a record.
    options->WriteNtuple(output);

    gDirectory->cd();
    output->mkdir("data");
    gDirectory->cd();
    gDirectory->cd("../data");

    // Define our output ntuple.
    TTree* outtree = new TTree("ReadoutSim", "ReadoutSim");

    int num_step = 0;
    std::vector<int>*   p_pixel_idx = 0;
    std::vector<int>*   p_pixel_idy = 0;
    std::vector<int>    pixel_idx;
    std::vector<int>    pixel_idy;

    outtree->Branch("Run",           &run,               "Run/I");
    outtree->Branch("Event",         &event,             "Event/I");
    outtree->Branch("TrackID",       &trackID,           "TrackID/I");
    outtree->Branch("PDGCode",       &pDGCode,           "PDGCode/I");
    outtree->Branch("numPhotons",    &numPhotons,        "numPhotons/I");
    outtree->Branch("energy",        &penergyAtAnode);
    outtree->Branch("numElectrons",  &pelectronAtAnode);
    outtree->Branch("x",             &pxPosAtAnode);
    outtree->Branch("y",             &pyPosAtAnode);
    outtree->Branch("z",             &pzPosAtAnode);
    outtree->Branch("timeAtAnode",   &ptimeAtAnode);
    outtree->Branch("num_step",      &num_step);
    outtree->Branch("identifier",    &identifier);
    outtree->Branch("pixel_idx",     &p_pixel_idx);
    outtree->Branch("pixel_idy",     &p_pixel_idy);

    if (verbose)
        std::cout << "gramsreadoutsim: turned on" << std::endl;

    int num_row = intree->GetEntries();
    int current_eventid = -1;

      for(int i=0;i<num_row;i++){
          intree->GetEntry(i);

          if(event != current_eventid){
              current_eventid = event;
              num_step = 1;
              if(i!=(num_row-1)){
                  do {
                      intree->GetEntry(i + num_step);
                      if(event == current_eventid){
                          num_step += 1;
                      }
                  } while((current_eventid==event)&&((i + num_step)<num_row));
                  intree->GetEntry(i);
              }
          }

          std::tie(pixel_idx, pixel_idy) = assignPixelID->Assign(pxPosAtAnode, pyPosAtAnode);
          p_pixel_idx = &pixel_idx;
          p_pixel_idy = &pixel_idy;

          outtree->Fill();
    }

    outtree->Write();
    output->Close();
}
