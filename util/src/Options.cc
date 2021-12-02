#include "Options.h"

// ROOT includes
#include "TROOT.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TTree.h"
#include "TKey.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

// C++ includes
#include <getopt.h>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <iterator>
#include <exception>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>

// XML parser
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax/HandlerBase.hpp>

namespace util {

  // Why don't we get this flag from the command line? Because this is
  // the routine that parses the command line! At this point in the
  // processing we don't know the state of the --debug option yet.
  const bool debug = false;

  bool Options::ParseOptions(int argc, char** argv, const std::string a_programName)
  {
    bool success = true;

    // Default XML file name.
    m_optionsFile = "options.xml";

    // Save the path of the running program.
    m_progPath = std::string( argv[0] );

    if (debug) {
      for ( int i = 0; i != argc; i++ ) {
	std::cout << "ParseOptions: argv[" << i << "]=" << argv[i] << std::endl;
      }
    }

    // If the program name argument is empty, then derive the likely
    // program tag from the end of the program path. For example, if
    // the program path is "~/GramsDetSim-work/gramsdetsim", then
    // assume the tag would be "gramsdetsim".
    std::string tagName(a_programName);
    if ( tagName.empty() ) {
      tagName = m_progPath.substr(m_progPath.find_last_of("/\\") + 1);
    }
    if (debug)
      std::cout << "Debug: tagName=" << tagName << std::endl;

    // Are there options on the command line?
    if (argc >= 2 ) {
      // Does the first one begin with a dash?
      if ( argv[1][0] != '-' ) {
	// No, so that first word must be the name of the XML file.
	m_optionsFile = std::string(argv[1]);
      }
      else {
	// The user could still have put "--options <filename>"
	// somewhere on the command line. We have to look for it. 
	for ( int i=0; i != argc; i++ ) {
	  if ( strncmp( argv[i], "--options", 8 ) == 0 ) {
	    // Found --options
	    std::string argument(argv[i]);
	    // Is there an '=' sign in this argument? There could be if
	    // it's "--options=myFile.xml", for example.
	    auto search = argument.find('=');
	    if ( search == std::string::npos ) {
	      // no '=', so the next word is the file name.
	      if ( i+1 < argc )
		m_optionsFile = std::string(argv[i+1]);
	      else {
		std::cerr << "ABORT: File " << __FILE__ << " Line " << __LINE__ << " " 
			  << std::endl
			  << "missing XML file name after '--options' " << std::endl;
		exit(EXIT_FAILURE);
	      }
	    }
	    else {
	      // The file name is the part after the '='
	      if ( search + 2 < argument.size() )
		m_optionsFile = argument.substr( search+1 );
	      else {
		std::cerr << "ABORT: File " << __FILE__ << " Line " << __LINE__ << " " 
			  << std::endl
			  << "missing XML file name after '--options=' " << std::endl;
		exit(EXIT_FAILURE);
	      }
	    } // argument contains '='
	    break;
	  } // found --options
	} // argv loop
      } // look for --options
    } // if there are command-line arguments
  
    if (debug) std::cout << "ParseOptions: m_optionsFile=" << m_optionsFile << std::endl;

    // If we don't suppress ROOT error messages, then the following
    // IsZombie test will display an unnecessary error message when
    // we're reading an XML file instead of a ROOT file.
    auto saveErrorLevel = gErrorIgnoreLevel;
    gErrorIgnoreLevel = 5000;

    // Is the options file actually a ROOT file?
    if ( TFile(m_optionsFile.c_str()).IsZombie() ) 
      {
	// It's not a ROOT file (the most probable case). XML parsing
	// goes here, filling m_options.
	auto parseSuccess = m_ParseXML(m_optionsFile, tagName);
	// Fold in whether the parsing worked with the overall success of this method.
	success = success && parseSuccess;
      }
    else
      {
	// It _is_ a ROOT file! Instead of parsing it as XML file,
	// we're going to fill m_options from a ROOT ntuple.
	std::cout << "ParseOptions: File '" << m_optionsFile 
		  << "' is a ROOT file." << std::endl;
	auto result = m_RootOptions(m_optionsFile);
	if ( ! result ) {
	  std::cerr << "ABORT: Invalid ROOT options file." << std::endl;
	  exit(EXIT_FAILURE);
	} 
      } // if ROOT file

    // Restore error level.
    gErrorIgnoreLevel = saveErrorLevel;
  
    // Override the contents of the options file with options on the command line. 
    // For more on getopt, along with global variables like "optarg", see
    // <https://www.informit.com/articles/article.aspx?p=175771>

    // If there are options to override:
    if ( ! m_options.empty() ) {
      // "option" is a struct defined in getopt.h
      std::vector<option> findOption;
      // Copy each entry in the options map into a structure needed for getopt_long.
      // First, set up some arguments so that getopt_long gets new memory locations
      // and arguments each time. 
      int result;
      std::string abbreviations = ":";
      int index = 0;
      // For the passing of the options to findOption to work correctly, I have
      // to make sure that each option name is in its own char* memory location that's
      // not going to change. Hence this array: up to 100 options that are up to
      // 50 characters long. 
      char cname[100][50];
      // For every entry in the options map...
      for ( auto option_iter = m_options.cbegin(); option_iter != m_options.cend(); ++option_iter )
	{
	  // Get the option's name.
	  auto name = (*option_iter).first;
	
	  // The abbreviation, if any:
	  auto brief = (*option_iter).second.brief;
 	
	  // Is an argument required? If so, set the appropriate flag
	  // for getopt_long.  Also, append to the abbreviations string
	  // that getopt_long needs for the processing of any short
	  // options; e.g., ":d:ef:g" means that the short options "-d",
	  // "-e", "-f", and "-g" can be used, and that options d and f
	  // both require arguments. Note that brief==0, assigned in the
	  // XML file parser, will be the value returned by getopt_long
	  // below if a short option is not specified.

	  auto required = required_argument; // defined in "getopt.h"
	  if ( (*option_iter).second.type == e_flag ) {
	    required = no_argument;
	    if (brief != 0) abbreviations.push_back(brief);
	  }
	  else {
	    if (brief != 0) {
	      abbreviations.push_back(brief);
	      abbreviations.push_back(':');
	    }
	  }

	  // Made sure the char* version of the option's name is in
	  // a memory location that will not change.
	  strcpy( cname[index], name.c_str() );
	
	  // Create an "option" struct (see getopt.h) for this option. 
	  findOption.push_back( {cname[index], required, 0, brief} );
	  index++;
	}

      // An "end-of-list" struct for getopt_long. 
      findOption.push_back( {0,0,0,0} );
    
      // Extract from the findOption vector an array of struct
      // (struct option*) required by getopt_long.
      auto findOptionData = findOption.data();
    
      if (debug) std::cout << "ParseOptions: abbreviations=" << abbreviations << std::endl;
    
      // Look through all the options on the command line:
      index = 0;
      optind = 0; // reset getopt_long
      while ((result = getopt_long(argc, argv, abbreviations.c_str(), findOptionData, &index)) != -1)
	{
	  // Test to see if an option was completely invalid for some
	  // reason. (Yes, we have to compare an int with a char!
	  // Welcome to getopt!)
	  if ( char(result) == '?' ) {
	    std::cerr << "WARNING: File " << __FILE__ << " Line " << __LINE__ << " " 
		      << std::endl
		      << "Option on command line not found in XML file " << std::endl;
	    success = false;
	    continue;
	  }
	  // Test to see if an option was missing.
	  if ( char(result) == ':' ) {
	    std::cerr << "WARNING: File " << __FILE__ << " Line " << __LINE__ << " " 
		      << std::endl
		      << "A command-line argument is missing a required option (not sure which) "
		      << std::endl;
	    success = false;
	    continue;
	  }

	  if (debug) {
	    std::cout << "ParseOptions: result=" << result 
		      << " (" << char(result) << ")" << std::endl;
	    std::cout << "ParseOptions: index=" << index << std::endl;
	    std::cout << "ParseOptions: findOption.size()=" << findOption.size() 
		      << " (but the last entry is NULL)" << std::endl;
	    for ( size_t i = 0; i < findOption.size() - 1; i++ ) {
	      std::cout << "ParseOptions: findOption(" << i << "): name=" << findOption.at(i).name
			<< " has_arg=" << findOption.at(i).has_arg 
			<< " val=" << findOption.at(i).val 
			<< " (" << char(findOption.at(i).val) << ")" << std::endl;
	    }	    
	  }

	  // We've found an option on the command line. Did it match on
	  // the short option? If so, we have to locate it in
	  // findOption, as the value of "index" from getopt_long will
	  // be irrevelant.
	  if ( result != 0 ) {
	    index = 0;
	    for ( auto i = findOption.cbegin(); i != findOption.cend(); i++, index++ ) {
	      if ( (*i).val == result ) break;
	    }
	  }
	  if (debug) std::cout << "ParseOptions: index=" << index << std::endl;

	  // We matched on a long option, or we've just set a value of
	  // "index" for the findOption vector. Locate the name in our
	  // map.
	  auto name = std::string(findOption.at(index).name);
	  auto search = m_options.find(name);
	  if ( search == m_options.end() )
	    {
	      // Something is seriously wrong. If we get here, 
	      // getopt found an option that is not in our map but
	      // was in the list of options that we told getopt
	      // to look for.
	      std::cerr << "WARNING: File " << __FILE__ << " Line " << __LINE__ << " " 
			<< std::endl
			<< "Inconsistency: option '" << name << "' was found by getopt"
			<< " but is not present in the options map" << std::endl;
	      success = false;
	      break;
	    }

	  if (debug) {
	    std::cout << "ParseOptions: name=" << name << std::endl;
	    std::cout << "ParseOptions: type=" << (*search).second.type << std::endl;
	    if (optarg != NULL) std::cout << "ParseOptions: optarg=" << optarg << std::endl;
	  }

	  auto type = (*search).second.type;
	  // Did it require an argument that the user omitted?
	  // (Yes, we have to compare an integer with a char!)
	  if ( type != e_flag  &&  result == ':' ) {
	    std::cerr << "WARNING: File " << __FILE__ << " Line " << __LINE__ << " " 
		      << std::endl
		      << "There is no argument to --" << name
		      << " on the command line"
		      << std::endl << "   Using the value '" << (*search).second.value
		      << "'" << std::endl;
	    success = false;
	    break;
	  }
	  // The source of this option was the command line.
	  m_options[name].source = "Command Line";

	  // Convert the argument based on its type... with lots of 
	  // error-detection, because who knows what the users are doing?
	  switch (type)
	    {
	    case e_flag:
	      m_options[name].value = "1";
	      break;
	    case e_boolean:
	    case e_string:
	      m_options[name].value = optarg;
	      break;
	    case e_double:
	      try {
		// "stod" is "string to double".
		double value = std::stod(optarg);
		m_options[name].value = std::to_string(value);
	      } catch ( std::invalid_argument& e ) {
		std::cerr << "WARNING: File " << __FILE__ << " Line " << __LINE__ << " " 
			  << std::endl
			  << "Option '" << name << "' - could not convert '" << optarg
			  << "' to a floating-point value"
			  << std::endl << "   Using the value '" << m_options[name].value
			  << "'" << std::endl;
		success = false;
	      }
	      break;
	    case e_integer:
	      try {
		// "stoi" is "string to integer".
		int value = std::stoi(optarg);
		m_options[name].value = std::to_string(value);
	      } catch ( std::invalid_argument& e ) {
		std::cerr << "WARNING: File " << __FILE__ << " Line " << __LINE__ << " " 
			  << std::endl
			  << "Option '" << name << "' - could not convert '" << optarg
			  << "' to an integer value"
			  << std::endl << "   Using the value '" << m_options[name].value
			  << "'" << std::endl;
		success = false;
	      }
	      break;
	    default:
	      std::cerr << "WARNING: File " << __FILE__ << " Line " << __LINE__ << " " 
			<< std::endl
			<< "Option '" << name << "' has an invalid type of '"
			<< type << "' in the options map."
			<< std::endl << "   Option will be ignored. " << std::endl;
	      success = false;
	      break;
	    } // switch
	} // getopt_long loop
    } // if there are any options in the map

    if (debug) PrintOptions();

    return success;
  }

