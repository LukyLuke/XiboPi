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

#ifndef XIBO_EVENT_H
#define XIBO_EVENT_H

#include <map>
#include <list>
#include <string>

namespace Xibo {
  enum EVENTS {
    MESSAGE_RECEIVED,
    DISPLAY_REGISTERED,
    SCHEDULE_RECEIVED,
    RESOURCES_RECEIVED,
    RESOURCE_STORED,
    RESOURCE_FAILED,
    RESOURCE_REQUEST,
    RESOURCE_RECEIVED,
    INVENTORY_UPDATED,
    UPDATE_LAYOUT,
    SOAP_FAULT_RECEIVED
  };

  struct MessageEvent {
    std::string message = "";
    bool error = false;
  };

  class Event {
  public:
    virtual void eventFired(const EVENTS ev, const void * data) = 0;
  };

  class EventHandler {
  public:
    static void registerFor(const EVENTS ev, Event * event);
    const static void fire(const EVENTS ev, const void * data);

  private:
    EventHandler();
    ~EventHandler();
    std::map<const EVENTS, std::list<Event *>> registered;
  };
  static EventHandler * xiboEventInstance = nullptr;
};

#endif //XIBO_EVENT_H
