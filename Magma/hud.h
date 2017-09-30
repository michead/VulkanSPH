#pragma once

struct SDL_Window;
class GfxContext;

class HUD {
public:
  HUD(const GfxContext* context, SDL_Window* window);
  ~HUD();

  void setupNewFrame();
  void immediateText(const char* text);
  void render();

private:
  const GfxContext*     context;
};
