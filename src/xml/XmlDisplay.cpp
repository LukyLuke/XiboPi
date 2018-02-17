/*
 * Parsing the XML-Payload from the registerDisplay() response form XIBO
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

#include "XmlDisplay.h"
#include <string>
#include <cstring>

namespace Xibo {
  namespace Xml {
    XmlDisplay::XmlDisplay() { }
    
    XmlDisplay::~XmlDisplay() {
      XML_ParserFree(parser);
    }
    
    void XmlDisplay::parse(const std::string xml, Display *disp) {
      display = disp;
      resetParser();
      std::cout << xml << std::endl;
      
      int done = 0;
      if (XML_Parse(parser, xml.c_str(), xml.size(), done) == XML_STATUS_ERROR) {
        std::cerr << XML_ErrorString(XML_GetErrorCode(parser)) << " at line " << XML_GetCurrentLineNumber(parser);
        display->message = std::string(XML_ErrorString(XML_GetErrorCode(parser)));
      }
    }
    
    void XmlDisplay::resetParser() {
      if (!XML_ParserReset(parser, NULL)) {
        XML_ParserFree(parser);
        parser = XML_ParserCreate(NULL);
      }
      XML_SetUserData(parser, this);
      XML_SetElementHandler(parser, XmlDisplay::startElementCallback, XmlDisplay::endElementCallback);
      XML_SetCharacterDataHandler(parser, XmlDisplay::characterDataCallback);
    }
    
    void XmlDisplay::expatStartElement(const char *name, const char **attrs) {
      if (!displayTag && (strcmp(TAG_DISPLAY, name) == 0)) {
        displayTag = true;
        parseDisplayAttrs(attrs);
      }
      else if (displayTag && (strcmp(TAG_COMMAND, name) == 0)) {
        commandTag = true;
        display->commands.push_back({"", "", ""});
      }
      else if (commandTag) {
        if (strcmp(TAG_CMD_CMD, name) == 0) {
          commandCmdTag = true;
        }
        else if (strcmp(TAG_CMD_VAL, name) == 0) {
          commandValTag = true;
        } else {
          display->commands.back().name.assign(name);
        }
      }
      else if (displayTag && ((strcmp(TAG_SCREENSHOT, name) == 0) || (strcmp(TAG_SCREENSHOT_WIN, name) == 0))) {
        screenshotTag = true;
      }
      else if (displayTag && ((strcmp(TAG_TIMEZONE, name) == 0) || (strcmp(TAG_TIMEZONE_WIN, name) == 0))) {
        timezoneTag = true;
      }
      else if (displayTag && ((strcmp(TAG_NAME, name) == 0) || (strcmp(TAG_NAME_WIN, name) == 0))) {
        displayNameTag = true;
      }
      else if (displayTag && !commandTag) {
        settingsTag.assign(name);
      }
    }

    void XmlDisplay::expatEndElement(const char *name) {
      if (strcmp(TAG_DISPLAY, name) == 0) {
        displayTag = false;
      }
      else if (strcmp(TAG_COMMAND, name) == 0) {
        commandTag = false;
      }
      else if (commandTag) {
        if (strcmp(TAG_CMD_CMD, name) == 0) {
          commandCmdTag = false;
        }
        else if (strcmp(TAG_CMD_VAL, name) == 0) {
          commandValTag = false;
        }
      }
      else if ((strcmp(TAG_SCREENSHOT, name) == 0) || (strcmp(TAG_SCREENSHOT_WIN, name) == 0)) {
        screenshotTag = false;
      }
      else if ((strcmp(TAG_TIMEZONE, name) == 0) || (strcmp(TAG_TIMEZONE_WIN, name) == 0)) {
        timezoneTag = false;
      }
      else if ((strcmp(TAG_NAME, name) == 0) || (strcmp(TAG_NAME_WIN, name) == 0)) {
        displayNameTag = false;
      }
      else if (!settingsTag.empty()) {
        settingsTag.clear();
      }
    }
    
    void XmlDisplay::expatCharacterData(const char *text, int len) {
      if (screenshotTag) {
        strncpy((char*)(&display->screenShotRequested), text, len);
      }
      else if (timezoneTag) {
        display->displayTimeZone.assign(text, len);
      }
      else if (displayNameTag) {
        display->displayName.assign(text, len);
      }
      else if (!settingsTag.empty()) {
        display->config[settingsTag].assign(text, len);
      }
      else if (commandTag) {
        if (commandCmdTag) {
          display->commands.back().command.assign(text, len);
        }
        else if (commandValTag) {
          display->commands.back().validation.assign(text, len);
        }
      }
    }

    void XmlDisplay::parseDisplayAttrs(const char **attrs) {
      for (int i = 0; attrs[i]; i += 2) {
        if      (strcmp("message", attrs[i]) == 0)              { display->message = attrs[i + 1]; }
        else if (strcmp("version_instructions", attrs[i]) == 0) { display->version = attrs[i + 1]; }
        else if (strcmp("status", attrs[i]) == 0)               { display->status = static_cast<DisplayStatus>(atoi(attrs[i + 1])); }
      }
    }
    
  }
}
