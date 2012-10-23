include $(top_srcdir)/build-aux/autotroll.mk

ACLOCAL_AMFLAGS = -I m4 --install

AM_CFLAGS = -fvisibility=hidden
AM_CPPFLAGS =
AM_CXXFLAGS = -fvisibility=hidden -fvisibility-inlines-hidden
AM_LDFLAGS =

lib_LTLIBRARIES = src/libqtuiotouch.la

BUILT_SOURCES = \
	src/qtuioplugin.moc.cpp \
	src/qtuiotouch.moc.cpp

EXTRA_DIST = src/qtuio.json

src_libqtuiotouch_la_SOURCES = \
	$(BUILT_SOURCES) \
	src/qtuioplugin.hpp \
	src/qtuioplugin.cpp \
	src/qtuiotouch.hpp \
	src/qtuiotouch.cpp
src_libqtuiotouch_la_CXXFLAGS = \
	$(AM_CXXFLAGS) \
	$(QT_CXXFLAGS)
src_libqtuiotouch_la_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	$(QT_CPPFLAGS)
src_libqtuiotouch_la_LDFLAGS = \
	-module \
	-avoid-version \
	-no-undefined \
	-export-dynamic \
	$(QT_LDFLAGS)
