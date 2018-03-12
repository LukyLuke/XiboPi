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

namespace Xibo {
  XiboRegion::XiboRegion(XiboDisplay * display, XiboClient * client) {
    this->display = display;
    this->client = client;
  }

  XiboRegion::~XiboRegion() {
  }
  
  void XiboRegion::show(const Xml::XmlLayout::Region * reg) {
    region = reg;
    std::cout << "Region: " << region->id << std::endl;
  }
}