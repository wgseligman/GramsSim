/// \file GramsG4WriteHitsAction.cc
/// \brief User-action class to write out hits.

/// This is a user-action class that writes out the
/// event hits. At present, the output takes the form
/// of basic (and inefficient) ROOT n-tuple.

#include "GramsG4WriteHitsAction.hh"
#include "GramsG4LArHit.hh"

#include "Options.h" // in util/
#include "UserAction.h" // in g4util/
#include "Analysis.h" // in g4util/

#include "G4Run.hh"
#include "G4Event.hh"

namespace gramsg4 {

  WriteHitsAction::WriteHitsAction()
    : UserAction()
  {
    // Access (maybe create) the G4AnalysisManager.
    auto analysisManager = G4AnalysisManager::Instance();

    auto options = util::Options::GetInstance();

    G4String outputDirectory;
    options->GetOption("outputdirectory",outputDirectory);
    if ( outputDirectory.size() > 0 )
      analysisManager->SetNtupleDirectoryName(outputDirectory);

    G4bool verbose;
    options->GetOption("verbose",verbose);
    if (verbose)
      analysisManager->SetVerboseLevel(1);

    analysisManager->SetNtupleMerging(true);

    // Creating ntuple
    analysisManager->CreateNtuple("LArHits", "LAr TPC energy deposits");
    analysisManager->CreateNtupleIColumn("Run");
    analysisManager->CreateNtupleIColumn("Event");
    analysisManager->CreateNtupleIColumn("TrackID");
    analysisManager->CreateNtupleIColumn("PDGCode");
    analysisManager->CreateNtupleIColumn("numPhotons");
    analysisManager->CreateNtupleDColumn("energy");
    analysisManager->CreateNtupleDColumn("xPos");
    analysisManager->CreateNtupleDColumn("yPos");
    analysisManager->CreateNtupleDColumn("zPos");
    analysisManager->FinishNtuple();
  }

  WriteHitsAction::~WriteHitsAction() {}

  // The user-action classes we'll need to define
  // the output file and write hits.

  void WriteHitsAction::BeginOfRunAction(const G4Run*) {
    auto analysisManager = G4AnalysisManager::Instance();
    auto options = util::Options::GetInstance();

    G4String filename;
    options->GetOption("outputfile",filename);
    analysisManager->OpenFile(filename);
  }

  void WriteHitsAction::EndOfRunAction(const G4Run*) {
    auto analysisManager = G4AnalysisManager::Instance();

    // Save the n-tuple.
    analysisManager->Write();
    analysisManager->CloseFile();
  }

  void WriteHitsAction::BeginOfEventAction(const G4Event*) {}

  void WriteHitsAction::EndOfEventAction(const G4Event*) {}

} // namespace gramsg4