  // For the getters, it would perhaps be possible to create a
  // template.  But the resulting code would be even more difficult to
  // read, and there are only three types of values that matter for
  // this work (numbers, bools, strings). If I'm wrong, feel free to
  // template the heck out of this method!

  bool Options::GetOption(const std::string name, double& value) const
  {
    auto result = m_options.find( name );
    if ( result != m_options.end() )
      {
	if ( (*result).second.type == e_double ) {
	  // "stod" is "string to double"
	  value = std::stod((*result).second.value);
	  return true;
	}
      }
    return false;
  }

  bool Options::GetOption(const std::string name, int& value) const
  {
    auto result = m_options.find( name );
    if ( result != m_options.end() )
      {
	if ( (*result).second.type == e_integer ) {
	  // "stoi" is "string to integer"
	  value = std::stoi((*result).second.value);
	  return true;
	}
      }
    return false;
  }

  bool Options::GetOption(const std::string name, bool& value) const
  {
    auto result = m_options.find( name );
    if ( result != m_options.end() )
      {
	value = false;
	if ( ( (*result).second.type == e_boolean ) || 
	     ( (*result).second.type == e_flag    ) ) {
	  if ( (*result).second.value == "1" ) value = true;
	  return true;
	}
      }
    return false;
  }

  bool Options::GetOption(const std::string name, std::string& value) const
  {
    auto result = m_options.find( name );
    if ( result != m_options.end() )
      {
	if ( (*result).second.type == e_string ) {
	  value = (*result).second.value;
	  return true;
	}
      }
    return false;
  }

