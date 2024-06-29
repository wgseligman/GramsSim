# Example GramsG4 macro files

_If you want a formatted (or easier-to-read) version of this file, scroll to the bottom of [`GramsSim/README.md`](../README.md) for instructions. If you're reading this on github, then it's already formatted._

To get you started, here are examples of how to set up event
generation and the interactive display for [GramsG4](../GramsG4). To use one of
these files, e.g., mac/test.mac:

    ./gramsg4 --macrofile mac/test.mac

See README.md in the parent directory for more information. 

`run.mac` - shoots 20 1-MeV gammas along the z-axis, y-axis, and x-axis.

`batch.mac` - shoots 1000 1-MeV gamma along the z-axis.

`sky.mac` - shoots 1000 gammas from an imaginary sphere surrounding
the detector (the 'sky') focused on the center of the TPC. The energy
spectrum comes from a histogram (see scripts/Hist2Text.C).

`crab.mac` - a crude approximation of gammas from the Crab Nebula
directed at the top of the GRAMS detector.

`crab-45.mac` - a crude approximation of gammas from the Crab Nebula
directed at the top of the GRAMS detector at 45-degree angle.

`debug-geom.mac` - run Geant4's geometry validation (takes a long time!)

`hepmc3.mac` - how to run the simulation using an input file of
generated events.

`vis-hepmc3.mac` - run the simulation with both an input events file
and visualization.

## Working with Geant4 visualization

To run an interactive or visualization job with, e.g., mac/vis.mac

    ./gramsg4 --ui --uimacrofile mac/vis.mac

Note that to use the interactive OGL visualizer, you use the combination `--ui` and `--uimacrofile`
options (and/or edit the `uimacrofile` option in `options.xml`). This is to allow rapid switching
between visualization and non-interactive running of the simulation.  

Geant4 offers quite a few options for event visualization. Here's a [description](https://www.slac.stanford.edu/xorg/geant4/tutorial/MC2015G4WS/Visualization.pdf) 
(PDF file) of the different options. As of Aug-2021, two example visualization methods are offered
in these macro files.

### OpenGL

With [OpenGL](https://en.wikipedia.org/wiki/OpenGL), Geant4 offers an interactive
display. Unfortunately, OpenGL is now considered obsolete by some OS manufacturers
(Apple in particular) which means that support can be spotty. As noted in the 
[`GramsSim/README.md`](../README.md) file, you'll need OpenGL libraries installed on the system that's
running `gramsg4`. If you're using a local computer to view the display (e.g., you're running
`gramsg4` on a server by connecting from a laptop), you may
also need to [setup OpenGL](https://twiki.nevis.columbia.edu/twiki/bin/view/Main/X11OnLaptops)
on your local machine. 

The sample OpenGL macro files:

`vis.mac` - setup for interactive display. Particles are aimed along
the negative z-direction randomly across the top face of the inner
detector.

`vis-sphere.mac` - another interactive display. The particles are
generated on the surface of an imaginary sphere surrounding the
detector (the 'sky') and are focused on the center of the TPC. The
energy spectrum comes from a histogram (see scripts/Hist2Text.C).

`outline.mac` - just shows an outline of the detector, and is colored
so that it's more suitable for inclusion in talks. Give this one some
extra time to load, since it's turning off the visibility for a number
of volumes.

`vis-menus.mac` - a fancy demonstration of how to switch commands via
menus at the top of the G4 visualizer. This macro file invokes the
`draw-axes.mac`, `angle-view.mac`, `{x,y,z}-views.mac`,
`energy-*.mac`, and `plane-*.mac` sub-files from within its menus.

### HepRep

[HepRep](https://www.slac.stanford.edu/~perl/heprep/) is a *non-interactive* method of
visualization, in the sense that you run a separate program to display events 
*after* you run `gramsg4`. It has the advantage
that you can view the information for tracks on the display; e.g., instead of just
seeing a track you can also view the PDG code and energy of the particle.

The disadvantages of HepRep compared to OpenGL:

   - It's harder to use. In particular, the user interface is more complex. 

   - It can't be used to control Geant4 interactively.
   
   - It writes the event information in HepRep format, which uses XML, with each event
   in its own file. As a result HepRep outputs can take up a lot of disk space.
   
To use HepRep, run `gramsg4` with appropriate macro commands; `heprep.mac` shows
an example. For example:

    ./gramsg4 --macrofile mac/heprep.mac

To visualize a `.heprep` file, you must install both 
[Java](https://java.com/en/download/help/download_options.html)
and the [HepRApp](https://www.slac.stanford.edu/~perl/HepRApp/) visualizer. To run
HepRApp:

    java -jar HepRApp.jar 
   
Once you've started HepRApp, select `Open Data`, navigate to the directory with your `.heprep`
files, and open `G4Data0.heprep`. The `Previous Event` and `Next Event` buttons will move through the
`G4Data<N>.heprep` files. 
    
While running HepRApp, there are two key items in its `Options` menu: 

   - `Label Control` - This affects which labels are included in the event display. Probably
   the two most useful are `IMag` (the track energy) and `PDG`; `ID` (G4 track ID) and
   `PID` (parent ID) may also be helpful. 
   
   - `Cut Control` - This allows you to set cuts on which tracks are displayed. For example, in the `IMag`
   line, you can set the Min to `20 MeV` to show only those tracks with more than 20 MeV. Note
   that you *must* include units in the cut. 
