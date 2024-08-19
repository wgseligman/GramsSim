// simd.cc
// 02-Aug-2024 WGS

const bool debug = false;

// This is an evolving program to function as a rough display for
// GramsSim output. It started as an example by Andrea Pellot Jimenez
// that displayed information from the electron clusters arriving at
// the anode. I (WGS) added the user interface and additional plots
// for the results after each stage of the simulation.

// Elements of this code were taken from files in the following
// directory. I ran the different examples, and if I saw the kinds of
// things I wanted to do, I examined the code. It's confusing, but
// eventually the code started to make sense.

// `root-config --tutdir`/gui/

// This page was also quite helpful:
// https://root.cern/root/htmldoc/guides/users-guide/WritingGUI.html

// GramsSim includes
#include "SimulationDisplay.h"
#include "Options.h"

// ROOT includes
#include <TROOT.h>
#include <TApplication.h>

// ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....

// In interactive ROOT, this is the main routine that's called.
void simd()
{
  // Create a new display within the ROOT client program that's
  // running.
  new grams::SimulationDisplay(gClient->GetRoot());
}

// ....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....ooooOOOOoooo....

int main(int argc, char **argv)
{
  TApplication theApp("App", &argc, argv);

  if (gROOT->IsBatch()) {
    fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
    return 1;
  }

  simd();

  theApp.Run();

  return 0;
}
