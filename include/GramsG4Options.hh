/// 20-May-2010 WGS 
/// Implement a program-wide job-options processing service.

/// See README.md for documentation.

#ifndef GramsG4Options_h
#define GramsG4Options_h 1

#include "G4String.hh"
#include "G4Types.hh"
#include <map>
#include <string>

class GramsG4Options 
{
public:

  /// This is a singleton class.
  /// According to <https://stackoverflow.com/questions/12248747/singleton-with-multithreads>
  /// this method is compatible with multi-threaded running. 
  static GramsG4Options* GetInstance()
  {
    static GramsG4Options instance;
    return &instance;
  }

  /// Initialize the options from an XML file.  This should be called
  /// just once, from the main routine.  Note: This routine uses
  /// getopt_long, a standard GNU utility that will "mangle" the
  /// contents of argv.
  bool ParseOptions(int argc, char** argv, const std::string& programName);

  /// The "getters", one for each type of value.
  bool GetOption(const std::string name, double& value) const;
  bool GetOption(const std::string name, int& value) const;
  bool GetOption(const std::string name, bool& value) const;
  bool GetOption(const std::string name, std::string& value) const;

  // Display all the options as a tab;e.
  void PrintOptions() const;

  // Display all the options as a help message.
  void PrintHelp() const;

protected:
  /// Standard null constructor for a singleton class.
  GramsG4Options() {}

private:
  enum m_option_type { e_string, e_double, e_integer, e_boolean };
  struct m_option_attributes {
    std::string value;   ///< The option's value
    m_option_type type;  ///< string/double/integer/boolean
    char brief;          ///< Optional one-char short option
    std::string desc;    ///< Optional description for --help
  };

  std::map<G4String,m_option_attributes> m_options; ///< User options map

  // Parse the contents of the XML file... once we've
  // determined the name of that file. 
  bool m_ParseXML(const std::string filename, const std::string programTag);

  std::string m_progName;    ///< The name of the running program (argv[0])
  std::string m_xmlFile;     ///< The name of the options XML file used
};

#endif // GramsG4Options_h
