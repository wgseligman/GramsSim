// 26-Nov-2021 WGS
// Write the contents of the Options class to an ntuple.

#include "WriteOptions.h"
#include "Options.h"

#include "TROOT.h"
#include "TDirectory.h"
#include "TTree.h"

#include <string>

namespace util {

  bool WriteOptions::WriteNtuple( TDirectory* a_output, std::string a_ntupleName ) {
    // Start by assuming we'll succeed.
    bool success = true;

    // Almost certainly, ROOT's current directory is the output file
    // to which we're writing the ntuple. But just in case, save the
    // current directory, and switch to the output directory.
    auto saveDirectory = gROOT->CurrentDirectory();
    a_output->cd();

    // Create the ntuple in the output file.
    auto ntuple = new TTree(a_ntupleName.c_str(), "Options used for this program");

    // Set up the columns of the ntuple. 
    std::string name, value, type, brief, desc;
    ntuple->Branch("OptionName",&name);
    ntuple->Branch("OptionValue",&value);
    ntuple->Branch("OptionType",&type);
    ntuple->Branch("OptionBrief",&brief);
    ntuple->Branch("OptionDesc",&desc);

    // Loop over the options.  As it says in Option.h,
    // this is an inefficient operation, but hopefully no program will
    // do it more than once.
    auto options = util::Options::GetInstance();
    auto numOptions = options->NumberOfOptions();
    for ( size_t i = 0; i != numOptions; ++i ) {
      name = options->GetOptionName(i);
      value = options->GetOptionValue(i);
      type = options->GetOptionType(i);
      brief = options->GetOptionBrief(i);
      desc = options->GetOptionDescription(i);

      // Write out the ntuple entry.
      ntuple->Fill();
    }

    // Wrap up the ntuple.
    ntuple->Write();

    // Switch back to the original directory.
    saveDirectory->cd();

    return success;
  }

} // namespace util
