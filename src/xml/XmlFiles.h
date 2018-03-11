/*
 * Parsing the XML-Payload from the requiredFiles() response form XIBO
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

#ifndef XIBO_SOAP_FILES_H
#define XIBO_SOAP_FILES_H

namespace Xibo {
  namespace Xml {
      
    class XmlFiles {
    public:
      enum DownloadMethod { HTTP = 0, XMDS = 1 };
      struct Media {
        uint32_t id;
        uint32_t size;
        std::string hash;
        std::string path;
        std::string saveAs;
        DownloadMethod downloadMethod;
      };
      struct Layout {
        uint32_t id;
        uint32_t size;
        std::string hash;
        std::string path;
        std::string saveAs;
        DownloadMethod downloadMethod;
      };
      struct Resource {
        uint32_t id;
        uint32_t layout;
        uint32_t region;
        uint32_t media;
        std::string updated;
      };
      struct Resources {
        std::string message; // Possible XML-Parser Error Message
        std::string generated; // Datum generated
        std::string from; // Datum filterFrom/fliterFrom
        std::string upto; // Datum filterTo/fliterTo
        std::list<Media> media;
        std::list<Layout> layout;
        std::list<Resource> resource;
        std::list<uint32_t> blacklist;
        Resources() : message(""), generated(""), from(""), upto(""), media({}), layout({}), resource({}), blacklist({}) {};
        ~Resources() {};
      };

      XmlFiles();
      virtual ~XmlFiles();
      
      /**
      * Parsing the Resources payload sent from XIBO
      * 
      * @param std::string xml The XML Payload to parse
      * @param Resources *res Reference to a Resources-Struct to parse the xml into
      */
      void parse(const std::string xml, Resources *res);
      
    private:
      enum FileType { UNKNOWN, MEDIA, LAYOUT, RESOURCE, BLACKLIST };
      
      // Main XML-TagNames
      static constexpr const char* TAG_FILES = "files";
      static constexpr const char* TAG_FILE = "file";
      
      // Set if the above tags where opened
      bool filesTag = false;
      bool fileTag = false;
      bool subFileTag = false;
      bool blacklist = false;
      
      Resources *resources;
      XML_Parser parser;
      
      /**
       * Gets the "type" attribute from the XML-Tag and returns the Enum FileType based on the vale
       * 
       * @param char **attrs Attributes given on the Tag; Name is on the index, the value on the index+1
       */
      FileType getFileType(const char **attrs);
      
      /**
       * Parses the attributes on the "files" tag
       * 
       * @param char **attrs Attributes given on the Tag; Name is on the index, the value on the index+1
       */
      void parseFilesAttrs(const char **attrs);
      
      /**
       * Parses the attributes on the "file" tag if it's a Media-Type
       * 
       * @param char **attrs Attributes given on the Tag; Name is on the index, the value on the index+1
       */
      void parseMediaAttrs(const char **attrs);
      
      /**
       * Parses the attributes on the "file" tag if it's a Layout-Type
       * 
       * @param char **attrs Attributes given on the Tag; Name is on the index, the value on the index+1
       */
      void parseLayoutAttrs(const char **attrs);
      
      /**
       * Parses the attributes on the "file" tag if it's a Resource-Type
       * 
       * @param char **attrs Attributes given on the Tag; Name is on the index, the value on the index+1
       */
      void parseResourceAttrs(const char **attrs);
      
      /**
       * Parses the attributes on the "file" tag if it's a Blacklist-Type
       * 
       * @param char **attrs Attributes given on the Tag; Name is on the index, the value on the index+1
       */
      void parseBlacklistAttrs(const char **attrs);
      
      /**
       * EXPAT-Callback for a starting node
       */
      static void startElementCallback(void * userData, const char *name, const char **attrs) {
        (static_cast<XmlFiles *>(userData))->expatStartElement(name, attrs);
      }

      /**
       * EXPAT-Callback for a closing node
       */
      static void endElementCallback(void * userData, const char *name) {
        (static_cast<XmlFiles *>(userData))->expatEndElement(name);
      }

      /**
       * EXPAT-Callback for a CDATA Textblock
       */
      static void characterDataCallback(void * userData, const char *text, int len) {
        (static_cast<XmlFiles *>(userData))->expatCharacterData(text, len);
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

#endif // XIBO_SOAP_FILES_H
