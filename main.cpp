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
#include <gtk/gtk.h>

#include "src/Event.h"
#include "src/XiboDisplay.h"
#include "src/XiboClient.h"

const char server[] = "http://172.18.0.5/xmds.php?v=5";

int main(int argc, char* argv[]) {
  // Initialize GTK+
  gtk_init(&argc, &argv);

  Xibo::XiboDisplay display;
  display.init();

  Xibo::XiboClient xibo;
  if (xibo.connect(server)) {
    xibo.schedule();
  }

  // Run the main GTK+ event loop
  gtk_main();
  return 0;
}
