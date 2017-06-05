#include <iostream>
#include <vector>
#include "magma.h"

int main() {
  // Engine instance
  Magma magma;

  // Poll for user input.
  magma.mainLoop();

  return 0;
}
