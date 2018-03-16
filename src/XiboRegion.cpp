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
  }

  XiboRegion::~XiboRegion() {
  }
  
  void XiboRegion::show() {
    if (webView == NULL) {
      webView = display->addRegion(region->id, region->x, region->y, region->width, region->height);
    }
  }
  
}
