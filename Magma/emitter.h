#pragma once

#include "sph.h"

class Emitter {
public:
  void setSolver(SPH* sph);

private:
  SPH* sph;
};