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

namespace Xibo {
  XiboClient::XiboClient(const std::map<std::string, std::string> *conf, XiboDisplay *display) {
    this->config = conf;
    this->display = display;
    this->xmlDisplay = new Xml::XmlDisplay::Display();
    this->xmlSchedule = new Xml::XmlSchedule::Schedule();
    this->xmlFiles = new Xml::XmlFiles::Resources();
    this->xmlLayout = new Xml::XmlLayout::Layout();
  }
  
  XiboClient::~XiboClient() {
    if (soapProxy != NULL)
      delete soapProxy;
    delete xmlDisplay;
    delete xmlSchedule;
    delete xmlFiles;
    delete xmlLayout;
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
  
  const std::string XiboClient::getFileUrl(const uint32_t fileId) {
    std::string path = getConfig("base_uri");
    std::list<Xml::XmlFiles::Media>::const_iterator it = xmlFiles->media.cbegin();
    while (it != xmlFiles->media.cend()) {
      const Xml::XmlFiles::Media * m = &(*it);
      if (m->id == fileId) {
        return path.append(m->path);
      }
      it++;
    }
    return "";
  }
  
  bool XiboClient::connect(const char * server) {
    connectSoapProxy(server);
   
    const std::string serverKey("XIBODEV");
    const std::string hardwareKey("HardwareKey");
    const std::string displayName("DisplayName");
    const std::string clientType("windows");
    const std::string clientVersion("1.8.3");
    const std::string operationSystem("Linux");
    const std::string macAddress("11:22:33:aa:bb:cc");
    const std::string xmrChannel("Channel");
    const std::string publicKey("");
    std::string payload;

    int res = soapProxy->RegisterDisplay(serverKey, hardwareKey, displayName, clientType, clientVersion, 1, operationSystem, macAddress, xmrChannel, publicKey, payload);

    if (res == SOAP_OK) {
      Xml::XmlDisplay disp;
      disp.parse(payload, xmlDisplay);
      display->showStatus(xmlDisplay->message, 10);
      return true;
    }
    
    display->showStatus(soapProxy->soap_fault_string(), 60);
    return false;
  }
  
  void XiboClient::schedule() {
    const std::string serverKey("XIBODEV");
    const std::string hardwareKey("HardwareKey");
    std::string payload;
    
    int res = soapProxy->Schedule(serverKey, hardwareKey, payload);
    
    if (res == SOAP_OK) {
      Xml::XmlSchedule schedule;
      schedule.parse(payload, xmlSchedule);
      display->showStatus(xmlSchedule->message, 10);
      
      getRequiredResources();
      updateMediaCache();
      getLayout();
      
    } else {
      display->showStatus(soapProxy->soap_fault_string(), 60);
    }
  }
  
  const std::string XiboClient::getResource(const uint32_t region, const uint32_t media) {
    const std::string serverKey("XIBODEV");
    const std::string hardwareKey("HardwareKey");
    std::string payload;
    
    int res = soapProxy->GetResource(serverKey, hardwareKey, xmlSchedule->defaultLayout, std::to_string(region), std::to_string(media), payload);
    
    if (res == SOAP_OK) {
      return payload;
    }
    
    display->showStatus(soapProxy->soap_fault_string(), 60);
    return std::string(soapProxy->soap_fault_string());
  }
  
  void XiboClient::getResource(const Xml::XmlFiles::Media * media) {
    const std::string serverKey("XIBODEV");
    const std::string hardwareKey("HardwareKey");
    const std::string tmp("/tmp/XiboDev/");
    xsd__base64Binary payload = xsd__base64Binary();
    
    int res = soapProxy->GetFile(serverKey, hardwareKey, media->id, "media", 0, media->size, payload);
    
    if (res == SOAP_OK) {
      std::string file = std::string(getConfig("cache")).append(media->path);

      FILE * fp = fopen(file.c_str(), "w");
      if (fp == NULL) {
        display->showStatus(std::string("Unable to write File: ").append(file), 60);
        return;
      }
      
      fwrite(payload.__ptr, sizeof(char), payload.__size, fp);
      fclose(fp);
    } else {
      display->showStatus(soapProxy->soap_fault_string(), 60);
    }
  }
  
  void XiboClient::updateMediaCache() {
    const std::string serverKey("XIBODEV");
    const std::string hardwareKey("HardwareKey");
    std::string payload = "";
    bool success = false;
    
    int res = soapProxy->MediaInventory(serverKey, hardwareKey, payload, success);
    
    if ((res == SOAP_OK) && success) {
      return;
    }
    
    std::list<Xml::XmlFiles::Media>::const_iterator it = xmlFiles->media.cbegin();
    while (it != xmlFiles->media.cend()) {
      getResource(&(*it));
      it++;
    }
  }
  
  void XiboClient::getRequiredResources() {
    const std::string serverKey("XIBODEV");
    const std::string hardwareKey("HardwareKey");
    std::string payload;
    
    int res = soapProxy->RequiredFiles(serverKey, hardwareKey, payload);
    
    if (res == SOAP_OK) {
      Xml::XmlFiles resources;
      resources.parse(payload, xmlFiles);
      display->showStatus(xmlFiles->message, 10);
      
    } else {
      display->showStatus(soapProxy->soap_fault_string(), 60);
    }
  }
  
  void XiboClient::getLayout() {
    const std::string serverKey("XIBODEV");
    const std::string hardwareKey("HardwareKey");
    xsd__base64Binary payload = xsd__base64Binary();
    
    int res = soapProxy->GetFile(serverKey, hardwareKey, xmlSchedule->defaultLayout, "layout", 0, 0, payload);
    
    if (res == SOAP_OK) {
      Xml::XmlLayout layout;
      layout.parse(std::string(reinterpret_cast<const char *>(payload.__ptr), payload.__size), xmlLayout);
      display->showStatus(xmlFiles->message, 10);
      display->setLayout(xmlLayout, this);
      
    } else {
      display->showStatus(soapProxy->soap_fault_string(), 60);
    }
  }
}
