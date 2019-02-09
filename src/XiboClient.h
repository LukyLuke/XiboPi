/*
 * The main XIBO Client.
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

#ifndef XIBO_CLIENT_H
#define XIBO_CLIENT_H

#include "XiboConfig.h"
#include "Event.h"

#include <map>
#include <list>
#include <string>
#include <openssl/md5.h>
#include <ctime>
#include <cstring>

#include "api/soapxmdsBindingProxy.h"
#include "xml/XmlDisplay.h"
#include "xml/XmlSchedule.h"
#include "xml/XmlFiles.h"
#include "xml/XmlLayout.h"

namespace Xibo {
  class XiboClient : public EventListener {
  public:
    XiboClient();
    ~XiboClient();
    bool connect(const char * server);
    void schedule();
    void eventFired(const EVENTS ev, const void * data);

  private:
    xmdsBindingProxy * soapProxy = NULL;
    std::string serverKey;
    std::string hardwareKey;

    const std::string layoutCacheFileBase = "xibo_layout_";
    const std::string resourceCacheFileBase = "xibo_resouce_";

    const std::string clientType = "windows";
    const std::string operationSystem = "Linux";
    const std::string clientVersion = "1.8.3";
    const int clientCode = 1;

    Xml::XmlDisplay::Display * xmlDisplay;
    Xml::XmlSchedule::Schedule * xmlSchedule;
    Xml::XmlFiles::Resources * xmlFiles;
    Xml::XmlLayout::Layout * xmlLayout;

    void connectSoapProxy(const char * server);
    bool fetchAndStoreMediaFile(const std::string file, const Xml::XmlFiles::Media * media);
    bool fetchAndStoreLayout(const std::string file, const Xml::XmlFiles::Layout * layout);
    bool fetchAndStoreResource(const std::string file, const uint32_t id, const std::string type, uint32_t size);
    const void getResource(const uint32_t region, const uint32_t media);
    const void getRequiredResources();
    const void updateMediaCache();
    const bool validateMediaHash(const std::string file, const std::string required);
    const std::string getMediaHash(const std::string file);
    const std::string getCurrentDateString();

    const void fireMessageEvent(const EVENTS ev, const std::string message);
    const void fireDisplayEvent(const EVENTS ev, const Xml::XmlDisplay::Display display);
    const void fireSchedulerEvent(const EVENTS ev, const Xml::XmlSchedule::Schedule schedule);
    const void fireLayoutEvent(const EVENTS ev, const Xml::XmlLayout::Layout layout);
    const void fireResourcesEvent(const EVENTS ev, const Xml::XmlFiles::Resources resources);
    const void fireMediaResourceEvent(const EVENTS ev, const Xml::XmlFiles::MediaResource resource);
    const void fireMediaInventoryEvent(const EVENTS ev, const bool success);
  };
}

#endif //XIBO_CLIENT_H
