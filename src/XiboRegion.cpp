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

#include "XiboRegion.h"

#include "XiboClient.h"
#include "XiboDisplay.h"

namespace Xibo {
  XiboRegion::XiboRegion(XiboDisplay * display, XiboClient * client, const Xml::XmlLayout::Region * reg) {
    this->display = display;
    this->client = client;
    this->region = reg;
    this->webView = nullptr;
  }

  XiboRegion::~XiboRegion() {
  }
  
  void XiboRegion::prepareWebView() {
    if (webView != NULL) {
      return;
    }
    
#ifndef DEBUG
    GdkRGBA rgba = {1, 0, 0, 1};
#else
    GdkRGBA rgba = {0, 0, 0, 0};
#endif
    webView = display->addRegion(region->id, region->x, region->y, region->width, region->height);
    webkit_web_view_set_background_color(webView, &rgba);
    media = region->media.cbegin();
  }
  
  void XiboRegion::show() {
    prepareWebView();
    
    // There is no media, we can't show something
    if (region->media.empty()) {
      return;
    }
    
    // Retart with the first media
    if (media == region->media.end()) {
      media = region->media.begin();
    }
    
    // Show the media
    std::string result;
    const Xml::XmlLayout::Media * m = &(* media);
    if (m->file > 0) {
      result = client->getFileUrl(m->file);
      webkit_web_view_load_uri(webView, result.c_str());
      
    } else if ((m->type == "webpage") && (m->render == "native")) {
      webkit_web_view_load_uri(webView, urldecode(m->options.at("uri")).c_str());
      
    } else {
      result = client->getResource(region->id, m->id);
      webkit_web_view_load_html(webView, result.c_str(), client->getConfig("base_uri").c_str());
    }
    
    // Start the timer for the next show-event if there is a second one and a duration
    if (region->media.size() > 1) {
      if (timerStatus != 0) {
        g_source_remove(timerStatus);
      }
      timerStatus = g_timeout_add_seconds(m->duration, timer, this);
      
      // Next run has to take the next media
      media++;
    }
  }
  
  gboolean XiboRegion::timer(gpointer data) {
    XiboRegion * region = (XiboRegion *) data;
    region->show();
    return FALSE;
  }
  
  inline unsigned char XiboRegion::from_hex(unsigned char ch) {
    if (ch <= '9' && ch >= '0')
      ch -= '0';
    else if (ch <= 'f' && ch >= 'a')
      ch -= 'a' - 10;
    else if (ch <= 'F' && ch >= 'A')
      ch -= 'A' - 10;
    else 
      ch = 0;
    return ch;
  }

  const std::string XiboRegion::urldecode(const std::string str) {
    std::string result;
    std::string::size_type i;
    for (i = 0; i < str.size(); ++i) {
      if (str[i] == '+') {
        result += ' ';

      } else if (str[i] == '%' && str.size() > i+2) {
        const unsigned char ch1 = from_hex(str[i+1]);
        const unsigned char ch2 = from_hex(str[i+2]);
        const unsigned char ch = (ch1 << 4) | ch2;
        result += ch;
        i += 2;
        
      } else {
        result += str[i];
      }
    }
    return result;
  }
  
}
