_VERSION = 0.7
VERSION  = `git describe --tags --dirty 2>/dev/null || echo $(_VERSION)`

PKG_CONFIG = pkg-config

# paths
PREFIX = /usr/local
MANDIR = $(PREFIX)/share/man
DATADIR = $(PREFIX)/share

#XWAYLAND =
#XLIBS =
# Uncomment to build XWayland support
XWAYLAND = -DXWAYLAND
XLIBS = xcb xcb-icccm xcb-util xcb-cursor xcb-keysyms #xcb-util-wm 

CC = cc
