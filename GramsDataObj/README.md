# Data Objects

A "data object" is a C++ type (class, struct, etc.) that is written as
a Branch (column) of a
[TTree](https://root.cern.ch/doc/master/classTTree.html) (n-tuple) in
a ROOT file.

The data objects are defined as header files in [the include/
directory](./include). If any implementations are needed, they're
either in `.icc` files in the `include/` directory or in `.cc` files
in [the src/ directory](./src).

Because of the way [ROOT dictionary
generation](https://root.cern/manual/io_custom_classes/) works, the
single [Linkdef.hh](./include/LinkDef.hh) file must include the
necessary `#pragma` definitions for every data object. If you add a
new data object, be sure to edit this file.

## General concepts

Each of the main TTrees produced by the analysis programs have the following properties:

### Event ID

The [EventID](./include/EventID.h) encapsulates what, in many
experiments, is simply the run and event number. However, in a balloon
or satellite experiment, it may be that there are different methods
for assigning an event ID; e.g., UTC time.

As a precaution (and also to saving on typing `if (run == N and event
== M)`), the `EventID` class is used instead. You can sort on
`EventID` or test it for equality, without having to modify the code
if there's a switch from "run/event" to distinguish events.

### Friend Trees

Each tree has one row for each value of EventID. They're all in sync
row-for-row; if row 2345 in TTree `gramsg4` refers to a given event,
row 2345 in TTree `DetSim` refers to the same event.

The standard ROOT method for working with multiple TTrees in this
fashion is to use [friend
trees](https://root.cern/manual/trees/#widening-a-ttree-through-friends). This
is a way of "adding columns" to a tree without modifying the original
file.

For example, consider tree `gramsg4` in file `gramsg4.root` and tree
`DetSim` in file `gramsdetsim.root`. To process through both trees at
once:

```c++
   // Open the files and trees.
   auto myFile = TFile::Open("gramsg4.root");
   auto tree = myFile->Get<TTree>("gramsg4");

   auto myFriendFile = TFile::Open("gramsdetsim.root");
   auto friendTree = myFriendFile->Get<TTree>("DetSim");

   // Declare that the DetSim tree is a friend to the gramsg4 tree.
   tree->AddFriend(friendTree);

   // Set up branches. Note that just "Clusters" would be fine instead
   // of "DetSim.Clusters" if there were no other columns named
   // "Clusters".
   auto mcLArHits = new grams::MCLArHits();
   tree->SetBranchAddress("LArHits", &mcLArHits);
   auto clusters = new grams::Clusters();
   tree->SetBranchAddress("DetSim.Clusters", &clusters);

   // Loop over the rows in the combined trees.
     for (int iEntry = 0; tree->LoadTree(iEntry) >= 0; ++iEntry) {
      // Load the data for the given tree entry
      tree->GetEntry(iEntry);

      // Do whatever with mcLArHits and clusters.
   }
```

### Tree Index

Each TTree has an
[index](https://root.cern/doc/v608/classTTreeIndex.html#a02300b69681c45b9e20503ad1d86d8c8)
based on the [EventID](./include/EventID.h). When processing a TTree,
typically one reads the rows (entries) sequentially. For example, to
read the data product [MCTrackList](./include/MCTrackList.h), one
might do:

```c++
 // Define the input file.
  auto input = new TFile("gramsg4.root");

  // Define the which TTree to read.
  TTreeReader myReader("gramsg4", input);

  // Define which variable(s) we'll read. This behaves like a pointer;
  // in the code we'll use "*tracklist". 
  TTreeReaderValue<grams::MCTrackList> tracklist(myReader, "TrackInfo");

  // For each row in the TTree:
  while (myReader.Next()) {

    // ... do something with *tracklist ...

  }
```

However, if you want to read a specific row in the TTree, you can use
the index. For these trees, the index is defined using
`grams::EventID::Index()`. For example, if you know that you want to
look at the specific event "Run=0, Event=1234", instead of looping
with `myReader.Next()` as in the above example, you could do:

```c++
  // Define the input file.
  auto input = new TFile("gramsg4.root");

  // Define the input tree and branch:
  TTree* myTree = input->Get<TTree>("gramsg4");;
  auto tracklist = new grams::MCTrackList();
  myTree->SetBranchAddress("TrackInfo",&tracklist);

  // Select a particular row:
  grams::EventID myEvent(0,1234);
  myTree->GetEntryWithIndex( myEvent.Index() );

  // ... at this point, (*tracklist) is the list of tracks for run=0, event=1234
```
