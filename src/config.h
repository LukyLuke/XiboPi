#include <iostream>
#include <map>
#include <string>

#ifndef XIBO_CONFIG_H
#define XIBO_CONFIG_H
namespace config {
  /**
   * This is the main configuration meinly just a Key-Value pair
   */
  struct data: std::map<std::string, std::string> {
    bool iskey(const std::string& s) const {
      return count(s) != 0;
    }
  };
  
  /**
   * The extraction operator reads config::data until EOF.
   * Invalid data is ignored.
   */
  std::istream& operator >> (std::istream& stream, data& d) {
    std::string line, key, value;
    
    // For each (key, value) pair in the file
    while (std::getline(stream, line)) {
      std::string::size_type begin = line.find_first_not_of(" \f\t\v");
      
      // Skip blank lines and comments
      if (begin == std::string::npos) continue;
      if (std::string("#;").find(line[begin]) != std::string::npos) continue;
      
      // Extract the key
      std::string::size_type end = line.find('=', begin);
      key = line.substr(begin, end - begin);
      
      // No leading/trailing whitespaces
      key.erase(key.find_last_not_of(" \f\t\v") + 1);
      
      // No blank keys allowed
      if (key.empty()) continue;
      
      // Extract the value
      begin = line.find_first_not_of(" \f\n\r\t\v", end + 1);
      end   = line.find_last_not_of(" \f\n\r\t\v") + 1;
      value = line.substr(begin, end - begin);
      
      // Insert the properly extracted (key, value) pair
      d[key] = value;
    }
    
    return stream;
  }
  
  /**
   * The insertion operator writes all config::data to stream.
   */
  std::ostream& operator << (std::ostream& outs, const data& d) {
    data::const_iterator iter;
    for (iter = d.begin(); iter != d.end(); iter++) {
      outs << iter->first << " = " << iter->second << std::endl;
    }
    return outs;
  }
}
#endif //XIBO_CONFIG_H
