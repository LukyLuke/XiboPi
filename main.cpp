#include <iostream>
#include <fstream>

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include "src/XiboClient.h"
#include "src/config.h"

static void destroyWindow(GtkWidget * widget, GtkWidget * window);
static gboolean closeWebView(WebKitWebView * webView, GtkWidget * window);
WebKitWebView * initWebView(GtkWidget * window);
void hideCursor(GtkWidget * window);
void readConfiguration(const char * file);

config::data configuration;
const char server[] = "http://xibo.localhost/xmds.php?v=5";
const char config_file[] = "config.ini";

int main(int argc, char* argv[]) {
  readConfiguration(config_file);
  
  // Initialize GTK+
  gtk_init(&argc, &argv);

  // Create an 800x600 window that will contain the browser instance
  GtkWidget * window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(window, "destroy", G_CALLBACK(destroyWindow), NULL);
  //gtk_window_fullscreen(GTK_WINDOW(window));
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

  // Load a web page into the browser instance
  WebKitWebView *webView = initWebView(window);
  webkit_web_view_load_uri(webView, "about:blank");
  
  Xibo::XiboClient xibo(&configuration);
  xibo.connect(server);

  // Make sure the main window and all its contents are visible and the it has the focus
  gtk_widget_grab_focus(GTK_WIDGET(webView));
  gtk_widget_show_all(window);
  
  // Hide the Mouse-cursor
  hideCursor(window);

  // Run the main GTK+ event loop
  gtk_main();

  return 0;
}

static void destroyWindow(GtkWidget * widget, GtkWidget * window) {
  gtk_main_quit();
}

static gboolean closeWebView(WebKitWebView * webView, GtkWidget * window) {
  gtk_widget_destroy(window);
  return TRUE;
}

void hideCursor(GtkWidget * window) {
  GdkCursor * cursor = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_BLANK_CURSOR);
  GdkWindow * gdkWindow = gtk_widget_get_window(window);
  gdk_window_set_cursor(gdkWindow, cursor);
}

WebKitWebView * initWebView(GtkWidget * window) {
  // Create a browser instance
  WebKitWebView * webView = WEBKIT_WEB_VIEW(webkit_web_view_new());

  // Put the browser area into the main window
  gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(webView));
  
  // Set up callbacks so that if the browser instance is closed, the program will exit
  g_signal_connect(webView, "close", G_CALLBACK(closeWebView), window);
  return webView;
}

void readConfiguration(const char * file) {
  std::ifstream ifs(file, std::ifstream::in);
  ifs >> configuration;
  ifs.close();
}
