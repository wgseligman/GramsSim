/// 20-May-2020 WGS 
/// Implement a program-wide job-options processing service.

/// See README.md for documentation.

#ifndef Options_h
#define Options_h 1

#include <TDirectory.h>
#include <map>
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
		      const std::string programName);
    
    /// The "getters", one for each type of value.
    bool GetOption(const std::string name, double& value) const;
    bool GetOption(const std::string name, int& value) const;
    bool GetOption(const std::string name, bool& value) const;
    bool GetOption(const std::string name, std::string& value) const;

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

    // The argument to this method is the output directory for the
    // ntuple. Presumably this will be a ROOT file.
    bool WriteNtuple(TDirectory* output, std::string outputNtuple = "Options");


  protected:
    /// Standard null constructor for a singleton class.
    Options() {}
    
  private:
    enum m_option_type { e_string, e_double, e_integer, e_boolean, e_flag};
    struct m_option_attributes {
      std::string value;   ///< The option's value
      m_option_type type;  ///< string/double/integer/boolean/flag
      char brief;          ///< Optional one-char short option
      std::string desc;    ///< Optional description for --help
    };

    std::map<std::string,m_option_attributes> m_options; ///< User options map

    // Parse the contents of the XML file... once we've
    // determined the name of that file. 
    bool m_ParseXML(const std::string filename, const std::string programTag);

    std::string m_progName;    ///< The name of the running program (argv[0])
    std::string m_xmlFile;     ///< The name of the options XML file used
  };

} // namespace util

#endif // Options_h
