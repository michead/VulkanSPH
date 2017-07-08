#pragma once
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <array>
#include <functional>
#include <vector>

enum EEventType {
  EVT_MOUSE_DOWN_LEFT_BTN,
  EVT_MOUSE_DOWN_RIGHT_BTN,
  EVT_MOUSE_UP_LEFT_BTN,
  EVT_MOUSE_UP_RIGHT_BTN,
  EVT_MOUSE_DRAG_LEFT_BTN,
  EVT_MOUSE_DRAG_RIGHT_BTN,
  EVT_QUIT,
  EVT_COUNT
};

typedef SDL_Event Event;
typedef std::function<void(Event)> EventCallback;

class EventHandler {
public:
  void handle(Event evt);
  void addListener(EEventType evt, const EventCallback& callback);
  void removeListeners(EEventType evt);

private:
  void notifyListeners(EEventType evt, Event _evt);

  bool isDraggingL = false;
  bool isDraggingR = false;
  std::array<std::vector<EventCallback>, EVT_COUNT> listeners;
};