  void Options::PrintOptions() const
  {
    // Neatly-formatted columnar output is not one of C++ strengths (I
    // still remember COBOL!) but let's see what we can do. To make
    // things look neat, find the maximum field widths.
    size_t nameWidth = 0;
    size_t valueWidth = 0;
    size_t sourceWidth = 0;
    for ( auto iter = m_options.cbegin(); iter != m_options.cend(); ++iter )
      {
	nameWidth = std::max( nameWidth, (*iter).first.size() );
	valueWidth = std::max( valueWidth, (*iter).second.value.size() );
	sourceWidth = std::max( sourceWidth, (*iter).second.source.size() );
      }
    valueWidth += 2;

    std::cout << std::endl << m_options.size() << " options:" << std::endl;
    std::cout << std::left
	      << std::setw(nameWidth) << "Option" 
	      << std::right << "  "
	      << std::setw(5) << "short"
	      << std::left << "  "
	      << std::setw(valueWidth) << "value" 
	      << std::left
	      << std::setw(9) << "type" 
	      << std::left << "  "
	      << std::setw(sourceWidth) << "source" 
	      << std::left << "  "
	      << std::setw(20) << "desc" 
	      << std::endl;
    std::cout << std::left
	      << std::setw(nameWidth) << "------" 
	      << std::right << "  "
	      << std::setw(5) << "-----"
	      << std::left << "  "
	      << std::setw(valueWidth) << "-----" 
	      << std::left
	      << std::setw(9) << "------" 
	      << std::left << "  "
	      << std::setw(sourceWidth) << "------" 
	      << std::left << "  "
	      << std::setw(20) << "----" 
	      << std::endl;

    for ( auto iter = m_options.cbegin(); iter != m_options.cend(); ++iter )
      {
	std::cout << std::left
		  << std::setw(nameWidth) << (*iter).first;

	auto brief = (*iter).second.brief;
	std::cout << std::right << "    ";
	if ( brief == 0 ) std::cout << "   ";
	else std::cout <<  brief << "  ";

	std::cout << std::left << "  "
		  << std::setw(valueWidth);
	if ( ( (*iter).second.type == e_boolean ) ||
	     ( (*iter).second.type == e_flag    ) )
	  {
	    if ( (*iter).second.value == "1" ) 
	      std::cout << "true";
	    else 
	      std::cout << "false";
	  }
	else std::cout << (*iter).second.value;

	std::cout << std::left << std::setw(9);
	switch ( (*iter).second.type )
	  {
	  case e_flag:
	    std::cout << "flag";
	    break;
	  case e_boolean:
	    std::cout << "bool";
	    break;
	  case e_integer:
	    std::cout << "integer";
	    break;
	  case e_double:
	    std::cout << "double";
	    break;
	  case e_string:
	    std::cout << "string";
	    break;
	  default:
	    std::cout << "unknown";
	  }

	std::cout << std::left << "  "
		  << std::setw(sourceWidth);
	std::cout << (*iter).second.source;

	std::cout << "  " << std::left << (*iter).second.desc;
	std::cout << std::endl;
      }
    std::cout << std::right << std::endl;
  }


