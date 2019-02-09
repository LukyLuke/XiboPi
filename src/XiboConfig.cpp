/*
 * Configuration Singelton
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

#include "XiboConfig.h"


namespace config {
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
      if ((begin == std::string::npos) || (std::string("#;").find(line[begin]) != std::string::npos)) {
        continue;
      }

      // Extract the key
      std::string::size_type end = line.find('=', begin);
      key = line.substr(begin, end - begin);

      // No leading/trailing whitespaces
      key.erase(key.find_last_not_of(" \f\t\v") + 1);

      // No blank keys allowed
      if (key.empty()) {
        continue;
      }

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

namespace Xibo {
  const char config_file[] = "xibo.conf";
  const char config_dir[]  = ".Xibo/";
  const char cache_dir[]  = "cache/";

  XiboConfig::XiboConfig() {}

  XiboConfig::~XiboConfig() {}

  XiboConfig * XiboConfig::getInstance() {
    if (xiboConfigInstance == nullptr) {
      xiboConfigInstance = new XiboConfig;
      xiboConfigInstance->init();
    }
    return xiboConfigInstance;
  }

  const void XiboConfig::insert(std::string key, std::string value) {
    getInstance()->configuration.insert(std::pair<std::string, std::string>(key, value));
  }

  const std::string XiboConfig::get(std::string key) {
    return get(key, "");
  }

  const std::string XiboConfig::get(std::string key, std::string default_value) {
    if (getInstance()->configuration.count(key) != 0) {
      return getInstance()->configuration.find(key)->second;
    }
    std::cout << "[XiboConfig] Not found: '" << key << "'; Fallback: '" << default_value << "'; You should add a '" << key << " = VALUE' entry to the Configuration" << std::endl;
    return default_value;
  }

  const void XiboConfig::checkEnvironment() {
    XiboConfig * inst = getInstance();

    // Create configuration directory and cache directory if needed
    int ret;
    mode_t mode = 0760;
    ret = mkdir(inst->get("home").c_str(), mode);
    if (ret == 0 || errno == EEXIST) {
      ret = mkdir(inst->get("cache").c_str(), mode);
    }

    // Hard fail in case of we where not able to initialize the configuration
    if (ret != 0 && errno != EEXIST) {
      std::cout << "[XiboConfig] Error(" << strerror(errno) << ") while create cache directory: " << inst->get("cache") << std::endl;
      exit(1);
    }

    // Change to the config directory which is our home
    ret = chdir(inst->get("home").c_str());
    if (ret != 0) {
      std::cout << "[XiboConfig] Error(" << strerror(errno) << ") while change to directory: " << inst->get("home") << std::endl;
      exit(1);
    }
  }

  void XiboConfig::init() {
    std::string config_dir = getConfigDir();
    std::string conf = std::string(config_dir).append(config_file);
    readConfiguration(conf.c_str());

    configuration.insert(std::pair<std::string, std::string>("config", conf));
    configuration.insert(std::pair<std::string, std::string>("home", std::string(config_dir)));
    configuration.insert(std::pair<std::string, std::string>("cache", std::string(config_dir).append(cache_dir)));
    configuration.insert(std::pair<std::string, std::string>("base_uri", std::string("file:///").append(config_dir).append(cache_dir)));
  }

  void XiboConfig::readConfiguration(const char * file) {
    std::cout << "[XiboConfig] Reading Configuration: " << file << std::endl;
    std::ifstream ifs(file, std::ifstream::in);
    ifs >> configuration;
    ifs.close();
  }

  const std::string XiboConfig::getConfigDir() {
    const char * home = getenv("HOME");
    if (home == NULL) {
      home = getpwuid(getuid())->pw_dir;
    }
    return std::string(home).append("/").append(config_dir);
  }
}
