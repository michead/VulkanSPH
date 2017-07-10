#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h> 
#include <iostream>
#include <vector>
#include "magma.h"

int main() {
  Magma magma;
  magma.mainLoop();

  // Print memory leak report
  _CrtDumpMemoryLeaks();
  return 0;
}
