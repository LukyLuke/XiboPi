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

#include "XmlFiles.h"
#include <string>
#include <cstring>

namespace Xibo {
  namespace Xml {
    XmlFiles::XmlFiles() {
      this->parser = XML_ParserCreate(NULL);
    }
    
    XmlFiles::~XmlFiles() {
      XML_ParserFree(parser);
    }
    
    void XmlFiles::parse(const std::string xml, Resources *res) {
      resources = res;
      resetParser();
#ifdef DEBUG
      std::cout << xml << std::endl;
#endif
      
      int done = 0;
      if (XML_Parse(parser, xml.c_str(), xml.size(), done) == XML_STATUS_ERROR) {
        std::cerr << XML_ErrorString(XML_GetErrorCode(parser)) << " at line " << XML_GetCurrentLineNumber(parser);
        resources->message = std::string(XML_ErrorString(XML_GetErrorCode(parser)));
      }
    }
    
    void XmlFiles::resetParser() {
      if (!XML_ParserReset(parser, NULL)) {
        XML_ParserFree(parser);
        parser = XML_ParserCreate(NULL);
      }
      XML_SetUserData(parser, this);
      XML_SetElementHandler(parser, XmlFiles::startElementCallback, XmlFiles::endElementCallback);
      XML_SetCharacterDataHandler(parser, XmlFiles::characterDataCallback);
    }
    
    void XmlFiles::expatStartElement(const char *name, const char **attrs) {
      if (strcmp(TAG_FILES, name) == 0) {
        filesTag = true;
        parseFilesAttrs(attrs);
      }
      else if (filesTag && (strcmp(TAG_FILE, name) == 0)) {
        subFileTag == fileTag;
        blacklist == blacklist && subFileTag;
        fileTag = true;
        
        switch (getFileType(attrs)) {
          case MEDIA:
            resources->media.push_back({0, 0, "", "", "", DownloadMethod::XMDS});
            parseMediaAttrs(attrs);
            break;
            
          case LAYOUT:
            resources->layout.push_back({0, 0, "", "", "", DownloadMethod::XMDS});
            parseLayoutAttrs(attrs);
            break;
            
          case RESOURCE:
            resources->resource.push_back({0, 0, 0, 0, ""});
            parseResourceAttrs(attrs);
            break;
            
          case BLACKLIST:
            blacklist = true;
            break;
            
          default:
            if (blacklist) {
              parseBlacklistAttrs(attrs);
            }
            break;
        }
        
      }
    }

    void XmlFiles::expatEndElement(const char *name) {
      if (strcmp(TAG_FILES, name) == 0) {
        filesTag = false;
      }
      else if (strcmp(TAG_FILE, name) == 0) {
        if (subFileTag) { subFileTag = false; }
        else            { fileTag = false; }
      }
    }
    
    void XmlFiles::expatCharacterData(const char *text, int len) {
      // There is no CDATA Content in this XML
    }
    
    XmlFiles::FileType XmlFiles::getFileType(const char **attrs) {
      for (int i = 0; attrs[i]; i += 2) {
        if (strcmp("type", attrs[i]) == 0) {
          if (strcmp("media", attrs[i + 1]) == 0) {
            return FileType::MEDIA;
          }
          if (strcmp("layout", attrs[i + 1]) == 0) {
            return FileType::LAYOUT;
          }
          if (strcmp("resource", attrs[i + 1]) == 0) {
            return FileType::RESOURCE;
          }
          if (strcmp("blacklist", attrs[i + 1]) == 0) {
            return FileType::BLACKLIST;
          }
        }
      }
      return FileType::UNKNOWN;
    }

    void XmlFiles::parseFilesAttrs(const char **attrs) {
      for (int i = 0; attrs[i]; i += 2) {
        if      (strcmp("generated", attrs[i]) == 0)  { resources->generated.assign(attrs[i + 1]); }
        // Yes, this is a typo in XIBO-XML
        else if (strcmp("fitlerFrom", attrs[i]) == 0) { resources->from.assign(attrs[i + 1]); }
        else if (strcmp("fitlerTo", attrs[i]) == 0)   { resources->upto.assign(attrs[i + 1]); }
        // But I want to be prepared for them to correct the typo :)
        else if (strcmp("filterFrom", attrs[i]) == 0) { resources->from.assign(attrs[i + 1]); }
        else if (strcmp("filterTo", attrs[i]) == 0)   { resources->upto.assign(attrs[i + 1]); }
      }
    }

    void XmlFiles::parseBlacklistAttrs(const char **attrs) {
      for (int i = 0; attrs[i]; i += 2) {
        if (strcmp("id", attrs[i]) == 0) { resources->blacklist.push_back(atoi(attrs[i + 1])); }
      }
    }
    
    void XmlFiles::parseLayoutAttrs(const char **attrs) {
      for (int i = 0; attrs[i]; i += 2) {
        if      (strcmp("id", attrs[i]) == 0)       { resources->layout.back().id = atoi(attrs[i + 1]); }
        else if (strcmp("size", attrs[i]) == 0)     { resources->layout.back().size = atoi(attrs[i + 1]); }
        else if (strcmp("md5", attrs[i]) == 0)      { resources->layout.back().hash.assign(attrs[i + 1]); }
        else if (strcmp("path", attrs[i]) == 0)     { resources->layout.back().path.assign(attrs[i + 1]); }
        else if (strcmp("saveAs", attrs[i]) == 0)   { resources->layout.back().saveAs.assign(attrs[i + 1]); }
        else if (strcmp("download", attrs[i]) == 0) { resources->layout.back().downloadMethod = static_cast<DownloadMethod>(atoi(attrs[i + 1])); }
      }
    }
    
    void XmlFiles::parseMediaAttrs(const char **attrs) {
      for (int i = 0; attrs[i]; i += 2) {
        if      (strcmp("id", attrs[i]) == 0)       { resources->media.back().id = atoi(attrs[i + 1]); }
        else if (strcmp("size", attrs[i]) == 0)     { resources->media.back().size = atoi(attrs[i + 1]); }
        else if (strcmp("md5", attrs[i]) == 0)      { resources->media.back().hash.assign(attrs[i + 1]); }
        else if (strcmp("path", attrs[i]) == 0)     { resources->media.back().path.assign(attrs[i + 1]); }
        else if (strcmp("saveAs", attrs[i]) == 0)   { resources->media.back().saveAs.assign(attrs[i + 1]); }
        else if (strcmp("download", attrs[i]) == 0) { resources->media.back().downloadMethod = static_cast<DownloadMethod>(atoi(attrs[i + 1])); }
      }
    }
    
    void XmlFiles::parseResourceAttrs(const char **attrs) {
      for (int i = 0; attrs[i]; i += 2) {
        if      (strcmp("id", attrs[i]) == 0)       { resources->resource.back().id = atoi(attrs[i + 1]); }
        else if (strcmp("layoutid", attrs[i]) == 0) { resources->resource.back().layout = atoi(attrs[i + 1]); }
        else if (strcmp("regionid", attrs[i]) == 0) { resources->resource.back().region = atoi(attrs[i + 1]); }
        else if (strcmp("mediaid", attrs[i]) == 0)  { resources->resource.back().media = atoi(attrs[i + 1]); }
        else if (strcmp("updated", attrs[i]) == 0)  { resources->resource.back().updated.assign(attrs[i + 1]); }
      }
    }
    
  }
}
