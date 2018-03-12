/*
 * Parsing the XML-Payload from the schedule() response form XIBO
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

#ifndef XIBO_SOAP_SCHEDULE_H
#define XIBO_SOAP_SCHEDULE_H

namespace Xibo {
  namespace Xml {
      
    class XmlSchedule {
    public:
      struct Layout {
        uint32_t layoutId; // file
        std::string from; // fromdt
        std::string upto; // todt
        uint32_t priority;
        uint32_t eventId; // scheduleid
        std::list<std::string> dependents; // files
      };

      struct Command {
        uint32_t eventId; // scheduleid
        std::string date; // Datum
        std::string code;
      };
      
      struct Schedule {
        std::string message; // Possible XML-Parser Error Message
        std::string generated; // Datum generated
        std::string from; // Datum filterFrom/fliterFrom
        std::string upto; // Datum filterTo/fliterTo
        uint32_t defaultLayout;
        std::list<Layout> layouts;
        std::list<Command> commands;
        std::list<Layout> overlays;
        std::list<std::string> defaults;
        std::list<std::string> dependents;
        std::list<std::string> dependants;
        Schedule() : message(""), generated(""), from(""), upto(""), defaultLayout(0), layouts({}), commands({}), overlays({}), defaults({}), dependents({}), dependants({}) {};
        ~Schedule() {};
      };

      XmlSchedule();
      ~XmlSchedule();
      
      /**
      * Parsing the Schedule payload sent from XIBO
      * 
      * @param std::string xml The XML Payload to parse
      * @param Schedule *schedule Reference to a Schedule-Struct to parse the xml into
      */
      void parse(const std::string xml, Schedule *schedule);
      
    private:
      // Main XML-TagNames
      static constexpr const char* TAG_SCHEDULE = "schedule";
      static constexpr const char* TAG_LAYOUT = "layout";
      static constexpr const char* TAG_DEFAULT = "default";
      static constexpr const char* TAG_DEPENDENTS = "dependents";
      static constexpr const char* TAG_DEPENDANTS = "dependants";
      static constexpr const char* TAG_FILE = "file";
      static constexpr const char* TAG_COMMAND = "command";
      static constexpr const char* TAG_OVERLAYS = "overlays";
      static constexpr const char* TAG_OVERLAY = "overlay";
      
      // Set if the above tags where opened
      bool scheduleTag = false;
      bool layoutTag = false;
      bool dependentsTag = false;
      bool dependantsTag = false;
      bool fileTag = false;
      bool commandTag = false;
      bool overlaysTag = false;
      bool overlayTag = false;
      bool defaultTag = false;
      
      Schedule *schedule;
      XML_Parser parser;
      
      /**
       * Parses the attributes on the "layout" and "overlay" tag
       * 
       * @param char **attrs Attributes given on the Tag; Name is on the index, the value on the index+1
       */
      void parseLayoutAttrs(const char **attrs);
      
      /**
       * Parses the attributes on the "schedule" tag
       * 
       * @param char **attrs Attributes given on the Tag; Name is on the index, the value on the index+1
       */
      void parseScheduleAttrs(const char **attrs);
      
      /**
       * Parses the attributes on the "default" tag
       * 
       * @param char **attrs Attributes given on the Tag; Name is on the index, the value on the index+1
       */
      void parseDefaultAttrs(const char **attrs);
      
      /**
       * Parses the attributes on the "command" tag
       * 
       * @param char **attrs Attributes given on the Tag; Name is on the index, the value on the index+1
       */
      void parseCommandAttrs(const char **attrs);
      
      /**
       * EXPAT-Callback for a starting node
       */
      static void startElementCallback(void * userData, const char *name, const char **attrs) {
        (static_cast<XmlSchedule *>(userData))->expatStartElement(name, attrs);
      }

      /**
       * EXPAT-Callback for a closing node
       */
      static void endElementCallback(void * userData, const char *name) {
        (static_cast<XmlSchedule *>(userData))->expatEndElement(name);
      }

      /**
       * EXPAT-Callback for a CDATA Textblock
       */
      static void characterDataCallback(void * userData, const char *text, int len) {
        (static_cast<XmlSchedule *>(userData))->expatCharacterData(text, len);
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

#endif // XIBO_SOAP_SCHEDULE_H
