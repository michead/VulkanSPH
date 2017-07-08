#include "perf.h"

void Perf::onFrameStart() {
  timeOld   = timeNow;
  timeNow   = SDL_GetPerformanceCounter();
  deltaTime = (double)((timeNow - timeOld) * 1000 / SDL_GetPerformanceFrequency());
}

double Perf::getLastDeltaTime() {
  return deltaTime;
}
