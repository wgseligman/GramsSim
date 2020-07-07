/// \file GramsG4/include/GramsG4RunMode.hh
/// \brief Singleton to share run mode between classes.
///
///  This G4 application can run in these modes:
///  - Batch mode: The user has supplied commands in a G4 macro file.
///    Assume all generator parameters are set by that file.
///  - UI mode: Similar to batch mode, but set some default parameters
///    so the user can see pretty pictures without thinking much about it.
///  - Command-line mode: Take the generator parameters from the command
///    line. 
///

#ifndef _GramsG4RunMode_H_
#define _GramsG4RunMode_H_

enum eRunMode { batchMode, uiMode, commandMode };

class GramsG4RunMode 
{
public:
   /// This is a singleton class.
   /// According to <https://stackoverflow.com/questions/12248747/singleton-with-multithreads>
   /// this method is compatible with multi-threaded running. 

   static GramsG4RunMode* GetInstance() {
      static GramsG4RunMode instance;
      return &instance;
   }

   void SetRunMode( eRunMode m ) { m_runMode = m; }
   eRunMode GetRunMode() { return m_runMode; }

private:
   eRunMode m_runMode;

};

#endif // _GramsG4RunMode_H_