  void Options::PrintHelp() const
  {
    std::cerr << "Usage:" << std::endl;
    std::cerr << "  " << m_progPath << std::endl;
    for ( auto iter = m_options.cbegin(); iter != m_options.cend(); ++iter )
      {
	// To line up the descriptions of the boolean options,
	// first format the rest of the line, then write it
	// with a length limit.
	std::ostringstream text;
	text << "    [ "; 
      
	auto brief = (*iter).second.brief;
	if ( brief != 0 )
	  text <<  "-" << brief << " | ";
      
	auto desc = (*iter).second.desc;
	auto flag = (*iter).second.type == e_flag;
	text << "--" << (*iter).first;
	if ( ! flag ) {
	  text << " <" << desc << ">";
	}
	text << " ]";

	std::cerr << std::left << std::setw(40);
	std::cerr << text.str();

	// Now put in the flag comment, if any.
	if ( flag  &&  ! desc.empty()) {
	  std::cerr << "# " << desc;
	}
	std::cerr << std::endl;
      }
    std::cerr << std::right << std::endl;
    std::cerr << "See " << m_optionsFile << " for details." << std::endl;
    std::cerr << std::endl;
  }


  // The "getters" to access individual options in the order they're
  // stored in the map. Note that everything is returned as a
  // std::string.

