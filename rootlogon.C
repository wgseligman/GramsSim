// These lines are automatically executed when you start interactive
// ROOT from this directory.

// These lines assume you have created a GramsSim build directory
// following the recipe in the documentation:
// https://github.com/wgseligman/GramsSim/blob/develop/README.md and
// are running the script with the above command in that directory. If
// you've moved the script, then you'll have to edit the "gSystem"
// lines.

{
  gInterpreter->AddIncludePath("../GramsSim/GramsDataObj/include");
  // This line creates a text string containing the library name, with
  // the appropriate extension (.so or .dylib) for this operating
  // system.
  TString lib(Form("./libDictionary.%s", gSystem->GetSoExt()));
  gSystem->Load(lib.Data());
}
