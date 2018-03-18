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

#include <map>
#include <list>
#include <string>

#include "api/soapxmdsBindingProxy.h"
#include "XiboDisplay.h"
#include "xml/XmlDisplay.h"
#include "xml/XmlSchedule.h"
#include "xml/XmlFiles.h"
#include "xml/XmlLayout.h"

#ifndef XIBO_CLIENT_H
#define XIBO_CLIENT_H

namespace Xibo {
  class XiboClient {
  public:
    XiboClient(const std::map<std::string, std::string> * conf, XiboDisplay * display);
    ~XiboClient();
    bool connect(const char * server);
    void schedule();
    void getResource(const Xml::XmlFiles::Media * media);
    const std::string getResource(const uint32_t region, const uint32_t media);
    const std::string getFileUrl(const uint32_t fileId);
    const std::string getConfig(const std::string key);
  
  private:
    const std::map<std::string, std::string> * config;
    xmdsBindingProxy * soapProxy = NULL;
    XiboDisplay * display = NULL;
    
    Xml::XmlDisplay::Display * xmlDisplay;
    Xml::XmlSchedule::Schedule * xmlSchedule;
    Xml::XmlFiles::Resources * xmlFiles;
    Xml::XmlLayout::Layout * xmlLayout;
    
    void connectSoapProxy(const char * server);
    void getRequiredResources();
    void getLayout();
    void updateMediaCache();
    const std::string getMediaHash(const Xml::XmlFiles::Media * media);
    const std::string getCurrentDateString();
  };
}

#endif //XIBO_CLIENT_H
