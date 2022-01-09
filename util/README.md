# Directory and Namespace `util`

This directory contains modules that I find generally useful in the
C++ programs I write. 

- [Directory and Namespace `util`](#directory-and-namespace--util-)
  * [Options - parse XML file and command line](#options---parse-xml-file-and-command-line)
    + [Format of `options.xml`](#format-of--optionsxml-)
      - [Defining new options](#defining-new-options)
        * [Boolean values](#boolean-values)
        * [Flags](#flags)
        * [Vectors](#vectors)
      - [Abbreviating options](#abbreviating-options)
      - [Overriding &lt;global&gt;](#overriding--lt-global-gt-)
    + [Accessing options from within your program](#accessing-options-from-within-your-program)
    + [Implementing the `-h/--help` option](#implementing-the---h---help--option)
    + [Other `Options` methods](#other--options--methods)
      - [Displaying a table of all the options](#displaying-a-table-of-all-the-options)
      - [Going through options one-by-one](#going-through-options-one-by-one)
      - [Saving options to a ROOT file](#saving-options-to-a-root-file)
      - [Restoring options from a ROOT file](#restoring-options-from-a-root-file)

<small><i><a href='http://ecotrust-canada.github.io/markdown-toc/'>Table of contents generated with markdown-toc</a></i></small>


## Options - parse XML file and command line

*Although you'll see the text `gramsg4` below, the Options class is generic and can work with any program. Just substitute the program's name for `gramsg4`.*

### Format of `options.xml`

If you look at the file [`options.xml`](../options.xml), what you'll first notice that is that within the `<parameters>` tag there is
more than one section. It looks something like this:

```XML 
<parameters>
  <global>
    <option [...] />
    <option [...] />
  </global>

  <gramssky>
    <option [...] />
    <option [...] />
  </gramssky>

  <gramsg4>
    <option [...] />
    <option [...] />
  </gramsg4>

  <gramsdetsim>
    <option [...] />
    <option [...] />
  </gramsdetsim>
</parameters>
```

The idea is that this same options XML file can be used
by more than one program in an analysis. The `<global>` section
contains parameters that would apply to any program. Individual
programs will have their options in their own sections. For the
purpose of these examples, we're using the name `gramsg4`. If you
created a program with the name `myanalysis`, in the options XML
file you'd have:

```XML
  <myanalysis>
    <option [...] />
    <option [...] />
  </myanalysis>
```

Within these sections there are `option` tags:

```
Anatomy of <option> tag: 

   name  - used on the command line, and by the program
           to fetch the value of the option
           
   short - optional one-character abbreviation for
           a short command-line option
           
   value - the number/text/bool passed to the program;
           can be overridden on the command line

   type  - string/bool/flag/integer/double/vector
   
   desc  - optional; brief description of the option (keep it 
           less than 20 characters); used in the --help|-h message
```

#### Defining new options

Note that the options in this XML file can be overridden by command-line options.
However, the contents of this file _define_ those command-line options. For example, assume 
this line appears in the XML file:

```XML  
  <option name="energyCut" value="12.5" type="double" desc="muon energy cut [MeV]" />
```

This means that you can set `energyCut` by either editing the XML file, or by using the
option on the command line; e.g.:

    ./gramsg4 --energyCut 15.6

The default name and location of the XML file is "options.xml" in the
current directory. You can override this on the command line; e.g.:

    ./gramsg4 myOptions.xml  

If you're going to use that format, the XML file name must be the
first argument. You can also supply the XML file name via the
`--options` option, e.g.:

    ./gramsg4 --options myWorkDirectory/myOptions.xml

Of course, you can mix and mash to your heart's content:


    ./gramsg4 --energyCut 199.9 --options myEnergyStudyOptions.xml

##### Boolean values

If a job option has `type="boolean"`, then it's a feature that can be
turned on or off. Possible values are `true`, `false`, `on`, `off`,
`0`, `1`.

For example:

```XML  
  <option name="recombination" type="boolean" value="on" desc="turn recombination on/off" />
```  

##### Flags

An option that has `type="flag"` is different from a boolean. A flag takes no arguments on
the command line; either it's there or
it isn't. Examples of a flag are `--verbose` and `--help`.

For example, if this is in the XML file:

```XML  
  <option name="makeHistograms" type="flag" desc="make my special hists"/>
```  

then you could do this on the command line:
 
    ./gramsg4 --makeHistograms --energyCut 123.45
    
In this example, if `--makeHistograms` is present on the command line, the value of the option is `true`; if `--makeHistograms` is not on the command line, the value of the option is `false`.

##### Vectors

As far as Options are concerned, a "vector" is a sequence or tuple of numbers. Here's an example:

    <option name="direction" value="(0,0,1)" type="vector" desc="initial direction"/>

The number format is fairly flexible. 
Any characters that are not part of a numeric format will be ignored. 
All of the following are equivalent:
```XML
    <option name="direction" value="(0.0,0.0,1.0)" type="vector" desc="initial direction"/>
    <option name="direction" value="<0;0;1>" type="vector" desc="initial direction"/>
    <option name="direction" value="[0E20 0 10E-1]" type="vector" desc="initial direction"/>
    <option name="direction" value="+0 -0 1" type="vector" desc="initial direction"/>
    <option name="direction" value="(0nowisthewinterofourdiscontent0,1)" type="vector" desc="initial direction"/>
    <option name="direction" value=".0 .0 .1e1" type="vector" desc="initial direction"/>
```
If you are going to supply a vector on the command line, you'll have
to enclose it in quotes. For example:

    ./gramssky --direction "(0,0.707,-0.707)"

#### Abbreviating options

You can define one-character short options:

```XML
  <option name="energyCut" short="e" value="12.5" type="double" />
```
Then you can do:

    ./gramsg4 -e 123.45 

Be careful not to overuse the short options, since they can make the
command line harder to understand. 

If you duplicate the short character between different options the behavior is
unpredictable. However, case is significant; e.g., you can do this:

```XML
  <option name="energyMin" short="e" value="12.5" type="double" desc="min pion energy [MeV]"/>
  <option name="energyMax" short="E" value="125.0" type="double" desc="max pion energy [Mev]"/>
```  

The usual UNIX shell flexibility is available with these options. Assume these lines are in 
the options XML file. 

```XML
    <option name="verbose" short="v" type="flag" desc="display details"/>
    <option name="nthreads" short="t" value="0" type="integer" desc="number of threads"/>
```

Then all of the following are equivalent:

    ./gramsg4 --nthreads 5 --verbose
    ./gramsg4 -v --nthreads=5
    ./gramsg4 -t 5 --verbose
    ./gramsg4 -vt5

#### Overriding &lt;global&gt;

Suppose you had something like this in the options XML file:

```XML
<parameters>
  <global>
    <option name="myoption" value="value1" ... />
  </global>

  <myprogram>
    <option name="myoption" value="value2" ... />
  </myprogram>
</parameters>
```

Then for every other program that uses the Options class, the value of `myoption` would be `value1`. But for the program `myprogram`, the value of `myoption` would be `value2`.

This is not a good practice. It's probably better to use the command line for this sort of override; e.g.,
```
./myprogram --myoption=value2
```

### Accessing options from within your program

Just having an option defined in the XML file is not enough.
You need the programming to do something with that option. Typically you'd initiate the parsing of the options XML file and the command line by invoking `ParseOptions` in your program's `main` routine. 

The three arguments to `util::Options::ParseOptions` are:
   1. The number of arguments on the command line; normally that is the first argument to the main routine (`argc`). 
   2. An array of C-style character strings (or type char**) that contains the arguments on the command line; normally this is the second argument to the main routine (`argv`). The contents of this array will be altered by `ParseOptions`. 
   3. The third argument can be one of the following:
      - A character string. This should match a tag-block of the same name in the XML file. The examples below use `gramsg4` in order to select the tag block `<gramsg4> ... <\gramsg4>`. 
      - Omitted. In this case, the name of the executing program (in `argv[0]`) will be used to search for a matching tag-block within the XML file. Any path specifications for the program will be omitted in searching for a tag block; e.g., if you're running `~/grams/GramsSim-work/bin/gramsdetsim` then `ParseOptions` will look for a tag block beginning with `<gramsdetsim>`.
      - The string `"ALL"`. In that case, all the tag-blocks will be read in and used. Note that if multiple tag blocks have options with the same `name` attribute, then last one in the file will be used, overriding the ones above it. 
         
As noted above, here we use `gramsg4` as an example:

```C++
#include "Options.h"
#include <iostream> 
// ...

int main( int argc, char** argv ) {

// ...

//  Parse the contents of the options XML file, with overrides
//  from the command line. 

    auto result = options->ParseOptions(argc, argv, "gramsg4");

    // Abort if we couldn't parse the job options.
    if (result) std::cout << "ParseOptions succeeded" << std::endl;
    else {
        std::cerr << "ABORT: File " << __FILE__ << " Line " << __LINE__ << " " 
                << std::endl 
                << "Aborting job due to failure to parse options"
                << std::endl;
        exit(EXIT_FAILURE);
    }
}
```

After that one-time initialization in your `main` routine, you can access the value of a given option from any method:

```C++
#include "Options.h" 
  
  // ...
  
  std::string optionValue; /* ... or int or double or bool ... */
  auto success = util::Options::GetInstance()->GetOption("option-name",optionValue);
  if (success) { ... do whatever with optionValue ... }
  else { there is no option with name "option-name" that is of the type of optionValue }
```  

For example, assume there's a double-precision option defined with the name "energyCut"
in the options XML File:

```C++
#include "Options.h" 
#include <iostream> 
  // ...
  // Save the pointer to the Options object.
  auto options = util::Options::GetInstance();
  // ...
  double myCut;
  auto success = options->GetOption("energyCut",myCut);
  if (success) { 
     ... do whatever with myCut ... 
  }
  else {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " 
           << std::endl 
           << "There is no floating-point option defined for 'energyCut'"
           << std::endl;
  }

```

To fetch a vector, use `std::vector<double>`; e.g.,
```C++
#include "Options.h" 
#include "TVector3.h"
#include <iostream> 
#include <vector>
// ...
  auto options = util::Options::GetInstance();
  // ...
  std::vector<double> myVector;
  options->GetOption("direction",myVector);
  if ( myVector.size() == 3 ) {
     TVector3 direction(myVector[0], myVector[1], myVector[2]);
     // ...
  }
  else {
     std::cerr << "A direction vector needs exactly three values!" << std::endl;
  }     
```

### Implementing the `-h/--help` option

The method `util::Options::PrintHelp()` can be used to implement the `-h` and `--help` options for your program:
```C++
  // Check for help message.
  bool help;
  options->GetOption("help",help);
  if (help) {
    options->PrintHelp();
    exit(EXIT_SUCCESS);
  }
```

This will display all the relevant options in the XML file, along with their descriptions.

For example, using `GramsSim/GramsG4` and `GramsSim/options.xml` as of 2-Dec-2021:
```
$ ./gramsg4 -h
Usage:
  ./gramsg4
    [ -d | --debug ]                    
    [ -g | --gdmlfile <input GDML detector desc> ]
    [ --gdmlout <write parsed GDML to this file> ]
    [ -h | --help ]                     # show help then exit
    [ -i | --inputgen <input generator events> ]
    [ --larstepsize <LAr TPC step size> ]
    [ -m | --macrofile <G4 macro file> ]
    [ -t | --nthreads <number of threads> ]
    [ --options <XML file of options> ] 
    [ -o | --outputfile <output file> ] 
    [ -p | --physicslist <physics list> ]
    [ --rngdir <rng save/restore directory> ]
    [ --rngperevent <rng save per event> ]
    [ --rngrestorefile <restore rng from file> ]
    [ -s | --rngseed <random number seed> ]
    [ --scint <turn on/off scintillation> ]
    [ -l | --showphysicslists ]         # show physics lists then exit
    [ --ui ]                            # start UI session
    [ --uimacrofile <G4 macro file for UI> ]
    [ -v | --verbose ]                  # display details

See options.xml for details.

```

### Other `Options` methods

#### Displaying a table of all the options

`util::Options::PrintOptions()` will print all the options and their values as a text table. 
This is handy for debugging. In the case of `gramsg4`, the `PrintOptions()` method is called
if the `--verbose` or `-v` option is turned on:

In the code:

```C++
  bool verbose;
  options->GetOption("verbose",verbose);
  if (verbose) {
    // Display all program options and other details.
    options->PrintOptions();
    // Print other program details.
  }

```

On 2-Dec-2021, the output from `./gramsg4 -v` included:

```
20 options:
Option            short  value                            type       source        desc                
------            -----  -----                            ------     ------        ----                
debug               d    false                            flag       global        
gdmlfile            g    grams.gdml                       string     gramsg4       input GDML detector desc
gdmlout                                                   string     gramsg4       write parsed GDML to this file
help                h    false                            flag       global        show help then exit
inputgen            i                                     string     gramsg4       input generator events
larstepsize              0.020000                         double     gramsg4       LAr TPC step size
macrofile           m    mac/batch.mac                    string     gramsg4       G4 macro file
nthreads            t    0                                integer    gramsg4       number of threads
options                  options.xml                      string     global        XML file of options
outputfile          o    gramsg4                          string     gramsg4       output file
physicslist         p    FTFP_BERT_LIV+OPTICAL+STEPLIMIT  string     gramsg4       physics list
rngdir                                                    string     gramsg4       rng save/restore directory
rngperevent              0                                integer    gramsg4       rng save per event
rngrestorefile                                            string     gramsg4       restore rng from file
rngseed             s    -1                               integer    gramsg4       random number seed
scint                    true                             bool       gramsg4       turn on/off scintillation
showphysicslists    l    false                            flag       gramsg4       show physics lists then exit
ui                       false                            flag       gramsg4       start UI session
uimacrofile              mac/vis-menus.mac                string     gramsg4       G4 macro file for UI
verbose             v    true                             flag       Command Line  display details
```

Note that `Options` keeps track of which tag block was the source of a given option, or if the option came from the command line. 

#### Going through options one-by-one

There are times when it's useful to "iterate" through the internal table of all available options;
for example, to save the options in an ntuple for later reference. The following methods are available (these are the lines from [`Option.h`](include/Option.h)):

```C++
    /// Provide a way to access the "i-th" option stored by this
    /// class. Note that these routines are very inefficient (map
    /// iterators are not random-access) so don't use them inside
    /// frequently-executed loops and such.
    size_t NumberOfOptions() const;
    std::string GetOptionName( size_t i ) const;
    std::string GetOptionValue( size_t i ) const;
    std::string GetOptionType( size_t i ) const;
    std::string GetOptionBrief( size_t i ) const;
    std::string GetOptionDescription( size_t i ) const;
    std::string GetOptionSource( size_t i ) const;
```

#### Saving options to a ROOT file

The utility method `WriteOptions` can be used to write the option to an ntuple in an output file in [ROOT](https://root.cern.ch/) format. This lets you record the values used to run the program that generated that particular file.

```C++
#include "Options.h" // in util/ 
// ... call ParseOptions ...
// Define a ROOT output file, e.g.,:
auto output = TFile::Open("output-file-name.root","RECREATE")
// ...
options->WriteNtuple(output);
```

`WriteNtuple` can take a second argument, the name of the options ntuple. If you don't supply one, the default is `Options`.

#### Restoring options from a ROOT file

Suppose you have an ROOT file that was created by a program that had its options saved using the `WriteOptions` method described above. You'd like to rerun the program with those same options, perhaps with one or more options changed via the command line. Let's further suppose that, due to the complexities of file management over a long analysis, you've lost the original XML options file that generated the ROOT file. 

The `Options` class can automatically recognize ROOT files that are passed to the program in place of an XML file. For example:
```
./gramsdetsim gramsdetsim.root
```
or
```
./gramsdetsim -v --options gramsdetsim.root
```
The `ParseOptions` method will search the ROOT file for an ntuple with a name that contains the text `Options`. It will then populate its list of options from that ntuple, and accept any option overrides on the command line. For example:
```
./gramsdetsim -v --options gramsdetsim.root --rho 1.5
```
Take care! In this particular example, the default output file for `gramsdetsim` is `gramsdetsim.root`. So we're reading our options from the same file to which we're going to write our output; you've overridden the value of `rho` which will be written to the output file. To avoid unpredictable behaviors, you probably want to make sure the files from which you're reading options and to which you're writing output are different:
```
./gramsdetsim -v --options gramsdetsim.root --rho 1.5 --outputfile gramsdetsim-revised.root
```
