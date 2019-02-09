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
  XiboClient::XiboClient() {
    this->xmlDisplay = new Xml::XmlDisplay::Display();
    this->xmlSchedule = new Xml::XmlSchedule::Schedule();
    this->xmlFiles = new Xml::XmlFiles::Resources();
    this->xmlLayout = new Xml::XmlLayout::Layout();
    EventHandler::registerFor(RESOURCE_REQUEST, this);

    this->serverKey = XiboConfig::get("server_key", "XIBODEV");
    this->hardwareKey = XiboConfig::get("hardware_key", "HardwareKey");
  }

  XiboClient::~XiboClient() {
    EventHandler::stop();

    if (soapProxy != NULL)
      delete soapProxy;
    delete xmlDisplay;
    delete xmlSchedule;
    delete xmlFiles;
    delete xmlLayout;
  }

  const void XiboClient::fireMessageEvent(const EVENTS ev, const std::string message) {
    EventHandler::fire((const void *) new Event<std::string>({ev, message}));
  }

  const void XiboClient::fireDisplayEvent(const EVENTS ev, const Xml::XmlDisplay::Display display) {
    EventHandler::fire((const void *) new Event<Xml::XmlDisplay::Display>({ev, display}));
  }

  const void XiboClient::fireSchedulerEvent(const EVENTS ev, const Xml::XmlSchedule::Schedule schedule) {
    EventHandler::fire((const void *) new Event<Xml::XmlSchedule::Schedule>({ev, schedule}));
    getRequiredResources();
  }

  const void XiboClient::fireLayoutEvent(const EVENTS ev, const Xml::XmlLayout::Layout layout) {
    EventHandler::fire((const void *) new Event<Xml::XmlLayout::Layout>({ev, layout}));
  }

  const void XiboClient::fireResourcesEvent(const EVENTS ev, const Xml::XmlFiles::Resources resources) {
    EventHandler::fire((const void *) new Event<Xml::XmlFiles::Resources>({ev, resources}));
    updateMediaCache();
  }

  const void XiboClient::fireMediaResourceEvent(const EVENTS ev, const Xml::XmlFiles::MediaResource resource) {
    EventHandler::fire((const void *) new Event<Xml::XmlFiles::MediaResource>({ev, resource}));
  }

  const void XiboClient::fireMediaInventoryEvent(const EVENTS ev, const bool success) {
    EventHandler::fire((const void *) new Event<bool>({ev, success}));
    if (success) {
      getLayout();
    }
  }

  void XiboClient::eventFired(const EVENTS ev, const void * data) {
    if (ev == RESOURCE_REQUEST) {
      const Xml::XmlLayout::Media media = *(const Xml::XmlLayout::Media *)data;
      getResource(media.region, media.id);
    }
  }

  void XiboClient::connectSoapProxy(const char * server) {
    if (soapProxy == NULL) {
      std::cout << "[XiboClient] Connection to " << server << std::endl;
      soapProxy = new xmdsBindingProxy(server);
    }
  }

  bool XiboClient::connect(const char * server) {
    connectSoapProxy(server);

    // Not yet... Or probably when XMR is used?
    const std::string publicKey("");

    const std::string displayName(XiboConfig::get("display_name", "DisplayName"));
    const std::string macAddress(XiboConfig::get("mac_address", "11:22:33:aa:bb:cc"));
    const std::string xmrChannel(XiboConfig::get("xmr_channel", "Channel"));

    std::string payload;
    int res = soapProxy->RegisterDisplay(serverKey, hardwareKey, displayName, clientType, clientVersion, clientCode, operationSystem, macAddress, xmrChannel, publicKey, payload);

    if (res == SOAP_OK) {
      Xml::XmlDisplay disp;
      disp.parse(payload, xmlDisplay);
      fireDisplayEvent(DISPLAY_REGISTERED, *xmlDisplay);
      return true;
    }

    fireMessageEvent(SOAP_FAULT_RECEIVED, soapProxy->soap_fault_string());
    return false;
  }

  void XiboClient::schedule() {
    std::string payload;
    int res = soapProxy->Schedule(serverKey, hardwareKey, payload);

    if (res == SOAP_OK) {
      Xml::XmlSchedule schedule;
      schedule.parse(payload, xmlSchedule);
      fireSchedulerEvent(SCHEDULE_RECEIVED, *xmlSchedule);

    } else {
      fireMessageEvent(SOAP_FAULT_RECEIVED, soapProxy->soap_fault_string());
    }
  }

  const void XiboClient::getResource(const uint32_t region, const uint32_t media) {
    std::string payload;
    int res = soapProxy->GetResource(serverKey, hardwareKey, xmlSchedule->defaultLayout, std::to_string(region), std::to_string(media), payload);

    if (res == SOAP_OK) {
      fireMediaResourceEvent(RESOURCE_RECEIVED, Xml::XmlFiles::MediaResource({media, region, (uint32_t)payload.length(), payload}));
      return;
    }
    fireMessageEvent(SOAP_FAULT_RECEIVED, soapProxy->soap_fault_string());
  }

  bool XiboClient::storeResources(const Xml::XmlFiles::Media * media) {
    // Do not fetch the file if we already have the same on the client
    std::string file = std::string(XiboConfig::get("cache")).append(media->saveAs);
    const std::string hash = getMediaHash(file);
    if (strncmp(hash.c_str(), media->hash.c_str(), MD5_DIGEST_LENGTH) == 0) {
      return true;
    }

    // If the hashes are not equal, fetch the media and store it
    xsd__base64Binary payload = xsd__base64Binary();
    int res = soapProxy->GetFile(serverKey, hardwareKey, media->id, "media", 0, media->size, payload);
    if (res == SOAP_OK) {
      FILE * fp = fopen(file.c_str(), "w");
      if (fp == NULL) {
        fireMessageEvent(RESOURCE_FAILED, std::string("Unable to write File: ").append(file));
        return false;
      }

      fwrite(payload.__ptr, sizeof(char), payload.__size, fp);
      fclose(fp);
      fireMessageEvent(RESOURCE_STORED, file);
      return true;
    }

    fireMessageEvent(SOAP_FAULT_RECEIVED, soapProxy->soap_fault_string());
    return false;
  }

  const void XiboClient::updateMediaCache() {
    const std::string date = getCurrentDateString();
    std::string payload = "<files>";
    bool success = false;
    bool downloaded= false;

    // First download all resources if needed
    std::list<Xml::XmlFiles::Media>::const_iterator media = xmlFiles->media.cbegin();
    while (media != xmlFiles->media.cend()) {
      std::string file = std::string(XiboConfig::get("cache")).append((*media).saveAs);
      downloaded = storeResources(&(*media));

      payload.append("<file type='media' complete='")
        .append(downloaded ? "1" : "0")
        .append("' id='")
        .append(std::to_string((*media).id))
        .append("' lastChecked='")
        .append(date)
        .append("' md5='")
        .append(getMediaHash(file))
        .append("' />");
        media++;
    }

    // Append the Layouts
    // TODO: Check first in the cache and fetch them if needed
    // TODO: Is it really the "defaultLayout"?
    downloaded = false;
    payload.append("<file type='layout' complete='")
      .append(downloaded ? "1" : "0")
      .append("' lastChecked='")
      .append(date)
      .append("' id='")
      .append(std::to_string(xmlSchedule->defaultLayout))
      .append("' md5='' />");

    // Append all Resources
    // TODO: Check first in the cache and fetch them if needed
    std::list<Xml::XmlFiles::Resource>::const_iterator resource = xmlFiles->resource.cbegin();
    while (resource != xmlFiles->resource.cend()) {
      downloaded = false;
      payload.append("<file type='resource' complete='")
        .append(downloaded ? "1" : "0")
        .append("' lastChecked='")
        .append(date)
        .append("' id='")
        .append(std::to_string((*resource).id))
        .append("' md5='' />");
        resource++;
    }
    payload.append("</files>");

    // Finally send the Data
    int res = soapProxy->MediaInventory(serverKey, hardwareKey, payload, success);
    if (res == SOAP_OK) {
      fireMediaInventoryEvent(INVENTORY_UPDATED, true);
      return;
    }
    fireMediaInventoryEvent(INVENTORY_UPDATED, false);
    fireMessageEvent(SOAP_FAULT_RECEIVED, soapProxy->soap_fault_string());
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

  const std::string XiboClient::getMediaHash(const std::string file) {
    std::string result;
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

  const void XiboClient::getRequiredResources() {
    std::string payload;
    int res = soapProxy->RequiredFiles(serverKey, hardwareKey, payload);
    if (res == SOAP_OK) {
      Xml::XmlFiles resources;
      resources.parse(payload, xmlFiles);
      fireResourcesEvent(RESOURCES_RECEIVED, *xmlFiles);
      return;
    }
    fireMessageEvent(SOAP_FAULT_RECEIVED, soapProxy->soap_fault_string());
  }

  const void XiboClient::getLayout() {
    xsd__base64Binary payload = xsd__base64Binary();
    int res = soapProxy->GetFile(serverKey, hardwareKey, xmlSchedule->defaultLayout, "layout", 0, 0, payload);

    if (res == SOAP_OK) {
      Xml::XmlLayout layout;
      layout.parse(std::string(reinterpret_cast<const char *>(payload.__ptr), payload.__size), xmlLayout);
      fireLayoutEvent(UPDATE_LAYOUT, *xmlLayout);
      return;
    }
    fireMessageEvent(SOAP_FAULT_RECEIVED, soapProxy->soap_fault_string());
  }
}
