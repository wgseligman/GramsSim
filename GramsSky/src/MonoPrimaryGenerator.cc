#include "MonoPrimaryGenerator.h"

#include <memory>

namespace gramssky {

  MonoPrimaryGenerator::MonoPrimaryGenerator()
  {}

  MonoPrimaryGenerator::~MonoPrimaryGenerator()
  {}

  std::shared_ptr<ParticleInfo> MonoPrimaryGenerator::Generate()
  {
    return std::make_shared<ParticleInfo>();
  }

} // namespace gramssky
