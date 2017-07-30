#pragma once

class GfxContext;
class SDLWindow;

class HUD {
  HUD(const GfxContext* context, SDL_Window* window);

  void render();

private:
  const GfxContext* context;
};
