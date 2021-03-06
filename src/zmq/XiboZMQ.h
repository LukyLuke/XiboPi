/*
 * The ZeroMQ connector to Xibo-Server.
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

#include <zmq.h>
#include <string>
#include <iostream>

#ifndef XIBO_ZMQ_H
#define XIBO_ZMQ_H

namespace Xibo {
  class XiboZMQ {
    public:
      XiboZMQ();
      XiboZMQ(const char *endpoint);
      void setEndpoint(const char *endpoint);
      virtual ~XiboZMQ();
  };
}

#endif //XIBO_ZMQ_H