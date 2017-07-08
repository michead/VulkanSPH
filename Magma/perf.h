#pragma once
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

class Perf {
private:
  Uint64 timeOld;
  Uint64 timeNow;
  double deltaTime;

public:
  Perf() {
    timeOld = SDL_GetPerformanceCounter();
    timeNow = 0;
  }

  void   onFrameStart();
  double getLastDeltaTime();
};
