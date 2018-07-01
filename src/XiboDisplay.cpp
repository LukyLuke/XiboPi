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
  }

  void XiboDisplay::eventFired(const EVENTS ev, const void * data) {
    if ((ev == SOAP_FAULT_RECEIVED) || (ev == RESOURCE_FAILED) || (ev == MESSAGE_RECEIVED)) {
      std::cout << "XiboDisplay::String: " << *(std::string*)data << std::endl;
      showStatus(*(std::string*)data, 60);

    } else if (ev == UPDATE_LAYOUT) {
      const Xml::XmlLayout::Layout * layout = (const Xml::XmlLayout::Layout *)data;
      std::cout << "XiboDisplay::Layout: " << layout->message << std::endl;
      setLayout(layout);

    } else if (ev == DISPLAY_REGISTERED) {
      const Xml::XmlDisplay::Display * display = (const Xml::XmlDisplay::Display*)data;
      std::cout << "XiboDisplay::Display: " << display->message << std::endl;
    }
  }

  void XiboDisplay::init() {
    EventHandler::registerFor(DISPLAY_REGISTERED, this);
    EventHandler::registerFor(UPDATE_LAYOUT, this);
    EventHandler::registerFor(RESOURCE_FAILED, this);
    EventHandler::registerFor(SOAP_FAULT_RECEIVED, this);

    // Create a new Window and make if Fullscreen with no border
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_fullscreen(GTK_WINDOW(window));
    gtk_style_context_add_class(gtk_widget_get_style_context(window), "window");

    // Resize to fullscreen
    GdkRectangle geometry = {0, 0, 0, 0};
    GdkMonitor * monitor = gdk_display_get_primary_monitor(gdk_display_get_default());
    gdk_monitor_get_geometry(monitor, &geometry);
    gtk_window_resize(GTK_WINDOW(window), geometry.width, geometry.height);

    // Make sure the main window and all its contents are visible
    loadStatusStyles();
    initFixedArea();
    gtk_widget_show_all(window);

    // Hide the Cursor and set RGBA-Mode
    prepareWindow();

    // Signals for closing and destroying
    g_signal_connect(window, "destroy", G_CALLBACK(XiboDisplay::destroyWindow), NULL);
  }

  void XiboDisplay::destroyWindow(GtkWidget * widget, GtkWidget * window) {
    delete window;
    gtk_main_quit();
  }

  gboolean XiboDisplay::closeWebView(WebKitWebView * webView, GtkWidget * window) {
    gtk_widget_destroy(window);
    delete webView;
    return TRUE;
  }

  void XiboDisplay::prepareWindow() {
    // Hide the cursor
    GdkCursor * cursor = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_BLANK_CURSOR);
    GdkWindow * gdkWindow = gtk_widget_get_window(GTK_WIDGET(window));
    gdk_window_set_cursor(gdkWindow, cursor);

    // Set RGBA mode so we can set the webViews background transparent
    GdkScreen * screen = gtk_window_get_screen(GTK_WINDOW(window));
    GdkVisual * rgbaVisual = gdk_screen_get_rgba_visual(screen);
    if (rgbaVisual) {
      gtk_widget_set_visual(GTK_WIDGET(window), rgbaVisual);
      gtk_widget_set_app_paintable(GTK_WIDGET(window), TRUE);
    }
  }

  void XiboDisplay::loadStatusStyles() {
    GtkCssProvider * provider = gtk_css_provider_new();
    GdkDisplay * display = gdk_display_get_default();
    GdkScreen * screen = gdk_display_get_default_screen(display);

    std::string css = ".status { background-color: rgba(213,66,29,0.8); border-radius:12px; border:2px solid rgba(165,51,22,0.9); font-weight:bold; font-size:22pt; padding:0.4em; margin:1em;}";

    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_css_provider_load_from_data(provider, css.c_str(), css.size(), NULL);

    g_object_unref(provider);
  }

  void XiboDisplay::setWindowBackground() {
    GtkCssProvider * provider = gtk_css_provider_new();
    GdkDisplay * display = gdk_display_get_default();
    GdkScreen * screen = gdk_display_get_default_screen(display);

    std::string css = std::string(".window { background-color:").append(background).append(";}");

    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_css_provider_load_from_data(provider, css.c_str(), css.size(), NULL);

    g_object_unref(provider);
  }

  void XiboDisplay::initFixedArea() {
    // Creates an Overlay show messages on top of the fixed container
    overlay = gtk_overlay_new();
    gtk_container_add(GTK_CONTAINER(window), overlay);

    // The Grid is used to show the Overlay-messages
    grid = gtk_grid_new();
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), grid);

    // Creates the GtkFixed and add it to the background of the overlay
    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(overlay), GTK_WIDGET(fixed));
  }

  void XiboDisplay::showStatus(const std::string message, int time) {
    if (message.empty()) {
      return;
    }
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

  void XiboDisplay::setLayout(const Xml::XmlLayout::Layout * layout) {
    width = layout->width;
    height = layout->height;
    background.assign(layout->backgroundColor);
    setWindowBackground();

    int32_t w;
    int32_t h;
    gtk_window_get_size(GTK_WINDOW(window), &w, &h);

    float xs = (float)w / width;
    float ys = (float)h / height;
    if (xs < ys) {
      scale = xs;
    } else {
      scale = ys;
    }
    offset_y = ((float)((height * scale) - h) / 2) * scale;
    offset_x = ((float)((width * scale) - w) / 2) * scale;

    for (auto it = layout->regions.cbegin(); it != layout->regions.end(); ++it) {
      prepareRegion(&(*it));
    }
  }

  void XiboDisplay::prepareRegion(const Xml::XmlLayout::Region * region) {
    XiboRegion * reg = new XiboRegion(this, region);
    reg->show();
    regions.push_back(*reg);
  }

  WebKitWebView * XiboDisplay::addRegion(const uint32_t id, const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h) {
    WebKitWebView * web = WEBKIT_WEB_VIEW(webkit_web_view_new());
    gtk_widget_set_size_request(GTK_WIDGET(web), (uint32_t)(w * scale), (uint32_t)(h * scale));
    gtk_fixed_put(GTK_FIXED(fixed), GTK_WIDGET(web), (uint32_t)(x * scale) + offset_x, (uint32_t)(y * scale) + offset_y);

    webkit_web_view_load_uri(web, "about:blank");
    gtk_widget_show_all(window);

    g_signal_connect(web, "close", G_CALLBACK(XiboDisplay::closeWebView), window);
    return web;
  }
}
