// These lines are automatically executed when you start interactive
// ROOT from this directory.

// These lines assume you have created a GramsSim build directory
// following the recipe in the documentation:
// https://github.com/wgseligman/GramsSim/blob/develop/README.md and
// are running the script with the above command in that directory. If
// you've moved the script, then you'll have to edit the "gSystem"
// lines.

{
  gSystem->AddIncludePath("../GramsSim/GramsDataObj/include");
  std::string arch(gSystem->GetBuildArch());
  if ( arch.substr(0,3) == "mac" )
    gSystem->Load("./libDictionary.dylib");
  else
    gSystem->Load("./libDictionary.so");
}
