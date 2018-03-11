/*
 * Parsing the XML-Payload from the getFile() response for a LayoutRequest from XIBO
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

#ifndef XIBO_SOAP_Layout_H 
#define XIBO_SOAP_Layout_H

namespace Xibo {
  namespace Xml {
      
    class XmlLayout {
    public:
      struct Audio {
        std::string uri;
        uint32_t volume;
        bool loop;
        uint32_t media;
      };
      
      struct Media {
        uint32_t id;
        std::string type;
        std::string render;
        uint32_t duration;
        bool useDuration;
        uint32_t file;
        std::map<std::string, std::string> options;
        std::map<std::string, std::string> raw;
        std::list<Audio> audio;
      };
      
      struct Region {
        uint32_t id;
        uint32_t x;
        uint32_t y;
        uint32_t z;
        uint32_t width;
        uint32_t height;
        std::map<std::string, std::string> options;
        std::list<Media> media;
      };

      struct Layout {
        std::string message;
        uint32_t width;
        uint32_t height;
        uint32_t version;
        std::string backgroundColor;
        uint32_t zindex;
        std::string backgroundImage;
        std::list<Region> regions;
        
        Layout() : message(""), width(0), height(0), version(3), backgroundColor("#000"), zindex(0), backgroundImage(""), regions({}) {};
        ~Layout() {};
      };

      XmlLayout();
      virtual ~XmlLayout();
      
      /**
      * Parsing the Layout payload sent from XIBO
      * 
      * @param std::string xml The XML Payload to parse
      * @param Layout *Layout Reference to a Layout-Struct to parse the xml into
      */
      void parse(const std::string xml, Layout *lay);
      
    private:
      // Main XML-TagNames
      static constexpr const char* TAG_LAYOUT = "layout";
      static constexpr const char* TAG_REGION = "region";
      static constexpr const char* TAG_OPTION = "options";
      static constexpr const char* TAG_MEDIA = "media";
      static constexpr const char* TAG_AUDIO = "audio";
      static constexpr const char* TAG_RAW = "raw";
      
      
      // Set if the above tags where opened
      bool layoutTag = false;
      bool regionTag = false;
      bool mediaTag = false;
      bool audioTag = false;
      bool rawTag = false;
      
      // Name of the currently open Settings-Tag
      std::string settingsTag;
      
      Layout *layout;
      XML_Parser parser;
      
      /**
       * Parses the attributes on the "Layout" tag
       * 
       * @param char **attrs Attributes given on the Tag; Name is on the index, the value on the index+1
       */
      void parseLayoutAttrs(const char **attrs);
      
      /**
       * Parses the attributes on the "Region" tag
       * 
       * @param char **attrs Attributes given on the Tag; Name is on the index, the value on the index+1
       */
      void parseRegionAttrs(const char **attrs);
      
      /**
       * Parses the attributes on the "Media" tag
       * 
       * @param char **attrs Attributes given on the Tag; Name is on the index, the value on the index+1
       */
      void parseMediaAttrs(const char **attrs);
      
      /**
       * Parses the attributes on the "Audio" tag
       * 
       * @param char **attrs Attributes given on the Tag; Name is on the index, the value on the index+1
       */
      void parseAudioAttrs(const char **attrs);
      
      /**
       * EXPAT-Callback for a starting node
       */
      static void startElementCallback(void * userData, const char *name, const char **attrs) {
        (static_cast<XmlLayout *>(userData))->expatStartElement(name, attrs);
      }

      /**
       * EXPAT-Callback for a closing node
       */
      static void endElementCallback(void * userData, const char *name) {
        (static_cast<XmlLayout *>(userData))->expatEndElement(name);
      }

      /**
       * EXPAT-Callback for a CDATA Textblock
       */
      static void characterDataCallback(void * userData, const char *text, int len) {
        (static_cast<XmlLayout *>(userData))->expatCharacterData(text, len);
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

#endif // XIBO_SOAP_Layout_H
