#include <X11/X.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>
#include <X11/Xatom.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#define POSX 0
#define POSY 0
#define WIDTH 500
#define HEIGHT 500
#define BORDER 0 
#define LINE 5

#define C_BLUE "#98edfb"

static Display *dpy;
static Window root;
static int scr;
static GC gc;
static Visual *vis;


static Window createWindow(int x, int y, int w, int h, int b) {
  XSetWindowAttributes xwa;
  xwa.background_pixel = WhitePixel(dpy, scr);
  xwa.border_pixel = BlackPixel(dpy, scr);
  xwa.event_mask = ButtonMotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask;

  return XCreateWindow(dpy, root, x, y, w, h, b, DefaultDepth(dpy, scr), InputOutput, vis,
                       CWBackPixel | CWBorderPixel | CWEventMask, &xwa);
}

// Create graphics context
static GC createGC(int lineW, XftColor* fg_color) {
    XGCValues xgcv;
    unsigned long valuemask;

    xgcv.line_style = LineSolid;
    xgcv.line_width = lineW;
    xgcv.cap_style = CapButt;
    xgcv.join_style = JoinMiter;
	xgcv.fill_style = FillSolid;
    xgcv.foreground = fg_color->pixel;
    xgcv.background = WhitePixel(dpy, scr);

    valuemask = GCForeground | GCBackground | GCFillStyle | GCLineWidth | GCCapStyle | GCJoinStyle;
    return  XCreateGC(dpy, root, valuemask, &xgcv);
}

static void createColor(XftColor* color, const char* name){
    if (!XftColorAllocName(dpy, vis, DefaultColormap(dpy, scr), name, color))
        errx(1, "Cannot allocate color %s", name);

    color->pixel |= 0xff << 24; // something about transparency with dwm
}

static void run(XftColor* color){
    XEvent ev;
    int init = 0;
    int prev_x=0, prev_y=0;
    int swapcolor = 0;

    while (XNextEvent(dpy, &ev) == 0){
        switch (ev.type) {
            case ButtonPress:
                if (ev.xbutton.button == Button1)
                    XDrawPoint(dpy, ev.xbutton.window, gc, ev.xbutton.x, ev.xbutton.y);
                else if (ev.xbutton.button == Button3) {
                    XSetForeground(dpy, gc, swapcolor ? BlackPixel(dpy, scr) : color->pixel);
                    swapcolor = !swapcolor;
                }
                break;
            case MotionNotify:
                if (init) {
                    XDrawLine(dpy, ev.xbutton.window, gc, prev_x, prev_y, ev.xbutton.x, ev.xbutton.y);
                } else {
                    XDrawPoint(dpy, ev.xbutton.window, gc, ev.xbutton.x, ev.xbutton.y);
                    init = 1;
                }
                prev_x = ev.xbutton.x, prev_y = ev.xbutton.y;
                break;
            case ButtonRelease:
                init = 0;
                break;
            case KeyPress:
                if (XkbKeycodeToKeysym(dpy, ev.xkey.keycode, 0, 0) == XK_q)
                    return;
        }
    }
}

int main() {
    if ((dpy = XOpenDisplay(NULL)) == NULL)
        errx(1, "Cannot connect to the X server\n");

    Window win;
    XftColor *blue = malloc(sizeof(XftColor));
    scr = DefaultScreen(dpy);
    root = RootWindow(dpy, scr);
    vis = DefaultVisual(dpy, scr);

    win = createWindow(POSX, POSY, WIDTH, HEIGHT, BORDER);
    createColor(blue, C_BLUE);

    gc = createGC(LINE, blue);

    XStoreName(dpy, win, "Dumb Paint");
    XMapWindow(dpy, win);
    run(blue);

    // cleanup
    XUnmapWindow(dpy, win);
    XDestroyWindow(dpy, win);
	XFreeGC(dpy, gc);
	XftColorFree(dpy, vis, DefaultColormap(dpy, scr), blue);
	XCloseDisplay(dpy);

    return 0;
}