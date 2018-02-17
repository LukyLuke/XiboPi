/*
 * The Display of the XIBO-Client used to show content.
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

#include "XiboDisplay.h"

namespace Xibo {
  XiboDisplay::XiboDisplay() {
    
  }

  XiboDisplay::~XiboDisplay() {
    webView = NULL;
    window = NULL;
  }
  
  void XiboDisplay::init() {
    // Create a new Window and make if Fullscreen with no border
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //gtk_window_fullscreen(GTK_WINDOW(window));
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    
    hideCursor();
    initWebView();
    
    // Make sure the main window and all its contents are visible and the it has the focus
    gtk_widget_grab_focus(GTK_WIDGET(webView));
    gtk_widget_show_all(window);
    
    // Signals for closing and destroying
    g_signal_connect(window, "destroy", G_CALLBACK(XiboDisplay::destroyWindow), NULL);
    g_signal_connect(webView, "close", G_CALLBACK(XiboDisplay::closeWebView), window);
  }
  
  void XiboDisplay::destroyWindow(GtkWidget * widget, GtkWidget * window) {
    gtk_main_quit();
  }
  
  gboolean XiboDisplay::closeWebView(WebKitWebView * webView, GtkWidget * window) {
    gtk_widget_destroy(window);
    return TRUE;
  }
  
  void XiboDisplay::hideCursor() {
    GdkCursor * cursor = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_BLANK_CURSOR);
    GdkWindow * gdkWindow = gtk_widget_get_window(window);
    gdk_window_set_cursor(gdkWindow, cursor);
  }
  
  void XiboDisplay::initWebView() {
    webView = WEBKIT_WEB_VIEW(webkit_web_view_new());
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(webView));
    webkit_web_view_load_uri(webView, "about:blank");
  }
  
  void XiboDisplay::showStatus(const std::string message, int time) {
    webkit_web_view_load_plain_text(webView, message.c_str());
  }
}
