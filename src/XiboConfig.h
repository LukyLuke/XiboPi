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

#ifndef XIBO_CONFIG_H
#define XIBO_CONFIG_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>

#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>

namespace config {
  /**
   * This is the main configuration
   */
  struct data: std::map<std::string, std::string> {
    bool iskey(const std::string& s) const {
      return count(s) != 0;
    }
  };
}

namespace Xibo {
  class XiboConfig {
  public:
    const static void insert(std::string key, std::string value);
    const static std::string get(std::string key);
    const static void checkEnvironment();

  private:
    XiboConfig();
    ~XiboConfig();
    void readConfiguration(const char * file);
    const std::string getConfigDir();
    config::data configuration;

    static XiboConfig * getInstance();
    void init();
  };
  static XiboConfig * xiboConfigInstance = nullptr;
}
#endif // XIBO_CONFIG_H
