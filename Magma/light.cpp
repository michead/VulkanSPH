#include "light.h"

Light::Light() {
  pos = { 0, 0, 0 };
}

Light::Light(const ConfigNode& lightObj) {
  const ConfigNode& posNode = lightObj["pos"];
  pos = { posNode[0], posNode[1], posNode[2] };
}
