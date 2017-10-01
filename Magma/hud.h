#pragma once
#include <functional>
#include <utility>
#include <vector>
#include <imgui\imgui.h>

struct SDL_Window;
class GfxContext;

class HUD {
public:
  HUD(const GfxContext* context, SDL_Window* window);
  ~HUD();

  void setupNewFrame();
  void registerWindow(const char* name, std::function<void()> drawFn);
  void unregisterWindow(const char* name);
  void render();

private:
  const GfxContext*                                          context;
  std::vector<std::pair<const char*, std::function<void()>>> drawFns;
};
