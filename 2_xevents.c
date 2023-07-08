#include <X11/X.h>
#include <X11/XKBlib.h>
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
static int scr;

static Window createWindow(int x, int y, int w, int h, int b) {
  Window win;
  XSetWindowAttributes xwa;

  xwa.background_pixel = WhitePixel(dpy, scr);
  xwa.border_pixel = BlackPixel(dpy, scr);
  xwa.event_mask = ButtonPress;

  return XCreateWindow(dpy, root, x, y, w, h, b, DefaultDepth(dpy, scr), InputOutput, DefaultVisual(dpy, scr),
                       CWBackPixel | CWBorderPixel | CWEventMask, &xwa);
}

static void run(){
    XEvent ev;
    while (XNextEvent(dpy, &ev) == 0){
        switch (ev.type) {
            case ButtonPress:
                return;
        }
    }
}

int main() {
    if ((dpy = XOpenDisplay(NULL)) == NULL)
        err(1, "Cannot connect to the X server\n");

    Window win;
    scr = DefaultScreen(dpy);
    root= RootWindow(dpy, scr);

    win = createWindow(POSX, POSY, WIDTH, HEIGHT, BORDER);
    XMapWindow(dpy, win);

    run();

    // cleanup
    XUnmapWindow(dpy, win);
    XDestroyWindow(dpy, win);

    return 0;
}