#include "PositionGenerator.h"
#include "EnergyGenerator.h"
#include "TransformCoordinates.h"

namespace gramssky {

  // Constructor
  PositionGenerator::PositionGenerator()
  {
    // Initialize for the transform from the celestial sphere to the
    // detector coordinates.
    m_transform = new TransformCoordinates();
  }

  // Destructor
  PositionGenerator::~PositionGenerator()
  {
    // Clean up pointers.
    delete m_transform;
    delete m_energyGenerator;
  }

} // namespace gramssky
