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

#include "XiboClient.h"
#include "api/xmdsBinding.nsmap"
#include "xml/XmlDisplay.h"

namespace Xibo {
  XiboClient::XiboClient(const std::map<std::string, std::string> *conf, XiboDisplay *display) {
    this->config = conf;
    this->display = display;
  }
  
  XiboClient::~XiboClient() {
    soapProxy = NULL;
  }
  
  const std::string XiboClient::getConfig(const std::string key) {
    if (config->count(key) != 0) {
      return config->find(key)->second;
    }
    return "";
  }
  
  void XiboClient::connectSoapProxy(const char * server) {
    if (soapProxy == NULL) {
      soapProxy = new xmdsBindingProxy(server);
    }
  }
  
  bool XiboClient::connect(const char * server) {
    connectSoapProxy(server);
   
    const std::string serverKey ("XIBODEV");
    const std::string hardwareKey("HardwareKey");
    const std::string displayName("DisplayName");
    const std::string clientType("windows");
    const std::string clientVersion("1.8.3");
    const std::string operationSystem("Linux");
    const std::string macAddress("11:22:33:aa:bb:cc");
    const std::string xmrChannel("Channel");
    const std::string publicKey("");
    std::string activationMessage;

    int res = soapProxy->RegisterDisplay(serverKey, hardwareKey, displayName, clientType, clientVersion, 1, operationSystem, macAddress, xmrChannel, publicKey, activationMessage);

    if (res == SOAP_OK) {
      Xml::XmlDisplay::Display d;
      Xml::XmlDisplay disp;
      disp.parse(activationMessage, &d);
      display->showStatus(d.message, 60);
      
      //std::cout << d.commands.front().command << std::endl;
      
    } else {
      std::cout << "NOT OK" << std::endl;
      std::cout << "Error: " << soapProxy->soap_fault_string() << std::endl;
      display->showStatus(soapProxy->soap_fault_string(), 60);
    }
    return (res == SOAP_OK);
  }
}
