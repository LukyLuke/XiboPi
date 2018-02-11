#include <map>
#include <string>
#include "api/soapxmdsBindingProxy.h"

#ifndef XIBO_CLIENT_H
#define XIBO_CLIENT_H

namespace Xibo {
  class XiboClient {
  public:
    XiboClient(const std::map<std::string, std::string> *conf);
    virtual ~XiboClient();
    bool connect(const char * server);
  
  private:
    const std::map<std::string, std::string> *config;
    xmdsBindingProxy * soapProxy;
    
    const std::string getConfig(const std::string key);
    void connectSoapProxy(const char * server);
  };
}

#endif //XIBO_CLIENT_H
