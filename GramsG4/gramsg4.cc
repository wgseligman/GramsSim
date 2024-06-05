//
/// \file GramsG4/gramsg4.cc
/// \author William Seligman (seligman@nevis.columbia.edu)
///
// --------------------------------------------------------------
//      GEANT 4 - gramsg4
// --------------------------------------------------------------

// Accommodate different versions of Geant4. 
#include "G4Version.hh"

// For processing command-line and XML file options.
#include "Options.h" // in util/

// For appending the detector geometry to the output.
#include "Geometry.h" // in util/

#include "Analysis.h"    // in g4util/
#include "FixAnalysis.h" // in g4util/

// There are three mandatory classes needed for any Geant4 application
// to work. One is the detector geometry:
#include "GramsG4DetectorConstruction.hh"

// The other two are a way to create or fetch primary events, and
// something for the simulation to with its events. These are both
// defined here:
#include "GramsG4ActionInitialization.hh"

// Classes required for the UserActionManager.
// See these header files for a description
// of what a "user action manager" is and
// why it's useful. 
#include "UserAction.h"        // in g4util/
#include "UserActionManager.h" // in g4util/

// The user-action class that will set any user-assigned run and event
// numbers.
#include "GramsG4NumbersAction.hh"

// The user-action class that will write output.
#include "GramsG4WriteNtuplesAction.hh"

// Setting up the random-number engine.
#include "GramsG4RandomSeedAction.hh"

#if G4VERSION_NUMBER<1070

// This is the "old" way of handling multi-threading in Geant4.
// There is a different run manager for the multi-threaded case.

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#else // G4VERSION >= 1070

// This is the "modern" method of handling multi-threaded
// execution in Geant4. There is a single run-manager factory.
#include "G4RunManagerFactory.hh"

#endif // multi-threaded run-manager includes

// Solely for setting the verbosity of the physics display.
#include "G4EmParameters.hh"
#include "G4HadronicParameters.hh"

#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4TransportationManager.hh"

// Refer to examples/extended/physicslists/extensibleFactory within
// the Geant4 installation for details about the physics-list
// factory. All the physics-list code (including the stubs for example
// user physics lists) was pulled from there.
#include "G4PhysListFactoryAlt.hh"
// Allow ourselves to extend the short names for physics ctor
// addition/replace along the same lines as EMX, EMY, etc
#include "G4PhysListRegistry.hh"

// For turning on optical photons.
#include "G4OpticalPhysics.hh"

// Allow ourselves to give the user extra info about available physics ctors
#include "G4PhysicsConstructorFactory.hh"

// Pull in a user defined physics list definition into the main
// program and register it with the factory (doesn't have to be the
// main program but the .o containing the declaration _must_ get
// linked/loaded)
#include "G4PhysListStamper.hh"  // defines macro for factory registration
#include "MySpecialPhysList.hh"
G4_DECLARE_PHYSLIST_FACTORY(MySpecialPhysList);

// --------------------------------------------------------------

// forward declaration.
void PrintAvailablePhysics();

int main(int argc,char **argv)
{
  // Initialize the options from the XML file and the
  // command line. Make sure this happens first!
  auto options = util::Options::GetInstance();

  // The third argument of ParseOptions, 'gramsg4', is the name of the
  // tag to use for this program's options. See options.xml and/or
  // README.md to see how this works.
  auto result = options->ParseOptions(argc, argv, "gramsg4");

  // Abort if we couldn't parse the job options.
  if (result) G4cout << "ParseOptions succeeded" << G4endl;
  else {
    G4ExceptionDescription description;
    description << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
		<< "GramsG4: Aborting job due to failure to parse options";
    G4Exception("GramsG4 main()","invalid options",
		FatalException, description);
    exit(EXIT_FAILURE);
  }

  // We may not be debugging this at this moment, but if we
  // want to use the debug flag later in this routine,
  // let's have it ready.
  G4bool debug;
  options->GetOption("debug",debug);

  // Check for help message.
  G4bool help;
  options->GetOption("help",help);
  if (help) {
    options->PrintHelp();
    exit(EXIT_SUCCESS);
  }

  // Does the user just want to see the physics lists?
  G4bool showLists;
  options->GetOption("showphysicslists",showLists);
  if (showLists) {
    PrintAvailablePhysics();
    exit(EXIT_SUCCESS);
  }

  G4bool verbose;
  options->GetOption("verbose",verbose);
  if (verbose) {
    // Display all program options and physics lists.
    options->PrintOptions();
    PrintAvailablePhysics();
  }
  
  // Set up the Geant4 Run Manager. First, deal with multiple threads
  // (if available).

  G4int nThreads;
  options->GetOption("nthreads",nThreads);
  if ( nThreads <= 0 ) nThreads = 1;

  // A sanity check: If we're reading a file of generated events, then
  // the number of threads has to be set to 1. Otherwise each
  // individual thread will read the entire input file.
  std::string inputFile;
  result = options->GetOption("inputgen",inputFile);
  if ( result  &&  !inputFile.empty()  &&  nThreads > 1 ) {
    G4ExceptionDescription description;
    description << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
		<< "GramsG4: the number of execution threads is "
		<< nThreads << G4endl
		<< "and we're also reading events from file '" << inputFile
		<< "'." << G4endl
		<< "Each individual thread would read the same events from the same file."
		<< G4endl
		<< "Setting number of threads to 1";
    G4Exception("GramsG4 main()","reading file of events with more than one thread",
		JustWarning, description);
    nThreads = 1;
  }

#if G4VERSION_NUMBER<1070

  // This is the "old" way of handling multi-threading in Geant4.
  // There is a different run manager for the multi-threaded case.

#ifdef G4MULTITHREADED
  auto runManager = new G4MTRunManager;
  if (verbose) G4cout << "GramsG4::main(): Setting number of worker threads to "
		      << nThreads << G4endl;
  runManager->SetNumberOfThreads(nThreads);
#else
  auto runManager = new G4RunManager;
#endif

#else // G4VERSION >= 1070

  // The "modern" way to implement threads: Use a run-manager factory. 
  if (verbose) G4cout << "GramsG4::main(): Setting number of worker threads to "
		      << nThreads << G4endl;

  auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default, nThreads);

