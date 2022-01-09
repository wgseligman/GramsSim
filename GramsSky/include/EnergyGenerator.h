// 06-Jan-2022 WGS 

// An interface for all the GramsSky energy generators. This is part
// of a "strategy design pattern"; here we define the methods that
// must be implemented by specific energy generators.

#ifndef Grams_EnergyGenerator_h
#define Grams_EnergyGenerator_h

namespace gramssky {

  class EnergyGenerator
  {
  public:

    // Constructor. 
    EnergyGenerator() {} 

    // Destructor.
    virtual ~EnergyGenerator() {}

    // Generate a particle's energy. This method MUST be overridden by
    // a method that inherits it.
    virtual double Generate() = 0;
  };

} // namespace gramssky

#endif // Grams_EnergyGenerator_h
