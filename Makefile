.POSIX:
.SUFFIXES:

include config.mk

VINDLECFLAGS = `$(PKG_CONFIG) --cflags $(PKGS)` $(WLR_INCS) \
	-I. -Iinclude -Isrc -Iparser -DWLR_USE_UNSTABLE -D_POSIX_C_SOURCE=200809L -DVERSION=\"$(VERSION)\" \
	$(XWAYLAND) -g -Wall -Wextra -Wno-unused-parameter -O1 -std=c11 $(CFLAGS)
LDLIBS    = `$(PKG_CONFIG) --libs $(PKGS)` $(WLR_LIBS) -lm $(LIBS)

SCANNER   = `$(PKG_CONFIG) --variable=wayland_scanner wayland-scanner`
PROTOCOLS = `$(PKG_CONFIG) --variable=pkgdatadir wayland-protocols`

SMSG_CFLAGS = `$(PKG_CONFIG) --cflags wayland-client` -Wall -Wextra -Wno-unused-parameter
SMSG_LDLIBS = `$(PKG_CONFIG) --libs wayland-client`

all: vindle smsg/smsg

vindle: src/vindle.o src/util.o parser/parser.o dwl-ipc-unstable-v2-protocol.o src/wlr_ext_workspace_v1.o ext-workspace-v1-protocol.o
	$(CC) $^ $(VINDLECFLAGS) $(LDFLAGS) $(LDLIBS) -o $@

src/vindle.o: src/vindle.c include/client.h config.h include/dwl-ipc.h dwl-ipc-unstable-v2-protocol.h \
	include/ext-workspace.h src/wlr_ext_workspace_v1.h ext-workspace-v1-protocol.h \
	cursor-shape-v1-protocol.h pointer-constraints-unstable-v1-protocol.h \
	wlr-layer-shell-unstable-v1-protocol.h \
	wlr-output-power-management-unstable-v1-protocol.h xdg-shell-protocol.h \
	parser/parser.h
	$(CC) $(VINDLECFLAGS) -o $@ -c $<

src/util.o: src/util.c src/util.h
	$(CC) $(VINDLECFLAGS) -o $@ -c $<

src/wlr_ext_workspace_v1.o: src/wlr_ext_workspace_v1.c src/wlr_ext_workspace_v1.h ext-workspace-v1-protocol.h
	$(CC) $(VINDLECFLAGS) -o $@ -c $<

parser/parser.o: parser/parser.c parser/parser.h
	$(CC) $(VINDLECFLAGS) -o $@ -c $<

dwl-ipc-unstable-v2-protocol.o: dwl-ipc-unstable-v2-protocol.c

ext-workspace-v1-protocol.o: ext-workspace-v1-protocol.c

# protocol headers and glue
cursor-shape-v1-protocol.h:
	$(SCANNER) enum-header $(PROTOCOLS)/staging/cursor-shape/cursor-shape-v1.xml $@
pointer-constraints-unstable-v1-protocol.h:
	$(SCANNER) enum-header $(PROTOCOLS)/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml $@
wlr-layer-shell-unstable-v1-protocol.h:
	$(SCANNER) enum-header protocols/wlr-layer-shell-unstable-v1.xml $@
wlr-output-power-management-unstable-v1-protocol.h:
	$(SCANNER) server-header protocols/wlr-output-power-management-unstable-v1.xml $@
xdg-shell-protocol.h:
	$(SCANNER) server-header $(PROTOCOLS)/stable/xdg-shell/xdg-shell.xml $@
dwl-ipc-unstable-v2-protocol.h:
	$(SCANNER) server-header protocols/dwl-ipc-unstable-v2.xml $@
dwl-ipc-unstable-v2-protocol.c:
	$(SCANNER) private-code protocols/dwl-ipc-unstable-v2.xml $@

# smsg 
smsg/dwl-ipc-unstable-v2-protocol.h:
	$(SCANNER) client-header protocols/dwl-ipc-unstable-v2.xml $@

smsg/dwl-ipc-unstable-v2-protocol.c:
	$(SCANNER) private-code protocols/dwl-ipc-unstable-v2.xml $@

smsg/dwl-ipc-unstable-v2-protocol.o: smsg/dwl-ipc-unstable-v2-protocol.c
	$(CC) $(SMSG_CFLAGS) -o $@ -c $<

smsg/smsg.o: smsg/smsg.c smsg/dwl-ipc-unstable-v2-protocol.h
	$(CC) $(SMSG_CFLAGS) -o $@ -c $<

smsg/smsg: smsg/smsg.o smsg/dwl-ipc-unstable-v2-protocol.o
	$(CC) $^ $(SMSG_CFLAGS) $(SMSG_LDLIBS) -o $@

ext-workspace-v1-protocol.h:
	$(SCANNER) server-header protocols/ext-workspace-v1.xml $@
ext-workspace-v1-protocol.c:
	$(SCANNER) private-code protocols/ext-workspace-v1.xml $@

clean:
	rm -f vindle smsg/smsg src/*.o parser/*.o smsg/*.o smsg/*-protocol.h smsg/*-protocol.c *.o *-protocol.h *-protocol.c

install: vindle smsg/smsg
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f vindle $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/vindle
	cp -f smsg/smsg $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/smsg
	mkdir -p $(DESTDIR)/etc/vindle
	cp -r example/* $(DESTDIR)/etc/vindle
	mkdir -p $(DESTDIR)$(MANDIR)/man1
	cp -f vindle.1 $(DESTDIR)$(MANDIR)/man1
	chmod 644 $(DESTDIR)$(MANDIR)/man1/vindle.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/vindle
	rm -f $(DESTDIR)$(PREFIX)/bin/smsg
	rm -f $(DESTDIR)$(MANDIR)/man1/vindle.1
	rm -rf $(DESTDIR)/etc/vindle

.SUFFIXES: .c .o
.c.o:
	$(CC) $(VINDLECFLAGS) -o $@ -c $<