  size_t Options::NumberOfOptions() const { 
    return m_options.size(); 
  }

  std::string Options::GetOptionName( size_t i ) const {
    auto j = m_options.cbegin(); 
    std::advance(j,i); 
    return (*j).first; 
  }

  std::string Options::GetOptionValue( size_t i ) const {
    auto j = m_options.cbegin(); 
    std::advance(j,i); 
    return (*j).second.value; 
  }

  std::string Options::GetOptionType( size_t i ) const {
    auto j = m_options.cbegin(); 
    std::advance(j,i); 
    auto type =  (*j).second.type;
 
   switch (type) {
     case e_string:
      return "string";
      break;
    case e_double:
      return "double";
      break;
    case e_integer:
      return "integer";
      break;
    case e_boolean:
      return "boolean";
      break;
    case e_flag:
      return "flag";
      break;
    default:
      return "";
    }
  }

  std::string Options::GetOptionBrief( size_t i ) const {
    auto j = m_options.cbegin(); 
    std::advance(j,i); 
    // We store "brief" as a char, which requires a bit of care to
    // convert to std::string. The simplest way is to use the
    // std::string(n,c), which creates a string of n copies of char c.
    std::string brief( 1, (*j).second.brief );
    return brief;
  }

  std::string Options::GetOptionDescription( size_t i ) const {
    auto j = m_options.cbegin(); 
    std::advance(j,i); 
    return (*j).second.desc; 
  }

  std::string Options::GetOptionSource( size_t i ) const {
    auto j = m_options.cbegin(); 
    std::advance(j,i); 
    return (*j).second.source; 
  }

  bool Options::WriteNtuple( TDirectory* a_output, std::string a_ntupleName ) {
    // Start by assuming we'll succeed.
    bool success = true;

    // Almost certainly, ROOT's current directory is the output file
    // to which we're writing the ntuple. But just in case, save the
    // current directory, and switch to the output directory.
    auto saveDirectory = gROOT->CurrentDirectory();
    a_output->cd();

    // Create the ntuple in the output file.
    std::string title = std::string("Options used for ") + m_progPath;
    auto ntuple = new TTree(a_ntupleName.c_str(), title.c_str());

    // Set up the columns of the ntuple. 
    std::string name, value, type, brief, desc, source;
    ntuple->Branch("OptionName",&name);
    ntuple->Branch("OptionValue",&value);
    ntuple->Branch("OptionType",&type);
    ntuple->Branch("OptionBrief",&brief);
    ntuple->Branch("OptionDesc",&desc);
    ntuple->Branch("OptionSource",&source);

    // Loop over the options. As it says in Options.h, this is an
    // inefficient operation, but hopefully no program will do it more
    // than once.
    auto numOptions = NumberOfOptions();
    for ( size_t i = 0; i != numOptions; ++i ) {
      name = GetOptionName(i);
      value = GetOptionValue(i);
      type = GetOptionType(i);
      brief = GetOptionBrief(i);
      desc = GetOptionDescription(i);
      source = GetOptionSource(i);

      // Write out the ntuple entry.
      ntuple->Fill();
    }

    // Wrap up the ntuple.
    ntuple->Write();

    // Switch back to the original directory.
    saveDirectory->cd();

    return success;
  }

  // The following code is heavily adapted from
  // https://vichargrave.github.io/programming/xml-parsing-with-dom-in-cpp/
  // http://blog.f85.net/2012/02/xerces-c-tutorial.html

