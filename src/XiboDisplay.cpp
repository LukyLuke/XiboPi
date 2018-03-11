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
    this->regions = std::list<XiboRegion>();
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
    
    loadStyles();
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
  
  void XiboDisplay::loadStyles() {
    GtkCssProvider * provider = gtk_css_provider_new();
    GdkDisplay * display = gdk_display_get_default();
    GdkScreen * screen = gdk_display_get_default_screen(display);

    std::string css = ".status { background-color: rgba(213,66,29,0.8); border-radius:12px; border:2px solid rgba(165,51,22,0.9); font-weight:bold; font-size:22pt; padding:0.4em; margin:1em;}";
    
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_css_provider_load_from_data(provider, css.c_str(), css.size(), NULL);

    g_object_unref(provider);
  }
  
  void XiboDisplay::initWebView() {
    // Creates an Overlay show messages on top of the WebView container
    overlay = gtk_overlay_new();
    gtk_container_add(GTK_CONTAINER(window), overlay);
    
    // The Grud is used to show the Overlay-messages
    grid = gtk_grid_new();
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), grid);
    
    // Creates the WebView and add it to the background of the overlay
    webView = WEBKIT_WEB_VIEW(webkit_web_view_new());
    gtk_container_add(GTK_CONTAINER(overlay), GTK_WIDGET(webView));
    webkit_web_view_load_uri(webView, "about:blank");
  }
  
  void XiboDisplay::showStatus(const std::string message, int time) {
    if (message.empty()) return;
    GtkWidget * label = gtk_grid_get_child_at(GTK_GRID(grid), 0, 0);
    bool exists = label != NULL;
    
    if (!exists) {
      label = gtk_label_new(message.c_str());
      gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
      gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
      
      gtk_style_context_add_class(gtk_widget_get_style_context(label), "status");
      
      gtk_widget_set_hexpand(label, TRUE);
      gtk_widget_set_vexpand(label, TRUE);
      gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
      gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
      gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
      
    } else {
      std::string s(gtk_label_get_text(GTK_LABEL(label)));
      s.append("\n");
      s.append(message.c_str());
      gtk_label_set_text(GTK_LABEL(label), s.c_str());
      g_source_remove(timerStatus);
    }
    
    timerStatus = g_timeout_add_seconds(time, hideStatus, this);
    gtk_widget_show_all(grid);
  }
  
  gboolean XiboDisplay::hideStatus(gpointer data) {
    XiboDisplay * display = (XiboDisplay *) data;
    GtkWidget * label = gtk_grid_get_child_at(GTK_GRID(display->grid), 0, 0);
    if (label != NULL) {
      gtk_widget_destroy(label);
    }
    return FALSE;
  }
  
  void XiboDisplay::setLayout(const Xml::XmlLayout::Layout * layout, XiboClient * client) {
    width = layout->width;
    height = layout->height;
    background.assign(layout->backgroundColor);
    
    for (auto it = layout->regions.cbegin(); it != layout->regions.end(); ++it) {
      prepareRegion(&(*it), client);
    }
    std::cout << "Regions: " << regions.size() << std::endl;
  }
  
  void XiboDisplay::prepareRegion(const Xml::XmlLayout::Region * region, XiboClient * client) {
    regions.push_back(XiboRegion(this, client));
  }
  
}
