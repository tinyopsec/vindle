_VERSION = 0.1 
VERSION  = `git describe --tags --dirty 2>/dev/null || echo $(_VERSION)`

PKG_CONFIG = pkg-config

# paths
PREFIX = /usr/local
MANDIR = $(PREFIX)/share/man
DATDIR = $(PREFIX)/share
# Installed default config dir, used when the user config is missing.
# FreeBSD ports hierarchy: make ETCDIR=/usr/local/etc/vindle
ETCDIR = /etc/vindle

WLR_INCS = `$(PKG_CONFIG) --cflags wlroots-0.20`
WLR_LIBS = `$(PKG_CONFIG) --libs wlroots-0.20`

# Allow using an alternative wlroots installation
#WLR_INCS = -I/usr/include/pixman-1 -I/usr/include/elogind -I/usr/include/libdrm \
#	-I$(PWD)/wlroots/include
#WLR_LIBS = -Wl,-rpath,$(PWD)/wlroots/build -L$(PWD)/wlroots/build -lwlroots-0.19

#XWAYLAND =
#XLIBS =
# Uncomment to build XWayland support
XWAYLAND = -DXWAYLAND
XLIBS = xcb xcb-icccm

# lua (pkg-config name differs by platform)
# FreeBSD: make LUA_PKG=lua-5.4
LUA_PKG ?= lua5.4

PKGS      = wayland-server xkbcommon libinput $(LUA_PKG) $(XLIBS)

# Requires C11: _Alignas and anonymous unions in wlroots headers.
CC = cc
