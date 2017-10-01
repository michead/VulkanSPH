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
  void registerWindow(const char* label, std::function<void()> drawFn);
  void unregisterWindow(const char* label);
  void render();

  void group(const char* label, std::function<void()> drawInnerFn, bool isCollapsed = true, bool isIndented = false);
  void vec3Slider(const char* label, float* val, float minVal = -1000, float maxVal = 1000, const char* format = "%.1f");

private:
  const GfxContext*                                          context;
  std::vector<std::pair<const char*, std::function<void()>>> drawFns;
};
