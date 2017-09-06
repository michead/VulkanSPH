#include "light.h"

Light::Light() {
  pos = { 0, 0, 0 };
  ke  = { 0, 0, 0 };
}

Light::Light(const ConfigNode& lightObj) {
  const ConfigNode& posNode = lightObj["pos"];
  const ConfigNode& keNode  = lightObj["ke"];

  pos = { posNode[0], posNode[1], posNode[2] };
  ke  = {  keNode[0],  keNode[1],  keNode[2] };
}
