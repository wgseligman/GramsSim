// These lines are automatically executed when you start interactive
// ROOT from this directory.

// Let ROOT know about the header files in 'include' and the location
// of the ROOT dictionary for GramsSim.
{
  gInterpreter->AddIncludePath("include");
  // This line creates a text string containing the library name, with
  // the appropriate extension (.so or .dylib) for this operating
  // system.
  TString lib(Form("libDictionary.%s", gSystem->GetSoExt()));
  gSystem->Load(lib.Data());
}
