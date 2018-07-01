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
  EventHandler::EventHandler() {}
  EventHandler::~EventHandler() {}

  void EventHandler::registerFor(const EVENTS ev, Event * event) {
    if (xiboEventInstance == nullptr) {
      xiboEventInstance = new EventHandler();
    }
    std::map<const EVENTS, std::list<Event *>> * reg = &xiboEventInstance->registered;
    if (reg->find(ev) == reg->end()) {
      reg->insert(std::pair<const EVENTS, std::list<Event *>>(ev, {event}));
    } else {
      reg->find(ev)->second.push_back(event);
    }
  }

  const void EventHandler::fire(const EVENTS ev, const void * data) {
    if (xiboEventInstance != nullptr) {
      std::map<const EVENTS, std::list<Event *>> * reg = &xiboEventInstance->registered;
      std::map<const EVENTS, std::list<Event *>>::iterator it = reg->find(ev);
      if (it != reg->end()) {
        for (std::list<Event *>::const_iterator recv = it->second.cbegin(); recv != it->second.cend(); recv++) {
          (*recv)->eventFired(ev, data);
        }
      }
    }
  }

}
