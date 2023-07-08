#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <err.h>
#include <stdio.h>

#define POSX 500
#define POSY 500
#define WIDTH 500
#define HEIGHT 500
#define BORDER 2

static Display *dpy;
static Window root;
static XEvent ev;
static int scr;

int main() {
  dpy = XOpenDisplay(NULL);
  if (dpy == NULL)
    err(1, "Cannot connect to the X server\n");

  root = RootWindow(dpy, scr);

  scr = DefaultScreen(dpy);
  Window win = XCreateSimpleWindow(dpy, root, POSX, POSY, WIDTH, HEIGHT, BORDER, BlackPixel(dpy, scr), WhitePixel(dpy, scr));
  XMapWindow(dpy, win);

  while (XNextEvent(dpy, &ev) == 0) {
  }

  XUnmapWindow(dpy, win);
  XDestroyWindow(dpy, win);
  XCloseDisplay(dpy);

  return 0;
}
