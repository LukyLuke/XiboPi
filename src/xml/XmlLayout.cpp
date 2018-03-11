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

#include "XmlLayout.h"
#include <string>
#include <cstring>

namespace Xibo {
  namespace Xml {
    XmlLayout::XmlLayout() {
      this->parser = XML_ParserCreate(NULL);
    }
    
    XmlLayout::~XmlLayout() {
      XML_ParserFree(parser);
    }
    
    void XmlLayout::parse(const std::string xml, Layout *lay) {
      layout = lay;
      resetParser();
#ifdef DEBUG
      std::cout << xml << std::endl;
#endif
      
      int done = 0;
      if (XML_Parse(parser, xml.c_str(), xml.size(), done) == XML_STATUS_ERROR) {
        std::cerr << XML_ErrorString(XML_GetErrorCode(parser)) << " at line " << XML_GetCurrentLineNumber(parser);
        layout->message = std::string(XML_ErrorString(XML_GetErrorCode(parser)));
      }
    }
    
    void XmlLayout::resetParser() {
      if (!XML_ParserReset(parser, NULL)) {
        XML_ParserFree(parser);
        parser = XML_ParserCreate(NULL);
      }
      XML_SetUserData(parser, this);
      XML_SetElementHandler(parser, XmlLayout::startElementCallback, XmlLayout::endElementCallback);
      XML_SetCharacterDataHandler(parser, XmlLayout::characterDataCallback);
    }
    
    void XmlLayout::expatStartElement(const char *name, const char **attrs) {
      if (strcmp(TAG_LAYOUT, name) == 0) {
        layoutTag = true;
        parseLayoutAttrs(attrs);
      }
      else if (layoutTag && (strcmp(TAG_REGION, name) == 0)) {
        regionTag = true;
        layout->regions.push_back({0, 0, 0, 0, 0, 0, {}, {}});
        parseRegionAttrs(attrs);
      }
      else if (regionTag && (strcmp(TAG_MEDIA, name) == 0)) {
        mediaTag = true;
        layout->regions.back().media.push_back({0, "", "", 0, false, 0, {}, {}, {}});
        parseMediaAttrs(attrs);
      }
      else if (mediaTag && (strcmp(TAG_AUDIO, name) == 0)) {
        audioTag = true;
        layout->regions.back().media.back().audio.push_back({"", 0, false, 0});
        parseAudioAttrs(attrs);
      }
      else if (mediaTag && (strcmp(TAG_RAW, name) == 0)) {
        rawTag = true;
      }
      else if (regionTag || mediaTag) {
        settingsTag.assign(name);
      }
    }

    void XmlLayout::expatEndElement(const char *name) {
      if (strcmp(TAG_LAYOUT, name) == 0) {
        layoutTag = false;
      }
      else if (strcmp(TAG_REGION, name) == 0) {
        regionTag = false;
      }
      else if (strcmp(TAG_MEDIA, name) == 0) {
        mediaTag = false;
      }
      else if (strcmp(TAG_AUDIO, name) == 0) {
        audioTag = false;
      }
      else if (strcmp(TAG_RAW, name) == 0) {
        rawTag = false;
      }
      else if (!settingsTag.empty()) {
        settingsTag.clear();
      }
    }
    
    void XmlLayout::expatCharacterData(const char *text, int len) {
      if (audioTag) {
        layout->regions.back().media.back().audio.back().uri.assign(text, len);
      }
      else if (rawTag) {
        layout->regions.back().media.back().raw[settingsTag] = std::string(text, len);
      }
      else if (mediaTag) {
        layout->regions.back().media.back().options[settingsTag] = std::string(text, len);
      }
      else if (regionTag) {
        layout->regions.back().options[settingsTag] = std::string(text, len);
      }
    }

    void XmlLayout::parseLayoutAttrs(const char **attrs) {
      for (int i = 0; attrs[i]; i += 2) {
        if      (strcmp("message", attrs[i]) == 0)       { layout->message = attrs[i + 1]; }
        else if (strcmp("width", attrs[i]) == 0)         { layout->width = atoi(attrs[i + 1]); }
        else if (strcmp("height", attrs[i]) == 0)        { layout->height = atoi(attrs[i + 1]); }
        else if (strcmp("zindex", attrs[i]) == 0)        { layout->zindex = atoi(attrs[i + 1]); }
        else if (strcmp("bgcolor", attrs[i]) == 0)       { layout->backgroundColor = attrs[i + 1]; }
        else if (strcmp("background", attrs[i]) == 0)    { layout->backgroundImage = attrs[i + 1]; }
        else if (strcmp("schemaVersion", attrs[i]) == 0) { layout->version = atoi(attrs[i + 1]); }
      }
    }

    void XmlLayout::parseRegionAttrs(const char **attrs) {
      for (int i = 0; attrs[i]; i += 2) {
        if      (strcmp("id", attrs[i]) == 0)     { layout->regions.back().id = atoi(attrs[i + 1]); }
        else if (strcmp("height", attrs[i]) == 0) { layout->regions.back().height = atoi(attrs[i + 1]); }
        else if (strcmp("top", attrs[i]) == 0)    { layout->regions.back().x = atoi(attrs[i + 1]); }
        else if (strcmp("left", attrs[i]) == 0)   { layout->regions.back().y = atoi(attrs[i + 1]); }
        else if (strcmp("width", attrs[i]) == 0)  { layout->regions.back().width = atoi(attrs[i + 1]); }
        else if (strcmp("zindex", attrs[i]) == 0) { layout->regions.back().z = atoi(attrs[i + 1]); }
      }
    }

    void XmlLayout::parseMediaAttrs(const char **attrs) {
      for (int i = 0; attrs[i]; i += 2) {
        if      (strcmp("id", attrs[i]) == 0)          { layout->regions.back().media.back().id = atoi(attrs[i + 1]); }
        else if (strcmp("type", attrs[i]) == 0)        { layout->regions.back().media.back().type.assign(attrs[i + 1]); }
        else if (strcmp("render", attrs[i]) == 0)      { layout->regions.back().media.back().render.assign(attrs[i + 1]); }
        else if (strcmp("fileId", attrs[i]) == 0)      { layout->regions.back().media.back().file = atoi(attrs[i + 1]); }
        else if (strcmp("duration", attrs[i]) == 0)    { layout->regions.back().media.back().duration = atoi(attrs[i + 1]); }
        else if (strcmp("useDuration", attrs[i]) == 0) { layout->regions.back().media.back().useDuration = attrs[i + 1]; }
      }
    }

    void XmlLayout::parseAudioAttrs(const char **attrs) {
      for (int i = 0; attrs[i]; i += 2) {
        if      (strcmp("volume", attrs[i]) == 0)  { layout->regions.back().media.back().audio.back().volume = atoi(attrs[i + 1]); }
        else if (strcmp("mediaId", attrs[i]) == 0) { layout->regions.back().media.back().audio.back().media = atoi(attrs[i + 1]); }
        else if (strcmp("loop", attrs[i]) == 0)    { layout->regions.back().media.back().audio.back().loop = attrs[i + 1]; }
      }
    }
    
  }
}
