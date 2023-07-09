#include <X11/X.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>
#include <X11/Xatom.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

//main window attribs
#define POSX 0
#define POSY 0
#define WIDTH 800
#define HEIGHT 600
#define BORDER 0 
#define LINE 5

//child window attribs
#define CHILD_W 20 
#define CHILD_H 20
#define CHILD_B 2  //border

#define C_B "#0000ff"
#define C_G "#00ff00"
#define C_R "#ff0000"

typedef struct {
    Window win;
    XftColor *color;
} ColorButtons;

static Display *dpy;
static Window root;
static int scr;
static GC gc;
static Visual *vis;
static ColorButtons color_btns[3];
static const char *colors[3] = {C_B, C_G, C_R};

static Window createWindow(int x, int y, int w, int h, int b, Window * parent, int idx) {
    XSetWindowAttributes xwa = {
        .background_pixel = WhitePixel(dpy, scr),
        .border_pixel = BlackPixel(dpy, scr),
    };
    if (parent == NULL){
        xwa.event_mask = ButtonMotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask;
        return XCreateWindow(dpy, root, x, y, w, h, b, DefaultDepth(dpy, scr), InputOutput, vis,
                            CWBackPixel | CWBorderPixel | CWEventMask, &xwa);
    }
    xwa.event_mask = ButtonPress;
    xwa.background_pixel = color_btns[idx].color->pixel;
    return XCreateWindow(dpy, *parent, x, y, w, h, b, DefaultDepth(dpy, scr), InputOutput, vis,
                        CWBackPixel | CWBorderPixel | CWEventMask, &xwa);
}

// Create graphics context
static GC createGC(int lineW) {
    unsigned long valuemask;
    XGCValues xgcv = {
        .line_style = LineSolid,
        .line_width = lineW,
        .cap_style = CapButt,
        .join_style = JoinMiter,
        .fill_style = FillSolid,
        .foreground = BlackPixel(dpy, scr),
        .background = WhitePixel(dpy, scr),
    };
    valuemask = GCForeground | GCBackground | GCFillStyle | GCLineWidth | GCCapStyle | GCJoinStyle;
    return  XCreateGC(dpy, root, valuemask, &xgcv);
}

static void createColor(XftColor** color, const char* name){
    *color = malloc(sizeof(XftColor));
    if (!color)
        errx(1, "Can't alloc mem for color");

    if (!XftColorAllocName(dpy, vis, DefaultColormap(dpy, scr), name, *color))
        errx(1, "Cannot allocate color %s", name);

    // (*color)->pixel |= 0xff << 24; // something about transparency with dwm
}

static void run(){
    XEvent ev;
    int init = 0;
    int prev_x=0, prev_y=0;
    int color_set = 0;

    while (XNextEvent(dpy, &ev) == 0){
        switch (ev.type) {
            case ButtonPress:
                if (ev.xbutton.button == Button1){
                    color_set = 0;
                    for (int i=0; i<3; i++)
                        if (ev.xbutton.window == color_btns[i].win){
                            XSetForeground(dpy, gc, color_btns[i].color->pixel);
                            color_set = 1;
                            break;
                        }
                    // drawing mode if it wasn't a color set click
                    if (!color_set) 
                        XDrawPoint(dpy, ev.xbutton.window, gc, ev.xbutton.x, ev.xbutton.y);
                } else if (ev.xbutton.button == Button3)
                    XSetForeground(dpy, gc, BlackPixel(dpy, scr));
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
    scr = DefaultScreen(dpy);
    root = RootWindow(dpy, scr);
    vis = DefaultVisual(dpy, scr);

    win = createWindow(POSX, POSY, WIDTH, HEIGHT, BORDER, NULL, 0);
    int y = 0;
    for (int i=0; i<3; i++){
        createColor(&color_btns[i].color, colors[i]);
        color_btns[i].win = createWindow(0, y, CHILD_W, CHILD_H, CHILD_B, &win, i);
        y += CHILD_H + CHILD_B;
    }

    gc = createGC(LINE);

    // need this in i3 because otherwise i3 tiles the window no matter the set window size
    XSizeHints xsh = {
        .min_width = WIDTH,
        .min_height = HEIGHT,
        .max_width = WIDTH,
        .max_height = HEIGHT,
        .flags = PMinSize | PMaxSize,
    };
    XSetSizeHints(dpy, win, &xsh, XA_WM_NORMAL_HINTS);

    XStoreName(dpy, win, "Dumb Paint");
    XMapWindow(dpy, win);
    XMapSubwindows(dpy, win);
    run();

    // cleanup
    XUnmapSubwindows(dpy, win);
    XUnmapWindow(dpy, win);
    XDestroySubwindows(dpy, win);
    XDestroyWindow(dpy, win);
	XFreeGC(dpy, gc);
    for (int i=0; i<3; i++){
        XftColorFree(dpy, vis, DefaultColormap(dpy, scr), color_btns[i].color);
    }
	XCloseDisplay(dpy);

    return 0;
}
