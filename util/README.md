# GramsG4

## Directory and Namespace `util`

This directory contains modules that I find generally useful in the
programs I write. 

### Options - parse XML file and command line

*Although you'll see the text `gramsg4` below, the Options class is generic and can work with any program. Just substitute the program's name for `gramsg4`.*

#### Format of `options.xml`

If you look at the file `options.xml`, what you'll first notice that is that within the global `<parameters>` tag there are
more than one sections. It looks something like this:

```
<parameters>
  <global>
    <option [...] />
    <option [...] />
  </global>

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

The idea is that this same options XML file might eventually be used
by more than one program in an analysis. The `<global>` section
contains parameters that might apply to any program. Individual
programs will have their options in their own sections. For the
purpose of these examples, we're using the name `gramsg4`. If you
created a program with the name `myanalysis`, within the options XML
file you'd have:

```
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

   type  - string/bool/flag/integer/double
   
   desc  - optional; brief description of the option (keep it 
           less than 20 characters); used in the --help|-h message
```

##### Defining new options

Note that the options in this XML file can be overridden by command-line options.
However, the contents of this file _define_ those command-line options. For example, assume 
this line appears in the XML file:

```  
  <option name="energyCut" value="12.5" type="double" desc="muon energy cut [MeV]"/>
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

If a job option has `type="boolean"`, then it's a feature that can be
turned on or off. Possble values are `true`, `false`, `on`, `off`,
`0`, `1`.

```  
  <option name="recombination" type="boolean" value="on" desc="turn recombination on/off"/>
```  

This is different from a job option that has `type="flag"`. Then on
the command line the options takes no arguments; either it's there or
it isn't. For example, if this is in the XML file:

```  
  <option name="makeHistograms" type="flag" desc="make my special hists"/>
```  

then you could do this on the command line:

 
    ./gramsg4 --makeHistograms --energyCut 123.45                      

##### Abbreviating options

You can define one-character short options:

```
  <option name="energyCut" short="e" value="12.5" type="double" />
```
Then you can do:

    ./gramsg4 -e 123.45 

Be careful not to overuse the short options, since they can make the
command line harder to understand. 

If you duplicate the short character between different options the behavior is
unpredictable. However, case is significant; e.g., you can do this:

```
  <option name="energyMin" short="e" value="12.5" type="double" desc="min pion energy [MeV]"/>
  <option name="energyMax" short="E" value="125.0" type="double" desc="max pion energy [Mev]"/>
```  

The usual UNIX shell flexibility is available with these options. Assume this line is in 
the options XML file. 

```
    <option name="nthreads" short="t" value="0" type="integer" desc="number of threads"/>
```

Then all of the following are equivalent:

    ./gramsg4 --nthreads 5
    ./gramsg4 --nthreads=5
    ./gramsg4 -t 5
    ./gramsg4 -t5

### Accessing options from within your program

Just having an option defined in the XML file is not enough.
You need the programming to do something with that option. Typically you'd initiate the parsing of the options XML file and the command line in your program's `main` routine. 

The first two arguments to `util::Options::ParseOptions` are the standard C++ arguments to
the main routine; the contents of the second argument (`argv` in the example) will be altered
during the process. The third argument must
agree with a single tag-block within the XML file; as noted above we use
`gramsg4` here as an example. 
```
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

After that one-time initialization in your main routine, you can access the value of a given option from any method:

```
  #include "Options.h" 
  
  // ...
  
  std::string optionValue; /* ... or int or double or bool ... */
  auto success = util::Options::GetInstance()->GetOption("option-name",optionValue);
  if (success) { ... do whatever with optionValue ... }
  else { there is no option with name "option-name" that is of the type of optionValue }
```  

For example, assume there's a double-precision option defined with the name "energyCut"
in the options XML File:

```  
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

### Implementing the `-h/--help` option

The method `util::Options::PrintHelp()` can be used to implement the `-h` and `--help` options for your program:
```
  // Check for help message.
  bool help;
  options->GetOption("help",help);
  if (help) {
    options->PrintHelp();
    exit(EXIT_SUCCESS);
  }
```

This will display all the relevant options in the XML file, along with their descriptions.

For example, as of 17-Jul-2021:
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
    [ --noscint ]                       # turn off scintillation
    [ -t | --nthreads <number of threads> ]
    [ --options <XML file of options> ] 
    [ -o | --outputfile <output file> ] 
    [ -p | --physicslist <physics list> ]
    [ --rngdir <rng save/restore directory> ]
    [ --rngperevent <rng save per event> ]
    [ --rngrestorefile <restore rng from file> ]
    [ -s | --rngseed <random number seed> ]
    [ -l | --showphysicslists ]         # show physics lists then exit
    [ --ui ]                            # start UI session
    [ --uimacrofile <G4 macro file for UI> ]
    [ -v | --verbose ]                  # display details

See options.xml for details.

```

### Other `Options` methods.

#### Displaying a table of all the options

`util::Options::PrintOptions()` will print all the options and their values as a text table. 
This is handy for debugging. In the case of `gramsg4`, the `PrintOptions()` method is called
if the `--verbose` or `-v` option is turned on:

In the code:

```
  bool verbose;
  options->GetOption("verbose",verbose);
  if (verbose) {
    // Display all program options and other details.
    options->PrintOptions();
    // Print other program details.
  }

```

On 25-Nov-2021, the output from `./gramsg4 -v` included:

```
20 options:
Option            short  value                            type       desc                
------            -----  -----                            ----       ----                
debug               d    false                            flag       
gdmlfile            g    grams.gdml                       string     input GDML detector desc
gdmlout                                                   string     write parsed GDML to this file
help                h    false                            flag       show help then exit
inputgen            i                                     string     input generator events
larstepsize              0.020000                         double     LAr TPC step size
macrofile           m    mac/batch.mac                    string     G4 macro file
nthreads            t    0                                integer    number of threads
options                  options.xml                      string     XML file of options
outputfile          o    gramsg4                          string     output file
physicslist         p    FTFP_BERT_LIV+OPTICAL+STEPLIMIT  string     physics list
rngdir                                                    string     rng save/restore directory
rngperevent              0                                integer    rng save per event
rngrestorefile                                            string     restore rng from file
rngseed             s    -1                               integer    random number seed
scint                    true                             bool       turn on scintillation
showphysicslists    l    false                            flag       show physics lists then exit
ui                       false                            flag       start UI session
uimacrofile              mac/vis-menus.mac                string     G4 macro file for UI
verbose             v    true                             flag       display details
```

#### Going through options one-by-one

There are times when it's useful to "iterate" through the internal table of all available options;
for example, to save the options in an ntuple for later reference. The following methods are available:

```
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
```