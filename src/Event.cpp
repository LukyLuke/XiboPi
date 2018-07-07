/*
 * Event handler and broker for the Xibo Client.
 * Copyright (C) 2018  Lukas Zurschmiede <lukas.zurschmiede@ranta.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Event.h"

namespace Xibo {
  EventHandler::EventHandler() { }

  EventHandler::~EventHandler() { }

  void EventHandler::process() {
    if (xiboEventInstance != nullptr) {
      if (message_queue.empty()) {
        return;
      }

      // We need only the Event-Type EVENTS and don't care about the event data itself
      Event<void *> * event = (Event<void * > *) message_queue.front();
      message_queue.pop();
      std::map<const EVENTS, std::list<EventListener *>>::iterator it = registered.find(event->ev);
      if (it != registered.end()) {
        for (std::list<EventListener *>::const_iterator recv = it->second.cbegin(); recv != it->second.cend(); recv++) {
          (*recv)->eventFired(event->ev, &(event->data));
        }
      }
      delete event;
    }
  }

  void EventHandler::registerFor(const EVENTS ev, EventListener * event) {
    if (xiboEventInstance == nullptr) {
      xiboEventInstance = new EventHandler();
      gdk_threads_add_idle(processXiboEvents, (void *)nullptr);
    }

    std::map<const EVENTS, std::list<EventListener *>> * reg = &xiboEventInstance->registered;
    if (reg->find(ev) == reg->end()) {
      reg->insert(std::pair<const EVENTS, std::list<EventListener *>>(ev, {event}));
    } else {
      reg->find(ev)->second.push_back(event);
    }
  }

  const void EventHandler::fire(const void * data) {
    if (xiboEventInstance != nullptr) {
      xiboEventInstance->message_queue.push(data);
    }
  }

  const void EventHandler::stop() {
    delete xiboEventInstance;
    xiboEventInstance = nullptr;
  }
}

gboolean processXiboEvents(gpointer d) {
  if (xiboEventInstance != nullptr) {
    xiboEventInstance->process();
    return TRUE;
  }
  return FALSE;
}
