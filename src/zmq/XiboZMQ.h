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