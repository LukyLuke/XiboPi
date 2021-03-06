/*
 * The Display-Region of the XIBO-Client used to show content.
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

#ifndef XIBO_REGION_H
#define XIBO_REGION_H

#include <list>

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include "XiboDisplay.h"
#include "XiboConfig.h"
#include "Event.h"

#include "xml/XmlLayout.h"
#include "xml/XmlFiles.h"

namespace Xibo {
  class XiboDisplay;

  class XiboRegion : public EventListener {
    public:
      XiboRegion(XiboDisplay * display, const Xml::XmlLayout::Region * reg);
      ~XiboRegion();
      void show();
      void eventFired(const EVENTS ev, const void * data);

    private:
      const void fireResourceRequestEvent(const Xml::XmlLayout::Media media);

      Xml::XmlLayout::Region region;
      Xml::XmlFiles::Resources resources;
      XiboDisplay * display;
      WebKitWebView * webView;

      std::list<Xml::XmlLayout::Media>::const_iterator media;
      void prepareWebView();
      const std::string getFileUrl(const uint32_t fileId);

      inline unsigned char from_hex(unsigned char ch);
      const std::string urldecode(const std::string str);

      // Timer for changing media
      gint timerStatus = 0;
      static gboolean timer(gpointer data);
  };
}
#endif // XIBO_REGION_H
