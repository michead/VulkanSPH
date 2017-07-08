#include "event_handler.h"

#define isBtn(a, b) ((a).button.which & (b))

void EventHandler::handle(Event evt) {
  switch (evt.type) {
  case SDL_QUIT:
    notifyListeners(EVT_QUIT, evt);
    break;
  case SDL_MOUSEBUTTONDOWN:
    if (isBtn(evt, SDL_BUTTON_LMASK)) {
      isDraggingL = true;
      notifyListeners(EVT_MOUSE_DOWN_LEFT_BTN, evt);
    }
    if (isBtn(evt, SDL_BUTTON_RMASK)) {
      isDraggingR = true;
      notifyListeners(EVT_MOUSE_DOWN_RIGHT_BTN, evt);
    }
    break;
  case SDL_MOUSEBUTTONUP:
    if (isBtn(evt, SDL_BUTTON_LMASK)) {
      isDraggingL = false;
      notifyListeners(EVT_MOUSE_UP_LEFT_BTN, evt);
    }
    if (isBtn(evt, SDL_BUTTON_RMASK)) {
      isDraggingR = false;
      notifyListeners(EVT_MOUSE_UP_RIGHT_BTN, evt);
    }
    break;
  case SDL_MOUSEMOTION:
    if (isDraggingL) {
      notifyListeners(EVT_MOUSE_DRAG_LEFT_BTN, evt);
    }
    if (isDraggingR) {
      notifyListeners(EVT_MOUSE_DRAG_RIGHT_BTN, evt);
    }
    break;
  default:
    break;
  }
}

void EventHandler::addListener(EEventType evt, const EventCallback& callback) {
  listeners[evt].push_back(callback);
}

void EventHandler::removeListeners(EEventType evt) {
  listeners[evt].clear();
}

void EventHandler::notifyListeners(EEventType evt, Event _evt) {
  for (const EventCallback& cb : listeners[evt]) {
    cb(_evt);
  }
}
