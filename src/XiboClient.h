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
#include <string>

#include "api/soapxmdsBindingProxy.h"
#include "XiboDisplay.h"

#ifndef XIBO_CLIENT_H
#define XIBO_CLIENT_H

namespace Xibo {
  class XiboClient {
  public:
    XiboClient(const std::map<std::string, std::string> * conf, XiboDisplay * display);
    virtual ~XiboClient();
    bool connect(const char * server);
  
  private:
    const std::map<std::string, std::string> *config;
    xmdsBindingProxy * soapProxy = NULL;
    XiboDisplay * display = NULL;
    
    const std::string getConfig(const std::string key);
    void connectSoapProxy(const char * server);
  };
}

#endif //XIBO_CLIENT_H
