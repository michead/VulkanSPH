#include "light.h"
#include "logger.h"

Light::Light() {
  pos = { 0, 0, 0 };
}

Light::Light(const ConfigNode& lightObj) {
  try {
    const ConfigNode& posNode = lightObj.at("pos");
    pos = { posNode[0], posNode[1], posNode[2] };
  } catch (std::exception& e) {
    logger->error("Light position cannot be parsed.");
  }
}
