# GramsG4

## Directory and Namespace `util`

This directory contains modules that I find generally useful in the
programs I write. 

### Options - parse XML file and command line

*Although you'll see the text `gramsg4` below, the Options class is generic and can work with any program. Just substitute the program's name for `gramsg4`.*

If you look at the file `options.xml`, what you'll first notice that is that within the global `<parameters>` tag there are
at least two sections. It looks something like this:

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
</parameters>
```

The idea is that this same options XML file might eventually be used by more programs
in an analysis. The `<global>` section contains parameters that will apply to more
than one program. Individual programs will have their options in their own sections. For the purpose of these examples, we're using the name `gramsg4`. If you created a program with the name `myanalysis`, within the options XML file you'd have:
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

   type  - string/bool/integer/double
   
   desc  - optional; brief description of the option (keep it 
           less than 20 characters); used in the --help|-h message
```

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

If a job option has `type="boolean"`, then on the command line it takes no
arguments; either it's there or it isn't. For example, if this is
in the XML file:

```  
  <option name="makeHistograms" value="false" type="bool" desc="make my special hists"/>
```  

then you could do this on the command line:

 
    ./gramsg4 --makeHistograms --energyCut 123.45                      


You can define one-character short options:

```
  <option name="energyCut" short="e" value="12.5" type="double" />
```
Then you can do:

    ./gramsg4 -e 123.45 

Be careful not to overuse the short options, since they can make the
command harder to understand. 

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

### Programming new options

Just having an option defined in the XML file is not enough.
You need the programming to do something with that option. First you need to initiate the parsing of the options XML file and the command line. 

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



