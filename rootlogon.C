// These lines are automatically executed when you start interactive
// ROOT from this directory.

// Let ROOT know about the header files in 'include' and the location
// of the ROOT dictionary for GramsSim.
{
  gInterpreter->AddIncludePath("include");
  std::string arch(gSystem->GetBuildArch());
  if ( arch.substr(0,3) == "mac" ) {
    gSystem->Load("libUtilities.dylib");
    gSystem->Load("libDictionary.dylib");
  }
  else {
    gSystem->Load("libUtilities.so");
    gSystem->Load("libDictionary.so");
  }
}
