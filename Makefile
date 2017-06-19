#!/usr/bin/make -f

include Makefile.mk

# --------------------------------------------------------------

PREFIX  ?= /usr/local
DESTDIR ?=
BUILDDIR ?= build/bollie-echopeck.lv2

# --------------------------------------------------------------
# Default target is to build all plugins

all: build
build: bollie-echopeck

# --------------------------------------------------------------
# bollie-echopeck build rules

bollie-echopeck: $(BUILDDIR) $(BUILDDIR)/bollie-echopeck$(LIB_EXT) $(BUILDDIR)/manifest.ttl $(BUILDDIR)/modgui.ttl $(BUILDDIR)/bollie-echopeck.ttl $(BUILDDIR)/modgui

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/bollie-echopeck.o: src/b-echopeck.c
	$(CC) $^ $(BUILD_C_FLAGS) $(LINK_FLAGS) -lm -o $@ -c

$(BUILDDIR)/bollie-echopeck$(LIB_EXT): $(BUILDDIR)/bollie-echopeck.o
	$(CC) $^ $(BUILD_C_FLAGS) $(LINK_FLAGS) -lm $(SHARED) -o $@

$(BUILDDIR)/manifest.ttl: lv2ttl/manifest.ttl.in
	sed -e "s|@LIB_EXT@|$(LIB_EXT)|" $< > $@

$(BUILDDIR)/modgui.ttl: lv2ttl/modgui.ttl.in
	sed -e "s|@LIB_EXT@|$(LIB_EXT)|" $< > $@

$(BUILDDIR)/bollie-echopeck.ttl: lv2ttl/bollie-echopeck.ttl
	cp $< $@

$(BUILDDIR)/modgui: modgui
	mkdir -p $@ 
	cp -rv $^/* $@/

# --------------------------------------------------------------

clean:
	rm -f $(BUILDDIR)/bollie-echopeck* $(BUILDDIR)/bolliefilter* $(BUILDDIR)/*.ttl
	rm -fr $(BUILDDIR)/modgui

# --------------------------------------------------------------

install: build
	echo "Install"
	install -d $(DESTDIR)$(PREFIX)/lib/lv2/bollie-echopeck.lv2
	install -d $(DESTDIR)$(PREFIX)/lib/lv2/bollie-echopeck.lv2/modgui

	install -m 644 $(BUILDDIR)/*.so  $(DESTDIR)$(PREFIX)/lib/lv2/bollie-echopeck.lv2/
	install -m 644 $(BUILDDIR)/*.ttl $(DESTDIR)$(PREFIX)/lib/lv2/bollie-echopeck.lv2/
	cp -rv $(BUILDDIR)/modgui/* $(DESTDIR)$(PREFIX)/lib/lv2/bollie-echopeck.lv2/modgui/

# --------------------------------------------------------------
uninstall:
	echo "Uninstall"
	rm -fr $(DESTDIR)$(PREFIX)/lib/lv2/bollie-echopeck.lv2
	
# --------------------------------------------------------------
