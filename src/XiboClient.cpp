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
#include <openssl/md5.h>
#include <ctime>
#include <cstring>

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
    
    // Do not fetch the file if we already have the same on the client
    const std::string hash = getMediaHash(media);
    if (strncmp(hash.c_str(), media->hash.c_str(), MD5_DIGEST_LENGTH) == 0) {
      return;
    }
    
    // If the hashes are not equal, fetch the media and store it
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
    const std::string date = getCurrentDateString();
    std::string payload = "<files>";
    bool success = false;
    
    // First download all resources if needed
    std::list<Xml::XmlFiles::Media>::const_iterator it = xmlFiles->media.cbegin();
    while (it != xmlFiles->media.cend()) {
      getResource(&(*it));
      payload.append("<file type='media' complete='1' id='").append(std::to_string((*it).id))
        .append("' lastCheck='").append(date)
        .append("' md5='").append(getMediaHash(&(*it))).append("' />");
      it++;
    }
    payload.append("</files>");
    
    // Finally send the Data
    int res = soapProxy->MediaInventory(serverKey, hardwareKey, payload, success);
    if (res != SOAP_OK) {
      display->showStatus(soapProxy->soap_fault_string(), 60);
    }
  }
  
  const std::string XiboClient::getCurrentDateString() {
    struct tm * timeinfo;
    time_t rawtime;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%d.%m.%Y %I:%M:%S", timeinfo);
    return std::string(buffer);
  }
  
  const std::string XiboClient::getMediaHash(const Xml::XmlFiles::Media * media) {
    std::string result;
    std::string file = std::string(getConfig("cache")).append(media->path);
    uint32_t fp = open(file.c_str(), O_RDONLY);
    MD5_CTX c;
    char buf[512];
    unsigned char out[MD5_DIGEST_LENGTH];
    ssize_t bytes;

    MD5_Init(&c);
    bytes = read(fp, buf, 512);
    while (bytes > 0) {
      MD5_Update(&c, buf, bytes);
      bytes = read(fp, buf, 512);
    }

    MD5_Final(out, &c);
    close(fp);
    
    for (int n = 0; n < MD5_DIGEST_LENGTH; n++) {
      sprintf(buf, "%02x", out[n]);
      result.append(reinterpret_cast<const char *>(buf));
    }
    return result;
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
