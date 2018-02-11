
#include "XiboClient.h"
#include "api/xmdsBinding.nsmap"

namespace Xibo {
  XiboClient::XiboClient(const std::map<std::string, std::string> *conf) {
    this->config = conf;
    this->soapProxy = NULL;
  }
  
  XiboClient::~XiboClient() {
    if (soapProxy != NULL)
      soapProxy = NULL;
  }
  
  const std::string XiboClient::getConfig(const std::string key) {
    if (config->count(key) != 0) {
      return config->find(key)->second;
    }
    return "";
  }
  
  void XiboClient::connectSoapProxy(const char * server) {
    if (soapProxy == NULL) {
      soapProxy = new xmdsBindingProxy(server);
    }
  }
  
  bool XiboClient::connect(const char * server) {
    connectSoapProxy(server);
   
    const std::string serverKey ("XIBODEV");
    const std::string hardwareKey("HardwareKey");
    const std::string displayName("DisplayName");
    const std::string clientType("android");
    const std::string clientVersion("1.8.3");
    const std::string operationSystem("Linux");
    const std::string macAddress("11:22:33:aa:bb:cc");
    const std::string xmrChannel("Channel");
    const std::string publicKey("");
    std::string activationMessage;

    int res = soapProxy->RegisterDisplay(serverKey, hardwareKey, displayName, clientType, clientVersion, 1, operationSystem, macAddress, xmrChannel, publicKey, activationMessage);

    std::cout << "Result " << res;

    if (res == SOAP_OK) {
      std::cout << "OK" << std::endl;
      std::cout << "Message: " << activationMessage << std::endl;
    } else {
      std::cout << "NOT OK" << std::endl;
      std::cout << "Error: " << soapProxy->soap_fault_string() << std::endl;
    }
    return (res == SOAP_OK);
  }
}
