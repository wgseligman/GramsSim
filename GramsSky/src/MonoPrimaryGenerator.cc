#include "MonoPrimaryGenerator.h"
#include "PrimaryGenerator.h" // For GetTransform
#include "TransformCoordinates.h"

#include <memory>

namespace gramssky {

  MonoPrimaryGenerator::MonoPrimaryGenerator()
  {}

  MonoPrimaryGenerator::~MonoPrimaryGenerator()
  {}

  std::shared_ptr<ParticleInfo> MonoPrimaryGenerator::Generate()
  {
    auto stub = std::make_shared<ParticleInfo>();
    stub->SetZ(1);
    auto transformed = GetTransform()->Transform( stub );
    return transformed;
  }

} // namespace gramssky