  // For the XML Parser, we need to set up a custom ErrorHandler class.
  class CustomXMLErrorHandler : public xercesc::HandlerBase
  {
  public:
    CustomXMLErrorHandler() {}
    void error(const xercesc::SAXParseException& e)
    { handler(e); }
    void fatalError(const xercesc::SAXParseException& e)
    { handler(e); }
    void warning(const xercesc::SAXParseException& e)
    { handler(e); }
  private:
    void handler(const xercesc::SAXParseException& e)
    {
      char* message = xercesc::XMLString::transcode(e.getMessage());
      std::cerr << "line " << e.getLineNumber()
		<< ", column " << e.getColumnNumber()
		<< " -> " << message << std::endl << std::endl;
      xercesc::XMLString::release(&message);
      exit(EXIT_FAILURE);
    }
  };

  bool Options::m_ParseXML(const std::string& a_xmlFile, const std::string& a_program)
  {
    bool success = true;

    // General rule: everything created by transcode (whether it's char*
    // or XMLch*) must be released, otherwise you get memory leaks.
  
    // For every call to Initialize, there must be a call to Terminate.
    try { xercesc::XMLPlatformUtils::Initialize(); }
    catch (const xercesc::XMLException& toCatch) {
      char* message = xercesc::XMLString::transcode(toCatch.getMessage());
      std::cout << "ParseXML: Error during initialization!" << std::endl
		<< message << std::endl;
      xercesc::XMLString::release(&message);
      return false;
    }

    // Create an XML parser object.  Turn off all validation. (At least,
    // until we have a need to validate.)
    auto parser = new xercesc::XercesDOMParser();
    parser->setValidationScheme(xercesc::XercesDOMParser::Val_Never);
    parser->setDoNamespaces(false);
    parser->setDoSchema(false);
    parser->setValidationConstraintFatal(false);

    // Set up error handling. (Otherwise we'd have even more
    // exception tests than the ones we have below.)
    parser->setErrorHandler(new CustomXMLErrorHandler());

    // For everything we'd look for in an in XML file, the
    // tag name must be transcoded into Xerces-C custom format.
    auto rootTag = xercesc::XMLString::transcode("parameters");
    auto optionTag = xercesc::XMLString::transcode("option");
    auto nameAttr = xercesc::XMLString::transcode("name");
    auto shortAttr = xercesc::XMLString::transcode("short");
    auto valueAttr = xercesc::XMLString::transcode("value");
    auto typeAttr = xercesc::XMLString::transcode("type");
    auto descAttr = xercesc::XMLString::transcode("desc");

    // For the program-level tag blocks, store them in a list.. 
    std::set<std::string> programTags;

    // If a_program is "ALL", then we're going to add every single
    // program tag we find. Otherwise, initialize the program tags in
    // the list.
    bool allTags = ( a_program.compare("ALL") == 0 );
    if ( ! allTags ) {
      programTags.insert("global");
      programTags.insert(a_program);
    }

    // Read in the XML file and parse its contents. 
    parser->parse(a_xmlFile.c_str());

    // Get the XML document absorbed by the parser. 
    auto document = parser->adoptDocument();
    if (document == NULL) {
      std::cerr << "WARNING: File " << __FILE__ << " Line " << __LINE__ << " " 
		<< std::endl
		<< "ParseXML: could not get document from parser" << std::endl
		<< "   XML processing halted" << std::endl;
      return false;
    }

    // Start at the top (root) tag of the XML document:
    auto docRootNode = document->getDocumentElement();

    // Create an iterator that will take us, node-by-node
    // (or tag-by-tag if you prefer) through the rest of the
    // XML file. 
    auto walker 
      = document->createNodeIterator(docRootNode,
				     xercesc::DOMNodeFilter::SHOW_ELEMENT,
				     NULL,true); 

    // Use the iterator to "walk" through the XML:
    for (auto current_node = walker->nextNode(); 
	 current_node != 0; 
	 current_node = walker->nextNode()) {

      auto thisNodeName = current_node->getNodeName();
      auto parentNodeName = current_node->getParentNode()->getNodeName();

      // We want <option> tags that whose parents match the list in
      // programTags. If this is confusing, take a look at options.xml.

      // To compare the Xerces-C strings, we have to use compareIString.
      // It's like strcmp, but for the Xerces-C string format.
      if ( xercesc::XMLString::compareIString(thisNodeName,optionTag) != 0 )
	// This is not an <option> tag. 
	continue;

      // It _is_ an <option> tag. See if the parent tag is on our list
      // of recognized tags. First, convert the parent tag into a string. 
      auto ps = xercesc::XMLString::transcode(parentNodeName);
      std::string parentString(ps);
      xercesc::XMLString::release(&ps);

      // If the program name is "ALL", then insert the parent tag into
      // our list if it isn't already there.
      if (allTags) 
	programTags.insert(parentString);

      // Define a lambda function for the STL find algorithm to test
      // if two Xerces-C strings match.
      auto tagsMatch = [&](const std::string tag)
	{ return tag.compare(parentString) == 0;};

      // Search for a matching program tag in our list.
      auto const tagIter 
	= std::find_if(programTags.cbegin(), programTags.cend(), tagsMatch);
      if ( tagIter != programTags.cend() )
	{
	  // The current node is an <option> tag.  Covert current_node
	  // (a DOMNode*) into a DOMElement* to access its attributes.
	  auto element = dynamic_cast<xercesc::DOMElement*>(current_node);
	  if ( element == NULL ) {
	    std::cerr << "WARNING: File " << __FILE__ << " Line " << __LINE__ << " " 
		      << std::endl
		      << "ParseXML: Could not convert DOMNode* into DOMElement*" 
		      << std::endl
		      << "   XML processing halted" << std::endl;
	    return false;
	  }

	  // Fetch the name, value, and type attributes from the <option> tag.
	  // Xerces-C returns char*, whose memory does not get released unless
	  // we manually release it. 
	  char* cname
	    = xercesc::XMLString::transcode(element->getAttribute(nameAttr));
	  std::string name(cname);
	  char* cvalue
	    = xercesc::XMLString::transcode(element->getAttribute(valueAttr));
	  std::string value(cvalue);
	  char* ctype 
	    = xercesc::XMLString::transcode(element->getAttribute(typeAttr));
	  std::string type(ctype);
	  char* cbrief
	    = xercesc::XMLString::transcode(element->getAttribute(shortAttr));
	  std::string brief(cbrief);
	  char* cdesc
	    = xercesc::XMLString::transcode(element->getAttribute(descAttr));
	  std::string desc(cdesc);
	  xercesc::XMLString::release(&cname);
	  xercesc::XMLString::release(&cvalue);
	  xercesc::XMLString::release(&ctype);
	  xercesc::XMLString::release(&cbrief);
	  xercesc::XMLString::release(&cdesc);

	  // Validate as much as we can.

	  // Since users are crazy, convert the type to lower case.
	  auto lowC = [](unsigned char c){ return std::tolower(c); };
	  std::transform(type.begin(), type.end(), type.begin(), lowC);

	  // Look at the first letter of the type:

	  // "b" = boolean
	  // "d" or "f" = double or float
	  // "i" = integer
	  // "s" or "t" = string or text

	  // This is almost good enough, but not quite; "float" and
	  // "flag" both begin with "f". Disambiguate by looking at
	  // the first three characters, then use "g" as the 'first
	  // character' if it's a flag.
	  auto firstCharacter = type[0];
	  if ( type.compare(0,3,"fla") == 0 ) firstCharacter = 'g';

	  switch (firstCharacter)
	    {
	    case 'g':
	      m_options[name].type = e_flag;
	      // A flag is automatically 'off' until the user
	      // specifies it on the command line.
	      m_options[name].value = "0";
	      break;
	    case 'b':
	      // Wacky users require lower case for the logical values;
	      // e.g., they may use "ON" or "True".
	      std::transform(value.begin(), value.end(), value.begin(), lowC);
	  
	      m_options[name].type = e_boolean;
	      m_options[name].value = "0";
	      if ( value == "on"  || 
		   value[0] == 't'|| 
		   value[0] == '1' )
		m_options[name].value ="1";
	      break;
	    case 'f':
	    case 'd':
	      try {
		double test = std::stod(value);
		m_options[name].type = e_double;
		m_options[name].value = std::to_string(test);
	      } catch ( std::invalid_argument& e ) {
		success = false;
		std::cerr << "WARNING: File " << __FILE__ << " Line " << __LINE__ << " " 
			  << std::endl
			  << "<" << parentString << "><option name=\""
			  << name << "\" value=\"" << value << "\" type=\"" << type
			  << "\" /></" << parentString << "> :" << std::endl
			  << "   cannot convert 'value' to number" << std::endl;
	      }
	      break;
	    case 'i':
	      try {
		int test = std::stoi(value);
		m_options[name].type = e_integer;
		m_options[name].value = std::to_string(test);
	      } catch ( std::invalid_argument& e ) {
		std::cerr << "WARNING: File " << __FILE__ << " Line " << __LINE__ << " " 
			  << std::endl
			  << "<" << parentString << "><option name=\""
			  << name << "\" value=\"" << value << "\" type=\"" << type
			  << "\" /></" << parentString << "> :" << std::endl
			  << "   cannot convert 'value' to integer" << std::endl;
		success = false;
	      }
	      break;
	    case 's':
	    case 't':
	      m_options[name].type = e_string;
	      m_options[name].value = value;
	      break;
	    default:
	      std::cerr << "WARNING: File " << __FILE__ << " Line " << __LINE__ << " " 
			<< std::endl
			<< "<" << parentString << "><option name=\""
			<< name << "\" value=\"" << value << "\" type=\"" << type
			<< "\" /></" << parentString << "> :" << std::endl
			<< "   has an invalid 'type' (not string, boolean, flag, integer, or double)" 
			<< std::endl;
	      success = false;
	      ;
	    } // switch on type

	  // Handle the "short" attribute (which I call 'brief' here to avoid
	  // the C++ keyword 'short'. If there's no short option, set this
	  // to char(0) to avoid issues with getopt_long processing in ParseXML.
	  if ( brief.empty() ) m_options[name].brief = 0;
	  else m_options[name].brief = brief[0];

	  // The "desc" attribute:
	  m_options[name].desc = desc;

	  // The "source" attribute. 
	  m_options[name].source = *tagIter;

	} // appropriate <option> tag
    } // walking through the XML nodes

    // Clean up memory.
    xercesc::XMLString::release(&rootTag);
    xercesc::XMLString::release(&optionTag);
    xercesc::XMLString::release(&nameAttr);
    xercesc::XMLString::release(&shortAttr);
    xercesc::XMLString::release(&valueAttr);
    xercesc::XMLString::release(&typeAttr);
    xercesc::XMLString::release(&descAttr);
    document->release();
    xercesc::XMLPlatformUtils::Terminate();

    return success; 
  }


