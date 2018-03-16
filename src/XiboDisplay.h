/*
 * The Display of the XIBO-Client used to show content.
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
#include <list>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include "XiboRegion.h"
#include "xml/XmlLayout.h"

#ifndef XIBO_DISPLAY_H
#define XIBO_DISPLAY_H

namespace Xibo {
  class XiboDisplay {
    public:
      XiboDisplay();
      ~XiboDisplay();
      void init();
      void showStatus(const std::string message, int time);
      static void destroyWindow(GtkWidget * widget, GtkWidget * window);
      static gboolean closeWebView(WebKitWebView * webView, GtkWidget * window);
      void setLayout(const Xml::XmlLayout::Layout * layout, XiboClient * client);
      WebKitWebView * addRegion(const uint32_t id, const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h);
      
    private:
      GtkWidget * window = NULL;
      GtkWidget * fixed = NULL;
      GtkWidget * overlay = NULL;
      GtkWidget * grid = NULL;
      
      gint timerStatus = 0;
      static gboolean hideStatus(gpointer data);
      
      uint32_t width = 0;
      uint32_t height = 0;
      uint32_t offset_x = 0;
      uint32_t offset_y = 0;
      float scale = 1;
      std::string background = "";
      std::list<XiboRegion> regions;
      
      void initFixedArea();
      void hideCursor();
      void loadStatusStyles();
      void setWindowBackground();
      void prepareRegion(const Xml::XmlLayout::Region * region, XiboClient * client);
  };
}
#endif // XIBO_DISPLAY_H