#endif // multiple-threaded run manager.
  
  // g4alt::G4PhysListFactory is the extensible factory
  g4alt::G4PhysListFactory factory;
  
  // This is an example of how to assign an alternate name to an
  // existing physics-list extension.  For example, after the
  // following definitions, you could specific a physics list by
  // "FTFP_BERT+OPTICAL" instead of "FTFP_BERT+G4OpticalPhysics".
  auto plreg = G4PhysListRegistry::Instance();
  plreg->AddPhysicsExtension("OPTICAL","G4OpticalPhysics");
  plreg->AddPhysicsExtension("STEPLIMIT", "G4StepLimiterPhysics");
  plreg->AddPhysicsExtension("RADIO","G4RadioactiveDecayPhysics");
  plreg->AddPhysicsExtension("MYPHYSICS","MyG4PhysicsPhysics");
  if ( verbose ) {
    G4cout << "Extensible physics factory: adding extensions:" << G4endl
	   << "   OPTICAL   ===> G4OpticalPhysics" << G4endl
	   << "   STEPLIMIT ===> G4StepLimiterPhysics" << G4endl
	   << "   RADIO     ===> G4RadioactiveDecayPhysics" << G4endl
	   << "   MYPHYSICS ===> MyG4PhysicsPhysics" << G4endl
	   << G4endl;
  }
  
  G4String physicsList;
  options->GetOption("physicslist",physicsList);
  auto physics = factory.GetReferencePhysList(physicsList);
  if (verbose) G4cout << "GramsG4::main(): Setting physics list to "
		      << physicsList << G4endl;
  
  if ( physics == NULL )
    {
      G4ExceptionDescription description;
      description << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
		  << "GramsG4: Could not find or interpret physics list option"
		  << G4endl
		  << "Use '" << argv[0] << " -l' to see all available lists";
      G4Exception("GramsG4 main()","invalid option",
		  FatalException, description);
      exit(EXIT_FAILURE);
    }

  // Setting whether the details of the physics list are displayed
  // at the start of the job.
  auto emParam = G4EmParameters::Instance();
  auto hadParam = G4HadronicParameters::Instance();
  emParam->SetVerbose(0);
  hadParam->SetVerboseLevel(0);
  if (verbose) {
    emParam->SetVerbose(1);
    hadParam->SetVerboseLevel(1);
  }
  
  // Include optical photons if requested.
  // This was copied from 
  // artg4tk/artg4tk/pluginActions/physicsList/PhysicsList_service.cc 

  G4OpticalPhysics* opticalPhysics = (G4OpticalPhysics*) physics->GetPhysics("Optical");
  if ( opticalPhysics != NULL ) {

    if (verbose) G4cout << "GramsG4::main(): Optical physics is on" << G4endl;

    // There are many more optical-physics options than the ones
    // listed below. At some point we may want to expand the user
    // options to allow for greater control. For now, duplicate the
    // defaults used by artg4tk in LArSoft.

    G4bool isScint;
    options->GetOption("scint",isScint);
    if (verbose) {
      if (isScint)
	G4cout << "GramsG4::main(): Scintillation is on" << G4endl;
      else 
	G4cout << "GramsG4::main(): Scintillation is off" << G4endl;
    }
    G4bool isCerenkov;
    options->GetOption("cerenkov",isCerenkov);
    if (verbose) {
      if (isCerenkov)
	G4cout << "GramsG4::main(): Cerenkov light is on" << G4endl;
      else 
	G4cout << "GramsG4::main(): Cerenkov light is off" << G4endl;
    }

#if G4VERSION_NUMBER<1070
    // The following optical photon controls were eliminated in Geant4.11.0 and higher.
    opticalPhysics->SetScintillationStackPhotons(false);
    if (isScint)
      opticalPhysics->Configure(kScintillation,true);
    else
      opticalPhysics->Configure(kScintillation,false);

    opticalPhysics->SetCerenkovStackPhotons(false);
    if (isCerenkov)
      opticalPhysics->Configure(kCerenkov,true);
    else
      opticalPhysics->Configure(kCerenkov,false);
#else
    // These methods of controlling optical physics were introduced
    // in Geant4.7.
    auto opticalParams = G4OpticalParameters::Instance();
    if (isScint)
      opticalParams->SetProcessActivation("Scintillation",true);
    else
      opticalParams->SetProcessActivation("Scintillation",false);
    opticalParams->SetScintStackPhotons(false);

    if (isCerenkov)
      opticalParams->SetProcessActivation("Cerenkov",true);
    else
      opticalParams->SetProcessActivation("Cerenkov",false);
    opticalParams->SetCerenkovStackPhotons(false);
#endif
    
  } // if opticalphysics

  // Control the verbosity of the physics-list display at the
  // start of the run.
  

  
  // Usual initializations: detector, physics, and user actions.
  runManager->SetUserInitialization(new gramsg4::DetectorConstruction());
  runManager->SetUserInitialization(physics);

  // ***** Set up the User Action Manager *****
  // See the header files in directory g4util for a lengthy
  // description of this facility. 

  g4util::UserActionManager* uaManager = new g4util::UserActionManager();
  // The UserActionManager is itself a UserAction class.
  g4util::UserAction* uam = (g4util::UserAction*) uaManager;

  // Add this application's user actions to our user-action manager.
  uaManager->AddAndAdoptAction( new gramsg4::RandomSeedAction() );
  uaManager->AddAndAdoptAction( new gramsg4::NumbersAction() );
  uaManager->AddAndAdoptAction( new gramsg4::WriteNtuplesAction() );

  // Pass the g4util::UserActionManager to Geant4's user-action initializer.
  auto actInit = new gramsg4::ActionInitialization();
  actInit->SetUserActionLink(uam);
  runManager->SetUserInitialization(actInit);

  // ***** end User Action Manager setup *****

  // Tell Geant4 we're about to begin. 
  // Oct-2022 WGS: This is now supplied via the G4 macro file. 
  // runManager->Initialize();

  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
   
  const G4String command("/control/execute ");

  // Which macro file will we execute? If the user specified the UI
  // mode, then use the uimacrofile parameter. Otherwise use the
  // macrofile parameter. (This is to make it easier to switch between
  // batch mode and UI mode for testing.)

  G4bool uiMode(false);
  options->GetOption("ui",uiMode);
  G4String macroFile;
  if (uiMode) 
    options->GetOption("uimacrofile",macroFile);
  else
    options->GetOption("macrofile",macroFile);

  if (uiMode) {
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
    UImanager->ApplyCommand(command+macroFile);
    ui->SessionStart();
    delete ui;
  }
  else
    UImanager->ApplyCommand(command+macroFile);

  // Clean-up

  delete visManager;
  delete runManager;

  // At the end of the simulation, after all files have hopefully been
  // closed...

  // The G4AnalysisManager has a bug: The files it creates cannot be
  // opened in UPDATE mode. Post-process the file created by
  // G4AnalysisManager to remove the bug.

  G4String filename;
  options->GetOption("outputg4file",filename);
  //g4util::FixAnalysis( filename );

  // In addition to recording the options used to run this program,
  // let's see if we can write a ROOT form of the detector geometry to
  // the output file as well. The Geometry::GDML2ROOT() method with
  // default arguments should be able to take care of this.

  auto geometry = util::Geometry::GetInstance();
  geometry->GDML2ROOT("", filename);
  
  return 0;
}

// --------------------------------------------------------------

void PrintAvailablePhysics() {
  G4cout << "Available physics lists:"
	 << G4endl;
  g4alt::G4PhysListFactory factory;
  factory.PrintAvailablePhysLists();
  
  G4cout << G4endl;
  G4cout << "Available physics ctors that can be added:"
	 << G4endl;
  auto g4pctorFactory =
    G4PhysicsConstructorRegistry::Instance();
  g4pctorFactory->PrintAvailablePhysicsConstructors();
}
