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

#include <gdk/gdk.h>

#include <queue>
#include <map>
#include <list>
#include <string>
#include <iostream>

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

  template <class T>
  struct Event {
    const EVENTS ev;
    const T data;
  };

  class EventListener {
  public:
    virtual void eventFired(const EVENTS ev, const void * data) = 0;
  };

  class EventHandler {
  public:
    static void registerFor(const EVENTS ev, EventListener * event);
    const static void stop();
    const static void fire(const void * data);
    void process();

  private:
    EventHandler();
    ~EventHandler();

    std::queue<const void *> message_queue;
    std::map<const EVENTS, std::list<EventListener * > > registered;
  };

};
static Xibo::EventHandler * xiboEventInstance = nullptr;
static gboolean processXiboEvents(void * d);

#endif //XIBO_EVENT_H
