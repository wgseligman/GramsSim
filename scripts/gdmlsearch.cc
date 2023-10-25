// gdmlsearch.cc
// 29-Dec-2021 WGS
//
// This routine is a "scrap" to demonstrate how to search a geometry
// in ROOT and get information about individual volumes.

// ROOT includes
#include <TGeoManager.h>
#include <TGeoVolume.h>
#include <TGeoBBox.h>
#include <TGeoMatrix.h>
#include <TString.h>
#include <TRegexp.h>

// C++ includes
#include <iostream>

// Avoid meaningless warning message by omitting the names of the
// standard arguments to 'main'. However, if this is ever expanded
// beyond a stub (e.g., to get options from the XML file) then these
// arguments may have to be restored.

//int main(int argc, char** argv) {
int main(int, char**) {

  // In a real program, this flag would come from the Options class
  // (see GramsSim/util/README.md).
  bool debug = true;

  // ROOT cannot read a GDML file that contains formulas, loops, or
  // replicas. Geant4 can. Therefore, gramsg4 (a Geant4 simulation)
  // has a feature that can write out a ROOT-compatible GDML
  // file. It's the '--gdmlout' option, as defined in the options.xml
  // file. To use it:
  //
  // ./gramsg4 --gdmlout <output-geometry-file>
  //
  // This only has to be repeated if the contents of the input GDML
  // file (normally grams.gdml) are changed.
  //
  // In this example, I named that output geometry file 'parsed.gdml'.
  // I could have have used a file extension of .root so the file
  // would be in ROOT format. However, a GDML file is closer to
  // human-readable.
  //
  // Use ROOT's geometry manager to read in the geometry file. Note that
  // in real code, the name of this file should come in via the
  // Options class.
  auto geom = TGeoManager::Import("parsed.gdml");

  // This scrap program only does one thing for one volume. But it can
  // serve as an example for a more general geometry search routine,
  // which we'll need when we start thinking about reconstruction.
  //
  // So let's be a bit general and define a "starting volume" for our
  // search. In this particular case, we're going to search all the
  // volumes in the detector so we start at the top (the world
  // volume). In a more general routine, we might start at some other
  // volume.
  TGeoVolume* startVol = geom->GetTopVolume();

  // This is the volume name we're going to search for. Again, this
  // name should come in via Options class in a general routine, but
  // here I'm hard-coding it. For this example, I discovered that the
  // volume containing the anode was 'volTilePlane' by browsing
  // through grams.gdml and reading the comments.
  TString volName = "volTilePlane";

  // A "regular expression" (abbreviated regexp) is a powerful tool
  // for searching and manipulating text. Here we're using it in its
  // simplest possible form: search for a volume whose name contains
  // 'volTilePlane'.
  TRegexp searchFor(volName);

  // Even though this is a scrap routine, let's be as correct as we
  // can be. We're going to be manipulating ROOT's TGeoManager as we
  // search through the geometry. One day this code may be in a
  // subroutine. The calling routine may have the TGeoManager pointing
  // to a certain volume; we're about change that in our search. So
  // let's save the current state of the TGeoManager now, and restore
  // it at the end of the routine. This means that if the caller is
  // doing something complicated, we haven't ruined it for them.
  TString managerPath = startVol->GetGeoManager()->GetPath();

  // In general, a ROOT "geometry" consists of nodes that define
  // rotations and translations of volumes, and volumes contain
  // daughter nodes. A typical detector description looks like a
  // branching tree of nodes containing volumes containing nodes
  // containing volumes... There's a nice diagram of this in ROOT's
  // TGeoManager page at
  // https://root.cern.ch/doc/master/classTGeoManager.html.
  //
  // We're going to avoid that complexity by using an "iterator". This
  // will navigate through the geometry, beginning with our starting
  // volume, descending into the geometric tree. We don't have to
  // understand the geometric structure; the iterator will take care
  // of it for us.
  TGeoIterator next(startVol);

  // Define a variable for the current node being inspected by the
  // iterator.
  TGeoNode* current;

  // A variable that will contain the name of the node.
  TString nodePath;

  // For each node and subnode in the geometry:
  while ( ( current = next() ) ) {

    // Get the name of the node we're on; 'GetPath' puts that name
    // into the TString in its argument.
    next.GetPath( nodePath );

    // If the node's name contains the regular expression..
    if ( nodePath.Contains(searchFor) ) {

      // We found it! Look at the volume contained within the node.
      auto volume=current->GetVolume();

      if (debug) {
	std::cout << "Found " << volName << "!" << std::endl;
	std::cout << "Volume name is " << volume->GetName() << std::endl;
	std::cout << "Volume path is " << volume->GetGeoManager()->GetPath() << std::endl;
	std::cout << "Node name is " << current->GetName() << std::endl;
	std::cout << "Node Path is " << nodePath << std::endl;

	volume->Print();
      }

      // The immediate purpose of this routine is to determine the
      // z-location of the bottom of the anode. In the current GRAMS
      // design, particles drift in the +z direction toward the bottom
      // (low-z) surface of anode. If we decide to change the
      // coordinate system (e.g., in LArSoft particles drift in the -x
      // direction) then the following code must be changed. Or
      // perhaps we should accept a "drift direction" parameter in the
      // Options file?

      // Get the overall shape of the volume. We'll be in trouble if
      // the shape doesn't have a bounding box. Fortunately, at least
      // as of ROOT 6.22 (and all currently-forseeable future versions
      // of ROOT), every potential shape (cube, cone, cylinder,
      // sphere...) has a bounding box.
      auto box = dynamic_cast<TGeoBBox*>( volume->GetShape() );

      // Note that length units of the GRAMS GDML file are
      // _centimeters_. Geant4 automatically converts these to its
      // internal length units (mm). Either ROOT does not do this, or
      // it uses its default length unit of cm.

      // The center of the box.
      auto origin = box->GetOrigin();

      // The half-height of the box in z.
      auto halfZ = box->GetDZ();

      // Calculate the (x,y,z) of the bottom center of the box.
      double bottomCenter[3] = { origin[0], origin[1], origin[2] - halfZ };

      if (debug) {
	std::cout << "DZ = " << halfZ << std::endl;
	std::cout << "Z origin = " << origin[2] << std::endl;
	std::cout << "bottom center = (" 
		  << bottomCenter[0] << ","
		  << bottomCenter[1] << ","
		  << bottomCenter[2] << ")"
		  << std::endl;
      }

      // Use the TGeoManager to navigate to this node. (Yes,
      // 'GetGeoManager' will return the same value as 'geom' that we
      // defined above. But in a general routine that does not start
      // its searches at the top of the detector geometry, this is how
      // you get the TGeoManager pointer.)
      volume->GetGeoManager()->cd(nodePath.Data());

      // In ROOT's geometry, each node has a matrix that encodes the
      // rotation, translation, and scale of each volume. For the
      // GRAMS geometry, in which everything is at right angles, only
      // the translation may seem relevant, but again we're trying to
      // write a general routine instead of one that just works for
      // the anode.
      TGeoHMatrix managerMatrix = *(volume->GetGeoManager()->GetCurrentMatrix());

      if (debug) {
	auto translation = managerMatrix.GetTranslation();
	std::cout << "manager translation = (" 
		  << translation[0] << ","
		  << translation[1] << ","
		  << translation[2] << ")"
		  << std::endl;
	managerMatrix.Print();
      }

      // And now the fun part: Use the matrix to translate the bottom
      // center point to the global (world) coordinate system.
      double master[3];
      managerMatrix.LocalToMaster(bottomCenter,master);

      if (debug) {
	std::cout << "master coords = (" 
		  << master[0] << ","
		  << master[1] << ","
		  << master[2] << ")"
		  << std::endl;
      }

      // Assuming that the search was successful, the global
      // z-coordinate of the bottom of the anode will be in
      // master[2]. As of Dec-2021, that number will be... (drum
      // roll...) zero! Or close to it. That's because the coordinate
      // system was designed so that z=0 is defined by the edge of the
      // anode, as it is in LArSoft.

      // Exit the iterator loop if the search was successful. 
      break;
    } // search successful
  } // geometry iterator loop

  // We've done whatever we're going to do. Restore the TGeoManager to
  // the volume it used before we did our search.
  startVol->GetGeoManager()->cd( managerPath.Data() );

  if (debug) {
    std::cout << "Return to " 
	      << startVol->GetGeoManager()->GetCurrentVolume()->GetName()
	      << std::endl;
  }
}
