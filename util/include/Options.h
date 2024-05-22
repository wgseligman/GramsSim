/// 20-May-2020 WGS 
/// Implement a program-wide job-options processing service.

/// See README.md for documentation.

#ifndef Options_h
#define Options_h 1

#include <TDirectory.h>
#include <map>
#include <vector>
#include <string>

namespace util {

  class Options 
  {
  public:
    
    /// This is a singleton class.
    /// According to <https://stackoverflow.com/questions/12248747/singleton-with-multithreads>
    /// this method is compatible with multi-threaded running. 
    static Options* GetInstance()
    {
      static Options instance;
      return &instance;
    }

    /// ParseOptions initializes the options from an XML file or a
    /// ROOT file. This should be called just once, from the main
    /// routine. 

    /// Note: This routine uses getopt_long, a standard GNU utility
    /// that will "mangle" the contents of argv.
    bool ParseOptions(int argc, char** argv, 
		      const std::string programName = "");
    
    /// The "getters", one for each type of value.
    bool GetOption(const std::string name, double& value) const;
    bool GetOption(const std::string name, int& value) const;
    bool GetOption(const std::string name, bool& value) const;
    bool GetOption(const std::string name, std::string& value) const;
    bool GetOption(const std::string name, std::vector<double>& value) const;

    /// Display all the options as a table.
    void PrintOptions() const;

    /// Display all the options as a help message.
    void PrintHelp() const;

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
    std::string GetOptionLow( size_t i ) const;
    std::string GetOptionHigh( size_t i ) const;

    // The argument to this method is the output directory for the
    // ntuple. Presumably this will be a ROOT file.
    bool WriteNtuple(TDirectory* output, std::string outputNtuple = "Options");

    // Copy the input options ntuple from the input ROOT file and
    // merge its contents with the options we've already processed. Do
    // not overwrite any options in our internal list.

    // The intent to preserve a "history" of options selected
    // for a series of programs within an analysis chain. 

    // If you want to use this, you generally want call to call this
    // method _after_ Process Options and _before_ WriteNtuple; e.g.,

    // options->Process(int arv, char** argv );
    // ... (open input file from prior program) ...
    // options->CopyInputNtuple( inputfile );
    // options->WriteNtuple( outputfile );

    bool CopyInputNtuple(TDirectory* input, std::string inputNtupleName = "Options");

  protected:
    /// Standard null constructor for a singleton class.
    Options() {}
    
  private:
    enum m_option_type { e_string, e_double, e_integer, e_boolean, e_flag, e_vector};
    struct m_option_attributes {
      std::string value;   ///< The option's value
      m_option_type type;  ///< string/double/integer/boolean/flag
      char brief;          ///< Optional one-char short option
      std::string desc;    ///< Optional description for --help
      std::string source;  ///< From which place (XML tag block, command line) did this come from?
      std::string low;     ///< If non-empty, the lower bound of the option's numeric value.
      std::string high;    ///< If non-empty, the upper bound of the option's numeric value.
    };

    std::map<std::string,m_option_attributes> m_options; ///< User options map

    // Parse the contents of the XML file... once we've
    // determined the name of that file. 
    bool m_ParseXML(const std::string& filename, const std::string& programTag);

    // If it turns out that the options file is a ROOT file, look for
    // an Options ntuple.
    bool m_RootOptions(const std::string& filename);

    std::string m_progPath;    ///< The path of the running program (argv[0])
    std::string m_optionsFile; ///< The name of the options XML file used

    /// Convert a string like "(1,2,3)" into a vector of numbers.
    std::vector<double> m_stringToValues( const std::string& ) const;

    /// Convert a vector of numbers into a string like "(1,2,3)".
    std::string m_valuesToString( const std::vector<double>& ) const;
  };

} // namespace util

#endif // Options_h
