// gramselecsim.cc
// 25-Nov-2011 William Seligman

// Our function(s) for the detector response.
#include "UtilFunctions.h"
#include "AddNoise.h"
#include "ADConvert.h"
#include "ElecStructure.h"
#include "PreampProcessor.h"
#include "LoadOptionFile.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/

// ROOT includes
#include "TFile.h"
#include "TTree.h"

// C++ includes
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <numeric>
#include <ctime>
#include <unordered_map>

///////////////////////////////////////


int main(int argc,char **argv)
{
    auto options = util::Options::GetInstance();
    auto result = options->ParseOptions(argc, argv, "gramselecsim");

    if (! result) {
        std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
            << "gramselecsim: Aborting job due to failure to parse options"
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
      options->PrintOptions();
    }

    result = options->ParseOptions(argc, argv, "gramselecsim");
    std::string inputFileName;
    std::string inputNtupleName;
    options->GetOption("inputfile",     inputFileName);
    options->GetOption("inputntuple",   inputNtupleName);
    gramselecsim::LoadOptionFile optionloader;

    if (verbose)
        std::cout << "gramselecsim: input file = '" << inputFileName
              << "', input ntuple = '" << inputNtupleName
              << "'" << std::endl;

    auto input = TFile::Open(inputFileName.c_str());
    if (!input || input->IsZombie()) {
      std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
            << "gramselecsim: Could not open file '" << inputFileName << "'"
            << std::endl;
      exit(EXIT_FAILURE);
    }

    double driftVel;
    double pixel_plane_offset;
    double readout_centerx;
    double readout_centery;
    double pixel_sizex;
    double pixel_sizey;

    gDirectory->cd("header");
    TTree* inheadertree = (TTree*)input->Get("header/metatree");
    inheadertree->SetBranchAddress("DriftVel",            &driftVel             );
    inheadertree->SetBranchAddress("pixel_plane_offset",  &pixel_plane_offset   );
    inheadertree->SetBranchAddress("readout_centerx",     &readout_centerx      );
    inheadertree->SetBranchAddress("readout_centery",     &readout_centery      );
    inheadertree->SetBranchAddress("pixel_sizex",         &pixel_sizex          );
    inheadertree->SetBranchAddress("pixel_sizey",         &pixel_sizey          );
    inheadertree->GetEntry(0);

    TTree* intree = (TTree*)input->Get("data/ReadoutSim");

    gramselecsim::ADConvert* adconverter = NULL;
    adconverter = new gramselecsim::ADConvert();

    gramselecsim::AddNoise* addNoise = NULL;
    addNoise = new gramselecsim::AddNoise();

    int run;
    int event;
    int num_step;
    
    std::vector<double>   timeAtAnode;
    std::vector<double>   numElectrons;
    std::vector<double>*  ptimeAtAnode = 0;
    std::vector<double>*  pnumElectrons = 0;
    std::vector<int>*     p_pixel_idx = 0;
    std::vector<int>*     p_pixel_idy = 0;

    int nEntry = intree->GetEntries();
    intree->SetBranchAddress("Run",             &run);
    intree->SetBranchAddress("Event",           &event);
    intree->SetBranchAddress("timeAtAnode",     &ptimeAtAnode);
    intree->SetBranchAddress("numElectrons",    &pnumElectrons);
    intree->SetBranchAddress("pixel_idx",       &p_pixel_idx);
    intree->SetBranchAddress("pixel_idy",       &p_pixel_idy);
    intree->SetBranchAddress("num_step",        &num_step);

    // Now read in the options associated with the output file and ntuple.
    std::string outputFileName;
    options->GetOption("outputfile",    outputFileName);

    std::string outputNtupleName;
    options->GetOption("outputntuple",  outputNtupleName);

    gramselecsim::general_header    header_gen      = optionloader.GeneralHeader();
    gramselecsim::preamp_header     header_pre      = optionloader.PreampHeader();
    gramselecsim::noise_header      header_noise    = optionloader.NoiseHeader();
    gramselecsim::adc_header        header_adc      = optionloader.ADCHeader();

    std::string general_header_type     = gramselecsim::GeneralHeaderType();
    std::string preamp_header_type      = gramselecsim::PreampHeaderType();
    std::string noise_header_type       = gramselecsim::NoiseHeaderType();
    std::string adc_header_type         = gramselecsim::ADCHeaderType();

    double sample_freq = header_adc.sample_freq;

    auto output = TFile::Open(outputFileName.c_str(),"RECREATE");

    //meta data is recorded to header
    output->mkdir("header");
    gDirectory->cd();
    gDirectory->cd("header");

    TTree* metatree = new TTree("metatree","metatree");

    metatree->Branch("general",     &header_gen,        general_header_type.c_str() );
    metatree->Branch("preamp",      &header_pre,        preamp_header_type.c_str()  );
    metatree->Branch("noise",       &header_noise,      noise_header_type.c_str()   );
    metatree->Branch("adc",         &header_adc,        adc_header_type.c_str()     );
    metatree->Branch("DriftVel",            &driftVel,              "DriftVel/D" );
    metatree->Branch("pixel_plane_offset",  &pixel_plane_offset,    "pixel_plane_offset/D" );
    metatree->Branch("readout_centerx",     &readout_centerx,       "readout_centerx/D" );
    metatree->Branch("readout_centery",     &readout_centery,       "readout_centery/D" );
    metatree->Branch("pixel_sizex",         &pixel_sizex,           "pixel_sizex/D" );
    metatree->Branch("pixel_sizey",         &pixel_sizey,           "pixel_sizey/D" );
    metatree->Branch("sample_freq",         &sample_freq,           "sample_freq/D" );

    metatree->Fill();
    metatree->Write();

    //int response_current_bin = int(header_current.current_func_duration / header_gen.timebin_width);
    int num_tbin = int(header_gen.time_window / header_gen.timebin_width);

    gramselecsim::PreampProcessor* preampProcessor = NULL;
    preampProcessor = new gramselecsim::PreampProcessor(num_tbin);

    //information of each event
    gDirectory->cd();
    output->mkdir("data");
    gDirectory->cd();
    gDirectory->cd("../data");
    TTree* outtree = new TTree(outputNtupleName.c_str(),"Electronics Response");

    std::vector<double>*  p_charge_a = 0;
    std::vector<int>*     p_charge_d = 0;
    int pixel_idx = 0;
    int pixel_idy = 0;

    outtree->Branch("Run",              &run,          "Run/I");
    outtree->Branch("Event",            &event,        "Event/I");
    outtree->Branch("pixel_idx",        &pixel_idx,    "pixel_idx/I");
    outtree->Branch("pixel_idy",        &pixel_idy,    "pixel_idy/I");
    outtree->Branch("charge_A",         &p_charge_a);
    outtree->Branch("charge_D",         &p_charge_d);

    std::vector<int> pixel_pair(2, 0);
    std::vector<int> arrival_counter(num_tbin, 0);
    std::unordered_map<std::vector<int>, std::vector<int>, gramselecsim::HashVI>::iterator iter_mp;
    std::vector<int> arrival_time_counter_temp(num_tbin, 0);
    std::unordered_map<std::vector<int>, std::vector<int>, gramselecsim::HashVI> mp_pixel2waveform;
    std::vector<double> waveform_with_noise( num_tbin, 0.0 );
    std::vector<double> waveform_csa_shaper( num_tbin, 0.0 );
    std::vector<int>    digi_waveform( num_tbin, 0 );
    std::vector<int> zeros_array(num_tbin, 0);

    std::vector<int> hittime;

    int num_cl;
    int ti;
    time_t t1 = time(NULL);

    for(int istep=0;istep<nEntry;istep++){
        
        mp_pixel2waveform.clear();
        intree->GetEntry(istep);

        //Aggregate pixels electrons arrive at
        for(int j=0; j<num_step; j++){

            intree->GetEntry(istep + j);
            num_cl = (*p_pixel_idx).size();

            for(int jcl=0;jcl<num_cl;jcl++){

                ti = std::min(num_tbin-1, int(std::floor((*ptimeAtAnode)[jcl] / header_gen.timebin_width)));
                pixel_pair[0] = (*p_pixel_idx)[jcl];
                pixel_pair[1] = (*p_pixel_idy)[jcl];

                auto itr = mp_pixel2waveform.find(pixel_pair);

                //A cluster is the first one that arrives at the pixel
                if(itr == mp_pixel2waveform.end()){
                    std::fill(arrival_time_counter_temp.begin(), arrival_time_counter_temp.end(), 0);
                    arrival_time_counter_temp[ti] = (*pnumElectrons)[jcl];
                    mp_pixel2waveform[pixel_pair] = arrival_time_counter_temp;
                //At least One cluster already arrived at the pixel
                }   else{
                    mp_pixel2waveform[pixel_pair][ti] += (*pnumElectrons)[jcl];
                }
            }
        }

        for(iter_mp=mp_pixel2waveform.begin(); iter_mp != mp_pixel2waveform.end(); iter_mp++){

            std::vector<double> waveform(num_tbin, 0.0);
            pixel_idx = iter_mp->first[0];
            pixel_idy = iter_mp->first[1];
            std::vector<int> num_arrival_electron = iter_mp->second;
            std::vector<int> num_arrival_electron_with_noise;

            // Add noise to mp_pixel2waveform:[[ipix, ipiy]: e_arrival_flag]
            num_arrival_electron_with_noise = addNoise->ProcessElectronNoise(num_arrival_electron);
            
            //Add a response function
            waveform_csa_shaper = preampProcessor->ConvoluteResponse( num_arrival_electron_with_noise );
            p_charge_a = &waveform_csa_shaper;

            //Convert analog into digital
            digi_waveform = adconverter->Process(waveform_csa_shaper);
            p_charge_d = &digi_waveform;

            outtree->Fill();
        }

        mp_pixel2waveform.clear();
        istep += (num_step-1);
    }

    time_t t2 = time(NULL);
    std::cout << "Time: " << t2 - t1 << "s" << std::endl;

    outtree->Write();
    output->Close();
}
