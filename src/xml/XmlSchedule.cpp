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

#include "XmlSchedule.h"
#include <string>
#include <cstring>

namespace Xibo {
  namespace Xml {
    XmlSchedule::XmlSchedule() {
      this->parser = XML_ParserCreate(NULL);
    }

    XmlSchedule::~XmlSchedule() {
      XML_ParserFree(parser);
    }

    void XmlSchedule::parse(const std::string xml, Schedule *sche) {
      schedule = sche;
      resetParser();
#ifdef DEBUG
      std::cout << xml << std::endl;
#endif

      int done = 0;
      if (XML_Parse(parser, xml.c_str(), xml.size(), done) == XML_STATUS_ERROR) {
        std::cerr << "[XmlSchedule] " << XML_ErrorString(XML_GetErrorCode(parser)) << " at line " << XML_GetCurrentLineNumber(parser);
        schedule->message = std::string(XML_ErrorString(XML_GetErrorCode(parser)));
      }
    }

    void XmlSchedule::resetParser() {
      if (!XML_ParserReset(parser, NULL)) {
        XML_ParserFree(parser);
        parser = XML_ParserCreate(NULL);
      }
      XML_SetUserData(parser, this);
      XML_SetElementHandler(parser, XmlSchedule::startElementCallback, XmlSchedule::endElementCallback);
      XML_SetCharacterDataHandler(parser, XmlSchedule::characterDataCallback);
    }

    void XmlSchedule::expatStartElement(const char *name, const char **attrs) {
      if (!scheduleTag && (strcmp(TAG_SCHEDULE, name) == 0)) {
        scheduleTag = true;
        parseScheduleAttrs(attrs);
      }
      else if (scheduleTag && (strcmp(TAG_DEFAULT, name) == 0)) {
        defaultTag = true;
        parseDefaultAttrs(attrs);
      }
      else if (scheduleTag && (strcmp(TAG_DEPENDANTS, name) == 0)) {
        dependantsTag = true;
      }
      else if (scheduleTag && (strcmp(TAG_LAYOUT, name) == 0)) {
        layoutTag = true;
        schedule->layouts.push_back({0, "", "", 0, 0, {}});
        parseLayoutAttrs(attrs);
      }
      else if (layoutTag && (strcmp(TAG_DEPENDENTS, name) == 0)) {
        dependentsTag = true;
      }
      else if (layoutTag && (strcmp(TAG_COMMAND, name) == 0)) {
        commandTag = true;
        schedule->commands.push_back({0, "", ""});
        parseCommandAttrs(attrs);
      }
      else if (layoutTag && (strcmp(TAG_OVERLAYS, name) == 0)) {
        overlaysTag = true;
      }
      else if (overlaysTag && (strcmp(TAG_OVERLAY, name) == 0)) {
        overlayTag = true;
        schedule->layouts.push_back({0, "", "", 0, 0, {}});
        parseLayoutAttrs(attrs);
      }
      else if ((dependentsTag || dependantsTag) && (strcmp(TAG_FILE, name) == 0)) {
        fileTag = true;
      }

    }

    void XmlSchedule::expatEndElement(const char *name) {
      if (strcmp(TAG_SCHEDULE, name) == 0) {
        scheduleTag = false;
      }
      else if (strcmp(TAG_LAYOUT, name) == 0) {
        layoutTag = false;
      }
      else if (strcmp(TAG_DEFAULT, name) == 0) {
        defaultTag = false;
      }
      else if (strcmp(TAG_DEPENDENTS, name) == 0) {
        dependentsTag = false;
      }
      else if (strcmp(TAG_DEPENDANTS, name) == 0) {
        dependantsTag = false;
      }
      else if (strcmp(TAG_FILE, name) == 0) {
        fileTag = false;
      }
      else if (strcmp(TAG_COMMAND, name) == 0) {
        commandTag = false;
      }
      else if (strcmp(TAG_OVERLAYS, name) == 0) {
        overlaysTag = false;
      }
      else if (strcmp(TAG_OVERLAY, name) == 0) {
        overlayTag = false;
      }

    }

    void XmlSchedule::expatCharacterData(const char *text, int len) {
      if (fileTag && defaultTag) {
        schedule->defaults.push_back(std::string().assign(text, len));
      }
      else if (fileTag && dependentsTag) {
        schedule->dependents.push_back(std::string().assign(text, len));
      }
      else if (fileTag && dependantsTag) {
        schedule->dependants.push_back(std::string().assign(text, len));
      }
    }

    void XmlSchedule::parseScheduleAttrs(const char **attrs) {
      for (int i = 0; attrs[i]; i += 2) {
        if      (strcmp("generated", attrs[i]) == 0)  { schedule->generated.assign(attrs[i + 1]); }
        // Yes, this is a typo in XIBO-XML
        else if (strcmp("fitlerFrom", attrs[i]) == 0) { schedule->from.assign(attrs[i + 1]); }
        else if (strcmp("filterFrom", attrs[i]) == 0) { schedule->from.assign(attrs[i + 1]); }
        // Yes, this is a typo in XIBO-XML
        else if (strcmp("fitlerTo", attrs[i]) == 0)   { schedule->upto.assign(attrs[i + 1]); }
        else if (strcmp("filterTo", attrs[i]) == 0)   { schedule->upto.assign(attrs[i + 1]); }
      }
    }

    void XmlSchedule::parseDefaultAttrs(const char **attrs) {
      for (int i = 0; attrs[i]; i += 2) {
        if (strcmp("file", attrs[i]) == 0) { schedule->defaultLayout = atoi(attrs[i + 1]); }
      }
    }

    void XmlSchedule::parseLayoutAttrs(const char **attrs) {
      for (int i = 0; attrs[i]; i += 2) {
        if      (strcmp("scheduleid", attrs[i]) == 0) { schedule->layouts.back().eventId = atoi(attrs[i + 1]); }
        else if (strcmp("priority", attrs[i]) == 0)   { schedule->layouts.back().priority = atoi(attrs[i + 1]); }
        else if (strcmp("file", attrs[i]) == 0)       { schedule->layouts.back().layoutId = atoi(attrs[i + 1]); }
        else if (strcmp("fromdt", attrs[i]) == 0)     { schedule->layouts.back().from.assign(attrs[i + 1]); }
        else if (strcmp("todt", attrs[i]) == 0)       { schedule->layouts.back().upto.assign(attrs[i + 1]); }
      }
    }

    void XmlSchedule::parseCommandAttrs(const char **attrs) {
      for (int i = 0; attrs[i]; i += 2) {
        if      (strcmp("scheduleid", attrs[i]) == 0) { schedule->commands.back().eventId = atoi(attrs[i + 1]); }
        else if (strcmp("date", attrs[i]) == 0)       { schedule->commands.back().date.assign(attrs[i + 1]); }
        else if (strcmp("code", attrs[i]) == 0)       { schedule->commands.back().code.assign(attrs[i + 1]); }
      }
    }

  }
}
