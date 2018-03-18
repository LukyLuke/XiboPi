/*
 * <one line to give the program's name and a brief idea of what it does.>
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

#include <iostream>
#include <fstream>

#include <gtk/gtk.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

#include "src/XiboDisplay.h"
#include "src/XiboClient.h"
#include "src/config.h"

void readConfiguration(const char * file);
std::string getConfigDir();

config::data configuration;
const char server[] = "http://xibo.localhost/xmds.php?v=5";
const char config_file[] = "config.ini";

int main(int argc, char* argv[]) {
  std::string config_dir = getConfigDir();
  std::string conf = std::string(config_dir).append(config_file);
  readConfiguration(conf.c_str());
  
  configuration.insert(std::pair<std::string, std::string>("config", conf));
  configuration.insert(std::pair<std::string, std::string>("home", std::string(config_dir)));
  configuration.insert(std::pair<std::string, std::string>("cache", std::string(config_dir).append("cache/")));
  configuration.insert(std::pair<std::string, std::string>("base_uri", std::string("file:///").append(config_dir).append("cache/")));
  
  // Create configuration directory and cache directory if needed
  int ret;
  mode_t mode = 0760;
  ret = mkdir(configuration["home"].c_str(), mode);
  if (ret == 0 || errno == EEXIST) {
    ret = mkdir(configuration["cache"].c_str(), mode);
  }
  if (ret != 0 && errno != EEXIST) {
    std::cout << "Path: " << configuration["cache"] << std::endl;
    std::cout << "Error while create configuration: " << strerror(errno) << std::endl;
    exit(1);
  }
  
  // Change to the home directory
  ret = chdir(configuration["home"].c_str());
  if (ret != 0) {
    std::cout << "Path: " << configuration["home"] << std::endl;
    std::cout << "Error while change directory: " << strerror(errno) << std::endl;
    exit(1);
  }
  
  // Initialize GTK+
  gtk_init(&argc, &argv);
  
  Xibo::XiboDisplay display;
  display.init();
  
  Xibo::XiboClient xibo(&configuration, &display);
  if (xibo.connect(server)) {
    xibo.schedule();
  }

  // Run the main GTK+ event loop
  gtk_main();
  return 0;
}

void readConfiguration(const char * file) {
  std::ifstream ifs(file, std::ifstream::in);
  ifs >> configuration;
  ifs.close();
}

std::string getConfigDir() {
  const char * home = getenv("HOME");
  if (home == NULL) {
    home = getpwuid(getuid())->pw_dir;
  }
  return std::string(home).append("/.Xibo/");
}
