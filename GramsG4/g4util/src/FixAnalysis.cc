// A separate function to repair a ROOT file that was created by the
// G4AnalysisManager.
// 24-Oct-2023 WGS

// The G4AnalysisManager has a bug: The files it creates cannot be
// opened in UPDATE mode. This function Post-processes the file
// created by G4AnalysisManager to remove the bug.

// The key part of this procedure is to use TFileMerger, the same
// class used by the ROOT command-line utility hadd. Unfortunately,
// the TFileMerger documentation is terrible. The following web sites
// may be of help:

// The actual class documentation:
// https://root.cern.ch/doc/master/classTFileMerger.html

// This is the source code for hadd, which uses TFileMerger. Of course, there
// are no comments in the code.
// https://root.cern.ch/doc/master/hadd_8cxx_source.html

// An example of using TFileMerger.
// https://cpp.hotexamples.com/examples/-/TFileMerger/Merge/cpp-tfilemerger-merge-method-examples.html

// A description of the G4Analysis bug:
// https://root-forum.cern.ch/t/opening-a-file-in-update-mode-causes-error-in-tbasket-streamer-the-value-of-fnbytes-is-incorrect/43340/15

// WGS' encounter with the G4Analysis bug:
// https://root-forum.cern.ch/t/appending-to-file-opened-in-update-mode-corrupts-file/56801

#include "FixAnalysis.h"

// For ROOT I/O operations.
#include "TFile.h" 
#include "TSystem.h"
#include "TFileMerger.h"

#include <string>
#include <iostream>
#include <random>

namespace g4util {

  // Forward declaration.
  std::string random_string(std::size_t length);

  bool FixAnalysis( const std::string filename ) 
  {

    // The basic operation we're going to do is "merge" (actually copy)
    // the defective G4AnalysisManager-produced file into a new file.

    // Define a work file name. Put a random string in the name, so
    // that if multiple simulation jobs are running in the same
    // directory, the work files won't conflict. (But won't the user's
    // ROOT output file names conflict? Yes, but there's only so much
    // you can do to protect users.)

    std::string workfile = "work_" + random_string(8) + "_" + filename;
    //std::cout << "FixAnalysis - workfile = '" << workfile << "'" 
    //	      << std::endl;

    // Note: ROOT's methods cannot accept std::string as arguments
    // directly. So we have to use the std::string::c_str() method to
    // convert them to C-style strings.

    // Rename the G4Analysis-produced output file to the temporary work
    // file name.
    gSystem->Rename(filename.c_str(),workfile.c_str());

    // Use the TFileMerger class to copy the file. You need the double
    // "kFALSE,kFALSE" here; otherwise the routine will complain that the
    // file name is not a URL.
    TFileMerger* merger = new TFileMerger(kFALSE,kFALSE); 

    // The input file is the work file, that is, the file the
    // G4AnalysisManager created.
    auto input = TFile::Open(workfile.c_str());

    // This turns out to be mandatory: Get the compression setting of
    // the input file.
    auto compression = input->GetCompressionSettings();

    // Add the workfile to the list of files to be merged. Here, there's
    // only going to be one file in the merger's list.
    merger->AddFile(input); 

    // Tell TFileMerger to use the compression settings of the input
    // file. If you omit this, the output ntuples will be unreadable.
    merger->OutputFile(filename.c_str(),"RECREATE",compression);

    // Copy the input to the output, incidentally fixing the
    // G4AnalysisManager bug.
    merger->Merge();

    // Clean up.
    delete merger;
    input->Close();
    delete input;

    // Remove the work file; it just wastes disk space at this point.
    gSystem->Unlink(workfile.c_str());

    return true;
  }

  // A routine to create a random string of characters. Source:
  // https://inversepalindrome.com/blog/how-to-create-a-random-string-in-cpp

  std::string random_string(std::size_t length)
  {
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    std::string random_string;

    for (std::size_t i = 0; i < length; ++i)
      {
        random_string += CHARACTERS[distribution(generator)];
      }

    return random_string;
  }

} // namespace g4util