  bool Options::m_RootOptions(const std::string& a_optionsFile)
  {
    // Search through all the items in the file, looking for the
    // first TTree whose name include "Options".
    auto inputFile = TFile::Open(a_optionsFile.c_str());
    TIter next(inputFile->GetListOfKeys());
    TKey* key;
    TTree* ntuple;
    
    // For each item in the file:
    while ( ( key = (TKey*) next() ) ) {
      // Check that the key is valid.
      if (key != NULL) {
	// Read in the item.
	ntuple = (TTree*) key->ReadObj(); 
	// Is that item a TTree?
	if ( ntuple != NULL ) {
	  // What is the TTree's name?
	  std::string ntName = ntuple->GetName();
	  // Does the name contain "Options"?
	  if ( ntName.find("Options") != std::string::npos ) {
	    // Success!
	    std::cout << "ParseOptions: ntuple '" << ntName
		      << "' found in ROOT file" << std::endl;
	    
	    // Set up reading columns from this ntuple.
	    TTreeReader reader(ntName.c_str(), inputFile);
	    TTreeReaderValue<std::string> name(reader, "OptionName");
	    TTreeReaderValue<std::string> value(reader, "OptionValue");
	    TTreeReaderValue<std::string> type(reader, "OptionType");
	    TTreeReaderValue<std::string> brief(reader, "OptionBrief");
	    TTreeReaderValue<std::string> desc(reader, "OptionDesc");
	    TTreeReaderValue<std::string> source(reader, "OptionSource");
	    
	    // For each row in the ntuple
	    while ( reader.Next() ) {
	      // Add the row to the options map.
	      m_option_type eType = e_string;
	      if ( std::string(*type).compare("double")  == 0 ) eType = e_double;
	      if ( std::string(*type).compare("integer") == 0 ) eType = e_integer;
	      if ( std::string(*type).compare("boolean") == 0 ) eType = e_boolean;
	      if ( std::string(*type).compare("flag")    == 0 ) eType = e_flag;
	      m_option_attributes attr = { *value, eType, (*brief)[0], *desc, *source };
	      m_options[ *name ] = attr;
	    }
	    // We've filled the options map from the ntuple.
	    inputFile->Close();
	    return true;
	  } // name contains "Options"
	} // it's an ntuple
      } // key exists
    } // while looking through items

    // If we get here, we've failed! There are no ntuples whose name
    // contains "Options" in the ROOT file.
    std::cerr << "ERROR! File " << __FILE__ << " Line " << __LINE__ << " " 
	      << std::endl
	      << "No ntuple with a name containing 'Options' found in file '"
	      << a_optionsFile << "'" << std::endl;
    return false;
  }


} // namespace util
