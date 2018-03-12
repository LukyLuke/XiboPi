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

#include <string>
#include <map>
#include <list>
#include <iostream>

#include <expat.h>

#ifndef XIBO_SOAP_DISPLAY_H 
#define XIBO_SOAP_DISPLAY_H

namespace Xibo {
  namespace Xml {
      
    class XmlDisplay {
    public:
      enum DisplayStatus { READY = 0, ADDED = 1, WAITING = 2 };

      struct DisplayCommand {
        std::string name;
        std::string command;
        std::string validation;
      };

      struct Display {
        DisplayStatus status;
        std::string message;
        std::string version;
        std::string displayName;
        bool screenShotRequested;
        std::string displayTimeZone;
        std::map<std::string, std::string> config;
        std::list<DisplayCommand> commands;
        Display() : status(WAITING), message(""), version(""), displayName(""), screenShotRequested(false), displayTimeZone(""), config({}), commands({}) {};
        ~Display() {};
      };

      XmlDisplay();
      ~XmlDisplay();
      
      const constexpr char* statusToString(const DisplayStatus value) {
        const char* values[] = { "READY", "ADDED", "WAITING" };
        return values[static_cast<int>(value)];
      }
      
      /**
      * Parsing the Display payload sent from XIBO
      * 
      * @param std::string xml The XML Payload to parse
      * @param Display *display Reference to a Display-Struct to parse the xml into
      */
      void parse(const std::string xml, Display *display);
      
    private:
      // Main XML-TagNames
      static constexpr const char* TAG_SCREENSHOT = "screenShotRequested";
      static constexpr const char* TAG_TIMEZONE = "displayTimeZone";
      static constexpr const char* TAG_DISPLAY = "display";
      static constexpr const char* TAG_COMMAND = "commands";
      static constexpr const char* TAG_NAME = "displayName";
      static constexpr const char* TAG_CMD_CMD = "commandString";
      static constexpr const char* TAG_CMD_VAL = "validationString";
      
      // In case the display setting is defined as "windows" the TagName first characte is Uppercase
      static constexpr const char* TAG_SCREENSHOT_WIN = "ScreenShotRequested";
      static constexpr const char* TAG_TIMEZONE_WIN = "DisplayTimeZone";
      static constexpr const char* TAG_NAME_WIN = "DisplayName";
      
      // Set if the above tags where opened
      bool displayTag = false;
      bool commandTag = false;
      bool screenshotTag = false;
      bool timezoneTag = false;
      bool displayNameTag = false;
      bool commandCmdTag = false;
      bool commandValTag = false;
      
      // Name of the currently open Settings-Tag
      std::string settingsTag;
      
      Display *display;
      XML_Parser parser;
      
      /**
       * Parses the attributes on the "display" tag
       * 
       * @param char **attrs Attributes given on the Tag; Name is on the index, the value on the index+1
       */
      void parseDisplayAttrs(const char **attrs);
      
      /**
       * EXPAT-Callback for a starting node
       */
      static void startElementCallback(void * userData, const char *name, const char **attrs) {
        (static_cast<XmlDisplay *>(userData))->expatStartElement(name, attrs);
      }

      /**
       * EXPAT-Callback for a closing node
       */
      static void endElementCallback(void * userData, const char *name) {
        (static_cast<XmlDisplay *>(userData))->expatEndElement(name);
      }

      /**
       * EXPAT-Callback for a CDATA Textblock
       */
      static void characterDataCallback(void * userData, const char *text, int len) {
        (static_cast<XmlDisplay *>(userData))->expatCharacterData(text, len);
      }
      
      /**
       * Internal Expat-Callback for processing an open Node
       * 
       * @param char *name The Nodename
       * @param char **attrs Th Attributes
       */
      void expatStartElement(const char *name, const char **attrs);
      
      /**
       * Internal Expat-Callback for processing a closing Node
       * @param char *name Then Nodename
       */
      void expatEndElement(const char *name);
      
      /**
       * Internal Expat-Callback for processing text nodes
       * 
       * @param char *text The text to process
       * @param int len The text length
       */
      void expatCharacterData(const char *text, int len);
      
      /**
       * Resets the expat parser and recreates it
       */
      void resetParser();
    };
  }
}

#endif // XIBO_SOAP_DISPLAY_H
