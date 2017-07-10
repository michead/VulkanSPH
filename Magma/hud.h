#pragma once

class MVkContext;
class SDLWindow;

class HUD {
  HUD(const MVkContext* context, SDL_Window* window);

  void render();

private:
  const MVkContext* context;
};
